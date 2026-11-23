#ifndef LT48_AT89_TAIL_H
#define LT48_AT89_TAIL_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "../../NewDeviceLib/sources/mcu70/ic2-profile.h"
/* Resource closure only. No operation profile is admitted by this loader. */
typedef struct lt48_at89_tail_resources {
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[17];
    lt48_compile_variable vars[6];
    size_t variable_count;
    unsigned definition, family, pinmap;
    uint32_t primary_count, auxiliary_value;
    const unsigned char *fpga;
    lt48_wave_code read_applet;
    unsigned char *idle;
    size_t idle_size;
} lt48_at89_tail_resources;
lt48_status lt48_at89_tail_open(const lt48_installation *,size_t,lt48_at89_tail_resources **);
void lt48_at89_tail_close(lt48_at89_tail_resources *);
#endif
