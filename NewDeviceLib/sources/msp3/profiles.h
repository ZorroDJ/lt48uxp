/* Reuse exact one-field configuration25047 from msp10. */
static const struct msp430_profile msp3_profiles[]={
{365,266,130,16640,1,25047,2,{256,16384},{4096,49152},{4096,49152},5,msp10_fields_25047},
{366,267,130,24832,1,25047,2,{256,24576},{4096,40960},{4096,40960},5,msp10_fields_25047},
{367,268,130,33024,1,25047,2,{256,32768},{4096,32768},{4096,32768},5,msp10_fields_25047},
};
static const struct {unsigned definition,manufacturer,device;const char*vendor,*name;} msp3_members[]={
{365,0,0,"TI","MSP430F4250 *48SSOP"},
{366,0,0,"TI","MSP430F4260 *48SSOP"},
{367,0,0,"TI","MSP430F4270 *48SSOP"},
};
