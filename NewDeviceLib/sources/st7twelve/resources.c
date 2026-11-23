/* Finite ST7 source loader and audited operation owner. */
#include "../../../c/src/st7.h"
#include "../../../c/src/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admission.h"
struct st7_profile {const char*name;unsigned definition,family,pinmap,variables,code_start,eeprom_start,eeprom_end,option_bytes,operations[20];};
static const struct st7_profile profiles[]={
#include "profiles.inc"
};
struct lt48_st7_resources {
 lt48_rez*rez;lt48_pin_map*pins;lt48_compiled_wave*waves[20];
 lt48_compile_variable vars[2];lt48_runtime_frame_profile frame;
 lt48_wave_code workers[4];lt48_applet_address addresses[4];
 lt48_runtime_nested_wave nested[8];size_t nested_count;
 const unsigned char*fpga;unsigned char*idle;size_t idle_size;
 const struct st7_profile*profile;unsigned char*patched_config;
};
void lt48_st7_close(lt48_st7_resources*r){if(!r)return;for(unsigned i=0;i<20;i++)lt48_compiled_wave_close(r->waves[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r->patched_config);free(r);}
lt48_status lt48_st7_open(const lt48_installation*x,size_t device,lt48_st7_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);if(st)return st;
 if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10")||s.catalogue.device_class!=2014||!s.catalogue.vendor||strcmp(s.catalogue.vendor,"ST")||strcmp(s.definition_stem,"GADB___")||s.capacity_bytes!=65536||s.element_bits!=8||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 const struct st7_profile*p=NULL;for(size_t i=0;i<sizeof profiles/sizeof*profiles;i++){const struct st7_profile*q=profiles+i;if(s.definition_resource==q->definition&&s.family_code==q->family&&s.pinmap==q->pinmap&&s.catalogue.device_name_length==strlen(q->name)&&!memcmp(s.catalogue.device_name,q->name,s.catalogue.device_name_length))p=q;}if(!p)return LT48_UNSUPPORTED;
 lt48_st7_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;r->profile=p;char path[4096],name[32];size_t ix,fi;int n=snprintf(path,sizeof path,"%s/GADB___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(e) do{st=(e);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pin;TRY(lt48_rez_find_resource(r->rez,name,&ix));TRY(lt48_rez_get_pin_code(r->rez,ix,&pin));TRY(lt48_pin_map_parse(&pin,&r->pins));snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));TRY(lt48_rez_get_family_frame_profile(r->rez,fi,&r->frame));if(f.operation_count!=13||f.variable_count!=p->variables||p->variables>2){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<p->variables;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned seen=0,wc=0;for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));if(op.operation_id>=20||p->operations[op.operation_id]!=op.wave||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ix));TRY(lt48_rez_get_wave_code(r->rez,ix,&w));
  if(w.wave_type==6){if(wc==4||op.operation_id<15||op.operation_id>18||w.load_address<0x5100||w.load_address+w.data_size>0x5b00||w.length1!=w.data_size+4||w.length2!=w.data_size){st=LT48_FORMAT_ERROR;goto fail;}r->workers[wc]=w;r->addresses[wc++]=(lt48_applet_address){op.operation_id,w.load_address};}
  else if(w.wave_type==4&&op.operation_id==11&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a)r->fpga=w.data+2;
  else if(!w.wave_type){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,p->variables,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->waves[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;r->nested[r->nested_count++]=(lt48_runtime_nested_wave){op.operation_id,r->waves[op.operation_id]};}
  else{st=LT48_FORMAT_ERROR;goto fail;}
 }
 if(seen!=0x78a7f||wc!=4||!r->fpga||r->nested_count!=8){st=LT48_FORMAT_ERROR;goto fail;}
 { /* Original Protect sends Stop without checking the worker completion.
     * Capture its result before each Stop, keeping the other source steps. */
  unsigned wave=p->operations[6];snprintf(name,sizeof name,"W%x",wave);lt48_wave_code w;TRY(lt48_rez_find_resource(r->rez,name,&ix));TRY(lt48_rez_get_wave_code(r->rez,ix,&w));
  unsigned char raw[256];if(w.data_size>sizeof raw-8){st=LT48_FORMAT_ERROR;goto fail;}size_t n=2;unsigned captures=0;
  for(size_t k=2;k<w.data_size;k++){
   if(k+1<w.data_size&&w.data[k]==0x39&&w.data[k+1]==0){raw[n++]=0x7c;raw[n++]=(unsigned char)(30+captures++);}
   raw[n++]=w.data[k];
  }if(captures!=(p->option_bytes==3?2u:1u)){st=LT48_FORMAT_ERROR;goto fail;}raw[0]=(unsigned char)(n-2);raw[1]=(unsigned char)((n-2)>>8);lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(raw,n,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,p->variables,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->waves[19]);lt48_compact_wave_close(c);if(st)goto fail;
 }
 /* ST72F34x option monitor serial TX destroys X. The source loop needs X=15
  * after its first byte; restore it in place of the following redundant BRA.
  * Only the reversed embedded ST7 bytes change, never the source archive. */
 if(p->definition>=0x7c&&p->definition<=0x7e){
  const unsigned char signature[]={0xda,0x2a,0x5d,0xdd,0x20,0xd0,0x00,0xcd};
  unsigned hits=0;for(unsigned i=0;i<4;i++)if(r->addresses[i].wave==18){
   lt48_wave_code*w=&r->workers[i];r->patched_config=malloc(w->data_size);if(!r->patched_config){st=LT48_NO_MEMORY;goto fail;}memcpy(r->patched_config,w->data,w->data_size);
   for(size_t k=0;k+sizeof signature<=w->data_size;k++)if(!memcmp(w->data+k,signature,sizeof signature)){r->patched_config[k+3]=0x0f;r->patched_config[k+4]=0xae;hits++;}
   w->data=r->patched_config;
  }if(hits!=1){st=LT48_FORMAT_ERROR;goto fail;}
 }
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_st7_close(r);return st;
#undef TRY
}

/* Shared finite owner; source operations and authored preservation policy. */
unsigned lt48_st7_qualified(const lt48_st7_resources*r){return r&&LT48_ST7TWELVE_ADMITTED;}
static int st7_cell(const lt48_st7_resources*r,unsigned a){return a>=r->profile->code_start||(r->profile->eeprom_end&&a>=r->profile->eeprom_start&&a<r->profile->eeprom_end);}
lt48_status lt48_st7_validate(const lt48_st7_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(!((op>=LT48_OPERATION_READ&&op<=LT48_OPERATION_SECURE)||op==LT48_OPERATION_CONFIGURATION))return LT48_UNSUPPORTED;
 if(start||n!=65536)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||size!=n))return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_CONFIGURATION&&((image&&size!=n)||(!image&&size)))return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_CONFIGURATION){if((!image&&size)||(image&&size!=r->profile->option_bytes+1))return LT48_INVALID_ARGUMENT;return LT48_OK;}
 if(image)for(unsigned a=0;a<65536;a++)if(!st7_cell(r,a)&&image[a]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
typedef struct {const lt48_st7_resources*r;lt48_runtime_transport_session*wire;lt48_compile_variable vars[2];unsigned char buffers[32][32];uint32_t volts[4];lt48_runtime_configuration_value config[3];size_t config_count;unsigned mode,address,reserved;} st7_session;
static lt48_status st7_run(void*ctx,unsigned op){
 st7_session*s=ctx;if(op>=20||!s->r->waves[op])return LT48_UNSUPPORTED;
 lt48_runtime_buffer_seed seeds[30];size_t count=0;for(unsigned i=0;i<32;i++)if(i!=0&&i!=4)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 lt48_runtime_options o={0};o.initial_buffers=seeds;o.initial_buffer_count=count;o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.mode_buffer=4;o.address_buffer=0;o.parameter_buffer_id=2;o.root_timeout_seconds=120;o.nested_waves=s->r->nested;o.nested_wave_count=s->r->nested_count;o.configuration=s->config;o.configuration_count=s->config_count;memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 return lt48_owner_run_internal(s->wire,s->r->waves[op],s->r->pins,s->vars,s->r->profile->variables,&o,s->r->addresses,4,s->buffers,s->config,3,&s->config_count,s->volts);
}
static unsigned st7_option(const st7_session*s,unsigned field){for(size_t i=0;i<s->config_count;i++)if(s->config[i].object_id==0&&s->config[i].category==2&&s->config[i].field==field)return s->config[i].value;return 256;}
static lt48_status st7_options_read(st7_session*s,unsigned char v[3]){
 s->mode=8;s->address=0;memset(s->buffers,0,sizeof s->buffers);lt48_status st=st7_run(s,0);if(!st)st=st7_run(s,2);if(!st)st=st7_run(s,9);s->reserved=s->buffers[11][0];if(!st)st=st7_run(s,3);if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 unsigned fields[]={2,1,4};for(unsigned i=0;!st&&i<s->r->profile->option_bytes;i++){unsigned value=st7_option(s,fields[i]);if(value>255)st=LT48_FORMAT_ERROR;else v[i]=(unsigned char)value;}return st;
}
static lt48_status st7_options_write(st7_session*s,const unsigned char v[3]){
 const unsigned fields[]={2,1,4};s->config_count=s->r->profile->option_bytes;for(size_t i=0;i<s->config_count;i++)s->config[i]=(lt48_runtime_configuration_value){0,2,fields[i],v[i]};
 s->mode=8;memset(s->buffers,0,sizeof s->buffers);lt48_status st=st7_run(s,0);if(!st)st=st7_run(s,2);if(!st)st=st7_run(s,19);
 unsigned ack=s->r->profile->option_bytes==3?0x55:1;if(!st&&(s->buffers[30][0]!=ack||(s->r->profile->option_bytes==3&&s->buffers[31][0]!=ack)))st=LT48_DEVICE_MISMATCH;
 if(!st)st=st7_run(s,3);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 unsigned char after[3]={0};
 if(!st)st=st7_options_read(s,after);
 if(!st&&memcmp(after,v,s->r->profile->option_bytes))st=LT48_DEVICE_MISMATCH;
 return st;
}
static lt48_status st7_cycle(st7_session*s,int writing,unsigned voltage,const unsigned char*image,unsigned char*read){
 s->mode=writing?2:1;s->address=0;memset(s->buffers,0,sizeof s->buffers);memset(s->buffers[2],255,32);
 for(unsigned i=0;i<s->r->profile->variables;i++)if(s->vars[i].kind==0&&s->vars[i].variable_id==0)s->vars[i].value=(uint16_t)voltage;
 if(!writing)memset(read,255,65536);
 lt48_status st=st7_run(s,0);if(!st)st=st7_run(s,2);
 if(!st&&(st7_option(s,1)&2))st=LT48_SAFETY_REJECTED;
 for(unsigned a=0;!st&&a<65536;a++){
  /* Audited source holes have no wire effects and return FF. The finite
   * generated region profile lets the common owner skip those no-op calls. */
  if(!st7_cell(s->r,a))continue;
  s->address=a;s->buffers[2][0]=writing?image[a]:255;st=st7_run(s,writing?5:4);
  if(!st&&!writing)read[a]=s->buffers[1][0];
 }
 /* Last source byte captures completion into BUF9, separate from data in1. */
 if(!st&&s->buffers[9][0]!=1)st=LT48_DEVICE_MISMATCH;
 if(!st){st=st7_run(s,3);if(!st)st=lt48_runtime_transport_session_flush(s->wire);}
 return st;
}
static lt48_status st7_compare(const lt48_st7_resources*r,const unsigned char*actual,const unsigned char*expected,lt48_small_eprom_result*out){
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<65536;a++)if(st7_cell(r,a)&&actual[a]!=(expected?expected[a]:255)){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
lt48_status lt48_st7_execute_candidate(const lt48_st7_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result*out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof*out);out->first_mismatch=UINT32_MAX;lt48_status st=lt48_st7_validate(r,op,start,n,image,size,margin,type);
 if(!st&&(!api||!api->write||!api->read))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==LT48_OPERATION_READ&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(!st&&op==LT48_OPERATION_CONFIGURATION&&(!read||read_size<r->profile->option_bytes))st=LT48_BUFFER_TOO_SMALL;
 if(st){out->operation_status=st;return st;}
 st7_session s={0};s.r=r;memcpy(s.vars,r->vars,sizeof s.vars);unsigned char*actual=malloc(65536),*erased=NULL;if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 for(unsigned i=0;!st&&i<4;i++){const lt48_wave_code*w=r->workers+i;st=lt48_owner_load_applet_internal(s.wire,w->load_address,w->data,w->data_size);}
 if(!st)st=lt48_owner_load_fpga_internal(s.wire,r->fpga,2778);
 if(!st)st=lt48_runtime_transport_session_flush(s.wire);
 unsigned delta=type?50:25;unsigned voltage=op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_LOW?500-delta:op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;
 unsigned char before[3]={0},target[3]={0};if(!st)st=st7_options_read(&s,before);memcpy(target,before,sizeof target);
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE;
 int option_write=op==LT48_OPERATION_SECURE||(op==LT48_OPERATION_CONFIGURATION&&image);
 /* Bit0 is irreversible write protection; never set or try to clear it.
  * Read protection is cleared only by explicit Erase, not a read/verify. */
 if(!st&&(writing||option_write)&&r->profile->option_bytes==3&&s.reserved!=255)st=LT48_SAFETY_REJECTED;
 if(!st&&(writing||option_write)&&(before[1]&1))st=LT48_SAFETY_REJECTED;
 if(!st&&option_write){
  if(op==LT48_OPERATION_SECURE)target[1]|=2;else {unsigned mask=image[0];if(mask&~((1u<<r->profile->option_bytes)-1u))st=LT48_INVALID_ARGUMENT;for(unsigned i=0;i<r->profile->option_bytes;i++)if(mask&(1u<<i))target[i]=image[i+1];}
  if((target[1]&1)||((before[1]&2)&&memcmp(target,before,r->profile->option_bytes)))st=LT48_SAFETY_REJECTED;
 }
 if(!st&&op==LT48_OPERATION_ERASE){target[1]&=(unsigned char)~2u;if(before[1]&2)st=st7_options_write(&s,target);}
 if(!st&&(before[1]&2)&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_CONFIGURATION)st=LT48_SAFETY_REJECTED;
 if(!st&&(op!=LT48_OPERATION_CONFIGURATION||option_write))st=st7_cycle(&s,0,voltage,NULL,actual);
 if(!st&&writing){
  if(op==LT48_OPERATION_ERASE){erased=malloc(65536);if(!erased)st=LT48_NO_MEMORY;else{memset(erased,255,65536);image=erased;}}
  if(!st)st=st7_cycle(&s,1,500,image,NULL);
  if(!st)st=st7_cycle(&s,0,500,NULL,actual);
  unsigned char after[3];if(!st)st=st7_options_read(&s,after);if(!st&&memcmp(after,target,r->profile->option_bytes))st=LT48_DEVICE_MISMATCH;
 }
 if(!st&&op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_CONFIGURATION&&op!=LT48_OPERATION_SECURE)st=st7_compare(r,actual,op==LT48_OPERATION_BLANK_CHECK?NULL:image,out);
 if(!st&&(writing||(op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL))){unsigned v[]={500-delta,500+delta};for(unsigned i=0;!st&&i<2;i++){st=st7_cycle(&s,0,v[i],NULL,actual);if(!st)st=st7_compare(r,actual,image,out);}}
 if(!st&&op==LT48_OPERATION_CONFIGURATION&&option_write&&!(target[1]&2)){erased=malloc(65536);if(!erased)st=LT48_NO_MEMORY;else memcpy(erased,actual,65536);}
 if(!st&&option_write&&memcmp(target,before,r->profile->option_bytes))st=st7_options_write(&s,target);
 if(!st&&op==LT48_OPERATION_CONFIGURATION&&erased){st=st7_cycle(&s,0,500,NULL,actual);if(!st)st=st7_compare(r,actual,erased,out);}
 out->operation_status=st;const unsigned clean[]={3,1};lt48_owner_finish_fpga_internal(api,ctx,&s.wire,st7_run,&s,clean,2,r->idle,r->idle_size,st,&out->cleanup_status,&out->cleanup_completed);if(!st)st=out->cleanup_status;
 if(!st){out->completed_bytes=n;if(op==LT48_OPERATION_READ)memcpy(read,actual,n);if(op==LT48_OPERATION_CONFIGURATION){if(!read||read_size<r->profile->option_bytes)st=LT48_BUFFER_TOO_SMALL;else memcpy(read,target,r->profile->option_bytes);}}free(erased);free(actual);return st;
}

int lt48_st7_admitted_internal(unsigned c,unsigned d){return LT48_ST7TWELVE_ADMITTED&&c==2014&&(d==0x7c||d==0x7d||d==0x7e||d==0x7f||d==0x80||d==0x83||d==0x84||d==0x85||d==0x86);}
unsigned lt48_st7_option_bytes(const lt48_st7_resources*r){return r?r->profile->option_bytes:0;}
lt48_status lt48_st7_execute(const lt48_st7_resources*r,const lt48_protocol_transport*a,void*c,lt48_operation_kind o,uint32_t start,size_t n,const unsigned char*i,size_t ni,unsigned char*b,size_t nb,lt48_verify_margin_selection m,unsigned mt,lt48_small_eprom_result*z){if(!lt48_st7_qualified(r))return LT48_UNSUPPORTED;return lt48_st7_execute_candidate(r,a,c,o,start,n,i,ni,b,nb,m,mt,z);}
