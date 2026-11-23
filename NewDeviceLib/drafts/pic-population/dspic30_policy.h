#ifndef LT48_DSPIC30_POLICY_H
#define LT48_DSPIC30_POLICY_H
#include <labtool48/labtool48.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
/* DS70102D tables2-2,5-8/5-9,11-6/11-7 and11.7. Five editable
 * registers within7 sparse words. System configuration can be rewritten;
 * only FGS protection bits require chip erase for a zero-to-one change. */
typedef struct {
 uint32_t code_words,code_image_bytes,last_code_address,dense_bytes,image_span;
 uint16_t code_row_words,eeprom_words,configuration_words,configuration_slots;
} lt48_dspic30_image_policy;
typedef struct {uint32_t target_address,image_offset;unsigned region,bits,index;} lt48_dspic30_location;
typedef struct {uint16_t write_mask[5],read_mask[5],ones[5];} lt48_dspic30_configuration_rules;
static inline unsigned lt48_dspic30_configuration_slot(unsigned field){
 return field<3?field:field+2;
}
static inline int lt48_dspic30_image_policy_prepare(uint32_t words,uint32_t dense,lt48_dspic30_image_policy *out){
 if(!out||(words!=4096&&words!=8192&&words!=16384)||dense<4*words)return 0;
 uint32_t ee=dense/4-words;
 if(dense%4||(ee!=0&&ee!=512)||(words!=4096&&ee!=512))return 0;
 *out=(lt48_dspic30_image_policy){words,4*words,2*(words-1),dense,0x1f0001au,32,(uint16_t)ee,5,7};return 1;
}
static inline int lt48_dspic30_dense_to_location(const lt48_dspic30_image_policy *p,uint32_t dense,lt48_dspic30_location *out){
 if(!p||!out||dense%4||dense>=p->dense_bytes)return 0;
 if(dense<p->code_image_bytes)*out=(lt48_dspic30_location){dense/2,dense,1,24,dense/4};
 else {uint32_t i=(dense-p->code_image_bytes)/4;*out=(lt48_dspic30_location){0x7ffc00u+2*i,0xfff800u+4*i,2,16,i};}
 return 1;
}
static inline int lt48_dspic30_word_decode(const lt48_dspic30_location *loc,const unsigned char bytes[4],uint32_t *out){
 if(!loc||!bytes||!out||(loc->bits!=16&&loc->bits!=24))return 0;
 *out=bytes[0]|((uint32_t)bytes[1]<<8)|(loc->bits==24?((uint32_t)bytes[2]<<16):0);return 1;
}
static inline int lt48_dspic30_configuration_rules_prepare(unsigned classic,unsigned motor,lt48_dspic30_configuration_rules *out){
 if(!out||classic>1||motor>1)return 0;
 lt48_dspic30_configuration_rules r={{classic?0xc30f:0xc71f,0x803f,motor?0x87b3:0x80b3,3,0xc003},
 {classic?0xc30f:0xc71f,0x803f,motor?0x87b3:0x80b3,3,0xc003},{0,0,motor?0:0x700,4,0}};
 *out=r;return 1;
}
static inline int lt48_dspic30_configuration_normalize(const lt48_dspic30_configuration_rules *r,const uint32_t in[5],uint16_t out[5]){
 if(!r||!in||!out)return 0;
 uint16_t staged[5];for(unsigned i=0;i<5;i++){if(in[i]>65535)return 0;staged[i]=(uint16_t)((in[i]&r->write_mask[i])|r->ones[i]);}
 memcpy(out,staged,sizeof staged);return 1;
}
static inline int lt48_dspic30_configuration_read_image(const unsigned char *image,size_t size,uint32_t out[5]){
 if(!image||!out||size!=0x1f0001au)return 0;
 uint32_t staged[5];for(unsigned i=0;i<5;i++){uint32_t at=0x1f00000u+4*lt48_dspic30_configuration_slot(i);staged[i]=image[at]|((uint32_t)image[at+1]<<8);}
 memcpy(out,staged,sizeof staged);return 1;
}
static inline int lt48_dspic30_configuration_write_image(const lt48_dspic30_configuration_rules *r,unsigned char *image,size_t size,const uint32_t in[5]){
 if(!image||size!=0x1f0001au)return 0;
 uint16_t v[5];if(!lt48_dspic30_configuration_normalize(r,in,v))return 0;
 unsigned char staged[26];memcpy(staged,image+0x1f00000u,sizeof staged);
 for(unsigned i=0;i<5;i++){unsigned at=4*lt48_dspic30_configuration_slot(i);staged[at]=(unsigned char)v[i];staged[at+1]=(unsigned char)(v[i]>>8);}
 staged[12]=0x0f;staged[13]=0x31;staged[16]=0x0f;staged[17]=0x33;
 memcpy(image+0x1f00000u,staged,sizeof staged);return 1;
}
static inline int lt48_dspic30_configuration_edit(const lt48_dspic30_configuration_rules *r,unsigned char *image,size_t size,const lt48_runtime_configuration_value *edits,size_t count){
 if((count&&!edits)||count>5)return 0;
 uint32_t v[5];unsigned seen=0;if(!lt48_dspic30_configuration_read_image(image,size,v))return 0;
 for(size_t i=0;i<count;i++){unsigned f=edits[i].field;
  if(edits[i].object_id!=-1||edits[i].category!=2||f>=5||edits[i].value>65535||(seen&(1u<<f)))return 0;
  seen|=1u<<f;v[f]=edits[i].value;
 }
 return lt48_dspic30_configuration_write_image(r,image,size,v);
}
static inline int lt48_dspic30_configuration_before_verify(const lt48_dspic30_configuration_rules *r,const uint32_t requested[5],uint16_t plain[5],int *protect){
 if(!protect||!lt48_dspic30_configuration_normalize(r,requested,plain))return 0;
 *protect=(plain[3]&3)!=3;plain[3]=7;return 1;
}
static inline int lt48_dspic30_configuration_can_program(const lt48_dspic30_configuration_rules *r,const uint16_t current[5],const uint32_t requested[5],uint16_t out[5]){
 if(!current||!out)return 0;
 uint16_t staged[5];if(!lt48_dspic30_configuration_normalize(r,requested,staged))return 0;
 if((staged[3]&3)&~current[3])return 0;
 memcpy(out,staged,sizeof staged);return 1;
}
#endif
