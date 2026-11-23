/* Authored finite SyncMos SM2965/SM59164 source bindings. */
struct sg4_profile{const char*key;unsigned d,m,p,pins;};
static const struct sg4_profile sg4_profiles[]={
{"SyncMos:SM2965",0x66,98,68,40},
{"SyncMos:SM2965 *44",0x67,98,69,44},
{"SyncMos:SM59164",0x66,98,68,40},
{"SyncMos:SM59164 *44",0x67,98,69,44},
};
static const struct sg4_profile*sg4_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2014||s->element_bits!=8||s->capacity_bytes!=65536||strcmp(s->definition_stem,"GADB___"))return NULL;
char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
for(size_t i=0;i<sizeof sg4_profiles/sizeof*sg4_profiles;i++){const struct sg4_profile*p=&sg4_profiles[i];if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins)return p;}return NULL;}
