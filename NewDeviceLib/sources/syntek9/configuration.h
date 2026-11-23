#ifndef LT48_SN9_CONFIGURATION_H
#define LT48_SN9_CONFIGURATION_H
struct sn9_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct sn9_field sn9_fields_25047[]={
{0,0,1,"Protect",{NULL}},
};
#include "../cion2/configuration.inc"
static const struct sn9_field*sn9_schema(unsigned id,unsigned*count){switch(id){case 25113:*count=1;return cion2_fields;
case 25047:*count=1;return sn9_fields_25047;
default:*count=0;return NULL;}}
#endif
