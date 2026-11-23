/* AT89LP213/214/216 owner, using the exact GDBB applets and pin maps.
 * Included after the shared serial transport primitives. Source-audited only. */
static lt48_status lp21_read_settings(void *context,lt48_lp21_settings *out) {
    page_session *s=context;s->mode=13;
    lt48_status st=run(s,2);
    s->buffers[12][0]=32;
    s->config_count=13;
    for(unsigned i=0;i<11;i++)s->config[i]=(lt48_runtime_configuration_value){-1,0,i,UINT32_MAX};
    s->config[11]=(lt48_runtime_configuration_value){-1,1,0,UINT32_MAX};
    s->config[12]=(lt48_runtime_configuration_value){-1,2,0,UINT32_MAX};
    if(st==LT48_OK)st=run(s,6);
    if(st==LT48_OK) {
        if(s->config_count!=13)st=LT48_RUNTIME_ERROR;
        for(unsigned i=0;st==LT48_OK && i<13;i++) {
            unsigned max=i<11?1:i==11?2:255;
            if(s->config[i].value>max)st=LT48_DEVICE_MISMATCH;
            else out->value[i]=(unsigned char)s->config[i].value;
        }
        if(st==LT48_OK)memcpy(out->signature,s->config_image+2048,64);
    }
    if(st==LT48_OK)st=run(s,3);
    return st;
}
static lt48_status lp21_write_row(void *context,enum lt48_lp21_row row,
    const lt48_lp21_settings *wanted) {
    page_session *s=context;s->mode=13;
    lt48_status st=run(s,2);
    s->buffers[12][0]=32;s->config_count=13;
    for(unsigned i=0;i<11;i++)s->config[i]=(lt48_runtime_configuration_value){-1,0,i,wanted->value[i]};
    s->config[11]=(lt48_runtime_configuration_value){-1,1,0,wanted->value[11]};
    s->config[12]=(lt48_runtime_configuration_value){-1,2,0,wanted->value[12]};
    memcpy(s->config_image+2048,wanted->signature,64);
    if(st==LT48_OK)st=run(s,11+(unsigned)row);
    if(st==LT48_OK)st=run(s,3);
    return st;
}
static lt48_status lp21_cleanup(void *context,unsigned phase) {
    page_session *s=context;
    if(phase==0) {
        unsigned char reply[512]={0};const unsigned char stop[]={0,0};
        lt48_status st=cmd(s,stop,sizeof(stop),reply);
        return st==LT48_OK && !(reply[0]|reply[1])?LT48_PROTOCOL_ERROR:st;
    }
    if(phase==1)return run(s,3);
    if(phase==2)return run(s,1);
    return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
}
static lt48_status lp21_erase(page_session *s,lt48_lp21_settings *saved,
    lt48_serial_page_result *out) {
    /* Use mode 13 to enter ISP without the source mode-2 conditional erase.
     * Explicit erase is followed by code/lock/fuse verification. Signature
     * is preserved whether or not a silicon revision erases it with code. */
    s->mode=13;lt48_status st=run(s,2);
    if(st==LT48_OK)st=run(s,8);
    if(st==LT48_OK && s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=run(s,3);
    lt48_lp21_settings actual;
    if(st==LT48_OK)st=lp21_read_settings(s,&actual);
    if(st==LT48_OK && (actual.value[11] || memcmp(actual.value,saved->value,11) || actual.value[12]!=saved->value[12]))st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=cycle(s,11,0,2048,NULL,NULL,s->r->nominal,out);
    if(st==LT48_OK) {
        saved->value[11]=0;
        lt48_lp21_update_result update;
        st=lt48_lp21_update_settings(s,lp21_read_settings,lp21_write_row,saved,0x2000,&update);
    }
    return st;
}
static lt48_status lp21_execute(const lt48_serial_page_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size,
    unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,
    unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_serial_page_result *out) {
    if(!out || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_serial_page_validate(r,op,start,length,image,size);
    if(st==LT48_OK)st=lt48_serial_page_validate_verify_options(r,op,margin,margin_type);
    if(st==LT48_OK && ((op==LT48_OPERATION_READ && (!read || read_size<length)) ||
       (op==LT48_OPERATION_CONFIGURATION && (!read || read_size<77))))st=LT48_BUFFER_TOO_SMALL;
    if(st!=LT48_OK){out->operation_status=st;return st;}
    unsigned char settings_image[2112]={0},pending[2048];
    page_session s={0};s.r=r;s.api=api;s.context=context;s.mode=13;
    s.config_image=settings_image;memcpy(s.vars,r->vars,sizeof(s.vars));
    /* A cancellation between an applet download header and its payload
     * strands the resident controller waiting for data. Finish the bounded
     * RAM/FPGA setup before observing a new cancellation. A transport failure
     * during setup is different: shutdown is attempted, but cannot be proven. */
    lt48_protocol_transport clean=*api;clean.cancelled=NULL;
    int setup_uncertain=0;
    if(api->cancelled && api->cancelled(context))st=LT48_CANCELLED;
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(&clean,context,&s.wire);
    if(st==LT48_OK) {
        s.api=&clean;st=setup(&s);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
        if(st!=LT48_OK)setup_uncertain=1;
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;s.api=api;
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,context,&s.wire);
    for(unsigned attempt=0;st==LT48_OK && attempt<64;attempt++) {
        st=run(&s,9);if(st!=LT48_OK)break;
        if(s.buffers[6][0]==r->man && s.buffers[4][0]==r->dev)break;
        lt48_m37_id_choice choice=decide?decide(decision_context,r->man,r->dev,s.buffers[6][0],s.buffers[4][0]):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)break;
        if(choice!=LT48_M37_ID_RETRY || attempt==63)st=LT48_DEVICE_MISMATCH;
    }
    lt48_lp21_settings saved={0},observed={0};
    if(st==LT48_OK)st=lp21_read_settings(&s,&saved);
    int programming=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)) {
        lt48_lp21_settings wanted=saved;unsigned mask=0;
        if(op==LT48_OPERATION_SECURE) {
            unsigned level=image?image[0]:2;
            wanted.value[11]=(unsigned char)(level>saved.value[11]?level:saved.value[11]);mask=1u<<11;
        } else if(image) {
            mask=image[0]|image[1]*256u;memcpy(wanted.value,image+2,13);memcpy(wanted.signature,image+15,64);
        }
        lt48_lp21_update_result update;
        st=lt48_lp21_update_settings(&s,lp21_read_settings,lp21_write_row,&wanted,mask,&update);
        if(st==LT48_OK)observed=update.observed;
    } else if(st==LT48_OK) {
        if(op==LT48_OPERATION_ERASE || (programming && saved.value[11]==2))st=lp21_erase(&s,&saved,out);
        else if(saved.value[11]==2 || (programming && saved.value[11]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && programming) {
            st=cycle(&s,1,0,2048,NULL,pending,r->nominal,out);
            for(size_t i=0;st==LT48_OK && i<2048;i++) {
                if((op==LT48_OPERATION_PROGRAM && pending[i]!=255) || (pending[i]&image[i])!=image[i]) {
                    st=LT48_DEVICE_MISMATCH;out->first_mismatch=(uint32_t)i;out->mismatch_count=1;
                }
            }
            if(st==LT48_OK)st=cycle(&s,2,0,2048,image,NULL,r->nominal,out);
            for(unsigned i=0;st==LT48_OK && i<2;i++)st=cycle(&s,4,0,2048,image,NULL,margin_voltage(r,margin_type,i),out);
        } else if(st==LT48_OK && op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};unsigned n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
            for(unsigned i=0;st==LT48_OK && i<n;i++)st=cycle(&s,4,start,length,image,NULL,volts[i],out);
        } else if(st==LT48_OK && op!=LT48_OPERATION_ERASE)
            st=cycle(&s,op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:5,start,length,image,pending,r->nominal,out);
        if(st==LT48_OK)st=lp21_read_settings(&s,&observed);
        if(st==LT48_OK && (memcmp(saved.value,observed.value,13) || memcmp(saved.signature,observed.signature,64)))st=LT48_DEVICE_MISMATCH;
    }
    out->operation_status=st;
    s.api=&clean;
    const unsigned phases[]={0,1,2,3};
    st=lt48_owner_finish_internal(&clean,context,&s.wire,lp21_cleanup,&s,phases,4,st,&out->cleanup_status,&out->cleanup_completed);
    if(setup_uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
    if(st==LT48_OK) {
        if(op==LT48_OPERATION_READ)memcpy(read,pending,length);
        if(op==LT48_OPERATION_CONFIGURATION){memcpy(read,observed.value,13);memcpy(read+13,observed.signature,64);}
        out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?77:length;
    }
    return st;
}
