#ifndef LT48_MY4_CONFIGURATION_H
#define LT48_MY4_CONFIGURATION_H
struct my4_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct my4_field my4_fields_25047[]={
{0,0,1,"Protect",{NULL}},
};
static const struct my4_field*my4_schema(unsigned id,unsigned*count){switch(id){
case 25047:*count=1;return my4_fields_25047;
default:*count=0;return NULL;}}
#endif
