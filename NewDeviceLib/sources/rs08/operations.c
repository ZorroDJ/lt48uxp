/* Class2014 RS08 BDM owner. Original resources are BYO.
 * Raw reads are authoritative; source Program acknowledgements are not a
 * substitute for independent readback. No security or calibration changes. External12V programming supply. */
#include "../../../c/src/rs08.h"
#include "../../../c/src/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "members.h"
#include "admission.h"
struct lt48_rs08_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[24];
 lt48_compile_variable vars[4];lt48_wave_code workers[4];lt48_applet_address addresses[4];
 unsigned char *probe;size_t probe_size;unsigned probe_address;unsigned worker_count;const unsigned char *fpga;unsigned char *idle;size_t idle_size;
 const struct rs08_member *profile;
};
void lt48_rs08_close(lt48_rs08_resources *r){if(!r)return;for(unsigned i=0;i<24;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r->probe);free(r);}
unsigned lt48_rs08_blank_value(const lt48_rs08_resources *r){return r?r->profile->blank:0;}
unsigned lt48_rs08_minimum_timeout_ms(const lt48_rs08_resources *r){return r?30000u:30000u;}
int lt48_rs08_qualified(const lt48_rs08_resources *r){return r&&r->profile->stage<=LT48_RS08_STAGE;}
/* These are Read regions, independently audited from each original LOAD.
 * Program uses the original wave for every image address, never this map. */
/* RS08 has 2KiB Flash at3800..3FFF in the source16KiB address image. */
static int cell(const lt48_rs08_resources *r,unsigned a){(void)r;return a>=0x3800&&a<0x4000;}
static int trim(const lt48_rs08_resources *r,unsigned a){(void)r;return a==0x3ffa||a==0x3ffb;}
static int blank_skip(const lt48_rs08_resources *r,unsigned a){return trim(r,a)||a==0x3ffc;}


