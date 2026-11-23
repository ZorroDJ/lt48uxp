/* Authored guard for exact-source EFST D23/D24 W19. The unmapped voltage
 * routes occur only in source modes 13/14; these definitions have neither
 * Protect nor Unprotect. Legal modes retain only proved resident no-op defaults.
 * Owning code verifies the REZ digest, definition tuple, W19 and variable shape. */
#ifndef LT48_EFST_DEAD_PROTECTION_H
#define LT48_EFST_DEAD_PROTECTION_H
static int xp_efst_dead_definition(unsigned d) {return d==0x23 || d==0x24;}
static int xp_efst_mode_allowed(unsigned mode) {
    return mode==0 || mode==1 || mode==2 || mode==4 || mode==5 || mode==7 || mode==11;
}
static int xp_efst_dead_defaults(const lt48_compiled_wave *w) {
    static const size_t offsets[]={17,39,163,176};unsigned mask=0;
    if(lt48_compiled_wave_defaulted_pin_count(w)!=4)return 0;
    for(size_t i=0;i<4;i++) {
        lt48_defaulted_logical_pin p;
        if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK || p.logical_pin!=4)return 0;
        unsigned j=0;for(;j<4 && p.compact_offset!=offsets[j];j++);
        if(j==4 || (mask&(1u<<j)))return 0;
        if(p.compact_opcode!=(j<2?0x11:3) || p.classification!=(j<2?LT48_DEFAULTED_PIN_RESIDENT_NOOP:LT48_DEFAULTED_PIN_UNRESOLVED))return 0;
        mask|=1u<<j;
    }
    return mask==15;
}
#endif
