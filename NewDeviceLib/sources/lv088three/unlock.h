/* Authored explicit-address AA/55 command helpers for exact GDAA Dbd.
 * Replaces only source C2/C3 shortcut sites in RAM. Source REZ remains unchanged.
 * ELAN EM39LV088 Table3: AAA/555, with A14..A0 significant. */
static void lv088_emit(unsigned char*b,size_t*n,unsigned v){b[(*n)++]=0x74;b[(*n)++]=(unsigned char)v;b[(*n)++]=0xf0;}
static void lv088_address(unsigned char*b,size_t*n,unsigned address){
    const unsigned cmd[]={0xc4,0xc8,0xcc};
    for(unsigned i=0;i<3;i++){lv088_emit(b,n,cmd[i]);lv088_emit(b,n,(address>>(8*i))&255);}
}
static unsigned char *lv088_applet(unsigned op,const lt48_wave_code*w,size_t*out_size){
    static const unsigned char prefix[]={0x74,0xc2,0xf0,0x74,0xaa,0xf0,0x74,0x55,0xf0};
    unsigned sites[2],count,base;
    if(op==16){if(w->data_size!=204||w->load_address!=0x5000)return NULL;sites[0]=0x95;count=1;base=0x5000;}
    else if(op==20){if(w->data_size!=280||w->load_address!=0x8000)return NULL;sites[0]=0xdb;count=1;base=0x8000;}
    else if(op==23){if(w->data_size!=235||w->load_address!=0x9000)return NULL;sites[0]=0xa1;sites[1]=0xad;count=2;base=0x9000;}
    else return NULL;
    unsigned char*b=malloc(512);if(!b)return NULL;memcpy(b,w->data,w->data_size);size_t n=w->data_size;
    for(unsigned i=0;i<count;i++){
        unsigned site=sites[i],resume=base+site+9;
        unsigned char expected[9];memcpy(expected,prefix,9);if(op==20)expected[1]=0xc3;
        if(memcmp(b+site,expected,sizeof expected)){free(b);return NULL;}
        if(op==16){if(memcmp(b+site+9,"\40\262\375\342\360",5)){free(b);return NULL;}resume+=5;}
        if(op==23){if(b[site+9]!=0xe5||b[site+10]!=(i?0x11:0x10)||b[site+11]!=0xf0){free(b);return NULL;}resume+=3;}
        if(op==20){static const unsigned char suffix[]={0xe5,0x12,0xf0,0xe5,0x10,0xf0};if(memcmp(b+site+9,suffix,sizeof suffix)){free(b);return NULL;}resume+=6;}
        unsigned address=base+(unsigned)n;b[site]=2;b[site+1]=(unsigned char)(address>>8);b[site+2]=(unsigned char)address;memset(b+site+3,0,op==23?9:op==20?12:11);
        lv088_address(b,&n,0xaaa);lv088_emit(b,&n,0xc0);lv088_emit(b,&n,0xaa);
        lv088_address(b,&n,0x555);lv088_emit(b,&n,0xc0);lv088_emit(b,&n,0x55);
        lv088_address(b,&n,0xaaa);lv088_emit(b,&n,0xc0);
        if(op==16){static const unsigned char cmd[]={0x20,0xb2,0xfd,0xe2,0xf0};memcpy(b+n,cmd,sizeof cmd);n+=sizeof cmd;lv088_address(b,&n,0);}
        if(op==20){
            b[n++]=0xe5;b[n++]=0x12;b[n++]=0xf0;
            const unsigned cmd[]={0xc4,0xc8,0xcc};for(unsigned k=0;k<3;k++){lv088_emit(b,&n,cmd[k]);b[n++]=0xe5;b[n++]=(unsigned char)(0x17+k);b[n++]=0xf0;}
            lv088_emit(b,&n,0xc0);b[n++]=0xe5;b[n++]=0x10;b[n++]=0xf0;
            static const unsigned char inc[]={0x05,0x17,0xe5,0x17,0x70,8,0x05,0x18,0xe5,0x18,0x70,2,0x05,0x19};memcpy(b+n,inc,sizeof inc);n+=sizeof inc;
        }
        if(op==23){b[n++]=0xe5;b[n++]=(unsigned char)(i?0x11:0x10);b[n++]=0xf0;}
        b[n++]=2;b[n++]=(unsigned char)(resume>>8);b[n++]=(unsigned char)resume;
    }
    if(op==20){
        if(memcmp(b+0xd2,"\342\365\22\2\200\6",6)){free(b);return NULL;}
        unsigned address=base+(unsigned)n;b[0xd5]=2;b[0xd6]=(unsigned char)(address>>8);b[0xd7]=(unsigned char)address;
        for(unsigned k=0;k<3;k++){const unsigned char get[]={0x20,0xb2,0xfd,0xe2,0xf5,(unsigned char)(0x17+k)};memcpy(b+n,get,sizeof get);n+=sizeof get;}
        b[n++]=2;b[n++]=0x80;b[n++]=6;
    }
    if(n>512){free(b);return NULL;}*out_size=n;return b;
}

/* Extend only Program INIT's FIFO prefix with the requested byte address.
 * The source worker has no CPU address counter: C3 used FPGA's hidden counter.
 * The authored explicit-address loop keeps a separate 24-bit counter in17..19, below resident driver shadows1E..2F. */
static unsigned char *lv088_init(const lt48_wave_code*w,size_t*n){
 if(w->data_size!=156||memcmp(w->data+53,"\73\240\137",3))return NULL;
 unsigned char*b=malloc(165);if(!b)return NULL;memcpy(b,w->data,55);
 static const unsigned char addr[]={0x3e,0,0,0x3e,0,1,0x3e,0,2};memcpy(b+55,addr,sizeof addr);memcpy(b+64,w->data+55,101);*n=165;b[0]=163;b[1]=0;return b;
}
