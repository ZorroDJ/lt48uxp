/* Authored exact-source normalization: M84/D9d ChipID is40, so the
 * original ChipID==44 auxiliary-supply branch cannot execute. Its logical27
 * is absent from the DIP map. Preserve M85's mapped TQFP supply instruction. */
static lt48_status avr31_normalize_323_init(const unsigned char*data,size_t size,
                                           unsigned char out[437],size_t*length){
 if(size!=300||memcmp(data+45,"\x0b\x1b\x01",3))return LT48_SAFETY_REJECTED;
 memcpy(out,data,45);memcpy(out+45,data+48,size-48);*length=size-3;
 out[0]=(unsigned char)(*length-2);out[1]=(unsigned char)((*length-2)>>8);return LT48_OK;
}
