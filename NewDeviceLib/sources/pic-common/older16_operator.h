#ifndef LT48_PIC_OLDER16_OPERATOR_H
#define LT48_PIC_OLDER16_OPERATOR_H
#include "source_resources.h"
#include "pic17_configuration.h"
#include "../../drafts/pic-population/older16_profiles.h"
#include <stdlib.h>
#include <string.h>

/* Private PIC17/PIC18C owner. The complete source callback table controls
 * target addresses, configuration suffixes, pulses and protection. A source
 * binding is not a silicon qualification or a public admission. In particular
 * the three PIC17C75x constructors use one exact resident no-op exception.
 * Transport/FPGA preparation belongs to the caller and must use this exact
 * resource owner; startup and unconditional release are supplied below. */
typedef struct {
 const o16_profile *profile;
 lt48_pic_source_resources *resources;
 lt48_pic_source_session *session;
 lt48_pic_source_operation_contract operation;
} o16_owner;
typedef struct {
 lt48_pic_source_host host;
 lt48_pic_source_global_finish finish;
 void *finish_context;
 lt48_pic_range_cancelled cancelled;
 unsigned verify_count,verify_centivolts[4];
 unsigned blank_before_program,secure_after_program;
} o16_operation_io;

static inline void o16_close(o16_owner *owner) {
 if(!owner)return;
 lt48_pic_source_session_close_internal(owner->session);
 lt48_pic_source_resources_close_internal(owner->resources);free(owner);
}
static inline int o16_profile_known(const o16_profile *p) {
 for(size_t i=0;i<sizeof older16_profiles/sizeof *older16_profiles;i++)
  if(p==older16_profiles+i)return 1;
 return 0;
}
static inline lt48_status o16_open_policy(const char *path,const o16_profile *p,
 const unsigned char *image,size_t size,unsigned authored,o16_owner **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(authored>1||!path||!o16_profile_known(p)||!image||size!=p->backing_bytes||
    p->source_bytes>size||p->source_bytes%2||!p->code_words||
    p->operation_count>24||p->variable_count>32)return LT48_SAFETY_REJECTED;
 static const unsigned char hash[32]={0x24,0xb8,0x32,0x0d,0xa4,0xcf,0xd7,0x00,0xee,0x43,0x6f,0xf0,0x6b,0xd5,0x5d,0x9d,
  0xfb,0xd4,0x72,0xc5,0x3a,0x02,0x1a,0xd5,0x27,0xc9,0xbb,0xcb,0x3c,0x80,0x4d,0xec};
 o16_owner *o=calloc(1,sizeof *o);if(!o)return LT48_NO_MEMORY;o->profile=p;
 lt48_status st=p->family>=27&&p->family<=39?
  (authored?lt48_pic_source_resources_older16_authored_library_internal:lt48_pic_source_resources_older16_library_internal)(path,hash,(uint16_t)p->family,(uint16_t)p->pinmap,&o->resources):
  (authored?lt48_pic_source_resources_pic18c_authored_library_internal:lt48_pic_source_resources_library_internal)(path,hash,(uint16_t)p->family,(uint16_t)p->pinmap,&o->resources);
 const lt48_pin_map *pins=NULL;const lt48_compile_variable *variables=NULL;size_t count=0;
 if(st==LT48_OK)st=lt48_pic_source_resources_bindings_internal(o->resources,&pins,&variables,&count);
 if(st==LT48_OK&&(count!=p->variable_count||lt48_pic_source_resources_count_internal(o->resources)!=p->operation_count))st=LT48_SAFETY_REJECTED;
 for(size_t i=0;st==LT48_OK&&i<count;i++) {
  const lt48_compile_variable *a=variables+i,*b=p->variables+i;
  if(a->kind!=b->kind||a->variable_id!=b->variable_id||a->enabled!=b->enabled||a->value!=b->value||a->channel!=b->channel)st=LT48_SAFETY_REJECTED;
 }
 for(unsigned i=0;st==LT48_OK&&i<p->operation_count;i++) {
  lt48_pic_source_resource_entry e={0};st=lt48_pic_source_resources_entry_internal(o->resources,i,&e);
  if(st==LT48_OK&&(e.role!=p->roles[i]||e.wave!=p->waves[i]||e.source.wave_type!=p->types[i]||
     (e.source.wave_type==0&&!e.compiled)))st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK&&e.role==9)st=LT48_SAFETY_REJECTED; /* All selected parts are OTP/UV. */
 }
 if(st==LT48_OK&&!lt48_pic_source_geometry_prepare(p->source_bytes/2,16,p->backing_bytes,&o->operation.geometry))st=LT48_SAFETY_REJECTED;
 o->operation.element_bits=16;o->operation.init_role=3;o->operation.reset_role=4;
 o->operation.load_role=6;o->operation.program_role=8;
 o->operation.erase_role=LT48_PIC_ABSENT_ROLE;o->operation.configuration_role=10;
 unsigned char code_size[32]={0};for(unsigned i=0;i<4;i++)code_size[i]=(unsigned char)(p->code_words>>(8*i));
 lt48_runtime_buffer_seed seed={22,code_size,sizeof code_size};
 lt48_runtime_options options={0};options.device_image=image;options.device_image_size=size;
 options.device_element_bytes=2;options.max_steps=100000;options.initial_buffers=&seed;options.initial_buffer_count=1;
 if(st==LT48_OK)st=lt48_pic_source_resources_create_session_internal(o->resources,&options,&o->session);
 /* Exact PIC18C LOAD/PROGRAM and their word helpers use scratch buffers,
  * not the 4MiB backing image. Any undiscovered image access fails closed. */
 if(st==LT48_OK&&p->family>=347&&p->family<=351){
  st=lt48_pic_source_session_omit_callback_image_internal(o->session,6,1);
  if(st==LT48_OK)st=lt48_pic_source_session_omit_callback_image_internal(o->session,8,1);
 }
 if(st!=LT48_OK){o16_close(o);return st;}*out=o;return LT48_OK;
}

