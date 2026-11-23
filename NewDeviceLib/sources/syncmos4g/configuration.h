#ifndef LT48_SC4G_CONFIGURATION_H
#define LT48_SC4G_CONFIGURATION_H
struct sc4g_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct sc4g_field sc4g_fields_25270[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"WDT Enable",{NULL}},
{0,2,1,"P4.4 bi-direction I/O pin",{NULL}},
{0,3,1,"ALE bi-direction I/O pin",{NULL}},
{0,4,1,"P4.6 bi-direction I/O pin",{NULL}},
{0,5,1,"RESET bi-direction I/O pin",{NULL}},
{0,6,1,"ISP by P3.0 2clock pad reset",{NULL}},
{0,7,1,"ISP by P3.0 2clock power on",{NULL}},
{0,8,1,"ISP by P4.3 2clock pad reset",{NULL}},
{0,9,1,"ISP by P4.3 2clock power on",{NULL}},
{0,10,1,"ISP by P2.6 P2.7 pad reset",{NULL}},
{0,11,1,"ISP by P2.6 P2.7 pad power on",{NULL}},
{0,12,1,"ISP by first Addr pad reset",{NULL}},
{0,13,1,"ISP by first Addr pad power on",{NULL}},
{1,0,6,"Oscillator Select",{"XTAL","OSC","22.1184/2MHz","22.1184/4MHz","22.1184/8MHz","22.1184/16MHz","22.1184 MHz"}},
{1,1,6,"Reset Timer Select",{"25ms(default)","4ms","8ms","16ms","50ms","100ms","200ms"}},
{1,2,4,"ISP service program Addr",{"No ISP service","1F00h~1FFFh","1E00h~1FFFh","1D00h~1FFFh","1C00h~1FFFh"}},
};
static const struct sc4g_field*sc4g_schema(unsigned id,unsigned*count){switch(id){
case 25270:*count=17;return sc4g_fields_25270;
default:*count=0;return NULL;}}
#endif
