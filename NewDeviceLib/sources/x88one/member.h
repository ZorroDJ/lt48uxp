#ifndef LT48_X88ONE_MEMBER_H
#define LT48_X88ONE_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "admission.h"
static inline int x88one_member(const lt48_catalogue_entry*e){return e&&e->device_class==2005&&e->device_type==0x28&&e->vendor&&!strcmp(e->vendor,"Xicor")&&e->device_name_length==6&&!memcmp(e->device_name,"X88C64",6);}
#endif
