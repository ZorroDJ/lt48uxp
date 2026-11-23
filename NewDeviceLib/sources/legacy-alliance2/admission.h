/* Source-identical Alliance/ROHM profiles, gated by exact catalogue identity.
 * Silicon equivalence is not hardware qualification. */
#ifndef LT48_ALLIANCE2_H
#define LT48_ALLIANCE2_H
#include <stddef.h>
#include <string.h>
static int alliance2(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2007 || def!=0x35 || manufacturer!=0x52 || id!=0xa4 || !vendor || !name || length!=8)return 0;
 return (!strcmp(vendor,"Alliance")&&!memcmp(name,"AS29F040",8)) ||
        (!strcmp(vendor,"ROHM")&&!memcmp(name,"BR29F040",8));
}
#endif
