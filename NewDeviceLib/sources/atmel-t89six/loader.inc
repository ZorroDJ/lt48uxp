#include "admission.h"
unsigned lt48_t89six_capacity_internal(unsigned cls,unsigned d,unsigned m,unsigned p){
 if(cls!=2018||d<0x112||d>0x115||m!=157+d-0x112||p!=67+d-0x112)return 0;
 return 20480;
}
int lt48_t89six_admitted_internal(unsigned d){return d>=0x112&&d<=0x115&&(d<=0x113?LT48_T89_SIX_STAGE>=1:LT48_T89_SIX_STAGE>=2);}
lt48_status lt48_t89six_open(const lt48_installation*x,size_t index,lt48_s8253_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,index,&s);if(st)return st;
 if(strcmp(lt48_installation_source_profile(x),"Dataman 13.10")||!lt48_t89six_capacity_internal(s.catalogue.device_class,s.definition_resource,s.family_code,s.pinmap))return LT48_UNSUPPORTED;
 if(strcmp(s.catalogue.vendor,"Atmel")||s.capacity_bytes!=20480||s.element_bits!=8||strcmp(s.definition_stem,"GAF____"))return LT48_SAFETY_REJECTED;
 lt48_s8253_resources*r=calloc(1,sizeof*r);if(!r)return LT48_NO_MEMORY;
 r->kind=19;r->definition=s.definition_resource;r->family=s.family_code;r->pinmap=s.pinmap;r->flash=r->capacity=20480;r->nominal=500;r->variable_count=4;r->fields=11;r->configuration_id=60019;r->sstmx_applets=4;
 const unsigned cat[11]={0,0,0,0,0,2,2,2,2,2,2},field[11]={12,13,14,5,4,8,9,16,17,18,11};
 for(unsigned i=0;i<11;i++){r->configuration_category[i]=cat[i];r->configuration_field[i]=field[i];r->configuration_maximum[i]=i<5?1:255;}
 char path[4096],name[24];size_t ri,fi;snprintf(path,sizeof path,"%s/GAF____.rez",lt48_installation_path(x));
#define TRY(v) do{st=(v);if(st)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"126dcd7dd7252b5af78e151d34106f0c17b5e52575b9606a80b9f615f912c123",&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",r->pinmap);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 lt48_family_info f;snprintf(name,sizeof name,"M%x",r->family);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));if(f.variable_count!=4||f.operation_count!=16){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<4;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned mask=0,apps=0;
 for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
  if(w.wave_type==0&&op.operation_id<17&&!r->wave[op.operation_id]){
   lt48_compact_wave*c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile(c,r->pins,r->vars,4,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st)goto fail;
   if(op.operation_id==8){
    unsigned char capture[0x48];memcpy(capture,w.data,sizeof capture);capture[0]=sizeof capture-2;capture[1]=0;lt48_compact_wave*fragment=NULL;TRY(lt48_compact_decode(capture,sizeof capture,&fragment));st=lt48_compact_compile(fragment,r->pins,r->vars,4,1,0,&r->wave[11]);lt48_compact_wave_close(fragment);if(st)goto fail;
   }
   if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
  }else if(w.wave_type==6&&op.operation_id>=17&&op.operation_id<=20&&apps<4){unsigned bit=1u<<(op.operation_id-17),address=0x4100+0x100*(op.operation_id-17);if((mask&bit)||w.load_address!=address){st=LT48_FORMAT_ERROR;goto fail;}mask|=bit;r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,address};}
  else{st=LT48_FORMAT_ERROR;goto fail;}
 }
 if(mask!=15){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));r->fpga=r->idle;*out=r;return LT48_OK;
 fail:lt48_s8253_close(r);return st;
#undef TRY
}
lt48_status lt48_t89six_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=19||!n||a>=r->capacity||n>r->capacity-a)return LT48_INVALID_ARGUMENT;
 if((op<1||op>8)&&op!=10)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7||op==8||op==10)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 if(op==8&&((im&&(size!=1||im[0]<1||im[0]>3))||(!im&&size)))return LT48_INVALID_ARGUMENT;
 if(op==10){if(!im)return size?LT48_INVALID_ARGUMENT:LT48_OK;if(size!=46||(im[0]|(unsigned)im[1]<<8)&~2047u)return LT48_INVALID_ARGUMENT;for(unsigned i=0;i<11;i++)if((im[i*4+4]||im[i*4+5])||((i<5)&&(im[i*4+2]>1||im[i*4+3]))||im[i*4+3])return LT48_INVALID_ARGUMENT;}
 return LT48_OK;
}
