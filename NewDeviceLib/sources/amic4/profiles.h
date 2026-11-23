/* Authored finite AMIC OTP profiles; unmodified vendor payloads are BYO. */
#include "admission.h"
static inline int amic4_catalogue_member(const lt48_catalogue_entry*e){
 if(!AMIC4_ADMITTED||!e||e->device_class!=2034||strcmp(e->vendor,"AMIC"))return 0;
 const char*names[]={"A27020","A27020 *32PLCC","A27020A","A27020A *32PLCC"};
 for(unsigned i=0;i<4;i++)if(e->device_type==(i<2?40u:41u)&&strlen(names[i])==e->device_name_length&&!memcmp(names[i],e->device_name,e->device_name_length))return 1;
 return 0;
}
static inline int amic4_binding(const lt48_device_spec*s){return s&&amic4_catalogue_member(&s->catalogue)&&s->definition_resource==s->catalogue.device_type&&s->element_bits==8&&!s->secondary_element_count&&s->pin_count==32&&s->capacity_bytes==262144&&s->pinmap==21&&s->family_code==(s->definition_resource==40?25u:26u);}
