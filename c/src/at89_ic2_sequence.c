#include "at89_ic2_sequence.h"
#include <string.h>
#include <stdlib.h>
#include "internal.h"
static uint32_t counter(const lt48_ic2_frame *f) {
    return f->buffers[12][0]|(uint32_t)f->buffers[12][1]<<8|
        (uint32_t)f->buffers[12][2]<<16|(uint32_t)f->buffers[12][3]<<24;
}
/* The source PROGRAM increments buffer12 for each staged byte and emits its
 * two page-commit pulses when it reaches 128, then clears it. No partial final
 * page is sent: padding must be resolved by the operation owner beforehand. */
lt48_status lt48_ic2_memory_sequence(const lt48_at89_tail_resources *r,
    lt48_ic2_step_fn step,void *context,lt48_ic2_frame *f,int program,
    uint32_t start,size_t length,const unsigned char *input,unsigned char *output,
    size_t *completed,lt48_status *reset_status) {
    if(!r || !step || !f || !completed || !reset_status)return LT48_INVALID_ARGUMENT;
    *completed=0;*reset_status=LT48_OK;
    if(!mcu70_ic2_profile(r->definition,r->family,r->pinmap,r->primary_count))return LT48_UNSUPPORTED;
    if(program!=0 && program!=1)return LT48_INVALID_ARGUMENT;
    if(!length || start>=r->primary_count || length>r->primary_count-start)return LT48_SAFETY_REJECTED;
    if((program && (!input || (start&127) || (length&127))) || (!program && !output))return LT48_INVALID_ARGUMENT;
    memset(f->buffers,0,sizeof f->buffers);f->mode=program?2:1;
    for(unsigned b=0;b<4;b++)f->buffers[0][b]=(unsigned char)(start>>(8*b));
    lt48_status status=step(context,2,f);
    if(status==LT48_OK && program && counter(f))status=LT48_RUNTIME_ERROR;
    for(size_t i=0;status==LT48_OK && i<length;i++) {
        uint32_t address=start+(uint32_t)i;
        for(unsigned b=0;b<4;b++)f->buffers[0][b]=(unsigned char)(address>>(8*b));
        if(program)f->buffers[2][0]=input[i];
        status=step(context,program?6:4,f);
        if(status!=LT48_OK)break;
        if(program) {
            if(counter(f)!=((i+1)&127)){status=LT48_RUNTIME_ERROR;break;}
        } else output[i]=f->buffers[1][0];
        *completed=i+1;
    }
    /* Attempt reset even after initialization, capture or page failure. The
     * full owner still performs uncancellable power/rail cleanup separately. */
    *reset_status=step(context,3,f);
    return status==LT48_OK?*reset_status:status;
}

/* Real native transport binding for the stage runner. The full owner must
 * create the powered session and populate variables from the pinned resource. */
lt48_status lt48_ic2_transport_step(void *context,unsigned operation,lt48_ic2_frame *f) {
    lt48_ic2_transport *t=context;
    if(!t || !t->resources || !t->wire || !f || f->config_count>9)return LT48_INVALID_ARGUMENT;
    const lt48_at89_tail_resources *r=t->resources;
    if(!mcu70_ic2_profile(r->definition,r->family,r->pinmap,r->primary_count) || r->variable_count!=6)return LT48_UNSUPPORTED;
    if(operation!=0 && operation!=1 && operation!=2 && operation!=3 && operation!=4 && operation!=6 && operation!=8 && operation!=10)return LT48_UNSUPPORTED;
    lt48_runtime_buffer_seed seeds[31];size_t count=0;
    for(unsigned i=0;i<32;i++)if(i!=4)seeds[count++]=(lt48_runtime_buffer_seed){i,f->buffers[i],32};
    lt48_runtime_nested_wave nested={12,r->wave[12]};
    lt48_runtime_options o={0};o.mode=f->mode;o.use_explicit_buffer_ids=1;
    o.address_buffer=0;o.mode_buffer=4;o.parameter_buffer_id=1;
    o.initial_buffers=seeds;o.initial_buffer_count=count;
    o.configuration=f->config;o.configuration_count=f->config_count;
    o.nested_waves=&nested;o.nested_wave_count=1;o.root_timeout_seconds=10;
    memcpy(o.initial_voltage_centivolts,f->volts,sizeof f->volts);
    f->id_address=UINT32_MAX;f->id_failure=0;
    lt48_runtime_result *diagnostic=NULL;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(t->wire,10000,1);
    if(status==LT48_OK)status=lt48_owner_run_result_internal(t->wire,r->wave[operation],r->pins,
        t->variables,6,&o,NULL,0,f->buffers,f->config,9,&f->config_count,f->volts,&diagnostic);
    if(operation==2 && diagnostic && status==LT48_DEVICE_MISMATCH) {
        lt48_pin_group address;
        if(lt48_pin_map_find_group(r->pins,7,&address)==LT48_OK)for(size_t i=0;i<lt48_runtime_event_count(diagnostic);i++) {
            lt48_runtime_event event;if(lt48_runtime_get_event(diagnostic,i,&event)!=LT48_OK)break;
            if(event.kind==LT48_RUNTIME_RAW_WRITE && event.argument_count==address.pin_count) {
                unsigned value=0,match=1;
                for(size_t bit=0;bit<address.pin_count;bit++) {
                    if((event.arguments[bit]&~1u)!=(address.pins[bit]&~1u))match=0;
                    value|=(event.arguments[bit]&1u)<<bit;
                }
                if(match)f->id_address=value;
            }
            if(event.kind==LT48_RUNTIME_RESULT_LATCH && event.argument_count==2)f->id_failure=(unsigned)event.arguments[0];
        }
    }
    lt48_runtime_result_close(diagnostic);
    return status;
}

