#include "source_session.h"
#include "source_host.h"
#include "startup.h"
#include "../../../c/src/internal.h"
#include <stdlib.h>
#include <string.h>

struct lt48_pic_source_session {
    const lt48_pin_map *pins;const void *origin;
    lt48_runtime_nested_wave *callbacks;size_t callback_count;
    lt48_compile_variable *variables;size_t variable_count;
    lt48_runtime_configuration_value *configuration;size_t configuration_count;
    unsigned char *image;size_t image_size;unsigned element_bytes;unsigned char image_free_callbacks[256];
    unsigned char buffers[LT48_RUNTIME_BUFFER_COUNT][32];
    uint32_t voltages[4];size_t max_steps;uint32_t timeout;
    int explicit_configuration_object,configuration_object;
    int explicit_legacy_object,legacy_object;
    lt48_runtime_frame_profile frame;int frame_bound;
    uint8_t address_buffer,mode_buffer,parameter_buffer;
    lt48_applet_address *applets;size_t applet_count;int applets_bound;
    lt48_pic_source_applet_host_roles applet_host;int applet_host_bound;
    uint16_t applet_stream_role;int applet_stream_selected;
};
static void number(unsigned char *p,uint32_t value) {
    for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(value>>(8*i));
}
void lt48_pic_source_session_close_internal(lt48_pic_source_session *s) {
    if(!s)return;
    free(s->applets);free(s->callbacks);free(s->variables);free(s->configuration);free(s->image);free(s);
}
lt48_status lt48_pic_source_session_create_internal(const lt48_pin_map *pins,
    const lt48_compile_variable *variables,size_t variable_count,
    const lt48_runtime_nested_wave *callbacks,size_t callback_count,
    const lt48_runtime_options *options,lt48_pic_source_session **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!pins || !options || !callbacks || !callback_count || callback_count>256 ||
       (variable_count&&!variables) || variable_count>256 ||
       !options->device_image || !options->device_image_size ||
       !options->device_element_bytes || options->device_element_bytes>32 ||
       options->configuration_count>1024 ||
       (options->configuration_count&&!options->configuration) ||
       options->initial_buffer_count>LT48_RUNTIME_BUFFER_COUNT ||
       (options->initial_buffer_count&&!options->initial_buffers))return LT48_INVALID_ARGUMENT;
    if(options->parameter_buffer || options->parameter_size || options->capture_response_bytes ||
       options->capture_response_size || options->require_capture_responses ||
       options->stop_at_missing_capture || options->use_host_bitsort || options->host_gal_pes ||
       options->nested_waves || options->nested_wave_count || options->fuse_image ||
       options->host_epm_legacy || options->host_xc7000_ramp || options->embedded_images ||
       options->embedded_image_count)
        return LT48_UNSUPPORTED;
    if(options->use_explicit_buffer_ids &&
       (options->address_buffer>=LT48_RUNTIME_BUFFER_COUNT || options->mode_buffer>=LT48_RUNTIME_BUFFER_COUNT ||
        options->parameter_buffer_id>=LT48_RUNTIME_BUFFER_COUNT || options->address_buffer==options->mode_buffer))
        return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<callback_count;i++) {
        if(!callbacks[i].compiled)return LT48_INVALID_ARGUMENT;
        for(size_t j=0;j<i;j++)if(callbacks[i].operation==callbacks[j].operation)return LT48_INVALID_ARGUMENT;
    }
    for(size_t i=0;i<variable_count;i++)
        for(size_t j=0;j<i;j++)if(variables[i].variable_id==variables[j].variable_id)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_session *s=calloc(1,sizeof(*s));if(!s)return LT48_NO_MEMORY;
    s->address_buffer=options->use_explicit_buffer_ids?options->address_buffer:2;
    s->mode_buffer=options->use_explicit_buffer_ids?options->mode_buffer:9;
    s->parameter_buffer=options->use_explicit_buffer_ids?options->parameter_buffer_id:5;
    s->pins=pins;s->variable_count=variable_count;s->callback_count=callback_count;
    s->image_size=options->device_image_size;s->element_bytes=options->device_element_bytes;
    s->max_steps=options->max_steps;s->timeout=options->root_timeout_seconds;
    s->configuration_count=options->configuration_count;
    s->explicit_configuration_object=options->use_explicit_direct_configuration_object;
    s->configuration_object=options->direct_configuration_object;
    s->explicit_legacy_object=options->use_explicit_legacy_configuration_object;
    s->legacy_object=options->legacy_configuration_object;
    s->variables=malloc((variable_count?variable_count:1)*sizeof(*variables));
    s->callbacks=malloc(callback_count*sizeof(*callbacks));s->image=malloc(s->image_size);
    s->configuration=malloc((s->configuration_count?s->configuration_count:1)*sizeof(*s->configuration));
    if(!s->variables || !s->callbacks || !s->image || !s->configuration) {
        lt48_pic_source_session_close_internal(s);return LT48_NO_MEMORY;
    }
    if(variable_count)memcpy(s->variables,variables,variable_count*sizeof(*variables));
    memcpy(s->callbacks,callbacks,callback_count*sizeof(*callbacks));
    memcpy(s->image,options->device_image,s->image_size);
    if(s->configuration_count)memcpy(s->configuration,options->configuration,s->configuration_count*sizeof(*s->configuration));
    memcpy(s->voltages,options->initial_voltage_centivolts,sizeof(s->voltages));
    unsigned seen[LT48_RUNTIME_BUFFER_COUNT]={0};
    for(size_t i=0;i<options->initial_buffer_count;i++) {
        const lt48_runtime_buffer_seed *b=options->initial_buffers+i;
        if(b->buffer>=LT48_RUNTIME_BUFFER_COUNT || seen[b->buffer]++ || b->size>32 || (b->size&&!b->data)) {
            lt48_pic_source_session_close_internal(s);return LT48_INVALID_ARGUMENT;
        }
        if(b->size)memcpy(s->buffers[b->buffer],b->data,b->size);
    }
    *out=s;return LT48_OK;
}
/* Explicit source-owner opt-in. Unknown image dependencies fail at the
 * interpreter's normal image bounds check; no borrowed writable image or
 * speculative canonical state is exposed. Replacements retain full images. */
