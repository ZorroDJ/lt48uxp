/* P89V/LV51 owner, using the existing AddTab/8051 transport and runtime.
 * Source image: code 0..MaxFlash, FF gap to 0x10000, ISP block at 0x10000.
 * All source compact handlers execute intact. No implicit chip erase. */
static void p89_address(s8253_session*s,unsigned image){
 unsigned physical=image<65536?(image<s->r->flash?image:image-s->r->flash):image-65536+0xe000;
 for(unsigned i=0;i<4;i++){s->buffers[0][i]=(unsigned char)(physical>>(8*i));
 s->buffers[1][i]=(unsigned char)(image>>(8*i));
 }
}
static void p89_config(s8253_session*s,const unsigned char*v){s->config_count=2;
 for(unsigned i=0;i<2;i++)s->config[i]=(lt48_runtime_configuration_value){-1,0,i,v?v[i]:UINT32_MAX};
 }
static lt48_status p89_settings(s8253_session*s,unsigned char*out){
 s->mode=13;
 p89_address(s,0);
 lt48_status st=run_s8253(s,2);
 p89_config(s,NULL);
 if(!st)st=run_s8253(s,6);
 for(unsigned i=0;!st&&i<2;i++){if(s->config[i].value>1)st=LT48_DEVICE_MISMATCH;
 else out[i]=(unsigned char)s->config[i].value;
 }
 if(!st)st=run_s8253(s,3);
 return st;
}
#include "../pic-common/source_verify_voltage.h"
/* Same DAC byte as the source compact SET_VCC instruction. The host's
 * verification-voltage selector supplies the value, not the INIT callback. */
