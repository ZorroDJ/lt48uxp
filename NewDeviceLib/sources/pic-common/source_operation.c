#include "source_operation.h"

int lt48_pic_source_compact_program_dispatch_internal(lt48_pic_compact_program_state *state,
    lt48_pic_compact_program_call call,void *context) {
    return lt48_pic_compact_program_sequence(state,call,context);
}

static lt48_status source_operation_phase(lt48_pic_source_session *session,
    const lt48_pic_source_operation_contract *contract,uint16_t mode,uint32_t first,uint32_t last,
    const lt48_pic_source_range_options *options,
    lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!session || !contract || !options || !result)return LT48_INVALID_ARGUMENT;
    lt48_status st=LT48_INVALID_ARGUMENT;
    if(!contract->element_bits || contract->element_bits>32)goto done;
    const lt48_pic_source_geometry *g=&contract->geometry;
    lt48_pic_source_geometry expected;
    if(!lt48_pic_source_geometry_prepare(g->element_count,contract->element_bits,g->backing_bytes,&expected) ||
       g->element_bytes!=expected.element_bytes || g->traversal_bytes!=expected.traversal_bytes ||
       g->first_element!=expected.first_element || g->last_element!=expected.last_element ||
       g->last_file_byte!=expected.last_file_byte || g->checksum_last_byte!=expected.checksum_last_byte ||
       options->logical_image_size!=g->traversal_bytes)goto done;
    const unsigned char *image;size_t size;
    st=lt48_pic_source_session_image_internal(session,&image,&size);
    if(st!=LT48_OK)goto done;
    if(size!=g->backing_bytes){st=LT48_INVALID_ARGUMENT;goto done;}
    int transaction=mode==7 || mode==8;
    if(!transaction && !(mode==1 || mode==2 || mode==3 || mode==4 || mode==5 || mode==11))
        {st=LT48_UNSUPPORTED;goto done;}
    uint16_t action=transaction?(mode==7?contract->erase_role:contract->configuration_role):
        ((mode==2 || mode==3)?contract->program_role:contract->load_role);
    if(action==LT48_PIC_ABSENT_ROLE){st=LT48_UNSUPPORTED;goto done;}
    if(!transaction && (first>g->last_element || last>g->last_element))
        {st=LT48_INVALID_ARGUMENT;goto done;}
    result->cleanup_attempted=1;
    if(transaction) {
        /* Constructor-bound DEVINIT/action/DEVRESET; Pre/PostConfig are image
         * conversions owned by the separate configuration service. */
        uint16_t timeout=mode==7?30:10;
        lt48_pic_transaction_plan plan={mode,{contract->init_role,timeout},
            {action,timeout},{contract->reset_role,10}};
        st=lt48_pic_source_session_transaction_internal(session,&plan,options);
    } else {
        lt48_pic_range_contract plan={mode,contract->element_bits,
            contract->init_role,action,contract->reset_role,0xff};
        st=lt48_pic_source_session_range_internal(session,&plan,first,last,options,&result->traversal);
    }
done:
    result->operation_status=st;
    /* A caller can enter with the rails already enabled. Even a preflight
     * rejection must release that powered session. */
    if(options->started)result->cleanup_attempted=1;
    return st;
}
lt48_status lt48_pic_source_operation_execute_internal(lt48_pic_source_session *session,
    const lt48_pic_source_operation_contract *contract,uint16_t mode,uint32_t first,uint32_t last,
    const lt48_pic_source_range_options *options,lt48_pic_source_finish finish,void *finish_context,
    lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!finish)return LT48_INVALID_ARGUMENT;
    lt48_status st=source_operation_phase(session,contract,mode,first,last,options,result);
    if(result && result->cleanup_attempted)result->cleanup_status=finish(finish_context);
    return st==LT48_OK?result->cleanup_status:st;
}
typedef struct {
    lt48_pic_source_session *session;
    const lt48_pic_source_host *host;
    const lt48_pic_source_range_options *options;
    uint16_t mode;
    lt48_pic_source_global_finish finish;
    void *finish_context;
    lt48_pic_source_operation_result *result;
} source_operation_owner;
static lt48_status source_operation_start(void *context,lt48_pic_source_session *session,
    uint16_t mode,uint32_t address) {
    source_operation_owner *owner=context;
    return lt48_pic_source_host_begin_internal(session,owner->host,mode,address);
}
static lt48_status source_operation_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **result) {
    source_operation_owner *owner=context;
    return owner->options->runner(owner->options->context,machine,result);
}
static int source_operation_cancelled(void *context) {
    source_operation_owner *owner=context;
    return owner->options->cancelled && owner->options->cancelled(owner->options->context);
}
static lt48_status source_operation_finish(void *context) {
    source_operation_owner *owner=context;
    lt48_status source=lt48_pic_source_host_end_internal(owner->session,owner->host,owner->mode);
    lt48_status global=owner->finish(owner->finish_context,owner->result->operation_status,source);
    return source==LT48_OK?global:source;
}
lt48_status lt48_pic_source_operation_run_internal(lt48_pic_source_session *session,
    const lt48_pic_source_operation_contract *contract,const lt48_pic_source_host *host,
    uint16_t mode,uint32_t first,uint32_t last,const lt48_pic_source_range_options *options,
    lt48_pic_source_global_finish finish,void *finish_context,lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!session || !contract || !host || !options || !finish || !result)return LT48_INVALID_ARGUMENT;
    source_operation_owner owner={session,host,options,mode,finish,finish_context,result};
    if(!options->runner || options->start || !mode) {
        if(options->started) {
            result->cleanup_attempted=1;result->cleanup_status=source_operation_finish(&owner);
        }
        return LT48_INVALID_ARGUMENT;
    }
    lt48_pic_source_range_options bound=*options;
    bound.start=source_operation_start;bound.runner=source_operation_runner;
    bound.cancelled=source_operation_cancelled;bound.context=&owner;
    return lt48_pic_source_operation_execute_internal(session,contract,mode,first,last,&bound,
        source_operation_finish,&owner,result);
}

