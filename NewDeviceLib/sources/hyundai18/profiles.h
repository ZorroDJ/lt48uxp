/* Authored exact source profiles; vendor assets remain BYO. */
#define LT48_HYUNDAI18_STAGE 2
struct hy18_profile{const char*key;unsigned d,m,p,pins,capacity,stage;};
static const struct hy18_profile hy18_profiles[]={
{"Hyundai(Hynix):GMS34004TK",0x44,55,4,16,512,1},
{"Hyundai(Hynix):GMS34004TM *16SO",0x41,54,4,16,512,1},
{"Hyundai(Hynix):GMS34112TK",0x45,55,5,20,1024,1},
{"Hyundai(Hynix):GMS34112TM *20SO",0x42,54,5,20,1024,1},
{"Hyundai(Hynix):GMS34140TK",0x46,55,6,24,1024,1},
{"Hyundai(Hynix):GMS34140TM *24SO",0x43,54,6,24,1024,1},
{"Hyundai(Hynix):GMS36004TK *16SO",0x4a,57,7,16,512,2},
{"Hyundai(Hynix):GMS36004TM *16SO",0x47,56,7,16,512,2},
{"Hyundai(Hynix):GMS36112TK *20SO",0x4b,57,8,20,1024,2},
{"Hyundai(Hynix):GMS36112TM *20SO",0x48,56,8,20,1024,2},
{"Hyundai(Hynix):GMS36140TK *24SO",0x4c,57,9,24,1024,2},
{"Hyundai(Hynix):GMS36140TM *24SO",0x49,56,9,24,1024,2},
{"Hyundai(Hynix):GMS37004TK *16SO",0x4a,57,7,16,512,2},
{"Hyundai(Hynix):GMS37004TM *16SO",0x47,56,7,16,512,2},
{"Hyundai(Hynix):GMS37112TK *20SO",0x4b,57,8,20,1024,2},
{"Hyundai(Hynix):GMS37112TM *20SO",0x48,56,8,20,1024,2},
{"Hyundai(Hynix):GMS37140TK *24SO",0x4c,57,9,24,1024,2},
{"Hyundai(Hynix):GMS37140TM *24SO",0x49,56,9,24,1024,2},
};
static const struct hy18_profile*hy18_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2010||s->element_bits!=8||!s->definition_stem||strcmp(s->definition_stem,"GAB____"))return NULL;
char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
for(size_t i=0;i<sizeof hy18_profiles/sizeof*hy18_profiles;i++){const struct hy18_profile*p=&hy18_profiles[i];if(p->stage<=LT48_HYUNDAI18_STAGE&&!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity)return p;}return NULL;}
