/* Authored exact OTP MCU GAF bindings. Original assets remain BYO. */
static unsigned otp20_binding(const lt48_device_spec*s){
 static const struct{const char*key;unsigned d,m,p,pins,size,secondary,kind;}rows[]={
 {"Philips:87C51FA",0x35,15,0,40,8192,32,0},
 {"Philips:87C51FA *44",0x45,15,1,44,8192,32,0},
 {"Philips:87C51FB",0x36,15,0,40,16384,32,0},
 {"Philips:87C51FB *44",0x46,15,1,44,16384,32,0},
 {"Philips:87C51FC",0x37,15,0,40,32768,64,0},
 {"Philips:87C51FC *44",0x47,15,1,44,32768,64,0},
 {"Philips:87C51RD+",0x5d,24,10,40,65536,64,1},
 {"Philips:87C51RD+ *44",0x5e,24,11,44,65536,64,1},
 {"Philips:87C550 *44",0x49,19,8,44,4096,32,0},
 {"Philips:87C592 @68",0x50,17,9,40,16384,32,0},
 {"Philips:87C654X2 *44",0x2e,14,1,44,16384,32,5},
 {"Philips:P87C660X2 *44",0x6f,33,11,44,16384,64,4},
 {"Philips:P87C660X2 *44TQ",0x6f,33,11,44,16384,64,4},
 {"Philips:P87C661X2 *44",0x6f,33,11,44,16384,64,4},
 {"Philips:P87C661X2 *44TQ",0x6f,33,11,44,16384,64,4},
 {"TEMIC:TSC87C51",0xa3,72,19,40,4096,64,0},
 {"TEMIC:TSC87C51 *44",0xa5,72,20,44,4096,64,0},
 {"TEMIC:TSC87C52",0xa4,72,19,40,8192,64,0},
 {"TEMIC:TSC87C52 *44",0xa6,72,20,44,8192,64,0},
 {"Tekmos:TK87C51RD2 *44",0x5e,24,11,44,65536,64,1},
 };
 if(s->catalogue.device_class!=2018 || strcmp(s->definition_stem,"GAF____") || s->element_bits!=8 || s->secondary_element_bits!=8)return 0;
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins&&s->capacity_bytes==rows[i].size&&s->secondary_element_count==rows[i].secondary)return rows[i].kind+1;
 return 0;
}
