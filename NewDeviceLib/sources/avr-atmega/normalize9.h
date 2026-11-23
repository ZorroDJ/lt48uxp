/* Authored, source-SHA-scoped P53 optional-AVCC normalization.
 * The shared init references logical27, absent from P53. Preserve P54's mapped
 * auxiliary supply and omit only the P53 unmapped routing instruction. */
static lt48_status atmega9_normalize_init(const unsigned char*data,size_t size,
                                         unsigned char out[437],size_t*length){
 if(size!=437||data[0]!=0xb3||data[1]!=1||memcmp(data+34,"\x0b\x1b\x01",3))return LT48_SAFETY_REJECTED;
 memcpy(out,data,34);memcpy(out+34,data+37,size-37);*length=size-3;
 out[0]=(unsigned char)(*length-2);out[1]=(unsigned char)((*length-2)>>8);return LT48_OK;
}
