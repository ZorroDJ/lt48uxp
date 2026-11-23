#ifndef LT48_PIC_MIDRANGE14_PROTOCOL_H
#define LT48_PIC_MIDRANGE14_PROTOCOL_H
#include "classic14_protocol.h"
/* DS41572C/DS41561C/DS41686A common protocol subset. Lifecycle, source
 * configuration conversion, audited entry and public admission are separate. */
typedef struct {unsigned code_words,latches,internal_configuration,external_us,internal_data_us,internal_config_us,erase_us,has_revision;} lt48_pic_midrange_policy;
static inline int lt48_pic_midrange_valid(const lt48_pic_midrange_policy *p){
 return p&&(p->code_words==256||p->code_words==512||p->code_words==1024||p->code_words==2048)&&
  (p->latches==4||p->latches==16)&&p->internal_configuration<=1&&p->has_revision<=1&&
  (p->internal_configuration?p->external_us>=1000&&p->external_us<=2100&&p->internal_data_us>=2500&&p->internal_config_us>=5000:p->external_us>=3000)&&p->erase_us>=5000;
}
static inline int lt48_pic_midrange_address(const lt48_pic_midrange_policy *p,unsigned a,int write){
 return a<p->code_words||(a>=0x2000&&a<=0x2003)||a==0x2007||(!write&&p->has_revision&&a==0x2005)||(!write&&a>=0x2006&&a<=0x2009);
}
static inline lt48_status lt48_pic_midrange_read(const lt48_pic_midrange_policy *p,const lt48_pic_classic_wire *w,unsigned a,uint16_t *out,size_t n){
 if(!lt48_pic_midrange_valid(p)||!lt48_pic_classic_wire_valid(w)||!out||!n||n>0x2100||(uint64_t)a+n>0x2100)return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(!lt48_pic_midrange_address(p,a+(unsigned)i,0))return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_pic_classic_seek(w,a);
 for(size_t i=0;st==LT48_OK&&i<n;i++){
  st=lt48_pic_classic_command(w,4);if(st==LT48_OK)st=lt48_pic_classic_frame(w,1,out+i);
  if(st==LT48_OK&&i+1<n)st=lt48_pic_classic_command(w,6);
 }return st;
}
static inline lt48_status lt48_pic_midrange_commit(const lt48_pic_midrange_policy *p,const lt48_pic_classic_wire *w,unsigned a){
 lt48_status st;
 if(p->internal_configuration&&a>=0x2000){
  st=lt48_pic_classic_command(w,8);if(st==LT48_OK)st=w->delay(w->context,a==0x2007?p->internal_config_us:p->internal_data_us);
 }else{
  st=lt48_pic_classic_command(w,24);if(st==LT48_OK)st=w->delay(w->context,p->external_us);
  if(st==LT48_OK)st=lt48_pic_classic_command(w,10);
 }
 if(st==LT48_OK)st=w->delay(w->context,100);
 return st;
}
typedef lt48_status (*lt48_pic_midrange_commit_callback)(void *,const lt48_pic_midrange_policy *,unsigned);
static inline lt48_status lt48_pic_midrange_program_with_commit(const lt48_pic_midrange_policy *p,const lt48_pic_classic_wire *w,unsigned a,const uint16_t *values,size_t n,lt48_pic_midrange_commit_callback commit,void *context){
 if(!lt48_pic_midrange_valid(p)||!lt48_pic_classic_wire_valid(w)||!values||!n||n>0x2100||(uint64_t)a+n>0x2100)return LT48_INVALID_ARGUMENT;
 unsigned code=a<p->code_words;if(code&&(a%p->latches||n%p->latches||(uint64_t)a+n>p->code_words))return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(!lt48_pic_midrange_address(p,a+(unsigned)i,1)||values[i]>0x3fff)return LT48_INVALID_ARGUMENT;
 lt48_status st=LT48_OK;
 for(size_t i=0;st==LT48_OK&&i<n;i++){
  if(!i||!code)st=lt48_pic_classic_seek(w,a+(unsigned)i);
  if(st==LT48_OK)st=lt48_pic_classic_load(w,2,values[i]);
  if(st==LT48_OK&&(!code||(i+1)%p->latches==0))st=commit?commit(context,p,a+(unsigned)i):lt48_pic_midrange_commit(p,w,a+(unsigned)i);
  if(st==LT48_OK&&code&&i+1<n)st=lt48_pic_classic_command(w,6);
 }return st;
}
static inline lt48_status lt48_pic_midrange_program(const lt48_pic_midrange_policy *p,const lt48_pic_classic_wire *w,unsigned a,const uint16_t *values,size_t n){
 return lt48_pic_midrange_program_with_commit(p,w,a,values,n,NULL,NULL);
}
static inline lt48_status lt48_pic_midrange_erase(const lt48_pic_midrange_policy *p,const lt48_pic_classic_wire *w){
 if(!lt48_pic_midrange_valid(p)||!lt48_pic_classic_wire_valid(w))return LT48_INVALID_ARGUMENT;
 /* The specified configuration-bank entry erases code, IDs and configuration,
  * preserving both factory calibration words. Never enter a factory erase site. */
 lt48_status st=lt48_pic_classic_seek(w,0x2000);
 if(st==LT48_OK)st=lt48_pic_classic_command(w,9);
 if(st==LT48_OK)st=w->delay(w->context,p->erase_us);
 return st;
}
#endif