static inline lt48_status o16_open(const char *path,const o16_profile *p,
 const unsigned char *image,size_t size,o16_owner **out) {
 return o16_open_policy(path,p,image,size,1,out);
}

typedef struct {
 const o16_operation_io *io;lt48_pic_source_session *stage;
 lt48_pic_source_operation_result *result;uint16_t mode;unsigned touched;
} o16_execution;
static inline lt48_status o16_start(void *v,lt48_pic_source_session *session,uint16_t mode,uint32_t address) {
 o16_execution *e=v;e->touched=1;e->mode=mode;
 lt48_compile_variable voltage;lt48_status st=lt48_pic_source_session_variable_internal(session,0,&voltage);
 lt48_pic_source_host host=e->io->host;
 if(st==LT48_OK){host.voltage_centivolts=voltage.value;st=lt48_pic_source_host_begin_internal(session,&host,mode,address);}
 return st;
}
static inline lt48_status o16_runner(void *v,lt48_runtime_machine *machine,lt48_runtime_result **result) {
 o16_execution *e=v;return e->io->host.runner(e->io->host.context,machine,result);
}
static inline int o16_cancelled(void *v) {
 o16_execution *e=v;return e->io->cancelled&&e->io->cancelled(e->io->host.context);
}
static inline lt48_status o16_finish(void *v) {
 o16_execution *e=v;
 lt48_status source=LT48_OK;
 /* A failed range may have returned before its DEVRESET. Use Verify mode
  * for that reset so an interrupted Read cannot restart a configuration-read
  * suffix during release. Both reset and power-off ignore cancellation. */
 if(e->result->operation_status!=LT48_OK)
  source=lt48_pic_source_session_invoke_wave_internal(e->stage,4,NULL,4,0,10,
   e->io->host.cleanup_runner,e->io->host.context);
 lt48_status off=lt48_pic_source_host_end_internal(e->stage,&e->io->host,
  e->result->operation_status==LT48_OK?e->mode:4);
 if(source==LT48_OK)source=off;
 lt48_status global=e->io->finish(e->io->finish_context,e->result->operation_status,source);
 e->touched=0;return source==LT48_OK?global:source;
}
static inline lt48_status o16_retain_session(void *unused) {
 (void)unused;return LT48_OK;
}
static inline lt48_status o16_configuration_snapshot(o16_owner *owner,o16_execution *execution,
 lt48_pic_source_range_options *options,uint16_t *value) {
 lt48_pic_source_session *read=NULL;
 lt48_status st=lt48_pic_source_session_clone_internal(execution->stage,&read);
 if(st!=LT48_OK)return st;
 o16_execution snapshot=*execution;snapshot.stage=read;snapshot.mode=1;
 lt48_pic_source_range_options selected=*options;selected.context=&snapshot;
 lt48_pic_source_operation_result result;
 st=lt48_pic_source_operation_execute_internal(read,&owner->operation,1,0xfe00,0xfe00,
  &selected,o16_retain_session,NULL,&result);
 execution->touched|=snapshot.touched;execution->mode=1;
 if(execution->touched)options->started=1;
 if(st==LT48_OK) {
  const unsigned char *image;size_t size;st=lt48_pic_source_session_image_internal(read,&image,&size);
  if(st==LT48_OK&&size>=0x1fc02)*value=(uint16_t)(image[0x1fc00]|(unsigned)image[0x1fc01]<<8);
  else if(st==LT48_OK)st=LT48_SAFETY_REJECTED;
 }
 lt48_pic_source_session_close_internal(read);return st;
}

