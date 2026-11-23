#ifndef LT48_SC4A_CONFIGURATION_H
#define LT48_SC4A_CONFIGURATION_H
struct sc4a_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct sc4a_field sc4a_fields_25187[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVD Enable",{NULL}},
{1,0,16,"WatchDog Timer Select",{"Disable","0000","0001","0010","0011","0100","0101","0110","0111","1000","1001","1010","1011","1100","1101","1110","1111"}},
{1,1,8,"Oscillator Select",{"extOSC","OSC 24MHz","OSC 20MHz","OSC 16MHz","OSC 12MHz","OSC 8MHz","OSC 4MHz","OSC 2MHz","OSC 1MHz"}},
};
static const struct sc4a_field*sc4a_schema(unsigned id,unsigned*count){switch(id){
case 25187:*count=4;return sc4a_fields_25187;
default:*count=0;return NULL;}}
#endif
