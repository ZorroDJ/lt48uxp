#ifndef LT48_OTP14_CONFIGURATION_TRANSACTION_H
#define LT48_OTP14_CONFIGURATION_TRANSACTION_H
#include <labtool48/labtool48.h>
typedef struct {uint16_t mask,protection;unsigned nominal,minimum,maximum;} otp14_configuration_policy;
typedef struct {
 void *context;
 lt48_status (*action)(void *,unsigned,uint16_t,int);
 lt48_status (*read)(void *,unsigned,uint16_t *);
} otp14_configuration_io;
static inline int otp14_configuration_target_valid(const otp14_configuration_policy *p,uint16_t target){
 if(!p||!p->mask||p->mask>0x3fff||!p->protection||(p->protection&~p->mask)||
    !p->minimum||p->nominal<475||p->nominal>525||p->minimum>p->nominal||p->nominal>p->maximum||target>0x3fff)return 0;
 if(p->protection==0x10)return 1;
 if(p->protection==0x3fb0)return (target&p->protection)==0||(target&p->protection)==p->protection;
 if(p->protection!=0x30&&p->protection!=0x3f30&&p->protection!=0x3330)return 0;
 unsigned pair=(target>>4)&3;
 for(unsigned shift=4;shift<14;shift+=2)if((p->protection>>shift)&3)
  if(((p->protection>>shift)&3)!=3||((target>>shift)&3)!=pair)return 0;
 return 1;
}
static inline lt48_status otp14_configuration_check(const otp14_configuration_policy *p,
 const otp14_configuration_io *io,uint16_t expected){
 const unsigned voltages[]={p->nominal,p->minimum,p->maximum};
 for(unsigned i=0;i<3;i++){
  uint16_t actual=0;lt48_status st=io->read(io->context,voltages[i],&actual);
  if(st!=LT48_OK)return st;
  if((actual^expected)&p->mask)return LT48_DEVICE_MISMATCH;
 }
 return LT48_OK;
}
/* Configuration readback never trusts callback success alone. Protection is
 * deferred until unprotected configuration verifies at all requested levels.
 * The adapter must supply qualified physical reads, voltage and entry/cleanup. */
static inline lt48_status otp14_configuration_program(const otp14_configuration_policy *p,
 const otp14_configuration_io *io,uint16_t target){
 if(!io||!io->action||!io->read||!otp14_configuration_target_valid(p,target))return LT48_SAFETY_REJECTED;
 target|=0x3fff^p->mask;
 lt48_status st=io->action(io->context,14,target,0);
 if(st==LT48_OK)st=otp14_configuration_check(p,io,target|p->protection);
 if(st==LT48_OK&&(target&p->protection)!=p->protection){
  st=io->action(io->context,10,target,0);
  if(st==LT48_OK)st=otp14_configuration_check(p,io,target);
 }
 return st;
}
static inline lt48_status otp14_configuration_secure(const otp14_configuration_policy *p,
 const otp14_configuration_io *io){
 if(!io||!io->action||!io->read||!otp14_configuration_target_valid(p,0x3fff))return LT48_SAFETY_REJECTED;
 uint16_t current=0;lt48_status st=io->read(io->context,p->nominal,&current);
 uint16_t target=(current&~p->protection)|(0x3fff^p->mask);
 if(st==LT48_OK)st=io->action(io->context,10,target,1);
 return st==LT48_OK?otp14_configuration_check(p,io,target):st;
}
#endif
