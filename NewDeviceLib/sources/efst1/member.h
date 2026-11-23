#ifndef LT48_EFST1_MEMBER_H
#define LT48_EFST1_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Exact source membership; no family-name admission. */
static int efst1_member(const lt48_catalogue_entry*e){return e&&e->device_class==2007&&e->device_type==197&&e->manufacturer_id==140&&e->device_id==0&&e->vendor&&!strcmp(e->vendor,"EFST")&&e->device_name_length==9&&!memcmp(e->device_name,"F49B002UA",9);}
#endif
