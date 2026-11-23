/* Exact source-audited identity; byte49 loader verifies resource digests. */
#ifndef LT48_MACRONIX_LV040_H
#define LT48_MACRONIX_LV040_H
#include <stddef.h>
#include <string.h>
static int macronix_lv040(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 return cls==2007 && def==0xa && manufacturer==0xc2 && id==0x4f &&
  vendor && !strcmp(vendor,"Macronix") && name && length==9 && !memcmp(name,"MX29LV040",9);
}
#endif
