#define LT48_LPC16_STAGE 3
static int lpc16_admitted(unsigned d){return(d==0x143||d==0x144)?LT48_LPC16_STAGE>=1:(d>=0x145&&d<=0x147)?LT48_LPC16_STAGE>=2:(d>=0x168&&d<=0x16a)?LT48_LPC16_STAGE>=3:0;}
