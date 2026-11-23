/* Owned manufacturer-backed corrections; original BYO resources unchanged. */
#ifndef LT48_PIC18_IMAGE_CORRECTIONS_H
#define LT48_PIC18_IMAGE_CORRECTIONS_H
#include "applet_image.h"
#include <string.h>
static const struct { uint32_t d,m,p; const char *source; unsigned char original[14],corrected[14]; } pic18_image_corrections[]={
 {587,434,212,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",{59,239,95,63,0,211,229,0,3,192,3,224,3,64},{59,239,95,63,0,211,197,0,3,192,3,224,3,64}},
 {588,435,212,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",{59,239,95,63,0,211,229,0,15,192,15,224,15,64},{59,239,95,63,0,211,197,0,15,192,15,224,15,64}},
};
static inline int lt48_pic18_correct_image_policy(uint32_t d,uint32_t m,uint32_t p,const char *source,const lt48_pic_applet_image_policy *original,lt48_pic_applet_image_policy *owned) {
 if(!original||!owned||!source)return -1;
 for(size_t i=0;i<sizeof(pic18_image_corrections)/sizeof(*pic18_image_corrections);i++) {
  if(pic18_image_corrections[i].d!=d||pic18_image_corrections[i].m!=m||pic18_image_corrections[i].p!=p)continue;
  if(strcmp(source,pic18_image_corrections[i].source)||original->configuration_count!=14||memcmp(original->configuration_mask,pic18_image_corrections[i].original,14))return -1;
  *owned=*original;owned->configuration_mask=pic18_image_corrections[i].corrected;return 1;
 }return 0;
}
#endif
