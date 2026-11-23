/* GC/GCB full operation ownership. Included by pal_session.c to retain the
 * source-bound frame and voltage variables across stages. No USB setup or
 * permanent programmer state is changed here. */
static lt48_status pof_end_range(lt48_pal_wave_session *s,lt48_status operation) {
    if(operation!=LT48_OK&&operation!=LT48_DEVICE_MISMATCH)return operation;
    lt48_runtime_result *r=NULL;
    lt48_status reset=pal_run_wave(s,s->reset,s->lifecycle_mode,NULL,0,&r);
    lt48_runtime_result_close(r);
    if(reset==LT48_OK)s->pof_range_reset=1;
    return reset==LT48_OK?operation:reset;
}
static lt48_status pof_range(lt48_pal_wave_session *s,const lt48_pof_trs_source *source,
    unsigned mode,const unsigned char *input,size_t size,unsigned char *output,
    lt48_pof_operation_result *result) {
    lt48_status status=LT48_OK;
    if(!s->requires_cleanup)status=lt48_pal_wave_session_enter_mode(s,mode);
    else if(s->pof_range_reset||s->lifecycle_mode!=mode)
        status=lt48_pal_wave_session_switch_mode(s,mode);
    if(status==LT48_OK)status=lt48_pof_session_image_internal(s,source,mode,input,size,output,&result->traversal);
    return pof_end_range(s,status);
}
static lt48_status pof_verify(lt48_pal_wave_session *s,const lt48_pof_trs_source *source,
    const unsigned char *input,size_t size,lt48_pof_operation_result *result) {
    uint32_t margins[2]={0,0};unsigned bound=0;
    if(lt48_pal_resources_pof_internal(s->resources)==1) {
        const char *names[]={"_VAR_VerifyVCC0","_VAR_VerifyVCC1"};
        for(unsigned i=0;i<2;i++) {
            uint16_t id;lt48_status status=lt48_pal_resources_parameter_internal(s->resources,names[i],&id,&margins[i]);
            if(status==LT48_OK)bound++;
            else if(status!=LT48_NOT_FOUND)return status;
        }
    }
    if(!bound) {
        lt48_status status=pof_range(s,source,4,input,size,NULL,result);
        if(status==LT48_OK)result->verify_passes++;
        return status;
    }
    if(!margins[0]&&!margins[1])return LT48_FORMAT_ERROR;
    uint16_t vcc_id;uint32_t value;
    lt48_status status=lt48_pal_resources_parameter_internal(s->resources,"_VAR_VCC",&vcc_id,&value);
    if(status!=LT48_OK)return status;
    size_t index=0;while(index<s->variable_count&&s->variables[index].variable_id!=vcc_id)index++;
    if(index==s->variable_count)return LT48_FORMAT_ERROR;
    uint16_t saved=s->variables[index].value;
    for(unsigned i=0;i<2&&status==LT48_OK;i++) {
        if(!margins[i])continue;
        if(margins[i]>UINT16_MAX){status=LT48_FORMAT_ERROR;break;}
        s->variables[index].value=(uint16_t)margins[i];
        status=pof_range(s,source,4,input,size,NULL,result);
        if(status==LT48_OK)result->verify_passes++;
    }
    s->variables[index].value=saved;
    return status;
}
lt48_status lt48_pof_execute_internal(const lt48_pal_resources *resources,
    const lt48_pof_trs_source *source,const lt48_protocol_transport *api,
    void *context,lt48_operation_kind operation,const unsigned char *input,
    size_t input_size,unsigned char *output,size_t output_size,
    lt48_pof_operation_result *result) {
    if(!resources||!source||!api||!api->write||!api->read||!result)return LT48_INVALID_ARGUMENT;
    memset(result,0,sizeof(*result));result->traversal.first_mismatch=SIZE_MAX;
    const lt48_pof_core *core=lt48_pal_resources_pof_core_internal(resources);
    if(!core)return LT48_UNSUPPORTED;
    int program=operation==LT48_OPERATION_PROGRAM||operation==LT48_OPERATION_OVER_PROGRAM;
    int verify=operation==LT48_OPERATION_VERIFY||operation==LT48_OPERATION_COMPARE;
    int read=operation==LT48_OPERATION_READ,blank=operation==LT48_OPERATION_BLANK_CHECK;
    int secure=operation==LT48_OPERATION_SECURE;
    if(!program&&!verify&&!read&&!blank&&!secure)return LT48_UNSUPPORTED;
    size_t size=core->total_bytes;
    if((program||verify)&&(!input||input_size!=size))return LT48_INVALID_ARGUMENT;
    if(read&&(!output||output_size<size))return LT48_BUFFER_TOO_SMALL;
    uint16_t secure_op=0;
    lt48_status status=LT48_OK;
    if(secure)status=lt48_pal_resources_operation_internal(resources,"_FUN_SECU",&secure_op);
    unsigned selected;const unsigned char *fpga;size_t fpga_size;
    if(status==LT48_OK)status=lt48_pal_resources_pof_fpga_internal(resources,&selected,&fpga,&fpga_size);
    if(status!=LT48_OK)return status;
    unsigned char *pending=read?malloc(size):NULL;
    if(read&&!pending)return LT48_NO_MEMORY;
    lt48_pal_wave_session *s=NULL;
    status=lt48_pal_wave_session_create(resources,api,context,&s);
    unsigned mode=secure?8:read?1:blank||operation==LT48_OPERATION_PROGRAM?11:program?2:4;
    /* Verify's margin must be selected before DevInit; defer first startup
     * to pof_range in that path, after assigning the source VCC variable. */
    if(status==LT48_OK&&!verify)status=lt48_pal_wave_session_enter_mode(s,mode);
    if(status==LT48_OK) {
        if(read||blank)status=pof_range(s,source,mode,NULL,size,pending,result);
        else if(secure) {
            lt48_runtime_result *wave=NULL;
            status=pal_run_wave(s,secure_op,8,NULL,0,&wave);
            lt48_runtime_result_close(wave);
            status=pof_end_range(s,status);
            if(status==LT48_OK)result->secured=1;
        } else {
            if(operation==LT48_OPERATION_PROGRAM)status=pof_range(s,source,11,NULL,size,NULL,result);
            if(status==LT48_OK&&program) {
                status=pof_range(s,source,2,input,size,NULL,result);
                if(status==LT48_OK)result->programmed=1;
            }
            if(status==LT48_OK)status=pof_verify(s,source,input,size,result);
        }
    }
    if(s&&s->requires_cleanup) {
        if(status==LT48_OK) {
            result->cleanup_status=lt48_pal_wave_session_finish_mode(s);
            if(result->cleanup_status!=LT48_OK) {
                lt48_palce_recovery_result recovery;
                lt48_status recovered=lt48_pal_wave_session_recover(s,&recovery);
                result->cleanup_completed=recovered==LT48_OK&&recovery.completed;
                status=result->cleanup_status;
            } else result->cleanup_completed=1;
        } else {
            lt48_palce_recovery_result recovery;
            result->cleanup_status=lt48_pal_wave_session_recover(s,&recovery);
            result->cleanup_completed=result->cleanup_status==LT48_OK&&recovery.completed;
        }
    }
    if(s&&status==LT48_OK) {
        const unsigned char good[]={0x40,4};
        status=lt48_runtime_transport_session_controller_write(s->transport,good,sizeof good);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->transport);
    }
    if(status!=LT48_OK)(void)lt48_runtime_transport_error_indicator_internal(api,context);
    if(status==LT48_OK&&read)memcpy(output,pending,size);
    result->operation_status=status;
    free(pending);lt48_pal_wave_session_close(s);return status;
}
