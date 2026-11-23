#ifndef LT48_W27E4096TWO_MEMBER_H
#define LT48_W27E4096TWO_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "admission.h"
static inline int w27e4096two_member(const lt48_catalogue_entry*e){return e&&e->device_class==2003&&e->device_type==0xa2&&e->vendor&&!strcmp(e->vendor,"Winbond")&&((e->device_name_length==8&&!memcmp(e->device_name,"W27E4096",8))||(e->device_name_length==12&&!memcmp(e->device_name,"W27E4096 *40",12)));}
#endif
