#ifndef LT48_PIC_OLDER17_PUBLIC_H
#define LT48_PIC_OLDER17_PUBLIC_H
#include "older16_operator.h"
#include "source_verify_voltage.h"
/* Parallel PIC17 cohort,including C75 with its distinct physical configuration
 * and backing image. Exact source digest and private corrections checked open. */
static inline const o16_profile *pic_older17_match(const lt48_device_spec *s) {
 if(!s||s->catalogue.device_class!=2015||!s->definition_stem||strcmp(s->definition_stem,"GADC___")||
    !s->catalogue.vendor||strcmp(s->catalogue.vendor,"Microchip")||s->element_bits!=16)return NULL;
 for(size_t i=0;i<sizeof older16_profiles/sizeof *older16_profiles;i++) {
  const o16_profile *p=older16_profiles+i;
  if(p->family>39)continue;
  if(s->definition_resource==p->definition&&s->family_code==p->family&&s->pinmap==p->pinmap&&
     s->capacity_bytes==p->source_bytes&&s->catalogue.device_name_length==strlen(p->name)&&
     !memcmp(s->catalogue.device_name,p->name,strlen(p->name)))return p;
 }
 return NULL;
}
#endif
