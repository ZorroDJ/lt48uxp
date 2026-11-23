#include "at89_c5132_sequence.h"
#include "internal.h"
#include "at89_c5132_guard.h"
#include "../../NewDeviceLib/sources/at89/c5132_timeout_guard.c"
#include <stdlib.h>
#include <string.h>
static int valid(const lt48_c5132_resources *r) {
    return r && r->base && r->base->definition==0xf1 && r->base->family==125 &&
        r->base->pinmap==55 && r->base->primary_count==69632 && r->base->auxiliary_value==69760;
}
void lt48_c5132_resources_close(lt48_c5132_resources *r) {
    if(!r)return;
    lt48_compiled_wave_close(r->extra_read);lt48_compiled_wave_close(r->extra_write);lt48_at89_tail_close(r->base);free(r);
}
lt48_status lt48_c5132_resources_open(const lt48_installation *x,size_t index,lt48_c5132_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_c5132_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    lt48_status status=lt48_at89_tail_open(x,index,&r->base);
    if(status==LT48_OK && !valid(r))status=LT48_UNSUPPORTED;
    if(status==LT48_OK)status=lt48_c5132_guard_applet(r->base->read_applet.data,r->base->read_applet.data_size,
        r->guarded_read,sizeof r->guarded_read,&r->guarded_read_size);
    size_t resource;lt48_wave_code w;lt48_compact_wave *compact=NULL;
    if(status==LT48_OK)status=lt48_rez_find_resource(r->base->rez,"W104",&resource);
    if(status==LT48_OK)status=lt48_rez_get_wave_code(r->base->rez,resource,&w);
    /* The archive, definition and operation mapping were pinned by the loader.
     * This is the complete original 128-byte read loop, before any edits. */
    if(status==LT48_OK && (w.wave_type || w.data_size!=327))status=LT48_SAFETY_REJECTED;
    if(status==LT48_OK) {
        unsigned char prefix[0x32];prefix[0]=0x30;prefix[1]=0;memcpy(prefix+2,w.data+2,0x30);
        status=lt48_compact_decode(prefix,sizeof prefix,&compact);
        if(status==LT48_OK)status=lt48_compact_compile(compact,r->base->pins,r->base->vars,3,1,0,&r->extra_read);
    }
    lt48_compact_wave_close(compact);compact=NULL;
    if(status==LT48_OK) {
        /* Split before the first host-image edit. A checked read must finish
         * before the caller may enter this suffix and mutate the device. */
        unsigned char suffix[279];suffix[0]=0x15;suffix[1]=1;
        memcpy(suffix+2,w.data+0x32,277);
        status=lt48_compact_decode(suffix,sizeof suffix,&compact);
        if(status==LT48_OK)status=lt48_compact_compile(compact,r->base->pins,r->base->vars,3,1,0,&r->extra_write);
    }
    lt48_compact_wave_close(compact);
    if(status!=LT48_OK)lt48_c5132_resources_close(r);else *out=r;
    return status;
}
static void address(lt48_c5132_frame *f,uint32_t a) {
    for(unsigned i=0;i<4;i++)f->buffers[0][i]=(unsigned char)(a>>(8*i));
}
lt48_status lt48_c5132_transport_prepare(lt48_c5132_transport *t) {
    if(!t || !t->wire || !valid(t->resources) || !t->api || !t->api->write || !t->api->read)return LT48_INVALID_ARGUMENT;
    t->guard_loaded=0;
    lt48_status status=lt48_owner_load_applet_internal(t->wire,0x4100,t->resources->guarded_read,t->resources->guarded_read_size);
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(t->wire);
    if(status==LT48_OK)status=lt48_c5132_guard_check(t->api,t->context);
    if(status==LT48_OK)t->guard_loaded=1;
    return status;
}
lt48_status lt48_c5132_transport_step(void *v,unsigned operation,lt48_c5132_frame *f) {
    lt48_c5132_transport *t=v;
    if(!t || !f || !t->wire || !valid(t->resources) || f->config_count>5)return LT48_INVALID_ARGUMENT;
    if(operation>13 || (operation!=0 && operation!=1 && operation!=2 && operation!=3 && operation!=4 && operation!=5 && operation!=6 && operation!=8 && operation!=10 && operation!=13))return LT48_UNSUPPORTED;
    int reading=operation==4 || operation==5 || operation==13 || (operation==2 && f->mode==1);
    if(operation!=0 && operation!=1 && operation!=3 && (!t->guard_loaded || !t->api || !t->api->write || !t->api->read))return LT48_SAFETY_REJECTED;
    const lt48_at89_tail_resources *r=t->resources->base;
    unsigned char *image=NULL;
    if(operation==8 || operation==13){image=malloc(69760);if(!image)return LT48_NO_MEMORY;memset(image,255,69760);memcpy(image+69632,f->extra,128);}
    lt48_runtime_buffer_seed seeds[31];size_t count=0;
    for(unsigned i=0;i<32;i++)if(i!=4)seeds[count++]=(lt48_runtime_buffer_seed){i,f->buffers[i],32};
    lt48_runtime_nested_wave nested={12,r->wave[12]};lt48_runtime_options o={0};
    o.mode=f->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=4;o.parameter_buffer_id=1;
    o.initial_buffers=seeds;o.initial_buffer_count=count;o.configuration=f->config;o.configuration_count=f->config_count;
    o.nested_waves=&nested;o.nested_wave_count=1;o.root_timeout_seconds=10;
    if(image){o.device_image=image;o.device_image_size=69760;o.device_element_bytes=1;}
    memcpy(o.initial_voltage_centivolts,f->volts,sizeof f->volts);
    lt48_runtime_result *result=NULL;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(t->wire,10000,1);
    if(status==LT48_OK)status=lt48_owner_run_result_internal(t->wire,operation==13?t->resources->extra_read:operation==8?t->resources->extra_write:r->wave[operation],
        r->pins,t->variables,3,&o,NULL,0,f->buffers,f->config,5,&f->config_count,f->volts,&result);
    if(status==LT48_OK && reading) {
        status=lt48_runtime_transport_session_flush(t->wire);
        if(status==LT48_OK)status=lt48_c5132_guard_check(t->api,t->context);
        if(status!=LT48_OK)t->guard_loaded=0;
    }
    if(status==LT48_OK && image) {
        const unsigned char *updated;size_t n;
        status=lt48_runtime_get_image(result,&updated,&n);
        if(status==LT48_OK && n!=69760)status=LT48_RUNTIME_ERROR;
        if(status==LT48_OK)memcpy(f->extra,updated+69632,128);
    }
    if(status!=LT48_OK && operation!=0 && operation!=1 && operation!=3)t->guard_loaded=0;
    lt48_runtime_result_close(result);free(image);return status;
}
lt48_status lt48_c5132_memory_sequence(const lt48_c5132_resources *r,lt48_c5132_step_fn step,
    void *context,lt48_c5132_frame *f,int program,uint32_t start,size_t length,
    const unsigned char *input,unsigned char *output,size_t *completed,lt48_status *reset) {
    if(!step || !f || !completed || !reset)return LT48_INVALID_ARGUMENT;
    *completed=0;*reset=LT48_OK;if(!valid(r))return LT48_UNSUPPORTED;
    if(program!=0 && program!=1)return LT48_INVALID_ARGUMENT;
    if(!length || start>=69632 || length>69632-start)return LT48_SAFETY_REJECTED;
    if((program && (!input || (start&127) || (length&127))) || (!program && !output))return LT48_INVALID_ARGUMENT;
    memset(f->buffers,0,sizeof f->buffers);f->mode=program?2:1;address(f,start);
    lt48_status status=step(context,2,f);
    for(size_t i=0;status==LT48_OK && i<length;i++) {
        address(f,start+(uint32_t)i);if(program)f->buffers[2][0]=input[i];
        status=step(context,program?6:4,f);if(status!=LT48_OK)break;
        if(!program)output[i]=f->buffers[1][0];
        *completed=i+1;
    }
    *reset=step(context,3,f);return status==LT48_OK?*reset:status;
}
static void seed(lt48_c5132_frame *f,const unsigned char *values) {
    const unsigned fields[]={5,4,7,9,11};f->config_count=5;
    for(unsigned i=0;i<5;i++)f->config[i]=(lt48_runtime_configuration_value){0,i<2?0:i==2?1:2,fields[i],values?values[i]:UINT32_MAX};
}
static lt48_status values(const lt48_c5132_frame *f,unsigned char out[5]) {
    const unsigned fields[]={5,4,7,9,11};
    for(unsigned i=0;i<5;i++) {
        unsigned found=UINT32_MAX,category=i<2?0:i==2?1:2;
        for(size_t j=0;j<f->config_count;j++)if(f->config[j].object_id==0 && f->config[j].category==category && f->config[j].field==fields[i])found=f->config[j].value;
        if(found>(i<2?1u:i==2?3u:255u))return LT48_DEVICE_MISMATCH;
        out[i]=(unsigned char)found;
    }
    return LT48_OK;
}
static lt48_status settings_sequence(const lt48_c5132_resources *r,lt48_c5132_step_fn step,
    void *context,lt48_c5132_frame *f,const unsigned char *desired,unsigned char *actual,lt48_status *reset,const unsigned char *preserved) {
    if(!step || !f || !actual || !reset)return LT48_INVALID_ARGUMENT;
    *reset=LT48_OK;if(!valid(r))return LT48_UNSUPPORTED;
    if(desired)for(unsigned i=0;i<3;i++)if(desired[i]>(i<2?1u:3u))return LT48_INVALID_ARGUMENT;
    unsigned char observed[5],expected_extra[128];lt48_status status=LT48_OK;
    if(desired) {
        memset(f->buffers,0,sizeof f->buffers);f->mode=8;seed(f,desired);
        status=step(context,2,f);
        if(status==LT48_OK){if(preserved)memcpy(f->extra,preserved,128);else status=step(context,13,f);}
        if(status==LT48_OK)status=step(context,8,f);
        if(status==LT48_OK)memcpy(expected_extra,f->extra,128);
        *reset=step(context,3,f);if(status==LT48_OK)status=*reset;
    }
    if(status==LT48_OK) {
        memset(f->buffers,0,sizeof f->buffers);f->mode=1;seed(f,NULL);
        status=step(context,2,f);if(status==LT48_OK)status=values(f,observed);
        if(status==LT48_OK && desired)status=step(context,13,f);
        *reset=step(context,3,f);if(status==LT48_OK)status=*reset;
        if(status==LT48_OK && desired && (memcmp(observed,desired,5) || memcmp(f->extra,expected_extra,128)))status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)memcpy(actual,observed,5);
    return status;
}

