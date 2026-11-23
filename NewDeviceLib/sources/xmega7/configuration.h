static const struct numicro_field x7_fields_25264[]={
{1,0,3,"BLBB (Boot Loader Section)",{"Read and Write Lock","Read Lock","Write Lock","No Lock Bits"}},
{1,1,3,"BLBA (Application Section)",{"Read and Write Lock","Read Lock","Write Lock","No Lock Bits"}},
{1,2,3,"BLBAT(Application Table Section)",{"Read and Write Lock","Read Lock","Write Lock","No Lock Bits"}},
{1,3,2,"Lock Bits Program",{"Read and Write Lock","Write Lock","No Lock Bits"}},
{2,0,255,"Fuse Byte0",{NULL}},
{2,1,255,"Fuse Byte1",{NULL}},
{2,2,255,"Fuse Byte2",{NULL}},
{2,3,255,"Fuse Byte4",{NULL}},
{2,4,255,"Fuse Byte5",{NULL}},
};

static const struct numicro_field x7_fields_25353[]={
{1,0,3,"BLBB (Boot Loader Section)",{"Read and Write Lock","Read Lock","Write Lock","No Lock Bits"}},
{1,1,3,"BLBA (Application Section)",{"Read and Write Lock","Read Lock","Write Lock","No Lock Bits"}},
{1,2,3,"BLBAT(Application Table Section)",{"Read and Write Lock","Read Lock","Write Lock","No Lock Bits"}},
{1,3,2,"Lock Bits Program",{"Read and Write Lock","Write Lock","No Lock Bits"}},
{2,0,255,"Fuse Byte0",{NULL}},
{2,1,255,"Fuse Byte1",{NULL}},
{2,2,255,"Fuse Byte2",{NULL}},
{2,3,255,"Fuse Byte4",{NULL}},
{2,4,255,"Fuse Byte5",{NULL}},
{2,5,255,"Fuse Byte6",{NULL}},
};

static const struct numicro_field*x7_schema(unsigned id,unsigned*count){switch(id){
case 25264:*count=9;return x7_fields_25264;case 25353:*count=10;return x7_fields_25353;default:*count=0;return NULL;}}
