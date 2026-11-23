/* Serial EPROM owner: source counter workers operate from address zero.
 * Writes require whole images. Partial reads acquire then slice the full image. */
static lt48_status sep7_validate(const lt48_gaaf_serial_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r||!r->sep7||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;
 if(op<1||op>7||(op==7&&!r->waves[5]))return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if(type>2||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
static lt48_status sep7_execute(const lt48_gaaf_serial_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t os,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_gaaf_serial_result*z){
 if(!z)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=sep7_validate(r,op,a,n,im,size,margin,type);if(!st&&(!api||!api->read||!api->write))st=LT48_INVALID_ARGUMENT;if(!st&&op==1&&(!read||os<n))st=LT48_BUFFER_TOO_SMALL;if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual){z->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 lt48_gaaf_serial_frame f;lt48_gaaf_serial_frame_init(r,&f);lt48_runtime_transport_session*w=NULL;st=lt48_runtime_transport_session_create(api,ctx,&w);if(!st)st=lt48_gaaf_serial_setup(r,&f,w);
 if(!st&&r->waves[2])for(unsigned attempt=0;attempt<64;attempt++){
  uint16_t man=0,dev=0;st=lt48_gaaf_serial_read_id(r,&f,w,&man,&dev);if(st)break;if(man==r->manufacturer&&dev==r->device)break;
  lt48_m37_id_choice choice=decide?decide(dc,r->manufacturer,r->device,man,dev):LT48_M37_ID_ABORT;
  if(choice==LT48_M37_ID_IGNORE)break;
  if(choice!=LT48_M37_ID_RETRY){st=LT48_DEVICE_MISMATCH;break;}if(attempt==63)st=LT48_SAFETY_REJECTED;
 }
 int writing=op==3||op==4;if(!st)st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,1,r->nominal,actual,r->capacity);
 if(writing){for(size_t i=0;!st&&i<r->capacity;i++)if(op==3?actual[i]!=255:(actual[i]&im[i])!=im[i])st=LT48_SAFETY_REJECTED;if(!st)st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,2,r->nominal,(unsigned char*)im,r->capacity);}
 if(!st&&op==7){f.mode=7;st=lt48_runtime_frame_set_mode(f.buffers,r->frame_profile.buffer_count,r->frame_profile.reset_mask,r->frame_profile.mode_buffer,7,0,0);if(!st)st=lt48_gaaf_serial_wave(r,&f,w,3);if(!st)st=lt48_gaaf_serial_wave(r,&f,w,5);if(!st)st=lt48_gaaf_serial_wave(r,&f,w,r->reset_op);}
 if(!st&&op!=1){unsigned delta=type==2?0:r->nominal/(type?10:20),v[2]={r->nominal-delta,r->nominal+delta},count=type==2?1:2;if(!writing&&op!=7){if(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL){v[0]=r->nominal;count=1;}else if(margin==LT48_VERIFY_MARGIN_LOW)count=1;else if(margin==LT48_VERIFY_MARGIN_HIGH){v[0]=v[1];count=1;}}
  for(unsigned k=0;!st&&k<count;k++){st=lt48_gaaf_serial_stream(r,&f,w,api,ctx,1,v[k],actual,r->capacity);if(!st)st=gaaf_compare(actual+a,im,a,n,op==2||op==7,z);}
 }
 z->operation_status=st;lt48_runtime_transport_session_close(w);lt48_protocol_transport recovery=*api;recovery.cancelled=NULL;z->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,&recovery,ctx,st);z->cleanup_completed=z->cleanup_status==LT48_OK;if(!st)st=z->cleanup_status;if(!st){if(op==1)memcpy(read,actual+a,n);z->completed_bytes=n;}free(actual);return st;
}
