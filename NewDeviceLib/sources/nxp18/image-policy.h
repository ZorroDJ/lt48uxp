/* Authored scope restriction: UM10398 Rev12.4 section26.3.8/table372.
 * This array-only admission does not expose CRP or NO_ISP programming.
 * Bytes and vector checksum otherwise remain exactly as supplied. */
static int nxp18_image_allowed(const unsigned char*image,size_t size){
 if(!image||size<0x300)return 0;
 uint32_t crp=0;for(unsigned i=0;i<4;i++)crp|=(uint32_t)image[0x2fc+i]<<(8*i);
 return crp!=0x12345678u&&crp!=0x87654321u&&crp!=0x43218765u&&crp!=0x4e697370u;
}
