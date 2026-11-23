#ifndef LT48_W29WORDTWO_MEMBER_H
#define LT48_W29WORDTWO_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "admission.h"
static inline int w29wordtwo_member(const lt48_catalogue_entry*e){return e&&e->device_class==2005&&e->vendor&&!strcmp(e->vendor,"Winbond")&&((e->device_type==78&&e->device_name_length==15&&!memcmp(e->device_name,"W29C101 *40TS/W",15))||(e->device_type==79&&e->device_name_length==15&&!memcmp(e->device_name,"W29C102 *40TS/W",15)));}
#endif
