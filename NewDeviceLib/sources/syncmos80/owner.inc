/* SyncMos SM59R: original AddTab callbacks over the shared C transport.
 * Images concatenate only the declared, exposed regions. Configuration is
 * separate. All writes require a complete image and independent readback. */
static void sy80_address(s8253_session*s,unsigned address){
 unsigned base=0,k=n79_region(s->r,address,&base);
 for(unsigned b=0;b<4;b++){
  s->buffers[0][b]=(unsigned char)((s->r->n79_physical[k]+address-base)>>(8*b));
  s->buffers[1][b]=(unsigned char)((s->r->region_bank[k]+address-base)>>(8*b));
  s->buffers[2][b]=(unsigned char)(s->r->capacity>>(8*b));
  s->buffers[9][b]=b?0:1;
 }
 /* Program uses an explicit checked Erase below. Do not repeat the source
  * INIT's conditional erase through its host-owned Temp buffer. */
 memset(s->buffers[20],0,32);s->buffers[20][0]=0x55;
}
static lt48_status sy80_settings(s8253_session*s,uint32_t*v){
 s->mode=13;sy80_address(s,0);numicro_config(s,NULL);
 lt48_status st=run_s8253(s,2);if(!st)st=run_s8253(s,6);
 for(unsigned i=0;!st&&i<s->r->fields;i++){
  if(s->config[i].value>s->r->configuration_maximum[i])st=LT48_DEVICE_MISMATCH;
  else v[i]=s->config[i].value;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status sy80_encode(const lt48_s8253_resources*r,const uint32_t*v,unsigned char*out,size_t*length){
 lt48_runtime_configuration_value cfg[32];
 for(unsigned i=0;i<r->fields;i++)cfg[i]=(lt48_runtime_configuration_value){-1,r->configuration_category[i],r->configuration_field[i],v[i]};
 lt48_runtime_options o={0};o.configuration=cfg;o.configuration_count=r->fields;o.use_explicit_buffer_ids=1;o.mode_buffer=8;o.parameter_buffer_id=3;o.mode=13;
 lt48_runtime_result*z=NULL;lt48_status st=lt48_runtime_interpret(r->wave[11],r->pins,r->vars,r->variable_count,&o,&z);size_t n=0;
 for(size_t i=0;!st&&i<lt48_runtime_event_count(z);i++){
  lt48_runtime_event e;st=lt48_runtime_get_event(z,i,&e);
  if(!st&&e.kind==LT48_RUNTIME_Q2){if(n>=256)st=LT48_FORMAT_ERROR;else out[n++]=(unsigned char)e.arguments[0];}
 }
 lt48_runtime_result_close(z);*length=n;return st?st:n?LT48_OK:LT48_FORMAT_ERROR;
}
static lt48_status sy80_apply(s8253_session*s,const uint32_t*old,const uint32_t*want){
 unsigned char a[256],b[256];size_t na=0,nb=0;
 lt48_status st=sy80_encode(s->r,old,a,&na);if(!st)st=sy80_encode(s->r,want,b,&nb);
 if(st)return st;
 if(na!=nb)return LT48_SAFETY_REJECTED;
 for(size_t i=0;i<na;i++)if((a[i]&b[i])!=b[i])return LT48_SAFETY_REJECTED;
 s->mode=13;sy80_address(s,0);st=run_s8253(s,2);numicro_config(s,want);
 if(!st)st=run_s8253(s,11);
 if(!st&&s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);if(!st)st=reset;
 uint32_t got[32];if(!st)st=sy80_settings(s,got);
 if(!st&&memcmp(got,want,s->r->fields*sizeof*got))st=LT48_DEVICE_MISMATCH;
 return st;
}
static lt48_status sy80_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;sy80_address(s,a);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);size_t done=0;
 while(!st&&done<n){
  unsigned at=a+(unsigned)done,base=0,k=n79_region(s->r,at,&base);size_t take=n-done;
  if(take>base+s->r->region_length[k]-at)take=base+s->r->region_length[k]-at;
  sy80_address(s,at);st=run_s8253(s,writing?8:5);
  for(size_t q=0;!st&&q<take;){size_t chunk=take-q;if(chunk>4096)chunk=4096;st=block_s8253(s,writing,writing?(unsigned char*)im+done+q:out+done+q,chunk);q+=chunk;}
  if(!st)st=stop_s8253(s);
  done+=take;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status sy80_erase(s8253_session*s){
 s->mode=7;sy80_address(s,0);lt48_status st=run_s8253(s,2);
 if(!st)st=run_s8253(s,10);
 if(!st&&s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status sy80_cleanup(void*context,unsigned phase){
 s8253_session*s=context;
 if(phase==1&&s->sy80_restore){uint32_t now[32];lt48_status st=sy80_settings(s,now);
  if(!st&&memcmp(now,s->sy80_saved,s->r->fields*sizeof*now))st=sy80_apply(s,now,s->sy80_saved);
  lt48_status reset=cleanup_sst(s,phase);return st?st:reset;
 }
 return cleanup_sst(s,phase);
}
static lt48_status execute_sy80(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_sy80_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_HIGH))st=LT48_INVALID_ARGUMENT;
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<4*r->fields))))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);numicro_config(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}
 st=setup_s8253(&s);if(!st)s.ready=1;uint32_t saved[32]={0},want[32]={0};
 if(!st)st=sy80_settings(&s,saved);
 int writing=op==3||op==4,locked=saved[0]!=0;
 if(!st&&locked&&op!=7&&!writing&&op!=8&&op!=10)st=LT48_SAFETY_REJECTED;
 if(!st&&(op==8||op==10)){
  memcpy(want,saved,sizeof want);if(op==8)want[0]=1;
  else if(im){uint32_t mask=0;for(unsigned b=0;b<4;b++)mask|=(uint32_t)im[b]<<(8*b);
   for(unsigned i=0;i<r->fields;i++)if(mask>>i&1){want[i]=0;for(unsigned b=0;b<4;b++)want[i]|=(uint32_t)im[4+4*i+b]<<(8*b);}
  }
  if(memcmp(want,saved,r->fields*sizeof*want))st=sy80_apply(&s,saved,want);
  if(!st&&op==10)for(unsigned i=0;i<r->fields;i++)for(unsigned b=0;b<4;b++)actual[4*i+b]=(unsigned char)(want[i]>>(8*b));
 }else if(!st){
  if(writing){
   if(!locked){st=sy80_cycle(&s,0,0,n,NULL,actual,r->nominal);for(size_t i=0;!st&&i<n;i++)if(op==3?actual[i]!=255:(actual[i]&im[i])!=im[i])st=LT48_SAFETY_REJECTED;}
   if(!st){memcpy(s.sy80_saved,saved,sizeof saved);s.sy80_saved[0]=0;s.sy80_restore=1;st=sy80_erase(&s);}
   if(!st)st=sy80_cycle(&s,1,0,n,im,NULL,r->nominal);
  }else if(op==7)st=sy80_erase(&s);
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){
   st=sy80_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2||op==7?NULL:im,n);
  }
  if(!st&&(writing||op==7)){
   st=sy80_settings(&s,want);
   if(!st&&writing){saved[0]=0;if(memcmp(want,saved,r->fields*sizeof*want))st=sy80_apply(&s,want,saved);}
   if(!st&&op==7&&want[0])st=LT48_DEVICE_MISMATCH;
  }
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,sy80_cleanup,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual,n);if(op==10)memcpy(read,actual,4*r->fields);z->completed_bytes=op==10?4*r->fields:n;}
 free(actual);return st;
}
