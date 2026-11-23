#ifndef LT48_PIC_REMAINING14_MODERN_WIRE_H
#define LT48_PIC_REMAINING14_MODERN_WIRE_H
#include "classic14_protocol.h"
/* Primary100us adaptive14-bit OTP algorithm. Original fixed-helper families
 * put retries/margin in role8; reload-helper families put them in role13.
 * Both are represented once here; their source calling/cursor audit remains
 * explicit. No source status alone can establish successful programming. */
static inline int rm14_modern_wire_valid(const lt48_pic_classic_wire *w){return w&&w->transfer&&w->delay;}
static inline lt48_status rm14_modern_read_word(const lt48_pic_classic_wire *w,uint16_t *value) {
 if(!rm14_modern_wire_valid(w)||!value)return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_pic_classic_command(w,4);
 return st==LT48_OK?lt48_pic_classic_frame(w,1,value):st;
}
typedef lt48_status (*rm14_modern_pulse)(void *);
static inline lt48_status rm14_modern_cycle_with_pulse(const lt48_pic_classic_wire *w,uint16_t value,rm14_modern_pulse pulse,void *context) {
 if(!rm14_modern_wire_valid(w)||value>0x3fff)return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_pic_classic_load(w,2,value);
 if(st==LT48_OK&&pulse)return pulse(context);
 if(st==LT48_OK)st=lt48_pic_classic_command(w,8);
 if(st==LT48_OK)st=w->delay(w->context,100);
 if(st==LT48_OK)st=lt48_pic_classic_command(w,14);
 return st;
}
static inline lt48_status rm14_modern_program_word_with_pulse(const lt48_pic_classic_wire *w,uint16_t value,unsigned mask,rm14_modern_pulse pulse,void *context) {
 if(!mask||mask>0x3fff||value>0x3fff)return LT48_INVALID_ARGUMENT;
 unsigned attempts=0;int matched=0;
 for(;attempts<25;attempts++) {
  uint16_t actual=0;lt48_status st=rm14_modern_cycle_with_pulse(w,value,pulse,context);
  if(st==LT48_OK)st=rm14_modern_read_word(w,&actual);
  if(st!=LT48_OK)return st;
  if(!((actual^value)&mask)){attempts++;matched=1;break;}
 }
 if(!matched)return LT48_DEVICE_MISMATCH;
 for(unsigned i=0;i<3*attempts;i++){lt48_status st=rm14_modern_cycle_with_pulse(w,value,pulse,context);if(st!=LT48_OK)return st;}
 return LT48_OK;
}
static inline lt48_status rm14_modern_program_configuration_with_pulse(const lt48_pic_classic_wire *w,uint16_t target,unsigned mask,rm14_modern_pulse pulse,void *context) {
 if(!mask||mask>0x3fff)return LT48_INVALID_ARGUMENT;
 for(unsigned i=0;i<100;i++){lt48_status st=rm14_modern_cycle_with_pulse(w,target,pulse,context);if(st!=LT48_OK)return st;}
 uint16_t actual=0;lt48_status st=rm14_modern_read_word(w,&actual);
 return st==LT48_OK&&((actual^target)&mask)?LT48_DEVICE_MISMATCH:st;
}
static inline lt48_status rm14_modern_program_word(const lt48_pic_classic_wire *w,uint16_t value,unsigned mask) {
 return rm14_modern_program_word_with_pulse(w,value,mask,NULL,NULL);
}
static inline lt48_status rm14_modern_program_configuration(const lt48_pic_classic_wire *w,uint16_t target,unsigned mask) {
 return rm14_modern_program_configuration_with_pulse(w,target,mask,NULL,NULL);
}
#endif
