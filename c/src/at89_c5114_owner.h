#ifndef LT48_AT89_C5114_OWNER_H
#define LT48_AT89_C5114_OWNER_H
#include "at89_c5114_sequence.h"
typedef lt48_at89_tail_resources lt48_c5114_resources;
lt48_status lt48_c5114_open(const lt48_installation *,size_t,lt48_c5114_resources **);
void lt48_c5114_close(lt48_c5114_resources *);
/* Exact C5114 owner; source-audited. Hardware qualification is separate. */
lt48_status lt48_c5114_validate(const lt48_at89_tail_resources *,lt48_operation_kind,
    uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_c5114_execute(const lt48_at89_tail_resources *,
    const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,
    const unsigned char *,size_t,unsigned char *,size_t,
    lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
#endif
