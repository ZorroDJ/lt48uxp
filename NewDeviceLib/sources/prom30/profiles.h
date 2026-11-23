/* Authored exact-source compact memory bindings. Vendor resources are BYO. */
struct prom30_profile{const char*key;unsigned cls,d,m,p,pins,capacity,bits,blank;};
static const struct prom30_profile*prom30_binding(const lt48_device_spec*s){
 static const struct prom30_profile rows[]={
 {"Waferscale:WS57C191",2012,0x7c,60,65,24,2048,8,255},
 {"Waferscale:WS57C191B",2012,0x7c,60,65,24,2048,8,255},
 {"Waferscale:WS57C191C",2012,0x7d,61,65,24,2048,8,255},
 {"Waferscale:WS57C291",2012,0x7c,60,65,24,2048,8,255},
 {"Waferscale:WS57C291B",2012,0x7c,60,65,24,2048,8,255},
 {"Waferscale:WS57C291C",2012,0x7d,61,65,24,2048,8,255},
 {"Waferscale:WS57C43",2012,0x7e,60,66,24,4096,8,255},
 {"Waferscale:WS57C43B",2012,0x7e,60,66,24,4096,8,255},
 {"Waferscale:WS57C43C",2012,0x7f,61,66,24,4096,8,255},
 {"Waferscale:WS57C49",2012,0x80,60,67,24,8192,8,255},
 {"Waferscale:WS57C49B",2012,0x80,60,67,24,8192,8,255},
 {"Waferscale:WS57C49C",2012,0x81,61,67,24,8192,8,255},
 {"Waferscale:WS57C45",2012,0x85,65,70,24,2048,8,0},
 {"Waferscale:WS57C45C",2012,0x85,65,70,24,2048,8,0},
 {"Waferscale:WS57C51",2012,0x82,62,68,28,16384,8,255},
 {"Waferscale:WS57C51B",2012,0x82,62,68,28,16384,8,255},
 {"Waferscale:WS57C51C",2012,0x83,63,68,28,16384,8,255},
 {"Waferscale:WS57C71C",2012,0x84,64,69,28,32768,8,255},
 {"ASAHI KASEI:AK27CX161",2000,0xf1,118,54,24,2048,8,255},
 {"ASAHI KASEI:AK27CX162",2000,0xf1,118,54,24,2048,8,255},
 {"ASAHI KASEI:AK27CX321",2000,0xf2,118,55,24,4096,8,255},
 {"ASAHI KASEI:AK27CX322",2000,0xf2,118,55,24,4096,8,255},
 {"ASAHI KASEI:AK27CX641",2000,0xf3,118,56,24,8192,8,255},
 {"ASAHI KASEI:AK27CX642",2000,0xf3,118,56,24,8192,8,255},
 {"Atmel:AT27HC641",2000,0xf3,118,56,24,8192,8,255},
 {"Atmel:AT27HC642",2000,0xf3,118,56,24,8192,8,255},
 {"ICT:AK27CX321",2000,0xf2,118,55,24,4096,8,255},
 {"ICT:AK27CX322",2000,0xf2,118,55,24,4096,8,255},
 {"ICT:AK27CX641",2000,0xf3,118,56,24,8192,8,255},
 {"ICT:AK27CX642",2000,0xf3,118,56,24,8192,8,255},
 };
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct prom30_profile*b=&rows[i];
  if(!strcmp(key,b->key)&&s->catalogue.device_class==b->cls&&s->definition_resource==b->d&&s->family_code==b->m&&s->pinmap==b->p&&s->pin_count==b->pins&&s->capacity_bytes==b->capacity&&s->element_bits==b->bits)return b;
 }return NULL;
}
