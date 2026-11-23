#ifndef LT48_PIC_CLASSIC14_TAIL_OPERATOR_H
#define LT48_PIC_CLASSIC14_TAIL_OPERATOR_H
#include "classic14_protocol.h"
#include <string.h>
/* Private full operation owner. DS41284E (617), DS40245B (716), DS41237D
 * (785). These three bounded behavior policies do not admit catalogue names. */
enum { LT48_TAIL14_WORK_WORDS=0x2200 };
typedef struct {
 unsigned family,file_words,eeprom_bytes,calibration_words,code_latches;
 unsigned configuration_mask,protection_mask,external_us,end_command;
 unsigned internal_program_us,internal_data_us,erase_us;
} lt48_pic_tail14_policy;
static const lt48_pic_tail14_policy lt48_pic_tail14_policies[]={
 {256,0x2008,0,1,1,0x0fff,0x0040,3500,10,0,0,20000},
 {54,0x2008,0,0,1,0x20cf,0x2000,2000,14,0,0,6000},
 {55,0x2200,256,2,4,0x0fff,0x00c0,0,0,3000,6000,6000}
};
static inline int lt48_pic_tail14_valid(const lt48_pic_tail14_policy *p){
 if(!p)return 0;
 for(unsigned i=0;i<3;i++)if(!memcmp(p,lt48_pic_tail14_policies+i,sizeof *p))return 1;
 return 0;
}
typedef struct {
 const lt48_pic_classic_wire *wire;void *context;
 lt48_status (*cleanup)(void *,lt48_status);
 lt48_status (*select_voltage)(void *,unsigned);
 /* Production adapters must deliver Begin/Delay/End without an intervening
  * transport flush for externally timed pulses. Logical fixtures may use
  * the explicit command-level implementation below. */
 lt48_status (*commit)(void *,unsigned,unsigned,unsigned);
 const unsigned *verify_levels;unsigned verify_count;
} lt48_pic_tail14_io;
typedef struct {
 lt48_status operation_status,cleanup_status;
 unsigned mismatches,first_mismatch,factory_saved;
 uint16_t original_calibration[2];
} lt48_pic_tail14_result;
static inline unsigned lt48_pic_tail14_mask(const lt48_pic_tail14_policy *p,unsigned a){
 if(a<0x800||(a>=0x2000&&a<0x2004))return 0x3fff;
 if(a==0x2007)return p->configuration_mask;
 if(a>=0x2008&&a<0x2008+p->calibration_words)return 0x3fff;
 return a>=0x2100&&a<0x2100+p->eeprom_bytes?255:0;
}
static inline int lt48_pic_tail14_range(const lt48_pic_tail14_policy *p,unsigned a,unsigned n,int write){
 if(!n||n>LT48_TAIL14_WORK_WORDS)return 0;
 if(a<0x800)return n<=0x800-a;
 if(a>=0x2000&&a<0x2004)return n<=0x2004-a;
 if(a==0x2007)return n<=(write?1u:1+p->calibration_words);
 if(!write&&a>=0x2008&&a<0x2008+p->calibration_words)return n<=0x2008+p->calibration_words-a;
 return a>=0x2100&&a<0x2100+p->eeprom_bytes&&n<=0x2100+p->eeprom_bytes-a;
}
static inline lt48_status lt48_pic_tail14_read(const lt48_pic_tail14_policy *p,
 const lt48_pic_classic_wire *w,unsigned a,uint16_t *v,unsigned n){
 if(!lt48_pic_tail14_valid(p)||!lt48_pic_classic_wire_valid(w)||!v||!lt48_pic_tail14_range(p,a,n,0))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,a);
 for(unsigned i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_command(w,a>=0x2100?5:4);
  if(s==LT48_OK)s=lt48_pic_classic_frame(w,1,v+i);
  if(s==LT48_OK&&a>=0x2100)v[i]&=255;
  if(s==LT48_OK&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_tail14_commit(const lt48_pic_tail14_policy *p,const lt48_pic_tail14_io *io,unsigned a){
 unsigned cmd=p->external_us?24:8;
 unsigned us=p->external_us?p->external_us:a>=0x2100?p->internal_data_us:p->internal_program_us;
 if(io->commit)return io->commit(io->context,cmd,us,p->end_command);
 lt48_status s=lt48_pic_classic_command(io->wire,cmd);
 if(s==LT48_OK)s=io->wire->delay(io->wire->context,us);
 if(s==LT48_OK&&p->end_command)s=lt48_pic_classic_command(io->wire,p->end_command);
 if(s==LT48_OK)s=io->wire->delay(io->wire->context,100);
 return s;
}
static inline lt48_status lt48_pic_tail14_write(const lt48_pic_tail14_policy *p,
 const lt48_pic_tail14_io *io,unsigned a,const uint16_t *v,unsigned n){
 if(!lt48_pic_tail14_valid(p)||!io||!lt48_pic_classic_wire_valid(io->wire)||!v||!lt48_pic_tail14_range(p,a,n,1))return LT48_INVALID_ARGUMENT;
 unsigned code=a<0x800,data=a>=0x2100;
 if(code&&(a%p->code_latches||n%p->code_latches))return LT48_INVALID_ARGUMENT;
 for(unsigned i=0;i<n;i++)if(v[i]>(data?255:0x3fff))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(io->wire,a);
 for(unsigned i=0;s==LT48_OK&&i<n;i++){
  if(i&&!code&&!data)s=lt48_pic_classic_seek(io->wire,a+i);
  if(s==LT48_OK)s=lt48_pic_classic_load(io->wire,data?3:2,v[i]);
  if(s==LT48_OK&&(!code||(i+1)%p->code_latches==0))s=lt48_pic_tail14_commit(p,io,a+i);
  if(s==LT48_OK&&i+1<n&&(code||data))s=lt48_pic_classic_command(io->wire,6);
 }
 return s;
}
static inline lt48_status lt48_pic_tail14_scan(const lt48_pic_tail14_policy *p,
 const lt48_pic_classic_wire *w,uint16_t *out){
 for(unsigned a=0;a<LT48_TAIL14_WORK_WORDS;a++)out[a]=0x3fff;
 lt48_status s=lt48_pic_tail14_read(p,w,0,out,0x800);
 if(s==LT48_OK)s=lt48_pic_tail14_read(p,w,0x2000,out+0x2000,4);
 if(s==LT48_OK)s=lt48_pic_tail14_read(p,w,0x2007,out+0x2007,1+p->calibration_words);
 if(s==LT48_OK&&p->eeprom_bytes)s=lt48_pic_tail14_read(p,w,0x2100,out+0x2100,p->eeprom_bytes);
 return s;
}
static inline lt48_status lt48_pic_tail14_check(const lt48_pic_tail14_policy *p,
 const uint16_t *want,const uint16_t *got,int blank,lt48_pic_tail14_result *r){
 r->mismatches=0;r->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<p->file_words;a++){
  if(blank&&a>=0x2008&&a<0x2008+p->calibration_words)continue;
  if(((blank?0x3fff:want[a])^got[a])&lt48_pic_tail14_mask(p,a)){
   if(!r->mismatches)r->first_mismatch=2*a;
   r->mismatches++;
  }
 }
 return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
static inline lt48_status lt48_pic_tail14_verify(const lt48_pic_tail14_policy *p,
 const lt48_pic_tail14_io *io,const uint16_t *want,uint16_t *work,int blank,lt48_pic_tail14_result *r){
 lt48_status s=LT48_OK;unsigned n=io->verify_count?io->verify_count:1;
 for(unsigned i=0;s==LT48_OK&&i<n;i++){
  if(io->verify_count)s=io->select_voltage(io->context,io->verify_levels[i]);
  if(s==LT48_OK)s=lt48_pic_tail14_scan(p,io->wire,work);
  if(s==LT48_OK)s=lt48_pic_tail14_check(p,want,work,blank,r);
 }
 if(io->verify_count){lt48_status reset=io->select_voltage(io->context,0);if(s==LT48_OK)s=reset;}
 return s;
}
static inline int lt48_pic_tail14_overlap(const uint16_t *a,unsigned an,const uint16_t *b,unsigned bn){
 if(!a||!b)return 0;
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return x<y?y-x<an*sizeof *a:x-y<bn*sizeof *b;
}
static inline lt48_status lt48_pic_tail14_execute(const lt48_pic_tail14_policy *p,
 const lt48_pic_tail14_io *io,lt48_operation_kind op,const uint16_t *image,
 uint16_t *output,uint16_t *work,uint16_t *expected,lt48_pic_tail14_result *r){
 if(!r)return LT48_INVALID_ARGUMENT;
 memset(r,0,sizeof *r);r->first_mismatch=UINT32_MAX;
 if(!lt48_pic_tail14_valid(p)||!io||!lt48_pic_classic_wire_valid(io->wire)||!io->cleanup||!work||!expected||
  op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE||(op==LT48_OPERATION_READ&&!output)||
  lt48_pic_tail14_overlap(work,LT48_TAIL14_WORK_WORDS,expected,LT48_TAIL14_WORK_WORDS)||
  lt48_pic_tail14_overlap(image,p->file_words,work,LT48_TAIL14_WORK_WORDS)||
  lt48_pic_tail14_overlap(image,p->file_words,expected,LT48_TAIL14_WORK_WORDS)||
  lt48_pic_tail14_overlap(output,p->file_words,work,LT48_TAIL14_WORK_WORDS)||
  lt48_pic_tail14_overlap(output,p->file_words,expected,LT48_TAIL14_WORK_WORDS)||
  io->verify_count>3||(io->verify_count&&(!io->select_voltage||!io->verify_levels)))return LT48_INVALID_ARGUMENT;
 int programming=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 if((programming||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image)return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned a=0;a<p->file_words;a++)if(image[a]>0x3fff||
  (a!=0x2007&&image[a]!=0x3fff&&(image[a]&~lt48_pic_tail14_mask(p,a))))return LT48_SAFETY_REJECTED;
 lt48_status s=lt48_pic_tail14_scan(p,io->wire,work);
 for(unsigned a=0;a<LT48_TAIL14_WORK_WORDS;a++)expected[a]=image&&a<p->file_words?image[a]:0x3fff;
 expected[0x2007]|=0x3fff^p->configuration_mask;
 for(unsigned a=0x2100;a<0x2100+p->eeprom_bytes;a++)expected[a]&=255;
 if(s==LT48_OK){
  r->factory_saved=1;
  for(unsigned i=0;i<p->calibration_words;i++){
   r->original_calibration[i]=work[0x2008+i];
   if(programming&&expected[0x2008+i]!=0x3fff&&expected[0x2008+i]!=work[0x2008+i])s=LT48_SAFETY_REJECTED;
   expected[0x2008+i]=work[0x2008+i];
  }
 }
 if(s==LT48_OK&&op==LT48_OPERATION_ERASE){
  s=lt48_pic_classic_seek(io->wire,0x2000);
  if(s==LT48_OK)s=lt48_pic_classic_command(io->wire,9);
  if(s==LT48_OK)s=io->wire->delay(io->wire->context,p->erase_us);
  if(s==LT48_OK&&p->eeprom_bytes)s=lt48_pic_classic_command(io->wire,11);
  if(s==LT48_OK&&p->eeprom_bytes)s=io->wire->delay(io->wire->context,p->erase_us);
  if(s==LT48_OK)s=lt48_pic_tail14_verify(p,io,expected,work,1,r);
 }else if(s==LT48_OK&&programming){
  if((work[0x2007]&p->protection_mask)!=p->protection_mask)s=LT48_SAFETY_REJECTED;
  if(s==LT48_OK&&op==LT48_OPERATION_PROGRAM)s=lt48_pic_tail14_check(p,NULL,work,1,r);
  if(s==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM)for(unsigned a=0;a<0x2100;a++){
   unsigned mask=lt48_pic_tail14_mask(p,a);
   /* DS40245B2.3.3.1: configuration can be rewritten in either direction;
    * CP alone requires bulk erase to return from protected0 to unprotected1. */
   if(p->family==54&&a==0x2007)mask&=p->protection_mask;
   if((expected[a]&~work[a])&mask){s=LT48_SAFETY_REJECTED;break;}
  }
  uint16_t cfg=expected[0x2007];expected[0x2007]|=p->protection_mask;
  if(s==LT48_OK)s=lt48_pic_tail14_write(p,io,0,expected,0x800);
  if(s==LT48_OK)s=lt48_pic_tail14_write(p,io,0x2000,expected+0x2000,4);
  if(s==LT48_OK&&p->eeprom_bytes)s=lt48_pic_tail14_write(p,io,0x2100,expected+0x2100,p->eeprom_bytes);
  if(s==LT48_OK)s=lt48_pic_tail14_write(p,io,0x2007,expected+0x2007,1);
  if(s==LT48_OK)s=lt48_pic_tail14_verify(p,io,expected,work,0,r);
  expected[0x2007]=cfg;
  if(s==LT48_OK&&(cfg&p->protection_mask)!=p->protection_mask){
   uint16_t got=0;s=lt48_pic_tail14_write(p,io,0x2007,&cfg,1);
   if(s==LT48_OK)s=lt48_pic_tail14_read(p,io->wire,0x2007,&got,1);
   if(s==LT48_OK&&((got^cfg)&p->configuration_mask))s=LT48_DEVICE_MISMATCH;
  }
 }else if(s==LT48_OK&&op==LT48_OPERATION_SECURE){
  uint16_t cfg=work[0x2007]&~p->protection_mask,got=0;
  s=lt48_pic_tail14_write(p,io,0x2007,&cfg,1);
  if(s==LT48_OK)s=lt48_pic_tail14_read(p,io->wire,0x2007,&got,1);
  if(s==LT48_OK&&((got^cfg)&p->configuration_mask))s=LT48_DEVICE_MISMATCH;
 }else if(s==LT48_OK&&op!=LT48_OPERATION_READ){
  s=op==LT48_OPERATION_VERIFY?lt48_pic_tail14_verify(p,io,expected,work,0,r):lt48_pic_tail14_check(p,expected,work,op==LT48_OPERATION_BLANK_CHECK,r);
 }
 if(s==LT48_OK&&p->calibration_words){
  uint16_t cal[2]={0};s=lt48_pic_tail14_read(p,io->wire,0x2008,cal,p->calibration_words);
  if(s==LT48_OK&&memcmp(cal,r->original_calibration,p->calibration_words*sizeof *cal))s=LT48_DEVICE_MISMATCH;
 }
 r->operation_status=s;r->cleanup_status=io->cleanup(io->context,s);
 if(s==LT48_OK)s=r->cleanup_status;
 if(s==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,work,p->file_words*sizeof *output);
 return s;
}
#endif
