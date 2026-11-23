#ifndef LT48_PIC_SOURCE_APPLET_FINISH_SEQUENCE_H
#define LT48_PIC_SOURCE_APPLET_FINISH_SEQUENCE_H
#include <stdint.h>
/* Original 00468860 and 004254c0. SETTLE implements 00442980 and may
 * update status. OFF is the selected POWEROFF role, not DEVRESET. FPGA
 * may reset status even after an OFF error. RELEASE is 49 AA 55; progress
 * closure and error reporting are host/UI services, not device callbacks.
 * Independent authored rail shutdown is outside this original sequence. */
enum lt48_pic_applet_finish_phase {
    LT48_PIC_APP_FINISH_SETTLE, LT48_PIC_APP_FINISH_OFF,
    LT48_PIC_APP_FINISH_FPGA, LT48_PIC_APP_FINISH_RELEASE,
    LT48_PIC_APP_FINISH_PROGRESS, LT48_PIC_APP_FINISH_REPORT
};
typedef struct {
    uint16_t status, severity, saved_status, saved_severity;
    unsigned has_fpga, aborted, started;
} lt48_pic_applet_finish_state;
typedef void (*lt48_pic_applet_finish_call)(void *,enum lt48_pic_applet_finish_phase,
    lt48_pic_applet_finish_state *);
static inline void lt48_pic_applet_finish_sequence(lt48_pic_applet_finish_state *s,
    lt48_pic_applet_finish_call call,void *context) {
    call(context,LT48_PIC_APP_FINISH_SETTLE,s);
    s->saved_status=s->status;s->saved_severity=s->severity;
    if(s->status!=1) {
        call(context,LT48_PIC_APP_FINISH_OFF,s);
        if(s->has_fpga)call(context,LT48_PIC_APP_FINISH_FPGA,s);
    }
    if(s->saved_status==2)s->aborted=1;
    if(s->status!=1)call(context,LT48_PIC_APP_FINISH_RELEASE,s);
    s->started=0;
    call(context,LT48_PIC_APP_FINISH_PROGRESS,s);
    call(context,LT48_PIC_APP_FINISH_REPORT,s);
}
#endif
