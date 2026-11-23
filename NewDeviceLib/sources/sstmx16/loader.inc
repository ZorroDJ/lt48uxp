#include "profiles.h"
#include "admission.h"
static const struct sm16_profile*sm16_find(unsigned d){for(size_t i=0;i<sizeof sm16_profiles/sizeof*sm16_profiles;i++)if(sm16_profiles[i].definition==d)return sm16_profiles+i;return NULL;}
unsigned lt48_sstmx16_capacity_internal(unsigned cls,unsigned d,unsigned family,unsigned pinmap){const struct sm16_profile*p=sm16_find(d);return cls==2038&&p&&p->family==family&&p->pinmap==pinmap?p->capacity:0;}
int lt48_sstmx16_admitted_internal(unsigned d){return sm16_find(d)&&sm16_admitted(d);}
lt48_status lt48_sstmx16_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_catalogue_entry e;lt48_status early=lt48_installation_get_device(x,index,&e);if(early)return early;if(e.device_class!=2038||!sm16_find(e.device_type))return LT48_UNSUPPORTED;
 lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 if(s.catalogue.device_class!=2038)return LT48_UNSUPPORTED;
 const struct sm16_profile*p=sm16_find(s.definition_resource);if(!p)return LT48_UNSUPPORTED;
 unsigned found=0;for(size_t i=0;i<sizeof sm16_members/sizeof*sm16_members;i++){const char*n=sm16_members[i].name;if(sm16_members[i].definition==p->definition&&sm16_members[i].manufacturer==s.catalogue.manufacturer_id&&sm16_members[i].device==s.catalogue.device_id&&!strcmp(sm16_members[i].vendor,s.catalogue.vendor)&&strlen(n)==s.catalogue.device_name_length&&!memcmp(n,s.catalogue.device_name,s.catalogue.device_name_length))found=1;}
 unsigned schema_count=0;if(sm16_schema(p->configuration,&schema_count)!=p->schema||schema_count!=p->fields)return LT48_FORMAT_ERROR;
 if(!found||s.family_code!=p->family||s.pinmap!=p->pinmap||s.capacity_bytes!=p->capacity)return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->kind=10;r->sstmx_variant=p->variant;r->sstmx_applets=p->apps;r->n79_xp=1;r->manufacturer=s.catalogue.manufacturer_id;r->device=s.catalogue.device_id;r->definition=p->definition;r->family=p->family;r->pinmap=p->pinmap;r->capacity=p->capacity;r->flash=p->capacity;r->nominal=p->nominal;r->fields=p->fields;r->configuration_id=p->configuration;r->regions=1;r->region_length[0]=p->capacity;
 r->configuration_lock=0;r->n79_locks=p->variant==1?0xf8f:7;for(unsigned i=0;i<p->fields;i++){r->configuration_category[i]=p->schema[i].category;r->configuration_field[i]=p->schema[i].field;r->configuration_maximum[i]=p->schema[i].maximum;}
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/%s",lt48_installation_path(x),"GDAE___.rez");
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=p->variables||f.configuration_set!=p->configuration||f.operation_count!=p->ops){st=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=f.variable_count;for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;
 for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(!w.wave_type&&op.operation_id<12&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st){fprintf(stderr,"NuMicro D%x role%u compile status%d\n",p->definition,op.operation_id,st);goto fail;}}
 else if(w.wave_type==6&&apps<p->apps){unsigned a=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:op.operation_id==23?0x9000:0;unsigned bit=a==0x6000?1:a==0x7800?2:a==0x8000?4:a==0x7000?8:16;if(!a||a!=w.load_address||(mask&bit)){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};}
 else if(w.wave_type==4&&op.operation_id==27u&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_UNSUPPORTED;goto fail;}}
 if(mask!=(p->apps==5?31u:15u)||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
lt48_status lt48_sstmx16_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=10||!n||start>=r->capacity||n>r->capacity-start)return LT48_SAFETY_REJECTED;
 if((op<1||op>8)&&op!=10)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7||op==8||op==10)&&(start||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==10){if(!im&&!size)return LT48_OK;if(!im||size!=2+4*r->fields)return LT48_INVALID_ARGUMENT;unsigned mask=im[0]|(unsigned)im[1]<<8;if(mask&~((1u<<r->fields)-1))return LT48_INVALID_ARGUMENT;for(unsigned i=0;i<r->fields;i++){uint32_t v=0;for(unsigned k=0;k<4;k++)v|=(uint32_t)im[2+4*i+k]<<(8*k);if((mask>>i&1)&&v>r->configuration_maximum[i])return LT48_INVALID_ARGUMENT;}}
 else if(op==8){if(im&&(size!=1||*im!=1))return LT48_INVALID_ARGUMENT;if(!im&&size)return LT48_INVALID_ARGUMENT;}
 else if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
