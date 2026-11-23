/* Source-bound HC11 A/E/L owner. Original payloads remain bring-your-own.
 * Bytecode and source target kernels are shared; physical mapping/voltages and
 * CONFIG defaults are taken from the exact selected definition. */
#include "../../../c/src/hc11.h"
#include "../../../c/src/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "members.h"
#include "admission.h"
struct lt48_hc11_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[20];
 lt48_runtime_nested_wave nested[10];size_t nested_count;
 lt48_compile_variable vars[10];lt48_wave_code workers[4];lt48_applet_address addresses[4];
 const unsigned char *fpga;unsigned char *idle;size_t idle_size;
 unsigned stage,config_mask,config_value,capacity,worker_count,extension;
};
/* The source stop returns a byte even where its compact caller omits capture.
 * Consume it explicitly before another worker; never depend on FIFO overwrite. */
static lt48_status compile_wave(lt48_hc11_resources *r,const lt48_wave_code *w,lt48_compiled_wave **out){
 lt48_compact_wave *original=NULL,*normalized=NULL;lt48_status st=lt48_compact_decode(w->data,w->data_size,&original);
 unsigned char bytes[1024];size_t used=2;if(w->data_size>sizeof bytes/2){lt48_compact_wave_close(original);return LT48_FORMAT_ERROR;}memcpy(bytes,w->data,2);
 for(size_t i=0;st==LT48_OK&&i<lt48_compact_instruction_count(original);i++){
  lt48_compact_instruction ins;st=lt48_compact_get_instruction(original,i,&ins);if(st!=LT48_OK)break;
  size_t next=i+1<lt48_compact_instruction_count(original)?0:w->data_size;
  if(!next){lt48_compact_instruction following;st=lt48_compact_get_instruction(original,i+1,&following);if(st!=LT48_OK)break;next=following.offset;}
  size_t n=next-ins.offset;if(n>sizeof bytes-used){st=LT48_FORMAT_ERROR;break;}memcpy(bytes+used,w->data+ins.offset,n);used+=n;
  if(ins.opcode==0x39&&w->data[ins.offset+1]==0){
   int captured=next<w->data_size&&w->data[next]==0x7c;
   if(!captured){if(used+2>sizeof bytes){st=LT48_FORMAT_ERROR;break;}bytes[used++]=0x7c;bytes[used++]=75;}
  }
 }
 bytes[0]=(unsigned char)(used-2);bytes[1]=(unsigned char)((used-2)>>8);
 if(st==LT48_OK)st=lt48_compact_decode(bytes,used,&normalized);
 if(st==LT48_OK)st=lt48_compact_compile_with_pin_policy(normalized,r->pins,r->vars,10,1,LT48_PIN_FALLBACK_STRICT,out);
 lt48_compact_wave_close(original);lt48_compact_wave_close(normalized);return st;
}
void lt48_hc11_close(lt48_hc11_resources *r){if(!r)return;for(unsigned i=0;i<20;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);}
lt48_status lt48_hc11_open_candidate(const lt48_installation *x,size_t device,lt48_hc11_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);if(st!=LT48_OK)return st;
 const struct hc11_member *member=NULL;
 for(size_t i=0;i<sizeof hc11_members/sizeof *hc11_members;i++){const struct hc11_member *m=hc11_members+i;if(s.catalogue.vendor&&!strcmp(s.catalogue.vendor,m->vendor)&&s.catalogue.device_name_length==strlen(m->name)&&!memcmp(s.catalogue.device_name,m->name,s.catalogue.device_name_length)&&s.definition_resource==m->definition&&s.family_code==m->family&&s.pinmap==m->pinmap)member=m;}
 if(!member||s.catalogue.device_class!=2013||strcmp(s.definition_stem,"GADA___")||(s.capacity_bytes!=512&&s.capacity_bytes!=640&&s.capacity_bytes!=2048)||s.element_bits!=8||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 lt48_hc11_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->stage=member->stage;r->extension=member->stage>14;r->capacity=(unsigned)s.capacity_bytes;
 char path[4096],name[32];size_t index,family;
 int n=snprintf(path,sizeof path,"%s/GADA___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"aca05831121f0e30654601df133b298cec1d33e7ffb94626c5d62634e690c452",&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",s.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",s.family_code);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&family));TRY(lt48_rez_get_family_info(r->rez,family,&f));
 if(f.variable_count!=10||f.operation_count!=(r->extension&&s.definition_resource!=0xc4?14u:13u)){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<10;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,family,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned seen=0,worker=0;
 for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,family,i,&op));
  if(op.operation_id>=19||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;
  snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));
  if(w.wave_type==6){if(worker>=4||w.length1!=w.data_size+4||w.length2!=w.data_size||w.load_address<(r->extension?0x5100:0x5495)||(size_t)w.load_address+w.data_size>0x5856){st=LT48_FORMAT_ERROR;goto fail;}r->workers[worker]=w;r->addresses[worker++]=(lt48_applet_address){op.operation_id,w.load_address};}
  else if(w.wave_type==4&&op.operation_id==10){if(w.data_size!=2780||w.data[0]!=0xda||w.data[1]!=0x0a){st=LT48_FORMAT_ERROR;goto fail;}r->fpga=w.data+2;}
  else if(w.wave_type==0){st=compile_wave(r,&w,&r->wave[op.operation_id]);if(st!=LT48_OK)goto fail;if(r->nested_count>=10){st=LT48_FORMAT_ERROR;goto fail;}r->nested[r->nested_count++]=(lt48_runtime_nested_wave){op.operation_id,r->wave[op.operation_id]};}
  else{st=LT48_UNSUPPORTED;goto fail;}
 }
 if(worker!=(r->extension&&s.definition_resource!=0xc4?4u:3u)||r->nested_count!=9||!r->fpga||seen!=(r->extension&&s.definition_resource!=0xc4?0x6eef3u:0x6aef3u)){st=LT48_FORMAT_ERROR;goto fail;}
 r->worker_count=worker;
 if(r->extension){
  /* Source mode8 performs rail/pin setup without touching CONFIG or starting
   * another worker. Reuse its mode2 EEPROM program handshake, excluding the
   * CONFIG rewrite. Worker13 flag1 bulk-erases only EEPROM with BPROT=10. */
  static const unsigned char entry[]={0x17,0,0x41,0x29,4,0x2d,0x3d,0x0d,0x3e,0x29,0,0x11,0x0a,0,0x3b,1,0x3e,0x22,0,0x3e,0x22,1};
  unsigned char code[sizeof entry];memcpy(code,entry,sizeof code);code[0]=sizeof code-2;
  lt48_wave_code authored={0};authored.data=code;authored.data_size=sizeof code;
  TRY(compile_wave(r,&authored,&r->wave[19]));
 }
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_hc11_close(r);return st;
#undef TRY
}
lt48_status lt48_hc11_open(const lt48_installation *x,size_t d,lt48_hc11_resources **out){lt48_status st=lt48_hc11_open_candidate(x,d,out);if(st==LT48_OK&&(*out)->stage>LT48_HC11_STAGE){lt48_hc11_close(*out);*out=NULL;return LT48_UNSUPPORTED;}return st;}
lt48_status lt48_hc11_set_configuration(lt48_hc11_resources *r,unsigned mask,unsigned value){if(!r||mask>15||value>15)return LT48_INVALID_ARGUMENT;if(r->extension)return mask?LT48_UNSUPPORTED:LT48_OK;if((mask&8)&&!(value&8))return LT48_UNSUPPORTED;r->config_mask=mask;r->config_value=value;return LT48_OK;}
lt48_status lt48_hc11_validate(const lt48_hc11_resources *r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *data,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
 if(r->extension&&op==LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
 if(start||n!=r->capacity)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!data||size!=r->capacity))return LT48_INVALID_ARGUMENT;
 if((data&&size!=r->capacity)||(!data&&size))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
