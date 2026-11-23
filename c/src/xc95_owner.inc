/* TXC95Group 0045cfe0 / 0045d0f0 / 0045d440. Included in the shared PLD
 * session unit to reuse its exact frame, row traversal and cleanup. */
static unsigned xc95_parameter(const lt48_pal_wave_session *s,unsigned id) {
 for(size_t i=0;i<s->variable_count;i++)if(s->variables[i].variable_id==id)return s->variables[i].value;
 return 0;
}
static lt48_status xc95_wave(lt48_pal_wave_session *s,unsigned op,unsigned mode) {
 lt48_runtime_result *result=NULL;lt48_status status=pal_run_wave(s,op,mode,NULL,0,&result);
 lt48_runtime_result_close(result);return status;
}
static lt48_status xc95_blank(lt48_pal_wave_session *s,lt48_pal_operation_result *out) {
 lt48_status status=lt48_pal_wave_session_switch_mode(s,11);
 if(status==LT48_OK)status=lt48_pal_wave_session_blank_check(s,&out->comparison);
 return status;
}
static lt48_status xc95_erase(lt48_pal_wave_session *s,lt48_pal_operation_result *out) {
 unsigned limit=xc95_parameter(s,15);if(!limit||limit>30)return LT48_FORMAT_ERROR;
 for(unsigned attempt=0;attempt<limit;attempt++) {
  lt48_status status=lt48_pal_wave_session_switch_mode(s,7);
  if(status==LT48_OK)status=xc95_wave(s,6,7);
  if(status==LT48_OK)status=xc95_blank(s,out);
  if(status!=LT48_OK)return status;
  if(out->comparison.matched){out->erased=1;return LT48_OK;}
 }
 return LT48_DEVICE_MISMATCH;
}
lt48_status lt48_xc95_execute_internal(const lt48_pal_resources *r,
 const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,
 const unsigned char *input,size_t input_count,unsigned char *read,size_t read_size,
 unsigned protection,lt48_pal_operation_result *out) {
 if(!r||!api||!api->write||!api->read||!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->comparison.first_mismatch=SIZE_MAX;
 if(!lt48_pal_resources_xc95_internal(r))return LT48_UNSUPPORTED;
 int program=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 int expected=program||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE;
 if(!expected&&op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
 size_t count=lt48_pal_resources_fuse_code(r)->total_fuse_count;
 unsigned max_protection=lt48_pal_resources_family_code(r)==4?1:3;
 if(protection>max_protection||(op==LT48_OPERATION_SECURE&&!protection))return LT48_INVALID_ARGUMENT;
 if(expected&&(!input||input_count!=count))return LT48_INVALID_ARGUMENT;
 if(expected)for(size_t i=0;i<count;i++)if(input[i]>1)return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_READ&&(!read||read_size<count))return LT48_BUFFER_TOO_SMALL;
 unsigned char *pending=op==LT48_OPERATION_READ?malloc(count):NULL;
 if(op==LT48_OPERATION_READ&&!pending)return LT48_NO_MEMORY;
 lt48_pal_wave_session *s=NULL;lt48_status status=lt48_pal_wave_session_create(r,api,ctx,&s);
 if(status==LT48_OK) {
  s->configuration[0]=(lt48_runtime_configuration_value){0,0,0,protection&1};
  s->configuration[1]=(lt48_runtime_configuration_value){0,0,1,(protection>>1)&1};
  s->configuration_count=2;
  status=lt48_pal_wave_session_enter_mode(s,op==LT48_OPERATION_SECURE?8:op==LT48_OPERATION_ERASE?7:program||op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_READ?1:4);
 }
 if(status==LT48_OK) {
  if(op==LT48_OPERATION_READ)status=lt48_pal_wave_session_read_fuses(s,1,pending,count,&out->completed_cells);
  else if(op==LT48_OPERATION_ERASE)status=xc95_erase(s,out);
  else if(op==LT48_OPERATION_BLANK_CHECK) {
   status=lt48_pal_wave_session_blank_check(s,&out->comparison);
   if(status==LT48_OK&&!out->comparison.matched)status=LT48_DEVICE_MISMATCH;
  } else if(op==LT48_OPERATION_SECURE) {
   status=xc95_wave(s,7,8);
   /* Original SECU keeps failed selections set in flag1/flag2 after its
    * bounded programming/readback loop. Do not report that as success. */
   if(status==LT48_OK&&(s->buffers[14][0]||(max_protection==3&&s->buffers[15][0])))status=LT48_DEVICE_MISMATCH;
   if(status==LT48_OK)out->secured=1;
  } else {
   if(program) {
    if(op==LT48_OPERATION_PROGRAM) {
     status=lt48_pal_wave_session_blank_check(s,&out->comparison);
     if(status==LT48_OK&&!out->comparison.matched)status=xc95_erase(s,out);
    }
    unsigned limit=xc95_parameter(s,14);if(!limit||limit>25)status=LT48_FORMAT_ERROR;
    for(unsigned attempt=0;status==LT48_OK&&attempt<limit;attempt++) {
     status=lt48_pal_wave_session_switch_mode(s,2);
     if(status==LT48_OK)status=lt48_pal_wave_session_program_fuses(s,input,count,&out->completed_cells);
     if(status==LT48_OK)status=lt48_pal_wave_session_switch_mode(s,3);
     if(status==LT48_OK)status=lt48_pal_wave_session_verify_fuses(s,3,input,count,&out->comparison);
     if(status!=LT48_OK||out->comparison.matched)break;
     if(attempt+1==limit)status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)out->programmed=1;
   }
   if(status==LT48_OK)status=lt48_pal_wave_session_switch_mode(s,4);
   if(status==LT48_OK)status=lt48_pal_wave_session_verify_fuses(s,4,input,count,&out->comparison);
   if(status==LT48_OK&&!out->comparison.matched)status=LT48_DEVICE_MISMATCH;
  }
 }
 out->operation_status=status;
 if(s&&s->requires_cleanup) {
  if(status==LT48_OK) {
   out->cleanup_status=lt48_pal_wave_session_finish_mode(s);
   if(out->cleanup_status!=LT48_OK){lt48_palce_recovery_result recovery;lt48_status recovered=lt48_pal_wave_session_recover(s,&recovery);out->cleanup_completed=recovered==LT48_OK&&recovery.completed;}
  }
  else {lt48_palce_recovery_result recovery;out->cleanup_status=lt48_pal_wave_session_recover(s,&recovery);}
  if(out->cleanup_status==LT48_OK)out->cleanup_completed=1;
  if(out->cleanup_status!=LT48_OK&&status==LT48_OK)status=out->cleanup_status;
 }
 if(s&&status==LT48_OK){const unsigned char good[]={0x40,4};status=lt48_runtime_transport_session_controller_write(s->transport,good,sizeof(good));if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->transport);}
 if(status!=LT48_OK)(void)lt48_runtime_transport_error_indicator_internal(api,ctx);
 if(status==LT48_OK&&pending)memcpy(read,pending,count);
 out->operation_status=status;free(pending);lt48_pal_wave_session_close(s);return status;
}
