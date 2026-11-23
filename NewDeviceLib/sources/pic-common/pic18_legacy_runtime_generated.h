/* Exact generated legacy protocol policy; no admission. */
#ifndef LT48_PIC18_LEGACY_RUNTIME_GENERATED_H
#define LT48_PIC18_LEGACY_RUNTIME_GENERATED_H
typedef struct { uint32_t definition;uint16_t family,pinmap;uint8_t erase_patch; } lt48_pic18_legacy_runtime_policy;
static const lt48_pic18_legacy_runtime_policy lt48_pic18_legacy_runtime_policies[]={
 {53,42,22,0},
 {54,43,23,0},
 {55,44,24,0},
 {72,61,25,1},
 {73,62,26,1},
 {74,63,27,1},
 {75,64,28,1},
 {80,69,29,0},
 {81,70,30,0},
 {82,69,29,0},
 {83,70,30,0},
 {84,71,31,1},
 {85,72,32,1},
 {86,73,33,1},
 {87,74,34,1},
 {92,79,31,1},
 {96,83,35,1},
 {97,84,36,1},
 {98,85,37,1},
 {99,86,38,1},
 {100,87,35,1},
 {101,88,36,1},
 {102,89,37,1},
 {103,90,38,1},
 {141,124,53,1},
 {142,125,54,1},
 {143,126,55,1},
 {144,127,56,1},
 {145,128,53,1},
 {146,129,54,1},
 {147,130,55,1},
 {148,131,56,1},
};
static inline const lt48_pic18_legacy_runtime_policy *lt48_pic18_legacy_runtime_find(uint32_t d,uint16_t f,uint16_t p) {
 for(size_t i=0;i<sizeof(lt48_pic18_legacy_runtime_policies)/sizeof(*lt48_pic18_legacy_runtime_policies);i++) {
  const lt48_pic18_legacy_runtime_policy *r=lt48_pic18_legacy_runtime_policies+i;
  if(r->definition==d&&r->family==f&&r->pinmap==p)return r;
 } return NULL;
}
static const unsigned char lt48_pic18_legacy_program_original_sha[32]={0x1d,0xb5,0xe0,0x14,0x7f,0x0a,0x15,0x44,0x0b,0x19,0x62,0xaa,0x52,0x1f,0x45,0xe9,0xee,0x1f,0xb4,0xe1,0x3e,0x3e,0x42,0x03,0x53,0x4f,0x12,0xf4,0x6d,0x0a,0x23,0xca};
static const unsigned char lt48_pic18_legacy_program_patched_sha[32]={0x4f,0xaa,0x82,0x97,0x84,0x1f,0x60,0x42,0xc0,0x9f,0x4b,0x4d,0x9d,0x8f,0xb9,0x26,0x55,0x93,0x24,0x2f,0x0c,0x02,0x3c,0xe8,0xb8,0x6d,0x85,0x15,0x3b,0x56,0x93,0x93};
static const unsigned char lt48_pic18_legacy_configuration_original_sha[32]={0x7f,0x9a,0xeb,0x21,0x0e,0x26,0xf2,0x8a,0xa1,0xad,0xc0,0x99,0x69,0x5b,0x58,0x1a,0xa7,0xae,0xa2,0xe0,0x9e,0x6b,0xfa,0x64,0xbc,0x81,0xda,0x29,0x5a,0xab,0x3e,0x52};
static const unsigned char lt48_pic18_legacy_configuration_patched_sha[32]={0x59,0x42,0x2a,0x59,0x4b,0xf8,0x98,0xd6,0xd4,0x78,0x28,0xfc,0x31,0x33,0xfb,0xe4,0x9b,0x95,0x85,0xe1,0x30,0x28,0xba,0xeb,0x21,0x03,0xdb,0x50,0x71,0x64,0x96,0x32};
static const unsigned char lt48_pic18_legacy_erase_original_sha[32]={0x3d,0xa8,0xda,0x80,0xb4,0xbf,0xcb,0x83,0x79,0x07,0x6d,0xb0,0x23,0xad,0x55,0xda,0xae,0x12,0x49,0x74,0xd5,0xe9,0x19,0x57,0x0d,0xdd,0xbf,0x97,0x9a,0x0c,0x4b,0x1d};
static const unsigned char lt48_pic18_legacy_erase_patched_sha[32]={0x44,0xce,0xea,0xe3,0x72,0xb0,0x97,0x7c,0xeb,0x8e,0xa5,0x23,0x03,0x96,0xd5,0x29,0x64,0x30,0x3c,0x56,0x0b,0x32,0x0a,0x29,0xcb,0x2b,0x7f,0xb1,0x04,0x5b,0xb4,0xb7};
typedef struct { uint32_t definition;uint16_t family,pinmap;const char *name; } lt48_pic18_legacy_runtime_member;
static const lt48_pic18_legacy_runtime_member lt48_pic18_legacy_runtime_members[]={
 {80,69,29,"PIC18F1230"},
 {80,69,29,"PIC18F1230 *18SO"},
 {82,69,29,"PIC18F1330"},
 {82,69,29,"PIC18F1330 *18SO"},
 {81,70,30,"PIC18F1230 *20SSOP"},
 {83,70,30,"PIC18F1330 *20SSOP"},
 {92,79,31,"PIC18F2423"},
 {92,79,31,"PIC18F2423 *28SO"},
 {92,79,31,"PIC18F2423 @28QFN"},
 {84,71,31,"PIC18F2450"},
 {84,71,31,"PIC18F2450 *28SO"},
 {84,71,31,"PIC18F2450 @28QFN"},
 {72,61,25,"PIC18F2455"},
 {72,61,25,"PIC18F2455 *28SO"},
 {72,61,25,"PIC18F2458"},
 {72,61,25,"PIC18F2458 *28SO"},
 {96,83,35,"PIC18F2480"},
 {96,83,35,"PIC18F2480 *28SO"},
 {96,83,35,"PIC18F2480 @28QFN"},
 {141,124,53,"PIC18F2523"},
 {141,124,53,"PIC18F2523 *28SO"},
 {141,124,53,"PIC18F2523 @28QFN"},
 {145,128,53,"PIC18F2550"},
 {145,128,53,"PIC18F2550 *28SO"},
 {145,128,53,"PIC18F2553"},
 {145,128,53,"PIC18F2553 *28SO"},
 {100,87,35,"PIC18F2580"},
 {100,87,35,"PIC18F2580 *28SO"},
 {100,87,35,"PIC18F2580 @28QFN"},
 {92,79,31,"PIC18LF2423"},
 {92,79,31,"PIC18LF2423 *28SO"},
 {92,79,31,"PIC18LF2423 @28QFN"},
 {85,72,32,"PIC18F4450"},
 {73,62,26,"PIC18F4455"},
 {73,62,26,"PIC18F4458"},
 {97,84,36,"PIC18F4480"},
 {142,125,54,"PIC18F4523"},
 {146,129,54,"PIC18F4550"},
 {146,129,54,"PIC18F4553"},
 {101,88,36,"PIC18F4580"},
 {142,125,54,"PIC18LF4523"},
 {86,73,33,"PIC18F4450 *44TQ"},
 {74,63,27,"PIC18F4455 *44TQ"},
 {74,63,27,"PIC18F4458 *44TQ"},
 {98,85,37,"PIC18F4480 *44TQ"},
 {143,126,55,"PIC18F4523 *44TQ"},
 {147,130,55,"PIC18F4550 *44TQ"},
 {147,130,55,"PIC18F4553 *44TQ"},
 {102,89,37,"PIC18F4580 *44TQ"},
 {143,126,55,"PIC18LF4523 *44TQ"},
 {87,74,34,"PIC18F4450 @44QFN"},
 {75,64,28,"PIC18F4455 @44QFN"},
 {75,64,28,"PIC18F4458 @44QFN"},
 {99,86,38,"PIC18F4480 @44QFN"},
 {144,127,56,"PIC18F4523 @44QFN"},
 {148,131,56,"PIC18F4550 @44QFN"},
 {148,131,56,"PIC18F4553 @44QFN"},
 {103,90,38,"PIC18F4580 @44QFN"},
 {144,127,56,"PIC18LF4523 @44QFN"},
 {53,42,22,"PIC18LF4620"},
 {54,43,23,"PIC18LF4620 *44TQ"},
 {55,44,24,"PIC18LF4620 @44QFN"},
};
static inline int lt48_pic18_legacy_runtime_member_allowed(uint32_t d,uint16_t f,uint16_t p,const unsigned char *name,size_t size) {
 if(!name)return 0;
 for(size_t i=0;i<sizeof(lt48_pic18_legacy_runtime_members)/sizeof(*lt48_pic18_legacy_runtime_members);i++) {
  const lt48_pic18_legacy_runtime_member *r=lt48_pic18_legacy_runtime_members+i;
  if(r->definition==d&&r->family==f&&r->pinmap==p&&strlen(r->name)==size&&!memcmp(r->name,name,size))return 1;
 }return 0;
}
#endif
