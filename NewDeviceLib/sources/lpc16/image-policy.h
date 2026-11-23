/* Conservative rejection of CRP/NO_ISP words at both LPC family offsets.
 * Standalone protection is not an admitted operation. No checksum rewriting. */
static int lpc16_image_allowed(const unsigned char*p,size_t n){
 static const unsigned a[]={0x1fc,0x2fc};if(!p)return 0;
 for(size_t i=0;i<2;i++)if(n>=a[i]+4){uint32_t v=(uint32_t)p[a[i]]|((uint32_t)p[a[i]+1]<<8)|((uint32_t)p[a[i]+2]<<16)|((uint32_t)p[a[i]+3]<<24);if(v==0x12345678||v==0x87654321||v==0x43218765||v==0x4e697370)return 0;}
 return 1;
}
