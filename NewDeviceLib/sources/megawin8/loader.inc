#include "profiles.h"
#include "admission.h"
static const struct mw8_profile*mw8_find(unsigned cls,unsigned d){unsigned kind=cls==2038?42:0;for(size_t i=0;i<sizeof mw8_profiles/sizeof*mw8_profiles;i++)if(mw8_profiles[i].kind==kind&&mw8_profiles[i].definition==d)return mw8_profiles+i;return NULL;}
unsigned lt48_mw8_capacity_internal(unsigned cls,unsigned d,unsigned family,unsigned pins){const struct mw8_profile*p=mw8_find(cls,d);return p&&p->family==family&&p->pinmap==pins?p->capacity:0;}
int lt48_mw8_admitted_internal(unsigned cls,unsigned d){return mw8_find(cls,d)&&LT48_MW8_ADMITTED;}
lt48_status lt48_mw8_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out){return LT48_INVALID_ARGUMENT;}
 *out=NULL;
 lt48_catalogue_entry e;lt48_status st=lt48_installation_get_device(x,index,&e);if(st)return st;const struct mw8_profile*p=mw8_find(e.device_class,e.device_type);if(!p)return LT48_UNSUPPORTED;
 lt48_device_spec s;st=lt48_installation_resolve_device(x,index,&s);if(st)return st;unsigned found=0;for(size_t i=0;i<sizeof mw8_members/sizeof*mw8_members;i++){const char*n=mw8_members[i].name;if(mw8_members[i].kind==p->kind&&mw8_members[i].definition==p->definition&&!strcmp(s.catalogue.vendor,mw8_members[i].vendor)&&strlen(n)==s.catalogue.device_name_length&&!memcmp(n,s.catalogue.device_name,s.catalogue.device_name_length))found=1;}
 if(!found||s.family_code!=p->family||s.pinmap!=p->pinmap||s.capacity_bytes!=p->capacity||s.pin_count!=p->pins||s.element_bits!=8)return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->kind=p->kind;r->definition=p->definition;r->family=p->family;r->pinmap=p->pinmap;r->capacity=p->capacity;r->fields=p->fields;r->configuration_id=p->configuration;r->configuration_lock=1;r->nominal=p->nominal;r->regions=p->regions;r->n79_xp=p->kind==42;r->mf74_page=p->page;r->flash=p->length[0];
 memcpy(r->region_length,p->length,sizeof p->length);memcpy(r->region_bank,p->logical,sizeof p->logical);memcpy(r->n79_physical,p->physical,sizeof p->physical);
 unsigned count;const struct mw8_field*schema=mw8_schema(p->configuration,&count);if(count!=p->fields){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<count;i++){r->configuration_category[i]=schema[i].category;r->configuration_field[i]=schema[i].field;r->configuration_maximum[i]=schema[i].maximum;}
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/%s",lt48_installation_path(x),"GDAE___.rez");
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));TRY(lt48_pin_map_complete_groups_internal(r->pins,36));
 snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=p->variables||f.operation_count!=p->operations||f.configuration_set!=p->configuration){st=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=f.variable_count;for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 /* Exact source maps only: no missing-pin fallback. */
 if(!w.wave_type&&op.operation_id<17&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}}
 else if(w.wave_type==6&&apps<8){unsigned addr=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:op.operation_id==18?0x5800:op.operation_id==23?0x9000:op.operation_id==24?0x9800:0;if(!addr||addr!=w.load_address){st=LT48_FORMAT_ERROR;goto fail;}mask|=1u<<(op.operation_id-17);r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,addr};}
 else if(w.wave_type==4&&op.operation_id==(p->kind==42?27u:30u)&&w.data_size==2780&&!r->fpga)r->fpga=w.data+2;else{st=LT48_UNSUPPORTED;goto fail;}}
 if(mask!=223||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}r->sstmx_applets=apps;TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
lt48_status lt48_mw8_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=42||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;
 if(((op<1||op>8)&&op!=10))return LT48_UNSUPPORTED;
 if((op==7||op==8||op==10||(op==3||op==4))&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==10){if(!im&&!size)return LT48_OK;if(!im||size!=4+4*r->fields)return LT48_INVALID_ARGUMENT;uint32_t mask=im[0]|(uint32_t)im[1]<<8|(uint32_t)im[2]<<16|(uint32_t)im[3]<<24;if(mask&~((1u<<r->fields)-1))return LT48_INVALID_ARGUMENT;for(unsigned i=0;i<r->fields;i++)if(mask>>i&1){uint32_t v=0;for(unsigned k=0;k<4;k++)v|=(uint32_t)im[4+4*i+k]<<(8*k);if(v>r->configuration_maximum[i]||(i==12&&(v&1)))return LT48_INVALID_ARGUMENT;}return LT48_OK;}
 if(op==8)return (!im&&!size)||(im&&size==1&&*im==1)?LT48_OK:LT48_INVALID_ARGUMENT;
 if(op>=3&&op<=6&&(!im||size!=n)){return LT48_INVALID_ARGUMENT;}
 return LT48_OK;
}
