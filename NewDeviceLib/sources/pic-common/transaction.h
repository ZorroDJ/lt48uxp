#ifndef LT48_PIC_COMMON_TRANSACTION_H
#define LT48_PIC_COMMON_TRANSACTION_H
#include <labtool48/labtool48.h>
#include <stdint.h>

typedef enum { LT48_PIC_TX_MODE,LT48_PIC_TX_START,LT48_PIC_TX_PRE,
    LT48_PIC_TX_ACTION,LT48_PIC_TX_POST } lt48_pic_transaction_phase;
typedef struct { uint16_t role,timeout_seconds; } lt48_pic_transaction_step;
typedef struct {
    uint16_t mode;
    lt48_pic_transaction_step pre,action,post;
} lt48_pic_transaction_plan;
#define LT48_PIC_ABSENT_ROLE UINT16_MAX
typedef lt48_status (*lt48_pic_transaction_invoke)(void *,lt48_pic_transaction_phase,uint16_t,uint16_t);

/* Parameterized shared callback order; deliberately propagates Post failure
 * even where original configuration worker 004503a0 discards DEVRESET status.
 * Caller retains the operation session and must run cleanup on any result. */
static inline lt48_status lt48_pic_transaction_run(const lt48_pic_transaction_plan *plan,
    int started,lt48_pic_transaction_invoke invoke,void *context) {
    if(!plan||!invoke||!plan->mode||plan->action.role==LT48_PIC_ABSENT_ROLE||
       !plan->action.timeout_seconds||
       (plan->pre.role!=LT48_PIC_ABSENT_ROLE&&!plan->pre.timeout_seconds)||
       (plan->post.role!=LT48_PIC_ABSENT_ROLE&&!plan->post.timeout_seconds))return LT48_INVALID_ARGUMENT;
    lt48_status st=invoke(context,LT48_PIC_TX_MODE,plan->mode,0);
    if(st==LT48_OK&&!started)st=invoke(context,LT48_PIC_TX_START,plan->mode,0);
    if(st==LT48_OK&&plan->pre.role!=LT48_PIC_ABSENT_ROLE)
        st=invoke(context,LT48_PIC_TX_PRE,plan->pre.role,plan->pre.timeout_seconds);
    if(st==LT48_OK)st=invoke(context,LT48_PIC_TX_ACTION,plan->action.role,plan->action.timeout_seconds);
    if(st==LT48_OK&&plan->post.role!=LT48_PIC_ABSENT_ROLE)
        st=invoke(context,LT48_PIC_TX_POST,plan->post.role,plan->post.timeout_seconds);
    return st;
}
#endif
