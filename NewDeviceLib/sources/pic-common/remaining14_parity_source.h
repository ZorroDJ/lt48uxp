#ifndef LT48_PIC_REMAINING14_PARITY_SOURCE_H
#define LT48_PIC_REMAINING14_PARITY_SOURCE_H
#include "remaining14_parity_operator.h"
#include "remaining14_parity_wire.h"
#include "../../../c/src/internal.h"
#include "otp14_source_adapter.h"

typedef struct {
    rm14_resources *binding;
    /* Borrowed exact resource/session; close through binding only. */
    otp14_source_adapter source;unsigned verify_selection;
    lt48_pic_classic_wire wire;
    lt48_status (*word_timeout)(void *,unsigned);rm14_parity_pulse pulse;lt48_status (*flush)(void *);
} rm14_parity_source;

static inline lt48_status rm14_parity_source_timeout(rm14_parity_source *a,unsigned milliseconds) {
    return a->word_timeout?a->word_timeout(a->wire.context,milliseconds):LT48_OK;
}
static inline lt48_status rm14_parity_source_read_word(rm14_parity_source *a,uint16_t *raw) {
    lt48_status st=rm14_parity_source_timeout(a,10000);
    return st==LT48_OK?rm14_parity_read_raw(&a->wire,raw):st;
}

static inline void rm14_parity_source_levels(rm14_parity_source *a) {
    /* Exact W502 DEVINIT tail: HS(FOSC=2) selects450cV; otherwise400cV.
     * Both paths select550cV. Resolve this before the enclosing owner takes
     * its immutable per-operation list of verify levels. A fresh adapter
     * cannot inherit the previous operation's DEVINIT variable updates. */
    a->source.policy.configuration.minimum=(a->source.image[0x400e]&3)==2?450:400;
    a->source.policy.configuration.maximum=550;
}

