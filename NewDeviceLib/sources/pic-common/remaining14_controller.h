#ifndef LT48_PIC_REMAINING14_CONTROLLER_H
#define LT48_PIC_REMAINING14_CONTROLLER_H
#include "../../../c/src/internal.h"
#include "remaining14_parity.h"

/* Opt-in raw16 bridge for the parity OTPs. Raw captures remain physical
 * words, including both OTP parity cells. Source-bound power/FPGA entry,
 * commands, frame START/STOP, addresses and cleanup belong to the owner.
 * This does not widen the existing public classic14 transfer interface. */
typedef struct {
    lt48_runtime_transport_session *wire;
    unsigned data,clock;
} rm14_controller;

static inline lt48_status rm14_controller_bind(rm14_controller *c,
    lt48_runtime_transport_session *wire,const lt48_pin_map *pins,
    unsigned data_logical,unsigned clock_logical) {
    if(!c||!wire||!pins||data_logical>UINT16_MAX||clock_logical>UINT16_MAX)
        return LT48_INVALID_ARGUMENT;
    lt48_logical_pin d,k;
    lt48_status st=lt48_pin_map_find_logical(pins,(uint16_t)data_logical,&d);
    if(st==LT48_OK)st=lt48_pin_map_find_logical(pins,(uint16_t)clock_logical,&k);
    if(st!=LT48_OK)return st;
    if(d.engine_pin>=192||k.engine_pin>=192||d.engine_pin%4||k.engine_pin%4||d.engine_pin==k.engine_pin)
        return LT48_SAFETY_REJECTED;
    *c=(rm14_controller){wire,d.engine_pin,k.engine_pin};return LT48_OK;
}

static inline lt48_status rm14_controller_transfer(void *context,int read,
    unsigned bits,uint32_t value,uint32_t *out) {
    rm14_controller *c=context;
    if(!c||!c->wire||!out||(read!=0&&read!=1)||c->data>=192||c->clock>=192||
       c->data%4||c->clock%4||c->data==c->clock||
       (read&&bits!=16)||(!read&&bits!=1&&bits!=6&&bits!=16)||
       (bits!=1&&bits!=6&&bits!=16)||value>=(UINT32_C(1)<<bits))return LT48_INVALID_ARGUMENT;
    const unsigned char packet[]={(unsigned char)(read?0x60:0x58),
        (unsigned char)(c->data+(read?2:(value&1))),
        (unsigned char)c->clock,(unsigned char)(bits-1),
        (unsigned char)value,(unsigned char)(value>>8)};
    lt48_status st=LT48_OK;
    /* Resident60 samples first, then emits bits-1 clocks. The source primes
     * that first sample with18 after START. Capture failure cannot overwrite
     * the caller's raw word with zero or stale data. */
    if(read){const unsigned char prime[]={0x18,(unsigned char)(c->clock+1)};
        st=lt48_runtime_transport_session_controller_write(c->wire,prime,sizeof prime);}
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c->wire,packet,read?4:4+(bits+7)/8);
    if(st==LT48_OK&&read) {
        unsigned char bytes[2];
        st=lt48_runtime_transport_session_controller_capture_internal(c->wire,bytes,2);
        if(st==LT48_OK)*out=bytes[0]|(uint32_t)bytes[1]<<8;
    }else if(st==LT48_OK)*out=0;
    return st;
}
#endif
