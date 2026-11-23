#ifndef LT48_PIC_BASELINE12_OTP_CONTROLLER_H
#define LT48_PIC_BASELINE12_OTP_CONTROLLER_H
#include "baseline12_otp_parallel.h"
#include "baseline12_otp_serial.h"
#include "../../../c/src/internal.h"
/* Real resident-controller bridge. Caller owns the exact source entry,
 * voltage, FPGA upload and cleanup. This header grants no device admission. */
typedef struct {
    lt48_runtime_transport_session *wire;
    const lt48_pin_map *pins;
    void *context;
    lt48_status (*entry)(void *,unsigned);
    unsigned prog,incpc,data,clock;
    lt48_compiled_wave *parallel_waves[7];
} b12otp_controller;
static inline void b12otp_controller_close(b12otp_controller *c) {
    if(c)for(unsigned i=0;i<7;i++){lt48_compiled_wave_close(c->parallel_waves[i]);c->parallel_waves[i]=NULL;}
}
static inline lt48_status b12otp_controller_table(const uint16_t *words,unsigned n,lt48_compiled_wave **out) {
    if(!words||!n||n>8||!out)return LT48_INVALID_ARGUMENT;
    unsigned char raw[18];raw[0]=2*n;raw[1]=0;
    for(unsigned i=0;i<n;i++){raw[2+2*i]=words[i];raw[3+2*i]=words[i]>>8;}
    return lt48_compiled_word_table_internal(raw,2+2*n,out);
}
static inline lt48_status b12otp_controller_bind_parallel(b12otp_controller *c) {
    if(!c||!c->wire||!c->pins||!c->entry)return LT48_INVALID_ARGUMENT;
    for(unsigned i=0;i<7;i++)if(c->parallel_waves[i])return LT48_INVALID_ARGUMENT;
    lt48_logical_pin prog,inc;lt48_pin_group group;
    lt48_status st=lt48_pin_map_find_logical(c->pins,15,&prog);
    if(st==LT48_OK)st=lt48_pin_map_find_logical(c->pins,16,&inc);
    if(st==LT48_OK)st=lt48_pin_map_find_group(c->pins,9,&group);
    if(st!=LT48_OK)return st;
    if(prog.engine_pin>=192||inc.engine_pin>=192||prog.engine_pin%4||inc.engine_pin%4||
       prog.engine_pin==inc.engine_pin||group.pin_count!=12)return LT48_SAFETY_REJECTED;
    for(unsigned i=0;i<12;i++) {
        if(group.pins[i]>=192||group.pins[i]%4||group.pins[i]==prog.engine_pin||group.pins[i]==inc.engine_pin)return LT48_SAFETY_REJECTED;
        for(unsigned j=0;j<i;j++)if(group.pins[j]==group.pins[i])return LT48_SAFETY_REJECTED;
    }
    c->prog=prog.engine_pin;c->incpc=inc.engine_pin;
    for(unsigned i=0;st==LT48_OK&&i<4;i++) {
        uint16_t words[]={0,(uint16_t)((i<2?c->prog:c->incpc)+(i&1))};
        st=b12otp_controller_table(words,2,c->parallel_waves+i);
    }
    const uint16_t release[]={7,9,2},drive[]={9,9,6},read[]={11,9,5};
    if(st==LT48_OK)st=b12otp_controller_table(release,3,c->parallel_waves+4);
    if(st==LT48_OK)st=b12otp_controller_table(drive,3,c->parallel_waves+5);
    if(st==LT48_OK)st=b12otp_controller_table(read,3,c->parallel_waves+6);
    if(st!=LT48_OK)b12otp_controller_close(c);
    return st;
}
static inline lt48_status b12otp_controller_bind_serial(b12otp_controller *c,unsigned data,unsigned clock) {
    if(!c||!c->wire||!c->pins||!c->entry)return LT48_INVALID_ARGUMENT;
    lt48_logical_pin d,k;lt48_status st=lt48_pin_map_find_logical(c->pins,data,&d);
    if(st==LT48_OK)st=lt48_pin_map_find_logical(c->pins,clock,&k);
    if(st!=LT48_OK)return st;
    if(d.engine_pin>=192||k.engine_pin>=192||d.engine_pin%4||k.engine_pin%4||d.engine_pin==k.engine_pin)return LT48_SAFETY_REJECTED;
    c->data=d.engine_pin;c->clock=k.engine_pin;return LT48_OK;
}
static inline lt48_status b12otp_controller_run(b12otp_controller *c,unsigned index,uint16_t value,uint16_t *read) {
    if(!c||!c->wire||index>=7||!c->parallel_waves[index])return LT48_INVALID_ARGUMENT;
    unsigned char bytes[2]={(unsigned char)value,(unsigned char)(value>>8)};
    lt48_runtime_buffer_seed seed={6,bytes,2};lt48_runtime_options opts={0};
    opts.initial_buffers=&seed;opts.initial_buffer_count=1;opts.require_capture_responses=1;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    lt48_status st=lt48_runtime_machine_create(c->parallel_waves[index],c->pins,NULL,0,&opts,&machine);
    if(st==LT48_OK)st=lt48_runtime_transport_session_run(c->wire,machine,&result);
    if(st==LT48_OK&&read) {
        const unsigned char *data;size_t n;
        st=lt48_runtime_get_buffer(result,5,&data,&n);
        if(st==LT48_OK&&n<2)st=LT48_PROTOCOL_ERROR;
        if(st==LT48_OK)*read=(data[0]|(uint16_t)data[1]<<8)&4095;
    }
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return st;
}
static inline lt48_status b12otp_controller_entry(void *v,unsigned level) {
    b12otp_controller *c=v;if(!c||!c->entry)return LT48_INVALID_ARGUMENT;
    return c->entry(c->context,level);
}
static inline lt48_status b12otp_controller_control(void *v,unsigned signal,unsigned high) {
    if(signal>1||high>1)return LT48_INVALID_ARGUMENT;
    return b12otp_controller_run(v,2*signal+high,0,NULL);
}
static inline lt48_status b12otp_controller_drive(void *v,uint16_t value) {
    if(value>4095)return LT48_INVALID_ARGUMENT;
    return b12otp_controller_run(v,5,value,NULL);
}
static inline lt48_status b12otp_controller_release(void *v){return b12otp_controller_run(v,4,0,NULL);}
static inline lt48_status b12otp_controller_read(void *v,uint16_t *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    return b12otp_controller_run(v,6,0,out);
}
static inline lt48_status b12otp_controller_delay(void *v,unsigned us) {
    b12otp_controller *c=v;if(!c||!c->wire||!us||us>255)return LT48_INVALID_ARGUMENT;
    const unsigned char bytes[]={0x4e,(unsigned char)us};
    return lt48_runtime_transport_session_controller_write(c->wire,bytes,sizeof bytes);
}
static inline lt48_status b12otp_controller_pulse(void *v,unsigned us) {
    b12otp_controller *c=v;if(!c||!c->wire||!c->parallel_waves[0]||us!=100)return LT48_INVALID_ARGUMENT;
    const unsigned char bytes[]={0x19,(unsigned char)c->prog,97};
    return lt48_runtime_transport_session_controller_write(c->wire,bytes,sizeof bytes);
}
static inline lt48_status b12otp_controller_serial_transfer(void *v,int read,unsigned bits,uint32_t value,uint32_t *out) {
    b12otp_controller *c=v;
    if(!c||!c->wire||c->data==c->clock||!out||(read&&bits!=14)||(!read&&bits!=1&&bits!=6&&bits!=14)||value>=(1u<<bits))return LT48_INVALID_ARGUMENT;
    const unsigned char bytes[]={(unsigned char)(read?0x60:0x58),(unsigned char)(c->data+(read?2:value&1)),
        (unsigned char)c->clock,(unsigned char)(bits-1),(unsigned char)value,(unsigned char)(value>>8)};
    lt48_status st=LT48_OK;
    /* Resident60 samples before toggling and emits only bits-1 clocks.
     * Match the original source's leading18 pulse after the START bit. */
    if(read){const unsigned char prime[]={0x18,(unsigned char)(c->clock+1)};
        st=lt48_runtime_transport_session_controller_write(c->wire,prime,sizeof prime);}
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c->wire,bytes,read?4:4+(bits+7)/8);
    if(st==LT48_OK&&read) {
        unsigned char got[2];st=lt48_runtime_transport_session_controller_capture_internal(c->wire,got,2);
        if(st==LT48_OK)*out=(got[0]|(uint32_t)got[1]<<8)&4095;
    } else if(st==LT48_OK)*out=0;
    return st;
}
#endif
