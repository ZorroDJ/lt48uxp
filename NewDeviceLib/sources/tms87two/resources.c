/* Finite TMS87 source loader and audited operation owner. */
#include "../../../c/src/tms87.h"
#include "../../../c/src/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admission.h"
#include "gpio.inc"
struct tms87_profile {const char*name;unsigned definition,family,pinmap,variables,capacity,operations[24];};
static const struct tms87_profile profiles[]={
#include "profiles.inc"
};
struct lt48_tms87_resources {
 lt48_rez*rez;lt48_pin_map*pins;lt48_compiled_wave*waves[26];
 lt48_compile_variable vars[6];lt48_runtime_frame_profile frame;
 lt48_wave_code workers[3];lt48_applet_address addresses[3];
 lt48_runtime_nested_wave nested[6];size_t nested_count;
 const unsigned char*fpga;unsigned char*idle;size_t idle_size;
 const struct tms87_profile*profile;
};
void lt48_tms87_close(lt48_tms87_resources*r){if(!r)return;for(unsigned i=0;i<26;i++)lt48_compiled_wave_close(r->waves[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);}
lt48_status lt48_tms87_open(const lt48_installation*x,size_t device,lt48_tms87_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);
 if(st)return st;
 if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10")||s.catalogue.device_class!=2000||!s.catalogue.vendor||strcmp(s.catalogue.vendor,"TI")||strcmp(s.definition_stem,"GA_____")||s.element_bits!=8||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 const struct tms87_profile*p=NULL;for(size_t i=0;i<sizeof profiles/sizeof*profiles;i++){const struct tms87_profile*q=profiles+i;
 if(s.definition_resource==q->definition&&s.family_code==q->family&&s.pinmap==q->pinmap&&s.catalogue.device_name_length==strlen(q->name)&&!memcmp(s.catalogue.device_name,q->name,s.catalogue.device_name_length))p=q;}if(!p||s.capacity_bytes!=p->capacity)return LT48_UNSUPPORTED;
 lt48_tms87_resources*r=calloc(1,sizeof*r);
 if(!r)return LT48_NO_MEMORY;
 r->profile=p;char path[4096],name[32];size_t ix,fi;int n=snprintf(path,sizeof path,"%s/GA_____.rez",lt48_installation_path(x));
 if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(e) do{st=(e);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"a286d6799117d4e188067376cab75843fbb9299f4990091d91ad27d8abab99ad",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pin;TRY(lt48_rez_find_resource(r->rez,name,&ix));TRY(lt48_rez_get_pin_code(r->rez,ix,&pin));TRY(lt48_pin_map_parse(&pin,&r->pins));snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));TRY(lt48_rez_get_family_frame_profile(r->rez,fi,&r->frame));
 if(f.operation_count!=10||f.variable_count!=p->variables||p->variables>6){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<p->variables;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned seen=0,wc=0;for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
 if(op.operation_id>=24||p->operations[op.operation_id]!=op.wave||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ix));TRY(lt48_rez_get_wave_code(r->rez,ix,&w));
  if(w.wave_type==6){if(wc==3||op.operation_id<16||op.operation_id>18||w.load_address<0x5100||w.load_address+w.data_size>0x5700||w.length1!=w.data_size+4||w.length2!=w.data_size){st=LT48_FORMAT_ERROR;goto fail;}r->workers[wc]=w;r->addresses[wc++]=(lt48_applet_address){op.operation_id,w.load_address};}
  else if(w.wave_type==4&&op.operation_id==21&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a)r->fpga=w.data+2;
  else if(!w.wave_type){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,p->variables,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->waves[op.operation_id]);lt48_compact_wave_close(c);
 if(st)goto fail;
 r->nested[r->nested_count++]=(lt48_runtime_nested_wave){op.operation_id,r->waves[op.operation_id]};}
  else{st=LT48_FORMAT_ERROR;goto fail;}
 }
 if(seen!=0x2702c7||wc!=3||!r->fpga||r->nested_count!=6){st=LT48_FORMAT_ERROR;goto fail;}
 {const unsigned char*raw[]={tms87_gpio_init,tms87_gpio_word,tms87_gpio_reset};const size_t sizes[]={sizeof tms87_gpio_init,sizeof tms87_gpio_word,sizeof tms87_gpio_reset};for(unsigned i=0;i<3;i++){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(raw[i],sizes[i],&c));st=lt48_compact_compile(c,r->pins,r->vars,p->variables,1,0,&r->waves[22+i]);lt48_compact_wave_close(c);if(st)goto fail;}}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_tms87_close(r);return st;
