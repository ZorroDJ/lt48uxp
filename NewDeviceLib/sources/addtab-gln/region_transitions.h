/* Authored native partial-range correction for digest-bound GL-N/GL-P profiles.
 * The original engineer applet clears C4/C8. Set the requested address after
 * entry, matching the later Ddf/De0/De1 READ/BLKCHK/VERIFY source routines.
 * Programming restores its own address from four parameters and is unchanged. */
#ifndef LT48_GLN_REGION_TRANSITIONS_H
#define LT48_GLN_REGION_TRANSITIONS_H
static unsigned char *gln_region_wave(unsigned definition,unsigned operation,
    const unsigned char *code,size_t size,size_t *out_size) {
    switch(definition) {
    case 0xb8:case 0xb9:case 0xbf:case 0xc0:
    case 0xba:case 0xbb:case 0xbc:case 0xc1:case 0xc3:case 0xc4:case 0xc5:
    case 0xc8:case 0xc9:case 0xca:
    case 0xb7:case 0xcc:case 0xcd:case 0xce:break;
    default:return NULL;
    }
    if(operation!=5 && operation!=7 && operation!=9)return NULL;
    static const unsigned char address[]={0x4d,2,0,2,0,0,0,
        0x3b,0xc4,0x3e,0,0,0x3b,0xc8,0x3e,0,1,
        0x3b,0xcc,0x3e,0,2,0x3b,0xc6,0x3e,0,3};
    static const unsigned char entry[]={0x5d,1,5,16,0x4e,14,0x3d,26,
        0x39,5,0x3b,0x88,0x39,0,0x60};
    size_t at=2;
    if(size<at+sizeof(address)+sizeof(entry) ||
       memcmp(code+at,address,sizeof(address)) ||
       memcmp(code+at+sizeof(address),entry,sizeof(entry)))return NULL;
    unsigned char *out=malloc(size);if(!out)return NULL;memcpy(out,code,size);
    memcpy(out+at,entry,sizeof(entry));memcpy(out+at+sizeof(entry),address,sizeof(address));
    *out_size=size;return out;
}
#endif
