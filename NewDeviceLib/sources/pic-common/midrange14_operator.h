#ifndef LT48_PIC_MIDRANGE14_OPERATOR_H
#define LT48_PIC_MIDRANGE14_OPERATOR_H
#include "midrange14_protocol.h"
#include <string.h>
enum {LT48_MIDRANGE_IMAGE_WORDS=0x200a};
typedef struct {
 const lt48_pic_midrange_policy *protocol;
 uint16_t configuration_mask,code_protect,identity_mask,identity_value;
} lt48_pic_midrange_operation_policy;
typedef struct {
 const lt48_pic_classic_wire *wire;void *context;
 lt48_status (*cleanup)(void *,lt48_status);
 const unsigned *verify_levels;unsigned verify_count;
 lt48_status (*select_voltage)(void *,unsigned);
 lt48_pic_midrange_commit_callback commit;
} lt48_pic_midrange_io;
typedef struct {
 lt48_status operation_status,cleanup_status;
 unsigned mismatches,first_mismatch,factory_saved;
 uint16_t original_identity,original_revision,original_calibration[2];
} lt48_pic_midrange_result;
static inline int lt48_pic_midrange_overlap(const uint16_t *a,const uint16_t *b){
 if(!a||!b)return 0;
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return (x<y?y-x:x-y)<LT48_MIDRANGE_IMAGE_WORDS*sizeof(*a);
}
static inline unsigned lt48_pic_midrange_mask(const lt48_pic_midrange_operation_policy *p,unsigned a){
 return a==0x2007?p->configuration_mask:lt48_pic_midrange_address(p->protocol,a,0)?0x3fff:0;
}
static inline int lt48_pic_midrange_factory(const lt48_pic_midrange_operation_policy *p,unsigned a){return (p->protocol->has_revision&&a==0x2005)||a==0x2006||a==0x2008||a==0x2009;}
static inline lt48_status lt48_pic_midrange_scan(const lt48_pic_midrange_operation_policy *p,const lt48_pic_classic_wire *w,uint16_t *out){
 for(unsigned a=0;a<LT48_MIDRANGE_IMAGE_WORDS;a++)out[a]=0x3fff;
 lt48_status st=lt48_pic_midrange_read(p->protocol,w,0,out,p->protocol->code_words);
 if(st==LT48_OK)st=lt48_pic_midrange_read(p->protocol,w,0x2000,out+0x2000,4);
 if(st==LT48_OK){unsigned base=p->protocol->has_revision?0x2005:0x2006;st=lt48_pic_midrange_read(p->protocol,w,base,out+base,0x200a-base);}return st;
}
static inline lt48_status lt48_pic_midrange_check(const lt48_pic_midrange_operation_policy *p,const uint16_t *want,const uint16_t *got,int blank,lt48_pic_midrange_result *r){
 r->mismatches=0;r->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<LT48_MIDRANGE_IMAGE_WORDS;a++){
  if(blank&&lt48_pic_midrange_factory(p,a))continue;
  unsigned mask=lt48_pic_midrange_mask(p,a);
  if(((blank?0x3fff:want[a])^got[a])&mask){if(!r->mismatches)r->first_mismatch=2*a;r->mismatches++;}
 }return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
static inline lt48_status lt48_pic_midrange_verify_levels(const lt48_pic_midrange_operation_policy *p,const lt48_pic_midrange_io *io,const uint16_t *expected,uint16_t *work,int blank,lt48_pic_midrange_result *r){
 if(!io->verify_count)return lt48_pic_midrange_check(p,expected,work,blank,r);
 lt48_status st=LT48_OK;
 for(unsigned i=0;st==LT48_OK&&i<io->verify_count;i++){
  st=io->select_voltage(io->context,io->verify_levels[i]);
  if(st==LT48_OK)st=lt48_pic_midrange_scan(p,io->wire,work);
  if(st==LT48_OK)st=lt48_pic_midrange_check(p,expected,work,blank,r);
 }
 lt48_status reset=io->select_voltage(io->context,0);
 return st==LT48_OK?reset:st;
}
/* Private complete logical lifecycle, with Program requiring blank writable
 * memory. Over-program permits only1-to-0 changes. Hardware entry, source
 * conversion, atomic controller pulse delivery and public admission are separate. */
