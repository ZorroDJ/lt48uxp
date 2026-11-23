/* Authored native partial-range corrections for digest-bound EW54 profiles.
 * Source engineer entry clears low address bytes. Restore the requested
 * address after entry, matching the Spansion sibling's instruction ordering.
 * Source reset-less Spansion profiles need RESET# between native repeated
 * main/OTP stages. Pin 5 and 1 ms come from the same source's reset/ID paths. */
#ifndef LT48_EW_REGION_TRANSITIONS_H
#define LT48_EW_REGION_TRANSITIONS_H
static unsigned char *ew_region_wave(unsigned definition,unsigned operation,
    const unsigned char *code,size_t size,size_t *out_size) {
    if((definition==0xe4 || definition==0xe6) && operation==3) {
        static const unsigned char reset[]={8,0,0x11,5,0,0x32,0xe8,3,0,0};
        if(size!=2 || code[0] || code[1])return NULL;
        unsigned char *out=malloc(sizeof(reset));if(!out)return NULL;
        memcpy(out,reset,sizeof(reset));*out_size=sizeof(reset);return out;
    }
    if(definition!=0xb4 || (operation!=5 && operation!=7 && operation!=8 && operation!=9))return NULL;
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