lt48_status lt48_c5132_settings_sequence(const lt48_c5132_resources *r,lt48_c5132_step_fn step,
    void *context,lt48_c5132_frame *f,const unsigned char *desired,unsigned char *actual,lt48_status *reset) {
    return settings_sequence(r,step,context,f,desired,actual,reset,NULL);
}
lt48_status lt48_c5132_restore_sequence(const lt48_c5132_resources *r,lt48_c5132_step_fn step,
    void *context,lt48_c5132_frame *f,lt48_c5132_erase_state *state,lt48_status *reset) {
    if(!state || !reset)return LT48_INVALID_ARGUMENT;
    if(!state->restore_pending || state->erase_incomplete)return LT48_SAFETY_REJECTED;
    unsigned char actual[5];lt48_status status=settings_sequence(r,step,context,f,state->settings,actual,reset,state->extra);
    if(status==LT48_OK)state->restore_pending=0;
    return status;
}
lt48_status lt48_c5132_erase_sequence(const lt48_c5132_resources *r,lt48_c5132_step_fn step,
    void *context,lt48_c5132_frame *f,lt48_c5132_erase_state *state,lt48_status *reset) {
    if(!step || !f || !state || !reset)return LT48_INVALID_ARGUMENT;
    *reset=LT48_OK;if(!valid(r))return LT48_UNSUPPORTED;
    if(state->restore_pending)return LT48_SAFETY_REJECTED;
    state->verified=0;state->mismatch_address=UINT32_MAX;state->erase_incomplete=0;
    unsigned char *image=malloc(69632);if(!image)return LT48_NO_MEMORY;
    lt48_status status=lt48_c5132_settings_sequence(r,step,context,f,NULL,state->settings,reset);
    if(status==LT48_OK){
        memset(f->buffers,0,sizeof f->buffers);f->mode=7;
        status=step(context,2,f);
        if(status==LT48_OK)status=step(context,13,f);
        if(status==LT48_OK) {
            memcpy(state->extra,f->extra,128);state->settings[2]=0;
            state->restore_pending=1;state->erase_incomplete=1;
            status=step(context,10,f);if(status==LT48_OK)state->erase_incomplete=0;
        }
        *reset=step(context,3,f);if(status==LT48_OK)status=*reset;
    }
    if(status==LT48_OK)status=lt48_c5132_restore_sequence(r,step,context,f,state,reset);
    if(status==LT48_OK){
        size_t done;status=lt48_c5132_memory_sequence(r,step,context,f,0,0,69632,NULL,image,&done,reset);
        if(status==LT48_OK)for(size_t i=0;i<69632;i++){
            if(image[i]!=255){state->mismatch_address=(uint32_t)i;status=LT48_DEVICE_MISMATCH;break;}
            state->verified=i+1;
        }
    }
    free(image);return status;
}
