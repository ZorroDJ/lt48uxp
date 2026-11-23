/* Authored OTP memory owner reusing the audited XP transfer and cleanup. */
static lt48_status xp13_otp_compare(xp_session*s,uint32_t start,size_t length,const unsigned char*image,
 unsigned char*pending,unsigned blank,uint16_t vcc,lt48_xp_flash_result*out){
 lt48_status st=cycle(s,1,start,length,NULL,pending,vcc,out);
 if(st==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=(blank?255:image[i])){if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;}
 return st==LT48_OK&&out->mismatch_count?LT48_DEVICE_MISMATCH:st;
}
static lt48_status xp13_otp_execute(const lt48_xp_flash_resources*r,const lt48_protocol_transport*api,void*ctx,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void*decision_ctx,lt48_xp_flash_result*out){
 memset(out,0,sizeof*out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_xp_flash_validate(r,op,start,length,image,size);if(st!=LT48_OK)return st;
 if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
 st=lt48_xp_flash_validate_verify_options(r,op,margin,margin_type);if(st!=LT48_OK)return st;
 unsigned char*pending=malloc(length);if(!pending)return LT48_NO_MEMORY;
 xp_session s={0};s.r=r;s.api=api;s.context=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st!=LT48_OK){free(pending);return st;}
 st=setup(&s);
 if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)){
  st=cycle(&s,1,start,length,NULL,pending,r->nominal,out);
  for(size_t i=0;st==LT48_OK&&i<length;i++)if((op==LT48_OPERATION_PROGRAM&&pending[i]!=255)||(pending[i]&image[i])!=image[i]){out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;st=LT48_DEVICE_MISMATCH;}
  if(st==LT48_OK)st=cycle(&s,2,start,length,image,NULL,r->nominal,out);
  for(unsigned i=0;st==LT48_OK&&i<2;i++)st=xp13_otp_compare(&s,start,length,image,pending,0,margin_voltage(r,margin_type,i),out);
 }else if(st==LT48_OK&&op==LT48_OPERATION_READ)st=cycle(&s,1,start,length,NULL,pending,r->nominal,out);
 else if(st==LT48_OK){
  uint16_t v[2]={r->nominal,r->nominal};unsigned n=1;
  if(op==LT48_OPERATION_VERIFY){if(margin==LT48_VERIFY_MARGIN_ALL){n=2;v[0]=margin_voltage(r,margin_type,0);v[1]=margin_voltage(r,margin_type,1);}else if(margin!=LT48_VERIFY_MARGIN_NOMINAL)v[0]=margin_voltage(r,margin_type,margin==LT48_VERIFY_MARGIN_HIGH);}
  for(unsigned i=0;st==LT48_OK&&i<n;i++)st=xp13_otp_compare(&s,start,length,image,pending,op==LT48_OPERATION_BLANK_CHECK,v[i],out);
 }
 out->operation_status=st;out->cleanup_status=sst_page_cleanup(&s,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(st==LT48_OK)st=out->cleanup_status;
 if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
 if(st==LT48_OK&&op!=LT48_OPERATION_READ){
  lt48_xp_flash_result fresh;st=xp13_otp_execute(r,api,ctx,LT48_OPERATION_READ,start,length,NULL,0,pending,length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_ctx,&fresh);
  out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
  if(st==LT48_OK){for(size_t i=0;i<length;i++)if(pending[i]!=(op==LT48_OPERATION_BLANK_CHECK?255:image[i])){if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;}if(out->mismatch_count)st=LT48_DEVICE_MISMATCH;}
  if(st!=LT48_OK){lt48_protocol_transport safe=*api;safe.cancelled=NULL;lt48_runtime_transport_session*w=NULL;lt48_status led=lt48_runtime_transport_session_create(&safe,ctx,&w);unsigned char error[]={0x40,2};if(led==LT48_OK)led=lt48_runtime_transport_session_controller_write(w,error,2);if(led==LT48_OK)led=lt48_runtime_transport_session_flush(w);lt48_runtime_transport_session_close(w);if(led!=LT48_OK){out->cleanup_status=led;out->cleanup_completed=0;}}
 }
 out->operation_status=st;free(pending);return st;
}
