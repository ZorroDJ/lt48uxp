/* Authored schemas. Source applets and bitstreams are not distributed. */
static const struct numicro_field mf74_fields_25365[]={
{0,0,1,"Security Sector Lock",{NULL}},
};
static const struct numicro_field mf74_fields_25364[]={
{0,0,1,"OTP LOCK",{NULL}},
{1,0,3,"Lock Bits Program",{"No Lock Bits","Upper quarter(1/4)","Upper half(1/2)","Protect All"}},
{1,1,1,"Status Register",{"SRWD = 0","SRWD = 1"}},
};
static const struct numicro_field mf74_fields_25230[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"Scramble Enable",{NULL}},
{0,2,1,"MOVC Inhibit",{NULL}},
{0,3,1,"HWBS",{NULL}},
{0,4,1,"HWBS2",{NULL}},
{0,5,1,"ISPAS0",{NULL}},
{0,6,1,"ISPAS1",{NULL}},
{0,7,1,"ISPAS2",{NULL}},
{0,8,1,"6 Clock Enable.",{NULL}},
{0,9,1,"ENRCO",{NULL}},
{0,10,1,"OSCDN",{NULL}},
{0,11,1,"FZWDTCR",{NULL}},
};
static const struct numicro_field mf74_fields_25101[]={
{0,0,1,"Lock",{NULL}},
{0,1,1,"Scramble Enable",{NULL}},
{0,2,1,"MOVC Inhibit",{NULL}},
{0,3,1,"6 Clock Enable.",{NULL}},
{0,4,1,"HWBS",{NULL}},
{0,5,1,"OSCDN",{NULL}},
{0,6,1,"FZWDTCR",{NULL}},
{1,0,3,"ISP Memory size",{"4k","2k","1k","Disable"}},
};
static const struct numicro_field*mf74_schema(unsigned id,unsigned*count){switch(id){
case 25365:*count=1;return mf74_fields_25365;
case 25364:*count=3;return mf74_fields_25364;
case 25230:*count=12;return mf74_fields_25230;
case 25101:*count=8;return mf74_fields_25101;
default:*count=0;return NULL;}}
