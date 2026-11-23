#ifndef LT48_PIC18_J_ADAPTERS_H
#define LT48_PIC18_J_ADAPTERS_H
#include <stddef.h>
#include <stdint.h>
#include <string.h>
typedef struct {uint8_t chip,dip;const char *role;} lt48_pic18_j_adapter_net;
typedef struct {
 const char *key;uint16_t definition,family,pinmap;
 uint8_t chip_pins,dip_pins,dip1_zif;
 const lt48_pic18_j_adapter_net *nets;size_t net_count;
 uint8_t vcap_chip,vcap_ground_chip;uint16_t effective_cap_uf_tenths;
 uint8_t unused_core_dip,internal_regulator;const char *note;
} lt48_pic18_j_adapter_profile;
#include "pic18_j_adapters_generated.h"
static inline const lt48_pic18_j_adapter_profile *lt48_pic18_j_adapter_find(
 const char *key,uint16_t d,uint16_t f,uint16_t p) {
 if(!key)return NULL;
 for(size_t i=0;i<sizeof(j_adapter_profiles)/sizeof(*j_adapter_profiles);i++) {
  const lt48_pic18_j_adapter_profile *r=j_adapter_profiles+i;
  if(r->definition==d&&r->family==f&&r->pinmap==p&&!strcmp(r->key,key))return r;
 }
 return NULL;
}
#endif
