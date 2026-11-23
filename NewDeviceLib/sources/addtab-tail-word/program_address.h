/* Private D120 trial: bind the source W9d program applet (explicit unlock)
 * and supply its fourth address byte in both compact PROGRAM branches.
 * The caller's REZ digest and D120 profile remain mandatory. */
#ifndef LT48_TAIL_PROGRAM_ADDRESS_H
#define LT48_TAIL_PROGRAM_ADDRESS_H
static unsigned char *tail_program_address_wave(unsigned definition,unsigned operation,
    const unsigned char *code,size_t size,size_t *out_size) {
    static const unsigned char setup[]={0x3d,0x14,0x3e,0x0e,0,0x39,5,0x3b,0x25,
        0x3e,0x15,0,0x3e,0,0,0x3e,0,1,0x3e,0,2};
    if(definition!=0x120 || operation!=8 || size!=129 ||
       code[0]!=127 || code[1]!=0)return NULL;
    size_t positions[2],count=0;
    for(size_t i=2;i+sizeof(setup)<=size;i++)if(!memcmp(code+i,setup,sizeof(setup))) {
        if(count==2)return NULL;
        positions[count++]=i+sizeof(setup);
    }
    if(count!=2)return NULL;
    unsigned char *out=malloc(size+6);if(!out)return NULL;
    size_t source=0,destination=0;
    for(size_t i=0;i<2;i++) {
        size_t length=positions[i]-source;memcpy(out+destination,code+source,length);
        destination+=length;source=positions[i];
        out[destination++]=0x3e;out[destination++]=0;out[destination++]=3;
    }
    memcpy(out+destination,code+source,size-source);
    out[0]=133;out[1]=0;*out_size=size+6;return out;
}
#endif