typedef struct hc11_session {
 const lt48_hc11_resources *r;lt48_runtime_transport_session *wire;
 const lt48_protocol_transport *api;void *context;
 unsigned char buffers[76][32],image[2048];lt48_compile_variable vars[10];
 lt48_runtime_configuration_value configuration[16];size_t configuration_count;
 uint32_t volts[4];unsigned mode,address;unsigned char saved_config;int restore_config,active;
} hc11_session;
static lt48_status run(void *context,unsigned op){
 hc11_session *s=context;if(op>=20||!s->r->wave[op])return LT48_UNSUPPORTED;
 if(op==4)s->active=s->mode!=8;
 if(op==9)s->active=1;
 s->buffers[75][0]=1;
 lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[74];size_t n=0;
 for(unsigned i=0;i<76;i++)if(i!=0&&i!=12)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=12;o.parameter_buffer_id=4;
 o.initial_buffers=seeds;o.initial_buffer_count=n;o.root_timeout_seconds=15;o.nested_waves=s->r->nested;o.nested_wave_count=s->r->nested_count;
 o.configuration=s->configuration;o.configuration_count=s->configuration_count;
 o.device_image=s->image;o.device_image_size=s->r->capacity;o.device_element_bytes=1;memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
 lt48_status st=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,10,&o,&machine);
 if(st==LT48_OK)st=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->worker_count,&result);
 if(st==LT48_OK){const unsigned char *p;size_t count;
  for(unsigned i=0;i<76&&st==LT48_OK;i++){st=lt48_runtime_get_buffer(result,i,&p,&count);if(st==LT48_OK&&count!=32)st=LT48_RUNTIME_ERROR;if(st==LT48_OK)memcpy(s->buffers[i],p,32);}
  if(st==LT48_OK&&s->buffers[75][0]!=1)st=LT48_PROTOCOL_ERROR;
  const lt48_runtime_configuration_value *cfg=NULL;if(st==LT48_OK)st=lt48_runtime_get_configuration(result,&cfg,&count);if(st==LT48_OK&&count>16)st=LT48_RUNTIME_ERROR;if(st==LT48_OK){memcpy(s->configuration,cfg,count*sizeof *cfg);s->configuration_count=count;}
  for(unsigned i=0;i<4&&st==LT48_OK;i++)st=lt48_runtime_get_voltage(result,i,&s->volts[i]);
  for(size_t i=0;i<lt48_runtime_event_count(result)&&st==LT48_OK;i++){lt48_runtime_event e;st=lt48_runtime_get_event(result,i,&e);if(st==LT48_OK&&(e.kind==LT48_RUNTIME_RESULT_LATCH||(e.kind==LT48_RUNTIME_RETURN_STATUS&&!e.arguments[0])))st=LT48_DEVICE_MISMATCH;}
 }
 if(st==LT48_OK&&(op==5||op==9||op==17))s->active=0;
 lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return st;
}
static lt48_status setup(hc11_session *s){lt48_status st=LT48_OK;for(unsigned i=0;i<s->r->worker_count&&st==LT48_OK;i++){const lt48_wave_code *w=s->r->workers+i;st=lt48_owner_load_applet_internal(s->wire,w->load_address,w->data,w->data_size);}if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);if(st==LT48_OK)st=run(s,0);return st;}
/* Source INIT reads CONFIG while powered and leaves a read worker active.
 * Capture that byte before RESET stops the worker and removes the rails. */
