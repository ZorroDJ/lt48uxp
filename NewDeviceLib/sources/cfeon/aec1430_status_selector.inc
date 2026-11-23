/* Authored correction: select the three-byte branch of the dual-entry
 * 6000h applet before its status-read command. Caller pins REZ and D/M/W.
 * The uncorrected stream consumes CS as the selector and shifts all pins.
 * The supplied buffer must have six spare bytes. No vendor payload here. */
static lt48_status aec1430_status_selector(unsigned char *bytes,size_t *length) {
    static const unsigned char site[]={0x3d,0x11,0x39,0x05};
    size_t offsets[3],count=0,size=*length;
    for(size_t i=2;i+sizeof(site)<=size;i++)if(!memcmp(bytes+i,site,sizeof(site))) {
        if(count==3)return LT48_SAFETY_REJECTED;
        offsets[count++]=i+2;
    }
    if(count!=3 || size<2 || size>65529)return LT48_SAFETY_REJECTED;
    unsigned declared=bytes[0]|(unsigned)bytes[1]<<8;
    if(declared+2!=size)return LT48_FORMAT_ERROR;
    for(size_t k=3;k>0;k--) {
        size_t at=offsets[k-1];
        memmove(bytes+at+2,bytes+at,size-at);
        bytes[at]=0x3b;bytes[at+1]=1;size+=2;
    }
    declared+=6;bytes[0]=(unsigned char)declared;bytes[1]=(unsigned char)(declared>>8);
    *length=size;return LT48_OK;
}
