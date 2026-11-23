/* Array-only owner: never reads a fictional lock flag or invokes Protect.
 * The original erase callback's forced success is followed by full raw Read.
 * Both Program forms erase first, matching the established common owner policy. */
static lt48_status execute_snas4(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_sn9_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_HIGH))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==1&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);
 if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 if(st){free(actual);return st;}
 st=setup_s8253(&s);if(!st)s.ready=1;
 int writing=op==3||op==4;
 if(!st&&(writing||op==7)){
  st=sn9_erase(&s);
  if(!st)st=sn9_cycle(&s,0,0,n,NULL,actual,r->nominal);
  if(!st)st=w78bank_compare(z,0,actual,NULL,n);
 }
 if(!st&&writing)st=sn9_cycle(&s,1,0,n,im,NULL,r->nominal);
 uint32_t volts[4]={0};unsigned count=0;
 if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
 if(r->definition==318)for(unsigned i=0;i<count;i++){if(volts[i]<300)volts[i]=300;if(volts[i]>360)volts[i]=360;}
 unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
 if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;
 else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
 for(unsigned pass=0;!st&&pass<passes;pass++){
  st=sn9_cycle(&s,0,a,n,NULL,actual,volts[pass]);
  if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2||op==7?NULL:im,n);
 }
 z->operation_status=st;
 lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;
 const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&(writing||op==7)){
  st=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
  if(!st)st=setup_s8253(&s);
  if(!st)st=sn9_cycle(&s,0,0,n,NULL,actual,r->nominal);
  if(!st)st=w78bank_compare(z,0,actual,op==7?NULL:im,n);
  z->operation_status=st;
  st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 }
 if(!st){if(op==1)memcpy(read,actual,n);z->completed_bytes=n;}
 free(actual);return st;
}
