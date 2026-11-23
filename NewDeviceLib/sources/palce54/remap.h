/* Authored original-00443620 gather/constant semantics. Input is snapshotted
 * by the caller; all 256 output bits start clear. No hardware side effects. */
#ifndef LT48_PALCE54_REMAP_H
#define LT48_PALCE54_REMAP_H
static unsigned palce54_gather_bit(unsigned word,const unsigned char input[32]){
 if(word<256u)return(input[word/8u]>>(word&7u))&1u;
 return(word&256u)?word&1u:0u;
}
#endif
