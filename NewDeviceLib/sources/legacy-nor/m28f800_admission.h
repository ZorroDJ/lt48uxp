/* Authored exact aliases; both names bind the same original procedure. */
#ifndef LT48_M28F800_ADMISSION_H
#define LT48_M28F800_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int m28f800(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2005 || manufacturer!=0x1c || !vendor ||
    (strcmp(vendor,"Motorola")&&strcmp(vendor,"Freescale")) || !name)return 0;
 const char *expected=def==0x4c && id==0xef?"M28F800A2-T *48TS":
   def==0x4d && id==0xdf?"M28F800A2-B *48TS":NULL;
 return expected && length==strlen(expected) && !memcmp(name,expected,length);
}
#endif
