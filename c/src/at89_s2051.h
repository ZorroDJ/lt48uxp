#ifndef LT48_AT89_S2051_H
#define LT48_AT89_S2051_H
#include <labtool48/labtool48.h>
/* Exact parallel-procedure resources and verified source aliases. */
struct lt48_s2051_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[16];
 lt48_compile_variable vars[6];lt48_wave_code applet[4];lt48_applet_address addresses[4];
 const unsigned char *fpga;unsigned char *idle;size_t idle_size;
 unsigned definition,family,pinmap,flash,manufacturer,device;
};
lt48_status lt48_s2051_open(const lt48_installation *,size_t,lt48_s2051_resources **);
void lt48_s2051_close(lt48_s2051_resources *);
lt48_status lt48_s2051_address(const lt48_s2051_resources *,uint32_t,uint32_t *,uint32_t *);
lt48_status lt48_s2051_validate(const lt48_s2051_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_s2051_execute(const lt48_s2051_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
#endif
