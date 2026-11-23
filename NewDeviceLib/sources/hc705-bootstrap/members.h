/* Exact class2014 catalogue binding; source payloads remain BYO. */
struct hc705_boot_member {const char *vendor,*name;unsigned definition,family,pinmap,capacity,limit,stage;};
static const struct hc705_boot_member hc705_boot_members[]={
{"Freescale","MC68HC705C8",0x0,0,0,8192,7936,8},
{"Motorola","MC68HC705C8",0x0,0,0,8192,7936,8},
{"Freescale","MC68HC705C4A",0x7,8,0,8192,4352,8},
{"Motorola","MC68HC705C4A",0x7,8,0,8192,4352,8},
{"Freescale","MC68HC705C9",0x9,9,0,16384,16128,8},
{"Freescale","MC68HC705D9",0x9,9,0,16384,16128,8},
{"Motorola","MC68HC705C9",0x9,9,0,16384,16128,8},
{"Motorola","MC68HC705D9",0x9,9,0,16384,16128,8},
{"Freescale","MC68HC705C8 *44",0x5,5,4,8192,7936,18},
{"Motorola","MC68HC705C8 *44",0x5,5,4,8192,7936,18},
{"Freescale","MC68HC705C4A *44",0x8,7,4,8192,4352,18},
{"Motorola","MC68HC705C4A *44",0x8,7,4,8192,4352,18},
{"Freescale","MC68HC705C9 *44",0xa,10,6,16384,16128,18},
{"Freescale","MC68HC705D9 *44",0xa,10,6,16384,16128,18},
{"Motorola","MC68HC705C9 *44",0xa,10,6,16384,16128,18},
{"Motorola","MC68HC705D9 *44",0xa,10,6,16384,16128,18},
{"Freescale","MC68HC705C8A *44Q",0x6,6,5,8192,7936,18},
{"Motorola","MC68HC705C8A *44Q",0x6,6,5,8192,7936,18},
};
