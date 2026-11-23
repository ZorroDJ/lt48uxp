#define LT48_W78D24_STAGE 3
static int lt48_w78d24_admitted(unsigned d){return d>=0xce&&d<=0xd3&&LT48_W78D24_STAGE>=(d==0xce||d==0xd1?1:d==0xcf||d==0xd2?2:3);}
