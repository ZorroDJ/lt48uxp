#include "lpc_flash.h"
#include "internal.h"
#include "../../NewDeviceLib/sources/lpc-flash/profiles.h"
#include "../../NewDeviceLib/sources/pic-common/source_verify_voltage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void lt48_lpcflash_close(lt48_lpcflash_resources*r){if(!r)return;
 for(unsigned i=0;i<24;i++)lt48_compiled_wave_close(r->wave[i]);
 lt48_pin_map_close(r->pins);
 lt48_rez_close(r->rez);
 free(r->idle);
 free(r);
 }
 
int lt48_lpcflash_has_isp_choice(const lt48_lpcflash_resources*r){return r&&!(r->family>=94&&r->family<=106);}
unsigned lt48_lpcflash_field_max(const lt48_lpcflash_resources*r,unsigned f){if(!r||f>=r->fields)return 0;return f>=4?7:r->family==88&&f==2?31:r->family==88&&f==3?1:255;}
unsigned lt48_lpcflash_field_count(const lt48_lpcflash_resources*r){return r?r->fields:0;
 }
 
lt48_status lt48_lpcflash_set_erase(lt48_lpcflash_resources*r,unsigned mask,int preserve){if(!r||!mask||mask>>r->sectors||preserve<0||preserve>1)return LT48_INVALID_ARGUMENT;
 if(!lt48_lpcflash_has_isp_choice(r)&&(preserve||mask!=((1u<<r->sectors)-1)))return LT48_UNSUPPORTED;
 r->sector_mask=mask;
 r->preserve_isp=(unsigned)preserve;
 return LT48_OK;
 }
 
lt48_status lt48_lpcflash_open(const lt48_installation*x,size_t index,lt48_lpcflash_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec s;
 lt48_status st=lt48_installation_resolve_device(x,index,&s);
 if(st)return st;
 
 if(s.catalogue.device_class!=2010||strcmp(s.definition_stem,"GAB____")||strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;
 
 char key[256];
 snprintf(key,sizeof key,"%s:%.*s",s.catalogue.vendor,(int)s.catalogue.device_name_length,s.catalogue.device_name);
 const struct lpcflash_binding*b=NULL;
 
 for(size_t i=0;i<sizeof lpcflash_bindings/sizeof *lpcflash_bindings;i++)if(!strcmp(key,lpcflash_bindings[i].key))b=&lpcflash_bindings[i];
 if(!b)return LT48_UNSUPPORTED;
 
 if(s.definition_resource!=b->definition||s.family_code!=b->family||s.pinmap!=b->pinmap||s.pin_count!=b->pins||s.capacity_bytes!=b->capacity||s.element_bits!=8)return LT48_SAFETY_REJECTED;
 
 const struct lpcflash_family*f=NULL;
 for(size_t i=0;i<sizeof lpcflash_families/sizeof *lpcflash_families;i++)if(lpcflash_families[i].family==b->family)f=&lpcflash_families[i];
 if(!f)return LT48_UNSUPPORTED;
 
 lt48_lpcflash_resources*r=calloc(1,sizeof *r);
 if(!r)return LT48_NO_MEMORY;
 r->definition=b->definition;
 r->family=b->family;
 r->pinmap=b->pinmap;
 r->capacity=b->capacity;
 r->fields=f->fields;
 r->sectors=(f->fields-4);
 r->sector_mask=(1u<<r->sectors)-1;
 
 char path[4096],name[24];
 size_t ri,fi;
 int n=snprintf(path,sizeof path,"%s/GAB____.rez",lt48_installation_path(x));
 if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;
 goto fail;
 }
 
#define TRY(v) do { st=(v); if(st)goto fail; } while(0)
 TRY(lt48_owner_rez_open_internal(path,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9",&r->rez));
 
 lt48_pin_code pc;
 snprintf(name,sizeof name,"P%x",r->pinmap);
 TRY(lt48_rez_find_resource(r->rez,name,&ri));
 TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));
 TRY(lt48_pin_map_parse(&pc,&r->pins));
 
 lt48_family_info family;
 snprintf(name,sizeof name,"M%x",r->family);
 TRY(lt48_rez_find_resource(r->rez,name,&fi));
 TRY(lt48_rez_get_family_info(r->rez,fi,&family));
 if(family.variable_count>16||family.operation_count!=10){st=LT48_FORMAT_ERROR;
 goto fail;
 }
 r->var_count=family.variable_count;
 
 for(size_t i=0;i<r->var_count;i++){lt48_family_variable v;
 TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
 r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]}
 ;
 if(v.values[1]==0)r->nominal=v.values[3];
 }
 
 static const unsigned ids[]={0,1,2,3,4,5,6,7,22,23}
 ;
 unsigned seen=0;
 
 for(unsigned i=0;i<10;i++){lt48_family_operation op;
 TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
 unsigned j=0;
 for(;j<10&&ids[j]!=op.operation_id;j++);
 if(j==10||(seen&(1u<<j))||op.wave!=f->ops[j]){st=LT48_FORMAT_ERROR;
 goto fail;
 }
 seen|=1u<<j;
 
  lt48_wave_code w;
 snprintf(name,sizeof name,"W%x",op.wave);
 TRY(lt48_rez_find_resource(r->rez,name,&ri));
 TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(w.wave_type){st=LT48_FORMAT_ERROR;
 goto fail;
 }
 
  lt48_compact_wave*c=NULL;
 TRY(lt48_compact_decode(w.data,w.data_size,&c));
 st=lt48_compact_compile(c,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);
 lt48_compact_wave_close(c);
 if(st)goto fail;
 if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;
 goto fail;
 }
 }
 
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
 *out=r;
 return LT48_OK;
 
