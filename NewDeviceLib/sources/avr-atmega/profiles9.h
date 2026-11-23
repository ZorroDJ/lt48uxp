/* Authored exact Dataman13.10 NEW-route ATmega64/128 and M1/C1 bindings.
 * P53 describes a32pin programming adapter, not the64pin target package. */
static const struct atmega_binding atmega9_bindings[]={
 {"Atmel:ATMega128/L @QFN(NEW)",0x112,0xc4,0x53,135168,32},
 {"Atmel:ATMega128/L@64TQ(NEW)",0x112,0xc4,0x53,135168,32},
 {"Atmel:ATMega128A @64TQ",0x112,0xc4,0x53,135168,32},
 {"Atmel:ATMega128A @QFN",0x112,0xc4,0x53,135168,32},
 {"Atmel:ATMega16M1 *32TQ",0x113,0xc5,0x54,16896,32},
 {"Atmel:ATMega32C1 *32TQ",0x115,0xc7,0x54,33792,32},
 {"Atmel:ATMega32M1 *32TQ",0x114,0xc6,0x54,33792,32},
 {"Atmel:ATMega64/L @64TQ(NEW)",0x111,0xc3,0x53,67584,32},
 {"Atmel:ATMega64/L@64QFN(NEW)",0x111,0xc3,0x53,67584,32},
 };
static int atmega9_binding_matches(const lt48_device_spec*s){
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof(atmega9_bindings)/sizeof(*atmega9_bindings);i++){const struct atmega_binding*b=&atmega9_bindings[i];if(!strcmp(key,b->key)&&s->definition_resource==b->definition&&s->family_code==b->family&&s->pinmap==b->pinmap&&s->capacity_bytes==b->capacity&&s->pin_count==b->pins)return 1;}return 0;
}