static inline lt48_status lt48_pic_midrange_execute(const lt48_pic_midrange_operation_policy *p,const lt48_pic_midrange_io *io,lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *work,uint16_t *expected,lt48_pic_midrange_result *r){
 if(!r)return LT48_INVALID_ARGUMENT;
 memset(r,0,sizeof(*r));r->first_mismatch=UINT32_MAX;
 if(!p||!lt48_pic_midrange_valid(p->protocol)||!p->configuration_mask||p->configuration_mask>0x3fff||!p->code_protect||(p->code_protect&~p->configuration_mask)||!p->identity_mask||p->identity_mask>0x3fff||p->identity_value>0x3fff||!io||!lt48_pic_classic_wire_valid(io->wire)||!io->cleanup||!work||!expected||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE||
    lt48_pic_midrange_overlap(work,expected)||lt48_pic_midrange_overlap(image,work)||lt48_pic_midrange_overlap(image,expected)||
    (op==LT48_OPERATION_READ&&(!output||lt48_pic_midrange_overlap(output,work)||lt48_pic_midrange_overlap(output,expected)))||
    (io->verify_count&&(!io->verify_levels||!io->select_voltage||io->verify_count>3))||
    (p->protocol->internal_configuration&&!io->commit&&
     (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_SECURE)))return LT48_INVALID_ARGUMENT;
 int programming=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 if((programming||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image)return LT48_INVALID_ARGUMENT;
 if(image)for(unsigned a=0;a<LT48_MIDRANGE_IMAGE_WORDS;a++)if((image[a]>0x3fff&&image[a]!=0xffff)||(!lt48_pic_midrange_mask(p,a)&&(image[a]&0x3fff)!=0x3fff))return LT48_SAFETY_REJECTED;
 lt48_status st=lt48_pic_midrange_scan(p,io->wire,work);
 for(unsigned a=0;a<LT48_MIDRANGE_IMAGE_WORDS;a++)expected[a]=image?image[a]&0x3fff:0x3fff;
 if(st==LT48_OK){
  r->original_identity=work[0x2006];r->original_revision=work[0x2005];r->original_calibration[0]=work[0x2008];r->original_calibration[1]=work[0x2009];r->factory_saved=1;
  if((work[0x2006]^p->identity_value)&p->identity_mask){st=LT48_DEVICE_MISMATCH;r->first_mismatch=0x400c;r->mismatches=1;}
  for(unsigned a=0x2005;a<=0x2009;a++)if(lt48_pic_midrange_factory(p,a)){
   if(expected[a]==0x3fff)expected[a]=work[a];else if(programming&&expected[a]!=work[a])st=LT48_SAFETY_REJECTED;
  }
 }
 if(st==LT48_OK&&programming){
  if(!(work[0x2007]&p->code_protect))st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM)st=lt48_pic_midrange_check(p,expected,work,1,r);
  if(st==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM)for(unsigned a=0;a<LT48_MIDRANGE_IMAGE_WORDS;a++)if((expected[a]&~work[a])&lt48_pic_midrange_mask(p,a)){st=LT48_SAFETY_REJECTED;break;}
  uint16_t final=expected[0x2007];expected[0x2007]|=p->code_protect|(0x3fff^p->configuration_mask);
  if(st==LT48_OK)st=lt48_pic_midrange_program_with_commit(p->protocol,io->wire,0,expected,p->protocol->code_words,io->commit,io->context);
  if(st==LT48_OK)st=lt48_pic_midrange_program_with_commit(p->protocol,io->wire,0x2000,expected+0x2000,4,io->commit,io->context);
  if(st==LT48_OK)st=lt48_pic_midrange_program_with_commit(p->protocol,io->wire,0x2007,expected+0x2007,1,io->commit,io->context);
  if(st==LT48_OK)st=lt48_pic_midrange_scan(p,io->wire,work);
  if(st==LT48_OK)st=lt48_pic_midrange_verify_levels(p,io,expected,work,0,r);
  expected[0x2007]=final;
  if(st==LT48_OK&&!(final&p->code_protect)){
   st=lt48_pic_midrange_program_with_commit(p->protocol,io->wire,0x2007,&final,1,io->commit,io->context);uint16_t got=0;
   if(st==LT48_OK)st=lt48_pic_midrange_read(p->protocol,io->wire,0x2007,&got,1);
   if(st==LT48_OK&&((got^final)&p->configuration_mask))st=LT48_DEVICE_MISMATCH;
  }
 }else if(st==LT48_OK&&op==LT48_OPERATION_ERASE){
  st=lt48_pic_midrange_erase(p->protocol,io->wire);
  if(st==LT48_OK)st=lt48_pic_midrange_scan(p,io->wire,work);
  if(st==LT48_OK)st=lt48_pic_midrange_check(p,expected,work,1,r);
 }else if(st==LT48_OK&&op==LT48_OPERATION_SECURE){
  uint16_t cfg=work[0x2007]&~p->code_protect,got=0;
  st=lt48_pic_midrange_program_with_commit(p->protocol,io->wire,0x2007,&cfg,1,io->commit,io->context);
  if(st==LT48_OK)st=lt48_pic_midrange_read(p->protocol,io->wire,0x2007,&got,1);
  if(st==LT48_OK&&((got^cfg)&p->configuration_mask))st=LT48_DEVICE_MISMATCH;
 }else if(st==LT48_OK&&op!=LT48_OPERATION_READ){
  st=op==LT48_OPERATION_VERIFY?lt48_pic_midrange_verify_levels(p,io,expected,work,0,r):lt48_pic_midrange_check(p,expected,work,op==LT48_OPERATION_BLANK_CHECK,r);
 }
 if(st==LT48_OK&&r->factory_saved){uint16_t factory[5];unsigned revision=p->protocol->has_revision,base=revision?0x2005:0x2006;
  st=lt48_pic_midrange_read(p->protocol,io->wire,base,factory,0x200a-base);
  if(st==LT48_OK&&((revision&&factory[0]!=r->original_revision)||factory[revision]!=r->original_identity||factory[revision+2]!=r->original_calibration[0]||factory[revision+3]!=r->original_calibration[1]))st=LT48_DEVICE_MISMATCH;
 }
 r->operation_status=st;r->cleanup_status=io->cleanup(io->context,st);
 if(st==LT48_OK)st=r->cleanup_status;
 if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,work,LT48_MIDRANGE_IMAGE_WORDS*sizeof(*output));
 return st;
}
#endif
