/* Authored M29W128 partial-range address restoration after engineer entry. */
#ifndef LT48_MICRON27_REGION_TRANSITIONS_H
#define LT48_MICRON27_REGION_TRANSITIONS_H
static unsigned char *micron27_region_wave(unsigned definition,unsigned operation,
    const unsigned char *code,size_t size,size_t *out_size) {
    if((definition!=0xb0 && definition!=0xb1 && definition!=0xb2 && definition!=0xb3 && definition!=0xb5 && definition!=0xb6) || (operation!=5 && operation!=7 && operation!=8 && operation!=9))return NULL;
    static const unsigned char address[]={0x4d,2,0,2,0,0,0,
        0x3b,0xc4,0x3e,0,0,0x3b,0xc8,0x3e,0,1,
        0x3b,0xcc,0x3e,0,2,0x3b,0xc6,0x3e,0,3};
    static const unsigned char entry[]={0x5d,1,5,16,0x4e,14,0x3d,26,
        0x39,5,0x3b,0x88,0x39,0,0x60};
    size_t at=operation==8?6:2;
    if(size<at+sizeof(address)+sizeof(entry) ||
       memcmp(code+at,address,sizeof(address)) ||
       memcmp(code+at+sizeof(address),entry,sizeof(entry)))return NULL;
    unsigned char *out=malloc(size);if(!out)return NULL;memcpy(out,code,size);
    memcpy(out+at,entry,sizeof(entry));memcpy(out+at+sizeof(entry),address,sizeof(address));
    *out_size=size;return out;
}
#endif
