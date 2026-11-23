/* Authored finite binding. Imported procedures remain bring-your-own. */
#include "admission.h"
static const struct legacy14_profile*aduc812one_binding(const lt48_device_spec*s){
 static const struct legacy14_profile p={"Analog Devices:ADuC812 @52Q",186,109,74,44,8832,8,10,10,3,0,1,500};
 if(!LT48_ADUC812ONE_ADMITTED||s->catalogue.device_class!=2013||strcmp(s->definition_stem,"GADA___")||s->definition_resource!=p.d||s->family_code!=p.m||s->pinmap!=p.p||s->pin_count!=p.pins||s->capacity_bytes!=p.capacity||s->element_bits!=8||s->secondary_element_count)return NULL;
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 return !strcmp(key,p.key)?&p:NULL;
}
