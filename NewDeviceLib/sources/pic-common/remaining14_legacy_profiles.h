#ifndef LT48_PIC_REMAINING14_LEGACY_PROFILES_H
#define LT48_PIC_REMAINING14_LEGACY_PROFILES_H
#include "profile.h"
/* Private exact bindings. No admission through these candidate descriptors. */
static const lt48_pic_region rm14_c84_regions[]={
 {0,1024,0,0x3fff,2,2,LT48_PIC_REGION_CODE},
 {0x2000,4,0x4000,0x3fff,2,2,LT48_PIC_REGION_IDS},
 {0x2007,1,0x400e,0x1f,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0x2100,64,0x4200,0xff,2,2,LT48_PIC_REGION_EEPROM},
};
static const lt48_pic_region rm14_f83_regions[]={
 {0,512,0,0x3fff,2,2,LT48_PIC_REGION_CODE},
 {0x2000,4,0x4000,0x3fff,2,2,LT48_PIC_REGION_IDS},
 {0x2007,1,0x400e,0x3fff,2,2,LT48_PIC_REGION_CONFIGURATION},
 {0x2100,64,0x4200,0xff,2,2,LT48_PIC_REGION_EEPROM},
};
static const lt48_pic_config_word rm14_c84_configuration[]={{0x400e,0x1f,0x3fe0,0x10,0x10}};
static const lt48_pic_config_word rm14_f83_configuration[]={{0x400e,0x3fff,0,0x3ff0,0x3ff0}};
static const lt48_pic_profile rm14_legacy_profiles[]={
 {423,287,109,LT48_PIC_COMPACT_LEGACY16,8,13,17024,1024,1024,rm14_c84_regions,4,
  NULL,NULL,0,rm14_c84_configuration,1,NULL,NULL,NULL,NULL,NULL,0,0,NULL},
 {424,285,109,LT48_PIC_COMPACT_LEGACY16,8,13,17024,512,512,rm14_f83_regions,4,
  NULL,NULL,0,rm14_f83_configuration,1,NULL,NULL,NULL,NULL,NULL,0,0,NULL},
};
#endif
