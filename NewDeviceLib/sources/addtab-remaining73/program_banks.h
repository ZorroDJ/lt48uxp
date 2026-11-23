/* Authored programming boundary for D80's two-CE flash.
 * Original region table describes one continuous 16 MiB array. The recovered
 * FPGA changes die at 8 MiB, but the original programming applet enters
 * unlock-bypass only on its selected die. Re-run the source setup per bank.
 * Resource/source digest binding is enforced by the containing C engine.
 */
#ifndef LT48_R73_PROGRAM_BANKS_H
#define LT48_R73_PROGRAM_BANKS_H
#include <stdint.h>
static uint32_t remaining73_program_chunk(unsigned tail,unsigned definition,
 unsigned mode,uint32_t logical,uint32_t length) {
 const uint32_t boundary=0x800000u;
 if(tail && definition==0x80 && mode==2 && logical<boundary && length>boundary-logical)
  return boundary-logical;
 return length;
}
#endif
