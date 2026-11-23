#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/* Shared NMOS68705 owner. BYO source programming procedure; authored NUM
 * acquisition. Production admission remains closed; see execute_with_contract_internal.
 * No programmer EEPROM, security applet or nonvolatile maintenance handler. */
#include "../../../c/src/nmos68705.h"
#include "../../../c/src/internal.h"
#include "num_transport.h"
#include <labtool48/pin_control.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "assets.inc"
struct lt48_nmos68705_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[8];
 lt48_compile_variable vars[6];lt48_wave_code program;
 const unsigned char *source_fpga;unsigned char *idle;size_t idle_size;
 unsigned capacity,pgroup,variant; /* All silicon variants remain unqualified. */
 int admitted;lt48_nmos68705_num_contract contract;
};
int lt48_nmos68705_qualified(const lt48_nmos68705_resources *r){return r&&r->admitted;}
lt48_status lt48_nmos68705_get_contract_internal(const lt48_nmos68705_resources *r,lt48_nmos68705_num_contract *out){
 if(!r||!out)return LT48_INVALID_ARGUMENT;
 return lt48_nmos68705_contract_for_variant_internal(r->variant,out);
}
const char *lt48_nmos68705_read_contract_status(const lt48_nmos68705_resources *r){
 if(!r)return "No selected NUM contract";
 if(r->variant==0)return "R3 decoder contract: real captured PB mapping and bootstrap identity; board entry remains unqualified";
 if(r->variant>=4)return "P3/P5 decoder contract: primary measured PB mapping and bootstrap identity; board entry and oscillator/security applicability remain unqualified";
 if(r->variant==2)return "U3 bootstrap identity known; NUM address mapping and board entry remain unqualified";
 return "R5/U5 bootstrap identity, NUM behavior and board entry remain unqualified";
}
static int cell(unsigned n,unsigned a){return (a>=0x80&&a<=(n==4096?0xf38u:0x784u))||a>=n-8;}
static unsigned mor(unsigned n){return n==4096?0xf38u:0x784u;}
void lt48_nmos68705_close(lt48_nmos68705_resources *r){if(!r)return;for(unsigned i=0;i<8;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);}
lt48_status lt48_nmos68705_open(const lt48_installation *x,size_t device,lt48_nmos68705_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);if(st!=LT48_OK)return st;
 const char *names[]={"MC68705R3","MC68705R5","MC68705U3","MC68705U5","MC68705P3","MC68705P5"};unsigned which=6;
 for(unsigned i=0;i<6;i++)if(s.catalogue.device_name_length==strlen(names[i])&&!memcmp(s.catalogue.device_name,names[i],strlen(names[i])))which=i;
 if(which==6||!s.catalogue.vendor||(strcmp(s.catalogue.vendor,"Motorola")&&strcmp(s.catalogue.vendor,"Freescale"))||s.catalogue.device_class!=2013||strcmp(s.definition_stem,"GADA___")||s.element_bits!=8||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 unsigned pg=which>=4;
 if(s.definition_resource!=(pg?0xb8u:0xb9u)||s.family_code!=(pg?0x6bu:0x6cu)||s.pinmap!=(pg?0x48u:0x49u)||s.capacity_bytes!=(pg?2048u:4096u))return LT48_SAFETY_REJECTED;
 lt48_nmos68705_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->capacity=(unsigned)s.capacity_bytes;r->pgroup=pg;r->variant=which;
 /* Failure leaves the zero-initialized contract unavailable, never guessed. */
 (void)lt48_nmos68705_contract_for_variant_internal(which,&r->contract);
 char path[4096],name[32];size_t idx,family;int n=snprintf(path,sizeof path,"%s/GADA___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(a) do{st=(a);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"aca05831121f0e30654601df133b298cec1d33e7ffb94626c5d62634e690c452",&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",s.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&idx));TRY(lt48_rez_get_pin_code(r->rez,idx,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 lt48_family_info f;snprintf(name,sizeof name,"M%x",s.family_code);TRY(lt48_rez_find_resource(r->rez,name,&family));TRY(lt48_rez_get_family_info(r->rez,family,&f));
 if(f.variable_count!=6||f.operation_count!=9){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<6;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,family,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned seen=0;
 for(size_t i=0;i<9;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,family,i,&op));
  if(op.operation_id>=16||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;
  snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&idx));TRY(lt48_rez_get_wave_code(r->rez,idx,&w));
  if(w.wave_type==0&&op.operation_id<8){
   /* Source reset sends termination without capturing its byte. The owner
    * explicitly stops/drains first; retain only the subsequent rail ramp. */
   unsigned char suffix[96];const unsigned char *data=w.data;size_t len=w.data_size;
   if(op.operation_id==5){if(len<4||len>sizeof suffix||memcmp(data+2,"\x39\0",2)){st=LT48_FORMAT_ERROR;goto fail;}memcpy(suffix,data+2,len-2);len-=2;suffix[0]=(unsigned char)(len-2);suffix[1]=0;data=suffix;}
   if(op.operation_id==7){
    /* Preserve the actual byte status before source LOAD-style echo/copy.
     * Adds only a host memmove: controller/FPGA transactions are unchanged. */
    static const unsigned char retain[]={0x49,13,0,5,0,4};
    if(len<7||len+sizeof retain>sizeof suffix||memcmp(data+2,"\x3e\x07\0\x7c\x05",5)){st=LT48_FORMAT_ERROR;goto fail;}
    memcpy(suffix,data,7);memcpy(suffix+7,retain,sizeof retain);memcpy(suffix+7+sizeof retain,data+7,len-7);len+=sizeof retain;suffix[0]=(unsigned char)(len-2);suffix[1]=0;data=suffix;
   }
   lt48_compact_wave *cw=NULL;st=lt48_compact_decode(data,len,&cw);
   if(st==LT48_OK)st=lt48_compact_compile_with_pin_policy(cw,r->pins,r->vars,6,1,LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);
   lt48_compact_wave_close(cw);if(st!=LT48_OK)goto fail;
  }else if(w.wave_type==6&&op.operation_id==13){if(w.load_address!=0x51b4||w.data_size!=134){st=LT48_FORMAT_ERROR;goto fail;}r->program=w;}
  else if(w.wave_type==4&&op.operation_id==10){if(w.data_size!=2780){st=LT48_FORMAT_ERROR;goto fail;}r->source_fpga=w.data+2;}
  else if(w.wave_type!=6||op.operation_id!=15){st=LT48_UNSUPPORTED;goto fail;}
 }
 if(seen!=0xa4f3||!r->source_fpga||!r->program.data){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_nmos68705_close(r);return st;
#undef TRY
}
lt48_status lt48_nmos68705_validate(const lt48_nmos68705_resources *r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *data,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(start||n!=r->capacity)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!data||size!=n))return LT48_INVALID_ARGUMENT;
 if((data&&size!=n)||(!data&&size))return LT48_INVALID_ARGUMENT;
 if(data)for(unsigned a=0;a<n;a++)if(!cell(r->capacity,a)&&data[a]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
typedef struct {
 const lt48_nmos68705_resources *r;lt48_runtime_transport_session *wire;
 const lt48_protocol_transport *api;void *context;
 unsigned char buffers[14][32];uint32_t volts[4];unsigned address,mode;
 int poisoned,capture,programming;
} session;
static lt48_status wire_status(session *s,lt48_status st){if(st!=LT48_OK)s->poisoned=1;return st;}
static lt48_status command(session *s,const unsigned char *p,size_t n){lt48_status st=lt48_runtime_transport_session_controller_write(s->wire,p,n);if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);return wire_status(s,st);}
static lt48_status delay_us(unsigned us){
#ifdef _WIN32
 Sleep((us+999)/1000);return LT48_OK;
#else
 struct timespec t={(time_t)(us/1000000),(long)(us%1000000)*1000};return nanosleep(&t,NULL)?LT48_RUNTIME_ERROR:LT48_OK;
#endif
}
static lt48_status status(session *s){return wire_status(s,lt48_runtime_transport_session_status(s->wire,1));}
static lt48_status load_fpga(session *s,const unsigned char *data){lt48_status st=lt48_owner_load_fpga_internal(s->wire,data,2778);if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);return wire_status(s,st);}
static lt48_status load_worker(session *s,unsigned addr,const unsigned char *data,size_t n){lt48_status st=lt48_owner_load_applet_internal(s->wire,(uint16_t)addr,data,n);if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);return wire_status(s,st);}
static lt48_status run(session *s,unsigned op){
 if(op>=8||!s->r->wave[op])return LT48_UNSUPPORTED;
 lt48_runtime_buffer_seed seeds[12];unsigned count=0;
 for(unsigned i=0;i<14;i++)if(i!=4&&i!=12)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 lt48_runtime_options o={0};o.use_explicit_buffer_ids=1;o.address_buffer=4;o.mode_buffer=12;o.parameter_buffer_id=7;o.address=s->address;o.mode=s->mode;o.initial_buffers=seeds;o.initial_buffer_count=count;memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 lt48_applet_address app={13,0x51b4};lt48_runtime_machine *m=NULL;lt48_runtime_result *r=NULL;
 lt48_status st=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->r->vars,6,&o,&m);
 if(st==LT48_OK)st=lt48_runtime_transport_session_run_with_applets(s->wire,m,&app,1,&r);
 if(st==LT48_OK){const unsigned char *p;size_t n;for(unsigned i=0;i<14&&st==LT48_OK;i++){st=lt48_runtime_get_buffer(r,i,&p,&n);if(st==LT48_OK&&n!=32)st=LT48_RUNTIME_ERROR;if(st==LT48_OK)memcpy(s->buffers[i],p,32);}for(unsigned i=0;i<4&&st==LT48_OK;i++)st=lt48_runtime_get_voltage(r,i,s->volts+i);}
 lt48_runtime_result_close(r);lt48_runtime_machine_close(m);return wire_status(s,st);
}
static lt48_status off(session *s){for(unsigned i=4;i;i--){const unsigned char b[]={0x10,(unsigned char)(i-1),0};lt48_status st=command(s,b,sizeof b);if(st!=LT48_OK)return st;}return delay_us(1000);}
static lt48_status baseline(session *s){lt48_status st=off(s);if(st==LT48_OK)st=load_fpga(s,s->r->idle);if(st==LT48_OK){const unsigned char b[]={0x49,0xaa,0x55};st=command(s,b,sizeof b);}return st;}
static lt48_status stop_num(session *s){if(!s->capture)return LT48_OK;lt48_status st=wire_status(s,lt48_num_stop_internal(s->wire));if(st==LT48_OK)s->capture=0;return st;}
static lt48_status read_cycle(session *s,const lt48_nmos68705_num_contract *contract,unsigned vcc,unsigned char *image){
 lt48_status st=baseline(s);lt48_pin_configuration c={0};lt48_pin_plan plan;
 c.verified_baseline_fpga=1;c.discharge_us=1000;c.settle_us=1000;
 c.routes[(s->r->pgroup?11:5)-1]=LT48_PIN_TPIC_SINK;
 c.routes[(s->r->pgroup?13:8)-1]=LT48_PIN_BOTTOM_SOURCE;
 c.routes[(s->r->pgroup?16:11)-1]=LT48_PIN_BOTTOM_SOURCE;
 c.routes[(s->r->pgroup?18:20)-1]=LT48_PIN_TOP_SOURCE;
 if(st==LT48_OK)st=lt48_pin_control_plan(&c,&plan,NULL,0);
 /* This plan selects analog routes with all DACs zero under baseline only. */
 for(size_t i=0;st==LT48_OK&&i<plan.count;i++){
  const lt48_pin_step *p=plan.steps+i;
  if(p->kind==LT48_PIN_CONTROLLER_BYTES)st=command(s,p->bytes,p->size);
  else if(p->kind==LT48_PIN_WAIT_US)st=delay_us(p->delay_us);
  else if(p->kind==LT48_PIN_STATUS_CHECK)st=status(s);
  else st=LT48_SAFETY_REJECTED;
 }
 if(st==LT48_OK)st=load_worker(s,0x6e00,num_prepare,sizeof num_prepare);
 if(st==LT48_OK)st=wire_status(s,lt48_num_prepare_ready_internal(s->wire));
 if(st==LT48_OK)st=load_worker(s,0x6000,num_worker,sizeof num_worker);
 if(st==LT48_OK)st=load_fpga(s,s->r->pgroup?num_p_fpga:num_ru_fpga);
 if(st==LT48_OK)st=wire_status(s,lt48_num_stop_internal(s->wire));
 /* DAC settings model the proposed adapter, NOT measured NUM voltage. */
 for(unsigned ch=1;st==LT48_OK&&ch<4;ch++){unsigned code=ch==3?95:vcc/10+13;const unsigned char b[]={0x10,(unsigned char)ch,(unsigned char)code};st=command(s,b,sizeof b);if(st==LT48_OK)st=delay_us(1000);}
 if(st==LT48_OK)st=status(s);
 if(st==LT48_OK){s->capture=1;st=wire_status(s,lt48_num_start_internal(s->wire));}
 lt48_num_reader reader;
 if(st==LT48_OK)st=lt48_num_reader_init_internal(&reader,s->wire,s->r->capacity);
 if(st==LT48_OK){st=lt48_nmos68705_num_collect(contract,lt48_num_cached_pair_internal,&reader,image,s->r->capacity,&(lt48_nmos68705_num_result){0});if(reader.failed)s->poisoned=1;}
 if(!s->poisoned){lt48_status done=stop_num(s);if(st==LT48_OK)st=done;if(!s->poisoned){done=baseline(s);if(st==LT48_OK)st=done;}}
 return st;
}
static lt48_status stop_program(session *s){
 if(!s->programming)return LT48_OK;
 const unsigned char zero=0;unsigned char terminal;
 lt48_status st=command(s,&zero,1);
 if(st==LT48_OK)st=wire_status(s,lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1));
 if(st==LT48_OK&&terminal!=0)st=LT48_PROTOCOL_ERROR;
 if(st==LT48_OK)s->programming=0;else s->poisoned=1;
 return st;
}
static lt48_status program(session *s,const unsigned char *image){
 lt48_status st=baseline(s);
 if(st==LT48_OK)st=load_worker(s,0x51b4,s->r->program.data,s->r->program.data_size);
 if(st==LT48_OK)st=load_fpga(s,s->r->source_fpga);
 if(st==LT48_OK)st=run(s,0);
 s->mode=2;s->address=0;
 if(st==LT48_OK){s->programming=1;st=run(s,4);}
 for(unsigned a=0;st==LT48_OK&&a<s->r->capacity;a++){
  s->address=a;memset(s->buffers[5],0,32);memset(s->buffers[13],0,32);memset(s->buffers[7],0,32);s->buffers[7][0]=image[a];
  st=run(s,7);
  if(st==LT48_OK&&s->buffers[13][0]!=1){
   /* The original worker returns after timeout with failure00, terminal00.
    * Drain the terminal before rails/FPGA changes; do not send another stop
    * byte after the worker has already returned to the resident dispatcher. */
   unsigned char terminal=255;
   if(s->buffers[13][0]!=0){s->poisoned=1;st=LT48_PROTOCOL_ERROR;}
   else{
    st=wire_status(s,lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1));
    if(st==LT48_OK&&terminal!=0){s->poisoned=1;st=LT48_PROTOCOL_ERROR;}
    if(st==LT48_OK){s->programming=0;st=LT48_DEVICE_MISMATCH;}
   }
  }
  if(st==LT48_OK&&s->buffers[5][0]!=image[a])st=LT48_DEVICE_MISMATCH;
 }
 if(!s->poisoned){lt48_status done=stop_program(s);if(st==LT48_OK)st=done;if(!s->poisoned){done=run(s,5);if(st==LT48_OK)st=done;}if(!s->poisoned){done=baseline(s);if(st==LT48_OK)st=done;}}
 return st;
}
static lt48_status compare(const lt48_nmos68705_resources *r,const unsigned char *actual,const unsigned char *expected,lt48_small_eprom_result *out){
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<r->capacity;a++)if(cell(r->capacity,a)&&actual[a]!=(expected?expected[a]:0)){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static int contract_valid(const lt48_nmos68705_num_contract *c,unsigned n){
 if(!c||c->space_size!=n||c->bootstrap_start!=(n==4096?0xf80u:0x785u)||c->bootstrap_size!=(n==4096?120u:115u))return 0;
 unsigned mask=0,hash=0;for(unsigned i=0;i<32;i++)hash|=c->bootstrap_sha256[i];
 for(unsigned i=0;i<8;i++){unsigned b=c->address_bits[i];if(b>=(n==4096?12u:11u)||(mask&(1u<<b)))return 0;mask|=1u<<b;}
 return hash&&(mask&63)==63;
}
lt48_status lt48_nmos68705_execute_with_contract_internal(const lt48_nmos68705_resources *r,const lt48_nmos68705_num_contract *contract,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *input,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_nmos68705_validate(r,op,start,length,input,size,margin,type);
 if(st==LT48_OK&&(!api||!api->read||!api->write||!contract_valid(contract,r->capacity)))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))st=LT48_BUFFER_TOO_SMALL;
 if(st==LT48_OK&&api->cancelled&&api->cancelled(context))st=LT48_CANCELLED;
 if(st!=LT48_OK){out->operation_status=st;out->cleanup_completed=1;return st;}
 unsigned char actual[4096];session s={0};s.r=r;s.api=api;s.context=context;
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 unsigned delta=type?50:25;
 unsigned first_voltage=op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_LOW?500-delta:op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;
 if(st==LT48_OK)st=read_cycle(&s,contract,first_voltage,actual);
 int writing=op==LT48_OPERATION_PROGRAM;
 if(st==LT48_OK&&writing){
  /* Do not alter oscillator/security options as a side effect of Program.
   * Selected option byte must match the observed byte before any EPROM pulse. */
  if(input[mor(r->capacity)]!=actual[mor(r->capacity)])st=LT48_SAFETY_REJECTED;
  for(unsigned a=0;st==LT48_OK&&a<r->capacity;a++)if(cell(r->capacity,a)&&a!=mor(r->capacity)&&actual[a])st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK)st=program(&s,input);
 }
 unsigned volts[]={500,500-delta,500+delta};
 if(st==LT48_OK&&(writing||op==LT48_OPERATION_VERIFY)){
  unsigned begin=0,end=3;
  if(!writing&&margin!=LT48_VERIFY_MARGIN_ALL){begin=margin==LT48_VERIFY_MARGIN_LOW?1:margin==LT48_VERIFY_MARGIN_HIGH?2:0;end=begin+1;}
  for(unsigned i=begin;st==LT48_OK&&i<end;i++){if(!writing&&i==begin){}else st=read_cycle(&s,contract,volts[i],actual);if(st==LT48_OK)st=compare(r,actual,input,out);}
 }else if(st==LT48_OK&&op!=LT48_OPERATION_READ)st=compare(r,actual,op==LT48_OPERATION_BLANK_CHECK?NULL:input,out);
 out->operation_status=st;out->cleanup_status=LT48_OK;
 if(s.poisoned){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
 else if(s.wire){lt48_status done=stop_num(&s);if(done==LT48_OK)done=stop_program(&s);if(done==LT48_OK)done=baseline(&s);if(done==LT48_OK){const unsigned char led[]={0x40,st==LT48_OK?4:2};done=command(&s,led,sizeof led);}if(done==LT48_OK)done=status(&s);out->cleanup_status=done;out->cleanup_completed=done==LT48_OK;}else out->cleanup_completed=1;
 lt48_runtime_transport_session_close(s.wire);
 if(st==LT48_OK)st=out->cleanup_status;
 if(st==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,actual,length);}
 return st;
}
lt48_status lt48_nmos68705_execute(const lt48_nmos68705_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *data,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_nmos68705_validate(r,op,start,n,data,size,margin,type);
 if(st==LT48_OK&&!lt48_nmos68705_qualified(r))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK){out->operation_status=st;return st;}
 return lt48_nmos68705_execute_with_contract_internal(r,&r->contract,api,context,op,start,n,data,size,read,read_size,margin,type,out);
}
