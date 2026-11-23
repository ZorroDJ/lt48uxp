#ifndef LT48_MW27_CONFIGURATION_H
#define LT48_MW27_CONFIGURATION_H
struct mw27_field {unsigned category,field,maximum;const char*label;const char*options[8];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct mw27_field mw27_fields_25193[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"SB",{NULL}},
{0,2,1,"HWBS2",{NULL}},
{0,3,1,"HWBS",{NULL}},
{0,4,1,"ISPAS0",{NULL}},
{0,5,1,"ISPAS1",{NULL}},
{0,6,1,"ISPAS2",{NULL}},
{0,7,1,"FAPD",{NULL}},
{0,8,1,"HWPS0",{NULL}},
{0,9,1,"HWPS1",{NULL}},
{0,10,1,"HWPS2",{NULL}},
{0,11,1,"HWWIDL",{NULL}},
{0,12,1,"HWENW",{NULL}},
{0,13,1,"WDTCR_WP",{NULL}},
{2,0,255,"IAP Memory Set",{NULL}},
};
static const struct mw27_field mw27_fields_25120[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"SB",{NULL}},
{0,2,1,"HWBS",{NULL}},
{0,3,1,"ISPAS0",{NULL}},
{0,4,1,"ISPAS1",{NULL}},
{0,5,1,"ENLVR",{NULL}},
{0,6,1,"LVFWP",{NULL}},
{0,7,1,"ENROSC",{NULL}},
{0,8,1,"OSCDN",{NULL}},
{0,9,1,"HWPS0",{NULL}},
{0,10,1,"HWPS1",{NULL}},
{0,11,1,"HWPS2",{NULL}},
{0,12,1,"HWWIDL",{NULL}},
{0,13,1,"HWENW",{NULL}},
{2,0,255,"IAP Memory Set",{NULL}},
};
static const struct mw27_field mw27_fields_25133[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"SB",{NULL}},
{0,2,1,"HWBS",{NULL}},
{0,3,1,"ISPAS0",{NULL}},
{0,4,1,"ISPAS1",{NULL}},
{0,5,1,"ENLVR",{NULL}},
{0,6,1,"LVFWP",{NULL}},
{0,7,1,"ENROSC",{NULL}},
{0,8,1,"HWBS2",{NULL}},
{0,9,1,"OSCDN",{NULL}},
{0,10,1,"HWPS0",{NULL}},
{0,11,1,"HWPS1",{NULL}},
{0,12,1,"HWPS2",{NULL}},
{0,13,1,"HWWIDL",{NULL}},
{0,14,1,"HWENW",{NULL}},
{2,0,255,"IAP Memory Set",{NULL}},
};
static const struct mw27_field mw27_fields_25166[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"SB",{NULL}},
{0,2,1,"HWBS",{NULL}},
{0,3,1,"ENLVRC",{NULL}},
{0,4,1,"LVFWP",{NULL}},
{0,5,1,"ENROSC",{NULL}},
{0,6,1,"EVLVRO",{NULL}},
{0,7,1,"HWBS2",{NULL}},
{0,8,1,"OSCDN",{NULL}},
{0,9,1,"HWWIDL",{NULL}},
{0,10,1,"HWENW",{NULL}},
{0,11,1,"WDSFWP",{NULL}},
{1,0,3,"ISP service program Addr",{"F000H","F800H","FC00H","No ISP code"}},
{1,1,7,"WDT Pre-Scalar",{"FOSC/2","FOSC/4","FOSC/8","FOSC/16","FOSC/32","FOSC/64","FOSC/128","FOSC/256"}},
{2,0,255,"IAP Memory Set",{NULL}},
};
static const struct mw27_field*mw27_schema(unsigned id,unsigned*count){switch(id){
case 25193:*count=15;return mw27_fields_25193;
case 25120:*count=15;return mw27_fields_25120;
case 25133:*count=16;return mw27_fields_25133;
case 25166:*count=15;return mw27_fields_25166;
default:*count=0;return NULL;}}
#endif
