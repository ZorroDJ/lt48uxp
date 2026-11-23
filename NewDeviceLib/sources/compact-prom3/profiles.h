/* Authored compact PROM bindings; vendor resources remain BYO. */
static const struct prom30_profile*compact_prom3_binding(const lt48_device_spec*s){
 static const struct prom30_profile rows[]={
 {"Atmel:34C128 *20SO",2000,392,178,124,20,16384,8,255},
 {"NEC:uPD27HC65",2000,246,121,56,24,8192,8,255},
 {"NVM:3060",2000,191,78,31,8,512,8,255},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct prom30_profile*b=rows+i;if(!strcmp(key,b->key)&&s->catalogue.device_class==b->cls&&s->definition_resource==b->d&&s->family_code==b->m&&s->pinmap==b->p&&s->pin_count==b->pins&&s->capacity_bytes==b->capacity&&s->element_bits==b->bits)return b;}return NULL;
}
