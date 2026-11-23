#ifndef LT48_PIC17_CONFIGURATION_H
#define LT48_PIC17_CONFIGURATION_H
#include <labtool48/labtool48.h>
/* Physical configuration word at FE00, per DS30139I Table4-2/4-3 and
 * DS30274B. Source fields6/7/8 encode FOSC/WDT/processor low two bits.
 * Processor0=protected,1=extended MCU,2=MCU,3=microprocessor. PM2 exists
 * after C42 and must agree with the selected legal processor mode. */
typedef struct {unsigned oscillator,watchdog,processor,brownout;} lt48_pic17_configuration;
static inline unsigned p17_configuration_mask(unsigned family) {
 return family<27||family>39?0:family<=28?0x5f:family<=36?0x805f:0xc05f;
}
static inline unsigned p17_configuration_protection(unsigned family) {
 return family<=28?0x50:0x8050;
}
static inline lt48_status p17_configuration_encode(unsigned family,const lt48_pic17_configuration *c,uint16_t *out) {
 unsigned mask=p17_configuration_mask(family);
 if(!mask||!c||!out||c->oscillator>3||c->watchdog>3||c->processor>3||c->brownout>1||
    (family<=36&&c->brownout!=1))return LT48_INVALID_ARGUMENT;
 unsigned word=(~mask&65535)|c->oscillator|(c->watchdog<<2)|((c->processor&1)<<4)|((c->processor&2)<<5);
 if(family>=29&&c->processor)word|=0x8000;
 if(family>=37&&c->brownout)word|=0x4000;
 *out=(uint16_t)word;return LT48_OK;
}
static inline lt48_status p17_configuration_decode(unsigned family,uint16_t word,lt48_pic17_configuration *out) {
 unsigned mask=p17_configuration_mask(family);
 if(!mask||!out)return LT48_INVALID_ARGUMENT;
 if((word|mask)!=65535)return LT48_SAFETY_REJECTED;
 unsigned processor=((word>>4)&1)|((word>>5)&2);
 if(family>=29&&!!(word&0x8000)!=!!processor)return LT48_SAFETY_REJECTED;
 *out=(lt48_pic17_configuration){word&3,(word>>2)&3,processor,family>=37?!!(word&0x4000):1};
 return LT48_OK;
}
/* Dataman's older PIC17 image stores only low CFG bits; its Pre/Post
 * converter forces high byte FF. Legal source processor0 implies PM2=0,
 * even though that bit is not stored as a physical mirror in the image. */
static inline lt48_status p17_configuration_image_decode(unsigned family,uint16_t word,lt48_pic17_configuration *out) {
 if(family>=37&&family<=39){unsigned mask=p17_configuration_mask(family);return p17_configuration_decode(family,(uint16_t)((word&mask)|(~mask&65535)),out);}
 if(family<27||family>36||!out)return LT48_UNSUPPORTED;
 if((word&0xffa0)!=0xffa0)return LT48_SAFETY_REJECTED;
 unsigned pm=((word>>4)&1)|((word>>5)&2);
 *out=(lt48_pic17_configuration){word&3,(word>>2)&3,pm,1};return LT48_OK;
}
static inline lt48_status p17_configuration_image_word(unsigned family,uint16_t physical,uint16_t *out) {
 lt48_pic17_configuration c;lt48_status st=p17_configuration_decode(family,physical,&c);
 if(st!=LT48_OK)return st;
 if(!out)return LT48_INVALID_ARGUMENT;
 if(family>=37&&family<=39){*out=physical;return LT48_OK;}
 if(family>36)return LT48_UNSUPPORTED;
 *out=physical|0xff00;return LT48_OK;
}
static inline lt48_status p17_configuration_image_validate(unsigned family,unsigned words,
 const unsigned char *image,size_t size) {
 if(!image||!words||words>16384||size!=(family>=37&&family<=39?0x1fc12u:0x1fc02u))return LT48_INVALID_ARGUMENT;
 lt48_pic17_configuration c;lt48_status st=p17_configuration_image_decode(family,
  (uint16_t)(image[0x1fc00]|(unsigned)image[0x1fc01]<<8),&c);
 if(st!=LT48_OK)return st;
 for(size_t i=2*words;i<0x1fc00;i++)if(image[i]!=255)return LT48_SAFETY_REJECTED;
 for(size_t i=0x1fc02;i<size;i++)if(image[i]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
#endif
