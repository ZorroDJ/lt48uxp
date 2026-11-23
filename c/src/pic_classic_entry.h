#ifndef LT48_PIC_CLASSIC_ENTRY_H
#define LT48_PIC_CLASSIC_ENTRY_H
/* Authored 88x electrical request sequence, DS41287D figures 3-1/3-3.
 * Preserve source pin groups/DAC variables, use VPP-first entry and disconnect
 * both VDD banks before removing VPP. Analog transition times need hardware
 * qualification; byte-level requests cannot prove a waveform. */
static lt48_status pic_classic_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,unsigned role,
 lt48_compiled_wave **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;if(!source||!pins||!vars||n!=11||(role!=3&&role!=4))return LT48_SAFETY_REJECTED;
 size_t count=lt48_compact_instruction_count(source);
 if(count!=(role==3?49u:5u))return LT48_SAFETY_REJECTED;
 /* Exact source clock/data and exit shape guard, in addition to the owner's
  * fixed REZ hash and generated exact profile binding. */
 lt48_compact_instruction in;
 for(unsigned i=0;i<2;i++) {
  lt48_status s=lt48_compact_get_instruction(source,role==3?i+2:i,&in);if(s!=LT48_OK)return s;
  if(in.opcode!=0x11||in.operand_size!=2||in.operands[0]!=(i?24:23)||in.operands[1])return LT48_SAFETY_REJECTED;
 }
 if(role==4) {
  const unsigned char op[]={0x29,0x32,0x36};
  const unsigned char size[]={4,4,0};
  const unsigned char operands[][4]={{14,3,0,13},{0xe8,3,0,0},{0}};
  for(unsigned i=0;i<3;i++) {
   lt48_status s=lt48_compact_get_instruction(source,i+2,&in);if(s!=LT48_OK)return s;
   if(in.opcode!=op[i]||in.operand_size!=size[i]||memcmp(in.operands,operands[i],size[i]))return LT48_SAFETY_REJECTED;
  }
 }
 const unsigned ids[]={0,3,13,18,19,20};unsigned found[6]={0};
 for(size_t i=0;i<n;i++)for(unsigned j=0;j<6;j++)if(vars[i].variable_id==ids[j])found[j]++;
 for(unsigned i=0;i<6;i++)if(found[i]!=1)return LT48_SAFETY_REJECTED;
 const unsigned char entry[]={
  0x11,23,0,0x11,24,0,0x11,14,0,
  0x03,14,3,0x00,3,0x32,100,0,0,0,
  0x02,1,0,0x02,2,0,0x0d,2,1,0x0d,2,2,0x32,0xf4,1,0,0,
  0x41,23,4,18,0x41,22,4,19,0x41,29,4,20,0x40,24,4,0,0,0,0
 };
 const unsigned char exit[]={
  0x11,23,0,0x11,24,0,0x0e,2,1,0,0x0e,2,2,0,0x32,100,0,0,0,
  0x29,14,3,0,13,0x32,0xe8,3,0,0,0x36
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
