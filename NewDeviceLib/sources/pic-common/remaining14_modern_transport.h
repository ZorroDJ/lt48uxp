#ifndef LT48_PIC_REMAINING14_MODERN_TRANSPORT_H
#define LT48_PIC_REMAINING14_MODERN_TRANSPORT_H
#include "remaining14_modern_source.h"
#include "remaining14_parity_transport.h"
typedef rm14_parity_transport rm14_modern_transport;
static inline lt48_status rm14_modern_transport_transfer(void *v,int read,unsigned bits,uint32_t value,uint32_t *out) {
 rm14_modern_transport *c=v;
 if(!out||(read!=0&&read!=1)||(read&&bits!=14)||(!read&&bits!=1&&bits!=6&&bits!=14)||
    (bits!=1&&bits!=6&&bits!=14)||value>=(1u<<bits))return LT48_INVALID_ARGUMENT;
 if(c->source.api->cancelled&&c->source.api->cancelled(c->source.context))return LT48_CANCELLED;
 lt48_status st=LT48_OK;
 if(!c->source.wire||!c->source.normal)st=otp14_transport_queue(&c->source,1);
 const unsigned char bytes[]={(unsigned char)(read?0x60:0x58),(unsigned char)(c->data+(read?2:value&1)),
  (unsigned char)c->clock,(unsigned char)(bits-1),(unsigned char)value,(unsigned char)(value>>8)};
 if(st==LT48_OK&&read){const unsigned char prime[]={0x18,(unsigned char)(c->clock+1)};st=lt48_runtime_transport_session_controller_write(c->source.wire,prime,sizeof prime);}
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c->source.wire,bytes,read?4:4+(bits+7)/8);
 if(st==LT48_OK&&read){unsigned char got[2];st=lt48_runtime_transport_session_controller_capture_internal(c->source.wire,got,2);
  if(st==LT48_OK)*out=(got[0]|(unsigned)got[1]<<8)&0x3fff;}
 else if(st==LT48_OK)*out=0;
 return st;
}
typedef lt48_status (*rm14_otp_source_factory)(const char *,const lt48_device_spec *,const rm14_profile *,
 const unsigned char *,size_t,lt48_pic_source_runner,lt48_pic_source_runner,
 lt48_status (*)(void *,const unsigned char *,size_t),lt48_status (*)(void *),void *,
 const lt48_pic_classic_wire *,rm14_modern_source **);
/* Complete dormant production join. Preparation/cleanup follows the existing
 * compact OTP transport; exact source lifecycle and ordinary14 owner are separate.
 * Caller supplies the verified idle image. Partial FPGA download forbids
 * subsequent traffic and cannot be reported as successful cleanup. */
