/* Authored source-bound AMD/TI PROM profiles. Original resources are BYO. */
static int prom17_binding(const lt48_device_spec*s){
 static const struct {const char*key;unsigned d,m,p,pins,capacity,bits;} rows[]={
 {"AMD:Am27S12",0x10e,0x80,0x48,16,512,4},
 {"AMD:Am27S12A",0x10e,0x80,0x48,16,512,4},
 {"AMD:Am27S13",0x10e,0x80,0x48,16,512,4},
 {"AMD:Am27S13A",0x10e,0x80,0x48,16,512,4},
 {"AMD:Am27LS18",0x10f,0x80,0x2,16,32,8},
 {"AMD:Am27LS19",0x10f,0x80,0x2,16,32,8},
 {"AMD:Am27S18/A",0x10f,0x80,0x2,16,32,8},
 {"AMD:Am27S19/A",0x10f,0x80,0x2,16,32,8},
 {"AMD:Am27S19SA",0x10f,0x80,0x2,16,32,8},
 {"AMD:Am27S20",0x110,0x80,0x48,16,256,4},
 {"AMD:Am27S21",0x110,0x80,0x48,16,256,4},
 {"AMD:Am27S28/A",0x111,0x80,0x3,20,512,8},
 {"AMD:Am27S29/A",0x111,0x80,0x3,20,512,8},
 {"TI:TBP18S030",0x118,0x83,0x2,16,32,8},
 {"TI:TBP18SA030",0x118,0x83,0x2,16,32,8},
 {"TI:TBP18S42",0x119,0x83,0x3,20,512,8},
 {"TI:TBP18SA42",0x119,0x83,0x3,20,512,8},
 };
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 if(s->catalogue.device_class!=2000||strcmp(s->definition_stem,"GA_____"))return 0;
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){
  if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins&&s->capacity_bytes==rows[i].capacity&&s->element_bits==rows[i].bits)return 1;
 }return 0;
}
