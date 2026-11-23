#ifndef LT48_AT89_RE2_GUARD_H
#define LT48_AT89_RE2_GUARD_H
#include <labtool48/labtool48.h>
/* Experimental RE2 owner primitive. Caller must flush the runtime queue and
 * have a guarded GETID/READ applet selected. Clear only before a new logical
 * transaction; query after every capture before accepting its bytes. */
lt48_status lt48_re2_guard_check(const lt48_protocol_transport *,void *,int);
#endif