static lt48_status get_config(hc11_session *s,unsigned char *value){s->mode=1;s->address=0;lt48_status st=run(s,4);if(st==LT48_OK)*value=s->buffers[5][0];lt48_status stop=run(s,5);return st==LT48_OK?stop:st;}
static lt48_status put_config(hc11_session *s,unsigned char value){
 s->mode=8;s->address=0;lt48_status st=run(s,4);if(st!=LT48_OK)return st;
 memset(s->buffers[0x28],0,32);s->configuration_count=8;
 for(unsigned i=0;i<8;i++)s->configuration[i]=(lt48_runtime_configuration_value){0,0,i,(value>>i)&1};
 st=run(s,9);if(st==LT48_OK&&s->buffers[5][0]!=1)st=LT48_PROTOCOL_ERROR;
 lt48_status off=run(s,1);return st==LT48_OK?off:st;
}
/* Queue-reset08 is the existing Cypress bridge command used by source uploads.
 * Use cancellation-disabled raw I/O only during cleanup, before stopping the
 * worker; an unconsumed data reply must not be mistaken for terminal01. */
static lt48_status clear_reply(hc11_session *s){
 const unsigned char request[64]={0x2c,8,0,0};unsigned char reply[64];size_t n=0;
 lt48_status st=s->api->write(s->context,2,request,sizeof request,&n);
 if(st==LT48_OK&&n!=sizeof request)st=LT48_PROTOCOL_ERROR;
 if(st==LT48_OK)st=s->api->read(s->context,0x88,reply,sizeof reply,&n);
 if(st==LT48_OK&&(n<2||reply[0]!=1||reply[1]!=1))st=LT48_PROTOCOL_ERROR;
 return st;
}