lt48_status lt48_rs08_open_candidate(const lt48_installation *x,size_t device,lt48_rs08_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);if(st)return st;
 const struct rs08_member *p=NULL;
 for(size_t i=0;i<sizeof rs08_members/sizeof *rs08_members;i++){
  const struct rs08_member *m=rs08_members+i;
  if(s.catalogue.vendor&&!strcmp(s.catalogue.vendor,m->vendor)&&s.catalogue.device_name_length==strlen(m->name)&&!memcmp(s.catalogue.device_name,m->name,s.catalogue.device_name_length)&&s.definition_resource==m->definition&&s.family_code==m->family&&s.pinmap==m->pinmap&&s.capacity_bytes==m->capacity)p=m;
 }
 if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;
 if(!p||s.catalogue.device_class!=2014||strcmp(s.definition_stem,"GADB___")||s.element_bits!=8||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 lt48_rs08_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->profile=p;
 char path[4096],name[32];size_t index,fi;int n=snprintf(path,sizeof path,"%s/GADB___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(a) do{st=(a);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b",&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",s.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 lt48_family_info f;snprintf(name,sizeof name,"M%x",s.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count!=p->variables||f.operation_count!=13u){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<r->profile->variables;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned workers=0,seen=0;
 for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));if(op.operation_id>=24||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;
  snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));

  if(w.wave_type==6){if(workers>=4||w.load_address<0x5100u||w.load_address+w.data_size>0x5800u||w.length1!=w.data_size+4||w.length2!=w.data_size){st=LT48_FORMAT_ERROR;goto fail;}if(op.operation_id==15){
    /* Authored FOPT probe: only the initial read address changes. The
     * original read loop and BDM helpers remain byte-for-byte BYO. */
    if(w.load_address!=0x5336||w.data_size!=138||w.data[0x4d]!=0x75||w.data[0x4e]!=0x0d||w.data[0x4f]!=0||w.data[0x50]!=0x75||w.data[0x51]!=0x0e||w.data[0x52]!=0x38){st=LT48_FORMAT_ERROR;goto fail;}
    r->probe=malloc(w.data_size);if(!r->probe){st=LT48_NO_MEMORY;goto fail;}
    memcpy(r->probe,w.data,w.data_size);r->probe[0x4f]=0x10;r->probe[0x52]=0x02;r->probe_size=w.data_size;r->probe_address=w.load_address;
   }
   r->workers[workers]=w;r->addresses[workers++]=(lt48_applet_address){op.operation_id,w.load_address};}
  else if(w.wave_type==4&&op.operation_id==11){if(w.data_size!=2780||w.data[0]!=0xda||w.data[1]!=0x0a){st=LT48_FORMAT_ERROR;goto fail;}r->fpga=w.data+2;}
  else if(op.operation_id==6 || op.operation_id>=20){/* Explicit security operation is excluded. */}
  else if(!w.wave_type){lt48_compact_wave *cw=NULL;unsigned char normalized[512];size_t used=2;
   /* C consumes Stop/completion once. Preserve all source captures and
    * scratch translations; independent raw Read validates programmed bytes. */
   if(op.operation_id==4||op.operation_id==5){
    lt48_compact_wave *raw=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&raw));
    unsigned terminal=0,skip_depth=0;
    for(size_t k=0;k<lt48_compact_instruction_count(raw);k++){
     lt48_compact_instruction a;st=lt48_compact_get_instruction(raw,k,&a);if(st)break;
     if(skip_depth){if(a.opcode==0x5e)skip_depth++;if(a.opcode==0x60)skip_depth--;continue;}
     if(a.opcode==0x39&&a.operand_size==1&&a.operands[0]==0){terminal=op.operation_id!=3;continue;}
     if(terminal&&a.opcode==0x7c&&a.operands[0]==9)continue;
     if(terminal&&a.opcode==0x5e&&a.operands[0]==9){skip_depth=1;terminal=0;continue;}
     terminal=0;
     size_t z=1+a.operand_size;if(used+z>sizeof normalized){st=LT48_FORMAT_ERROR;break;}
     memcpy(normalized+used,w.data+a.offset,z);

     used+=z;
     if(op.operation_id==5&&a.opcode==0x7c&&a.operands[0]==9){
      /* Preserve the raw echo before the original FF-hole masking copy. */
      const unsigned char echo[]={0x49,10,0,9,0,4};
      if(used+sizeof echo>sizeof normalized){st=LT48_FORMAT_ERROR;break;}
      memcpy(normalized+used,echo,sizeof echo);used+=sizeof echo;
     }
    }
    lt48_compact_wave_close(raw);if(st||skip_depth){if(!st)st=LT48_FORMAT_ERROR;goto fail;}
    normalized[0]=(unsigned char)(used-2);normalized[1]=(unsigned char)((used-2)>>8);
   }
   TRY(lt48_compact_decode(op.operation_id==4||op.operation_id==5?normalized:w.data,op.operation_id==4||op.operation_id==5?used:w.data_size,&cw));st=lt48_compact_compile_with_pin_policy(cw,r->pins,r->vars,p->variables,1,LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);lt48_compact_wave_close(cw);if(st)goto fail;if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}}
  else{st=LT48_UNSUPPORTED;goto fail;}
 }
 if(workers!=4u||!r->fpga||seen!=0xb88ffu){st=LT48_FORMAT_ERROR;goto fail;}
 r->worker_count=workers;
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_rs08_close(r);return st;
#undef TRY
}
lt48_status lt48_rs08_open(const lt48_installation *x,size_t device,lt48_rs08_resources **out){return lt48_rs08_open_candidate(x,device,out);}
lt48_status lt48_rs08_validate(const lt48_rs08_resources *r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *image,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(start||n!=16384)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||size!=n))return LT48_INVALID_ARGUMENT;
 if((image&&size!=n)||(!image&&size))return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned a=0;a<n;a++)if(!cell(r,a)&&image[a]!=255)return LT48_SAFETY_REJECTED;
 /* Source forces NVOPT FF. FF or saved unlocked01 input is accepted;
  * security requests are rejected before writes. */
 if(op==LT48_OPERATION_PROGRAM&&image[0x3ffc]!=255&&image[0x3ffc]!=1)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
