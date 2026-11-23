#ifndef LT48_PIC_SOURCE_APPLET_PROGRAM_SEQUENCE_H
#define LT48_PIC_SOURCE_APPLET_PROGRAM_SEQUENCE_H
#include <stdint.h>
/* Original 00466c10's public program sequencing, below auto-increment UI.
 * This is a phase scheduler, not transport, preservation or device admission.
 * Every phase callback receives the shared state and can mark abort/cancel.
 * START mode/reset/address and phase-specific ranges belong to the adapter.
 */
enum lt48_pic_applet_program_phase {
    LT48_PIC_APP_PROGRAM_MODE, LT48_PIC_APP_PROGRAM_STARTUP,
    LT48_PIC_APP_PROGRAM_ERASE, LT48_PIC_APP_PROGRAM_BLANK,
    LT48_PIC_APP_PROGRAM_ASK_ERASE, LT48_PIC_APP_PROGRAM_INIT,
    LT48_PIC_APP_PROGRAM_PROGRESS, LT48_PIC_APP_PROGRAM_STREAM,
    LT48_PIC_APP_PROGRAM_END_STREAM, LT48_PIC_APP_PROGRAM_VERIFY,
    LT48_PIC_APP_PROGRAM_PROTECT, LT48_PIC_APP_PROGRAM_FINISH,
    LT48_PIC_APP_PROGRAM_RESULT
};
typedef struct {
    uint32_t flags;
    unsigned has_download_program,has_download_erase,started;
    unsigned aborted,cancelled,blank;
    int operation_result;
} lt48_pic_applet_program_state;
typedef int (*lt48_pic_applet_program_call)(void *,enum lt48_pic_applet_program_phase,lt48_pic_applet_program_state *);
static inline int lt48_pic_applet_program_sequence(lt48_pic_applet_program_state *s,
    lt48_pic_applet_program_call call,void *context) {
    if(!s||!call||(s->flags&UINT32_C(0x40)))return -1;
    s->aborted=0;s->cancelled=0;s->blank=1;s->operation_result=0;
    if(!s->has_download_program)return 0;
    int ok=1;call(context,LT48_PIC_APP_PROGRAM_MODE,s);
    if(!s->started)ok=call(context,LT48_PIC_APP_PROGRAM_STARTUP,s);
    if(ok) {
        if(s->has_download_erase&&(s->flags&UINT32_C(0x40000))&&!s->aborted&&!s->cancelled)
            ok=call(context,LT48_PIC_APP_PROGRAM_ERASE,s);
        if(ok&&(s->flags&4)&&!s->aborted&&!s->cancelled)
            ok=call(context,LT48_PIC_APP_PROGRAM_BLANK,s);
        if(ok&&!s->blank&&!s->cancelled) {
            if(!(s->flags&UINT32_C(0x1000))&&!call(context,LT48_PIC_APP_PROGRAM_ASK_ERASE,s))s->aborted=1;
            else {
                s->blank=1;
                if(s->has_download_erase)ok=call(context,LT48_PIC_APP_PROGRAM_ERASE,s);
            }
        }
        if(ok) {
            if((s->flags&8)&&!s->aborted&&!s->cancelled) {
                if(!call(context,LT48_PIC_APP_PROGRAM_INIT,s)||
                   !call(context,LT48_PIC_APP_PROGRAM_PROGRESS,s)||
                   !call(context,LT48_PIC_APP_PROGRAM_STREAM,s)||
                   !call(context,LT48_PIC_APP_PROGRAM_END_STREAM,s)) {
                    ok=0;goto finish;
                }
                ok=1;
            }
            if((s->flags&16)&&!s->aborted&&!s->cancelled)
                ok=call(context,LT48_PIC_APP_PROGRAM_VERIFY,s);
            if(ok&&(s->flags&32)&&!s->aborted&&!s->cancelled)
                ok=call(context,LT48_PIC_APP_PROGRAM_PROTECT,s);
        }
    }
finish:
    s->operation_result=ok;call(context,LT48_PIC_APP_PROGRAM_FINISH,s);
    return call(context,LT48_PIC_APP_PROGRAM_RESULT,s);
}
#endif
