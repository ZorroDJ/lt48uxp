#ifndef LT48_PIC18_X220_PC_RELOCATION_H
#define LT48_PIC18_X220_PC_RELOCATION_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Authored, temporary RAM overlay for the exact BYO X220 configuration applet.
 * Inject DS39592D's GOTO0x100000 before the original initialization. Preserve
 * registers, stack and original data/timing body. No source bytes distributed. */
static inline lt48_status lt48_pic18_x220_pc_relocation(
    const unsigned char *original,size_t size,uint16_t base,
    unsigned char *output,size_t capacity,size_t *output_size) {
    static const unsigned char expected[32]={
        0xc6,0xa2,0x26,0x1b,0x03,0x67,0xf2,0x6b,0x87,0xca,0xa9,0xe4,0x13,0x8b,0x76,0x8e,
        0x7a,0x93,0x95,0xaf,0x7b,0xd6,0x59,0xda,0x66,0x6f,0x81,0xdc,0x6d,0xed,0x9f,0x65};
    const size_t site=0xbd,required=873;
    if(!original||!output||!output_size)return LT48_INVALID_ARGUMENT;
    *output_size=0;
    if(size!=789||base!=0x9800)return LT48_SAFETY_REJECTED;
    if(capacity<required)return LT48_BUFFER_TOO_SMALL;
    unsigned char actual[32];lt48_sha256(original,size,actual);
    if(memcmp(actual,expected,32)||memcmp(original+site,"\x74\xd4\xf0",3))return LT48_SAFETY_REJECTED;
    memcpy(output,original,size);size_t used=size;
    uint16_t target=(uint16_t)(base+used),resume=(uint16_t)(base+site+3);
    output[site]=2;output[site+1]=(unsigned char)(target>>8);output[site+2]=(unsigned char)target;
    const unsigned words[2]={0xef00,0xf800};
    for(unsigned i=0;i<2;i++) {
        const unsigned char bytes[10]={0xd4,4,0xc0,0,0xd4,8,0xc0,(unsigned char)words[i],0xc0,(unsigned char)(words[i]>>8)};
        for(unsigned j=0;j<10;j++) {
            output[used++]=0x74;output[used++]=bytes[j];output[used++]=0xf0;
            if(j==3||j==7||j==9){output[used++]=0x30;output[used++]=0x94;output[used++]=0xfd;}
        }
    }
    output[used++]=0x74;output[used++]=0xd4;output[used++]=0xf0;
    output[used++]=2;output[used++]=(unsigned char)(resume>>8);output[used++]=(unsigned char)resume;
    if(used!=required||base+used>0xa000)return LT48_SAFETY_REJECTED;
    *output_size=used;return LT48_OK;
}
#endif
