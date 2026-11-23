/* Authored profiles; vendor controller/FPGA payloads remain bring-your-own. */
/* Uses the shared numicro_field ABI. */
static const struct numicro_field nw21_fields_25209[]={
{0,0,1,"OSC0=1",{NULL}},
{0,1,1,"OSC1=1",{NULL}},
{0,2,1,"BPFR=1",{NULL}},
{0,3,1,"CBOD=1",{NULL}},
{0,4,1,"PRHI=1",{NULL}},
{0,5,1,"PMODE=1",{NULL}},
{0,6,1,"WDTCK=1",{NULL}},
{0,7,1,"MOVC Inhibit",{NULL}},
{0,8,1,"CBOV0=1",{NULL}},
{0,9,1,"CBOV1=1",{NULL}},
{0,10,1,"FS1=1",{NULL}},
{0,11,1,"EEPROM Protect",{NULL}},
{0,12,1,"Lock Bit Active",{NULL}},
};
static const struct numicro_field nw21_fields_25211[]={
{0,0,1,"OSC0=1",{NULL}},
{0,1,1,"OSC1=1",{NULL}},
{0,2,1,"BPFR=1",{NULL}},
{0,3,1,"CBOV0=1",{NULL}},
{0,4,1,"CBOV1=1",{NULL}},
{0,5,1,"PRHI=1",{NULL}},
{0,6,1,"PRD=1",{NULL}},
{0,7,1,"WDTCK=1",{NULL}},
{0,8,1,"PWM_Odd=1",{NULL}},
{0,9,1,"PWM_Even=1",{NULL}},
{0,10,1,"SCF=1",{NULL}},
{0,11,1,"FS1=1",{NULL}},
{0,12,1,"EEPROM Protect",{NULL}},
{0,13,1,"Lock Bit Active",{NULL}},
};
static const struct numicro_field nw21_fields_25195[]={
{0,0,1,"OSC0=1",{NULL}},
{0,1,1,"OSC1=1",{NULL}},
{0,2,1,"BPFR=1",{NULL}},
{0,3,1,"CBOD=1",{NULL}},
{0,4,1,"BOV=1",{NULL}},
{0,5,1,"PRHI=1",{NULL}},
{0,6,1,"PRD=1",{NULL}},
{0,7,1,"WDTCK=1",{NULL}},
{0,8,1,"NVMS0=1",{NULL}},
{0,9,1,"NVMS1=1",{NULL}},
{0,10,1,"HS1=1",{NULL}},
{0,11,1,"FS1=1",{NULL}},
{0,12,1,"EEPROM Protect",{NULL}},
{0,13,1,"Lock Bit Active",{NULL}},
};
static const struct numicro_field nw21_fields_25077[]={
{0,0,1,"Lock Bit Active",{NULL}},
{0,1,1,"MOVC Inhibit",{NULL}},
{0,2,1,"Encryption Enable",{NULL}},
{0,3,1,"P4 Reboot Enable",{NULL}},
};
static const struct numicro_field nw21_fields_25194[]={
{0,0,1,"OSC0=1",{NULL}},
{0,1,1,"OSC1=1",{NULL}},
{0,2,1,"BPFR=1",{NULL}},
{0,3,1,"CBOV0=1",{NULL}},
{0,4,1,"CBOV1=1",{NULL}},
{0,5,1,"WDTCK=1",{NULL}},
{0,6,1,"FS1=1",{NULL}},
{0,7,1,"EEPROM Protect",{NULL}},
{0,8,1,"Lock Bit Active",{NULL}},
};

#include "../n76one/configuration.h"
static const struct numicro_field*nw21_schema(unsigned id,unsigned*count){switch(id){
case 25399:*count=11;return n76one_fields;
case 25209:*count=sizeof nw21_fields_25209/sizeof*nw21_fields_25209;return nw21_fields_25209;
case 25211:*count=sizeof nw21_fields_25211/sizeof*nw21_fields_25211;return nw21_fields_25211;
case 25195:*count=sizeof nw21_fields_25195/sizeof*nw21_fields_25195;return nw21_fields_25195;
case 25077:*count=sizeof nw21_fields_25077/sizeof*nw21_fields_25077;return nw21_fields_25077;
case 25194:*count=sizeof nw21_fields_25194/sizeof*nw21_fields_25194;return nw21_fields_25194;
default:*count=0;return NULL;}}
