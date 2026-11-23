#ifndef LT48_PIC_SOURCE_HOST_H
#define LT48_PIC_SOURCE_HOST_H
#include "source_session.h"

typedef lt48_status (*lt48_pic_source_controller_write)(void *,const unsigned char *,size_t);
/* force=0: cached status, querying if absent. force=1: force a settled query.
 * The adapter supplies timeout/status parsing, with raw controller status
 * separate from transport failure. No source callback is hidden in this hook. */
typedef lt48_status (*lt48_pic_source_controller_status)(void *,int,uint16_t *);
typedef struct {uint16_t voltage_variable,poweron_role,poweroff_role;} lt48_pic_source_host_binding;
typedef struct {
    uint16_t voltage_variable,poweron_role,poweroff_role;
    uint32_t voltage_centivolts,minimum_centivolts,maximum_centivolts;
    lt48_pic_source_controller_write write;
    lt48_pic_source_controller_status status;
    lt48_pic_source_runner runner;
    void *context;
    lt48_pic_source_runner cleanup_runner;
} lt48_pic_source_host;

/* The owner prepares transport/FPGA first. These shared source-bound services
 * deliberately do not execute DEVINIT/DEVRESET, owned by the operation plan. */
lt48_status lt48_pic_source_host_begin_internal(lt48_pic_source_session *,
    const lt48_pic_source_host *,uint16_t,uint32_t);
lt48_status lt48_pic_source_host_end_internal(lt48_pic_source_session *,
    const lt48_pic_source_host *,uint16_t);
#endif
