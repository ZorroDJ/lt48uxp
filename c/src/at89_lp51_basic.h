#ifndef LT48_AT89_LP51_BASIC_H
#define LT48_AT89_LP51_BASIC_H
#include <labtool48/labtool48.h>
/* Exact parallel-procedure resources and separately verifiable fragments. */
struct lt48_lp51_basic_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[16];
 lt48_compile_variable vars[5];lt48_wave_code applet[6];lt48_applet_address addresses[6];
 const unsigned char *fpga;unsigned char *idle;size_t idle_size;
 unsigned definition,family,pinmap,flash,manufacturer,device;
};
lt48_status lt48_lp51_basic_open(const lt48_installation *,size_t,lt48_lp51_basic_resources **);
void lt48_lp51_basic_close(lt48_lp51_basic_resources *);
lt48_status lt48_lp51_basic_address(const lt48_lp51_basic_resources *,uint32_t,uint32_t *,uint32_t *);
/* 0 nonmutating init; 1 nonmutating reset; 2 fuses only; 3 locks only. */
lt48_status lt48_lp51_basic_compile_fragment(const lt48_wave_code *,unsigned,const lt48_pin_map *,const lt48_compile_variable *,lt48_compiled_wave **);
lt48_status lt48_lp51_basic_validate(const lt48_lp51_basic_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_lp51_basic_execute(const lt48_lp51_basic_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
#endif
