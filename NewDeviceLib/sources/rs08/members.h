/* Exact Dataman13.10 aliases; shared definition, DIP8 first. */
struct rs08_member {const char *vendor,*name;unsigned definition,family,pinmap,capacity,blank,variables,part,stage,first,supply,channel;};
static const struct rs08_member rs08_members[]={
{"Freescale","MC9RS08KA2",107,100,72,16384,255,4,1,2,14336,500,2},
{"Freescale","MC9RS08KA2 *8SO",107,100,72,16384,255,4,1,2,14336,500,2},
};
