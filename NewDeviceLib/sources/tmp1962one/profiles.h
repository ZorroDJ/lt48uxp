/* Authored finite source binding; original assets and adapter remain BYO. */
#include "admission.h"
static inline int tmp1962one_member(const lt48_catalogue_entry*e){const char*n="TMP1962F10AXB @281BGA";return TMP1962ONE_ADMITTED&&e->device_class==2031&&e->device_type==0x14c&&!strcmp(e->vendor,"Toshiba")&&e->device_name_length==strlen(n)&&!memcmp(e->device_name,n,strlen(n));}
static inline int tmp1962one_binding(const lt48_device_spec*s){return tmp1962one_member(&s->catalogue)&&s->definition_resource==0x14c&&s->family_code==0x126&&s->pinmap==0x69&&s->pin_count==48&&s->capacity_bytes==1048576&&s->element_bits==16&&!s->secondary_element_count;}
#ifdef LT48_TMP1962ONE_XP_INTERNAL
static const uint32_t tmp1962one_sectors[]={0,0x20000,0x40000,0x60000,0x80000,0xa0000,0xc0000,0xe0000};
static const struct xp_closure tmp1962one_closure={0x14c,0x126,0x69,48,1048576,250,tmp1962one_sectors,8};
static const struct xp_shared_word_shape tmp1962one_shape={0x14c,16,7,1};
static const struct xp_primary_erase tmp1962one_primary={0x14c,tmp1962one_sectors,8};
#endif
