#ifndef LT48_FLASH51_IMAGE_REGIONS_H
#define LT48_FLASH51_IMAGE_REGIONS_H
/* Authored source-addressed file policy: physical code/data windows only.
 * FFFF on 89F54/F58 is a source security cell, not writable image data. */
static inline int lt48_flash51_address_mapped(unsigned family,unsigned end,unsigned a){
 if(!family||family>=18)return 1;
 if((family==11||family==12)&&a==65535)return 0;
 return a<end||a>=(family<=15?0xf000u:0xe000u);
}
#endif
