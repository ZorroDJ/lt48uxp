/* Exact original catalogue bindings; vendor payloads remain BYO. */
struct hc705_ca_member {const char *vendor,*name;unsigned definition,family,pinmap,capacity,limit,stage;};
static const struct hc705_ca_member hc705_ca_members[]={
{"Freescale","MC68HC705C8A",0x11,16,11,8192,7936,4},
{"Motorola","MC68HC705C8A",0x11,16,11,8192,7936,4},
{"Freescale","MC68HC705C8A *44",0x12,17,12,8192,7936,4},
{"Motorola","MC68HC705C8A *44",0x12,17,12,8192,7936,4},
{"Freescale","MC68HC705C9A",0x18,23,18,16384,16128,10},
{"Motorola","MC68HC705C9A",0x18,23,18,16384,16128,10},
{"Freescale","MC68HC705C9A *44",0x19,25,20,16384,16128,10},
{"Motorola","MC68HC705C9A *44",0x19,25,20,16384,16128,10},
{"Freescale","MC68HC705C9A *44QFP",0x1a,24,19,16384,16128,10},
{"Motorola","MC68HC705C9A *44QFP",0x1a,24,19,16384,16128,10},
};
