#ifndef LT48_DRAFT_PIC18_F6_ERASE_SELECTOR_H
#define LT48_DRAFT_PIC18_F6_ERASE_SELECTOR_H
#include <labtool48/labtool48.h>
#include <string.h>
/* DS39643B table3-1/table3-2 specify FF87 for chip erase. The source
 * 0F87 agrees with that document's inconsistent flowchart, selecting only
 * four code-block mask bits. Use the explicit full-chip table selector for
 * all four exact profiles, including six/eight-block 96/128KiB parts.
 * This is RAM-only, BYO firmware composition, never a programmer update. */
static inline lt48_status lt48_pic18_patch_f6_erase_selector(const unsigned char *original,size_t size,
 uint16_t base,unsigned char *output,size_t capacity){
 static const unsigned char expected[32]={0x3d,0xa8,0xda,0x80,0xb4,0xbf,0xcb,0x83,0x79,0x07,0x6d,0xb0,0x23,0xad,0x55,0xda,0xae,0x12,0x49,0x74,0xd5,0xe9,0x19,0x57,0x0d,0xdd,0xbf,0x97,0x9a,0x0c,0x4b,0x1d};
 if(!original||!output||original==output)return LT48_INVALID_ARGUMENT;
 if(size!=800||base!=0x9000)return LT48_SAFETY_REJECTED;
 if(capacity<size)return LT48_BUFFER_TOO_SMALL;
 unsigned char actual[32];lt48_sha256(original,size,actual);
 if(memcmp(actual,expected,32)||memcmp(original+409,"\x74\x0f\xf0\x30\x94\xfd",6)||
    memcmp(original+418,"\x74\x0f\xf0\x30\x94\xfd",6))return LT48_SAFETY_REJECTED;
 memcpy(output,original,size);output[410]=output[419]=255;return LT48_OK;
}
#endif
