/* Exact GAD compact HC705 instruction-injection procedures. Source resources
 * remain BYO. No new FPGA or programmer applet, no electrical erase. */
#include "../../../c/src/hc705_compact.h"
#include "../../../c/src/internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "admission.h"
struct lt48_hc705_compact_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[8];
 lt48_compile_variable vars[5];unsigned capacity,kind,stage;
 unsigned char *idle;size_t idle_size;
};
static unsigned mor(const lt48_hc705_compact_resources *r){return r->kind==1?0x17:0x7f1;}
static int cell(const lt48_hc705_compact_resources *r,unsigned a){return a==mor(r)||(a>=(r->kind==1?0x200u:0x300u)&&a<=(r->kind==1?0x3efu:0x7cfu))||a>=r->capacity-8;}
void lt48_hc705_compact_close(lt48_hc705_compact_resources *r){if(!r)return;for(unsigned i=0;i<8;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);}
/* Keep the source instruction/address algorithm, but make the programming
 * dwell explicit and keep EPGM enable/delay/disable in one bridge packet.
 * The original raw-group path flushes between bytes; it cannot itself prove
 * a minimum dwell. Authored fixed-byte writes use the same idle-FPGA GPIOs.
 * No original REZ is modified or bundled. */
static lt48_status program_wave(const lt48_wave_code *w,const lt48_pin_code *pc,const lt48_pin_map *source,unsigned kind,unsigned char **out,size_t *out_size,lt48_pin_map **map){
 lt48_compact_wave *cw=NULL;lt48_status st=lt48_compact_decode(w->data,w->data_size,&cw);if(st)return st;
 lt48_pin_group data;st=lt48_pin_map_find_group(source,8,&data);
 if(st||data.pin_count!=8||pc->data_size>60000||pc->data[1]>247){lt48_compact_wave_close(cw);return LT48_SAFETY_REJECTED;}
 unsigned char *pins=malloc(pc->data_size+32),*raw=malloc(w->data_size+1200);
 if(!pins||!raw){free(pins);free(raw);lt48_compact_wave_close(cw);return LT48_NO_MEMORY;}
 memcpy(pins,pc->data,pc->data_size);pins[1]+=8;
 for(unsigned k=0;k<8;k++){
  lt48_logical_pin exists;if(lt48_pin_map_find_logical(source,(uint16_t)(128+k),&exists)!=LT48_NOT_FOUND){st=LT48_SAFETY_REJECTED;goto done;}
  unsigned pos=data.pins[k]/4-(48-pc->pin_count)/2;unsigned char *p=pins+pc->data_size+4*k;
  p[0]=(unsigned char)(128+k);p[1]=0;p[2]=(unsigned char)pos;p[3]=0;
 }
 lt48_pin_code extended={pc->pin_count,pins,pc->data_size+32};st=lt48_pin_map_parse(&extended,map);if(st)goto done;
 size_t n=2;unsigned active=0,index=0,complete=0,value=0;
 for(size_t j=0;j<lt48_compact_instruction_count(cw);j++){
  lt48_compact_instruction i;st=lt48_compact_get_instruction(cw,j,&i);if(st)goto done;
  if(i.opcode==0x40&&i.operand_size==6&&i.operands[0]==5&&i.operands[1]==4){
   value=(unsigned)i.operands[2]|(unsigned)i.operands[3]<<8|(unsigned)i.operands[4]<<16|(unsigned)i.operands[5]<<24;
   if(!active&&(value==0x10||value==0x12)){active=1;index=0;}
   if(active&&value==0x3f){unsigned us=kind==1?10000:4000;raw[n++]=0x32;for(unsigned k=0;k<4;k++)raw[n++]=(unsigned char)(us>>(8*k));}
  }
  if(active&&i.opcode==0x16&&i.operand_size==2&&i.operands[0]==8&&i.operands[1]==5){
   unsigned alternative=index==1&&(value==0x10||value==0x12);
   if((index==0&&value!=0x10&&value!=0x12)||(index==1&&!alternative&&value!=0x18)||(index==2&&value!=0x3f)||(index==3&&value!=0x18)||index>3){st=LT48_SAFETY_REJECTED;goto done;}
   for(unsigned k=0;k<8;k++){raw[n++]=0x11;raw[n++]=(unsigned char)(128+k);raw[n++]=(unsigned char)((value>>k)&1);}
   if(!alternative&&++index==4){active=0;complete++;}
  }else{size_t z=1+i.operand_size;memcpy(raw+n,w->data+i.offset,z);n+=z;}
 }
 if(active||complete!=(kind==1?3u:2u)||n>65537){st=LT48_SAFETY_REJECTED;goto done;}
 raw[0]=(unsigned char)(n-2);raw[1]=(unsigned char)((n-2)>>8);*out=raw;*out_size=n;raw=NULL;
done:free(pins);free(raw);lt48_compact_wave_close(cw);if(st){lt48_pin_map_close(*map);*map=NULL;}return st;
}
lt48_status lt48_hc705_compact_open_candidate(const lt48_installation *x,size_t device,lt48_hc705_compact_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);if(st!=LT48_OK)return st;
 const char *names[]={"MC68HC705K1","MC68HC705J1A","MC68HC705KJ1"};unsigned kind=0;
 for(unsigned i=0;i<3;i++)if(s.catalogue.device_name_length==strlen(names[i])&&!memcmp(s.catalogue.device_name,names[i],strlen(names[i])))kind=i+1;
 if(!kind||!s.catalogue.vendor||(strcmp(s.catalogue.vendor,"Motorola")&&strcmp(s.catalogue.vendor,"Freescale"))||s.catalogue.device_class!=2012||strcmp(s.definition_stem,"GAD____")||s.catalogue.manufacturer_id||s.catalogue.device_id||s.element_bits!=8)return LT48_UNSUPPORTED;
 /* Source-relative D/M labels changed in 7.60. The exact normalized device,
  * variables, pins and operation payloads are identical; no name-only reuse. */
 const char *profile=lt48_installation_source_profile(x),*source_hash=NULL;unsigned label_offset=0;
 if(!strcmp(profile,"Dataman 13.10"))source_hash="aa366e3c14e1e088730223f8be3e1343a16c551039f95fcdf7d101bc45c46706";
 else if(!strcmp(profile,"AEC 14.30"))source_hash="0ebe7b02a1369cf6008291e78557ab86fd5013850d873d8ab054a941416f4573";
 else if(!strcmp(profile,"AEC 7.60")){source_hash="a0c26a04f9ad03df4741f4e4aae41b3ce62d9444db915a17dc5b23e203867412";label_offset=2;}
 else return LT48_UNSUPPORTED;
 if(s.definition_resource!=0x3e + kind-label_offset||s.family_code!=0x1f + kind-label_offset||s.pinmap!=0x21 + kind||s.capacity_bytes!=(kind==1?1024u:2048u))return LT48_SAFETY_REJECTED;
 lt48_hc705_compact_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->capacity=(unsigned)s.capacity_bytes;r->kind=kind;r->stage=kind*2;
 char path[4096],name[24];size_t index,fi;int n=snprintf(path,sizeof path,"%s/GAD____.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(a) do{st=(a);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,source_hash,&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",s.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 lt48_family_info family;snprintf(name,sizeof name,"M%x",s.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&family));
 if(family.variable_count!=5||family.operation_count!=(kind==1?6u:7u)){st=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<5;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 unsigned seen=0;
 for(size_t i=0;i<family.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;lt48_compact_wave *cw=NULL;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
  if(op.operation_id>7||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<op.operation_id;
  snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));if(w.wave_type){st=LT48_UNSUPPORTED;goto fail;}
  unsigned char *authored=NULL;size_t authored_size=w.data_size;lt48_pin_map *owned_map=NULL;
  if(op.operation_id==5){st=program_wave(&w,&pc,r->pins,kind,&authored,&authored_size,&owned_map);if(st!=LT48_OK)goto fail;}
  st=lt48_compact_decode(authored?authored:w.data,authored_size,&cw);
  if(st==LT48_OK)st=lt48_compact_compile_with_pin_policy(cw,owned_map?owned_map:r->pins,r->vars,5,1,kind==3&&op.operation_id==4?LT48_PIN_FALLBACK_RESIDENT_NOOP:LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);
  free(authored);lt48_pin_map_close(owned_map);lt48_compact_wave_close(cw);if(st!=LT48_OK)goto fail;
  size_t defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
  if(defaults!=(kind==3&&op.operation_id==4?2u:0u)){st=LT48_SAFETY_REJECTED;goto fail;}
  for(size_t k=0;k<defaults;k++){lt48_defaulted_logical_pin d;TRY(lt48_compiled_wave_get_defaulted_pin(r->wave[op.operation_id],k,&d));if(d.logical_pin!=20+k||d.compact_offset!=854+3*k||d.compact_opcode!=0x11||d.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP){st=LT48_SAFETY_REJECTED;goto fail;}}
 }
 if(seen!=(kind==1?0x3fu:0xbfu)){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_hc705_compact_close(r);return st;
#undef TRY
}
lt48_status lt48_hc705_compact_open(const lt48_installation *x,size_t device,lt48_hc705_compact_resources **out){return lt48_hc705_compact_open_candidate(x,device,out);}
int lt48_hc705_compact_qualified(const lt48_hc705_compact_resources *r){return r&&r->stage<=LT48_HC705_COMPACT_STAGE;}
lt48_status lt48_hc705_compact_validate(const lt48_hc705_compact_resources *r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *image,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_UNSUPPORTED;
 if(start||n!=r->capacity)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||size!=n))return LT48_INVALID_ARGUMENT;
 if((image&&size!=n)||(!image&&size))return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned a=0;a<n;a++)if(!cell(r,a)&&image[a]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
typedef struct {
 const lt48_hc705_compact_resources *r;lt48_runtime_transport_session *wire;
 unsigned char buffers[32][32];lt48_compile_variable vars[5];uint32_t volts[4];
 unsigned address,mode;const unsigned char *image;int poisoned;
} session;
static int transport_error(lt48_status st){return st==LT48_USB_ERROR||st==LT48_PROTOCOL_ERROR||st==LT48_TIMEOUT||st==LT48_CANCELLED;}
static lt48_status run(void *context,unsigned op){
 session *s=context;if(op>=8||!s->r->wave[op])return LT48_UNSUPPORTED;
 lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[7];size_t n=0;
 for(unsigned i=0;i<9;i++)if(i!=0&&i!=3)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 o.initial_buffers=seeds;o.initial_buffer_count=n;o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=3;o.parameter_buffer_id=2;
 o.device_image=s->image;o.device_image_size=s->image?s->r->capacity:0;o.device_element_bytes=1;
 memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 lt48_runtime_configuration_value cfg[1];size_t count=0;
 lt48_status st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,5,&o,NULL,0,s->buffers,cfg,1,&count,s->volts);
 if(transport_error(st))s->poisoned=1;
 return st;
}
static void vcc(session *s,unsigned value){for(unsigned i=0;i<5;i++)if(s->vars[i].variable_id==0)s->vars[i].value=value;}
static lt48_status cycle(session *s,int write,unsigned voltage,const unsigned char *image,unsigned char *output){
 /* Independent Read is used for comparisons; source Verify's expected-image
  * MOR adjustment cannot manufacture a matching byte in this owner. */
 s->mode=write?2:1;s->address=0;s->image=image;vcc(s,voltage);memset(s->buffers,0,sizeof s->buffers);
 lt48_status st=run(s,0);if(st==LT48_OK)st=run(s,2);
 for(unsigned a=0;st==LT48_OK&&a<s->r->capacity;a++){
  /* Address zero runs source factory-mode entry even though it is padding.
   * Do not pulse reserved addresses or rewrite preserved MOR. The original
   * J1A source also visits COPR at7F0 and reserved7F2..7F7; the Linux
   * image contract exposes the user array, MOR and vectors only. */
  if(write&&a&&(!cell(s->r,a)||a==mor(s->r)))continue;
  s->address=a;s->buffers[2][0]=write?image[a]:0;
  st=run(s,write?5:4);
  if(st==LT48_OK&&!write)output[a]=cell(s->r,a)?s->buffers[1][0]:255;
  /* Source PROGRAM does not provide raw per-byte readback. Only the fresh
   * source READ cycles below can verify the mutation; BUF1 may be stale. */
 }
 lt48_status done=s->poisoned?LT48_CLEANUP_FAILED:run(s,3);return st==LT48_OK?done:st;
}
static lt48_status compare(const lt48_hc705_compact_resources *r,const unsigned char *actual,const unsigned char *wanted,lt48_small_eprom_result *out){
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<r->capacity;a++)if(cell(r,a)&&actual[a]!=(wanted?wanted[a]:0)){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
/* Cleanup must not issue a new controller header after a partial frame or
 * FPGA body. A fresh host queue cannot repair unknown device framing. */
static lt48_status finish(session *s,const lt48_protocol_transport *api,void *context,lt48_status operation){
 lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
 if(s->poisoned)return LT48_CLEANUP_FAILED;
 lt48_protocol_transport clean=*api;clean.cancelled=NULL;
 for(unsigned phase=0;phase<8;phase++){
  lt48_status st=lt48_runtime_transport_session_create(&clean,context,&s->wire);
  if(st==LT48_OK){
   if(!phase)st=run(s,1);
   else if(phase<=4){const unsigned char off[]={0x10,(unsigned char)(4-phase),0};st=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof off);}
   else if(phase==5)st=lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
   else if(phase==6){const unsigned char done[]={0x49,0xaa,0x55};st=lt48_runtime_transport_session_controller_write(s->wire,done,sizeof done);}
   else{const unsigned char led[]={0x40,operation==LT48_OK?4:2};st=lt48_runtime_transport_session_controller_write(s->wire,led,sizeof led);}
   if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
  }
  lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
  if(st!=LT48_OK){if(transport_error(st))s->poisoned=1;return st;}
 }
 return LT48_OK;
}
lt48_status lt48_hc705_compact_execute_internal(const lt48_hc705_compact_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_hc705_compact_validate(r,op,start,length,image,size,margin,type);
 if(st==LT48_OK&&(!api||!api->read||!api->write))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){out->operation_status=st;return st;}
 session s={0};s.r=r;memcpy(s.vars,r->vars,sizeof s.vars);unsigned char actual[2048];
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
 if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
 if(transport_error(st))s.poisoned=1;
 unsigned delta=type?50:25,first=op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_LOW?500-delta:op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;
 if(st==LT48_OK)st=cycle(&s,0,first,NULL,actual);
 /* EPMSEC protects J1A/KJ1; never publish a protected/fabricated image. */
 if(st==LT48_OK&&r->kind!=1&&(actual[mor(r)]&0x40))st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM){
  if(actual[mor(r)]!=image[mor(r)])st=LT48_SAFETY_REJECTED;
  for(unsigned a=0;st==LT48_OK&&a<r->capacity;a++)if(cell(r,a)&&a!=mor(r)&&actual[a])st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK)st=cycle(&s,1,500,image,NULL);
 }
 if(st==LT48_OK){
  if(op==LT48_OPERATION_PROGRAM){st=cycle(&s,0,500,NULL,actual);if(st==LT48_OK)st=compare(r,actual,image,out);}
  else if(op!=LT48_OPERATION_READ)st=compare(r,actual,op==LT48_OPERATION_BLANK_CHECK?NULL:image,out);
 }
 if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||(op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL))){
  const unsigned volts[]={500-delta,500+delta};for(unsigned i=0;i<2&&st==LT48_OK;i++){st=cycle(&s,0,volts[i],NULL,actual);if(st==LT48_OK)st=compare(r,actual,image,out);}
 }
 out->operation_status=st;
 out->cleanup_status=finish(&s,api,context,st);out->cleanup_completed=out->cleanup_status==LT48_OK;
 if(st==LT48_OK)st=out->cleanup_status;
 if(st==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,actual,length);}return st;
}

/* The audit entry is private. Production callers cannot bypass admission with
 * a stale resource pointer or by calling the transport wrapper directly. */
lt48_status lt48_hc705_compact_execute(const lt48_hc705_compact_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 if(!lt48_hc705_compact_qualified(r)){memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;out->operation_status=LT48_SAFETY_REJECTED;return LT48_SAFETY_REJECTED;}
 return lt48_hc705_compact_execute_internal(r,api,context,op,start,length,image,size,read,read_size,margin,type,out);
}
