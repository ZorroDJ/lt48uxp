#ifndef LT48_PIC_MIDRANGE14_ADAPTER_H
#define LT48_PIC_MIDRANGE14_ADAPTER_H
/* Included by the native PIC owner after its source/runtime helpers. */
static const lt48_pic_profile pic_midrange14_source_profile={.strategy=LT48_PIC_COMPACT_ROWS};
static const lt48_pic_midrange14_profile *pic_midrange14_match(const lt48_device_spec *spec){
 if(!spec||spec->catalogue.device_class!=2015||!spec->definition_stem||strcmp(spec->definition_stem,"GADC___")||
    spec->element_bits!=14||(spec->capacity_bytes!=16400&&spec->capacity_bytes!=16404)||strcmp(spec->catalogue.vendor,"Microchip"))return NULL;
 for(size_t i=0;i<sizeof(pic_midrange14_profiles)/sizeof(*pic_midrange14_profiles);i++){
  const lt48_pic_midrange14_profile *p=pic_midrange14_profiles+i;
  if(spec->definition_resource==p->definition&&spec->family_code==p->family&&spec->pinmap==p->pinmap&&
     spec->catalogue.device_name_length==strlen(p->name)&&!memcmp(spec->catalogue.device_name,p->name,strlen(p->name)))return p;
 }return NULL;
}
static lt48_status pic_midrange14_open(const lt48_installation *x,const lt48_device_spec *spec,
 const lt48_pic_midrange14_profile *p,lt48_pic_resources **out){
 if(!x||!spec||!p||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(pic_midrange14_match(spec)!=p||!lt48_pic_midrange_valid(&p->protocol))return LT48_SAFETY_REJECTED;
 lt48_pic_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
 r->midrange14=p;r->profile=&pic_midrange14_source_profile;r->capacity=LT48_MIDRANGE_IMAGE_WORDS*2;r->core=14;r->program_words=p->protocol.code_words;
 char path[4096],name[32],hex[65];unsigned char sha[32];size_t idx,fi;lt48_status st;
#define MTRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
 int n=snprintf(path,sizeof(path),"%s/GADC___.rez",lt48_installation_path(x));
 if(n<0||(size_t)n>=sizeof(path)){st=LT48_INVALID_ARGUMENT;goto fail;}
 MTRY(lt48_rez_open(path,&r->rez));MTRY(lt48_rez_sha256(r->rez,sha));
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",sha[i]);
 if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec")){st=LT48_SAFETY_REJECTED;goto fail;}
 snprintf(name,sizeof(name),"P%x",p->pinmap);lt48_pin_code pins;
 MTRY(lt48_rez_find_resource(r->rez,name,&idx));MTRY(lt48_rez_get_pin_code(r->rez,idx,&pins));MTRY(lt48_pin_map_parse(&pins,&r->pins));
 snprintf(name,sizeof(name),"M%x",p->family);lt48_family_info info;
 MTRY(lt48_rez_find_resource(r->rez,name,&fi));MTRY(lt48_rez_get_family_info(r->rez,fi,&info));
 if(info.variable_count!=10||info.operation_count!=10){st=LT48_SAFETY_REJECTED;goto fail;}r->var_count=10;
 const unsigned ids[]={0,5,6,3},selected[]={p->vcc,p->low,p->high,p->vpp};unsigned seen=0;
 for(unsigned i=0;i<10;i++){
  lt48_family_variable v;MTRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
  for(unsigned j=0;j<4;j++)if(r->vars[i].variable_id==ids[j]){
   if((seen&(1u<<j))||r->vars[i].kind||r->vars[i].value!=p->source_voltages[j]){st=LT48_SAFETY_REJECTED;goto fail;}
   seen|=1u<<j;r->vars[i].value=(uint16_t)selected[j];
  }
  if(r->vars[i].variable_id==19&&r->vars[i].value!=p->protocol.code_words){st=LT48_SAFETY_REJECTED;goto fail;}
  if(r->vars[i].variable_id==20&&r->vars[i].value!=p->protocol.latches){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 if(seen!=15){st=LT48_SAFETY_REJECTED;goto fail;}
 for(unsigned i=0;i<10;i++){
  lt48_family_operation op;MTRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
  if(op.operation_id>=32||r->wave[op.operation_id]){st=LT48_SAFETY_REJECTED;goto fail;}
  snprintf(name,sizeof(name),"W%x",op.wave);lt48_wave_code w;lt48_compact_wave *raw=NULL;
  MTRY(lt48_rez_find_resource(r->rez,name,&idx));MTRY(lt48_rez_get_wave_code(r->rez,idx,&w));
  if(w.wave_type){st=LT48_UNSUPPORTED;goto fail;}
  MTRY(lt48_compact_decode(w.data,w.data_size,&raw));
  st=lt48_compact_compile(raw,r->pins,r->vars,10,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(raw);if(st!=LT48_OK)goto fail;
  if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 const unsigned roles[]={0,1,3,4,6,8,9,10,11,12};
 for(unsigned i=0;i<10;i++)if(!r->wave[roles[i]]){st=LT48_SAFETY_REJECTED;goto fail;}
 for(unsigned role=3;role<=4;role++){
  lt48_compiled_wave *changed=NULL;MTRY(lt48_pic_midrange_entry_compile(r->pins,r->vars,10,role,&changed));
  lt48_compiled_wave_close(r->wave[role]);r->wave[role]=changed;
 }
 MTRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;
 return LT48_OK;
fail:lt48_pic_close(r);
 return st;
#undef MTRY
}
typedef struct {pic_session s;classic_wire_context wire;lt48_pic_result *result;unsigned vcc,cleaned;} midrange14_context;
static lt48_status midrange14_transfer(void *v,int read,unsigned bits,uint32_t value,uint32_t *out){return classic_wire_transfer(&((midrange14_context*)v)->wire,read,bits,value,out);}
static lt48_status midrange14_delay(void *v,unsigned us){return classic_wire_delay(&((midrange14_context*)v)->wire,us);}
static lt48_status midrange14_reenter(void *v){
 midrange14_context *c=v;lt48_status st=reset(&c->s);
 for(unsigned i=4;st==LT48_OK&&i;i--){const unsigned char off[]={0x10,(unsigned char)(i-1),0};st=lt48_runtime_transport_session_controller_write(c->s.wire,off,sizeof(off));}
 if(st==LT48_OK)st=midrange14_delay(c,10000);
 if(st==LT48_OK)st=p17_start(&c->s,8,c->vcc);
 return st;
}
static lt48_status midrange14_cleanup(void *v,lt48_status operation){
 midrange14_context *c=v;c->cleaned++;lt48_runtime_transport_session_close(c->s.wire);c->s.wire=NULL;
 lt48_protocol_transport clean=*c->s.api;clean.cancelled=NULL;
 lt48_status first=lt48_runtime_transport_session_create(&clean,c->s.context,&c->s.wire);
 if(first==LT48_OK)first=reset(&c->s);
 (void)finish(&c->s,operation==LT48_OK?first:operation,c->result);
 if(first!=LT48_OK)c->result->cleanup_status=first;
 return c->result->cleanup_status;
}
static lt48_status midrange14_voltage(void *v,unsigned level){
 midrange14_context *c=v;if(level!=0&&level!=5&&level!=6)return LT48_INVALID_ARGUMENT;
 c->vcc=p17_voltage(c->s.r,level);
 return c->vcc?LT48_OK:LT48_SAFETY_REJECTED;
}
static lt48_status midrange14_commit(void *v,const lt48_pic_midrange_policy *p,unsigned address){
 midrange14_context *c=v;unsigned char bytes[17];size_t n;lt48_status st=lt48_pic_midrange_commit_packet(p,address,c->wire.data,c->wire.clock,bytes,sizeof(bytes),&n);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c->s.wire,bytes,n);
 return st;
}
static lt48_status pic_midrange14_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,lt48_pic_result *out){
 if(op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_OVER_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)image=NULL;
 midrange14_context c={0};int upload_pending=0;c.s.r=r;c.s.api=api;c.s.context=context;c.s.mode=8;c.result=out;c.vcc=p17_voltage(r,0);memcpy(c.s.vars,r->vars,sizeof(c.s.vars));
 size_t n=r->capacity;c.s.image=malloc(n);uint16_t *target=malloc(n),*output=malloc(n),*work=malloc(n),*expected=malloc(n);
 if(!c.s.image||!target||!output||!work||!expected){free(c.s.image);free(target);free(output);free(work);free(expected);
 return LT48_NO_MEMORY;}
 memset(c.s.image,255,n);for(unsigned i=0;i<n/2;i++)target[i]=image?word(image+2*i):0x3fff;
 lt48_logical_pin data,clock;lt48_status st=lt48_pin_map_find_logical(r->pins,52,&data);
 if(st==LT48_OK)st=lt48_pin_map_find_logical(r->pins,53,&clock);
 if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,context,&c.s.wire);
 if(st==LT48_OK){c.wire=(classic_wire_context){&c.s,data.engine_pin,clock.engine_pin,c.vcc};
  for(unsigned ch=4;st==LT48_OK&&ch;ch--){const unsigned char off[]={0x10,(unsigned char)(ch-1),0};st=lt48_runtime_transport_session_controller_write(c.s.wire,off,sizeof(off));}
  const unsigned char release[]={0x49,0xaa,0x55},fpga[]={0x38,0xaa,0x55},busy[]={0x56,0x0a,1,0x40,1};
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,release,sizeof(release));
  if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c.s.wire);
  if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(c.s.wire,10000,1);
  if(st==LT48_OK){upload_pending=1;st=lt48_runtime_transport_session_controller_write(c.s.wire,fpga,sizeof(fpga));}
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,r->idle,r->idle_size);
  if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c.s.wire);
  if(st==LT48_OK)upload_pending=0;
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,busy,sizeof(busy));
 }
 if(st==LT48_OK){
  const lt48_pic_midrange14_profile *p=r->midrange14;
  lt48_pic_midrange_operation_policy policy={&p->protocol,p->configuration_mask,p->code_protect,p->identity_mask,p->identity_value};
  lt48_pic_classic_wire wire={&c,midrange14_transfer,midrange14_delay,midrange14_reenter};
  unsigned levels[3],count=0;
  if(margin==LT48_VERIFY_MARGIN_ALL){levels[count++]=0;levels[count++]=5;levels[count++]=6;}
  else levels[count++]=margin==LT48_VERIFY_MARGIN_LOW?5:margin==LT48_VERIFY_MARGIN_HIGH?6:0;
  lt48_pic_midrange_io io={&wire,&c,midrange14_cleanup,levels,count,midrange14_voltage,midrange14_commit};lt48_pic_midrange_result result;
  st=lt48_pic_midrange_execute(&policy,&io,op,image?target:NULL,output,work,expected,&result);
  /* Invalid input has no logical wire activity and therefore no owner
   * cleanup callback. The outer adapter still owns the uploaded idle engine. */
  if(!c.cleaned){result.operation_status=st;result.cleanup_status=midrange14_cleanup(&c,st);}
  out->operation_status=result.operation_status;out->cleanup_status=result.cleanup_status;out->cleanup_completed=result.cleanup_status==LT48_OK;
  out->mismatch_count=result.mismatches;out->first_mismatch=result.first_mismatch;
  if(st==LT48_OK){out->completed_bytes=n;if(op==LT48_OPERATION_READ)for(unsigned i=0;i<n/2;i++)put_word(read+2*i,output[i]);}
 }else {
  out->operation_status=st;if(c.s.wire)st=finish(&c.s,st,out);
  /* The resident FPGA stream cannot be cancelled/reframed by assumption. */
  if(upload_pending){out->cleanup_completed=0;if(out->cleanup_status==LT48_OK)out->cleanup_status=LT48_PROTOCOL_ERROR;}
 }
 lt48_pic_source_session_close_internal(c.s.source);lt48_runtime_transport_session_close(c.s.wire);
 free(c.s.image);free(target);free(output);free(work);free(expected);
 return st;
}
#endif
