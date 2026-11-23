/* Authored profiles; vendor controller/FPGA payloads remain bring-your-own. */
struct msp430_field {unsigned category,field,maximum;const char*label;const char*options[4];};
static const struct msp430_field msp430_fields_25313[]={
{0,0,1,"Protect",{NULL}},
{1,0,1,"Overwrite Calibration Data(SegmentA)",{"NO.data will be preserved","Yes.data will be erased,prog"}},
};
static const struct msp430_field*msp430_schema(unsigned id,unsigned*count){switch(id){
case 25313:*count=sizeof msp430_fields_25313/sizeof*msp430_fields_25313;return msp430_fields_25313;
default:*count=0;return NULL;}}
