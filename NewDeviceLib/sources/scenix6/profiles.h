/* Authored exact SX profiles; original assets are supplied by the user. */
struct sx6_profile{const char*key;unsigned d,m,p,pins;};
static const struct sx6_profile sx6_profiles[]={
{"SCENIX:SX18AC/DP",0x11e,135,80,18},
{"SCENIX:SX18AC/SO",0x11e,135,80,18},
{"SCENIX:SX20AC/SS",0x11f,136,81,20},
{"SCENIX:SX28AC/DP",0x120,137,82,28},
{"SCENIX:SX28AC/SO",0x120,137,82,28},
{"SCENIX:SX28AC/SS",0x121,138,83,28},
};
static const struct sx6_profile*sx6_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2000||s->element_bits!=12||s->capacity_bytes!=4128||!s->definition_stem||strcmp(s->definition_stem,"GA_____"))return NULL;
char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
for(size_t i=0;i<sizeof sx6_profiles/sizeof*sx6_profiles;i++){const struct sx6_profile*p=&sx6_profiles[i];if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins)return p;}return NULL;}