lt48_status lt48_pic_source_session_omit_callback_image_internal(lt48_pic_source_session *s,uint16_t role,int omit){
 if(!s||role>=256||omit<0||omit>1)return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<s->callback_count;i++)if(s->callbacks[i].operation==role){s->image_free_callbacks[role]=(unsigned char)omit;return LT48_OK;}
 return LT48_UNSUPPORTED;
}
static lt48_status source_invoke(lt48_pic_source_session *s,
    uint16_t role,uint32_t mode,uint32_t address,unsigned char *expected,unsigned char *observed,
    size_t width,const lt48_compiled_wave *replacement,uint32_t timeout,
    lt48_pic_source_runner runner,void *context) {
    if(!s || !runner || width>32 || ((expected||observed) && width!=s->element_bytes))return LT48_INVALID_ARGUMENT;
    const lt48_compiled_wave *wave=NULL;
    for(size_t i=0;i<s->callback_count;i++)if(s->callbacks[i].operation==role)wave=s->callbacks[i].compiled;
    if(!wave)return LT48_UNSUPPORTED;
    if(replacement)wave=replacement;
    unsigned char buffers[LT48_RUNTIME_BUFFER_COUNT][32];memcpy(buffers,s->buffers,sizeof(buffers));
    number(buffers[s->address_buffer],address);number(buffers[s->mode_buffer],mode);
    if(expected)memcpy(buffers[6],expected,width);
    if(observed)memcpy(buffers[5],observed,width);
    lt48_runtime_buffer_seed seeds[LT48_RUNTIME_BUFFER_COUNT];
    for(unsigned i=0;i<LT48_RUNTIME_BUFFER_COUNT;i++)seeds[i]=(lt48_runtime_buffer_seed){i,buffers[i],32};
    lt48_runtime_options opts={0};opts.mode=mode;opts.address=address;opts.use_explicit_buffer_ids=1;
    opts.address_buffer=s->address_buffer;opts.mode_buffer=s->mode_buffer;opts.parameter_buffer_id=s->parameter_buffer;
    opts.initial_buffers=seeds;opts.initial_buffer_count=LT48_RUNTIME_BUFFER_COUNT;
    opts.nested_waves=s->callbacks;opts.nested_wave_count=s->callback_count;
    int image_free=!replacement&&role<256&&s->image_free_callbacks[role];
    opts.device_image=image_free?NULL:s->image;opts.device_image_size=image_free?0:s->image_size;opts.device_element_bytes=s->element_bytes;
    opts.configuration=s->configuration;opts.configuration_count=s->configuration_count;
    opts.use_explicit_direct_configuration_object=s->explicit_configuration_object;
    opts.direct_configuration_object=s->configuration_object;
    opts.use_explicit_legacy_configuration_object=s->explicit_legacy_object;
    opts.legacy_configuration_object=s->legacy_object;
    opts.max_steps=s->max_steps;opts.root_timeout_seconds=timeout;
    memcpy(opts.initial_voltage_centivolts,s->voltages,sizeof(s->voltages));
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    lt48_status st=lt48_runtime_machine_create(wave,s->pins,s->variables,s->variable_count,&opts,&machine);
    if(st==LT48_OK)st=runner(context,machine,&result);
    if(st==LT48_OK && (!result || lt48_runtime_machine_get_state(machine)!=LT48_RUNTIME_MACHINE_FINISHED))st=LT48_RUNTIME_ERROR;
    if(st!=LT48_OK)goto done;
    lt48_status outcome=LT48_OK;
    for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
        lt48_runtime_event e;st=lt48_runtime_get_event(result,i,&e);if(st!=LT48_OK)goto done;
        if(e.kind==LT48_RUNTIME_EXECUTE_WAVE) {
            if(e.argument_count!=1 || e.arguments[0]>UINT16_MAX){st=LT48_PROTOCOL_ERROR;goto done;}
            int found=0;
            for(size_t j=0;j<s->applet_count;j++)if(s->applets[j].wave==e.arguments[0])found=1;
            if(!found){st=LT48_UNSUPPORTED;goto done;}
        }
        if(e.kind==LT48_RUNTIME_RESULT_LATCH || (e.kind==LT48_RUNTIME_RETURN_STATUS && e.argument_count && !e.arguments[0]))
            outcome=LT48_DEVICE_MISMATCH;
    }
    const unsigned char *image;size_t span;
    st=lt48_runtime_get_image(result,&image,&span);if(st!=LT48_OK)goto done;
    if(span!=(image_free?0:s->image_size)){st=LT48_RUNTIME_ERROR;goto done;}
    for(unsigned i=0;i<LT48_RUNTIME_BUFFER_COUNT;i++) {
        const unsigned char *p;size_t n;st=lt48_runtime_get_buffer(result,i,&p,&n);if(st!=LT48_OK)goto done;
        if(n!=32){st=LT48_RUNTIME_ERROR;goto done;}memcpy(buffers[i],p,32);
    }
    lt48_compile_variable variables[256];uint32_t voltages[4];
    for(size_t i=0;i<s->variable_count;i++) {
        st=lt48_runtime_get_variable(result,s->variables[i].variable_id,variables+i);if(st!=LT48_OK)goto done;
    }
    for(unsigned i=0;i<4;i++){st=lt48_runtime_get_voltage(result,i,voltages+i);if(st!=LT48_OK)goto done;}
    const lt48_runtime_configuration_value *configuration;size_t count;
    st=lt48_runtime_get_configuration(result,&configuration,&count);if(st!=LT48_OK)goto done;
    if(count>1024){st=LT48_FORMAT_ERROR;goto done;}
    lt48_runtime_configuration_value *copy=malloc((count?count:1)*sizeof(*copy));
    if(!copy){st=LT48_NO_MEMORY;goto done;}if(count)memcpy(copy,configuration,count*sizeof(*copy));
    if(!image_free)memcpy(s->image,image,span);
    memcpy(s->buffers,buffers,sizeof(buffers));
    if(s->variable_count)memcpy(s->variables,variables,s->variable_count*sizeof(*variables));
    memcpy(s->voltages,voltages,sizeof(voltages));free(s->configuration);s->configuration=copy;s->configuration_count=count;
    st=outcome;
    if(st==LT48_OK){if(expected)memcpy(expected,buffers[6],width);if(observed)memcpy(observed,buffers[5],width);}
