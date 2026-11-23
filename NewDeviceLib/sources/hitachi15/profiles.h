static const struct gaaf_profile h15_profiles[]={
{13,4,2,8192,{18,4,65535,21,24,65535,65535,65535,65535,65535,26,65535,34,33,65535,65535,65535,65535,65535,65535}},
{14,5,2,16384,{18,4,65535,21,24,65535,65535,65535,65535,65535,26,65535,34,33,65535,65535,65535,65535,65535,65535}},
{15,6,2,32768,{18,4,65535,21,24,65535,65535,65535,65535,65535,26,65535,34,33,65535,65535,65535,65535,65535,65535}},
};
static int h15_binding(const lt48_device_spec*s){
static const struct{const char*key;unsigned d;} rows[]={
{"Hitachi:HD64F3670F @48 FP",13},
{"Hitachi:HD64F3670F @64 FP",13},
{"Hitachi:HD64F3672B @48 FP",14},
{"Hitachi:HD64F3672F @48 FP",14},
{"Hitachi:HD64F3672F @64 FP",14},
{"Renesas(H):HD64F36014B @48 FP",15},
{"Renesas(H):HD64F36014F @48 FP",15},
{"Renesas(H):HD64F36014F @48 QFN",15},
{"Renesas(H):HD64F36014F @64 FP",15},
{"Renesas(H):HD64F36024B @48 FP",15},
{"Renesas(H):HD64F36024F @48 FP",15},
{"Renesas(H):HD64F36024F @64 FP",15},
{"Renesas(H):HD64F3612B @48 FP",14},
{"Renesas(H):HD64F3612F @48 FP",14},
{"Renesas(H):HD64F3612F @64 FP",14},
};char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(rows[i].d==s->definition_resource&&!strcmp(key,rows[i].key))return 1;return 0;}
