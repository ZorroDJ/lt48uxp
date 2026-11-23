/* Exact MX28FP source identities. Package variants require separate audit. */
#ifndef LT48_MX28P2_H
#define LT48_MX28P2_H
#include <stddef.h>
#include <string.h>
static int mx28p2(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2007 || manufacturer!=0xc2 || !vendor || strcmp(vendor,"Macronix") || !name)return 0;
 if(def==0x92 && id==0x3c && length==11 && !memcmp(name,"MX28F2000TP",11))return 1;
 if(length!=10)return 0;
 return (def==0x91 && id==0x1a && !memcmp(name,"MX28F1000P",10)) ||
        (def==0x92 && id==0x2a && !memcmp(name,"MX28F2000P",10));
}
#endif
