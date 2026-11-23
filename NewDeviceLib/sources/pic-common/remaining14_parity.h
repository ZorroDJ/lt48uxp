#ifndef LT48_PIC_REMAINING14_PARITY_H
#define LT48_PIC_REMAINING14_PARITY_H
#include <labtool48/labtool48.h>
/* DS30278B section2.2 / DS30457A: logical14-bit words travel as16bits.
 * Physical bit0 supplies even parity on alternating logical bits0,2,...12;
 * physical bit1 supplies odd parity on logical bits1,3,...13. The parity cells
 * are OTP cells too: a decoded14-bit monotonicity check is insufficient. */
static inline lt48_status rm14_parity_encode(unsigned logical,uint16_t *physical) {
 if(!physical||logical>0x3fff)return LT48_INVALID_ARGUMENT;
 unsigned even=0,odd=1;
 for(unsigned bit=0;bit<14;bit+=2){even^=(logical>>bit)&1u;odd^=(logical>>(bit+1))&1u;}
 *physical=(uint16_t)((logical<<2)|even|(odd<<1));return LT48_OK;
}
static inline unsigned rm14_parity_decode(uint16_t physical){return physical>>2;}
static inline int rm14_parity_valid(uint16_t physical) {
 uint16_t expected=0;return rm14_parity_encode(rm14_parity_decode(physical),&expected)==LT48_OK&&expected==physical;
}
static inline int rm14_parity_can_program(uint16_t physical,unsigned logical) {
 uint16_t target=0;return rm14_parity_encode(logical,&target)==LT48_OK&&!(target&~physical);
}
#endif
