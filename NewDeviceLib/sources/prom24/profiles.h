/* Authored source-bound AMD/TI PROM profiles. Original resources are BYO. */
static int prom24_binding(const lt48_device_spec*s){
 static const struct {const char*key;unsigned d,m,p,pins,capacity,bits;} rows[]={
 {"AMD:Am27LS191A",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27PS191",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27PS191A",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27PS291",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27S190/A",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27S191/A",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27S191SA",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27S290/A",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27S291/A",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27S291SA",0x113,0x81,0x4a,24,2048,8},
 {"AMD:Am27S43/A",0x112,0x81,0x49,24,4096,8},
 {"AMD:Am27S49/A",0x115,0x81,0x4c,24,8192,8},
 {"AMD:Am27PS181/A",0x116,0x82,0x4d,24,1024,8},
 {"AMD:Am27S180/A",0x116,0x82,0x4d,24,1024,8},
 {"AMD:Am27S181/A",0x116,0x82,0x4d,24,1024,8},
 {"AMD:Am27S31/A",0x117,0x82,0x4d,24,512,8},
 {"AMD:Am27S45/A/SA",0x114,0x82,0x4b,24,2049,8},
 {"AMD:Am27S47/A/SA",0x114,0x82,0x4b,24,2049,8},
 {"TI:TBP28L22",0x11b,0x84,0x1,20,256,8},
 {"TI:TBP28L42",0x11a,0x84,0x3,20,512,8},
 {"TI:TBP28LA22",0x11b,0x84,0x1,20,256,8},
 {"TI:TBP28P42",0x11a,0x84,0x3,20,512,8},
 {"TI:TBP28S42",0x11a,0x84,0x3,20,512,8},
 {"TI:TBP28SA42",0x11a,0x84,0x3,20,512,8},
 };
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 if(s->catalogue.device_class!=2000||strcmp(s->definition_stem,"GA_____"))return 0;
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){
  if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins&&s->capacity_bytes==rows[i].capacity&&s->element_bits==rows[i].bits)return 1;
 }return 0;
}