static lt48_status cleanup(void *context,unsigned op){
 hc11_session *s=context;if(op!=100)return run(s,op);
 if(s->r->extension){
  if(!s->active)return LT48_OK;
  lt48_status st=clear_reply(s);const unsigned char stop=0;unsigned char terminal=0;
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,&stop,1);
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1);
  if(st==LT48_OK&&terminal!=1)st=LT48_PROTOCOL_ERROR;
  if(st==LT48_OK)s->active=0;
  return st;
 }
 if(!s->restore_config)return LT48_OK;
 lt48_status st=LT48_OK;
 if(s->active){st=clear_reply(s);const unsigned char stop=0;unsigned char terminal=0;if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,&stop,1);if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1);if(st==LT48_OK&&terminal!=1)st=LT48_PROTOCOL_ERROR;if(st==LT48_OK)s->active=0;}
 if(st==LT48_OK)st=put_config(s,s->saved_config);
 unsigned char actual=0;if(st==LT48_OK)st=get_config(s,&actual);if(st==LT48_OK&&actual!=s->saved_config)st=LT48_DEVICE_MISMATCH;
 if(st==LT48_OK)s->restore_config=0;
 return st;
}
static void voltage(hc11_session *s,unsigned v){for(unsigned i=0;i<10;i++)if(s->vars[i].variable_id==0||s->vars[i].variable_id==2)s->vars[i].value=v;}
static lt48_status read_cycle(hc11_session *s,unsigned v,unsigned char *out){voltage(s,v);s->mode=1;s->address=0;lt48_status st=run(s,4);for(unsigned i=0;st==LT48_OK&&i<s->r->capacity;i++){s->address=i;st=run(s,6);if(st==LT48_OK)out[i]=s->buffers[5][0];}lt48_status stop=run(s,5);return st==LT48_OK?stop:st;}
static lt48_status mutate(hc11_session *s,const unsigned char *data){
 if(s->r->extension){
  s->mode=8;s->address=0;lt48_status st=run(s,4);
  if(st==LT48_OK){for(unsigned i=0;i<10;i++)if(s->vars[i].variable_id==39){unsigned a=s->vars[i].value;memset(s->buffers[0x22],0,32);s->buffers[0x22][0]=a;s->buffers[0x22][1]=a>>8;}s->active=1;st=run(s,19);}
  for(unsigned i=0;st==LT48_OK&&i<s->r->capacity;i++){s->address=i;s->buffers[7][0]=data?data[i]:255;st=run(s,7);if(st==LT48_OK&&s->buffers[5][0]!=s->buffers[7][0])st=LT48_DEVICE_MISMATCH;}
  /* Source RESET mode8 stops the worker and removes power; it does not write
   * CONFIG. All nominal/margin readbacks below use fresh bootstrap sessions. */
  lt48_status stop=run(s,5);return st==LT48_OK?stop:st;
 }

 unsigned char config=0;lt48_status st=get_config(s,&config);if(st!=LT48_OK)return st;s->saved_config=config;s->restore_config=1;s->mode=2;s->address=0;
 st=run(s,4);for(unsigned i=0;st==LT48_OK&&i<s->r->capacity;i++){s->address=i;s->buffers[7][0]=data?data[i]:255;st=run(s,7);if(st==LT48_OK&&s->buffers[5][0]!=s->buffers[7][0])st=LT48_DEVICE_MISMATCH;}
 lt48_status stop=run(s,5);unsigned char actual=0;
 if(stop==LT48_OK)stop=get_config(s,&actual);
 if(stop==LT48_OK&&actual!=config)stop=LT48_DEVICE_MISMATCH;
 if(stop==LT48_OK)s->restore_config=0;
 return st==LT48_OK?stop:st;
}
static lt48_status compare(size_t capacity,const unsigned char *p,const unsigned char *wanted,lt48_small_eprom_result *out){out->mismatch_count=0;out->first_mismatch=UINT32_MAX;for(unsigned i=0;i<capacity;i++)if(p[i]!=(wanted?wanted[i]:255)){if(!out->mismatch_count)out->first_mismatch=i;out->mismatch_count++;}return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;}
lt48_status lt48_hc11_execute(const lt48_hc11_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *input,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_hc11_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);lt48_small_eprom_result *receipt=&out->operation;receipt->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_hc11_validate(r,op,start,length,input,size,margin,type);
 if(st==LT48_OK&&(!api||!api->write||!api->read))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<r->capacity))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){receipt->operation_status=st;return st;}
 hc11_session s={0};s.r=r;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof s.vars);if(r->extension&&r->capacity!=640){s.configuration_count=8;for(unsigned i=0;i<8;i++)s.configuration[i]=(lt48_runtime_configuration_value){0,0,i+8,1};}if(input)memcpy(s.image,input,r->capacity);else memset(s.image,255,r->capacity);unsigned char data[2048];
 st=lt48_runtime_transport_session_create(api,context,&s.wire);if(st==LT48_OK)st=setup(&s);
 if(st==LT48_OK&&op==LT48_OPERATION_CONFIGURATION){unsigned char old=0;st=get_config(&s,&old);unsigned char wanted=(old&~r->config_mask)|(r->config_value&r->config_mask);if(st==LT48_OK&&r->config_mask&&wanted!=old){s.saved_config=old;s.restore_config=1;st=put_config(&s,wanted);if(st==LT48_OK)st=get_config(&s,&old);if(st==LT48_OK&&old!=wanted)st=LT48_DEVICE_MISMATCH;if(st==LT48_OK)s.restore_config=0;}if(st==LT48_OK){out->configuration=old;out->configuration_valid=1;}}
 else {
  /* K CONFIG does not force EEON in bootstrap. Do not accept floating/disabled
   * EEPROM reads as blank, nor modify a disabled array. Its CONFIG editor is
   * outside this memory profile; preflight is read-only. */
  if(st==LT48_OK&&r->extension&&r->capacity==640){unsigned char config=0;st=get_config(&s,&config);if(st==LT48_OK&&!(config&1))st=LT48_SAFETY_REJECTED;}
  int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE;
  if(st==LT48_OK&&writing)st=mutate(&s,op==LT48_OPERATION_PROGRAM?input:NULL);
  unsigned delta=type?50:25;unsigned vv[]={500,500-delta,500+delta},count=writing||op==LT48_OPERATION_VERIFY?3:1;
  if(!writing&&op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_ALL){vv[0]=margin==LT48_VERIFY_MARGIN_LOW?500-delta:margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;count=1;}
  for(unsigned i=0;i<count&&st==LT48_OK;i++){st=read_cycle(&s,vv[i],data);if(st==LT48_OK&&op!=LT48_OPERATION_READ)st=compare(r->capacity,data,op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_ERASE?NULL:input,receipt);}
 }
 receipt->operation_status=st;const unsigned prefix[]={100,1};lt48_owner_finish_fpga_internal(api,context,&s.wire,cleanup,&s,prefix,2,r->idle,r->idle_size,st,&receipt->cleanup_status,&receipt->cleanup_completed);
 if(st==LT48_OK)st=receipt->cleanup_status;
 if(st==LT48_OK){receipt->completed_bytes=op==LT48_OPERATION_CONFIGURATION?1:r->capacity;if(op==LT48_OPERATION_READ)memcpy(read,data,r->capacity);}return st;
}

int lt48_hc11_is_extension(const lt48_hc11_resources *r){return r&&r->extension;}
