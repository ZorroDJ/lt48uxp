/* Authored exact Dataman 13.10 TEMIC bindings; resources remain BYO. */
static int temic26_binding(const lt48_device_spec*s){
 static const struct{const char*key;unsigned d,p,size;}rows[]={
 {"TEMIC:TS87C51RB2",0x97,17,16384},
 {"TEMIC:TS87C51RB2 *44",0x98,18,16384},
 {"TEMIC:TS87C51RC2",0x99,17,32768},
 {"TEMIC:TS87C51RC2 *44",0x9a,18,32768},
 {"TEMIC:TS87C51RD2",0x9b,17,65536},
 {"TEMIC:TS87C51RD2 *44",0x9c,18,65536},
 {"TEMIC:TS87C51U2",0x97,17,16384},
 {"TEMIC:TS87C51U2 *44",0x98,18,16384},
 {"TEMIC:TS87C52X2",0x95,17,8192},
 {"TEMIC:TS87C52X2 *44",0x96,18,8192},
 {"TEMIC:TS87C54X2",0x97,17,16384},
 {"TEMIC:TS87C54X2 *44",0x98,18,16384},
 {"TEMIC:TS87C58X2",0x99,17,32768},
 {"TEMIC:TS87C58X2 *44",0x9a,18,32768},
 {"TEMIC:TSC71RAX2",0x95,17,8192},
 {"TEMIC:TSC71RAX2 *44",0x96,18,8192},
 {"TEMIC:TSC71RBX2",0x97,17,16384},
 {"TEMIC:TSC71RBX2 *44",0x98,18,16384},
 {"TEMIC:TSC71RCX2",0x99,17,32768},
 {"TEMIC:TSC71RCX2 *44",0x9a,18,32768},
 {"TEMIC:TSC71RDX2",0x9b,17,65536},
 {"TEMIC:TSC71RDX2 *44",0x9c,18,65536},
 {"TEMIC:TSC87C54",0x97,17,16384},
 {"TEMIC:TSC87C54 *44",0x98,18,16384},
 {"TEMIC:TSC87C58",0x99,17,32768},
 {"TEMIC:TSC87C58 *44",0x9a,18,32768},
 };
 if(s->catalogue.device_class!=2018 || strcmp(s->definition_stem,"GAF____") || s->family_code!=68 || s->secondary_element_count!=64 || s->element_bits!=8 || s->secondary_element_bits!=8)return 0;
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->pinmap==rows[i].p&&s->capacity_bytes==rows[i].size&&s->pin_count==(rows[i].p==17?40u:44u))return 1;
 return 0;
}
