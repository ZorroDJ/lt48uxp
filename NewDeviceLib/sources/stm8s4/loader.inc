#include "profiles.h"
#include "configuration.h"
#include "admission.h"
static const struct s8_profile*s8_find(unsigned d){for(size_t i=0;i<sizeof s8_profiles/sizeof*s8_profiles;i++)if(s8_profiles[i].definition==d)return s8_profiles+i;return NULL;}
unsigned lt48_stm8s4_capacity_internal(unsigned cls,unsigned d,unsigned f,unsigned p){const struct s8_profile*z=s8_find(d);return cls==2048&&z&&z->family==f&&z->pinmap==p?z->capacity:0;}
int lt48_stm8s4_admitted_internal(unsigned d){return s8_find(d)&&s8_admitted(d);}
lt48_status lt48_stm8s4_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_catalogue_entry e;lt48_status st=lt48_installation_get_device(x,index,&e);if(st)return st;if(e.device_class!=2048||!s8_find(e.device_type))return LT48_UNSUPPORTED;
 lt48_device_spec s;st=lt48_installation_resolve_device(x,index,&s);if(st)return st;const struct s8_profile*p=s8_find(s.definition_resource);unsigned found=0;
 for(size_t i=0;i<sizeof s8_members/sizeof*s8_members;i++){const char*n=s8_members[i].name;if(s8_members[i].definition==p->definition&&s8_members[i].manufacturer==s.catalogue.manufacturer_id&&s8_members[i].device==s.catalogue.device_id&&!strcmp(s8_members[i].vendor,s.catalogue.vendor)&&strlen(n)==s.catalogue.device_name_length&&!memcmp(n,s.catalogue.device_name,s.catalogue.device_name_length))found=1;}
 if(!found||s.family_code!=p->family||s.pinmap!=p->pinmap||s.capacity_bytes!=p->capacity)return LT48_SAFETY_REJECTED;
 unsigned schema_count=0;if(!s8_schema(p->configuration,&schema_count)||schema_count!=p->fields)return LT48_FORMAT_ERROR;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->kind=15;r->sstmx_applets=p->apps;r->definition=p->definition;r->family=p->family;r->pinmap=p->pinmap;r->capacity=p->capacity;r->flash=p->code;r->nominal=p->nominal;r->fields=p->fields;r->configuration_id=p->configuration;r->stm8s4_page=p->page;unsigned count=0;const struct numicro_field*schema=s8_schema(p->configuration,&count);for(unsigned i=0;i<count;i++){r->configuration_category[i]=schema[i].category;r->configuration_field[i]=schema[i].field;r->configuration_maximum[i]=schema[i].maximum;};r->regions=2;r->variable_count=p->variables;r->manufacturer=s.catalogue.manufacturer_id;r->device=s.catalogue.device_id;
 r->region_length[0]=p->data;r->region_length[1]=p->code;r->n79_physical[0]=r->region_bank[0]=0x4000;r->n79_physical[1]=r->region_bank[1]=0x8000;
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/GDCC___.rez",lt48_installation_path(x));
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=p->variables||f.configuration_set!=p->configuration||f.operation_count!=p->ops){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<p->variables;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;
 for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(!w.wave_type&&op.operation_id<17&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;}
 else if(w.wave_type==6&&apps<p->apps){unsigned a=op.operation_id==17?0x6000:op.operation_id==21?0x7000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==23?0x9000:op.operation_id==24?0x9800:op.operation_id==26?0xa000:0;unsigned bit=op.operation_id==24?1:op.operation_id==17?4:op.operation_id==21?8:op.operation_id==19?16:op.operation_id==20?32:op.operation_id==23?64:128;if(!a||a!=w.load_address||(mask&bit)){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};}
 else if(w.wave_type==4&&op.operation_id==30&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_UNSUPPORTED;goto fail;}}
 if(apps!=p->apps||!r->fpga||!r->wave[15]){st=LT48_FORMAT_ERROR;goto fail;}for(unsigned i=0;i<12;i++)if(i!=4&&!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}

int lt48_stm8s4_member_internal(unsigned d){return s8_find(d)!=NULL;}
