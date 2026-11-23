#ifndef LT48_OTP14_OPERATOR_H
#define LT48_OTP14_OPERATOR_H
#include "otp14_configuration_transaction.h"
#include "cleanup.h"
#include <string.h>

enum { OTP14_IMAGE_WORDS=0x2008 };
typedef struct {
    unsigned code_words;
    uint16_t id_mask;
    otp14_configuration_policy configuration;
} otp14_operator_policy;
typedef struct {
    void *context;
    /* scan must physically read code, IDs and configuration at the requested
     * voltage. program writes only code and IDs, never protection/config. */
    lt48_status (*scan)(void *,unsigned,uint16_t *);
    lt48_status (*program)(void *,const uint16_t *);
    otp14_configuration_io configuration;
    lt48_pic_cleanup_io cleanup;
} otp14_operator_io;
typedef struct {
    lt48_status operation_status;
    lt48_pic_cleanup_result cleanup;
    unsigned mismatches,first_mismatch;
} otp14_operator_result;
static inline int otp14_operator_overlap(const uint16_t *a,const uint16_t *b) {
    if(!a||!b)return 0;
    uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
    return (x<y?y-x:x-y)<OTP14_IMAGE_WORDS*sizeof(*a);
}
static inline unsigned otp14_operator_mask(const otp14_operator_policy *p,unsigned a) {
    return a<p->code_words?0x3fff:a>=0x2000&&a<=0x2003?p->id_mask:
        a==0x2007?p->configuration.mask:0;
}
static inline lt48_status otp14_operator_compare(const otp14_operator_policy *p,
    const uint16_t *want,const uint16_t *got,int blank,otp14_operator_result *r) {
    r->mismatches=0;r->first_mismatch=UINT32_MAX;
    for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)if(((blank?0x3fff:want[a])^got[a])&otp14_operator_mask(p,a)) {
        if(!r->mismatches)r->first_mismatch=2*a;
        r->mismatches++;
    }
    return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
/* Whole OTP/UV operation owner, shared by policies rather than part names.
 * Staged public reads commit only after successful global cleanup. Program
 * never erases: it checks blank, or forbids 0->1 changes for Over-program.
 * Code/IDs and unprotected configuration verify before any protection action.
 * Adapters still need complete source-wire/entry and profile qualifications. */
static inline lt48_status otp14_operator_execute_levels(const otp14_operator_policy *p,
    const otp14_operator_io *io,lt48_operation_kind op,const uint16_t *image,
    uint16_t *output,uint16_t *scratch,otp14_operator_result *result,const unsigned *verify_levels,unsigned verify_count) {
    if(!p||!io||!io->scan||!io->program||!scratch||!result||!p->code_words||p->code_words>0x2000||
       !p->id_mask||p->id_mask>0x3fff||!otp14_configuration_target_valid(&p->configuration,0x3fff)||
       !io->configuration.action||!io->configuration.read||!io->cleanup.source||!io->cleanup.command)
        return LT48_INVALID_ARGUMENT;
    if(op==LT48_OPERATION_ERASE||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_READ&&!output)||
       ((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image))
        return LT48_INVALID_ARGUMENT;
    if(otp14_operator_overlap(image,scratch)||
       (op==LT48_OPERATION_READ&&otp14_operator_overlap(output,scratch)))return LT48_INVALID_ARGUMENT;
    /* Invalid image/policy rejects before any access or cleanup side effects. */
    if(image)for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++) {
        if(image[a]>0x3fff||(!otp14_operator_mask(p,a)&&image[a]!=0x3fff))return LT48_SAFETY_REJECTED;
    }
    if(image&&!otp14_configuration_target_valid(&p->configuration,image[0x2007]))return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_VERIFY&&verify_levels) {
        if(!verify_count||verify_count>3)return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<verify_count;i++)if(verify_levels[i]<p->configuration.minimum||verify_levels[i]>p->configuration.maximum)return LT48_INVALID_ARGUMENT;
    }
    result->mismatches=0;result->first_mismatch=UINT32_MAX;
    lt48_status st=LT48_OK;
    if(op==LT48_OPERATION_SECURE)st=otp14_configuration_secure(&p->configuration,&io->configuration);
    else if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM) {
        st=io->scan(io->context,p->configuration.nominal,scratch);
        if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM)st=otp14_operator_compare(p,NULL,scratch,1,result);
        if(st==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM) {
            for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)if(image[a]&~scratch[a]&otp14_operator_mask(p,a)) {
                st=LT48_SAFETY_REJECTED;result->first_mismatch=2*a;break;
            }
        }
        /* Whole-image programming cannot temporarily remove OTP protection.
         * Reject that state before any write; Secure and physical Read remain
         * available without claiming an undocumented protection bypass. */
        if(st==LT48_OK&&(scratch[0x2007]&p->configuration.protection)!=p->configuration.protection)
            st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK)st=io->program(io->context,image);
        uint16_t unlocked=image[0x2007]|p->configuration.protection;
        if(st==LT48_OK)st=otp14_configuration_program(&p->configuration,&io->configuration,unlocked);
        const unsigned levels[]={p->configuration.nominal,p->configuration.minimum,p->configuration.maximum};
        for(unsigned i=0;st==LT48_OK&&i<3;i++) {
            st=io->scan(io->context,levels[i],scratch);
            if(st==LT48_OK) {
                /* Verify expected configuration as unprotected until all code
                 * and ID comparisons finish. Never modify the caller image. */
                for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)if(((a==0x2007?unlocked:image[a])^scratch[a])&otp14_operator_mask(p,a)) {
                    if(!result->mismatches)result->first_mismatch=2*a;
                    result->mismatches++;
                }
                if(result->mismatches)st=LT48_DEVICE_MISMATCH;
            }
        }
        if(st==LT48_OK&&(image[0x2007]&p->configuration.protection)!=p->configuration.protection) {
            st=io->configuration.action(io->configuration.context,10,image[0x2007],0);
            if(st==LT48_OK)st=otp14_configuration_check(&p->configuration,&io->configuration,image[0x2007]);
        }
    } else {
        const unsigned levels[]={p->configuration.nominal,p->configuration.minimum,p->configuration.maximum};
        unsigned passes=op==LT48_OPERATION_VERIFY?(verify_levels?verify_count:3):1;
        for(unsigned i=0;st==LT48_OK&&i<passes;i++) {
            st=io->scan(io->context,verify_levels&&op==LT48_OPERATION_VERIFY?verify_levels[i]:levels[i],scratch);
            if(st==LT48_OK&&op!=LT48_OPERATION_READ)st=otp14_operator_compare(p,image,scratch,op==LT48_OPERATION_BLANK_CHECK,result);
        }
    }
    result->operation_status=st;
    st=lt48_pic_cleanup(&io->cleanup,st,&result->cleanup);
    if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,scratch,OTP14_IMAGE_WORDS*sizeof(*output));
    return st;
}
static inline lt48_status otp14_operator_execute(const otp14_operator_policy *p,const otp14_operator_io *io,
    lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result) {
    return otp14_operator_execute_levels(p,io,op,image,output,scratch,result,NULL,0);
}
#endif
