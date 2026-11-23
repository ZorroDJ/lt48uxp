#ifndef LT48_PIC24_KLKM_CONFIGURATION_H
#define LT48_PIC24_KLKM_CONFIGURATION_H
#include "pic24_ka_configuration.h"

/* DS30000625E tables2-2/2-3 and3-7; DS30001037D registers23-1..7;
 * DS30003030C registers25-1..7. Seven registers, eight sparse slots.
 * KL reads FICD6:5 as1; KM reads them as0. Both retain the programming
 * specification's E3h convention when writing FICD. FWDT5 is implemented
 * on both families. FV KM also implements FPOR2 (RETCFG). No admission. */
typedef lt48_pic24_ka_image_policy lt48_pic24_klkm_image_policy;
typedef lt48_pic24_ka_location lt48_pic24_klkm_location;
typedef lt48_pic24_ka_configuration_rules lt48_pic24_klkm_configuration_rules;

static inline int lt48_pic24_klkm_image_policy_prepare(uint32_t words,uint32_t dense,
 lt48_pic24_klkm_image_policy *out){
 if(!out||(words!=2816&&words!=5632))return 0;
 lt48_pic24_klkm_image_policy p;
 if(!lt48_pic24_ka_image_policy_prepare(words,dense,&p))return 0;
 p.image_span=0x1f00020u;p.configuration_words=7;p.configuration_slots=8;
 *out=p;return 1;
}
static inline int lt48_pic24_klkm_dense_to_location(const lt48_pic24_klkm_image_policy *p,
 uint32_t dense,lt48_pic24_klkm_location *out){
 return lt48_pic24_ka_dense_to_location(p,dense,out);
}
static inline int lt48_pic24_klkm_word_decode(const lt48_pic24_klkm_location *p,
 const unsigned char bytes[4],uint32_t *out){
 return lt48_pic24_ka_word_decode(p,bytes,out);
}
static inline int lt48_pic24_klkm_configuration_rules_prepare(unsigned series,uint32_t words,
 unsigned km,unsigned pins,lt48_pic24_klkm_configuration_rules *out){
 if(!out||series!=1||km>1||pins!=28||(words!=2816&&words!=5632))return 0;
 lt48_pic24_klkm_configuration_rules r;
 if(!lt48_pic24_ka_configuration_rules_prepare(1,words,0,28,&r))return 0;
 r.fv=km;
 r.write_mask[2]=r.read_mask[2]=0xe7;
 r.write_mask[4]=r.read_mask[4]=0xff;
 r.write_mask[5]=r.read_mask[5]=km?0xff:0xfb;
 /* Compare implemented bits only. The shared source ReadCfg convention
  * clears FICD5 even on KL,whose silicon reports that reserved bit as1. */
 r.write_mask[6]=r.read_mask[6]=0x83;r.ones[6]=0x60;
 r.write_mask[7]=r.read_mask[7]=r.ones[7]=0;
 *out=r;return 1;
}
static inline int lt48_pic24_klkm_configuration_normalize(const lt48_pic24_klkm_configuration_rules *r,
 const uint32_t input[7],uint16_t output[7]){
 if(!r||!input||!output)return 0;
 uint32_t expanded[8]={0};uint16_t staged[8];
 memcpy(expanded,input,7*sizeof(*input));
 if(!lt48_pic24_ka_configuration_normalize(r,expanded,staged))return 0;
 memcpy(output,staged,7*sizeof(*output));return 1;
}
static inline int lt48_pic24_klkm_configuration_read_image(const unsigned char *image,size_t size,
 uint32_t output[7]){
 if(!image||!output||size!=0x1f00020u)return 0;
 uint32_t staged[7];
 for(unsigned i=0;i<7;i++){
  uint32_t at=0x1f00000u+4*(i?i+1:0);
  staged[i]=image[at]|((uint32_t)image[at+1]<<8);
 }
 memcpy(output,staged,sizeof(staged));return 1;
}
static inline int lt48_pic24_klkm_configuration_write_image(const lt48_pic24_klkm_configuration_rules *r,
 unsigned char *image,size_t size,const uint32_t input[7]){
 if(!image||size!=0x1f00020u)return 0;
 uint16_t values[7];if(!lt48_pic24_klkm_configuration_normalize(r,input,values))return 0;
 unsigned char staged[32];memcpy(staged,image+0x1f00000u,sizeof(staged));
 for(unsigned i=0;i<7;i++){
  unsigned at=4*(i?i+1:0);staged[at]=(unsigned char)values[i];staged[at+1]=(unsigned char)(values[i]>>8);
 }
 memcpy(image+0x1f00000u,staged,sizeof(staged));return 1;
}
static inline int lt48_pic24_klkm_configuration_edit(const lt48_pic24_klkm_configuration_rules *r,
 unsigned char *image,size_t size,const lt48_runtime_configuration_value *edits,size_t count){
 if((count&&!edits)||count>7)return 0;
 uint32_t values[7];unsigned seen=0;
 if(!lt48_pic24_klkm_configuration_read_image(image,size,values))return 0;
 for(size_t i=0;i<count;i++){
  if(edits[i].object_id!=-1||edits[i].category!=2||edits[i].field>=7||edits[i].value>65535||
     (seen&(1u<<edits[i].field)))return 0;
  seen|=1u<<edits[i].field;values[edits[i].field]=edits[i].value;
 }
 return lt48_pic24_klkm_configuration_write_image(r,image,size,values);
}
static inline int lt48_pic24_klkm_configuration_before_verify(const lt48_pic24_klkm_configuration_rules *r,
 const uint32_t requested[7],uint16_t unprotected[7],int *protect){
 if(!protect||!lt48_pic24_klkm_configuration_normalize(r,requested,unprotected))return 0;
 *protect=unprotected[0]!=15||unprotected[1]!=3;unprotected[0]=15;unprotected[1]=3;return 1;
}
static inline int lt48_pic24_klkm_configuration_can_program(const lt48_pic24_klkm_configuration_rules *r,
 const uint16_t current[7],const uint32_t requested[7],uint16_t output[7]){
 if(!r||!current||!requested||!output)return 0;
 uint16_t staged[7];if(!lt48_pic24_klkm_configuration_normalize(r,requested,staged))return 0;
 for(unsigned i=0;i<7;i++)if((staged[i]&r->read_mask[i])&~current[i])return 0;
 memcpy(output,staged,sizeof(staged));return 1;
}
#endif
