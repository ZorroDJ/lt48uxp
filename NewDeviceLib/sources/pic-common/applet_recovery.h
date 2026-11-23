#ifndef LT48_PIC18_APPLET_RECOVERY_H
#define LT48_PIC18_APPLET_RECOVERY_H
#include <labtool48/labtool48.h>
#include <string.h>

/* BYO original payload; no original firmware is embedded here. This exact
 * Program handler has a ten-count DJNZ R7 P10 loop after lowering PGC.
 * At 24 MHz / twelve clocks per machine cycle, 200 iterations give 200 us.
 * Only the immediate changes: addresses, stack use and final registers do not.
 * Caller must separately bind an admitted manufacturer/profile policy. */
static inline lt48_status lt48_pic18_patch_recovery(
    const unsigned char *original,size_t size,uint16_t base,
    unsigned char *output,size_t capacity) {
    static const unsigned char expected[32]={
        0x41,0x5e,0xba,0xd8,0x53,0xba,0x49,0xce,0xcf,0xd3,0x09,0xab,0x83,0x99,0xf4,0x99,
        0x1d,0x22,0xf8,0xc8,0xdb,0x1c,0x09,0xd2,0xb5,0xcc,0x64,0x9b,0x6f,0x29,0x91,0x78};
    if(!original||!output)return LT48_INVALID_ARGUMENT;
    if(capacity<size)return LT48_BUFFER_TOO_SMALL;
    if(size!=1731||base!=0x8000)return LT48_SAFETY_REJECTED;
    unsigned char actual[32];lt48_sha256(original,size,actual);
    if(memcmp(actual,expected,32)||memcmp(original+735,"\xf0\x7f\x0a\xdf\xfe",5))
        return LT48_SAFETY_REJECTED;
    memcpy(output,original,size);output[737]=200;
    return LT48_OK;
}
#endif
