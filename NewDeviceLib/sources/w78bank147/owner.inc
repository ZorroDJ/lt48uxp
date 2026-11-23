/* Banked W77/W78/W79 owner. Original callbacks execute intact. */
static unsigned w78bank_region(const lt48_s8253_resources*r,unsigned a,unsigned*base){unsigned off=0;for(unsigned i=0;i<r->regions;i++){if(a<off+r->region_length[i]){*base=off;return i;}off+=r->region_length[i];}*base=off;return r->regions;}
static void w78bank_address(s8253_session*s,unsigned a){unsigned base=0,k=w78bank_region(s->r,a,&base);unsigned physical=a-base,logical=s->r->region_bank[k]+physical;
 for(unsigned i=0;i<4;i++){s->buffers[0][i]=(unsigned char)(physical>>(8*i));s->buffers[1][i]=(unsigned char)(logical>>(8*i));}}
static lt48_status w78bank_settings(s8253_session*s,unsigned char*out){s->mode=13;
 w78bank_address(s,0);
 sst_config(s,NULL);
 lt48_status st=run_s8253(s,2);
 if(!st)st=run_s8253(s,6);
 for(unsigned i=0;
 !st&&i<s->r->fields;
 i++){if(s->config[i].value>1)st=LT48_DEVICE_MISMATCH;
 else out[i]=(unsigned char)s->config[i].value;
 }
 lt48_status reset=run_s8253(s,3);
 return st?st:reset;
 }
