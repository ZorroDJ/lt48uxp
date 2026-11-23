/* Authored bindings; original callbacks/FPGA payloads remain BYO. */
struct lm20_profile{const char*key;unsigned d,m,p,pins,capacity,operations,group,secure,erase,configuration;};
#include "../hms99one/profiles.h"
static const struct lm20_profile*lm20_binding(const lt48_device_spec*s){const struct lm20_profile*extra=hms99one_binding(s);if(extra)return extra;if(s->catalogue.device_class!=2013||strcmp(s->definition_stem,"GADA___"))return NULL;static const struct lm20_profile rows[]={
{"ISSI:IS89C(E)54",131,69,38,40,16384,10,1,3,1,1},
{"ISSI:IS89C(E)54 *44",132,70,39,44,16384,10,1,3,1,1},
{"ISSI:IS89C(E)58",133,69,38,40,32768,10,1,3,1,1},
{"ISSI:IS89C(E)58 *44",134,70,39,44,32768,10,1,3,1,1},
{"ISSI:IS89C(E)64",135,71,38,40,65536,10,1,3,1,1},
{"ISSI:IS89C(E)64 *44",136,72,39,44,65536,10,1,3,1,1},
{"NOVATEK:NT68F63",157,88,53,42,61440,10,6,1,1,1},
{"NOVATEK:NT68F65",156,87,53,42,32768,10,6,1,1,1},
{"Philips:87C055 *42 SDIP",162,92,57,42,53248,9,3,0,0,0},
{"Philips:87C51MA2 *44",190,111,76,44,32768,9,4,3,0,1},
{"Philips:87C51MB2 *44",189,111,76,44,65536,9,4,3,0,1},
{"Philips:87C51MC2 *44",188,111,76,44,98304,9,4,3,0,1},
{"Philips:87C748",160,90,55,24,2048,8,2,1,0,0},
{"Philips:87C749",161,91,56,28,2048,8,2,1,0,0},
{"Philips:87C750",159,90,55,24,1024,8,2,1,0,0},
{"Philips:87C751",160,90,55,24,2048,8,2,1,0,0},
{"Philips:87C752",161,91,56,28,2048,8,2,1,0,0},
{"Philips:PXAC37K *44",140,76,42,44,32768,9,5,3,0,1},
{"Philips:PXAG37K *44",140,76,42,44,32768,9,5,3,0,1},
{"Winbond:W78E354 @68",90,38,20,40,16384,10,7,0,1,1},
};char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);for(size_t i=0;i<sizeof rows/sizeof*rows;i++){const struct lm20_profile*p=rows+i;if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity&&s->element_bits==8&&!s->secondary_element_count)return p;}return NULL;}
