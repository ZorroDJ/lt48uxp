#ifndef LT48_PIC_BASELINE12_PROTOCOL_H
#define LT48_PIC_BASELINE12_PROTOCOL_H
#include "classic14_protocol.h"
/* Private common baseline Flash wire owner. Address FFF is a file convention;
 * a fresh entry selects the physical configuration register. Admission still
 * requires whole C/GUI operations, original host preservation and power/pins. */
typedef struct {
 unsigned code_words,pc_mask,id_base,backup_address,data_words;
 unsigned configuration_mask,code_protect,data_protect,calibration;
 unsigned program_us,discharge_us,erase_us,backup_words;
} lt48_pic_baseline12_policy;
static inline int lt48_pic_baseline12_valid(const lt48_pic_baseline12_policy *p){
 return p&&(p->code_words==256||p->code_words==512||p->code_words==1024||p->code_words==2048)&&
  p->pc_mask==2*p->code_words-1&&(p->data_words==0||p->data_words==64)&&
  p->id_base==p->code_words+p->data_words&&p->id_base+8<p->pc_mask&&
  p->calibration<=1&&(p->calibration?p->backup_address==p->id_base+4&&(p->backup_words==1||p->backup_words==4):!p->backup_address&&!p->backup_words)&&
  p->configuration_mask&&p->configuration_mask<=0xfff&&p->code_protect&&
  !((p->code_protect|p->data_protect)&~p->configuration_mask)&&
  (p->data_words?!!p->data_protect:!p->data_protect)&&p->program_us>=2000&&p->program_us<=10000&&
  (p->discharge_us==100||p->discharge_us==300)&&p->erase_us>=10000&&p->erase_us<=30000;
}
static inline int lt48_pic_baseline12_address(const lt48_pic_baseline12_policy *p,unsigned a,int write){
 if(a==0xfff)return 1;
 if(a<p->code_words+p->data_words)return 1;
 if(a>=p->id_base&&a<p->id_base+4)return 1;
 if(p->calibration&&a>=p->backup_address&&a<p->backup_address+p->backup_words)return 1;
 (void)write;return 0;
}
static inline lt48_status lt48_pic_baseline12_seek(const lt48_pic_baseline12_policy *p,
 const lt48_pic_classic_wire *w,unsigned a){
 if(!lt48_pic_baseline12_valid(p)||!lt48_pic_classic_wire_valid(w)||!lt48_pic_baseline12_address(p,a,0))return LT48_INVALID_ARGUMENT;
 lt48_status st=w->reenter(w->context);
 if(a==0xfff)return st;
 /* Config-entry PC increments to zero. Revisiting its numerical address
  * after wrapping never grants configuration access. */
 for(unsigned i=0;st==LT48_OK&&i<=a;i++)st=lt48_pic_classic_command(w,6);
 return st;
}
static inline lt48_status lt48_pic_baseline12_read(const lt48_pic_baseline12_policy *p,
 const lt48_pic_classic_wire *w,unsigned a,uint16_t *values,size_t n){
 if(!lt48_pic_baseline12_valid(p)||!lt48_pic_classic_wire_valid(w)||!values||!n||n>0x1000||(uint64_t)a+n>0x1000||(a==0xfff&&n!=1))return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(!lt48_pic_baseline12_address(p,a+(unsigned)i,0))return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_pic_baseline12_seek(p,w,a);
 for(size_t i=0;st==LT48_OK&&i<n;i++){
  st=lt48_pic_classic_command(w,4);if(st==LT48_OK)st=lt48_pic_classic_frame(w,1,values+i);
  values[i]&=0xfff;
  if(st==LT48_OK&&i+1<n)st=lt48_pic_classic_command(w,6);
 }
 return st;
}
static inline lt48_status lt48_pic_baseline12_program(const lt48_pic_baseline12_policy *p,
 const lt48_pic_classic_wire *w,unsigned a,const uint16_t *values,size_t n){
 if(!lt48_pic_baseline12_valid(p)||!values||!n||n>0x1000||(uint64_t)a+n>0x1000||(a==0xfff&&n!=1))return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(values[i]>0xfff||!lt48_pic_baseline12_address(p,a+(unsigned)i,1))return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_pic_baseline12_seek(p,w,a);
 for(size_t i=0;st==LT48_OK&&i<n;i++){
  st=lt48_pic_classic_load(w,2,values[i]);
  if(st==LT48_OK)st=lt48_pic_classic_command(w,8);
  if(st==LT48_OK)st=w->delay(w->context,p->program_us);
  if(st==LT48_OK)st=lt48_pic_classic_command(w,14);
  if(st==LT48_OK)st=w->delay(w->context,p->discharge_us);
  if(st==LT48_OK&&i+1<n)st=lt48_pic_classic_command(w,6);
 }
 return st;
}
/* Primitive erase sites. The operation owner must save/restore both factory
 * calibration words and select the complete sequence for Flash-data parts. */
static inline lt48_status lt48_pic_baseline12_erase_site(const lt48_pic_baseline12_policy *p,
 const lt48_pic_classic_wire *w,unsigned address){
 if(!lt48_pic_baseline12_valid(p)||(address!=0xfff&&address!=p->id_base&&(!p->data_words||address!=p->code_words)))return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_pic_baseline12_seek(p,w,address);
 if(st==LT48_OK)st=lt48_pic_classic_command(w,9);
 if(st==LT48_OK)st=w->delay(w->context,p->erase_us);
 return st;
}
#endif
