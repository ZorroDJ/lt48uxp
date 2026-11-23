#ifndef LT48_PIC_SOURCE_GEOMETRY_H
#define LT48_PIC_SOURCE_GEOMETRY_H
#include <stdint.h>
#include <limits.h>

typedef struct {
    uint32_t element_count, element_bytes, traversal_bytes, backing_bytes;
    uint32_t first_element, last_element, last_file_byte, checksum_last_byte;
} lt48_pic_source_geometry;

/* Compact host 0044e138..0044e21d. The serialized auxiliary dword is
 * the image allocation extent, not a second element count. Traversal and
 * callback backing are distinct: legacy configuration can lie beyond code.
 * Restrict the service to representable, nonempty source geometry. */
static inline int lt48_pic_source_geometry_prepare(uint32_t count,uint32_t bits,
    uint32_t backing,lt48_pic_source_geometry *out) {
    if(!out || !count || !bits || bits>32 || !backing || backing>INT32_MAX)return 0;
    uint32_t width=(bits+7)/8;
    uint64_t dense=(uint64_t)count*width;
    if(dense<8 || dense>INT32_MAX || backing<dense)return 0;
    lt48_pic_source_geometry g={count,width,(uint32_t)dense,backing,
        0,count-1,(uint32_t)dense-1,(uint32_t)dense-8};
    *out=g;return 1;
}
#endif
