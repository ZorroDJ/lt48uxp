#include <labtool48/families_generated.h>
#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exact Dataman 13.10 serial PROM procedures: compact early parts and
 * XC17S A / XC17V RAM applets. Admission is generated from audited tuples. */
struct lt48_xc17_resources {
    lt48_rez *rez; lt48_pin_map *pins; lt48_compiled_wave *wave[32];
    lt48_compile_variable vars[32];size_t var_count;
    lt48_applet_address address[3];
    const unsigned char *applet[3];size_t applet_size[3];
    unsigned char *idle;size_t idle_size;unsigned capacity,width,check_id,legacy,apps,cfgbits,writable,nominal,margins[3],margin_count;
};
void lt48_xc17_close(lt48_xc17_resources*r){if(!r)return;for(unsigned i=0;i<32;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);}
unsigned lt48_xc17_configuration_bits(const lt48_xc17_resources*r){return r?r->cfgbits:0;}
uint32_t lt48_xc17_capacity(const lt48_xc17_resources*r){return r?r->capacity:0;}
lt48_status lt48_xc17_open(const lt48_installation*x,size_t device,lt48_xc17_resources**out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,device,&spec);if(st!=LT48_OK)return st;
 unsigned wide=spec.element_bits==128;
 if(spec.catalogue.device_class!=LT48_FAMILY_XC17_DEVICE_CLASS || strcmp(spec.definition_stem,"GAB____") ||
    !(lt48_family_xc17_admitted(spec.definition_resource,spec.family_code,spec.pinmap,spec.capacity_bytes,spec.element_bits)||lt48_family_serial_prom_admitted(spec.definition_resource,spec.family_code,spec.pinmap,spec.capacity_bytes,spec.element_bits)||lt48_family_serial_prom_config_admitted(spec.definition_resource,spec.family_code,spec.pinmap,spec.capacity_bytes,spec.element_bits)))return LT48_UNSUPPORTED;
 lt48_xc17_resources*r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;r->capacity=spec.capacity_bytes;r->width=spec.element_bits/8;r->legacy=spec.family_code<=0x34;r->cfgbits=r->legacy?(spec.family_code<1||spec.family_code>4):(wide?3:1);r->writable=r->legacy?spec.family_code>=5:1;
 char path[4096],name[32];size_t i,fi;
 int n=snprintf(path,sizeof(path),"%s/GAB____.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof(path)){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,LT48_FAMILY_XC17_SOURCE_SHA256,&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&i));TRY(lt48_rez_get_pin_code(r->rez,i,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 lt48_family_info f;snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count>32||(!r->legacy&&(f.variable_count!=(wide?18:16)||f.operation_count!=(wide?13:11)))){st=LT48_FORMAT_ERROR;goto fail;}
 r->var_count=f.variable_count;for(i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 for(i=0;i<r->var_count;i++)if(r->vars[i].variable_id==0)r->nominal=r->vars[i].value;
 for(unsigned id=8;id<=10;id++)for(i=0;i<r->var_count;i++)if(r->vars[i].variable_id==id&&r->vars[i].value)r->margins[r->margin_count++]=r->vars[i].value;
 if(!r->margin_count){r->margins[0]=r->nominal==330?314:475;r->margins[1]=r->nominal==330?346:525;r->margin_count=2;}
 if(r->nominal!=330&&r->nominal!=500){st=LT48_FORMAT_ERROR;goto fail;}
 lt48_family_operation_options options;TRY(lt48_rez_get_family_operation_options(r->rez,fi,&options));r->check_id=!!(options.checkbox_values&2);
 unsigned apps=0;
 for(size_t k=0;k<f.operation_count;k++){
  lt48_family_operation op;lt48_wave_code w;lt48_compact_wave*c=NULL;TRY(lt48_rez_get_family_operation(r->rez,fi,k,&op));
  if(op.operation_id>=32||r->wave[op.operation_id]){st=LT48_FORMAT_ERROR;goto fail;}
  snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&i));TRY(lt48_rez_get_wave_code(r->rez,i,&w));
  if(w.wave_type==6){
   unsigned addr=op.operation_id==9?0x5000:op.operation_id==11?(wide?0x7079:0x5068):op.operation_id==12?(wide?0x7000:0x5032):0;
   unsigned size=op.operation_id==9?50:op.operation_id==11?(wide?320:407):op.operation_id==12?(wide?121:54):0;
   if(!addr||apps==3||w.load_address!=addr||w.data_size!=size){st=LT48_FORMAT_ERROR;goto fail;}
   r->address[apps]=(lt48_applet_address){op.operation_id,(uint16_t)addr};r->applet[apps]=w.data;r->applet_size[apps++]=size;continue;
  }
  if(w.wave_type){st=LT48_UNSUPPORTED;goto fail;}
  TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile(c,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
  if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 r->apps=apps; if(apps!=(r->legacy?0:3)){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_xc17_close(r);return st;
#undef TRY
}
const char *lt48_xc17_profile_internal(const lt48_xc17_resources*r){return !r->legacy?LT48_FAMILY_XC17_PROFILE:r->writable?LT48_FAMILY_SERIAL_PROM_CONFIG_PROFILE:LT48_FAMILY_SERIAL_PROM_PROFILE;}
unsigned lt48_xc17_mask_internal(const lt48_xc17_resources*r){return 0x3fu|(r->writable?(1u<<(LT48_OPERATION_CONFIGURATION-1)):0);}
lt48_status lt48_xc17_validate(const lt48_xc17_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op<1||op>LT48_OPERATION_KIND_COUNT||!(lt48_xc17_mask_internal(r)&(1u<<(op-1))))return LT48_UNSUPPORTED;
 /* Original LOAD/PROGRAM advance a streaming cursor, not the address buffer. */
 if(start||n!=r->capacity)return LT48_SAFETY_REJECTED;
 if(op>=3&&op<=6&&(!image||size!=n))return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_CONFIGURATION&&(!image||size!=1||image[0]>=(1u<<lt48_xc17_configuration_bits(r))))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
lt48_status lt48_xc17_validate_verify_options(const lt48_xc17_resources*r,lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type){
 (void)op;if(!r||type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;return LT48_OK;
}
typedef struct {const lt48_xc17_resources*r;const lt48_protocol_transport*api;void*context;lt48_runtime_transport_session*wire;lt48_compile_variable vars[32];uint32_t volts[4];unsigned char buf[32][32];unsigned mode;lt48_runtime_configuration_value config[16];size_t config_count;} session;
static void setcfg(session*s,int obj,unsigned cat,unsigned field,unsigned value){for(size_t i=0;i<s->config_count;i++)if(s->config[i].object_id==obj&&s->config[i].category==cat&&s->config[i].field==field){s->config[i].value=value;return;}if(s->config_count<16)s->config[s->config_count++]=(lt48_runtime_configuration_value){obj,cat,field,value};}
static unsigned getcfg(session*s,unsigned cat,unsigned field){for(size_t i=0;i<s->config_count;i++)if(s->config[i].object_id==0&&s->config[i].category==cat&&s->config[i].field==field)return s->config[i].value;return UINT32_MAX;}
static lt48_status run(session*s,unsigned op){
 if(op>=32||!s->r->wave[op])return LT48_UNSUPPORTED;
 lt48_runtime_options o={0};lt48_runtime_nested_wave nested[32];size_t nn=0;lt48_runtime_buffer_seed seeds[31];size_t ns=0;
 for(unsigned i=0;i<32;i++){if(s->r->wave[i])nested[nn++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};if(i!=4)seeds[ns++]=(lt48_runtime_buffer_seed){i,s->buf[i],32};}
 o.mode=s->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=4;o.parameter_buffer_id=1;o.initial_buffers=seeds;o.initial_buffer_count=ns;o.nested_waves=nested;o.nested_wave_count=nn;o.configuration=s->config;o.configuration_count=s->config_count;o.root_timeout_seconds=10;memcpy(o.initial_voltage_centivolts,s->volts,sizeof(s->volts));
 return lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&o,s->r->address,s->r->apps,s->buf,s->config,16,&s->config_count,s->volts);
}

static lt48_status start(session*s,unsigned mode,unsigned vcc){
 memset(s->buf,0,sizeof(s->buf));s->mode=mode;for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
 lt48_status st=run(s,0);for(unsigned i=0;st==LT48_OK&&i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);if(st==LT48_OK)st=run(s,2);return st;
}
static lt48_status end(session*s,lt48_status st){
 /* An unsuccessful write must never commit pending configuration changes. */
 if(st!=LT48_OK){s->mode=1;return st;}
 st=run(s,3);return st==LT48_OK?run(s,1):st;
}
static unsigned configuration(session*s){
 if(s->r->width!=16)return getcfg(s,1,0);
 unsigned mask=0;for(unsigned i=0;i<3;i++){unsigned b=getcfg(s,0,23+i);if(b>1)return UINT32_MAX;mask|=b<<i;}return mask;
}
static void config_set(session*s,unsigned mask){if(s->r->width!=16)setcfg(s,0,1,0,mask);else for(unsigned i=0;i<3;i++)setcfg(s,0,0,23+i,(mask>>i)&1);}
static lt48_status read_config(session*s,lt48_xc17_result*out){
 if(!s->r->cfgbits)return LT48_OK;
 lt48_status st=start(s,1,s->r->nominal);if(st==LT48_OK){unsigned value=configuration(s);if(value>=(1u<<lt48_xc17_configuration_bits(s->r)))st=LT48_RUNTIME_ERROR;else{out->configuration_valid=1;out->configuration=value;}}return end(s,st);
}
static lt48_status scan(session*s,lt48_operation_kind op,const unsigned char*expected,unsigned char*dest,unsigned vcc,lt48_xc17_result*out){
 unsigned width=s->r->width;lt48_status st=start(s,op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_VERIFY?4:1,vcc);
 for(unsigned a=0;st==LT48_OK&&a<s->r->capacity;a+=width){
  unsigned element=a/width;for(unsigned i=0;i<4;i++)s->buf[0][i]=element>>(8*i);
  st=run(s,4);if(st!=LT48_OK)break;
  if(dest)memcpy(dest+a,s->buf[1],width);
  if(op!=LT48_OPERATION_READ)for(unsigned b=0;b<width;b++)if(s->buf[1][b]!=(op==LT48_OPERATION_BLANK_CHECK?255:expected[a+b])){if(!out->mismatch_count)out->first_mismatch=a+b;out->mismatch_count++;}
  out->completed_bytes=a+width;if(s->api->progress&&!(element&127))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,a+width,s->r->capacity,0,0,0);
  if(out->mismatch_count&&op!=LT48_OPERATION_COMPARE)st=LT48_DEVICE_MISMATCH;
 }
 st=end(s,st);return st==LT48_OK&&out->mismatch_count?LT48_DEVICE_MISMATCH:st;
}
static lt48_status cleanup_wave(void *owner,unsigned op){return run(owner,op);}
static lt48_status finish(session*s,lt48_status operation,lt48_xc17_result*out){
 const unsigned prefix[]={25,1};s->mode=1;
 return lt48_owner_finish_internal(s->api,s->context,&s->wire,cleanup_wave,s,prefix+(s->r->legacy?1:0),s->r->legacy?1:2,operation,&out->cleanup_status,&out->cleanup_completed);
}

