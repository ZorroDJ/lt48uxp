/* Exact source-audited identity; byte49 loader verifies resource digests. */
#ifndef LT48_MX080TS_H
#define LT48_MX080TS_H
#include <stddef.h>
#include <string.h>
static int mx080ts(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 return cls==2007 && def==0x28 && manufacturer==0xc2 && id==0xd5 &&
  vendor && !strcmp(vendor,"Macronix") && name && length==14 && !memcmp(name,"MX29F080 *40TS",14);
}
#endif
