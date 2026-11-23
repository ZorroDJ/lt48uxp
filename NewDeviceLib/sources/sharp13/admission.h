#define LT48_SHARP13_STAGE 2
static int sh13_admitted(unsigned d){return(d==0x12||d==0x13||d==0x14)?LT48_SHARP13_STAGE>=1:LT48_SHARP13_STAGE>=2;}
