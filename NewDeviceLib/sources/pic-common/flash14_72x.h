#ifndef LT48_PIC_FLASH14_72X_H
#define LT48_PIC_FLASH14_72X_H
#include "profile.h"
#include "physical_qfn44.h"
/* Authored 1:1 QFN44 programming map, DS41332D figure 2-4.
 * The original P5E adapter map is non-geometric; replace it only for these
 * exact two 72X definitions. All six supply pins are retained. */
static inline lt48_status lt48_pic_72x_select_pinmap(const lt48_pic_profile *p,lt48_pin_map **pins){
 if(!p||!pins||!*pins)return LT48_INVALID_ARGUMENT;
 if(!p->classic14||!lt48_pic_classic_is72x(p->classic14->protocol)||p->pinmap!=94)return LT48_OK;
 if((p->definition!=0x17b||p->family!=264)&&(p->definition!=0x17e||p->family!=265))return LT48_SAFETY_REJECTED;
 if(lt48_pin_map_package_pin_count(*pins)!=44)return LT48_SAFETY_REJECTED;
 return lt48_pic_qfn44_programming_map(pins);
}

/* DS41332D VPP-first entry: clock/data low before VIHH, 8--9 V VPP,
 * then VDD and at least 250 us hold. Exact source library and owner bindings
 * guard the original callbacks; this authored entry removes the optional
 * truncated source ID check, which the owner replaces with the full masked ID.
 * Source Size is a last address, not the canonical implemented-word count. */
static inline lt48_status lt48_pic_72x_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,unsigned role,
 const lt48_pic_classic_protocol *p,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars||n!=9||!lt48_pic_classic_is72x(p)||(role!=3&&role!=4))return LT48_SAFETY_REJECTED;
 if(lt48_compact_instruction_count(source)!=(role==3?47u:8u))return LT48_SAFETY_REJECTED;
 for(unsigned i=0;i<2;i++){
  lt48_compact_instruction in;lt48_status s=lt48_compact_get_instruction(source,i,&in);if(s!=LT48_OK)return s;
  if(in.opcode!=0x11||in.operand_size!=2||in.operands[0]!=(i?24:23)||in.operands[1])return LT48_SAFETY_REJECTED;
 }
 unsigned size=0,vpp=0;
 for(size_t i=0;i<n;i++){
  if(vars[i].variable_id==19){if(vars[i].value!=p->program_words-1)return LT48_SAFETY_REJECTED;size++;}
  if(vars[i].variable_id==3){if(vars[i].value!=850)return LT48_SAFETY_REJECTED;vpp++;}
 }
 if(size!=1||vpp!=1)return LT48_SAFETY_REJECTED;
 unsigned char entry[]={
  0x11,23,0,0x11,24,0,0x11,14,0,0x03,14,3,0x00,3,0x32,100,0,0,0,
  0x02,1,0,0x02,2,0,0x0d,2,1,0x0d,2,2,0x32,0xf4,1,0,0,
  0x40,22,4,0,0,0,0,0x40,24,4,0,0,0,0
 };
 /* Full code extent is deliberately seeded as a count in the owned context. */
 for(unsigned i=0;i<4;i++)entry[39+i]=(unsigned char)(p->program_words>>(8*i));
 const unsigned char exit[]={
  0x11,23,0,0x11,24,0,0x0e,2,1,0,0x0e,2,2,0,0x32,100,0,0,0,
  0x01,3,0,0x32,0xe8,3,0,0,0x36
 };
 const unsigned char *payload=role==3?entry:exit;size_t bytes=role==3?sizeof(entry):sizeof(exit);
 unsigned char raw[128];raw[0]=(unsigned char)bytes;raw[1]=(unsigned char)(bytes>>8);memcpy(raw+2,payload,bytes);
 lt48_compact_wave *changed=NULL;lt48_status s=lt48_compact_decode(raw,bytes+2,&changed);
 if(s==LT48_OK)s=lt48_compact_compile(changed,pins,vars,n,1,0,out);
 if(s==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))s=LT48_SAFETY_REJECTED;
 if(s!=LT48_OK){lt48_compiled_wave_close(*out);*out=NULL;}
 lt48_compact_wave_close(changed);return s;
}
#endif
