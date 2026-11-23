#ifndef LT48_PIC_COMMON_OTP14_WORD_H
#define LT48_PIC_COMMON_OTP14_WORD_H
#include <labtool48/labtool48.h>
#include <string.h>

/* DS30228K figure 2-2: N starts at one, then 3*N overprogram cycles.
 * Section 2.2.1.5 requires Load Data before every Begin Programming.
 * This is an owned correction of one exact shared source component; it
 * grants no device admission, package, power, configuration or protection.
 * The host must separately verify at the specified VDD limits. */
static inline lt48_status lt48_pic_otp14_word_compile(
 const lt48_compact_wave *source,const lt48_pin_map *pins,
 const lt48_compile_variable *vars,size_t count,lt48_compiled_wave **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars)return LT48_INVALID_ARGUMENT;
 const unsigned char expected[32]={
  0x2f,0x93,0x69,0xd5,0x25,0xcb,0xe5,0xae,0x31,0x95,0xf5,0x42,0x7c,0x7b,0xf0,0x8b,
  0xcd,0x44,0xfb,0x24,0x54,0x19,0xbd,0x53,0x74,0x8b,0xad,0xc3,0xd8,0x9c,0x18,0xbb};
 unsigned char original[175]={173,0};size_t used=2;
 if(lt48_compact_instruction_count(source)!=35)return LT48_SAFETY_REJECTED;
 for(size_t i=0;i<35;i++) {
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);
  if(st!=LT48_OK)return st;
  if(used>=sizeof(original)||in.operand_size>sizeof(original)-used-1)return LT48_SAFETY_REJECTED;
  original[used++]=in.opcode;memcpy(original+used,in.operands,in.operand_size);used+=in.operand_size;
 }
 if(used!=sizeof(original))return LT48_SAFETY_REJECTED;
 unsigned char hash[32];lt48_sha256(original,used,hash);
 if(memcmp(hash,expected,sizeof(hash)))return LT48_SAFETY_REJECTED;
 unsigned timing=0,retry=0;
 for(size_t i=0;i<count;i++) {
  if(vars[i].variable_id==8) {
   if(vars[i].kind!=1||vars[i].enabled!=1||vars[i].value!=0||vars[i].channel!=100)return LT48_SAFETY_REJECTED;
   timing++;
  }
  if(vars[i].variable_id==17) {
   if(vars[i].kind!=2||vars[i].enabled!=1||vars[i].value!=25||vars[i].channel)return LT48_SAFETY_REJECTED;
   retry++;
  }
 }
 if(timing!=1||retry!=1)return LT48_SAFETY_REJECTED;
 unsigned char raw[210]={0};used=2;
 /* Keep the original control structure, but reload within each pulse loop.
  * The exact hash gate above binds every offset, operand and branch source. */
 for(size_t i=0x25;i<sizeof(original);i++) {
  if(i==0x37||i==0x85){memcpy(raw+used,original+2,0x23);used+=0x23;}
  raw[used++]=i==0x2c?1:original[i];
 }
 if(used!=sizeof(raw))return LT48_SAFETY_REJECTED;
 raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
 lt48_compact_wave *changed=NULL;lt48_status st=lt48_compact_decode(raw,used,&changed);
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,count,1,0,out);
 lt48_compact_wave_close(changed);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK&&*out){lt48_compiled_wave_close(*out);*out=NULL;}
 return st;
}
#endif
