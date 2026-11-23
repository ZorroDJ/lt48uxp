#ifndef LT48_PIC_BASELINE12_OTP_PARALLEL_H
#define LT48_PIC_BASELINE12_OTP_PARALLEL_H
#include "baseline12_otp_operator.h"
/* DS30190H sections 1.1--1.3. Logical bus adapter only: exact pin maps,
 * voltage entry and resident byte commands must be separately qualified. */
enum { B12OTP_PROG=0,B12OTP_INCPC=1 };
typedef struct {
    void *context;
    /* Fresh physical entry: config selected, PROG high, INCPC low. */
    lt48_status (*entry)(void *,unsigned voltage_field);
    lt48_status (*control)(void *,unsigned signal,unsigned high);
    lt48_status (*drive_data)(void *,uint16_t);
    lt48_status (*release_data)(void *);
    lt48_status (*read_data)(void *,uint16_t *);
    lt48_status (*delay)(void *,unsigned);
    /* Atomic low/high PROG cycle, ending high; duration excludes USB latency. */
    lt48_status (*program_pulse)(void *,unsigned);
    unsigned code_words,access_us,address,level,clock_high,verify_pending,entered;
} b12otp_parallel;
static inline int b12otp_parallel_valid(const b12otp_parallel *p) {
    return p&&p->entry&&p->control&&p->drive_data&&p->release_data&&p->read_data&&
        p->delay&&p->program_pulse&&p->access_us&&p->access_us<=100&&
        (p->code_words==512||p->code_words==1024||p->code_words==2048);
}
static inline lt48_status b12otp_parallel_enter(void *v,unsigned address,unsigned level) {
    b12otp_parallel *p=v;
    if(!b12otp_parallel_valid(p)||(address!=4095&&address>=p->code_words+4)||
       (level!=0&&level!=5&&level!=6))return LT48_INVALID_ARGUMENT;
    p->entered=0;
    lt48_status st=p->entry(p->context,level);
    if(st==LT48_OK)st=p->release_data(p->context);
    if(address!=4095)for(unsigned a=0;st==LT48_OK&&a<=address;a++) {
        st=p->control(p->context,B12OTP_INCPC,1);
        if(st==LT48_OK)st=p->delay(p->context,p->access_us);
        if(st==LT48_OK)st=p->control(p->context,B12OTP_INCPC,0);
    }
    if(st==LT48_OK){p->entered=1;p->address=address;p->level=level;p->clock_high=p->verify_pending=0;}
    return st;
}
static inline lt48_status b12otp_parallel_read(void *v,uint16_t *out) {
    b12otp_parallel *p=v;
    if(!b12otp_parallel_valid(p)||!p->entered||!out)return LT48_INVALID_ARGUMENT;
    lt48_status st=p->release_data(p->context);uint16_t got=0;
    if(st==LT48_OK)st=p->control(p->context,p->verify_pending?B12OTP_PROG:B12OTP_INCPC,p->verify_pending?0:1);
    if(st==LT48_OK)st=p->delay(p->context,p->access_us);
    if(st==LT48_OK)st=p->read_data(p->context,&got);
    /* Finish a verify pulse even after read failure. Fast-read INCPC stays
     * high: bringing it low here would silently advance the selected word. */
    if(p->verify_pending){lt48_status end=p->control(p->context,B12OTP_PROG,1);if(st==LT48_OK)st=end;}
    if(st==LT48_OK){p->clock_high=!p->verify_pending;p->verify_pending=0;*out=got;}
    else p->entered=0;
    return st;
}
static inline lt48_status b12otp_parallel_increment(void *v) {
    b12otp_parallel *p=v;
    if(!b12otp_parallel_valid(p)||!p->entered||p->verify_pending||p->address>=p->code_words+3)return LT48_INVALID_ARGUMENT;
    lt48_status st=LT48_OK;
    if(!p->clock_high)st=p->control(p->context,B12OTP_INCPC,1);
    if(st==LT48_OK)st=p->delay(p->context,p->access_us);
    if(st==LT48_OK)st=p->control(p->context,B12OTP_INCPC,0);
    if(st==LT48_OK){p->address++;p->clock_high=0;}else p->entered=0;
    return st;
}
static inline lt48_status b12otp_parallel_pulses(void *v,uint16_t target,unsigned us,unsigned count) {
    b12otp_parallel *p=v;
    if(!b12otp_parallel_valid(p)||!p->entered||p->level!=0||p->verify_pending||
       target>4095||us!=100||!count||count>10000)return LT48_INVALID_ARGUMENT;
    lt48_status st=LT48_OK;
    /* A fast read leaves INCPC high. Its falling edge changes PC, so writing
     * the same word requires fresh entry and seek rather than lowering it. */
    if(p->clock_high)st=b12otp_parallel_enter(p,p->address,p->level);
    for(unsigned n=0;st==LT48_OK&&n<count;n++) {
        st=p->drive_data(p->context,target);
        if(st==LT48_OK)st=p->program_pulse(p->context,us);
        if(st==LT48_OK)st=p->delay(p->context,p->access_us);
        if(st==LT48_OK)p->verify_pending=1;
        if(st==LT48_OK&&n+1<count) {
            st=p->release_data(p->context);
            if(st==LT48_OK)st=p->control(p->context,B12OTP_PROG,0);
            if(st==LT48_OK)st=p->delay(p->context,p->access_us);
            if(st==LT48_OK)st=p->control(p->context,B12OTP_PROG,1);
            if(st==LT48_OK)p->verify_pending=0;
        }
    }
    if(st!=LT48_OK)p->entered=0;
    return st;
}
#endif
