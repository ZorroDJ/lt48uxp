/* H8 M4/M5/M6 source Program/Verify plus authored raw Read.
 * No source Erase operation exists. Do not synthesize one from Program FF.
 * Partial reads/verification stream the source array from zero; programming
 * requires the entire blank source image, preserving no invented regions. */
static lt48_status h15_validate(const lt48_hitachi_resources *r,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,
 size_t size,lt48_verify_margin_selection margin,unsigned type) {
 if(!r||!length||start>=r->core->capacity||length>r->core->capacity-start)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||size!=length))return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_PROGRAM&&(start||length!=r->core->capacity))return LT48_SAFETY_REJECTED;
 if(type>2||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
static lt48_status h15_execute(const lt48_hitachi_resources *owner,
 const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,uint32_t start,
 size_t length,const unsigned char *image,size_t image_size,unsigned char *output,
 size_t output_size,lt48_verify_margin_selection margin,unsigned type,
 lt48_hitachi_result *out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status s=h15_validate(owner,op,start,length,image,image_size,margin,type);
 if(s==LT48_OK&&(!api||!api->read||!api->write))s=LT48_INVALID_ARGUMENT;
 if(s==LT48_OK&&op==LT48_OPERATION_READ&&(!output||output_size<length))s=LT48_BUFFER_TOO_SMALL;
 if(s!=LT48_OK){out->operation_status=s;return s;}
 const lt48_gaaf_serial_resources *r=owner->core;size_t cap=r->capacity;
 unsigned char *actual=malloc(cap),*expected=malloc(cap);
 if(!actual||!expected){free(actual);free(expected);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 lt48_gaaf_serial_frame f;lt48_gaaf_serial_frame_init(r,&f);
 lt48_runtime_transport_session *wire=NULL;
 s=lt48_runtime_transport_session_create(api,ctx,&wire);
 if(s==LT48_OK)s=lt48_gaaf_serial_setup(r,&f,wire);
#define H15_STREAM(mode,voltage,data) lt48_gaaf_serial_stream(r,&f,wire,api,ctx,mode,voltage,data,cap)
 if(s==LT48_OK)s=H15_STREAM(1,r->nominal,actual);
 if(s==LT48_OK){
  memcpy(expected,actual,cap);
  if(op==LT48_OPERATION_PROGRAM){
   s=compare(actual,NULL,0,cap,out);
   if(s==LT48_OK)memcpy(expected,image,cap);
  }else if(op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE){
   s=compare(actual+start,image,start,length,out);
   memcpy(expected+start,image,length);
  }else if(op==LT48_OPERATION_BLANK_CHECK){
   s=compare(actual+start,NULL,start,length,out);
   memset(expected+start,255,length);
  }
 }
 if(s==LT48_OK&&op==LT48_OPERATION_PROGRAM)s=H15_STREAM(2,r->nominal,expected);
 if(s==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_BLANK_CHECK)){
  unsigned delta=type==2?0:r->nominal/(type?10:20);
  unsigned volts[2]={r->nominal-delta,r->nominal+delta},n=type==2?1:2;
  if(op==LT48_OPERATION_BLANK_CHECK){volts[0]=r->nominal;n=1;}
  else if(op==LT48_OPERATION_VERIFY){
   if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
   else if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
   else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
  }
  for(unsigned i=0;s==LT48_OK&&i<n;i++)s=H15_STREAM(4,volts[i],expected);
  if(s==LT48_OK)s=H15_STREAM(1,r->nominal,actual);
  if(s==LT48_OK)s=compare(actual,expected,0,cap,out);
 }
 out->operation_status=s;lt48_runtime_transport_session_close(wire);wire=NULL;
 out->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,api,ctx,s);
 if(s==LT48_OK&&out->cleanup_status==LT48_OK&&op==LT48_OPERATION_PROGRAM){
  lt48_gaaf_serial_frame_init(r,&f);
  s=lt48_runtime_transport_session_create(api,ctx,&wire);
  if(s==LT48_OK)s=lt48_gaaf_serial_setup(r,&f,wire);
  if(s==LT48_OK)s=H15_STREAM(1,r->nominal,actual);
  if(s==LT48_OK)s=compare(actual,expected,0,cap,out);
  out->operation_status=s;lt48_runtime_transport_session_close(wire);
  out->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,api,ctx,s);
 }
 out->cleanup_completed=out->cleanup_status==LT48_OK;
 if(s==LT48_OK)s=out->cleanup_status;
 if(s==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(output,actual+start,length);}
 free(actual);free(expected);return s;
#undef H15_STREAM
}
