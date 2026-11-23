/* Source-bound HC711 K/KA/E/S memory owner. Original payloads remain BYO.
 * The shared SCI envelope uses authored timed EPROM RAM kernels; exact source
 * pin/supply profiles and nonvolatile CONFIG preservation are explicit below. */
#include "../../../c/src/hc711.h"
#include "../../../c/src/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "members.h"
#include "admission.h"
struct lt48_hc711_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[23];
 lt48_runtime_nested_wave nested[10];size_t nested_count;
 lt48_compile_variable vars[12];lt48_wave_code workers[4];lt48_applet_address addresses[4];
 const unsigned char *fpga;unsigned char *idle;size_t idle_size;
 unsigned stage,capacity,worker_count,var_count,ka2,e,e20; unsigned char *patched[4]; unsigned char kernels[512]; size_t kernels_size;
};
/* The source stop returns a byte even where its compact caller omits capture.
 * Consume it explicitly before another worker; never depend on FIFO overwrite. */
static lt48_status compile_wave(lt48_hc711_resources *r,const lt48_wave_code *w,lt48_compiled_wave **out){
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
 if(st==LT48_OK)st=lt48_compact_compile_with_pin_policy(normalized,r->pins,r->vars,r->var_count,1,LT48_PIN_FALLBACK_STRICT,out);
 lt48_compact_wave_close(original);lt48_compact_wave_close(normalized);
 return st;
}
void lt48_hc711_close(lt48_hc711_resources *r){if(!r)return;for(unsigned i=0;i<23;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);for(unsigned i=0;i<4;i++)free(r->patched[i]);free(r->idle);free(r);}
static lt48_status prepare_kernels(lt48_hc711_resources *r);
lt48_status lt48_hc711_open_candidate(const lt48_installation *x,size_t device,lt48_hc711_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);if(st!=LT48_OK)return st;
 const struct hc711_member *member=NULL;
 for(size_t i=0;i<sizeof hc711_members/sizeof *hc711_members;i++){const struct hc711_member *m=hc711_members+i;if(s.catalogue.vendor&&!strcmp(s.catalogue.vendor,m->vendor)&&s.catalogue.device_name_length==strlen(m->name)&&!memcmp(s.catalogue.device_name,m->name,s.catalogue.device_name_length)&&s.definition_resource==m->definition&&s.family_code==m->family&&s.pinmap==m->pinmap)member=m;}
 if(!member||s.catalogue.device_class!=2013||strcmp(s.definition_stem,"GADA___")||(s.capacity_bytes!=32768&&s.capacity_bytes!=65536)||s.element_bits!=8||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 lt48_hc711_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->stage=member->stage;r->capacity=(unsigned)s.capacity_bytes;r->ka2=s.definition_resource==0xc7;r->e=s.definition_resource>=0xcb;r->e20=s.definition_resource==0xcb;
 char path[4096],name[32];size_t index,family;
 int n=snprintf(path,sizeof path,"%s/GADA___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"aca05831121f0e30654601df133b298cec1d33e7ffb94626c5d62634e690c452",&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",s.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",s.family_code);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&family));TRY(lt48_rez_get_family_info(r->rez,family,&f));
 if(f.variable_count!=(r->ka2?8u:r->e&&!r->e20?11u:12u)||f.operation_count!=(r->ka2?10u:13u)){st=LT48_FORMAT_ERROR;goto fail;}
 r->var_count=(unsigned)f.variable_count;
 for(unsigned i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,family,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned seen=0,worker=0;
 for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,family,i,&op));
  if(op.operation_id>=19||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;
  snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));
  if(w.wave_type==6){if(worker>=4||w.length1!=w.data_size+4||w.length2!=w.data_size||w.load_address<0x5100||(size_t)w.load_address+w.data_size>(r->e?0x552du:0x5487u)){st=LT48_FORMAT_ERROR;goto fail;}r->workers[worker]=w;r->addresses[worker++]=(lt48_applet_address){op.operation_id,w.load_address};}
  else if(w.wave_type==4&&op.operation_id==10){if(w.data_size!=2780||w.data[0]!=0xda||w.data[1]!=0x0a){st=LT48_FORMAT_ERROR;goto fail;}r->fpga=w.data+2;}
  else if(w.wave_type==0){st=compile_wave(r,&w,&r->wave[op.operation_id]);if(st!=LT48_OK)goto fail;if(r->nested_count>=10){st=LT48_FORMAT_ERROR;goto fail;}r->nested[r->nested_count++]=(lt48_runtime_nested_wave){op.operation_id,r->wave[op.operation_id]};}
  else{st=LT48_UNSUPPORTED;goto fail;}
 }
 if(worker!=3||r->nested_count!=(r->ka2?6u:9u)||!r->fpga||seen!=(r->ka2?0xacf3u:0x6aef3u)){st=LT48_FORMAT_ERROR;goto fail;}
 r->worker_count=worker;
 if(r->e){
  /* Selected E source has a GPIO acceleration. Use the already audited E9
   * @52 SCI image W150 and the original common K loader envelope instead.
   * Both are in this exact SHA-guarded GADA library; P4d is identical. */
  const unsigned waves[]={0x165,0x166,0x167},ops[]={15,13,11};
  for(unsigned i=0;i<3;i++){lt48_wave_code w;snprintf(name,sizeof name,"W%x",waves[i]);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));if(w.wave_type!=6){st=LT48_FORMAT_ERROR;goto fail;}r->workers[i]=w;r->addresses[i]=(lt48_applet_address){ops[i],w.load_address};}
  lt48_wave_code w;TRY(lt48_rez_find_resource(r->rez,"W150",&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));if(w.wave_type!=4||w.data_size!=2780){st=LT48_FORMAT_ERROR;goto fail;}r->fpga=w.data+2;
 }

 TRY(prepare_kernels(r));
 /* Exact source rail-entry prefix, ending before source CONFIG mutation or
  * implicit read/program startup. Subsequent workers are selected explicitly. */
 for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;TRY(lt48_rez_get_family_operation(r->rez,family,i,&op));if(op.operation_id!=4)continue;
  lt48_wave_code w;snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));
  if(w.data_size<(r->e?0x25u:0x2bu)){st=LT48_FORMAT_ERROR;goto fail;}unsigned char bytes[0x2b];size_t n=r->e?0x25u:sizeof bytes;memcpy(bytes,w.data,n);bytes[0]=n-2;bytes[1]=0;
  lt48_wave_code a={0};a.data=bytes;a.data_size=n;lt48_compiled_wave_close(r->wave[4]);r->wave[4]=NULL;TRY(compile_wave(r,&a,&r->wave[4]));
 }
 static const unsigned char entries[3][24]={
  {0,0,0x11,0x24,1,0x41,0x29,4,0x2d,0x3d,0x0f,0x3e,0x29,0,0x11,0x0a,0,0x3b,1,0x3e,0x22,0,0x3e,0x22},
  {0,0,0x11,0x24,1,0x41,0x29,4,0x2d,0x3d,0x0d,0x3e,0x29,0,0x11,0x0a,0,0x3b,1,0x3e,0x22,0,0x3e,0x22},
  {0,0,0x11,0x24,1,0x41,0x29,4,0x2d,0x3d,0x0d,0x3e,0x29,0,0x11,0x0a,0,0x3b,0,0x3e,0x22,0,0x3e,0x22}};
 for(unsigned i=0;i<3;i++){unsigned char bytes[40];memcpy(bytes,entries[i],24);bytes[24]=1;size_t n=25;if(i==2){const unsigned char delay[]={0x3e,0x27,0,0x3e,0x27,1};memcpy(bytes+n,delay,6);n+=6;}if(i==2){memmove(bytes+6,bytes+2,n-2);bytes[2]=0;bytes[3]=5;bytes[4]=0x31;bytes[5]=0x14;n+=4;}bytes[0]=n-2;bytes[1]=0;lt48_wave_code a={0};a.data=bytes;a.data_size=n;TRY(compile_wave(r,&a,&r->wave[19+i]));}
 {const unsigned char bytes[]={5,0,0x3e,7,0,0x7c,5};lt48_wave_code a={0};a.data=bytes;a.data_size=sizeof bytes;TRY(compile_wave(r,&a,&r->wave[22]));}
 {const unsigned char bytes[]={5,0,0x11,5,1,0x7c,5};lt48_wave_code a={0};a.data=bytes;a.data_size=sizeof bytes;lt48_compiled_wave_close(r->wave[6]);r->wave[6]=NULL;TRY(compile_wave(r,&a,&r->wave[6]));}
 for(size_t j=0;j<r->nested_count;j++)r->nested[j].compiled=r->wave[r->nested[j].operation];
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_hc711_close(r);
 return st;
