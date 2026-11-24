#include <labtool48/labtool48.h>
#include "internal.h"
#include <string.h>

lt48_status lt48_installation_idle_fpga(const char *path,unsigned char **data,size_t *size) {
    if(!path||!data||!size)return LT48_INVALID_ARGUMENT;
    return lt48_load_verified_idle_fpga_internal(path,data,size);
}
static int idle_valid(const unsigned char *idle,size_t size) {
    static const unsigned char digest[32]={0x16,0x05,0xd6,0x10,0xbc,0x48,0xdc,0x5a,0x38,0x78,0xd0,0x63,0x3e,0xaa,0xf3,0x47,0x3b,0xc0,0x1c,0xed,0xbf,0xcf,0x1c,0xff,0x04,0x09,0xd3,0x3a,0xd0,0x8f,0x0b,0xab};
    unsigned char actual[32];
    if(!idle||size!=2778)return 0;
    lt48_sha256(idle,size,actual);return !memcmp(actual,digest,32);
}
static lt48_status write_bytes(lt48_runtime_transport_session *s,const unsigned char *b,size_t n) {
    return lt48_runtime_transport_session_controller_write(s,b,n);
}
static lt48_status read_bytes(lt48_runtime_transport_session *s,unsigned char *b,unsigned n) {
    return lt48_runtime_transport_session_controller_capture_internal(s,b,n);
}
static lt48_status reset(lt48_runtime_transport_session *s) {
    const unsigned char cmd[]={0x49,0xaa,0x55};
    lt48_status st=write_bytes(s,cmd,sizeof(cmd));
    return st==LT48_OK?lt48_runtime_transport_session_flush(s):st;
}
/* Four original host values followed by all 256 byte values. Each byte uses
 * resident4C plus the existing bridge0E grant/read, not a fabricated bulk echo. */
static lt48_status echo(lt48_runtime_transport_session *s,lt48_hardware_test_result *r) {
    const unsigned char first[]={0x55,0xaa,0,0xff};
    for(unsigned i=0;i<260;i++) {
        unsigned char value=i<4?first[i]:(unsigned char)(i-4),reply=0;
        unsigned char cmd[]={0x4c,value};
        lt48_status st=write_bytes(s,cmd,sizeof(cmd));
        if(st==LT48_OK)st=read_bytes(s,&reply,1);
        if(st!=LT48_OK)return st;
        ++r->echo_checks;
        if(reply!=value){r->expected=value;r->observed=reply;return LT48_DEVICE_MISMATCH;}
    }
    return LT48_OK;
}
/* All-low/high plus six address signatures and complements distinguish every
 * pair of the48 lines. Socket must be empty: opposing digital levels occur. */
static unsigned pattern_value(unsigned pattern,unsigned pin) {
    return pattern<2?pattern:((pin>>((pattern-2)/2))&1u)^((pattern-2)&1u);
}
static lt48_status socket_test(lt48_runtime_transport_session *s,
    const unsigned char *idle,size_t size,lt48_hardware_test_result *r) {
    lt48_status st=reset(s);
    const unsigned char load[]={0x38,0xaa,0x55},capture[]={0x47,0xaa,0x55};
    if(st==LT48_OK)st=write_bytes(s,load,sizeof(load));
    if(st==LT48_OK)st=write_bytes(s,idle,size);
    if(st==LT48_OK)st=write_bytes(s,capture,sizeof(capture));
    if(st==LT48_OK)st=read_bytes(s,r->empty_raw,12);
    if(st!=LT48_OK)return st;
    r->empty_capture_completed=1;
    /* Both polarities must report no contact. This is a guard, not proof that
     * the operator removed every possible electrically invisible fixture. */
    for(unsigned i=0;i<12;i++)if(r->empty_raw[i])return LT48_SAFETY_REJECTED;
    st=reset(s);
    for(unsigned pattern=0;st==LT48_OK&&pattern<14;pattern++) {
        unsigned char drive[48*4];
        for(unsigned pin=0;pin<48;pin++) {
            drive[4*pin]=0x42;drive[4*pin+1]=0;drive[4*pin+2]=0xf0;
            drive[4*pin+3]=(unsigned char)(4*pin+pattern_value(pattern,pin));
        }
        st=write_bytes(s,drive,sizeof(drive));
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s);
        for(unsigned pin=0;st==LT48_OK&&pin<48;pin++) {
            unsigned expected=pattern_value(pattern,pin);unsigned char reply=0;
            /* Resident42: little-endian address then value;43: address/read.
             * Select the channel without changing its previously driven state. */
            unsigned char cmd[]={0x42,0,0xf0,(unsigned char)(4*pin+expected),0x43,0,0xf0};
            st=write_bytes(s,cmd,sizeof(cmd));
            if(st==LT48_OK)st=read_bytes(s,&reply,1);
            if(st!=LT48_OK)break;
            ++r->pin_checks;
            if((reply&1u)!=expected) {
                r->failed_pins|=UINT64_C(1)<<pin;
                if(!r->failed_pin){r->failed_pin=pin+1;r->failed_pattern=pattern;r->expected=expected;r->observed=reply&1u;}
            }
        }
        if(st==LT48_OK) {
            const unsigned char status=0x54;unsigned char fault=0;
            st=write_bytes(s,&status,1);
            if(st==LT48_OK)st=read_bytes(s,&fault,1);
            if(st==LT48_OK&&(fault&1u))st=LT48_SAFETY_REJECTED;
        }
        if(st==LT48_OK)++r->patterns_completed;
        /* Stop at the first failing pattern rather than continue contention. */
        if(st==LT48_OK&&r->failed_pins)st=LT48_DEVICE_MISMATCH;
    }
    return st;
}
lt48_status lt48_hardware_test_execute(lt48_hardware_test_kind kind,
    const unsigned char *idle,size_t size,const lt48_protocol_transport *api,
    void *ctx,lt48_hardware_test_result *r) {
    if(!r)return LT48_INVALID_ARGUMENT;
    memset(r,0,sizeof(*r));r->operation_status=LT48_INVALID_ARGUMENT;
    if(!api||!api->read||!api->write||kind>LT48_HARDWARE_TEST_EMPTY_SOCKET||kind<0)return LT48_INVALID_ARGUMENT;
    if(kind==LT48_HARDWARE_TEST_EMPTY_SOCKET&&!idle_valid(idle,size)) {
        r->operation_status=LT48_SAFETY_REJECTED;return LT48_SAFETY_REJECTED;
    }
    if(api->cancelled&&api->cancelled(ctx)){r->operation_status=LT48_CANCELLED;return LT48_CANCELLED;}
    lt48_runtime_transport_session *s=NULL;
    lt48_status st=lt48_runtime_transport_session_create(api,ctx,&s);
    if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(s,5000,0);
    if(st==LT48_OK)st=kind==LT48_HARDWARE_TEST_ECHO?echo(s,r):socket_test(s,idle,size,r);
    lt48_runtime_transport_session_close(s);r->operation_status=st;
    if(kind==LT48_HARDWARE_TEST_EMPTY_SOCKET) {
        lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;s=NULL;
        lt48_status cs=lt48_runtime_transport_session_create(&cleanup,ctx,&s);
        if(cs==LT48_OK)cs=reset(s);
        lt48_runtime_transport_session_close(s);r->cleanup_status=cs;r->cleanup_completed=cs==LT48_OK;
        if(cs!=LT48_OK)return LT48_CLEANUP_FAILED;
    }
    return st;
}
