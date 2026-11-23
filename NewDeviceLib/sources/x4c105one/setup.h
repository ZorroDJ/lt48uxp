/* Authored X4C105 safety transforms. Requires the documented passive adapter:
 * 47 uF CAP-VSS; 10k CE/WE/OE and serial-bus pullups to VCC. Never actively
 * drive CAP, RESET, O0..3 or D0..3. Separate NOVRAM is not an image region.
 * Input is gated by the complete Dataman GA hash in compact_i2c.c. */
static lt48_status x4c105_pins(const lt48_pin_code *source, lt48_pin_map **out) {
    unsigned char raw[128]; size_t p=3,q=3;
    if(source->pin_count!=20 || source->data_size>sizeof raw || source->data_size<3 ||
       source->data[0]!=4 || source->data[1]!=4)return LT48_SAFETY_REJECTED;
    memcpy(raw,source->data,3);
    for(unsigned i=0;i<4;i++) {
        if(p+4>source->data_size)return LT48_FORMAT_ERROR;
        unsigned kind=source->data[p]|source->data[p+1]<<8;
        size_t n=source->data[p+2]|source->data[p+3]<<8;p+=4;
        if(n>source->data_size-p)return LT48_FORMAT_ERROR;
        size_t keep=kind==3?0:kind==7?2:n;
        if((kind==3&&n!=12)||(kind==7&&(n!=3||memcmp(source->data+p,"\0\1\2",3))))return LT48_SAFETY_REJECTED;
        raw[q++]=(unsigned char)kind;raw[q++]=(unsigned char)(kind>>8);
        raw[q++]=(unsigned char)keep;raw[q++]=0;
        memcpy(raw+q,source->data+p+(kind==7?1:0),keep);q+=keep;p+=n;
    }
    memcpy(raw+q,source->data+p,source->data_size-p);q+=source->data_size-p;
    lt48_pin_code code=*source;code.data=raw;code.data_size=q;
    return lt48_pin_map_parse(&code,out);
}
static lt48_status x4c105_wave(unsigned op,const lt48_wave_code *source,
                              unsigned char *raw,size_t *n) {
    *n=source->data_size;
    if(*n>512)return LT48_SAFETY_REJECTED;
    memcpy(raw,source->data,*n);
    if(op==0) {
        if(*n!=19 || memcmp(raw+7,"\5\2\0\0\0",5))return LT48_SAFETY_REJECTED;
        /* Precharge DAC before attaching the VCC driver. */
        memmove(raw+16,raw+12,*n-12);
        static const unsigned char supply[]={0,0,0x11,0,2,0x39,0x20,0x39,0x43};
        memcpy(raw+7,supply,sizeof supply);*n+=4;
    } else if(op==2) {
        if(*n!=256 || memcmp(raw+10,"\3\0\0\0\0",5))return LT48_SAFETY_REJECTED;
        memmove(raw+19,raw+15,*n-15);
        static const unsigned char supply[]={0,0,0x11,0,2,0x39,0x20,0x39,0x43};
        memcpy(raw+10,supply,sizeof supply);*n+=4;
    } else if(op==6) {
        if(*n!=49 || memcmp(raw+39,"\6\2\0\0\66",5))return LT48_SAFETY_REJECTED;
        /* Source STOP/tP remains. Pull serial pins low before VCC falls,
         * hold ground through EEPROM completion + NOVRAM AUTOSTORE. */
        static const unsigned char tail[]={0x11,23,0,0x11,25,0,0x06,2,0,0,
            0x01,1,0,0x32,0x20,0x4e,0,0,0x36};
        memcpy(raw+39,tail,sizeof tail);*n=39+sizeof tail;
    } else if(op==1) {
        if(*n!=12)return LT48_SAFETY_REJECTED;
        /* Also used after failures: allow capacitor charge while powered,
         * remove I/O backfeed, discharge VCC, wait before releasing ground. */
        static const unsigned char off[]={0,0,0x32,0x20,0xa1,7,0,
            0x11,23,0,0x11,25,0,0x06,2,0,0,0x01,1,0,
            0x32,0x20,0x4e,0,0,0x36,0x12,0,2,0x33,0,
            0x11,23,0,0x11,25,0,0x11,0,0};
        memcpy(raw,off,sizeof off);*n=sizeof off;
    }
    raw[0]=(unsigned char)(*n-2);raw[1]=(unsigned char)((*n-2)>>8);
    return LT48_OK;
}