fail:lt48_lpcflash_close(r);
 return st;
 
#undef TRY
}
 
lt48_status lt48_lpcflash_validate(const lt48_lpcflash_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*input,size_t size){
 if(!r||(!input&&size))return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_READ||op==LT48_OPERATION_MEMORY_PROTECT||op<2||op>10)return LT48_UNSUPPORTED;
 
 /* Sector CRC comparison cannot establish a partial-byte verification. */
 if(start||n!=r->capacity)return LT48_SAFETY_REJECTED;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)&&(r->preserve_isp||r->sector_mask!=((1u<<r->sectors)-1)))return LT48_SAFETY_REJECTED;
 if(op==LT48_OPERATION_ERASE&&!r->preserve_isp&&r->sector_mask!=((1u<<r->sectors)-1))return LT48_SAFETY_REJECTED;
 
 if(op>=3&&op<=6&&(!input||size!=n))return LT48_INVALID_ARGUMENT;
 
 if(op==LT48_OPERATION_SECURE&&input&&(size!=1||!input[0]||input[0]>7))return LT48_INVALID_ARGUMENT;
 
 if(op==LT48_OPERATION_CONFIGURATION&&input){if(size!=24)return LT48_INVALID_ARGUMENT;
 uint32_t mask=0;
 for(unsigned i=0;i<4;i++)mask|=(uint32_t)input[i]<<(8*i);
 if(mask>>r->fields)return LT48_INVALID_ARGUMENT;
 if(r->family==88&&(((mask&4)&&input[6]>31)||((mask&8)&&input[7]>1)))return LT48_INVALID_ARGUMENT;
 for(unsigned i=4;i<r->fields;i++)if((mask&(1u<<i))&&input[4+i]>7)return LT48_INVALID_ARGUMENT;
 }
 
 return LT48_OK;
 
}
 
typedef struct lf_state{const lt48_lpcflash_resources*r;
 lt48_runtime_transport_session*wire;
 unsigned char buffers[76][32];
 lt48_runtime_configuration_value cfg[64];
 size_t nc;
 lt48_compile_variable vars[16];
 uint32_t volts[4];
 unsigned mode;
 const unsigned char*image;
 lt48_m37_id_decision_fn decide;
 void*decision_context;
 int ignore;
 }
 lf_state;
 
static void put32(unsigned char*p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));
 }
 
static unsigned get32(const unsigned char*p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;
 }
 
static void config(lf_state*s,int obj,unsigned cat,unsigned field,unsigned value){for(size_t i=0;i<s->nc;i++)if(s->cfg[i].object_id==obj&&s->cfg[i].category==cat&&s->cfg[i].field==field){s->cfg[i].value=value;
 return;
 }
 s->cfg[s->nc++]=(lt48_runtime_configuration_value){obj,(uint16_t)cat,(uint16_t)field,value}
 ;
 }
 
