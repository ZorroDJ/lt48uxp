#ifndef LT48_ST7_H
#define LT48_ST7_H
#include <labtool48/labtool48.h>

lt48_status lt48_st7_open(const lt48_installation*,size_t,lt48_st7_resources**);
void lt48_st7_close(lt48_st7_resources*);
unsigned lt48_st7_qualified(const lt48_st7_resources*);
lt48_status lt48_st7_validate(const lt48_st7_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
lt48_status lt48_st7_execute_candidate(const lt48_st7_resources*,const lt48_protocol_transport*,void*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result*);
int lt48_st7_admitted_internal(unsigned,unsigned);
unsigned lt48_st7_option_bytes(const lt48_st7_resources*);
#endif
