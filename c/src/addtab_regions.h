#ifndef LT48_ADDTAB_REGIONS_H
#define LT48_ADDTAB_REGIONS_H
#include <stddef.h>
#include <stdint.h>

/* Byte units, matching original 0046a640/0046a720 after word conversion.
 * Table extents may exceed the exposed buffer; never extend a request into
 * that unexposed tail. Physical regions can overlap (main/engineer modes). */
struct lt48_addtab_region { uint32_t address, length; };
static inline int lt48_addtab_region_chunk(const struct lt48_addtab_region *regions,
    size_t count, uint32_t capacity, uint32_t start, uint32_t length,
    uint32_t *physical, uint32_t *chunk) {
    uint64_t offset=0;
    if(!regions || !count || !physical || !chunk || !length ||
       start>=capacity || length>capacity-start)return 0;
    for(size_t i=0;i<count;i++) {
        uint64_t end=offset+regions[i].length;
        if(!regions[i].length)return 0;
        if((uint64_t)start<end) {
            uint64_t delta=(uint64_t)start-offset;
            uint64_t address=(uint64_t)regions[i].address+delta;
            uint64_t available=end-start;
            uint32_t n=available<length?(uint32_t)available:length;
            if(address>UINT32_MAX || n-1>UINT32_MAX-address)return 0;
            *physical=(uint32_t)address;*chunk=n;return 1;
        }
        offset=end;
    }
    return 0;
}
#endif
