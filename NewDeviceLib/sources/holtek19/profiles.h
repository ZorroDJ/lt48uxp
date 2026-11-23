/* Authored exact-source Holtek OTP bindings; original resources are BYO. */
struct holtek19_profile{const char*key;unsigned cls,d,m,p,pins,capacity,bits,blank,stage;};
static const struct holtek19_profile*holtek19_binding(const lt48_device_spec*s){
 static const struct holtek19_profile rows[]={
 {"HOLTEK:HT48R10-B *18DIP",2012,0x9a,79,84,18,2048,14,0,1},
 {"HOLTEK:HT48R10-B *20DIP",2012,0x97,78,82,20,2048,14,0,1},
 {"HOLTEK:HT48R11-B *24DIP",2012,0x9b,78,85,24,2048,14,0,1},
 {"HOLTEK:HT48R11-F *18DIP",2012,0x99,79,83,18,2048,14,0,1},
 {"HOLTEK:HT48R11-F *20DIP",2012,0x98,78,87,20,2048,14,0,1},
 {"HOLTEK:HT48R12-B *18DIP",2012,0x9a,79,84,18,2048,14,0,1},
 {"HOLTEK:HT48R12-B *20DIP",2012,0x97,78,82,20,2048,14,0,1},
 {"HOLTEK:HT48R12-B *20SOIC",2012,0x97,78,82,20,2048,14,0,1},
 {"HOLTEK:HT48R12-B *24DIP",2012,0x9b,78,85,24,2048,14,0,1},
 {"HOLTEK:HT48R31-B *18DIP",2012,0x9d,79,83,18,4096,14,0,1},
 {"HOLTEK:HT48R31-B *20DIP",2012,0x9e,78,87,20,4096,14,0,1},
 {"HOLTEK:HT48R31-B *20SOIC",2012,0x9f,78,87,20,4096,14,0,1},
 {"HOLTEK:HT48R31-B *28DIP",2012,0x9c,78,86,28,4096,14,0,1},
 {"HOLTEK:HT48R32-B *18DIP",2012,0x9d,79,83,18,4096,14,0,1},
 {"HOLTEK:HT48R32-B *20DIP",2012,0x9e,78,87,20,4096,14,0,1},
 {"HOLTEK:HT48R32-B *28DIP",2012,0x9c,78,86,28,4096,14,0,1},
 {"HOLTEK:HT48R32-F *20SOIC",2012,0x9f,78,87,20,4096,14,0,1},
 {"HOLTEK:HT48R50-B *28DIP",2012,0xa0,80,86,28,8192,15,0,2},
 {"HOLTEK:HT48R50-B *40DIP",2012,0xa1,80,88,40,8192,15,0,2},
 };
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct holtek19_profile*b=&rows[i];
  if(!strcmp(key,b->key)&&s->catalogue.device_class==b->cls&&s->definition_resource==b->d&&s->family_code==b->m&&s->pinmap==b->p&&s->pin_count==b->pins&&s->capacity_bytes==b->capacity&&s->element_bits==b->bits)return b;
 }return NULL;
}
