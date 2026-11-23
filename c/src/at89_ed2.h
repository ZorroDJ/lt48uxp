#ifndef LT48_AT89_ED2_INTERNAL_H
#define LT48_AT89_ED2_INTERNAL_H
#include <labtool48/labtool48.h>
/* Internal building blocks, not support admission. Fields are source flags,
 * lock level, then numeric extra-memory values. Preserve HSB bit 4 on ED/ID. */
typedef struct lt48_ed2_layout {
    unsigned definition,family,pinmap,flags,numbers,capacity,cc03;
} lt48_ed2_layout;
lt48_status lt48_ed2_layout_for(unsigned,lt48_ed2_layout *);
lt48_status lt48_ed2_address(const lt48_ed2_layout *,uint32_t,uint32_t *,uint32_t *);
lt48_status lt48_ed2_decode(const lt48_ed2_layout *,unsigned,const unsigned char *,unsigned char *);
lt48_status lt48_ed2_encode(const lt48_ed2_layout *,unsigned,const unsigned char *,unsigned *);
struct lt48_ed2_resources {
    lt48_ed2_layout layout;
    lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[15];
    lt48_compile_variable vars[6];size_t var_count;
    lt48_wave_code applet[4];lt48_applet_address addresses[4];
    const unsigned char *fpga;unsigned char *idle;size_t idle_size;
    unsigned manufacturer,device;
};
enum lt48_ed2_fragment { LT48_ED2_PROGRAM_INIT, LT48_ED2_EXTRA_WRITE, LT48_ED2_HSB_WRITE };
lt48_status lt48_ed2_compile_fragment(const lt48_wave_code *,enum lt48_ed2_fragment,
    const lt48_pin_map *,const lt48_compile_variable *,size_t,lt48_compiled_wave **);
lt48_status lt48_ed2_open(const lt48_installation *,size_t,lt48_ed2_resources **);
void lt48_ed2_close(lt48_ed2_resources *);
lt48_status lt48_ed2_validate(const lt48_ed2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
lt48_status lt48_ed2_execute(const lt48_ed2_resources *,const lt48_protocol_transport *,void *,
    lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,
    lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
#endif
