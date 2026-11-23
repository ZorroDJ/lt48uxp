#define LT48_W78BANK147_STAGE 2
static int lt48_w78bank147_admitted(unsigned d){return d>=0x9a&&d<=0xb1&&d!=0xa5&&d!=0xa6&&LT48_W78BANK147_STAGE>=(d>=0x9a&&d<=0x9d?1:2);}
