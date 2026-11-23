/* Standalone functional test, original 00444d50. Internal until full admission.
 * margin=0: +/-5%, 1: +/-10%, 2: nominal. Stop at the first mismatch. */
static lt48_status vector_owner_packet(void *context,const unsigned char *data,size_t size) {
    lt48_pal_wave_session *s=context;
    return lt48_runtime_transport_session_controller_write(s->transport,data,size);
}
static lt48_status vector_owner_supply(lt48_pal_wave_session *s,unsigned cv) {
    unsigned char code=(unsigned char)(cv?cv/10+13:0);
    for(unsigned channel=1;channel<=2;channel++) {
        unsigned char bytes[]={0x10,(unsigned char)channel,code};
        lt48_status status=vector_owner_packet(s,bytes,sizeof bytes);
        if(status!=LT48_OK)return status;
        s->voltage[channel]=cv;
    }
    return LT48_OK;
}
lt48_status lt48_pof_vectors_validate_internal(const lt48_pal_resources *resources,
    const lt48_pof_file_metadata *vectors,unsigned margin,unsigned x_mode) {
    lt48_status status=LT48_INVALID_ARGUMENT;
    lt48_vector_device_profile profile;
    if(!resources||!vectors||!vectors->vectors||!vectors->row_count||margin>2||x_mode>2)return status;
    status=lt48_pal_resources_vector_profile_internal(resources,&profile);
    if(status!=LT48_OK)return status;
    if(vectors->pin_count!=profile.pin_count){status=LT48_DEVICE_MISMATCH;return status;}
    /* The admitted source candidates all use 5 V; margin conversion is
     * audited for this nominal value. Additional supplies need that audit. */
    if(profile.centivolts!=500){status=LT48_UNSUPPORTED;return status;}
    size_t stride=(size_t)vectors->pin_count+1;
    if(vectors->row_count>SIZE_MAX/stride){status=LT48_INVALID_ARGUMENT;return status;}
    for(size_t row=0;row<vectors->row_count;row++) {
        const char *v=vectors->vectors+row*stride;
        for(size_t i=0;i<vectors->pin_count;i++)if(!v[i]||!strchr("01BCDFHKLNPUXZ",v[i])) {
            status=LT48_INVALID_ARGUMENT;return status;
        }
        if(v[vectors->pin_count]){status=LT48_INVALID_ARGUMENT;return status;}
    }
    return LT48_OK;
}
lt48_status lt48_pof_vectors_execute_internal(const lt48_pal_resources *resources,
    const lt48_pof_file_metadata *vectors,unsigned margin,unsigned x_mode,
    const lt48_protocol_transport *api,void *context,lt48_vector_operation_result *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch_row=SIZE_MAX;
    lt48_status status=LT48_INVALID_ARGUMENT;
    lt48_vector_device_profile profile;
    if(!api||!api->write||!api->read)goto done;
    status=lt48_pof_vectors_validate_internal(resources,vectors,margin,x_mode);
    if(status!=LT48_OK)goto done;
    status=lt48_pal_resources_vector_profile_internal(resources,&profile);
    if(status!=LT48_OK)goto done;
    size_t stride=(size_t)vectors->pin_count+1;
    lt48_pal_wave_session *s=NULL;
    status=lt48_pal_wave_session_create(resources,api,context,&s);
    if(status!=LT48_OK)goto done;
    status=lt48_pof_vector_prepare_internal(s);
    unsigned voltages[2]={margin==0?475u:margin==1?450u:500u,margin==0?525u:550u};
    unsigned passes=margin==2?1:2;
    if(status==LT48_OK) {
        s->voltage[1]=s->voltage[2]=voltages[0];
        status=lt48_vector_power_internal(profile.ground,profile.ground_count,
            profile.supply,profile.supply_count,voltages[0],1,vector_owner_packet,s);
    }
    const unsigned char busy[]={0x40,1};
    if(status==LT48_OK)status=vector_owner_packet(s,busy,sizeof busy);
    if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->transport,1);
    for(unsigned pass=0;pass<passes&&status==LT48_OK;pass++) {
        if(pass) {
            const unsigned char discharge[]={0x4f,0xf4,1},settle[]={0x4f,100,0};
            status=vector_owner_supply(s,0);
            if(status==LT48_OK)status=vector_owner_packet(s,discharge,sizeof discharge);
            if(status==LT48_OK)status=vector_owner_supply(s,voltages[pass]);
            if(status==LT48_OK)status=vector_owner_packet(s,settle,sizeof settle);
        }
        unsigned threshold=voltages[pass]/33;threshold=threshold<2?0:threshold-2;
        if(threshold>15)threshold=15;
        unsigned char threshold_packet[]={0x0f,(unsigned char)threshold};
        if(status==LT48_OK)status=vector_owner_packet(s,threshold_packet,sizeof threshold_packet);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->transport);
        for(size_t row=0;row<vectors->row_count&&status==LT48_OK;row++) {
            lt48_vector_cycle_result result;
            status=lt48_vector_cycle_transport_internal(vectors->vectors+row*stride,
                vectors->pin_count,x_mode,api,context,&result);
            if(status!=LT48_OK)break;
            out->completed_rows++;
            if(result.mismatch_count) {
                out->first_mismatch_row=row;out->first_mismatch_centivolts=voltages[pass];
                out->mismatch=result;status=LT48_DEVICE_MISMATCH;
            }
        }
        if(status==LT48_OK)out->completed_passes++;
    }
    if(status==LT48_OK||status==LT48_DEVICE_MISMATCH) {
        out->cleanup_status=lt48_vector_power_internal(profile.ground,profile.ground_count,
            profile.supply,profile.supply_count,profile.centivolts,0,vector_owner_packet,s);
        if(out->cleanup_status==LT48_OK) {
            s->voltage[1]=s->voltage[2]=0;
            lt48_runtime_result *wave=NULL;
            out->cleanup_status=pal_run_wave(s,profile.power_off,10,NULL,0,&wave);
            lt48_runtime_result_close(wave);
        }
        if(out->cleanup_status==LT48_OK)out->cleanup_status=lt48_runtime_transport_session_flush(s->transport);
        if(out->cleanup_status==LT48_OK){out->cleanup_completed=1;s->requires_cleanup=0;}
    }
    if(s->requires_cleanup) {
        lt48_palce_recovery_result recovery;
        lt48_status recovered=lt48_pal_wave_session_recover(s,&recovery);
        if(out->cleanup_status==LT48_OK)out->cleanup_status=recovered;
        out->cleanup_completed=recovered==LT48_OK&&recovery.completed;
    }
    if(status==LT48_OK&&out->cleanup_status!=LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK||status==LT48_DEVICE_MISMATCH) {
        unsigned char indicator[]={0x40,status==LT48_OK?4:2};
        lt48_status light=vector_owner_packet(s,indicator,sizeof indicator);
        if(light==LT48_OK)light=lt48_runtime_transport_session_flush(s->transport);
        if(status==LT48_OK)status=light;
    }
    lt48_pal_wave_session_close(s);
done:
    out->operation_status=status;return status;
}
