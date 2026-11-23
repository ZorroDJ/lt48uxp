#ifndef LT48_SHARP_S3_ADMISSION_H
#define LT48_SHARP_S3_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int sharp_s3(unsigned cls,unsigned def,unsigned man,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 const char *part="LH28F320S3 @80 uBGA";
 return cls==2007&&def==0x82&&man==0xb0&&id==0xd4&&vendor&&!strcmp(vendor,"SHARP")&&name&&length==strlen(part)&&!memcmp(name,part,length);
}
#endif
