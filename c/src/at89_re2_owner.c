#include "at89_re2.h"
#include "at89_re2_guard.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
/* Exact RE2 operation owner; source-audited, hardware qualification pending. */
lt48_status lt48_re2_validate(const lt48_re2_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=(131072u) || length>(131072u)-start)return LT48_SAFETY_REJECTED;
    if((op<1 || op>8) && op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)
        if(start || length!=(131072u))return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_CONFIGURATION) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=6 || (image[0]&0xe0))return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<5;i++)if((image[0]&(1u<<i)) && image[i+1]>(i<4?1:3))return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_SECURE) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=1 || !image[0] || image[0]>3)return LT48_INVALID_ARGUMENT;
    } else if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE) {
        if(!image || size!=length)return LT48_INVALID_ARGUMENT;
    }
    return LT48_OK;
}
typedef struct re2_session {
    const lt48_re2_resources *r;const lt48_protocol_transport *api;void *ctx;
    lt48_runtime_transport_session *wire;lt48_compile_variable vars[6];
    unsigned char buffers[32][32];lt48_runtime_configuration_value config[5];size_t config_count;
    uint32_t volts[4];unsigned mode,restore,ready;unsigned char saved[5];
} re2_session;
static void address_re2(re2_session *s,unsigned image) {
    uint32_t bank=0,physical=0,remaining=0;lt48_re2_address(image,&bank,&physical,&remaining);
    for(unsigned i=0;i<4;i++){s->buffers[0][i]=(unsigned char)(physical>>(8*i));s->buffers[1][i]=(unsigned char)(image>>(8*i));}

}
static lt48_status run_re2(re2_session *s,unsigned op) {
    lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[31];size_t n=0;
    for(unsigned i=0;i<32;i++)if(i!=8)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    o.mode=s->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=8;o.parameter_buffer_id=3;
    o.initial_buffers=seeds;o.initial_buffer_count=n;o.configuration=s->config;o.configuration_count=s->config_count;
    memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);o.root_timeout_seconds=10;
    lt48_status st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
    if(st==LT48_OK)st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,6,&o,s->r->addresses,6,s->buffers,s->config,5,&s->config_count,s->volts);
    return st;
}
static lt48_status transfer_re2(re2_session *s,int input,unsigned ep,unsigned char *p,size_t n) {
    if(s->api->cancelled && s->api->cancelled(s->ctx))return LT48_CANCELLED;
    size_t done=0;lt48_status st=input?s->api->read(s->ctx,(uint8_t)ep,p,n,&done):s->api->write(s->ctx,(uint8_t)ep,p,n,&done);
    return st==LT48_OK && done!=n?LT48_PROTOCOL_ERROR:st;
}
static lt48_status command_re2(re2_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    lt48_status st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK)st=transfer_re2(s,0,2,(unsigned char *)p,n);
    if(st==LT48_OK && reply)st=transfer_re2(s,1,0x88,reply,512);
    return st;
}
static lt48_status stop_re2(re2_session *s) {
    const unsigned char stop[]={0,0};unsigned char reply[512]={0};
    lt48_status st=command_re2(s,stop,sizeof stop,reply);
    return st==LT48_OK && (reply[0]!=1 || reply[1]!=0)?LT48_PROTOCOL_ERROR:st;
}
static lt48_status block_re2(re2_session *s,int writing,unsigned char *data,size_t n) {
    unsigned char h[]={writing?0x48:0x43,(unsigned char)(n/512),(unsigned char)(n/512>>8),(unsigned char)(n%512),(unsigned char)(n%512>>8),writing?1:0},reply[512]={0};
    lt48_status st=command_re2(s,h,sizeof h,NULL);
    if(st==LT48_OK)st=transfer_re2(s,!writing,writing?4:0x88,data,n);
    if(st==LT48_OK && writing) {
        st=transfer_re2(s,1,0x88,reply,512);if(st==LT48_OK && (reply[0]!=1 || reply[1]!=0))st=LT48_PROTOCOL_ERROR;
        const unsigned char end[]={0x0c,1};if(st==LT48_OK)st=command_re2(s,end,sizeof end,reply);
        if(st==LT48_OK && ((reply[0]!=1 || reply[1]!=0) || !(reply[2]&1)))st=LT48_DEVICE_MISMATCH;
    }
    return st;
}
static void configure_re2(re2_session *s,const unsigned char *v) {
    s->config_count=5;
    for(unsigned i=0;i<5;i++)s->config[i]=(lt48_runtime_configuration_value){-1,i<4?0:1,i<4?i:0,v?v[i]:UINT32_MAX};
}
/* The RAM latch is initialized while the resident controller owns the bus.
 * This does not require executing a pin-recovery handler before pin setup. */
