/* CC254x original debug path. Read advances an explicit 32KiB bank counter;
 * Program uses source 512-byte pages and normalizes the last lock byte. */
lt48_status lt48_cc254x5_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=14||!n||a>=r->capacity||n>r->capacity-a)return LT48_INVALID_ARGUMENT;
 if((op<1||op>8)&&op!=10)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7||op==8||op==10)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==10){if(!im&&!size)return LT48_OK;if(!im||size!=6||im[0]>1||im[1]||im[3]||im[4]||im[5]||im[2]>1)return LT48_INVALID_ARGUMENT;}
 else if(op==8){if(im&&(size!=1||im[0]!=1))return LT48_INVALID_ARGUMENT;if(!im&&size)return LT48_INVALID_ARGUMENT;}
 else if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
static void c25_word(s8253_session*s,unsigned b,unsigned v){for(unsigned k=0;k<4;k++)s->buffers[b][k]=v>>(8*k);}
static lt48_status c25_settings(s8253_session*s,unsigned*lock){s->mode=13;numicro_config(s,NULL);lt48_status st=run_s8253(s,2);if(!st)st=run_s8253(s,6);if(!st){if(s->config[0].value>1)st=LT48_DEVICE_MISMATCH;else *lock=s->config[0].value;}lt48_status reset=run_s8253(s,3);return st?st:reset;}
static lt48_status c25_cycle(s8253_session*s,int writing,size_t n,unsigned char*data,unsigned cv){
 s->mode=writing?2:1;c25_word(s,0,0);c25_word(s,1,0);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);
 for(size_t off=0;!st&&off<n;){size_t take=n-off;if(take>32768)take=32768;c25_word(s,0,(unsigned)off);c25_word(s,1,(unsigned)off);st=run_s8253(s,writing?8:5);
  for(size_t q=0;!st&&q<take;){size_t chunk=take-q;if(chunk>4096)chunk=4096;st=block_s8253(s,writing,data+off+q,chunk);q+=chunk;}
  if(!st)st=stop_s8253(s);
  off+=take;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_cc254x5(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_cc254x5_validate(r,op,a,n,im,size);if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL))st=LT48_INVALID_ARGUMENT;if(!st&&((op==1&&(!out||outsize<n))||(op==10&&(!out||outsize<4))))st=LT48_BUFFER_TOO_SMALL;if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity),*expected=malloc(r->capacity);if(!actual||!expected){free(actual);free(expected);return LT48_NO_MEMORY;}memset(expected,255,r->capacity);
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=13;s.device_image=expected;s.device_image_update=expected;s.device_image_size=r->capacity;memcpy(s.vars,r->vars,sizeof s.vars);numicro_config(&s,NULL);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;if(!st)st=sm16_identify(&s,decide,dc);unsigned lock=0,want=0;int writing=op==3||op==4;
 if(!st)st=c25_settings(&s,&lock);
 if(!st&&(op==8||op==10)){want=op==8?1:im&&(im[0]&1)?im[2]:lock;if(want!=lock){if(lock)st=LT48_SAFETY_REJECTED;else{st=c25_cycle(&s,0,r->capacity,expected,r->nominal);s.mode=13;if(!st)st=run_s8253(&s,2);uint32_t value=want;numicro_config(&s,&value);if(!st)st=run_s8253(&s,11);lt48_status reset=run_s8253(&s,3);if(!st)st=reset;if(!st)st=c25_settings(&s,&lock);if(!st&&lock!=want)st=LT48_DEVICE_MISMATCH;}}if(!st&&op==10){memset(actual,0,4);actual[0]=(unsigned char)lock;}
 }else if(!st&&op==7){s.mode=7;st=run_s8253(&s,2);if(!st)st=run_s8253(&s,10);if(!st&&s.buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;lt48_status reset=run_s8253(&s,3);if(!st)st=reset;if(!st)st=c25_settings(&s,&lock);if(!st&&lock)st=LT48_DEVICE_MISMATCH;if(!st)st=c25_cycle(&s,0,r->capacity,actual,r->nominal);if(!st)st=w78bank_compare(z,0,actual,NULL,r->capacity);
 }else if(!st){if(lock)st=LT48_SAFETY_REJECTED;if(!st&&writing){st=c25_cycle(&s,0,r->capacity,actual,r->nominal);memcpy(expected,im,n);
  /* Original INIT mode2 mutates the host image lock byte to unlocked. The
   * mutable-image result is copied back by the finite kind14 runtime path. */
  for(size_t k=0;!st&&k<n;k++)if(op==3?actual[k]!=255:(actual[k]&(k==n-1?(expected[k]|128):expected[k]))!=(k==n-1?(expected[k]|128):expected[k])){z->first_mismatch=k;z->mismatch_count=1;st=LT48_DEVICE_MISMATCH;}
  if(!st)st=c25_cycle(&s,1,n,expected,r->nominal);
 }
 uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];for(unsigned pass=0;!st&&pass<passes;pass++){st=c25_cycle(&s,0,a+n,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual+a,op==2?NULL:writing?expected:im,n);}
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&(writing||op==7)){lt48_serial_page_result fresh;st=execute_cc254x5(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,decide,dc,&fresh);if(!st)st=w78bank_compare(z,0,actual,writing?expected:NULL,r->capacity);if(st){z->operation_status=st;lt48_runtime_transport_session*wire=NULL;lt48_status cs;int done;lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cs,&done);if(!done){z->cleanup_status=cs;z->cleanup_completed=0;}}}
 if(!st){if(op==1)memcpy(out,actual+a,n);if(op==10)memcpy(out,actual,4);z->completed_bytes=op==10?4:n;}free(actual);free(expected);return st;
}
