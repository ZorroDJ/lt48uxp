#ifndef LT48_PIC_COMMON_ADDTAB_MAP_H
#define LT48_PIC_COMMON_ADDTAB_MAP_H
#include <stddef.h>
#include <stdint.h>

/* Source table addresses, canonical image offsets and dense host positions
 * have distinct meanings. Increment is a separate host callback parameter. */
typedef struct {
    uint32_t address, image_offset, length, increment;
} lt48_pic_map_segment;
typedef struct {
    const lt48_pic_map_segment *segments;
    size_t count;
    uint32_t dense_capacity;
    uint16_t element_bytes;
} lt48_pic_addtab_map;
typedef struct {
    uint32_t dense_offset, image_offset, device_address, length, increment;
    size_t segment;
} lt48_pic_map_span;

static inline int lt48_pic_addtab_map_valid(const lt48_pic_addtab_map *map) {
    if(!map || !map->segments || !map->count || !map->dense_capacity || !map->element_bytes)return 0;
    uint64_t total=0;
    for(size_t i=0;i<map->count;i++) {
        const lt48_pic_map_segment *s=&map->segments[i];
        if(!s->length || !s->increment || s->address%map->element_bytes ||
            (uint64_t)s->address+s->length>UINT64_C(0x100000000) ||
            (uint64_t)s->image_offset+s->length>UINT64_C(0x100000000))return 0;
        total+=s->length;
        if(total>UINT32_MAX)return 0;
    }
    return map->dense_capacity<=total;
}
static inline int lt48_pic_addtab_map_next(const lt48_pic_addtab_map *map,
    uint32_t dense_offset,uint32_t remaining,lt48_pic_map_span *out) {
    if(!out || !lt48_pic_addtab_map_valid(map) || !remaining ||
        dense_offset>=map->dense_capacity || remaining>map->dense_capacity-dense_offset ||
        dense_offset%map->element_bytes || remaining%map->element_bytes)return 0;
    uint32_t prefix=0;
    for(size_t i=0;i<map->count;i++) {
        const lt48_pic_map_segment *s=&map->segments[i];
        if(dense_offset-prefix<s->length) {
            uint32_t within=dense_offset-prefix,n=s->length-within;
            if(n>remaining)n=remaining;
            lt48_pic_map_span result={dense_offset,s->image_offset+within,
                (s->address+within)/map->element_bytes,n,s->increment,i};
            *out=result;return 1;
        }
        prefix+=s->length;
    }
    return 0;
}
#endif
