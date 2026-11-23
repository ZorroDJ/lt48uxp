#ifndef LT48_PIC_COMMON_INTERNAL_PROGRAM_H
#define LT48_PIC_COMMON_INTERNAL_PROGRAM_H
#include "profile.h"
#include <labtool48/labtool48.h>
#include <stdlib.h>
#include <string.h>

/* Opt-in six-bit ICSP policy, DS41390D/DS41439A section 4.3.8.
 * Retain source traversal, latch packing and waits. Change only validated
 * external begin/wait/end triplets into internal begin/wait. The caller must
 * bind the policy to qualified source resources; this helper admits nothing.
 * Rebuild compact structure before compilation, so branch targets are resolved
 * from the new instruction stream rather than patching compiled offsets. */
static inline lt48_status lt48_pic_internal_program_compile_shape(
 const lt48_compact_wave *source,const lt48_pin_map *pins,
 const lt48_compile_variable *vars,size_t var_count,
 unsigned data_pin,unsigned clock_pin,unsigned code_wait,unsigned data_wait,unsigned eeprom_bytes,unsigned source_data_triplets,
 lt48_compiled_wave **output) {
 if(!output)return LT48_INVALID_ARGUMENT;
 *output=NULL;
 if(!source||!pins||!vars||data_pin>255||clock_pin>255||data_pin==clock_pin||
    code_wait>255||data_wait>255||code_wait==data_wait||eeprom_bytes>256||source_data_triplets>1)return LT48_INVALID_ARGUMENT;
 unsigned found_code=0,found_data=0;
 for(size_t i=0;i<var_count;i++) {
  unsigned id=vars[i].variable_id;
  if(id==code_wait||id==data_wait) {
   /* Source kind-one values select scale 100; channel is the count. */
   if(vars[i].kind!=1||vars[i].value!=1||
      (id==code_wait?vars[i].channel<25:vars[i].channel<50))return LT48_SAFETY_REJECTED;
   if(id==code_wait)found_code++;else found_data++;
  }
 }
 if(found_code!=1||found_data!=1)return LT48_SAFETY_REJECTED;
 size_t count=lt48_compact_instruction_count(source),size=2;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);
  if(st!=LT48_OK)return st;
  if(size>65534||in.operand_size>65535-size-1)return LT48_SAFETY_REJECTED;
  size+=1+in.operand_size;
 }
 unsigned char *raw=malloc(size);if(!raw)return LT48_NO_MEMORY;
 size_t used=2;unsigned triplets=0,code_triplets=0,data_triplets=0;
 lt48_status st=LT48_OK;
 for(size_t i=0;i<count;i++) {
  lt48_compact_instruction in;st=lt48_compact_get_instruction(source,i,&in);if(st!=LT48_OK)break;
  if(in.opcode==0x1a&&in.operand_size==8&&in.operands[6]==6) {
   unsigned command=in.operands[2]|(unsigned)in.operands[3]<<8|
    (unsigned)in.operands[4]<<16|(unsigned)in.operands[5]<<24;
   if(command==0x18) {
    lt48_compact_instruction wait,end;
    if(i+2>=count||lt48_compact_get_instruction(source,i+1,&wait)!=LT48_OK||
       lt48_compact_get_instruction(source,i+2,&end)!=LT48_OK||
       in.operands[0]!=data_pin||in.operands[1]!=clock_pin||in.operands[7]!=1||
       wait.opcode!=0x31||wait.operand_size!=1||
       (wait.operands[0]!=code_wait&&wait.operands[0]!=data_wait)||
       end.opcode!=0x1a||end.operand_size!=8||
       end.operands[0]!=data_pin||end.operands[1]!=clock_pin||
       end.operands[2]!=10||end.operands[3]||end.operands[4]||end.operands[5]||
       end.operands[6]!=6||end.operands[7]!=1) {st=LT48_SAFETY_REJECTED;break;}
    raw[used++]=in.opcode;memcpy(raw+used,in.operands,8);raw[used+2]=8;used+=8;
    raw[used++]=wait.opcode;raw[used++]=wait.operands[0];
    triplets++;if(wait.operands[0]==code_wait)code_triplets++;else data_triplets++;
    i+=2;continue;
   }
   if(command==10){st=LT48_SAFETY_REJECTED;break;}
  }
  raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);used+=in.operand_size;
 }
 unsigned expected_data=source_data_triplets;
 if(st==LT48_OK&&(triplets!=2+expected_data||code_triplets!=2||data_triplets!=expected_data))st=LT48_SAFETY_REJECTED;
 lt48_compact_wave *changed=NULL;
 if(st==LT48_OK){raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
  st=lt48_compact_decode(raw,used,&changed);}
 if(st==LT48_OK)st=lt48_compact_compile(changed,pins,vars,var_count,1,0,output);
 if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*output)) {
  lt48_compiled_wave_close(*output);*output=NULL;st=LT48_SAFETY_REJECTED;
 }
 lt48_compact_wave_close(changed);free(raw);return st;
}
static inline lt48_status lt48_pic_internal_program_compile_extent(
 const lt48_compact_wave *source,const lt48_pin_map *pins,
 const lt48_compile_variable *vars,size_t var_count,
 unsigned data_pin,unsigned clock_pin,unsigned code_wait,unsigned data_wait,unsigned eeprom_bytes,
 lt48_compiled_wave **output) {
 return lt48_pic_internal_program_compile_shape(source,pins,vars,var_count,data_pin,clock_pin,
     code_wait,data_wait,eeprom_bytes,eeprom_bytes?1:0,output);
}
static inline lt48_status lt48_pic_internal_program_compile_policy(
 const lt48_compact_wave *source,const lt48_pin_map *pins,
 const lt48_compile_variable *vars,size_t var_count,
 const lt48_pic_internal_program_policy *policy,unsigned eeprom_bytes,lt48_compiled_wave **output) {
 if(!output)return LT48_INVALID_ARGUMENT;
 *output=NULL;
 if(!policy||policy->data_wait>511)return LT48_INVALID_ARGUMENT;
 unsigned dormant=policy->data_wait&LT48_PIC_INTERNAL_DORMANT_DATA;
 if(dormant&&eeprom_bytes)return LT48_SAFETY_REJECTED;
 return lt48_pic_internal_program_compile_shape(source,pins,vars,var_count,policy->data_pin,
     policy->clock_pin,policy->code_wait,lt48_pic_internal_data_wait(policy),eeprom_bytes,
     dormant?1:eeprom_bytes?1:0,output);
}
/* Preserve the original EEPROM-bearing source-shape contract for callers
 * that have not explicitly supplied an independently qualified extent. */
static inline lt48_status lt48_pic_internal_program_compile(
 const lt48_compact_wave *source,const lt48_pin_map *pins,
 const lt48_compile_variable *vars,size_t var_count,
 unsigned data_pin,unsigned clock_pin,unsigned code_wait,unsigned data_wait,
 lt48_compiled_wave **output) {
 return lt48_pic_internal_program_compile_extent(source,pins,vars,var_count,
  data_pin,clock_pin,code_wait,data_wait,256,output);
}
#endif
