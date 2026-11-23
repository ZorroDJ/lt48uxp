#ifndef LT48_MW6_CONFIGURATION_H
#define LT48_MW6_CONFIGURATION_H
struct mw6_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct mw6_field mw6_fields_25062[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"Scramble Enable",{NULL}},
{0,2,1,"MOVC Inhibit",{NULL}},
{0,3,1,"Boot from ISP memory Enable",{NULL}},
{0,4,1,"6 Clock Enable.",{NULL}},
{2,0,15,"ISP Memory Set",{NULL}},
};
static const struct mw6_field*mw6_schema(unsigned id,unsigned*count){switch(id){
case 25062:*count=6;return mw6_fields_25062;
default:*count=0;return NULL;}}
#endif
