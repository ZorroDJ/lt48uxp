/* Authored exact-source Winbond page-flash bindings, vendor resources BYO. */
static int winbond_page2_binding(const lt48_device_spec*s){
 char key[128];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 return s->catalogue.device_class==2005 && s->pin_count==32 && s->element_bits==8 && !s->secondary_element_count &&
 ((!strcmp(key,"Winbond:W29C020C")&&s->definition_resource==12&&s->family_code==12&&s->pinmap==5&&s->capacity_bytes==262144) ||
  (!strcmp(key,"Winbond:W29C040")&&s->definition_resource==13&&s->family_code==13&&s->pinmap==6&&s->capacity_bytes==524288));
}
