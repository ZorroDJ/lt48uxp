/* Exact original13.10 bindings; no public catalogue admission yet. */
static const lt48_pic_profile pic_cal14_source_profile={.strategy=LT48_PIC_COMPACT_ROWS};
static lt48_status pic_cal14_open(const lt48_installation *x,const lt48_device_spec *spec,lt48_pic_resources **out){
 if(!x||!spec||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 unsigned channel=spec->definition_resource==338&&spec->family_code==229&&spec->pinmap==77?2:
  spec->definition_resource==339&&spec->family_code==230&&spec->pinmap==78?1:0;
 if(!channel||spec->catalogue.device_class!=2015||!spec->definition_stem||strcmp(spec->definition_stem,"GADC___")||spec->element_bits!=14||spec->capacity_bytes!=17152)return LT48_UNSUPPORTED;
 lt48_pic_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
 r->calibrated14=spec->definition_resource;r->profile=&pic_cal14_source_profile;r->capacity=17152;r->program_words=1024;r->core=14;
 char path[4096],name[32],hex[65];size_t idx,fi;unsigned char sha[32];lt48_status st;
#define C14TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
 if(snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x))>=(int)sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
 C14TRY(lt48_rez_open(path,&r->rez));C14TRY(lt48_rez_sha256(r->rez,sha));
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",sha[i]);
 if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec")){st=LT48_SAFETY_REJECTED;goto fail;}
 snprintf(name,sizeof name,"P%x",spec->pinmap);C14TRY(lt48_rez_find_resource(r->rez,name,&idx));lt48_pin_code pc;
 C14TRY(lt48_rez_get_pin_code(r->rez,idx,&pc));C14TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",spec->family_code);C14TRY(lt48_rez_find_resource(r->rez,name,&fi));lt48_family_info f;C14TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count!=8||f.operation_count!=10){st=LT48_SAFETY_REJECTED;goto fail;}
 const lt48_compile_variable expected[]={
  {0,0,1,500,channel},{0,3,1,1250,3},{1,13,0,0,2},{1,8,1,1,40},
  {1,10,0,1,80},{2,19,0,1024,0},{0,5,1,450,channel},{0,6,1,550,channel}};
 r->var_count=8;
 for(unsigned i=0;i<8;i++){
  lt48_family_variable v;C14TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
  r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
  if(memcmp(r->vars+i,expected+i,sizeof *r->vars)){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 const unsigned roles[]={0,1,3,4,6,8,10,9,11,12},waves[]={281,5,282,283,284,285,287,286,288,289};
 for(unsigned i=0;i<10;i++){
  lt48_family_operation op;C14TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
  if(op.operation_id!=roles[i]||op.wave!=waves[i]){st=LT48_SAFETY_REJECTED;goto fail;}
  if(op.operation_id!=0&&op.operation_id!=1&&op.operation_id!=3&&op.operation_id!=4)continue;
  snprintf(name,sizeof name,"W%x",op.wave);C14TRY(lt48_rez_find_resource(r->rez,name,&idx));lt48_wave_code w;
  C14TRY(lt48_rez_get_wave_code(r->rez,idx,&w));if(w.wave_type){st=LT48_UNSUPPORTED;goto fail;}
  lt48_compact_wave *raw=NULL;C14TRY(lt48_compact_decode(w.data,w.data_size,&raw));
  st=lt48_compact_compile(raw,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(raw);
  if(st!=LT48_OK)goto fail;
  if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 C14TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
 *out=r;return LT48_OK;
fail:lt48_pic_close(r);return st;
#undef C14TRY
}
typedef struct {pic_session s;classic_wire_context wire;lt48_pic_result *result;unsigned vcc;} cal14_context;
static lt48_status cal14_reenter(void *v){
 cal14_context *c=v;
 lt48_status st=reset(&c->s);
 /* Explicit discharge before VPP-first original DEVINIT. */
 for(unsigned channel=4;st==LT48_OK&&channel;channel--){const unsigned char off[]={0x10,(unsigned char)(channel-1),0};st=lt48_runtime_transport_session_controller_write(c->s.wire,off,sizeof off);}
 if(st==LT48_OK)st=classic_wire_delay(&c->wire,10000);
 if(st==LT48_OK)st=p17_start(&c->s,8,c->vcc);
 return st;
}
static lt48_status cal14_transfer(void *v,int read,unsigned bits,uint32_t value,uint32_t *out){return classic_wire_transfer(&((cal14_context*)v)->wire,read,bits,value,out);}
static lt48_status cal14_delay(void *v,unsigned us){return classic_wire_delay(&((cal14_context*)v)->wire,us);}
static lt48_status cal14_voltage(void *v,unsigned field){
 cal14_context *c=v;if(field!=0&&field!=5&&field!=6)return LT48_INVALID_ARGUMENT;
 c->vcc=p17_voltage(c->s.r,field);return c->vcc>=450&&c->vcc<=550?LT48_OK:LT48_SAFETY_REJECTED;
}
static lt48_status cal14_cleanup(void *v,lt48_status operation){cal14_context *c=v;(void)finish(&c->s,operation,c->result);return c->result->cleanup_status;}
static lt48_status cal14_save_factory(void *v,uint16_t calibration,uint16_t bandgap){
 cal14_context *c=v;char directory[4096],file[4096];const char *chosen=getenv("LT48_CALIBRATION_RECOVERY_DIR");
 if(chosen){if(snprintf(directory,sizeof directory,"%s",chosen)>=(int)sizeof directory)return LT48_FILE_ERROR;}
 else{
  const char *base=getenv("XDG_DATA_HOME"),*home=getenv("HOME");char root[4096];
  if(base){if(snprintf(root,sizeof root,"%s",base)>=(int)sizeof root)return LT48_FILE_ERROR;}
  else if(!home||snprintf(root,sizeof root,"%s/.local/share",home)>=(int)sizeof root)return LT48_FILE_ERROR;
  if(snprintf(directory,sizeof directory,"%s/labtool48",root)>=(int)sizeof directory)return LT48_FILE_ERROR;
  if(baseline12_directory(directory)!=LT48_OK)return LT48_FILE_ERROR;
  size_t n=strlen(directory);if(n+sizeof "/calibration-recovery">sizeof directory)return LT48_FILE_ERROR;
  memcpy(directory+n,"/calibration-recovery",sizeof "/calibration-recovery");
 }
 if(baseline12_directory(directory)!=LT48_OK)return LT48_FILE_ERROR;
 static unsigned sequence;FILE *f=NULL;
 for(unsigned i=0;i<64&&!f;i++){
  if(snprintf(file,sizeof file,"%s/calibrated14-D%x-%lld-%u.json",directory,c->s.r->calibrated14,(long long)time(NULL),++sequence)>=(int)sizeof file)return LT48_FILE_ERROR;
  f=fopen(file,"wx");if(!f&&errno!=EEXIST)return LT48_FILE_ERROR;
 }
 if(!f)return LT48_FILE_ERROR;
 int ok=fprintf(f,"{\"schema\":1,\"engine\":\"calibrated14\",\"definition\":%u,\"osccal_address\":1023,\"osccal\":%u,\"configuration_address\":8199,\"bandgap_mask\":12288,\"bandgap\":%u,\"source_sha256\":\"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec\"}\n",c->s.r->calibrated14,calibration,bandgap)>0;
#ifdef _WIN32
 if(fflush(f)||_commit(_fileno(f)))ok=0;
#else
 if(fflush(f)||fsync(fileno(f)))ok=0;
#endif
 if(fclose(f))ok=0;
#ifndef _WIN32
 if(ok){FILE *d=fopen(directory,"r");if(!d)ok=0;else{if(fsync(fileno(d)))ok=0;fclose(d);}}
#endif
 return ok?LT48_OK:LT48_FILE_ERROR;
}
static lt48_status pic_cal14_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,lt48_pic_result *out){
 cal14_context c={0};c.s.r=r;c.s.api=api;c.s.context=context;c.s.mode=8;c.result=out;c.vcc=500;
 memcpy(c.s.vars,r->vars,sizeof c.s.vars);
 /* Original optional family-ID checkbox, explicitly disabled for manually
  * selected operation. No missing-configuration defaults are permitted. */
 c.s.config[0]=(lt48_runtime_configuration_value){-1,0,1,0};c.s.config_count=1;
 c.s.image=malloc(r->capacity);uint16_t *target=malloc(r->capacity),*output=malloc(r->capacity),*work=malloc(r->capacity),*expected=malloc(r->capacity);
 if(!c.s.image||!target||!output||!work||!expected){free(c.s.image);free(target);free(output);free(work);free(expected);return LT48_NO_MEMORY;}
 memset(c.s.image,255,r->capacity);for(unsigned i=0;i<LT48_CAL14_WORDS;i++)target[i]=image?word(image+2*i)&0x3fff:0x3fff;
 lt48_logical_pin data,clock;lt48_status st=lt48_pin_map_find_logical(r->pins,48,&data);
 if(st==LT48_OK)st=lt48_pin_map_find_logical(r->pins,49,&clock);
 if(st==LT48_OK&&data.engine_pin==clock.engine_pin)st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,context,&c.s.wire);
 int upload_pending=0;
 if(st==LT48_OK){
  c.wire=(classic_wire_context){&c.s,data.engine_pin,clock.engine_pin,c.vcc};
  const unsigned char release[]={0x49,0xaa,0x55},fpga[]={0x38,0xaa,0x55},busy[]={0x56,0x0a,1,0x40,1};
  for(unsigned ch=4;st==LT48_OK&&ch;ch--){const unsigned char off[]={0x10,(unsigned char)(ch-1),0};st=lt48_runtime_transport_session_controller_write(c.s.wire,off,sizeof off);}
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,release,sizeof release);
  if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c.s.wire);
  if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(c.s.wire,10000,1);
  if(st==LT48_OK){upload_pending=1;st=lt48_runtime_transport_session_controller_write(c.s.wire,fpga,sizeof fpga);}
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,r->idle,r->idle_size);
  if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c.s.wire);
  if(st==LT48_OK)upload_pending=0;
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,busy,sizeof busy);
 }
 lt48_pic_cal14_result result={0};
 if(st==LT48_OK){
  lt48_pic_classic_wire wire={&c,cal14_transfer,cal14_delay,cal14_reenter};unsigned levels[3],count=0;
  if(margin==LT48_VERIFY_MARGIN_ALL){levels[count++]=0;levels[count++]=5;levels[count++]=6;}
  else levels[count++]=margin==LT48_VERIFY_MARGIN_LOW?5:margin==LT48_VERIFY_MARGIN_HIGH?6:0;
  lt48_pic_cal14_io io={.wire=&wire,.context=&c,.cleanup=cal14_cleanup,.save_factory=cal14_save_factory,.select_voltage=cal14_voltage,.verify_levels=levels,.verify_count=count};
  st=lt48_pic_cal14_execute(&io,op,image?target:NULL,output,work,expected,&result);
  out->operation_status=result.operation_status;out->cleanup_status=result.cleanup_status;out->cleanup_completed=result.cleanup_status==LT48_OK;
  out->first_mismatch=result.first_mismatch;out->mismatch_count=result.mismatches;
  if(st==LT48_OK){out->completed_bytes=r->capacity;if(op==LT48_OPERATION_READ)for(unsigned i=0;i<LT48_CAL14_WORDS;i++)put_word(read+2*i,output[i]);}
 }else{out->operation_status=st;if(c.s.wire)st=finish(&c.s,st,out);if(upload_pending){out->cleanup_completed=0;if(out->cleanup_status==LT48_OK)out->cleanup_status=LT48_PROTOCOL_ERROR;}}
 lt48_pic_source_session_close_internal(c.s.source);lt48_runtime_transport_session_close(c.s.wire);
 free(c.s.image);free(target);free(output);free(work);free(expected);return st;
}
