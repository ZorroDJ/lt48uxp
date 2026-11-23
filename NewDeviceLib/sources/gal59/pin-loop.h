/* Authored implementation of original interpreter 004411DC (expanded 06).
 * Shared I/O-list loop index selects the pin; TTL offset is added as a byte.
 * This is software behavior, not a new physical pin capability claim. */
#ifndef LT48_GAL59_PIN_LOOP_H
#define LT48_GAL59_PIN_LOOP_H
static inline lt48_status gal59_loop_ttl(const lt48_pin_map *map,
    unsigned group_id,size_t index,unsigned offset,uint64_t *out) {
    lt48_pin_group group;
    lt48_status st=lt48_pin_map_find_group(map,(uint16_t)group_id,&group);
    if(st!=LT48_OK)return st;
    if(index>=group.pin_count || group.pins[index]>0xfbu || offset>255u)
        return LT48_RUNTIME_ERROR;
    *out=(group.pins[index]+offset)&255u;
    return LT48_OK;
}
#endif
