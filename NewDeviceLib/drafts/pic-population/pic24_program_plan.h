#ifndef LT48_PIC24_PROGRAM_PLAN_H
#define LT48_PIC24_PROGRAM_PLAN_H
#include "pic24_image_policy.h"
#include <string.h>
/* Source FJ40 row-stream contract. It is not an admitted operator backend.
 * A truncated final row is not flushed by source command zero. The stream
 * must include complete rows, with the configuration slots erased in latches;
 * their real values remain in the image for the separate configuration step. */
typedef struct {
 uint32_t first_image_byte,image_byte_count,first_target_address,row_count;
 unsigned char source_header[9];
} lt48_pic24_program_plan;
static inline int lt48_pic24_program_plan_prepare(const lt48_pic24_image_policy *p,
 uint32_t first,uint32_t count,unsigned selector,unsigned bank,
 lt48_pic24_program_plan *out) {
 if(!p || !out || selector>255 || bank>255 || !count || first%256 || count%256 ||
    first>=p->configuration_first_image_byte || first>=p->source_image_bytes ||
    count>p->source_image_bytes-first)return 0;
 uint32_t target;
 if(!lt48_pic24_image_to_target(p,first,&target))return 0;
 uint32_t cfg=p->configuration_first_target_address;
 lt48_pic24_program_plan plan={first,count,target,count/256,
  {5,(unsigned char)selector,(unsigned char)(target>>16),(unsigned char)(target>>8),
   (unsigned char)target,64,(unsigned char)(cfg>>16),(unsigned char)(cfg>>8),(unsigned char)bank}};
 *out=plan;return 1;
}
static inline int lt48_pic24_program_row(const lt48_pic24_image_policy *p,
 const unsigned char *image,size_t size,uint32_t offset,unsigned char row[256]) {
 if(!p || !image || !row || size!=p->source_image_bytes || offset%256 ||
    offset>=p->source_image_bytes || p->source_image_bytes-offset<256)return 0;
 /* Stage before publishing, including when output aliases input. */
 unsigned char staged[256];
 for(unsigned i=0;i<64;i++){
  uint32_t cursor=offset+4*i,word;
  if(cursor>=p->configuration_first_image_byte)word=0xffffff;
  else if(!lt48_pic24_image_word(p,image,size,cursor,&word))return 0;
  staged[4*i]=(unsigned char)word;staged[4*i+1]=(unsigned char)(word>>8);
  staged[4*i+2]=(unsigned char)(word>>16);staged[4*i+3]=0;
 }
 memcpy(row,staged,sizeof(staged));return 1;
}
#endif
