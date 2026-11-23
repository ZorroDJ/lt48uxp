#ifndef LT48_AB11_CONFIGURATION_H
#define LT48_AB11_CONFIGURATION_H
struct ab11_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct ab11_field ab11_fields_25308[]={
{1,0,7,"Oscillator Select",{"External RC","Internal RC 4MHz","Internal RC 2MHz","Internal RC 1MHz","Internal RC 8MHz","reserved","reserved","Crystal/ceramic"}},
{1,1,1,"Lock Bits Program",{"Disable","Enable"}},
{1,2,3,"Function Select",{"00: 2.4V","01: 2.7V","10: 3.0V","11: 4.0V"}},
{1,3,1,"LVR Select",{"Enable","Disable"}},
};
static const struct ab11_field*ab11_schema(unsigned id,unsigned*count){switch(id){
case 25308:*count=4;return ab11_fields_25308;
default:*count=0;return NULL;}}
#endif
