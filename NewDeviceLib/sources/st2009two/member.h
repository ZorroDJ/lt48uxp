#ifndef LT48_ST2009TWO_MEMBER_H
#define LT48_ST2009TWO_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
static inline int st2009two_member(const lt48_catalogue_entry*e){return e&&e->device_class==2009&&(e->device_type==0x13||e->device_type==0x14)&&e->vendor&&!strcmp(e->vendor,"ST");}
#endif
