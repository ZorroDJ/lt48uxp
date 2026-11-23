/* Authored M59PW1282 partial-read initialization correction.
 * A22/VPP is an address input during read. Source region mapping repeats
 * physical zero for each 8 MiB bank, but READ/VERIFY/BLANK setup does not
 * restore that bank bit. Seed DEVINIT with the logical byte address instead.
 * Program setup explicitly latches A22 before raising VPP and is unchanged.
 * Containing engine binds this profile to its original source digest.
 */
#ifndef LT48_R73_READ_START_H
#define LT48_R73_READ_START_H
#include <stdint.h>
static uint32_t remaining73_initial_address(unsigned tail,unsigned definition,
 unsigned mode,uint32_t logical,uint32_t physical) {
 return tail==4 && definition==0xef && (mode==1 || mode==4 || mode==5 || mode==11)
  ? logical : physical;
}
#endif
