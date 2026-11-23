#ifndef LT48_PIC_REMAINING14_MODERN_SOURCE_H
#define LT48_PIC_REMAINING14_MODERN_SOURCE_H
#include "remaining14_modern_policies.h"
#include "remaining14_modern_wire.h"
#include "../../../c/src/internal.h"
#include "otp14_source_adapter.h"
typedef struct {
 rm14_resources *binding;otp14_source_adapter source;rm14_otp_policy policy;unsigned verify_selection;
 lt48_pic_classic_wire wire;lt48_status (*word_timeout)(void *,unsigned);rm14_modern_pulse pulse;lt48_status (*flush)(void *);
} rm14_modern_source;
static inline void rm14_modern_source_close(rm14_modern_source *a){if(a){rm14_close(a->binding);free(a);}}
static inline lt48_status rm14_modern_source_export(rm14_modern_source *a) {
 lt48_pic_source_state s;lt48_status st=lt48_pic_source_session_state_internal(a->source.session,&s);
 if(st!=LT48_OK)return st;
 if(s.variable_count!=a->source.variable_count||s.configuration_count>1024||s.image_size!=sizeof a->source.image)return LT48_SAFETY_REJECTED;
 memcpy(a->source.variables,s.variables,s.variable_count*sizeof *s.variables);
 if(s.configuration_count)memcpy(a->source.fields,s.configuration,s.configuration_count*sizeof *s.configuration);
 a->source.field_count=s.configuration_count;memcpy(a->source.image,s.image,sizeof a->source.image);
 memcpy(a->source.buffers,s.buffers,sizeof a->source.buffers);memcpy(a->source.rails,s.voltages,sizeof a->source.rails);return LT48_OK;
}
static inline lt48_status rm14_modern_source_begin(rm14_modern_source *a,unsigned mode,unsigned voltage) {
 memset(a->source.buffers,0,sizeof a->source.buffers);
 /* The verified original constructor's code-size buffer survives the public
  * code-only traversal. These families do not declare old Size variable19. */
 otp14_source_number(a->source.buffers[22],a->binding->profile->code_words);
 lt48_status st=otp14_source_prepare(&a->source,mode);
 for(size_t i=0;i<a->source.variable_count;i++)if(a->source.variables[i].variable_id==0)a->source.variables[i].value=voltage;
 /* Synchronize the changed voltage declaration with the shared session. */
 if(st==LT48_OK) {
  lt48_pic_source_state s={a->source.variables,a->source.variable_count,a->source.fields,a->source.field_count,
   a->source.image,sizeof a->source.image,(const unsigned char (*)[32])a->source.buffers,{0},0,0};
  memcpy(s.voltages,a->source.rails,sizeof s.voltages);st=lt48_pic_source_session_restore_internal(a->source.session,&s);
 }
 const lt48_pic_source_lifecycle lifecycle={.voltage_variable=0,.poweron_role=0,.init_role=3,.reset_role=4,.poweroff_role=1,
  .timeout_seconds=10,.voltage_centivolts=voltage,.minimum_centivolts=a->policy.common.configuration.minimum,
  .maximum_centivolts=a->policy.common.configuration.maximum,.selector_write=otp14_source_selector,
  .ready=otp14_source_ready,.runner=otp14_source_startup_runner,.context=&a->source};
 if(st==LT48_OK)st=lt48_pic_source_session_begin_internal(a->source.session,&lifecycle,(uint16_t)mode,0);
 if(st==LT48_OK)st=rm14_modern_source_export(a);
 return st;
}
static inline lt48_status rm14_modern_source_end(rm14_modern_source *a,lt48_status st,unsigned mode) {
 if(a->flush){lt48_status flushed=a->flush(a->wire.context);if(st==LT48_OK)st=flushed;}
 /* Failed reads must not restart the source ReadCFG suffix with a target
  * left mid-frame. Mode4 retains the same electrical reset without that read. */
 return otp14_source_end(&a->source,st,st==LT48_OK?mode:4);
}
static inline lt48_status rm14_modern_source_timeout(rm14_modern_source *a,unsigned ms) {
 return a->word_timeout?a->word_timeout(a->wire.context,ms):LT48_OK;
}
static inline lt48_status rm14_modern_source_read_word(rm14_modern_source *a,uint16_t *out) {
 lt48_status st=rm14_modern_source_timeout(a,10000);
 return st==LT48_OK?rm14_modern_read_word(&a->wire,out):st;
}
static inline lt48_status rm14_modern_source_increment(rm14_modern_source *a){return lt48_pic_classic_command(&a->wire,6);}
static inline lt48_status rm14_modern_source_bank(rm14_modern_source *a){return lt48_pic_classic_load(&a->wire,0,0x3fff);}
static inline lt48_status rm14_modern_source_scan(void *v,unsigned level,uint16_t *out) {
 rm14_modern_source *a=v;for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++)out[i]=0x3fff;
 lt48_status st=rm14_modern_source_begin(a,1,level);
 for(unsigned i=0;st==LT48_OK&&i<a->policy.common.code_words;i++) {
  st=rm14_modern_source_read_word(a,out+i);
  if(st==LT48_OK&&i+1<a->policy.common.code_words)st=rm14_modern_source_increment(a);
 }
 if(st==LT48_OK)st=rm14_modern_source_bank(a);
 for(unsigned i=0;st==LT48_OK&&i<8;i++) {
  if(i<4||i==7)st=rm14_modern_source_read_word(a,out+0x2000+i);
  if(st==LT48_OK&&i<7)st=rm14_modern_source_increment(a);
 }
 return rm14_modern_source_end(a,st,1);
}
/* Actual role8 reads before programming on fixed-helper and ordinary
 * families. The two reload-helper families own that condition inside role13
 * and start a pulse even for matching code. Retain that distinction. */
