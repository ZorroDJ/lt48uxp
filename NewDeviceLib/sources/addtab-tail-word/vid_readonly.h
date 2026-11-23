#ifndef LT48_TAIL_VID_READONLY_H
#define LT48_TAIL_VID_READONLY_H
/* D57/Dac private trial. Caller verifies REZ/profile digests. Retain original
 * rail/address setup and cleanup, replacing command-mode ID with direct reads.
 * A9=VID hardware autoselect requires no unlock or F0 write. */
static unsigned char *tail_vid_readonly_wave(unsigned definition,unsigned operation,
    const unsigned char *code,size_t size,size_t *out_size) {
    static const unsigned char old[]={0x3d,0x10,0x39,5,0x3b,0x90,0x3b,0xf0,
        0x11,2,0,0x11,4,0,0x3b,0xff,0x7c,5,0x3b,0xff,0x7c,3,
        0x49,5,1,3,0,1,0x39,0};
    static const unsigned char replacement[]={0x11,2,0,0x11,4,0,
        0x7c,5,0x3b,0xd0,0x7c,3,0x49,5,1,3,0,1,
        0x3b,0xd0,0x3b,0x3d,0x3b,0x35};
    if((definition!=0x57 && definition!=0xac) || operation!=4 ||
       size!=116 || code[0]!=114 || code[1]!=0)return NULL;
    size_t offset=0,count=0;
    for(size_t i=2;i+sizeof(old)<=size;i++)if(!memcmp(code+i,old,sizeof(old))) {
        offset=i;count++;
    }
    if(count!=1)return NULL;
    size_t n=size-sizeof(old)+sizeof(replacement);
    unsigned char *out=malloc(n);if(!out)return NULL;
    memcpy(out,code,offset);memcpy(out+offset,replacement,sizeof(replacement));
    memcpy(out+offset+sizeof(replacement),code+offset+sizeof(old),size-offset-sizeof(old));
    out[0]=(unsigned char)(n-2);out[1]=(unsigned char)((n-2)>>8);*out_size=n;
    return out;
}
/* Sector protect/unprotect is deferred for these M-family profiles. */
static int tail_vid_defer_unprotect(unsigned tail,unsigned definition) {
    return tail && (definition==0x57 || definition==0xac);
}
#endif
