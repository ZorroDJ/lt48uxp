#ifndef LT48_WT10_CONFIGURATION_H
#define LT48_WT10_CONFIGURATION_H
struct wt10_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct wt10_field*wt10_schema(unsigned id,unsigned*count){switch(id){
default:*count=0;return NULL;}}
#endif
