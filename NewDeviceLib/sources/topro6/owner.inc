/* TOPRO TP2808: original AddTab route. Protect is write-only: its worker
 * response acknowledges the command, not a separately read security state. */
static void tp6_address(s8253_session*s,unsigned address){
 unsigned base=0,k=n79_region(s->r,address,&base);
 for(unsigned b=0;b<4;b++){
  s->buffers[0][b]=(unsigned char)((s->r->n79_physical[k]+address-base)>>(8*b));
  s->buffers[1][b]=(unsigned char)((s->r->region_bank[k]+address-base)>>(8*b));
  s->buffers[3][b]=0;s->buffers[9][b]=b?0:1;
 }
}
static lt48_status tp6_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;lt48_status st=w78bank_voltage(s,cv);
 size_t completed=0;
 while(!st&&completed<n){
  unsigned address=a+completed,base=0,k=n79_region(s->r,address,&base),offset=address-base;
  size_t take=n-completed;if(take>s->r->region_length[k]-offset)take=s->r->region_length[k]-offset;
  tp6_address(s,base);st=run_s8253(s,2);if(!st)st=run_s8253(s,writing?8:5);
  size_t total=writing?take:offset+take;
  unsigned char*scratch=writing?NULL:malloc(total);if(!writing&&!scratch)st=LT48_NO_MEMORY;
  for(size_t q=0;!st&&q<total;){size_t chunk=total-q;if(chunk>4096)chunk=4096;st=a28_block(s,writing,writing?(unsigned char*)im+completed+q:scratch+q,chunk);q+=chunk;}
  if(!st&&writing){unsigned char end[]={0x0c,1},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;else if(!st&&!(reply[2]&1)){unsigned char actual;lt48_status drain=block_s8253(s,0,&actual,1);st=drain?drain:LT48_DEVICE_MISMATCH;}}
  if(!st&&!writing){memcpy(out+completed,scratch+offset,take);}
  free(scratch);
  if(!st)st=stop_s8253(s);
  lt48_status reset=run_s8253(s,3);if(!st)st=reset;
  completed+=take;
 }
 return st;
}

static lt48_status execute_tp6(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_tp6_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_HIGH))st=LT48_INVALID_ARGUMENT;
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<4))))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);numicro_config(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}
 st=setup_s8253(&s);if(!st)s.ready=1;
 int writing=op==3||op==4;
 if(!st&&(op==8||op==10)){
  uint32_t want[32]={1};s.mode=13;tp6_address(&s,0);st=run_s8253(&s,2);numicro_config(&s,want);
  if(!st)st=run_s8253(&s,11);
  if(!st&&s.buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
  lt48_status reset=run_s8253(&s,3);if(!st)st=reset;
  memset(actual,0,4);actual[0]=1;
 }else if(!st&&op==7){
  s.mode=7;tp6_address(&s,0);st=run_s8253(&s,2);if(!st)st=run_s8253(&s,10);
  if(!st&&s.buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
  lt48_status reset=run_s8253(&s,3);if(!st)st=reset;
  if(!st)st=tp6_cycle(&s,0,0,n,NULL,actual,r->nominal);
  if(!st)st=w78bank_compare(z,0,actual,NULL,n);
 }else if(!st){
  if(writing){
   st=tp6_cycle(&s,0,0,n,NULL,actual,r->nominal);
   for(size_t i=0;!st&&i<n;i++)if((actual[i]&im[i])!=im[i])st=LT48_SAFETY_REJECTED;
   if(!st)st=tp6_cycle(&s,1,0,n,im,NULL,r->nominal);
  }
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){
   st=tp6_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2?NULL:im,n);
  }
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual,n);if(op==10)memcpy(read,actual,4);z->completed_bytes=op==10?4:n;}
 free(actual);return st;
}
