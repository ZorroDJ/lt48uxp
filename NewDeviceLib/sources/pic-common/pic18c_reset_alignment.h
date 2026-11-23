#ifndef LT48_PIC18C_RESET_ALIGNMENT_H
#define LT48_PIC18C_RESET_ALIGNMENT_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Candidate primary-defined safe exit for exact W201: retain the source's
 * zero address/scratch and VPP ramp/disconnect, then set clock/data low.
 * MCLR is already low before either digital line can clock again. The CPU
 * table pointer resets in hardware; no serial address-restoration sequence
 * is needed while leaving ICSP. Raw source remains available. This is a
 * native lifecycle choice,not a claim that the whole Windows application
 * executes its source callback with the same incoming serial phase. */
static inline lt48_status lt48_pic18c_reset_alignment_compile(const lt48_wave_code *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t count,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 static const unsigned char expected[32]={0x6c,0x1d,0x3b,0xcc,0xa5,0xac,0x55,0xba,0xa5,0x19,0x21,0xa0,0x4d,0x0a,0xbd,0x47,0x57,0x79,0xba,0xea,0xda,0xa0,0xaa,0xe6,0x65,0x7a,0x73,0x45,0x6f,0xcc,0xab,0x81};
 if(!source||!pins||!vars||source->wave_type||source->data_size!=56||!source->data)return LT48_SAFETY_REJECTED;
 unsigned char hash[32];lt48_sha256(source->data,source->data_size,hash);if(memcmp(hash,expected,32))return LT48_SAFETY_REJECTED;
 unsigned char raw[29];raw[0]=27;raw[1]=0;
 memcpy(raw+2,source->data+33,13); /* Zero scratch/address. */
 memcpy(raw+15,source->data+48,7); /* Original VPP-to-zero/disconnect and delay. */
 memcpy(raw+22,"\x11\x17\x00\x11\x18\x00",6);raw[28]=0x36;
 lt48_compact_wave *wave=NULL;lt48_status st=lt48_compact_decode(raw,sizeof raw,&wave);
 if(st==LT48_OK)st=lt48_compact_compile(wave,pins,vars,count,1,0,out);
 lt48_compact_wave_close(wave);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK&&*out){lt48_compiled_wave_close(*out);*out=NULL;}return st;
}
#endif
