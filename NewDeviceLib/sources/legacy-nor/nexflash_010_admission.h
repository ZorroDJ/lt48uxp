/* Exact source-audited identity; byte49 loader verifies resource digests. */
#ifndef LT48_NEXFLASH_010_H
#define LT48_NEXFLASH_010_H
#include <stddef.h>
#include <string.h>
static int nexflash_010(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 return cls==2007 && def==0x19 && manufacturer==1 && id==0x20 &&
  vendor && !strcmp(vendor,"NexFlash") && name && length==8 && !memcmp(name,"NX29F010",8);
}
#endif
