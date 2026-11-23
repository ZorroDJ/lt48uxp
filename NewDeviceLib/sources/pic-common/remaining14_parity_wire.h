#ifndef LT48_PIC_REMAINING14_PARITY_WIRE_H
#define LT48_PIC_REMAINING14_PARITY_WIRE_H
#include "remaining14_parity.h"
#include "classic14_protocol.h"
/* DS30278B/DS30457A:18 clocks, START/16 physical cells/STOP. The wire
 * preserves both parity cells; voltage, entry and global cleanup are owned
 * by the exact source adapter. No profile admission occurs here. */
static inline lt48_status rm14_parity_frame(const lt48_pic_classic_wire *w,int read,uint16_t *value) {
    if(!w||!w->transfer||!w->delay||!value||(read!=0&&read!=1))return LT48_INVALID_ARGUMENT;
    uint32_t out=0;
    lt48_status st=w->transfer(w->context,0,1,0,&out);
    if(st==LT48_OK)st=w->transfer(w->context,read,16,read?0:*value,&out);
    uint16_t captured=(uint16_t)out;
    if(st==LT48_OK)st=w->transfer(w->context,0,1,0,&out);
    if(st==LT48_OK)st=w->delay(w->context,1);
    if(st==LT48_OK&&read)*value=captured;
    return st;
}
static inline lt48_status rm14_parity_load(const lt48_pic_classic_wire *w,unsigned command,uint16_t raw) {
    if(!w||!w->transfer||!w->delay||(command!=0&&command!=2))return LT48_INVALID_ARGUMENT;
    lt48_status st=lt48_pic_classic_command(w,command);
    return st==LT48_OK?rm14_parity_frame(w,0,&raw):st;
}
static inline lt48_status rm14_parity_read_raw(const lt48_pic_classic_wire *w,uint16_t *raw) {
    if(!w||!w->transfer||!w->delay||!raw)return LT48_INVALID_ARGUMENT;
    lt48_status st=lt48_pic_classic_command(w,4);
    return st==LT48_OK?rm14_parity_frame(w,1,raw):st;
}
typedef lt48_status (*rm14_parity_pulse)(void *);
static inline lt48_status rm14_parity_cycle_with_pulse(const lt48_pic_classic_wire *w,uint16_t raw,rm14_parity_pulse pulse,void *context) {
    /* Primary program-cycle box includes Load Data on every cycle. */
    lt48_status st=rm14_parity_load(w,2,raw);
    if(st==LT48_OK&&pulse)return pulse(context);
    if(st==LT48_OK)st=lt48_pic_classic_command(w,8);
    if(st==LT48_OK)st=w->delay(w->context,100);
    if(st==LT48_OK)st=lt48_pic_classic_command(w,14);
    return st;
}
static inline lt48_status rm14_parity_program_masked_with_pulse(const lt48_pic_classic_wire *w,unsigned logical,uint16_t mask,rm14_parity_pulse pulse,void *context) {
    if(!w||!w->transfer||!w->delay||!mask)return LT48_INVALID_ARGUMENT;
    uint16_t target;lt48_status st=rm14_parity_encode(logical,&target);
    if(st!=LT48_OK)return st;
    unsigned attempts=0;int matched=0;
    for(;attempts<25;attempts++) {
        uint16_t raw=0;st=rm14_parity_cycle_with_pulse(w,target,pulse,context);
        if(st==LT48_OK)st=rm14_parity_read_raw(w,&raw);
        if(st!=LT48_OK)return st;
        if(!((raw^target)&mask)){attempts++;matched=1;break;}
    }
    if(!matched)return LT48_DEVICE_MISMATCH;
    for(unsigned i=0;i<3*attempts;i++) {
        st=rm14_parity_cycle_with_pulse(w,target,pulse,context);if(st!=LT48_OK)return st;
    }
    return LT48_OK;
}
static inline lt48_status rm14_parity_program_word(const lt48_pic_classic_wire *w,unsigned logical) {
    return rm14_parity_program_masked_with_pulse(w,logical,0xffff,NULL,NULL);
}
static inline lt48_status rm14_parity_program_configuration_with_pulse(const lt48_pic_classic_wire *w,unsigned logical,rm14_parity_pulse pulse,void *context) {
    uint16_t target;lt48_status st=rm14_parity_encode(logical,&target);
    if(st!=LT48_OK)return st;
    for(unsigned i=0;i<100;i++) {
        st=rm14_parity_cycle_with_pulse(w,target,pulse,context);if(st!=LT48_OK)return st;
    }
    uint16_t raw=0;st=rm14_parity_read_raw(w,&raw);
    /* Parity is clocked but not checked for configuration/ID locations. */
    return st==LT48_OK&&rm14_parity_decode(raw)!=logical?LT48_DEVICE_MISMATCH:st;
}
static inline lt48_status rm14_parity_program_masked(const lt48_pic_classic_wire *w,unsigned logical,uint16_t mask) {
 return rm14_parity_program_masked_with_pulse(w,logical,mask,NULL,NULL);
}
static inline lt48_status rm14_parity_program_configuration(const lt48_pic_classic_wire *w,unsigned logical) {
 return rm14_parity_program_configuration_with_pulse(w,logical,NULL,NULL);
}
#endif
