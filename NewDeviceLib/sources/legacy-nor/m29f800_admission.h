/* Authored exact bindings; original source resources remain bring-your-own. */
#ifndef LT48_M29F800_ADMISSION_H
#define LT48_M29F800_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int m29f800(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2005 || manufacturer!=0x1c || !vendor ||
    (strcmp(vendor,"Motorola")&&strcmp(vendor,"Freescale")) || !name)return 0;
 const char *expected=def==0x4a && id==0x5d?"M29F800A3-T *48TS":
   def==0x4b && id==0x5e?"M29F800A3-B *48TS":NULL;
 return expected && length==strlen(expected) && !memcmp(name,expected,length);
}
#endif
