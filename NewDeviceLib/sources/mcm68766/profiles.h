/* Authored exact bindings; vendor waves remain BYO. */
#include "admission.h"
static int mcm68766_binding(const lt48_device_spec *s){
 return LT48_MCM68766_ADMITTED && s->catalogue.device_class==2000 &&
  s->definition_resource==0x123 && s->family_code==0x8c && s->pinmap==0x55 &&
  s->capacity_bytes==8192 && s->element_bits==8 && s->pin_count==24 &&
  !s->catalogue.manufacturer_id && !s->catalogue.device_id &&
  s->catalogue.device_name_length==8 && !memcmp(s->catalogue.device_name,"MCM68766",8) &&
  (!strcmp(s->catalogue.vendor,"Motorola")||!strcmp(s->catalogue.vendor,"Freescale"));
}