static void settings_seed(lt48_ic2_frame *f,const unsigned char *values) {
    const unsigned fields[]={4,5,10,11,6,8,9,11};
    f->config_count=9;f->config[0]=(lt48_runtime_configuration_value){-1,0,1,1};
    for(unsigned i=0;i<8;i++)f->config[i+1]=(lt48_runtime_configuration_value){0,i<4?0:i==4?1:2,fields[i],values?values[i]:UINT32_MAX};
}
static lt48_status settings_values(const lt48_ic2_frame *f,unsigned char out[8]) {
    const unsigned fields[]={4,5,10,11,6,8,9,11};
    for(unsigned i=0;i<8;i++) {
        uint32_t value=UINT32_MAX;unsigned category=i<4?0:i==4?1:2;
        for(size_t j=0;j<f->config_count;j++)if(f->config[j].object_id==0 &&
            f->config[j].category==category && f->config[j].field==fields[i])value=f->config[j].value;
        if(value>(i<4?1u:i==4?3u:255u))return LT48_DEVICE_MISMATCH;
        out[i]=(unsigned char)value;
    }
    return LT48_OK;
}
lt48_status lt48_ic2_settings_sequence(const lt48_at89_tail_resources *r,
    lt48_ic2_step_fn step,void *context,lt48_ic2_frame *f,const unsigned char *desired,
    unsigned char *actual,lt48_status *reset_status) {
    if(!r || !step || !f || !actual || !reset_status)return LT48_INVALID_ARGUMENT;
    *reset_status=LT48_OK;
    if(!mcu70_ic2_profile(r->definition,r->family,r->pinmap,r->primary_count))return LT48_UNSUPPORTED;
    if(desired)for(unsigned i=0;i<5;i++)if(desired[i]>(i<4?1u:3u))return LT48_INVALID_ARGUMENT;
    lt48_status status=LT48_OK;
    if(desired) {
        memset(f->buffers,0,sizeof f->buffers);f->mode=8;settings_seed(f,desired);
        status=step(context,2,f);
        if(status==LT48_OK)status=step(context,8,f);
        *reset_status=step(context,3,f);
        if(status==LT48_OK)status=*reset_status;
    }
    unsigned char observed[8];
    if(status==LT48_OK) {
        memset(f->buffers,0,sizeof f->buffers);f->mode=1;settings_seed(f,NULL);
        status=step(context,2,f); /* DEVINIT calls LOADconfig in mode1. */
        if(status==LT48_OK)status=settings_values(f,observed);
        *reset_status=step(context,3,f);
        if(status==LT48_OK)status=*reset_status;
        if(status==LT48_OK && desired && memcmp(desired,observed,8))status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)memcpy(actual,observed,8);
    return status;
}

lt48_status lt48_ic2_erase_sequence(const lt48_at89_tail_resources *r,
    lt48_ic2_step_fn step,void *context,lt48_ic2_frame *f,
    lt48_ic2_erase_state *state,lt48_status *reset_status) {
    if(!r || !step || !f || !state || !reset_status)return LT48_INVALID_ARGUMENT;
    *reset_status=LT48_OK;
    if(!mcu70_ic2_profile(r->definition,r->family,r->pinmap,r->primary_count))return LT48_UNSUPPORTED;
    /* Never overwrite an outstanding restoration record. */
    if(state->restore_pending)return LT48_SAFETY_REJECTED;
    state->verified=0;state->mismatch_address=UINT32_MAX;state->erase_incomplete=0;
    unsigned char *image=malloc(r->primary_count),actual[8];
    if(!image)return LT48_NO_MEMORY;
    lt48_status status=lt48_ic2_settings_sequence(r,step,context,f,NULL,state->settings,reset_status);
    if(status==LT48_OK) {
        state->settings[4]=0; /* Chip erase intentionally removes protection. */
        memset(f->buffers,0,sizeof f->buffers);f->mode=7;
        status=step(context,2,f);
        if(status==LT48_OK) {
            state->restore_pending=1;state->erase_incomplete=1;
            status=step(context,10,f);
            if(status==LT48_OK)state->erase_incomplete=0;
        }
        *reset_status=step(context,3,f);
        if(status==LT48_OK)status=*reset_status;
    }
    if(status==LT48_OK) {
        status=lt48_ic2_settings_sequence(r,step,context,f,state->settings,actual,reset_status);
        if(status==LT48_OK)state->restore_pending=0;
    }
    if(status==LT48_OK) {
        size_t completed=0;
        status=lt48_ic2_memory_sequence(r,step,context,f,0,0,r->primary_count,NULL,image,&completed,reset_status);
        /* A completed capture is required before certifying blank contents. */
        if(status==LT48_OK)for(size_t i=0;i<r->primary_count;i++) {
            if(image[i]!=255){state->mismatch_address=(uint32_t)i;status=LT48_DEVICE_MISMATCH;break;}
            state->verified=i+1;
        }
    }
    free(image);return status;
}
