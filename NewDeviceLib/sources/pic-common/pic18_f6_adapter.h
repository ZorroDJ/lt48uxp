#ifndef LT48_PIC18_F6_ADAPTER_H
#define LT48_PIC18_F6_ADAPTER_H
#include <labtool48/labtool48.h>
#include "pic18_f6_runtime_profiles.h"
#include "pic18_f6x20_adapter.h"
/* Three primary64TQ figures and source P15/P16/P51 join to the same
 * fourteen-net programming-only carrier as the earlier F6X20 group. */
#define lt48_f6_adapter_nets lt48_f6x20_adapter_nets
static inline const char *lt48_f6_adapter_note(const lt48_device_spec *s){
 if(!s||s->catalogue.device_class!=2048||!s->definition_stem||strcmp(s->definition_stem,"GDCC___")||!s->catalogue.device_name)return NULL;
 for(size_t i=0;i<sizeof(lt48_pic18_f6_runtime_profiles)/sizeof(*lt48_pic18_f6_runtime_profiles);i++){
  const lt48_pic18_f6_runtime_policy *p=lt48_pic18_f6_runtime_profiles+i;
  if(p->definition==s->definition_resource&&p->family==s->family_code&&p->pinmap==s->pinmap&&strlen(p->name)==s->catalogue.device_name_length&&!memcmp(p->name,s->catalogue.device_name,strlen(p->name)))
   return "Requires an authored F6 TQFP64 programming-only carrier to DIP32, centered in ZIF48 (DIP1 at ZIF9): chip VDD/AVDD 10/19/26/38/57 to DIP32; VSS/AVSS 9/20/25/41/56 to DIP16; MCLR7 to DIP1; PGC42 to DIP23; PGD37 to DIP20; PGM43 to DIP24. All other chip leads remain unconnected. A straight adapter is incompatible. This is source/specification-audited wiring; manufactured continuity and hardware qualification remain pending.";
 }return NULL;
}
#endif
