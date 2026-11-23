/* Combined x8 ST arrays. Original host transport/setup and finite worker
 * deltas; no VID protection operations or OTP programming are exposed. */
static lt48_status st2009two_locks(const lt48_gaaf_serial_resources*r,const lt48_gaaf_serial_frame*f,unsigned*out){
 unsigned n=r->definition==0x14?8:4,seen=0,value=0;
 for(size_t i=0;i<f->configuration_count;i++){
  const lt48_runtime_configuration_value*c=&f->configuration[i];
  if(!c->object_id&&!c->category&&c->field<n){unsigned bit=1u<<c->field;if(seen&bit)return LT48_RUNTIME_ERROR;seen|=bit;if(c->value)value|=bit;}
 }
 if(seen!=(1u<<n)-1)return LT48_RUNTIME_ERROR;
 *out=value;return LT48_OK;
}
static lt48_status st2009two_execute(const lt48_gaaf_serial_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char*image,size_t image_size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void*decision_ctx,lt48_gaaf_serial_result*out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_gaaf_serial_validate(r,op,start,length,image,image_size,margin,margin_type);
 if(st==LT48_OK&&(!api||!api->read||!api->write))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){out->operation_status=st;return st;}
 size_t mutable=r->capacity-64;unsigned flash=r->definition==0x14?524288:262144;
 int writing=op==3||op==4||op==7;
 unsigned char*pending=malloc(r->capacity),*expected=writing?malloc(r->capacity):NULL;
 if(!pending||(writing&&!expected)){free(pending);free(expected);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 lt48_gaaf_serial_frame f;lt48_gaaf_serial_frame_init(r,&f);lt48_runtime_transport_session*w=NULL;
 st=lt48_runtime_transport_session_create(api,ctx,&w);if(st==LT48_OK)st=lt48_gaaf_serial_setup(r,&f,w);
 for(unsigned attempt=0;st==LT48_OK&&attempt<64;attempt++){
  uint16_t man=0,id=0;st=lt48_gaaf_serial_read_id(r,&f,w,&man,&id);if(st!=LT48_OK||((man==r->manufacturer)&&(id==r->device)))break;
  lt48_runtime_transport_session_close(w);w=NULL;st=lt48_gaaf_serial_cleanup(r,&f,api,ctx,LT48_DEVICE_MISMATCH);if(st!=LT48_OK)break;
  lt48_m37_id_choice c=decide?decide(decision_ctx,r->manufacturer,r->device,man,id):LT48_M37_ID_ABORT;
  if(c!=LT48_M37_ID_IGNORE&&c!=LT48_M37_ID_RETRY){st=LT48_DEVICE_MISMATCH;break;}
  if(attempt==63&&c==LT48_M37_ID_RETRY){st=LT48_SAFETY_REJECTED;break;}
  lt48_gaaf_serial_frame_init(r,&f);st=lt48_runtime_transport_session_create(api,ctx,&w);if(st==LT48_OK)st=lt48_gaaf_serial_setup(r,&f,w);if(c==LT48_M37_ID_IGNORE)break;
 }
 unsigned locks=0,after=0;
 if(st==LT48_OK)st=gaaf_stream_range(r,&f,w,api,ctx,1,r->nominal,pending,r->capacity,0);
 if(st==LT48_OK)st=st2009two_locks(r,&f,&locks);
 if(st==LT48_OK&&writing){
  memcpy(expected,pending,r->capacity);
  if(op==7)memset(expected,255,mutable);
  else if(memcmp(image+mutable,pending+mutable,64))st=LT48_SAFETY_REJECTED;
  else memcpy(expected,image,mutable);
  if(st==LT48_OK&&locks)st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK&&op==4)for(unsigned a=0;a<flash;a++)if((pending[a]&expected[a])!=expected[a]){st=LT48_SAFETY_REJECTED;break;}
  if(st==LT48_OK&&(op==3||op==7))st=lt48_gaaf_serial_erase(r,&f,w);
  if(st==LT48_OK)st=gaaf_stream_range(r,&f,w,api,ctx,2,r->nominal,expected,mutable,0);
 }
 if(st==LT48_OK&&!writing&&op!=1){size_t count=length;if(op==2&&start+count>mutable)count=start<mutable?mutable-start:0;st=gaaf_compare(pending+start,image,start,count,op==2,out);}
 if(st==LT48_OK&&(writing||op==5)){
  unsigned delta=margin_type==2?0:r->nominal/(margin_type?10:20),volts[2]={r->nominal-delta,r->nominal+delta},count=margin_type==2?1:2;
  if(!writing&&margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;count=1;}else if(!writing&&margin==LT48_VERIFY_MARGIN_LOW)count=1;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];count=1;}
  for(unsigned i=0;st==LT48_OK&&i<count;i++){
   st=gaaf_stream_range(r,&f,w,api,ctx,1,volts[i],pending,r->capacity,0);
   if(st==LT48_OK)st=st2009two_locks(r,&f,&after);
   if(st==LT48_OK&&after!=locks)st=LT48_SAFETY_REJECTED;
   if(st==LT48_OK)st=gaaf_compare(pending+(writing?0:start),writing?expected:image,writing?0:start,writing?r->capacity:length,0,out);
  }
 }
 out->operation_status=st;lt48_runtime_transport_session_close(w);w=NULL;
 out->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,api,ctx,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(st==LT48_OK)st=out->cleanup_status;
 /* Fresh power/setup/raw Read checks retention and preserved OTP separately. */
 if(st==LT48_OK&&writing){
  lt48_gaaf_serial_frame_init(r,&f);st=lt48_runtime_transport_session_create(api,ctx,&w);if(st==LT48_OK)st=lt48_gaaf_serial_setup(r,&f,w);
  if(st==LT48_OK)st=gaaf_stream_range(r,&f,w,api,ctx,1,r->nominal,pending,r->capacity,0);
  if(st==LT48_OK)st=st2009two_locks(r,&f,&after);
  if(st==LT48_OK&&after!=locks)st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK)st=gaaf_compare(pending,expected,0,r->capacity,0,out);
  lt48_runtime_transport_session_close(w);out->operation_status=st;out->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,api,ctx,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(st==LT48_OK)st=out->cleanup_status;
 }
 if(st==LT48_OK){out->completed_bytes=length;if(op==1)memcpy(read,pending+start,length);}
 free(expected);free(pending);return st;
}
