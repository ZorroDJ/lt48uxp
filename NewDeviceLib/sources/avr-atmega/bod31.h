/* Authored correction for canonical GAB Ma0/Ma1, W135/W136. The original
 * callbacks shift BODLEVEL[2:0] by one bit. Manufacturer2549Q Table30-3 and
 * original-x86 FC->FD witnesses are retained separately. Only six operand
 * bytes change across the two callbacks; no original assets are replaced. */
static lt48_status avr31_correct_bod(const unsigned char*data,size_t size,
                                    unsigned op,unsigned char out[1024],size_t*length){
 static const unsigned read_offsets[]={0x11e,0x129,0x134};
 static const unsigned write_offsets[]={0x183,0x18e,0x199};
 if((op==23&&size!=556)||(op==7&&size!=783)||(op!=7&&op!=23))return LT48_SAFETY_REJECTED;
 memcpy(out,data,size);*length=size;
 for(unsigned i=0;i<3;i++){
  unsigned p=op==23?read_offsets[i]:write_offsets[i];
  unsigned char expected[6]={0x45,op==23?6:22,op==23?0:i+1,op==23?1:6,op==23?i+1:0,1};
  if(memcmp(data+p,expected,sizeof expected))return LT48_SAFETY_REJECTED;
  out[p+(op==23?4:2)]=(unsigned char)i;
 }
 return LT48_OK;
}