#undef TRY
}
lt48_status lt48_hc711_open(const lt48_installation *x,size_t d,lt48_hc711_resources **out){lt48_status st=lt48_hc711_open_candidate(x,d,out);if(st==LT48_OK&&(*out)->stage>LT48_HC711_STAGE){lt48_hc711_close(*out);*out=NULL;return LT48_UNSUPPORTED;}return st;}
static int eprom_address(const lt48_hc711_resources *r,unsigned a){return r->ka2?a<32768:r->e?(a>=0xd000||(r->e20&&a>=0x9000&&a<0xb000)):a>=0xa000;}
static int eeprom_address(const lt48_hc711_resources *r,unsigned a){return !r->ka2&&(r->e?(a>=0xb600&&a<0xb800):(a>=0xd80&&a<0x1000));}
lt48_status lt48_hc711_validate(const lt48_hc711_resources *r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *data,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(start||n!=r->capacity)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!data||size!=r->capacity))return LT48_INVALID_ARGUMENT;
 if((data&&size!=r->capacity)||(!data&&size))return LT48_INVALID_ARGUMENT;
 if(data)for(unsigned i=0;i<r->capacity;i++){if(!eprom_address(r,i)&&!eeprom_address(r,i)&&data[i]!=255)return LT48_SAFETY_REJECTED;}
 return LT48_OK;
}
typedef struct hc711_session {
 const lt48_hc711_resources *r;lt48_runtime_transport_session *wire;
 const lt48_protocol_transport *api;void *context;
 unsigned char buffers[76][32],image[65536];lt48_compile_variable vars[12];
 lt48_runtime_configuration_value configuration[16];size_t configuration_count;
 uint32_t volts[4];unsigned mode,address;unsigned char saved_config;int restore_config,active;
} hc711_session;
static lt48_status run(void *context,unsigned op){
 hc711_session *s=context;if(op>=23||!s->r->wave[op])return LT48_UNSUPPORTED;
 if(op>=19)s->active=1;
 
 s->buffers[75][0]=1;
 lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[74];size_t n=0;
 for(unsigned i=0;i<76;i++)if(i!=0&&i!=12)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=12;o.parameter_buffer_id=4;
 o.initial_buffers=seeds;o.initial_buffer_count=n;o.root_timeout_seconds=15;o.nested_waves=s->r->nested;o.nested_wave_count=s->r->nested_count;
 o.configuration=s->configuration;o.configuration_count=s->configuration_count;
 o.device_image=s->image;o.device_image_size=s->r->capacity;o.device_element_bytes=1;memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
 lt48_status st=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&o,&machine);
 if(st==LT48_OK)st=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->worker_count,&result);
 if(st==LT48_OK){const unsigned char *p;size_t count;
  for(unsigned i=0;i<76&&st==LT48_OK;i++){st=lt48_runtime_get_buffer(result,i,&p,&count);if(st==LT48_OK&&count!=32)st=LT48_RUNTIME_ERROR;if(st==LT48_OK)memcpy(s->buffers[i],p,32);}
  if(st==LT48_OK&&s->buffers[75][0]!=1)st=LT48_PROTOCOL_ERROR;
  const lt48_runtime_configuration_value *cfg=NULL;if(st==LT48_OK)st=lt48_runtime_get_configuration(result,&cfg,&count);if(st==LT48_OK&&count>16)st=LT48_RUNTIME_ERROR;if(st==LT48_OK){memcpy(s->configuration,cfg,count*sizeof *cfg);s->configuration_count=count;}
  for(unsigned i=0;i<4&&st==LT48_OK;i++)st=lt48_runtime_get_voltage(result,i,&s->volts[i]);
  for(size_t i=0;i<lt48_runtime_event_count(result)&&st==LT48_OK;i++){lt48_runtime_event e;st=lt48_runtime_get_event(result,i,&e);if(st==LT48_OK&&(e.kind==LT48_RUNTIME_RESULT_LATCH||(e.kind==LT48_RUNTIME_RETURN_STATUS&&!e.arguments[0])))st=LT48_DEVICE_MISMATCH;}
 }
 if(st==LT48_OK&&(op==5||op==9||op==17))s->active=0;
 lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);
 return st;
}
static lt48_status setup(hc711_session *s){lt48_status st=LT48_OK;for(unsigned i=0;i<s->r->worker_count&&st==LT48_OK;i++){const lt48_wave_code *w=s->r->workers+i;st=lt48_owner_load_applet_internal(s->wire,w->load_address,w->data,w->data_size);}if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,0x6000,s->r->kernels,s->r->kernels_size);if(st==LT48_OK)st=run(s,0);
 return st;}
