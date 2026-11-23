#ifndef LT48_PIC_COMMON_ENTRY_H
#define LT48_PIC_COMMON_ENTRY_H
#include <labtool48/labtool48.h>
#include "profile.h"
#include <stdlib.h>
#include <string.h>

/* Require two leading pin-low instructions; add one controller quiet interval.
 * Recompile structured control flow and require every original compiled word
 * after the prefix to survive unchanged. Branches may be present, but altered
 * targets, instructions or buffer operations are rejected. Source digest and
 * no-default-pin gates remain mandatory; this transform admits no device. */
static inline lt48_status lt48_pic_entry_compile_quiet(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t var_count,
 const lt48_pic_entry_policy *policy,lt48_compiled_wave **output) {
 if(!output)return LT48_INVALID_ARGUMENT;
 *output=NULL;
 if(!source||!pins||!vars||!policy||policy->operation_id>=32||
    policy->clock_pin>255||policy->data_pin>255||policy->clock_pin==policy->data_pin||
    !lt48_pic_entry_quiet_us(policy)||lt48_pic_entry_quiet_us(policy)>1000000)return LT48_INVALID_ARGUMENT;
 uint32_t quiet=lt48_pic_entry_quiet_us(policy);
 size_t count=lt48_compact_instruction_count(source),size=7;
 if(count<2)return LT48_SAFETY_REJECTED;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);
  if(st!=LT48_OK)return st;
  if(i<2 && (in.opcode!=0x11 || in.operand_size!=2 || in.operands[1] ||
      in.operands[0]!=(i?policy->data_pin:policy->clock_pin)))return LT48_SAFETY_REJECTED;
  if(size>65534||in.operand_size>65535-size-1)return LT48_SAFETY_REJECTED;
  size+=1+in.operand_size;
 }
 unsigned char *raw=malloc(size);if(!raw)return LT48_NO_MEMORY;
 size_t used=2;lt48_status st=LT48_OK;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)break;
  raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);used+=in.operand_size;
  if(i==1) {
   raw[used++]=0x32;
   for(unsigned j=0;j<4;j++)raw[used++]=(unsigned char)(quiet>>(8*j));
  }
 }
 lt48_compact_wave *changed=NULL;
 if(st==LT48_OK){raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
  st=lt48_compact_decode(raw,used,&changed);}
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,var_count,1,0,output);
 lt48_compiled_wave *original=NULL;
 if(st==LT48_OK)st=lt48_compact_compile(source,pins,vars,var_count,1,0,&original);
 if(st==LT48_OK) {
  size_t before_count=0,after_count=0;
  const uint16_t *before=lt48_compiled_wave_words(original,&before_count);
  const uint16_t *after=lt48_compiled_wave_words(*output,&after_count);
  /* Count header + two GPIO instructions = five words. The inserted fixed
   * delay is exactly three words; all suffix words, including branch offsets,
   * must be byte-identical to the separately compiled original. */
  if(before_count<5||after_count!=before_count+3||
     memcmp(before+1,after+1,4*sizeof(*before))||after[5]!=0x24||
     after[6]!=(uint16_t)quiet||after[7]!=(uint16_t)(quiet>>16)||
     memcmp(before+5,after+8,(before_count-5)*sizeof(*before)))st=LT48_SAFETY_REJECTED;
 }
 if(st==LT48_OK && lt48_compiled_wave_defaulted_pin_count(*output)) {
  lt48_compiled_wave_close(*output);*output=NULL;st=LT48_SAFETY_REJECTED;
 }
 if(st!=LT48_OK && *output){lt48_compiled_wave_close(*output);*output=NULL;}
 lt48_compiled_wave_close(original);lt48_compact_wave_close(changed);free(raw);return st;
}
/* Explicit manufacturer ID/revision normalization. Require the complete
 * raw14-read / source-literal / comparison / mismatch-return shape.
 * Only insert two right shifts; every original instruction is retained.
 * Structured compact branches are recompiled, not patched as byte offsets. */
/* Replace exactly the source's byte-only identity tail with the independently
 * specified full device-bit comparison. All preceding operations and selector
 * variables survive. Every read bit and expected bit has an explicit origin;
 * revisions are omitted, while the complete remaining ID width is compared. */
