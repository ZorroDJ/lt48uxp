/* Authored, exact Dataman13.10 SST/Winbond Page closure. Vendor assets stay BYO. */
#include "profiles.h"
static int at29_selected(const lt48_installation*x,size_t device) {
 lt48_catalogue_entry e;
 return lt48_installation_get_device(x,device,&e)==LT48_OK && at29_binding(&e);
}
static lt48_status at29_open(const lt48_installation*x,size_t device,lt48_xp_flash_resources**out) {
 lt48_device_spec d;lt48_status st;lt48_family_info f;lt48_pin_code pc;size_t at,fi;char path[4096],name[24],hex[65];unsigned char digest[32];
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 st=lt48_installation_resolve_device(x,device,&d);if(st!=LT48_OK)return st;
 static const struct{unsigned m,p,size,page,v;}shape[]={
 {0,0,65536,128,500},{0,1,131072,128,500},{1,2,262144,256,500},
 {2,3,524288,512,500},{1,3,524288,256,500},{3,0,65536,128,330},
 {4,1,131072,128,330},{5,2,262144,256,330},{5,3,524288,256,330}};
 if(!at29_binding(&d.catalogue)||d.element_bits!=8||d.pin_count!=32||d.definition_resource>8)return LT48_SAFETY_REJECTED;
 unsigned def=d.definition_resource;
 if(d.family_code!=shape[def].m||d.pinmap!=shape[def].p||d.capacity_bytes!=shape[def].size)return LT48_SAFETY_REJECTED;
 if(snprintf(path,sizeof path,"%s/GDBA___.rez",lt48_installation_path(x))>=(int)sizeof path)return LT48_INVALID_ARGUMENT;
 lt48_xp_flash_resources*r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
#define SST_TRY(call) do{st=(call);if(st!=LT48_OK)goto fail;}while(0)
 SST_TRY(lt48_rez_open(path,&r->rez));SST_TRY(lt48_rez_sha256(r->rez,digest));
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"4129e4261b9eeb7a4b7f4272d99b3e8d4ec45e41f9150a86baa57f909e8fc7ff")){st=LT48_SAFETY_REJECTED;goto fail;}
 r->at29=1;r->page_bytes=shape[def].page;r->boot=d.family_code==1||d.family_code==2||d.family_code==4||d.family_code==5;r->capacity=d.capacity_bytes;r->element_bytes=1;r->man=d.catalogue.manufacturer_id;r->dev=d.catalogue.device_id;r->nominal=shape[def].v;
 snprintf(name,sizeof name,"P%x",d.pinmap);SST_TRY(lt48_rez_find_resource(r->rez,name,&at));SST_TRY(lt48_rez_get_pin_code(r->rez,at,&pc));SST_TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",d.family_code);SST_TRY(lt48_rez_find_resource(r->rez,name,&fi));SST_TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.operation_count!=(r->boot?17u:13u)||f.variable_count!=4||f.configuration_set!=(r->boot?10002u:10001u)){st=LT48_FORMAT_ERROR;goto fail;}
 r->var_count=4;
 for(unsigned i=0;i<4;i++){lt48_family_variable v;SST_TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 if(r->vars[0].kind!=0||r->vars[0].variable_id!=0||r->vars[0].value!=r->nominal||r->vars[0].channel!=0||r->vars[3].variable_id!=2||r->vars[3].value!=r->page_bytes){st=LT48_SAFETY_REJECTED;goto fail;}
 for(unsigned i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code code;SST_TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);SST_TRY(lt48_rez_find_resource(r->rez,name,&at));SST_TRY(lt48_rez_get_wave_code(r->rez,at,&code));
  if(code.wave_type==0&&(op.operation_id<=4||op.operation_id==10||op.operation_id==6||op.operation_id==11)){
   lt48_compact_wave*w=NULL;SST_TRY(lt48_compact_decode(code.data,code.data_size,&w));st=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(w);if(st!=LT48_OK)goto fail;
  }else if(code.wave_type==6){
   unsigned addr=(op.operation_id==16||op.operation_id==18)?0x5000:op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==21?0x7000:op.operation_id==20?0x8000:op.operation_id==23?0x9000:op.operation_id==24?0x9800:0;
   if(!addr||code.load_address!=addr||!code.data_size||code.data_size>0x800||code.length1!=code.data_size+4||code.length2!=code.data_size||r->applet_count>=8){st=LT48_FORMAT_ERROR;goto fail;}
   r->applet[r->applet_count]=code;r->addresses[r->applet_count++]=(lt48_applet_address){op.operation_id,code.load_address};
  }else if(code.wave_type==4&&op.operation_id==27&&code.data_size==2780&&code.data[0]==0xda&&code.data[1]==0x0a)r->fpga=code.data+2;
  else{st=LT48_UNSUPPORTED;goto fail;}
 }
 if(r->applet_count!=(r->boot?8u:6u)||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<5;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
 if(!r->wave[10]){st=LT48_FORMAT_ERROR;goto fail;}
 SST_TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_xp_flash_close(r);return st;
#undef SST_TRY
}
