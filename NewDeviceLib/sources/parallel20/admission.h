#define LT48_PARALLEL20_STAGE 2
static int p20_admitted(unsigned d){return(d==0x101||d==0x103)?LT48_PARALLEL20_STAGE>=1:LT48_PARALLEL20_STAGE>=2;}
