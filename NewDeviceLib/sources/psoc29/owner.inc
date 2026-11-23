/* PSoC1 source ABI: B7 mode, B0 address, B1 code size. Worker counters
 * start at zero; partial reads stream the whole array then slice. */
static void p29_address(s8253_session*s){memset(s->buffers[0],0,32);for(unsigned k=0;k<4;k++)s->buffers[1][k]=s->r->capacity>>(8*k);}
static lt48_status p29_cycle(s8253_session*s,unsigned mode,unsigned char*data,unsigned cv){
 s->mode=mode;p29_address(s);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);
 /* Original INIT mode2 invokes ERASE. Retain it for both programming modes. */
 if(!st&&mode==2&&s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
 for(size_t off=0;!st&&off<s->r->capacity;){size_t n=s->r->capacity-off;if(n>4096)n=4096;st=block_s8253(s,mode!=1,data+off,n);off+=n;}
 if(!st){st=stop_s8253(s);}
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status p29_finish(s8253_session*s,lt48_status st,lt48_serial_page_result*z){
 z->operation_status=st;lt48_protocol_transport clean=*s->api;clean.cancelled=NULL;s->api=&clean;const unsigned phases[]={0,1,2,3};return lt48_owner_finish_internal(&clean,s->ctx,&s->wire,cleanup_sst,s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
}
static lt48_status execute_psoc29(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 if(!z){return LT48_INVALID_ARGUMENT;}
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_psoc29_validate(r,op,a,n,im,size);
 if(!st&&(!api||!api->read||!api->write||type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==1&&(!out||outsize<n)){st=LT48_BUFFER_TOO_SMALL;}
 if(st){z->operation_status=st;return st;}
 unsigned char*actual=malloc(r->capacity),*expected=malloc(r->capacity);if(!actual||!expected){free(actual);free(expected);z->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);p29_address(&s);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;if(!st)st=sm16_identify(&s,decide,dc);
 int writing=op==3||op==4;
 if(!st&&op==8){
  /* Original indexed PROTECT reads the external HEX security record at100000.
   * Require the complete explicit record. No invented default lock level/readback. */
  unsigned char*record=calloc(0x100000+r->psoc_security_bytes,1);if(!record)st=LT48_NO_MEMORY;else{memcpy(record+0x100000,im,r->psoc_security_bytes);s.device_image=record;s.device_image_size=0x100000+r->psoc_security_bytes;s.mode=8;p29_address(&s);st=w78bank_voltage(&s,r->nominal);if(!st)st=run_s8253(&s,11);if(!st&&s.buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;s.device_image=NULL;s.device_image_size=0;free(record);}
 }else if(!st){
  if(writing){memcpy(expected,im,r->capacity);st=p29_cycle(&s,2,expected,r->nominal);}
  else if(op==7){s.mode=7;p29_address(&s);st=w78bank_voltage(&s,r->nominal);if(!st)st=run_s8253(&s,10);if(!st&&s.buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;memset(expected,255,r->capacity);}
  else {st=p29_cycle(&s,1,actual,r->nominal);if(!st){memcpy(expected,actual,r->capacity);if(op==5||op==6)memcpy(expected+a,im,n);if(op!=1)st=w78bank_compare(z,a,actual+a,op==2?NULL:im,n);}}
  if(!st&&(writing||op==7||op==5)){
   unsigned cv[2]={type?r->nominal*90/100:r->nominal*95/100,type?r->nominal*110/100:r->nominal*105/100};
   /* Original explicit VerifyVCC0/1 values override the calculated pair. */
   for(unsigned k=0;k<r->variable_count;k++)if(r->vars[k].kind==0&&r->vars[k].variable_id>=2&&r->vars[k].variable_id<=3)cv[r->vars[k].variable_id-2]=r->vars[k].value;
   unsigned passes=writing||op==7||margin==LT48_VERIFY_MARGIN_ALL?2:1;if(!writing&&op!=7&&margin==LT48_VERIFY_MARGIN_NOMINAL)cv[0]=r->nominal;else if(!writing&&op!=7&&margin==LT48_VERIFY_MARGIN_HIGH)cv[0]=cv[1];
   if(op==7)st=p29_cycle(&s,1,actual,r->nominal);
   if(!st&&op==7)st=w78bank_compare(z,0,actual,NULL,r->capacity);
   for(unsigned k=0;!st&&k<passes;k++)st=p29_cycle(&s,4,expected,cv[k]);
   if(!st){st=p29_cycle(&s,1,actual,r->nominal);}
   if(!st){st=w78bank_compare(z,0,actual,expected,r->capacity);}
  }
 }
 st=p29_finish(&s,st,z);
 if(!st&&(writing||op==7)){
  s.api=api;s.ready=0;s.mode=1;memset(s.buffers,0,sizeof s.buffers);memset(s.volts,0,sizeof s.volts);memcpy(s.vars,r->vars,sizeof s.vars);p29_address(&s);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;if(!st)st=sm16_identify(&s,decide,dc);if(!st){st=p29_cycle(&s,1,actual,r->nominal);}
   if(!st){st=w78bank_compare(z,0,actual,expected,r->capacity);}st=p29_finish(&s,st,z);
 }
 if(!st){if(op==1)memcpy(out,actual+a,n);z->completed_bytes=op==8?r->psoc_security_bytes:n;}z->operation_status=st;free(actual);free(expected);return st;
}
