/* Authored correction, applied only after exact GDABA source hash/profile checks.
 * W11 prepares word address 80h and lock word in Out, but omits PRP dispatch.
 * Reuse the device's own downloaded program applet with C0h and Out[0:1].
 * ST M28W160C command table 4 and protection-register section specify C0h.
 * Existing lock-register readback remains mandatory in the native owner. */
#ifndef LT48_ST_LOCK_WRITE_H
#define LT48_ST_LOCK_WRITE_H
static unsigned char *intel_word_st_lock_write(const unsigned char *code,size_t size,size_t *out_size) {
    static const unsigned char tail[]={0x40,2,4,1,0,0,0,0x4a,1,2,12,4};
    static const unsigned char dispatch[]={0x3d,20,0x39,5,0x3b,0xc0,0x3e,3,0,0x3e,3,1,0x7c,2,0x39,0};
    if(size!=169 || memcmp(code+157,tail,sizeof(tail)))return NULL;
    unsigned char *result=malloc(size+sizeof(dispatch));if(!result)return NULL;
    memcpy(result,code,157);memcpy(result+157,dispatch,sizeof(dispatch));
    memcpy(result+157+sizeof(dispatch),code+157,size-157);*out_size=size+sizeof(dispatch);result[0]=(unsigned char)(*out_size-2);result[1]=(unsigned char)((*out_size-2)>>8);return result;
}
#endif
