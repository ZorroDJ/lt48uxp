/* Authored exact ELAN compact MCU source bindings. */
struct el5_profile{const char*key;unsigned d,m,p,pins,capacity,cfg_count;unsigned fields[2];};
static const struct el5_profile el5_profiles[]={
{"ELAN:EM78P156E",0x95,81,46,18,2048,2,{4,6}},
{"ELAN:EM78P447SA",0x96,82,48,28,8192,1,{4}},
{"ELAN:EM78P447SB",0x97,82,49,32,8192,1,{4}},
{"ELAN:EM78P451",0x98,83,50,40,8192,1,{4}},
{"ELAN:EM78P458",0x99,84,47,20,8192,2,{4,5}},
};
static const struct el5_profile*el5_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2013||s->element_bits!=13||!s->definition_stem||strcmp(s->definition_stem,"GADA___"))return NULL;
char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
for(size_t i=0;i<sizeof el5_profiles/sizeof*el5_profiles;i++){const struct el5_profile*p=&el5_profiles[i];if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity)return p;}return NULL;}
