#ifndef LT48_WT8_CONFIGURATION_H
#define LT48_WT8_CONFIGURATION_H
struct wt8_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct wt8_field*wt8_schema(unsigned id,unsigned*count){switch(id){
default:*count=0;return NULL;}}
#endif
