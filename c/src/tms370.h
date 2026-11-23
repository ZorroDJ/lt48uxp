#ifndef LT48_TMS370_H
#define LT48_TMS370_H
#include <labtool48/labtool48.h>

lt48_status lt48_tms370_open(const lt48_installation*,size_t,lt48_tms370_resources**);
void lt48_tms370_close(lt48_tms370_resources*);
unsigned lt48_tms370_qualified(const lt48_tms370_resources*);
lt48_status lt48_tms370_validate(const lt48_tms370_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
lt48_status lt48_tms370_execute_candidate(const lt48_tms370_resources*,const lt48_protocol_transport*,void*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result*);
int lt48_tms370_admitted_internal(unsigned,unsigned);
#endif
