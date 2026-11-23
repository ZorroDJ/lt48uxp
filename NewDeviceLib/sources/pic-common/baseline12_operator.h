#ifndef LT48_PIC_BASELINE12_OPERATOR_H
#define LT48_PIC_BASELINE12_OPERATOR_H
#include "baseline12_protocol.h"
#include <string.h>
/* Private full-image operation composition. Original host sequencing, power,
 * transport, adapters and public admission are separate qualification gates. */
enum { LT48_BASELINE12_IMAGE_WORDS=0x1000 };
typedef struct {
 const lt48_pic_classic_wire *wire;
 void *context;
 lt48_status (*cleanup)(void *,lt48_status);
 /* Must save recovery data durably before an erase can destroy the backup
  * bank. A failed save aborts before the first destructive command. */
 lt48_status (*save_factory)(void *,uint16_t,const uint16_t *,unsigned);
 const unsigned *verify_levels;
 unsigned verify_count;
 lt48_status (*select_voltage)(void *,unsigned);
} lt48_pic_baseline12_io;
typedef struct {
 lt48_status operation_status,cleanup_status;
 unsigned mismatches,first_mismatch;
 uint16_t original_calibration,original_backup,original_backups[4];
 unsigned calibration_saved;
} lt48_pic_baseline12_result;
static inline int lt48_pic_baseline12_overlap(const uint16_t *a,const uint16_t *b){
 if(!a||!b)return 0;
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return (x<y?y-x:x-y)<0x1000*sizeof *a;
}
static inline unsigned lt48_pic_baseline12_mask(const lt48_pic_baseline12_policy *p,unsigned a){
 return a==0xfff?p->configuration_mask:lt48_pic_baseline12_address(p,a,0)?0xfff:0;
}
static inline lt48_status lt48_pic_baseline12_scan(const lt48_pic_baseline12_policy *p,
 const lt48_pic_classic_wire *w,uint16_t *out){
 for(unsigned a=0;a<0x1000;a++)out[a]=0xfff;
 lt48_status st=lt48_pic_baseline12_read(p,w,0,out,p->code_words+p->data_words);
 if(st==LT48_OK)st=lt48_pic_baseline12_read(p,w,p->id_base,out+p->id_base,4);
 if(st==LT48_OK&&p->calibration)st=lt48_pic_baseline12_read(p,w,p->backup_address,out+p->backup_address,p->backup_words);
 if(st==LT48_OK)st=lt48_pic_baseline12_read(p,w,0xfff,out+0xfff,1);
 return st;
}
static inline lt48_status lt48_pic_baseline12_check(const lt48_pic_baseline12_policy *p,
 const uint16_t *want,const uint16_t *got,int blank,lt48_pic_baseline12_result *r){
 r->mismatches=0;r->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<0x1000;a++){
  unsigned mask=lt48_pic_baseline12_mask(p,a);
  if(blank&&p->calibration&&(a==p->code_words-1||(a>=p->backup_address&&a<p->backup_address+p->backup_words)))continue;
  if(((blank?0xfff:want[a])^got[a])&mask){if(!r->mismatches)r->first_mismatch=2*a;r->mismatches++;}
 }
 return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
