/* Finite class2021 extension; vendor resources remain BYO. */
#ifndef LT48_EP22FOUR_PROFILES_H
#define LT48_EP22FOUR_PROFILES_H
#include "admission.h"
static inline int ep22four_definition(unsigned d,unsigned m,unsigned p){return EP22FOUR_ADMITTED&&(d==0x35||d==0x36)&&m==0x1e&&p==0x14;}
static inline int ep22four_binding(const lt48_device_spec*s){if(!ep22four_definition(s->definition_resource,s->family_code,s->pinmap)||s->catalogue.device_class!=2021||s->pin_count!=24||strcmp(s->definition_stem,"GBB____"))return 0;
 const char*name=NULL;if(!strcmp(s->catalogue.vendor,"Altera"))name=s->definition_resource==0x35?"EP22V10E":"EP22V10";else if(!strcmp(s->catalogue.vendor,"Intel"))name=s->definition_resource==0x35?"85C22V10":"iPLD22V10";
 return name&&s->catalogue.device_name_length==strlen(name)&&!memcmp(s->catalogue.device_name,name,strlen(name));}
#endif
