#include "../otp13five/verify-voltage.h"
/* Author-written host operation owner. Every target waveform is imported intact.
 * Word addressing, image conversion, configuration and reset commits remain
 * explicit rather than being inferred from an MCU architecture name. */
static lt48_status legacy14_validate(const lt48_mcu51_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||!r->l14||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;
 const struct legacy14_profile*p=r->l14;
 if(p->bits==16&&((a|n)&1))return LT48_SAFETY_REJECTED;
 if(op==LT48_OPERATION_CONFIGURATION){
  if(p->kind!=5)return LT48_UNSUPPORTED;
  if(a||n!=r->capacity)return LT48_SAFETY_REJECTED;
  if(!im&&!size)return LT48_OK;
  if(!im||size!=4||!im[0]||(im[0]&~7u))return LT48_INVALID_ARGUMENT;
  for(unsigned i=0;i<3;i++)if((im[0]&(1u<<i))&&im[i+1]>1)return LT48_INVALID_ARGUMENT;
  return LT48_OK;
 }
 if(op==LT48_OPERATION_ERASE)return !p->erase?LT48_UNSUPPORTED:(a||n!=r->capacity)?LT48_SAFETY_REJECTED:LT48_OK;
 if(op==LT48_OPERATION_SECURE)return !p->secure?LT48_UNSUPPORTED:(a||n!=r->capacity)?LT48_SAFETY_REJECTED:LT48_OK;
 if(op<1||op>6)return LT48_UNSUPPORTED;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if(op==3||op==4){
  if((p->kind==1||(p->kind>=6&&p->kind!=10))&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
  if(p->kind==1)for(unsigned i=0;i<0x2000;i++)if(im[i]!=255)return LT48_SAFETY_REJECTED;
 }
 return LT48_OK;
}
static void l14_addr(mcu_session*s,unsigned byte){unsigned addr=byte/(s->r->l14->bits/8);
 for(unsigned i=0;i<4;i++)s->buffers[4][i]=(unsigned char)(addr>>(8*i));
}
static lt48_runtime_configuration_value*l14_cfg(mcu_session*s,int obj,unsigned cat,unsigned field){
 for(size_t i=0;i<s->l14count;i++)if(s->l14cfg[i].object_id==obj&&s->l14cfg[i].category==cat&&s->l14cfg[i].field==field)return s->l14cfg+i;
 return NULL;
}
static void l14_seed(mcu_session*s){
 const lt48_runtime_configuration_value v[]={ {-1,0,1,1},{-1,0,5,0},{0,1,0,1},{0,1,1,1},{0,1,2,3},{0,2,1,s->r->capacity==8192?0x40:0xf8},{0,0,0,0},{0,0,1,0},{0,0,2,0} };
 memcpy(s->l14cfg,v,sizeof v);s->l14count=sizeof v/sizeof*v;
}
static lt48_status l14_init(mcu_session*s,lt48_m37_id_decision_fn decide,void*ctx){
 for(unsigned attempt=0;attempt<64;attempt++){
  lt48_status st=run(s,4);
  if(!st)return st;
  if(s->r->l14->kind!=1||s->fault!=6)return s->fault==9?LT48_SAFETY_REJECTED:st;
  unsigned got=s->buffers[5][0]|(unsigned)s->buffers[5][1]<<8;
  lt48_compile_variable*id=var(s,36);
  lt48_m37_id_choice choice=decide?decide(ctx,0,id?id->value:0,0,got):LT48_M37_ID_ABORT;
  if(choice==LT48_M37_ID_ABORT)return LT48_DEVICE_MISMATCH;
  if(choice==LT48_M37_ID_IGNORE&&id)id->value=(uint16_t)got;
  else if(choice!=LT48_M37_ID_RETRY)return LT48_INVALID_ARGUMENT;
 }
 return LT48_DEVICE_MISMATCH;
}
static lt48_status l14_id(mcu_session*s,lt48_m37_id_decision_fn decide,void*ctx){
 if(s->r->l14->kind!=5&&s->r->l14->kind!=10)return LT48_OK;
 unsigned expected=s->r->l14->kind==10?0x3132:0x3234;
 for(unsigned i=0;i<64;i++){
  lt48_status st=run(s,2);
 if(st)return st;
  unsigned actual=s->buffers[2][0]|(unsigned)s->buffers[2][1]<<8;
  if(actual==expected)return LT48_OK; /* Original catalogue ID, not operation selection. */
  lt48_m37_id_choice c=decide?decide(ctx,0,(uint16_t)expected,0,(uint16_t)actual):LT48_M37_ID_ABORT;
  if(c==LT48_M37_ID_IGNORE)return LT48_OK;
  if(c!=LT48_M37_ID_RETRY)return LT48_DEVICE_MISMATCH;
 }
 return LT48_DEVICE_MISMATCH;
}
static unsigned l14_flags(mcu_session*s){unsigned bits=0;for(unsigned i=0;i<3;i++){lt48_runtime_configuration_value*v=l14_cfg(s,0,0,i);
 if(v&&v->value)bits|=1u<<i;}return bits;}
static lt48_status l14_settings(mcu_session*s,unsigned*bits){s->mode=1;l14_addr(s,0);lt48_status st=l14_init(s,NULL,NULL);
 if(!st)*bits=l14_flags(s);
 lt48_status reset=run(s,5);return st?st:reset;}
static lt48_status l14_visit(mcu_session*s,int writing,unsigned mode,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv,lt48_m37_id_decision_fn decide,void*dc,lt48_mcu51_result*z){
 s->mode=mode;l14_addr(s,a);lt48_compile_variable*v=var(s,0);
 if(v)v->value=cv;
 lt48_status st=l14_init(s,decide,dc);
 if(!st&&s->r->l14->kind==5&&l14_flags(s))st=LT48_SAFETY_REJECTED;
 unsigned width=s->r->l14->bits/8;
 for(size_t i=0;!st&&i<n;i+=width){
  l14_addr(s,a+(unsigned)i);
  for(unsigned b=0;b<width;b++)s->buffers[5][b]=s->buffers[7][b]=writing?im[i+b]:255;
  st=run(s,writing?7:6);
  if(!st&&out)memcpy(out+i,s->buffers[5],width);
  if(s->api->progress&&((i&127u)==0||i+width==n))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,i+width,n,0,0,0);
 }
 lt48_status reset=run(s,5);
 if(!st)st=reset;
 if(!st)z->completed_bytes=n;
 return st;
}
static lt48_status l14_compare(lt48_mcu51_result*z,unsigned base,const unsigned char*a,const unsigned char*b,size_t n){
 z->mismatch_count=0;z->first_mismatch=UINT32_MAX;
 for(size_t i=0;i<n;i++)if(a[i]!=(b?b[i]:255)){if(!z->mismatch_count)z->first_mismatch=base+(unsigned)i;z->mismatch_count++;}
 return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status l14_secure(mcu_session*s,unsigned level,lt48_m37_id_decision_fn decide,void*dc){
 s->mode=8;l14_addr(s,0);lt48_status st=l14_init(s,decide,dc);
 if(s->r->l14->kind==1)l14_cfg(s,0,1,2)->value=level==3?0:level==2?1:2;
 else if(s->r->l14->kind==3)l14_cfg(s,0,1,1)->value=level;
 if(!st)st=run(s,9);
 lt48_status reset=run(s,5);return st?st:reset;
}
static lt48_status l14_cleanup(mcu_session*s,lt48_status op,lt48_mcu51_result*z){
 lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
 lt48_protocol_transport clean=*s->api;clean.cancelled=NULL;
 lt48_status st=lt48_runtime_transport_session_create(&clean,s->context,&s->wire);
 if(!st)st=run(s,1);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 z->cleanup_status=st;
 for(unsigned c=0;c<4;c++){
  lt48_runtime_transport_session_close(s->wire);s->wire=NULL;st=lt48_runtime_transport_session_create(&clean,s->context,&s->wire);
  unsigned char off[]={0x10,(unsigned char)c,0};
 if(!st)st=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof off);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
  if(!z->cleanup_status)z->cleanup_status=st;
 }
 st=lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 if(!z->cleanup_status)z->cleanup_status=st;
 const unsigned char finish[]={0x49,0xaa,0x55,0x40,op?2:4};st=lt48_runtime_transport_session_controller_write(s->wire,finish,sizeof finish);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 if(!z->cleanup_status)z->cleanup_status=st;
 z->cleanup_completed=!z->cleanup_status;lt48_runtime_transport_session_close(s->wire);s->wire=NULL;return op?op:z->cleanup_status;
}
static lt48_status legacy14_execute(const lt48_mcu51_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,const lt48_mcu51_options*options,lt48_m37_id_decision_fn decide,void*dc,lt48_mcu51_result*z){
 if(!api||!api->write||!api->read||!z)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;
 lt48_status st=legacy14_validate(r,op,start,n,im,size);
 lt48_mcu51_options def={LT48_VERIFY_MARGIN_ALL,0,1,NULL,0};
 if(!options)options=&def;
 if(!st)st=lt48_mcu51_validate_options(r,op,options);
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<3))))st=LT48_BUFFER_TOO_SMALL;
 if(st)return st;
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity),*desired=malloc(r->capacity);
 if(!actual||!desired){free(actual);free(desired);return LT48_NO_MEMORY;}
 if(im&&size==n)memcpy(desired,im,n);
 mcu_session s={0};s.r=r;s.api=api;s.context=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);l14_seed(&s);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 if(!st)st=lt48_owner_load_fpga_internal(s.wire,r->fpga?r->fpga:r->idle,r->fpga?2778:r->idle_size);
 for(size_t i=0;!st&&i<r->aduc812_count;i++)st=lt48_owner_load_applet_internal(s.wire,r->aduc812_applets[i].load_address,r->aduc812_applets[i].data,r->aduc812_applets[i].data_size);
 if(!st)st=run(&s,0);
 if(!st)st=l14_id(&s,decide,dc);
 unsigned post_mask=3;
 int programming=op==3||op==4;
 if(!st&&programming&&r->l14->kind==1){s.l14image=desired;s.l14image_size=n;st=run(&s,3);post_mask=l14_cfg(&s,0,1,2)->value;}
 if(!st&&op==7){s.mode=7;l14_addr(&s,0);st=l14_init(&s,decide,dc);
 if(!st)st=run(&s,8);
 lt48_status reset=run(&s,5);
 if(!st)st=reset;
  if(!st)st=l14_visit(&s,0,1,0,r->capacity,NULL,actual,r->nominal,decide,dc,z);
 if(!st)st=l14_compare(z,0,actual,NULL,r->capacity);
 }else if(!st&&(op==10||(op==8&&r->l14->kind==5))){
  unsigned saved=0,want=0;st=l14_settings(&s,&saved);want=saved;
  if(op==8)want=7;else if(im)for(unsigned i=0;i<3;i++)if(im[0]&(1u<<i)){if(im[i+1])want|=1u<<i;else want&=~(1u<<i);}
  if(!st&&(want&saved)!=saved)st=LT48_SAFETY_REJECTED;
  if(!st&&want!=saved){for(unsigned i=0;i<3;i++)l14_cfg(&s,0,0,i)->value=(want>>i)&1;s.mode=8;l14_addr(&s,0);st=l14_init(&s,decide,dc);
 if(!st)st=run(&s,9);
 if(!st)st=run(&s,5);
 unsigned got=0;
 if(!st)st=l14_settings(&s,&got);
 if(!st&&got!=want)st=LT48_DEVICE_MISMATCH;}
  if(!st&&op==10)for(unsigned i=0;i<3;i++)actual[i]=(want>>i)&1;
 }else if(!st&&op==8){st=l14_secure(&s,options->protection_level,decide,dc);}
 else if(!st){
  if(programming){st=l14_visit(&s,0,1,start,n,NULL,actual,r->nominal,decide,dc,z);
   if(!st)for(size_t i=0;i<n;i++)if(op==3?actual[i]!=255:(r->l14->kind!=4&&(actual[i]&desired[i])!=desired[i])){st=LT48_DEVICE_MISMATCH;z->first_mismatch=start+(unsigned)i;z->mismatch_count=1;break;}
   if(!st)st=l14_visit(&s,1,(op==3||r->l14->kind==10)?2:3,start,n,desired,NULL,r->nominal,decide,dc,z);
  }
  uint32_t volts[4]={0};unsigned count=0;
 if(!st)st=(r->l14->kind==8||r->l14->kind==9)?otp13five_verify_voltages(r,options->margin_type,volts,&count):lt48_pic_source_verify_voltages(r->nominal,NULL,0,options->margin_type,0,volts,&count);
  unsigned passes=programming||(op==5&&options->margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!programming&&(op!=5||options->margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;
  else if(!programming&&options->margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned i=0;!st&&i<passes;i++){st=l14_visit(&s,0,op==2?11:1,start,n,NULL,actual,volts[i],decide,dc,z);
 if(!st&&op!=1)st=l14_compare(z,start,actual,op==2?NULL:desired,n);}
  if(!st&&programming&&r->l14->kind==1&&post_mask!=3)st=l14_secure(&s,post_mask==0?3:post_mask==1?2:1,decide,dc);
 }
 z->operation_status=st;st=l14_cleanup(&s,st,z);
 if(!st){if(op==1)memcpy(read,actual,n);
 if(op==10)memcpy(read,actual,3);
 z->completed_bytes=op==10?3:n;}
 free(actual);free(desired);return st;
}
