/* Class2004 combined-window owner. Original workers read/program main flash
 * then the 64KiB window selected by88. Sector captures are retained separately
 * from the original checkbox DWORD, whose indexed fields alias modulo32. */
typedef struct {const lt48_protocol_transport*api;void*ctx;unsigned observe,pending,count;unsigned char flags[256];} c36_wire;
static lt48_status c36_write(void*ctx,uint8_t ep,const unsigned char*b,size_t n,size_t*done){c36_wire*w=ctx;if(w->observe&&ep==2&&n&&b[0]==0x2c){unsigned captures=0;for(size_t i=1;i+1<n&&b[i];i+=2)captures+=b[i]==3;if(captures>1)return LT48_PROTOCOL_ERROR;w->pending=captures;}return w->api->write(w->ctx,ep,b,n,done);}
static lt48_status c36_read(void*ctx,uint8_t ep,unsigned char*b,size_t n,size_t*done){c36_wire*w=ctx;lt48_status s=w->api->read(w->ctx,ep,b,n,done);if(!s&&w->observe&&w->pending&&ep==0x88){if(*done!=n||n<3||w->count>=256)return LT48_PROTOCOL_ERROR;w->flags[w->count++]=b[2]&1;w->pending=0;}return s;}
static int c36_cancel(void*ctx){c36_wire*w=ctx;return w->api->cancelled&&w->api->cancelled(w->ctx);}
static unsigned c36_sectors(const lt48_gaaf_serial_resources*r){for(size_t i=0;i<r->variable_count;i++)if(r->variables[i].variable_id==21)return r->variables[i].value|((unsigned)r->variables[i].channel<<16);return 0;}
lt48_status lt48_combined36_validate_internal(const lt48_gaaf_serial_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r||!n||a>=r->capacity||n>r->capacity-a)return LT48_INVALID_ARGUMENT;
 if(op<LT48_OPERATION_READ||op>LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
 if(type>2||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
static lt48_status c36_snapshot(const lt48_gaaf_serial_resources*r,lt48_gaaf_serial_frame*f,lt48_runtime_transport_session*w,const lt48_protocol_transport*api,c36_wire*ctx,unsigned char*out){
 lt48_status s=lt48_runtime_transport_session_flush(w);ctx->count=ctx->pending=0;ctx->observe=1;
 if(!s)s=lt48_gaaf_serial_stream(r,f,w,api,ctx,1,r->nominal,out,r->capacity);
 ctx->observe=0;if(!s&&(ctx->pending||ctx->count!=c36_sectors(r)))s=LT48_PROTOCOL_ERROR;return s;
}
static lt48_status c36_flags(c36_wire*w,const unsigned char*saved,unsigned n){if(w->count!=n)return LT48_PROTOCOL_ERROR;return memcmp(w->flags,saved,n)?LT48_DEVICE_MISMATCH:LT48_OK;}
lt48_status lt48_combined36_execute_internal(const lt48_gaaf_serial_resources*r,const lt48_protocol_transport*original,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*decision_ctx,lt48_gaaf_serial_result*z){
 if(!z)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_combined36_validate_internal(r,op,a,n,im,size,margin,type);
 if(!st&&(!original||!original->read||!original->write))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==LT48_OPERATION_READ&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 unsigned sectors=c36_sectors(r);if(!sectors||sectors>256){z->operation_status=LT48_FORMAT_ERROR;return LT48_FORMAT_ERROR;}
 unsigned char*actual=malloc(r->capacity),*expected=malloc(r->capacity),saved[256];
 if(!actual||!expected){free(actual);free(expected);z->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 c36_wire monitor={.api=original,.ctx=ctx};lt48_protocol_transport api={.write=c36_write,.read=c36_read,.cancelled=c36_cancel};lt48_runtime_transport_session*w=NULL;lt48_gaaf_serial_frame f;lt48_gaaf_serial_frame_init(r,&f);int ignore=0;
 st=lt48_runtime_transport_session_create(&api,&monitor,&w);if(!st)st=lt48_gaaf_serial_setup(r,&f,w);
 for(unsigned attempt=0;!st&&attempt<64;attempt++){
  uint16_t man=0,id=0;st=lt48_gaaf_serial_read_id(r,&f,w,&man,&id);if(st|| (man==r->manufacturer&&id==r->device))break;
  lt48_runtime_transport_session_close(w);w=NULL;lt48_status clean=lt48_gaaf_serial_cleanup(r,&f,&api,&monitor,LT48_DEVICE_MISMATCH);if(clean){st=clean;break;}
  lt48_m37_id_choice choice=decide?decide(decision_ctx,r->manufacturer,r->device,man,id):LT48_M37_ID_ABORT;
  if(choice!=LT48_M37_ID_RETRY&&choice!=LT48_M37_ID_IGNORE){st=LT48_DEVICE_MISMATCH;break;}
  if(attempt==63&&choice==LT48_M37_ID_RETRY){st=LT48_SAFETY_REJECTED;break;}
  lt48_gaaf_serial_frame_init(r,&f);st=lt48_runtime_transport_session_create(&api,&monitor,&w);if(!st)st=lt48_gaaf_serial_setup(r,&f,w);if(choice==LT48_M37_ID_IGNORE){ignore=1;break;}
 }
 if(!st)st=c36_snapshot(r,&f,w,&api,&monitor,actual);
 if(!st)memcpy(saved,monitor.flags,sectors);
 int programming=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM,writing=programming||op==LT48_OPERATION_ERASE;
 /* Standalone sector changes are deferred. Unlike the original temporary-VID
  * branch, protected mutations reject before any programming/erase worker. */
 if(!st&&writing)for(unsigned i=0;i<sectors;i++)if(saved[i]){st=LT48_SAFETY_REJECTED;break;}
 if(!st){memcpy(expected,actual,r->capacity);if(im&&(programming||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))memcpy(expected+a,im,n);}
 else memset(expected,255,r->capacity);
 if(!st&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK))st=compare(actual+a,NULL,a,n,1,z);
 if(!st&&(op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))st=compare(actual+a,im,a,n,0,z);
 if(!st&&op==LT48_OPERATION_OVER_PROGRAM)for(size_t i=0;i<n;i++)if((actual[i]&im[i])!=im[i]){z->first_mismatch=(uint32_t)i;z->mismatch_count=1;st=LT48_DEVICE_MISMATCH;break;}
 if(!st&&(op==LT48_OPERATION_PROGRAM||(op==LT48_OPERATION_BLANK_CHECK&&!a&&n==r->capacity)))st=lt48_gaaf_serial_stream(r,&f,w,&api,&monitor,11,r->nominal,actual,r->capacity);
 if(!st&&programming)st=lt48_gaaf_serial_stream(r,&f,w,&api,&monitor,2,r->nominal,expected,r->capacity);
 if(!st&&op==LT48_OPERATION_ERASE){memset(expected,255,r->capacity);st=lt48_gaaf_serial_erase(r,&f,w);if(!st&&f.buffers[4][0]!=1)st=LT48_DEVICE_MISMATCH;}
 if(!st&&op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_BLANK_CHECK){
  unsigned delta=type==2?0:r->nominal/(type?10:20),v[2]={r->nominal-delta,r->nominal+delta},passes=type==2?1:2;
  if(!writing){if(margin==LT48_VERIFY_MARGIN_NOMINAL){v[0]=r->nominal;passes=1;}else if(margin==LT48_VERIFY_MARGIN_LOW)passes=1;else if(margin==LT48_VERIFY_MARGIN_HIGH){v[0]=v[1];passes=1;}}
  for(unsigned i=0;!st&&i<passes;i++)st=lt48_gaaf_serial_stream(r,&f,w,&api,&monitor,4,v[i],expected,r->capacity);
  if(!st)st=c36_snapshot(r,&f,w,&api,&monitor,actual);
  if(!st)st=compare(actual,expected,0,r->capacity,0,z);
  if(!st)st=c36_flags(&monitor,saved,sectors);
 }
 z->operation_status=st;lt48_runtime_transport_session_close(w);w=NULL;z->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,&api,&monitor,st);z->cleanup_completed=!z->cleanup_status;if(!st)st=z->cleanup_status;
 if(!st&&writing){
  lt48_gaaf_serial_frame_init(r,&f);st=lt48_runtime_transport_session_create(&api,&monitor,&w);if(!st)st=lt48_gaaf_serial_setup(r,&f,w);
  if(!st&&!ignore){uint16_t man,id;st=lt48_gaaf_serial_read_id(r,&f,w,&man,&id);if(!st&&(man!=r->manufacturer||id!=r->device))st=LT48_DEVICE_MISMATCH;}
  if(!st)st=c36_snapshot(r,&f,w,&api,&monitor,actual);
  if(!st)st=compare(actual,expected,0,r->capacity,0,z);
  if(!st)st=c36_flags(&monitor,saved,sectors);
  if(!st&&op==LT48_OPERATION_ERASE)st=lt48_gaaf_serial_stream(r,&f,w,&api,&monitor,11,r->nominal,expected,r->capacity);
  if(!st&&op==LT48_OPERATION_ERASE){unsigned delta=type==2?0:r->nominal/(type?10:20),v[2]={r->nominal-delta,r->nominal+delta};for(unsigned i=0;!st&&i<(type==2?1u:2u);i++)st=lt48_gaaf_serial_stream(r,&f,w,&api,&monitor,4,v[i],expected,r->capacity);}
  z->operation_status=st;lt48_runtime_transport_session_close(w);z->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,&api,&monitor,st);z->cleanup_completed=!z->cleanup_status;if(!st)st=z->cleanup_status;
 }
 if(!st){if(op==LT48_OPERATION_READ)memcpy(read,actual+a,n);z->completed_bytes=n;}z->operation_status=st;free(actual);free(expected);return st;
}
