/* Exact TPI code/configuration resources. Vendor assets stay BYO. */
static lt48_status tpi15_open(const lt48_installation*x,size_t device,lt48_xp_flash_resources**out) {
 lt48_device_spec d;lt48_status st;lt48_family_info f;lt48_pin_code pc;size_t at,fi;char path[4096],name[24],hex[65];unsigned char digest[32];
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 st=lt48_installation_resolve_device(x,device,&d);if(st!=LT48_OK)return st;
 if(d.catalogue.device_class!=2038||!tpi15_binding(&d.catalogue)||d.element_bits!=8)return LT48_SAFETY_REJECTED;
 if(snprintf(path,sizeof path,"%s/GDAE___.rez",lt48_installation_path(x))>=(int)sizeof path)return LT48_INVALID_ARGUMENT;
 lt48_xp_flash_resources*r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
#define OTP_TRY(call) do{st=(call);if(st!=LT48_OK)goto fail;}while(0)
 OTP_TRY(lt48_rez_open(path,&r->rez));OTP_TRY(lt48_rez_sha256(r->rez,digest));
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36")){st=LT48_SAFETY_REJECTED;goto fail;}
 r->tpi15=1;for(size_t i=0;i<sizeof tpi15_closures/sizeof*tpi15_closures;i++)if(tpi15_closures[i].definition==d.definition_resource)r->closure=tpi15_closures+i;if(!r->closure){st=LT48_SAFETY_REJECTED;goto fail;}if(d.family_code!=r->closure->family||d.pinmap!=r->closure->pinmap||d.pin_count!=r->closure->pins||d.capacity_bytes!=r->closure->capacity){st=LT48_SAFETY_REJECTED;goto fail;}r->capacity=d.capacity_bytes;r->element_bytes=1;r->man=d.catalogue.manufacturer_id;r->dev=d.catalogue.device_id;r->nominal=500;
 snprintf(name,sizeof name,"P%x",d.pinmap);OTP_TRY(lt48_rez_find_resource(r->rez,name,&at));OTP_TRY(lt48_rez_get_pin_code(r->rez,at,&pc));OTP_TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",d.family_code);OTP_TRY(lt48_rez_find_resource(r->rez,name,&fi));OTP_TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if((f.operation_count!=15&&f.operation_count!=16)||f.variable_count!=(unsigned)(d.definition_resource>=0x160?4:3)){st=LT48_FORMAT_ERROR;goto fail;}
 r->var_count=f.variable_count;
 for(unsigned i=0;i<r->var_count;i++){lt48_family_variable v;OTP_TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 if(r->vars[0].kind!=0||r->vars[0].variable_id!=0||r->vars[0].value!=500||r->vars[0].channel!=(unsigned)(d.definition_resource==0x161?0:(d.definition_resource>=0x15c&&d.definition_resource<=0x15e)?2:1)){st=LT48_SAFETY_REJECTED;goto fail;}
 for(unsigned i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code code;OTP_TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));snprintf(name,sizeof name,"W%x",op.wave);OTP_TRY(lt48_rez_find_resource(r->rez,name,&at));OTP_TRY(lt48_rez_get_wave_code(r->rez,at,&code));
  if(code.wave_type==0&&(op.operation_id<16)){
   lt48_compact_wave*w=NULL;OTP_TRY(lt48_compact_decode(code.data,code.data_size,&w));st=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);lt48_compact_wave_close(w);if(st!=LT48_OK)goto fail;

  }else if(code.wave_type==6){
   unsigned addr=op.operation_id==16?0x5000:op.operation_id==17?0x6000:op.operation_id==18?0x5800:op.operation_id==19?0x7800:op.operation_id==21?0x7000:op.operation_id==20?0x8000:op.operation_id==23?0x9000:op.operation_id==24?0x9800:0;
   if(!addr||code.load_address!=addr||!code.data_size||code.data_size>0x800||code.length1!=code.data_size+4||code.length2!=code.data_size||r->applet_count>=8){st=LT48_FORMAT_ERROR;goto fail;}
   r->applet[r->applet_count]=code;r->addresses[r->applet_count++]=(lt48_applet_address){op.operation_id,code.load_address};
  }else if(code.wave_type==4&&op.operation_id==27&&code.data_size==2780&&code.data[0]==0xda&&code.data[1]==0x0a)r->fpga=code.data+2;
  else{st=LT48_UNSUPPORTED;goto fail;}
 }
 if((r->applet_count!=6&&r->applet_count!=7)||!r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<4;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
 OTP_TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_xp_flash_close(r);return st;
#undef OTP_TRY
}
