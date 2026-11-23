/* Exact SST PH28SF040 source identity. */
#ifndef LT48_SST1_H
#define LT48_SST1_H
#include <stddef.h>
#include <string.h>
static int sst1(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 return cls==2007 && def==0x32 && manufacturer==0xbf && id==4 && vendor &&
  !strcmp(vendor,"SST") && name && length==9 && !memcmp(name,"PH28SF040",9);
}
#endif
