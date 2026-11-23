#include "regions.h"
/* Region streams retain original physical and logical address spaces.
 * Mutations require the complete image: no unselected region can be discarded
 * by the NT68F631B bank erase performed inside its Program callback. */
static void ntaux_address(s8253_session*s,unsigned region){
 memset(s->buffers[0],0,32);memset(s->buffers[1],0,32);
 unsigned logical=s->r->region_bank[region];
 for(unsigned i=0;i<4;i++)s->buffers[1][i]=(unsigned char)(logical>>(8*i));
}
static lt48_status ntaux_cycle(s8253_session*s,unsigned region,int writing,
 const unsigned char*image,unsigned char*actual,unsigned voltage){
 s->mode=writing?2:1;ntaux_address(s,region);
 lt48_status st=w78bank_voltage(s,voltage);
 if(!st)st=run_s8253(s,2);
 if(!st)st=run_s8253(s,writing?8:5);
 unsigned length=s->r->region_length[region];
 for(unsigned offset=0;!st&&offset<length;){
  unsigned chunk=length-offset;if(chunk>4096)chunk=4096;
  st=block_s8253(s,writing,writing?(unsigned char*)image+offset:actual+offset,chunk);offset+=chunk;
 }
 if(!st)st=stop_s8253(s);
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status ntaux_read_all(s8253_session*s,unsigned char*actual,unsigned voltage){
 unsigned offset=0;lt48_status st=LT48_OK;
 for(unsigned region=0;!st&&region<s->r->regions;region++){
  st=ntaux_cycle(s,region,0,NULL,actual+offset,voltage);offset+=s->r->region_length[region];
 }
 return st;
}
static lt48_status execute_ntaux(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,
 lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*image,size_t size,unsigned char*out,size_t outsize,
 lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_novatek25_validate(r,op,a,n,image,size);
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==1&&(!out||outsize<n))st=LT48_BUFFER_TOO_SMALL;
 if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}
 st=setup_s8253(&s);if(!st)s.ready=1;int writing=op==3||op==4;
 if(!st&&op==7){
  s.mode=7;ntaux_address(&s,0);st=run_s8253(&s,2);
  if(!st)st=run_s8253(&s,10);
  lt48_status reset=run_s8253(&s,3);if(!st)st=reset;
  if(!st)st=ntaux_read_all(&s,actual,r->nominal);
  if(!st)st=w78bank_compare(z,0,actual,NULL,r->capacity);
 }else if(!st){
  if(writing){
   st=ntaux_read_all(&s,actual,r->nominal);
   for(size_t i=0;!st&&i<n;i++){
    int mismatch=op==3?(r->definition!=508&&actual[i]!=255):((actual[i]&image[i])!=image[i]);
    if(mismatch){z->first_mismatch=(uint32_t)i;z->mismatch_count=1;st=LT48_DEVICE_MISMATCH;}
   }
   unsigned offset=0;
   for(unsigned region=0;!st&&region<r->regions;region++){
    st=ntaux_cycle(&s,region,1,image+offset,NULL,r->nominal);offset+=r->region_length[region];
   }
  }
  uint32_t volts[4]={0};unsigned count=0;
  if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;
  else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned pass=0;!st&&pass<passes;pass++){
   st=ntaux_read_all(&s,actual,volts[pass]);
   if(!st&&op!=1)st=w78bank_compare(z,a,actual+a,op==2?NULL:image,n);
  }
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;
 const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&(writing||op==7)){
  lt48_serial_page_result fresh;
  st=execute_ntaux(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,NULL,NULL,&fresh);
  if(!st)st=w78bank_compare(z,0,actual,writing?image:NULL,r->capacity);
  if(st){
   z->operation_status=st;lt48_runtime_transport_session*wire=NULL;lt48_status cleanup;int done;
   lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cleanup,&done);
   if(!done){z->cleanup_status=cleanup;z->cleanup_completed=0;}
  }
 }
 if(!st){if(op==1)memcpy(out,actual+a,n);z->completed_bytes=n;}
 free(actual);return st;
}
