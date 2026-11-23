#define LT48_WORD14_STAGE 2
static int word14_admitted(unsigned d){return d==0x61||d==0x62?LT48_WORD14_STAGE>=1:LT48_WORD14_STAGE>=2;}
