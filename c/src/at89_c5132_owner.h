#ifndef LT48_AT89_C5132_OWNER_H
#define LT48_AT89_C5132_OWNER_H
#include "at89_c5132_sequence.h"
/* Exact C5132 owner; experimental, not admitted until endpoint and GUI qualification. */
lt48_status lt48_c5132_validate(const lt48_c5132_resources *,lt48_operation_kind,
    uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_c5132_execute(const lt48_c5132_resources *,
    const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,
    const unsigned char *,size_t,unsigned char *,size_t,
    lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
#endif
