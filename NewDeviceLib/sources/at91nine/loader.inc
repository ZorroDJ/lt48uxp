#include "profiles.h"
#include "admission.h"
#include "../at91four/bridge.inc"
unsigned lt48_at91nine_capacity_internal(unsigned cls,unsigned d,unsigned m,unsigned p){for(size_t i=0;i<sizeof a91_profiles/sizeof*a91_profiles;i++){const struct a91_profile*q=&a91_profiles[i];if(q->cls==cls&&q->d==d&&q->m==m&&q->p==p)return q->cap;}return 0;}
int lt48_at91nine_admitted_internal(void){return A91_ADMITTED;}
lt48_status lt48_at91nine_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 const struct a91_profile*q=NULL;for(size_t i=0;i<sizeof a91_profiles/sizeof*a91_profiles;i++)if(s.catalogue.device_class==a91_profiles[i].cls&&s.definition_resource==a91_profiles[i].d&&s.family_code==a91_profiles[i].m&&s.pinmap==a91_profiles[i].p)q=&a91_profiles[i];if(!q)return LT48_UNSUPPORTED;
 unsigned found=0;for(size_t i=0;i<sizeof a91_names/sizeof*a91_names;i++)if(s.catalogue.device_class==a91_names[i].cls&&s.definition_resource==a91_names[i].d&&!strcmp(s.catalogue.vendor,a91_names[i].vendor)&&s.catalogue.device_name_length==strlen(a91_names[i].name)&&!memcmp(s.catalogue.device_name,a91_names[i].name,s.catalogue.device_name_length))found=1;
 if(!found||s.capacity_bytes!=q->cap||s.element_bits!=8*q->width||s.pin_count!=(q->p==114?48u:40u))return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;r->kind=18;r->n79_xp=1;r->definition=s.definition_resource;r->family=s.family_code;r->pinmap=s.pinmap;r->capacity=r->flash=q->cap;r->nominal=330;r->variable_count=3;r->fields=3;r->configuration_id=25110;r->configuration_category[0]=0;r->configuration_category[1]=r->configuration_category[2]=2;r->configuration_field[2]=1;r->configuration_maximum[0]=1;r->configuration_maximum[1]=q->locks;r->configuration_maximum[2]=3;r->regions=1;r->region_length[0]=q->cap;r->sstmx_applets=8;r->a91_width=q->width;r->a91_vcc=q->vcc;
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/%s",lt48_installation_path(x),q->source);
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,q->sha,&r->rez));lt48_pin_code pc;snprintf(name,sizeof name,"P%x",r->pinmap);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));lt48_family_info f;snprintf(name,sizeof name,"M%x",r->family);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=3||f.configuration_set!=25110||f.operation_count!=16){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<3;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}if(r->vars[0].value!=330||r->vars[1].value!=q->core||r->vars[2].value!=q->page){st=LT48_FORMAT_ERROR;goto fail;}
 unsigned apps=0,mask=0;
 for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(w.wave_type==0&&(op.operation_id<4||op.operation_id==6||op.operation_id==10||op.operation_id==11)&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,3,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;}
 else if(w.wave_type==6&&apps<8){unsigned a=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:op.operation_id==23?0x9000:op.operation_id==18?0x5800:op.operation_id==24?0x9800:op.operation_id==26?0xa000:0;unsigned bit=op.operation_id<32?1u<<op.operation_id:0;if(!a||w.load_address!=a||(mask&bit)){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};}
 else if(w.wave_type==4&&op.operation_id==27&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_FORMAT_ERROR;goto fail;}}
 if(apps!=8||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}for(unsigned i=0;i<12;i++)if((i<4||i==6||i==10||i==11)&&!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));if(q->cls==2038&&q->p==115&&A91FOUR_ADMITTED){lt48_s8253_resources*bridge=NULL;st=a91_four_bridge(x,r,&bridge);if(st)goto fail;lt48_s8253_close(r);r=bridge;}*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
