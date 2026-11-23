#ifndef LT48_OB16_CONFIGURATION_H
#define LT48_OB16_CONFIGURATION_H
struct ob16_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct ob16_field ob16_fields_25359[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVRVTH 2.0V",{NULL}},
{0,2,1,"LVRE disable",{NULL}},
{0,3,1,"CK20K_SEL enable",{NULL}},
{0,4,1,"XTAL32K=0",{NULL}},
{0,5,1,"CLKOUT bi-direction I/O pin.",{NULL}},
{0,6,1,"RESET bi-direction I/O pin.",{NULL}},
{1,0,6,"Peripheral clock select",{"24MHz","12MHz","6MHz","3MHz","1.5MHz","0.75MHz","reserved"}},
{1,1,6,"CPU clock select",{"reserved","12MHz","6MHz","3MHz","1.5MHz","0.75MHz","reserved"}},
{1,2,3,"LVR Select",{"Disable","no debounce","10us debounce","20us debounce"}},
{2,0,255,"Port 0 Value",{NULL}},
{2,1,255,"P1/P3 Value",{NULL}},
{2,2,255,"P0 mode 1",{NULL}},
{2,3,255,"P1/P3 mode 1",{NULL}},
};
static const struct ob16_field ob16_fields_25339[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"CLKOUT bi-direction I/O pin.",{NULL}},
{0,2,1,"RESET bi-direction I/O pin.",{NULL}},
{1,0,5,"Oscillator Select",{"24MHz","12MHz","6MHz","3MHz","1.5MHz","0.75MHz"}},
};
static const struct ob16_field*ob16_schema(unsigned id,unsigned*count){switch(id){
case 25359:*count=14;return ob16_fields_25359;
case 25339:*count=4;return ob16_fields_25339;
default:*count=0;return NULL;}}
#endif
