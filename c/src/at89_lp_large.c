#include "at89_lp_large.h"
#include <string.h>

lt48_status lt48_lp_large_compile_fragment(const lt48_wave_code *source,
    enum lt48_lp_large_fragment part,const lt48_pin_map *pins,
    const lt48_compile_variable *vars,size_t count,lt48_compiled_wave **out) {
    static const unsigned char protect_hash[32]={
        0x4a,0xe9,0xfd,0x87,0x15,0x47,0xd0,0xde,0x0b,0xec,0xf6,0x90,0xc9,0xa0,0xa8,0xb9,
        0x80,0xae,0xe3,0x10,0xc8,0x17,0x4d,0x67,0x4a,0x2e,0x1f,0xf6,0xdc,0x04,0x89,0xc8};
    static const unsigned char erase_hash[32]={
        0xa5,0xb0,0x8f,0xfd,0xd5,0xab,0xbb,0x62,0xa9,0xfb,0x74,0x4e,0x2c,0x05,0x13,0x5c,
        0xdc,0xdc,0xf4,0xf5,0x52,0xa3,0xc4,0x06,0x7c,0xbd,0x22,0xd5,0x37,0x61,0x00,0x5d};
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!source || !pins || !vars || part<LT48_LP_LARGE_FUSES || part>LT48_LP_LARGE_CHIP_ERASE)
        return LT48_INVALID_ARGUMENT;
    size_t size=part==LT48_LP_LARGE_CHIP_ERASE?236:253;
    if(source->wave_type || !source->data || source->data_size!=size)return LT48_SAFETY_REJECTED;
    unsigned char digest[32];lt48_sha256(source->data,size,digest);
    if(memcmp(digest,part==LT48_LP_LARGE_CHIP_ERASE?erase_hash:protect_hash,32))return LT48_SAFETY_REJECTED;
    size_t begin=part==LT48_LP_LARGE_LOCKS?0x72:2;
    size_t end=part==LT48_LP_LARGE_FUSES?0x72:part==LT48_LP_LARGE_LOCKS?0xf6:0x2c;
    unsigned char raw[256];size_t body=end-begin;
    raw[0]=(unsigned char)body;raw[1]=(unsigned char)(body>>8);
    memcpy(raw+2,source->data+begin,body);
    lt48_compact_wave *decoded=NULL;
    lt48_status st=lt48_compact_decode(raw,body+2,&decoded);
    if(st==LT48_OK)st=lt48_compact_compile(decoded,pins,vars,count,1,0,out);
    lt48_compact_wave_close(decoded);return st;
}
