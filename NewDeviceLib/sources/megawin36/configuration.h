#ifndef LT48_MW36_CONFIGURATION_H
#define LT48_MW36_CONFIGURATION_H
struct mw36_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct mw36_field mw36_fields_25259[]={
{0,0,1,"HWBS",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"P40IOE",{NULL}},
{0,3,1,"P41IOE",{NULL}},
{0,4,1,"ENRCO",{NULL}},
{0,5,1,"OSCDN",{NULL}},
{0,6,1,"HWBS2",{NULL}},
{0,7,1,"BODRE",{NULL}},
{0,8,1,"BODWP",{NULL}},
{0,9,1,"HWWIDL",{NULL}},
{0,10,1,"HWENW",{NULL}},
{0,11,1,"WDTRCO",{NULL}},
{0,12,1,"WDSFWP",{NULL}},
{1,0,7,"ISP Memory size",{"4K bytes","3.5K bytes","3K bytes","2.5K bytes","2K bytes","1.5K bytes","1K bytes","No ISP code"}},
{1,1,7,"WDT Pre-Scalar",{"FOSC/2","FOSC/4","FOSC/8","FOSC/16","FOSC/32","FOSC/64","FOSC/128","FOSC/256"}},
{2,0,255,"IAP boundary",{NULL}},
};
static const struct mw36_field mw36_fields_25258[]={
{0,0,1,"HWBS",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"ENRCO",{NULL}},
{0,3,1,"OSCDN",{NULL}},
{0,4,1,"HWBS2",{NULL}},
{0,5,1,"BODRE",{NULL}},
{0,6,1,"HWWIDL",{NULL}},
{0,7,1,"HWENW",{NULL}},
{0,8,1,"WDTRCO",{NULL}},
{0,9,1,"WDSFWP",{NULL}},
{1,0,7,"ISP Memory size",{"4K bytes","3.5K bytes","3K bytes","2.5K bytes","2K bytes","1.5K bytes","1K bytes","No ISP code"}},
{1,1,7,"WDT Pre-Scalar",{"FOSC/2","FOSC/4","FOSC/8","FOSC/16","FOSC/32","FOSC/64","FOSC/128","FOSC/256"}},
{2,0,255,"IAP boundary",{NULL}},
};
static const struct mw36_field mw36_fields_25231[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"HWBS",{NULL}},
{0,2,1,"HWBS2",{NULL}},
{0,3,1,"ISPAS0",{NULL}},
{0,4,1,"ISPAS1",{NULL}},
{0,5,1,"ISPAS2",{NULL}},
{0,6,1,"6 Clock Enable.",{NULL}},
{0,7,1,"ENRCO",{NULL}},
{0,8,1,"OSCDN",{NULL}},
{0,9,1,"HWPS0",{NULL}},
{0,10,1,"HWPS1",{NULL}},
{0,11,1,"HWPS2",{NULL}},
{0,12,1,"HWWIDL",{NULL}},
{0,13,1,"HWENW",{NULL}},
{0,14,1,"WDSFWP",{NULL}},
};
#include "../mwM2/configuration.inc"
#include "../mwF6one/configuration.inc"
static const struct mw36_field*mw36_schema(unsigned id,unsigned*count){switch(id){
#include "../mwF6one/schema-cases.inc"
#include "../mwM2/schema-cases.inc"
case 25259:*count=16;return mw36_fields_25259;
case 25258:*count=13;return mw36_fields_25258;
case 25231:*count=15;return mw36_fields_25231;
default:*count=0;return NULL;}}
#endif
