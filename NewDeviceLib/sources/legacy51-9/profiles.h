/* Authored exact Dataman legacy OTP identities. Vendor resources remain BYO. */
struct legacy51_9_profile {const char*key;unsigned d,m,p,pins,size,manufacturer;};
static const struct legacy51_9_profile*legacy51_9_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2013||strcmp(s->definition_stem,"GADA___")||s->element_bits!=8||s->secondary_element_count)return NULL;
 static const struct legacy51_9_profile rows[]={
 {"AMIC:AP160L *44",0x6f,58,28,44,8192,55},
 {"Atmel:AT87F51",0x6b,54,26,40,4096,30},
 {"Atmel:AT87F51 *44",0x6c,55,28,44,4096,30},
 {"Atmel:AT87F51RC",0x7f,65,36,40,32768,30},
 {"Atmel:AT87F51RC *44",0x80,66,37,44,32768,30},
 {"Atmel:AT87F52",0x6d,56,27,40,8192,30},
 {"Atmel:AT87F52 *44",0x6e,57,28,44,8192,30},
 {"Atmel:AT87F55WD",0x81,67,36,40,20480,30},
 {"Atmel:AT87F55WD *44",0x82,68,37,44,20480,30},
 };
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct legacy51_9_profile*p=&rows[i];if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->size)return p;}return NULL;
}
