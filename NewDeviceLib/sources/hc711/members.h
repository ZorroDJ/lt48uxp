struct hc711_member {const char *vendor,*name;unsigned definition,family,pinmap,stage;};
static const struct hc711_member hc711_members[]={
{"Freescale","MC68HC711K4 @84",0xc6,0x77,0x4d,2},
{"Motorola","MC68HC711K4 @84",0xc6,0x77,0x4d,2},
{"Freescale","MC68HC711KA2 @68",0xc7,0x78,0x4d,4},
{"Motorola","MC68HC711KA2 @68",0xc7,0x78,0x4d,4},
{"Freescale","MC68HC711E9 @52",0xcc,0x7d,0x4d,6},
{"Motorola","MC68HC711E9 @52",0xcc,0x7d,0x4d,6},
{"Freescale","MC68HC711E20 @52",0xcb,0x7c,0x4d,8},
{"Motorola","MC68HC711E20 @52",0xcb,0x7c,0x4d,8},
{"Freescale","MC68S711E9 @52",0xcd,0x7e,0x4d,10},
{"Motorola","MC68S711E9 @52",0xcd,0x7e,0x4d,10},
};