done:
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return st;
}
lt48_status lt48_pic_source_session_invoke_internal(lt48_pic_source_session *s,
    uint16_t role,uint32_t mode,uint32_t address,unsigned char *expected,unsigned char *observed,
    size_t width,lt48_pic_source_runner runner,void *context) {
    return source_invoke(s,role,mode,address,expected,observed,width,NULL,s?s->timeout:0,runner,context);
}
lt48_status lt48_pic_source_session_invoke_wave_internal(lt48_pic_source_session *s,
    uint16_t role,const lt48_compiled_wave *wave,uint32_t mode,uint32_t address,
    uint32_t timeout,lt48_pic_source_runner runner,void *context) {
    return source_invoke(s,role,mode,address,NULL,NULL,0,wave,timeout,runner,context);
}
int lt48_pic_source_session_has_callback_internal(const lt48_pic_source_session *s,uint16_t role) {
    if(!s)return 0;
    for(size_t i=0;i<s->callback_count;i++)if(s->callbacks[i].operation==role)return 1;
    return 0;
}
lt48_status lt48_pic_source_session_invoke_current_internal(lt48_pic_source_session *s,
    uint16_t role,uint32_t timeout,lt48_pic_source_runner runner,void *context) {
    if(!s||!runner||!timeout)return LT48_INVALID_ARGUMENT;
    uint32_t mode=0,address=0;
    for(unsigned i=0;i<4;i++) {
        mode|=(uint32_t)s->buffers[s->mode_buffer][i]<<(8*i);
        address|=(uint32_t)s->buffers[s->address_buffer][i]<<(8*i);
    }
    return source_invoke(s,role,mode,address,NULL,NULL,0,NULL,timeout,runner,context);
}
lt48_status lt48_pic_source_session_applet_callback_internal(lt48_pic_source_session *s,
    uint16_t role,uint16_t mode,uint32_t address,uint32_t capacity,uint8_t size_buffer,
    lt48_pic_source_runner runner,void *context) {
    if(!s||!runner)return LT48_INVALID_ARGUMENT;
    if(!lt48_pic_source_session_has_callback_internal(s,role))return LT48_UNSUPPORTED;
    lt48_status status=lt48_pic_source_session_applet_mode_internal(s,mode,0,address,capacity,size_buffer);
    if(status!=LT48_OK)return status;
    return lt48_pic_source_session_invoke_current_internal(s,role,10,runner,context);
}
lt48_status lt48_pic_source_session_state_internal(lt48_pic_source_session *s,lt48_pic_source_state *state) {
    if(!s || !state)return LT48_INVALID_ARGUMENT;
    *state=(lt48_pic_source_state){s->variables,s->variable_count,s->configuration,s->configuration_count,
        s->image,s->image_size,(const unsigned char (*)[32])s->buffers,{0},s->applet_stream_role,s->applet_stream_selected};
    memcpy(state->voltages,s->voltages,sizeof(s->voltages));return LT48_OK;
}
lt48_status lt48_pic_source_session_restore_internal(lt48_pic_source_session *s,const lt48_pic_source_state *state) {
    if(!s || !state || state->variable_count!=s->variable_count ||
       (state->variable_count && !state->variables) || state->configuration_count>1024 ||
       (state->configuration_count && !state->configuration) || !state->image ||
       state->image_size!=s->image_size || !state->buffers)return LT48_INVALID_ARGUMENT;
    if((state->applet_stream_selected!=0&&state->applet_stream_selected!=1)||
       (state->applet_stream_selected && (!s->applet_host_bound ||
        (state->applet_stream_role!=s->applet_host.read&&state->applet_stream_role!=s->applet_host.program&&
         state->applet_stream_role!=s->applet_host.verify&&state->applet_stream_role!=s->applet_host.blank))))
        return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<s->variable_count;i++) {
        const lt48_compile_variable *a=s->variables+i,*b=state->variables+i;
        if(a->variable_id!=b->variable_id || a->kind!=b->kind ||
           (a->kind==0 && a->channel!=b->channel))return LT48_INVALID_ARGUMENT;
    }
    for(size_t i=0;i<state->configuration_count;i++)for(size_t j=0;j<i;j++) {
        const lt48_runtime_configuration_value *a=state->configuration+i,*b=state->configuration+j;
        if(a->object_id==b->object_id && a->category==b->category && a->field==b->field)
            return LT48_INVALID_ARGUMENT;
    }
    /* Copy before releasing the old array: the supplied view may alias it. */
    lt48_runtime_configuration_value *configuration=malloc((state->configuration_count?state->configuration_count:1)*sizeof(*configuration));
    if(!configuration)return LT48_NO_MEMORY;
    if(state->configuration_count)memcpy(configuration,state->configuration,state->configuration_count*sizeof(*configuration));
    if(s->variable_count)memmove(s->variables,state->variables,s->variable_count*sizeof(*s->variables));
    memmove(s->image,state->image,s->image_size);memmove(s->buffers,state->buffers,sizeof(s->buffers));
    memcpy(s->voltages,state->voltages,sizeof(s->voltages));
    s->applet_stream_role=state->applet_stream_role;s->applet_stream_selected=state->applet_stream_selected;
    free(s->configuration);s->configuration=configuration;s->configuration_count=state->configuration_count;
    return LT48_OK;
}
lt48_status lt48_pic_source_session_image_internal(lt48_pic_source_session *s,const unsigned char **image,size_t *span) {
    if(!s || !image || !span)return LT48_INVALID_ARGUMENT;
    *image=s->image;*span=s->image_size;return LT48_OK;
}
lt48_status lt48_pic_source_session_clone_internal(const lt48_pic_source_session *s,
    lt48_pic_source_session **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;if(!s)return LT48_INVALID_ARGUMENT;
    lt48_runtime_buffer_seed seeds[LT48_RUNTIME_BUFFER_COUNT];
    for(unsigned i=0;i<LT48_RUNTIME_BUFFER_COUNT;i++)seeds[i]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_options options={0};options.device_image=s->image;options.device_image_size=s->image_size;
    options.device_element_bytes=s->element_bytes;options.configuration=s->configuration;
    options.configuration_count=s->configuration_count;options.initial_buffers=seeds;
    options.use_explicit_buffer_ids=1;options.address_buffer=s->address_buffer;
    options.mode_buffer=s->mode_buffer;options.parameter_buffer_id=s->parameter_buffer;
    options.initial_buffer_count=LT48_RUNTIME_BUFFER_COUNT;options.max_steps=s->max_steps;
    options.root_timeout_seconds=s->timeout;options.use_explicit_direct_configuration_object=s->explicit_configuration_object;
    options.direct_configuration_object=s->configuration_object;
    options.use_explicit_legacy_configuration_object=s->explicit_legacy_object;
    options.legacy_configuration_object=s->legacy_object;
    memcpy(options.initial_voltage_centivolts,s->voltages,sizeof(s->voltages));
    lt48_status status=lt48_pic_source_session_create_internal(s->pins,s->variables,s->variable_count,
        s->callbacks,s->callback_count,&options,out);
    if(status==LT48_OK){
        memcpy((*out)->image_free_callbacks,s->image_free_callbacks,sizeof s->image_free_callbacks);
        (*out)->origin=s->origin;(*out)->frame=s->frame;(*out)->frame_bound=s->frame_bound;
        (*out)->applet_host=s->applet_host;(*out)->applet_host_bound=s->applet_host_bound;
        (*out)->applet_stream_role=s->applet_stream_role;(*out)->applet_stream_selected=s->applet_stream_selected;
        if(s->applets_bound)status=lt48_pic_source_session_bind_applets_internal(*out,s->applets,s->applet_count);
        if(status!=LT48_OK){lt48_pic_source_session_close_internal(*out);*out=NULL;}
    }
    return status;
}
lt48_status lt48_pic_source_session_bind_frame_internal(lt48_pic_source_session *s,const lt48_runtime_frame_profile *frame) {
    if(!s||!frame||!frame->buffer_count||frame->buffer_count>LT48_RUNTIME_BUFFER_COUNT||
       frame->mode_buffer!=s->mode_buffer||s->mode_buffer>=frame->buffer_count||
       s->address_buffer>=frame->buffer_count||s->parameter_buffer>=frame->buffer_count)
        return LT48_INVALID_ARGUMENT;
    if(frame->buffer_count<32 && (frame->reset_mask&~((UINT32_C(1)<<frame->buffer_count)-1u)))
        return LT48_INVALID_ARGUMENT;
    if(s->frame_bound && (s->frame.buffer_count!=frame->buffer_count||s->frame.mode_buffer!=frame->mode_buffer||
        s->frame.reset_mask!=frame->reset_mask))return LT48_SAFETY_REJECTED;
    s->frame=*frame;s->frame_bound=1;return LT48_OK;
}
lt48_status lt48_pic_source_session_bind_applets_internal(lt48_pic_source_session *s,
    const lt48_applet_address *applets,size_t count) {
    if(!s||count>256||(count&&!applets))return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<count;i++) {
        for(size_t j=0;j<i;j++)if(applets[j].wave==applets[i].wave)return LT48_INVALID_ARGUMENT;
        for(size_t j=0;j<s->callback_count;j++)if(s->callbacks[j].operation==applets[i].wave)return LT48_INVALID_ARGUMENT;
    }
    if(s->applets_bound) {
        if(count!=s->applet_count)return LT48_SAFETY_REJECTED;
        for(size_t i=0;i<count;i++)if(applets[i].wave!=s->applets[i].wave||applets[i].address!=s->applets[i].address)
            return LT48_SAFETY_REJECTED;
        return LT48_OK;
    }
    lt48_applet_address *copy=count?malloc(count*sizeof(*copy)):NULL;
    if(count&&!copy)return LT48_NO_MEMORY;
    if(count)memcpy(copy,applets,count*sizeof(*copy));
    s->applets=copy;s->applet_count=count;s->applets_bound=1;return LT48_OK;
}
lt48_status lt48_pic_source_session_applets_internal(const lt48_pic_source_session *s,
    const lt48_applet_address **applets,size_t *count) {
    if(!s||!applets||!count)return LT48_INVALID_ARGUMENT;
    *applets=s->applets;*count=s->applet_count;return LT48_OK;
}
lt48_status lt48_pic_source_session_bind_applet_host_internal(lt48_pic_source_session *s,
    const lt48_pic_source_applet_host_roles *roles) {
    if(!s||!roles)return LT48_INVALID_ARGUMENT;
    if(s->applet_host_bound && (s->applet_host.read!=roles->read||s->applet_host.program!=roles->program||
       s->applet_host.verify!=roles->verify||s->applet_host.blank!=roles->blank))return LT48_SAFETY_REJECTED;
    if(!s->applet_host_bound) {
        s->applet_stream_role=roles->read;s->applet_stream_selected=1;
    }
    s->applet_host=*roles;s->applet_host_bound=1;return LT48_OK;
}
lt48_status lt48_pic_source_session_applet_stream_role_internal(const lt48_pic_source_session *s,uint16_t *role) {
    if(!s||!role)return LT48_INVALID_ARGUMENT;
    if(!s->applet_host_bound||!s->applet_stream_selected)return LT48_UNSUPPORTED;
    *role=s->applet_stream_role;return LT48_OK;
}
lt48_status lt48_pic_source_session_applet_invoke_selected_internal(lt48_pic_source_session *s,
    uint32_t timeout,lt48_pic_source_runner runner,void *context) {
    uint16_t role;lt48_status status=lt48_pic_source_session_applet_stream_role_internal(s,&role);
    if(status!=LT48_OK)return status;
    return lt48_pic_source_session_invoke_current_internal(s,role,timeout,runner,context);
}
lt48_status lt48_pic_source_session_applet_span_internal(lt48_pic_source_session *s,
    const lt48_pic_map_span *span,uint8_t image_buffer,uint8_t increment_buffer) {
    if(!s||!span||!s->frame_bound||!span->length||!span->increment||
       image_buffer>=s->frame.buffer_count||increment_buffer>=s->frame.buffer_count||
       image_buffer==increment_buffer||image_buffer==s->address_buffer||image_buffer==s->mode_buffer||
       increment_buffer==s->address_buffer||increment_buffer==s->mode_buffer||
       span->image_offset>s->image_size||span->length>s->image_size-span->image_offset)
        return LT48_INVALID_ARGUMENT;
    number(s->buffers[s->address_buffer],span->device_address);
    number(s->buffers[image_buffer],span->image_offset);
    number(s->buffers[increment_buffer],span->increment);
    return LT48_OK;
}
lt48_status lt48_pic_source_session_applet_mode_internal(lt48_pic_source_session *s,
    uint16_t mode,int reset,uint32_t address,uint32_t capacity,uint8_t chip_size_buffer) {
    if(!s||!s->frame_bound||(reset!=0&&reset!=1)||chip_size_buffer>=s->frame.buffer_count||
       chip_size_buffer==s->address_buffer||chip_size_buffer==s->mode_buffer)return LT48_INVALID_ARGUMENT;
    lt48_status status=lt48_runtime_frame_set_mode(s->buffers,s->frame.buffer_count,s->frame.reset_mask,
        s->mode_buffer,mode,!reset,reset?0:UINT32_C(0x200));
    if(status==LT48_OK){
        number(s->buffers[chip_size_buffer],capacity);number(s->buffers[s->address_buffer],address);
        if(s->applet_host_bound) {
            s->applet_stream_role=(mode==2||mode==7)?s->applet_host.program:
                (mode==4||mode==5)?s->applet_host.verify:mode==11?s->applet_host.blank:s->applet_host.read;
            s->applet_stream_selected=1;
        }
    }
    return status;
}
static lt48_status configuration_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **out) {
    (void)context;lt48_runtime_machine_state state;
    lt48_status status=lt48_runtime_machine_advance(machine,&state);if(status!=LT48_OK)return status;
    if(state!=LT48_RUNTIME_MACHINE_FINISHED)return LT48_UNSUPPORTED;
    const lt48_runtime_result *result=lt48_runtime_machine_result(machine);
    for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
        lt48_runtime_event event;status=lt48_runtime_get_event(result,i,&event);if(status!=LT48_OK)return status;
        switch(event.kind) {
        case LT48_RUNTIME_EVALUATE_VARIABLE:
        case LT48_RUNTIME_BUFFER_ARITHMETIC:
        case LT48_RUNTIME_BUFFER_BIT_COPY:
        case LT48_RUNTIME_BUFFER_MEMMOVE:
        case LT48_RUNTIME_BUFFER_BYTE_OPERATION:
        case LT48_RUNTIME_BUFFER_FILL:
        case LT48_RUNTIME_WAVE_WATCHDOG:break;
        case LT48_RUNTIME_RETURN_STATUS:
            if(!event.argument_count||!event.arguments[0])return LT48_DEVICE_MISMATCH;
            break;
        default:return LT48_UNSUPPORTED;
        }
    }
    return lt48_runtime_machine_take_result(machine,out);
}
lt48_status lt48_pic_source_session_configuration_internal(lt48_pic_source_session *s,
    uint16_t pre,uint16_t post,uint16_t mode,uint32_t address,
    const lt48_pic_composition_field *fields,size_t field_count,
    const lt48_runtime_configuration_value *edits,size_t edit_count) {
    if(!s||(field_count&&!fields)||(edit_count&&!edits)||field_count>1024||edit_count>1024)
        return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<edit_count;i++) {
        const lt48_runtime_configuration_value *e=edits+i;unsigned found=0;
        for(size_t j=0;j<i;j++)if(edits[j].object_id==e->object_id&&edits[j].category==e->category&&edits[j].field==e->field)
            return LT48_INVALID_ARGUMENT;
        for(size_t j=0;j<field_count;j++)if(lt48_pic_composition_field_same(e->object_id,e->category,e->field,fields+j)) {
            if(++found>1||fields[j].minimum>fields[j].maximum||e->value<fields[j].minimum||e->value>fields[j].maximum)
                return LT48_INVALID_ARGUMENT;
        }
        if(!found)return LT48_UNSUPPORTED;
    }
    lt48_pic_source_session *stage=NULL;lt48_status status=lt48_pic_source_session_clone_internal(s,&stage);
    if(status!=LT48_OK)return status;
    status=lt48_pic_source_session_invoke_internal(stage,pre,mode,address,NULL,NULL,0,configuration_runner,NULL);
    if(status!=LT48_OK)goto done;
    for(size_t i=0;i<edit_count;i++) {
        const lt48_runtime_configuration_value *e=edits+i;size_t found=SIZE_MAX;
        for(size_t j=0;j<stage->configuration_count;j++) {
            lt48_runtime_configuration_value *v=stage->configuration+j;
            if(v->object_id==e->object_id&&v->category==e->category&&v->field==e->field) {
                if(found!=SIZE_MAX){status=LT48_FORMAT_ERROR;goto done;}
                found=j;
            }
        }
        if(found==SIZE_MAX) {
            /* An explicit source-schema edit addresses an existing host UI
             * field even when conditional PreConfig leaves it untouched.
             * Domain/identity validation above is required; unknown edits
             * still fail before cloning or changing the public session. */
            if(stage->configuration_count==1024){status=LT48_INVALID_ARGUMENT;goto done;}
            lt48_runtime_configuration_value *values=realloc(stage->configuration,
                (stage->configuration_count+1)*sizeof(*values));
            if(!values){status=LT48_NO_MEMORY;goto done;}
            stage->configuration=values;found=stage->configuration_count++;
            values[found]=*e;
        }
        stage->configuration[found].value=e->value;
    }
    status=lt48_pic_source_session_invoke_internal(stage,post,mode,address,NULL,NULL,0,configuration_runner,NULL);
    if(status==LT48_OK) {
        lt48_pic_source_state state;status=lt48_pic_source_session_state_internal(stage,&state);
        if(status==LT48_OK)status=lt48_pic_source_session_restore_internal(s,&state);
    }
