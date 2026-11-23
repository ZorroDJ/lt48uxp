#ifndef LT48_DRAFT_PIC24_KA_IMAGE_POLICY_H
#define LT48_DRAFT_PIC24_KA_IMAGE_POLICY_H
#include <stddef.h>
#include <stdint.h>
/* DS30009919D2.2: three distinct address/width spaces. The canonical image
 * preserves the original sparse addressing; dense transfer capacity excludes
 * holes and the separately handled configuration registers. This descriptor
 * grants no runtime/GUI admission and performs no hardware I/O. */
enum lt48_pic24_ka_region {LT48_PIC24_KA_CODE=1,LT48_PIC24_KA_EEPROM,LT48_PIC24_KA_CONFIGURATION};
typedef struct {
 uint32_t code_words,code_image_bytes,last_code_address,dense_bytes,image_span;
 uint16_t code_row_words,eeprom_words,configuration_words,configuration_slots;
} lt48_pic24_ka_image_policy;
typedef struct {uint32_t target_address,image_offset;unsigned region,bits,index;} lt48_pic24_ka_location;
static inline int lt48_pic24_ka_image_policy_prepare(uint32_t code_words,uint32_t source_dense_bytes,
 lt48_pic24_ka_image_policy *out){
 if(!out||(code_words!=2816&&code_words!=5632&&code_words!=11264)||
    source_dense_bytes!=4*(code_words+256))return 0;
 *out=(lt48_pic24_ka_image_policy){code_words,4*code_words,2*(code_words-1),source_dense_bytes,
  0x01f00024u,32,256,8,9};return 1;
}
static inline int lt48_pic24_ka_target_to_image(const lt48_pic24_ka_image_policy *p,
 uint32_t address,lt48_pic24_ka_location *out){
 if(!p||!out||address%2)return 0;
 lt48_pic24_ka_location r={address,address*2,0,0,0};
 if(address<=p->last_code_address){r.region=LT48_PIC24_KA_CODE;r.bits=24;r.index=address/2;}
 else if(address>=0x7ffe00u&&address<=0x7ffffeu){r.region=LT48_PIC24_KA_EEPROM;r.bits=16;r.index=(address-0x7ffe00u)/2;}
 else if(address>=0xf80000u&&address<=0xf80010u&&address!=0xf80002u){
  r.region=LT48_PIC24_KA_CONFIGURATION;r.bits=16;r.index=(address-0xf80000u)/2;
  if(r.index)r.index--; /* F80002 is an unimplemented slot, not field1. */
 }else return 0;
 *out=r;return 1;
}
static inline int lt48_pic24_ka_image_to_target(const lt48_pic24_ka_image_policy *p,
 uint32_t offset,lt48_pic24_ka_location *out){
 if(!p||!out||offset%4||offset>=p->image_span)return 0;
 return lt48_pic24_ka_target_to_image(p,offset/2,out);
}
static inline int lt48_pic24_ka_dense_to_location(const lt48_pic24_ka_image_policy *p,
 uint32_t offset,lt48_pic24_ka_location *out){
 if(!p||!out||offset%4||offset>=p->dense_bytes)return 0;
 uint32_t address=offset<p->code_image_bytes?offset/2:0x7ffe00u+(offset-p->code_image_bytes)/2;
 return lt48_pic24_ka_target_to_image(p,address,out);
}
/* Only meaningful bits participate in comparisons. Read/program transport
 * must separately establish source padding and unimplemented upper-byte
 * behavior. No ignored byte is claimed to be a writable device cell. */
static inline int lt48_pic24_ka_word_decode(const lt48_pic24_ka_location *location,
 const unsigned char bytes[4],uint32_t *word){
 if(!location||!bytes||!word||(location->bits!=16&&location->bits!=24))return 0;
 uint32_t value=bytes[0]|((uint32_t)bytes[1]<<8);
 if(location->bits==24)value|=(uint32_t)bytes[2]<<16;
 *word=value;return 1;
}
#endif
