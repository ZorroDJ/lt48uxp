/* Authored, exact-source correction for GDCA D26/M26 W8b ReadCfg.
 * The caller verifies the complete REZ digest and selected definition first.
 * No original waveform, applet or FPGA image is embedded here.
 */
static lt48_status adesto_correct_parallel_config(unsigned char *bytes,size_t *length) {
    static const unsigned char impossible[]={0x5e,3,0,0xff,0,0,0};
    static const unsigned char stale_sector[]={0x5e,0x19,0,0xf0,0,0,0};
    size_t size=*length;
    static const unsigned char extra_read[]={0x3b,0xff,0x7c,0x19};
    size_t first=SIZE_MAX,last=SIZE_MAX,extra=SIZE_MAX;unsigned a=0,b=0,c=0;
    for(size_t i=0;i+sizeof(impossible)<=size;i++) {
        if(!memcmp(bytes+i,impossible,sizeof(impossible))){first=i;a++;}
        if(!memcmp(bytes+i,stale_sector,sizeof(stale_sector))){last=i;b++;}
    }
    for(size_t i=0;i+sizeof(extra_read)<=size;i++)
        if(!memcmp(bytes+i,extra_read,sizeof(extra_read))){extra=i;c++;}
    if(a!=1 || b!=1 || c!=1 || extra>=first || first>=last)return LT48_SAFETY_REJECTED;
    /* Buffer3 was ANDed with F0. The corresponding serial source W91
     * already tests F0 and uses that current byte for sector31 as well. */
    bytes[first+3]=0xf0;
    bytes[last+1]=3;
    /* 32h and 35h have the same seven parallel dummy cycles. The applet
     * already generates those; discard neither the first sector nor read a
     * thirty-third, undefined byte. Match the sibling lockdown traversal. */
    memmove(bytes+extra,bytes+extra+sizeof(extra_read),size-extra-sizeof(extra_read));
    *length=size-sizeof(extra_read);
    unsigned declared=bytes[0]|(unsigned)bytes[1]<<8;
    if(declared<sizeof(extra_read))return LT48_FORMAT_ERROR;
    declared-=sizeof(extra_read);bytes[0]=(unsigned char)declared;bytes[1]=(unsigned char)(declared>>8);
    return LT48_OK;
}
