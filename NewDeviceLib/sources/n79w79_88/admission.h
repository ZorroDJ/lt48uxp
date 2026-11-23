/* Finite reviewed stages; zero until first qualification. */
#define LT48_N79W79_STAGE 4
static int n79_stage(unsigned d,unsigned xp){return xp?3:d>=0x185?4:d>=0x15f?1:2;}
static int n79_admitted(unsigned d,unsigned xp){return n79_stage(d,xp)<=LT48_N79W79_STAGE;}