static inline void rm14_parity_source_close(rm14_parity_source *a) {
    if(!a)return;
    rm14_close(a->binding);free(a);
}
static inline lt48_status rm14_parity_source_begin(rm14_parity_source *a,unsigned mode,unsigned level) {
    lt48_status st=otp14_source_begin(&a->source,mode,level);
    /* DEVINIT derives source margin voltages from oscillator configuration.
     * The source-session result, not a name-based default, owns those values. */
    if(st==LT48_OK)for(size_t i=0;i<a->source.variable_count;i++) {
        const lt48_compile_variable *v=a->source.variables+i;
        if(v->variable_id==5)a->source.policy.configuration.minimum=v->value;
        if(v->variable_id==6)a->source.policy.configuration.maximum=v->value;
    }
    return st;
}
static inline lt48_status rm14_parity_source_end(rm14_parity_source *a,lt48_status st,unsigned mode) {
    if(a->flush){lt48_status flushed=a->flush(a->wire.context);if(st==LT48_OK)st=flushed;}
    return otp14_source_end(&a->source,st,mode);
}
static inline lt48_status rm14_parity_source_configuration_bank(rm14_parity_source *a) {
    return rm14_parity_load(&a->wire,0,0xffff);
}
static inline lt48_status rm14_parity_source_increment(rm14_parity_source *a) {
    return lt48_pic_classic_command(&a->wire,6);
}
static inline lt48_status rm14_parity_source_scan(void *v,unsigned level,uint16_t *out) {
    rm14_parity_source *a=v;
    for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++)out[i]=0x3fff;
    lt48_status st=rm14_parity_source_begin(a,1,level);
    for(unsigned i=0;st==LT48_OK&&i<a->source.policy.code_words;i++) {
        uint16_t raw=0;st=rm14_parity_source_read_word(a,&raw);
        /* Preserve the source's parity-error detection for programmed code.
         * The physically erasedFFFF special case remains a readable blank. */
        if(st==LT48_OK&&raw!=0xffff&&!rm14_parity_valid(raw))st=LT48_DEVICE_MISMATCH;
        if(st==LT48_OK)out[i]=rm14_parity_decode(raw);
        if(st==LT48_OK&&i+1<a->source.policy.code_words)st=rm14_parity_source_increment(a);
    }
    if(st==LT48_OK)st=rm14_parity_source_configuration_bank(a);
    for(unsigned i=0;st==LT48_OK&&i<8;i++) {
        if(i<4||i==7) {
            uint16_t raw=0;st=rm14_parity_source_read_word(a,&raw);
            if(st==LT48_OK)out[0x2000+i]=(uint16_t)(rm14_parity_decode(raw));
        }
        if(st==LT48_OK&&i<7)st=rm14_parity_source_increment(a);
    }
    return rm14_parity_source_end(a,st,1);
}
static inline lt48_status rm14_parity_source_raw_scan(void *v,unsigned level,uint16_t *out,unsigned count) {
    rm14_parity_source *a=v;
    if(count!=a->source.policy.code_words)return LT48_INVALID_ARGUMENT;
    lt48_status st=rm14_parity_source_begin(a,1,level);
    for(unsigned i=0;st==LT48_OK&&i<count;i++) {
        st=rm14_parity_source_read_word(a,out+i);
        if(st==LT48_OK&&i+1<count)st=rm14_parity_source_increment(a);
    }
    return rm14_parity_source_end(a,st,1);
}
static inline lt48_status rm14_parity_source_program(void *v,const uint16_t *image) {
    rm14_parity_source *a=v;
    lt48_status st=rm14_parity_source_begin(a,2,a->source.policy.configuration.nominal);
    for(unsigned i=0;st==LT48_OK&&i<a->source.policy.code_words;i++) {
        st=rm14_parity_source_timeout(a,5000);
        if(st==LT48_OK)st=rm14_parity_program_masked_with_pulse(&a->wire,image[i],0xffff,a->pulse,a->wire.context);
        if(st==LT48_OK&&i+1<a->source.policy.code_words)st=rm14_parity_source_increment(a);
    }
    if(st==LT48_OK)st=rm14_parity_source_configuration_bank(a);
    for(unsigned i=0;st==LT48_OK&&i<4;i++) {
        /* Original LOAD preserves all14 ID bits for Read/Verify. Only
         * Blank Check ignores upper ID bits; parity is ignored in this bank. */
        st=rm14_parity_source_timeout(a,5000);
        if(st==LT48_OK)st=rm14_parity_program_masked_with_pulse(&a->wire,image[0x2000+i],0xfffc,a->pulse,a->wire.context);
        if(st==LT48_OK&&i<3)st=rm14_parity_source_increment(a);
    }
    return rm14_parity_source_end(a,st,2);
}
static inline lt48_status rm14_parity_source_config_read(void *v,unsigned level,uint16_t *out) {
    rm14_parity_source *a=v;lt48_status st=rm14_parity_source_begin(a,1,level);
    if(st==LT48_OK)st=rm14_parity_source_configuration_bank(a);
    for(unsigned i=0;st==LT48_OK&&i<7;i++)st=rm14_parity_source_increment(a);
    uint16_t raw=0;if(st==LT48_OK)st=rm14_parity_source_read_word(a,&raw);
    if(st==LT48_OK)*out=rm14_parity_decode(raw);
    return rm14_parity_source_end(a,st,1);
}
static inline lt48_status rm14_parity_source_config_action(void *v,unsigned role,uint16_t target,int standalone) {
    rm14_parity_source *a=v;(void)standalone;
    if((role!=14&&role!=10)||!rm14_parity_configuration_valid(target))return LT48_SAFETY_REJECTED;
    a->source.image[0x400e]=target;a->source.image[0x400f]=target>>8;
    lt48_status st=rm14_parity_source_begin(a,2,a->source.policy.configuration.nominal);
    if(st==LT48_OK)st=rm14_parity_source_configuration_bank(a);
    for(unsigned i=0;st==LT48_OK&&i<7;i++)st=rm14_parity_source_increment(a);
    if(st==LT48_OK)st=rm14_parity_source_timeout(a,10000);
    if(st==LT48_OK)st=rm14_parity_program_configuration_with_pulse(&a->wire,target,a->pulse,a->wire.context);
    return rm14_parity_source_end(a,st,2);
}
static inline lt48_status rm14_parity_source_execute(rm14_parity_source *a,lt48_operation_kind operation,
    const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result) {
    if(!a)return LT48_INVALID_ARGUMENT;
    if(image)for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++){a->source.image[2*i]=image[i];a->source.image[2*i+1]=image[i]>>8;}
    rm14_parity_source_levels(a);
    const rm14_parity_operator_io io={{a,rm14_parity_source_scan,rm14_parity_source_program,
        {a,rm14_parity_source_config_action,rm14_parity_source_config_read},
        {&a->source,otp14_source_cleanup_callback,otp14_source_cleanup_command}},rm14_parity_source_raw_scan};
    return rm14_parity_operator_execute_options(a->binding,&a->source.policy,&io,operation,image,output,scratch,result,a->verify_selection);
}
static inline lt48_status rm14_parity_source_open(const char *path,const lt48_device_spec *spec,
    const rm14_profile *profile,const unsigned char *image,size_t span,
    lt48_pic_source_runner runner,lt48_pic_source_runner cleanup_runner,
    lt48_status (*command)(void *,const unsigned char *,size_t),lt48_status (*ready)(void *),void *context,
    const lt48_pic_classic_wire *wire,rm14_parity_source **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!profile||profile->family<342||profile->family>346||!runner||!cleanup_runner||!command||!ready||
       !wire||!wire->transfer||!wire->delay)return LT48_INVALID_ARGUMENT;
    rm14_parity_source *a=calloc(1,sizeof *a);if(!a)return LT48_NO_MEMORY;
    lt48_status st=rm14_open(path,spec,profile,image,span,&a->binding);
    if(st!=LT48_OK){free(a);return st;}
    a->wire=*wire;a->source.resources=a->binding->resources;a->source.session=a->binding->session;
    a->source.policy=(otp14_operator_policy){profile->code_words,0x3fff,{0x3fff,0x3f30,500,400,600}};
    a->source.runner=runner;a->source.cleanup_runner=cleanup_runner;
    a->source.command=command;a->source.ready=ready;a->source.context=context;
    lt48_pic_source_state state;st=lt48_pic_source_session_state_internal(a->source.session,&state);
    if(st==LT48_OK&&(state.variable_count>256||state.configuration_count>1024||state.image_size!=sizeof a->source.image))st=LT48_SAFETY_REJECTED;
    if(st!=LT48_OK){rm14_parity_source_close(a);return st;}
    a->source.variable_count=state.variable_count;a->source.field_count=state.configuration_count;
    memcpy(a->source.variables,state.variables,state.variable_count*sizeof *state.variables);
    if(state.configuration_count)memcpy(a->source.fields,state.configuration,state.configuration_count*sizeof *state.configuration);
    memcpy(a->source.image,state.image,sizeof a->source.image);memcpy(a->source.buffers,state.buffers,sizeof a->source.buffers);
    memcpy(a->source.rails,state.voltages,sizeof a->source.rails);
    rm14_parity_source_levels(a);
    *out=a;return LT48_OK;
}
#endif
