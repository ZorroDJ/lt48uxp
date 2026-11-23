/* Exact identity for the authored automatic-program correction. */
#ifndef LT48_MX28F1000_AUTO_ADMISSION_H
#define LT48_MX28F1000_AUTO_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int mx28f1000_auto(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 return cls==2007 && def==0x90 && manufacturer==0xc2 && id==0x11 &&
  vendor && !strcmp(vendor,"Macronix") && name && length==9 && !memcmp(name,"MX28F1000",9);
}
#endif
