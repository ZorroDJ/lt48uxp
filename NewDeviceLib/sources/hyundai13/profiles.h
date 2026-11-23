/* Authored exact profiles; original procedures remain BYO. */
#define LT48_HYUNDAI13_STAGE 2
struct hy13_profile{const char*key;unsigned d,m,p,pins,capacity,stage,start,blank;};
static const struct hy13_profile hy13_profiles[]={
{"Hyundai(Hynix):GMS81504T",0x169,167,109,30,65536,2,0,255},
{"Hyundai(Hynix):GMS81608T",0x168,166,108,42,65536,2,0,255},
{"Hyundai(Hynix):GMS87C1102",0x170,170,113,16,65536,2,63488,0},
{"Hyundai(Hynix):GMS87C1202",0x171,170,114,20,65536,2,63488,0},
{"Hyundai(Hynix):GMS87C1408",0x172,171,115,28,65536,2,57344,0},
{"Hyundai(Hynix):GMS87C2020",0x179,175,122,40,20480,2,45056,0},
{"Hyundai(Hynix):HMS87C1102A",0x16a,168,110,16,65536,1,63488,0},
{"Hyundai(Hynix):HMS87C1104A",0x16b,169,110,16,65536,1,61440,0},
{"Hyundai(Hynix):HMS87C1202A",0x16c,168,111,20,65536,1,63488,0},
{"Hyundai(Hynix):HMS87C1204A",0x16d,169,111,20,65536,1,61440,0},
{"Hyundai(Hynix):HMS87C1302A",0x16e,168,112,24,65536,1,63488,0},
{"Hyundai(Hynix):HMS87C1304A",0x16f,169,112,24,65536,1,61440,0},
{"Hyundai(Hynix):HMS87C5216",0x178,174,121,40,16384,2,49152,0},
};
static const struct hy13_profile*hy13_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2000||s->element_bits!=8||!s->definition_stem||strcmp(s->definition_stem,"GA_____"))return NULL;
char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
for(size_t i=0;i<sizeof hy13_profiles/sizeof*hy13_profiles;i++){const struct hy13_profile*p=&hy13_profiles[i];if(p->stage<=LT48_HYUNDAI13_STAGE&&!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->capacity)return p;}return NULL;}
