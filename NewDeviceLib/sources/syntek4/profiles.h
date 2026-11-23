/* Authored finite source-bound Syntek compact profiles. */
struct sy4_profile{const char*key;unsigned d,m,p,pins,capacity;};
static const struct sy4_profile sy4_profiles[]={
{"Syntek:STK99110N18",0xa2,81,89,18,2048},
{"Syntek:STK99110N20",0xa3,82,90,20,2048},
{"Syntek:STK97DV03A ",0xa3,82,90,20,2048},
{"Syntek:STK99120N28",0xa4,81,91,28,4096},
};
static const struct sy4_profile*sy4_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2012||s->element_bits!=14||!s->definition_stem||strcmp(s->definition_stem,"GAD____"))return NULL;
char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
for(size_t i=0;i<sizeof sy4_profiles/sizeof*sy4_profiles;i++){const struct sy4_profile*p=&sy4_profiles[i];if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity)return p;}return NULL;}
