/* Authored exact catalogue bindings; source resources remain bring-your-own. */
#ifndef LT48_MX26B_ADMISSION_H
#define LT48_MX26B_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int mx26b(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2007||manufacturer!=0xc2||!vendor||strcmp(vendor,"Macronix")||!name)return 0;
 const char *expected=def==0x6a&&id==0xce?"MX26C1000B ":def==0x6b&&id==0xcf?"MX26C2000B ":def==0x6c&&id==0xc0?"MX26C4000B ":NULL;
 return expected&&length==strlen(expected)&&!memcmp(name,expected,length);
}
#endif
