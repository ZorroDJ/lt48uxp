/* Four exact source bindings; this private adapter grants no catalogue support. */
static lt48_status pic_tail14_open(const lt48_installation *x,const lt48_device_spec *spec,const lt48_pic_tail14_profile *p,lt48_pic_resources **out){
 if(!x||!spec||!p||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(p->policy>=3||spec->catalogue.device_class!=2015||!spec->definition_stem||strcmp(spec->definition_stem,"GADC___")||spec->element_bits!=14)return LT48_UNSUPPORTED;
 if(spec->definition_resource!=p->definition||spec->family_code!=p->family||spec->pinmap!=p->pinmap||spec->capacity_bytes!=p->capacity)return LT48_SAFETY_REJECTED;
 lt48_pic_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
 r->tail14=p;r->profile=&pic_cal14_source_profile;r->capacity=p->capacity;r->program_words=2048;r->core=14;
 char path[4096],name[32],hex[65];size_t idx,fi;unsigned char sha[32];lt48_status st;
#define T14TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
 if(snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x))>=(int)sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
 T14TRY(lt48_rez_open(path,&r->rez));T14TRY(lt48_rez_sha256(r->rez,sha));
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",sha[i]);
 if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec")){st=LT48_SAFETY_REJECTED;goto fail;}
 snprintf(name,sizeof name,"P%x",p->pinmap);T14TRY(lt48_rez_find_resource(r->rez,name,&idx));lt48_pin_code pc;
 T14TRY(lt48_rez_get_pin_code(r->rez,idx,&pc));T14TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",p->family);T14TRY(lt48_rez_find_resource(r->rez,name,&fi));lt48_family_info f;T14TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count!=p->var_count||f.variable_count>16||f.operation_count!=10){st=LT48_SAFETY_REJECTED;goto fail;}
 r->var_count=p->var_count;
 for(unsigned i=0;i<r->var_count;i++){
  lt48_family_variable v;T14TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
  r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
  if(memcmp(r->vars+i,p->vars+i,sizeof *r->vars)){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 for(unsigned i=0;i<10;i++){
  lt48_family_operation op;T14TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
  if(op.operation_id!=p->roles[i]||op.wave!=p->waves[i]){st=LT48_SAFETY_REJECTED;goto fail;}
  if(op.operation_id!=0&&op.operation_id!=1&&op.operation_id!=3&&op.operation_id!=4)continue;
  snprintf(name,sizeof name,"W%x",op.wave);T14TRY(lt48_rez_find_resource(r->rez,name,&idx));lt48_wave_code w;
  T14TRY(lt48_rez_get_wave_code(r->rez,idx,&w));if(w.wave_type){st=LT48_UNSUPPORTED;goto fail;}
  lt48_compact_wave *raw=NULL;T14TRY(lt48_compact_decode(w.data,w.data_size,&raw));
  st=lt48_compact_compile(raw,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(raw);
  if(st!=LT48_OK)goto fail;
  if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 T14TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_pic_close(r);return st;
#undef T14TRY
}
/* Reuse the already-qualified electrical session/entry/cleanup adapter.
 * Its context contains no family-dependent factory or memory geometry. */
static lt48_status tail14_commit(void *v,unsigned command,unsigned us,unsigned end){
 cal14_context *c=v;unsigned char packet[20];size_t n=0;
 if(!((command==24&&((us==2000&&end==14)||(us==3500&&end==10)))||
      (command==8&&!end&&(us==3000||us==6000))))return LT48_SAFETY_REJECTED;
 unsigned data=c->wire.data,clock=c->wire.clock;
 packet[n++]=0x58;packet[n++]=(unsigned char)data;packet[n++]=(unsigned char)clock;packet[n++]=5;packet[n++]=(unsigned char)command;
 packet[n++]=0x4f;packet[n++]=(unsigned char)(us/100);packet[n++]=(unsigned char)(us/25600);
 if(end){packet[n++]=0x58;packet[n++]=(unsigned char)data;packet[n++]=(unsigned char)clock;packet[n++]=5;packet[n++]=(unsigned char)end;}
 packet[n++]=0x4e;packet[n++]=100;
 return lt48_runtime_transport_session_controller_write(c->s.wire,packet,n);
}
static lt48_status pic_tail14_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,lt48_pic_result *out){
 cal14_context c={0};c.s.r=r;c.s.api=api;c.s.context=context;c.s.mode=8;c.result=out;c.vcc=500;
 memcpy(c.s.vars,r->vars,sizeof c.s.vars);c.s.config[0]=(lt48_runtime_configuration_value){-1,0,1,0};c.s.config_count=1;
 c.s.image=malloc(r->capacity);uint16_t *target=malloc(r->capacity),*output=malloc(r->capacity),*work=malloc(2*LT48_TAIL14_WORK_WORDS),*expected=malloc(2*LT48_TAIL14_WORK_WORDS);
 if(!c.s.image||!target||!output||!work||!expected){free(c.s.image);free(target);free(output);free(work);free(expected);return LT48_NO_MEMORY;}
 memset(c.s.image,255,r->capacity);for(unsigned i=0;i<r->capacity/2;i++)target[i]=image?word(image+2*i)&0x3fff:0x3fff;
 lt48_logical_pin data,clock;lt48_status st=lt48_pin_map_find_logical(r->pins,r->tail14->data_pin,&data);
 if(st==LT48_OK)st=lt48_pin_map_find_logical(r->pins,r->tail14->clock_pin,&clock);
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
 lt48_pic_tail14_result result={0};
 if(st==LT48_OK){
  lt48_pic_classic_wire wire={&c,cal14_transfer,cal14_delay,cal14_reenter};unsigned levels[3],count=0;
  if(margin==LT48_VERIFY_MARGIN_ALL){levels[count++]=0;levels[count++]=5;levels[count++]=6;}
  else levels[count++]=margin==LT48_VERIFY_MARGIN_LOW?5:margin==LT48_VERIFY_MARGIN_HIGH?6:0;
  lt48_pic_tail14_io io={.wire=&wire,.context=&c,.cleanup=cal14_cleanup,.select_voltage=cal14_voltage,.commit=tail14_commit,.verify_levels=levels,.verify_count=count};
  st=lt48_pic_tail14_execute(lt48_pic_tail14_policies+r->tail14->policy,&io,op,image?target:NULL,output,work,expected,&result);
  out->operation_status=result.operation_status;out->cleanup_status=result.cleanup_status;out->cleanup_completed=result.cleanup_status==LT48_OK;
  out->first_mismatch=result.first_mismatch;out->mismatch_count=result.mismatches;
  if(st==LT48_OK){out->completed_bytes=r->capacity;if(op==LT48_OPERATION_READ)for(unsigned i=0;i<r->capacity/2;i++)put_word(read+2*i,output[i]);}
 }else{out->operation_status=st;if(c.s.wire)st=finish(&c.s,st,out);if(upload_pending){out->cleanup_completed=0;if(out->cleanup_status==LT48_OK)out->cleanup_status=LT48_PROTOCOL_ERROR;}}
 lt48_pic_source_session_close_internal(c.s.source);lt48_runtime_transport_session_close(c.s.wire);
 free(c.s.image);free(target);free(output);free(work);free(expected);return st;
}
