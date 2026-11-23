#ifndef LT48_PIC24_CONFIGURATION_RULES_H
#define LT48_PIC24_CONFIGURATION_RULES_H
#include <stdint.h>
/* Explicit mandatory states in DS39768D table3-6, DS30009934C section3.7,
 * and DS30000510F table3-7. Image/editor order is CW2,CW1 or CW4..CW1.
 * These rules do not describe all option meanings or unimplemented bits. */
enum lt48_pic24_configuration_family {
 LT48_PIC24_GA0,LT48_PIC24_GA1_GB0,LT48_PIC24_GA2,LT48_PIC24_GB2
};
typedef struct {unsigned words;uint16_t force_zero[4],force_one[4];} lt48_pic24_configuration_rules;
static inline int lt48_pic24_configuration_rules_prepare(unsigned family,
 lt48_pic24_configuration_rules *out){
 if(!out || family>LT48_PIC24_GB2)return 0;
 lt48_pic24_configuration_rules r={family==LT48_PIC24_GA0?2:4,{0},{0}};
 r.force_zero[r.words-1]=0x8000; /* CW1<15> must be zero in all four groups. */
 if(family==LT48_PIC24_GA2 || family==LT48_PIC24_GB2){
  r.force_zero[2]=family==LT48_PIC24_GA2?0x4800:0x4000;
  r.force_one[2]=0x0004;r.force_one[1]=0x0080;r.force_one[0]=0x0200;
 }
 *out=r;return 1;
}
static inline int lt48_pic24_configuration_normalize(
 const lt48_pic24_configuration_rules *r,const uint32_t *input,unsigned count,uint16_t *out){
 if(!r || !input || !out || count!=r->words || (count!=2 && count!=4))return 0;
 uint16_t values[4];
 for(unsigned i=0;i<count;i++){
  if(input[i]>65535 || (r->force_zero[i]&r->force_one[i]))return 0;
  values[i]=(uint16_t)((input[i]&~r->force_zero[i])|r->force_one[i]);
 }
 for(unsigned i=0;i<count;i++)out[i]=values[i];
 return 1;
}
#endif
