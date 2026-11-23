#ifndef LT48_PIC_COMMON_CLEANUP_H
#define LT48_PIC_COMMON_CLEANUP_H
#include <labtool48/labtool48.h>

typedef struct {
    void *context;
    /* Both callbacks must ignore cancellation and use fresh bounded transport
     * state after an earlier error. Source reset/off are still attempted. */
    lt48_status (*source)(void *, unsigned role);
    lt48_status (*command)(void *, const unsigned char *, size_t);
} lt48_pic_cleanup_io;
typedef struct { lt48_status status; unsigned attempted; int completed; } lt48_pic_cleanup_result;

/* Source DEVRESET/POWEROFF are not sufficient to clear all analog selections.
 * The resident 49 AA 55 path clears DACs, serial banks, UDN/TPIC latches and
 * digital enables. Every stage is attempted after failure; success is never
 * reported if any cleanup stage failed. Preserve the original operation error.
 * This helper does not imply analog discharge or electrical isolation timing. */
static inline lt48_status lt48_pic_cleanup(const lt48_pic_cleanup_io *io,
    lt48_status operation, lt48_pic_cleanup_result *result) {
    if (!io || !io->source || !io->command || !result) return LT48_INVALID_ARGUMENT;
    result->status=LT48_OK; result->attempted=0; result->completed=0;
    for (unsigned phase=0; phase<8; phase++) {
        lt48_status st;
        if (phase<2) st=io->source(io->context,phase==0?4:1);
        else if (phase<6) {
            const unsigned char off[]={0x10,(unsigned char)(5-phase),0};
            st=io->command(io->context,off,sizeof off);
        } else if (phase==6) {
            const unsigned char clear[]={0x49,0xaa,0x55};
            st=io->command(io->context,clear,sizeof clear);
        } else {
            const unsigned char led[]={0x40,operation==LT48_OK && result->status==LT48_OK?4:2};
            st=io->command(io->context,led,sizeof led);
        }
        result->attempted++;
        if (result->status==LT48_OK) result->status=st;
    }
    result->completed=result->status==LT48_OK;
    return operation==LT48_OK?result->status:operation;
}
#endif
