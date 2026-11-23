/* Authored exact-source admission bindings. Original resources are BYO. */
static int ptc_xp4_binding(const lt48_device_spec *s) {
 return s->catalogue.device_class==2034 && !strcmp(s->catalogue.vendor,"PTC") &&
 s->element_bits==8 && !s->secondary_element_count && s->pin_count==32 && s->family_code==3 &&
 ((s->definition_resource==3 && s->pinmap==3 && s->capacity_bytes==262144) ||
  (s->definition_resource==4 && s->pinmap==4 && s->capacity_bytes==524288));
}