typedef struct {
 const lt48_rs08_resources *r;lt48_runtime_transport_session *wire;
 unsigned char buffers[32][32];lt48_compile_variable vars[4];uint32_t volts[4];
 unsigned address,mode;const unsigned char *image;int poisoned,active;
} session;
static int transport_error(lt48_status st){return st==LT48_USB_ERROR||st==LT48_PROTOCOL_ERROR||st==LT48_TIMEOUT||st==LT48_CANCELLED;}
static lt48_status run(void *context,unsigned op){
 session *s=context;if(op>=24||!s->r->wave[op])return LT48_UNSUPPORTED;
 lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[30];size_t n=0;
 for(unsigned i=0;i<32;i++)if(i!=0&&i!=4)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 o.device_image=s->image;o.device_image_size=s->image?16384:0;o.device_element_bytes=1;
 o.initial_buffers=seeds;o.initial_buffer_count=n;o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=4;o.parameter_buffer_id=1;
 memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 lt48_runtime_configuration_value cfg[1];size_t count=0;
 lt48_status st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,4,&o,s->r->addresses,s->r->worker_count,s->buffers,cfg,1,&count,s->volts);
 if(transport_error(st))s->poisoned=1;
 return st;
}
static lt48_status reset(session *s,lt48_status st){
 /* Original terminal LOAD/PROGRAM sends Stop before DEVRESET. Erase
  * also gets an explicit Stop after its completion, before any pin writes.
  * DEVRESET retains its source ramp and resident no-op Stop. */
 if(!s->poisoned&&s->active){const unsigned char stop=0;lt48_status z=lt48_runtime_transport_session_controller_write(s->wire,&stop,1);if(transport_error(z))s->poisoned=1;if(!z)s->active=0;if(!st)st=z;}
 if(!s->poisoned){lt48_status z=run(s,3);if(!z){s->active=0;z=lt48_runtime_transport_session_flush(s->wire);}if(transport_error(z))s->poisoned=1;if(!st)st=z;}
 return st;
}
static lt48_status begin(session *s,unsigned mode,unsigned voltage,unsigned first){
 (void)first;s->mode=mode;s->address=0;memset(s->buffers,0,sizeof s->buffers);memset(s->buffers[2],255,32);
 for(unsigned i=0;i<4;i++)if(s->vars[i].variable_id==0)s->vars[i].value=voltage;
 lt48_status st=run(s,0);if(!st){s->active=1;st=run(s,2);}return st;
}
static lt48_status security(session *s){
 /* FOPT0210 is accessible independently of secured Flash (NXP4.6.4).
  * Read only one byte with an address-relocated source read worker. */
 lt48_status st=lt48_owner_load_applet_internal(s->wire,s->r->probe_address,s->r->probe,s->r->probe_size);
 if(transport_error(st))s->poisoned=1;
 if(!st)st=begin(s,1,s->r->profile->supply,0);
 if(!st){s->address=0x3800;st=run(s,4);if(!st&&!(s->buffers[1][0]&1))st=LT48_SAFETY_REJECTED;}
 st=reset(s,st);
 if(!s->poisoned){for(unsigned i=0;i<s->r->worker_count;i++)if(s->r->workers[i].load_address==s->r->probe_address){const lt48_wave_code *w=s->r->workers+i;lt48_status z=lt48_owner_load_applet_internal(s->wire,w->load_address,w->data,w->data_size);if(transport_error(z))s->poisoned=1;if(!st)st=z;}}
 return st;
}
static lt48_status cycle(session *s,int write,unsigned voltage,const unsigned char *image,unsigned char *output){
 s->image=image;lt48_status st=begin(s,write?2:1,voltage,s->r->profile->first);
 for(unsigned a=0;!st&&a<16384;a++){
  s->address=a;s->buffers[2][0]=write?image[a]:255;st=run(s,write?5:4);
  if(!st&&write&&cell(s->r,a)){
   unsigned expected=trim(s->r,a)||a==0x3ffc?255:image[a];
   if(s->buffers[10][0]!=expected){st=LT48_DEVICE_MISMATCH;s->poisoned=1;}
  }
  if(!st&&!write)output[a]=cell(s->r,a)?s->buffers[1][0]:255;
 }
 return reset(s,st);
}
static lt48_status erase(session *s){
 s->image=NULL;lt48_status st=begin(s,7,s->r->profile->supply,s->r->profile->first);
 if(!st){s->active=1;st=run(s,7);if(!st&&s->buffers[1][0]!=1)st=LT48_DEVICE_MISMATCH;}
 return reset(s,st);
}
static lt48_status compare(const lt48_rs08_resources *r,const unsigned char *actual,const unsigned char *wanted,int blank,lt48_small_eprom_result *out){
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<16384;a++)if(cell(r,a)&&!(blank&&blank_skip(r,a))&&(a==0x3ffc?(actual[a]&1):actual[a])!=(wanted?(a==0x3ffc?(wanted[a]&1):wanted[a]):255)){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
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
lt48_status lt48_rs08_execute_internal(const lt48_rs08_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_rs08_validate(r,op,start,length,image,size,margin,type);
 if(!st&&(!api||!api->read||!api->write))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==LT48_OPERATION_READ&&(!read||read_size<length))st=LT48_BUFFER_TOO_SMALL;
 if(st){out->operation_status=st;return st;}
 session s={0};s.r=r;memcpy(s.vars,r->vars,sizeof s.vars);
 unsigned char *actual=malloc(16384),*before=malloc(16384),*effective=malloc(16384);
 if(!actual||!before||!effective){free(actual);free(before);free(effective);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 for(unsigned i=0;i<r->worker_count&&!st;i++){const lt48_wave_code *w=r->workers+i;st=lt48_owner_load_applet_internal(s.wire,w->load_address,w->data,w->data_size);}
 if(!st)st=lt48_owner_load_fpga_internal(s.wire,r->fpga,2778);
 if(!st)st=lt48_runtime_transport_session_flush(s.wire);
 if(transport_error(st))s.poisoned=1;
 if(!st)st=security(&s);
 unsigned supply=r->profile->supply,delta=supply*(type?10u:5u)/100u,first=op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_LOW?supply-delta:op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_HIGH?supply+delta:supply;
 if(!st)st=cycle(&s,0,first,NULL,actual);
 if(!st&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE)){
  memcpy(before,actual,16384);
  if(op==LT48_OPERATION_PROGRAM)for(unsigned a=0;a<16384;a++)if(trim(r,a)&&image[a]!=255&&image[a]!=before[a]){st=LT48_SAFETY_REJECTED;out->first_mismatch=a;break;}
  if(op==LT48_OPERATION_PROGRAM){
   if(!st)st=compare(r,actual,NULL,1,out);
   if(st==LT48_DEVICE_MISMATCH)st=LT48_SAFETY_REJECTED;
   memcpy(effective,image,16384);for(unsigned a=0;a<16384;a++)if(trim(r,a))effective[a]=before[a];effective[0x3ffc]=1;
   if(!st)st=cycle(&s,1,supply,effective,NULL);
  }else st=erase(&s);
  if(!st)st=security(&s);
  if(!st)st=cycle(&s,0,supply,NULL,actual);
  if(!st)for(unsigned a=0;a<16384;a++)if(trim(r,a)&&actual[a]!=before[a]){out->first_mismatch=a;out->mismatch_count++;st=LT48_DEVICE_MISMATCH;break;}
 }
 if(!st&&(op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)){
  memcpy(effective,image,16384);
  for(unsigned a=0;a<16384;a++)if(trim(r,a)&&image[a]==255)effective[a]=actual[a];
  effective[0x3ffc]&=1;
 }
 if(!st&&op!=LT48_OPERATION_READ)st=compare(r,actual,op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_ERASE?NULL:effective,op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_ERASE,out);
 if(!st&&(op==LT48_OPERATION_PROGRAM||(op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL))){const unsigned volts[]={supply-delta,supply+delta};for(unsigned i=0;i<2&&!st;i++){st=cycle(&s,0,volts[i],NULL,actual);if(!st)st=compare(r,actual,effective,0,out);}}
 out->operation_status=st;out->cleanup_status=finish(&s,api,context,st);out->cleanup_completed=out->cleanup_status==LT48_OK;if(!st)st=out->cleanup_status;
 if(!st){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,actual,length);}
 free(actual);free(before);free(effective);return st;
}
lt48_status lt48_rs08_execute(const lt48_rs08_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 if(!lt48_rs08_qualified(r)){memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;out->operation_status=LT48_SAFETY_REJECTED;return LT48_SAFETY_REJECTED;}
 return lt48_rs08_execute_internal(r,api,context,op,start,length,image,size,read,read_size,margin,type,out);
}
