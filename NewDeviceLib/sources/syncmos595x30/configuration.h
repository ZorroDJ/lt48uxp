#ifndef LT48_SX30_CONFIGURATION_H
#define LT48_SX30_CONFIGURATION_H
struct sx30_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct sx30_field sx30_fields_25333[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"System clock 6T Enable",{NULL}},
{0,2,1,"ISP entry mechanism Enable",{NULL}},
{0,3,1,"Oscillator power saving Enable",{NULL}},
{1,0,8,"ISP service program Addr",{"No ISP service","1E00h~1FFFh","1C00h~1FFFh","1A00h~1FFFh","1800h~1FFFh","1600h~1FFFh","1400h~1FFFh","1200h~1FFFh","1000h~1FFFh"}},
};
static const struct sx30_field sx30_fields_25357[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVR Filter Disable",{NULL}},
{0,2,1,"System clock 6T Enable",{NULL}},
{0,3,1,"ISP entry mechanism(P2.6P2.7=0)",{NULL}},
{0,4,1,"Oscillator power saving Enable",{NULL}},
{0,5,1,"ISP entry mechanism(P4.3=0)",{NULL}},
{1,0,9,"ISP service program Addr",{"No ISP service","2E00h~2FFFh","2C00h~2FFFh","2A00h~2FFFh","2800h~2FFFh","2600h~2FFFh","2400h~2FFFh","2200h~2FFFh","2000h~2FFFh","No ISP service"}},
{1,1,3,"XTAL Noise Filter",{"12MHz","20MHz","20MHz","40MHz"}},
};
static const struct sx30_field sx30_fields_25334[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVR Filter Disable",{NULL}},
{0,2,1,"System clock 6T Enable",{NULL}},
{0,3,1,"ISP entry mechanism Enable",{NULL}},
{0,4,1,"Oscillator power saving Enable",{NULL}},
{1,0,8,"ISP service program Addr",{"No ISP service","3E00h~3FFFh","3C00h~3FFFh","3A00h~3FFFh","3800h~3FFFh","3600h~3FFFh","3400h~3FFFh","3200h~3FFFh","3000h~3FFFh"}},
};
static const struct sx30_field sx30_fields_25356[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVR Filter Disable",{NULL}},
{0,2,1,"System clock 6T Enable",{NULL}},
{0,3,1,"BLPEN Enable",{NULL}},
{0,4,1,"Oscillator power saving Enable",{NULL}},
{0,5,1,"BLPEN2 Enable",{NULL}},
{1,0,9,"ISP service program Addr",{"No ISP service","4E00h~4FFFh","4C00h~4FFFh","4A00h~4FFFh","4800h~4FFFh","4600h~4FFFh","4400h~4FFFh","4200h~4FFFh","4000h~4FFFh","No ISP service"}},
{1,1,3,"XTAL Noise Filter",{"12MHz","20MHz","20MHz","40MHz"}},
};
static const struct sx30_field sx30_fields_25352[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVR Filter Disable",{NULL}},
{0,2,1,"System clock 6T Enable",{NULL}},
{0,3,1,"BLPEN Enable",{NULL}},
{0,4,1,"Oscillator power saving Enable",{NULL}},
{0,5,1,"BLPEN2 Enable",{NULL}},
{1,0,9,"ISP service program Addr",{"No ISP service","7E00h~7FFFh","7C00h~7FFFh","7A00h~7FFFh","7800h~7FFFh","7600h~7FFFh","7400h~7FFFh","7200h~7FFFh","7000h~7FFFh","No ISP service"}},
{1,1,3,"XTAL Noise Filter",{"12MHz","20MHz","20MHz","40MHz"}},
};
static const struct sx30_field sx30_fields_25347[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVR Filter Disable",{NULL}},
{0,2,1,"System clock 6T Enable",{NULL}},
{0,3,1,"BLPEN Enable",{NULL}},
{0,4,1,"Oscillator power saving Enable",{NULL}},
{0,5,1,"BLPEN2 Enable",{NULL}},
{1,0,9,"ISP service program Addr",{"No ISP service","FE00h~FFFFh","FC00h~FFFFh","FA00h~FFFFh","F800h~FFFFh","F600h~FFFFh","F400h~FFFFh","F200h~FFFFh","F000h~FFFFh","No ISP service"}},
{1,1,3,"XTAL Noise Filter",{"12MHz","20MHz","20MHz","40MHz"}},
};
static const struct sx30_field sx30_fields_25346[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVR Filter Disable",{NULL}},
{0,2,1,"System clock 6T Enable",{NULL}},
{0,3,1,"Oscillator power saving Enable",{NULL}},
{1,0,3,"XTAL Noise Filter",{"12MHz","20MHz","20MHz","40MHz"}},
};
static const struct sx30_field sx30_fields_25345[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"LVR Filter Disable",{NULL}},
{0,2,1,"System clock 6T Enable",{NULL}},
{0,3,1,"Oscillator power saving Enable",{NULL}},
};
static const struct sx30_field*sx30_schema(unsigned id,unsigned*count){switch(id){
case 25333:*count=5;return sx30_fields_25333;
case 25357:*count=8;return sx30_fields_25357;
case 25334:*count=6;return sx30_fields_25334;
case 25356:*count=8;return sx30_fields_25356;
case 25352:*count=8;return sx30_fields_25352;
case 25347:*count=8;return sx30_fields_25347;
case 25346:*count=5;return sx30_fields_25346;
case 25345:*count=4;return sx30_fields_25345;
default:*count=0;return NULL;}}
#endif
