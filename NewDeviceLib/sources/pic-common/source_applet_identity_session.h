#ifndef LT48_PIC_SOURCE_APPLET_IDENTITY_SESSION_H
#define LT48_PIC_SOURCE_APPLET_IDENTITY_SESSION_H
#include "source_session.h"
#include "source_applet_identity.h"
#include <string.h>
typedef struct {
    uint16_t getid_role,manufacturer,device,table_index;
    uint8_t manufacturer_buffer,device_buffer;
} lt48_pic_source_identity_session_plan;
typedef struct {unsigned matched,diagnostic;uint16_t manufacturer,device;} lt48_pic_source_identity_result;
static inline int lt48_pic_source_identity_echo(void *opaque,uint32_t address,uint16_t *manufacturer,uint16_t *device) {
    lt48_pic_source_identity_result *r=opaque;(void)address;
    *manufacturer=r->manufacturer;*device=r->device;return 1;
}
static inline void lt48_pic_source_identity_diagnose(void *opaque,unsigned kind,uint16_t value) {
    lt48_pic_source_identity_result *r=opaque;(void)value;r->diagnostic|=kind;
}
/* Active-session substage, with no preparation/power/cleanup hidden inside.
 * The operation owner supplies a source-bound plan and stages the session.
 * Indexed ID procedures require a separately bound address table. */
static inline lt48_status lt48_pic_source_identity_session_profile(lt48_pic_source_session *session,
    const lt48_pic_source_identity_session_plan *p,lt48_pic_source_runner runner,void *context,
    lt48_pic_source_identity_result *result,int compact) {
    if(result)memset(result,0,sizeof(*result));
    if(!session||!p||!result)return LT48_INVALID_ARGUMENT;
    if(p->getid_role==UINT16_MAX){result->matched=1;return LT48_OK;}
    if(p->table_index)return LT48_UNSUPPORTED;
    if(!runner||p->manufacturer_buffer>=32||p->device_buffer>=32||p->manufacturer_buffer==p->device_buffer)
        return LT48_INVALID_ARGUMENT;
    if(!lt48_pic_source_session_has_callback_internal(session,p->getid_role))return LT48_UNSUPPORTED;
    lt48_status status=LT48_OK;
    if(!compact) {
    lt48_pic_source_state state;status=lt48_pic_source_session_state_internal(session,&state);
    if(status!=LT48_OK)return status;
    unsigned char buffers[32][32];memcpy(buffers,state.buffers,sizeof(buffers));
    buffers[p->manufacturer_buffer][0]=(unsigned char)p->manufacturer;
    buffers[p->manufacturer_buffer][1]=(unsigned char)(p->manufacturer>>8);
    buffers[p->device_buffer][0]=(unsigned char)p->device;
    buffers[p->device_buffer][1]=(unsigned char)(p->device>>8);
    state.buffers=(const unsigned char (*)[32])buffers;
    status=lt48_pic_source_session_restore_internal(session,&state);
    }
    if(status==LT48_OK)status=lt48_pic_source_session_invoke_current_internal(session,p->getid_role,10,runner,context);
    if(status!=LT48_OK)return status;
    const unsigned char *m,*d;
    status=lt48_pic_source_session_buffer_internal(session,p->manufacturer_buffer,&m);
    if(status==LT48_OK)status=lt48_pic_source_session_buffer_internal(session,p->device_buffer,&d);
    if(status!=LT48_OK)return status;
    result->manufacturer=(uint16_t)(m[0]|(uint16_t)m[1]<<8);result->device=(uint16_t)(d[0]|(uint16_t)d[1]<<8);
    lt48_pic_source_identity_plan comparison={p->manufacturer,p->device,1,0,NULL,0};
    result->matched=(unsigned)lt48_pic_source_identity_check_profile(&comparison,lt48_pic_source_identity_echo,
        lt48_pic_source_identity_diagnose,result,compact);
    return LT48_OK;
}
static inline lt48_status lt48_pic_source_identity_session(lt48_pic_source_session *session,
    const lt48_pic_source_identity_session_plan *p,lt48_pic_source_runner runner,void *context,
    lt48_pic_source_identity_result *result) {
    return lt48_pic_source_identity_session_profile(session,p,runner,context,result,0);
}
/* Compact 004506b0 retains the current ManCode/DevCode words before GETID;
 * unlike the applet override, it does not seed catalogue words first. */
static inline lt48_status lt48_pic_source_compact_identity_session(lt48_pic_source_session *session,
    const lt48_pic_source_identity_session_plan *p,lt48_pic_source_runner runner,void *context,
    lt48_pic_source_identity_result *result) {
    return lt48_pic_source_identity_session_profile(session,p,runner,context,result,1);
}
#endif
