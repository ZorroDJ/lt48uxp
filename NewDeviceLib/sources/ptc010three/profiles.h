/* Authored finite source bindings; vendor assets remain BYO. */
#include "admission.h"
static const char*ptc010three_names[]={"PT28C010 ","PT28C010 *32PLCC","PT28C010 *32TS"};
static inline int ptc010three_member(const lt48_catalogue_entry*e){if(!PTC010THREE_ADMITTED||e->device_class!=2034||e->device_type!=2||strcmp(e->vendor,"PTC"))return 0;for(unsigned i=0;i<3;i++)if(strlen(ptc010three_names[i])==e->device_name_length&&!memcmp(ptc010three_names[i],e->device_name,e->device_name_length))return 1;return 0;}
static inline int ptc010three_binding(const lt48_device_spec*s){return ptc010three_member(&s->catalogue)&&s->definition_resource==2&&s->family_code==2&&s->pinmap==2&&s->pin_count==32&&s->capacity_bytes==131072&&s->element_bits==8&&!s->secondary_element_count;}
