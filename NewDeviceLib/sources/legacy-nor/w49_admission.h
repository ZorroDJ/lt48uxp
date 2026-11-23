/* Authored exact catalogue bindings; vendor payloads remain bring-your-own. */
#ifndef LT48_W49_ADMISSION_H
#define LT48_W49_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int w49_dummy(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2029||manufacturer!=0xda||!vendor||strcmp(vendor,"Winbond")||!name)return 0;
 const char *expected=def==0x1e&&id==0x2f?"W49L102P *44PS":def==0x1d&&id==0x2f?"W49L102Q *40TS/W":def==0x25&&id==0x3d?"W49L401 *48TS ":NULL;
 return expected&&length==strlen(expected)&&!memcmp(name,expected,length);
}
static int w49_t16(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2004||manufacturer!=0xda||!vendor||strcmp(vendor,"Winbond")||!name)return 0;
 const char *expected=def==0xa8&&id==0xae?"W49F201T *48TS":def==0xa7&&id==0x3e?"W49L201T *48TS":NULL;
 return expected&&length==strlen(expected)&&!memcmp(name,expected,length);
}
#endif
