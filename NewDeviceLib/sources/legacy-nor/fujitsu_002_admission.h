#ifndef LT48_FUJITSU_002_ADMISSION_H
#define LT48_FUJITSU_002_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int fujitsu_002(unsigned cls,unsigned def,unsigned man,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 const char *key=def==3&&id==0x5d?"MBM29F002SB/E *40TS":def==2&&id==0xdc?"MBM29F002ST/E *40TS":NULL;
 return cls==2007&&man==4&&key&&vendor&&!strcmp(vendor,"Fujitsu")&&name&&
  length==strlen(key)&&!memcmp(name,key,length);
}
#endif
