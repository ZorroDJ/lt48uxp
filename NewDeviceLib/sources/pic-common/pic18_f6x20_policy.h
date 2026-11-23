#ifndef LT48_PIC18_F6X20_POLICY_H
#define LT48_PIC18_F6X20_POLICY_H
#include <stdint.h>
/* Exact original D/M/P contexts, independently checked against DS39583B.
 * Bulk80h preserves application configuration; only protection resets. */
static inline const unsigned char *lt48_pic18_f6x20_erase_mask(uint32_t d,uint16_t f,uint16_t p) {
 static const unsigned char small[]={0,0,0,0,0,0,0,0,15,192,15,224,15,64};
 static const unsigned char large[]={0,0,0,0,0,0,0,0,255,192,255,224,255,64};
 if(p!=14u)return 0;
 if((d==36u||d==37u)&&f==26u)return small;
 if(d==38u&&f==27u)return large;
 return 0;
}
static inline int lt48_pic18_f6x20_configuration_binding(uint32_t d,uint16_t f,uint16_t p) {
 return lt48_pic18_f6x20_erase_mask(d,f,p)!=0;
}
#endif
