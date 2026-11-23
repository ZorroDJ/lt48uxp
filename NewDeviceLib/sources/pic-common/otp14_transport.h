#ifndef LT48_OTP14_TRANSPORT_H
#define LT48_OTP14_TRANSPORT_H
#include "otp14_source_adapter.h"

typedef struct {
    const lt48_protocol_transport *api;lt48_protocol_transport cleanup_api;
    void *context;lt48_runtime_transport_session *wire;int normal;
} otp14_transport_context;
static inline lt48_status otp14_transport_queue(otp14_transport_context *c,int normal) {
    lt48_runtime_transport_session_close(c->wire);c->wire=NULL;c->normal=normal;
    return lt48_runtime_transport_session_create(normal?c->api:&c->cleanup_api,c->context,&c->wire);
}
static inline lt48_status otp14_transport_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **out) {
    otp14_transport_context *c=context;lt48_status st=LT48_OK;
    if(!c->wire||!c->normal)st=otp14_transport_queue(c,1);
    if(st==LT48_OK)st=lt48_runtime_transport_session_run(c->wire,machine,out);
    /* A finished interpreter may leave a write-only suffix queued. Deliver
     * it before replacing this queue for reset/cleanup or another stage. */
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c->wire);
    return st;
}
static inline lt48_status otp14_transport_cleanup_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **out) {
    otp14_transport_context *c=context;lt48_status st=otp14_transport_queue(c,0);
    if(st==LT48_OK)st=lt48_runtime_transport_session_run(c->wire,machine,out);
    /* A finished interpreter may leave a write-only suffix queued. Deliver
     * it before replacing this queue for reset/cleanup or another stage. */
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c->wire);
    return st;
}
static inline lt48_status otp14_transport_command(void *context,const unsigned char *bytes,size_t n) {
    otp14_transport_context *c=context;
    int selector=n==2&&bytes[0]==0x0f&&(bytes[1]==7||bytes[1]==15);
    int cleanup=(n==3&&bytes[0]==0x10&&bytes[1]<4&&bytes[2]==0)||
        (n==3&&bytes[0]==0x49&&bytes[1]==0xaa&&bytes[2]==0x55)||
        (n==2&&bytes[0]==0x40&&(bytes[1]==2||bytes[1]==4));
    if(!selector&&!cleanup)return LT48_SAFETY_REJECTED;
    lt48_status st=otp14_transport_queue(c,selector);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c->wire,bytes,n);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c->wire);
    return st;
}
static inline lt48_status otp14_transport_ready(void *context) {
    otp14_transport_context *c=context;
    return c->wire&&c->normal?lt48_runtime_transport_session_status(c->wire,1):LT48_PROTOCOL_ERROR;
}
static inline void otp14_transport_progress(void *context,unsigned mode,size_t completed,size_t total) {
    otp14_transport_context *c=context;
    (void)mode; /* Existing transport stage also represents generic memory work. */
    if(c->api->progress)c->api->progress(c->context,LT48_EXECUTION_READ_OPERATION,
        completed,total,0,0,0);
}
/* Dormant production transport join for candidate qualification. The caller
 * supplies a verified idle image and exact source/profile binding. No file
 * discovery, package inference or registry admission occurs here. Fresh host
 * queues are used for cleanup; this does not promise recovery of a device
 * FIFO that was left mid-frame by a physical transport failure. */
static inline lt48_status otp14_transport_execute_options(const char *path,const unsigned char hash[32],
    uint16_t family,uint16_t pinmap,otp14_operator_policy policy,
    const unsigned char *idle,size_t idle_size,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result,int margin) {
    if(!api||!idle||idle_size!=2778||!scratch||!result||!policy.code_words||policy.code_words>0x2000||
       !policy.id_mask||policy.id_mask>0x3fff||!otp14_configuration_target_valid(&policy.configuration,0x3fff))
        return LT48_INVALID_ARGUMENT;
    if(op==LT48_OPERATION_ERASE||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_READ&&!output)||
       ((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image))
        return LT48_INVALID_ARGUMENT;
    if(otp14_operator_overlap(image,scratch)||(op==LT48_OPERATION_READ&&otp14_operator_overlap(output,scratch)))return LT48_INVALID_ARGUMENT;
    if(image) {
        for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)if(image[a]>0x3fff||(!otp14_operator_mask(&policy,a)&&image[a]!=0x3fff))return LT48_SAFETY_REJECTED;
        if(!otp14_configuration_target_valid(&policy.configuration,image[0x2007]))return LT48_SAFETY_REJECTED;
    }
    if(margin>=0&&margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    memset(result,0,sizeof(*result));result->first_mismatch=UINT32_MAX;
    otp14_transport_context wire={.api=api,.cleanup_api=*api,.context=context};wire.cleanup_api.cancelled=NULL;
    otp14_source_adapter *a=NULL;
    lt48_status st=otp14_source_open(path,hash,family,pinmap,policy,otp14_transport_runner,
        otp14_transport_cleanup_runner,otp14_transport_command,otp14_transport_ready,&wire,&a);
    if(st!=LT48_OK)return st;
    a->progress=otp14_transport_progress;
    /* Establish rail-off/released outputs before entering the FPGA download
     * grammar. Once download starts, a failed stream can consume later control
     * bytes as configuration data; never claim those bytes cleared anything. */
    lt48_status guard=LT48_OK;
    for(unsigned phase=0;phase<5;phase++) {
        const unsigned char off[]={0x10,(unsigned char)(3-phase),0},clear[]={0x49,0xaa,0x55};
        lt48_status current=otp14_transport_command(&wire,phase<4?off:clear,3);
        if(guard==LT48_OK)guard=current;
    }
    if(guard!=LT48_OK){result->operation_status=guard;result->cleanup=(lt48_pic_cleanup_result){guard,5,0};
        lt48_runtime_transport_session_close(wire.wire);otp14_source_close(a);return guard;}
    int download_started=0;
    st=otp14_transport_queue(&wire,1);
    if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(wire.wire,10000,1);
    const unsigned char download[]={0x38,0xaa,0x55},busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK){download_started=1;st=lt48_runtime_transport_session_controller_write(wire.wire,download,sizeof download);}
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(wire.wire,idle,idle_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(wire.wire,busy,sizeof busy);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(wire.wire);
    if(st!=LT48_OK&&download_started) {
        result->operation_status=st;result->cleanup=(lt48_pic_cleanup_result){LT48_CLEANUP_FAILED,5,0};
        /* No subsequent controller traffic: framing needs external recovery. */
        lt48_runtime_transport_session_close(wire.wire);otp14_source_close(a);return st;
    }
    if(st==LT48_OK)st=otp14_source_execute_options(a,op,image,output,scratch,result,margin);
    if(!result->cleanup.attempted) {
        otp14_operator_io io=otp14_source_io(a);result->operation_status=st;
        st=lt48_pic_cleanup(&io.cleanup,st,&result->cleanup);
    }
    lt48_runtime_transport_session_close(wire.wire);otp14_source_close(a);return st;
}
static inline lt48_status otp14_transport_execute(const char *path,const unsigned char hash[32],uint16_t family,uint16_t pinmap,
    otp14_operator_policy policy,const unsigned char *idle,size_t idle_size,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result) {
    return otp14_transport_execute_options(path,hash,family,pinmap,policy,idle,idle_size,api,context,op,image,output,scratch,result,-1);
}
#endif
