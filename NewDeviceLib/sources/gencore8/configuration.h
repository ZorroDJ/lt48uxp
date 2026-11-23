#ifndef LT48_GC8_CONFIGURATION_H
#define LT48_GC8_CONFIGURATION_H
struct gc8_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct gc8_field*gc8_schema(unsigned id,unsigned*count){switch(id){
default:*count=0;return NULL;}}
#endif
