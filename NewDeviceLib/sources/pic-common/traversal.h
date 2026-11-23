#ifndef LT48_PIC_COMMON_TRAVERSAL_H
#define LT48_PIC_COMMON_TRAVERSAL_H
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t first,last,total_bytes,chunk_bytes;
    uint16_t element_bytes;
} lt48_pic_range_plan;
typedef struct { lt48_pic_range_plan plan;uint32_t next,remaining; } lt48_pic_range_iterator;
typedef struct { uint32_t address,count,byte_offset,byte_count; } lt48_pic_range_span;

/* Original compact host helper 0044e430: inclusive range, normalized order,
 * one-percent chunks capped at 8192 bytes. Current compact PIC profiles use
 * two-byte elements. Power-of-two widths keep the capped chunk aligned.
 * Signed-overflow and unsupported-width inputs are rejected, not emulated. */
static inline int lt48_pic_range_prepare(uint32_t first,uint32_t last,unsigned width,
    lt48_pic_range_plan *out) {
    if(!out || !width || width>32 || (width&(width-1)) || first>INT32_MAX || last>INT32_MAX)return 0;
    if(first>last){uint32_t swap=first;first=last;last=swap;}
    uint64_t total=((uint64_t)last-first+1)*width;
    if(total>INT32_MAX)return 0;
    uint32_t chunk=(uint32_t)total/100;
    if(chunk>8192)chunk=8192;
    else {chunk-=chunk%width;if(!chunk)chunk=width;}
    lt48_pic_range_plan plan={first,last,(uint32_t)total,chunk,(uint16_t)width};
    *out=plan;return 1;
}
static inline int lt48_pic_range_begin(uint32_t first,uint32_t last,unsigned width,
    lt48_pic_range_iterator *out) {
    lt48_pic_range_plan plan;
    if(!out || !lt48_pic_range_prepare(first,last,width,&plan))return 0;
    lt48_pic_range_iterator iterator={plan,plan.first,plan.last-plan.first+1};
    *out=iterator;return 1;
}
static inline int lt48_pic_range_next(lt48_pic_range_iterator *iterator,lt48_pic_range_span *out) {
    if(!iterator || !out || !iterator->remaining || !iterator->plan.element_bytes)return 0;
    uint32_t count=iterator->plan.chunk_bytes/iterator->plan.element_bytes;
    if(count>iterator->remaining)count=iterator->remaining;
    if(!count)return 0;
    lt48_pic_range_span span={iterator->next,count,
        (iterator->next-iterator->plan.first)*iterator->plan.element_bytes,
        count*iterator->plan.element_bytes};
    iterator->next+=count;iterator->remaining-=count;*out=span;return 1;
}
#endif
