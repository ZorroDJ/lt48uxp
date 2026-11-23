#ifndef LT48_DRAFT_PIC24_KA_CONFIGURATION_H
#define LT48_DRAFT_PIC24_KA_CONFIGURATION_H
#include "pic24_ka_image_policy.h"
#include <labtool48/labtool48.h>
#include <string.h>
/* DS39927C/DS30009995E26.1 and DS30009919D3.8. Logical editor/policy only.
 * FICD6 is programmed1 per the programming specification, but not compared:
 * the family register diagrams call it unimplemented/read0. KA3 F parts must
 * keep FPOR4/2 at1; KA1 20-pin parts must keep FPOR4 at1. These are distinct
 * from bits that are actually readable user configuration.
 * Authored conversion deliberately fixes the source W54f FOSC duplicate-byte
 * store; category2 fields retain their source identities. No admission/I/O. */
typedef struct {
 uint16_t write_mask[8],read_mask[8],ones[8];
 uint32_t code_words;
 unsigned series,fv,pins;
} lt48_pic24_ka_configuration_rules;
static inline int lt48_pic24_ka_configuration_rules_prepare(unsigned series,
 uint32_t words,unsigned fv,unsigned pins,lt48_pic24_ka_configuration_rules *out){
 if(!out||(pins!=20&&pins!=28)||fv>1||
    !((series==1&&!fv&&(words==2816||words==5632))||(series==3&&words==11264)))return 0;
 lt48_pic24_ka_configuration_rules r={{15,3,0x87,255,0xdf,0xfb,0x83,255},
  {15,3,0x87,255,0xdf,0xfb,0x83,255},{0},words,series,fv,pins};
 r.ones[6]=0x40;
 if(series==1&&pins==20){r.ones[5]=0x10;
 r.read_mask[5]&=(uint16_t)~0x10u;
 }
 if(series==3){
  r.write_mask[2]=r.read_mask[2]=0xe7;
  r.write_mask[4]=r.read_mask[4]=255;
  r.write_mask[5]=r.read_mask[5]=255;
  r.write_mask[7]=r.read_mask[7]=0xdf;
  if(!fv){r.ones[5]=0x14;
  r.read_mask[5]&=(uint16_t)~0x14u;
  }
 }
 *out=r;
 return 1;
}
static inline int lt48_pic24_ka_configuration_normalize(const lt48_pic24_ka_configuration_rules *r,
 const uint32_t input[8],uint16_t output[8]){
 if(!r||!input||!output)return 0;
 uint16_t staged[8];
 for(unsigned i=0;i<8;i++){if(input[i]>65535)return 0;
 staged[i]=(uint16_t)((input[i]&r->write_mask[i])|r->ones[i]);
 }
 unsigned bss=(staged[0]>>1)&7;
 if(bss==3||(r->series==1&&(bss==4||bss==0))||
    (r->code_words==2816&&(bss==5||bss==1))||!(staged[3]&0x18)||!(staged[6]&3))return 0;
 memcpy(output,staged,sizeof(staged));
 return 1;
}
static inline int lt48_pic24_ka_configuration_read_image(const unsigned char *image,size_t size,uint32_t output[8]){
 if(!image||!output||size!=0x1f00024u)return 0;
 uint32_t staged[8];
 for(unsigned i=0;i<8;i++){uint32_t a=0x1f00000u+4*(i?i+1:0);
 staged[i]=image[a]|((uint32_t)image[a+1]<<8);
 }
 memcpy(output,staged,sizeof(staged));
 return 1;
}
static inline int lt48_pic24_ka_configuration_write_image(const lt48_pic24_ka_configuration_rules *rules,
 unsigned char *image,size_t size,const uint32_t values[8]){
 if(!image||size!=0x1f00024u)return 0;
 uint16_t normalized[8];
 if(!lt48_pic24_ka_configuration_normalize(rules,values,normalized))return 0;
 unsigned char staged[36];
 memcpy(staged,image+0x1f00000u,sizeof(staged));
 for(unsigned i=0;i<8;i++){unsigned a=4*(i?i+1:0);
 staged[a]=(unsigned char)normalized[i];
 staged[a+1]=(unsigned char)(normalized[i]>>8);
 }
 memcpy(image+0x1f00000u,staged,sizeof(staged));
 return 1;
}
static inline int lt48_pic24_ka_configuration_edit(const lt48_pic24_ka_configuration_rules *rules,
 unsigned char *image,size_t size,const lt48_runtime_configuration_value *edits,size_t count){
 if((count&&!edits)||count>8)return 0;
 uint32_t values[8];
 unsigned seen=0;
 if(!lt48_pic24_ka_configuration_read_image(image,size,values))return 0;
 for(size_t i=0;i<count;i++){
  if(edits[i].object_id!=-1||edits[i].category!=2||edits[i].field>=8||edits[i].value>65535||
     (seen&(1u<<edits[i].field)))return 0;
  seen|=1u<<edits[i].field;
  values[edits[i].field]=edits[i].value;
 }
 return lt48_pic24_ka_configuration_write_image(rules,image,size,values);
}
static inline int lt48_pic24_ka_configuration_before_verify(const lt48_pic24_ka_configuration_rules *rules,
 const uint32_t requested[8],uint16_t unprotected[8],int *protect_after_verify){
 if(!protect_after_verify||!lt48_pic24_ka_configuration_normalize(rules,requested,unprotected))return 0;
 *protect_after_verify=unprotected[0]!=15||unprotected[1]!=3;
 unprotected[0]=15;
 unprotected[1]=3;
 return 1;
}
static inline int lt48_pic24_ka_configuration_can_program(const lt48_pic24_ka_configuration_rules *rules,
 const uint16_t current[8],const uint32_t requested[8],uint16_t normalized[8]){
 if(!rules||!current||!requested||!normalized)return 0;
 uint16_t stage[8];
 if(!lt48_pic24_ka_configuration_normalize(rules,requested,stage))return 0;
 /* The selected source callback performs4004 programming only, no per-word
  * erase. Any requested zero-to-one transition requires explicit chip erase. */
 for(unsigned i=0;i<8;i++)if((stage[i]&rules->read_mask[i])&~current[i])return 0;
 memcpy(normalized,stage,sizeof(stage));
 return 1;
}
#endif
