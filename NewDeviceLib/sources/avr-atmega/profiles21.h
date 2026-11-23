/* Authored exact Dataman13.10 bindings: 164/324/644/1284. */
static const struct atmega_binding atmega21_bindings[]={
 {"Atmel:ATMega164/V",0xb5,0x94,0x3f,16896,40},
 {"Atmel:ATMega164/V *44TQ",0xb6,0x94,0x40,16896,44},
 {"Atmel:ATMega164P/V",0xbb,0x98,0x3f,16896,40},
 {"Atmel:ATMega164P/V *44TQ",0xbc,0x98,0x40,16896,44},
 {"Atmel:ATMega164P/V @44MLF",0xbc,0x98,0x40,16896,44},
 {"Atmel:ATmega1284",0xc1,0x97,0x3f,135168,40},
 {"Atmel:ATmega1284 *44TQ",0xc2,0x97,0x40,135168,44},
 {"Atmel:ATmega1284 @44MLF",0xc2,0x97,0x40,135168,44},
 {"Atmel:ATmega1284P",0xc3,0x9b,0x3f,135168,40},
 {"Atmel:ATmega1284P *44TQ",0xc4,0x9b,0x40,135168,44},
 {"Atmel:ATmega1284P @44MLF",0xc4,0x9b,0x40,135168,44},
 {"Atmel:ATmega324/V",0xb7,0x95,0x3f,33792,40},
 {"Atmel:ATmega324/V *44TQ",0xb8,0x95,0x40,33792,44},
 {"Atmel:ATmega324P/V",0xbd,0x99,0x3f,33792,40},
 {"Atmel:ATmega324P/V *44TQ",0xbe,0x99,0x40,33792,44},
 {"Atmel:ATmega324P/V @44MLF",0xbe,0x99,0x40,33792,44},
 {"Atmel:ATmega644/V",0xb9,0x96,0x3f,67584,40},
 {"Atmel:ATmega644/V *44TQ",0xba,0x96,0x40,67584,44},
 {"Atmel:ATmega644P/V",0xbf,0x9a,0x3f,67584,40},
 {"Atmel:ATmega644P/V *44TQ",0xc0,0x9a,0x40,67584,44},
 {"Atmel:ATmega644P/V @44MLF",0xc0,0x9a,0x40,67584,44},
};
static int atmega21_binding_matches(const lt48_device_spec*s){
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof(atmega21_bindings)/sizeof(*atmega21_bindings);i++){const struct atmega_binding*b=&atmega21_bindings[i];if(!strcmp(key,b->key)&&s->definition_resource==b->definition&&s->family_code==b->family&&s->pinmap==b->pinmap&&s->capacity_bytes==b->capacity&&s->pin_count==b->pins)return 1;}return 0;
}