typedef lt48_status (*rm14_otp_source_executor)(rm14_modern_source *,lt48_operation_kind,const uint16_t *,uint16_t *,uint16_t *,otp14_operator_result *);
static inline lt48_status rm14_modern_transport_execute_owned_options(const char *path,const lt48_device_spec *spec,
    const rm14_profile *p,rm14_otp_source_factory factory,rm14_otp_source_executor execute,const unsigned char *idle,size_t idle_size,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,
    const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result,unsigned verify_selection) {
    if(verify_selection>4)return LT48_INVALID_ARGUMENT;
    if(!p||!factory||!execute||!api||!idle||idle_size!=2778||!scratch||!result)return LT48_INVALID_ARGUMENT;
    if(op==LT48_OPERATION_ERASE||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_READ&&!output)||
       ((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image))return LT48_INVALID_ARGUMENT;
    if(otp14_operator_overlap(image,scratch)||(op==LT48_OPERATION_READ&&otp14_operator_overlap(output,scratch)))return LT48_INVALID_ARGUMENT;
    if(image)for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++)if(image[i]>0x3fff)return LT48_SAFETY_REJECTED;
    memset(result,0,sizeof *result);result->first_mismatch=UINT32_MAX;
    unsigned char initial[16400];for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++) {
        unsigned value=image?image[i]:0x3fff;initial[2*i]=value;initial[2*i+1]=value>>8;
    }
    rm14_modern_transport c={.source={.api=api,.cleanup_api=*api,.context=context}};c.source.cleanup_api.cancelled=NULL;
    lt48_pic_classic_wire raw={&c,rm14_modern_transport_transfer,rm14_parity_transport_delay,NULL};
    rm14_modern_source *a=NULL;
    lt48_status st=factory(path,spec,p,initial,sizeof initial,otp14_transport_runner,
        otp14_transport_cleanup_runner,otp14_transport_command,otp14_transport_ready,&c.source,&raw,&a);
    if(st!=LT48_OK)return st;
    a->verify_selection=verify_selection;
    const rm14_otp_policy *policy=&a->policy;
    if(image) {
        lt48_status validation=LT48_OK;
        for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++)if(image[i]>0x3fff||
            (!otp14_operator_mask(&policy->common,i)&&image[i]!=0x3fff)){validation=LT48_SAFETY_REJECTED;break;}
        for(unsigned i=0x2000;i<=0x2003;i++)if((image[i]|policy->common.id_mask)!=0x3fff)validation=LT48_SAFETY_REJECTED;
        if(!rm14_otp_target_valid(policy,image[0x2007]))validation=LT48_SAFETY_REJECTED;
        if(validation!=LT48_OK){result->operation_status=validation;rm14_modern_source_close(a);return validation;}
    }
    a->word_timeout=rm14_parity_transport_timeout;a->pulse=rm14_parity_transport_pulse;a->flush=rm14_parity_transport_flush;
    const lt48_pin_map *pins;const lt48_compile_variable *vars;size_t nv;
    st=lt48_pic_source_resources_bindings_internal(a->binding->resources,&pins,&vars,&nv);
    lt48_logical_pin data,clock;
    if(st==LT48_OK)st=lt48_pin_map_find_logical(pins,24,&data);
    if(st==LT48_OK)st=lt48_pin_map_find_logical(pins,23,&clock);
    if(st!=LT48_OK){rm14_modern_source_close(a);return st;}
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
    if(st==LT48_OK)st=execute(a,op,image,output,scratch,result);
    if(!result->cleanup.attempted) {
        const lt48_pic_cleanup_io cleanup={a,rm14_modern_source_cleanup,rm14_modern_source_cleanup_command};
        result->operation_status=st;st=lt48_pic_cleanup(&cleanup,st,&result->cleanup);
    }
done:
    lt48_runtime_transport_session_close(c.source.wire);rm14_modern_source_close(a);return st;
}
static inline lt48_status rm14_modern_transport_execute_owned(const char *path,const lt48_device_spec *spec,
 const rm14_profile *p,rm14_otp_source_factory factory,rm14_otp_source_executor execute,const unsigned char *idle,size_t idle_size,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind op,const uint16_t *image,
 uint16_t *output,uint16_t *scratch,otp14_operator_result *result){
 return rm14_modern_transport_execute_owned_options(path,spec,p,factory,execute,idle,idle_size,api,context,op,image,output,scratch,result,0);
}
static inline lt48_status rm14_modern_transport_execute_bound(const char *path,const lt48_device_spec *spec,
 const rm14_profile *p,rm14_otp_source_factory factory,const unsigned char *idle,size_t idle_size,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind op,const uint16_t *image,
 uint16_t *output,uint16_t *scratch,otp14_operator_result *result){
 return rm14_modern_transport_execute_owned(path,spec,p,factory,rm14_modern_source_execute,idle,idle_size,api,context,op,image,output,scratch,result);
}
static inline lt48_status rm14_modern_transport_execute(const char *path,const lt48_device_spec *spec,
 const rm14_profile *p,const unsigned char *idle,size_t idle_size,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result) {
 return rm14_modern_transport_execute_bound(path,spec,p,rm14_modern_source_open,idle,idle_size,api,context,op,image,output,scratch,result);
}
#endif