/* PIC18C DEVINIT(Read) obtains all UID/CFG bytes before prefetching code.
 * No code capture is needed here; this remains useful on a protected device.
 * Reset through the exact authored exit before publishing the snapshot. */
static inline lt48_status o16_pic18c_snapshot(o16_owner *owner,o16_execution *execution,
 lt48_pic_source_range_options *options,unsigned char out[15]){
 lt48_pic_source_session *read=NULL;lt48_status st=lt48_pic_source_session_clone_internal(execution->stage,&read);
 if(st!=LT48_OK)return st;
 o16_execution snapshot=*execution;snapshot.stage=read;snapshot.mode=1;snapshot.touched=0;
 unsigned char bytes[15];
 st=lt48_pic_source_session_voltage_value_internal(read,0,500);
 if(st==LT48_OK)st=o16_start(&snapshot,read,1,0);
 if(st==LT48_OK)st=lt48_pic_source_session_invoke_wave_internal(read,3,NULL,1,0,10,o16_runner,&snapshot);
 if(st==LT48_OK){const unsigned char *image;size_t size;st=lt48_pic_source_session_image_internal(read,&image,&size);
  if(st==LT48_OK&&size>=0x300007){memcpy(bytes,image+0x200000,8);memcpy(bytes+8,image+0x300000,7);}
  else if(st==LT48_OK)st=LT48_SAFETY_REJECTED;
 }
 if(snapshot.touched){
  lt48_status reset=lt48_pic_source_session_invoke_wave_internal(read,4,NULL,1,0,10,
   execution->io->host.cleanup_runner,execution->io->host.context);
  if(st==LT48_OK)st=reset;
  execution->touched=1;execution->mode=1;options->started=1;
 }
 if(st==LT48_OK)memcpy(out,bytes,15);
 lt48_pic_source_session_close_internal(read);(void)owner;return st;
}
static inline unsigned o16_pic18c_configuration_mask(unsigned family,unsigned byte){
 static const unsigned masks[7]={255,0x27,15,15,0,1,1};
 return byte>=7?0:byte==5&&family==351?0:masks[byte];
}
static inline lt48_status o16_pic18c_check_configuration(o16_owner *owner,o16_execution *execution,
 lt48_pic_source_range_options *options){
 unsigned char actual[15];lt48_status st=o16_pic18c_snapshot(owner,execution,options,actual);
 if(st!=LT48_OK)return st;
 const unsigned char *expected;size_t size;st=lt48_pic_source_session_image_internal(execution->stage,&expected,&size);
 if(st!=LT48_OK||size<0x300007)return st==LT48_OK?LT48_SAFETY_REJECTED:st;
 for(unsigned i=0;i<15;i++){
  unsigned offset=i<8?0x200000+i:0x300000+i-8;
  unsigned mask=i<8?255:o16_pic18c_configuration_mask(owner->profile->family,i-8);
  if((actual[i]^expected[offset])&mask){execution->result->traversal.first_mismatch=offset/2;return LT48_DEVICE_MISMATCH;}
 }
 return LT48_OK;
}
static inline lt48_status o16_pic18c_secure_prepare(o16_owner *owner,o16_execution *execution,
 lt48_pic_source_range_options *options){
 unsigned char actual[15];lt48_status st=o16_pic18c_snapshot(owner,execution,options,actual);
 if(st!=LT48_OK)return st;
 lt48_pic_source_state state;st=lt48_pic_source_session_state_internal(execution->stage,&state);
 unsigned char *image=st==LT48_OK?malloc(state.image_size):NULL;if(st==LT48_OK&&!image)st=LT48_NO_MEMORY;
 if(st==LT48_OK){memcpy(image,state.image,state.image_size);memcpy(image+0x200000,actual,8);memcpy(image+0x300000,actual+8,7);state.image=image;st=lt48_pic_source_session_restore_internal(execution->stage,&state);}
 free(image);
 /* W209/W20a use the direct current-options object (-1), not named
  * object-table entry0. Their CP field0 value0 programs physical CP=00. */
 const lt48_pic_composition_field field={-1,1,0,0,1,1};const lt48_runtime_configuration_value edit={-1,1,0,0};
 if(st==LT48_OK)st=lt48_pic_source_session_configuration_internal(execution->stage,11,12,8,0,&field,1,&edit,1);
 if(st==LT48_OK){const unsigned char *prepared;size_t size;
  st=lt48_pic_source_session_image_internal(execution->stage,&prepared,&size);
  if(st==LT48_OK&&(size<0x300007||prepared[0x300000]!=0))st=LT48_SAFETY_REJECTED;
 }
 return st;
}

