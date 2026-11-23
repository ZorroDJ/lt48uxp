#ifndef LT48_PIC24_CONFIGURATION_COMPARE_DRAFT_H
#define LT48_PIC24_CONFIGURATION_COMPARE_DRAFT_H
#include "pic24_configuration_rules.h"
#include <labtool48/labtool48.h>
typedef struct {unsigned word;uint16_t expected,observed,differing_bits;} lt48_pic24_configuration_mismatch;
/* DS39768D 4.6.3, DS30009934C 4.6.3, DS30000510F 4.6.3 require
 * readback compared against programmed configuration. Compare all low16 bits;
 * checksum-only CW1 masks do not authorize suppressing readback differences.
 * Requested values normalize identically to the C Program adapter. Canonical
 * order CW2..CW1 or CW4..CW1. Caller must supply independent, successful Read. */
static inline lt48_status lt48_pic24_configuration_compare(unsigned family,
 const uint32_t *requested,const uint16_t *observed,unsigned count,
 lt48_pic24_configuration_mismatch *mismatch){
 if(!requested || !observed || !mismatch)return LT48_INVALID_ARGUMENT;
 lt48_pic24_configuration_rules rules;uint16_t expected[4];
 if(!lt48_pic24_configuration_rules_prepare(family,&rules) ||
    !lt48_pic24_configuration_normalize(&rules,requested,count,expected))return LT48_INVALID_ARGUMENT;
 *mismatch=(lt48_pic24_configuration_mismatch){count,0,0,0};
 for(unsigned i=0;i<count;i++)if(expected[i]!=observed[i]){
  *mismatch=(lt48_pic24_configuration_mismatch){i,expected[i],observed[i],(uint16_t)(expected[i]^observed[i])};
  return LT48_DEVICE_MISMATCH;
 }
 return LT48_OK;
}
#endif
