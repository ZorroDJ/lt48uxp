#ifndef LT48_PIC18_EEPROM_TIMEOUT_H
#define LT48_PIC18_EEPROM_TIMEOUT_H
#include "pic18_single_panel_setup.h"
/* Compose the qualified panel/recovery transform with a timeout-only status
 * stub. Exact original SHA guards are inherited. The polling timeout LJMP
 * redirects to appended authored code: report D400=2,restore DPTR=F000,
 * then execute the original WREN-disable/return path. No stack is used.
 * Full runtime loading/transport qualification remains separate. */
static inline lt48_status lt48_pic18_patch_f6_program(const unsigned char *original,size_t size,
 uint16_t address,unsigned char *output,size_t capacity,size_t *written){
 if(written)*written=0;
 if(!original||!output||!written||original==output)return LT48_INVALID_ARGUMENT;
 if(address!=0x8000||(size!=2118&&size!=2004))return LT48_SAFETY_REJECTED;
 if(capacity<size+12)return LT48_BUFFER_TOO_SMALL;
 size_t site=size==2118?0x816u:0x7a5u;
 unsigned resume=size==2118?0x881cu:0x87aau;
 if(memcmp(original+site-3,"\x30\x8d\x03\x02",4)||
    original[site+1]!=(unsigned char)(resume>>8)||original[site+2]!=(unsigned char)resume)
  return LT48_SAFETY_REJECTED;
 lt48_status status=lt48_pic18_patch_panel_setup(original,size,address,output,capacity);
 if(status!=LT48_OK)return status;
 unsigned target=address+(unsigned)size;
 output[site]=0x02;output[site+1]=(unsigned char)(target>>8);output[site+2]=(unsigned char)target;
 const unsigned char stub[]={0x90,0xd4,0x00,0x74,0x02,0xf0,0x90,0xf0,0x00,0x02,(unsigned char)(resume>>8),(unsigned char)resume};
 memcpy(output+size,stub,sizeof stub);*written=size+sizeof stub;return LT48_OK;
}
#endif
