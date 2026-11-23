/* Authored context for one original zero-filled lookup / resident no-op.
 * Source GBB digest is enforced by the PAL resource owner. */
#ifndef LT48_EP35_RESIDENT_NOOP_H
#define LT48_EP35_RESIDENT_NOOP_H
static inline int ep35_resident_noop(unsigned d,unsigned m,unsigned p,unsigned op,unsigned wave){
 return d==0x39u&&m==33u&&p==24u&&op==2u&&wave==0x7bu;
}
#endif
