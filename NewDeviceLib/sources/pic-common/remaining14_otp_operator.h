#ifndef LT48_PIC_REMAINING14_OTP_OPERATOR_H
#define LT48_PIC_REMAINING14_OTP_OPERATOR_H
#include "otp14_operator.h"
#include <stdlib.h>
/* Finite common whole-OTP owner for the remaining ordinary14-bit routes.
 * Policies carry explicit CP states, mandatory config bits and factory-word
 * ownership. They do not infer silicon behavior from names or code size. */
typedef struct {
 otp14_operator_policy common;
 unsigned oscillator_margins; /* Exact parity-group DEVINIT rule. */
 unsigned verify_selection; /* 0=default all; otherwise public margin enum+1. */
 uint16_t blank_id_mask; /* zero uses normal ID mask; source mode11 may ignore upper bits. */
 uint16_t protection_values[4];unsigned protection_count;
 uint16_t configuration_zero,configuration_one;
 uint16_t invalid_configuration_mask,invalid_configuration_value;
 unsigned factory_word; /* UINT32_MAX means none. */
} rm14_otp_policy;
typedef struct {
 void *context;
 lt48_status (*scan)(void *,unsigned,uint16_t *);
 lt48_status (*program)(void *,const uint16_t *,unsigned);
 otp14_configuration_io configuration;
 lt48_pic_cleanup_io cleanup;
} rm14_otp_io;
static inline int rm14_otp_target_valid(const rm14_otp_policy *p,unsigned value) {
 if(!p||p->oscillator_margins>1||p->verify_selection>4||value>0x3fff||!p->common.code_words||p->common.code_words>8192||!p->common.id_mask||p->common.id_mask>0x3fff||
    !p->common.configuration.mask||p->common.configuration.mask>0x3fff||!p->common.configuration.protection||
    (p->common.configuration.protection&~p->common.configuration.mask)||
    !p->protection_count||p->protection_count>4||
    (p->configuration_one&p->configuration_zero)||((p->configuration_one|p->configuration_zero)&~0x3fff)||
    ((p->configuration_one|p->configuration_zero)&p->common.configuration.mask)||
    ((p->configuration_one|p->configuration_zero|p->common.configuration.mask)!=0x3fff)||
    (p->invalid_configuration_mask&&(value&p->invalid_configuration_mask)==p->invalid_configuration_value)||
    (p->factory_word!=UINT32_MAX&&p->factory_word>=p->common.code_words)||
    p->common.configuration.nominal<475||p->common.configuration.nominal>525||
    !p->common.configuration.minimum||p->common.configuration.minimum>p->common.configuration.nominal||
    p->common.configuration.maximum<p->common.configuration.nominal)return 0;
 unsigned found=0,has_unprotected=0;
 for(unsigned i=0;i<p->protection_count;i++) {
  unsigned state=p->protection_values[i];if(state&~p->common.configuration.protection)return 0;
  for(unsigned j=0;j<i;j++)if(p->protection_values[j]==state)return 0;
  if(state==p->common.configuration.protection)has_unprotected=1;
  if((value&p->common.configuration.protection)==state)found=1;
 }
 return found&&has_unprotected&&p->protection_values[0]==0;
}
static inline uint16_t rm14_otp_configuration_target(const rm14_otp_policy *p,unsigned value) {
 return (uint16_t)((value|p->configuration_one)&~p->configuration_zero);
}
static inline lt48_status rm14_otp_check(const rm14_otp_policy *p,const uint16_t *want,const uint16_t *got,
 int blank,otp14_operator_result *r) {
 r->mismatches=0;r->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++) {
  if(a==p->factory_word&&(blank||(want&&want[a]==0x3fff)))continue;
  unsigned mask=otp14_operator_mask(&p->common,a);
  if(blank&&a>=0x2000&&a<=0x2003&&p->blank_id_mask)mask=p->blank_id_mask;
  if(((blank?0x3fff:want[a])^got[a])&mask){if(!r->mismatches)r->first_mismatch=2*a;r->mismatches++;}
 }
 return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
