/* Class2014 HC705 bootstrap owner. Exact original resources remain BYO.
 * Raw reads are authoritative; source Program acknowledgements are not a
 * substitute for independent readback. No electrical erase or secure action. */
#include "../../../c/src/hc705_bd.h"
#include "../../../c/src/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "members.h"
#include "admission.h"
struct lt48_hc705_bd_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[20];
 lt48_compile_variable vars[9];lt48_wave_code workers[3];lt48_applet_address addresses[3];
 unsigned char *strict_program;unsigned worker_count;const unsigned char *fpga;unsigned char *idle;size_t idle_size;
 const struct hc705_bd_member *profile;
};
void lt48_hc705_bd_close(lt48_hc705_bd_resources *r){if(!r)return;for(unsigned i=0;i<20;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r->strict_program);free(r);}
unsigned lt48_hc705_bd_blank_value(const lt48_hc705_bd_resources *r){return r?r->profile->blank:0;}
unsigned lt48_hc705_bd_minimum_timeout_ms(const lt48_hc705_bd_resources *r){return r&&r->profile->part==9?100000u:30000u;}
int lt48_hc705_bd_qualified(const lt48_hc705_bd_resources *r){return r&&r->profile->stage<=LT48_HC705_BD_STAGE;}
static int cell(const lt48_hc705_bd_resources *r,unsigned a){return (a>=r->profile->start&&a<=r->profile->end)||a>=r->profile->capacity-16;}

