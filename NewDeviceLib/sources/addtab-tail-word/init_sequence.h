/* Private trial: preload FPGA word data with target RESET asserted.
 * Not admitted for public device operations. Exact two shared INIT shapes, bound by private source profiles. */
#ifndef LT48_TAIL_WORD_INIT_SEQUENCE_H
#define LT48_TAIL_WORD_INIT_SEQUENCE_H
static unsigned char *tail_word_init_wave(unsigned definition,unsigned operation,
    const unsigned char *code,size_t size,size_t *out_size) {
    static const unsigned char load[]={0x3b,0xc1,0x3b,0xff,0x3b,0xff};
    size_t expected_size=(definition==0xc2 || definition==0xe3 || definition==0xcf ||
        definition==0xd0 || definition==0xd1 || definition==0xbd || definition==0xbe ||
        definition==0xc6 || definition==0xc7)?103:
        (definition==0x13b || definition==0x13c || definition==0xac)?93:0;
    if(!expected_size || operation!=2 || size!=expected_size ||
       code[24]!=0x11 || code[25]!=5 || code[26]!=0 ||
       memcmp(code+size-sizeof(load),load,sizeof(load)))return NULL;
    unsigned char *out=malloc(size);if(!out)return NULL;
    memcpy(out,code,27);memcpy(out+27,load,sizeof(load));
    memcpy(out+27+sizeof(load),code+27,size-27-sizeof(load));
    *out_size=size;return out;
}
#endif