static inline lt48_status lt48_pic_baseline12_restore_word(const lt48_pic_baseline12_policy *p,
 const lt48_pic_classic_wire *w,unsigned a,uint16_t value){
 lt48_status st=value==0xfff?LT48_OK:lt48_pic_baseline12_program(p,w,a,&value,1);uint16_t got=0;
 if(st==LT48_OK)st=lt48_pic_baseline12_read(p,w,a,&got,1);
 return st==LT48_OK&&got!=value?LT48_DEVICE_MISMATCH:st;
}
static inline lt48_status lt48_pic_baseline12_verify_levels(const lt48_pic_baseline12_policy *p,
 const lt48_pic_baseline12_io *io,const uint16_t *expected,uint16_t *work,int blank,lt48_pic_baseline12_result *r){
 if(!io->verify_count)return lt48_pic_baseline12_check(p,expected,work,blank,r);
 if(!io->verify_levels||!io->select_voltage||io->verify_count>3)return LT48_INVALID_ARGUMENT;
 lt48_status st=LT48_OK;
 for(unsigned i=0;st==LT48_OK&&i<io->verify_count;i++){
  st=io->select_voltage(io->context,io->verify_levels[i]);
  if(st==LT48_OK)st=lt48_pic_baseline12_scan(p,io->wire,work);
  if(st==LT48_OK)st=lt48_pic_baseline12_check(p,expected,work,blank,r);
 }
 lt48_status reset=io->select_voltage(io->context,0);return st==LT48_OK?reset:st;
}
static inline lt48_status lt48_pic_baseline12_erase_preserving(const lt48_pic_baseline12_policy *p,
 const lt48_pic_classic_wire *w,lt48_pic_baseline12_result *r){
 lt48_status st=lt48_pic_baseline12_erase_site(p,w,0xfff);
 /* Restore/verify the primary before erasing the bank containing its backup.
  * If writing fails, the original backup has not been destroyed. */
 if(st==LT48_OK&&p->calibration)st=lt48_pic_baseline12_restore_word(p,w,p->code_words-1,r->original_calibration);
 if(st==LT48_OK&&p->data_words)st=lt48_pic_baseline12_erase_site(p,w,p->code_words);
 if(st==LT48_OK)st=lt48_pic_baseline12_erase_site(p,w,p->id_base);
 /* For parts without Flash data, the ID-bank erase also erases code. */
 if(st==LT48_OK&&p->calibration&&!p->data_words)st=lt48_pic_baseline12_restore_word(p,w,p->code_words-1,r->original_calibration);
 if(p->calibration)for(unsigned i=0;st==LT48_OK&&i<p->backup_words;i++)st=lt48_pic_baseline12_restore_word(p,w,p->backup_address+i,r->original_backups[i]);
 return st;
}
static inline lt48_status lt48_pic_baseline12_execute(const lt48_pic_baseline12_policy *p,
 const lt48_pic_baseline12_io *io,lt48_operation_kind op,const uint16_t *image,
 uint16_t *output,uint16_t *work,uint16_t *expected,lt48_pic_baseline12_result *r){
 if(!r)return LT48_INVALID_ARGUMENT;
 memset(r,0,sizeof *r);r->first_mismatch=UINT32_MAX;
 if(!lt48_pic_baseline12_valid(p)||!io||!io->wire||!io->cleanup||!work||!expected||
  lt48_pic_baseline12_overlap(work,expected)||lt48_pic_baseline12_overlap(image,work)||lt48_pic_baseline12_overlap(image,expected)||
  (op==LT48_OPERATION_READ&&!output)||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image)return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned a=0;a<0x1000;a++)if(image[a]>0xfff||(!lt48_pic_baseline12_mask(p,a)&&image[a]!=0xfff))return LT48_SAFETY_REJECTED;
 lt48_status st=lt48_pic_baseline12_scan(p,io->wire,work);
 if(st==LT48_OK&&p->calibration){r->original_calibration=work[p->code_words-1];r->original_backup=work[p->backup_address];memcpy(r->original_backups,work+p->backup_address,p->backup_words*sizeof(uint16_t));r->calibration_saved=1;}
 if(image)memcpy(expected,image,0x1000*sizeof *expected);else for(unsigned a=0;a<0x1000;a++)expected[a]=0xfff;
 if(st==LT48_OK&&p->calibration){
  if(expected[p->code_words-1]==0xfff)expected[p->code_words-1]=r->original_calibration;
  for(unsigned i=0;i<p->backup_words;i++)if(expected[p->backup_address+i]==0xfff)expected[p->backup_address+i]=r->original_backups[i];
 }
 if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE)){
  if(p->calibration&&(r->original_calibration&0xf00)!=0xc00)st=LT48_SAFETY_REJECTED;
  if(p->calibration&&op!=LT48_OPERATION_ERASE){if(expected[p->code_words-1]!=r->original_calibration)st=LT48_SAFETY_REJECTED;for(unsigned i=0;i<p->backup_words;i++)if(expected[p->backup_address+i]!=r->original_backups[i])st=LT48_SAFETY_REJECTED;}
 }
 if(st==LT48_OK&&p->calibration&&op==LT48_OPERATION_ERASE){
  st=io->save_factory?io->save_factory(io->context,r->original_calibration,r->original_backups,p->backup_words):LT48_SAFETY_REJECTED;
 }
 if(st==LT48_OK&&op==LT48_OPERATION_ERASE){
  st=lt48_pic_baseline12_erase_preserving(p,io->wire,r);
  if(st==LT48_OK)st=lt48_pic_baseline12_scan(p,io->wire,work);
  if(st==LT48_OK)st=lt48_pic_baseline12_check(p,expected,work,1,r);
 }else if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)){
  if((work[0xfff]&(p->code_protect|p->data_protect))!=(p->code_protect|p->data_protect))st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM)for(unsigned a=0;a<0x1000;a++)if((expected[a]&~work[a])&lt48_pic_baseline12_mask(p,a)){st=LT48_SAFETY_REJECTED;break;}
  unsigned cfg=expected[0xfff];expected[0xfff]|=p->code_protect|p->data_protect;
  if(st==LT48_OK)st=lt48_pic_baseline12_program(p,io->wire,0,expected,p->code_words-(p->calibration?1:0));
  if(st==LT48_OK&&p->data_words)st=lt48_pic_baseline12_program(p,io->wire,p->code_words,expected+p->code_words,p->data_words);
  if(st==LT48_OK)st=lt48_pic_baseline12_program(p,io->wire,p->id_base,expected+p->id_base,4);
  if(st==LT48_OK)st=lt48_pic_baseline12_program(p,io->wire,0xfff,expected+0xfff,1);
  if(st==LT48_OK)st=lt48_pic_baseline12_scan(p,io->wire,work);
  if(st==LT48_OK)st=lt48_pic_baseline12_verify_levels(p,io,expected,work,0,r);
  expected[0xfff]=cfg;
  if(st==LT48_OK&&(cfg&(p->code_protect|p->data_protect))!=(p->code_protect|p->data_protect)){
   st=lt48_pic_baseline12_program(p,io->wire,0xfff,expected+0xfff,1);uint16_t got;
   if(st==LT48_OK)st=lt48_pic_baseline12_read(p,io->wire,0xfff,&got,1);
   if(st==LT48_OK&&((got^cfg)&p->configuration_mask))st=LT48_DEVICE_MISMATCH;
  }
 }else if(st==LT48_OK&&op==LT48_OPERATION_SECURE){
  uint16_t cfg=work[0xfff]&~(p->code_protect|p->data_protect),got=0;
  st=lt48_pic_baseline12_program(p,io->wire,0xfff,&cfg,1);
  if(st==LT48_OK)st=lt48_pic_baseline12_read(p,io->wire,0xfff,&got,1);
  if(st==LT48_OK&&((cfg^got)&p->configuration_mask))st=LT48_DEVICE_MISMATCH;
 }else if(st==LT48_OK&&op!=LT48_OPERATION_READ){
  st=op==LT48_OPERATION_VERIFY?lt48_pic_baseline12_verify_levels(p,io,expected,work,0,r):
   lt48_pic_baseline12_check(p,expected,work,op==LT48_OPERATION_BLANK_CHECK,r);
 }
 if(st==LT48_OK&&p->calibration){uint16_t a=0,b[4]={0};
  st=lt48_pic_baseline12_read(p,io->wire,p->code_words-1,&a,1);
  if(st==LT48_OK)st=lt48_pic_baseline12_read(p,io->wire,p->backup_address,b,p->backup_words);
  if(st==LT48_OK&&(a!=r->original_calibration||memcmp(b,r->original_backups,p->backup_words*sizeof(uint16_t))))st=LT48_DEVICE_MISMATCH;
 }
 r->operation_status=st;r->cleanup_status=io->cleanup(io->context,st);
 if(st==LT48_OK&&r->cleanup_status!=LT48_OK)st=r->cleanup_status;
 if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,work,0x1000*sizeof *output);
 return st;
}
#endif
