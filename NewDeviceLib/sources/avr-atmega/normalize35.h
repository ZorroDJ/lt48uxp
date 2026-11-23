/* Omit only the unreachable auxiliary-supply instruction for these exact
 * source layouts and package discriminators. Mapped package paths stay intact. */
static lt48_status avr35_normalize_init(unsigned family,unsigned chip,const unsigned char*d,size_t n,unsigned char*out,size_t*size){
 size_t expected,off;
 if((family==129||family==130)&&chip==64){expected=455;off=0x2d;}
 else if(family==126&&chip==32){expected=461;off=0x33;}
 else if(family==127&&chip==40){expected=464;off=0x2d;}
 else return LT48_SAFETY_REJECTED;
 if(n!=expected||d[0]!=(unsigned char)(n-2)||d[1]!=(n-2)>>8||memcmp(d+off,"\x0b\x1b\x01",3))return LT48_SAFETY_REJECTED;
 memcpy(out,d,off);memcpy(out+off,d+off+3,n-off-3);*size=n-3;out[0]=(unsigned char)(*size-2);out[1]=(unsigned char)((*size-2)>>8);return LT48_OK;
}