static lt48_status clear_guard_re2(re2_session *s) {
    const unsigned char one=1;
    return lt48_owner_load_applet_internal(s->wire,0x5466,&one,1);
}
static lt48_status query_guard_re2(re2_session *s,int reselect) {
    lt48_status st=LT48_OK;
    if(reselect) {
        const unsigned char select[]={0x3b,0xaa,0x55,0,0x50};
        st=lt48_runtime_transport_session_controller_write(s->wire,select,sizeof select);
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK)st=lt48_re2_guard_check(s->api,s->ctx,0);
    if(st==LT48_OK && reselect) {
        /* Original command 0 performs pin recovery and returns to controller. */
        const unsigned char end[]={0x2c,1,0,0,0};unsigned char reply[512];
        st=command_re2(s,end,sizeof end,reply);
        if(st==LT48_OK && (reply[0]!=1 || reply[1]!=0))st=LT48_PROTOCOL_ERROR;
    }
    return st;
}
static lt48_status settings_read_re2(re2_session *s,unsigned char out[5]) {
    s->mode=13;address_re2(s,0);configure_re2(s,NULL);
    lt48_status st=clear_guard_re2(s);
    if(st==LT48_OK)st=run_re2(s,6);
    if(st==LT48_OK)st=query_guard_re2(s,1);
    for(unsigned i=0;st==LT48_OK && i<5;i++) {
        if(s->config[i].value>(i<4?1u:3u))st=LT48_DEVICE_MISMATCH;
        else out[i]=(unsigned char)s->config[i].value;
    }
    return st;
}
static lt48_status settings_write_re2(re2_session *s,const unsigned char *desired,int fuses) {
    s->mode=13;address_re2(s,0);configure_re2(s,desired);
    lt48_status st=run_re2(s,14);
    if(st==LT48_OK)st=run_re2(s,fuses?12:13);
    if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
    unsigned char actual[5];if(st==LT48_OK)st=settings_read_re2(s,actual);
    if(st==LT48_OK)for(unsigned i=fuses?0u:4;i<(fuses?4u:5u);i++)
        if(actual[i]!=desired[i])st=LT48_DEVICE_MISMATCH;
    return st;
}
static lt48_status restore_re2(re2_session *s) {
    lt48_status st=settings_write_re2(s,s->saved,1);
    if(st==LT48_OK)s->restore=0;
    return st;
}
static lt48_status read_re2(re2_session *s,unsigned start,size_t n,unsigned char *out,unsigned vcc) {
    /* Source setup starts at wire address zero. Read and discard prefixes
     * for partial ranges; preserve the two independent 64-KiB banks. */
    s->vars[0].value=vcc;s->mode=1;size_t done=0;lt48_status st=LT48_OK;unsigned char block[4096];
    while(st==LT48_OK && done<n) {
        unsigned a=start+(unsigned)done;uint32_t bank,wire,remaining;
        st=lt48_re2_address(a,&bank,&wire,&remaining);
        if(st!=LT48_OK)break;
        unsigned base=a-wire,boundary=a+remaining;
        size_t take=n-done;if(take>boundary-a)take=boundary-a;
        address_re2(s,base);st=clear_guard_re2(s);
        if(st==LT48_OK)st=run_re2(s,15);
        if(st==LT48_OK)st=run_re2(s,5);
        size_t offset=0,total=a-base+take;
        while(st==LT48_OK && offset<total) {
            size_t chunk=total-offset;if(chunk>sizeof block)chunk=sizeof block;
            st=block_re2(s,0,block,chunk);
            if(st==LT48_OK)st=query_guard_re2(s,0);
            size_t skip=a-base>offset?a-base-offset:0;if(skip>chunk)skip=chunk;
            if(st==LT48_OK && skip<chunk)memcpy(out+done+offset+skip-(a-base),block+skip,chunk-skip);
            offset+=chunk;
        }
        if(st==LT48_OK)st=stop_re2(s);
        if(st==LT48_OK)st=run_re2(s,3);
        done+=take;
    }
    s->vars[0].value=330;return st;
}
static lt48_status write_re2_region(re2_session *s,unsigned base,unsigned end,const unsigned char *image) {
    s->mode=2;address_re2(s,base);lt48_status st=run_re2(s,15);
    if(st==LT48_OK)st=run_re2(s,8);
    for(unsigned a=base;st==LT48_OK && a<end;) {
        size_t n=end-a;if(n>4096)n=4096;
        st=block_re2(s,1,(unsigned char *)image+a,n);a+=(unsigned)n;
    }
    if(st==LT48_OK)st=stop_re2(s);
    if(st==LT48_OK)st=run_re2(s,3);
    return st;
}
static lt48_status write_re2(re2_session *s,const unsigned char *image) {
    unsigned bases[]={0,65536},ends[]={65536,131072};
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=write_re2_region(s,bases[i],ends[i],image);
    return st;
}
static lt48_status mismatch_re2(lt48_serial_page_result *out,unsigned base,const unsigned char *a,const unsigned char *b,size_t n,int blank) {
    for(size_t i=0;i<n;i++)if(a[i]!=(blank?255:b[i])) {
        if(!out->mismatch_count)out->first_mismatch=base+(unsigned)i;
        out->mismatch_count++;
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status erase_re2(re2_session *s,unsigned char *saved,unsigned char *actual,lt48_serial_page_result *out) {
    saved[4]=0;memcpy(s->saved,saved,5);s->restore=1;
    s->mode=7;address_re2(s,0);lt48_status st=run_re2(s,10);
    if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=restore_re2(s);
    unsigned char cfg[5];if(st==LT48_OK)st=settings_read_re2(s,cfg);
    if(st==LT48_OK && cfg[4]!=0)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=read_re2(s,0,131072,actual,330);
    if(st==LT48_OK)st=mismatch_re2(out,0,actual,NULL,131072,1);
    return st;
}
static lt48_status cleanup_re2(void *v,unsigned phase) {
    re2_session *s=v;
    if(phase==0)return stop_re2(s);
    if(phase==1)return s->ready?run_re2(s,3):LT48_OK;
    if(phase==2)return run_re2(s,1);
    if(phase==4)return s->restore?restore_re2(s):LT48_OK;
    return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
}
static lt48_status setup_re2(re2_session *s) {
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<6;i++) {
        const lt48_wave_code *a=&s->r->applet[i];
        st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
        if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,a->load_address,a->data,a->data_size);
    }
    if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);
    if(st==LT48_OK)st=run_re2(s,2);
    if(st==LT48_OK)st=clear_guard_re2(s);
    if(st==LT48_OK)st=run_re2(s,0);
    if(st==LT48_OK)st=query_guard_re2(s,1);
    if(st==LT48_OK && (s->buffers[7][0]!=0x58 || s->buffers[5][0]!=0x57 ||
       s->buffers[12][0]!=0xff || s->buffers[13][0]!=0xfa))st=LT48_DEVICE_MISMATCH;
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof busy);
    return st;
}
lt48_status lt48_re2_execute(const lt48_re2_resources *r,const lt48_protocol_transport *api,void *ctx,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void *decision_ctx,lt48_serial_page_result *out) {
    if(!out || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_re2_validate(r,op,start,length,image,size);
    if(st==LT48_OK && (type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
    unsigned fields=r?5u:0;
    if(st==LT48_OK && ((op==LT48_OPERATION_READ && (!read || read_size<length)) || (op==LT48_OPERATION_CONFIGURATION && (!read || read_size<fields))))st=LT48_BUFFER_TOO_SMALL;
    if(st!=LT48_OK){out->operation_status=st;return st;}
    if(api->cancelled && api->cancelled(ctx)){out->operation_status=LT48_CANCELLED;out->cleanup_completed=1;return LT48_CANCELLED;}
    unsigned char *actual=malloc((131072u)),saved[5],desired[5];
    if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    re2_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=13;memcpy(s.vars,r->vars,sizeof s.vars);
    lt48_protocol_transport clean=*api;clean.cancelled=NULL;int setup_uncertain=0,setup_started=0;
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
    if(st==LT48_OK) {
        setup_started=1;s.api=&clean;st=setup_re2(&s);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
        setup_uncertain=st!=LT48_OK;s.ready=st==LT48_OK;
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;s.api=api;
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
    /* RE2 has an entry handshake, not a catalogue JEDEC ID comparison. */
    (void)decide;(void)decision_ctx;
    if(st==LT48_OK)st=settings_read_re2(&s,saved);
    int programming=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)) {
        memcpy(desired,saved,fields);
        if(op==LT48_OPERATION_SECURE){unsigned level=image?image[0]:3;if(level>desired[4])desired[4]=(unsigned char)level;}
        else if(image)for(unsigned i=0;i<5;i++)if(image[0]&(1u<<i))desired[i]=image[i+1];
        int fuses=memcmp(desired,saved,4)!=0,locks=desired[4]!=saved[4];
        if(desired[4]<saved[4] || (fuses && saved[4]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && fuses)st=settings_write_re2(&s,desired,1);
        if(st==LT48_OK && locks)st=settings_write_re2(&s,desired,0);
        if(st==LT48_OK)memcpy(saved,desired,fields);
    } else if(st==LT48_OK) {
        if(op==LT48_OPERATION_ERASE || op==LT48_OPERATION_PROGRAM)st=erase_re2(&s,saved,actual,out);
        else if(saved[4]>=2 || (programming && saved[4]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && programming) {
            if(op==LT48_OPERATION_OVER_PROGRAM) {
                st=read_re2(&s,0,131072,actual,330);
                for(unsigned i=0;st==LT48_OK && i<131072;i++)if((actual[i]&image[i])!=image[i]){out->first_mismatch=i;out->mismatch_count=1;st=LT48_DEVICE_MISMATCH;}
            }
            if(st==LT48_OK)st=write_re2(&s,image);
            unsigned volts[]={type?300u:314u,type?360u:346u};
            for(unsigned i=0;st==LT48_OK && i<2;i++) {
                st=read_re2(&s,0,(131072u),actual,volts[i]);
                if(st==LT48_OK)st=mismatch_re2(out,0,actual,image,(131072u),0);
            }
        } else if(st==LT48_OK && op!=LT48_OPERATION_ERASE) {
            unsigned volts[]={type?300u:314u,type?360u:346u};
            unsigned passes=op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL?2:1;
            if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL)volts[0]=330;
            else if(margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
            for(unsigned i=0;st==LT48_OK && i<passes;i++) {
                st=read_re2(&s,start,length,actual,volts[i]);
                if(st==LT48_OK && op!=LT48_OPERATION_READ)st=mismatch_re2(out,start,actual,image,length,op==LT48_OPERATION_BLANK_CHECK);
            }
        }
    }
    if(st==LT48_OK && s.restore)st=restore_re2(&s);
    if(st==LT48_OK)st=settings_read_re2(&s,desired);
    if(st==LT48_OK && memcmp(saved,desired,fields))st=LT48_DEVICE_MISMATCH;
    out->operation_status=st;
    if(!setup_started){lt48_runtime_transport_session_close(s.wire);out->cleanup_completed=1;free(actual);return st;}
    s.api=&clean;const unsigned phases[]={0,1,4,2,3};
    st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_re2,&s,phases,5,st,&out->cleanup_status,&out->cleanup_completed);
    if(setup_uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
    if(st==LT48_OK) {
        if(op==LT48_OPERATION_READ)memcpy(read,actual,length);
        if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,desired,fields);
        out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?fields:length;
    }
    free(actual);return st;
}
