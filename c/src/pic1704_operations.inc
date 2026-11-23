/* Shared compact row strategy. Traversal and configuration are supplied
 * by qualified profiles; this implementation uses 14-bit word images. */
static unsigned p17_voltage(const lt48_pic_resources *r,unsigned id) {
    for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==id)return r->vars[i].value;
    return 0;
}
static lt48_status p17_start(pic_session *s,unsigned mode,unsigned vcc) {
    memset(s->buffers,0,sizeof(s->buffers));s->mode=mode;
    /* Original 0044f440 selects the low-voltage path at <=4 V. */
    unsigned char selector[]={0x0f,0};
    lt48_status st=lt48_pic_source_voltage_selector_internal(vcc,selector+1);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,selector,2);
    if(st==LT48_OK)st=start(s,vcc);
    return st;
}
#include "pic_classic_protocol.inc"
typedef struct {
    pic_session *session;
    const unsigned char *source;
    unsigned char *destination;
    unsigned mode;
} rows_range_context;
static lt48_status rows_range_invoke(void *context,lt48_pic_range_phase phase,uint16_t role,
    uint32_t address,unsigned char *data_out,unsigned char *data_in,size_t width) {
    rows_range_context *range=context;pic_session *s=range->session;
    if(width!=2)return LT48_UNSUPPORTED;
    if(phase==LT48_PIC_RANGE_MODE){s->mode=role;return LT48_OK;}
    if(phase!=LT48_PIC_RANGE_ELEMENT)return LT48_UNSUPPORTED;
    unsigned value=range->mode==1?0x3fff:word(data_out),got=0;
    /* The original program dispatcher seeds both expected and observed data
     * before PROGRAM. Row state and every other buffer remain persistent. */
    if(range->mode==2)dword(s->buffers[5],value);
    lt48_status st=run(s,role,address,value,&got);
    if(st==LT48_OK)put_word(data_in,range->mode==1?(got&0x3fff):got);
    if(st==LT48_OK && range->mode==1 && s->api->progress && !(address&127))
        s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,2*address+2,s->r->capacity,0,0,0);
    return st;
}
static lt48_status rows_range_image_io(void *context,int write,uint32_t offset,unsigned char *data,size_t count) {
    rows_range_context *range=context;const lt48_pic_resources *r=range->session->r;
    if((uint64_t)offset+count>r->capacity || (offset&1) || (count&1))return LT48_FORMAT_ERROR;
    if(write) {
        if(!range->destination)return LT48_INVALID_ARGUMENT;
        for(size_t i=0;i<count;i+=2)if(word_mask(r,(offset+(unsigned)i)/2))
            memcpy(range->destination+offset+i,data+i,2);
    } else {
        if(!range->source)return LT48_INVALID_ARGUMENT;
        memcpy(data,range->source+offset,count);
    }
    return LT48_OK;
}
static int rows_range_cancelled(void *context) {
    rows_range_context *range=context;pic_session *s=range->session;
    return s->api->cancelled && s->api->cancelled(s->context);
}
static lt48_status rows_range(pic_session *s,const lt48_pic_window *window,unsigned mode,
    const unsigned char *source,unsigned char *destination) {
    if(s->r->profile->classic14&&s->r->profile->classic14->protocol)
        return classic_protocol_range(s,window,mode,source,destination);
    rows_range_context context={s,source,destination,mode};
    /* Pre/Post conversion belongs to the enclosing configuration transaction
     * for this already-qualified strategy, not each physical-read window. */
    lt48_pic_range_contract contract={mode,14,LT48_PIC_ABSENT_ROLE,
        mode==1?6:8,LT48_PIC_ABSENT_ROLE,0xff};
    lt48_pic_range_execution_result result;
    return lt48_pic_range_execute(&contract,window->address,window->address+window->count-1,1,
        rows_range_invoke,rows_range_image_io,rows_range_cancelled,&context,&result);
}
static lt48_status p17_scan(pic_session *s,unsigned char *dest,unsigned vcc,lt48_pic_result *out) {
    /* LOAD bypasses CONFIG reads in verify modes. Always read physically,
     * then compare in the owner, including the two configuration words. */
    lt48_status st=p17_start(s,1,vcc);
    for(unsigned a=0;a<s->r->capacity/2;a++)put_word(dest+2*a,0x3fff);
    for(size_t w=0;st==LT48_OK && w<lt48_pic_read_window_count(s->r->profile);w++) {
        const lt48_pic_window *window=&s->r->profile->read_windows[w];
        st=rows_range(s,window,1,NULL,dest);
    }
    lt48_status end=reset(s);if(st==LT48_OK)st=end;(void)out;return st;
}
static lt48_status p17_compare(pic_session *s,const unsigned char *want,const unsigned char *got,int blank,lt48_pic_result *out) {
    out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
    for(unsigned a=0;a<s->r->capacity/2;a++) {
        unsigned mask=word_mask(s->r,a),w=blank?0x3fff:word(want+2*a);
        if((w^word(got+2*a))&mask){if(!out->mismatch_count)out->first_mismatch=a*2;out->mismatch_count++;}
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
typedef struct {pic_session *session;unsigned vcc;unsigned char *programmed;} rows_transaction_context;
static lt48_status rows_transaction_invoke(void *context,lt48_pic_transaction_phase phase,uint16_t role,uint16_t timeout) {
    rows_transaction_context *tx=context;pic_session *s=tx->session;
    if(phase==LT48_PIC_TX_MODE){s->mode=role;return LT48_OK;}
    if(phase==LT48_PIC_TX_START)return p17_start(s,role,tx->vcc);
    /* PROTECT consumes this image. PostConfig can encode fields differently;
     * its output must not replace the expected bytes for physical readback. */
    if(phase==LT48_PIC_TX_ACTION && tx->programmed)
        memcpy(tx->programmed,s->image,s->r->capacity);
    return run_with_timeout(s,role,0,0,NULL,timeout);
}
static lt48_status rows_transaction(pic_session *s,unsigned mode,unsigned vcc,unsigned char *programmed) {
    rows_transaction_context context={s,vcc,programmed};
    /* Qualified authored row-owner conversion sequence. This is distinct from
     * the original host's DEVINIT/action/DEVRESET transaction. */
    const lt48_pic_transaction_plan erase={7,{11,30},{9,30},{12,10}};
    const lt48_pic_transaction_plan configuration={8,{11,10},{10,10},{12,10}};
    const lt48_pic_transaction_plan *plan=mode==7?&erase:mode==8?&configuration:NULL;
    return lt48_pic_transaction_run(plan,0,rows_transaction_invoke,&context);
}
static lt48_status p17_configuration(pic_session *s,unsigned vcc) {
    if(s->r->profile->classic14&&s->r->profile->classic14->protocol) {
        for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
        lt48_status st=classic_protocol_configuration(s),end=reset(s);
        return st==LT48_OK?end:st;
    }
    unsigned char *programmed=malloc(s->r->capacity);
    if(!programmed)return LT48_NO_MEMORY;
    lt48_status st=rows_transaction(s,8,vcc,programmed);
    if(st==LT48_OK)memcpy(s->image,programmed,s->r->capacity);
    free(programmed);
    lt48_status end=reset(s);return st==LT48_OK?end:st;
}
static int rows_configuration_diff(const lt48_pic_resources *r,const unsigned char *a,const unsigned char *b) {
    for(size_t i=0;i<r->profile->configuration_count;i++) {
        const lt48_pic_config_word *c=&r->profile->configuration[i];
        if((word(a+c->image_offset)^word(b+c->image_offset))&c->compare_mask)return 1;
    }
    return 0;
}
static void rows_configuration_unprotect(const lt48_pic_resources *r,unsigned char *image) {
    for(size_t i=0;i<r->profile->configuration_count;i++) {
        const lt48_pic_config_word *c=&r->profile->configuration[i];
        put_word(image+c->image_offset,word(image+c->image_offset)|c->unprotected_ones);
    }
}
static int rows_configuration_protection_requested(const lt48_pic_resources *r,const unsigned char *image) {
    for(size_t i=0;i<r->profile->configuration_count;i++) {
        const lt48_pic_config_word *c=&r->profile->configuration[i];
        if((word(image+c->image_offset)&c->unprotected_ones)!=c->unprotected_ones)return 1;
    }
    return 0;
}
static lt48_status pic_compact_rows_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,lt48_pic_result *out) {
    pic_session s={0};s.r=r;s.api=api;s.context=context;
    memcpy(s.vars,r->vars,sizeof(s.vars));size_t n=r->capacity;
    /* Source Pre/PROTECT/Post callbacks preserve factory words beyond the
     * canonical exported image. Keep the complete 16-bit word-address plane
     * as private callback backing; operator reads/writes still use n bytes. */
    s.image_capacity=n>0x20000?n:0x20000;
    s.image=malloc(s.image_capacity);unsigned char *actual=malloc(n),*target=malloc(n);
    if(!s.image||!actual||!target){free(s.image);free(actual);free(target);return LT48_NO_MEMORY;}
    memset(s.image,255,s.image_capacity);
    for(unsigned a=0;a<n/2;a++)put_word(target+2*a,image?(word(image+2*a)&0x3fff):0x3fff);
    /* Reserved configuration bits read as one; source PostConfig does the same. */
    for(size_t i=0;i<r->profile->configuration_count;i++) {
        const lt48_pic_config_word *c=&r->profile->configuration[i];
        put_word(target+c->image_offset,word(target+c->image_offset)|c->reserved_ones);
    }
    if(r->profile->classic14&&((lt48_pic_classic_is7x(r->profile->classic14->protocol)&&r->profile->configuration_count==2)||lt48_pic_classic_is87x(r->profile->classic14->protocol)))
        put_word(target+0x400e,word(target+0x400e)|0x800); /* Required RESV fuse. */
    memcpy(s.image,target,n);
    /* DEVINIT's inline device ID check is controlled by option bit 1. */
    s.config[0]=(lt48_runtime_configuration_value){-1,0,1,1};s.config_count=1;
    if(r->profile->classic14&&r->profile->classic14->protocol)s.config[0].value=0;
    unsigned nominal=p17_voltage(r,0),low=p17_voltage(r,5),high=p17_voltage(r,6);
    unsigned classic_protocol=r->profile->classic14&&r->profile->classic14->protocol;
    uint16_t factory_before[2]={0},factory_after[2]={0};
    lt48_status st=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(st!=LT48_OK)goto done;
    /* As in the OTP owner, establish an unpowered/released target before the
     * FPGA stream changes controller grammar. A truncated stream must never
     * consume attempted cleanup packets as more configuration data. */
    for(unsigned c=4;st==LT48_OK&&c>0;c--){
        const unsigned char off[]={0x10,(unsigned char)(c-1),0};
        st=lt48_runtime_transport_session_controller_write(s.wire,off,sizeof off);
    }
    if(st==LT48_OK){const unsigned char clear[]={0x49,0xaa,0x55};st=lt48_runtime_transport_session_controller_write(s.wire,clear,sizeof clear);}
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
    if(st!=LT48_OK){out->operation_status=st;st=finish(&s,st,out);goto done;}
    const unsigned char fpga[]={0x38,0xaa,0x55},busy[]={0x56,0x0a,1,0x40,1};
    st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,fpga,3);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
    if(st!=LT48_OK){
        out->operation_status=st;out->cleanup_completed=0;out->cleanup_status=LT48_CLEANUP_FAILED;
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;goto done;
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof busy);
    if(st==LT48_OK&&classic_protocol)st=classic_protocol_factory(&s,nominal,factory_before);
    if(st==LT48_OK && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) {
        st=p17_scan(&s,actual,nominal,out);
        if(st==LT48_OK && op==LT48_OPERATION_PROGRAM)st=p17_compare(&s,NULL,actual,1,out);
        if(st==LT48_OK && op==LT48_OPERATION_OVER_PROGRAM) {
            for(unsigned a=0;a<n/2;a++)if(word_mask(r,a)&word(target+2*a)&~word(actual+2*a)){st=LT48_SAFETY_REJECTED;out->first_mismatch=2*a;break;}
        }
        /* CP and WRT must remain disabled until all code and IDs are verified. */
        rows_configuration_unprotect(r,s.image);
        if(st==LT48_OK)st=p17_start(&s,2,nominal);
        for(size_t w=0;st==LT48_OK && w<lt48_pic_program_window_count(r->profile);w++) {
            const lt48_pic_window *window=&r->profile->program_windows[w];
            st=rows_range(&s,window,2,target,NULL);
        }
        unsigned pending=r->profile->word_program?r->profile->word_program->row_pending_buffer:24;
        if(st==LT48_OK && word(s.buffers[pending])!=0){
#ifdef LT48_PIC_FIXTURE_TRACE
            fprintf(stderr,"PIC pending buffer %u value %u\n",pending,word(s.buffers[pending]));
#endif
            st=LT48_RUNTIME_ERROR;
        } /* no uncommitted row */
        if(st==LT48_OK)st=reset(&s);
        if(st==LT48_OK)st=p17_configuration(&s,nominal);
        if(st==LT48_OK)st=p17_scan(&s,actual,low,out);
        if(st==LT48_OK)st=p17_compare(&s,s.image,actual,0,out);
        if(st==LT48_OK)st=p17_scan(&s,actual,high,out);
        if(st==LT48_OK)st=p17_compare(&s,s.image,actual,0,out);
        if(st==LT48_OK && rows_configuration_protection_requested(r,target)) {
            memcpy(s.image,target,n);st=p17_configuration(&s,nominal);
            if(st==LT48_OK)st=p17_scan(&s,actual,nominal,out);
            if(st==LT48_OK && rows_configuration_diff(r,actual,target))st=LT48_DEVICE_MISMATCH;
        }
    } else if(st==LT48_OK && op==LT48_OPERATION_ERASE) {
        st=r->profile->classic14&&r->profile->classic14->protocol?
            classic_protocol_erase(&s,nominal):rows_transaction(&s,7,nominal,NULL);
        if(st==LT48_OK)st=reset(&s);
        if(st==LT48_OK)st=p17_scan(&s,actual,nominal,out);
        if(st==LT48_OK)st=p17_compare(&s,NULL,actual,1,out);
    } else if(st==LT48_OK && op==LT48_OPERATION_SECURE) {
        st=p17_scan(&s,actual,nominal,out);
        if(st==LT48_OK) {
            memcpy(s.image,actual,n);
            for(size_t i=0;i<r->profile->configuration_count;i++) {
                const lt48_pic_config_word *c=&r->profile->configuration[i];
                put_word(s.image+c->image_offset,word(s.image+c->image_offset)&~c->secure_clear);
            }
            st=p17_configuration(&s,nominal);
            if(st==LT48_OK)st=p17_scan(&s,actual,nominal,out);
            if(st==LT48_OK && rows_configuration_diff(r,actual,s.image))st=LT48_DEVICE_MISMATCH;
        }
    } else if(st==LT48_OK) {
        unsigned passes=op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL?2:1;
        for(unsigned pass=0;st==LT48_OK && pass<passes;pass++) {
            unsigned v=op==LT48_OPERATION_VERIFY && margin!=LT48_VERIFY_MARGIN_NOMINAL?
                (margin==LT48_VERIFY_MARGIN_HIGH || pass?high:low):nominal;
            st=p17_scan(&s,actual,v,out);
            if(st==LT48_OK && op!=LT48_OPERATION_READ)st=p17_compare(&s,target,actual,op==LT48_OPERATION_BLANK_CHECK,out);
        }
    }
    if(st==LT48_OK&&classic_protocol) {
        st=classic_protocol_factory(&s,nominal,factory_after);
        if(st==LT48_OK&&memcmp(factory_before,factory_after,sizeof factory_before))st=LT48_DEVICE_MISMATCH;
    }
    out->operation_status=st;st=finish(&s,st,out);
    if(st==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,actual,n);
    if(st==LT48_OK)out->completed_bytes=n;
done:lt48_pic_source_session_close_internal(s.source);free(s.image);free(actual);free(target);return st;
}