static lt48_status p89_voltage(s8253_session *s,unsigned cv){
    const unsigned char bytes[]={0x10,0,(unsigned char)(20u*cv/100u)};
    lt48_status st=lt48_runtime_transport_session_controller_write(s->wire,bytes,sizeof bytes);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK){s->vars[0].value=cv;s->volts[0]=cv;}
    return st;
}
static lt48_status p89_cycle(s8253_session*s,int writing,unsigned start,size_t n,const unsigned char*image,unsigned char*out,unsigned volts){
 s->vars[0].value=volts;
 s->mode=writing?2:1;
 size_t done=0;
 lt48_status st=p89_voltage(s,volts);
 while(!st&&done<n){unsigned a=start+(unsigned)done;
 unsigned end=a<s->r->flash?s->r->flash:a<65536?65536:73728;
 size_t take=n-done;
 if(take>end-a)take=end-a;
  p89_address(s,a);
 st=run_s8253(s,2);
 if(!st)st=run_s8253(s,writing?8:5);
  for(size_t offset=0;!st&&offset<take;){size_t chunk=take-offset;
 if(chunk>4096)chunk=4096;
 st=block_s8253(s,writing,writing?(unsigned char*)image+done+offset:out+done+offset,chunk);
 offset+=chunk;
 }
  if(!st)st=stop_s8253(s);
 if(!st)st=run_s8253(s,3);
 done+=take;
 }
 if(!st)st=p89_voltage(s,s->r->nominal);
 return st;
}
static lt48_status p89_apply(s8253_session*s,const unsigned char*saved,const unsigned char*want){
 /* Both source fields are one-way programming commands. Do not invent an
  * inverse command when the UI asks to clear a programmed bit. */
 if(want[0]<saved[0]||want[1]<saved[1]||(saved[0]&&want[1]!=saved[1]))return LT48_SAFETY_REJECTED;
 unsigned char step[2]={0,want[1]},actual[2];
 lt48_status st=LT48_OK;
 if(want[1]!=saved[1]){s->mode=13;
 p89_address(s,0);
 st=run_s8253(s,2);
 p89_config(s,step);
 if(!st)st=run_s8253(s,11);
 if(!st&&s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
 if(!st)st=run_s8253(s,3);
 if(!st)st=p89_settings(s,actual);
 if(!st&&(actual[0]!=0||actual[1]!=want[1]))st=LT48_DEVICE_MISMATCH;
 }
 if(!st&&want[0]!=saved[0]){step[0]=1;
 step[1]=0;
 s->mode=13;
 p89_address(s,0);
 st=run_s8253(s,2);
 p89_config(s,step);
 if(!st)st=run_s8253(s,11);
 if(!st&&s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
 if(!st)st=run_s8253(s,3);
 if(!st)st=p89_settings(s,actual);
 if(!st&&actual[0]!=1)st=LT48_DEVICE_MISMATCH;
 }
 return st;
}
static lt48_status cleanup_p89(void*v,unsigned phase){s8253_session*s=v;
 if(phase==0)return stop_s8253(s);
 if(phase==1)return s->ready?run_s8253(s,3):LT48_OK;
 if(phase==2)return run_s8253(s,1);
 return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
 }
static lt48_status execute_p89v(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*decision_ctx,lt48_serial_page_result*out){
 if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);
 out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_p89v_validate(r,op,start,n,image,size);
 if(!st&&(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
 if(!st&&((op==LT48_OPERATION_READ&&(!read||read_size<n))||(op==LT48_OPERATION_CONFIGURATION&&(!read||read_size<2))))st=LT48_BUFFER_TOO_SMALL;
 if(st){out->operation_status=st;
 return st;
 }
 if(api->cancelled&&api->cancelled(ctx)){out->operation_status=LT48_CANCELLED;
 out->cleanup_completed=1;
 return LT48_CANCELLED;
 }
 unsigned char*actual=malloc(73728),saved[2]={0},want[2]={0};
 if(!actual)return LT48_NO_MEMORY;
 s8253_session s={0};
 s.r=r;
 s.api=api;
 s.ctx=ctx;
 s.mode=13;
 memcpy(s.vars,r->vars,sizeof s.vars);
 lt48_protocol_transport clean=*api;
 clean.cancelled=NULL;
 unsigned started=0,uncertain=0;
 st=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
 if(!st){started=1;
 s.api=&clean;
 st=setup_s8253(&s);
 if(!st)st=lt48_runtime_transport_session_flush(s.wire);
 uncertain=st!=LT48_OK;
 s.ready=!st;
 lt48_runtime_transport_session_close(s.wire);
 s.wire=NULL;
 s.api=api;
 }
 if(!st)st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 for(unsigned attempt=0;!st&&attempt<64;attempt++){
  p89_address(&s,0);
 st=run_s8253(&s,2);
 if(!st)st=run_s8253(&s,4);
 if(st)break;
  unsigned man=s.buffers[7][0],dev=s.buffers[5][0];
 if(man==r->manufacturer&&dev==r->device)break;
  lt48_m37_id_choice choice=decide?decide(decision_ctx,r->manufacturer,r->device,man,dev):LT48_M37_ID_ABORT;
  if(choice==LT48_M37_ID_IGNORE)break;
 if(choice!=LT48_M37_ID_RETRY||attempt==63)st=LT48_DEVICE_MISMATCH;
 }
 if(!st)st=p89_settings(&s,saved);
 int programming=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 if(!st&&op==LT48_OPERATION_ERASE){s.mode=7;
 p89_address(&s,0);
 st=run_s8253(&s,2);
 if(!st)st=run_s8253(&s,10);
 /* Source status is FF, not a chip readback. */if(!st&&s.buffers[3][0]!=255)st=LT48_DEVICE_MISMATCH;
 if(!st)st=run_s8253(&s,3);
 if(!st)st=p89_settings(&s,want);
 if(!st&&want[0])st=LT48_DEVICE_MISMATCH;
 if(!st)st=p89_cycle(&s,0,0,73728,NULL,actual,r->nominal);
 if(!st)st=mismatch_s8253(out,0,actual,NULL,73728,1);
 }
 else if(!st&&(op==LT48_OPERATION_CONFIGURATION||op==LT48_OPERATION_SECURE)){
  memcpy(want,saved,2);
 if(op==LT48_OPERATION_SECURE)want[0]=1;
 else if(image)for(unsigned i=0;i<2;i++)if(image[0]&(1u<<i))want[i]=image[i+1];
  st=p89_apply(&s,saved,want);
 if(!st&&saved[0])want[1]=saved[1];
 }
 else if(!st){
  if(saved[0])st=LT48_SAFETY_REJECTED;
 /* Never publish all-FF protected reads as code. */
  if(!st&&programming){st=p89_cycle(&s,0,0,73728,NULL,actual,r->nominal);
 for(unsigned i=0;!st&&i<73728;i++)if(op==LT48_OPERATION_PROGRAM?actual[i]!=255:(actual[i]&image[i])!=image[i]){out->first_mismatch=i;
 out->mismatch_count=1;
 st=LT48_DEVICE_MISMATCH;
 }if(!st)st=p89_cycle(&s,1,0,73728,image,NULL,r->nominal);
 }
  uint32_t volts[4]={0};unsigned count=0;
  if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
  unsigned passes=programming||(op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!programming&&(op!=LT48_OPERATION_VERIFY||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;
 else if(!programming&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
  for(unsigned i=0;!st&&i<passes;i++){st=p89_cycle(&s,0,start,n,NULL,actual,volts[i]);
 if(!st&&op!=LT48_OPERATION_READ)st=mismatch_s8253(out,start,actual,image,n,op==LT48_OPERATION_BLANK_CHECK);
 }
  if(!st)st=p89_settings(&s,want);
 if(!st&&memcmp(saved,want,2))st=LT48_DEVICE_MISMATCH;
 }
 out->operation_status=st;
 if(started){s.api=&clean;
 const unsigned phases[]={0,1,2,3};
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_p89,&s,phases,4,st,&out->cleanup_status,&out->cleanup_completed);
 if(uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;
 out->cleanup_completed=0;
 }}
 else{lt48_runtime_transport_session_close(s.wire);
 out->cleanup_completed=1;
 }
 if(!st){if(op==LT48_OPERATION_READ)memcpy(read,actual,n);
 if(op==LT48_OPERATION_CONFIGURATION){memcpy(read,want,2);if(want[0])read[1]=255;}
 out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?2:n;
 }
 free(actual);
 return st;
}
