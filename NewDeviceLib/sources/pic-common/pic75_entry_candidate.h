#ifndef LT48_PIC75_ENTRY_CANDIDATE_H
#define LT48_PIC75_ENTRY_CANDIDATE_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Private primary-defined candidate, not a public factory default. Exact
 * W2c uses absent logical0 for its VDD route. The original P5 loader/compiler
 * leave it0, producing resident1C01(no-op). Route the declared P5 VCC group
 * through the existing source variable0/channel0 emitter instead. Original
 * raw source remains immutable. Whole host/adapter and full operation gates
 * remain required; this is not a claim of a whole Windows application bug. */
static inline lt48_status lt48_pic75_entry_candidate_compile(const lt48_wave_code *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t count,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 static const unsigned char expected[32]={0xa8,0xfb,0x77,0x1a,0x30,0xd8,0xab,0xdb,0x3b,0x8b,0x34,0xe7,0x2a,0xd5,0x62,0xa4,0x53,0x47,0xbe,0xeb,0xfa,0x39,0xa8,0xc9,0x9f,0x00,0x25,0xfe,0xc7,0xcf,0xda,0x49};
 if(!source||!pins||!vars||source->wave_type||source->data_size!=312||!source->data)return LT48_SAFETY_REJECTED;
 unsigned char hash[32];lt48_sha256(source->data,source->data_size,hash);if(memcmp(hash,expected,32))return LT48_SAFETY_REJECTED;
 lt48_pin_group vcc,gnd;if(lt48_pin_map_find_group(pins,2,&vcc)!=LT48_OK||vcc.pin_count!=1||vcc.pins[0]!=148||lt48_pin_map_find_group(pins,1,&gnd)!=LT48_OK||gnd.pin_count!=1||gnd.pins[0]!=84)return LT48_SAFETY_REJECTED;
 unsigned matched=0;for(size_t i=0;i<count;i++)if(vars[i].variable_id==0){if(vars[i].kind||vars[i].channel||!vars[i].enabled)return LT48_SAFETY_REJECTED;matched++;}
 if(matched!=1)return LT48_SAFETY_REJECTED;
 unsigned char raw[312];memcpy(raw,source->data,sizeof raw);if(memcmp(raw+28,"\x03\x00\x00",3))return LT48_SAFETY_REJECTED;
 memcpy(raw+28,"\x05\x02\x00",3);
 lt48_compact_wave *wave=NULL;lt48_status st=lt48_compact_decode(raw,sizeof raw,&wave);
 if(st==LT48_OK)st=lt48_compact_compile(wave,pins,vars,count,1,0,out);
 lt48_compact_wave_close(wave);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK&&*out){lt48_compiled_wave_close(*out);*out=NULL;}return st;
}
#endif
