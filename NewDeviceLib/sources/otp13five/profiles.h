/* Authored exact bindings; the imported waveform assets remain BYO. */
#ifndef LT48_OTP13FIVE_PROFILES_H
#define LT48_OTP13FIVE_PROFILES_H
#include "admission.h"
static const struct legacy14_profile*otp13five_binding(const lt48_device_spec*s){
 if(!OTP13FIVE_ADMITTED||s->catalogue.device_class!=2013||strcmp(s->definition_stem,"GADA___")||s->secondary_element_count)return NULL;
 static const struct legacy14_profile rows[]={
 {"Intel:8755A",158,89,54,40,2048,8,7,8,3,0,0,500},
 {"Zilog:Z89371",168,98,63,40,8192,16,7,9,3,0,0,640},
 {"Zilog:Z89371 *44Q",170,98,64,44,8192,16,7,9,3,0,0,640},
 {"Zilog:Z89371B",169,99,63,40,8192,16,7,9,3,0,0,640},
 {"Zilog:Z89371B *44Q",171,99,64,44,8192,16,7,9,3,0,0,640},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct legacy14_profile*p=rows+i;if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity&&s->element_bits==p->bits)return p;}return NULL;
}
#endif
