#ifndef LT48_PIC_SOURCE_STARTUP_H
#define LT48_PIC_SOURCE_STARTUP_H
#include "transaction.h"

typedef enum { LT48_PIC_START_POWERON, LT48_PIC_START_STATUS,
    LT48_PIC_START_SETTLE, LT48_PIC_START_ARM, LT48_PIC_START_BUSY } lt48_pic_start_phase;
typedef lt48_status (*lt48_pic_start_invoke)(void *,lt48_pic_start_phase,uint16_t,uint16_t *);
#define LT48_PIC_SOURCE_POWER_TIMEOUT_SECONDS 10u

/* Original compact helper 00425550 through its return. STATUS obtains cached
 * status or queries when absent; SETTLE forces 4f 64 00 and drains the reply.
 * ARM sends 56 0a 01; BUSY sends 40 01. Neither DEVINIT nor DEVRESET belongs
 * here. Unlike the original, transport write failures cannot report success.
 * The caller always releases power after an attempted start, even on failure. */
static inline lt48_status lt48_pic_startup_run(uint16_t poweron,
    lt48_pic_start_invoke invoke,void *context) {
    if(!invoke || poweron==LT48_PIC_ABSENT_ROLE)return LT48_INVALID_ARGUMENT;
    uint16_t controller=0;
    lt48_status power=invoke(context,LT48_PIC_START_POWERON,poweron,&controller);
    lt48_status st=invoke(context,LT48_PIC_START_STATUS,0,&controller);
    if(st==LT48_OK && !controller)st=invoke(context,LT48_PIC_START_SETTLE,0,&controller);
    if(st==LT48_OK)st=invoke(context,LT48_PIC_START_STATUS,0,&controller);
    if(st!=LT48_OK)return power==LT48_OK?st:power;
    if(controller==2)return power==LT48_OK?LT48_SAFETY_REJECTED:power;
    if(power!=LT48_OK)return power;
    st=invoke(context,LT48_PIC_START_ARM,0,&controller);
    if(st==LT48_OK)st=invoke(context,LT48_PIC_START_BUSY,0,&controller);
    return st;
}
#endif
