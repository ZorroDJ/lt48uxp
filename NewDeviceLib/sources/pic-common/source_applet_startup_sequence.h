#ifndef LT48_PIC_SOURCE_APPLET_STARTUP_SEQUENCE_H
#define LT48_PIC_SOURCE_APPLET_STARTUP_SEQUENCE_H
#include <stdint.h>
/* Original 00468580. Phase bodies remain separate resource/contact/ID/transport
 * services. Win32 button values describe the original policy, not a UI API.
 * A negative callback result is an authored transport-error stop; zero for a
 * contact or ID check is a mismatch. Other zero results stop startup.
 * ID Ignore repeats FPGA/selector/power entry with checks disabled. */
enum lt48_pic_applet_startup_phase {
    LT48_PIC_APP_START_DOWNLOAD,LT48_PIC_APP_START_CONTACT,
    LT48_PIC_APP_START_CONTACT_CHOICE,LT48_PIC_APP_START_CONTACT_LOG,
    LT48_PIC_APP_START_FPGA,LT48_PIC_APP_START_SELECTOR,
    LT48_PIC_APP_START_POWER,LT48_PIC_APP_START_ID,
    LT48_PIC_APP_START_CLEANUP,LT48_PIC_APP_START_ID_CHOICE
};
enum lt48_pic_applet_startup_choice {LT48_PIC_APP_START_ABORT=3,
    LT48_PIC_APP_START_RETRY=4,LT48_PIC_APP_START_IGNORE=5};
typedef struct {
    uint32_t flags,mode,nominal;
    unsigned has_fpga,checked,cancelled;
} lt48_pic_applet_startup_state;
typedef int (*lt48_pic_applet_startup_call)(void *,enum lt48_pic_applet_startup_phase,
    lt48_pic_applet_startup_state *);
static inline int lt48_pic_startup_sequence_profile(lt48_pic_applet_startup_state *s,
    lt48_pic_applet_startup_call call,void *context,int applet) {
    if(!s||!call)return -1;
    if(call(context,LT48_PIC_APP_START_DOWNLOAD,s)!=1)return 0;
    for(;;) {
        if(s->cancelled)return 0;
        if(!s->checked&&(s->flags&1u)) {
            int matched=call(context,LT48_PIC_APP_START_CONTACT,s);
            if(matched<0)return 0;
            if(!matched) {
                int choice=call(context,LT48_PIC_APP_START_CONTACT_CHOICE,s);
                if(call(context,LT48_PIC_APP_START_CONTACT_LOG,s)<0)return 0;
                if(choice==LT48_PIC_APP_START_RETRY)continue;
                if(choice==LT48_PIC_APP_START_ABORT){s->cancelled=1;return 0;}
                if(choice!=LT48_PIC_APP_START_IGNORE)return 0;
            }
        }
        if(s->has_fpga&&call(context,LT48_PIC_APP_START_FPGA,s)<=0)return 0;
        if(call(context,LT48_PIC_APP_START_SELECTOR,s)<=0)return 0;
        if(call(context,LT48_PIC_APP_START_POWER,s)<=0)return 0;
        if(s->checked)return 1;
        if(!(s->flags&2u)||(applet&&((s->mode==7&&!(s->flags&UINT32_C(0x80000)))||
           (s->mode==2&&(s->flags&UINT32_C(0x40000))&&!(s->flags&UINT32_C(0x80000)))))) {
            s->checked=1;return 1;
        }
        int matched=call(context,LT48_PIC_APP_START_ID,s);
        if(matched<0)return 0;
        if(matched)return 1;
        if(call(context,LT48_PIC_APP_START_CLEANUP,s)<0)return 0;
        int choice=call(context,LT48_PIC_APP_START_ID_CHOICE,s);
        if(choice==LT48_PIC_APP_START_ABORT)return 0;
        if(choice==LT48_PIC_APP_START_IGNORE)s->checked=1;
        else if(choice!=LT48_PIC_APP_START_RETRY)return 0;
    }
}
static inline int lt48_pic_applet_startup_sequence(lt48_pic_applet_startup_state *s,
    lt48_pic_applet_startup_call call,void *context) {
    return lt48_pic_startup_sequence_profile(s,call,context,1);
}
/* Original compact 0044f440 checks ID whenever source flag 2 requests it. */
static inline int lt48_pic_compact_startup_sequence(lt48_pic_applet_startup_state *s,
    lt48_pic_applet_startup_call call,void *context) {
    return lt48_pic_startup_sequence_profile(s,call,context,0);
}
#endif