/* OTP cells cannot be restored from zero to one. Read all declared code
 * before any programming pulse, including nonblank-allowed Over Program. */
static inline lt48_status o16_otp_preflight(o16_owner *owner,o16_execution *execution,
 lt48_pic_source_range_options *options) {
 lt48_pic_source_session *read=NULL;
 lt48_status st=lt48_pic_source_session_clone_internal(execution->stage,&read);
 if(st!=LT48_OK)return st;
 o16_execution snapshot=*execution;snapshot.stage=read;snapshot.mode=1;
 lt48_pic_source_range_options selected=*options;selected.context=&snapshot;
 lt48_pic_source_operation_result result;
 st=lt48_pic_source_operation_execute_internal(read,&owner->operation,1,0,owner->profile->code_words-1,
  &selected,o16_retain_session,NULL,&result);
 execution->touched|=snapshot.touched;execution->mode=1;
 if(execution->touched)options->started=1;
 if(st==LT48_OK) {
  const unsigned char *actual,*expected;size_t a,e;
  st=lt48_pic_source_session_image_internal(read,&actual,&a);
  if(st==LT48_OK)st=lt48_pic_source_session_image_internal(execution->stage,&expected,&e);
  if(st==LT48_OK&&(a!=e||a<2*owner->profile->code_words))st=LT48_SAFETY_REJECTED;
  for(unsigned i=0;st==LT48_OK&&i<2*owner->profile->code_words;i++)
   if(expected[i]&~actual[i]) {
    execution->result->traversal.first_mismatch=i/2;st=LT48_SAFETY_REJECTED;
   }
  if(owner->profile->family>=347)for(unsigned i=0;st==LT48_OK&&i<15;i++){
   unsigned offset=i<8?0x200000+i:0x300000+i-8;
   unsigned mask=i<8?255:o16_pic18c_configuration_mask(owner->profile->family,i-8);
   if((expected[offset]&~actual[offset])&mask){execution->result->traversal.first_mismatch=offset/2;st=LT48_SAFETY_REJECTED;}
  }
 }
 lt48_pic_source_session_close_internal(read);return st;
}

/* Retain Dataman's image encoding while using an independently checked
 * physical word in the operation policy. The original Pre/Post result must
 * agree with that source image; no physical PM2 bit is invented in a file. */
static inline lt48_status o16_set_configuration(lt48_pic_source_session *stage,unsigned family,
 uint16_t physical,int convert) {
 uint16_t word;lt48_status st=p17_configuration_image_word(family,physical,&word);
 if(st!=LT48_OK)return st;
 lt48_pic_source_state state;st=lt48_pic_source_session_state_internal(stage,&state);
 if(st!=LT48_OK||state.image_size<0x1fc02)return st==LT48_OK?LT48_SAFETY_REJECTED:st;
 unsigned char *image=malloc(state.image_size);if(!image)return LT48_NO_MEMORY;
 memcpy(image,state.image,state.image_size);image[0x1fc00]=word;image[0x1fc01]=word>>8;
 state.image=image;st=lt48_pic_source_session_restore_internal(stage,&state);free(image);
 if(st==LT48_OK&&convert)st=lt48_pic_source_session_configuration_internal(stage,11,12,2,0,NULL,0,NULL,0);
 if(st==LT48_OK&&convert) {
  st=lt48_pic_source_session_state_internal(stage,&state);
  if(st==LT48_OK&&(state.image[0x1fc00]!=(unsigned char)word||state.image[0x1fc01]!=(unsigned char)(word>>8)))st=LT48_SAFETY_REJECTED;
 }
 return st;
}

