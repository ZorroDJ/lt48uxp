/* Authored compact PROM bindings; vendor resources remain BYO. */
static const struct prom30_profile*atmel_hc2_binding(const lt48_device_spec*s){
 static const struct prom30_profile rows[]={
 {"Atmel:AT28HC191/L",2000,245,120,57,24,2048,8,255},
 {"Atmel:AT28HC291/L",2000,245,120,57,24,2048,8,255},
 };char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++){const struct prom30_profile*b=rows+i;if(!strcmp(key,b->key)&&s->catalogue.device_class==b->cls&&s->definition_resource==b->d&&s->family_code==b->m&&s->pinmap==b->p&&s->pin_count==b->pins&&s->capacity_bytes==b->capacity&&s->element_bits==b->bits)return b;}return NULL;
}
