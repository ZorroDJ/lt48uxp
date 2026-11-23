#ifndef LT48_SO30_CONFIGURATION_H
#define LT48_SO30_CONFIGURATION_H
struct so30_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct so30_field so30_fields_25047[]={
{0,0,1,"Protect",{NULL}},
};
static const struct so30_field so30_fields_25061[]={
{0,0,1,"WDT Enable",{NULL}},
{0,1,1,"Code Protect",{NULL}},
{1,0,2,"Oscillator Select",{"XT Osc","HF","RC"}},
{1,1,2,"Low Voltage Detector Selection",{"3.6V","2.0V","Disable"}},
{1,2,2,"Instruction cycle",{"eight cycle","two cycle","four cycle"}},
{1,3,1,"Prog Config Setting",{"from file","config"}},
};
static const struct so30_field*so30_schema(unsigned id,unsigned*count){switch(id){
case 25047:*count=1;return so30_fields_25047;
case 25061:*count=6;return so30_fields_25061;
default:*count=0;return NULL;}}
#endif
