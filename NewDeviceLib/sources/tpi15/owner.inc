/* Original TPI entry, raw memory transport and source configuration restore. */
static lt48_status tpi15_cfg(xp_session*s,unsigned*flags,unsigned*lock){
 s->config_count=0;lt48_status st=run(s,6);if(st)return st;
 unsigned paged=s->r->closure->definition>=0x160, n=s->r->closure->definition==0x15e||s->r->closure->definition==0x15f?4:3,seen=0;*flags=0;*lock=UINT32_MAX;
 for(size_t i=0;i<s->config_count;i++){lt48_runtime_configuration_value*v=s->config+i;
  if(v->category==0&&v->field<n){if(v->value>1)return LT48_PROTOCOL_ERROR;*flags|=(unsigned)v->value<<v->field;seen|=1u<<v->field;}
  if(v->category==1&&v->field==paged)*lock=(unsigned)v->value;
  if(paged&&v->category==1&&v->field==0){if(v->value>3)return LT48_PROTOCOL_ERROR;*flags|=v->value<<8;seen|=1u<<n;}
 }
 return seen==((1u<<(n+paged))-1)&&*lock<=2?LT48_OK:LT48_PROTOCOL_ERROR;
}
static lt48_status tpi15_execute(const lt48_xp_flash_resources*r,const lt48_protocol_transport*api,void*ctx,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void*decision_ctx,lt48_xp_flash_result*out){
 memset(out,0,sizeof*out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_xp_flash_validate(r,op,start,length,image,size);if(st)return st;
 if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
 st=lt48_xp_flash_validate_verify_options(r,op,margin,margin_type);if(st)return st;
 unsigned char*p=malloc(length);if(!p)return LT48_NO_MEMORY;
 xp_session s={0};s.r=r;s.api=api;s.context=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);unsigned flags=0,lock=0;
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(p);return st;}st=setup(&s);
 if(!st)st=identify(&s,decide,decision_ctx);
 if(st==LT48_OK)st=tpi15_cfg(&s,&flags,&lock);
 if(st==LT48_OK&&lock&&op!=LT48_OPERATION_ERASE)st=LT48_DEVICE_MISMATCH;
 if(st==LT48_OK&&op==LT48_OPERATION_ERASE){
  st=run(&s,10);
  /* Erase source also erases configuration: restore selected fuse bits,
   * leave memory lock clear, never touch signature/calibration sections. */
  if(st==LT48_OK){for(size_t i=0;i<s.config_count;i++){lt48_runtime_configuration_value*v=s.config+i;if(v->category==0)v->value=flags>>v->field&1;else if(v->category==1)v->value=r->closure->definition>=0x160&&v->field==0?flags>>8:0;}st=run(&s,11);}
 }else if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)){
  st=cycle(&s,1,0,length,NULL,p,r->nominal,out);
  for(size_t i=0;st==LT48_OK&&i<length;i++)if((op==LT48_OPERATION_PROGRAM&&p[i]!=255)||(p[i]&image[i])!=image[i]){out->first_mismatch=i;out->mismatch_count++;st=LT48_DEVICE_MISMATCH;}
  if(st==LT48_OK)st=cycle(&s,2,0,length,image,NULL,r->nominal,out);
 }
 if(st==LT48_OK){
  unsigned passes=(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||(op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL))?2:1;
  for(unsigned k=0;st==LT48_OK&&k<passes;k++){
   uint16_t volts=passes==2?margin_voltage(r,margin_type,k):op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_NOMINAL?margin_voltage(r,margin_type,margin==LT48_VERIFY_MARGIN_HIGH):r->nominal;
   st=cycle(&s,1,0,length,NULL,p,volts,out);
   if(st==LT48_OK&&op!=LT48_OPERATION_READ)for(size_t i=0;i<length;i++)if(p[i]!=(op==LT48_OPERATION_ERASE||op==LT48_OPERATION_BLANK_CHECK?255:image[i])){if(!out->mismatch_count)out->first_mismatch=i;out->mismatch_count++;}
   if(out->mismatch_count)st=LT48_DEVICE_MISMATCH;
  }
 }
 if(st==LT48_OK){unsigned after,locked;st=tpi15_cfg(&s,&after,&locked);if(st==LT48_OK&&(after!=flags||locked))st=LT48_DEVICE_MISMATCH;}
 out->operation_status=st;out->cleanup_status=sst_page_cleanup(&s,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(!st)st=out->cleanup_status;
 if(!st&&op!=LT48_OPERATION_READ){
  xp_session fresh={0};fresh.r=r;fresh.api=api;fresh.context=ctx;fresh.mode=1;memcpy(fresh.vars,r->vars,sizeof fresh.vars);
  st=lt48_runtime_transport_session_create(api,ctx,&fresh.wire);
  if(!st){unsigned after,locked;st=setup(&fresh);if(!st)st=tpi15_cfg(&fresh,&after,&locked);if(!st&&(after!=flags||locked))st=LT48_DEVICE_MISMATCH;
   if(!st)st=cycle(&fresh,1,0,length,NULL,p,r->nominal,out);
   if(!st)for(size_t i=0;i<length;i++)if(p[i]!=(op==LT48_OPERATION_ERASE||op==LT48_OPERATION_BLANK_CHECK?255:image[i])){if(!out->mismatch_count)out->first_mismatch=i;out->mismatch_count++;}
   if(out->mismatch_count)st=LT48_DEVICE_MISMATCH;
   out->cleanup_status=sst_page_cleanup(&fresh,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(!st)st=out->cleanup_status;
  }
 }
 if(!st&&op==LT48_OPERATION_READ)memcpy(read,p,length);
 free(p);out->operation_status=st;return st;
}
