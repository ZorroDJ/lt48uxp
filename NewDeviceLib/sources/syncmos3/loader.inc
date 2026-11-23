#include "profiles.h"
#include "admission.h"
/* Preserve original zero lookups only at pinned source sites. Logical7
 * ramp/select commands still update the global VPPM rail; never discard them. */
static int sm3_defaults(unsigned op,const lt48_compiled_wave*w){
 unsigned count=op==2||op==10||op==11?1:op==3?2:0;
 if(lt48_compiled_wave_defaulted_pin_count(w)!=count)return 0;
 for(unsigned i=0;i<count;i++){lt48_defaulted_logical_pin p;unsigned logical=op==2?8:7,offset=op==2?0x0d:op==3?(i?0x2d:0x09):op==10?0x87:0x3b,opcode=op==2?0x11:op==3&&i?0x04:0x27;
  if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)||p.logical_pin!=logical||p.compact_offset!=offset||p.compact_opcode!=opcode||p.classification!=(op==2?LT48_DEFAULTED_PIN_RESIDENT_NOOP:LT48_DEFAULTED_PIN_UNRESOLVED))return 0;
 }
 return 1;
}
static const struct sm3_profile*sm3_find(unsigned cls,unsigned d){unsigned kind=cls==2038?32:0;for(size_t i=0;i<sizeof sm3_profiles/sizeof*sm3_profiles;i++)if(sm3_profiles[i].kind==kind&&sm3_profiles[i].definition==d)return sm3_profiles+i;return NULL;}
unsigned lt48_sm3_capacity_internal(unsigned cls,unsigned d,unsigned family,unsigned pins){const struct sm3_profile*p=sm3_find(cls,d);return p&&p->family==family&&p->pinmap==pins?p->capacity:0;}
int lt48_sm3_admitted_internal(unsigned cls,unsigned d){return sm3_find(cls,d)&&LT48_SM3_ADMITTED;}
lt48_status lt48_sm3_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out){return LT48_INVALID_ARGUMENT;}
 *out=NULL;
 lt48_catalogue_entry e;lt48_status st=lt48_installation_get_device(x,index,&e);if(st)return st;const struct sm3_profile*p=sm3_find(e.device_class,e.device_type);if(!p)return LT48_UNSUPPORTED;
 lt48_device_spec s;st=lt48_installation_resolve_device(x,index,&s);if(st)return st;unsigned found=0;for(size_t i=0;i<sizeof sm3_members/sizeof*sm3_members;i++){const char*n=sm3_members[i].name;if(sm3_members[i].kind==p->kind&&sm3_members[i].definition==p->definition&&!strcmp(s.catalogue.vendor,sm3_members[i].vendor)&&strlen(n)==s.catalogue.device_name_length&&!memcmp(n,s.catalogue.device_name,s.catalogue.device_name_length))found=1;}
 if(!found||s.family_code!=p->family||s.pinmap!=p->pinmap||s.capacity_bytes!=p->capacity||s.pin_count!=p->pins||s.element_bits!=8)return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->kind=p->kind;r->definition=p->definition;r->family=p->family;r->pinmap=p->pinmap;r->capacity=p->capacity;r->fields=p->fields;r->configuration_id=p->configuration;r->configuration_lock=0;r->nominal=p->nominal;r->regions=p->regions;r->n79_xp=p->kind==32;r->mf74_page=p->page;r->flash=p->length[0];
 memcpy(r->region_length,p->length,sizeof p->length);memcpy(r->region_bank,p->logical,sizeof p->logical);memcpy(r->n79_physical,p->physical,sizeof p->physical);
 unsigned count;const struct sm3_field*schema=sm3_schema(p->configuration,&count);if(count!=p->fields){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<count;i++){r->configuration_category[i]=schema[i].category;r->configuration_field[i]=schema[i].field;r->configuration_maximum[i]=schema[i].maximum;}
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/%s",lt48_installation_path(x),p->kind==32?"GDAE___.rez":"GDCC___.rez");
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,p->kind==32?"9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36":"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));TRY(lt48_pin_map_complete_groups_internal(r->pins,36));
 snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=p->variables||f.operation_count!=p->operations||f.configuration_set!=p->configuration){st=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=f.variable_count;for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 /* Broad compilation is immediately restricted to exact pinned zero sites. */
 if(!w.wave_type&&op.operation_id<17&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_ANALYSIS_ALL,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;if(!sm3_defaults(op.operation_id,r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}}
 else if(w.wave_type==6&&apps<8){unsigned addr=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0;if(!addr||addr!=w.load_address){st=LT48_FORMAT_ERROR;goto fail;}mask|=1u<<(op.operation_id-17);r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,addr};}
 else if(w.wave_type==4&&op.operation_id==(p->kind==32?27u:30u)&&w.data_size==2780&&!r->fpga)r->fpga=w.data+2;else{st=LT48_UNSUPPORTED;goto fail;}}
 if(mask!=29||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}r->sstmx_applets=apps;TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
lt48_status lt48_sm3_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=32||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;
 if(op<1||op>8)return LT48_UNSUPPORTED;
 if((op==7||op==8||op==3||op==4)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==8)return (!im&&!size)||(im&&size==1&&*im==1)?LT48_OK:LT48_INVALID_ARGUMENT;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
