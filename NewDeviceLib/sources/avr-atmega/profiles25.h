/* Authored exact AVR25 bindings. Original resources remain BYO. */
struct avr25_schema {unsigned family,fields,invert,reserved,extended_start,swap_locks,no_boot,numeric_base;};
static const struct avr25_schema avr25_schemas[]={
 {0x88,16,0xe940,0xff0000,16,0,0,0},
 {0x89,16,0xe940,0xff0000,16,0,0,0},
 {0x8b,16,0xe940,0xff0000,16,0,0,0},
 {0x91,17,0xb800,0xfe0000,16,0,1,0},
 {0x92,19,0x1b800,0xf80000,16,1,0,0},
 {0x93,19,0x1b800,0xf80000,16,1,0,0},
 {0x8a,20,0xf900,0xe10000,17,0,0,0},
 {0x8c,19,0xf900,0xf10000,17,0,0,0},
 {0x8d,19,0xf900,0xf10000,17,0,0,0},
 {0x8e,20,0x1f900,0xf00000,16,0,0,0},
 {0x8f,19,0x1f900,0xf80000,16,0,0,0},
 {0x90,19,0x1f900,0xf80000,16,0,0,0}
};
static const struct avr25_schema*avr25_schema_for(unsigned family){
 for(size_t i=0;i<sizeof avr25_schemas/sizeof *avr25_schemas;i++){if(avr25_schemas[i].family==family)return &avr25_schemas[i];}return NULL;
}
static const struct atmega_binding avr25_bindings[]={
 {"Atmel:ATmega32/L",0xa9,0x8b,0x39,33792,40},
 {"Atmel:ATmega32/L *44TQ",0xa8,0x8b,0x38,33792,44},
 {"Atmel:ATMega32/L @44MLF",0xa8,0x8b,0x38,33792,44},
 {"Atmel:ATMega8515/L",0xa2,0x88,0x36,8704,40},
 {"Atmel:ATMega8515/L *44",0xa3,0x88,0x37,8704,44},
 {"Atmel:ATMega8515/L *44TQ",0xa3,0x88,0x37,8704,44},
 {"Atmel:ATmega8535/L",0xa5,0x89,0x39,8704,40},
 {"Atmel:ATmega8535/L *44TQ",0xa4,0x89,0x38,8704,44},
 {"Atmel:ATmega48/V",0xaf,0x91,0x3d,4352,28},
 {"Atmel:ATmega48/V *32TQ",0xb0,0x91,0x3e,4352,32},
 {"Atmel:ATmega48/V @32MLF",0xb0,0x91,0x3e,4352,32},
 {"Atmel:ATmega88/V",0xb1,0x92,0x3d,8704,28},
 {"Atmel:ATmega88/V *32TQ",0xb2,0x92,0x3e,8704,32},
 {"Atmel:ATmega88/V @32MLF",0xb2,0x92,0x3e,8704,32},
 {"Atmel:ATmega168/V",0xb3,0x93,0x3d,16896,28},
 {"Atmel:ATmega168/V *32TQ",0xb4,0x93,0x3e,16896,32},
 {"Atmel:ATmega168/V @32MLF",0xb4,0x93,0x3e,16896,32},
 {"Atmel:ATmega162/V",0xa7,0x8a,0x3b,16896,40},
 {"Atmel:ATmega162/V *44TQ",0xa6,0x8a,0x3a,16896,44},
 {"Atmel:ATMega165/V @64TQ",0xab,0x8d,0x3c,16896,32},
 {"Atmel:ATMega165P/PV @64QFN",0xac,0x8e,0x3c,16896,32},
 {"Atmel:ATMega165P/PV @64TQ",0xac,0x8e,0x3c,16896,32},
 {"Atmel:ATMega169/L @64TQ",0xaa,0x8c,0x3c,16896,32},
 {"Atmel:ATMega329/V @64TQ",0xad,0x8f,0x3c,33792,32},
 {"Atmel:ATMega649/V @64TQ",0xae,0x90,0x3c,67584,32},
};
static int avr25_binding_matches(const lt48_device_spec*s){
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof avr25_bindings/sizeof *avr25_bindings;i++){const struct atmega_binding*b=&avr25_bindings[i];if(!strcmp(key,b->key)&&s->definition_resource==b->definition&&s->family_code==b->family&&s->pinmap==b->pinmap&&s->capacity_bytes==b->capacity&&s->pin_count==b->pins)return 7;}return 0;
}
