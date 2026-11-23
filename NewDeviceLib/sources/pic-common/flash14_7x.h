#ifndef LT48_PIC_FLASH14_7X_H
#define LT48_PIC_FLASH14_7X_H
#include "profile.h"
#include <string.h>
#include "physical_qfn44.h"
static inline lt48_status lt48_pic_7x_select_pinmap(const lt48_pic_profile *p,lt48_pin_map **pins){
 if(!p||!pins||!*pins)return LT48_INVALID_ARGUMENT;
 if(!p->classic14||!lt48_pic_classic_is7x(p->classic14->protocol)||p->pinmap!=14)return LT48_OK;
 if((p->definition!=0x34||p->family!=48)&&(p->definition!=0x38||p->family!=50))return LT48_SAFETY_REJECTED;
 return lt48_pic_qfn44_programming_map(pins);
}
/* Full code/ID/settings plane for legacy code-only definitions. Source
 * metadata is retained; this authored span is exported for admitted aliases. */
static inline int lt48_pic_7x_source_capacity(const lt48_pic_profile *p,unsigned capacity){
 return p&&p->classic14&&lt48_pic_classic_is7x(p->classic14->protocol)&&
  capacity==p->program_words*2&&p->image_span==0x4010&&p->configuration_count==1;
}
static inline lt48_status lt48_pic_7x_entry_compile(const lt48_compact_wave *source,
 const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t n,unsigned role,
 const lt48_pic_classic_protocol *p,lt48_compiled_wave **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;if(!source||!pins||!vars||!lt48_pic_classic_is7x(p)||(role!=3&&role!=4)||n!=(p->configuration_words==1?8u:9u))return LT48_SAFETY_REJECTED;
 if(lt48_compact_instruction_count(source)!=(p->configuration_words==1?(role==3?9u:7u):(role==3?41u:5u)))return LT48_SAFETY_REJECTED;
 const unsigned ids[]={0,3,5,6};const unsigned values[]={500,1300,475,525};
 for(unsigned i=0;i<4;i++){
  unsigned found=0;for(size_t j=0;j<n;j++)if(vars[j].variable_id==ids[i]){
   if(vars[j].kind!=0||!vars[j].enabled||vars[j].value!=values[i])return LT48_SAFETY_REJECTED;
   found++;
  }
  if(found!=1)return LT48_SAFETY_REJECTED;
 }
 const unsigned char entry[]={
  0x11,14,0,0x11,23,0,0x11,24,0,0x03,14,3,0x00,3,0x32,100,0,0,0,
  0x02,1,0,0x02,2,0,0x0d,2,1,0x0d,2,2,0x32,0xf4,1,0,0,
  0x40,24,4,0,0,0,0
 };
 const unsigned char exit[]={
  0x11,23,0,0x11,24,0,0x0e,2,1,0,0x0e,2,2,0,0x32,100,0,0,0,
  0x01,3,0,0x32,0xe8,3,0,0,0x36
 };
 const unsigned char *payload=role==3?entry:exit;size_t bytes=role==3?sizeof(entry):sizeof(exit);
 unsigned char raw[128];raw[0]=(unsigned char)bytes;raw[1]=(unsigned char)(bytes>>8);memcpy(raw+2,payload,bytes);
 lt48_compact_wave *changed=NULL;lt48_status s=lt48_compact_decode(raw,bytes+2,&changed);
 if(s==LT48_OK)s=lt48_compact_compile(changed,pins,vars,n,1,0,out);
 if(s==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*out))s=LT48_SAFETY_REJECTED;
 if(s!=LT48_OK){lt48_compiled_wave_close(*out);*out=NULL;}lt48_compact_wave_close(changed);return s;
}
/* Six-bit source command 08h starts the externally ended cycle. command()
 * supplies one microsecond of guard; use 999 more for the nominal 1-ms hold. */
static inline lt48_status lt48_pic_7x_commit(const lt48_pic_classic_wire *w){
 lt48_status s=lt48_pic_classic_command(w,8);
 if(s==LT48_OK)s=w->delay(w->context,999);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,14);
 return s;
}
static inline lt48_status lt48_pic_7x_program(const lt48_pic_classic_protocol *p,const lt48_pic_classic_wire *w,
 unsigned address,const uint16_t *values,size_t n){
 if(!lt48_pic_classic_is7x(p)||!lt48_pic_classic_wire_valid(w)||!values||!lt48_pic_classic_range(p,address,n,1))return LT48_INVALID_ARGUMENT;
 unsigned code=address<p->program_words;
 if(code&&(address%2||n%2))return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(values[i]>0x3fff)return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  if(!code&&i)s=lt48_pic_classic_seek(w,address+(unsigned)i);
  if(s==LT48_OK)s=lt48_pic_classic_load(w,2,values[i]);
  if(s==LT48_OK&&(!code||(i&1)))s=lt48_pic_7x_commit(w);
  if(s==LT48_OK&&code&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_7x_configuration(const lt48_pic_classic_protocol *p,const lt48_pic_classic_wire *w,
 const uint16_t *values,size_t n){
 if(!lt48_pic_classic_is7x(p)||!lt48_pic_classic_wire_valid(w)||!values||n!=p->configuration_words)return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(values[i]>0x3fff)return LT48_INVALID_ARGUMENT;
 lt48_status s=LT48_OK;
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_seek(w,0x2007+(unsigned)i);
  uint16_t value=values[i]|(0x3fff^p->configuration_mask[i]);
  if(!i&&p->configuration_words==2)value|=0x800; /* RESV must stay one. */
  if(s==LT48_OK)s=lt48_pic_classic_load(w,2,value);
  if(s==LT48_OK)s=lt48_pic_7x_commit(w);
 }
 return s;
}
static inline lt48_status lt48_pic_7x_erase(const lt48_pic_classic_protocol *p,const lt48_pic_classic_wire *w){
 if(!lt48_pic_classic_is7x(p)||!lt48_pic_classic_wire_valid(w))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,0x2000);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,9);
 if(s==LT48_OK)s=w->delay(w->context,30000);
 return s;
}
#endif
