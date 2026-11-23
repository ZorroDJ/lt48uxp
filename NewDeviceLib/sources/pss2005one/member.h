#ifndef LT48_PSS2005ONE_MEMBER_H
#define LT48_PSS2005ONE_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "admission.h"
static inline int pss2005one_member(const lt48_catalogue_entry*e){return e&&e->device_class==2005&&e->device_type==14&&e->vendor&&!strcmp(e->vendor,"PSS")&&((e->device_name_length==9&&!memcmp(e->device_name,"PS29FS001",9))||(e->device_name_length==9&&!memcmp(e->device_name,"PS29LP001",9)));}
#endif
