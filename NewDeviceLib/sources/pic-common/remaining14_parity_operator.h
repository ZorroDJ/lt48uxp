#ifndef LT48_PIC_REMAINING14_PARITY_OPERATOR_H
#define LT48_PIC_REMAINING14_PARITY_OPERATOR_H
#include "remaining14_resources.h"
#include "remaining14_parity.h"
#include "remaining14_otp_operator.h"

/* The existing whole OTP owner supplies staged reads, seven operations,
 * configuration-last ordering, margin verification and mandatory cleanup.
 * For the five parity OTPs a decoded logical preflight is insufficient.
 * The extra raw scan must start its own source-bound read entry and finish
 * reset, just like io.scan; global cleanup remains unconditional in the
 * enclosing owner. The adapter must not discard parity cells. */
typedef struct {
    otp14_operator_io logical;
    lt48_status (*raw_scan)(void *,unsigned,uint16_t *,unsigned);
} rm14_parity_operator_io;
typedef struct {
    const otp14_operator_policy *policy;
    const rm14_parity_operator_io *io;
    const uint16_t *previous;
    uint16_t *raw;
    lt48_operation_kind operation;
    unsigned scans;uint16_t ids[4];
} rm14_parity_operation;

static inline lt48_status rm14_parity_operation_scan(void *v,unsigned level,uint16_t *out) {
    rm14_parity_operation *o=v;
    lt48_status st=o->io->logical.scan(o->io->logical.context,level,out);
    if(st==LT48_OK&&!o->scans)for(unsigned i=0;i<4;i++)o->ids[i]=out[0x2000+i];
    /* The common policy owns the blank-only ID mask; retain full reads. */
    o->scans++;return st;
}
static inline lt48_status rm14_parity_operation_program(void *v,const uint16_t *image) {
    rm14_parity_operation *o=v;
    for(unsigned i=0;i<4;i++)if(image[0x2000+i]&~o->ids[i])return LT48_SAFETY_REJECTED;
    unsigned count=o->policy->code_words;
    lt48_status st=o->io->raw_scan(o->io->logical.context,o->policy->configuration.nominal,o->raw,count);
    if(st!=LT48_OK)return st;
    /* Two scans must describe the same cells. A fresh raw read cannot silently
     * substitute a different chip/state for the logical blank/OTP preflight. */
    for(unsigned a=0;a<count;a++) {
        if(rm14_parity_decode(o->raw[a])!=o->previous[a])return LT48_DEVICE_MISMATCH;
        if(o->operation==LT48_OPERATION_PROGRAM&&o->raw[a]!=0xffff)return LT48_SAFETY_REJECTED;
        if(!rm14_parity_can_program(o->raw[a],image[a]))return LT48_SAFETY_REJECTED;
    }
    return o->io->logical.program(o->io->logical.context,image);
}

static inline int rm14_parity_configuration_valid(unsigned value){
 if(value>0x3fff)return 0;
 unsigned cp=value&0x3f30;
 return cp==0||cp==0x1510||cp==0x2a20||cp==0x3f30;
}
static inline lt48_status rm14_parity_operation_program_adapter(void *v,const uint16_t *image,unsigned skip){
 (void)skip;return rm14_parity_operation_program(v,image);
}
/* Private whole-operation entry. Exact source constructor and explicit
 * primary/audited policy are required. No catalogue admission is granted by
 * this wrapper; source entry/configuration/preservation joins remain gates. */
static inline lt48_status rm14_parity_operator_execute_options(const rm14_resources *r,
    const otp14_operator_policy *policy,const rm14_parity_operator_io *io,
    lt48_operation_kind operation,const uint16_t *image,uint16_t *output,
    uint16_t *scratch,otp14_operator_result *result,unsigned verify_selection) {
    if(!r||!r->profile||!r->resources||!r->session||!policy||!io||!io->raw_scan||
       !io->logical.scan||!io->logical.program)return LT48_INVALID_ARGUMENT;
    const rm14_profile *p=r->profile;
    if(p->family<342||p->family>346||p->has_erase||p->source_bytes!=16400||p->backing_bytes!=16400||
       policy->code_words!=p->code_words||policy->id_mask!=0x3fff||policy->configuration.mask!=0x3fff||
       policy->configuration.protection!=0x3f30)return LT48_SAFETY_REJECTED;
    uint16_t *raw=malloc(p->code_words*sizeof *raw);if(!raw)return LT48_NO_MEMORY;
    rm14_parity_operation context={.policy=policy,.io=io,.previous=scratch,.raw=raw,.operation=operation};
    const rm14_otp_policy expanded={.common=*policy,.oscillator_margins=1,.verify_selection=verify_selection,.blank_id_mask=15,
        .protection_values={0,0x1510,0x2a20,0x3f30},.protection_count=4,.factory_word=UINT32_MAX};
    const rm14_otp_io guarded={&context,rm14_parity_operation_scan,rm14_parity_operation_program_adapter,
        io->logical.configuration,io->logical.cleanup};
    lt48_status st=rm14_otp_execute(&expanded,&guarded,operation,image,output,scratch,result);
    free(raw);return st;
}
static inline lt48_status rm14_parity_operator_execute(const rm14_resources *r,const otp14_operator_policy *policy,
 const rm14_parity_operator_io *io,lt48_operation_kind operation,const uint16_t *image,uint16_t *output,
 uint16_t *scratch,otp14_operator_result *result){
 return rm14_parity_operator_execute_options(r,policy,io,operation,image,output,scratch,result,0);
}
#endif
