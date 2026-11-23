/* Authored exact source bindings; original programming assets remain BYO. */
static const struct prom30_profile*cypress_prom11_binding(const lt48_device_spec*s){
 static const struct prom30_profile rows[]={
 {"Cypress:CY7C225",2011,5,5,2,24,512,8,255},
 {"Cypress:CY7C235",2011,4,4,1,24,1025,8,255},
 {"Cypress:CY7C245",2011,0,0,0,24,2050,8,255},
 {"Cypress:CY7C245A",2011,1,1,0,24,2050,8,0},
 {"Cypress:CY7C251",2011,6,6,3,28,16384,8,0},
 {"Cypress:CY7C254",2011,6,6,3,28,16384,8,0},
 {"Cypress:CY7C291",2011,2,2,0,24,2048,8,255},
 {"Cypress:CY7C291A",2011,3,3,0,24,2048,8,0},
 {"Cypress:CY7C292",2011,2,2,0,24,2048,8,255},
 {"Cypress:CY7C292A",2011,3,3,0,24,2048,8,0},
 {"Cypress:CY7C293A",2011,3,3,0,24,2048,8,0},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct prom30_profile*b=rows+i;
 if(!strcmp(key,b->key)&&s->catalogue.device_class==b->cls&&s->definition_resource==b->d&&s->family_code==b->m&&s->pinmap==b->p&&s->pin_count==b->pins&&s->capacity_bytes==b->capacity&&s->element_bits==b->bits)return b;
 }return NULL;
}
