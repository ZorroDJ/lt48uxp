/* Authored MYSON OTP bindings, reusing the qualified class2013 owner. */
static const struct legacy51_9_profile*mysonM2_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2013||strcmp(s->definition_stem,"GADA___")||s->element_bits!=8||s->secondary_element_count)return NULL;
static const struct legacy51_9_profile rows[]={
{"MYSON:MTV112M",0xa6,96,61,40,32768,0},
{"MYSON:MTV230M *44",0xa7,97,62,44,98304,0},
};char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
for(size_t i=0;i<sizeof rows/sizeof*rows;i++){const struct legacy51_9_profile*p=&rows[i];if(!strcmp(key,p->key)&&s->definition_resource==p->d&&s->family_code==p->m&&s->pinmap==p->p&&s->pin_count==p->pins&&s->capacity_bytes==p->size)return p;}return NULL;}