static inline lt48_status lt48_pic_entry_compile_identity(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t var_count,
 const lt48_pic_entry_policy *policy,lt48_compiled_wave **output) {
 unsigned shift=lt48_pic_entry_revision_bits(policy),variable=lt48_pic_entry_id_variable(policy);
 if(!shift||shift>=14||policy->id_mask!=(0x3fffu&~((1u<<shift)-1u))||
    (policy->id_expected&~policy->id_mask))return LT48_INVALID_ARGUMENT;
 unsigned found=0;
 for(size_t i=0;i<var_count;i++)if(vars[i].variable_id==variable) {
  if(vars[i].kind!=2||vars[i].enabled||vars[i].channel||vars[i].value!=policy->id_source_value)
   return LT48_SAFETY_REJECTED;
  found++;
 }
 if(found!=1)return LT48_SAFETY_REJECTED;
 const unsigned char expected[][8]={
  {(unsigned char)policy->clock_pin,1},
  {(unsigned char)policy->data_pin,(unsigned char)policy->clock_pin,12,14,1},
  {(unsigned char)policy->data_pin,(unsigned char)policy->clock_pin,0,0,0,0,1,1},
  {11,0,12,(unsigned char)shift,8},
  {1,4,(unsigned char)variable},{11,1,1},{6,0},{0},{0}
 };
 const unsigned char opcode[]={0x2b,0x1b,0x1a,0x45,0x41,0x5d,0x6c,0x6b,0x60};
 const unsigned char lengths[]={2,5,8,5,3,3,2,1,0};
 size_t count=lt48_compact_instruction_count(source),match=0,size=12;unsigned matches=0,uses=0;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)return st;
  if(size>65534||in.operand_size>65535-size-1)return LT48_SAFETY_REJECTED;
  size+=1+in.operand_size;
  if(in.opcode==0x41&&in.operand_size==3&&in.operands[2]==variable)uses++;
 }
 for(size_t start=2;start+9<=count;start++) {
  unsigned same=1;
  for(unsigned j=0;j<9;j++) {
   lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,start+j,&in);if(st!=LT48_OK)return st;
   if(in.opcode!=opcode[j]||in.operand_size!=lengths[j]||memcmp(in.operands,expected[j],lengths[j])){same=0;break;}
  }
  if(same){match=start;matches++;}
 }
 if(matches!=1||uses!=1)return LT48_SAFETY_REJECTED;
 unsigned char *raw=malloc(size);if(!raw)return LT48_NO_MEMORY;
 size_t used=2;lt48_status st=LT48_OK;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)break;
  if(i==match+3) {
   const unsigned char zero[]={0x40,11,4,0,0,0,0};memcpy(raw+used,zero,sizeof(zero));used+=sizeof(zero);
  }
  if(i==match+4) {
   unsigned id=policy->id_expected>>shift;
   const unsigned char literal[]={0x40,1,4,(unsigned char)id,(unsigned char)(id>>8),0,0};
   memcpy(raw+used,literal,sizeof(literal));used+=sizeof(literal);
  }else {
   raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);
   if(i==match+3)raw[used+4]=(unsigned char)(14-shift);
   used+=in.operand_size;
  }
 }
 lt48_compact_wave *changed=NULL;
 if(st==LT48_OK){raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
  st=lt48_compact_decode(raw,used,&changed);}
 if(st==LT48_OK)st=lt48_pic_entry_compile_quiet(changed,pins,vars,var_count,policy,output);
 lt48_compact_wave_close(changed);free(raw);return st;
}

static inline lt48_status lt48_pic_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t var_count,
 const lt48_pic_entry_policy *policy,lt48_compiled_wave **output) {
 if(!output)return LT48_INVALID_ARGUMENT;
 *output=NULL;
 if(!source||!pins||!vars||!policy)return LT48_INVALID_ARGUMENT;
 if(policy->id_mask)return lt48_pic_entry_compile_identity(source,pins,vars,var_count,policy,output);
 unsigned shift=lt48_pic_entry_revision_bits(policy),variable=lt48_pic_entry_id_variable(policy);
 if(shift>=14 || (!shift&&variable))return LT48_INVALID_ARGUMENT;
 if(!shift)return lt48_pic_entry_compile_quiet(source,pins,vars,var_count,policy,output);
 unsigned found=0;
 for(size_t i=0;i<var_count;i++)if(vars[i].variable_id==variable) {
  if(vars[i].kind!=2 || vars[i].enabled || vars[i].channel || vars[i].value>0x3fff)return LT48_SAFETY_REJECTED;
  found++;
 }
 if(found!=1)return LT48_SAFETY_REJECTED;
 size_t count=lt48_compact_instruction_count(source),size=14,match=0;
 if(count<9)return LT48_SAFETY_REJECTED;
 const unsigned char expected[][8]={
  {(unsigned char)policy->clock_pin,1},
  {(unsigned char)policy->data_pin,(unsigned char)policy->clock_pin,12,14,1},
  {(unsigned char)policy->data_pin,(unsigned char)policy->clock_pin,0,0,0,0,1,1},
  {1,4,(unsigned char)variable},{12,1,1},{6,0},{0},{0}
 };
 const unsigned char opcode[]={0x2b,0x1b,0x1a,0x41,0x5d,0x6c,0x6b,0x60};
 const unsigned char lengths[]={2,5,8,3,3,2,1,0};
 unsigned matches=0;
 for(size_t start=2;start+8<=count;start++) {
  unsigned same=1;
  for(unsigned j=0;j<8;j++) {
   lt48_compact_instruction in;
   lt48_status st=lt48_compact_get_instruction(source,start+j,&in);if(st!=LT48_OK)return st;
   if(in.opcode!=opcode[j] || in.operand_size!=lengths[j] ||
      memcmp(in.operands,expected[j],lengths[j])){same=0;break;}
  }
  if(same){match=start;matches++;}
 }
 if(matches!=1)return LT48_SAFETY_REJECTED;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;
  lt48_status st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)return st;
  if(size>65534 || in.operand_size>65535-size-1)return LT48_SAFETY_REJECTED;
  size+=1+in.operand_size;
 }
 unsigned char *raw=malloc(size);if(!raw)return LT48_NO_MEMORY;
 size_t used=2;lt48_status st=LT48_OK;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)break;
  raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);used+=in.operand_size;
  if(i==match+3) {
   const unsigned char shifts[]={0x45,12,0,12,(unsigned char)shift,8,
                                0x45,1,0,1,(unsigned char)shift,8};
   memcpy(raw+used,shifts,sizeof(shifts));used+=sizeof(shifts);
  }
 }
 lt48_compact_wave *normalized=NULL;
 if(st==LT48_OK) {raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
  st=lt48_compact_decode(raw,used,&normalized);}
 if(st==LT48_OK)st=lt48_pic_entry_compile_quiet(normalized,pins,vars,var_count,policy,output);
 lt48_compact_wave_close(normalized);free(raw);return st;
}
#endif
