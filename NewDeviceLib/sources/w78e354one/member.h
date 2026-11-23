#ifndef LT48_W78E354ONE_MEMBER_H
#define LT48_W78E354ONE_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
static inline int w78e354one_member(const lt48_catalogue_entry*e){return e&&e->device_class==2038&&e->device_type==0x37&&e->vendor&&!strcmp(e->vendor,"Winbond")&&e->device_name_length==7&&!memcmp(e->device_name,"W78E354",7);}
#endif
