/* Authored metadata; uses shared numicro_field ABI. Vendor payloads BYO. */
static const struct numicro_field sm16_fields_25013[]={
{0,0,1,"Lock Bit 1",{NULL}},
{0,1,1,"Lock Bit 2",{NULL}},
{0,2,1,"Lock Bit 3",{NULL}},
{0,3,1,"Enable 6X Mode",{NULL}},
{2,0,255,"Status Byte",{NULL}},
{2,1,255,"Boot Vector",{NULL}},
};
static const struct numicro_field sm16_fields_25049[]={
{0,0,1,"Lock Bit 1",{NULL}},
{0,1,1,"Lock Bit 2",{NULL}},
{0,2,1,"Lock Bit 3",{NULL}},
{2,0,255,"Status Byte",{NULL}},
{2,1,255,"Boot Vector",{NULL}},
};
static const struct numicro_field sm16_fields_25132[]={
{0,0,1,"Disable Extern-IAP",{NULL}},
{0,1,1,"Disable Extern-Boot",{NULL}},
{0,2,1,"Disable Extern-MOVC",{NULL}},
{0,3,1,"Disable Extern-Host Cmd",{NULL}},
{0,4,1,"Clock Double(6 Clock)",{NULL}},
{0,5,1,"Boot From User Vector",{NULL}},
{0,6,1,"Boot From Zero",{NULL}},
{0,7,1,"Secure Page0",{NULL}},
{0,8,1,"Secure Page1",{NULL}},
{0,9,1,"Secure Page2",{NULL}},
{0,10,1,"Secure Page3",{NULL}},
{0,11,1,"Secure Page4",{NULL}},
{2,0,255,"Boot Vector",{NULL}},
};
static const struct numicro_field*sm16_schema(unsigned id,unsigned*count){switch(id){
case 25013:*count=sizeof sm16_fields_25013/sizeof*sm16_fields_25013;return sm16_fields_25013;
case 25049:*count=sizeof sm16_fields_25049/sizeof*sm16_fields_25049;return sm16_fields_25049;
case 25132:*count=sizeof sm16_fields_25132/sizeof*sm16_fields_25132;return sm16_fields_25132;
default:*count=0;return NULL;}}
