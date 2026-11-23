#ifndef LT48_PIC_REMAINING14_ATOMIC_PULSE_H
#define LT48_PIC_REMAINING14_ATOMIC_PULSE_H
#include "../../../c/src/internal.h"
/* One controller-write block reserves the complete Begin/Delay/End burst.
 * protocol.write_block flushes before a block that cannot fit; it never splits
 * this14-byte block across USB deliveries. The data latch is loaded first,
 * before Begin. This prevents a USB round trip during an external pulse. */
static inline lt48_status rm14_atomic_pulse(lt48_runtime_transport_session *wire,unsigned data,unsigned clock) {
 if(!wire||data>=192||clock>=192||data%4||clock%4||data==clock)return LT48_INVALID_ARGUMENT;
 const unsigned char bytes[]={0x58,(unsigned char)data,(unsigned char)clock,5,8,
  0x4e,101,0x58,(unsigned char)data,(unsigned char)clock,5,14,0x4e,1};
 return lt48_runtime_transport_session_controller_write(wire,bytes,sizeof bytes);
}
#endif
