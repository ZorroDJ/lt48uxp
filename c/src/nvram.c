#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <labtool48/labtool48.h>
#include "internal.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <errno.h>
#endif
/* Called only after a STORE/status barrier, with socket power still applied.
 * Older X2004/X2444 parts specify 10 ms maximum. Do not cancel this hold. */
lt48_status lt48_nvram_store_hold_internal(void) {
#ifdef _WIN32
    Sleep(20);
#else
    struct timespec delay={0,20000000L};
    while(nanosleep(&delay,&delay)!=0)if(errno!=EINTR)return LT48_RUNTIME_ERROR;
#endif
    return LT48_OK;
}

/* Dallas DS1386/DS1486 tREC: at least 200 ms after VCC exceeds 4.5 V.
 * Called after the controller status barrier confirms rail commands arrived. */
lt48_status lt48_timekeeper_power_hold_internal(void) {
#ifdef _WIN32
    Sleep(220);
#else
    struct timespec delay={0,220000000L};
    while(nanosleep(&delay,&delay)!=0)if(errno!=EINTR)return LT48_RUNTIME_ERROR;
#endif
    return LT48_OK;
}
