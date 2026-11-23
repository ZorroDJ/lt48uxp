#ifndef LT48_PIC_REMAINING14_ORDINARY_SOURCE_H
#define LT48_PIC_REMAINING14_ORDINARY_SOURCE_H
#include "remaining14_modern_source.h"
/* Seven source-pinned ordinary OTP aliases reuse the same whole owner and
 * ICSP engine. PIC14000's calibration block is a distinct policy, not inferred
 * from its4096word geometry. The CE62x source layout covers program/ID/config,
 * and does not expose an ICSP route for the chip's internal serial EEPROM. */
static inline lt48_status rm14_ordinary_policy(const rm14_profile *p,rm14_otp_policy *out) {
 if(!p||!out||p->has_erase||p->source_bytes!=16400||p->backing_bytes!=16400)return LT48_INVALID_ARGUMENT;
 *out=(rm14_otp_policy){.blank_id_mask=15,.common={p->code_words,0x3fff,{0x3f7f,0x3f30,500,475,525}},
  .protection_values={0,0x1510,0x2a20,0x3f30},.protection_count=4,.configuration_one=0x80,.factory_word=UINT32_MAX};
 switch(p->family) {
 case 41:out->protection_count=2;out->protection_values[1]=0x3f30;break;
 case 42:out->protection_count=3;out->protection_values[2]=0x3f30;break;
 case 43:break;
 case 227:case 228:
  out->common.configuration.mask=0x3fff;out->common.configuration.protection=0x3f60;
  out->protection_values[1]=0x1520;out->protection_values[2]=0x2a40;out->protection_values[3]=0x3f60;
  out->configuration_one=0;out->factory_word=p->code_words-1;
  out->invalid_configuration_mask=7;out->invalid_configuration_value=3;break;
 default:return LT48_UNSUPPORTED;
 }
 for(unsigned i=0;i<p->variable_count;i++) {
  const lt48_compile_variable *v=p->variables+i;
  if(v->variable_id==0)out->common.configuration.nominal=v->value;
  if(v->variable_id==5)out->common.configuration.minimum=v->value;
  if(v->variable_id==6)out->common.configuration.maximum=v->value;
 }
 return rm14_otp_target_valid(out,0x3fff)?LT48_OK:LT48_SAFETY_REJECTED;
}
static inline lt48_status rm14_ordinary_source_open(const char *path,const lt48_device_spec *spec,const rm14_profile *p,
 const unsigned char *image,size_t span,lt48_pic_source_runner runner,lt48_pic_source_runner cleanup_runner,
 lt48_status (*command)(void *,const unsigned char *,size_t),lt48_status (*ready)(void *),void *context,
 const lt48_pic_classic_wire *wire,rm14_modern_source **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!runner||!cleanup_runner||!command||!ready||!rm14_modern_wire_valid(wire))return LT48_INVALID_ARGUMENT;
 rm14_modern_source *a=calloc(1,sizeof *a);if(!a)return LT48_NO_MEMORY;
 lt48_status st=rm14_ordinary_policy(p,&a->policy);
 if(st==LT48_OK)st=rm14_open(path,spec,p,image,span,&a->binding);
 if(st!=LT48_OK){free(a);return st;}
 a->source.policy=a->policy.common;a->source.resources=a->binding->resources;a->source.session=a->binding->session;
 a->source.runner=runner;a->source.cleanup_runner=cleanup_runner;a->source.command=command;a->source.ready=ready;a->source.context=context;
 a->wire=*wire;a->source.variable_count=p->variable_count;
 st=rm14_modern_source_export(a);
 if(st!=LT48_OK){rm14_modern_source_close(a);return st;}*out=a;return LT48_OK;
}
#endif
