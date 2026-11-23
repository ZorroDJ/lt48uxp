/* Explicit byte/bit preservation for generated PIC protocol policies.
 * These helpers do not infer factory ranges or read hardware. The owner must
 * capture a qualified chip read and check a later independent chip read.
 * Profile masks are immutable; snapshot/image storage must be disjoint. */
#ifndef LT48_PIC_PRESERVATION_H
#define LT48_PIC_PRESERVATION_H
#include <labtool48/labtool48.h>
#include <stdint.h>
#include <string.h>
typedef struct {uint32_t address,count;const unsigned char *masks;} lt48_pic_preserve_span;
typedef struct {const lt48_pic_preserve_span *spans;size_t count;} lt48_pic_preserve_policy;
typedef struct {uint32_t first_address;size_t mismatches;} lt48_pic_preserve_result;
static inline lt48_status lt48_pic_preserve_size(const lt48_pic_preserve_policy *p,size_t image_size,size_t *out) {
 if(!p||!out||image_size>UINT32_MAX||(p->count&&!p->spans))return LT48_INVALID_ARGUMENT;
 size_t total=0;
 for(size_t i=0;i<p->count;i++) {
  const lt48_pic_preserve_span *s=p->spans+i;
  if(!s->count||s->address>image_size||s->count>image_size-s->address||s->count>SIZE_MAX-total)return LT48_SAFETY_REJECTED;
  for(size_t j=0;j<i;j++) {
   const lt48_pic_preserve_span *t=p->spans+j;
   if((uint64_t)s->address+s->count>t->address&&(uint64_t)t->address+t->count>s->address)return LT48_SAFETY_REJECTED;
  }
  total+=s->count;
 }
 *out=total;return LT48_OK;
}
static inline lt48_status lt48_pic_preserve_preflight(const lt48_pic_preserve_policy *p,
 const unsigned char *image,size_t image_size,const unsigned char *snapshot,size_t snapshot_size) {
 size_t required=0;lt48_status st=lt48_pic_preserve_size(p,image_size,&required);
 if(st!=LT48_OK)return st;
 if(!image||snapshot_size!=required||(required&&!snapshot))return LT48_INVALID_ARGUMENT;
 uintptr_t a=(uintptr_t)image,b=(uintptr_t)snapshot;
 if(image_size>UINTPTR_MAX-a||snapshot_size>UINTPTR_MAX-b)return LT48_SAFETY_REJECTED;
 if(required&&a+image_size>b&&b+snapshot_size>a)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
static inline lt48_status lt48_pic_preserve_capture(const lt48_pic_preserve_policy *p,
 const unsigned char *image,size_t image_size,unsigned char *snapshot,size_t snapshot_size) {
 lt48_status st=lt48_pic_preserve_preflight(p,image,image_size,snapshot,snapshot_size);
 if(st!=LT48_OK)return st;
 size_t offset=0;
 for(size_t i=0;i<p->count;i++){memcpy(snapshot+offset,image+p->spans[i].address,p->spans[i].count);offset+=p->spans[i].count;}
 return LT48_OK;
}
static inline lt48_status lt48_pic_preserve_apply(const lt48_pic_preserve_policy *p,
 unsigned char *image,size_t image_size,const unsigned char *snapshot,size_t snapshot_size) {
 lt48_status st=lt48_pic_preserve_preflight(p,image,image_size,snapshot,snapshot_size);
 if(st!=LT48_OK)return st;
 size_t offset=0;
 for(size_t i=0;i<p->count;i++) {
  const lt48_pic_preserve_span *s=p->spans+i;
  for(uint32_t j=0;j<s->count;j++){unsigned mask=s->masks?s->masks[j]:255;image[s->address+j]=(unsigned char)((image[s->address+j]&~mask)|(snapshot[offset+j]&mask));}
  offset+=s->count;
 }
 return LT48_OK;
}
static inline lt48_status lt48_pic_preserve_check(const lt48_pic_preserve_policy *p,
 const unsigned char *image,size_t image_size,const unsigned char *snapshot,size_t snapshot_size,lt48_pic_preserve_result *result) {
 if(!result)return LT48_INVALID_ARGUMENT;
 *result=(lt48_pic_preserve_result){UINT32_MAX,0};
 lt48_status st=lt48_pic_preserve_preflight(p,image,image_size,snapshot,snapshot_size);
 if(st!=LT48_OK)return st;
 size_t offset=0;
 for(size_t i=0;i<p->count;i++) {
  const lt48_pic_preserve_span *s=p->spans+i;
  for(uint32_t j=0;j<s->count;j++)if((image[s->address+j]^snapshot[offset+j])&(s->masks?s->masks[j]:255)) {
   if(s->address+j<result->first_address)result->first_address=s->address+j;
   result->mismatches++;
  }
  offset+=s->count;
 }
 return result->mismatches?LT48_DEVICE_MISMATCH:LT48_OK;
}
#endif
