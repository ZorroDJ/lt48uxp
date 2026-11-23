/* Authored exact Dataman 13.10 bindings. Original procedures remain BYO. */
#define LT48_HYUNDAI24_STAGE 2
struct hy24_profile {const char*key;unsigned d,m,p,pins,size,secondary,stage,levels,operations,lock_field;};
static const struct hy24_profile hy24_profiles[]={
 {"Hyundai(Hynix):GMS80C701",0x52,11,0,40,8192,32,1,2,9,1},
 {"Hyundai(Hynix):GMS80C701 *44",0x54,11,1,44,8192,32,1,2,9,1},
 {"Hyundai(Hynix):GMS97C/L51",0x51,10,0,40,4096,32,1,2,9,1},
 {"Hyundai(Hynix):GMS97C/L51 *44",0x53,10,1,44,4096,32,1,2,9,1},
 {"Hyundai(Hynix):GMS97C/L54",0xe0,116,47,40,16384,64,2,2,9,4},
 {"Hyundai(Hynix):GMS97C/L54 *44",0xe1,116,48,44,16384,64,2,2,9,4},
 {"Hyundai(Hynix):GMS97C/L56",0xe2,116,47,40,24576,64,2,2,9,4},
 {"Hyundai(Hynix):GMS97C/L56 *44",0xe3,116,48,44,24576,64,2,2,9,4},
 {"Hyundai(Hynix):GMS97C/L58",0xe4,116,47,40,32768,64,2,2,9,4},
 {"Hyundai(Hynix):GMS97C/L58 *44",0xe5,116,48,44,32768,64,2,2,9,4},
 {"Hyundai(Hynix):GMS97C1051",0xde,114,46,20,1024,32,2,2,9,1},
 {"Hyundai(Hynix):GMS97C2051",0xdf,115,46,20,2048,32,2,2,9,1},
 {"Hyundai(Hynix):GMS97C52",0x52,11,0,40,8192,32,2,2,9,1},
 {"Hyundai(Hynix):GMS97C52 *44",0x54,11,1,44,8192,32,2,2,9,1},
 {"Hyundai(Hynix):GMS97C8032",0xe8,118,49,40,32768,64,2,2,9,1},
 {"Hyundai(Hynix):GMS97L1051",0xde,114,46,20,1024,32,2,2,9,1},
 {"Hyundai(Hynix):GMS97L2051",0xdf,115,46,20,2048,32,2,2,9,1},
 {"Hyundai(Hynix):GMS99C58",0xe6,117,47,40,32768,64,2,2,10,4},
 {"Hyundai(Hynix):GMS99C58 *44",0xe7,117,48,44,32768,64,2,2,10,4},
 {"Versachips:V87C54",0x9d,69,17,40,16384,0,2,1,8,8},
 {"Versachips:V87C54 *44",0x9e,69,18,44,16384,0,2,1,8,8},
 {"Versachips:V87C58",0x9f,69,17,40,32768,0,2,1,8,8},
 {"Versachips:V87C58 *44",0xa0,69,18,44,32768,0,2,1,8,8},
 {"Weltrend:STSL82933",0xe9,119,50,40,4096,0,2,1,9,1},
};
static const struct hy24_profile*hy24_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2018||strcmp(s->definition_stem,"GAF____")||s->element_bits!=8||s->secondary_element_bits!=8)return NULL;
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof hy24_profiles/sizeof *hy24_profiles;i++){const struct hy24_profile*p=&hy24_profiles[i];
 if(p->stage<=LT48_HYUNDAI24_STAGE&&!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->size&&s->secondary_element_count==p->secondary)return p;}
 return NULL;
}
