#ifndef LT48_AT89_IC2_OWNER_H
#define LT48_AT89_IC2_OWNER_H
#include "at89_ic2_sequence.h"
/* Exact IC2 owner; source-audited. Hardware qualification is separate. */
lt48_status lt48_ic2_validate(const lt48_at89_tail_resources *,lt48_operation_kind,
    uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_ic2_execute(const lt48_at89_tail_resources *,
    const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,
    const unsigned char *,size_t,unsigned char *,size_t,
    lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
#endif
