#include "profiles.h"
/* Finite source loader; admission and operation owner are separate gates. */
lt48_status lt48_ite2_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 const struct ite2_profile*p=ite2_find(s.catalogue.device_class,s.definition_resource);if(!p)return LT48_UNSUPPORTED;
 const char*n=p->definition?"IT6350-256 @48LQ":"IT6350-128 @48LQ";
 if(strcmp(s.catalogue.vendor,"ITE")||s.catalogue.device_name_length!=strlen(n)||memcmp(n,s.catalogue.device_name,strlen(n))||s.family_code!=p->family||s.pinmap!=p->pinmap||s.capacity_bytes!=p->capacity||s.pin_count!=p->pins||s.element_bits!=8)return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->kind=50;r->definition=p->definition;r->family=p->family;r->pinmap=p->pinmap;r->capacity=p->capacity;r->nominal=320;r->regions=1;r->region_length[0]=p->capacity;r->flash=p->capacity;r->n79_xp=1;
 char path[4096],name[32];size_t ri,fi;snprintf(path,sizeof path,"%s/GDAF___.rez",lt48_installation_path(x));
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"55f4a58e34a4941f2ae04fd0ad2ec54011afe8b85e99669f60a6b180a17db613",&r->rez));
 snprintf(name,sizeof name,"P%x",p->pinmap);lt48_pin_code pc;TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));TRY(lt48_pin_map_complete_groups_internal(r->pins,36));
 snprintf(name,sizeof name,"M%x",p->family);lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=3||f.operation_count!=14||f.configuration_set){st=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=f.variable_count;for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned mask=0,count=0;
 for(unsigned i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
  if(!w.wave_type&&op.operation_id<17&&!r->wave[op.operation_id]){lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;}
  else if(w.wave_type==6&&count<8){unsigned address=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:op.operation_id==23?0x9000:op.operation_id==25?0x9a00:op.operation_id==26?0xa000:0;if(!address||address!=w.load_address){st=LT48_FORMAT_ERROR;goto fail;}mask|=1u<<(op.operation_id-17);r->applet[count]=w;r->addresses[count++]=(lt48_applet_address){op.operation_id,address};}
  else if(w.wave_type==4&&op.operation_id==27&&w.data_size==2780&&!r->fpga)r->fpga=w.data+2;else{st=LT48_FORMAT_ERROR;goto fail;}
 }
 if(mask!=861||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}r->sstmx_applets=count;TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}

int lt48_ite2_admitted_internal(unsigned cls,unsigned d){return LT48_ITE2_ADMITTED&&ite2_find(cls,d);}
