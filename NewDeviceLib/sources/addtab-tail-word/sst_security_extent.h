#ifndef LT48_SST_SECURITY_EXTENT_H
#define LT48_SST_SECURITY_EXTENT_H
/* SST39VF160xC/320xC: Security ID survives chip erase (DS20005020C,
 * Security ID and Table 7). Source EndErase includes the appended ID bytes;
 * use actual erasable extent for verification and preserve the appended ID.
 * Caller has already checked the exact source resources and catalogue IDs. */
static uint32_t tail_sst_erase_end(unsigned tail,unsigned definition,
                                uint32_t main_size,uint32_t source_end) {
    if(tail && (((definition==0xf0 || definition==0xf1) && main_size==4194304u && source_end==4194560u) ||
                (definition==0xf2 && main_size==2097152u && source_end==2097408u)))
        return main_size;
    return source_end;
}
/* GLS36VF3203 table 7 gives x16 user words 100008..100087.
 * Df0 stores the x8 start 100010; generic byte translation would halve its
 * bank selector. Correct only the exact private profile and source window. */
static int tail_sst_region_address(unsigned tail,unsigned definition,
    uint32_t main_size,uint32_t start,uint32_t *physical) {
    if(!tail || definition!=0xf0)return 1;
    if(main_size!=4194304u)return 0;
    if(start<main_size)return 1;
    uint32_t offset=start-main_size;
    if(offset>=256u || *physical!=0x100010u+offset)return 0;
    *physical=0x200010u+offset;
    return 1;
}
#endif
