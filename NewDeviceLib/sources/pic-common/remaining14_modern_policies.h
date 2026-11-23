#ifndef LT48_PIC_REMAINING14_MODERN_POLICIES_H
#define LT48_PIC_REMAINING14_MODERN_POLICIES_H
#include "remaining14_resources.h"
#include "remaining14_otp_operator.h"
/* Finite source policies for20 fixed-helper and6 reload-helper aliases.
 * No public admission: exact constructor, primary applicability, source/effect
 * and normal C/GTK joins are still required. Mandatory bits are also checked
 * against actual original Pre/Post execution in the private qualification. */
static inline lt48_status rm14_modern_policy(const rm14_profile *p,rm14_otp_policy *out) {
 if(!p||!out||p->has_erase||p->backing_bytes!=16400)return LT48_INVALID_ARGUMENT;
 *out=(rm14_otp_policy){.common={p->code_words,0x3fff,{0x3f7f,0x3f30,500,475,525}},
  .protection_values={0,0x1510,0x2a20,0x3f30},.protection_count=4,.configuration_one=0x80,.factory_word=UINT32_MAX};
 switch(p->family) {
 case 356:case 368:case 369:
  out->common.configuration.protection=0x3300;out->protection_count=2;
  out->protection_values[1]=0x3300;break;
 case 358:case 363:break;
 case 359:
  out->common.configuration.mask=0x3fff;out->common.configuration.protection=0x3f60;
  out->protection_values[1]=0x1520;out->protection_values[2]=0x2a40;out->protection_values[3]=0x3f60;
  out->configuration_one=0;out->factory_word=p->code_words-1;
  out->invalid_configuration_mask=7;out->invalid_configuration_value=3;break;
 case 360:case 361:case 367:
  out->common.configuration.mask=0x3f3f;out->configuration_one=0xc0;break;
 case 362:
  out->common.configuration.mask=0x7f;out->common.configuration.protection=0x30;
  out->protection_values[1]=0x10;out->protection_values[2]=0x20;out->protection_values[3]=0x30;
  out->configuration_one=0x3f80;break;
 case 364:case 365:case 366:
  out->common.configuration.mask=0x3f3f;out->configuration_zero=0x40;
  if(p->family==364){out->protection_count=2;out->protection_values[1]=0x3f30;}
  if(p->family==365){out->protection_count=3;out->protection_values[2]=0x3f30;}
  break;
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
#endif
