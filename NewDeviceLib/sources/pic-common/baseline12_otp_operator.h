#ifndef LT48_PIC_BASELINE12_OTP_OPERATOR_H
#define LT48_PIC_BASELINE12_OTP_OPERATOR_H
#include "cleanup.h"
#include <string.h>
/* Private operation owner. No admission follows from policy validity. Exact
 * source callbacks, serial/parallel adapters and profiles require audit. */
enum { B12OTP_WORDS=4096, B12OTP_CONFIG=4095 };
typedef struct {
    unsigned code_words;
    uint16_t configuration_mask,protection_mask,blank_configuration_mask;
    unsigned calibration,calibration_visible_protected,retry_limit;
    unsigned pulse_us,margin_factor,configuration_us;
    /* PIC12/PIC16C505 count failed attempts; parallel counts all attempts. */
    unsigned margin_excludes_success;
    /* Optional source policy extensions; zero retains the existing PIC policy. */
    unsigned id_mask,margin_addend,configuration_smart,calibration_untyped;
} b12otp_policy;
typedef struct {
    void *context;
    /* Fresh entry selects config. Adapter seeks from there, not by assuming
     * an increment from code wraps back into configuration. Voltage is the
     * original variable field: 0 nominal/program, 5 minimum, 6 maximum. */
    lt48_status (*enter)(void *,unsigned address,unsigned voltage_field);
    lt48_status (*read)(void *,uint16_t *);
    lt48_status (*increment)(void *);
    /* Complete count program cycles and leave the same address selected.
     * Parallel adapters must consume intervening verify phases. */
    lt48_status (*pulses)(void *,uint16_t,unsigned pulse_us,unsigned count);
    lt48_pic_cleanup_io cleanup;
} b12otp_io;
typedef struct {
    lt48_status operation_status;
    lt48_pic_cleanup_result cleanup;
    unsigned mismatches,first_mismatch,programmed_words;
    uint16_t calibration;
} b12otp_result;
static inline unsigned b12otp_mask(const b12otp_policy *p,unsigned a) {
    return a<p->code_words?4095:a<p->code_words+4?(p->id_mask?p->id_mask:15):
        a==B12OTP_CONFIG?p->configuration_mask:0;
}
static inline int b12otp_overlap(const uint16_t *a,const uint16_t *b) {
    if(!a||!b)return 0;
    uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
    return (x<y?y-x:x-y)<B12OTP_WORDS*sizeof(*a);
}
static inline int b12otp_valid(const b12otp_policy *p) {
    return p&&(p->code_words==512||p->code_words==1024||p->code_words==2048)&&
        p->configuration_mask&&p->configuration_mask<=4095&&p->protection_mask&&
        !(p->protection_mask&~p->configuration_mask)&&
        !(p->blank_configuration_mask&~p->configuration_mask)&&
        p->retry_limit&&p->retry_limit<=255&&p->pulse_us&&p->pulse_us<=10000&&
        p->margin_factor&&p->margin_factor<=32&&p->margin_excludes_success<=1&&p->id_mask<=4095&&p->margin_addend<=32&&p->configuration_smart<=1&&p->calibration_untyped<=1&&(!p->calibration_untyped||p->calibration)&&p->configuration_us&&
        p->configuration_us%p->pulse_us==0&&p->configuration_us/p->pulse_us<=10000;
}
static inline lt48_status b12otp_scan(const b12otp_policy *p,const b12otp_io *io,
    unsigned level,uint16_t *out) {
    for(unsigned a=0;a<B12OTP_WORDS;a++)out[a]=4095;
    lt48_status st=io->enter(io->context,0,level);
    for(unsigned a=0;st==LT48_OK&&a<p->code_words+4;a++) {
        st=io->read(io->context,out+a);
        if(st==LT48_OK&&out[a]>4095)st=LT48_PROTOCOL_ERROR;
        if(st==LT48_OK&&a+1<p->code_words+4)st=io->increment(io->context);
    }
    if(st==LT48_OK)st=io->enter(io->context,B12OTP_CONFIG,level);
    if(st==LT48_OK)st=io->read(io->context,out+B12OTP_CONFIG);
    if(st==LT48_OK&&out[B12OTP_CONFIG]>4095)st=LT48_PROTOCOL_ERROR;
    return st;
}
static inline lt48_status b12otp_compare(const b12otp_policy *p,const uint16_t *want,
    const uint16_t *got,int blank,int preserved,uint16_t cal,uint16_t cfg,b12otp_result *r) {
    r->mismatches=0;r->first_mismatch=UINT32_MAX;
    for(unsigned a=0;a<B12OTP_WORDS;a++) {
        unsigned mask=b12otp_mask(p,a),v=blank?4095:want[a];
        if(blank&&p->calibration&&a==p->code_words-1)continue;
        if(!blank&&!preserved&&p->calibration&&a==p->code_words-1&&want[a]==4095)continue;
        if(blank&&a==B12OTP_CONFIG)mask=p->blank_configuration_mask;
        if(preserved&&p->calibration&&a==p->code_words-1)v=cal;
        if(preserved&&a==B12OTP_CONFIG)v=cfg;
        if((v^got[a])&mask) {
            if(!r->mismatches)r->first_mismatch=2*a;
            r->mismatches++;
        }
    }
    return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
/* Source retry/verify and extra pulses, reused by code and smart configuration. */
static inline lt48_status b12otp_program_word(const b12otp_policy *p,
    const b12otp_io *io,uint16_t target,unsigned mask) {
    unsigned n=0;
    uint16_t got=0;
    lt48_status st=LT48_OK;
    do {
        st=io->pulses(io->context,target,p->pulse_us,1);
        n++;
        if(st==LT48_OK)st=io->read(io->context,&got);
    } while(st==LT48_OK&&((got^target)&mask)&&n<p->retry_limit);
    if(st==LT48_OK&&((got^target)&mask))st=LT48_DEVICE_MISMATCH;
    unsigned margin=p->margin_factor*(n-p->margin_excludes_success)+p->margin_addend;
    if(st==LT48_OK&&margin)st=io->pulses(io->context,target,p->pulse_us,margin);
    if(st==LT48_OK)st=io->read(io->context,&got);
    if(st==LT48_OK&&((got^target)&mask))st=LT48_DEVICE_MISMATCH;
    return st;
}
static inline lt48_status b12otp_configuration(const b12otp_policy *p,const b12otp_io *io,uint16_t target) {
    uint16_t old=0,got=0;
    lt48_status st=io->enter(io->context,B12OTP_CONFIG,0);
    if(st==LT48_OK)st=io->read(io->context,&old);
    if(st==LT48_OK&&(target&~old&p->configuration_mask))st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK&&((old^target)&p->configuration_mask)) {
        /* Program only newly cleared cells. Source Protect drives the
         * oscillator/WDTE pins high and clears unused upper payload bits.
         * Retain that payload even for old logical one-bit CP definitions:
         * later dies may implement more CP cells than the source UI exposes. */
        uint16_t payload=(uint16_t)((target|~old)&4095);
        if(p->configuration_smart)payload=target&p->configuration_mask;
        else if((target&p->protection_mask)!=p->protection_mask)payload&=p->configuration_mask;
        st=p->configuration_smart?b12otp_program_word(p,io,payload,4095):io->pulses(io->context,payload,p->pulse_us,p->configuration_us/p->pulse_us);
    }
    if(st==LT48_OK)st=io->read(io->context,&got);
    if(st==LT48_OK&&((got^target)&p->configuration_mask))st=LT48_DEVICE_MISMATCH;
    return st;
}
static inline lt48_status b12otp_check_calibration(const b12otp_policy *p,const b12otp_io *io,uint16_t expected) {
    if(!p->calibration||!p->calibration_visible_protected)return LT48_OK;
    uint16_t got=0;
    lt48_status st=io->enter(io->context,p->code_words-1,0);
    if(st==LT48_OK)st=io->read(io->context,&got);
    if(st==LT48_OK&&got!=expected)st=LT48_DEVICE_MISMATCH;
    return st;
}
static inline lt48_status b12otp_execute(const b12otp_policy *p,const b12otp_io *io,
    lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,b12otp_result *r) {
    if(!b12otp_valid(p)||!io||!io->enter||!io->read||!io->increment||!io->pulses||
       !io->cleanup.source||!io->cleanup.command||!r||!scratch)return LT48_INVALID_ARGUMENT;
    if(op==LT48_OPERATION_ERASE||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    int programming=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
    int needs_image=programming||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE;
    if((needs_image&&!image)||(op==LT48_OPERATION_READ&&!output)||
       b12otp_overlap(image,scratch)||b12otp_overlap(output,scratch))return LT48_INVALID_ARGUMENT;
    if(needs_image) {
        for(unsigned a=0;a<B12OTP_WORDS;a++)if(image[a]>4095||(!b12otp_mask(p,a)&&image[a]!=4095))return LT48_SAFETY_REJECTED;
        unsigned cp=image[B12OTP_CONFIG]&p->protection_mask;
        if(cp&&cp!=p->protection_mask)return LT48_SAFETY_REJECTED;
    }
    memset(r,0,sizeof *r);r->first_mismatch=UINT32_MAX;
    lt48_status st=LT48_OK;
    if(op==LT48_OPERATION_SECURE) {
        uint16_t cfg=0;
        st=io->enter(io->context,B12OTP_CONFIG,0);
        if(st==LT48_OK)st=io->read(io->context,&cfg);
        int was_unprotected=(cfg&p->protection_mask)==p->protection_mask;
        uint16_t cal=4095;
        if(st==LT48_OK&&was_unprotected&&p->calibration) {
            st=io->enter(io->context,p->code_words-1,0);
            if(st==LT48_OK)st=io->read(io->context,&cal);
            if(st==LT48_OK&&!p->calibration_untyped&&(cal&0xf00)!=0xc00)st=LT48_SAFETY_REJECTED;
            r->calibration=cal;
        }
        if(st==LT48_OK)st=b12otp_configuration(p,io,cfg&~p->protection_mask);
        if(st==LT48_OK&&was_unprotected)st=b12otp_check_calibration(p,io,cal);
    } else if(programming) {
        st=b12otp_scan(p,io,5,scratch);
        uint16_t cfg=image[B12OTP_CONFIG]|p->protection_mask;
        uint16_t cal=p->calibration?scratch[p->code_words-1]:4095;
        int restore=0;
        if(st==LT48_OK&&(scratch[B12OTP_CONFIG]&p->protection_mask)!=p->protection_mask)st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK&&p->calibration) {
            uint16_t supplied=image[p->code_words-1];
            if(cal==4095) {
                if(!p->calibration_untyped&&(supplied&0xf00)!=0xc00)st=LT48_SAFETY_REJECTED;
                else {cal=supplied;restore=supplied!=4095;}
            } else if((!p->calibration_untyped&&(cal&0xf00)!=0xc00)||(supplied!=4095&&supplied!=cal))st=LT48_SAFETY_REJECTED;
            r->calibration=cal;
        }
        if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM)st=b12otp_compare(p,NULL,scratch,1,0,0,0,r);
        /* Check the complete image before the first irreversible pulse. */
        for(unsigned a=0;st==LT48_OK&&a<B12OTP_WORDS;a++) {
            unsigned v=a==B12OTP_CONFIG?cfg:image[a];
            if(p->calibration&&a==p->code_words-1)v=cal;
            if(v&~scratch[a]&b12otp_mask(p,a)) {st=LT48_SAFETY_REJECTED;r->first_mismatch=2*a;}
        }
        if(st==LT48_OK)st=io->enter(io->context,0,0);
        for(unsigned a=0;st==LT48_OK&&a<p->code_words+4;a++) {
            unsigned mask=b12otp_mask(p,a);uint16_t target=image[a];
            int skip=p->calibration&&a==p->code_words-1&&!restore;
            if(p->calibration&&a==p->code_words-1)target=cal;
            if(!skip&&(target&mask)!=mask) {
                st=b12otp_program_word(p,io,target,mask);
                if(st==LT48_OK)r->programmed_words++;
            }
            if(st==LT48_OK&&a+1<p->code_words+4)st=io->increment(io->context);
        }
        if(st==LT48_OK)st=b12otp_configuration(p,io,cfg);
        const unsigned levels[]={5,6};
        for(unsigned i=0;st==LT48_OK&&i<2;i++) {
            st=b12otp_scan(p,io,levels[i],scratch);
            if(st==LT48_OK)st=b12otp_compare(p,image,scratch,0,1,cal,cfg,r);
        }
        if(st==LT48_OK&&(image[B12OTP_CONFIG]&p->protection_mask)!=p->protection_mask)
            st=b12otp_configuration(p,io,image[B12OTP_CONFIG]);
        if(st==LT48_OK)st=b12otp_check_calibration(p,io,cal);
    } else {
        const unsigned levels[]={0,5,6};
        unsigned count=op==LT48_OPERATION_VERIFY?3:1;
        for(unsigned i=0;st==LT48_OK&&i<count;i++) {
            st=b12otp_scan(p,io,levels[i],scratch);
            if(st==LT48_OK&&op!=LT48_OPERATION_READ)
                st=b12otp_compare(p,image,scratch,op==LT48_OPERATION_BLANK_CHECK,0,0,0,r);
        }
    }
    r->operation_status=st;
    st=lt48_pic_cleanup(&io->cleanup,st,&r->cleanup);
    if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,scratch,B12OTP_WORDS*sizeof(*output));
    return st;
}
#endif
