#ifndef LT48_NMOS68705_INTERNAL_H
#define LT48_NMOS68705_INTERNAL_H
#include <labtool48/nmos68705_num.h>
/* Variant indices match the exact constructor member table: R3,R5,U3,U5,P3,P5.
 * A decoder contract is not admission or a physical NUM-entry qualification. */
lt48_status lt48_nmos68705_contract_for_variant_internal(unsigned,
 lt48_nmos68705_num_contract *);
lt48_status lt48_nmos68705_get_contract_internal(const lt48_nmos68705_resources *,
 lt48_nmos68705_num_contract *);
/* Offline audit entry: caller supplies the independently identified NUM
 * contract. Never used by the USB wrapper or GTK execution path. */
lt48_status lt48_nmos68705_execute_with_contract_internal(const lt48_nmos68705_resources *,
 const lt48_nmos68705_num_contract *,const lt48_protocol_transport *,void *,
 lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,
 unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
#endif
