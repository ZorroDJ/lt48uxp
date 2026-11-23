#ifndef LT48_PIC_LEGACY16_POLICY_H
#define LT48_PIC_LEGACY16_POLICY_H
#include "profile.h"
#include <string.h>
#include <stdlib.h>
/* Two exact original PROGRAM components have different write semantics.
 * CP layout does not identify the cycle: F83 and F84A both replicate CP,
 * while the F83 component uses erase/write8 and F84A data uses program-only24.
 * Closed source identities keep this independent of manufacturer/part names. */
static inline lt48_status lt48_pic_legacy16_program_only(const lt48_wave_code *wave,unsigned *out){
 if(!wave||!out||wave->wave_type)return LT48_INVALID_ARGUMENT;
 static const unsigned char erase_write[32]={0x43,0x68,0x67,0xb4,0xe1,0x4e,0xc8,0x64,0x33,0xe0,0x34,0x70,0xbd,0x89,0x79,0x7e,0x2d,0x6d,0xb4,0x70,0x5b,0x99,0x8b,0x09,0xe5,0x8b,0x7c,0x46,0xa8,0xef,0x01,0x6c};
 static const unsigned char program_only[32]={0x56,0x1f,0x35,0xf8,0x9c,0x79,0x9e,0x41,0x8f,0xde,0xd7,0x29,0x45,0x0c,0xf6,0xfd,0x1d,0x82,0x63,0xea,0xf0,0xf8,0xe5,0xd4,0x98,0x82,0xc9,0x09,0x19,0x62,0x71,0x17};
 unsigned char actual[32];lt48_sha256(wave->data,wave->data_size,actual);
 if(!memcmp(actual,erase_write,32)){*out=0;return LT48_OK;}
 if(!memcmp(actual,program_only,32)){*out=1;return LT48_OK;}
 return LT48_SAFETY_REJECTED;
}
/* DS30189D2.3.1.9/DS30262E2.3.1.10 specify a loaded all-ones
 * data word before bulk EEPROM erase. The exact original W408 starts11/8
 * without that load. Construct a RAM-only prefix, retaining every original
 * instruction and the subsequent protection-reset sequence. */
static inline lt48_status lt48_pic_legacy16_erase_compile(const lt48_wave_code *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||source->wave_type||source->data_size<2||source->data_size>65501||!pins||!vars||n!=8)return LT48_SAFETY_REJECTED;
 static const unsigned char expected[32]={0x7e,0xc1,0x0b,0x90,0x44,0x61,0x40,0xe5,0xaa,0x4f,0x43,0x5c,0x26,0x15,0x72,0x98,0x1f,0x19,0x81,0xf0,0x15,0xe3,0xdb,0xdb,0x98,0xaf,0xf2,0x0b,0xa8,0x9d,0xd7,0xf6};
 unsigned char actual[32];lt48_sha256(source->data,source->data_size,actual);if(memcmp(actual,expected,32))return LT48_SAFETY_REJECTED;
 const unsigned char prefix[]={
  0x1a,24,23,3,0,0,0,6,1, 0x1a,24,23,0,0,0,0,1,1,
  0x1a,24,23,0xff,0x3f,0,0,14,1, 0x1a,24,23,0,0,0,0,1,1};
 size_t size=source->data_size+sizeof prefix;unsigned char *raw=malloc(size);if(!raw)return LT48_NO_MEMORY;
 raw[0]=(unsigned char)(size-2);raw[1]=(unsigned char)((size-2)>>8);
 memcpy(raw+2,prefix,sizeof prefix);memcpy(raw+2+sizeof prefix,source->data+2,source->data_size-2);
 lt48_compact_wave *changed=NULL;lt48_status st=lt48_compact_decode(raw,size,&changed);
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,n,1,0,out);
 lt48_compact_wave_close(changed);free(raw);return st;
}
/* DEVINIT reassigns verify voltages; changing the declaration alone is not
 * sufficient. Guard the source digest and change only its 6.0V literal. */
static inline lt48_status lt48_pic_legacy84a_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars||n!=8||lt48_compact_instruction_count(source)!=27)return LT48_SAFETY_REJECTED;
 unsigned char raw[122]={120,0},hash[32];size_t at=2;
 for(size_t i=0;i<27;i++){
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)return st;
  if(in.offset!=at||1+in.operand_size>sizeof(raw)-at)return LT48_SAFETY_REJECTED;
  raw[at++]=in.opcode;memcpy(raw+at,in.operands,in.operand_size);at+=in.operand_size;
 }
 if(at!=sizeof(raw))return LT48_SAFETY_REJECTED;
 static const unsigned char expected[32]={0xcb,0x39,0x3c,0x29,0xa5,0x3d,0x71,0x25,0x2b,0xec,0x88,0x62,0x05,0xb1,0xf1,0x53,0x58,0x22,0xc2,0x85,0x20,0x05,0xcc,0x63,0xc9,0x07,0x76,0xac,0x1b,0x66,0x63,0x55};
 lt48_sha256(raw,sizeof(raw),hash);if(memcmp(hash,expected,sizeof(hash)))return LT48_SAFETY_REJECTED;
 unsigned high=0;
 for(size_t i=0;i<n;i++)if(vars[i].variable_id==6){
  if(vars[i].kind||vars[i].channel!=2||vars[i].value!=550)return LT48_SAFETY_REJECTED;
  high++;
 }
 if(high!=1)return LT48_SAFETY_REJECTED;
 raw[114]=0x26;raw[115]=0x02; /* 600 -> 550 centivolts, no offset changes. */
 lt48_compact_wave *changed=NULL;lt48_status st=lt48_compact_decode(raw,sizeof(raw),&changed);
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,n,1,0,out);
 lt48_compact_wave_close(changed);return st;
}
#endif
