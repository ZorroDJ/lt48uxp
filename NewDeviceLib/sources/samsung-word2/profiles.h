/* Authored exact Samsung source bindings; imported binaries remain BYO. */
static const struct gaaf_profile samsung_word2_profiles[]={
{330,285,7,1048576,{9,17,13,438,16,439,443,65535,441,442,65535,65535,65535,27,19,20,65535,18,65535,7}},
{329,284,7,1048576,{9,17,13,438,16,439,443,65535,441,442,65535,65535,65535,27,19,20,65535,18,65535,6}},
};
static int samsung_word2_binding(const lt48_device_spec*s){
 char key[128];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 return s->catalogue.device_class==2003 && s->pin_count==48 && s->element_bits==16 && !s->secondary_element_count && ((!strcmp(key,"Samsung:KM28U800B *48TS")&&s->definition_resource==330) || (!strcmp(key,"Samsung:KM28U800T *48TS")&&s->definition_resource==329));
}
