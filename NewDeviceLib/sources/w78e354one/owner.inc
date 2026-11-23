/* W78E354: four watchdog/supervisor options are not code-protection levels.
 * Preserve the entire physical option byte and ROM-MAP; no option/lock editor. */
static lt48_status w354_cleanup(xp_session*s,lt48_status operation){
 const lt48_xp_eprom_resources*r=s->r;
 const lt48_protocol_transport*original=s->api;
 lt48_protocol_transport api=*original;
 api.cancelled=NULL;
 lt48_status result=LT48_OK,st;
 
 lt48_runtime_transport_session_close(s->wire);
 s->wire=NULL;
 s->api=&api;
 
 for(unsigned phase=0;phase<8;phase++){
  st=lt48_runtime_transport_session_create(&api,s->context,&s->wire);
 
  if(!st&&phase==0){const unsigned char stop[]={0,0};
 unsigned char reply[512]={0};
 st=cmd(s,stop,2,reply);
 if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;
 }
  else if(!st&&phase==1)st=run(s,3);
 
  else if(!st&&phase==2)st=run(s,1);
 
  else if(!st&&phase>=3&&phase<7){unsigned char off[]={0x10,6-phase,0};
 st=lt48_runtime_transport_session_controller_write(s->wire,off,3);
 }
  else if(!st){const unsigned char head[]={0x38,0xaa,0x55},finish[]={0x49,0xaa,0x55},led[]={0x40,operation==LT48_OK&&result==LT48_OK?4:2};
 st=lt48_runtime_transport_session_controller_write(s->wire,head,3);
 if(!st)st=lt48_runtime_transport_session_controller_write(s->wire,r->idle,r->idle_size);
 if(!st)st=lt48_runtime_transport_session_controller_write(s->wire,finish,3);
 if(!st)st=lt48_runtime_transport_session_controller_write(s->wire,led,2);
 }
  if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 if(!result&&st)result=st;
 lt48_runtime_transport_session_close(s->wire);
 s->wire=NULL;
 
 }s->api=original;
 return result;
 
}
static lt48_status w354_settings(xp_session*s,unsigned*option,unsigned*map){
 s->mode=14;
 lt48_status st=run(s,2);
 if(!st)st=run(s,6);
 if(!st)*option=s->buffers[15][0]^255u;
 
 if(!st)st=run(s,5);
 if(!st)*map=s->buffers[16][0];
 if(!st)st=run(s,3);
 return st;
 
}
static lt48_status w354_restore(xp_session*s,unsigned raw){
 unsigned current=0,map=0;
 lt48_status st=w354_settings(s,&current,&map);
 if(st||current==raw)return st;
 
 if((current&raw)!=raw)return LT48_SAFETY_REJECTED;
 
 s->mode=13;
 st=run(s,2);
 s->buffers[15][0]=(unsigned char)raw;
 
 if(!st)st=run(s,11);
 if(!st&&s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
 if(!st)st=run(s,3);
 
 if(!st)st=w354_settings(s,&current,&map);
 if(!st&&current!=raw)st=LT48_DEVICE_MISMATCH;
 return st;
 
}
static lt48_status w354_compare(const unsigned char*actual,const unsigned char*expected,uint32_t base,size_t n,int blank,lt48_xp_eprom_result*z){
 z->mismatch_count=0;
 z->first_mismatch=UINT32_MAX;
 
 for(size_t i=0;i<n;i++)if(actual[i]!=(blank?255:expected[i])){if(!z->mismatch_count)z->first_mismatch=base+(uint32_t)i;
 z->mismatch_count++;
 }
 return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
 
}
static lt48_status execute_w78e354one(const lt48_xp_eprom_resources*r,const lt48_protocol_transport*api,void*context,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*decision_context,lt48_xp_eprom_result*z){
 if(!z)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof(*z));
 z->first_mismatch=UINT32_MAX;
 
 lt48_status st=lt48_xp_eprom_validate(r,op,start,n,image,size);
 if(!st&&(!api||!api->read||!api->write))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==1&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(!st)st=lt48_xp_eprom_validate_verify_options(r,op,margin,type);
 if(st){z->operation_status=st;
 return st;
 }
 unsigned char*actual=malloc(r->capacity);
 if(!actual){z->operation_status=LT48_NO_MEMORY;
 return LT48_NO_MEMORY;
 }
 xp_session s={0};
 s.r=r;
 s.api=api;
 s.context=context;
 s.mode=1;
 s.protection=1;
 memcpy(s.vars,r->vars,sizeof(s.vars));
 for(unsigned j=0;j<4;j++)s.buffers[1][j]=(unsigned char)(r->capacity>>(8*j));
 
 unsigned option=0,map=0,after=0,aftermap=0;
 int known=0,writing=op==3||op==4||op==7;
 
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 if(!st)st=setup(&s);
 if(!st)st=identify(&s,decide,decision_context);
 if(!st){st=w354_settings(&s,&option,&map);
 known=!st;
 }
 if(!st)st=cycle(&s,1,0,r->capacity,NULL,actual,r->nominal,z);
 
 if(!st&&(op==2||op==3))st=w354_compare(actual+(op==3?0:start),NULL,op==3?0:start,op==3?r->capacity:n,1,z);
 
 if(!st&&(op==5||op==6))st=w354_compare(actual+start,image,start,n,0,z);
 
 if(!st&&op==4)for(size_t i=0;i<n;i++)if((actual[i]&image[i])!=image[i]){z->first_mismatch=start+(uint32_t)i;
 z->mismatch_count=1;
 st=LT48_DEVICE_MISMATCH;
 break;
 }
 if(!st&&(op==2||op==3))st=cycle(&s,11,op==3?0:start,op==3?r->capacity:n,NULL,NULL,r->nominal,z);
 
 if(!st&&(op==3||op==4))st=cycle(&s,2,0,n,image,NULL,r->nominal,z);
 
 if(!st&&op==7){s.mode=7;
 memset(s.buffers[0],0,32);
 st=run(&s,2);
 if(!st)st=run(&s,10);
 if(!st&&s.buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
 if(!st)st=run(&s,3);
 }
 if(!st&&writing){st=cycle(&s,1,0,r->capacity,NULL,actual,r->nominal,z);
 if(!st)st=w354_compare(actual,op==7?NULL:image,0,r->capacity,op==7,z);
 }
 if(!st&&(writing||op==5)){
  unsigned low=margin_voltage(r,type,0),high=margin_voltage(r,type,1),volts[2]={low,high},passes=type==2?1:2;
 
  if(!writing&&margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;
 passes=1;
 }else if(!writing&&margin==LT48_VERIFY_MARGIN_LOW)passes=1;
 else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=high;
 passes=1;
 }
  for(unsigned i=0;!st&&i<passes;i++){
   if(op!=7)st=cycle(&s,4,writing?0:start,writing?r->capacity:n,image,NULL,volts[i],z);
 else st=cycle(&s,11,0,r->capacity,NULL,NULL,volts[i],z);
 
   if(!st)st=cycle(&s,1,0,r->capacity,NULL,actual,volts[i],z);
 if(!st)st=w354_compare(actual+(writing?0:start),op==7?NULL:image,writing?0:start,writing?r->capacity:n,op==7,z);
 
  }
 }
 z->operation_status=st;
 z->cleanup_status=w354_cleanup(&s,st);
 z->cleanup_completed=!z->cleanup_status;
 if(!st)st=z->cleanup_status;
 
 /* Use uncancelled recovery to restore settings even after a failed erase.
  * A refused or failed restoration remains an error; never claim preservation. */
 if(known&&writing){
  lt48_status operation=st;
 lt48_protocol_transport recovery=*api;
 recovery.cancelled=NULL;
 s.api=&recovery;
 
  lt48_status restore=lt48_runtime_transport_session_create(&recovery,context,&s.wire);
 if(!restore)restore=setup(&s);
 if(!restore)restore=w354_restore(&s,option);
 if(!restore)restore=w354_settings(&s,&after,&aftermap);
 if(!restore&&(after!=option||aftermap!=map))restore=LT48_DEVICE_MISMATCH;
 
  if(!restore&&!operation){restore=cycle(&s,1,0,r->capacity,NULL,actual,r->nominal,z);
 if(!restore)restore=w354_compare(actual,op==7?NULL:image,0,r->capacity,op==7,z);
 }
  z->cleanup_status=w354_cleanup(&s,restore?restore:operation);
 z->cleanup_completed=!z->cleanup_status;
 
  if(restore){z->cleanup_status=restore;
 z->cleanup_completed=0;
 }st=operation?operation:restore?restore:z->cleanup_status;
 s.api=api;
 
 }
 if(!st&&!writing){st=lt48_runtime_transport_session_create(api,context,&s.wire);
 if(!st)st=setup(&s);
 if(!st)st=w354_settings(&s,&after,&aftermap);
 if(!st&&(after!=option||aftermap!=map))st=LT48_DEVICE_MISMATCH;
 z->cleanup_status=w354_cleanup(&s,st);
 z->cleanup_completed=!z->cleanup_status;
 if(!st)st=z->cleanup_status;
 }
 z->operation_status=st;
 if(!st){z->completed_bytes=n;
 if(op==1)memcpy(read,actual+start,n);
 }free(actual);
 return st;
 
}
