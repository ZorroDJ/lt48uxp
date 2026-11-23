/* Authored decoder for the source-defined C5114 configuration register.
 * Low 11/10/01 select lock levels 0/1/2. Low 00 is not supported.
 * Five flags are the inverse of raw bits 3..7. Bit 2 is not a flag.
 * Returns zero on an unsupported encoding without publishing output.
 */
#ifndef LT48_C5114_CONFIGURATION_H
#define LT48_C5114_CONFIGURATION_H
static int lt48_c5114_decode_configuration(unsigned char raw,unsigned char out[6]) {
    unsigned low=raw&3u;
    if(!low)return 0;
    for(unsigned i=0;i<5;i++)out[i]=(unsigned char)(((raw>>(i+3))&1u)^1u);
    out[5]=(unsigned char)(3u-low);
    return 1;
}
#endif
