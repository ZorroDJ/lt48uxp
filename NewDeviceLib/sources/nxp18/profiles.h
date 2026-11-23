/* Original LPC111x/11C workflows; vendor resources remain BYO. */
static const uint32_t nxp18_sectors[]={0};
struct nxp18_profile{struct xp_closure base;struct xp_shared_word_shape shape;unsigned variables;};
static const struct nxp18_profile nxp18_profiles[]={
{ {354,227,145,32,8192,330,nxp18_sectors,1}, {354,14,7,0},2},
{ {355,228,145,32,16384,330,nxp18_sectors,1}, {355,14,7,0},2},
{ {356,229,145,32,24576,330,nxp18_sectors,1}, {356,14,7,0},2},
{ {357,230,145,32,32768,330,nxp18_sectors,1}, {357,14,7,0},2},
{ {358,231,146,48,16384,330,nxp18_sectors,1}, {358,14,7,0},2},
{ {359,232,146,48,32768,330,nxp18_sectors,1}, {359,14,7,0},2},
};
static const struct nxp18_profile*nxp18_find(unsigned cls,unsigned d){if(cls!=2038)return NULL;for(size_t i=0;i<sizeof nxp18_profiles/sizeof*nxp18_profiles;i++)if(nxp18_profiles[i].base.definition==d)return nxp18_profiles+i;return NULL;}
static int nxp18_binding(const lt48_device_spec*s){static const struct{const char*key;unsigned definition,manufacturer,device;}rows[]={
{"NXP:LPC1111/101 @32QFN",354,1054,20523},
{"NXP:LPC1111/102 @32QFN",354,9494,36907},
{"NXP:LPC1111/201 @32QFN",354,1046,20523},
{"NXP:LPC1111/202 @32QFN",354,9494,53291},
{"NXP:LPC1112/101 @32QFN",355,1069,20523},
{"NXP:LPC1112/102 @32QFN",355,9508,53291},
{"NXP:LPC1112/201 @32QFN",355,1061,20523},
{"NXP:LPC1112/202 @32QFN",355,9508,36907},
{"NXP:LPC1113/201 @32QFN",356,1076,20523},
{"NXP:LPC1113/202 @32QFN",356,9522,36907},
{"NXP:LPC1113/301 @32QFN",356,1076,4139},
{"NXP:LPC1113/302 @32QFN",356,9522,4139},
{"NXP:LPC1114/201 @32QFN",357,1092,20523},
{"NXP:LPC1114/202 @32QFN",357,9536,36907},
{"NXP:LPC1114/301 @32QFN",357,1092,4139},
{"NXP:LPC1114/302 @32QFN",357,9536,4139},
{"NXP:LPC11C12/301 @48LQ",358,5153,4139},
{"NXP:LPC11C14/301 @48LQ",359,5184,4139},
};char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].definition&&s->catalogue.manufacturer_id==rows[i].manufacturer&&s->catalogue.device_id==rows[i].device)return 1;return 0;}
