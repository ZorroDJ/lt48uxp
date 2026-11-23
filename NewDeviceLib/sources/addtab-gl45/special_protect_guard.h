/* Authored guard for exact Dataman 13.10 GDCAA Dd6 PROTECT defaults.
 * The enclosing REZ digest is pinned by the loader. These references occur
 * only at Address == SecSiDev after DEVINIT. Native protection visits only
 * main-array sector starts strictly below main_size; special locking excluded.
 */
#ifndef LT48_GL45_SPECIAL_PROTECT_GUARD_H
#define LT48_GL45_SPECIAL_PROTECT_GUARD_H
static int addtab_gl45_protect_context(unsigned d,unsigned op) {
    return d==0xd6 && op==11;
}
static int addtab_gl45_protect_defaults(const lt48_compiled_wave *w,unsigned d,unsigned op) {
    if(!addtab_gl45_protect_context(d,op)||lt48_compiled_wave_defaulted_pin_count(w)!=2)return 0;
    static const unsigned offsets[]={18,32},ops[]={4,3};
    for(size_t i=0;i<2;i++) {
        lt48_defaulted_logical_pin p;
        if(lt48_compiled_wave_get_defaulted_pin(w,i,&p)!=LT48_OK ||
           p.logical_pin!=0 || p.classification!=LT48_DEFAULTED_PIN_UNRESOLVED ||
           p.compact_offset!=offsets[i] || p.compact_opcode!=ops[i])return 0;
    }
    return 1;
}
#endif
