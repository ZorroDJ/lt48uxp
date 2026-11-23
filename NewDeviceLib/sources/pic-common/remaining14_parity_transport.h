#ifndef LT48_PIC_REMAINING14_PARITY_TRANSPORT_H
#define LT48_PIC_REMAINING14_PARITY_TRANSPORT_H
#include "remaining14_parity_source.h"
#include "remaining14_controller.h"
#include "remaining14_atomic_pulse.h"
#include "otp14_transport.h"
typedef struct {otp14_transport_context source;unsigned data,clock;} rm14_parity_transport;
static inline lt48_status rm14_parity_transport_flush(void *v) {
    rm14_parity_transport *c=v;
    return c->source.wire?lt48_runtime_transport_session_flush(c->source.wire):LT48_OK;
}
static inline lt48_status rm14_parity_transport_pulse(void *v) {
    rm14_parity_transport *c=v;
    if(c->source.api->cancelled&&c->source.api->cancelled(c->source.context))return LT48_CANCELLED;
    return rm14_atomic_pulse(c->source.wire,c->data,c->clock);
}
static inline lt48_status rm14_parity_transport_timeout(void *v,unsigned milliseconds) {
    rm14_parity_transport *c=v;
    return lt48_runtime_transport_session_wave_watchdog(c->source.wire,milliseconds,1);
}
static inline lt48_status rm14_parity_transport_transfer(void *v,int read,unsigned bits,uint32_t value,uint32_t *out) {
    rm14_parity_transport *c=v;
    if(c->source.api->cancelled&&c->source.api->cancelled(c->source.context))return LT48_CANCELLED;
    lt48_status st=LT48_OK;
    if(!c->source.wire||!c->source.normal)st=otp14_transport_queue(&c->source,1);
    rm14_controller raw={c->source.wire,c->data,c->clock};
    return st==LT48_OK?rm14_controller_transfer(&raw,read,bits,value,out):st;
}
static inline lt48_status rm14_parity_transport_delay(void *v,unsigned us) {
    rm14_parity_transport *c=v;
    if(!us||us>255)return LT48_INVALID_ARGUMENT;
    const unsigned char bytes[]={0x4e,(unsigned char)us};
    return lt48_runtime_transport_session_controller_write(c->source.wire,bytes,sizeof bytes);
}
/* Complete dormant production join. Preparation/cleanup follows the existing
 * compact OTP transport; exact source lifecycle and raw16 owner are separate.
 * Caller supplies the verified idle image. Partial FPGA download forbids
 * subsequent traffic and cannot be reported as successful cleanup. */
static inline lt48_status rm14_parity_transport_execute_options(const char *path,const lt48_device_spec *spec,
    const rm14_profile *p,const unsigned char *idle,size_t idle_size,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,
    const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result,unsigned verify_selection) {
    if(verify_selection>4)return LT48_INVALID_ARGUMENT;
    if(!p||!api||!idle||idle_size!=2778||!scratch||!result)return LT48_INVALID_ARGUMENT;
    if(op==LT48_OPERATION_ERASE||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_READ&&!output)||
       ((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image))return LT48_INVALID_ARGUMENT;
    if(otp14_operator_overlap(image,scratch)||(op==LT48_OPERATION_READ&&otp14_operator_overlap(output,scratch)))return LT48_INVALID_ARGUMENT;
    const otp14_operator_policy policy={p->code_words,0x3fff,{0x3fff,0x3f30,500,400,600}};
    if(image){for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++){
        if(image[i]>0x3fff||(!otp14_operator_mask(&policy,i)&&image[i]!=0x3fff))return LT48_SAFETY_REJECTED;
        }
        if(!rm14_parity_configuration_valid(image[0x2007]))return LT48_SAFETY_REJECTED;}
    memset(result,0,sizeof *result);result->first_mismatch=UINT32_MAX;
    unsigned char initial[16400];for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++) {
        unsigned value=image?image[i]:0x3fff;initial[2*i]=value;initial[2*i+1]=value>>8;
    }
    rm14_parity_transport c={.source={.api=api,.cleanup_api=*api,.context=context}};c.source.cleanup_api.cancelled=NULL;
    lt48_pic_classic_wire raw={&c,rm14_parity_transport_transfer,rm14_parity_transport_delay,NULL};
    rm14_parity_source *a=NULL;
    lt48_status st=rm14_parity_source_open(path,spec,p,initial,sizeof initial,otp14_transport_runner,
        otp14_transport_cleanup_runner,otp14_transport_command,otp14_transport_ready,&c.source,&raw,&a);
    if(st!=LT48_OK)return st;
    a->verify_selection=verify_selection;
    a->word_timeout=rm14_parity_transport_timeout;a->pulse=rm14_parity_transport_pulse;a->flush=rm14_parity_transport_flush;
    const lt48_pin_map *pins;const lt48_compile_variable *vars;size_t nv;
    st=lt48_pic_source_resources_bindings_internal(a->binding->resources,&pins,&vars,&nv);
    lt48_logical_pin data,clock;
    if(st==LT48_OK)st=lt48_pin_map_find_logical(pins,24,&data);
    if(st==LT48_OK)st=lt48_pin_map_find_logical(pins,23,&clock);
    if(st!=LT48_OK){rm14_parity_source_close(a);return st;}
    c.data=data.engine_pin;c.clock=clock.engine_pin;
    lt48_status guard=LT48_OK;
    for(unsigned i=0;i<5;i++) {
        const unsigned char off[]={0x10,(unsigned char)(3-i),0},clear[]={0x49,0xaa,0x55};
        lt48_status current=otp14_transport_command(&c.source,i<4?off:clear,3);
        if(guard==LT48_OK)guard=current;
    }
    if(guard!=LT48_OK){result->operation_status=guard;result->cleanup=(lt48_pic_cleanup_result){guard,5,0};st=guard;goto done;}
    st=otp14_transport_queue(&c.source,1);
    if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(c.source.wire,10000,1);
    int download_started=0;
    const unsigned char download[]={0x38,0xaa,0x55},busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK){download_started=1;st=lt48_runtime_transport_session_controller_write(c.source.wire,download,sizeof download);}
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.source.wire,idle,idle_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.source.wire,busy,sizeof busy);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c.source.wire);
    if(st!=LT48_OK&&download_started) {
        result->operation_status=st;result->cleanup=(lt48_pic_cleanup_result){LT48_CLEANUP_FAILED,5,0};goto done;
    }
    if(st==LT48_OK)st=rm14_parity_source_execute(a,op,image,output,scratch,result);
    if(!result->cleanup.attempted) {
        const lt48_pic_cleanup_io cleanup={&a->source,otp14_source_cleanup_callback,otp14_source_cleanup_command};
        result->operation_status=st;st=lt48_pic_cleanup(&cleanup,st,&result->cleanup);
    }
done:
    lt48_runtime_transport_session_close(c.source.wire);rm14_parity_source_close(a);return st;
}
static inline lt48_status rm14_parity_transport_execute(const char *path,const lt48_device_spec *spec,
 const rm14_profile *p,const unsigned char *idle,size_t idle_size,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result){
 return rm14_parity_transport_execute_options(path,spec,p,idle,idle_size,api,context,op,image,output,scratch,result,0);
}
#endif
