/* Authored finite class2013 host owner. Original compact callbacks and pin maps
 * provide all target cycles. Byte captures, not a callback's expected-data echo,
 * establish Read/Blank/Verify results. Physical qualification remains separate. */
static int lm20_mapped(const lt48_mcu51_resources*r,unsigned a){return r->lm20->group!=3||a<0x4000||(a>=0xc000&&(a&63u)<36);}
static lt48_status lm20_validate(const lt48_mcu51_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||!r->lm20||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;
 const struct lm20_profile*p=r->lm20;
 if(p->group==8&&!LT48_HMS99ONE_ADMITTED)return LT48_UNSUPPORTED;
 if(op==10){
  if(!p->configuration)return LT48_UNSUPPORTED;
  if(a||n!=r->capacity)return LT48_SAFETY_REJECTED;
  if(!im&&!size)return LT48_OK;
  if(p->group!=7||!im||size!=5||!im[0]||(im[0]&~15u))return LT48_INVALID_ARGUMENT;
  for(unsigned i=0;i<4;i++)if((im[0]>>i&1)&&im[i+1]>1)return LT48_INVALID_ARGUMENT;
  return LT48_OK;
 }
 if(op==7||op==8)return !(op==7?p->erase:p->secure)?LT48_UNSUPPORTED:(a||n!=r->capacity)?LT48_SAFETY_REJECTED:LT48_OK;
 if(op<1||op>6)return LT48_UNSUPPORTED;
 if(op>=3&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if(op==3||op==4){
  if(a||n!=r->capacity)return LT48_SAFETY_REJECTED;
  if(p->group==3)for(unsigned i=0;i<r->capacity;i++)if(!lm20_mapped(r,i)&&im[i])return LT48_SAFETY_REJECTED;
 }
 return LT48_OK;
}
static void lm20_addr(mcu_session*s,unsigned a){for(unsigned i=0;i<4;i++)s->buffers[4][i]=(unsigned char)(a>>(8*i));}
static void lm20_seed(mcu_session*s){
 s->l14count=0;
 s->l14cfg[s->l14count++]=(lt48_runtime_configuration_value){-1,0,1,1};
 s->l14cfg[s->l14count++]=(lt48_runtime_configuration_value){0,0,1,1};
 unsigned fields[]={0,3,5};
 for(unsigned i=0;i<3;i++)s->l14cfg[s->l14count++]=(lt48_runtime_configuration_value){0,1,fields[i],0};
 for(unsigned i=18;i<=24;i++)s->l14cfg[s->l14count++]=(lt48_runtime_configuration_value){0,0,i,0};
if(s->r->lm20->group==8)s->l14cfg[s->l14count++]=(lt48_runtime_configuration_value){0,1,6,0};
}
static lt48_status lm20_init(mcu_session*s,lt48_m37_id_decision_fn decide,void*ctx){
 for(unsigned attempt=0;attempt<64;attempt++){
  unsigned group=s->r->lm20->group;
  if(group==1&&s->mode==1)l14_cfg(s,0,1,0)->value=UINT32_MAX;
  lt48_status st=run(s,4);
  if(!st){
   if(group==1&&s->mode==1&&l14_cfg(s,0,1,0)->value>3)return LT48_FORMAT_ERROR;
   if(group==5&&s->mode==1&&!l14_cfg(s,0,1,3)->value&&s->buffers[5][0]!=0xc8)return LT48_FORMAT_ERROR;
   if(group==7&&s->mode==1&&!(s->buffers[5][0]&1))return LT48_SAFETY_REJECTED;
   return st;
  }
  if(s->fault==9)return LT48_SAFETY_REJECTED;
  if(s->fault!=5&&s->fault!=6)return st;
  unsigned g=s->r->lm20->group,expected=s->buffers[13][0],actual=s->buffers[5][0];
  lt48_compile_variable*mid=var(s,g==3?37:38),*did=var(s,36);
  unsigned manufacturer=mid?mid->value:0,device=did?did->value:0;
  if(g==5){manufacturer=0x15;device=s->lm20_captures<3?0xea:1;}
  if(g==3||g==7)expected=s->fault==5?manufacturer:device;
  if(g==5)expected=s->lm20_captures==1?manufacturer:device;
  if(!decide)return LT48_DEVICE_MISMATCH;
  lt48_m37_id_choice c=decide(ctx,manufacturer,device,expected==manufacturer?actual:manufacturer,expected==manufacturer?0:actual);
  if(c==LT48_M37_ID_ABORT)return LT48_DEVICE_MISMATCH;
  if(c==LT48_M37_ID_IGNORE){
   s->ignore_id=1;l14_cfg(s,-1,0,1)->value=0;l14_cfg(s,0,0,1)->value=0;
   if(g==1){if(mid&&expected==mid->value)mid->value=actual;else if(did)did->value=actual;}
   if(g==3){lt48_compile_variable*v=s->fault==5?mid:did;if(v)v->value=actual;}
  }else if(c!=LT48_M37_ID_RETRY)return LT48_INVALID_ARGUMENT;
 }
 return LT48_DEVICE_MISMATCH;
}
static unsigned lm20_level(mcu_session*s){
 unsigned g=s->r->lm20->group;
 if(g==1)return l14_cfg(s,0,1,0)->value;
 if(g==5)return l14_cfg(s,0,1,3)->value;
 if(g==6)return l14_cfg(s,0,1,5)->value;
 if(g==7){unsigned flags=0;for(unsigned i=0;i<4;i++)flags|=l14_cfg(s,0,0,18+i)->value<<i;return flags;}
 if(g==4){unsigned flags=0;for(unsigned i=0;i<3;i++)flags|=l14_cfg(s,0,0,22+i)->value<<i;return flags;}
 return 0;
}
static lt48_status lm20_visit(mcu_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv,lt48_m37_id_decision_fn decide,void*ctx,lt48_mcu51_result*z){
 unsigned begin=s->r->lm20->group==3?0:a,end=a+(unsigned)n;
 if(s->r->lm20->group==8){s->l14image=writing?(unsigned char*)im:NULL;s->l14image_size=writing?n:0;} /* Interpreter clones this read-only source image. */
 s->mode=writing?2:1;lm20_addr(s,begin);lt48_compile_variable*v=var(s,0);if(v)v->value=cv;
 /* Sentinel prevents an unrecognised security encoding inheriting unlocked. */
 if(s->r->lm20->group==1)l14_cfg(s,0,1,0)->value=UINT32_MAX;
 lt48_status st=lm20_init(s,decide,ctx);
 if(!st&&!writing){
  unsigned g=s->r->lm20->group;
  if(g==7){if(!(s->buffers[5][0]&1))st=LT48_SAFETY_REJECTED;}
  else if(s->r->lm20->configuration&&lm20_level(s))st=LT48_SAFETY_REJECTED;
  if(g==5&&!lm20_level(s)&&s->buffers[5][0]!=0xc8)st=LT48_SAFETY_REJECTED;
 }
 for(unsigned i=begin;!st&&i<end;i++){
  lm20_addr(s,i);s->buffers[5][0]=255;s->buffers[7][0]=writing?im[i-a]:255;
  st=run(s,writing?7:6);
  if(!st&&out&&i>=a)out[i-a]=s->buffers[5][0];
  if(s->api->progress&&((i&127u)==0||i+1==end))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,i+1-begin,end-begin,0,0,0);
 }
 if(st&&writing&&s->r->lm20->group==8)s->mode=1; /* Failed writes must not enter the file-replaying mode2 postamble. */
 lt48_status reset=run(s,5);if(!st)st=reset;if(!st)z->completed_bytes=n;return st;
}
static lt48_status lm20_compare(mcu_session*s,lt48_mcu51_result*z,unsigned a,const unsigned char*actual,const unsigned char*expected,size_t n){
 z->mismatch_count=0;z->first_mismatch=UINT32_MAX;
 for(size_t i=0;i<n;i++){unsigned want=expected?expected[i]:(!lm20_mapped(s->r,a+(unsigned)i)?0:255);
  if(actual[i]!=want){if(!z->mismatch_count)z->first_mismatch=a+(unsigned)i;z->mismatch_count++;}}
 return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status lm20_execute(const lt48_mcu51_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,const lt48_mcu51_options*options,lt48_m37_id_decision_fn decide,void*dc,lt48_mcu51_result*z){
 if(!api||!api->write||!api->read||!z)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;
 lt48_mcu51_options defaults={LT48_VERIFY_MARGIN_ALL,0,1,NULL,0};if(!options)options=&defaults;
 lt48_status st=lm20_validate(r,op,a,n,im,size);if(!st)st=lt48_mcu51_validate_options(r,op,options);
 if(!st&&(op==1||op==10)&&(!read||read_size<(op==10?4:n)))st=LT48_BUFFER_TOO_SMALL;
 if(st)return st;
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;
 mcu_session s={0};s.r=r;s.api=api;s.context=ctx;memcpy(s.vars,r->vars,sizeof s.vars);lm20_seed(&s);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 if(!st)st=lt48_owner_load_fpga_internal(s.wire,r->fpga?r->fpga:r->idle,r->fpga?2778:r->idle_size);
 if(!st)st=run(&s,0);
 if(!st&&op==10){s.mode=1;lm20_addr(&s,0);st=lm20_init(&s,decide,dc);
  if(st==LT48_SAFETY_REJECTED&&s.fault==9)st=LT48_OK;
  unsigned level=lm20_level(&s),g=r->lm20->group;
  if(!st&&g==1&&level>3)st=LT48_FORMAT_ERROR;
  lt48_status reset=run(&s,5);if(!st)st=reset;
  if(!st&&im){
   unsigned want=level;
   for(unsigned i=0;i<4;i++)if(im[0]>>i&1){if(im[i+1])want|=1u<<i;else want&=~(1u<<i);}
   if((want&level)!=level)st=LT48_SAFETY_REJECTED;
   if(!st&&want!=level){s.mode=8;lm20_addr(&s,0);st=lm20_init(&s,decide,dc);
    for(unsigned i=0;i<4;i++)l14_cfg(&s,0,0,18+i)->value=want>>i&1;
    if(!st)st=run(&s,9);
    reset=run(&s,5);if(!st)st=reset;
    if(!st){s.mode=1;lm20_addr(&s,0);st=lm20_init(&s,decide,dc);if(!st&&lm20_level(&s)!=want)st=LT48_DEVICE_MISMATCH;reset=run(&s,5);if(!st)st=reset;}
    level=want;
   }
  }
  if(!st)for(unsigned i=0;i<4;i++)actual[i]=(unsigned char)(level>>(8*i));
 }else if(!st&&op==8){
  unsigned g=r->lm20->group,saved=0,level=options->protection_level;
  if(r->lm20->configuration){s.mode=1;lm20_addr(&s,0);st=lm20_init(&s,decide,dc);if(st==LT48_SAFETY_REJECTED&&s.fault==9)st=LT48_OK;saved=lm20_level(&s);lt48_status reset=run(&s,5);if(!st)st=reset;}
  if(!st&&g!=4&&saved>r->lm20->secure)st=LT48_FORMAT_ERROR;
  if(g!=4&&saved>level)level=saved;
  s.mode=8;lm20_addr(&s,0);if(!st)st=lm20_init(&s,decide,dc);
  l14_cfg(&s,0,1,0)->value=level;l14_cfg(&s,0,1,3)->value=level;l14_cfg(&s,0,1,5)->value=level;if(g==8)l14_cfg(&s,0,1,6)->value=level;
  for(unsigned i=0;i<3;i++)l14_cfg(&s,0,0,22+i)->value=(saved>>i&1)||level>=i+1;
  if(!st)st=run(&s,9);
  lt48_status reset=run(&s,5);if(!st)st=reset;
  if(!st&&r->lm20->configuration){s.mode=1;lm20_addr(&s,0);if(g==1)l14_cfg(&s,0,1,0)->value=UINT32_MAX;st=lm20_init(&s,decide,dc);if(st==LT48_SAFETY_REJECTED&&s.fault==9)st=LT48_OK;unsigned want=g==4?saved|((1u<<level)-1):level;if(!st&&lm20_level(&s)!=want)st=LT48_DEVICE_MISMATCH;reset=run(&s,5);if(!st)st=reset;}

 }else if(!st){
  if(op==7){s.mode=7;lm20_addr(&s,0);st=lm20_init(&s,decide,dc);if(!st)st=run(&s,8);if(r->lm20->group==8&&st==LT48_RUNTIME_ERROR&&s.fault==30)st=LT48_DEVICE_MISMATCH;lt48_status reset=run(&s,5);if(!st)st=reset;}
  if(!st&&(op==3||op==4)){
   st=lm20_visit(&s,0,a,n,NULL,actual,r->nominal,decide,dc,z);
   if(!st)for(size_t i=0;i<n;i++)if(op==3?actual[i]!=(!lm20_mapped(r,a+(unsigned)i)?0:255):(actual[i]&im[i])!=im[i]){st=LT48_DEVICE_MISMATCH;z->first_mismatch=a+(unsigned)i;z->mismatch_count=1;break;}
   if(!st)st=lm20_visit(&s,1,a,n,im,NULL,r->nominal,decide,dc,z);
  }
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,options->margin_type,0,volts,&count);
  unsigned passes=op==3||op==4||(op==5&&options->margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(op!=3&&op!=4&&(op!=5||options->margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;
  else if(op!=3&&op!=4&&options->margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned i=0;!st&&i<passes;i++){st=lm20_visit(&s,0,a,n,NULL,actual,volts[i],decide,dc,z);if(!st&&op!=1)st=lm20_compare(&s,z,a,actual,op==2||op==7?NULL:im,n);}
 }
 z->operation_status=st;st=l14_cleanup(&s,st,z);
 if(!st){if(op==1||op==10)memcpy(read,actual,op==10?4:n);z->completed_bytes=op==10?4:n;}
 free(actual);return st;
}