static unsigned getconfig(lf_state*s,unsigned field){for(size_t i=0;i<s->nc;i++)if(s->cfg[i].object_id==-2&&s->cfg[i].category==2&&s->cfg[i].field==field)return s->cfg[i].value;
 return UINT32_MAX;
 }
 
static lt48_status step(lf_state*s,unsigned op){
 lt48_runtime_buffer_seed seeds[75];
 size_t ns=0;
 for(unsigned i=0;i<76;i++)if(i!=4)seeds[ns++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32}
 ;
 lt48_runtime_nested_wave nested[24];
 size_t nn=0;
 for(unsigned i=0;i<24;i++)if(s->r->wave[i])nested[nn++]=(lt48_runtime_nested_wave){i,s->r->wave[i]}
 ;
 
 lt48_runtime_options o={0}
 ;
 o.host_avr_parallel=1;
 o.mode=s->mode;
 o.use_explicit_buffer_ids=1;
 o.address_buffer=0;
 o.mode_buffer=4;
 o.parameter_buffer_id=1;
 o.initial_buffers=seeds;
 o.initial_buffer_count=ns;
 o.nested_waves=nested;
 o.nested_wave_count=nn;
 o.configuration=s->cfg;
 o.configuration_count=s->nc;
 o.use_explicit_direct_configuration_object=1;
 o.direct_configuration_object=-2;
 o.root_timeout_seconds=10;
 memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 o.device_image=s->image;
 o.device_image_size=s->image?s->r->capacity:0;
 o.device_element_bytes=1;
 
 lt48_runtime_machine*m=NULL;
 lt48_runtime_result*result=NULL;
 lt48_status st=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&o,&m);
 if(!st)st=lt48_runtime_transport_session_run(s->wire,m,&result);
 
 if(!st){const unsigned char*p;
 size_t n;
 const lt48_runtime_configuration_value*c;
 for(unsigned i=0;!st&&i<76;i++){st=lt48_runtime_get_buffer(result,i,&p,&n);
 if(!st&&n==32)memcpy(s->buffers[i],p,32);
 else if(!st)st=LT48_RUNTIME_ERROR;
 }
 if(!st)st=lt48_runtime_get_configuration(result,&c,&n);
 if(!st&&n>64)st=LT48_RUNTIME_ERROR;
 if(!st){memcpy(s->cfg,c,n*sizeof *c);
 s->nc=n;
 }
 for(unsigned i=0;!st&&i<4;i++)st=lt48_runtime_get_voltage(result,i,&s->volts[i]);
 for(size_t i=0;!st&&i<lt48_runtime_event_count(result);i++){lt48_runtime_event e;
 st=lt48_runtime_get_event(result,i,&e);
 if(!st&&(e.kind==LT48_RUNTIME_RESULT_LATCH||(e.kind==LT48_RUNTIME_RETURN_STATUS&&!e.arguments[0])))st=LT48_DEVICE_MISMATCH;
 }
 }
 
 lt48_runtime_result_close(result);
 lt48_runtime_machine_close(m);
 return st;
 
}
 
static lt48_status end(lf_state*s,lt48_status st){lt48_status a=step(s,3),b=step(s,1);
 return st?st:a?a:b;
 }
 
static lt48_status start(lf_state*s,unsigned mode,unsigned volts){memset(s->buffers,0,sizeof s->buffers);
 s->mode=mode;
 for(size_t i=0;i<s->r->var_count;i++)if(!s->vars[i].variable_id)s->vars[i].value=volts;
 lt48_status st=step(s,0);
 if(!st)st=step(s,2);
 return st;
 }
 
static lt48_status settings(lf_state*s,unsigned char*values,int write){
 if(write)for(unsigned i=0;i<s->r->fields;i++)config(s,-2,2,i,values[i]);
 lt48_status st=start(s,8,s->r->nominal);
 if(!st)st=step(s,write?7:23);
 
 if(!st)for(unsigned i=0;i<s->r->fields;i++){unsigned v=getconfig(s,i);
 if(v>255||(i>=4&&v>7)){st=LT48_DEVICE_MISMATCH;
 break;
 }
 values[i]=(unsigned char)v;
 }
 return end(s,st);
 
}
 