static lt48_status w78bank_apply(s8253_session*s,const unsigned char*saved,const unsigned char*want){
 for(unsigned i=0;
 i<s->r->fields;
 i++)if(want[i]<saved[i])return LT48_SAFETY_REJECTED;
 s->mode=13;
 w78bank_address(s,0);
 lt48_status st=run_s8253(s,2);
 sst_config(s,want);
 if(!st)st=run_s8253(s,11);
 if(!st&&s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);
 if(!st)st=reset;
 unsigned char got[8];
 if(!st)st=w78bank_settings(s,got);
 if(!st&&memcmp(got,want,s->r->fields))st=LT48_DEVICE_MISMATCH;
 return st;
}
static lt48_status w78bank_voltage(s8253_session*s,unsigned cv){unsigned ch=s->r->vars[0].channel;const unsigned char bytes[]={0x10,ch,(ch==0?20*cv/100:10*cv/100+13)};
 lt48_status st=lt48_runtime_transport_session_controller_write(s->wire,bytes,sizeof bytes);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 if(!st){s->vars[0].value=cv;s->volts[ch]=cv;}return st;
}
static lt48_status w78bank_cycle(s8253_session*s,int write,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=write?2:1;
 w78bank_address(s,a);
 lt48_status st=w78bank_voltage(s,cv);
 if(!st)st=run_s8253(s,2);
 /* INIT mode2 erases once; running it again between regions destroys main. */
 size_t done=0;
 while(!st&&done<n){unsigned current=a+(unsigned)done,base=0,k=w78bank_region(s->r,current,&base),end=base+s->r->region_length[k];
 size_t take=n-done;
 if(take>end-current)take=end-current;
  w78bank_address(s,current);
 if(!st)st=run_s8253(s,write?8:5);
  for(size_t off=0;
 !st&&off<take;
 ){size_t chunk=take-off;
 if(chunk>4096)chunk=4096;
 st=block_s8253(s,write,write?(unsigned char*)im+done+off:out+done+off,chunk);
 off+=chunk;
 }
  if(!st)st=stop_s8253(s);
 done+=take;
 }
 lt48_status reset=run_s8253(s,3);
 return st?st:reset;
}
static lt48_status w78bank_compare(lt48_serial_page_result*z,unsigned base,const unsigned char*got,const unsigned char*want,size_t n){
 z->mismatch_count=0;
 z->first_mismatch=UINT32_MAX;
 for(size_t i=0;
 i<n;
 i++)if(got[i]!=(want?want[i]:255)){if(!z->mismatch_count)z->first_mismatch=base+(unsigned)i;
 z->mismatch_count++;
 }
 return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status execute_w78bank147(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){

 if(!z||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);
 z->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_w78bank147_validate(r,op,a,n,im,size);
 if(!st&&(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<r->fields))))st=LT48_BUFFER_TOO_SMALL;
 if(st)return st;
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;
 z->cleanup_completed=1;
 return LT48_CANCELLED;
 }
 unsigned char*actual=malloc(r->capacity);
 if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};
 s.r=r;
 s.api=api;
 s.ctx=ctx;
 s.mode=1;
 memcpy(s.vars,r->vars,sizeof s.vars);
 sst_config(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 if(st){free(actual);
 return st;
 }
 st=setup_s8253(&s);
 if(!st)s.ready=1;
 for(unsigned attempt=0;!st&&attempt<64;attempt++){
 w78bank_address(&s,0);s.mode=1;st=run_s8253(&s,2);if(!st)st=run_s8253(&s,4);
 if(st)break;
 unsigned man=s.buffers[7][0],dev=s.buffers[5][0];
 if(man==r->manufacturer&&dev==r->device)break;
 lt48_m37_id_choice choice=decide?decide(dc,r->manufacturer,r->device,man,dev):LT48_M37_ID_ABORT;
 if(choice==LT48_M37_ID_IGNORE)break;
 if(choice!=LT48_M37_ID_RETRY||attempt==63)st=LT48_DEVICE_MISMATCH;
 }

 unsigned char saved[8]={0},want[8]={0};
 if(!st)st=w78bank_settings(&s,saved);
 int writing=op==3||op==4;
 if(!st&&op!=7&&op!=8&&op!=10&&!writing&&(saved[0]||saved[1]))st=LT48_SAFETY_REJECTED;
 if(!st&&(op==8||op==10)){
  memcpy(want,saved,r->fields);
 if(op==8)want[0]=1;
 else if(im)for(unsigned i=0;
 i<r->fields;
 i++)if(im[0]&(1u<<i))want[i]=im[i+1];
  if(memcmp(want,saved,r->fields))st=w78bank_apply(&s,saved,want);
 if(!st&&op==10)memcpy(actual,want,r->fields);
 }else if(!st&&op==7){s.mode=7;
 w78bank_address(&s,0);
 st=run_s8253(&s,2);
 if(!st)st=run_s8253(&s,10);
 if(!st&&s.buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
  lt48_status reset=run_s8253(&s,3);
 if(!st)st=reset;
 if(!st)st=w78bank_settings(&s,want);
 for(unsigned i=0;
 !st&&i<r->fields;
 i++)if(want[i])st=LT48_DEVICE_MISMATCH;
  if(!st)st=w78bank_cycle(&s,0,0,r->capacity,NULL,actual,r->nominal);
 if(!st)st=w78bank_compare(z,0,actual,NULL,r->capacity);
 }else if(!st){
  if(writing){st=w78bank_cycle(&s,1,0,n,im,NULL,r->nominal);
 if(!st)st=w78bank_settings(&s,want);
 for(unsigned i=0;
 !st&&i<r->fields;
 i++)if(want[i])st=LT48_DEVICE_MISMATCH;
 }
  uint32_t volts[4]={0};
 unsigned count=0;
 if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;
 else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;
 !st&&pass<passes;
 pass++){st=w78bank_cycle(&s,0,a,n,NULL,actual,volts[pass]);
 if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2?NULL:im,n);
 }
  /* Erase-before-program clears security; restore non-lock options afterward. */
  if(!st&&writing){memcpy(want,saved,r->fields);
 want[0]=want[1]=0;
 unsigned char blank[8]={0};
 if(memcmp(want,blank,r->fields))st=w78bank_apply(&s,blank,want);
 }
 }
 z->operation_status=st;
 lt48_protocol_transport clean=*api;
 clean.cancelled=NULL;
 s.api=&clean;
 const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual,n);
 if(op==10)memcpy(read,actual,r->fields);
 z->completed_bytes=op==10?r->fields:n;
 }
 free(actual);
 return st;
}
