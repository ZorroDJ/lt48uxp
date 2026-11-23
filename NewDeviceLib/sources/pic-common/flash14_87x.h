#ifndef LT48_PIC_FLASH14_87X_H
#define LT48_PIC_FLASH14_87X_H
#include "profile.h"
#include <string.h>
/* Authored DS39025F command contract, distinct from DS39589C A-generation.
 * Logical requests only; entry, power, pin mapping and publication are separate. */
/* Exact legacy source entry/reset shapes are replaced by a bounded normal
 * high-voltage entry. The fixed source digest is enforced by the profile owner. */
static inline lt48_status lt48_pic_87x_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,unsigned role,
 const lt48_pic_classic_protocol *p,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source||!pins||!vars||n!=10||!lt48_pic_classic_is87x(p)||(role!=3&&role!=4)||
    lt48_compact_instruction_count(source)!=(role==3?66u:3u))return LT48_SAFETY_REJECTED;
 lt48_compact_instruction first;
 lt48_status s=lt48_compact_get_instruction(source,0,&first);
 if(s!=LT48_OK)return s;
 if(role==3){
  if(first.opcode!=0x41||first.operand_size!=3||first.operands[0]!=23||first.operands[1]!=4||first.operands[2]!=16)return LT48_SAFETY_REJECTED;
 }else if(first.opcode!=0x29||first.operand_size!=4||first.operands[0]!=14||first.operands[1]!=3)return LT48_SAFETY_REJECTED;
 const unsigned ids[]={0,3,5,6,19},values[]={500,1300,450,550,p->program_words};
 for(unsigned i=0;i<5;i++){
  unsigned found=0;for(size_t j=0;j<n;j++)if(vars[j].variable_id==ids[i]){
   if(vars[j].value!=values[i]||(i<4&&(vars[j].kind!=0||!vars[j].enabled)))return LT48_SAFETY_REJECTED;
   found++;
  }
  if(found!=1)return LT48_SAFETY_REJECTED;
 }
 const unsigned char entry[]={
  0x11,14,0,0x11,23,0,0x11,24,0,
  0x02,1,0,0x02,2,0,0x0d,2,1,0x0d,2,2,0x32,100,0,0,0,
  0x03,14,3,0x00,3,0x32,100,0,0,0,0x40,24,4,0,0,0,0
 };
 const unsigned char exit[]={
  0x11,23,0,0x11,24,0,0x0e,2,1,0,0x0e,2,2,0,0x32,100,0,0,0,
  0x01,3,0,0x32,0xe8,3,0,0,0x36
 };
 const unsigned char *payload=role==3?entry:exit;size_t bytes=role==3?sizeof(entry):sizeof(exit);
 unsigned char raw[128];raw[0]=(unsigned char)bytes;raw[1]=(unsigned char)(bytes>>8);memcpy(raw+2,payload,bytes);
 lt48_compact_wave *changed=NULL;s=lt48_compact_decode(raw,bytes+2,&changed);
 if(s==LT48_OK)s=lt48_compact_compile(changed,pins,vars,n,1,0,out);
 if(s==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))s=LT48_SAFETY_REJECTED;
 if(s!=LT48_OK){lt48_compiled_wave_close(*out);*out=NULL;}
 lt48_compact_wave_close(changed);return s;
}
static inline int lt48_pic_87x_configuration_valid(const lt48_pic_classic_protocol *p,uint16_t value){
 unsigned upper=(value>>12)&3,lower=(value>>4)&3;
 return lt48_pic_classic_is87x(p)&&value<=0x3fff&&upper==lower&&
  (p->program_words!=2048||upper==0||upper==3);
}
static inline lt48_status lt48_pic_87x_program(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,unsigned address,const uint16_t *values,size_t n){
 if(!lt48_pic_classic_is87x(p)||!lt48_pic_classic_wire_valid(w)||!values||
    !lt48_pic_classic_range(p,address,n,1))return LT48_INVALID_ARGUMENT;
 unsigned data=address>=0x2100;
 for(size_t i=0;i<n;i++)if(values[i]>(data?255:0x3fff))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  if(s==LT48_OK)s=lt48_pic_classic_load(w,data?3:2,values[i]);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,0x18);
  if(s==LT48_OK)s=w->delay(w->context,4000);
  if(s==LT48_OK&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_87x_configuration(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,uint16_t value){
 if(!lt48_pic_classic_wire_valid(w))return LT48_INVALID_ARGUMENT;
 if(!lt48_pic_87x_configuration_valid(p,value))return LT48_SAFETY_REJECTED;
 lt48_status s=lt48_pic_classic_seek(w,0x2007);
 /* The programming specification requires RESV bit 11 to remain one. */
 if(s==LT48_OK)s=lt48_pic_classic_load(w,2,value|0xc00);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,0x18);
 if(s==LT48_OK)s=w->delay(w->context,4000);
 return s;
}
static inline lt48_status lt48_pic_87x_erase(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w){
 if(!lt48_pic_classic_is87x(p)||!lt48_pic_classic_wire_valid(w))return LT48_INVALID_ARGUMENT;
 uint16_t cfg=0;
 lt48_status s=lt48_pic_classic_read_range(p,w,0x2007,&cfg,1);
 /* DS39025F 2.5.1 ordinary bulk erase retains IDs/configuration. To provide
  * whole-chip erase, enable CPD when entirely unprotected, check it took, then
  * use 2.5.2 protection reset. Never report success if that prerequisite fails. */
 if(s==LT48_OK&&(cfg&0x3130)==0x3130){
  s=lt48_pic_87x_configuration(p,w,cfg&~0x100);
  if(s==LT48_OK)s=lt48_pic_classic_read_range(p,w,0x2007,&cfg,1);
  if(s==LT48_OK&&(cfg&0x100))s=LT48_DEVICE_MISMATCH;
 }
 if(s==LT48_OK)s=lt48_pic_classic_seek(w,0x2007);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,1);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,7);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,8);
 if(s==LT48_OK)s=w->delay(w->context,8000);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,1);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,7);
 return s;
}
#endif
