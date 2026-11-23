static const uint32_t emtc18_sectors_D22[]={0,16384,24576,32768,65536,131072,196608};
static const uint32_t emtc18_sectors_D21[]={0,65536,131072,196608,229376,237568,245760};
static const uint32_t emtc18_sectors_D24[]={0,16384,24576,32768,65536,131072,196608};
static const uint32_t emtc18_sectors_D23[]={0,65536,131072,196608,229376,237568,245760};
static const uint32_t emtc18_sectors_D25[]={0};
static const struct xp_closure emtc18_closures[]={
 {34,25,15,32,262144,500,emtc18_sectors_D22,7},
 {33,24,15,32,262144,500,emtc18_sectors_D21,7},
 {36,27,15,32,262144,500,emtc18_sectors_D24,7},
 {35,26,15,32,262144,500,emtc18_sectors_D23,7},
 {37,28,15,32,262144,500,emtc18_sectors_D25,1},
};
