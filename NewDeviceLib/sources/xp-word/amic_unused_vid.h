/* Exact-source AMIC D93/D94 only. Their P1e 44-pin map omits logical 4.
 * Original zero-filled lookup sends UDN 22/01 to ZIF1, outside the selected
 * ZIF3..46 package; RESET sends 22/00. These analog writes are NOT no-ops.
 * The owner must verify the REZ digest, W27/W2c, P1e and VID/channel tuple.
 * No other analog default is admitted by this guard. */
#ifndef LT48_AMIC_UNUSED_VID_H
#define LT48_AMIC_UNUSED_VID_H
static int xp_amic_unused_vid_definition(unsigned d){return d==0x93 || d==0x94;}
static int xp_amic_unused_vid_defaults(const lt48_compiled_wave *w,unsigned op){
    static const size_t init_at[]={24,44,47,173,208},reset_at[]={9,30,45};
    size_t count=op==2?5:op==3?3:0;const size_t *at=op==2?init_at:reset_at;
    if(!count || lt48_compiled_wave_defaulted_pin_count(w)!=count)return 0;
    unsigned seen=0;
    for(size_t i=0;i<count;i++){
        lt48_defaulted_logical_pin p;
        if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK)return 0;
        size_t j=0;for(;j<count && p.compact_offset!=at[j];j++);
        if(j==count || (seen&(1u<<j)))return 0;
        unsigned digital=op==2?j<3:j==2;
        if(p.logical_pin!=(op==2 && j==2?7:4) ||
           p.compact_opcode!=(digital?0x11:op==2?3:4) ||
           p.classification!=(digital?LT48_DEFAULTED_PIN_RESIDENT_NOOP:LT48_DEFAULTED_PIN_UNRESOLVED))return 0;
        seen|=1u<<j;
    }
    return seen==((1u<<count)-1);
}
#endif