lt48_status lt48_xc17_execute(const lt48_xc17_resources*r,const lt48_protocol_transport*api,void*context,lt48_operation_kind op,uint32_t offset,size_t n,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_xc17_result*out){
 if(!api||!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_xc17_validate(r,op,offset,n,image,size);if(st!=LT48_OK)return st;st=lt48_xc17_validate_verify_options(r,op,margin,type);if(st!=LT48_OK)return st;
 if(op==LT48_OPERATION_READ&&(!read||read_size<n))return LT48_BUFFER_TOO_SMALL;
 unsigned char*pending=malloc(n);if(!pending)return LT48_NO_MEMORY;
 session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;memcpy(s.vars,r->vars,sizeof(s.vars));setcfg(&s,-1,0,1,r->check_id);setcfg(&s,-1,0,5,0);
 st=lt48_runtime_transport_session_create(api,context,&s.wire);if(st!=LT48_OK)goto done;
 const unsigned char busy[]={0x56,0x0a,1,0x40,1};
 st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
 if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
 for(unsigned i=0;st==LT48_OK&&i<r->apps;i++)st=lt48_owner_load_applet_internal(s.wire,r->address[i].address,r->applet[i],r->applet_size[i]);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
 if(st==LT48_OK)st=read_config(&s,out);
 unsigned original=out->configuration;
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM)st=scan(&s,LT48_OPERATION_BLANK_CHECK,NULL,NULL,r->nominal,out);
 if(st==LT48_OK&&(writing||op==LT48_OPERATION_CONFIGURATION)){
  unsigned want=op==LT48_OPERATION_CONFIGURATION?image[0]:original;
  /* OTP configuration: 64-bit polarity can only go High -> Low; the three
   * 128-bit options can only be set. Refuse an impossible reversal first. */
  if((r->width!=16&&want>original)||(r->width==16&&(original&~want)))st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK&&op==LT48_OPERATION_CONFIGURATION)st=scan(&s,LT48_OPERATION_READ,NULL,pending,r->nominal,out);
  if(st==LT48_OK){if(r->cfgbits)config_set(&s,want);st=start(&s,2,r->nominal);}
  for(unsigned a=0;st==LT48_OK&&writing&&a<n;a+=r->width){
   unsigned e=a/r->width;for(unsigned i=0;i<4;i++)s.buf[0][i]=e>>(8*i);memcpy(s.buf[2],image+a,r->width);memcpy(s.buf[1],image+a,r->width);st=run(&s,5);
   if(st==LT48_OK&&memcmp(s.buf[1],image+a,r->width)){out->first_mismatch=a;out->mismatch_count++;st=LT48_DEVICE_MISMATCH;}
   if(api->progress&&!(e&127))api->progress(context,LT48_EXECUTION_READ_OPERATION,a+r->width,n,0,0,0);
  }
  st=end(&s,st);
  const unsigned char*expected=writing?image:pending;
  for(unsigned i=0;st==LT48_OK&&i<r->margin_count;i++)st=scan(&s,LT48_OPERATION_VERIFY,expected,NULL,r->margins[i],out);
  if(st==LT48_OK)st=read_config(&s,out);
  if(st==LT48_OK&&r->cfgbits&&out->configuration!=want)st=LT48_DEVICE_MISMATCH;
  if(st==LT48_OK)st=scan(&s,LT48_OPERATION_VERIFY,expected,NULL,r->nominal,out);
 }else if(st==LT48_OK&&op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_NOMINAL){
  for(unsigned i=0;st==LT48_OK&&i<r->margin_count;i++)if(margin==LT48_VERIFY_MARGIN_ALL||(margin==LT48_VERIFY_MARGIN_LOW&&i==0)||(margin==LT48_VERIFY_MARGIN_HIGH&&i==1))st=scan(&s,op,image,NULL,r->margins[i],out);
 }else if(st==LT48_OK)st=scan(&s,op,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,out);
 out->operation_status=st;st=finish(&s,st,out);if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,n);if(st==LT48_OK)out->completed_bytes=n;
done:free(pending);return st;
}
