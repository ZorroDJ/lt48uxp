/* Exact source-audited identity; byte49 loader verifies resource digests. */
#ifndef LT48_HYNIX_080_H
#define LT48_HYNIX_080_H
#include <stddef.h>
#include <string.h>
static int hynix_080(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 return cls==2007 && def==0x25 && manufacturer==0xad && id==0xd5 &&
  vendor && !strcmp(vendor,"Hyundai(Hynix)") && name && length==15 && !memcmp(name,"HY29F080T *40TS",15);
}
#endif
