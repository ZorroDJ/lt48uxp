#ifndef LT48_PIC_CALIBRATED14_OPERATOR_H
#define LT48_PIC_CALIBRATED14_OPERATOR_H
#include "classic14_protocol.h"
#include <string.h>
/* DS41191D, PIC12F629/675 and PIC16F630/676. Private until source,
 * controller, independent effects and normal GUI publication gates pass.
 * Word addresses are the original file layout, not a compact SRAM image. */
enum { LT48_CAL14_WORDS=0x2180, LT48_CAL14_CAL=0x3ff,
       LT48_CAL14_CFG=0x2007, LT48_CAL14_CP=0x180 };
typedef struct {
 const lt48_pic_classic_wire *wire;
 void *context;
 lt48_status (*cleanup)(void *,lt48_status);
 /* Durable recovery journal: both values must survive process interruption. */
 lt48_status (*save_factory)(void *,uint16_t,uint16_t);
 lt48_status (*select_voltage)(void *,unsigned);
 const unsigned *verify_levels;
 unsigned verify_count;
} lt48_pic_cal14_io;
typedef struct {
 lt48_status operation_status,cleanup_status;
 unsigned mismatches,first_mismatch;
 uint16_t calibration,bandgap;
 unsigned factory_saved;
} lt48_pic_cal14_result;
static inline unsigned lt48_pic_cal14_mask(unsigned a){
 if(a<0x400||(a>=0x2000&&a<0x2004))return 0x3fff;
 if(a==LT48_CAL14_CFG)return 0x31ff;
 return a>=0x2100&&a<LT48_CAL14_WORDS?0xff:0;
}
static inline int lt48_pic_cal14_range(unsigned a,unsigned n){
 if(!n)return 0;
 if(a<0x400)return n<=0x400-a;
 if(a>=0x2000&&a<0x2004)return n<=0x2004-a;
 if(a==LT48_CAL14_CFG)return n==1;
 return a>=0x2100&&a<LT48_CAL14_WORDS&&n<=LT48_CAL14_WORDS-a;
}
static inline lt48_status lt48_pic_cal14_read(const lt48_pic_classic_wire *w,
 unsigned a,uint16_t *v,unsigned n){
 if(!lt48_pic_classic_wire_valid(w)||!v||!lt48_pic_cal14_range(a,n))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,a);
 for(unsigned i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_command(w,a>=0x2100?5:4);
  if(s==LT48_OK)s=lt48_pic_classic_frame(w,1,v+i);
  if(s==LT48_OK)v[i]&=lt48_pic_cal14_mask(a+i);
  if(s==LT48_OK&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_cal14_write(const lt48_pic_classic_wire *w,
 unsigned a,const uint16_t *v,unsigned n){
 if(!lt48_pic_classic_wire_valid(w)||!v||!lt48_pic_cal14_range(a,n))return LT48_INVALID_ARGUMENT;
 for(unsigned i=0;i<n;i++)if(v[i]&~lt48_pic_cal14_mask(a+i))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,a);
 for(unsigned i=0;s==LT48_OK&&i<n;i++){
  /* Configuration-space latches are reset between UID words. */
  if(i&&a>=0x2000&&a<0x2100)s=lt48_pic_classic_seek(w,a+i);
  if(s==LT48_OK)s=lt48_pic_classic_load(w,a>=0x2100?3:2,v[i]);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,8);
  /* Use 8ms for every internal pulse: original EEPROM and erase restoration
   * use double tPW, while streaming code uses 4ms. Both exceed the primary
   * maxima of 2.5ms code / 6ms EEPROM; the longer code wait is deliberate. */
  if(s==LT48_OK)s=w->delay(w->context,8000);
  if(s==LT48_OK&&i+1<n&&(a<0x2000||a>=0x2100))s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_cal14_scan(const lt48_pic_classic_wire *w,uint16_t *v){
 for(unsigned a=0;a<LT48_CAL14_WORDS;a++)v[a]=0x3fff;
 lt48_status s=lt48_pic_cal14_read(w,0,v,0x400);
 if(s==LT48_OK)s=lt48_pic_cal14_read(w,0x2000,v+0x2000,4);
 if(s==LT48_OK)s=lt48_pic_cal14_read(w,LT48_CAL14_CFG,v+LT48_CAL14_CFG,1);
 if(s==LT48_OK)s=lt48_pic_cal14_read(w,0x2100,v+0x2100,128);
 return s;
}
static inline lt48_status lt48_pic_cal14_check(const uint16_t *want,
 const uint16_t *got,int blank,lt48_pic_cal14_result *r){
 r->mismatches=0;r->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<LT48_CAL14_WORDS;a++){
  unsigned mask=lt48_pic_cal14_mask(a);
  if(blank){if(a==LT48_CAL14_CAL)continue;if(a==LT48_CAL14_CFG)mask=0x1ff;}
  if(((blank?0x3fff:want[a])^got[a])&mask){
   if(!r->mismatches)r->first_mismatch=2*a;
   r->mismatches++;
  }
 }
 return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
static inline int lt48_pic_cal14_overlap(const uint16_t *a,const uint16_t *b){
 if(!a||!b)return 0;
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return (x<y?y-x:x-y)<LT48_CAL14_WORDS*sizeof *a;
}
static inline lt48_status lt48_pic_cal14_verify(const lt48_pic_cal14_io *io,
 const uint16_t *want,uint16_t *work,int blank,lt48_pic_cal14_result *r){
 lt48_status s=LT48_OK;
 unsigned n=io->verify_count?io->verify_count:1;
 for(unsigned i=0;s==LT48_OK&&i<n;i++){
  if(io->verify_count)s=io->select_voltage(io->context,io->verify_levels[i]);
  if(s==LT48_OK)s=lt48_pic_cal14_scan(io->wire,work);
  if(s==LT48_OK)s=lt48_pic_cal14_check(want,work,blank,r);
 }
 if(io->verify_count){lt48_status reset=io->select_voltage(io->context,0);if(s==LT48_OK)s=reset;}
 return s;
}
static inline lt48_status lt48_pic_cal14_execute(const lt48_pic_cal14_io *io,
 lt48_operation_kind op,const uint16_t *image,uint16_t *output,
 uint16_t *work,uint16_t *expected,lt48_pic_cal14_result *r){
 if(!r)return LT48_INVALID_ARGUMENT;
 memset(r,0,sizeof *r);r->first_mismatch=UINT32_MAX;
 if(!io||!lt48_pic_classic_wire_valid(io->wire)||!io->cleanup||!work||!expected||
  op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE||
  (op==LT48_OPERATION_READ&&!output)||
  lt48_pic_cal14_overlap(work,expected)||lt48_pic_cal14_overlap(image,work)||
  lt48_pic_cal14_overlap(image,expected)||lt48_pic_cal14_overlap(output,expected)||
  lt48_pic_cal14_overlap(output,work)||
  io->verify_count>3||(io->verify_count&&(!io->select_voltage||!io->verify_levels)))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||
  op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image)return LT48_INVALID_ARGUMENT;
 /* Canonical erased 3FFF is accepted for holes and byte lanes. Other values
  * outside implemented bits are rejected before electrical access. */
 if(image)for(unsigned a=0;a<LT48_CAL14_WORDS;a++)
  if(image[a]>0x3fff||(a!=LT48_CAL14_CFG&&image[a]!=0x3fff&&(image[a]&~lt48_pic_cal14_mask(a))))return LT48_SAFETY_REJECTED;
 const lt48_pic_classic_wire *w=io->wire;
 lt48_status s=lt48_pic_cal14_scan(w,work);
 if(s==LT48_OK){r->calibration=work[LT48_CAL14_CAL];r->bandgap=work[LT48_CAL14_CFG]&0x3000;r->factory_saved=1;}
 for(unsigned a=0;a<LT48_CAL14_WORDS;a++)expected[a]=image?image[a]:0x3fff;
 if(s==LT48_OK){
  if(expected[LT48_CAL14_CAL]==0x3fff)expected[LT48_CAL14_CAL]=r->calibration;
  expected[LT48_CAL14_CFG]=(expected[LT48_CAL14_CFG]&0x1ff)|r->bandgap;
  for(unsigned a=0x2100;a<LT48_CAL14_WORDS;a++)expected[a]&=255;
 }
 if(s==LT48_OK&&(op==LT48_OPERATION_ERASE||op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)){
  if((r->calibration&0x3c00)!=0x3400)s=LT48_SAFETY_REJECTED;
  if(op!=LT48_OPERATION_ERASE&&expected[LT48_CAL14_CAL]!=r->calibration)s=LT48_SAFETY_REJECTED;
 }
 if(s==LT48_OK&&op==LT48_OPERATION_ERASE){
  s=io->save_factory?io->save_factory(io->context,r->calibration,r->bandgap):LT48_SAFETY_REJECTED;
  if(s==LT48_OK)s=lt48_pic_classic_seek(w,0x2000);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,9);
  if(s==LT48_OK)s=w->delay(w->context,16000);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,11);
  if(s==LT48_OK)s=w->delay(w->context,16000);
  if(s==LT48_OK)s=lt48_pic_cal14_write(w,LT48_CAL14_CAL,&r->calibration,1);
  uint16_t cfg=r->bandgap|0x1ff;
  if(s==LT48_OK)s=lt48_pic_cal14_write(w,LT48_CAL14_CFG,&cfg,1);
  if(s==LT48_OK)s=lt48_pic_cal14_verify(io,expected,work,1,r);
 }else if(s==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)){
  if((work[LT48_CAL14_CFG]&LT48_CAL14_CP)!=LT48_CAL14_CP)s=LT48_SAFETY_REJECTED;
  if(s==LT48_OK&&op==LT48_OPERATION_PROGRAM)s=lt48_pic_cal14_check(NULL,work,1,r);
  if(s==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM){
   for(unsigned a=0;a<0x2100;a++)if((expected[a]&~work[a])&lt48_pic_cal14_mask(a)){s=LT48_SAFETY_REJECTED;break;}
  }
  uint16_t cfg=expected[LT48_CAL14_CFG];expected[LT48_CAL14_CFG]|=LT48_CAL14_CP;
  if(s==LT48_OK)s=lt48_pic_cal14_write(w,0,expected,0x3ff);
  if(s==LT48_OK)s=lt48_pic_cal14_write(w,0x2000,expected+0x2000,4);
  if(s==LT48_OK)s=lt48_pic_cal14_write(w,0x2100,expected+0x2100,128);
  if(s==LT48_OK)s=lt48_pic_cal14_write(w,LT48_CAL14_CFG,expected+LT48_CAL14_CFG,1);
  if(s==LT48_OK)s=lt48_pic_cal14_verify(io,expected,work,0,r);
  expected[LT48_CAL14_CFG]=cfg;
  if(s==LT48_OK&&(cfg&LT48_CAL14_CP)!=LT48_CAL14_CP){
   s=lt48_pic_cal14_write(w,LT48_CAL14_CFG,&cfg,1);
   uint16_t got=0;
   if(s==LT48_OK)s=lt48_pic_cal14_read(w,LT48_CAL14_CFG,&got,1);
   if(s==LT48_OK&&got!=cfg)s=LT48_DEVICE_MISMATCH;
  }
 }else if(s==LT48_OK&&op==LT48_OPERATION_SECURE){
  uint16_t cfg=work[LT48_CAL14_CFG]&~LT48_CAL14_CP,got=0;
  s=lt48_pic_cal14_write(w,LT48_CAL14_CFG,&cfg,1);
  if(s==LT48_OK)s=lt48_pic_cal14_read(w,LT48_CAL14_CFG,&got,1);
  if(s==LT48_OK&&got!=cfg)s=LT48_DEVICE_MISMATCH;
 }else if(s==LT48_OK&&op!=LT48_OPERATION_READ){
  s=op==LT48_OPERATION_VERIFY?lt48_pic_cal14_verify(io,expected,work,0,r):
   lt48_pic_cal14_check(expected,work,op==LT48_OPERATION_BLANK_CHECK,r);
 }
 if(s==LT48_OK){
  uint16_t cal=0,cfg=0;
  s=lt48_pic_cal14_read(w,LT48_CAL14_CAL,&cal,1);
  if(s==LT48_OK)s=lt48_pic_cal14_read(w,LT48_CAL14_CFG,&cfg,1);
  if(s==LT48_OK&&(cal!=r->calibration||(cfg&0x3000)!=r->bandgap))s=LT48_DEVICE_MISMATCH;
 }
 r->operation_status=s;r->cleanup_status=io->cleanup(io->context,s);
 if(s==LT48_OK)s=r->cleanup_status;
 if(s==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,work,LT48_CAL14_WORDS*sizeof *output);
 return s;
}
#endif
