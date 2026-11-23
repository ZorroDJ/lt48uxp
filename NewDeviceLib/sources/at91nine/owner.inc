/* Authored FFPI orchestration. Original applets/FPGA stay authenticated BYO.
 * Configuration exposes set-mask actions; security has no readable source state.
 * Whole-image ranges avoid claiming unqualified odd-byte start semantics. */
lt48_status lt48_at91nine_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=18||!n)return LT48_INVALID_ARGUMENT;
 if((op<1||op>8)&&op!=10)return LT48_UNSUPPORTED;
 if(a||n!=r->capacity)return LT48_SAFETY_REJECTED;
 if(r->a91_width==1&&(r->configuration_maximum[1]>255||r->capacity>65536)&&(op==3||op==4||op==10))return LT48_UNSUPPORTED;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if(op==8&&((im&&(size!=1||im[0]!=1))||(!im&&size)))return LT48_INVALID_ARGUMENT;
 if(op==10){if(!im)return size?LT48_INVALID_ARGUMENT:LT48_OK;if(size!=14||(im[0]&~7u)||im[1])return LT48_INVALID_ARGUMENT;for(unsigned i=0;i<3;i++)if(im[0]>>i&1){uint32_t v=0;for(unsigned b=0;b<4;b++)v|=(uint32_t)im[2+4*i+b]<<(8*b);if(v>r->configuration_maximum[i])return LT48_INVALID_ARGUMENT;}}
 return LT48_OK;
}
static lt48_status a91_settings(s8253_session*s,uint32_t*v){
 s->mode=13;n79_address(s,0);uint32_t initial[3]={0,0,0};numicro_config(s,initial);lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=run_s8253(s,2);if(!st)st=run_s8253(s,6);for(unsigned i=0;!st&&i<3;i++){if(s->config[i].value>s->r->configuration_maximum[i])st=LT48_DEVICE_MISMATCH;else v[i]=s->config[i].value;}lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status a91_apply(s8253_session*s,const uint32_t*v){
 s->mode=13;n79_address(s,0);lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=run_s8253(s,2);numicro_config(s,v);if(!st)st=run_s8253(s,11);lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status a91_block(s8253_session*s,int writing,unsigned char*p,size_t n){
 if(!writing)return block_s8253(s,0,p,n);
 unsigned width=s->r->a91_width;if(n%width)return LT48_INVALID_ARGUMENT;
 unsigned char h[]={0x48,(unsigned char)(n/512),(unsigned char)(n/512>>8),(unsigned char)(n%512),(unsigned char)(n%512>>8),(unsigned char)width},reply[512]={0};lt48_status st=command_s8253(s,h,sizeof h,NULL);
 if(!st)st=transfer_s8253(s,0,4,p,n);
 if(!st)st=transfer_s8253(s,1,0x88,reply,sizeof reply);
 if(!st&&!(reply[0]|reply[1])){unsigned char actual[2];lt48_status drain=block_s8253(s,0,actual,width);st=drain?drain:LT48_DEVICE_MISMATCH;}return st;
}
static lt48_status a91_cycle(s8253_session*s,int writing,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;n79_address(s,0);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);
 for(size_t q=0;!st&&q<n;){size_t take=n-q;if(take>4096)take=4096;st=a91_block(s,writing,writing?(unsigned char*)im+q:out+q,take);q+=take;}
 if(!st&&writing){unsigned char end[]={0x0c,1},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;else if(!st&&!(reply[2]&1)){unsigned char actual[2];st=block_s8253(s,0,actual,s->r->a91_width);if(!st)st=LT48_DEVICE_MISMATCH;}}
 if(!st)st=stop_s8253(s);
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_at91nine(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_at91nine_validate(r,op,a,n,im,size);if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL))st=LT48_INVALID_ARGUMENT;if(!st&&type==1&&(op==3||op==4||(op==5&&margin!=LT48_VERIFY_MARGIN_NOMINAL)))st=LT48_SAFETY_REJECTED;if(!st&&((op==1&&(!out||outsize<n))||(op==10&&(!out||outsize<12))))st=LT48_BUFFER_TOO_SMALL;if(st)return z->operation_status=st;
 if(api->cancelled&&api->cancelled(ctx)){z->cleanup_completed=1;return z->operation_status=LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return z->operation_status=LT48_NO_MEMORY;s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;uint32_t saved[3]={0},want[3]={0};int writing=op==3||op==4;
 if(!st&&(writing||op==10))st=a91_settings(&s,saved);
 if(!st&&(op==8||op==10)){
  memcpy(want,saved,sizeof want);if(op==8)want[0]=1;
  if(op==10&&im)for(unsigned i=0;i<3;i++)if(im[0]>>i&1){uint32_t v=0;for(unsigned b=0;b<4;b++)v|=(uint32_t)im[2+4*i+b]<<(8*b);want[i]|=v;}
  if(op==8||im)st=a91_apply(&s,want);
  if(!st&&!want[0]&&(op==8||im)){uint32_t got[3];st=a91_settings(&s,got);if(!st&&memcmp(got,want,sizeof got))st=LT48_DEVICE_MISMATCH;}
  if(!st&&op==10)for(unsigned i=0;i<3;i++)for(unsigned b=0;b<4;b++)actual[4*i+b]=(unsigned char)(want[i]>>(8*b));
 }else if(!st){
  if(writing)st=a91_cycle(&s,1,n,im,NULL,r->nominal);
  if(!st&&op==7){s.mode=7;n79_address(&s,0);st=run_s8253(&s,2);if(!st)st=run_s8253(&s,10);lt48_status reset=run_s8253(&s,3);if(!st)st=reset;uint32_t got[3];if(!st)st=a91_settings(&s,got);if(!st&&(got[0]||got[1]||got[2]))st=LT48_DEVICE_MISMATCH;}
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){st=a91_cycle(&s,0,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,0,actual,(op==2||op==7)?NULL:im,n);}
  /* Readback precedes lock restoration. Security is not recoverable state. */
  if(!st&&writing){st=a91_apply(&s,saved);if(!st){uint32_t got[3];st=a91_settings(&s,got);if(!st&&memcmp(got,saved,sizeof got))st=LT48_DEVICE_MISMATCH;}}
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&(writing||op==7)){lt48_serial_page_result fresh;st=execute_at91nine(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,NULL,NULL,&fresh);if(!st)st=w78bank_compare(z,0,actual,op==7?NULL:im,r->capacity);if(st){z->operation_status=st;lt48_runtime_transport_session*wire=NULL;lt48_status cs;int done;lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cs,&done);if(!done){z->cleanup_status=cs;z->cleanup_completed=0;}}}
 if(!st){if(op==1)memcpy(out,actual,n);if(op==10)memcpy(out,actual,12);z->completed_bytes=op==10?12:n;}free(actual);return st;
}
