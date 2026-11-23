#define LT48_SST_STAGE 2
static inline int lt48_sst_admitted(unsigned d){return d>=0xb2 && d<=0xc1 && (LT48_SST_STAGE==2 || (LT48_SST_STAGE==1 && (d==0xb8||d==0xb9||d==0xc0||d==0xc1)));}
