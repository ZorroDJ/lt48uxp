/* Generated candidates only. No operational admission. */
#ifndef LT48_PIC_ICSP6_CANDIDATES_H
#define LT48_PIC_ICSP6_CANDIDATES_H
#include "profile.h"
static const lt48_pic_internal_program_policy pic_icsp6_internal={52,53,8,9};
static const lt48_pic_config_word pic_icsp6_configuration[]={
 {0x1000e,0x3fff,0,0x180,0x180},
 {0x10010,0x3703,0x8fc,3,0},
};
static const lt48_pic_region pic_icsp6_regions_2048[]={
 {0,2048,0,0x3fff,2,2,LT48_PIC_REGION_CODE},
 {0x8000,4,0x10000,0x3fff,2,2,LT48_PIC_REGION_IDS},
 {0x8007,1,0x1000e,0x3fff,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0x8008,1,0x10010,0x3703,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0xf000,256,0x1e000,0xff,2,2,LT48_PIC_REGION_EEPROM},
};
static const lt48_pic_window pic_icsp6_read_2048[]={{0,2048},{0x8000,9},{0xf000,256}};
static const lt48_pic_window pic_icsp6_program_2048[]={{0,2048},{0x8000,4},{0xf000,256}};
static const lt48_pic_region pic_icsp6_regions_4096[]={
 {0,4096,0,0x3fff,2,2,LT48_PIC_REGION_CODE},
 {0x8000,4,0x10000,0x3fff,2,2,LT48_PIC_REGION_IDS},
 {0x8007,1,0x1000e,0x3fff,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0x8008,1,0x10010,0x3703,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0xf000,256,0x1e000,0xff,2,2,LT48_PIC_REGION_EEPROM},
};
static const lt48_pic_window pic_icsp6_read_4096[]={{0,4096},{0x8000,9},{0xf000,256}};
static const lt48_pic_window pic_icsp6_program_4096[]={{0,4096},{0x8000,4},{0xf000,256}};
static const lt48_pic_region pic_icsp6_regions_8192[]={
 {0,8192,0,0x3fff,2,2,LT48_PIC_REGION_CODE},
 {0x8000,4,0x10000,0x3fff,2,2,LT48_PIC_REGION_IDS},
 {0x8007,1,0x1000e,0x3fff,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0x8008,1,0x10010,0x3703,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0xf000,256,0x1e000,0xff,2,2,LT48_PIC_REGION_EEPROM},
};
static const lt48_pic_window pic_icsp6_read_8192[]={{0,8192},{0x8000,9},{0xf000,256}};
static const lt48_pic_window pic_icsp6_program_8192[]={{0,8192},{0x8000,4},{0xf000,256}};
static const lt48_pic_profile pic_icsp6_candidates[]={
 {91,74,30,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {92,75,31,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {93,75,35,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {94,76,32,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {95,76,33,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {96,77,31,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {97,77,35,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {98,78,32,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {99,78,33,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {100,79,34,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {101,80,31,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {102,80,35,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {103,81,34,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {104,82,30,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {105,83,31,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {106,83,35,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {107,84,32,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {108,84,33,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,2048,2048,pic_icsp6_regions_2048,5,pic_icsp6_read_2048,pic_icsp6_program_2048,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {109,85,31,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {110,85,35,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {111,86,32,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {112,86,33,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {113,87,34,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {114,88,31,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {115,88,35,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {116,89,34,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {117,90,30,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,4096,4096,pic_icsp6_regions_4096,5,pic_icsp6_read_4096,pic_icsp6_program_4096,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {118,91,32,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
 {119,91,33,LT48_PIC_COMPACT_ROWS,10,10,0x1e200,8192,8192,pic_icsp6_regions_8192,5,pic_icsp6_read_8192,pic_icsp6_program_8192,3,pic_icsp6_configuration,2,NULL,&pic_icsp6_internal,NULL,NULL,NULL,0,0,NULL},
};
#endif
