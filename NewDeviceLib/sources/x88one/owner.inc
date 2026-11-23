/* X88C64 byte EEPROM. Reuse source pin compiler, power, original FPGA,
 * bounded page transport and cleanup. Unlike NOR, either bit direction is
 * writable. The original INIT intentionally clears BPR before every write. */
static lt48_status x88one_execute(const lt48_gaaf_serial_resources *r,
 const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,
 uint32_t start,size_t length,const unsigned char *image,size_t image_size,
 unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,
 unsigned margin_type,lt48_gaaf_serial_result *out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status status=lt48_gaaf_serial_validate(r,op,start,length,image,image_size,margin,margin_type);
 if(!status&&(!api||!api->read||!api->write))status=LT48_INVALID_ARGUMENT;
 if(!status&&op==LT48_OPERATION_READ&&(!read||read_size<length))status=LT48_BUFFER_TOO_SMALL;
 if(status)return out->operation_status=status;
 unsigned char *actual=malloc(r->capacity),*expected=malloc(r->capacity);
 if(!actual||!expected){free(actual);free(expected);return out->operation_status=LT48_NO_MEMORY;}
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE;
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);
 lt48_runtime_transport_session *wire=NULL;
 status=lt48_runtime_transport_session_create(api,ctx,&wire);
 if(!status)status=lt48_gaaf_serial_setup(r,&frame,wire);
 /* Capture the whole array before mutation; verify every neighbor afterwards. */
 size_t span=writing?r->capacity:length;
 uint32_t base=writing?0:start;
 if(!status)status=lt48_legacy_macronix6_stream_internal(r,&frame,wire,api,ctx,1,r->nominal,actual,span,base);
 if(!status&&writing){
  memcpy(expected,actual,r->capacity);
  if(op==LT48_OPERATION_ERASE)memset(expected,255,r->capacity);
  else memcpy(expected+start,image,length);
  uint32_t first=start&~31u;
  size_t extent=((size_t)start+length+31)&~(size_t)31;
  status=lt48_legacy_macronix6_stream_internal(r,&frame,wire,api,ctx,2,r->nominal,expected+first,extent-first,first);
 }
 if(!status&&(writing||op==LT48_OPERATION_VERIFY)){
  unsigned delta=margin_type==2?0:r->nominal/(margin_type?10:20);
  unsigned volts[2]={r->nominal-delta,r->nominal+delta},passes=margin_type==2?1:2;
  if(!writing){
   if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;passes=1;}
   else if(margin==LT48_VERIFY_MARGIN_LOW)passes=1;
   else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];passes=1;}
  }
  for(unsigned k=0;!status&&k<passes;k++){
   status=lt48_legacy_macronix6_stream_internal(r,&frame,wire,api,ctx,1,volts[k],actual,span,base);
   if(!status)status=compare(actual,writing?expected:image,base,span,0,out);
  }
 }
 if(!status&&op==LT48_OPERATION_BLANK_CHECK)status=compare(actual,NULL,start,length,1,out);
 if(!status&&op==LT48_OPERATION_COMPARE)status=compare(actual,image,start,length,0,out);
 lt48_runtime_transport_session_close(wire);
 lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
 out->operation_status=status;
 out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,&cleanup,ctx,status);
 out->cleanup_completed=!out->cleanup_status;
 if(!status)status=out->cleanup_status;
 if(!status&&writing){
  lt48_gaaf_serial_result fresh;
  status=x88one_execute(r,api,ctx,LT48_OPERATION_READ,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,margin_type,&fresh);
  if(!fresh.cleanup_completed){out->cleanup_completed=0;out->cleanup_status=fresh.cleanup_status;}
  if(!status)status=compare(actual,expected,0,r->capacity,0,out);
 }
 if(!status){if(op==LT48_OPERATION_READ)memcpy(read,actual,length);out->completed_bytes=length;}
 out->operation_status=status;free(expected);free(actual);return status;
}
