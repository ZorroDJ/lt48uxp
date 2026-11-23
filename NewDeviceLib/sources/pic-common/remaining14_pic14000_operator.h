#ifndef LT48_PIC_REMAINING14_PIC14000_OPERATOR_H
#define LT48_PIC_REMAINING14_PIC14000_OPERATOR_H
#include "remaining14_otp_operator.h"
/* DS30555B sections3/4: separate user-code and calibration protection.
 * Calibration is64words atFC0..FFF; its16bit additive checksum is zero.
 * Factory trim bits1/6 travel with saved calibration. No fabricated defaults. */
enum {RM14_CAL_FIRST=0xfc0,RM14_CAL_END=0x1000,RM14_CPP=0x1e30,RM14_CPC=0x2180,RM14_TRIM=0x42};
static inline unsigned rm14_cal_checksum(const uint16_t *image){
 unsigned sum=0;for(unsigned a=RM14_CAL_FIRST;a<RM14_CAL_END;a++)sum+=image[a];return sum&0xffff;
}
static inline int rm14_cal_placeholder(const uint16_t *image){
 for(unsigned a=RM14_CAL_FIRST;a<RM14_CAL_END;a++)if(image[a]!=0x3fff)return 0;
 return 1;
}
static inline int rm14_pic14000_configuration_valid(unsigned value){
 unsigned cpp=value&RM14_CPP,cpc=value&RM14_CPC;
 return value<=0x3fff&&(cpp==0||cpp==RM14_CPP)&&(cpc==0||cpc==RM14_CPC);
}
/* Exact original W73 DEVINIT rule, checked across all low3-bit values.
 * The source inspects the raw low two bits (including factory trim bit1). */
static inline void rm14_pic14000_levels(rm14_otp_policy *p,unsigned configuration){
 p->common.configuration.minimum=(configuration&3)==2?450:400;
 p->common.configuration.maximum=550;
}
static inline lt48_status rm14_pic14000_check(const rm14_otp_policy *p,const uint16_t *want,
 const uint16_t *got,int blank,otp14_operator_result *r){
 r->mismatches=0;r->first_mismatch=UINT32_MAX;
 if(blank&&rm14_cal_checksum(got)&&(!want||rm14_cal_checksum(want))){r->first_mismatch=2*RM14_CAL_FIRST;r->mismatches=1;return LT48_DEVICE_MISMATCH;}
 for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++){
  if(blank&&a>=RM14_CAL_FIRST&&a<RM14_CAL_END)continue;
  unsigned mask=otp14_operator_mask(&p->common,a);
  if(blank&&a>=0x2000&&a<=0x2003)mask=15;
  if(blank&&a==0x2007)mask&=~(RM14_CPC|RM14_TRIM);
  if(((blank?0x3fff:want[a])^got[a])&mask){if(!r->mismatches)r->first_mismatch=2*a;r->mismatches++;}
 }
 return r->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
