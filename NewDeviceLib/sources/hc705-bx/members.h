/* Exact Dataman13.10 bindings; authored source only. */
struct hc705_bx_member {const char *vendor,*name;unsigned definition,family,pinmap,capacity,start,end,blank,variables,part,stage;};
static const struct hc705_bx_member hc705_bx_members[]={
{"Freescale","MC68HC705B16 *52",20,19,14,16384,0,0,0,8,16,8},
{"Motorola","MC68HC705B16 *52",20,19,14,16384,0,0,0,8,16,8},
{"Freescale","MC68HC705B16 @64",21,20,15,16384,0,0,0,8,16,8},
{"Motorola","MC68HC705B16 @64",21,20,15,16384,0,0,0,8,16,8},
{"Freescale","XC68HC705B32 *52",22,21,16,32768,0,0,0,8,32,8},
{"Motorola","XC68HC705B32 *52",22,21,16,32768,0,0,0,8,32,8},
{"Freescale","MC68HC705X32 @64Q",23,22,17,32768,0,0,0,8,32,8},
{"Motorola","MC68HC705X32 @64Q",23,22,17,32768,0,0,0,8,32,8},
{"Freescale","MC68HC705B5 *52",19,18,13,8192,0,0,0,7,5,10},
{"Motorola","MC68HC705B5 *52",19,18,13,8192,0,0,0,7,5,10},
};