/* Queue-reset08 is the existing Cypress bridge command used by source uploads.
 * Use cancellation-disabled raw I/O only during cleanup, before stopping the
 * worker; an unconsumed data reply must not be mistaken for terminal01. */
static lt48_status clear_reply(hc711_session *s){
 const unsigned char request[64]={0x2c,8,0,0};unsigned char reply[64];size_t n=0;
 lt48_status st=s->api->write(s->context,2,request,sizeof request,&n);
 if(st==LT48_OK&&n!=sizeof request)st=LT48_PROTOCOL_ERROR;
 if(st==LT48_OK)st=s->api->read(s->context,0x88,reply,sizeof reply,&n);
 if(st==LT48_OK&&(n<2||reply[0]!=1||reply[1]!=1))st=LT48_PROTOCOL_ERROR;
 return st;
}

static lt48_status cleanup(void *context,unsigned op){
 hc711_session *s=context;if(op!=100)return run(s,op);if(!s->active)return LT48_OK;
 lt48_status st=clear_reply(s);const unsigned char stop=0;unsigned char terminal=0;
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,&stop,1);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1);
 if(st==LT48_OK&&terminal!=1)st=LT48_PROTOCOL_ERROR;
 if(st==LT48_OK)s->active=0;
 return st;
}
static void voltage(hc711_session *s,unsigned v){for(unsigned i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0||s->vars[i].variable_id==2)s->vars[i].value=v;}
static void address(hc711_session *s,unsigned a){memset(s->buffers[0x22],0,32);s->buffers[0x22][0]=a;s->buffers[0x22][1]=a>>8;}
static lt48_status transfer(hc711_session *s,unsigned a,unsigned n,unsigned offset,unsigned kind,const unsigned char *input,unsigned char *output){
 s->mode=8;s->address=0;lt48_status st=run(s,4);address(s,a);

 if(st==LT48_OK)st=run(s,19+kind);
 for(unsigned i=0;i<n&&st==LT48_OK;i++){
  s->address=offset+i;
  if(kind){s->buffers[7][0]=input[offset+i];st=run(s,22);if(st==LT48_OK&&s->buffers[5][0]!=input[offset+i])st=LT48_DEVICE_MISMATCH;}
  else{st=run(s,6);if(st==LT48_OK)output[offset+i]=s->buffers[5][0];}
 }
 lt48_status stop=run(s,5);return st==LT48_OK?stop:st;
}
static lt48_status read_cycle(hc711_session *s,unsigned v,unsigned char *out){voltage(s,v);memset(out,255,s->r->capacity);lt48_status st=LT48_OK;
 if(s->r->e){st=transfer(s,0xb600,512,0xb600,0,NULL,out);if(st==LT48_OK&&s->r->e20)st=transfer(s,0x9000,8192,0x9000,0,NULL,out);if(st==LT48_OK)st=transfer(s,0xd000,12288,0xd000,0,NULL,out);return st;}
 if(!s->r->ka2)st=transfer(s,0xd80,640,0xd80,0,NULL,out);
 if(st==LT48_OK)st=transfer(s,0x2000,s->r->ka2?32768:24576,s->r->ka2?0:0xa000,0,NULL,out);
 return st;
}
static lt48_status mutate(hc711_session *s,const unsigned char *data){
 lt48_status st=LT48_OK;if(s->r->e){st=transfer(s,0xb600,512,0xb600,1,data,NULL);if(st==LT48_OK&&s->r->e20)st=transfer(s,0x9000,8192,0x9000,2,data,NULL);if(st==LT48_OK)st=transfer(s,0xd000,12288,0xd000,2,data,NULL);return st;}
 if(!s->r->ka2)st=transfer(s,0xd80,640,0xd80,1,data,NULL);
 if(st==LT48_OK)st=transfer(s,0x2000,s->r->ka2?32768:24576,s->r->ka2?0:0xa000,2,data,NULL);
 return st;
}
static lt48_status compare(size_t capacity,const unsigned char *p,const unsigned char *wanted,lt48_small_eprom_result *out){out->mismatch_count=0;out->first_mismatch=UINT32_MAX;for(unsigned i=0;i<capacity;i++)if(p[i]!=(wanted?wanted[i]:255)){if(!out->mismatch_count)out->first_mismatch=i;out->mismatch_count++;}return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;}
lt48_status lt48_hc711_execute(const lt48_hc711_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *input,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *receipt){
 if(!receipt)return LT48_INVALID_ARGUMENT;
 memset(receipt,0,sizeof *receipt);receipt->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_hc711_validate(r,op,start,length,input,size,margin,type);
 if(st==LT48_OK&&(!api||!api->write||!api->read))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<r->capacity))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){receipt->operation_status=st;
 return st;}
 hc711_session s={0};s.r=r;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof s.vars);if(input)memcpy(s.image,input,r->capacity);else memset(s.image,255,r->capacity);unsigned char data[65536];
 st=lt48_runtime_transport_session_create(api,context,&s.wire);if(st==LT48_OK)st=setup(&s);
 int writing=op==LT48_OPERATION_PROGRAM;
 if(st==LT48_OK&&r->e){unsigned char cfg=0;st=transfer(&s,0x103f,1,0,0,NULL,&cfg);if(st==LT48_OK&&(cfg&3)!=3)st=LT48_SAFETY_REJECTED;}
 if(st==LT48_OK&&writing){/* EPROM is UV/OTP: refuse nonblank EPROM before EEPROM autoerase. */st=read_cycle(&s,500,data);if(st==LT48_OK)for(unsigned i=0;i<r->capacity;i++)if(eprom_address(r,i)&&data[i]!=255){st=LT48_SAFETY_REJECTED;break;}if(st==LT48_OK)st=mutate(&s,input);}
 unsigned delta=type?50:25;unsigned vv[]={500,500-delta,500+delta},count=writing||op==LT48_OPERATION_VERIFY?3:1;
 if(!writing&&op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_ALL){vv[0]=margin==LT48_VERIFY_MARGIN_LOW?500-delta:margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;count=1;}
 for(unsigned i=0;i<count&&st==LT48_OK;i++){st=read_cycle(&s,vv[i],data);if(st==LT48_OK&&op!=LT48_OPERATION_READ)st=compare(r->capacity,data,op==LT48_OPERATION_BLANK_CHECK?NULL:input,receipt);}
 receipt->operation_status=st;const unsigned prefix[]={100,1};lt48_owner_finish_fpga_internal(api,context,&s.wire,cleanup,&s,prefix,2,r->idle,r->idle_size,st,&receipt->cleanup_status,&receipt->cleanup_completed);
 if(st==LT48_OK)st=receipt->cleanup_status;
 if(st==LT48_OK){receipt->completed_bytes=r->capacity;if(op==LT48_OPERATION_READ)memcpy(read,data,r->capacity);}return st;
}

