#ifndef LT48_PIC_FLASH14_62X_H
#define LT48_PIC_FLASH14_62X_H
#include "profile.h"
/* These source verify-voltage variables are disabled editor fields, not
 * disabled electrical rails. Keep that flag; override their exact values only
 * in this bounded owner instead of weakening the general power-policy guard. */
static inline lt48_status lt48_pic_62x_power_apply(const lt48_pic_power_policy *p,lt48_compile_variable *vars,size_t n){
 if(!p||!vars||p->count!=2||!lt48_pic_power_policy_valid(p))return LT48_SAFETY_REJECTED;
 for(unsigned i=0;i<2;i++){
  const lt48_pic_voltage_binding *b=p->bindings+i;
  if(b->variable_id!=5+i||b->source_value!=(i?600u:400u)||b->selected_value!=(i?550u:450u)||b->minimum!=450||b->maximum!=550)return LT48_SAFETY_REJECTED;
  unsigned found=0;
  for(size_t j=0;j<n;j++)if(vars[j].variable_id==b->variable_id){
   if(vars[j].kind!=0||vars[j].enabled||vars[j].channel!=2||vars[j].value!=b->source_value)return LT48_SAFETY_REJECTED;
   found++;
  }
  if(found!=1)return LT48_SAFETY_REJECTED;
 }
 for(unsigned i=0;i<2;i++)for(size_t j=0;j<n;j++)if(vars[j].variable_id==p->bindings[i].variable_id)vars[j].value=p->bindings[i].selected_value;
 return LT48_OK;
}
/* Shared DS30034D/DS41196G high-voltage entry. Guard source shape,
 * source Size and corrected voltage; keep PGM low throughout entry. */
static inline lt48_status lt48_pic_62x_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,unsigned role,
 const lt48_pic_classic_protocol *p,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars||n!=(p&&p->program_words==4096?10u:11u)||(!lt48_pic_classic_is62x(p)&&!lt48_pic_classic_is62xa(p))||(role!=3&&role!=4))return LT48_SAFETY_REJECTED;
 if(lt48_compact_instruction_count(source)!=(role==3?50u:5u))return LT48_SAFETY_REJECTED;
 const unsigned logical[]={14,21,23,24};
 for(unsigned i=0;i<(role==3?4u:2u);i++){
  lt48_compact_instruction in;lt48_status s=lt48_compact_get_instruction(source,i,&in);if(s!=LT48_OK)return s;
  if(in.opcode!=0x11||in.operand_size!=2||in.operands[0]!=(role==3?logical[i]:logical[i+2])||in.operands[1])return LT48_SAFETY_REJECTED;
 }
 unsigned size=0,vpp=0;
 for(size_t i=0;i<n;i++){
  if(vars[i].variable_id==19){if(vars[i].value!=p->program_words)return LT48_SAFETY_REJECTED;size++;}
  if(vars[i].variable_id==3){if(vars[i].value!=1250)return LT48_SAFETY_REJECTED;vpp++;}
 }
 if(size!=1||vpp!=1)return LT48_SAFETY_REJECTED;
 const unsigned char entry[]={
  0x11,14,0,0x11,21,0,0x11,23,0,0x11,24,0,
  0x03,14,3,0x00,3,0x32,100,0,0,0,
  0x02,1,0,0x02,2,0,0x0d,2,1,0x0d,2,2,0x32,0xf4,1,0,0,
  0x41,22,4,19,0x40,24,4,0,0,0,0
 };
 const unsigned char exit[]={
  0x11,23,0,0x11,24,0,0x11,21,0,
  0x0e,2,1,0,0x0e,2,2,0,0x32,100,0,0,0,0x01,3,0,0x32,0xe8,3,0,0,0x36
 };
 const unsigned char *payload=role==3?entry:exit;size_t bytes=role==3?sizeof(entry):sizeof(exit);
 unsigned char raw[128];raw[0]=(unsigned char)bytes;raw[1]=(unsigned char)(bytes>>8);memcpy(raw+2,payload,bytes);
 lt48_compact_wave *changed=NULL;lt48_status s=lt48_compact_decode(raw,bytes+2,&changed);
 if(s==LT48_OK)s=lt48_compact_compile(changed,pins,vars,n,1,0,out);
 if(s==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))s=LT48_SAFETY_REJECTED;
 if(s!=LT48_OK){lt48_compiled_wave_close(*out);*out=NULL;}lt48_compact_wave_close(changed);return s;
}
#endif
