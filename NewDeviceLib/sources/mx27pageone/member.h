#ifndef LT48_MX27PAGEONE_MEMBER_H
#define LT48_MX27PAGEONE_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "admission.h"
static inline int mx27pageone_member(const lt48_catalogue_entry*e){return e&&e->device_class==2005&&e->device_type==0x3d&&e->vendor&&!strcmp(e->vendor,"Macronix")&&e->device_name_length==9&&!memcmp(e->device_name,"MX27C1610",9);}
#endif
