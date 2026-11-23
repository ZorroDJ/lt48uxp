#ifndef LT48_SM30_CONFIGURATION_H
#define LT48_SM30_CONFIGURATION_H
struct sm30_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct sm30_field sm30_fields_25179[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"LVD Enable",{NULL}},
{0,2,1,"6 Clock Enable.",{NULL}},
{0,3,1,"WDT Enable",{NULL}},
{0,4,1,"Clock_out_Disable",{NULL}},
{2,0,15,"ISP Memory Set",{NULL}},
};
static const struct sm30_field*sm30_schema(unsigned id,unsigned*count){switch(id){
case 25179:*count=6;return sm30_fields_25179;
default:*count=0;return NULL;}}
#endif
