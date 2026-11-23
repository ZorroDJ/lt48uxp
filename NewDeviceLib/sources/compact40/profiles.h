/* Authored exact-source compact memory bindings. Vendor resources are BYO. */
struct compact40_profile{const char*key;unsigned cls,d,m,p,pins,capacity,bits,blank;};
static const struct compact40_profile*compact40_binding(const lt48_device_spec*s){
 static const struct compact40_profile rows[]={
 {"Atmel:AT27C513",2012,0x95,76,81,28,65536,8,255},
 {"Atmel:AT27C513R",2012,0x96,77,81,28,65536,8,255},
 {"Intel:27513",2012,0x92,76,80,28,65536,8,255},
 {"Intel:27C513",2012,0x91,75,80,28,65536,8,255},
 {"Intel:P27513",2012,0x91,75,80,28,65536,8,255},
 {"Microchip:27C513",2012,0x94,75,81,28,65536,8,255},
 {"Cypress:CY7C261",2012,0x88,68,72,24,8192,8,0},
 {"Cypress:CY7C263",2012,0x88,68,72,24,8192,8,0},
 {"Cypress:CY7C264",2012,0x88,68,72,24,8192,8,0},
 {"Cypress:CY7C266",2012,0x89,69,73,28,8192,8,0},
 {"Cypress:CY7C271",2012,0x8d,71,76,28,32768,8,0},
 {"Cypress:CY7C271A",2012,0x8e,72,76,28,32768,8,255},
 {"Cypress:CY7C274",2012,0x8f,73,78,28,32768,8,0},
 {"Cypress:CY7C276 *44",2012,0x90,74,79,44,32770,16,0},
 {"Cypress:CY7C281A",2012,0x8a,70,74,24,1024,8,0},
 {"Cypress:CY7C282A",2012,0x8a,70,74,24,1024,8,0},
 {"QP Semi:QP7C271",2012,0x8d,71,76,28,32768,8,0},
 {"Atmel:AT28BV64",2012,0x86,66,71,28,8192,8,255},
 {"Atmel:AT28C64/X",2012,0x87,67,71,28,8192,8,255},
 {"HARRIS:HM-6617",2012,0x8,6,6,24,2048,8,0},
 {"HARRIS:HM-6642",2012,0x7,7,7,24,512,8,0},
 {"Microchip:28C64A",2012,0x87,67,71,28,8192,8,255},
 {"Microchip:28C64AF",2012,0x87,67,71,28,8192,8,255},
 {"SEEQ:DQ36C16",2012,0x3,3,2,24,2048,8,255},
 {"SEEQ:DQ36C32",2012,0x4,3,3,24,4096,8,255},
 {"SEEQ:DQ52B13",2012,0x7a,58,55,24,2048,8,255},
 {"SEEQ:DQ52B33",2012,0x7b,59,64,28,8192,8,255},
 {"Xicor:X20C16",2012,0x6f,55,58,28,2048,8,255},
 {"Intel:P87C64",2012,0xd,12,10,28,8192,8,255},
 {"NEC:uPD17P012",2012,0x12,15,13,40,8192,8,255},
 {"NEC:uPD75P54",2012,0x11,14,12,20,1024,8,0},
 {"NEC:uPD75P64",2012,0x11,14,12,20,1024,8,0},
 {"NS:COP8780",2012,0x13,16,14,40,4096,8,255},
 {"NS:COP8780 *44",2012,0x14,16,15,44,4096,8,255},
 {"NS:COP8781",2012,0x16,16,17,28,4096,8,255},
 {"NS:COP8782",2012,0x15,16,16,20,4096,8,255},
 {"NS:COP8CxC9 *44PLCC",2012,0x3a,27,32,44,4096,8,0},
 {"NS:COP8CxE9 *44PLCC",2012,0x3b,28,32,44,8192,8,0},
 {"NS:COP8CxG9 *44PLCC",2012,0x3c,29,32,44,16384,8,0},
 {"NS:COP8S/CxR9 *44PLCC",2012,0x3d,30,32,44,32768,8,0},
 };
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct compact40_profile*b=&rows[i];
  if(!strcmp(key,b->key)&&s->catalogue.device_class==b->cls&&s->definition_resource==b->d&&s->family_code==b->m&&s->pinmap==b->p&&s->pin_count==b->pins&&s->capacity_bytes==b->capacity&&s->element_bits==b->bits)return b;
 }return NULL;
}
