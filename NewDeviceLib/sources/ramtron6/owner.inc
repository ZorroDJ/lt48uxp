/* Parallel VRS51 owner. Source Fuse is the boot-area selector (0..8).
 * Protect is a write action: READCFG does not report a lock-state field. */
lt48_status lt48_ramtron6_verify_options(const lt48_s8253_resources*r,lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type){
 if(!r||r->kind!=17||type>1||margin>LT48_VERIFY_MARGIN_ALL)return LT48_INVALID_ARGUMENT;
 if(r->nominal==330&&type==1&&(op==3||op==4||(op==5&&margin!=LT48_VERIFY_MARGIN_NOMINAL)))return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
lt48_status lt48_ramtron6_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=17||!n||a>=r->capacity||n>r->capacity-a)return LT48_INVALID_ARGUMENT;
 if((op<1||op>8)&&op!=10)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7||op==8||op==10)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if(op==8&&((im&& (size!=1||im[0]!=1))||(!im&&size)))return LT48_INVALID_ARGUMENT;
 if(op==10){if(!im)return size?LT48_INVALID_ARGUMENT:LT48_OK;if(size!=10||im[0]&~3u||im[1])return LT48_INVALID_ARGUMENT;for(unsigned k=0;k<2;k++)if(im[0]>>k&1){uint32_t v=0;for(unsigned b=0;b<4;b++)v|=(uint32_t)im[2+4*k+b]<<(8*b);if(v>(k?8u:1u))return LT48_INVALID_ARGUMENT;}}
 return LT48_OK;
}
static lt48_status r6_settings(s8253_session*s,uint32_t*fuse){
 s->mode=13;n79_address(s,0);uint32_t v[2]={0,UINT32_MAX};numicro_config(s,v);lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=run_s8253(s,2);if(!st)st=run_s8253(s,6);if(!st){if(s->config[1].value>8)st=LT48_DEVICE_MISMATCH;else *fuse=s->config[1].value;}lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status r6_apply(s8253_session*s,uint32_t fuse,unsigned protect){
 s->mode=13;n79_address(s,0);lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=run_s8253(s,2);uint32_t v[2]={protect,fuse};numicro_config(s,v);s->buffers[2][0]=0;if(!st)st=run_s8253(s,11);if(!st&&s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;lt48_status reset=run_s8253(s,3);if(!st)st=reset;/* Fuse is readable; protection itself is write-only. */uint32_t got=UINT32_MAX;if(!st)st=r6_settings(s,&got);if(!st&&got!=fuse)st=LT48_DEVICE_MISMATCH;return st;
}
static lt48_status r6_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;n79_address(s,a);lt48_status st=w78bank_voltage(s,cv);s->buffers[2][0]=0;if(!st)st=run_s8253(s,2);if(!st&&writing&&s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
 /* INIT mode2 erases exactly once. Continue the active stream across chunks.
  * Original XP status1 query is at the complete-transfer boundary. */
 for(size_t q=0;!st&&q<n;){size_t take=n-q;if(take>4096)take=4096;st=a28_block(s,writing,writing?(unsigned char*)im+q:out+q,take);q+=take;}
 if(!st&&writing){unsigned char end[]={0x0c,1},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;else if(!st&&!(reply[2]&1)){unsigned char actual;st=block_s8253(s,0,&actual,1);if(!st)st=LT48_DEVICE_MISMATCH;}}
 if(!st)st=stop_s8253(s);
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_ramtron6(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;/* No original GETID callback. */
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_ramtron6_validate(r,op,a,n,im,size);if(!st)st=lt48_ramtron6_verify_options(r,op,margin,type);if(!st&&((op==1&&(!out||outsize<n))||(op==10&&(!out||outsize<8))))st=LT48_BUFFER_TOO_SMALL;if(st)return z->operation_status=st;
 if(api->cancelled&&api->cancelled(ctx)){z->cleanup_completed=1;return z->operation_status=LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return z->operation_status=LT48_NO_MEMORY;s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;uint32_t fuse=0;int writing=op==3||op==4;
 if(!st&&(writing||op==8||op==10))st=r6_settings(&s,&fuse);
 if(!st&&(op==8||op==10)){unsigned protect=op==8;uint32_t want=fuse;if(op==10&&im){if(im[0]&1)protect=im[2];if(im[0]&2){want=0;for(unsigned k=0;k<4;k++)want|=(uint32_t)im[6+k]<<(8*k);}}if(op==8||im)st=r6_apply(&s,want,protect);if(!st&&op==10){memset(actual,0,8);for(unsigned k=0;k<4;k++)actual[4+k]=(unsigned char)(want>>(8*k));}
 }else if(!st){
  if(writing)st=r6_cycle(&s,1,0,n,im,NULL,r->nominal);
  if(!st&&op==7){s.mode=7;n79_address(&s,0);st=run_s8253(&s,2);s.buffers[2][0]=0;if(!st)st=run_s8253(&s,10);if(!st&&s.buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;lt48_status reset=run_s8253(&s,3);if(!st)st=reset;uint32_t got;if(!st)st=r6_settings(&s,&got);if(!st&&got)st=LT48_DEVICE_MISMATCH;}
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){st=r6_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,(op==2||op==7)?NULL:im,n);}
  if(!st&&writing)st=r6_apply(&s,fuse,0);
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&(writing||op==7)){lt48_serial_page_result fresh;st=execute_ramtron6(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,NULL,NULL,&fresh);if(!st)st=w78bank_compare(z,0,actual,op==7?NULL:im,r->capacity);if(st){z->operation_status=st;lt48_runtime_transport_session*wire=NULL;lt48_status cs;int done;lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cs,&done);if(!done){z->cleanup_status=cs;z->cleanup_completed=0;}}}
 if(!st){if(op==1)memcpy(out,actual,n);if(op==10)memcpy(out,actual,8);z->completed_bytes=op==10?8:n;}free(actual);return st;
}
