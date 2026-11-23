#ifndef LT48_AT89_RE2_H
#define LT48_AT89_RE2_H
#include <labtool48/labtool48.h>
/* Pre-admission resources: no public operation entry point yet. */
typedef struct lt48_re2_resources {
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[16]; /* 0..11 original; FCB, HSB, preamble, VCC/delay */
    lt48_compile_variable vars[6];
    lt48_wave_code applet[6];
    lt48_applet_address addresses[6];
    unsigned char *guarded[2];
    const unsigned char *fpga;
    unsigned char *idle;
    size_t idle_size;
} lt48_re2_resources;
lt48_status lt48_re2_open(const lt48_installation *,size_t,lt48_re2_resources **);
void lt48_re2_close(lt48_re2_resources *);
lt48_status lt48_re2_address(uint32_t,uint32_t *,uint32_t *,uint32_t *);
lt48_status lt48_re2_compile_fragment(const lt48_wave_code *,unsigned,const lt48_pin_map *,const lt48_compile_variable *,lt48_compiled_wave **);
lt48_status lt48_re2_validate(const lt48_re2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_re2_execute(const lt48_re2_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
#endif
