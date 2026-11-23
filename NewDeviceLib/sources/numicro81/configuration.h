/* Authored profiles; vendor controller/FPGA payloads remain bring-your-own. */
struct numicro_field {unsigned category,field,maximum;const char*label;const char*options[5];};
static const struct numicro_field numicro_fields_25253[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"CBS=0",{NULL}},
{0,2,1,"CBORST=0",{NULL}},
{0,3,1,"CBODEN=0",{NULL}},
{0,4,1,"CKF=0",{NULL}},
{1,0,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.8V","Brown out 4.5V"}},
{1,1,1,"Oscillator Select",{"internal RC 22MHz Clock","External 12MHz crystal Clock"}},
};
static const struct numicro_field numicro_fields_25361[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"CIOINI=0",{NULL}},
{0,2,1,"CBORST=0",{NULL}},
{0,3,1,"CBODEN=0",{NULL}},
{0,4,1,"CWDTPDEN=0",{NULL}},
{0,5,1,"CWDTEN=0",{NULL}},
{1,0,3,"Boot Selection",{"Boot LDROM with IAP","Boot LDROM without IAP","Boot PROM with IAP","Boot APROM without IAP"}},
{1,1,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.8V","Brown out 4.5V"}},
{1,2,1,"Oscillator Select",{"internal RC 22MHz Clock","External 12MHz crystal Clock"}},
};
static const struct numicro_field numicro_fields_25385[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"DFVSEN=0",{NULL}},
{0,3,1,"CIOINI=0",{NULL}},
{0,4,1,"CBORST=0",{NULL}},
{0,5,1,"CBODEN=0",{NULL}},
{0,6,1,"CGPFMFP=0",{NULL}},
{0,7,1,"CWDTPDEN=0",{NULL}},
{1,0,3,"Boot Selection",{"CBS[1:0]  00","CBS[1:0]  01","CBS[1:0]  10","CBS[1:0]  11"}},
{1,1,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.7V","Brown out 4.4V"}},
{1,2,1,"CPU clock select",{"External 4~24 MHz clock","Int. RC 22.1184 MHz clock"}},
{1,3,2,"WatchDog Timer Select",{"WDT active except Powerdown","WDT function is inactive","WDT active. WDT clock is on."}},
{2,0,1048575,"Data Flash Address",{NULL}},
};
static const struct numicro_field numicro_fields_25380[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"CBS=0",{NULL}},
{0,2,1,"CIOINI=0",{NULL}},
{0,3,1,"CBORST=0",{NULL}},
{0,4,1,"CBODEN=0",{NULL}},
{0,5,1,"P7XTAL=0",{NULL}},
{0,6,1,"CKF=0",{NULL}},
{0,7,1,"CWDTPDEN=0",{NULL}},
{0,8,1,"CWDTEN=0",{NULL}},
{1,0,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.8V","Brown out 4.5V"}},
{1,1,1,"Oscillator Select",{"internal RC 22MHz Clock","External 12MHz crystal Clock"}},
};
static const struct numicro_field numicro_fields_25328[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"CBS=0",{NULL}},
{0,3,1,"CBORST=0",{NULL}},
{0,4,1,"CKF=0",{NULL}},
{1,0,3,"Brown-out Reset Volt.",{"Brown out 2.7V","Brown out 2.7V","Brown out 3.8V","Disable 2.7V/3.8V"}},
{2,0,1048575,"Data Flash Address",{NULL}},
};
static const struct numicro_field numicro_fields_25376[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"CKF=0",{NULL}},
{1,0,3,"Boot Selection",{"CBS[1:0]  00","CBS[1:0]  01","CBS[1:0]  10","CBS[1:0]  11"}},
{1,1,3,"Brown-out Reset Enable",{"[00]BOD17 reset enable","[01]BOD20 reset enable","[10]BOD25 reset enable","[11]Disable all BOD function"}},
{1,2,1,"CPU clock select",{"External 12MHz crystal(HXT)","Internal RC 12MHz (HIRC)"}},
{2,0,1048575,"Data Flash Address",{NULL}},
};
static const struct numicro_field numicro_fields_25377[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"CWDTEN=0",{NULL}},
{1,0,3,"Boot Selection",{"CBS[1:0]  00","CBS[1:0]  01","CBS[1:0]  10","CBS[1:0]  11"}},
{1,1,3,"Brown-out Reset Enable",{"[00]BOD17 reset enable","[01]BOD20 reset enable","[10]BOD25 reset enable","[11]Disable all BOD function"}},
{1,2,1,"CPU clock select",{"External 12MHz crystal(HXT)","Internal RC 12MHz (HIRC)"}},
{2,0,1048575,"Data Flash Address",{NULL}},
};
static const struct numicro_field numicro_fields_25304[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"CBS=0",{NULL}},
{0,3,1,"CBORST=0",{NULL}},
{0,4,1,"CBODEN=0",{NULL}},
{0,5,1,"CKF=0",{NULL}},
{0,6,1,"CWDTEN=0",{NULL}},
{1,0,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.8V","Brown out 4.5V"}},
{1,1,1,"Oscillator Select",{"internal RC 22MHz Clock","External 12MHz crystal Clock"}},
};
static const struct numicro_field numicro_fields_25305[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"CBS=0",{NULL}},
{0,3,1,"CBORST=0",{NULL}},
{0,4,1,"CBODEN=0",{NULL}},
{0,5,1,"CKF=0",{NULL}},
{0,6,1,"CWDTEN=0",{NULL}},
{1,0,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.8V","Brown out 4.5V"}},
{1,1,1,"Oscillator Select",{"internal RC 22MHz Clock","External 12MHz crystal Clock"}},
{2,0,1048575,"Data Flash Address",{NULL}},
};
static const struct numicro_field numicro_fields_25438[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"CIOINI=0",{NULL}},
{0,3,1,"ICELOCK=0",{NULL}},
{0,4,1,"CBORST=0",{NULL}},
{0,5,1,"CBOVEXT=0",{NULL}},
{0,6,1,"CFOSC=0",{NULL}},
{0,7,1,"CFGXT1=0",{NULL}},
{0,8,1,"CWDTPDEN=0",{NULL}},
{1,0,3,"Chip Booting Selection",{"CBS[1:0]  00","CBS[1:0]  01","CBS[1:0]  10","CBS[1:0]  11"}},
{1,1,3,"Brown-out Select bits",{"CBOV[1:0]  00","CBOV[1:0]  01","CBOV[1:0]  10","CBOV[1:0]  11"}},
{1,2,2,"WatchDog Timer Select",{"CWDTEN[2:0] = 011","CWDTEN[2:0] = 111","CWDTEN[2:0] = Others"}},
{2,0,1048575,"Data Flash Address",{NULL}},
};
#include "../ilitek1/configuration.h"
static const struct numicro_field*numicro_schema(unsigned id,unsigned*count){switch(id){
case 25327:*count=12;return numicro_fields_25327;
case 25253:*count=sizeof numicro_fields_25253/sizeof*numicro_fields_25253;return numicro_fields_25253;
case 25361:*count=sizeof numicro_fields_25361/sizeof*numicro_fields_25361;return numicro_fields_25361;
case 25385:*count=sizeof numicro_fields_25385/sizeof*numicro_fields_25385;return numicro_fields_25385;
case 25380:*count=sizeof numicro_fields_25380/sizeof*numicro_fields_25380;return numicro_fields_25380;
case 25328:*count=sizeof numicro_fields_25328/sizeof*numicro_fields_25328;return numicro_fields_25328;
case 25376:*count=sizeof numicro_fields_25376/sizeof*numicro_fields_25376;return numicro_fields_25376;
case 25377:*count=sizeof numicro_fields_25377/sizeof*numicro_fields_25377;return numicro_fields_25377;
case 25304:*count=sizeof numicro_fields_25304/sizeof*numicro_fields_25304;return numicro_fields_25304;
case 25305:*count=sizeof numicro_fields_25305/sizeof*numicro_fields_25305;return numicro_fields_25305;
case 25438:*count=sizeof numicro_fields_25438/sizeof*numicro_fields_25438;return numicro_fields_25438;
default:*count=0;return NULL;}}
