#include "at89_lp51.h"
#include <string.h>

lt48_status lt48_lp51_compile_fragment(const lt48_wave_code *source,
    enum lt48_lp51_fragment part,const lt48_pin_map *pins,
    const lt48_compile_variable *vars,size_t count,lt48_compiled_wave **out) {
    static const unsigned char protect_hash[32]={
        0x93,0xc8,0x5b,0x8e,0x0e,0xa0,0x44,0x35,0xb5,0xbb,0x1f,0xeb,0x99,0xba,0x8a,0xf0,
        0x9c,0xda,0xf6,0xa7,0x82,0x93,0x37,0x67,0x6c,0x21,0xc6,0xbe,0xb2,0xf8,0xf3,0x88};
    static const unsigned char erase_hash[32]={
        0x39,0xcc,0x74,0xdf,0x90,0x91,0x00,0x0f,0x14,0x7d,0xce,0xef,0x1d,0x12,0x07,0xaa,0xb1,0x2c,0x13,0x1b,0x1c,0xac,0xab,0xdd,0x3d,0x74,0x15,0x3e,0xc5,0x1a,0x0d,0xdd};
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!source || !pins || !vars || part<LT48_LP51_FUSES || part>LT48_LP51_CHIP_ERASE)
        return LT48_INVALID_ARGUMENT;
    size_t size=part==LT48_LP51_CHIP_ERASE?250:981;
    if(source->wave_type || !source->data || source->data_size!=size)return LT48_SAFETY_REJECTED;
    unsigned char digest[32];lt48_sha256(source->data,size,digest);
    if(memcmp(digest,part==LT48_LP51_CHIP_ERASE?erase_hash:protect_hash,32))return LT48_SAFETY_REJECTED;
    size_t begin=part==LT48_LP51_LOCKS?0x31e:2;
    size_t end=part==LT48_LP51_FUSES?0x31e:part==LT48_LP51_LOCKS?0x3ce:0x2c;
    unsigned char raw[981];size_t body=end-begin;
    raw[0]=(unsigned char)body;raw[1]=(unsigned char)(body>>8);
    memcpy(raw+2,source->data+begin,body);
    if(part==LT48_LP51_FUSES) {
        unsigned found=0,device=0;
        for(size_t i=0;i<count;i++)if(vars[i].kind==2 && vars[i].variable_id==18) {
            found++;device=vars[i].value;
            if(vars[i].channel)return LT48_SAFETY_REJECTED;
        }
        if(found!=1 || (device!=0 && device!=0x69))return LT48_SAFETY_REJECTED;
        /* First compact instruction is MOV Page,dword(16). The source hash
         * fixes that location; the rest of the original encoding is retained. */
        raw[5]=(unsigned char)(device==0x69?19:16);
    }
    lt48_compact_wave *decoded=NULL;
    lt48_status st=lt48_compact_decode(raw,body+2,&decoded);
    if(st==LT48_OK)st=lt48_compact_compile(decoded,pins,vars,count,1,0,out);
    lt48_compact_wave_close(decoded);return st;
}
