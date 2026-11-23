/* ABOV MC81F4: original XP callbacks over the shared C transport.
 * Images concatenate only the declared, exposed regions. Configuration is
 * separate. All writes require a complete image and independent readback. */
static void ab11_address(s8253_session*s,unsigned address){
 unsigned base=0,k=n79_region(s->r,address,&base);
 for(unsigned b=0;b<4;b++){
  s->buffers[0][b]=(unsigned char)((s->r->n79_physical[k]+address-base)>>(8*b));
  s->buffers[1][b]=(unsigned char)((s->r->kind==37?s->r->capacity:s->r->region_bank[k]+address-base)>>(8*b));
  s->buffers[2][b]=0; /* XP parameter is scratch/status, not an image-size buffer. */
  s->buffers[9][b]=b?0:1;
 }

}
static lt48_status ab11_settings(s8253_session*s,uint32_t*v){
 s->mode=13;ab11_address(s,0);numicro_config(s,NULL);
 lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=run_s8253(s,2);if(!st)st=run_s8253(s,6);
 for(unsigned i=0;!st&&i<s->r->fields;i++){
  if(s->config[i].value>s->r->configuration_maximum[i])st=LT48_DEVICE_MISMATCH;
  else v[i]=s->config[i].value;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status ab11_apply(s8253_session*s,const uint32_t*old,const uint32_t*want){
 if(old[1]&&!want[1])return LT48_SAFETY_REJECTED; /* Source Protect=0 is a no-op; only Erase clears the lock. */
 lt48_status st=LT48_OK;
 s->mode=13;ab11_address(s,0);st=run_s8253(s,2);numicro_config(s,want);
 if(!st)st=run_s8253(s,11);
 if(!st&&s->buffers[s->r->kind==37?2:3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);if(!st)st=reset;
 uint32_t got[32];if(!st)st=ab11_settings(s,got);
 if(!st&&memcmp(got,want,s->r->fields*sizeof*got))st=LT48_DEVICE_MISMATCH;
 return st;
}
/* INIT replaces the host address with CONFIG_P (F000/C000/8000).
 * Preserve the original target start; partial Read consumes the prefix. */
static lt48_status ab11_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;ab11_address(s,0);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);
 size_t total=writing?n:a+n;
 for(size_t q=0;!st&&q<total;){size_t take=total-q;if(take>4096)take=4096;st=a28_block(s,writing,writing?(unsigned char*)im+q:out+q,take);q+=take;}
 if(!st&&writing){unsigned char end[]={0x0c,1},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;else if(!st&&!(reply[2]&1)){unsigned char actual;lt48_status drain=block_s8253(s,0,&actual,1);st=drain?drain:LT48_DEVICE_MISMATCH;}}
 if(!st&&!writing&&a)memmove(out,out+a,n);
 if(!st)st=stop_s8253(s);
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status ab11_erase(s8253_session*s){
 s->mode=7;ab11_address(s,0);lt48_status st=run_s8253(s,2);
 if(!st)st=run_s8253(s,10);
 if(!st&&s->buffers[s->r->kind==37?2:3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status ab11_cleanup(void*context,unsigned phase){
 s8253_session*s=context;
 if(phase==1&&s->sy80_restore){uint32_t now[32];lt48_status st=ab11_settings(s,now);
  if(!st&&memcmp(now,s->sy80_saved,s->r->fields*sizeof*now))st=ab11_apply(s,now,s->sy80_saved);
  lt48_status reset=cleanup_sst(s,phase);return st?st:reset;
 }
 return cleanup_sst(s,phase);
}
static lt48_status execute_ab11(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_ab11_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_HIGH))st=LT48_INVALID_ARGUMENT;
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<4*r->fields))))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);numicro_config(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}
 st=setup_s8253(&s);if(!st)s.ready=1;uint32_t saved[32]={0},want[32]={0};
 if(!st)st=ab11_settings(&s,saved);
 int writing=op==3||op==4,locked=saved[1]!=0;
 if(!st&&locked&&op!=7&&!writing&&op!=8&&op!=10)st=LT48_SAFETY_REJECTED;
 if(!st&&(op==8||op==10)){
  memcpy(want,saved,sizeof want);if(op==8)want[1]=1;
  else if(im){uint32_t mask=0;for(unsigned b=0;b<4;b++)mask|=(uint32_t)im[b]<<(8*b);
   for(unsigned i=0;i<r->fields;i++)if(mask>>i&1){want[i]=0;for(unsigned b=0;b<4;b++)want[i]|=(uint32_t)im[4+4*i+b]<<(8*b);}
  }
  if(memcmp(want,saved,r->fields*sizeof*want))st=ab11_apply(&s,saved,want);
  if(!st&&op==10)for(unsigned i=0;i<r->fields;i++)for(unsigned b=0;b<4;b++)actual[4*i+b]=(unsigned char)(want[i]>>(8*b));
 }else if(!st){
  if(writing){

   if(!st){memcpy(s.sy80_saved,saved,sizeof saved);s.sy80_saved[1]=0;s.sy80_restore=1;st=ab11_erase(&s);}
   if(!st)st=ab11_settings(&s,want);
   if(!st&&want[1])st=LT48_DEVICE_MISMATCH;
   if(!st)st=ab11_cycle(&s,1,0,n,im,NULL,r->nominal);
  }else if(op==7){memcpy(s.sy80_saved,saved,sizeof saved);s.sy80_saved[1]=0;s.sy80_restore=1;st=ab11_erase(&s);}
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){
   st=ab11_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2||op==7?NULL:im,n);
  }
  if(!st&&(writing||op==7)){
   st=ab11_settings(&s,want);
   if(!st&&(writing||op==7)){saved[1]=0;if(memcmp(want,saved,r->fields*sizeof*want))st=ab11_apply(&s,want,saved);}
   if(!st&&op==7&&want[1])st=LT48_DEVICE_MISMATCH;
  }
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,ab11_cleanup,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual,n);if(op==10)memcpy(read,actual,4*r->fields);z->completed_bytes=op==10?4*r->fields:n;}
 free(actual);return st;
}
