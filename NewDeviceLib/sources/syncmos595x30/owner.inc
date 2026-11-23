/* SyncMos SM595x / SM895x: original XP/AddTab callbacks over the shared C transport.
 * Images concatenate only the declared, exposed regions. Configuration is
 * separate. All writes require a complete image and independent readback. */
static void sx30_address(s8253_session*s,unsigned address){
 unsigned base=0,k=n79_region(s->r,address,&base);
 for(unsigned b=0;b<4;b++){
  s->buffers[0][b]=(unsigned char)((s->r->n79_physical[k]+address-base)>>(8*b));
  s->buffers[1][b]=(unsigned char)((s->r->kind==28?s->r->capacity:s->r->region_bank[k]+address-base)>>(8*b));
  s->buffers[2][b]=(unsigned char)(s->r->capacity>>(8*b));
  s->buffers[9][b]=b?0:1;
 }

}
static lt48_status sx30_settings(s8253_session*s,uint32_t*v){
 s->mode=13;sx30_address(s,0);numicro_config(s,NULL);
 lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=run_s8253(s,2);if(!st)st=run_s8253(s,6);
 for(unsigned i=0;!st&&i<s->r->fields;i++){
  if(s->config[i].value>s->r->configuration_maximum[i])st=LT48_DEVICE_MISMATCH;
  else v[i]=s->config[i].value;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status sx30_apply(s8253_session*s,const uint32_t*old,const uint32_t*want){
 (void)old;lt48_status st=LT48_OK;
 s->mode=13;sx30_address(s,0);st=run_s8253(s,2);numicro_config(s,want);
 if(!st)st=run_s8253(s,11);
 if(!st&&s->buffers[s->r->kind==28?2:3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);if(!st)st=reset;
 uint32_t got[32];if(!st)st=sx30_settings(s,got);
 if(!st&&memcmp(got,want,s->r->fields*sizeof*got))st=LT48_DEVICE_MISMATCH;
 return st;
}
static lt48_status sx30_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;sx30_address(s,a);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);size_t done=0;
 while(!st&&done<n){
  unsigned at=a+(unsigned)done,base=0,k=n79_region(s->r,at,&base);size_t take=n-done;
  if(take>base+s->r->region_length[k]-at)take=base+s->r->region_length[k]-at;
  sx30_address(s,at);if(s->r->kind==27)st=run_s8253(s,writing?8:5);
  for(size_t q=0;!st&&q<take;){size_t chunk=take-q;if(chunk>4096)chunk=4096;st=block_s8253(s,writing,writing?(unsigned char*)im+done+q:out+done+q,chunk);q+=chunk;}
  if(!st)st=stop_s8253(s);
  done+=take;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status sx30_erase(s8253_session*s){
 s->mode=7;sx30_address(s,0);lt48_status st=run_s8253(s,2);
 if(!st)st=run_s8253(s,10);
 if(!st&&s->buffers[s->r->kind==28?2:3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status sx30_cleanup(void*context,unsigned phase){
 s8253_session*s=context;
 if(phase==1&&s->sy80_restore){uint32_t now[32];lt48_status st=sx30_settings(s,now);
  if(!st&&memcmp(now,s->sy80_saved,s->r->fields*sizeof*now))st=sx30_apply(s,now,s->sy80_saved);
  lt48_status reset=cleanup_sst(s,phase);return st?st:reset;
 }
 return cleanup_sst(s,phase);
}
static lt48_status execute_sx30(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_sx30_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_HIGH))st=LT48_INVALID_ARGUMENT;
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<4*r->fields))))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);numicro_config(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}
 st=setup_s8253(&s);if(!st)s.ready=1;uint32_t saved[32]={0},want[32]={0};
 if(!st)st=sx30_settings(&s,saved);
 int writing=op==3||op==4,locked=saved[0]!=0;
 if(!st&&locked&&op!=7&&!writing&&op!=8&&op!=10)st=LT48_SAFETY_REJECTED;
 if(!st&&(op==8||op==10)){
  memcpy(want,saved,sizeof want);if(op==8)want[0]=1;
  else if(im){uint32_t mask=0;for(unsigned b=0;b<4;b++)mask|=(uint32_t)im[b]<<(8*b);
   for(unsigned i=0;i<r->fields;i++)if(mask>>i&1){want[i]=0;for(unsigned b=0;b<4;b++)want[i]|=(uint32_t)im[4+4*i+b]<<(8*b);}
  }
  if(memcmp(want,saved,r->fields*sizeof*want))st=sx30_apply(&s,saved,want);
  if(!st&&op==10)for(unsigned i=0;i<r->fields;i++)for(unsigned b=0;b<4;b++)actual[4*i+b]=(unsigned char)(want[i]>>(8*b));
 }else if(!st){
  if(writing){

   if(!st){memcpy(s.sy80_saved,saved,sizeof saved);s.sy80_saved[0]=0;s.sy80_restore=1;st=sx30_erase(&s);}
   if(!st)st=sx30_settings(&s,want);
   if(!st&&want[0])st=LT48_DEVICE_MISMATCH;
   if(!st)st=sx30_cycle(&s,1,0,n,im,NULL,r->nominal);
  }else if(op==7)st=sx30_erase(&s);
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){
   st=sx30_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2||op==7?NULL:im,n);
  }
  if(!st&&(writing||op==7)){
   st=sx30_settings(&s,want);
   if(!st&&writing){saved[0]=0;if(memcmp(want,saved,r->fields*sizeof*want))st=sx30_apply(&s,want,saved);}
   if(!st&&op==7&&want[0])st=LT48_DEVICE_MISMATCH;
  }
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,sx30_cleanup,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual,n);if(op==10)memcpy(read,actual,4*r->fields);z->completed_bytes=op==10?4*r->fields:n;}
 free(actual);return st;
}
