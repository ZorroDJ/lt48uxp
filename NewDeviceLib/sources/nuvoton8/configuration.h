static const struct numicro_field nv8_fields_25290[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"CBS=0",{NULL}},
{0,2,1,"CBORST=0",{NULL}},
{0,3,1,"CBOV=0",{NULL}},
{0,4,1,"CBODEN=0",{NULL}},
{0,5,1,"OSCFS=0",{NULL}},
{0,6,1,"CKF=0",{NULL}},
{0,7,1,"CWDTEN=0",{NULL}},
{1,0,3,"Oscillator Select",{"External crystal,4MHz~24MHz","External crystal,4MHz~24MHz","External oscillator fromXTAL1","Internal RC oscillator"}},
};
static const struct numicro_field nv8_fields_25445[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"CIOINI=0",{NULL}},
{0,2,1,"CBORST=0",{NULL}},
{0,3,1,"CBODEN=0",{NULL}},
{0,4,1,"CWDTPDEN=0",{NULL}},
{0,5,1,"CWDTEN=0",{NULL}},
{1,0,3,"Boot Selection",{"Boot LDROM with IAP","Boot LDROM without IAP","Boot PROM with IAP","Boot APROM without IAP"}},
{1,1,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.7V","Brown out 4.4V"}},
{1,2,1,"Oscillator Select",{"External 4~24 MHz clock","Int. RC 22.1184 MHz clock"}},
};
static const struct numicro_field nv8_fields_25385[]={
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
static const struct numicro_field nv8_fields_25408[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"MERASE=0",{NULL}},
{0,3,1,"CIOINI=0",{NULL}},
{0,4,1,"ICELOCK=0",{NULL}},
{0,5,1,"CBORST=0",{NULL}},
{0,6,1,"CBODEN=0",{NULL}},
{0,7,1,"CFOSC=0",{NULL}},
{0,8,1,"CFGXT1=0",{NULL}},
{0,9,1,"CWDTPDEN=0",{NULL}},
{1,0,3,"Boot Selection",{"CBS[1:0]  00","CBS[1:0]  01","CBS[1:0]  10","CBS[1:0]  11"}},
{1,1,3,"Brown-out Reset Volt.",{"Brown out 2.2V","Brown out 2.7V","Brown out 3.7V","Brown out 4.5V"}},
{1,2,2,"WatchDog Timer Enable",{"WDT active except Powerdown","WDT function is inactive","WDT active. WDT clock is on."}},
{2,0,1048575,"Data Flash Address",{NULL}},
};
static const struct numicro_field*nv8_schema(unsigned id,unsigned*count){switch(id){
case 25290:*count=sizeof nv8_fields_25290/sizeof*nv8_fields_25290;return nv8_fields_25290;
case 25445:*count=sizeof nv8_fields_25445/sizeof*nv8_fields_25445;return nv8_fields_25445;
case 25385:*count=sizeof nv8_fields_25385/sizeof*nv8_fields_25385;return nv8_fields_25385;
case 25408:*count=sizeof nv8_fields_25408/sizeof*nv8_fields_25408;return nv8_fields_25408;
default:*count=0;return NULL;}}
