#ifndef LT48_OTP14_SOURCE_ADAPTER_H
#define LT48_OTP14_SOURCE_ADAPTER_H
#include "otp14_operator.h"
#include "otp14_configuration.h"
#include "source_resources.h"
#include <stdlib.h>

typedef struct {
    lt48_pic_source_resources *resources;
    lt48_pic_source_session *session;
    otp14_operator_policy policy;
    lt48_compiled_wave *configuration[2];
    lt48_compile_variable variables[256];size_t variable_count;
    unsigned char image[OTP14_IMAGE_WORDS*2],buffers[LT48_RUNTIME_BUFFER_COUNT][32];
    lt48_runtime_configuration_value fields[1024];size_t field_count;
    uint32_t rails[4];
    lt48_pic_source_runner runner,cleanup_runner;void *context;
    lt48_status (*command)(void *,const unsigned char *,size_t);
    lt48_status (*ready)(void *);
    void (*progress)(void *,unsigned,size_t,size_t);
} otp14_source_adapter;

static inline void otp14_source_number(unsigned char *p,unsigned value) {
    for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(value>>(8*i));
}
static inline void otp14_source_close(otp14_source_adapter *a) {
    if(!a)return;
    lt48_pic_source_session_close_internal(a->session);
    lt48_compiled_wave_close(a->configuration[0]);lt48_compiled_wave_close(a->configuration[1]);
    lt48_pic_source_resources_close_internal(a->resources);free(a);
}
static inline lt48_status otp14_source_invoke(otp14_source_adapter *a,unsigned role,
    unsigned mode,unsigned address,unsigned expected,int cleanup,unsigned *observed) {
    otp14_source_number(a->buffers[2],address);otp14_source_number(a->buffers[9],mode);
    otp14_source_number(a->buffers[6],expected);
    lt48_pic_source_state state={a->variables,a->variable_count,a->fields,a->field_count,
        a->image,sizeof(a->image),(const unsigned char (*)[32])a->buffers,{0},0,0};
    memcpy(state.voltages,a->rails,sizeof a->rails);
    lt48_status st=lt48_pic_source_session_restore_internal(a->session,&state);
    const lt48_compiled_wave *root=role==14?a->configuration[0]:role==10?a->configuration[1]:NULL;
    if(st==LT48_OK)st=lt48_pic_source_session_invoke_wave_internal(a->session,(uint16_t)role,root,
        mode,address,10,cleanup?a->cleanup_runner:a->runner,a->context);
    if(st==LT48_OK||st==LT48_DEVICE_MISMATCH) {
        lt48_status exported=lt48_pic_source_session_state_internal(a->session,&state);
        if(exported!=LT48_OK)return exported;
        if(state.variable_count!=a->variable_count||state.configuration_count>1024||state.image_size!=sizeof a->image)return LT48_RUNTIME_ERROR;
        memcpy(a->variables,state.variables,a->variable_count*sizeof(*a->variables));
        memcpy(a->image,state.image,sizeof a->image);memcpy(a->buffers,state.buffers,sizeof a->buffers);
        memcpy(a->fields,state.configuration,state.configuration_count*sizeof(*a->fields));a->field_count=state.configuration_count;
        memcpy(a->rails,state.voltages,sizeof a->rails);
        if(observed)*observed=a->buffers[5][0]|(unsigned)a->buffers[5][1]<<8;
    }
    return st;
}
static inline lt48_status otp14_source_prepare(otp14_source_adapter *a,unsigned mode) {
    lt48_pic_source_state state={a->variables,a->variable_count,a->fields,a->field_count,
        a->image,sizeof(a->image),(const unsigned char (*)[32])a->buffers,{0},0,0};
    memcpy(state.voltages,a->rails,sizeof a->rails);
    lt48_status st=lt48_pic_source_session_restore_internal(a->session,&state);
    if(st==LT48_OK)st=lt48_pic_source_session_configuration_internal(a->session,11,12,(uint16_t)mode,0,NULL,0,NULL,0);
    if(st==LT48_OK)st=lt48_pic_source_session_state_internal(a->session,&state);
    if(st==LT48_OK&&state.configuration_count>1024)st=LT48_RUNTIME_ERROR;
    if(st==LT48_OK) {
        memcpy(a->variables,state.variables,a->variable_count*sizeof(*a->variables));
        memcpy(a->image,state.image,sizeof a->image);memcpy(a->buffers,state.buffers,sizeof a->buffers);
        memcpy(a->fields,state.configuration,state.configuration_count*sizeof(*a->fields));a->field_count=state.configuration_count;
        memcpy(a->rails,state.voltages,sizeof a->rails);
    }
    return st;
}
static inline lt48_status otp14_source_startup_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **out) {
    otp14_source_adapter *a=context;return a->runner(a->context,machine,out);
}
static inline lt48_status otp14_source_selector(void *context,uint8_t selector) {
    otp14_source_adapter *a=context;const unsigned char bytes[]={0x0f,selector};return a->command(a->context,bytes,2);
}
static inline lt48_status otp14_source_ready(void *context) {
    otp14_source_adapter *a=context;return a->ready(a->context);
}
static inline lt48_status otp14_source_begin(otp14_source_adapter *a,unsigned mode,unsigned voltage) {
    memset(a->buffers,0,sizeof a->buffers);
    lt48_status st=otp14_source_prepare(a,mode);
    for(size_t i=0;i<a->variable_count;i++)if(a->variables[i].variable_id==0)a->variables[i].value=voltage;
    if(st==LT48_OK&&lt48_pic_source_session_has_callback_internal(a->session,17))st=otp14_source_invoke(a,17,mode,0,0,0,NULL);
    const lt48_pic_source_lifecycle lifecycle={.voltage_variable=0,.poweron_role=0,.init_role=3,
        .reset_role=4,.poweroff_role=1,.timeout_seconds=10,.voltage_centivolts=voltage,
        .minimum_centivolts=a->policy.configuration.minimum,.maximum_centivolts=a->policy.configuration.maximum,
        .selector_write=otp14_source_selector,.ready=otp14_source_ready,.runner=otp14_source_startup_runner,.context=a};
    if(st==LT48_OK)st=lt48_pic_source_session_begin_internal(a->session,&lifecycle,(uint16_t)mode,0);
    if(st==LT48_OK) {
        lt48_pic_source_state state;st=lt48_pic_source_session_state_internal(a->session,&state);
        if(st==LT48_OK&&state.configuration_count>1024)st=LT48_RUNTIME_ERROR;
        if(st==LT48_OK){
            memcpy(a->variables,state.variables,a->variable_count*sizeof(*a->variables));
            memcpy(a->image,state.image,sizeof a->image);memcpy(a->buffers,state.buffers,sizeof a->buffers);
            memcpy(a->fields,state.configuration,state.configuration_count*sizeof(*a->fields));a->field_count=state.configuration_count;
            memcpy(a->rails,state.voltages,sizeof a->rails);
        }
    }
    return st;
}
static inline lt48_status otp14_source_end(otp14_source_adapter *a,lt48_status st,unsigned mode) {
    lt48_status end=otp14_source_invoke(a,4,mode,0,0,1,NULL);return st==LT48_OK?end:st;
}
static inline lt48_status otp14_source_scan(void *context,unsigned voltage,uint16_t *output) {
    otp14_source_adapter *a=context;lt48_status st=otp14_source_begin(a,1,voltage);
    for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++)output[i]=0x3fff;
    for(unsigned region=0;st==LT48_OK&&region<3;region++) {
        unsigned first=region==0?0:region==1?0x2000:0x2007;
        unsigned count=region==0?a->policy.code_words:region==1?4:1;
        for(unsigned i=0;st==LT48_OK&&i<count;i++) {
            unsigned value=0;st=otp14_source_invoke(a,6,1,first+i,0x3fff,0,&value);
            if(st==LT48_OK)output[first+i]=value&0x3fff;
            if(st==LT48_OK&&a->progress&&(!(i&127)||i+1==count))
                a->progress(a->context,1,2*(region==0?i+1:region==1?a->policy.code_words+i+1:a->policy.code_words+5),2*(a->policy.code_words+5));
        }
    }
    return otp14_source_end(a,st,1);
}
static inline lt48_status otp14_source_program(void *context,const uint16_t *image) {
    otp14_source_adapter *a=context;
    for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++){a->image[2*i]=image[i];a->image[2*i+1]=image[i]>>8;}
    lt48_status st=otp14_source_begin(a,2,a->policy.configuration.nominal);
    for(unsigned region=0;st==LT48_OK&&region<2;region++) {
        unsigned first=region?0x2000:0,count=region?4:a->policy.code_words;
        for(unsigned i=0;st==LT48_OK&&i<count;i++) {
            otp14_source_number(a->buffers[5],image[first+i]);
            st=otp14_source_invoke(a,8,2,first+i,image[first+i],0,NULL);
            if(st==LT48_OK&&a->progress&&(!(i&127)||i+1==count))
                a->progress(a->context,2,2*(region?a->policy.code_words+i+1:i+1),2*(a->policy.code_words+4));
        }
    }
    return otp14_source_end(a,st,2);
}
static inline lt48_status otp14_source_config_action(void *context,unsigned role,uint16_t target,int standalone) {
    otp14_source_adapter *a=context;
    if((role!=14&&role!=10)||!otp14_configuration_target_valid(&a->policy.configuration,target))return LT48_SAFETY_REJECTED;
    a->image[0x400e]=target;a->image[0x400f]=target>>8;
    lt48_status st=otp14_source_begin(a,2,a->policy.configuration.nominal);
    if(standalone)otp14_source_number(a->buffers[21],1);
    if(st==LT48_OK)st=otp14_source_invoke(a,role,2,0x2007,target,0,NULL);
    return otp14_source_end(a,st,2);
}
static inline lt48_status otp14_source_config_read(void *context,unsigned voltage,uint16_t *out) {
    otp14_source_adapter *a=context;lt48_status st=otp14_source_begin(a,1,voltage);unsigned value=0;
    if(st==LT48_OK)st=otp14_source_invoke(a,6,1,0x2007,0x3fff,0,&value);
    if(st==LT48_OK)*out=value&0x3fff;
    return otp14_source_end(a,st,1);
}
static inline lt48_status otp14_source_cleanup_callback(void *context,unsigned role) {
    return otp14_source_invoke(context,role,1,0,0,1,NULL);
}
static inline lt48_status otp14_source_cleanup_command(void *context,const unsigned char *bytes,size_t n) {
    otp14_source_adapter *a=context;return a->command(a->context,bytes,n);
}
static inline otp14_operator_io otp14_source_io(otp14_source_adapter *a) {
    otp14_operator_io io={a,otp14_source_scan,otp14_source_program,
        {a,otp14_source_config_action,otp14_source_config_read},
        {a,otp14_source_cleanup_callback,otp14_source_cleanup_command}};return io;
}
static inline lt48_status otp14_source_execute_options(otp14_source_adapter *a,lt48_operation_kind op,
    const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result,int margin) {
    if(!a)return LT48_INVALID_ARGUMENT;
    if(image)for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++){a->image[2*i]=image[i];a->image[2*i+1]=image[i]>>8;}
    lt48_status st=otp14_source_prepare(a,1);
    if(st!=LT48_OK)return st; /* pure conversion: no hardware has started */
    for(size_t i=0;i<a->variable_count;i++) {
        if(a->variables[i].variable_id==5)a->policy.configuration.minimum=a->variables[i].value;
        if(a->variables[i].variable_id==6)a->policy.configuration.maximum=a->variables[i].value;
    }
    otp14_operator_io io=otp14_source_io(a);
    unsigned levels[3],count=0;
    if(margin>=0&&op==LT48_OPERATION_VERIFY) {
        if(margin==LT48_VERIFY_MARGIN_NOMINAL)levels[count++]=a->policy.configuration.nominal;
        else if(margin==LT48_VERIFY_MARGIN_LOW)levels[count++]=a->policy.configuration.minimum;
        else if(margin==LT48_VERIFY_MARGIN_HIGH)levels[count++]=a->policy.configuration.maximum;
        else if(margin==LT48_VERIFY_MARGIN_ALL){levels[count++]=a->policy.configuration.minimum;levels[count++]=a->policy.configuration.maximum;}
        else return LT48_INVALID_ARGUMENT;
    }
    return otp14_operator_execute_levels(&a->policy,&io,op,image,output,scratch,result,count?levels:NULL,count);
}
static inline lt48_status otp14_source_execute(otp14_source_adapter *a,lt48_operation_kind op,
    const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result) {
    return otp14_source_execute_options(a,op,image,output,scratch,result,-1);
}
/* Private candidate constructor. Exact source hash and guarded word/config
 * compilation precede any I/O. Profile, package, transport and GUI admission
 * remain separate; this does not open a programmer or enable a device. */
