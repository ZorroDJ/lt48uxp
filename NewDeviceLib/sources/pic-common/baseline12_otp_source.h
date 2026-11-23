#ifndef LT48_PIC_BASELINE12_OTP_SOURCE_H
#define LT48_PIC_BASELINE12_OTP_SOURCE_H
#include "baseline12_otp_controller.h"
#include "source_resources.h"
#include "source_host.h"
#include <stdlib.h>
/* Private complete source lifecycle join. Source callbacks remain immutable;
 * authored operations use their own image, while source factory bookkeeping
 * stays in the source session. No catalogue admission follows from opening. */
typedef struct {
    lt48_pic_source_resources *resources;
    lt48_pic_source_session *session;
    lt48_runtime_frame_profile frame;
    b12otp_controller controller;
    b12otp_parallel parallel;
    b12otp_serial serial;
    lt48_protocol_transport api,cleanup_api;
    void *context;
    unsigned volts[7],level,mode,active,prepared,upload_pending,is_serial,factory_notice;
} b12otp_source;
static inline void b12otp_source_close(b12otp_source *s) {
    if(!s)return;
    b12otp_controller_close(&s->controller);
    lt48_runtime_transport_session_close(s->controller.wire);
    lt48_pic_source_session_close_internal(s->session);
    lt48_pic_source_resources_close_internal(s->resources);free(s);
}
static inline lt48_status b12otp_source_queue(b12otp_source *s,int cleanup) {
    lt48_runtime_transport_session_close(s->controller.wire);s->controller.wire=NULL;
    return lt48_runtime_transport_session_create(cleanup?&s->cleanup_api:&s->api,s->context,&s->controller.wire);
}
static inline lt48_status b12otp_source_runner(void *v,lt48_runtime_machine *m,lt48_runtime_result **out) {
    b12otp_source *s=v;lt48_status st=lt48_runtime_transport_session_run(s->controller.wire,m,out);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->controller.wire);
    if(st==LT48_OK)for(size_t i=0;i<lt48_runtime_event_count(*out);i++) {
        lt48_runtime_event event;st=lt48_runtime_get_event(*out,i,&event);if(st!=LT48_OK)break;
        if(event.kind==LT48_RUNTIME_HOST_NOTIFICATION)s->factory_notice=1;
    }
    return st;
}
static inline lt48_status b12otp_source_call(b12otp_source *s,unsigned role) {
    return lt48_pic_source_session_invoke_wave_internal(s->session,(uint16_t)role,NULL,s->mode,0,10,b12otp_source_runner,s);
}
static inline lt48_status b12otp_source_write(void *v,const unsigned char *p,size_t n) {
    b12otp_source *s=v;
    return lt48_runtime_transport_session_controller_write(s->controller.wire,p,n);
}
static inline lt48_status b12otp_source_status(void *v,int settle,uint16_t *status) {
    b12otp_source *s=v;
    return lt48_runtime_transport_session_controller_status_internal(s->controller.wire,settle,status);
}
static inline lt48_status b12otp_source_entry(void *v,unsigned level) {
    b12otp_source *s=v;
    if(!s||!s->prepared||(level!=0&&level!=5&&level!=6))return LT48_SAFETY_REJECTED;
    lt48_status st=lt48_runtime_transport_session_flush(s->controller.wire);
    if(st==LT48_OK&&s->active)st=b12otp_source_call(s,4);
    if(st==LT48_OK) {
        /* Original compact host clears the source-declared scratch frame at
         * a mode start. Pre/Post scratch is not an initial device cursor. */
        lt48_pic_source_state state;unsigned char buffers[LT48_RUNTIME_BUFFER_COUNT][32];
        st=lt48_pic_source_session_state_internal(s->session,&state);
        if(st==LT48_OK){memcpy(buffers,state.buffers,sizeof buffers);
            st=lt48_runtime_frame_set_mode(buffers,s->frame.buffer_count,s->frame.reset_mask,s->frame.mode_buffer,(uint16_t)s->mode,0,0);}
        if(st==LT48_OK){state.buffers=(const unsigned char (*)[32])buffers;st=lt48_pic_source_session_restore_internal(s->session,&state);}
    }
    s->active=1;s->level=level;
    lt48_pic_source_host host={.voltage_variable=0,.poweron_role=0,.poweroff_role=1,
        .voltage_centivolts=s->volts[level],.minimum_centivolts=s->volts[5],.maximum_centivolts=s->volts[6],
        .write=b12otp_source_write,.status=b12otp_source_status,.runner=b12otp_source_runner,.context=s};
    if(st==LT48_OK)st=lt48_pic_source_host_begin_internal(s->session,&host,(uint16_t)s->mode,0);
    /* Parallel mode8 leaves physical configuration selected; mode2 already
     * increments to code0 and cannot back this adapter's fresh-config contract.
     * Serial mode2 retains PIC12 factory read/reset bookkeeping. */
    if(st==LT48_OK)st=b12otp_source_call(s,3);
    return st;
}
static inline lt48_status b12otp_source_serial_select(void *v,unsigned level) {
    b12otp_source *s=v;if(level!=0&&level!=5&&level!=6)return LT48_INVALID_ARGUMENT;
    s->level=level;return LT48_OK;
}
static inline lt48_status b12otp_source_serial_entry(void *v) {
    b12otp_controller *c=v;b12otp_source *s=c->context;
    return b12otp_source_entry(s,s->level);
}
static inline lt48_status b12otp_source_cleanup_role(void *v,unsigned role) {
    b12otp_source *s=v;if(role!=4&&role!=1)return LT48_INVALID_ARGUMENT;
    if(s->upload_pending)return LT48_SAFETY_REJECTED;
    lt48_status st=b12otp_source_queue(s,1);
    if(st==LT48_OK)st=b12otp_source_call(s,role);
    return st;
}
static inline lt48_status b12otp_source_cleanup_command(void *v,const unsigned char *p,size_t n) {
    b12otp_source *s=v;
    if(s->upload_pending)return LT48_SAFETY_REJECTED;
    lt48_status st=b12otp_source_queue(s,1);
    if(st==LT48_OK)st=b12otp_source_write(s,p,n);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->controller.wire);
    return st;
}
static inline lt48_status b12otp_source_prepare(b12otp_source *s,const unsigned char *idle,size_t size) {
    if(!s||!idle||size!=2778||s->prepared||s->active)return LT48_INVALID_ARGUMENT;
    lt48_status first=LT48_OK;
    for(unsigned phase=0;phase<5;phase++) {
        const unsigned char off[]={0x10,(unsigned char)(3-phase),0},clear[]={0x49,0xaa,0x55};
        lt48_status st=b12otp_source_cleanup_command(s,phase<4?off:clear,3);
        if(first==LT48_OK)first=st;
    }
    if(first!=LT48_OK)return first;
    lt48_status st=b12otp_source_queue(s,0);
    if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(s->controller.wire,10000,1);
    const unsigned char download[]={0x38,0xaa,0x55};
    if(st==LT48_OK){s->upload_pending=1;st=b12otp_source_write(s,download,sizeof download);}
    if(st==LT48_OK)st=b12otp_source_write(s,idle,size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->controller.wire);
    if(st==LT48_OK){s->upload_pending=0;s->prepared=1;}
    return st;
}
static inline lt48_status b12otp_source_open(const char *path,unsigned family,unsigned pinmap,
    unsigned words,const unsigned char image[8192],const lt48_protocol_transport *api,void *context,b12otp_source **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!path||!image||!api||!api->write||!api->read||
       (words!=512&&words!=1024&&words!=2048))return LT48_INVALID_ARGUMENT;
    /* Exact source-family domain. Definition/name/profile admission remains
     * a separate gate in the normal operator. No arbitrary class dispatch. */
    int allowed=(family>=22&&family<=25)||(family<=2)||(family>=4&&family<=21&&family!=5)||family==26||family==40||family==225||family==226||family==231||family==232;
    unsigned expected_pin=(family==231||family==232)?79:(family==225||family==226)?75:family==40?6:
        ((family>=8&&family<=11)||(family>=16&&family<=19))?0:1;
    if(!allowed||pinmap!=expected_pin)return LT48_UNSUPPORTED;
    const unsigned char hash[32]={0x24,0xb8,0x32,0x0d,0xa4,0xcf,0xd7,0x00,0xee,0x43,0x6f,0xf0,0x6b,0xd5,0x5d,0x9d,
        0xfb,0xd4,0x72,0xc5,0x3a,0x02,0x1a,0xd5,0x27,0xc9,0xbb,0xcb,0x3c,0x80,0x4d,0xec};
    b12otp_source *s=calloc(1,sizeof *s);if(!s)return LT48_NO_MEMORY;
    s->api=*api;s->cleanup_api=*api;s->cleanup_api.cancelled=NULL;s->context=context;
    s->is_serial=family==40||family==225||family==226||family==231||family==232;
    s->mode=(family==231||family==232)?8:s->is_serial?2:8;
    lt48_status st=lt48_pic_source_resources_library_internal(path,hash,(uint16_t)family,(uint16_t)pinmap,&s->resources);
    const lt48_pin_map *pins=NULL;const lt48_compile_variable *vars=NULL;size_t count=0;
    if(st==LT48_OK)st=lt48_pic_source_resources_bindings_internal(s->resources,&pins,&vars,&count);
    unsigned declared_words=0;
    for(size_t i=0;st==LT48_OK&&i<count;i++)if(vars[i].variable_id==19&&vars[i].kind==2)
        declared_words=(family==231||family==232)?vars[i].value+1:family==40?vars[i].value:vars[i].value-4;
    if(st==LT48_OK&&declared_words!=words)st=LT48_SAFETY_REJECTED;
    lt48_runtime_options options={0};options.device_image=image;options.device_image_size=8192;options.device_element_bytes=2;
    if(st==LT48_OK)st=lt48_pic_source_resources_frame_internal(s->resources,&s->frame);
    if(st==LT48_OK)st=lt48_pic_source_resources_create_session_internal(s->resources,&options,&s->session);
    if(st==LT48_OK)st=lt48_pic_source_session_configuration_internal(s->session,11,12,8,0,NULL,0,NULL,0);
    for(unsigned i=0;st==LT48_OK&&i<3;i++) {
        unsigned id=i==0?0:i==1?5:6;lt48_compile_variable value;
        st=lt48_pic_source_session_variable_internal(s->session,(uint16_t)id,&value);
        if(st==LT48_OK&&(value.kind!=0||!value.value))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK)s->volts[id]=value.value;
    }
    if(st==LT48_OK&&(s->volts[5]>s->volts[0]||s->volts[0]>s->volts[6]))st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK)st=b12otp_source_queue(s,0);
    s->controller.pins=pins;s->controller.context=s;s->controller.entry=b12otp_source_entry;
    if(st==LT48_OK)st=s->is_serial?b12otp_controller_bind_serial(&s->controller,family==40?58:48,family==40?40:49):b12otp_controller_bind_parallel(&s->controller);
    s->parallel=(b12otp_parallel){.context=&s->controller,.entry=b12otp_controller_entry,.control=b12otp_controller_control,
        .drive_data=b12otp_controller_drive,.release_data=b12otp_controller_release,.read_data=b12otp_controller_read,
        .delay=b12otp_controller_delay,.program_pulse=b12otp_controller_pulse,.code_words=words,.access_us=10};
    s->serial=(b12otp_serial){.wire={&s->controller,b12otp_controller_serial_transfer,b12otp_controller_delay,b12otp_source_serial_entry},
        .voltage_context=s,.select_voltage=b12otp_source_serial_select,.code_words=words};
    if(st!=LT48_OK){b12otp_source_close(s);return st;}
    *out=s;return LT48_OK;
}
static inline b12otp_io b12otp_source_io(b12otp_source *s) {
    if(!s)return (b12otp_io){0};
    return (b12otp_io){.context=s->is_serial?(void *)&s->serial:(void *)&s->parallel,
        .enter=s->is_serial?b12otp_serial_enter:b12otp_parallel_enter,.read=s->is_serial?b12otp_serial_read:b12otp_parallel_read,
        .increment=s->is_serial?b12otp_serial_increment:b12otp_parallel_increment,.pulses=s->is_serial?b12otp_serial_pulses:b12otp_parallel_pulses,
        .cleanup={s,b12otp_source_cleanup_role,b12otp_source_cleanup_command}};
}
#endif
