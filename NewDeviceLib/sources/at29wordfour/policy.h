/* Authored finite bindings. Source PAGE units are bytes, not words. */
#define LT48_AT29WORD_ADMITTED 1
static int at29word_binding(const lt48_device_spec*s){
 if(!s||s->catalogue.device_class!=2005||strcmp(s->catalogue.vendor,"Atmel")||s->definition_resource<0x29||s->definition_resource>0x2c)return 0;
 unsigned i=s->definition_resource-0x29;
 return s->family_code==33+i&&s->pinmap==11+(i&1)&&s->capacity_bytes==131072&&s->element_bits==16;
}
