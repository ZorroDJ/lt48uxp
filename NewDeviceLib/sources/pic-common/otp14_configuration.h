#ifndef LT48_OTP14_CONFIGURATION_H
#define LT48_OTP14_CONFIGURATION_H
#include <labtool48/labtool48.h>
#include <stdio.h>
#include <string.h>
#include "otp14_configuration_sources.h"
/* DS30228K figure2-3: retain the source's 100 configuration cycles,
 * but obey 2.2.1.5 by reloading before every Begin. Exact source and
 * loop/data-frame shapes are mandatory. For role10 the owner supplies its
 * validated canonical target in expected-value buffer6; copy it to the data
 * latch buffer instead of inheriting the source's force-full-protect encoding.
 * No operation admission is granted. */
static inline int otp14_instruction_is(const lt48_compact_wave *s,size_t i,
 unsigned op,const unsigned char *args,size_t n) {
 lt48_compact_instruction in;
 return lt48_compact_get_instruction(s,i,&in)==LT48_OK&&in.opcode==op&&
  in.operand_size==n&&(!n||!memcmp(in.operands,args,n));
}
static inline lt48_status otp14_configuration_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t count,
 unsigned role,lt48_compiled_wave **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;if(!source||!pins||!vars||(role!=10&&role!=14))return LT48_INVALID_ARGUMENT;
 unsigned char original[4096]={0},raw[4143]={0};size_t used=2,n=lt48_compact_instruction_count(source);
 for(size_t i=0;i<n;i++) {
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);
  if(st!=LT48_OK)return st;
  if(used>=sizeof(original)||in.operand_size>sizeof(original)-used-1)return LT48_SAFETY_REJECTED;
  original[used++]=in.opcode;memcpy(original+used,in.operands,in.operand_size);used+=in.operand_size;
 }
 original[0]=(unsigned char)(used-2);original[1]=(unsigned char)((used-2)>>8);
 unsigned char sha[32];char hex[65];lt48_sha256(original,used,sha);
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",sha[i]);
 int allowed=0;
 for(size_t i=0;i<sizeof(otp14_configuration_sources)/sizeof(*otp14_configuration_sources);i++)
  if(otp14_configuration_sources[i].role==role&&!strcmp(hex,otp14_configuration_sources[i].sha256))allowed=1;
 if(!allowed)return LT48_SAFETY_REJECTED;
 unsigned timing=0;
 for(size_t i=0;i<count;i++)if(vars[i].variable_id==8){
  if(vars[i].kind!=1||vars[i].enabled!=1||vars[i].value!=0||vars[i].channel!=100)return LT48_SAFETY_REJECTED;
  timing++;
 }
 if(timing!=1)return LT48_SAFETY_REJECTED;
 const unsigned char load[]={24,23,2,0,0,0,6,1},begin[]={24,23,8,0,0,0,6,1};
 const unsigned char end[]={24,23,14,0,0,0,6,1},zero[]={24,23,0,0,0,0,1,1};
 const unsigned char payload[]={24,23,11,14,1},initial[]={12,4,100,0,0,0},loop[]={12,4,0,0,0,0};
 const unsigned char setup[]={12},wait[]={8},counter[]={12};
 size_t load_index=n,begin_index=n;unsigned loads=0,begins=0;
 for(size_t i=0;i<n;i++){
  if(otp14_instruction_is(source,i,0x1a,load,sizeof(load))){load_index=i;loads++;}
  if(otp14_instruction_is(source,i,0x1a,begin,sizeof(begin))){begin_index=i;begins++;}
 }
 if(loads!=1||begins!=1||load_index+4>=n||begin_index<2||begin_index+4>=n||
  !otp14_instruction_is(source,load_index+1,0x31,setup,1)||
  !otp14_instruction_is(source,load_index+2,0x1a,zero,8)||
  !otp14_instruction_is(source,load_index+3,0x19,payload,5)||
  !otp14_instruction_is(source,load_index+4,0x1a,zero,8)||
  !otp14_instruction_is(source,begin_index-2,0x40,initial,6)||
  !otp14_instruction_is(source,begin_index-1,0x62,loop,6)||
  !otp14_instruction_is(source,begin_index+1,0x31,wait,1)||
  !otp14_instruction_is(source,begin_index+2,0x1a,end,8)||
  !otp14_instruction_is(source,begin_index+3,0x4f,counter,1)||
  !otp14_instruction_is(source,begin_index+4,0x63,NULL,0))return LT48_SAFETY_REJECTED;
 unsigned char reload[35];size_t r=0;
 for(size_t i=load_index;i<load_index+5;i++){
  lt48_compact_instruction in;lt48_compact_get_instruction(source,i,&in);
  reload[r++]=in.opcode;memcpy(reload+r,in.operands,in.operand_size);r+=in.operand_size;
 }
 if(r!=sizeof(reload))return LT48_SAFETY_REJECTED;
 size_t added=2;
 const unsigned char target[]={0x49,11,0,6,0,2};
 for(size_t i=0;i<n;i++){
  lt48_compact_instruction in;lt48_compact_get_instruction(source,i,&in);
  if(role==10&&i==load_index){memcpy(raw+added,target,sizeof(target));added+=sizeof(target);}
  if(i==begin_index){
   if(role==10){memcpy(raw+added,target,sizeof(target));added+=sizeof(target);}
   memcpy(raw+added,reload,r);added+=r;
  }
  raw[added++]=in.opcode;memcpy(raw+added,in.operands,in.operand_size);added+=in.operand_size;
 }
 if(added!=used+sizeof(reload)+(role==10?2*sizeof(target):0))return LT48_SAFETY_REJECTED;
 raw[0]=(unsigned char)(added-2);raw[1]=(unsigned char)((added-2)>>8);
 lt48_compact_wave *changed=NULL;lt48_status st=lt48_compact_decode(raw,added,&changed);
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,count,1,0,out);
 lt48_compact_wave_close(changed);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))st=LT48_SAFETY_REJECTED;
 if(st!=LT48_OK&&*out){lt48_compiled_wave_close(*out);*out=NULL;}
 return st;
}
#endif
