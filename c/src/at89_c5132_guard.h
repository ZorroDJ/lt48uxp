#ifndef LT48_AT89_C5132_GUARD_H
#define LT48_AT89_C5132_GUARD_H
#include <labtool48/labtool48.h>
/* Experimental primitive. Flush the runtime queue before calling. Requires
 * the guarded 4100/4200 image, resident dispatch active, and initialized latch.
 * Does not clear sticky failure. No capture is valid until this succeeds. */
lt48_status lt48_c5132_guard_check(const lt48_protocol_transport *,void *);
#endif
