#ifndef LT48_PIC_SOURCE_APPLET_ACTION_H
#define LT48_PIC_SOURCE_APPLET_ACTION_H
#include <labtool48/labtool48.h>
#include <stdint.h>
#include <limits.h>

/* Non-table branch of original 00467e80. Addresses and range size are in
 * device elements, not dense-image bytes. A callback's transport success
 * and its DataIn Boolean result are separate. UI Retry/Ignore is not hidden
 * here: the adapter reports an unsuccessful Boolean as DEVICE_MISMATCH. */
typedef struct {uint32_t first,last,step;uint16_t width;} lt48_pic_source_applet_action_range;
typedef struct {uint32_t completed_elements,next_address,failed_address;} lt48_pic_source_applet_action_result;
typedef lt48_status (*lt48_pic_source_applet_action_invoke)(void *,uint32_t,uint32_t);
static inline int lt48_pic_source_applet_action_valid(const lt48_pic_source_applet_action_range *p) {
    if(!p||!p->step||!p->width||p->width>4||p->first>p->last||p->last>=INT32_MAX)return 0;
    uint64_t count=(uint64_t)p->last-p->first+1;
    /* The original can overrun a nonaligned terminal sector. Require a whole
     * number of source sectors before I/O rather than inventing clipping. */
    return count%p->step==0 && count*p->width<=INT32_MAX && (uint64_t)p->step*p->width<=INT32_MAX;
}
static inline lt48_status lt48_pic_source_applet_action_run(
    const lt48_pic_source_applet_action_range *p,lt48_pic_source_applet_action_invoke invoke,
    void *context,lt48_pic_source_applet_action_result *out) {
    if(out)*out=(lt48_pic_source_applet_action_result){0,p?p->first:0,UINT32_MAX};
    if(!out||!invoke||!lt48_pic_source_applet_action_valid(p))return LT48_INVALID_ARGUMENT;
    for(uint32_t address=p->first;address<=p->last;address+=p->step) {
        /* Actual source dispatcher receives ten seconds; the separate host
         * progress timer is thirty seconds and is not this callback timeout. */
        lt48_status st=invoke(context,address,10);
        out->next_address=address+p->step;
        if(st!=LT48_OK){out->failed_address=address;return st;}
        out->completed_elements+=p->step;
    }
    return LT48_OK;
}
#endif
