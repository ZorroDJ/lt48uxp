/* Source workers count the virtual FF prefix from address zero. Every Read
 * streams that original image and slices only after successful cleanup. */
static unsigned f41_start(const lt48_gaaf_serial_resources*r){for(size_t i=0;i<r->variable_count;i++)if(r->variables[i].variable_id==14)return r->variables[i].value|((unsigned)r->variables[i].channel<<16);return r->capacity;}
static lt48_status f41_validate(const lt48_hitachi_resources*o,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!o||!n||a>=o->core->capacity||n>o->core->capacity-a)return LT48_INVALID_ARGUMENT;
 if(op!=1&&op!=2&&op!=3&&op!=5&&op!=6&&op!=7)return LT48_UNSUPPORTED;
 if(type>2||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==3||op==5||op==6)&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if((op==3||op==7)&&(a||n!=o->core->capacity))return LT48_SAFETY_REJECTED;
 if(op==3)for(unsigned i=0;i<f41_start(o->core);i++)if(im[i]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
static lt48_status f41_execute(const lt48_hitachi_resources*o,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_hitachi_result*z){
 if(!z)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=f41_validate(o,op,a,n,im,size,margin,type);
 if(!st&&(!api||!api->write||!api->read))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==1&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 const lt48_gaaf_serial_resources*r=o->core;unsigned char*actual=malloc(r->capacity),*expected=malloc(r->capacity);
 if(!actual||!expected){free(actual);free(expected);z->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 memset(expected,255,r->capacity);if(im)memcpy(expected+a,im,n);
 lt48_gaaf_serial_frame f;lt48_gaaf_serial_frame_init(r,&f);lt48_runtime_transport_session*w=NULL;
 st=lt48_runtime_transport_session_create(api,ctx,&w);if(!st)st=lt48_gaaf_serial_setup(r,&f,w);
 if(!st&&op==7)st=lt48_gaaf_serial_erase(r,&f,w);
 if(!st)st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,1,r->nominal,actual,r->capacity);
 if(!st&&op!=1)st=compare(actual+a,op==3||op==2||op==7?NULL:expected+a,a,n,z);
 if(!st&&(op==2||op==3||op==7))st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,11,r->nominal,actual,r->capacity);
 if(!st&&op==5){memcpy(expected,actual,a);memcpy(expected+a+n,actual+a+n,r->capacity-a-n);}
 if(!st&&op==3)st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,2,r->nominal,expected,r->capacity);
 if(!st&&(op==3||op==5||op==7)){
  unsigned delta=type==2?0:r->nominal/(type?10:20),volts[2]={r->nominal-delta,r->nominal+delta},passes=type==2?1:2;
  if(op==5){if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;passes=1;}else if(margin==LT48_VERIFY_MARGIN_LOW)passes=1;else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];passes=1;}}
  for(unsigned i=0;!st&&i<passes;i++)st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,4,volts[i],expected,r->capacity);
  if(!st)st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,1,r->nominal,actual,r->capacity);
  if(!st)st=compare(actual+a,expected+a,a,n,z);
 }
 z->operation_status=st;lt48_runtime_transport_session_close(w);w=NULL;
 z->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,api,ctx,st);z->cleanup_completed=!z->cleanup_status;if(!st)st=z->cleanup_status;
 if(!st&&(op==3||op==7)){
  lt48_gaaf_serial_frame_init(r,&f);st=lt48_runtime_transport_session_create(api,ctx,&w);if(!st)st=lt48_gaaf_serial_setup(r,&f,w);
  if(!st)st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,1,r->nominal,actual,r->capacity);
  if(!st)st=compare(actual,expected,0,r->capacity,z);
  z->operation_status=st;lt48_runtime_transport_session_close(w);z->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,api,ctx,st);z->cleanup_completed=!z->cleanup_status;if(!st)st=z->cleanup_status;
 }
 if(!st){if(op==1)memcpy(read,actual+a,n);z->completed_bytes=n;}
 free(actual);free(expected);return st;
}
