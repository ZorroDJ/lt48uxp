/* Weltrend WT61/69: source entry handshake, source-sized page Program and
 * zero-start workers. Configuration/Secure are absent in the source. */
static void wt8_address(s8253_session*s,unsigned address){
 for(unsigned b=0;b<4;b++){s->buffers[0][b]=(unsigned char)(address>>(8*b));s->buffers[1][b]=(unsigned char)(s->r->capacity>>(8*b));s->buffers[2][b]=0;s->buffers[9][b]=b?0:1;}
}
static lt48_status wt8_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;wt8_address(s,0);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);
 if(!st&&(s->buffers[2][0]!=0x0d||s->buffers[12][0]!=2))st=LT48_DEVICE_MISMATCH;
 size_t total=writing?n:a+n;
 for(size_t q=0;!st&&q<total;){size_t take=total-q;if(take>4096)take=4096;st=a28_block(s,writing,writing?(unsigned char*)im+q:out+q,take);q+=take;}
 /* Shared XP host queries status1 once after the complete Program stream. */
 if(!st&&writing){unsigned char end[]={0x0c,1},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;else if(!st&&!(reply[2]&1)){unsigned char actual;lt48_status drain=block_s8253(s,0,&actual,1);st=drain?drain:LT48_DEVICE_MISMATCH;}}
 if(!st&&!writing&&a)memmove(out,out+a,n);
 if(!st)st=stop_s8253(s);
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status wt8_action(s8253_session*s,unsigned role){
 s->mode=7;wt8_address(s,0);
 lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=run_s8253(s,2);if(!st&&(s->buffers[2][0]!=0x0d||s->buffers[12][0]!=2))st=LT48_DEVICE_MISMATCH;if(!st)st=run_s8253(s,role);
 if(!st&&s->buffers[2][0]!=255)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_wt8(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_wt8_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==1&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(st)return z->operation_status=st;
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return z->operation_status=LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);numicro_config(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;
 int writing=op==3||op==4;
 if(!st){
  if(writing||op==7)st=wt8_action(&s,10); /* INIT mode2 does not erase. */
  if(!st&&writing)st=wt8_cycle(&s,1,0,n,im,NULL,r->nominal);
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){st=wt8_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2||op==7?NULL:im,n);}
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual,n);z->completed_bytes=n;}free(actual);return st;
}
