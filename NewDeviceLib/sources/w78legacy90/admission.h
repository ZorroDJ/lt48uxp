#define LT48_W78LEGACY90_STAGE 4
int lt48_w78legacy90_admitted_internal(unsigned d){unsigned stage=d==0x69||d==0x6a?1:d>=0x6b&&d<=0x6e?2:d>=0x61&&d<=0x68?3:d>=0x6f&&d<=0x70?4:99;return LT48_W78LEGACY90_STAGE>=stage;}
