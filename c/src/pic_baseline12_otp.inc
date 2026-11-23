/* Normal C operator integration. Exact candidate membership is disabled until
 * complete source/profile and normal GUI/inventory publication is sealed. */
static lt48_status pic_otp12_open(const lt48_installation *x,const lt48_device_spec *spec,
    const b12otp_profile *p,lt48_pic_resources **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!x||!spec||!p||spec->catalogue.device_class!=2015||!spec->definition_stem||
       strcmp(spec->definition_stem,"GADC___")||spec->element_bits!=12)return LT48_UNSUPPORTED;
    if(spec->definition_resource!=p->definition||spec->family_code!=p->family||spec->pinmap!=p->pinmap||
       spec->capacity_bytes!=8192||!spec->catalogue.vendor||(strcmp(spec->catalogue.vendor,"Microchip")&&hy4_match(spec)!=p&&h77_match(spec)!=p)||
       spec->catalogue.device_name_length!=strlen(p->name)||memcmp(spec->catalogue.device_name,p->name,strlen(p->name))||
       !b12otp_valid(&p->policy))return LT48_SAFETY_REJECTED;
    lt48_pic_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    r->otp12=p;r->core=12;r->capacity=8192;r->program_words=p->policy.code_words;
    char path[4096];int n=snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x));
    lt48_status st=LT48_INVALID_ARGUMENT;
    if(n<0||(size_t)n>=sizeof path)goto fail;
    r->otp_path=malloc((size_t)n+1);if(!r->otp_path){st=LT48_NO_MEMORY;goto fail;}memcpy(r->otp_path,path,(size_t)n+1);
    unsigned char initial[8192];for(unsigned a=0;a<4096;a++){initial[2*a]=255;initial[2*a+1]=15;}
    const lt48_protocol_transport probe={.write=pic_otp_probe_command_transport,.read=pic_otp_probe_read_transport};
    b12otp_source *source=NULL;
    st=b12otp_source_open(path,p->family,p->pinmap,p->policy.code_words,initial,&probe,NULL,&source);
    if(st==LT48_OK) {
        const lt48_pin_map *pins;const lt48_compile_variable *vars;size_t count;
        st=lt48_pic_source_resources_bindings_internal(source->resources,&pins,&vars,&count);
        if(st==LT48_OK&&(count!=p->variable_count||lt48_pic_source_resources_count_internal(source->resources)!=p->operation_count))st=LT48_SAFETY_REJECTED;
        for(size_t i=0;st==LT48_OK&&i<count;i++)if(memcmp(vars+i,p->variables+i,sizeof *vars))st=LT48_SAFETY_REJECTED;
        for(unsigned i=0;st==LT48_OK&&i<p->operation_count;i++) {
            lt48_pic_source_resource_entry entry;st=lt48_pic_source_resources_entry_internal(source->resources,i,&entry);
            if(st==LT48_OK&&(entry.role!=p->roles[i]||entry.wave!=p->waves[i]||!entry.compiled||entry.source.wave_type))st=LT48_SAFETY_REJECTED;
        }
    }
    b12otp_source_close(source);
    if(st==LT48_OK)st=lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size);
    if(st!=LT48_OK)goto fail;
    *out=r;return LT48_OK;
fail:lt48_pic_close(r);return st;
}
static lt48_status pic_otp12_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_pic_result *out) {
    const b12otp_policy *p=&r->otp12->policy;
    uint16_t *target=image?malloc(8192):NULL,*scratch=malloc(8192),*pending=malloc(8192);
    if((image&&!target)||!scratch||!pending){free(target);free(scratch);free(pending);return LT48_NO_MEMORY;}
    unsigned char source_image[8192];
    for(unsigned a=0;a<4096;a++) {
        unsigned value=image?word(image+2*a)&4095:4095;
        if(target)target[a]=(uint16_t)value;
        source_image[2*a]=value;source_image[2*a+1]=value>>8;
    }
    b12otp_source *source=NULL;b12otp_result result={0};
    lt48_status st=b12otp_source_open(r->otp_path,r->otp12->family,r->otp12->pinmap,p->code_words,source_image,api,context,&source);
    if(st==LT48_OK)st=b12otp_source_prepare(source,r->idle,r->idle_size);
    if(st==LT48_OK) {
        b12otp_io io=b12otp_source_io(source);
        st=b12otp_execute(p,&io,op,target,pending,scratch,&result);
    } else {
        result.operation_status=st;
        if(source&&!source->upload_pending) {
            const lt48_pic_cleanup_io cleanup={source,b12otp_source_cleanup_role,b12otp_source_cleanup_command};
            (void)lt48_pic_cleanup(&cleanup,st,&result.cleanup);
        } else result.cleanup.status=LT48_SAFETY_REJECTED;
    }
    out->operation_status=result.operation_status;out->cleanup_status=result.cleanup.status;
    out->cleanup_completed=result.cleanup.completed;out->mismatch_count=result.mismatches;out->first_mismatch=result.first_mismatch;
    if(st==LT48_OK&&op==LT48_OPERATION_READ)for(unsigned a=0;a<4096;a++)put_word(read+2*a,pending[a]&4095);
    if(st==LT48_OK){out->completed_bytes=8192;if(api->progress)api->progress(context,LT48_EXECUTION_READ_OPERATION,8192,8192,0,0,0);}
    b12otp_source_close(source);free(target);free(scratch);free(pending);return st;
}
