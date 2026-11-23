#ifndef LT48_PIC_REMAINING14_PIC14000_SOURCE_H
#define LT48_PIC_REMAINING14_PIC14000_SOURCE_H
#include "remaining14_modern_source.h"
#include "remaining14_pic14000_operator.h"
static inline lt48_status rm14_pic14000_policy(const rm14_profile *p,rm14_otp_policy *out){
 if(!p||!out||p->family!=44||p->has_erase||p->code_words!=4096||p->source_bytes!=16400||p->backing_bytes!=16400)return LT48_UNSUPPORTED;
 *out=(rm14_otp_policy){.common={4096,0x3fff,{0x3fff,RM14_CPP,500,475,525}},
  .protection_values={0,RM14_CPP},.protection_count=2,.factory_word=UINT32_MAX};
 for(unsigned n=0;n<p->variable_count;n++){
  const lt48_compile_variable *v=p->variables+n;
  if(v->variable_id==0)out->common.configuration.nominal=v->value;
  if(v->variable_id==5)out->common.configuration.minimum=v->value;
  if(v->variable_id==6)out->common.configuration.maximum=v->value;
 }
 out->common.configuration.minimum=400;out->common.configuration.maximum=550;
 return rm14_otp_target_valid(out,0x3fff)?LT48_OK:LT48_SAFETY_REJECTED;
}
static inline lt48_status rm14_pic14000_source_program(void *v,const uint16_t *image,unsigned preserve){
 rm14_modern_source *a=v;lt48_status st=rm14_modern_source_begin(a,2,a->policy.common.configuration.nominal);
 for(unsigned i=0;st==LT48_OK&&i<RM14_CAL_END;i++){
  if(!preserve||i<RM14_CAL_FIRST){
   /* Original W77 reads first and leaves matching cells unpulsed. */
   uint16_t current=0;st=rm14_modern_source_read_word(a,&current);
   if(st==LT48_OK&&current!=image[i]){
    st=rm14_modern_source_timeout(a,5000);
    if(st==LT48_OK)st=rm14_modern_program_word_with_pulse(&a->wire,image[i],0x3fff,a->pulse,a->wire.context);
   }
  }
  if(st==LT48_OK&&i+1<RM14_CAL_END)st=rm14_modern_source_increment(a);
 }
 if(st==LT48_OK)st=rm14_modern_source_bank(a);
 for(unsigned i=0;st==LT48_OK&&i<4;i++){
  /* W80 similarly compares each ID before invoking the word helper. */
  uint16_t current=0;st=rm14_modern_source_read_word(a,&current);
  if(st==LT48_OK&&current!=image[0x2000+i]){
   st=rm14_modern_source_timeout(a,5000);
   if(st==LT48_OK)st=rm14_modern_program_word_with_pulse(&a->wire,image[0x2000+i],0x3fff,a->pulse,a->wire.context);
  }
  if(st==LT48_OK&&i<3)st=rm14_modern_source_increment(a);
 }
 return rm14_modern_source_end(a,st,2);
}
static inline lt48_status rm14_pic14000_source_configuration(void *v,unsigned role,uint16_t target,int standalone){
 rm14_modern_source *a=v;(void)standalone;
 if((role!=10&&role!=14)||!rm14_pic14000_configuration_valid(target))return LT48_SAFETY_REJECTED;
 a->source.image[0x400e]=target;a->source.image[0x400f]=target>>8;
 lt48_status st=rm14_modern_source_begin(a,2,a->policy.common.configuration.nominal);
 if(st==LT48_OK)st=rm14_modern_source_bank(a);
 for(unsigned i=0;st==LT48_OK&&i<7;i++)st=rm14_modern_source_increment(a);
 if(st==LT48_OK)st=rm14_modern_source_timeout(a,10000);
 /* Ones deliberately leave programmed factory trim unchanged. The owning
  * operation separately verifies the actual preserved/restored trim value. */
 if(st==LT48_OK)st=rm14_modern_program_configuration_with_pulse(&a->wire,target,0x3fff^RM14_TRIM,a->pulse,a->wire.context);
 return rm14_modern_source_end(a,st,2);
}
static inline lt48_status rm14_pic14000_source_execute(rm14_modern_source *a,lt48_operation_kind op,
 const uint16_t *image,uint16_t *out,uint16_t *scratch,otp14_operator_result *result){
 if(!a||!a->binding||a->binding->profile->family!=44)return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned i=0;i<OTP14_IMAGE_WORDS;i++){a->source.image[2*i]=image[i];a->source.image[2*i+1]=image[i]>>8;}
 const rm14_otp_io io={a,rm14_modern_source_scan,rm14_pic14000_source_program,
  {a,rm14_pic14000_source_configuration,rm14_modern_source_config_read},
  {a,rm14_modern_source_cleanup,rm14_modern_source_cleanup_command}};
 rm14_otp_policy selected=a->policy;selected.verify_selection=a->verify_selection;
 return rm14_pic14000_execute(&selected,&io,op,image,out,scratch,result);
}
static inline lt48_status rm14_pic14000_source_open(const char *path,const lt48_device_spec *spec,const rm14_profile *p,
 const unsigned char *image,size_t span,lt48_pic_source_runner runner,lt48_pic_source_runner cleanup_runner,
 lt48_status (*command)(void *,const unsigned char *,size_t),lt48_status (*ready)(void *),void *context,
 const lt48_pic_classic_wire *wire,rm14_modern_source **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!runner||!cleanup_runner||!command||!ready||!rm14_modern_wire_valid(wire))return LT48_INVALID_ARGUMENT;
 if(!image||span!=16400||!rm14_pic14000_configuration_valid(image[0x400e]|(unsigned)image[0x400f]<<8))return LT48_SAFETY_REJECTED;
 rm14_modern_source *a=calloc(1,sizeof *a);if(!a)return LT48_NO_MEMORY;
 lt48_status st=rm14_pic14000_policy(p,&a->policy);
 if(st==LT48_OK)st=rm14_open(path,spec,p,image,span,&a->binding);
 if(st!=LT48_OK){free(a);return st;}
 a->source.policy=a->policy.common;a->source.resources=a->binding->resources;a->source.session=a->binding->session;
 a->source.runner=runner;a->source.cleanup_runner=cleanup_runner;a->source.command=command;a->source.ready=ready;a->source.context=context;
 a->wire=*wire;a->source.variable_count=p->variable_count;
 st=rm14_modern_source_export(a);if(st!=LT48_OK){rm14_modern_source_close(a);return st;}*out=a;return LT48_OK;
}
#endif
