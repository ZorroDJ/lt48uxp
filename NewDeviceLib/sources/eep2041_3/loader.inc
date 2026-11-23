#include "profiles.h"
#include "admission.h"
unsigned lt48_eep2041_3_capacity_internal(unsigned cls,unsigned d,unsigned f,unsigned p){return cls==2041&&p==4&&((d==18&&f==10)||(d==19&&f==11))?8192:0;}
int lt48_eep2041_3_admitted_internal(void){return EP41_ADMITTED;}
lt48_status lt48_eep2041_3_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 if(!lt48_eep2041_3_capacity_internal(s.catalogue.device_class,s.definition_resource,s.family_code,s.pinmap))return LT48_UNSUPPORTED;
 unsigned found=0;for(size_t i=0;i<sizeof ep41_names/sizeof*ep41_names;i++)if(s.definition_resource==ep41_names[i].d&&s.family_code==ep41_names[i].m&&s.pinmap==ep41_names[i].p&&!strcmp(s.catalogue.vendor,ep41_names[i].vendor)&&s.catalogue.device_name_length==strlen(ep41_names[i].name)&&!memcmp(s.catalogue.device_name,ep41_names[i].name,s.catalogue.device_name_length))found=1;
 if(!found||s.capacity_bytes!=8192||s.element_bits!=8||s.pin_count!=28)return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;r->kind=16;r->n79_xp=1;r->definition=s.definition_resource;r->family=s.family_code;r->pinmap=s.pinmap;r->capacity=r->flash=8192;r->nominal=500;r->variable_count=3;r->fields=1;r->configuration_id=10001;r->configuration_category[0]=1;r->configuration_field[0]=0;r->configuration_maximum[0]=1;r->regions=1;r->region_length[0]=8192;
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/GDBA___.rez",lt48_installation_path(x));
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"4129e4261b9eeb7a4b7f4272d99b3e8d4ec45e41f9150a86baa57f909e8fc7ff",&r->rez));
 lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,"P4",&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));lt48_family_info f;snprintf(name,sizeof name,"M%x",r->family);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=3||f.configuration_set!=10001||f.operation_count!=9){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<3;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;
 for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(w.wave_type==0&&op.operation_id<4&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,3,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;}
 else if(w.wave_type==6&&apps<4){unsigned a=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0,bit=op.operation_id==17?1:op.operation_id==19?2:op.operation_id==20?4:8;if(!a||w.load_address!=a||(mask&bit)){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};}
 else if(w.wave_type==4&&op.operation_id==27&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_FORMAT_ERROR;goto fail;}}
 if(mask!=15||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}for(unsigned i=0;i<4;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
