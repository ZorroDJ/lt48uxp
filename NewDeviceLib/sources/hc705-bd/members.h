/* Exact Dataman13.10 bindings; no original binary assets. */
struct hc705_bd_member {const char *vendor,*name;unsigned definition,family,pinmap,capacity,start,end,blank,variables,part,stage;};
static const struct hc705_bd_member hc705_bd_members[]={
{"Freescale","MC68HC705BD3",1,1,1,16384,8192,16127,0,7,3,8},
{"Motorola","MC68HC705BD3",1,1,1,16384,8192,16127,0,7,3,8},
{"Freescale","MC68HC705BD3 *42",2,2,2,16384,8192,16127,0,7,3,8},
{"Motorola","MC68HC705BD3 *42",2,2,2,16384,8192,16127,0,7,3,8},
{"Freescale","MC68HC705BD7",3,3,1,16384,4096,15871,0,9,7,8},
{"Motorola","MC68HC705BD7",3,3,1,16384,4096,15871,0,9,7,8},
{"Freescale","MC68HC705BD7 *42",4,4,3,16384,4096,15871,0,9,7,8},
{"Motorola","MC68HC705BD7 *42",4,4,3,16384,4096,15871,0,9,7,8},
{"Freescale","MC68HC705BD9 *42",27,26,21,65536,40960,65023,255,6,9,10},
{"Motorola","MC68HC705BD9 *42",27,26,21,65536,40960,65023,255,6,9,10},
};
