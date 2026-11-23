#ifndef LT48_PIC18_LEGACY_APPLET_RECOVERY_H
#define LT48_PIC18_LEGACY_APPLET_RECOVERY_H
#include <labtool48/labtool48.h>
#include <string.h>

/* BYO original payload; no original firmware is embedded here. This exact
 * Program handler has a ten-count DJNZ R7 P10 loop after lowering PGC.
 * At 24 MHz / twelve clocks per machine cycle, 200 iterations give 200 us.
 * Only the immediate changes: addresses, stack use and final registers do not.
 * Caller must separately bind an admitted manufacturer/profile policy. */
static inline lt48_status lt48_pic18_patch_legacy_recovery(
    const unsigned char *original,size_t size,uint16_t base,
    unsigned char *output,size_t capacity) {
    static const unsigned char expected[32]={0x1d,0xb5,0xe0,0x14,0x7f,0x0a,0x15,0x44,0x0b,0x19,0x62,0xaa,0x52,0x1f,0x45,0xe9,0xee,0x1f,0xb4,0xe1,0x3e,0x3e,0x42,0x03,0x53,0x4f,0x12,0xf4,0x6d,0x0a,0x23,0xca};
    if(!original||!output)return LT48_INVALID_ARGUMENT;
    if(capacity<size)return LT48_BUFFER_TOO_SMALL;
    if(size!=1614||base!=0x8000)return LT48_SAFETY_REJECTED;
    unsigned char actual[32];lt48_sha256(original,size,actual);
    if(memcmp(actual,expected,32)||memcmp(original+696,"\xf0\x7f\x0a\xdf\xfe",5))
        return LT48_SAFETY_REJECTED;
    memcpy(output,original,size);output[698]=200;
    return LT48_OK;
}
#endif
