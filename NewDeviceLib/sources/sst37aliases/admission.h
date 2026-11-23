#ifndef LT48_SST37ALIASES_POLICY_H
#define LT48_SST37ALIASES_POLICY_H
#define LT48_SST37ALIASES_ADMITTED 1
static int sst37aliases_binding(const lt48_catalogue_entry*e){
static const struct{const char*key;unsigned d;} rows[]={
{"Greenliant:GLS37VF010",184},
{"Greenliant:GLS37VF010 *32PLCC",184},
{"Greenliant:GLS37VF010 *32TS",184},
{"Greenliant:GLS37VF020",185},
{"Greenliant:GLS37VF020 *32PLCC",185},
{"Greenliant:GLS37VF020 *32TS",185},
{"Greenliant:GLS37VF040",186},
{"Greenliant:GLS37VF040 *32PLCC",186},
{"Greenliant:GLS37VF040 *32TS",186},
{"PMC (Chingis):Pm37LV512",182},
};char key[256];if(!e||e->device_class!=2007)return 0;snprintf(key,sizeof key,"%s:%.*s",e->vendor,(int)e->device_name_length,e->device_name);for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(rows[i].d==e->device_type&&!strcmp(rows[i].key,key))return 1;return 0;}
#endif
