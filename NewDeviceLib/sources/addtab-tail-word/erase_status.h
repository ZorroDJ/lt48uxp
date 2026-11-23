#ifndef LT48_TAIL_ERASE_STATUS_H
#define LT48_TAIL_ERASE_STATUS_H
/* Exact source-bound MX26L6420 procedure returns captured DQ7, not bool 1.
 * Caller has already verified closure/resource hashes and device identity. */
static unsigned tail_erase_success(unsigned tail,unsigned definition,unsigned fallback) {
    return tail && definition==0xe9?0x80u:fallback;
}
#endif
