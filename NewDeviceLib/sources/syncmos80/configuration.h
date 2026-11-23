#ifndef LT48_SY80_CONFIGURATION_H
#define LT48_SY80_CONFIGURATION_H
struct sy80_field {unsigned category,field,maximum;const char*label;const char*options[17];};
/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct sy80_field sy80_fields_25232[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"WDT Enable",{NULL}},
{0,2,1,"OCI_SDA bi-direction I/O pin",{NULL}},
{0,3,1,"ALE bi-direction I/O pin",{NULL}},
{0,4,1,"OCI_SCL bi-direction I/O pin",{NULL}},
{0,5,1,"RESET bi-direction I/O pin",{NULL}},
{0,6,1,"ISP by P3.0 2clock pad reset",{NULL}},
{0,7,1,"ISP by P3.0 2clock power on",{NULL}},
{0,8,1,"ISP by P4.3 2clock pad reset",{NULL}},
{0,9,1,"ISP by P4.3 2clock power on",{NULL}},
{0,10,1,"ISP by P2.6 P2.7 pad reset",{NULL}},
{0,11,1,"ISP by P2.6 P2.7 pad power on",{NULL}},
{0,12,1,"ISP by first Addr pad reset",{NULL}},
{0,13,1,"ISP by first Addr pad power on",{NULL}},
{1,0,8,"Oscillator Select",{"extOSC","OSC 24MHz","OSC 20MHz","OSC 16MHz","OSC 12MHz","OSC 8MHz","OSC 4MHz","OSC 2MHz","OSC 1MHz"}},
{1,1,6,"Reset Timer Select",{"25ms(default)","4ms","8ms","16ms","50ms","100ms","200ms"}},
{1,2,16,"ISP service program Addr",{"No ISP service","3F00h~3FFFh","3E00h~3FFFh","3D00h~3FFFh","3C00h~3FFFh","3B00h~3FFFh","3A00h~3FFFh","3900h~3FFFh","3800h~3FFFh","3700h~3FFFh","3600h~3FFFh","3500h~3FFFh","3400h~3FFFh","3300h~3FFFh","3200h~3FFFh","3100h~3FFFh","3000h~3FFFh"}},
};
static const struct sy80_field sy80_fields_25247[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"WDT Enable",{NULL}},
{0,2,1,"OCI_SCL bi-direction I/O pin",{NULL}},
{0,3,1,"ALE bi-direction I/O pin",{NULL}},
{0,4,1,"OCI_SDA bi-direction I/O pin",{NULL}},
{0,5,1,"RESET bi-direction I/O pin",{NULL}},
{0,6,1,"ISP by P3.0 2clock pad reset",{NULL}},
{0,7,1,"ISP by P3.0 2clock power on",{NULL}},
{0,8,1,"ISP by P4.3 2clock pad reset",{NULL}},
{0,9,1,"ISP by P4.3 2clock power on",{NULL}},
{0,10,1,"ISP by P2.6 P2.7 pad reset",{NULL}},
{0,11,1,"ISP by P2.6 P2.7 pad power on",{NULL}},
{0,12,1,"ISP by first Addr pad reset",{NULL}},
{0,13,1,"ISP by first Addr pad power on",{NULL}},
{1,0,8,"Oscillator Select",{"extOSC","OSC 24MHz","OSC 20MHz","OSC 16MHz","OSC 12MHz","OSC 8MHz","OSC 4MHz","OSC 2MHz","OSC 1MHz"}},
{1,1,6,"Reset Timer Select",{"25ms(default)","4ms","8ms","16ms","50ms","100ms","200ms"}},
{1,2,16,"ISP service program Addr",{"No ISP service","FF00h~FFFFh","FE00h~FFFFh","FD00h~FFFFh","FC00h~FFFFh","FB00h~FFFFh","FA00h~FFFFh","F900h~FFFFh","F800h~FFFFh","F700h~FFFFh","F600h~FFFFh","F500h~FFFFh","F400h~FFFFh","F300h~FFFFh","F200h~FFFFh","F100h~FFFFh","F000h~FFFFh"}},
};
static const struct sy80_field sy80_fields_25245[]={
{0,0,1,"Protect",{NULL}},
{0,1,1,"WDT Enable",{NULL}},
{0,2,1,"RTCCLK1",{NULL}},
{0,3,1,"OCI_SCL bi-direction I/O pin",{NULL}},
{0,4,1,"ALE bi-direction I/O pin",{NULL}},
{0,5,1,"OCI_SDA bi-direction I/O pin",{NULL}},
{0,6,1,"RESET bi-direction I/O pin",{NULL}},
{0,7,1,"ISP by P3.0 2clock pad reset",{NULL}},
{0,8,1,"ISP by P3.0 2clock power on",{NULL}},
{0,9,1,"ISP by P4.3 2clock pad reset",{NULL}},
{0,10,1,"ISP by P4.3 2clock power on",{NULL}},
{0,11,1,"ISP by P2.6 P2.7 pad reset",{NULL}},
{0,12,1,"ISP by P2.6 P2.7 pad power on",{NULL}},
{0,13,1,"ISP by first Addr pad reset",{NULL}},
{0,14,1,"ISP by first Addr pad power on",{NULL}},
{0,15,1,"Cmp1 positive as ext. pin",{NULL}},
{0,16,1,"Cmp1 positive as bandgap voltage",{NULL}},
{0,17,1,"Cmp1 enable",{NULL}},
{0,18,1,"Op1 enable",{NULL}},
{0,19,1,"Cmp0 positive as ext. pin",{NULL}},
{0,20,1,"Cmp0 positive as bandgap voltage",{NULL}},
{0,21,1,"Cmp0 enable",{NULL}},
{0,22,1,"Op0 enable",{NULL}},
{1,0,8,"Oscillator Select",{"extOSC","OSC 24MHz","OSC 20MHz","OSC 16MHz","OSC 12MHz","OSC 8MHz","OSC 4MHz","OSC 2MHz","OSC 1MHz"}},
{1,1,6,"Reset Timer Select",{"25ms(default)","4ms","8ms","16ms","50ms","100ms","200ms"}},
{1,2,16,"ISP service program Addr",{"No ISP service","FF00h~FFFFh","FE00h~FFFFh","FD00h~FFFFh","FC00h~FFFFh","FB00h~FFFFh","FA00h~FFFFh","F900h~FFFFh","F800h~FFFFh","F700h~FFFFh","F600h~FFFFh","F500h~FFFFh","F400h~FFFFh","F300h~FFFFh","F200h~FFFFh","F100h~FFFFh","F000h~FFFFh"}},
};
static const struct sy80_field*sy80_schema(unsigned id,unsigned*count){switch(id){
case 25232:*count=17;return sy80_fields_25232;
case 25247:*count=17;return sy80_fields_25247;
case 25245:*count=26;return sy80_fields_25245;
default:*count=0;return NULL;}}
#endif
