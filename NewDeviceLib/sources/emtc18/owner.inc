/* Authored Page-memory owner: exact source setup/downloads, bulk 48 programming,
 * independent byte Read verification, software protection via unchanged rewrite. */
static lt48_status emtc18_cycle(xp_session*s,unsigned mode,uint32_t start,size_t length,
 const unsigned char*image,unsigned char*read,uint16_t vcc,lt48_xp_flash_result*out){
 for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(start>>(8*j));
 s->mode=mode;s->vars[0].value=vcc;
 lt48_status st=run(s,2);out->completed_bytes=0;
 /* TPageMemGroup: floor(page_count/100), clamp to one..8192/page. */
 size_t pages=length/128u/100u;if(!pages)pages=1;if(pages>64)pages=64;size_t chunk=pages*128;
 for(size_t offset=0;st==LT48_OK&&offset<length;){
  size_t n=length-offset;if(n>chunk)n=chunk;
  st=lt48_runtime_transport_session_wave_watchdog(s->wire,mode==2?60000:5000,0);
  if(st!=LT48_OK)break;
  if(mode==1)st=block(s,0x43,read+offset,n,NULL);
  else{
   unsigned char reply[512]={0};st=block(s,0x48,(unsigned char*)image+offset,n,reply);
   if(st==LT48_OK&&!(reply[0]|reply[1])){
    unsigned char actual=0;lt48_status drain=block(s,0x43,&actual,1,NULL);
    out->mismatch_count++;out->first_mismatch=UINT32_MAX;st=drain==LT48_OK?LT48_DEVICE_MISMATCH:drain;
   }
  }
  if(st==LT48_OK){offset+=n;out->completed_bytes=offset;}
 }
 if(st==LT48_OK){const unsigned char stop[]={0,0};unsigned char reply[512]={0};st=cmd(s,stop,sizeof stop,reply);if(st==LT48_OK&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;}
 if(st==LT48_OK)st=run(s,3);
 return st;
}
static lt48_status emtc18_compare(xp_session*s,uint32_t start,size_t length,const unsigned char*image,
 unsigned char*pending,unsigned blank,uint16_t vcc,lt48_xp_flash_result*out){
 lt48_status st=emtc18_cycle(s,1,start,length,NULL,pending,vcc,out);
 if(st==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=(blank?255:image[i])){if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;}
 return st==LT48_OK&&out->mismatch_count?LT48_DEVICE_MISMATCH:st;
}
static lt48_status emtc18_execute(const lt48_xp_flash_resources*r,const lt48_protocol_transport*api,void*ctx,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void*decision_ctx,lt48_xp_flash_result*out){
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_xp_flash_validate(r,op,start,length,image,size);if(st!=LT48_OK)return st;
 if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
 st=lt48_xp_flash_validate_verify_options(r,op,margin,margin_type);if(st!=LT48_OK)return st;
 unsigned char*pending=malloc(length),*preserved=NULL;if(!pending)return LT48_NO_MEMORY;
 xp_session s={0};s.r=r;s.api=api;s.context=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);
 s.config_count=0;
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 if(st!=LT48_OK){free(pending);return st;}
 st=setup(&s);if(st==LT48_OK)st=identify(&s,decide,decision_ctx);
 if(st==LT48_OK&&r->wave[6]&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE)){
  for(size_t sector=0;st==LT48_OK&&sector<r->closure->count;sector++){
   uint32_t address=r->closure->sectors[sector];
   for(unsigned k=0;k<4;k++)s.buffers[0][k]=(unsigned char)(address>>(k*8));
   st=run(&s,6);if(st==LT48_OK&&s.buffers[2][0])st=LT48_SAFETY_REJECTED;
   if(st==LT48_OK)st=run(&s,3);
  }
 }
 if(st==LT48_OK&&op==LT48_OPERATION_READ)st=emtc18_cycle(&s,1,start,length,NULL,pending,r->nominal,out);
 else if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_SECURE)){
  if(op==LT48_OPERATION_SECURE){preserved=malloc(length);if(!preserved)st=LT48_NO_MEMORY;else st=emtc18_cycle(&s,1,start,length,NULL,preserved,r->nominal,out);}
  const unsigned char*desired=op==LT48_OPERATION_SECURE?preserved:image;
  if(st==LT48_OK)st=emtc18_cycle(&s,2,start,length,desired,NULL,r->nominal,out);
  for(unsigned i=0;st==LT48_OK&&i<2;i++)st=emtc18_compare(&s,start,length,desired,pending,0,margin_voltage(r,margin_type,i),out);
 }else if(st==LT48_OK&&op==LT48_OPERATION_ERASE){
  s.mode=7;
  for(size_t sector=0;st==LT48_OK&&sector<r->closure->count;sector++){
   uint32_t address=r->closure->sectors[sector];for(unsigned k=0;k<4;k++)s.buffers[0][k]=(unsigned char)(address>>(8*k));
   st=run(&s,2);if(st==LT48_OK)st=run(&s,10);if(st==LT48_OK&&!s.buffers[2][0])st=LT48_DEVICE_MISMATCH;if(st==LT48_OK)st=run(&s,3);
  }
  if(st==LT48_OK)st=emtc18_compare(&s,0,length,NULL,pending,1,r->nominal,out);
 }else if(st==LT48_OK){
  uint16_t volts[2]={r->nominal,r->nominal};unsigned count=1;
  if(op==LT48_OPERATION_VERIFY){if(margin==LT48_VERIFY_MARGIN_ALL){count=2;volts[0]=margin_voltage(r,margin_type,0);volts[1]=margin_voltage(r,margin_type,1);}else if(margin!=LT48_VERIFY_MARGIN_NOMINAL)volts[0]=margin_voltage(r,margin_type,margin==LT48_VERIFY_MARGIN_HIGH);}
  for(unsigned i=0;st==LT48_OK&&i<count;i++)st=emtc18_compare(&s,start,length,image,pending,op==LT48_OPERATION_BLANK_CHECK,volts[i],out);
 }
 out->operation_status=st;out->cleanup_status=sst_page_cleanup(&s,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(st==LT48_OK)st=out->cleanup_status;
 if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
 if(st==LT48_OK&&op!=LT48_OPERATION_READ){
  lt48_xp_flash_result fresh;st=emtc18_execute(r,api,ctx,LT48_OPERATION_READ,start,length,NULL,0,pending,length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_ctx,&fresh);
  out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
  if(st==LT48_OK){const unsigned char*expected=op==LT48_OPERATION_SECURE?preserved:image;
   for(size_t i=0;i<length;i++)if(pending[i]!=((op==LT48_OPERATION_ERASE||op==LT48_OPERATION_BLANK_CHECK)?255:expected[i])){if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;}
   if(out->mismatch_count)st=LT48_DEVICE_MISMATCH;
  }
  if(st!=LT48_OK){lt48_protocol_transport safe=*api;safe.cancelled=NULL;lt48_runtime_transport_session*w=NULL;lt48_status led=lt48_runtime_transport_session_create(&safe,ctx,&w);unsigned char error[]={0x40,2};if(led==LT48_OK)led=lt48_runtime_transport_session_controller_write(w,error,2);if(led==LT48_OK)led=lt48_runtime_transport_session_flush(w);lt48_runtime_transport_session_close(w);if(led!=LT48_OK){out->cleanup_status=led;out->cleanup_completed=0;}}
 }
 out->operation_status=st;free(preserved);free(pending);return st;
}
