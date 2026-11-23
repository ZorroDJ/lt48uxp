#ifndef LT48_PIC_COMMON_WAIT_H
#define LT48_PIC_COMMON_WAIT_H
#include <labtool48/labtool48.h>
#include "profile.h"
#include <stdlib.h>
#include <string.h>

/* Authored source-bound substitution. Preserve every byte/branch except the
 * nominated delay variable. The selected source delay must satisfy the
 * independently specified lower bound. Caller source hash gates remain required. */
static inline lt48_status lt48_pic_wait_compile(
 const lt48_compact_wave *source,const lt48_pin_map *pins,
 const lt48_compile_variable *vars,size_t var_count,
 const lt48_pic_wait_policy *policy,lt48_compiled_wave **output) {
 if(!output)return LT48_INVALID_ARGUMENT;
 *output=NULL;
 if(!source||!pins||!vars||!policy||!policy->bindings||!policy->count||
    policy->count>256||policy->operation_id>=32)return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<policy->count;i++) {
  const lt48_pic_wait_binding *b=policy->bindings+i;
  if(b->source_offset<2||b->source_variable>255||b->selected_variable>255||
     b->source_variable==b->selected_variable||!b->minimum_us)return LT48_SAFETY_REJECTED;
  unsigned old=0,new=0;
  for(size_t j=0;j<i;j++)if(policy->bindings[j].source_offset==b->source_offset)return LT48_SAFETY_REJECTED;
  for(size_t j=0;j<var_count;j++) {
   if(vars[j].variable_id==b->source_variable||vars[j].variable_id==b->selected_variable) {
    /* This bounded contract admits the source's long-delay representation only. */
    if(vars[j].kind!=1||vars[j].enabled!=1||vars[j].value!=1||
       !vars[j].channel)return LT48_SAFETY_REJECTED;
    if(vars[j].variable_id==b->source_variable)old++;
    else {new++;if((uint64_t)vars[j].channel*100<b->minimum_us)return LT48_SAFETY_REJECTED;}
   }
  }
  if(old!=1||new!=1)return LT48_SAFETY_REJECTED;
 }
 size_t count=lt48_compact_instruction_count(source),size=2;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);
  if(st!=LT48_OK)return st;
  if(size>65534||in.operand_size>65535-size-1)return LT48_SAFETY_REJECTED;
  size+=1+in.operand_size;
 }
 unsigned char *raw=malloc(size);if(!raw)return LT48_NO_MEMORY;
 unsigned char matched[256]={0};size_t used=2;lt48_status st=LT48_OK;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)break;
  raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);
  for(size_t j=0;j<policy->count;j++)if(in.offset==policy->bindings[j].source_offset) {
   const lt48_pic_wait_binding *b=policy->bindings+j;
   if(in.opcode!=0x31||in.operand_size!=1||in.operands[0]!=b->source_variable||matched[j]) {
    st=LT48_SAFETY_REJECTED;break;
   }
   raw[used]=(unsigned char)b->selected_variable;matched[j]=1;
  }
  if(st!=LT48_OK)break;
  used+=in.operand_size;
 }
 for(size_t j=0;st==LT48_OK&&j<policy->count;j++)if(!matched[j])st=LT48_SAFETY_REJECTED;
 lt48_compact_wave *changed=NULL;
 if(st==LT48_OK) {
  raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
  st=lt48_compact_decode(raw,used,&changed);
 }
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,var_count,1,0,output);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*output)) {
  lt48_compiled_wave_close(*output);*output=NULL;st=LT48_SAFETY_REJECTED;
 }
 lt48_compact_wave_close(changed);free(raw);return st;
}
#endif
