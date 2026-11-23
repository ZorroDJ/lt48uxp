#ifndef LT48_HY14_CONFIGURATION_H
#define LT48_HY14_CONFIGURATION_H
struct hy14_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct hy14_field hy14_fields_25047[]={
{0,0,1,"Protect",{NULL}},
};
static const struct hy14_field hy14_fields_25048[]={
{0,0,1,"RC osc = 1",{NULL}},
{0,1,1,"LOCK Enable",{NULL}},
{0,2,1,"Xin_opt = 1",{NULL}},
{0,3,1,"LF_on Enable",{NULL}},
{0,4,1,"IN_CLK (4MHz) Enable",{NULL}},
{0,5,1,"ONPb Enable",{NULL}},
{0,6,1,"Addr Fail RESET Enable",{NULL}},
};
static const struct hy14_field*hy14_schema(unsigned id,unsigned*count){switch(id){
case 25047:*count=1;return hy14_fields_25047;
case 25048:*count=7;return hy14_fields_25048;
default:*count=0;return NULL;}}
#endif
