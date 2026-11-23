/* Authored exact Philips/Tekmos GAF bindings. Original assets remain BYO. */
static unsigned philips39_binding(const lt48_device_spec*s){
 static const struct{const char*key;unsigned d,m,p,pins,size,secondary,kind;}rows[]={
 {"Philips:P87C51X2",0x5f,25,10,40,4096,64,3},
 {"Philips:P87C51X2 *44",0x60,25,11,44,4096,64,3},
 {"Philips:P87C51X2 *44TQ",0x60,25,11,44,4096,64,3},
 {"Philips:P87C52X2",0x61,26,10,40,8192,64,3},
 {"Philips:P87C52X2 *44",0x62,26,11,44,8192,64,3},
 {"Philips:P87C52X2 *44TQ",0x62,26,11,44,8192,64,3},
 {"Philips:P87C54X2",0x63,27,10,40,16384,64,3},
 {"Philips:P87C54X2 *44",0x64,27,11,44,16384,64,3},
 {"Philips:P87C54X2 *44TQ",0x64,27,11,44,16384,64,3},
 {"Philips:P87C58X2",0x65,28,10,40,32768,64,3},
 {"Philips:P87C58X2 *44",0x66,28,11,44,32768,64,3},
 {"Philips:P87C58X2 *44TQ",0x66,28,11,44,32768,64,3},
 {"Philips:P87CL52X2",0x61,26,10,40,8192,64,3},
 {"Philips:P87CL52X2 *44",0x62,26,11,44,8192,64,3},
 {"Philips:P87CL52X2 *44TQ",0x62,26,11,44,8192,64,3},
 {"Philips:P87CL54X2",0x63,27,10,40,16384,64,3},
 {"Philips:P87CL54X2 *44",0x64,27,11,44,16384,64,3},
 {"Philips:P87CL54X2 *44TQ",0x64,27,11,44,16384,64,3},
 {"Philips:P87C51RA2",0x67,29,10,40,8192,64,2},
 {"Philips:P87C51RA2 *44",0x68,29,11,44,8192,64,2},
 {"Philips:P87C51RA2 *44TQ",0x68,29,11,44,8192,64,2},
 {"Philips:P87C51RB2",0x69,30,10,40,16384,64,2},
 {"Philips:P87C51RB2 *44",0x6a,30,11,44,16384,64,2},
 {"Philips:P87C51RB2 *44TQ",0x6a,30,11,44,16384,64,2},
 {"Philips:P87C51RC2",0x6b,31,10,40,32768,64,2},
 {"Philips:P87C51RC2 *44",0x6c,31,11,44,32768,64,2},
 {"Philips:P87C51RC2 *44TQ",0x6c,31,11,44,32768,64,2},
 {"Philips:P87C51RD2",0x6d,32,10,40,65536,64,2},
 {"Philips:P87C51RD2 *44",0x6e,32,11,44,65536,64,2},
 {"Philips:P87C51RD2 *44TQ",0x6e,32,11,44,65536,64,2},
 {"Philips:87C51RA+",0x57,21,10,40,8192,64,1},
 {"Philips:87C51RA+ *44",0x58,21,11,44,8192,64,1},
 {"Philips:87C51RB+",0x59,22,10,40,16384,64,1},
 {"Philips:87C51RB+ *44",0x5a,22,11,44,16384,64,1},
 {"Philips:87C51RC+",0x5b,23,10,40,32768,64,1},
 {"Philips:87C51RC+ *44",0x5c,23,11,44,32768,64,1},
 {"Tekmos:TK87C51RA2 *44",0x58,21,11,44,8192,64,1},
 {"Tekmos:TK87C51RB2 *44",0x5a,22,11,44,16384,64,1},
 {"Tekmos:TK87C51RC2 *44",0x5c,23,11,44,32768,64,1},
 };
 if(s->catalogue.device_class!=2018 || strcmp(s->definition_stem,"GAF____") || s->element_bits!=8 || s->secondary_element_bits!=8)return 0;
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins&&s->capacity_bytes==rows[i].size&&s->secondary_element_count==rows[i].secondary)return rows[i].kind;
 return 0;
}
