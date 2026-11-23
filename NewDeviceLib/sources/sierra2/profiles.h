/* Authored finite Sierra multiplexed EEPROM profiles. Vendor assets remain BYO. */
#include "admission.h"
static inline int sierra2_member(const lt48_catalogue_entry*e){return SIERRA2_ADMITTED&&e->device_class==2034&&e->device_type==9&&!strcmp(e->vendor,"SIERRA")&&e->device_name_length==7&&(!memcmp(e->device_name,"SC22101",7)||!memcmp(e->device_name,"SC22201",7));}
static inline int sierra2_binding(const lt48_device_spec*s){return sierra2_member(&s->catalogue)&&s->definition_resource==9&&s->family_code==7&&s->pinmap==9&&s->pin_count==18&&s->element_bits==8&&s->capacity_bytes==128&&!s->secondary_element_count;}
