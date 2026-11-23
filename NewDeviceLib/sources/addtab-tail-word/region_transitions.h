/* Authored native partial-range correction for digest-bound GL-N/GL-P profiles.
 * The original engineer applet clears C4/C8. Set the requested address after
 * entry, matching the later Ddf/De0/De1 READ/BLKCHK/VERIFY source routines.
 * Programming restores its own address from four parameters and is unchanged. */
#ifndef LT48_TAIL_REGION_TRANSITIONS_H
#define LT48_TAIL_REGION_TRANSITIONS_H
static unsigned char *tail_region_wave(unsigned definition,unsigned operation,
    const unsigned char *code,size_t size,size_t *out_size) {
    if(definition!=0xc2 && definition!=0xcf && definition!=0xd0 && definition!=0xd1 && definition!=0xac &&
       definition!=0xbd && definition!=0xbe && definition!=0xc6 && definition!=0xc7)return NULL;
    if(operation!=5 && operation!=7 && operation!=9)return NULL;
    static const unsigned char address[]={0x4d,2,0,2,0,0,0,
        0x3b,0xc4,0x3e,0,0,0x3b,0xc8,0x3e,0,1,
        0x3b,0xcc,0x3e,0,2,0x3b,0xc6,0x3e,0,3};
    static const unsigned char entry[]={0x5d,1,5,16,0x4e,14,0x3d,26,
        0x39,5,0x3b,0x88,0x39,0,0x60};
    static const unsigned char m_entry[]={0x5d,1,5,16,0x4e,14,0x3d,26,0x39,5,0x39,0,0x60};
    const unsigned char *transition=definition==0xac?m_entry:entry;
    size_t entry_size=definition==0xac?sizeof(m_entry):sizeof(entry);
    size_t at=2;
    if(size<at+sizeof(address)+entry_size ||
       memcmp(code+at,address,sizeof(address)) ||
       memcmp(code+at+sizeof(address),transition,entry_size))return NULL;
    unsigned char *out=malloc(size);if(!out)return NULL;memcpy(out,code,size);
    memcpy(out+at,transition,entry_size);memcpy(out+at+entry_size,address,sizeof(address));
    *out_size=size;return out;
}
#endif
