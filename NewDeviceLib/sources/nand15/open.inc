/* Authored, exact Dataman13.10 SST/Winbond Page closure. Vendor assets stay BYO. */
#include "profiles.h"
#include "geometry.h"
#include "../nand2/geometry.h"
static int nand15_selected(const lt48_installation*x,size_t device) {
 lt48_catalogue_entry e;
 return lt48_installation_get_device(x,device,&e)==LT48_OK && nand15_binding(&e) && nand15_admitted(e.device_type);
}
static lt48_status nand15_open(const lt48_installation*x,size_t device,lt48_xp_flash_resources**out) {
 lt48_device_spec d;lt48_status st;lt48_family_info f;lt48_pin_code pc;size_t at,fi;char path[4096],name[24],hex[65];unsigned char digest[32];
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 st=lt48_installation_resolve_device(x,device,&d);if(st!=LT48_OK)return st;
 if(!nand15_binding(&d.catalogue)||d.element_bits!=8)return LT48_SAFETY_REJECTED;
 if(snprintf(path,sizeof path,"%s/GDBA___.rez",lt48_installation_path(x))>=(int)sizeof path)return LT48_INVALID_ARGUMENT;
 lt48_xp_flash_resources*r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
#define NAND_TRY(call) do{st=(call);if(st!=LT48_OK)goto fail;}while(0)
 NAND_TRY(lt48_rez_open(path,&r->rez));NAND_TRY(lt48_rez_sha256(r->rez,digest));
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"4129e4261b9eeb7a4b7f4272d99b3e8d4ec45e41f9150a86baa57f909e8fc7ff")){st=LT48_SAFETY_REJECTED;goto fail;}
 r->nand15=1;for(size_t i=0;i<sizeof nand15_closures/sizeof*nand15_closures;i++)if(nand15_closures[i].definition==d.definition_resource)r->closure=nand15_closures+i;if(d.definition_resource==31&&nand2_binding(&d.catalogue))r->closure=&nand2_closure;if(!r->closure){st=LT48_SAFETY_REJECTED;goto fail;}if(d.family_code!=r->closure->family||d.pinmap!=r->closure->pinmap||d.pin_count!=r->closure->pins||d.capacity_bytes!=r->closure->capacity){st=LT48_SAFETY_REJECTED;goto fail;}r->capacity=d.capacity_bytes;r->element_bytes=1;r->man=d.catalogue.manufacturer_id;r->dev=d.catalogue.device_id;r->nominal=330;
 snprintf(name,sizeof name,"P%x",d.pinmap);NAND_TRY(lt48_rez_find_resource(r->rez,name,&at));NAND_TRY(lt48_rez_get_pin_code(r->rez,at,&pc));NAND_TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",d.family_code);NAND_TRY(lt48_rez_find_resource(r->rez,name,&fi));NAND_TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.operation_count!=12||f.variable_count!=2||f.configuration_set){st=LT48_FORMAT_ERROR;goto fail;}
 r->var_count=f.variable_count;
 for(unsigned i=0;i<r->var_count;i++){lt48_family_variable v;NAND_TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 if(r->vars[0].kind!=0||r->vars[0].variable_id!=0||r->vars[0].value!=r->nominal||r->vars[0].channel!=0||r->vars[r->var_count-1].variable_id!=2||r->vars[r->var_count-1].value!=528){st=LT48_SAFETY_REJECTED;goto fail;}
 for(unsigned i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code code;NAND_TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);NAND_TRY(lt48_rez_find_resource(r->rez,name,&at));NAND_TRY(lt48_rez_get_wave_code(r->rez,at,&code));
  if(code.wave_type==0&&(op.operation_id<=4||op.operation_id==10)){
   lt48_compact_wave*w=NULL;NAND_TRY(lt48_compact_decode(code.data,code.data_size,&w));st=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(w);if(st!=LT48_OK)goto fail;
  }else if(code.wave_type==6){
   unsigned addr=op.operation_id==16?0x5000:op.operation_id==17?0x6000:op.operation_id==18?0x5800:op.operation_id==19?0x7800:op.operation_id==21?0x7000:op.operation_id==20?0x8000:op.operation_id==23?0x9000:0;
   if(!addr||code.load_address!=addr||!code.data_size||code.data_size>0x800||code.length1!=code.data_size+4||code.length2!=code.data_size||r->applet_count>=7){st=LT48_FORMAT_ERROR;goto fail;}
   r->applet[r->applet_count]=code;r->addresses[r->applet_count++]=(lt48_applet_address){op.operation_id,code.load_address};
  }else if(code.wave_type==4&&op.operation_id==27&&code.data_size==2780&&code.data[0]==0xda&&code.data[1]==0x0a)r->fpga=code.data+2;
  else{st=LT48_UNSUPPORTED;goto fail;}
 }
 if(r->applet_count!=5||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<5;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
 if(!r->wave[10]){st=LT48_FORMAT_ERROR;goto fail;}
 NAND_TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_xp_flash_close(r);return st;
#undef NAND_TRY
}
