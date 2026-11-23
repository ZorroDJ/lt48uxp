#ifndef LT48_HC908_RAM_HELPERS_H
#define LT48_HC908_RAM_HELPERS_H
/* Authored HC08 RAM code at B0. Data is at8C; monitor stack remains F0..FF.
 * AN2627 instruction encodings/cycles and QT/QY FLASH programming flow.
 * A 21-iteration delay gives 90 bus cycles between consecutive byte writes.
 * At the source monitor's ~2.46MHz bus:36.6us, within30..40us.
 * No UART activity occurs while target PGM/HVEN is asserted. */
static size_t hc908_program_ram(unsigned char *p,unsigned address,unsigned count) {
 if(!count||count>32||((address&31)+count)>32)return 0;
 const unsigned char body[]={
  0x9b,0xa6,1,0xc7,0xfe,8,0xc6,0xff,0xbe,0xc7,0,0,
  0xad,0,0xa6,9,0xc7,0xfe,8,0xad,0,0x45,0,0,
  0xd6,0,0x8c,0xd7,0,0,0xad,0,0xaf,1,0x65,0,0,0x26,0,
  0xa6,8,0xc7,0xfe,8,0xad,0,0xa6,0,0xc7,0xfe,8,0x83,
  0xa6,21,0x4b,0xfe,0x81};
 memcpy(p,body,sizeof body);p[10]=p[28]=(unsigned char)(address>>8);p[11]=p[29]=(unsigned char)address;p[36]=(unsigned char)count;
 const unsigned branches[]={13,20,31,45};
 for(unsigned i=0;i<4;i++)p[branches[i]]=(unsigned char)(52-(branches[i]+1));
 p[38]=(unsigned char)(24-39);return sizeof body;
}
static size_t hc908_erase_ram(unsigned char *p,unsigned address) {
 const unsigned char body[]={
  0x9b,0xa6,6,0xc7,0xfe,8,0xc6,0xff,0xbe,0xc7,0,0,0xad,0,
  0xa6,14,0xc7,0xfe,8,0x45,8,0,0xaf,0xff,0x65,0,0,0x26,0,
  0xa6,12,0xc7,0xfe,8,0xad,0,0xad,0,0xad,0,0xad,0,
  0xa6,0,0xc7,0xfe,8,0x83,0xa6,21,0x4b,0xfe,0x81};
 memcpy(p,body,sizeof body);p[10]=(unsigned char)(address>>8);p[11]=(unsigned char)address;
 const unsigned branches[]={13,35,37,39,41};
 for(unsigned i=0;i<5;i++)p[branches[i]]=(unsigned char)(48-(branches[i]+1));
 p[28]=(unsigned char)(22-29);return sizeof body;
}
/* JB12/JB16 rev1.1 requires >=200ms mass erase. Four full 16-bit loops
 * produce ~218ms at its source9.6001536MHz monitor bus, entirely in RAM. */
static size_t hc908_erase_ram_long(unsigned char *p,unsigned address){
 size_t n=hc908_erase_ram(p,address);
 memmove(p+33,p+29,n-29);
 const unsigned char loops[]={0xa6,4,0x45,0xff,0xff,0xaf,0xff,0x65,0,0,0x26,0xf9,0x4b,0xf4};
 memcpy(p+19,loops,sizeof loops);
 const unsigned branches[]={13,39,41,43,45};
 for(unsigned i=0;i<5;i++)p[branches[i]]=(unsigned char)(52-(branches[i]+1));
 return n+4;
}
#endif
