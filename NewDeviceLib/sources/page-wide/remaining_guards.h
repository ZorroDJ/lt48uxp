/* Authored, exact-source Page-wide compatibility rules. These are used only
 * after the owning engine verifies the GDBAA__.rez SHA-256 and closure tuple.
 * No imported firmware, FPGA or compact procedure is embedded here. */
#ifndef LT48_PAGE_WIDE_REMAINING_GUARDS_H
#define LT48_PAGE_WIDE_REMAINING_GUARDS_H
static int page_sharp_release_definition(unsigned d) {
    return d==0x57 || d==0x58 || d==0x5b || d==0x5c || d==0x5d || d==0x5e;
}
static int page_sharp_release_defaults(const lt48_compiled_wave *w) {
    static const size_t offsets[]={0x0c,0x28,0x44,0x63,0x7f};unsigned mask=0;
    for(size_t i=0;i<lt48_compiled_wave_defaulted_pin_count(w);i++) {
        lt48_defaulted_logical_pin p;
        if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK)return 0;
        if(p.classification==LT48_DEFAULTED_PIN_RESIDENT_NOOP)continue;
        if(p.logical_pin!=8 || p.compact_opcode!=4)return 0;
        unsigned j=0;for(;j<5 && p.compact_offset!=offsets[j];j++);
        if(j==5 || (mask&(1u<<j)))return 0;
        mask|=1u<<j;
    }
    return mask==31;
}
/* W6d masks DataIn with 2 before a DWORD comparison. Mask all four bytes:
 * one-byte capture and the original one-byte AND leave stale upper bytes. */
static int page_llyd_mask_correction(unsigned char *code,size_t size) {
    static const size_t offsets[]={0x2a,0x147};
    static const unsigned char expected[]={0x4b,1,2,2,0,0,0,1};
    for(unsigned i=0;i<2;i++)if(offsets[i]+sizeof(expected)>size ||
        memcmp(code+offsets[i],expected,sizeof(expected)))return 0;
    for(unsigned i=0;i<2;i++)code[offsets[i]+7]=4;
    return 1;
}
#endif
