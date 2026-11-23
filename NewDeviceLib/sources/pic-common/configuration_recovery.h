#ifndef LT48_PIC18_CONFIGURATION_RECOVERY_H
#define LT48_PIC18_CONFIGURATION_RECOVERY_H
#include <labtool48/labtool48.h>
#include <string.h>

/* Guarded BYO configuration writer. Two original post-PGC-low register
 * selections redirect to appended MOV A,#D4 / MOVX @DPTR,A, 400 NOPs and
 * LJMP back. No CALL/PUSH, instruction displacement or register changes.
 * The 9800..A000 applet slot bounds the complete overlay. */
static inline lt48_status lt48_pic18_patch_configuration_recovery(
    const unsigned char *original,size_t size,uint16_t base,
    unsigned char *output,size_t capacity,size_t *output_size) {
    static const unsigned char expected[32]={
        0x7f,0x9a,0xeb,0x21,0x0e,0x26,0xf2,0x8a,0xa1,0xad,0xc0,0x99,0x69,0x5b,0x58,0x1a,
        0xa7,0xae,0xa2,0xe0,0x9e,0x6b,0xfa,0x64,0xbc,0x81,0xda,0x29,0x5a,0xab,0x3e,0x52};
    static const size_t sites[2]={582,807};
    if(!original||!output||!output_size)return LT48_INVALID_ARGUMENT;
    *output_size=0;
    if(size!=834||base!=0x9800)return LT48_SAFETY_REJECTED;
    if(capacity<1670)return LT48_BUFFER_TOO_SMALL;
    unsigned char actual[32];lt48_sha256(original,size,actual);
    if(memcmp(actual,expected,32))return LT48_SAFETY_REJECTED;
    for(unsigned i=0;i<2;i++)if(memcmp(original+sites[i],"\x74\xd4\xf0",3)||original[sites[i]-1]!=0xf0)
        return LT48_SAFETY_REJECTED;
    memset(output,0,1670);memcpy(output,original,size);
    size_t used=size;
    for(unsigned i=0;i<2;i++) {
        used=(used+15u)&~(size_t)15u;
        uint16_t target=(uint16_t)(base+used),resume=(uint16_t)(base+sites[i]+3);
        output[sites[i]]=0x02;output[sites[i]+1]=(unsigned char)(target>>8);output[sites[i]+2]=(unsigned char)target;
        memcpy(output+used,original+sites[i],3);used+=3;
        used+=400; /* 400 classic machine cycles = 200 us at 24 MHz. */
        output[used++]=0x02;output[used++]=(unsigned char)(resume>>8);output[used++]=(unsigned char)resume;
    }
    if(used!=1670||base+used>0xa000)return LT48_SAFETY_REJECTED;
    *output_size=used;return LT48_OK;
}
#endif