static lt48_status check_id(lf_state*s){for(unsigned retry=0;retry<64;retry++){lt48_status st=start(s,8,s->r->nominal);
 if(!st)st=step(s,22);
 st=end(s,st);
 if(st!=LT48_DEVICE_MISMATCH||!s->decide||s->ignore)return st;
 unsigned actual=get32(s->buffers[6]);
 unsigned expected=0;
 for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==40)expected=s->vars[i].value;
 lt48_m37_id_choice c=s->decide(s->decision_context,0x15dd,(uint16_t)expected,0,(uint16_t)actual);
 if(c==LT48_M37_ID_ABORT)return st;
 if(c==LT48_M37_ID_IGNORE){s->ignore=1;
 return LT48_OK;
 }
 if(c!=LT48_M37_ID_RETRY)return LT48_INVALID_ARGUMENT;
 }
 return LT48_DEVICE_MISMATCH;
 }
 
static lt48_status scan(lf_state*s,const unsigned char*image,unsigned voltage,int keep_going,lt48_serial_page_result*out){s->image=image;
 for(unsigned i=0;i<s->r->sectors;i++)config(s,-2,0,i,1);
 unsigned stride=s->r->capacity/s->r->sectors;
 lt48_status st=start(s,4,voltage);
 for(unsigned a=0;!st&&a<s->r->capacity;a+=stride){put32(s->buffers[0],a);
 s->buffers[2][0]=image[a];
 st=step(s,4);
 if(!st&&get32(s->buffers[9])!=a+stride)st=LT48_RUNTIME_ERROR;
 if(!st&&s->buffers[1][0]!=s->buffers[2][0]){if(!out->mismatch_count)out->first_mismatch=a;
 out->mismatch_count++;
 if(!keep_going)st=LT48_DEVICE_MISMATCH;
 }
 }
 st=end(s,st);
 s->image=NULL;
 return !st&&out->mismatch_count?LT48_DEVICE_MISMATCH:st;
 }
 
static lt48_status cleanup(void*v,unsigned phase){lf_state*s=v;
 return phase==0?step(s,3):phase==1?step(s,1):lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
 }
 
