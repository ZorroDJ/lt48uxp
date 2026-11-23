/* STK10C68-C45 qualification runner. Datasheet-bound hardware NE procedure.
 * Shares the production C compact compiler/runtime, idle FPGA and USB bridge.
 * STK11C68 uses a separately admitted software sequence and unmodified pin map. */
#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct lt48_stk10c68_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *waves[6];
 lt48_compile_variable v;lt48_runtime_transport_session *wire;
 const lt48_protocol_transport *api;void *context;
 unsigned software_store;
 unsigned char buffers[9][32],*idle;size_t idle_size;uint32_t volts[4];
} device;
static lt48_status compile(device*d,unsigned index,const unsigned char *body,size_t len){
 unsigned char raw[256];if(len+2>sizeof raw)return LT48_INVALID_ARGUMENT;raw[0]=len;raw[1]=len>>8;memcpy(raw+2,body,len);
 lt48_compact_wave*c=NULL;lt48_status s=lt48_compact_decode(raw,len+2,&c);
 if(s==LT48_OK)s=lt48_compact_compile(c,d->pins,&d->v,1,1,0,&d->waves[index]);
 lt48_compact_wave_close(c);if(s==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(d->waves[index]))s=LT48_SAFETY_REJECTED;return s;
}
static lt48_status resource_wave(device*d,const char*name,unsigned index,int full_address){
 size_t i;lt48_wave_code w;lt48_status s=lt48_rez_find_resource(d->rez,name,&i);
 if(s==LT48_OK)s=lt48_rez_get_wave_code(d->rez,i,&w);
 if(s!=LT48_OK)return s;
 if(w.wave_type||w.data_size<3||w.data_size>250)return LT48_FORMAT_ERROR;
 unsigned char body[256];size_t len=w.data_size-2;memcpy(body,w.data+2,len);
 if(full_address){if(body[0]!=0x15)return LT48_SAFETY_REJECTED;body[0]=0x16;}
 if(index==0&&!d->software_store){const unsigned char suffix[]={0x11,32,1,0x12,5,2,0x16,4,0,0x32,0x10,0x27,0,0};memcpy(body+len,suffix,sizeof suffix);len+=sizeof suffix;}
 return compile(d,index,body,len);
}
#include "../../NewDeviceLib/sources/stk11one/profiles.h"
#include "../../NewDeviceLib/sources/stk11one/policy.inc"
static lt48_status open_device(device*d,const char*root){
 char path[4096],hex[65];unsigned char digest[32];size_t i;lt48_pin_code code;
 snprintf(path,sizeof path,"%s/GAB____.rez",root);lt48_status s=lt48_rez_open(path,&d->rez);
 if(s==LT48_OK)s=lt48_rez_sha256(d->rez,digest);
 if(s!=LT48_OK)return s;
 for(i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
 if(strcmp(hex,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9"))return LT48_SAFETY_REJECTED;
 s=lt48_rez_find_resource(d->rez,"P29",&i);
 if(s==LT48_OK)s=lt48_rez_get_pin_code(d->rez,i,&code);
 if(s!=LT48_OK)return s;
 if(d->software_store)return stk11one_policy(d,&code,root);
 /* P29's address/data/power buses match STK10C68. Add NE at chip pin 1
  * (logical 32); remove that pin from the donor's NC group. Keep pin26 NC. */
 unsigned char map[256];size_t out=3,pos=3;if(code.pin_count!=28||code.data_size>240||code.data[0]!=5||code.data[1]!=4)return LT48_SAFETY_REJECTED;
 memcpy(map,code.data,3);map[1]=5;
 for(unsigned g=0;g<5;g++){unsigned kind=code.data[pos]|code.data[pos+1]<<8,n=code.data[pos+2]|code.data[pos+3]<<8;pos+=4;
  map[out++]=kind;map[out++]=kind>>8;map[out++]=(kind==3)?1:n;map[out++]=0;
  if(kind==3){if(n!=2||code.data[pos]!=0||code.data[pos+1]!=25)return LT48_SAFETY_REJECTED;map[out++]=25;}else{memcpy(map+out,code.data+pos,n);out+=n;}pos+=n;
 }
 memcpy(map+out,code.data+pos,code.data_size-pos);out+=code.data_size-pos;
 map[out++]=32;map[out++]=0;map[out++]=0;map[out++]=0;lt48_pin_code native={28,map,out};
 s=lt48_pin_map_parse(&native,&d->pins);d->v=(lt48_compile_variable){0,0,1,500,0};
 if(s==LT48_OK)s=resource_wave(d,"Wc8",0,0);
 if(s==LT48_OK)s=resource_wave(d,"W2",1,0);
 if(s==LT48_OK)s=resource_wave(d,"Wcb",2,1);
 if(s==LT48_OK)s=resource_wave(d,"Wcc",3,1);
 /* CE high isolates every setup transition. OE high prevents accidental
  * RECALL during STORE; WE high prevents accidental STORE during RECALL. */
 const unsigned char store[]={0x11,4,1,0x11,3,1,0x11,17,1,0x12,5,2,
  0x11,32,0,0x11,17,0,0x11,4,0,0x32,100,0,0,0,
  0x11,4,1,0x11,17,1,0x11,32,1,0x32,0x20,0x4e,0,0};
 const unsigned char recall[]={0x11,4,1,0x11,17,1,0x12,5,2,0x11,3,0,
  0x11,32,0,0x11,4,0,0x32,100,0,0,0,
  0x11,4,1,0x11,32,1,0x11,3,1,0x32,0xe8,3,0,0};
 if(s==LT48_OK)s=compile(d,4,store,sizeof store);
 if(s==LT48_OK)s=compile(d,5,recall,sizeof recall);
 if(s==LT48_OK)s=lt48_load_verified_idle_fpga_internal(root,&d->idle,&d->idle_size);
 return s;
}
static lt48_status run(device*d,unsigned op){
 lt48_runtime_options o={0};lt48_runtime_nested_wave root={op,d->waves[op]};
 o.nested_waves=&root;o.nested_wave_count=1;
 lt48_runtime_buffer_seed seeds[8];unsigned n=0;lt48_runtime_machine*m=NULL;lt48_runtime_result*r=NULL;
 for(unsigned i=0;i<9;i++)if(i!=4)seeds[n++]=(lt48_runtime_buffer_seed){i,d->buffers[i],32};
 o.mode=1;o.use_explicit_buffer_ids=1;o.mode_buffer=4;o.address_buffer=0;o.parameter_buffer_id=2;o.initial_buffers=seeds;o.initial_buffer_count=n;o.root_timeout_seconds=10;
 memcpy(o.initial_voltage_centivolts,d->volts,sizeof d->volts);
 lt48_status s=lt48_runtime_machine_create(d->waves[op],d->pins,&d->v,1,&o,&m);
 if(s==LT48_OK)s=lt48_runtime_transport_session_run(d->wire,m,&r);
 if(s==LT48_OK){for(unsigned i=0;i<9;i++){const unsigned char*b;size_t z;s=lt48_runtime_get_buffer(r,i,&b,&z);if(s!=LT48_OK||z!=32){s=LT48_RUNTIME_ERROR;break;}memcpy(d->buffers[i],b,32);}
  for(unsigned i=0;i<4&&s==LT48_OK;i++)s=lt48_runtime_get_voltage(r,i,&d->volts[i]);}
 lt48_runtime_result_close(r);lt48_runtime_machine_close(m);return s;
}
static lt48_status barrier(device*d){return lt48_runtime_transport_session_status(d->wire,1);}
static lt48_status read_bytes(device*d,unsigned char*b){
 /* PROGRAM leaves the data drivers active; release them before reading. */
 const unsigned char init[]={0x12,5,2};lt48_compiled_wave *old=d->waves[5];d->waves[5]=NULL;
 lt48_status s=compile(d,5,init,sizeof init);
 if(s==LT48_OK)s=run(d,5);
 lt48_compiled_wave_close(d->waves[5]);d->waves[5]=old;
 for(unsigned i=0;s==LT48_OK&&i<8192;i++){for(unsigned j=0;j<4;j++)d->buffers[0][j]=(unsigned char)(i>>(8*j));s=run(d,2);
 if(s==LT48_OK)b[i]=d->buffers[1][0];
 if(d->api->progress)d->api->progress(d->context,LT48_EXECUTION_READ_OPERATION,i+1,8192,0,0,0);}
 return s;
}
static lt48_status write_bytes(device*d,const unsigned char*b,size_t n){
 for(unsigned i=0;i<n;i++){for(unsigned j=0;j<4;j++)d->buffers[0][j]=(unsigned char)(i>>(8*j));d->buffers[2][0]=b[i];lt48_status s=run(d,3);
 if(s!=LT48_OK)return s;}return barrier(d);
}

void lt48_stk10c68_close(lt48_stk10c68_resources *r) {
 if(!r)return;
 for(unsigned i=0;i<6;i++)lt48_compiled_wave_close(r->waves[i]);
 lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
uint32_t lt48_stk10c68_capacity(const lt48_stk10c68_resources *r){return r?8192:0;}
lt48_status lt48_stk10c68_open(const lt48_installation *x,size_t index,lt48_stk10c68_resources **out) {
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec spec;
 lt48_status s=lt48_installation_resolve_device(x,index,&spec);
 if(s!=LT48_OK)return s;
 int soft=stk11one_binding(&spec);
 if(!soft&&(spec.catalogue.device_class!=2010 || spec.definition_resource!=0xfffe ||
    spec.catalogue.device_name_length!=strlen("STK10C68-C45") ||
    memcmp(spec.catalogue.device_name,"STK10C68-C45",strlen("STK10C68-C45")) ||
    spec.capacity_bytes!=8192 || spec.pin_count!=28))return LT48_UNSUPPORTED;
 device *d=calloc(1,sizeof(*d));if(!d)return LT48_NO_MEMORY;d->software_store=soft;
 s=open_device(d,lt48_installation_path(x));
 if(s!=LT48_OK){lt48_stk10c68_close(d);return s;}
 *out=d;return LT48_OK;
}
lt48_status lt48_stk10c68_validate(const lt48_stk10c68_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
 if(!r || !length || start>=8192 || length>8192-start)return LT48_SAFETY_REJECTED;
 if(op<LT48_OPERATION_READ || op>LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
 if(op==LT48_OPERATION_ERASE && (start || length!=8192))return LT48_SAFETY_REJECTED;
 if(op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_ERASE && (!image || size!=length))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
lt48_status lt48_stk10c68_validate_verify_options(const lt48_stk10c68_resources *r,lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type) {
 if(!r || type>1)return LT48_INVALID_ARGUMENT;
 if(margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
 /* Only nominal voltage has been admitted for this native extension. */
 if(op==LT48_OPERATION_VERIFY && margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL)return LT48_UNSUPPORTED;
 return LT48_OK;
}
static lt48_status compare_bytes(const unsigned char *actual,const unsigned char *expected,uint32_t start,size_t n,lt48_stk10c68_result *out) {
 for(size_t i=0;i<n;i++)if(actual[i]!=expected[i]){
  if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
  out->mismatch_count++;
 }
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
lt48_status lt48_stk10c68_execute(const lt48_stk10c68_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_stk10c68_result *out) {
 if(!out || !api)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 lt48_status s=lt48_stk10c68_validate(r,op,start,length,image,size);
 if(s==LT48_OK)s=lt48_stk10c68_validate_verify_options(r,op,margin,type);
 if(s==LT48_OK && op==LT48_OPERATION_READ && (!read || read_size<length))s=LT48_BUFFER_TOO_SMALL;
 if(s!=LT48_OK){out->operation_status=s;return s;}
 device d=*r;d.wire=NULL;d.api=api;d.context=context;
 unsigned char actual[8192],expected[8192];
 int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE;
 int store_attempted=0;
 s=lt48_runtime_transport_session_create(api,context,&d.wire);
 if(s==LT48_OK)s=lt48_runtime_transport_session_wave_watchdog(d.wire,10000,1);
 for(unsigned ch=0;s==LT48_OK&&ch<4;ch++){unsigned char off[]={0x10,ch,0};s=lt48_runtime_transport_session_controller_write(d.wire,off,sizeof off);}
 if(s==LT48_OK)s=barrier(&d);
 const unsigned char head[]={0x38,0xaa,0x55};
 if(s==LT48_OK)s=lt48_runtime_transport_session_controller_write(d.wire,head,sizeof head);
 if(s==LT48_OK)s=lt48_runtime_transport_session_controller_write(d.wire,d.idle,d.idle_size);
 if(s==LT48_OK)s=run(&d,0);
 if(s==LT48_OK)s=barrier(&d);
 const unsigned char busy[]={0x56,0x0a,1,0x40,1};
 if(s==LT48_OK)s=lt48_runtime_transport_session_controller_write(d.wire,busy,sizeof busy);
 if(s==LT48_OK)s=run(&d,5);
 if(s==LT48_OK)s=read_bytes(&d,actual);
 if(s==LT48_OK && writing) {
  memcpy(expected,actual,8192);
  if(op==LT48_OPERATION_ERASE)memset(expected,255,8192);else memcpy(expected+start,image,length);
  s=write_bytes(&d,expected,8192);
  if(s==LT48_OK)s=read_bytes(&d,actual);
  if(s==LT48_OK)s=compare_bytes(actual,expected,0,8192,out);
  /* A failed SRAM verification must never STORE a partial/bad image. */
  if(s==LT48_OK){store_attempted=1;s=run(&d,4);}
  if(s==LT48_OK)s=barrier(&d);
  if(store_attempted){lt48_status hold=lt48_nvram_store_hold_internal();if(s==LT48_OK)s=hold;}
  if(s==LT48_OK)s=run(&d,1);
  if(s==LT48_OK)s=barrier(&d);
  if(s==LT48_OK)s=lt48_nvram_store_hold_internal();
  if(s==LT48_OK)s=run(&d,0);
  if(s==LT48_OK)s=barrier(&d);
  if(s==LT48_OK&&d.software_store)s=run(&d,5);
  if(s==LT48_OK)s=read_bytes(&d,actual);
  if(s==LT48_OK)s=compare_bytes(actual,expected,0,8192,out);
 }else if(s==LT48_OK && op!=LT48_OPERATION_READ){
  if(op==LT48_OPERATION_BLANK_CHECK){memset(expected,255,length);image=expected;}
  s=compare_bytes(actual+start,image,start,length,out);
 }
 out->operation_status=s;
 /* Cleanup cannot be cancelled, and tries every rail independently. */
 lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
 lt48_runtime_transport_session_close(d.wire);d.wire=NULL;
 lt48_status clean=lt48_runtime_transport_session_create(&cleanup,context,&d.wire);
 if(clean==LT48_OK)clean=run(&d,1);
 if(clean==LT48_OK)clean=lt48_runtime_transport_session_flush(d.wire);
 for(unsigned ch=0;ch<4;ch++) {
  lt48_runtime_transport_session_close(d.wire);d.wire=NULL;
  lt48_status t=lt48_runtime_transport_session_create(&cleanup,context,&d.wire);
  unsigned char off[]={0x10,ch,0};
  if(t==LT48_OK)t=lt48_runtime_transport_session_controller_write(d.wire,off,3);
  if(t==LT48_OK)t=lt48_runtime_transport_session_flush(d.wire);
  if(clean==LT48_OK)clean=t;
 }
 const unsigned char finish[]={0x49,0xaa,0x55};
 lt48_status t=lt48_runtime_transport_session_controller_write(d.wire,finish,3);
 if(t==LT48_OK)t=lt48_runtime_transport_session_flush(d.wire);
 if(clean==LT48_OK)clean=t;
 unsigned char led[]={0x40,(s==LT48_OK&&clean==LT48_OK)?4:2};
 t=lt48_runtime_transport_session_controller_write(d.wire,led,2);
 if(t==LT48_OK)t=lt48_runtime_transport_session_flush(d.wire);
 if(clean==LT48_OK)clean=t;
 out->cleanup_status=clean;out->cleanup_completed=clean==LT48_OK;
 lt48_runtime_transport_session_close(d.wire);
 if(s==LT48_OK)s=clean;
 if(s==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,actual+start,length);}
 return s;
}
