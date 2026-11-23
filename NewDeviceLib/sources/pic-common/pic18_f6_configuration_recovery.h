#ifndef LT48_DRAFT_PIC18_F6_CONFIGURATION_RECOVERY_H
#define LT48_DRAFT_PIC18_F6_CONFIGURATION_RECOVERY_H
#include <labtool48/labtool48.h>
#include <string.h>

/* Exact DS39643B BYO writer, reusing the established two-trampoline
 * recovery scheme. Original 873-byte SHA and both instruction sites guard
 * applicability; no layout-only admission. Guarded BYO configuration writer. Two original post-PGC-low register
 * selections redirect to appended MOV A,#D4 / MOVX @DPTR,A, 400 NOPs and
 * LJMP back. No CALL/PUSH, instruction displacement or register changes.
 * The 9800..A000 applet slot bounds the complete overlay. */
static inline lt48_status lt48_pic18_patch_f6_configuration_recovery(
    const unsigned char *original,size_t size,uint16_t base,
    unsigned char *output,size_t capacity,size_t *output_size) {
    static const unsigned char expected[32]={0x2d,0x2f,0xf9,0x67,0xed,0xc6,0x12,0xa3,0xc1,0x8d,0x33,0x9d,0x03,0xc6,0x7a,0x3b,0x7e,0x02,0x15,0xc8,0x23,0xed,0xe8,0xb0,0x37,0x72,0x96,0xa8,0x45,0x8f,0x39,0x5f};
    static const size_t sites[2]={582,807};
    if(output_size)*output_size=0;
    if(!original||!output||!output_size||original==output)return LT48_INVALID_ARGUMENT;
    if(size!=873||base!=0x9800)return LT48_SAFETY_REJECTED;
    if(capacity<1702)return LT48_BUFFER_TOO_SMALL;
    unsigned char actual[32];lt48_sha256(original,size,actual);
    if(memcmp(actual,expected,32))return LT48_SAFETY_REJECTED;
    for(unsigned i=0;i<2;i++)if(memcmp(original+sites[i],"\x74\xd4\xf0",3)||original[sites[i]-1]!=0xf0)
        return LT48_SAFETY_REJECTED;
    memset(output,0,1702);memcpy(output,original,size);
    size_t used=size;
    for(unsigned i=0;i<2;i++) {
        used=(used+15u)&~(size_t)15u;
        uint16_t target=(uint16_t)(base+used),resume=(uint16_t)(base+sites[i]+3);
        output[sites[i]]=0x02;output[sites[i]+1]=(unsigned char)(target>>8);output[sites[i]+2]=(unsigned char)target;
        memcpy(output+used,original+sites[i],3);used+=3;
        used+=400; /* 400 classic machine cycles = 200 us at 24 MHz. */
        output[used++]=0x02;output[used++]=(unsigned char)(resume>>8);output[used++]=(unsigned char)resume;
    }
    if(used!=1702||base+used>0xa000)return LT48_SAFETY_REJECTED;
    *output_size=used;return LT48_OK;
}
#endif
