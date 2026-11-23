#ifndef LT48_NTAUX_REGIONS_H
#define LT48_NTAUX_REGIONS_H
/* Authored bindings to original AddTab extents, not a flat-memory inference. */
#include "admission.h"
static inline int ntaux_admitted(unsigned d){unsigned stage=(d==470||d==471)?1:(d==472||d==473)?2:(d>=474&&d<=476)?3:(d==506||d==507)?4:d==508?5:99;return NOVATEK_AUX12_STAGE>=stage;}
static inline void ntaux_regions(lt48_s8253_resources*r){
 r->regions=r->definition==508?3:2;
 r->region_length[0]=r->definition==508?65536:r->capacity-128;
 r->region_length[1]=r->definition==508?65536:128;
 r->region_length[2]=r->definition==508?256:0;
 r->region_bank[0]=r->definition==508?65536:0;
 r->region_bank[1]=r->definition==508?0:r->region_length[0];
 r->region_bank[2]=131072;
 r->flash=r->definition==508?131072:r->region_length[0];
}

#endif