done:lt48_pic_source_session_close_internal(stage);return status;
}
lt48_status lt48_pic_source_session_configuration_complete_internal(
    lt48_pic_source_session *s,uint16_t pre,uint16_t post,uint16_t mode,uint32_t address,
    lt48_pic_configuration_completion completion,void *context) {
    if(!s||!completion)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_session *stage=NULL;
    lt48_status status=lt48_pic_source_session_clone_internal(s,&stage);
    if(status==LT48_OK)status=lt48_pic_source_session_invoke_internal(stage,pre,mode,address,NULL,NULL,0,configuration_runner,NULL);
    if(status==LT48_OK)status=completion(context,stage);
    if(status==LT48_OK)status=lt48_pic_source_session_invoke_internal(stage,post,mode,address,NULL,NULL,0,configuration_runner,NULL);
    if(status==LT48_OK) {
        lt48_pic_source_state state;status=lt48_pic_source_session_state_internal(stage,&state);
        if(status==LT48_OK)status=lt48_pic_source_session_restore_internal(s,&state);
    }
    lt48_pic_source_session_close_internal(stage);return status;
}
lt48_status lt48_pic_source_session_buffer_internal(lt48_pic_source_session *s,unsigned id,const unsigned char **data) {
    if(!s || !data || id>=LT48_RUNTIME_BUFFER_COUNT)return LT48_INVALID_ARGUMENT;
    *data=s->buffers[id];return LT48_OK;
}
lt48_status lt48_pic_source_session_voltage_value_internal(lt48_pic_source_session *s,uint16_t id,uint32_t value) {
    if(!s||value>UINT16_MAX)return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<s->variable_count;i++)if(s->variables[i].variable_id==id) {
        if(s->variables[i].kind!=0)return LT48_UNSUPPORTED;
        s->variables[i].value=(uint16_t)value;return LT48_OK;
    }
    return LT48_NOT_FOUND;
}
lt48_status lt48_pic_source_session_variable_internal(lt48_pic_source_session *s,
    uint16_t id,lt48_compile_variable *variable) {
    if(!s || !variable)return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<s->variable_count;i++)if(s->variables[i].variable_id==id) {
        *variable=s->variables[i];return LT48_OK;
    }
    return LT48_NOT_FOUND;
}

