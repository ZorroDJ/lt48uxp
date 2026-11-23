#ifndef LT48_PIC18_ERASE_KEY_RECOVERY_H
#define LT48_PIC18_ERASE_KEY_RECOVERY_H
#include <labtool48/labtool48.h>
#include <string.h>

/* BYO legacy erase writer, changed only for independently bound profiles
 * whose manufacturer specification requests 3F8F full-chip erase.
 * PIC18F1230/1330 require 0F87 and must retain the original payload.
 * This builder establishes byte identity only; source/FPGA/effect and
 * profile-membership qualification remain separate obligations. */
static inline lt48_status lt48_pic18_patch_erase_key(
    const unsigned char *original,size_t size,uint16_t base,
    unsigned char *output,size_t capacity) {
    static const unsigned char expected[32]={0x3d,0xa8,0xda,0x80,0xb4,0xbf,0xcb,0x83,0x79,0x07,0x6d,0xb0,0x23,0xad,0x55,0xda,0xae,0x12,0x49,0x74,0xd5,0xe9,0x19,0x57,0x0d,0xdd,0xbf,0x97,0x9a,0x0c,0x4b,0x1d};
    static const size_t offsets[]={410,419,683,692};
    if(!original||!output)return LT48_INVALID_ARGUMENT;
    if(size!=800||base!=0x9000)return LT48_SAFETY_REJECTED;
    if(capacity<size)return LT48_BUFFER_TOO_SMALL;
    unsigned char actual[32];lt48_sha256(original,size,actual);
    if(memcmp(actual,expected,32))return LT48_SAFETY_REJECTED;
    for(unsigned i=0;i<4;i++) {
        size_t at=offsets[i];unsigned value=i<2?0x0f:0x87;
        if(original[at-1]!=0x74||original[at]!=value||original[at+1]!=0xf0)
            return LT48_SAFETY_REJECTED;
    }
    memcpy(output,original,size);
    for(unsigned i=0;i<4;i++)output[offsets[i]]=i<2?0x3f:0x8f;
    return LT48_OK;
}
#endif