static inline lt48_status otp14_source_open(const char *path,const unsigned char hash[32],
    uint16_t family,uint16_t pinmap,otp14_operator_policy policy,
    lt48_pic_source_runner runner,lt48_pic_source_runner cleanup_runner,
    lt48_status (*command)(void *,const unsigned char *,size_t),lt48_status (*ready)(void *),void *context,otp14_source_adapter **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;if(!runner||!cleanup_runner||!command||!ready||!policy.code_words||policy.code_words>0x2000||
        !otp14_configuration_target_valid(&policy.configuration,0x3fff))return LT48_INVALID_ARGUMENT;
    otp14_source_adapter *a=calloc(1,sizeof(*a));if(!a)return LT48_NO_MEMORY;
    a->policy=policy;a->runner=runner;a->cleanup_runner=cleanup_runner;a->command=command;a->ready=ready;a->context=context;
    lt48_status st=lt48_pic_source_resources_otp14_library_internal(path,hash,family,pinmap,&a->resources);
    const lt48_pin_map *pins=NULL;const lt48_compile_variable *vars=NULL;size_t count=0;
    if(st==LT48_OK)st=lt48_pic_source_resources_bindings_internal(a->resources,&pins,&vars,&count);
    if(st==LT48_OK&&count>256)st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK){memcpy(a->variables,vars,count*sizeof(*vars));a->variable_count=count;}
    unsigned size=0;
    for(size_t i=0;st==LT48_OK&&i<count;i++)if(vars[i].variable_id==19){if(vars[i].value!=policy.code_words)st=LT48_SAFETY_REJECTED;size++;}
    if(st==LT48_OK&&size!=1)st=LT48_SAFETY_REJECTED;
    for(size_t i=0;st==LT48_OK&&i<lt48_pic_source_resources_count_internal(a->resources);i++) {
        lt48_pic_source_resource_entry e;st=lt48_pic_source_resources_entry_internal(a->resources,i,&e);
        if(st==LT48_OK&&(e.role==10||e.role==14)) {
            lt48_compact_wave *source=NULL;st=lt48_compact_decode(e.source.data,e.source.data_size,&source);
            if(st==LT48_OK)st=otp14_configuration_compile(source,pins,vars,count,e.role,&a->configuration[e.role==14?0:1]);
            lt48_compact_wave_close(source);
        }
    }
    if(st==LT48_OK&&(!a->configuration[0]||!a->configuration[1]))st=LT48_SAFETY_REJECTED;
    memset(a->image,255,sizeof a->image);
    lt48_runtime_options options={0};options.device_image=a->image;options.device_image_size=sizeof a->image;options.device_element_bytes=2;
    options.use_explicit_legacy_configuration_object=1;options.legacy_configuration_object=-2;
    if(st==LT48_OK)st=lt48_pic_source_resources_create_session_internal(a->resources,&options,&a->session);
    if(st==LT48_OK) {
        lt48_pic_source_state seeded;st=lt48_pic_source_session_state_internal(a->session,&seeded);
        if(st==LT48_OK&&seeded.configuration_count>1024)st=LT48_RUNTIME_ERROR;
        if(st==LT48_OK){memcpy(a->fields,seeded.configuration,seeded.configuration_count*sizeof(*a->fields));a->field_count=seeded.configuration_count;}
    }
    if(st!=LT48_OK){otp14_source_close(a);return st;}
    *out=a;return LT48_OK;
}
#endif
