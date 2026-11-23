#ifndef LT48_HC705_JJSRPL_H
#define LT48_HC705_JJSRPL_H
#include <labtool48/labtool48.h>
/* Private offline audits exercise the real owner without production admission. */
lt48_status lt48_hc705_jjsrpl_open_candidate(const lt48_installation *,size_t,lt48_hc705_jjsrpl_resources **);
lt48_status lt48_hc705_jjsrpl_execute_internal(const lt48_hc705_jjsrpl_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
#endif