/* The exact GADA SHA above guards these offsets. Redirect only kernel data and
 * its loader relocation fields; retain all original controller serial loops.
 * This avoids the opaque resident ROM helper and CONFIG EEPROM programming. */
static void put16(unsigned char *p,unsigned v){p[0]=v>>8;p[1]=v;}
static lt48_status prepare_kernels(lt48_hc711_resources *r){
 unsigned char *read=NULL,*program=NULL;const unsigned char *init=NULL;
 for(unsigned i=0;i<r->worker_count;i++){
  r->patched[i]=malloc(r->workers[i].data_size);if(!r->patched[i])return LT48_NO_MEMORY;
  memcpy(r->patched[i],r->workers[i].data,r->workers[i].data_size);r->workers[i].data=r->patched[i];
  if(r->addresses[i].wave==15)read=r->patched[i];
  if(r->addresses[i].wave==13)program=r->patched[i];
  if(r->addresses[i].wave==11)init=r->patched[i];
 }
 if(!read||!program||!init)return LT48_FORMAT_ERROR;
 if(memcmp(read+0x47,"\x7c\x53\x7d\x9f\x7e\x1a",6)||memcmp(program+0x529e - 0x51bc,"\x7c\x53\x7d\xdd\x7e\x50",6))return LT48_FORMAT_ERROR;
 memset(r->kernels,1,sizeof r->kernels);
 const unsigned char config[]={0x86,0x0f,0x97,0x3f};
 memcpy(r->kernels,config,4);memcpy(r->kernels+4,init+0x539f-0x5316,26);
 memcpy(r->kernels+0x40,config,4);memcpy(r->kernels+0x44,init+0x53dd - 0x5316,80);
 /* Skip unchanged bytes, matching the original ROM/GPIO utility semantics.
  * 4004 loop cycles plus explicit latch/pump overhead at audited2MHz.
  * No address changes while EPGM is asserted; actual readback is transmitted. */
 static const unsigned char eprom[]={0x86,0x0f,0x97,0x3f,0x18,0xce,0x20,0,0xce,0,0,
  0x1f,0x74,0x20,0xfc,0xa6,0x77,0x18,0xa1,0,0x27,0x19,0xc6,0x20,0xe7,0x2b,0x18,0xa7,0,
  0xc6,0x21,0xe7,0x2b,0x18,0x3c,0x18,0xce,2,0x3c,0x18,9,0x26,0xfc,
  0x18,0x38,0x6f,0x2b,0x1f,0x74,0x80,0xfc,0x18,0xa6,0,0xa7,0x77,0x18,8,0x20,0xcf};
 if(r->e){
  memset(r->kernels,1,4);memset(r->kernels+0x40,1,4);
  /* Source EEPROM algorithm with E register base/SCI offsets. */
  r->kernels[9]=0x10;r->kernels[12]=0x2e;r->kernels[16]=0x2f;r->kernels[21]=0x2e;r->kernels[25]=0x2f;
  r->kernels[0x49]=0x10;
  for(unsigned i=0x44;i<0x44+80;i++){if(r->kernels[i]==0x74)r->kernels[i]=0x2e;else if(r->kernels[i]==0x77)r->kernels[i]=0x2f;}
 }
 memcpy(r->kernels+0xa0,eprom,sizeof eprom);
 if(r->e){unsigned char *p=r->kernels+0xa0;memset(p,1,4);p[9]=0x10;p[12]=0x2e;p[16]=0x2f;p[48]=0x2e;p[55]=0x2f;unsigned reg=r->e20?0x36:0x3b;p[25]=reg;p[32]=reg;p[46]=reg;}
r->kernels_size=0x102;
 read[0x48]=0x60;read[0x4a]=0;read[0x4c]=30;put16(read+0x58,0x6006);
 unsigned char *q=program;
 q[0xe3]=0x60;q[0xe5]=0x40;q[0xe7]=84;put16(q+0xf3,0x6046);
 q[0xb0]=0x60;q[0xb2]=0xa0;q[0xb4]=sizeof eprom;put16(q+0xc0,0x60a6);put16(q+0xd5,0x6100);
 return LT48_OK;
}
