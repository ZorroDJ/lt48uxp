/* Exact catalogue admission; resource digests and profiles are checked by
 * the existing byte49 owner. Standalone protection modification is deferred. */
#ifndef LT48_MACRONIX_BOOTBLOCK4_H
#define LT48_MACRONIX_BOOTBLOCK4_H
#include <stddef.h>
#include <string.h>
static int macronix_bootblock4(unsigned cls,unsigned def,unsigned manufacturer,
 unsigned id,const char *vendor,const unsigned char *name,size_t length) {
 static const struct {unsigned def,id;const char *name;} parts[]={
  {0x2f,0x19,"MX29F001B"},{0x2f,0x18,"MX29F001T"},
  {0x31,0x46,"MX29F004B"},{0x31,0x45,"MX29F004T"}};
 if(cls!=2007 || manufacturer!=0xc2 || !vendor || strcmp(vendor,"Macronix") || !name)return 0;
 for(size_t n=0;n<sizeof(parts)/sizeof(*parts);n++)
  if(def==parts[n].def && id==parts[n].id && length==strlen(parts[n].name) && !memcmp(name,parts[n].name,length))return 1;
 return 0;
}
#endif
