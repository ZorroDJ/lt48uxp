#include "profiles.h"
#include "admission.h"
unsigned lt48_at28c64b1_capacity_internal(unsigned cls,unsigned d,unsigned f,unsigned p){return cls==A28_CLASS&&d==A28_DEFINITION&&f==A28_FAMILY&&p==A28_PINMAP?A28_CAPACITY:0;}
int lt48_at28c64b1_admitted_internal(void){return A28_ADMITTED;}
lt48_status lt48_at28c64b1_set_sdp(lt48_s8253_resources*r,unsigned choice){if(!r||r->kind!=16||choice>1)return LT48_INVALID_ARGUMENT;r->at28c64b1_sdp=choice;return LT48_OK;}
lt48_status lt48_at28c64b1_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 if(!lt48_at28c64b1_capacity_internal(s.catalogue.device_class,s.definition_resource,s.family_code,s.pinmap))return LT48_UNSUPPORTED;
 if(strcmp(s.catalogue.vendor,"Atmel")||s.catalogue.device_name_length!=strlen("AT28C64B")||memcmp(s.catalogue.device_name,"AT28C64B",s.catalogue.device_name_length)||s.capacity_bytes!=8256||s.pin_count!=28||s.element_bits!=8)return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;r->kind=16;r->definition=230;r->family=228;r->pinmap=53;r->capacity=8256;r->flash=8192;r->nominal=500;r->variable_count=5;r->fields=1;r->configuration_id=10001;r->configuration_category[0]=1;r->configuration_field[0]=0;r->configuration_maximum[0]=1;r->regions=2;r->region_length[0]=8192;r->region_length[1]=64;r->region_bank[1]=8192;r->n79_physical[1]=8128;
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/GDCA___.rez",lt48_installation_path(x));
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39",&r->rez));
 lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,"P35",&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,"Me4",&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=5||f.configuration_set!=10001||f.operation_count!=13){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<5;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned apps=0,mask=0;
 for(size_t i=0;i<f.operation_count;i++){lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
 if(w.wave_type==0&&op.operation_id<12&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,5,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;}
 else if(w.wave_type==6&&apps<4){unsigned a=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0,bit=op.operation_id==17?1:op.operation_id==19?2:op.operation_id==20?4:8;if(!a||w.load_address!=a||(mask&bit)){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,a};}
 else if(w.wave_type==4&&op.operation_id==30&&w.data_size==2780&&w.data[0]==0xda&&w.data[1]==0x0a&&!r->fpga)r->fpga=w.data+2;
 else{st=LT48_FORMAT_ERROR;goto fail;}}
 if(mask!=15||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}const unsigned required[]={0,1,2,3,5,7,8,9};for(unsigned i=0;i<8;i++)if(!r->wave[required[i]]){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
