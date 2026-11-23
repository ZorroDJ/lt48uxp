/* Exact Dataman13.10 J/P bindings. */
struct hc705_jp_member {const char *vendor,*name;unsigned definition,family,pinmap,capacity,limit,stage;};
static const struct hc705_jp_member hc705_jp_members[]={
{"Freescale","MC68HC705P6 *28SO",12,12,7,8192,0,8},
{"Freescale","MC68HC705P6/A",12,12,7,8192,0,8},
{"Motorola","MC68HC705P6 *28SO",12,12,7,8192,0,8},
{"Motorola","MC68HC705P6/A",12,12,7,8192,0,8},
{"Freescale","MC68HC705J2",13,13,8,4096,0,8},
{"Motorola","MC68HC705J2",13,13,8,4096,0,8},
{"Freescale","MC68HC705J2(J1EMUL)",14,14,8,2048,0,8},
{"Motorola","MC68HC705J2(J1EMUL)",14,14,8,2048,0,8},
{"Freescale","MC68HC705P9",11,11,7,8192,0,10},
{"Motorola","MC68HC705P9",11,11,7,8192,0,10},
};
