/* Authored extra FF write pulse: Micron MT28F160C3 Rev3 OTP exit requires
 * two FF commands. The original GETCONFIG9 ends with one FF pulse. */
static lt48_status micronc3two_otp_exit(const lt48_wave_code*code,unsigned char**copy,size_t*size){
 if(code->data_size!=312||code->data[0]!=0x36||code->data[1]!=1||memcmp(code->data+304,"\x3b\xc0\x3b\xff\x2c\x0b\x19\x00",8))return LT48_SAFETY_REJECTED;
 *size=code->data_size+4;*copy=malloc(*size);if(!*copy)return LT48_NO_MEMORY;
 memcpy(*copy,code->data,code->data_size);(*copy)[0]=0x3a;memcpy(*copy+code->data_size,"\x2c\x0b\x19\x00",4);return LT48_OK;
}

/* Array-only owner deliberately omits the original mode2 OTP rewrite. */
static lt48_status micronc3two_array_reset(const lt48_wave_code*code,unsigned char**copy,size_t*size){
 if(code->data_size!=79||memcmp(code->data+0x1d,"\x3d\x11",2)||code->data[0x3f]!=0x60||code->data[0x40]!=0x39)return LT48_SAFETY_REJECTED;
 *size=17;*copy=malloc(*size);if(!*copy)return LT48_NO_MEMORY;(*copy)[0]=15;(*copy)[1]=0;memcpy(*copy+2,code->data+0x40,15);return LT48_OK;
}