lt48_status lt48_lpcflash_execute(const lt48_lpcflash_resources*r,const lt48_protocol_transport*api,void*context,lt48_operation_kind op,uint32_t start_address,size_t n,const unsigned char*input,size_t size,unsigned char*read,size_t read_capacity,lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void*decision_context,lt48_serial_page_result*out){
 if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);
 out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_lpcflash_validate(r,op,start_address,n,input,size);
 if(!st&&(margin_type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
 if(!st&&op==LT48_OPERATION_CONFIGURATION&&(!read||read_capacity<r->fields))st=LT48_BUFFER_TOO_SMALL;
 if(st){out->operation_status=st;
 return st;
 }
 
 if(api->cancelled&&api->cancelled(context)){out->operation_status=LT48_CANCELLED;
 out->cleanup_completed=1;
 return LT48_CANCELLED;
 }
 
 lf_state s={0}
 ;
 s.r=r;
 s.decide=decide;
 s.decision_context=decision_context;
 memcpy(s.vars,r->vars,sizeof s.vars);
 config(&s,-1,0,1,0);
 config(&s,-2,1,0,r->preserve_isp);
 for(unsigned i=0;i<r->sectors;i++)config(&s,-2,0,i,!!(r->sector_mask&(1u<<i)));
 for(unsigned i=0;i<r->fields;i++)config(&s,-2,2,i,0);
 
 unsigned char saved[20]={0}
 ,want[20]={0}
 ,actual[20]={0}
 ;
 unsigned char*blank=malloc(r->capacity);
 if(!blank){out->operation_status=LT48_NO_MEMORY;
 return LT48_NO_MEMORY;
 }
 memset(blank,255,r->capacity);
 
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 int started=!st;
 if(!st)st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
 if(!st)st=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
 const unsigned char busy[]={0x56,0x0a,1,0x40,1}
 ;
 if(!st)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof busy);
 
 if(!st)st=check_id(&s);
 if(!st)st=settings(&s,saved,0);
 memcpy(want,saved,r->fields);
 if(!st&&op==LT48_OPERATION_ERASE&&r->preserve_isp)for(unsigned i=4;i<r->fields;i++)if((r->sector_mask&(1u<<(i-4)))&&(saved[i]&(i==r->fields-1?6u:4u))){st=LT48_SAFETY_REJECTED;break;}

 /* CRC_S is invalid under MOVCDIS; refuse protected comparisons instead of
  * mistaking an invalid target CRC for an array result. Program first erases. */
 if(!st&&(op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_OVER_PROGRAM))for(unsigned i=4;i<r->fields;i++)if((saved[i]&1)||(op==LT48_OPERATION_OVER_PROGRAM&&(saved[i]&6))){st=LT48_SAFETY_REJECTED;break;}

 
 if(!st&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE)){st=start(&s,7,r->nominal);
 if(!st)st=step(&s,6);
 st=end(&s,st);
 if(!st)st=settings(&s,actual,0);
 if(!st&&memcmp(saved,actual,4))st=LT48_DEVICE_MISMATCH;
 for(unsigned i=4;!st&&i<r->fields;i++){if((r->sector_mask&(1u<<(i-4)))&&(!r->preserve_isp||i<r->fields-1))want[i]=0;
 if(actual[i]!=want[i])st=LT48_DEVICE_MISMATCH;
 }
 if(!st&&!r->preserve_isp&&r->sector_mask==((1u<<r->sectors)-1))st=scan(&s,blank,r->nominal,0,out);
 }
 
 if(!st&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)){
  if(r->preserve_isp||r->sector_mask!=((1u<<r->sectors)-1))st=LT48_SAFETY_REJECTED;
 
  s.image=input;
 for(unsigned i=0;i<r->sectors;i++)config(&s,-2,0,i,1);
 unsigned stride=r->capacity/r->sectors;
 if(!st)st=start(&s,2,r->nominal);
 for(unsigned a=0;!st&&a<r->capacity;a+=stride){put32(s.buffers[0],a);
 s.buffers[2][0]=input[a];
 st=step(&s,5);
 if(!st&&get32(s.buffers[9])!=a+stride)st=LT48_RUNTIME_ERROR;
 }
 st=end(&s,st);
 s.image=NULL;
 
 }
 
 uint32_t volts[4]={0}
 ;
 unsigned nv=0;
 if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,margin_type,0,volts,&nv);
 
 if(!st&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_BLANK_CHECK)){unsigned passes=(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||margin==LT48_VERIFY_MARGIN_ALL)?2:1;
 unsigned v=margin==LT48_VERIFY_MARGIN_LOW?volts[0]:margin==LT48_VERIFY_MARGIN_HIGH?volts[1]:r->nominal;
 for(unsigned i=0;!st&&i<passes;i++)st=scan(&s,op==LT48_OPERATION_BLANK_CHECK?blank:input,passes==2?volts[i]:v,op==LT48_OPERATION_COMPARE,out);
 }
 
 if(!st&&(op==LT48_OPERATION_CONFIGURATION||op==LT48_OPERATION_SECURE)){
  if(op==LT48_OPERATION_SECURE){unsigned bits=input?input[0]:7;
 for(unsigned i=4;i<r->fields;i++)want[i]|=bits;
 }
 
  else if(input){unsigned mask=get32(input);
 for(unsigned i=0;i<r->fields;i++)if(mask&(1u<<i))want[i]=input[4+i];
 }
 
  for(unsigned i=4;!st&&i<r->fields;i++)if((saved[i]&want[i])!=saved[i])st=LT48_SAFETY_REJECTED;
 
  if(!st&&memcmp(saved,want,r->fields))st=settings(&s,want,1);
 
 }
 
 if(!st)st=settings(&s,actual,0);
 if(!st&&memcmp(want,actual,r->fields))st=LT48_DEVICE_MISMATCH;
 
 if(!st&&api->cancelled&&api->cancelled(context))st=LT48_CANCELLED;
 out->operation_status=st;
 
 if(started){const unsigned phases[]={0,1,2}
 ;
 st=lt48_owner_finish_internal(api,context,&s.wire,cleanup,&s,phases,3,st,&out->cleanup_status,&out->cleanup_completed);
 }
 else out->cleanup_completed=1;
 
 if(!st){if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,actual,r->fields);
 out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?r->fields:n;
 }
 free(blank);
 return st;
 
}
 