lt48_status lt48_hc705_bd_open_candidate(const lt48_installation *x,size_t device,lt48_hc705_bd_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);if(st)return st;
 const struct hc705_bd_member *p=NULL;
 for(size_t i=0;i<sizeof hc705_bd_members/sizeof *hc705_bd_members;i++){
  const struct hc705_bd_member *m=hc705_bd_members+i;
  if(s.catalogue.vendor&&!strcmp(s.catalogue.vendor,m->vendor)&&s.catalogue.device_name_length==strlen(m->name)&&!memcmp(s.catalogue.device_name,m->name,s.catalogue.device_name_length)&&s.definition_resource==m->definition&&s.family_code==m->family&&s.pinmap==m->pinmap&&s.capacity_bytes==m->capacity)p=m;
 }
 if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;
 if(!p||s.catalogue.device_class!=2014||strcmp(s.definition_stem,"GADB___")||s.element_bits!=8||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 lt48_hc705_bd_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->profile=p;
 char path[4096],name[32];size_t index,fi;int n=snprintf(path,sizeof path,"%s/GADB___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(a) do{st=(a);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b",&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",s.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 lt48_family_info f;snprintf(name,sizeof name,"M%x",s.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count!=p->variables||f.operation_count!=(p->part==9?9u:10u)){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<r->profile->variables;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned workers=0,seen=0;
 for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));if(op.operation_id>=20||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;
  snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));
  if(w.wave_type==6){if(workers>=3||w.load_address<0x5100||w.load_address+w.data_size>0x528a||w.length1!=w.data_size+4||w.length2!=w.data_size){st=LT48_FORMAT_ERROR;goto fail;}r->workers[workers]=w;r->addresses[workers++]=(lt48_applet_address){op.operation_id,w.load_address};}
  else if(w.wave_type==4&&op.operation_id==11){if(w.data_size!=2780||w.data[0]!=0xda||w.data[1]!=0x0a){st=LT48_FORMAT_ERROR;goto fail;}r->fpga=w.data+2;}
  else if(!w.wave_type&&op.operation_id==6){/* Secure is outside this owner; its unmapped source pins are never compiled/executed. */}
  else if(!w.wave_type){lt48_compact_wave *cw=NULL;unsigned char normalized[512];size_t used=2;
   /* C consumes completion once and checks raw acknowledgement before
    * the source LOAD/PROGRAM scratch translation. Retain all other tokens. */
   if(op.operation_id==3||op.operation_id==4||op.operation_id==5||(op.operation_id==2&&p->part==9)){
    lt48_compact_wave *raw=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&raw));
    unsigned terminal=0,skip_depth=0;
    for(size_t k=0;k<lt48_compact_instruction_count(raw);k++){
     lt48_compact_instruction a;st=lt48_compact_get_instruction(raw,k,&a);if(st)break;
     /* Exact source notice1/level3 is shown before execution in the GUI.
      * The original reporting call has no controller or result side effect. */
     if(op.operation_id==2&&p->part==9&&a.opcode==0x90){
      if(a.operand_size!=2||a.operands[0]!=1||a.operands[1]!=3){st=LT48_FORMAT_ERROR;break;}continue;
     }
     if(skip_depth){if(a.opcode==0x5e)skip_depth++;if(a.opcode==0x60)skip_depth--;continue;}
     if(a.opcode==0x39&&a.operand_size==1&&a.operands[0]==0){terminal=op.operation_id!=3;continue;}
     if(terminal&&a.opcode==0x7c&&a.operands[0]==9)continue;
     if(terminal&&a.opcode==0x5e&&a.operands[0]==9){skip_depth=1;terminal=0;continue;}
     terminal=0;
     size_t z=1+a.operand_size;if(used+z>sizeof normalized){st=LT48_FORMAT_ERROR;break;}
     memcpy(normalized+used,w.data+a.offset,z);
     if(op.operation_id==5&&a.opcode==0x7c)normalized[used+1]=9;
     used+=z;
    }
    lt48_compact_wave_close(raw);if(st||skip_depth){if(!st)st=LT48_FORMAT_ERROR;goto fail;}
    normalized[0]=(unsigned char)(used-2);normalized[1]=(unsigned char)((used-2)>>8);
   }
   TRY(lt48_compact_decode(op.operation_id==3||op.operation_id==4||op.operation_id==5||(op.operation_id==2&&p->part==9)?normalized:w.data,op.operation_id==3||op.operation_id==4||op.operation_id==5||(op.operation_id==2&&p->part==9)?used:w.data_size,&cw));st=lt48_compact_compile_with_pin_policy(cw,r->pins,r->vars,p->variables,1,LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);lt48_compact_wave_close(cw);if(st)goto fail;if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}}
  else{st=LT48_UNSUPPORTED;goto fail;}
 }
 if(workers!=2u||!r->fpga||seen!=(p->part==9?0x1883fu:0x1887fu)){st=LT48_FORMAT_ERROR;goto fail;}
 /* Preserve successful original worker instructions; failed programming must
  * return a distinct acknowledgement and wait for Stop inside its worker. */
 if(p->part!=9)for(unsigned i=0;i<workers;i++)if(r->addresses[i].wave==16){
  lt48_wave_code *w=&r->workers[i];unsigned pc=p->part==3?0x526d:0x5265;
  if(w->load_address!=(p->part==3?0x51bau:0x51b0u)||pc<w->load_address||pc+3>w->load_address+w->data_size){st=LT48_FORMAT_ERROR;goto fail;}
  size_t offset=pc-w->load_address;r->strict_program=malloc(w->data_size);if(!r->strict_program){st=LT48_NO_MEMORY;goto fail;}
  memcpy(r->strict_program,w->data,w->data_size);r->strict_program[offset]=2;r->strict_program[offset+1]=0x58;r->strict_program[offset+2]=0;w->data=r->strict_program;
 }
 r->worker_count=workers;
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_hc705_bd_close(r);return st;
#undef TRY
}
lt48_status lt48_hc705_bd_open(const lt48_installation *x,size_t device,lt48_hc705_bd_resources **out){return lt48_hc705_bd_open_candidate(x,device,out);}
lt48_status lt48_hc705_bd_validate(const lt48_hc705_bd_resources *r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *image,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(start||n!=r->profile->capacity)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||size!=n))return LT48_INVALID_ARGUMENT;
 if((image&&size!=n)||(!image&&size))return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned a=0;a<n;a++)if(!cell(r,a)&&image[a]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
typedef struct {
 const lt48_hc705_bd_resources *r;lt48_runtime_transport_session *wire;
 unsigned char buffers[32][32];lt48_compile_variable vars[9];uint32_t volts[4];
 unsigned address,mode;int poisoned,active;
} session;
static int transport_error(lt48_status st){return st==LT48_USB_ERROR||st==LT48_PROTOCOL_ERROR||st==LT48_TIMEOUT||st==LT48_CANCELLED;}
static lt48_status run(void *context,unsigned op){
 session *s=context;if(op>=20||!s->r->wave[op])return LT48_UNSUPPORTED;
 lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[30];size_t n=0;
 for(unsigned i=0;i<32;i++)if(i!=0&&i!=4)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 o.initial_buffers=seeds;o.initial_buffer_count=n;o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=4;o.parameter_buffer_id=1;
 memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 lt48_runtime_configuration_value cfg[1];size_t count=0;
 lt48_status st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->profile->variables,&o,s->r->addresses,s->r->worker_count,s->buffers,cfg,1,&count,s->volts);
 if(transport_error(st))s->poisoned=1;
 return st;
}
static lt48_status stop_worker(session *s,unsigned expected){
 const unsigned char stop=0;unsigned char terminal=0;
 lt48_status st=lt48_runtime_transport_session_controller_write(s->wire,&stop,1);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1);
 if(transport_error(st))s->poisoned=1;
 if(st==LT48_OK){s->active=0;if(terminal!=expected)st=LT48_DEVICE_MISMATCH;}return st;
}
static lt48_status cycle(session *s,int write,unsigned voltage,const unsigned char *image,unsigned char *output){
 s->mode=write?2:1;s->address=0;memset(s->buffers,0,sizeof s->buffers);memset(s->buffers[2],255,32);
 for(unsigned i=0;i<s->r->profile->variables;i++)if(s->vars[i].variable_id==0)s->vars[i].value=voltage;
 lt48_status st=run(s,0);if(st==LT48_OK){s->active=1;st=run(s,2);}
 for(unsigned a=0;st==LT48_OK&&a<s->r->profile->capacity;a++){
  /* BD3/BD7 source Program transfers FF through every image hole;
   * their bootstrap stream must advance through the full4000hex image.
   * BD9 alone skips holes in the source Program waveform. */
  if(write&&s->r->profile->part==9&&!cell(s->r,a))continue;
  s->address=a;s->buffers[2][0]=write?image[a]:255;st=run(s,write?5:4);
  if(st==LT48_OK&&write&&s->buffers[9][0]!=image[a]){
   st=LT48_DEVICE_MISMATCH;
   /* BD9 does not poll Stop until its complete RAM buffer has been filled.
    * On an invalid buffer echo, do not feed commands to that live receiver. */
   if(s->r->profile->part==9)s->poisoned=1;
  }
  if(st==LT48_OK&&!write)output[a]=cell(s->r,a)?s->buffers[1][0]:255;
 }
 if(!s->poisoned&&s->active){lt48_status stop=stop_worker(s,1);if(st==LT48_OK)st=stop;}
 if(!s->poisoned){
  s->mode=1; /* source Read-mode reset restores low rails after either operation */
  lt48_status reset=run(s,3);
  /* A final queued GPIO write must reach the controller before closing the
   * session or starting another pass; otherwise RESET can remain released. */
  if(reset==LT48_OK)reset=lt48_runtime_transport_session_flush(s->wire);
  if(transport_error(reset))s->poisoned=1;
  if(st==LT48_OK)st=reset;
 }
 return st;
}
static lt48_status compare(const lt48_hc705_bd_resources *r,const unsigned char *actual,const unsigned char *wanted,lt48_small_eprom_result *out){
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<r->profile->capacity;a++)if(cell(r,a)&&actual[a]!=(wanted?wanted[a]:lt48_hc705_bd_blank_value(r))){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status finish(session *s,const lt48_protocol_transport *api,void *context,lt48_status operation){
 lt48_runtime_transport_session_close(s->wire);s->wire=NULL;if(s->poisoned)return LT48_CLEANUP_FAILED;
 lt48_protocol_transport clean=*api;clean.cancelled=NULL;
 for(unsigned phase=0;phase<8;phase++){
  lt48_status st=lt48_runtime_transport_session_create(&clean,context,&s->wire);
  if(st==LT48_OK){
   if(!phase)st=run(s,1);
   else if(phase<=4){const unsigned char off[]={0x10,(unsigned char)(4-phase),0};st=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof off);}
   else if(phase==5)st=lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
   else if(phase==6){const unsigned char done[]={0x49,0xaa,0x55};st=lt48_runtime_transport_session_controller_write(s->wire,done,sizeof done);}
   else{const unsigned char led[]={0x40,operation==LT48_OK?4:2};st=lt48_runtime_transport_session_controller_write(s->wire,led,sizeof led);}
   if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
  }
  lt48_runtime_transport_session_close(s->wire);s->wire=NULL;if(st!=LT48_OK)return st;
 }
 return LT48_OK;
}
lt48_status lt48_hc705_bd_execute_internal(const lt48_hc705_bd_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_hc705_bd_validate(r,op,start,length,image,size,margin,type);
 if(st==LT48_OK&&(!api||!api->read||!api->write))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){out->operation_status=st;return st;}
 session s={0};s.r=r;memcpy(s.vars,r->vars,sizeof s.vars);unsigned char *actual=malloc(length);if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 for(unsigned i=0;i<r->worker_count&&st==LT48_OK;i++){const lt48_wave_code *w=r->workers+i;st=lt48_owner_load_applet_internal(s.wire,w->load_address,w->data,w->data_size);}
 /* Authored RAM continuation, never stored in programmer EEPROM/flash. */
 static const unsigned char failure[]={0x90,0xc8,0,0xe9,0x04,0xf0,0x20,0xb2,0xfd,0x20,0xb5,0xfa,0x74,0x99,0xf0,0x22};
 if(st==LT48_OK&&r->profile->part!=9)st=lt48_owner_load_applet_internal(s.wire,0x5800,failure,sizeof failure);
 if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s.wire,r->fpga,2778);
 if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
 if(transport_error(st))s.poisoned=1;
 unsigned delta=type?50:25,first=op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_LOW?500-delta:op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;
 if(st==LT48_OK)st=cycle(&s,0,first,NULL,actual);
 if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM){st=compare(r,actual,NULL,out);if(st==LT48_DEVICE_MISMATCH)st=LT48_SAFETY_REJECTED;if(st==LT48_OK)st=cycle(&s,1,500,image,NULL);if(st==LT48_OK)st=cycle(&s,0,500,NULL,actual);}
 if(st==LT48_OK&&op!=LT48_OPERATION_READ)st=compare(r,actual,op==LT48_OPERATION_BLANK_CHECK?NULL:image,out);
 if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||(op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL))){const unsigned volts[]={500-delta,500+delta};for(unsigned i=0;i<2&&st==LT48_OK;i++){st=cycle(&s,0,volts[i],NULL,actual);if(st==LT48_OK)st=compare(r,actual,image,out);}}
 out->operation_status=st;out->cleanup_status=finish(&s,api,context,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(st==LT48_OK)st=out->cleanup_status;
 if(st==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,actual,length);}free(actual);return st;
}
lt48_status lt48_hc705_bd_execute(const lt48_hc705_bd_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 if(!lt48_hc705_bd_qualified(r)){memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;out->operation_status=LT48_SAFETY_REJECTED;return LT48_SAFETY_REJECTED;}
 return lt48_hc705_bd_execute_internal(r,api,context,op,start,length,image,size,read,read_size,margin,type,out);
}
