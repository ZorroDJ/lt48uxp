/* Finite authored Dataman13.10 UTRON word mask-ROM bindings. */
static const struct gaaf_profile utron4_profiles[]={
{52,36,21,1048576,{11,17,33,70,44,65535,65535,65535,65535,65535,65535,65535,65535,71,59,60,65535,62,65535,65535,65535}},
{53,36,20,2097152,{11,17,33,70,44,65535,65535,65535,65535,65535,65535,65535,65535,71,59,60,65535,62,65535,65535,65535}},
{54,37,22,4194304,{11,17,33,70,44,65535,65535,65535,65535,65535,65535,65535,65535,72,59,60,65535,62,65535,65535,65535}},
{55,37,23,8388608,{11,17,33,70,44,65535,65535,65535,65535,65535,65535,65535,65535,72,59,60,65535,62,65535,65535,65535}},
};
static int utron4_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2003||strcmp(s->catalogue.vendor,"UTRON"))return 0;
static const struct{unsigned d;const char*name;}rows[]={
{53,"UT23C16100"},
{54,"UT23C32100"},
{55,"UT23C64100"},
{52,"UT23C8100"},
};
for(size_t i=0;i<sizeof rows/sizeof*rows;i++){if(rows[i].d==s->definition_resource&&strlen(rows[i].name)==s->catalogue.device_name_length&&!memcmp(rows[i].name,s->catalogue.device_name,s->catalogue.device_name_length))return 1;}return 0;}
