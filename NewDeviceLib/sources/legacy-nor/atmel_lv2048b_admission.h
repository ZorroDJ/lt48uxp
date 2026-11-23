#include <stddef.h>
#include <string.h>
static int atmel_lv2048b(unsigned cls,unsigned def,unsigned man,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 static const char key[]="AT49LV2048B *48TS";
 return cls==2003&&def==0xfe&&man==0x1f&&id==0x88&&vendor&&
  !strcmp(vendor,"Atmel")&&length==sizeof(key)-1&&!memcmp(name,key,length);
}
