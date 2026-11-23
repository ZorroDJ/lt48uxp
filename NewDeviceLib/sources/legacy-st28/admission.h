/* Exact ST M28F201 identity; authored zero-preconditioning is mandatory. */
#ifndef LT48_ST28_H
#define LT48_ST28_H
#include <stddef.h>
#include <string.h>
static int st28(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 return cls==2007 && def==0x89 && manufacturer==0x20 && id==0xf4 && vendor &&
  !strcmp(vendor,"ST") && name && length==7 && !memcmp(name,"M28F201",7);
}
#endif
