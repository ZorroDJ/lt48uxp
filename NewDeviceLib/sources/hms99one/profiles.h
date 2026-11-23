#define LT48_HMS99ONE_ADMITTED 1
/* Exact authored Hyundai source profile. No vendor payload. */
static const struct lm20_profile*hms99one_binding(const lt48_device_spec*s){
 static const struct lm20_profile p={"Hyundai(Hynix):HMS99C52",228,130,86,40,8192,11,8,6,1,0};
 if(!s||s->catalogue.device_class!=2013||strcmp(s->definition_stem,"GADA___")||s->definition_resource!=228||s->family_code!=130||s->pinmap!=86||s->pin_count!=40||s->capacity_bytes!=8192||s->element_bits!=8||s->secondary_element_count||strcmp(s->catalogue.vendor,"Hyundai(Hynix)")||s->catalogue.device_name_length!=8||memcmp(s->catalogue.device_name,"HMS99C52",8))return NULL;
 return &p;
}
