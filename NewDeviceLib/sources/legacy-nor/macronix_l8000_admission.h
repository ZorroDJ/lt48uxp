/* Authored exact catalogue admission; source resources are separately hash checked. */
#ifndef LT48_MACRONIX_L8000_ADMISSION_H
#define LT48_MACRONIX_L8000_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int macronix_l8000(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2005 || manufacturer!=0xc2 || !vendor || strcmp(vendor,"Macronix") || !name)return 0;
 static const struct {unsigned def,id;const char *name;} rows[]={
 {0x33,0x82,"MX29L8000B *40TS"},{0x34,0x83,"MX29L8000T *40TS"}};
 for(size_t i=0;i<sizeof(rows)/sizeof(*rows);i++)
  if(def==rows[i].def && id==rows[i].id && length==strlen(rows[i].name) && !memcmp(name,rows[i].name,length))return 1;
 return 0;
}
#endif