/* Full source host operation over a private frame. No speculative read,
 * configuration or scratch mutations reach the caller after operation or
 * cleanup failure. Physical target writes cannot be rolled back. The helper
 * accepts only complete images, retaining PIC18C's 4 MiB backing even though
 * its normal code traversal is much smaller. */
static inline lt48_status o16_execute(o16_owner *o,const o16_operation_io *io,
 lt48_operation_kind op,lt48_pic_source_operation_result *result) {
 if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
 if(!o||!io||!result||!o16_profile_known(o->profile)||!o->session||!o->resources||
    !lt48_pic_source_session_matches_origin_internal(o->session,o->resources)||
    !io->host.write||!io->host.status||!io->host.runner||!io->host.cleanup_runner||!io->finish||
    io->host.voltage_variable||io->host.poweron_role||io->host.poweroff_role!=1||
    io->host.voltage_centivolts!=500||!io->host.minimum_centivolts||
    io->host.minimum_centivolts>500||io->host.maximum_centivolts<500||
    io->verify_count>4||io->blank_before_program>1||io->secure_after_program>1)return LT48_INVALID_ARGUMENT;
 /* Bounds come from PIC17 primary programming specifications, or the
  * source-declared PIC18C VDDmin/VDDmax pair. Caller bounds cannot enlarge it. */
 unsigned minimum=o->profile->family<=36?250:450,maximum=o->profile->family<=36?600:550;
 if(o->profile->family>39) {
  minimum=420;maximum=550;
  for(unsigned i=0;i<o->profile->variable_count;i++) {
   const lt48_compile_variable *v=o->profile->variables+i;
   if(v->kind==0&&v->variable_id==5)minimum=v->value;
   if(v->kind==0&&v->variable_id==6)maximum=v->value;
  }
 }
 if(io->host.minimum_centivolts<minimum||io->host.maximum_centivolts>maximum)return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_ERASE)return result->operation_status=LT48_UNSUPPORTED;
 int program=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 if(!(program||op==LT48_OPERATION_READ||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||
      op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_SECURE))return result->operation_status=LT48_UNSUPPORTED;
 if((program||op==LT48_OPERATION_VERIFY)&&!io->verify_count)return LT48_INVALID_ARGUMENT;
 for(unsigned i=0;i<io->verify_count;i++)
  if(io->verify_centivolts[i]<io->host.minimum_centivolts||io->verify_centivolts[i]>io->host.maximum_centivolts)return LT48_INVALID_ARGUMENT;
 lt48_pic_source_session *stage=NULL;
 lt48_status st=lt48_pic_source_session_clone_internal(o->session,&stage);
 if(st!=LT48_OK)return result->operation_status=st;
 o16_execution e={io,stage,result,0,0};
 lt48_pic_source_range_options options={o->profile->source_bytes,0,0,0,o16_start,o16_runner,o16_cancelled,&e};
 if(o->profile->family<=39&&(program||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)) {
  const unsigned char *image;size_t size;st=lt48_pic_source_session_image_internal(stage,&image,&size);
  if(st==LT48_OK)st=p17_configuration_image_validate(o->profile->family,o->profile->code_words,image,size);
 }
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&o->profile->family<=39) {
  /* Unimplemented addresses have no target data. Start the replacement
   * image at FF and populate only code and physical configuration. */
  lt48_pic_source_state state;st=lt48_pic_source_session_state_internal(stage,&state);
  unsigned char *blank=st==LT48_OK?malloc(state.image_size):NULL;
  if(st==LT48_OK&&!blank)st=LT48_NO_MEMORY;
  if(st==LT48_OK){memset(blank,255,state.image_size);state.image=blank;st=lt48_pic_source_session_restore_internal(stage,&state);}
  free(blank);
 }
 if(st==LT48_OK&&op==LT48_OPERATION_SECURE&&o->profile->family<=39) {
  /* PIC17 PROTECT acts on the processor-mode setting, not an arbitrary
   * configuration word. Read the physical configuration first through the
   * source's mode1 entry/cache path, then change only that declared field.
   * The source secure callback owns its irreversible bit programming order.
   * Protected read scrambling remains a separate qualification obligation. */
  lt48_pic_source_operation_result read;
  e.mode=1;
  st=lt48_pic_source_operation_execute_internal(stage,&o->operation,1,0xfe00,0xfe00,
   &options,o16_retain_session,NULL,&read);
  if(e.touched)options.started=1;
  const lt48_pic_composition_field field={0,1,8,0,3,0};
  const lt48_runtime_configuration_value edit={0,1,8,0};
  if(st==LT48_OK)st=lt48_pic_source_session_configuration_internal(stage,11,12,8,0,&field,1,&edit,1);
  if(st==LT48_OK&&o->profile->family<=36) {
   const unsigned char *image;size_t size;st=lt48_pic_source_session_image_internal(stage,&image,&size);
   if(st==LT48_OK&&size>=0x1fc02) {
    uint16_t word=(uint16_t)(image[0x1fc00]|(unsigned)image[0x1fc01]<<8);
    if(o->profile->family>=29)word&=(uint16_t)~0x8000;
    st=o16_set_configuration(stage,o->profile->family,word,0);
   }else if(st==LT48_OK)st=LT48_SAFETY_REJECTED;
  }
 }
 if(st==LT48_OK&&program&&o->profile->family>36)st=lt48_pic_source_session_configuration_internal(stage,11,12,2,0,NULL,0,NULL,0);
 if(st==LT48_OK&&program&&io->secure_after_program&&o->profile->family>=347){
  const lt48_pic_composition_field field={-1,1,0,0,1,1};const lt48_runtime_configuration_value edit={-1,1,0,0};
  st=lt48_pic_source_session_configuration_internal(stage,11,12,2,0,&field,1,&edit,1);
 }
 if(st==LT48_OK&&op==LT48_OPERATION_SECURE&&o->profile->family>=347)st=o16_pic18c_secure_prepare(o,&e,&options);
 if(st==LT48_OK&&program&&o->profile->family>=347){
  unsigned char actual[15];st=o16_pic18c_snapshot(o,&e,&options,actual);
  if(st==LT48_OK&&actual[8]!=255){result->traversal.first_mismatch=0x180000;st=LT48_SAFETY_REJECTED;}
  if(st==LT48_OK)st=o16_otp_preflight(o,&e,&options);
 }
 unsigned configuration_mask=p17_configuration_mask(o->profile->family),protection=p17_configuration_protection(o->profile->family);
 unsigned configuration_target=0,configuration_before=0,protect_after=io->secure_after_program;
 /* PIC18C source PROTECT is the full UID/CFG programming callback. A full
  * image Program selects the original public configuration stage (1000h),
  * even when its CP field requests unprotected operation. */
 if(program&&o->profile->family>=347)protect_after=1;
 /* C75 DEVINIT does not program settings. Source PROTECT writes the whole
  * configuration word as the original final configuration stage. */
 if(program&&o->profile->family>=37&&o->profile->family<=39)protect_after=1;
 if(st==LT48_OK&&o->profile->family<=39) {
  const unsigned char *image;size_t size;st=lt48_pic_source_session_image_internal(stage,&image,&size);
  if(st==LT48_OK&&size>=0x1fc02)configuration_target=image[0x1fc00]|(unsigned)image[0x1fc01]<<8;
  else if(st==LT48_OK)st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK&&o->profile->family<=39&&(program||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)) {
   lt48_pic17_configuration decoded;st=p17_configuration_image_decode(o->profile->family,(uint16_t)configuration_target,&decoded);
   uint16_t physical;if(st==LT48_OK)st=p17_configuration_encode(o->profile->family,&decoded,&physical);
   if(st==LT48_OK)configuration_target=physical;
  }
  if(program&&!(configuration_target&protection))protect_after=1;
  if(st==LT48_OK&&program)st=o16_set_configuration(stage,o->profile->family,(uint16_t)configuration_target,1);
  if(st==LT48_OK&&program&&o->profile->family<=36)st=o16_otp_preflight(o,&e,&options);
  if(st==LT48_OK&&program) {
   uint16_t actual=0;st=o16_configuration_snapshot(o,&e,&options,&actual);
   configuration_before=actual;
   if(st==LT48_OK&&o->profile->family>=37&&!(actual&protection)){result->traversal.first_mismatch=0xfe00;st=LT48_SAFETY_REJECTED;}
   if(st==LT48_OK&&(configuration_target&~actual&configuration_mask)) {
    result->traversal.first_mismatch=0xfe00;st=LT48_SAFETY_REJECTED;
   }
  }
  if(st==LT48_OK&&program&&o->profile->family>=37)st=o16_otp_preflight(o,&e,&options);
 }
 /* This initial implementation deliberately shares the audited traversal and
  * transaction services. Silicon effects, protected reads, configuration
  * domains and the independent target model remain release gates. */
 uint16_t mode=op==LT48_OPERATION_READ?1:op==LT48_OPERATION_COMPARE?5:
  op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_SECURE?8:
  program?2:4;
 /* Original compact public Program invokes its write phase in mode2.
  * OVER_PROGRAM is the authored nonblank-allowed public operation, not a
  * request for internal mode3. PIC17 DEVINIT raises VPP only for mode2;
  * mode3 therefore cannot perform its write cycle. Preserve the source mode
  * and select blank checking separately. */
 unsigned phases=program?1+io->blank_before_program+io->verify_count+protect_after:
  op==LT48_OPERATION_VERIFY?io->verify_count:1;
 for(unsigned i=0;st==LT48_OK&&i<phases;i++) {
  uint16_t selected=mode;unsigned voltage=500;
  if(program) {
   if(io->blank_before_program&&i==0)selected=11;
   else if(i==io->blank_before_program)selected=mode;
   else if(i<=io->blank_before_program+io->verify_count) {
    selected=4;voltage=io->verify_centivolts[i-io->blank_before_program-1];
   }else selected=8;
  }else if(op==LT48_OPERATION_VERIFY)voltage=io->verify_centivolts[i];
  e.mode=selected;
  if(program&&o->profile->family<=39&&(selected==2||selected==8)) {
   uint16_t word=(uint16_t)configuration_target;
   if(selected==8&&io->secure_after_program) {
    lt48_pic17_configuration c;st=p17_configuration_decode(o->profile->family,word,&c);
    if(st==LT48_OK){c.processor=0;st=p17_configuration_encode(o->profile->family,&c,&word);}
   }
   if(st==LT48_OK)st=o16_set_configuration(stage,o->profile->family,word,1);
  }
  /* DS30274B note1: outside writing MCLR must track VDD. Apply this
   * before every phase, including retained frames that skip host START.
   * The odd-bank DAC steps10cV versus VCC5cV;round MCLR upward.
   * Variable3 remains the independent13V programming rail. */
  if(st==LT48_OK&&o->profile->family>=37&&o->profile->family<=39)
   st=lt48_pic_source_session_voltage_value_internal(stage,4,((voltage+9)/10)*10);
  if(st==LT48_OK)st=lt48_pic_source_session_voltage_value_internal(stage,0,voltage);
  if(st==LT48_OK) {
   lt48_pic_source_operation_result phase;
   /* A no-op phase finish retains the powered frame; the operation below
    * always performs the real source and global finish exactly once. */
   /* Unimplemented image holes produce no target observations. Read,
    * Compare, Verify and Blank traverse code cells and physical CFG only.
    * Program configuration belongs to its source DEVINIT, before code. */
   uint32_t last=o->profile->family<=39&&selected!=8?
    o->profile->code_words-1:o->operation.geometry.last_element;
   st=lt48_pic_source_operation_execute_internal(stage,&o->operation,selected,0,
    last,&options,o16_retain_session,NULL,&phase);
   result->traversal=phase.traversal;
   /* PIC17 DEVINIT mode2 invokes PROGRAMconfig itself before code.
    * The source PROGRAM callback is code-only, so a second INIT at FE00
    * would redundantly program configuration. No suffix is needed. */
   if(e.touched)options.started=1;
   /* Old PIC17 LOAD mode4 echoes expected configuration in its image hole.
    * Read the physical configuration independently through mode1; never
    * count that echo as verification. A requested protected mode is written
    * only after code/configuration verification, as in the source workflow. */
   if(st==LT48_OK&&(selected==1||selected==4||selected==5||selected==11)&&o->profile->family<=39) {
    uint16_t actual=0;st=o16_configuration_snapshot(o,&e,&options,&actual);
    unsigned expected=selected==11?65535:configuration_target;
    if(program&&o->profile->family>=37&&o->profile->family<=39)expected=configuration_before;
    if(selected!=11&&program&&!(configuration_target&protection))expected|=protection;
    if(st==LT48_OK&&selected==1) {
     lt48_pic_source_state state;st=lt48_pic_source_session_state_internal(stage,&state);
     unsigned char *copy=st==LT48_OK?malloc(state.image_size):NULL;
     if(st==LT48_OK&&!copy)st=LT48_NO_MEMORY;
     if(st==LT48_OK){memcpy(copy,state.image,state.image_size);copy[0x1fc00]=actual;copy[0x1fc01]=actual>>8;
      state.image=copy;st=lt48_pic_source_session_restore_internal(stage,&state);}
     free(copy);
    }
    if(st==LT48_OK&&selected!=1&&((actual^expected)&configuration_mask)) {
     result->traversal.first_mismatch=0xfe00;st=LT48_DEVICE_MISMATCH;
    }
   }
  }
 }
 if(st==LT48_OK&&o->profile->family>=37&&o->profile->family<=39&&(program||op==LT48_OPERATION_SECURE)){
  unsigned expected=configuration_target;
  if(program&&io->secure_after_program){lt48_pic17_configuration cfg;uint16_t word;st=p17_configuration_decode(o->profile->family,(uint16_t)expected,&cfg);if(st==LT48_OK){cfg.processor=0;st=p17_configuration_encode(o->profile->family,&cfg,&word);if(st==LT48_OK)expected=word;}}
  uint16_t actual=0;if(st==LT48_OK)st=o16_configuration_snapshot(o,&e,&options,&actual);
  if(st==LT48_OK&&((actual^expected)&configuration_mask)){result->traversal.first_mismatch=0xfe00;st=LT48_DEVICE_MISMATCH;}
 }
 if(st==LT48_OK&&o->profile->family>=347&&(program||op==LT48_OPERATION_SECURE||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_BLANK_CHECK))
  st=o16_pic18c_check_configuration(o,&e,&options);
 if(st==LT48_OK&&program&&o->profile->family<=36&&!protect_after)
  st=o16_set_configuration(stage,o->profile->family,(uint16_t)configuration_target,0);
 result->operation_status=st;
 if(e.touched) {
  result->cleanup_attempted=1;
  lt48_status restore=lt48_pic_source_session_voltage_value_internal(stage,0,500);
  result->cleanup_status=o16_finish(&e);
  if(result->cleanup_status==LT48_OK)result->cleanup_status=restore;
  if(st==LT48_OK)st=result->cleanup_status;
 }
 if(st==LT48_OK) {
  lt48_pic_source_state final;st=lt48_pic_source_session_state_internal(stage,&final);
  if(st==LT48_OK)st=lt48_pic_source_session_restore_internal(o->session,&final);
 }
 lt48_pic_source_session_close_internal(stage);return st;
}

