#ifndef LT48_SM2964TWO_MEMBER_H
#define LT48_SM2964TWO_MEMBER_H
#include <labtool48/labtool48.h>
#include <string.h>
static int sm2964two_member(const lt48_catalogue_entry*e){return e&&e->device_class==2007&&e->vendor&&!strcmp(e->vendor,"SyncMos")&&((e->device_type==23&&e->device_name_length==7&&!memcmp(e->device_name,"SM2964 ",7))||(e->device_type==24&&e->device_name_length==10&&!memcmp(e->device_name,"SM2964 *44",10)));}
#endif
