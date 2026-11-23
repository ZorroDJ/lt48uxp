#ifndef LT48_PIC_BASELINE12_OTP_SERIAL_H
#define LT48_PIC_BASELINE12_OTP_SERIAL_H
#include "baseline12_otp_operator.h"
#include "classic14_protocol.h"
/* DS30557G/DS30603B serial grammar only. Exact package, power entry,
 * controller-byte implementation and source host join remain separate gates.
 * No Flash commands (09h etc.) are available through this adapter. */
typedef struct {
    lt48_pic_classic_wire wire;
    void *voltage_context;
    lt48_status (*select_voltage)(void *,unsigned);
    unsigned code_words;
} b12otp_serial;
static inline int b12otp_serial_valid(const b12otp_serial *s) {
    return s&&lt48_pic_classic_wire_valid(&s->wire)&&s->select_voltage&&
        (s->code_words==512||s->code_words==1024);
}
static inline lt48_status b12otp_serial_enter(void *v,unsigned address,unsigned level) {
    b12otp_serial *s=v;
    if(!b12otp_serial_valid(s)||(address!=4095&&address>=s->code_words+4)||
       (level!=0&&level!=5&&level!=6))return LT48_INVALID_ARGUMENT;
    lt48_status st=s->select_voltage(s->voltage_context,level);
    if(st==LT48_OK)st=s->wire.reenter(s->wire.context);
    if(address==4095)return st;
    for(unsigned a=0;st==LT48_OK&&a<=address;a++)st=lt48_pic_classic_command(&s->wire,6);
    return st;
}
static inline lt48_status b12otp_serial_read(void *v,uint16_t *out) {
    b12otp_serial *s=v;if(!b12otp_serial_valid(s)||!out)return LT48_INVALID_ARGUMENT;
    uint16_t got=0;lt48_status st=lt48_pic_classic_command(&s->wire,4);
    if(st==LT48_OK)st=lt48_pic_classic_frame(&s->wire,1,&got);
    if(st==LT48_OK)*out=got&4095;
    return st;
}
static inline lt48_status b12otp_serial_increment(void *v) {
    b12otp_serial *s=v;if(!b12otp_serial_valid(s))return LT48_INVALID_ARGUMENT;
    return lt48_pic_classic_command(&s->wire,6);
}
static inline lt48_status b12otp_serial_pulses(void *v,uint16_t target,unsigned us,unsigned count) {
    b12otp_serial *s=v;
    if(!b12otp_serial_valid(s)||target>4095||us!=100||!count||count>10000)return LT48_INVALID_ARGUMENT;
    lt48_status st=lt48_pic_classic_load(&s->wire,2,target);
    for(unsigned n=0;st==LT48_OK&&n<count;n++) {
        st=lt48_pic_classic_command(&s->wire,8);
        if(st==LT48_OK)st=s->wire.delay(s->wire.context,us);
        if(st==LT48_OK)st=lt48_pic_classic_command(&s->wire,14);
    }
    return st;
}
#endif
