#ifndef LT48_PIC17_PM2_H
#define LT48_PIC17_PM2_H
#include <labtool48/labtool48.h>
#include <string.h>

/* Explicit authored correction, DS30139I Table4-1: PM2 is programmed at
 * FE0F, not FE08. Exact original W25 first programs PM0/PM1, then advances
 * one address before its third program-word call. Add seven address cycles
 * (three RA1 pulses each) before that call. Source bytes stay immutable.
 * Applies only to the hash-pinned old PIC17 PM2 callback; no generic patch. */
static inline lt48_status lt48_pic17_pm2_compile(const lt48_wave_code *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t count,
 lt48_compiled_wave **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars)return LT48_INVALID_ARGUMENT;
 static const unsigned char expected[32]={
  0x19,0x09,0x47,0x01,0xe8,0xc5,0x3d,0x9d,0x90,0xc0,0xd1,0x81,0x6b,0x80,0x75,0xb2,
  0xb4,0xd2,0x2e,0x77,0x63,0x8c,0xd1,0x08,0x92,0x58,0x2e,0x7d,0x5c,0xe9,0xb6,0x6f};
 unsigned char hash[32];
 if(source->wave_type!=0||source->data_size!=159||!source->data)return LT48_SAFETY_REJECTED;
 lt48_sha256(source->data,source->data_size,hash);
 if(memcmp(hash,expected,32))return LT48_SAFETY_REJECTED;
 unsigned char raw[243];memcpy(raw,source->data,0x87);
 for(unsigned i=0;i<21;i++)memcpy(raw+0x87+4*i,"\x2c\x0f\x0f\x01",4);
 memcpy(raw+0x87+84,source->data+0x87,159-0x87);
 raw[0]=241;raw[1]=0;
 lt48_compact_wave *wave=NULL;lt48_status st=lt48_compact_decode(raw,sizeof raw,&wave);
 if(st==LT48_OK)st=lt48_compact_compile(wave,pins,vars,count,1,0,out);
 lt48_compact_wave_close(wave);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK&&*out){lt48_compiled_wave_close(*out);*out=NULL;}
 return st;
}
#endif