typedef struct {
    lt48_pic_source_session *session;
    const lt48_pic_source_range_options *options;
    uint16_t mode;
} source_range;
static lt48_status source_range_invoke(void *opaque,lt48_pic_range_phase phase,
    uint16_t role,uint32_t address,unsigned char *expected,unsigned char *observed,size_t width) {
    source_range *range=opaque;
    lt48_pic_source_session *s=range->session;
    const lt48_pic_source_range_options *o=range->options;
    if(phase==LT48_PIC_RANGE_MODE) {
        range->mode=role;
        return lt48_runtime_frame_set_mode(s->buffers,s->frame_bound?s->frame.buffer_count:LT48_RUNTIME_BUFFER_COUNT,
            s->frame_bound?s->frame.reset_mask:o->reset_mask,s->mode_buffer,role,
            s->frame_bound?0:o->started,s->frame_bound?0:o->option_flags);
    }
    if(phase==LT48_PIC_RANGE_START)
        return o->start(o->context,s,range->mode,address);
    /* Pre/Post share the host's DataOut/DataIn too. Their transformations
     * must flow back to the worker, including blank/verify masks. */
    return lt48_pic_source_session_invoke_internal(s,role,range->mode,address,
        expected,observed,width,o->runner,o->context);
}
static lt48_status source_range_image(void *opaque,int write,uint32_t offset,
    unsigned char *data,size_t count) {
    source_range *range=opaque;
    if(offset>range->options->logical_image_size ||
       count>range->options->logical_image_size-offset)return LT48_INVALID_ARGUMENT;
    if(write)memcpy(range->session->image+offset,data,count);
    else memcpy(data,range->session->image+offset,count);
    return LT48_OK;
}
static int source_range_cancelled(void *opaque) {
    source_range *range=opaque;
    return range->options->cancelled && range->options->cancelled(range->options->context);
}
lt48_status lt48_pic_source_session_range_internal(lt48_pic_source_session *s,
    const lt48_pic_range_contract *contract,uint32_t first,uint32_t last,
    const lt48_pic_source_range_options *o,lt48_pic_range_execution_result *result) {
    if(result)*result=(lt48_pic_range_execution_result){0,UINT32_MAX};
    if(!s || !contract || !o || !result || !o->runner ||
       (o->started!=0 && o->started!=1) || (!o->started && !o->start) ||
       !o->logical_image_size || o->logical_image_size>s->image_size ||
       !contract->element_bits || contract->element_bits>256 ||
       (contract->element_bits+7u)/8u!=s->element_bytes ||
       o->logical_image_size%s->element_bytes ||
       (uint64_t)(first>last?first:last)*s->element_bytes+s->element_bytes>o->logical_image_size)
        return LT48_INVALID_ARGUMENT;
    if(!(contract->mode==1 || contract->mode==2 || contract->mode==3 ||
         contract->mode==4 || contract->mode==5 || contract->mode==11))return LT48_INVALID_ARGUMENT;
    lt48_pic_range_plan checked_plan;
    if(!lt48_pic_range_prepare(first,last,s->element_bytes,&checked_plan))return LT48_INVALID_ARGUMENT;
    /* Resolve the whole requested route before mode reset or startup. */
    uint16_t roles[]={contract->pre_role,contract->element_role,contract->post_role};
    for(unsigned j=0;j<3;j++) {
        if(roles[j]==LT48_PIC_ABSENT_ROLE) {
            if(j==1)return LT48_INVALID_ARGUMENT;
            continue;
        }
        int found=0;
        for(size_t i=0;i<s->callback_count;i++)if(s->callbacks[i].operation==roles[j])found=1;
        if(!found)return LT48_UNSUPPORTED;
    }
    source_range range={s,o,0};
    return lt48_pic_range_execute(contract,first,last,o->started,source_range_invoke,
        source_range_image,source_range_cancelled,&range,result);
}
static lt48_status source_transaction_invoke(void *opaque,lt48_pic_transaction_phase phase,
    uint16_t role,uint16_t timeout) {
    source_range *range=opaque;
    if(phase==LT48_PIC_TX_MODE)
        return source_range_invoke(range,LT48_PIC_RANGE_MODE,role,0,NULL,NULL,0);
    if(phase==LT48_PIC_TX_START)
        return source_range_invoke(range,LT48_PIC_RANGE_START,role,0,NULL,NULL,0);
    uint32_t saved=range->session->timeout;
    range->session->timeout=timeout;
    lt48_status st=lt48_pic_source_session_invoke_internal(range->session,role,range->mode,0,
        NULL,NULL,0,range->options->runner,range->options->context);
    range->session->timeout=saved;
    return st;
}
lt48_status lt48_pic_source_session_transaction_internal(lt48_pic_source_session *s,
    const lt48_pic_transaction_plan *plan,const lt48_pic_source_range_options *o) {
    if(!s || !plan || !o || !o->runner || (o->started!=0 && o->started!=1) ||
       (!o->started && !o->start) || !plan->mode || plan->action.role==LT48_PIC_ABSENT_ROLE)
        return LT48_INVALID_ARGUMENT;
    const lt48_pic_transaction_step steps[]={plan->pre,plan->action,plan->post};
    for(unsigned j=0;j<3;j++) {
        if(steps[j].role==LT48_PIC_ABSENT_ROLE)continue;
        if(!steps[j].timeout_seconds)return LT48_INVALID_ARGUMENT;
        int found=0;
        for(size_t i=0;i<s->callback_count;i++)if(s->callbacks[i].operation==steps[j].role)found=1;
        if(!found)return LT48_UNSUPPORTED;
    }
    source_range transaction={s,o,0};
    return lt48_pic_transaction_run(plan,o->started,source_transaction_invoke,&transaction);
}
lt48_status lt48_pic_source_voltage_selector_internal(uint32_t voltage,uint8_t *selector) {
    if(!selector || voltage>INT32_MAX)return LT48_INVALID_ARGUMENT;
    *selector=voltage>400?15:7;
    return LT48_OK;
}
static int source_has_role(const lt48_pic_source_session *s,uint16_t role) {
    for(size_t i=0;i<s->callback_count;i++)if(s->callbacks[i].operation==role)return 1;
    return 0;
}
static lt48_status source_lifecycle_call(lt48_pic_source_session *s,
    const lt48_pic_source_lifecycle *l,uint16_t role,uint16_t mode,uint32_t address) {
    uint32_t saved=s->timeout;s->timeout=l->timeout_seconds;
    lt48_status st=lt48_pic_source_session_invoke_internal(s,role,mode,address,
        NULL,NULL,0,l->runner,l->context);
    s->timeout=saved;return st;
}
lt48_status lt48_pic_source_session_begin_internal(lt48_pic_source_session *s,
    const lt48_pic_source_lifecycle *l,uint16_t mode,uint32_t address) {
    if(!s || !l || !l->runner || !l->selector_write || !l->ready || !l->timeout_seconds ||
       !l->minimum_centivolts || l->minimum_centivolts>l->maximum_centivolts ||
       l->voltage_centivolts<l->minimum_centivolts || l->voltage_centivolts>l->maximum_centivolts)
        return LT48_INVALID_ARGUMENT;
    uint8_t selector;lt48_status st=lt48_pic_source_voltage_selector_internal(l->voltage_centivolts,&selector);
    if(st!=LT48_OK)return st;
    /* Resolve startup AND cleanup before sending a selector or changing VDD. */
    if(!source_has_role(s,l->poweron_role) || !source_has_role(s,l->init_role) ||
       !source_has_role(s,l->reset_role) || !source_has_role(s,l->poweroff_role))return LT48_UNSUPPORTED;
    lt48_compile_variable *voltage=NULL;
    for(size_t i=0;i<s->variable_count;i++)if(s->variables[i].variable_id==l->voltage_variable)voltage=s->variables+i;
    if(!voltage || voltage->kind!=0 || !voltage->enabled)return LT48_UNSUPPORTED;
    if(l->voltage_centivolts>UINT16_MAX)return LT48_INVALID_ARGUMENT;
    /* Voltage kind 0 stores centivolts in value; channel is the DAC identity. */
    voltage->value=(uint16_t)l->voltage_centivolts;
    st=l->selector_write(l->context,selector);
    if(st==LT48_OK)st=source_lifecycle_call(s,l,l->poweron_role,mode,address);
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=l->ready(l->context);
    if(st==LT48_OK)st=source_lifecycle_call(s,l,l->init_role,mode,address);
    return st;
}
lt48_status lt48_pic_source_session_end_internal(lt48_pic_source_session *s,
    const lt48_pic_source_lifecycle *l,uint16_t mode,lt48_status operation,lt48_status *cleanup) {
    if(!s || !l || !l->runner || !l->timeout_seconds || !cleanup)return LT48_INVALID_ARGUMENT;
    lt48_status first=source_lifecycle_call(s,l,l->reset_role,mode,0);
    lt48_status off=source_lifecycle_call(s,l,l->poweroff_role,mode,0);
    if(first==LT48_OK)first=off;
    *cleanup=first;
    return operation==LT48_OK?first:operation;
}
typedef struct {
    lt48_pic_source_session *session;
    const lt48_pic_source_host *host;
    uint16_t mode;
    uint32_t address;
} source_start_context;
static lt48_status source_host_callback(source_start_context *c,uint16_t role) {
    return lt48_pic_source_session_invoke_wave_internal(c->session,role,NULL,c->mode,c->address,
        LT48_PIC_SOURCE_POWER_TIMEOUT_SECONDS,c->host->runner,c->host->context);
}
static lt48_status source_start_invoke(void *context,lt48_pic_start_phase phase,
    uint16_t role,uint16_t *controller) {
    source_start_context *c=context;const lt48_pic_source_host *h=c->host;
    if(phase==LT48_PIC_START_POWERON)return source_host_callback(c,role);
    if(phase==LT48_PIC_START_STATUS || phase==LT48_PIC_START_SETTLE)
        return h->status(h->context,phase==LT48_PIC_START_SETTLE,controller);
    static const unsigned char arm[]={0x56,0x0a,1},busy[]={0x40,1};
    if(phase==LT48_PIC_START_ARM)return h->write(h->context,arm,sizeof(arm));
    if(phase==LT48_PIC_START_BUSY)return h->write(h->context,busy,sizeof(busy));
    return LT48_INVALID_ARGUMENT;
}
lt48_status lt48_pic_source_host_begin_internal(lt48_pic_source_session *s,
    const lt48_pic_source_host *h,uint16_t mode,uint32_t address) {
    if(!s || !h || !h->runner || !h->write || !h->status || !mode ||
       !h->minimum_centivolts || h->minimum_centivolts>h->maximum_centivolts ||
       h->voltage_centivolts<h->minimum_centivolts || h->voltage_centivolts>h->maximum_centivolts ||
       h->voltage_centivolts>UINT16_MAX)return LT48_INVALID_ARGUMENT;
    /* Resolve entry and exit before touching hardware or voltage state. */
    if(!source_has_role(s,h->poweron_role) || !source_has_role(s,h->poweroff_role))return LT48_UNSUPPORTED;
    lt48_compile_variable *voltage=NULL;
    for(size_t i=0;i<s->variable_count;i++)if(s->variables[i].variable_id==h->voltage_variable)voltage=s->variables+i;
    if(!voltage || voltage->kind!=0 || !voltage->enabled)return LT48_UNSUPPORTED;
    unsigned char selector[]={0x0f,0};
    lt48_status st=lt48_pic_source_voltage_selector_internal(h->voltage_centivolts,selector+1);
    if(st!=LT48_OK)return st;
    voltage->value=(uint16_t)h->voltage_centivolts;
    st=h->write(h->context,selector,sizeof(selector));
    source_start_context context={s,h,mode,address};
    if(st==LT48_OK)st=lt48_pic_startup_run(h->poweron_role,source_start_invoke,&context);
    return st;
}
lt48_status lt48_pic_source_host_end_internal(lt48_pic_source_session *s,
    const lt48_pic_source_host *h,uint16_t mode) {
    if(!s || !h || !h->runner || !mode)return LT48_INVALID_ARGUMENT;
    if(!source_has_role(s,h->poweroff_role))return LT48_UNSUPPORTED;
    lt48_pic_source_host cleanup=*h;
    if(h->cleanup_runner)cleanup.runner=h->cleanup_runner;
    source_start_context context={s,&cleanup,mode,0};
    return source_host_callback(&context,h->poweroff_role);
}

lt48_status lt48_pic_source_session_bind_origin_internal(lt48_pic_source_session *session,const void *origin) {
    if(!session||!origin)return LT48_INVALID_ARGUMENT;
    if(session->origin&&session->origin!=origin)return LT48_SAFETY_REJECTED;
    session->origin=origin;return LT48_OK;
}
int lt48_pic_source_session_matches_origin_internal(const lt48_pic_source_session *session,const void *origin) {
    return session&&origin&&session->origin==origin;
}
