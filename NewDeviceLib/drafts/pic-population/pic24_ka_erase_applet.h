#ifndef LT48_DRAFT_PIC24_KA_ERASE_APPLET_H
#define LT48_DRAFT_PIC24_KA_ERASE_APPLET_H
#include <labtool48/labtool48.h>
#include <string.h>
/* BYO528-byte applet, relocated nowhere. DS30009919D Tables3-4/3-5 share
 * the same self-timed erase sequence except MOV #4064/#4050,W10. The authored
 * EEPROM variant changes only that immediate. Original bytes never live in
 * this header. The caller must run BOTH variants for whole-device erase and
 * check each completion plus independent code/EEPROM/configuration readback.
 * No electrical timing, runtime/GUI admission or hardware access is implied. */
static inline lt48_status lt48_pic24_ka_erase_applet(const unsigned char *original,size_t size,
 uint16_t base,uint16_t nvmcon,unsigned char *output,size_t capacity,size_t *written){
 static const unsigned char expected[32]={0xcd,0xbb,0xa6,0xa9,0xff,0x2c,0x73,0xa5,0xbc,0xc7,0x3b,0x9b,0x06,0x59,0xa0,0xb3,0xc8,0x17,0xb0,0x87,0x12,0x54,0x71,0x77,0x88,0x04,0xcc,0x4b,0x9d,0x00,0xb1,0x7b};
 static const unsigned char instruction[]={0x79,0x4a,0x7a,0x06,0x7b,0x24};
 if(!written)return LT48_INVALID_ARGUMENT;
 *written=0;
 if(!original||!output)return LT48_INVALID_ARGUMENT;
 if(size!=528||base!=0x5800||(nvmcon!=0x4064&&nvmcon!=0x4050))return LT48_SAFETY_REJECTED;
 unsigned char hash[32];lt48_sha256(original,size,hash);
 if(memcmp(hash,expected,32))return LT48_SAFETY_REJECTED;
 if(capacity<size)return LT48_BUFFER_TOO_SMALL;
 size_t found=SIZE_MAX;unsigned count=0;
 for(size_t i=0;i+sizeof(instruction)<=size;i++)if(!memcmp(original+i,instruction,sizeof(instruction))){found=i;count++;}
 if(count!=1)return LT48_SAFETY_REJECTED;
 unsigned char staged[528];memcpy(staged,original,size);
 if(nvmcon==0x4050){staged[found+1]=0x0a;staged[found+3]=0x05;}
 memcpy(output,staged,size);*written=size;return LT48_OK;
}
#endif
