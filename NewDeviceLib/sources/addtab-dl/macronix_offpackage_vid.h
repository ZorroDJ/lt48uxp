/* Exact GDCAA Dataman 13.10 SO44 defaults; enclosing source digest required.
 * VID is routed to ZIF1, outside the selected ZIF3..46 package. */
#ifndef LT48_ADDTAB_DL_MACRONIX_VID_H
#define LT48_ADDTAB_DL_MACRONIX_VID_H
static int addtab_dl_vid_context(unsigned d,unsigned op) {
    return (op==2 || op==3) && (d==0x40 || d==0x41 || d==0x103 || d==0x104);
}
static int addtab_dl_vid_defaults(const lt48_compiled_wave *w,unsigned d,unsigned op) {
    if(!addtab_dl_vid_context(d,op))return 0;
    const size_t total=op==3?3:(d==0x40 || d==0x41)?7:8;
    if(lt48_compiled_wave_defaulted_pin_count(w)!=total)return 0;
    size_t unresolved=0;
    for(size_t i=0;i<total;i++) {
        lt48_defaulted_logical_pin p;
        if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK)return 0;
        if(p.classification==LT48_DEFAULTED_PIN_RESIDENT_NOOP) {
            if(p.compact_opcode!=17 || (p.logical_pin!=5 && p.logical_pin!=8 &&
                !(op==2 && d>=0x103 && p.logical_pin==1)))return 0;
            continue;
        }
        static const unsigned init_offsets[]={73,76,98,101};
        static const unsigned init_ops[]={11,40,11,40};
        static const unsigned reset_offsets[]={9,21};
        if(p.logical_pin!=5 || p.classification!=LT48_DEFAULTED_PIN_UNRESOLVED)return 0;
        if(op==2) {
            if(unresolved>=4 || p.compact_offset!=init_offsets[unresolved] || p.compact_opcode!=init_ops[unresolved])return 0;
        } else if(unresolved>=2 || p.compact_offset!=reset_offsets[unresolved] || p.compact_opcode!=4)return 0;
        unresolved++;
    }
    return unresolved==(op==2?4u:2u);
}
#endif