/* A public operation retains one prepared frame and one powered session.
 * Only the dispatcher's explicit FINISH phase releases it. */
typedef struct {
    lt48_pic_source_session *session;
    const lt48_pic_source_operation_contract *contract;
    const lt48_pic_source_program_request *request;
    lt48_pic_source_range_options options;
    lt48_pic_source_program_finish finish;
    void *finish_context;
    lt48_pic_source_operation_result *result;
    unsigned touched;
    uint16_t mode;
} source_program_owner;
static lt48_status source_program_start(void *context,lt48_pic_source_session *session,
    uint16_t mode,uint32_t address) {
    source_program_owner *owner=context;
    owner->touched=1;
    lt48_status st=owner->request->options.start(owner->request->options.context,session,mode,address);
    if(st==LT48_OK)owner->options.started=1;
    return st;
}
static lt48_status source_program_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **result) {
    source_program_owner *owner=context;
    return owner->request->options.runner(owner->request->options.context,machine,result);
}
static int source_program_cancelled(void *context) {
    source_program_owner *owner=context;
    return owner->request->options.cancelled &&
        owner->request->options.cancelled(owner->request->options.context);
}
static lt48_status source_program_stage(source_program_owner *owner,uint16_t mode) {
    lt48_pic_source_operation_result stage;
    owner->mode=mode;
    lt48_status st=source_operation_phase(owner->session,owner->contract,mode,
        owner->request->first,owner->request->last,&owner->options,&stage);
    owner->result->traversal=stage.traversal;
    owner->result->operation_status=st;
    return st;
}
static int source_program_call(void *context,enum lt48_pic_compact_program_phase phase,
    lt48_pic_compact_program_state *state) {
    source_program_owner *owner=context;
    lt48_status st=LT48_OK;
    switch(phase) {
    case LT48_PIC_COMPACT_PROGRAM_FINISH:
        if(owner->touched || owner->options.started) {
            owner->result->cleanup_attempted++;
            st=owner->finish(owner->finish_context,owner->session,owner->mode,
                owner->result->operation_status);
            if(owner->result->cleanup_status==LT48_OK)owner->result->cleanup_status=st;
            owner->touched=0;owner->options.started=0;
        }
        return st==LT48_OK;
    case LT48_PIC_COMPACT_PROGRAM_ASK_ERASE:
        return owner->request->ask_erase &&
            owner->request->ask_erase(owner->request->options.context);
    case LT48_PIC_COMPACT_PROGRAM_BLANK:
        state->stopped=1;
        st=source_program_stage(owner,11);
        /* Original 00450220 clears +98 only for a reported blank-data
         * mismatch. A failed LOAD callback is not evidence of nonblank data. */
        if(st==LT48_DEVICE_MISMATCH && owner->result->traversal.first_mismatch!=UINT32_MAX)
            state->stopped=0;
        break;
    case LT48_PIC_COMPACT_PROGRAM_ERASE:st=source_program_stage(owner,7);break;
    case LT48_PIC_COMPACT_PROGRAM_WRITE:st=source_program_stage(owner,2);break;
    case LT48_PIC_COMPACT_PROGRAM_PROTECT:st=source_program_stage(owner,8);break;
    case LT48_PIC_COMPACT_PROGRAM_VERIFY:
        for(unsigned i=0;i<4;i++) {
            uint32_t voltage=owner->request->verify_voltages[i];
            if(!voltage)continue;
            st=lt48_pic_source_session_voltage_value_internal(owner->session,
                owner->request->voltage_variable,voltage);
            if(st==LT48_OK)st=source_program_stage(owner,4);
            if(st!=LT48_OK)break;
        }
        /* The original restores the nominal declaration before finish. */
        {lt48_status restore=lt48_pic_source_session_voltage_value_internal(owner->session,
            owner->request->voltage_variable,owner->request->nominal_voltage);
         if(st==LT48_OK)st=restore;}
        owner->result->operation_status=st;
        break;
    }
    return st==LT48_OK;
}
lt48_status lt48_pic_source_program_execute_internal(lt48_pic_source_session *session,
    const lt48_pic_source_operation_contract *contract,const lt48_pic_source_program_request *request,
    lt48_pic_source_program_finish finish,void *finish_context,lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!session || !contract || !request || !finish || !result)return LT48_INVALID_ARGUMENT;
    /* Preflight every requested action before startup. The adapter supplies
     * source-derived voltage slots and the qualified source startup/finish.
     * This does not infer preservation or qualify a silicon protocol. */
    const lt48_pic_source_range_options *o=&request->options;
    lt48_status st=LT48_INVALID_ARGUMENT;
    if(!o->runner || !o->start || o->started || request->stopped>1 || (request->flags&~UINT32_C(0x103c)) ||
       !(request->flags&0x3c))goto rejected;
    uint16_t roles[]={contract->init_role,contract->reset_role,
        (request->flags&(4|16))?contract->load_role:LT48_PIC_ABSENT_ROLE,
        (request->flags&8)?contract->program_role:LT48_PIC_ABSENT_ROLE,
        (request->flags&32)?contract->configuration_role:LT48_PIC_ABSENT_ROLE,
        contract->erase_role};
    if(((request->flags&(4|16)) && contract->load_role==LT48_PIC_ABSENT_ROLE) ||
       ((request->flags&8) && contract->program_role==LT48_PIC_ABSENT_ROLE) ||
       ((request->flags&32) && contract->configuration_role==LT48_PIC_ABSENT_ROLE))
        {st=LT48_UNSUPPORTED;goto rejected;}
    for(unsigned i=0;i<sizeof(roles)/sizeof(*roles);i++)
        if(roles[i]!=LT48_PIC_ABSENT_ROLE && !lt48_pic_source_session_has_callback_internal(session,roles[i]))
            {st=LT48_UNSUPPORTED;goto rejected;}
    lt48_pic_source_geometry checked;
    const lt48_pic_source_geometry *g=&contract->geometry;
    const unsigned char *image;size_t image_size;
    if(!contract->element_bits || contract->element_bits>32 ||
       !lt48_pic_source_geometry_prepare(g->element_count,contract->element_bits,g->backing_bytes,&checked) ||
       g->element_bytes!=checked.element_bytes || g->traversal_bytes!=checked.traversal_bytes ||
       g->first_element!=checked.first_element || g->last_element!=checked.last_element ||
       g->last_file_byte!=checked.last_file_byte || g->checksum_last_byte!=checked.checksum_last_byte ||
       o->logical_image_size!=g->traversal_bytes ||
       request->first>g->last_element || request->last>g->last_element ||
       lt48_pic_source_session_image_internal(session,&image,&image_size)!=LT48_OK ||
       image_size!=g->backing_bytes)goto rejected;
    if(request->flags&16) {
        unsigned count=0;lt48_compile_variable variable;
        if(!request->nominal_voltage || request->nominal_voltage>UINT16_MAX ||
           lt48_pic_source_session_variable_internal(session,request->voltage_variable,&variable)!=LT48_OK ||
           variable.kind!=0 || variable.value!=request->nominal_voltage)goto rejected;
        for(unsigned i=0;i<4;i++) {
            if(request->verify_voltages[i]>UINT16_MAX)goto rejected;
            if(request->verify_voltages[i])count++;
        }
        if(!count)goto rejected;
    }
    lt48_pic_source_session *stage=NULL;
    st=lt48_pic_source_session_clone_internal(session,&stage);
    if(st!=LT48_OK)goto rejected;
    if(request->prepare_image) {
        st=request->prepare_image(request->prepare_context,stage);
        if(st==LT48_OK && (request->flags&16)) {
            lt48_compile_variable voltage;
            st=lt48_pic_source_session_variable_internal(stage,request->voltage_variable,&voltage);
            if(st==LT48_OK && (voltage.kind!=0 || voltage.value!=request->nominal_voltage))
                st=LT48_SAFETY_REJECTED;
        }
        if(st!=LT48_OK) {
            lt48_pic_source_session_close_internal(stage);
            goto rejected;
        }
    }
    source_program_owner owner={stage,contract,request,*o,finish,finish_context,result,0,0};
    owner.options.start=source_program_start;owner.options.runner=source_program_runner;
    owner.options.cancelled=source_program_cancelled;owner.options.context=&owner;
    lt48_pic_compact_program_state state={request->flags,contract->erase_role!=LT48_PIC_ABSENT_ROLE,request->stopped};
    int ok=lt48_pic_source_compact_program_dispatch_internal(&state,source_program_call,&owner);
    st=result->operation_status;
    if(ok) {
        lt48_pic_source_state final;
        st=lt48_pic_source_session_state_internal(stage,&final);
        if(st==LT48_OK)st=lt48_pic_source_session_restore_internal(session,&final);
        result->operation_status=st;
    } else if(st==LT48_OK)st=result->cleanup_status==LT48_OK?LT48_CANCELLED:result->cleanup_status;
    lt48_pic_source_session_close_internal(stage);
    return st;
rejected:
    result->operation_status=st;
    if(o->started) {
        result->cleanup_attempted=1;
        result->cleanup_status=finish(finish_context,session,0,st);
    }
    return st;
}
