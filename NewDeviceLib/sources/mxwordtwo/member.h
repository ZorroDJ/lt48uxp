#ifndef LT48_MXWORDTWO_MEMBER_H
#define LT48_MXWORDTWO_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "admission.h"
static inline int mxwordtwo_member(const lt48_catalogue_entry*e){return e&&e->device_class==2005&&e->vendor&&!strcmp(e->vendor,"Macronix")&&((e->device_type==0x3a&&e->device_name_length==15&&!memcmp(e->device_name,"MX29F1611 *44PS",15))||(e->device_type==0x3f&&e->device_name_length==9&&!memcmp(e->device_name,"MX29F1615",9))||(e->device_type==0x3e&&e->device_name_length==10&&!memcmp(e->device_name,"MX29L1611G",10)));}
#endif