static inline lt48_status rm14_modern_source_program_if_needed(rm14_modern_source *a,uint16_t value,unsigned mask){
 unsigned family=a->binding->profile->family;
 lt48_status st=LT48_OK;
 if(family!=368&&family!=369){
  uint16_t actual=0;st=rm14_modern_source_read_word(a,&actual);
  if(st!=LT48_OK||!((actual^value)&mask))return st;
 }
 return rm14_modern_program_word_with_pulse(&a->wire,value,mask,a->pulse,a->wire.context);
}
static inline lt48_status rm14_modern_source_program(void *v,const uint16_t *image,unsigned skip_factory) {
 rm14_modern_source *a=v;lt48_status st=rm14_modern_source_begin(a,2,a->policy.common.configuration.nominal);
 for(unsigned i=0;st==LT48_OK&&i<a->policy.common.code_words;i++) {
  if(!skip_factory||i!=a->policy.factory_word) {
   st=rm14_modern_source_timeout(a,5000);
   if(st==LT48_OK)st=rm14_modern_source_program_if_needed(a,image[i],0x3fff);
  }
  if(st==LT48_OK&&i+1<a->policy.common.code_words)st=rm14_modern_source_increment(a);
 }
 if(st==LT48_OK)st=rm14_modern_source_bank(a);
 for(unsigned i=0;st==LT48_OK&&i<4;i++) {
  st=rm14_modern_source_timeout(a,5000);
  if(st==LT48_OK)st=rm14_modern_source_program_if_needed(a,image[0x2000+i],a->policy.common.id_mask);
  if(st==LT48_OK&&i<3)st=rm14_modern_source_increment(a);
 }
 return rm14_modern_source_end(a,st,2);
}
static inline lt48_status rm14_modern_source_config_read(void *v,unsigned level,uint16_t *out) {
 rm14_modern_source *a=v;lt48_status st=rm14_modern_source_begin(a,1,level);
 if(st==LT48_OK)st=rm14_modern_source_bank(a);
 for(unsigned i=0;st==LT48_OK&&i<7;i++)st=rm14_modern_source_increment(a);
 if(st==LT48_OK)st=rm14_modern_source_read_word(a,out);
 return rm14_modern_source_end(a,st,1);
}
static inline lt48_status rm14_modern_source_config_action(void *v,unsigned role,uint16_t target,int standalone) {
 rm14_modern_source *a=v;(void)standalone;
 if((role!=10&&role!=14)||!rm14_otp_target_valid(&a->policy,target))return LT48_SAFETY_REJECTED;
 target=rm14_otp_configuration_target(&a->policy,target);a->source.image[0x400e]=target;a->source.image[0x400f]=target>>8;
 lt48_status st=rm14_modern_source_begin(a,2,a->policy.common.configuration.nominal);
 if(st==LT48_OK)st=rm14_modern_source_bank(a);
 for(unsigned i=0;st==LT48_OK&&i<7;i++)st=rm14_modern_source_increment(a);
 if(st==LT48_OK)st=rm14_modern_source_timeout(a,10000);
 if(st==LT48_OK)st=rm14_modern_program_configuration_with_pulse(&a->wire,target,a->policy.common.configuration.mask,a->pulse,a->wire.context);
 return rm14_modern_source_end(a,st,2);
}
static inline lt48_status rm14_modern_source_cleanup(void *v,unsigned role) {
 rm14_modern_source *a=v;
 /* Normal Read already completed its source ReadCFG suffix. Global cleanup
  * must not issue another configuration read after VPP has been removed. */
 return otp14_source_invoke(&a->source,role,4,0,0,1,NULL);
}
static inline lt48_status rm14_modern_source_cleanup_command(void *v,const unsigned char *bytes,size_t n) {
 rm14_modern_source *a=v;return otp14_source_cleanup_command(&a->source,bytes,n);
}
static inline lt48_status rm14_modern_source_execute(rm14_modern_source *a,lt48_operation_kind op,
 const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result) {
 if(!a)return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++){a->source.image[2*i]=image[i];a->source.image[2*i+1]=image[i]>>8;}
 const rm14_otp_io io={a,rm14_modern_source_scan,rm14_modern_source_program,
  {a,rm14_modern_source_config_action,rm14_modern_source_config_read},
  {a,rm14_modern_source_cleanup,rm14_modern_source_cleanup_command}};
 rm14_otp_policy selected=a->policy;selected.verify_selection=a->verify_selection;
 return rm14_otp_execute(&selected,&io,op,image,output,scratch,result);
}
static inline lt48_status rm14_modern_source_open(const char *path,const lt48_device_spec *spec,const rm14_profile *p,
 const unsigned char *image,size_t span,lt48_pic_source_runner runner,lt48_pic_source_runner cleanup_runner,
 lt48_status (*command)(void *,const unsigned char *,size_t),lt48_status (*ready)(void *),void *context,
 const lt48_pic_classic_wire *wire,rm14_modern_source **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!runner||!cleanup_runner||!command||!ready||!rm14_modern_wire_valid(wire))return LT48_INVALID_ARGUMENT;
 rm14_modern_source *a=calloc(1,sizeof *a);if(!a)return LT48_NO_MEMORY;
 lt48_status st=rm14_modern_policy(p,&a->policy);
 if(st==LT48_OK)st=rm14_open(path,spec,p,image,span,&a->binding);
 if(st!=LT48_OK){free(a);return st;}
 a->source.policy=a->policy.common;a->source.resources=a->binding->resources;a->source.session=a->binding->session;
 a->source.runner=runner;a->source.cleanup_runner=cleanup_runner;a->source.command=command;a->source.ready=ready;a->source.context=context;
 a->wire=*wire;a->source.variable_count=p->variable_count;
 st=rm14_modern_source_export(a);
 if(st!=LT48_OK){rm14_modern_source_close(a);return st;}*out=a;return LT48_OK;
}
#endif
