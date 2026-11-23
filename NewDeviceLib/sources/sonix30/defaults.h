/* Exact source SHA, membership and zero-lookup sites; no generic fallback. */
static const struct{unsigned definition,op,offset,opcode,logical,classification;}so30_default_sites[]={
#include "../fm8one/defaults.inc"
#include "../soB3/defaults.inc"
{10,2,5,17,9,LT48_DEFAULTED_PIN_RESIDENT_NOOP},
{10,2,37,17,9,LT48_DEFAULTED_PIN_RESIDENT_NOOP},
{10,3,24,17,9,LT48_DEFAULTED_PIN_RESIDENT_NOOP},
{11,2,5,17,9,LT48_DEFAULTED_PIN_RESIDENT_NOOP},
{11,2,37,17,9,LT48_DEFAULTED_PIN_RESIDENT_NOOP},
{11,3,24,17,9,LT48_DEFAULTED_PIN_RESIDENT_NOOP},
};
static int so30_defaults(unsigned def,unsigned op,const lt48_compiled_wave*w){unsigned n=0;for(size_t i=0;i<sizeof so30_default_sites/sizeof*so30_default_sites;i++){if(so30_default_sites[i].definition!=def||so30_default_sites[i].op!=op)continue;lt48_defaulted_logical_pin p;if(lt48_compiled_wave_get_defaulted_pin(w,n++,&p)||p.logical_pin!=so30_default_sites[i].logical||p.compact_offset!=so30_default_sites[i].offset||p.compact_opcode!=so30_default_sites[i].opcode||p.classification!=so30_default_sites[i].classification)return 0;}return n==lt48_compiled_wave_defaulted_pin_count(w); }