/* Concrete connection to the production controller/USB queues. Resources,
 * FPGA preparation and optional contact/ID startup remain explicit caller
 * obligations; no hardware access occurs while constructing an owner. */
typedef struct {
 o16_owner *owner;lt48_operation_kind operation;const unsigned *voltages;
 size_t count;unsigned blank,secure;
} o16_wire_request;
static inline lt48_status o16_wire_execute(void *context,lt48_pic_source_session *session,
 const lt48_pic_source_host *host,lt48_pic_source_global_finish finish,void *finish_context,
 lt48_pic_range_cancelled cancelled,lt48_pic_source_operation_result *result) {
 o16_wire_request *r=context;
 if(!r||!r->owner||r->owner->session!=session||r->count>4||(r->count&&!r->voltages))
  return LT48_SAFETY_REJECTED;
 o16_operation_io io={0};io.host=*host;io.finish=finish;io.finish_context=finish_context;
 io.cancelled=cancelled;io.verify_count=(unsigned)r->count;
 for(size_t i=0;i<r->count;i++)io.verify_centivolts[i]=r->voltages[i];
 io.blank_before_program=r->blank;io.secure_after_program=r->secure;
 return o16_execute(r->owner,&io,r->operation,result);
}
static inline lt48_status o16_execute_transport(lt48_pic_source_transport *transport,o16_owner *owner,
 lt48_operation_kind operation,const unsigned *verify,size_t count,unsigned blank,unsigned secure,
 lt48_pic_source_operation_result *result) {
 o16_wire_request request={owner,operation,verify,count,blank,secure};
 return lt48_pic_source_transport_compact_owner_internal(transport,o16_wire_execute,&request,result);
}
#endif
