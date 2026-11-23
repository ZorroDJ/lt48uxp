#ifndef LT48_PIC24_IMAGE_POLICY_H
#define LT48_PIC24_IMAGE_POLICY_H
#include <stdint.h>
#include <stddef.h>
/* PIC24FJ GA0/GA1/GB0/GA2/GB2 candidate shared image contract.
 * Manufacturer memory addresses advance by two per 24-bit instruction.
 * The original source allocation uses four image bytes per instruction.
 * The fourth byte has no instruction bits; callers retain their chosen pad.
 * This draft is not an admitted device profile or an ICSP implementation. */
typedef struct {
 uint32_t source_image_bytes,instructions,last_target_address;
 uint32_t configuration_first_image_byte,configuration_first_target_address;
 uint16_t configuration_words,row_instructions,erase_page_instructions;
} lt48_pic24_image_policy;
static inline int lt48_pic24_image_policy_prepare(uint32_t source_image_bytes,
 uint32_t manufacturer_last_address,uint16_t configuration_words,
 lt48_pic24_image_policy *out) {
 if(!out || !source_image_bytes || source_image_bytes%4 ||
    manufacturer_last_address%2 || manufacturer_last_address>0xfffffe ||
    (configuration_words!=2 && configuration_words!=4))return 0;
 uint32_t instructions=manufacturer_last_address/2+1;
 if(instructions<=configuration_words || instructions%512 ||
    (uint64_t)instructions*4!=source_image_bytes)return 0;
 uint32_t first=instructions-configuration_words;
 *out=(lt48_pic24_image_policy){source_image_bytes,instructions,manufacturer_last_address,
     first*4,first*2,configuration_words,64,512};return 1;
}
static inline int lt48_pic24_target_to_image(const lt48_pic24_image_policy *policy,
 uint32_t address,uint32_t *offset) {
 if(!policy || !offset || address%2 || address>policy->last_target_address)return 0;
 *offset=(address/2)*4;return 1;
}
static inline int lt48_pic24_image_to_target(const lt48_pic24_image_policy *policy,
 uint32_t offset,uint32_t *address) {
 if(!policy || !address || offset%4 || offset>=policy->source_image_bytes)return 0;
 *address=(offset/4)*2;return 1;
}
static inline int lt48_pic24_image_word(const lt48_pic24_image_policy *policy,
 const unsigned char *image,size_t size,uint32_t offset,uint32_t *word) {
 uint32_t address;
 if(!policy || !image || !word || size!=policy->source_image_bytes ||
    !lt48_pic24_image_to_target(policy,offset,&address))return 0;
 *word=(uint32_t)image[offset]|((uint32_t)image[offset+1]<<8)|((uint32_t)image[offset+2]<<16);
 return 1;
}
static inline int lt48_pic24_image_store(const lt48_pic24_image_policy *policy,
 unsigned char *image,size_t size,uint32_t offset,uint32_t word,unsigned char padding) {
 uint32_t address;
 if(!policy || !image || size!=policy->source_image_bytes || word>0xffffff ||
    !lt48_pic24_image_to_target(policy,offset,&address))return 0;
 image[offset]=(unsigned char)word;image[offset+1]=(unsigned char)(word>>8);
 image[offset+2]=(unsigned char)(word>>16);image[offset+3]=padding;return 1;
}
#endif