static inline lt48_status rm14_otp_configuration_check(const rm14_otp_policy *p,const rm14_otp_io *io,uint16_t target) {
 const unsigned levels[]={p->common.configuration.nominal,p->common.configuration.minimum,p->common.configuration.maximum};
 for(unsigned i=0;i<3;i++) {
  uint16_t actual=0;lt48_status st=io->configuration.read(io->configuration.context,levels[i],&actual);
  if(st!=LT48_OK)return st;
  if((actual^target)&p->common.configuration.mask)return LT48_DEVICE_MISMATCH;
 }
 return LT48_OK;
}
static inline lt48_status rm14_otp_execute(const rm14_otp_policy *p,const rm14_otp_io *io,
 lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *r) {
 if(!rm14_otp_target_valid(p,0x3fff)||!io||!io->scan||!io->program||!io->configuration.action||
    !io->configuration.read||!io->cleanup.source||!io->cleanup.command||!scratch||!r)return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_ERASE||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
 if((op==LT48_OPERATION_READ&&!output)||
    ((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image))return LT48_INVALID_ARGUMENT;
 if(otp14_operator_overlap(image,scratch)||(op==LT48_OPERATION_READ&&otp14_operator_overlap(output,scratch)))return LT48_INVALID_ARGUMENT;
 if(image) {
  for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)if(image[a]>0x3fff||(!otp14_operator_mask(&p->common,a)&&image[a]!=0x3fff))return LT48_SAFETY_REJECTED;
  for(unsigned a=0x2000;a<=0x2003;a++)if((image[a]|p->common.id_mask)!=0x3fff)return LT48_SAFETY_REJECTED;
  if(!rm14_otp_target_valid(p,image[0x2007]))return LT48_SAFETY_REJECTED;
 }
 uint16_t *expected=NULL;
 if(image){expected=malloc(OTP14_IMAGE_WORDS*sizeof *expected);if(!expected)return LT48_NO_MEMORY;
  memcpy(expected,image,OTP14_IMAGE_WORDS*sizeof *expected);expected[0x2007]=rm14_otp_configuration_target(p,expected[0x2007]);}
 rm14_otp_policy resolved=*p;p=&resolved;
 memset(r,0,sizeof *r);r->first_mismatch=UINT32_MAX;
 const unsigned cp=p->common.configuration.protection;
 unsigned levels[]={p->common.configuration.nominal,p->common.configuration.minimum,p->common.configuration.maximum};
 lt48_status st=LT48_OK;
 if(op==LT48_OPERATION_SECURE) {
  uint16_t current=0;st=io->configuration.read(io->configuration.context,levels[0],&current);
  uint16_t target=rm14_otp_configuration_target(p,current&~cp);
  if(st==LT48_OK&&p->oscillator_margins){
   resolved.common.configuration.minimum=(current&3)==2?450:400;
   resolved.common.configuration.maximum=550;
  }
  if(st==LT48_OK)st=io->configuration.action(io->configuration.context,10,target,1);
  if(st==LT48_OK)st=rm14_otp_configuration_check(p,io,target);
 }else if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM) {
  st=io->scan(io->context,levels[0],scratch);
  unsigned skip_factory=0;
  if(st==LT48_OK&&p->factory_word!=UINT32_MAX) {
   unsigned a=p->factory_word,actual=scratch[a];skip_factory=actual!=0x3fff;
   if(skip_factory&&((actual&0x3c00)!=0x3400||(expected[a]!=0x3fff&&expected[a]!=actual)))st=LT48_SAFETY_REJECTED;
   if(st==LT48_OK&&skip_factory)expected[a]=actual;
   if(st==LT48_OK&&!skip_factory&&expected[a]!=0x3fff&&(expected[a]&0x3c00)!=0x3400)st=LT48_SAFETY_REJECTED;
  }
  if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM)st=rm14_otp_check(p,NULL,scratch,1,r);
  if(st==LT48_OK)for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)
   if(expected[a]&~scratch[a]&otp14_operator_mask(&p->common,a)){r->first_mismatch=2*a;st=LT48_SAFETY_REJECTED;break;}
  if(st==LT48_OK&&(scratch[0x2007]&cp)!=cp)st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK)st=io->program(io->context,expected,skip_factory);
  uint16_t target=expected[0x2007];expected[0x2007]|=cp;
  if(st==LT48_OK)st=io->configuration.action(io->configuration.context,14,expected[0x2007],0);
  if(st==LT48_OK)st=rm14_otp_configuration_check(p,io,expected[0x2007]);
  for(unsigned i=0;st==LT48_OK&&i<3;i++) {
   st=io->scan(io->context,levels[i],scratch);
   if(st==LT48_OK)st=rm14_otp_check(p,expected,scratch,0,r);
  }
  if(st==LT48_OK&&(target&cp)!=cp) {
   st=io->configuration.action(io->configuration.context,10,target,0);
   if(st==LT48_OK)st=rm14_otp_configuration_check(p,io,target);
  }
 }else {
  unsigned passes=op==LT48_OPERATION_VERIFY&&(!p->verify_selection||p->verify_selection==4)?3:1;
  for(unsigned i=0;st==LT48_OK&&i<passes;i++) {
   unsigned level=op==LT48_OPERATION_VERIFY&&p->verify_selection&&p->verify_selection<4?p->verify_selection-1:i;
   st=io->scan(io->context,levels[level],scratch);
   if(st==LT48_OK&&op!=LT48_OPERATION_READ)st=rm14_otp_check(p,expected,scratch,op==LT48_OPERATION_BLANK_CHECK,r);
  }
 }
 r->operation_status=st;st=lt48_pic_cleanup(&io->cleanup,st,&r->cleanup);
 if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,scratch,OTP14_IMAGE_WORDS*sizeof *output);
 free(expected);return st;
}
#endif