#undef TRY
}
unsigned lt48_tms87_qualified(const lt48_tms87_resources*r){return r&&LT48_TMS87TWO_ADMITTED;}
static int tms87_cell(const lt48_tms87_resources*r,unsigned a){return a<r->profile->capacity;}
static unsigned tms87_blank(const lt48_tms87_resources*r,unsigned a){(void)r;(void)a;return 255;}
lt48_status lt48_tms87_validate(const lt48_tms87_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op<LT48_OPERATION_READ||op>LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(start||n!=r->profile->capacity)return LT48_SAFETY_REJECTED;
 if(type>1||(unsigned)margin>LT48_VERIFY_MARGIN_ALL)return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||size!=n))return LT48_INVALID_ARGUMENT;
 if((image&&size!=n)||(!image&&size))return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned a=0;a<r->profile->capacity;a++)if(!tms87_cell(r,a)&&image[a]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
typedef struct{const lt48_tms87_resources*r;lt48_runtime_transport_session*wire;lt48_compile_variable vars[6];unsigned char buffers[32][32];uint32_t volts[4];unsigned mode,address,read_active;const unsigned char*retry_before;} tms87_session;
static lt48_status tms87_run(void*ctx,unsigned op){
 tms87_session*s=ctx;
 if(op==6&&s->mode==2)op=24;
 if(op>=26||!s->r->waves[op])return LT48_UNSUPPORTED;
 lt48_runtime_buffer_seed seeds[30];size_t count=0;for(unsigned i=0;i<32;i++)if(i!=1&&i!=7)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 lt48_runtime_options o={0};o.initial_buffers=seeds;o.initial_buffer_count=count;o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.mode_buffer=7;o.address_buffer=1;o.parameter_buffer_id=3;o.root_timeout_seconds=120;o.nested_waves=s->r->nested;o.nested_wave_count=s->r->nested_count;memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 size_t cc=0;return lt48_owner_run_mutable_internal(s->wire,s->r->waves[op],s->r->pins,s->vars,s->r->profile->variables,&o,s->r->addresses,3,s->buffers,NULL,0,&cc,s->volts);
}
static lt48_status tms87_cycle(tms87_session*s,int writing,unsigned voltage,const unsigned char*image,unsigned char*read){
 /* Start each independent Read or Program pass with source variables. */
 memcpy(s->vars,s->r->vars,sizeof s->vars);s->mode=writing?2:1;s->address=0;memset(s->buffers,0,sizeof s->buffers);memset(s->buffers[3],255,32);
 for(unsigned i=0;i<s->r->profile->variables;i++)if(s->vars[i].kind==0&&s->vars[i].variable_id==0)s->vars[i].value=(uint16_t)voltage;
 if(!writing)memset(read,255,s->r->profile->capacity);
 lt48_status st=lt48_owner_load_fpga_internal(s->wire,writing?s->r->idle:s->r->fpga,2778);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 if(!st)st=tms87_run(s,0);
 if(!st)st=tms87_run(s,writing?22:2);
 if(!st&&!writing)s->read_active=1;
 for(unsigned a=0;!st&&a<s->r->profile->capacity;a+=writing?2:1){
  s->address=a;
  if(writing&&s->retry_before&&s->retry_before[a]==image[a]&&s->retry_before[a+1]==image[a+1])continue;
  if(writing){unsigned odd=a+1;for(unsigned k=0;k<4;k++)s->buffers[5][k]=(unsigned char)(odd>>(8*k));s->buffers[4][0]=image[a];s->buffers[3][0]=image[odd];
   for(unsigned attempt=0;!st&&attempt<10;attempt++){st=tms87_run(s,23);if(!st&&s->buffers[6][0]==image[a]&&s->buffers[10][0]==image[odd])break;if(!st&&attempt==9)st=LT48_DEVICE_MISMATCH;}
  }
  else{st=tms87_run(s,7);if(!st)read[a]=s->buffers[2][0];}
 }
 if(!st)st=tms87_run(s,6);
 if(!st)s->read_active=0;
 if(!st&&!writing&&s->buffers[8][0]!=1)st=LT48_DEVICE_MISMATCH;
 if(!st)st=tms87_run(s,1);
 if(!st)st=lt48_runtime_transport_session_flush(s->wire);
 return st;
}
static lt48_status tms87_compare(const lt48_tms87_resources*r,const unsigned char*a,const unsigned char*e,lt48_small_eprom_result*out){
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned k=0;k<r->profile->capacity;k++)if(tms87_cell(r,k)&&a[k]!=(e?e[k]:tms87_blank(r,k))){if(!out->mismatch_count)out->first_mismatch=k;out->mismatch_count++;}
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
lt48_status lt48_tms87_execute_candidate(const lt48_tms87_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result*out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof*out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_tms87_validate(r,op,start,n,image,size,margin,type);
 if(!st&&(!api||!api->write||!api->read))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==LT48_OPERATION_READ&&(!read||read_size<n))st=LT48_BUFFER_TOO_SMALL;
 if(st){out->operation_status=st;return st;}
 unsigned char*actual=malloc(n);
 if(!actual){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 tms87_session s={0};s.r=r;memcpy(s.vars,r->vars,sizeof s.vars);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 for(unsigned i=0;!st&&i<3;i++)st=lt48_owner_load_applet_internal(s.wire,r->workers[i].load_address,r->workers[i].data,r->workers[i].data_size);
 if(!st)st=lt48_owner_load_fpga_internal(s.wire,r->fpga,2778);
 if(!st)st=lt48_runtime_transport_session_flush(s.wire);
 unsigned delta=type?50:25;unsigned voltage=op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_LOW?500-delta:op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;
 if(!st)st=tms87_cycle(&s,0,voltage,NULL,actual);
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 if(!st&&writing){
  /* EPROM must be blank for Program; Over Program permits only 1-to-0. */
  for(unsigned a=0;!st&&a<r->profile->capacity;a++)if(op==LT48_OPERATION_PROGRAM?actual[a]!=255:(actual[a]&image[a])!=image[a])st=LT48_SAFETY_REJECTED;
  s.retry_before=actual;
  if(!st)st=tms87_cycle(&s,1,500,image,NULL);
  s.retry_before=NULL;
  if(!st)st=tms87_cycle(&s,0,500,NULL,actual);
 }
 if(!st&&op!=LT48_OPERATION_READ)st=tms87_compare(r,actual,op==LT48_OPERATION_BLANK_CHECK?NULL:image,out);
 if(!st&&(writing||(op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL))){unsigned v[]={500-delta,500+delta};for(unsigned i=0;!st&&i<2;i++){st=tms87_cycle(&s,0,v[i],NULL,actual);
 if(!st)st=tms87_compare(r,actual,image,out);}}
 /* Recovery stops the reader without requesting another data capture. */
 if(s.mode==1)s.mode=0;
 out->operation_status=st;const unsigned clean[]={6,1};lt48_owner_finish_fpga_internal(api,ctx,&s.wire,tms87_run,&s,clean,2,r->idle,r->idle_size,st,&out->cleanup_status,&out->cleanup_completed);
 if(!st)st=out->cleanup_status;
 if(!st){out->completed_bytes=n;
 if(op==LT48_OPERATION_READ)memcpy(read,actual,n);}free(actual);return st;
}
int lt48_tms87_admitted_internal(unsigned c,unsigned d){return LT48_TMS87TWO_ADMITTED&&c==2000&&(d==0x11c||d==0x11d);}
lt48_status lt48_tms87_execute(const lt48_tms87_resources*r,const lt48_protocol_transport*a,void*c,lt48_operation_kind o,uint32_t start,size_t n,const unsigned char*i,size_t ni,unsigned char*b,size_t nb,lt48_verify_margin_selection m,unsigned mt,lt48_small_eprom_result*z){if(!lt48_tms87_qualified(r))return LT48_UNSUPPORTED;return lt48_tms87_execute_candidate(r,a,c,o,start,n,i,ni,b,nb,m,mt,z);}
