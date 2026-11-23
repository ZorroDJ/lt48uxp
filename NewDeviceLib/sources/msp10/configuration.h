/* Authored profiles; vendor controller/FPGA payloads remain bring-your-own. */
/* Shared field type declared by existing MSP430 profiles. */
static const struct msp430_field msp10_fields_25047[]={
{0,0,1,"Protect",{NULL}},
};
static const struct msp430_field*msp10_schema(unsigned id,unsigned*count){switch(id){
case 25047:*count=sizeof msp10_fields_25047/sizeof*msp10_fields_25047;return msp10_fields_25047;
default:*count=0;return NULL;}}
