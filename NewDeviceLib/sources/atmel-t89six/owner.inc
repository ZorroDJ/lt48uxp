/* Main FM0, boot FM1 and EEPROM follow the original public 0x5000 image.
 * The extra row is a separate configuration object, never appended implicitly. */
static void t89_address(s8253_session*s,unsigned a){for(unsigned b=0;b<4;b++)s->buffers[0][b]=(unsigned char)(a>>(8*b));}
static void t89_fields(s8253_session*s,const uint32_t*v){
 s->config_count=12;
 for(unsigned i=0;i<11;i++)s->config[i]=(lt48_runtime_configuration_value){0,s->r->configuration_category[i],s->r->configuration_field[i],v?v[i]:UINT32_MAX};
 s->config[11]=(lt48_runtime_configuration_value){-1,0,1,0};
}
static lt48_status t89_enter(s8253_session*s,unsigned mode){s->mode=mode;t89_address(s,0);lt48_status st=w78bank_voltage(s,s->r->nominal);
 if(!st)st=run_s8253(s,2);
 return st;}
static lt48_status t89_settings(s8253_session*s,uint32_t*v){
 t89_fields(s,NULL);lt48_status st=t89_enter(s,13);
 if(!st)st=run_s8253(s,12);
 for(unsigned i=0;!st&&i<11;i++){if(s->config[i].value>s->r->configuration_maximum[i])st=LT48_DEVICE_MISMATCH;else v[i]=s->config[i].value;}
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status t89_xrow(s8253_session*s,unsigned char*row){
 lt48_status st=t89_enter(s,13);
 if(!st)st=run_s8253(s,11);
 if(!st)memcpy(row,s->device_image+0x5000,128);
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status t89_apply(s8253_session*s,const uint32_t*values){
 lt48_status st=t89_enter(s,13);t89_fields(s,values);
 if(!st)st=run_s8253(s,8);
 lt48_status reset=run_s8253(s,3);
 if(!st)st=reset;
 uint32_t got[11];
 if(!st)st=t89_settings(s,got);
 if(!st&&memcmp(got,values,sizeof got))st=LT48_DEVICE_MISMATCH;
 return st;
}
static lt48_status t89_array(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*input,unsigned char*output,unsigned cv){
 lt48_status st=w78bank_voltage(s,cv);
 if(!st){s->mode=writing?2:1;t89_address(s,writing?a:0);st=run_s8253(s,2);}
 for(size_t i=0,end=writing?n:a+n;!st&&i<end;i++){
  if(s->api->cancelled&&s->api->cancelled(s->ctx)){st=LT48_CANCELLED;break;}
  t89_address(s,writing?a+(unsigned)i:(unsigned)i);
 if(writing)s->buffers[2][0]=input[i];
  st=run_s8253(s,writing?6:4);
 if(!st&&output&&i>=a)output[i-a]=s->buffers[1][0];
  if(!st&&s->api->progress&&((i&127u)==127u||i+1==n))s->api->progress(s->ctx,LT48_EXECUTION_READ_OPERATION,i+1,n,0,0,0);
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_t89six(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t os,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;
 if(!z||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_t89six_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL||(op==1&&(!out||os<n))||(op==10&&(!out||os<44))))st=LT48_INVALID_ARGUMENT;
 if(st){z->operation_status=st;z->cleanup_completed=1;
 return st;}
 unsigned char*actual=malloc(r->capacity),*host=malloc(0x5080);
 if(!actual||!host){free(actual);free(host);return LT48_NO_MEMORY;}memset(host,255,0x5080);
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;memcpy(s.vars,r->vars,sizeof s.vars);s.device_image=host;s.device_image_update=host;s.device_image_size=0x5080;t89_fields(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 if(!st)st=setup_s8253(&s);
 uint32_t saved[11],wanted[11],got[11];unsigned char xrow[128],after[128];int writing=op==3||op==4;
 if(!st)st=t89_settings(&s,saved);
 if(!st)memcpy(wanted,saved,sizeof saved);
 /* LB1/LB2 disable parallel verification; any lock disables programming. */
 if(!st&&op!=7&&op!=8&&op!=10&&(saved[1]||saved[2]||(writing&&saved[0])))st=LT48_SAFETY_REJECTED;
 if(!st&&(writing||op==7))st=t89_xrow(&s,xrow);
 if(!st&&(op==8||op==10)){
  if(op==8){unsigned level=im?im[0]:1;wanted[level-1]=1;}
  else if(im){unsigned mask=im[0]|(unsigned)im[1]<<8;for(unsigned i=0;i<11;i++)if(mask>>i&1){wanted[i]=0;for(unsigned b=0;b<4;b++)wanted[i]|=(uint32_t)im[2+4*i+b]<<(8*b);}}
  if(memcmp(saved,wanted,sizeof saved))st=t89_apply(&s,wanted);
  if(!st)st=t89_settings(&s,got);
 if(!st&&memcmp(got,wanted,sizeof got))st=LT48_DEVICE_MISMATCH;
 }else if(!st&&op==7){
  st=t89_enter(&s,7);
 if(!st)st=run_s8253(&s,10);
 lt48_status reset=run_s8253(&s,3);
 if(!st)st=reset;
  if(!st)st=t89_array(&s,0,0,r->capacity,NULL,actual,r->nominal);
 if(!st)st=w78bank_compare(z,0,actual,NULL,r->capacity);
 }else if(!st){
  if(writing){
   st=t89_array(&s,0,0,r->capacity,NULL,actual,r->nominal);
   for(size_t i=0;!st&&i<n;i++)if((op==3&&actual[i]!=255)||(op==4&&(actual[i]&im[i])!=im[i]))st=LT48_SAFETY_REJECTED;
   if(!st)st=t89_array(&s,1,0,n,im,NULL,r->nominal);
  }
  uint32_t levels[4]={r->nominal};unsigned count=1;
 if(!st&&(writing||op==5))st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,levels,&count);
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL)){levels[0]=r->nominal;count=1;}
  if(op==5&&margin==LT48_VERIFY_MARGIN_LOW)count=1;
  if(op==5&&margin==LT48_VERIFY_MARGIN_HIGH){levels[0]=levels[1];count=1;}
  for(unsigned pass=0;!st&&pass<count;pass++){
   st=t89_array(&s,0,a,n,NULL,actual,levels[pass]);
 if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2?NULL:im,n);
  }
 }
 if(!st&&(writing||op==7)){
  st=t89_xrow(&s,after);
 if(!st&&memcmp(xrow,after,sizeof xrow))st=LT48_DEVICE_MISMATCH;
  if(!st)st=t89_settings(&s,got);
 if(!st){if(op==7){saved[0]=saved[1]=saved[2]=0;}if(memcmp(got,saved,sizeof got))st=LT48_DEVICE_MISMATCH;}
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&(writing||op==7)){
  lt48_serial_page_result fresh;st=execute_t89six(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,NULL,NULL,&fresh);
 if(!st)st=w78bank_compare(z,0,actual,writing?im:NULL,r->capacity);
  if(st){z->operation_status=st;lt48_runtime_transport_session*wire=NULL;lt48_status cs;int done;lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cs,&done);
 if(!done){z->cleanup_status=cs;z->cleanup_completed=0;}}
 }
 if(!st){if(op==1)memcpy(out,actual,n);
 if(op==10)for(unsigned i=0;i<11;i++)for(unsigned b=0;b<4;b++)out[4*i+b]=(unsigned char)(got[i]>>(8*b));
 z->completed_bytes=n;}
 free(actual);free(host);
 return st;
}
