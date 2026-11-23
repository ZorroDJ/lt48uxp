/* Authored exact catalogue admission; source resources are separately hash checked. */
#ifndef LT48_MACRONIX_F1601_ADMISSION_H
#define LT48_MACRONIX_F1601_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int macronix_f1601(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2005 || manufacturer!=0xc2 || !vendor || strcmp(vendor,"Macronix") || !name)return 0;
 static const struct {unsigned def,id;const char *name;} rows[]={
 {0x50,0xf3,"MX29F1601 *44PS"}};
 for(size_t i=0;i<sizeof(rows)/sizeof(*rows);i++)
  if(def==rows[i].def && id==rows[i].id && length==strlen(rows[i].name) && !memcmp(name,rows[i].name,length))return 1;
 return 0;
}
#endif
