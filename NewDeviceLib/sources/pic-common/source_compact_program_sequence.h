#ifndef LT48_PIC_SOURCE_COMPACT_PROGRAM_SEQUENCE_H
#define LT48_PIC_SOURCE_COMPACT_PROGRAM_SEQUENCE_H
#include <stdint.h>

/* Public compact host 004554b0, after the separate Windows license gate.
 * Source-selected callbacks own the stage bodies. Auto-increment is a host
 * editing feature and is explicitly outside this hardware operation service.
 * stopped is the original object's +98 field; no silicon meaning is inferred.
 * Cleanup errors and protection errors remain failures in the authored host,
 * including where the original wrapper discards them. */
enum lt48_pic_compact_program_phase {
    LT48_PIC_COMPACT_PROGRAM_BLANK,
    LT48_PIC_COMPACT_PROGRAM_FINISH,
    LT48_PIC_COMPACT_PROGRAM_ASK_ERASE,
    LT48_PIC_COMPACT_PROGRAM_ERASE,
    LT48_PIC_COMPACT_PROGRAM_WRITE,
    LT48_PIC_COMPACT_PROGRAM_VERIFY,
    LT48_PIC_COMPACT_PROGRAM_PROTECT
};
typedef struct {
    uint32_t flags;
    unsigned has_erase,stopped;
} lt48_pic_compact_program_state;
typedef int (*lt48_pic_compact_program_call)(void *,
    enum lt48_pic_compact_program_phase,lt48_pic_compact_program_state *);
static inline int lt48_pic_compact_program_sequence(
    lt48_pic_compact_program_state *s,lt48_pic_compact_program_call call,void *context) {
    if(!s||!call||(s->flags&UINT32_C(0x40)))return -1;
    int ok=1,cleanup_ok=1;
    if(s->flags&4) {
        ok=call(context,LT48_PIC_COMPACT_PROGRAM_BLANK,s);
        if(!ok) {
            if(s->stopped)goto finish;
            if(!(s->flags&UINT32_C(0x1000))) {
                cleanup_ok=call(context,LT48_PIC_COMPACT_PROGRAM_FINISH,s);
                if(!cleanup_ok)goto finish;
                if(!call(context,LT48_PIC_COMPACT_PROGRAM_ASK_ERASE,s))goto finish;
            }
        }
    }
    if(s->flags&8) {
        if(((!(s->flags&4)&&(s->flags&UINT32_C(0x1000)))||!s->stopped)&&s->has_erase) {
            ok=call(context,LT48_PIC_COMPACT_PROGRAM_ERASE,s);
            if(!ok)goto finish;
        }
        ok=call(context,LT48_PIC_COMPACT_PROGRAM_WRITE,s);
        if(!ok)goto finish;
    }
    if(s->flags&16) {
        ok=call(context,LT48_PIC_COMPACT_PROGRAM_VERIFY,s);
        if(!ok)goto finish;
    }
    if(s->flags&32)ok=call(context,LT48_PIC_COMPACT_PROGRAM_PROTECT,s);
finish:
    if(!call(context,LT48_PIC_COMPACT_PROGRAM_FINISH,s))cleanup_ok=0;
    return ok&&cleanup_ok;
}
#endif
