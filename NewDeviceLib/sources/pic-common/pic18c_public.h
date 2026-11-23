#ifndef LT48_PIC18C_PUBLIC_H
#define LT48_PIC18C_PUBLIC_H
#include "older16_operator.h"
/* Exact PIC18C EPROM aliases; distinct from later Flash erase/row engines. */
static inline const o16_profile *pic18c_match(const lt48_device_spec *s){
 if(!s||s->catalogue.device_class!=2015||!s->definition_stem||strcmp(s->definition_stem,"GADC___")||!s->catalogue.vendor||strcmp(s->catalogue.vendor,"Microchip")||s->element_bits!=16)return NULL;
 for(size_t i=13;i<20;i++){
  const o16_profile *p=older16_profiles+i;
  if(s->definition_resource==p->definition&&s->family_code==p->family&&s->pinmap==p->pinmap&&(s->capacity_bytes==p->source_bytes||s->capacity_bytes==p->backing_bytes)&&s->catalogue.device_name_length==strlen(p->name)&&!memcmp(s->catalogue.device_name,p->name,strlen(p->name)))return p;
 }return NULL;
}
static inline lt48_status pic18c_image_validate(const o16_profile *p,const unsigned char *image,size_t n){
 if(!p||!image||n!=p->backing_bytes)return LT48_INVALID_ARGUMENT;
 if(image[0x300000]!=0&&image[0x300000]!=255)return LT48_SAFETY_REJECTED;
 for(size_t i=p->source_bytes;i<n;i++)if(!(i>=0x200000&&i<0x200008)&&!(i>=0x300000&&i<0x300007)&&image[i]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
#endif
