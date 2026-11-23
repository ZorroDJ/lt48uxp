#ifndef LT48_MW8_CONFIGURATION_H
#define LT48_MW8_CONFIGURATION_H
struct mw8_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct mw8_field mw8_fields_25340[]={
{0,0,1,"HWBS",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"HWBS2",{NULL}},
{0,3,1,"BO0REO",{NULL}},
{0,4,1,"HWWIDL",{NULL}},
{0,5,1,"WRENO",{NULL}},
{0,6,1,"HWENW",{NULL}},
{0,7,1,"NSWDT",{NULL}},
{0,8,1,"WDSFWP",{NULL}},
{0,9,1,"P36EN",{NULL}},
{1,0,7,"ISP Memory size",{"3.5K bytes","3K bytes","2.5K bytes","2K bytes","1.5K bytes","1K bytes","0.5K bytes","No ISP code"}},
{1,1,7,"WDT Pre-Scalar",{"FOSC/2","FOSC/4","FOSC/8","FOSC/16","FOSC/32","FOSC/64","FOSC/128","FOSC/256"}},
{2,0,255,"IAP boundary",{NULL}},
};
static const struct mw8_field*mw8_schema(unsigned id,unsigned*count){switch(id){
case 25340:*count=13;return mw8_fields_25340;
default:*count=0;return NULL;}}
#endif
