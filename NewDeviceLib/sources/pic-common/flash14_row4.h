#ifndef LT48_PIC_FLASH14_ROW4_H
#define LT48_PIC_FLASH14_ROW4_H
#include "profile.h"
#include <string.h>
/* Shared normal-HV DS39603C/DS39607C engine. Exact source bindings and
 * manufacturer/package gates are required before any catalogue admission. */
static inline lt48_status lt48_pic_row4_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,unsigned role,
 const lt48_pic_classic_protocol *p,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars||n!=9||!lt48_pic_classic_isrow4(p)||(role!=3&&role!=4)||
    lt48_compact_instruction_count(source)!=(role==3?(p->configuration_words==1?43u:42u):5u))return LT48_SAFETY_REJECTED;
 const unsigned logical[]={14,20,23,24};
 for(unsigned i=0;i<(role==3?4u:2u);i++){
  lt48_compact_instruction in;lt48_status s=lt48_compact_get_instruction(source,i,&in);if(s!=LT48_OK)return s;
  if(in.opcode!=0x11||in.operand_size!=2||in.operands[0]!=(role==3?logical[i]:logical[i+2])||in.operands[1])return LT48_SAFETY_REJECTED;
 }
 const unsigned ids[]={0,3,5,6,19},values[]={500,p->configuration_words==2?1300u:1200u,450,550,p->program_words};
 for(unsigned i=0;i<5;i++){
  unsigned found=0;for(size_t j=0;j<n;j++)if(vars[j].variable_id==ids[i]){
   if(vars[j].value!=values[i]||(i<4&&(vars[j].kind!=0||!vars[j].enabled)))return LT48_SAFETY_REJECTED;
   found++;
  }
  if(found!=1)return LT48_SAFETY_REJECTED;
 }
 /* VPP-first prevents internal-RC execution before programming entry. */
 const unsigned char entry[]={
  0x11,14,0,0x11,20,0,0x11,23,0,0x11,24,0,
  0x03,14,3,0x00,3,0x32,100,0,0,0,
  0x02,1,0,0x02,2,0,0x0d,2,1,0x0d,2,2,0x32,0xf4,1,0,0,
  0x40,24,4,0,0,0,0
 };
 const unsigned char exit[]={
  0x11,23,0,0x11,24,0,0x11,20,0,
  0x0e,2,1,0,0x0e,2,2,0,0x32,100,0,0,0,
  0x01,3,0,0x32,0xe8,3,0,0,0x36
 };
 const unsigned char *payload=role==3?entry:exit;size_t bytes=role==3?sizeof(entry):sizeof(exit);
 unsigned char raw[128];raw[0]=(unsigned char)bytes;raw[1]=(unsigned char)(bytes>>8);memcpy(raw+2,payload,bytes);
 lt48_compact_wave *changed=NULL;lt48_status s=lt48_compact_decode(raw,bytes+2,&changed);
 if(s==LT48_OK)s=lt48_compact_compile(changed,pins,vars,n,1,0,out);
 if(s==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))s=LT48_SAFETY_REJECTED;
 if(s!=LT48_OK){lt48_compiled_wave_close(*out);*out=NULL;}lt48_compact_wave_close(changed);return s;
}
static inline lt48_status lt48_pic_row4_commit(const lt48_pic_classic_wire *w){
 lt48_status s=lt48_pic_classic_command(w,0x18);
 if(s==LT48_OK)s=w->delay(w->context,3000);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,0x17);
 return s;
}
static inline lt48_status lt48_pic_row4_program(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,unsigned address,const uint16_t *values,size_t n){
 if(!lt48_pic_classic_isrow4(p)||!lt48_pic_classic_wire_valid(w)||!values||!lt48_pic_classic_range(p,address,n,1))return LT48_INVALID_ARGUMENT;
 unsigned code=address<p->program_words,data=address>=0x2100;
 if(code&&(address%4||n%4))return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(values[i]>(data?255:0x3fff))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  if(!code&&!data&&i)s=lt48_pic_classic_seek(w,address+(unsigned)i);
  if(s==LT48_OK)s=lt48_pic_classic_load(w,data?3:2,values[i]);
  if(s==LT48_OK&&(!code||((i+1)%4==0)))s=lt48_pic_row4_commit(w);
  if(s==LT48_OK&&i+1<n&&(code||data))s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_row4_configuration(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,const uint16_t *values,size_t n){
 if(!lt48_pic_classic_isrow4(p)||!lt48_pic_classic_wire_valid(w)||!values||n!=p->configuration_words)return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(values[i]>0x3fff)return LT48_INVALID_ARGUMENT;
 lt48_status s=LT48_OK;
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_seek(w,0x2007+(unsigned)i);
  if(s==LT48_OK)s=lt48_pic_classic_load(w,2,values[i]|(0x3fff^p->configuration_mask[i]));
  if(s==LT48_OK)s=lt48_pic_row4_commit(w);
 }
 return s;
}
static inline lt48_status lt48_pic_row4_erase(const lt48_pic_classic_protocol *p,const lt48_pic_classic_wire *w){
 if(!lt48_pic_classic_isrow4(p)||!lt48_pic_classic_wire_valid(w))return LT48_INVALID_ARGUMENT;
 /* CFG-domain selection includes IDs/EEPROM on both generations. */
 lt48_status s=lt48_pic_classic_seek(w,0x2000);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,0x1f);
 if(s==LT48_OK)s=w->delay(w->context,10000);
 return s;
}
#endif
