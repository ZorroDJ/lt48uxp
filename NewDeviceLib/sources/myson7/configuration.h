#ifndef LT48_MY7_CONFIGURATION_H
#define LT48_MY7_CONFIGURATION_H
struct my7_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct my7_field*my7_schema(unsigned id,unsigned*count){switch(id){
default:*count=0;return NULL;}}
#endif
