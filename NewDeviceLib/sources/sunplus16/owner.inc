/* Sunplus SPMC65P: two exposed AddTab regions and four hidden bytes.
 * All source callbacks use a private 64KiB physical-address view. */
static const unsigned su16_hidden[]={0x7fe0,0x7fe2,0x7fe3,0x7fef};
static void su16_address(s8253_session*s,unsigned k){
 for(unsigned b=0;b<4;b++){s->buffers[0][b]=(unsigned char)(s->r->n79_physical[k]>>(8*b));s->buffers[1][b]=(unsigned char)(s->r->region_bank[k]>>(8*b));s->buffers[2][b]=(unsigned char)(s->r->capacity>>(8*b));s->buffers[9][b]=b?0:1;}
}
static lt48_status su16_settings(s8253_session*s,unsigned char saved[4],unsigned*locked){
 s->mode=13;su16_address(s,0);numicro_config(s,NULL);lt48_status st=run_s8253(s,2);if(!st)st=run_s8253(s,6);
 if(!st){for(unsigned i=0;i<4;i++)saved[i]=s->device_image[su16_hidden[i]];*locked=s->config[0].value;if(*locked>1)st=LT48_DEVICE_MISMATCH;}
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status su16_read(s8253_session*s,unsigned char*out,unsigned cv){
 s->mode=1;su16_address(s,0);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);size_t done=0;
 for(unsigned k=0;!st&&k<s->r->regions;k++){su16_address(s,k);st=run_s8253(s,5);size_t n=s->r->region_length[k];
  for(size_t q=0;!st&&q<n;){size_t take=n-q;if(take>4096)take=4096;st=block_s8253(s,0,out+done+q,take);q+=take;}
  if(!st){st=stop_s8253(s);}
  done+=n;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status su16_preserved(s8253_session*s,const unsigned char expected[4],unsigned lock){
 unsigned char got[4];unsigned locked=0;lt48_status st=su16_settings(s,got,&locked);return st?st:locked!=lock||memcmp(got,expected,4)?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status su16_program(s8253_session*s,const unsigned char*im,const unsigned char saved[4]){
 memcpy(s->saved,saved,4);s->saved[4]=1;s->mode=2;su16_address(s,0);lt48_status st=run_s8253(s,2);size_t done=0;
 for(unsigned k=0;!st&&k<s->r->regions;k++){su16_address(s,k);st=run_s8253(s,8);size_t n=s->r->region_length[k];
  for(size_t q=0;!st&&q<n;){size_t take=n-q;if(take>4096)take=4096;st=a28_block(s,1,(unsigned char*)im+done+q,take);q+=take;}
  if(!st){unsigned char end[]={0x0c,1},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;else if(!st&&!(reply[2]&1)){unsigned char actual;lt48_status drain=block_s8253(s,0,&actual,1);st=drain?drain:LT48_DEVICE_MISMATCH;}}
  if(!st){st=stop_s8253(s);}
  done+=n;
 }
 /* RESET mode2 runs the original four hidden-byte restoration workers.
  * Attempt it on failure too, then independently check every preserved byte. */
 lt48_status reset=run_s8253(s,3);if(!st)st=reset;
 lt48_status preservation=su16_preserved(s,saved,0);if(!preservation)s->saved[4]=0;return st?st:preservation;
}
static lt48_status su16_cleanup(void*context,unsigned phase){s8253_session*s=context;
 if(phase==1&&s->ready&&s->saved[4]){for(unsigned i=0;i<4;i++)s->device_image_update[su16_hidden[i]]=s->saved[i];s->mode=2;su16_address(s,0);lt48_status st=run_s8253(s,2);if(!st)st=run_s8253(s,3);if(!st)st=su16_preserved(s,s->saved,0);if(!st)s->saved[4]=0;lt48_status reset=cleanup_sst(s,phase);return st?st:reset;}
 return cleanup_sst(context,phase);
}
static lt48_status execute_su16(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_su16_validate(r,op,a,n,im,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_HIGH))st=LT48_INVALID_ARGUMENT;
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<4))))st=LT48_BUFFER_TOO_SMALL;
 if(st)return z->operation_status=st;
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity),*shadow=malloc(65536);if(!actual||!shadow){free(actual);free(shadow);return LT48_NO_MEMORY;}memset(shadow,255,65536);
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);s.device_image=shadow;s.device_image_update=shadow;s.device_image_size=65536;numicro_config(&s,NULL);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;
 unsigned char saved[4]={0},want[4]={0};unsigned locked=0;if(!st)st=su16_settings(&s,saved,&locked);
 if(!st&&locked&&op!=8&&op!=10)st=LT48_SAFETY_REJECTED;
 if(!st&&(op==8||op==10)){
  unsigned requested=op==8?1:im&&(im[0]&1)?im[4]:locked;
  if(locked&&!requested)st=LT48_SAFETY_REJECTED;
  if(!st&&requested!=locked){s.mode=13;su16_address(&s,0);st=run_s8253(&s,2);uint32_t cfg[32]={1};numicro_config(&s,cfg);if(!st)st=run_s8253(&s,11);if(!st&&s.buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;lt48_status reset=run_s8253(&s,3);if(!st)st=reset;memcpy(want,saved,4);want[1]&=0x7f;if(!st)st=su16_preserved(&s,want,1);}
  if(!st&&op==10){memset(read,0,4);read[0]=(unsigned char)requested;}
 }else if(!st){
  int writing=op==3||op==4;
  if(writing){st=su16_read(&s,actual,r->nominal);for(size_t i=0;!st&&i<n;i++)if((actual[i]&im[i])!=im[i])st=LT48_SAFETY_REJECTED;
   size_t off=0;for(unsigned k=0;k<r->regions;k++){memcpy(shadow+r->n79_physical[k],im+off,r->region_length[k]);off+=r->region_length[k];}
   if(!st)st=su16_program(&s,im,saved);
  }
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){st=su16_read(&s,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual+a,op==2?NULL:im,n);}
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,su16_cleanup,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st){if(op==1)memcpy(read,actual+a,n);z->completed_bytes=op==10?4:n;}free(actual);free(shadow);return st;
}
