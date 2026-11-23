/* Authored profiles; vendor controller/FPGA payloads remain bring-your-own. */
/* Uses the shared numicro_field ABI. */
static const struct numicro_field nuvoton_fields_25238[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"MOVCL=0",{NULL}},
{0,2,1,"CBS=0",{NULL}},
{0,3,1,"CBORST=0",{NULL}},
{0,4,1,"CBOV0=1",{NULL}},
{0,5,1,"CBOV1=1",{NULL}},
{0,6,1,"CBODEN=0",{NULL}},
{0,7,1,"CFOSC=0",{NULL}},
{0,8,1,"INOSCFS=0",{NULL}},
{0,9,1,"CKF=0",{NULL}},
{0,10,1,"ROG=0",{NULL}},
{0,11,1,"EN6T=0",{NULL}},
{0,12,1,"CWDTEN=0",{NULL}},
};
static const struct numicro_field nuvoton_fields_25239[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"MOVCL=0",{NULL}},
{0,3,1,"CBS=0",{NULL}},
{0,4,1,"CBORST=0",{NULL}},
{0,5,1,"CBOV0=1",{NULL}},
{0,6,1,"CBOV1=1",{NULL}},
{0,7,1,"CBODEN=0",{NULL}},
{0,8,1,"CFOSC=0",{NULL}},
{0,9,1,"INOSCFS=0",{NULL}},
{0,10,1,"CKF=0",{NULL}},
{0,11,1,"ROG=0",{NULL}},
{0,12,1,"EN6T=0",{NULL}},
{0,13,1,"CWDTEN=0",{NULL}},
{2,0,255,"CHBDA",{NULL}},
};
static const struct numicro_field nuvoton_fields_25290[]={
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
static const struct numicro_field nuvoton_fields_25291[]={
{0,0,1,"DFEN=0",{NULL}},
{0,1,1,"Lock",{NULL}},
{0,2,1,"CBS=0",{NULL}},
{0,3,1,"CBORST=0",{NULL}},
{0,4,1,"CBOV=0",{NULL}},
{0,5,1,"CBODEN=0",{NULL}},
{0,6,1,"OSCFS=0",{NULL}},
{0,7,1,"CKF=0",{NULL}},
{0,8,1,"CWDTEN=0",{NULL}},
{1,0,3,"Oscillator Select",{"External crystal,4MHz~24MHz","External crystal,4MHz~24MHz","External oscillator fromXTAL1","Internal RC oscillator"}},
{2,0,255,"CHBDA",{NULL}},
};
static const struct numicro_field*nuvoton_schema(unsigned id,unsigned*count){switch(id){
case 25238:*count=sizeof nuvoton_fields_25238/sizeof*nuvoton_fields_25238;return nuvoton_fields_25238;
case 25239:*count=sizeof nuvoton_fields_25239/sizeof*nuvoton_fields_25239;return nuvoton_fields_25239;
case 25290:*count=sizeof nuvoton_fields_25290/sizeof*nuvoton_fields_25290;return nuvoton_fields_25290;
case 25291:*count=sizeof nuvoton_fields_25291/sizeof*nuvoton_fields_25291;return nuvoton_fields_25291;
default:*count=0;return NULL;}}
