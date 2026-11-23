/* Authored finite extension of the existing Zilog owner. */
static const struct zilog70_profile*z8tail_binding(const lt48_device_spec*s){
 static const struct zilog70_profile rows[]={
 {"Zilog:Z86E61",2012,0xbe,106,99,40,16384,8,255},
 {"Zilog:Z86E61 *44",2012,0xbf,107,100,44,16384,8,255},
 {"Zilog:Z86E63",2012,0xc0,106,99,40,32768,8,255},
 {"Zilog:Z86E63 *44",2012,0xc1,107,100,44,32768,8,255}};
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(unsigned i=0;i<4;i++){const struct zilog70_profile*b=&rows[i];if(!strcmp(key,b->key)&&s->catalogue.device_class==b->cls&&s->definition_resource==b->d&&s->family_code==b->m&&s->pinmap==b->p&&s->pin_count==b->pins&&s->capacity_bytes==b->capacity&&s->element_bits==b->bits)return b;}return NULL;
}
