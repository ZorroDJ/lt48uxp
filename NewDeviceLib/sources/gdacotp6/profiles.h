/* Authored finite profiles; original procedure assets remain BYO. */
#include "admission.h"
struct gdacotp6_profile{unsigned d,m,p,pins,variables,nominal,capacity;};
static const struct gdacotp6_profile gdacotp6_profiles[]={
{7,5,7,28,4,330,65536},
{5,4,5,48,7,330,524288},
{6,4,6,48,7,330,1048576},
{8,6,8,42,6,500,16384},
 };
static const struct{unsigned d;const char*vendor,*name;}gdacotp6_names[]={
{7,"EMTC","EM27SL512AU "},
{7,"EMTC","EM27SL512AU *28TS"},
{7,"EMTC","EM27SL512AU *32PLCC"},
{5,"OKI","MR27V402F *48TS"},
{6,"OKI","MR27V802F *48TS"},
{8,"Toshiba","TMP87PH46 *42SDIP"},
 };
static inline int gdacotp6_catalogue_member(const lt48_catalogue_entry*e){
 if(!GDACOTP6_ADMITTED||!e||e->device_class!=2034)return 0;
 for(unsigned i=0;i<sizeof gdacotp6_names/sizeof*gdacotp6_names;i++)if(e->device_type==gdacotp6_names[i].d&&!strcmp(e->vendor,gdacotp6_names[i].vendor)&&e->device_name_length==strlen(gdacotp6_names[i].name)&&!memcmp(e->device_name,gdacotp6_names[i].name,e->device_name_length))return 1;
 return 0;
}
static inline const struct gdacotp6_profile*gdacotp6_binding(const lt48_device_spec*s){
 if(!s||!gdacotp6_catalogue_member(&s->catalogue)||s->definition_resource!=s->catalogue.device_type||s->element_bits!=8||s->secondary_element_count)return NULL;
 for(unsigned i=0;i<sizeof gdacotp6_profiles/sizeof*gdacotp6_profiles;i++){const struct gdacotp6_profile*p=gdacotp6_profiles+i;if(s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity)return p;}
 return NULL;
}