static inline lt48_status rm14_pic14000_config_check(const rm14_otp_policy *p,const rm14_otp_io *io,unsigned expected){
 const unsigned levels[]={p->common.configuration.nominal,p->common.configuration.minimum,p->common.configuration.maximum};
 for(unsigned n=0;n<3;n++){
  uint16_t actual=0;lt48_status st=io->configuration.read(io->configuration.context,levels[n],&actual);
  if(st!=LT48_OK)return st;
  if(actual!=expected)return LT48_DEVICE_MISMATCH;
 }
 return LT48_OK;
}
static inline lt48_status rm14_pic14000_execute(const rm14_otp_policy *p,const rm14_otp_io *io,
 lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *r){
 if(!p||p->common.code_words!=RM14_CAL_END||p->common.id_mask!=0x3fff||
    p->common.configuration.mask!=0x3fff||p->common.configuration.protection!=RM14_CPP||
    !rm14_otp_target_valid(p,0x3fff)||!io||!io->scan||!io->program||!io->configuration.action||
    !io->configuration.read||!io->cleanup.source||!io->cleanup.command||!scratch||!r)return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_ERASE||op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
 if((op==LT48_OPERATION_READ&&!output)||
   ((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&!image)||
   otp14_operator_overlap(image,scratch)||(op==LT48_OPERATION_READ&&otp14_operator_overlap(output,scratch)))return LT48_INVALID_ARGUMENT;
 if(image){
  for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)if(image[a]>0x3fff||(!otp14_operator_mask(&p->common,a)&&image[a]!=0x3fff))return LT48_SAFETY_REJECTED;
  if(!rm14_pic14000_configuration_valid(image[0x2007]))return LT48_SAFETY_REJECTED;
 }
 uint16_t *expected=NULL;
 if(image){expected=malloc(OTP14_IMAGE_WORDS*sizeof *expected);if(!expected)return LT48_NO_MEMORY;memcpy(expected,image,OTP14_IMAGE_WORDS*sizeof *expected);}
 rm14_otp_policy resolved=*p;p=&resolved;resolved.common.configuration.minimum=400;resolved.common.configuration.maximum=550;
 memset(r,0,sizeof *r);r->first_mismatch=UINT32_MAX;lt48_status st=LT48_OK;
 unsigned levels[]={p->common.configuration.nominal,p->common.configuration.minimum,p->common.configuration.maximum};
 if(op==LT48_OPERATION_SECURE){
  uint16_t current=0;st=io->configuration.read(io->configuration.context,levels[0],&current);
  if(st==LT48_OK&&!rm14_pic14000_configuration_valid(current))st=LT48_SAFETY_REJECTED;
  unsigned target=current&~RM14_CPP;rm14_pic14000_levels(&resolved,target);
  /* Secure protects user code only. Never seal calibration as a side effect. */
  if(st==LT48_OK)st=io->configuration.action(io->configuration.context,10,target|RM14_TRIM,1);
  if(st==LT48_OK)st=rm14_pic14000_config_check(p,io,target);
 }else if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM){
  st=io->scan(io->context,levels[0],scratch);unsigned preserve=1,target=0;
  if(st==LT48_OK){
   unsigned current=scratch[0x2007];
   if(!rm14_pic14000_configuration_valid(current)||(current&RM14_CPP)!=RM14_CPP)st=LT48_SAFETY_REJECTED;
   else if(!rm14_cal_checksum(scratch)){
    /* Valid calibration is read-only to this operation even on unprotected JW. */
    if(!rm14_cal_placeholder(expected))for(unsigned a=RM14_CAL_FIRST;a<RM14_CAL_END;a++)
     if(expected[a]!=scratch[a]){st=LT48_SAFETY_REJECTED;r->first_mismatch=2*a;break;}
    memcpy(expected+RM14_CAL_FIRST,scratch+RM14_CAL_FIRST,(RM14_CAL_END-RM14_CAL_FIRST)*sizeof *expected);
    expected[0x2007]=(expected[0x2007]&~RM14_TRIM)|(current&RM14_TRIM);
   }else if((current&RM14_CPC)!=RM14_CPC||rm14_cal_placeholder(expected)||rm14_cal_checksum(expected))st=LT48_SAFETY_REJECTED;
   else preserve=0; /* Explicit valid saved calibration + trim restoration. */
   if((expected[0x2007]&RM14_CPC)==RM14_CPC)expected[0x2007]=(expected[0x2007]&~RM14_CPC)|(current&RM14_CPC);
   target=expected[0x2007];rm14_pic14000_levels(&resolved,target);
   levels[1]=p->common.configuration.minimum;levels[2]=p->common.configuration.maximum;
  }
  if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM)
   st=rm14_pic14000_check(p,preserve?NULL:expected,scratch,1,r);

  if(st==LT48_OK)for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)
   if(expected[a]&~scratch[a]&otp14_operator_mask(&p->common,a)){st=LT48_SAFETY_REJECTED;r->first_mismatch=2*a;break;}
  if(st==LT48_OK)st=io->program(io->context,expected,preserve);
  /* Defer both requested protections until all code/calibration/ID readback. */
  unsigned initial=(target|RM14_CPP);initial=(initial&~RM14_CPC)|(scratch[0x2007]&RM14_CPC);
  if(st==LT48_OK)st=io->configuration.action(io->configuration.context,14,initial|(preserve?RM14_TRIM:0),0);
  if(st==LT48_OK)st=rm14_pic14000_config_check(p,io,initial);
  if(st==LT48_OK)expected[0x2007]=initial;
  for(unsigned n=0;st==LT48_OK&&n<3;n++){
   st=io->scan(io->context,levels[n],scratch);if(st==LT48_OK)st=rm14_pic14000_check(p,expected,scratch,0,r);
  }
  if(st==LT48_OK&&target!=initial){
   st=io->configuration.action(io->configuration.context,10,target|RM14_TRIM,0);
   if(st==LT48_OK)st=rm14_pic14000_config_check(p,io,target);
  }
 }else{
  unsigned prefetched=0;
  if(expected&&(op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&rm14_cal_placeholder(image)){
   st=io->scan(io->context,levels[0],scratch);prefetched=st==LT48_OK;
   if(st==LT48_OK&&rm14_cal_checksum(scratch)){st=LT48_DEVICE_MISMATCH;r->first_mismatch=2*RM14_CAL_FIRST;r->mismatches=1;}
   if(st==LT48_OK){
    memcpy(expected+RM14_CAL_FIRST,scratch+RM14_CAL_FIRST,(RM14_CAL_END-RM14_CAL_FIRST)*sizeof *expected);
    expected[0x2007]=(expected[0x2007]&~RM14_TRIM)|(scratch[0x2007]&RM14_TRIM);
   }
  }
  if(expected&&prefetched&&(image[0x2007]&RM14_CPC)==RM14_CPC)
   expected[0x2007]=(expected[0x2007]&~RM14_CPC)|(scratch[0x2007]&RM14_CPC);
  if(expected)rm14_pic14000_levels(&resolved,expected[0x2007]);
  levels[1]=p->common.configuration.minimum;levels[2]=p->common.configuration.maximum;
  unsigned passes=op==LT48_OPERATION_VERIFY&&(!p->verify_selection||p->verify_selection==4)?3:1;
  for(unsigned n=0;st==LT48_OK&&n<passes;n++){
   unsigned level=op==LT48_OPERATION_VERIFY&&p->verify_selection&&p->verify_selection<4?p->verify_selection-1:n;
   if(prefetched&&level==0)prefetched=0;
   else st=io->scan(io->context,levels[level],scratch);
   if(st==LT48_OK&&expected&&n==0&&(image[0x2007]&RM14_CPC)==RM14_CPC)
    expected[0x2007]=(expected[0x2007]&~RM14_CPC)|(scratch[0x2007]&RM14_CPC);
   if(st==LT48_OK&&op!=LT48_OPERATION_READ)
    st=rm14_pic14000_check(p,op==LT48_OPERATION_BLANK_CHECK?NULL:expected,scratch,op==LT48_OPERATION_BLANK_CHECK,r);
  }
 }

 r->operation_status=st;st=lt48_pic_cleanup(&io->cleanup,st,&r->cleanup);
 if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(output,scratch,OTP14_IMAGE_WORDS*sizeof *output);
 free(expected);return st;
}
#endif
