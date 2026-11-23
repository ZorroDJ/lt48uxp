#ifndef LT48_PIC24_CONFIGURATION_WRITE_POLICY_H
#define LT48_PIC24_CONFIGURATION_WRITE_POLICY_H
#include "pic24_configuration_rules.h"
/* In-place configuration programming is one-to-zero. Protection activation
 * needs its own pre-exit verification sequence; this ordinary write route
 * requires GCP disabled in both the observed and requested CW1. */
static inline int lt48_pic24_configuration_write_prepare(unsigned family,
 const uint16_t *observed,const uint32_t *requested,unsigned count,uint16_t *output){
 lt48_pic24_configuration_rules rules;uint16_t values[4];
 if(!observed || !requested || !output || !lt48_pic24_configuration_rules_prepare(family,&rules) ||
  !lt48_pic24_configuration_normalize(&rules,requested,count,values))return 0;
 if(!(observed[count-1]&0x2000) || !(values[count-1]&0x2000))return 0;
 for(unsigned i=0;i<count;i++)if(values[i]&~observed[i]&~rules.force_one[i])return 0;
 for(unsigned i=0;i<count;i++)output[i]=values[i];
 return 1;
}
/* GCP programming follows verification and is confirmed by the original
 * status-checked NVM completion, not a post-reset protected-memory read.
 * Capture current words while unprotected; never infer settings from zeros
 * returned by an already protected or unreadable device. */
static inline int lt48_pic24_configuration_secure_prepare(unsigned family,
 const uint16_t *observed,unsigned count,uint16_t *output){
 lt48_pic24_configuration_rules rules;uint32_t requested[4];uint16_t values[4];
 if(!observed || !output || !lt48_pic24_configuration_rules_prepare(family,&rules) ||
  count!=rules.words || !(observed[count-1]&0x2000))return 0;
 for(unsigned i=0;i<count;i++)requested[i]=observed[i];
 requested[count-1]&=~0x2000u;
 if(!lt48_pic24_configuration_normalize(&rules,requested,count,values))return 0;
 for(unsigned i=0;i<count;i++)if(values[i]&~observed[i]&~rules.force_one[i])return 0;
 for(unsigned i=0;i<count;i++)output[i]=values[i];
 return 1;
}
#endif
