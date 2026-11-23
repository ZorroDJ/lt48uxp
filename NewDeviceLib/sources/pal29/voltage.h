/* Authored correction: TI1986 TTL Vol4 p3-25 specifies20..22V, nominal21V,
 * security pins, VCC<=0.4V. Original source nominal14V is below that range.
 * Exact pinned GBB D24/M0e/Pa; retain original VCC-off/pins/35us pulses. */
#ifndef LT48_PAL29_VOLTAGE_H
#define LT48_PAL29_VOLTAGE_H
static unsigned pal29_security_voltage(unsigned d,unsigned m,unsigned p){
 return d==0x24&&m==0x0e&&p==0x0a?2100u:0u;
}
#endif
