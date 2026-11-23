/* Authored, source-bound AVR22 staged bindings. Original assets are BYO. */
static const struct atmega_binding avr22_bindings[]={
 {"Atmel:ATMega8A",0x10d,0xbf,0x4f,8704,28},
 {"Atmel:ATMega8A *32TQ",0x10e,0xc0,0x50,8704,32},
 {"Atmel:ATMega8A @32MLF",0x10e,0xc0,0x50,8704,32},
 {"Atmel:ATMega8/L(NEW)",0x10d,0xbf,0x4f,8704,28},
 {"Atmel:ATMega8/L *32TQ(NEW)",0x10e,0xc0,0x50,8704,32},
 {"Atmel:ATMega8/L @32MLF(NEW)",0x10e,0xc0,0x50,8704,32},
 {"Atmel:ATMega16/L(NEW)",0x10f,0xc1,0x51,16896,40},
 {"Atmel:ATMega16/L *44TQ(NEW)",0x110,0xc2,0x52,16896,44},
 {"Atmel:ATMega16/L*44MLF(NEW)",0x110,0xc2,0x52,16896,44},
 {"Atmel:AT90CAN32 @64TQ",0xc5,0x9c,0x41,33792,32},
 {"Atmel:AT90CAN64 @64TQ",0xc6,0x9d,0x41,67584,32},
 {"Atmel:AT90CAN128 @64TQ",0xc7,0x9e,0x41,135168,32},
 {"Atmel:AT90USB1286 @64TQ",0xc8,0x9f,0x41,135168,32},
 {"Atmel:AT90USB1286 @64QFN",0xc8,0x9f,0x41,135168,32},
 {"Atmel:AT90USB1287 @64TQ",0xc8,0x9f,0x41,135168,32},
 {"Atmel:AT90USB1287 @64QFN",0xc8,0x9f,0x41,135168,32},
};
static int avr22_binding_matches(const lt48_device_spec*s){
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof(avr22_bindings)/sizeof(*avr22_bindings);i++){const struct atmega_binding*b=&avr22_bindings[i];if(!strcmp(key,b->key)&&s->definition_resource==b->definition&&s->family_code==b->family&&s->pinmap==b->pinmap&&s->capacity_bytes==b->capacity&&s->pin_count==b->pins)return s->definition_resource>=0x10d?4:s->definition_resource==0xc8?6:5;}return 0;
}
static const struct atmega_binding avr22_tiny_bindings[]={
 {"Atmel:ATtiny25/V",0x65,0x50,0x16,2176,8},
 {"Atmel:ATtiny25/V *8SO",0x65,0x50,0x16,2176,8},
 {"Atmel:ATtiny45/V",0x66,0x51,0x16,4352,8},
 {"Atmel:ATtiny45/V *8SO",0x66,0x51,0x16,4352,8},
 {"Atmel:ATtiny85/V",0x67,0x52,0x16,8704,8},
 {"Atmel:ATtiny85/V *8SO",0x67,0x52,0x16,8704,8},
};
static int avr22_tiny_binding_matches(const lt48_device_spec*s){
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof(avr22_tiny_bindings)/sizeof(*avr22_tiny_bindings);i++){const struct atmega_binding*b=&avr22_tiny_bindings[i];if(!strcmp(key,b->key)&&s->definition_resource==b->definition&&s->family_code==b->family&&s->pinmap==b->pinmap&&s->capacity_bytes==b->capacity&&s->pin_count==b->pins)return 1;}return 0;
}
