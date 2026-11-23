/* ITE IT6350: finite XP-derived byte transport. Source Read has a16bit
 * initial address, so all raw reads begin atzero and discard a prefix. */
lt48_status lt48_ite2_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=50||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;
 if(op<1||op>7)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
static void ite2_address(s8253_session*s,unsigned mode){
 s->mode=mode;memset(s->buffers,0,sizeof s->buffers);for(unsigned b=0;b<4;b++)s->buffers[1][b]=(unsigned char)(s->r->capacity>>(8*b));
}
static lt48_status ite2_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 ite2_address(s,writing?2:1);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);
 size_t total=writing?n:a+n;
 for(size_t q=0;!st&&q<total;){size_t take=total-q;if(take>4096)take=4096;st=a28_block(s,writing,writing?(unsigned char*)im+q:out+q,take);q+=take;}
 if(!st&&writing){unsigned char end[]={0x0c,1},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;else if(!st&&!(reply[2]&1)){unsigned char actual;lt48_status drain=block_s8253(s,0,&actual,1);st=drain?drain:LT48_DEVICE_MISMATCH;}}
 if(!st&&!writing&&a)memmove(out,out+a,n);
 if(!st)st=stop_s8253(s);
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status ite2_unprotect(s8253_session*s){
 ite2_address(s,13);lt48_status st=run_s8253(s,13);if(!st&&s->buffers[2][0]!=0x55)st=LT48_DEVICE_MISMATCH;return st;
}
static lt48_status ite2_erase(s8253_session*s){
 lt48_status st=ite2_unprotect(s);ite2_address(s,7);if(!st)st=run_s8253(s,2);if(!st)st=run_s8253(s,10);if(!st&&s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_ite2(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_serial_page_result*z){
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_ite2_validate(r,op,a,n,im,size);
 /* ITE inherits the original XP Verify methods and voltage helper. */
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==1&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;memcpy(s.vars,r->vars,sizeof s.vars);ite2_address(&s,1);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}
 st=setup_s8253(&s);if(!st)s.ready=1;
 if(!st&&(op==3||op==4)){
  if(op==3){st=ite2_erase(&s);if(!st)st=ite2_cycle(&s,0,0,r->capacity,NULL,actual,r->nominal);if(!st)st=w78bank_compare(z,0,actual,NULL,r->capacity);}
  else{st=ite2_unprotect(&s);if(!st)st=ite2_cycle(&s,0,0,r->capacity,NULL,actual,r->nominal);for(size_t i=0;!st&&i<n;i++)if((actual[i]&im[i])!=im[i]){z->first_mismatch=i;st=LT48_DEVICE_MISMATCH;}}
  if(!st)st=ite2_cycle(&s,1,0,n,im,NULL,r->nominal);
  if(!st)st=ite2_cycle(&s,0,0,n,NULL,actual,r->nominal);
  if(!st)st=w78bank_compare(z,0,actual,im,n);
 }else if(!st){
  if(op==7)st=ite2_erase(&s);
  uint32_t volts[4]={r->nominal,0};unsigned count=0,passes=1;
  if(!st&&op==5){st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);if(margin==LT48_VERIFY_MARGIN_ALL)passes=2;else if(margin==LT48_VERIFY_MARGIN_NOMINAL)volts[0]=r->nominal;else if(margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];}
  for(unsigned pass=0;!st&&pass<passes;pass++){
   st=ite2_cycle(&s,0,a,n,NULL,actual,volts[pass]);
   if(!st&&op!=1)st=w78bank_compare(z,a,actual,(op==2||op==7)?NULL:im,n);
  }
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual,n);z->completed_bytes=n;}free(actual);return st;
}
