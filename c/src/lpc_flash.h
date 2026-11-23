#ifndef LT48_LPC_FLASH_INTERNAL_H
#define LT48_LPC_FLASH_INTERNAL_H
#include <labtool48/labtool48.h>
struct lt48_lpcflash_resources {
 lt48_rez*rez;lt48_pin_map*pins;lt48_compiled_wave*wave[24];
 lt48_compile_variable vars[16];size_t var_count;
 unsigned definition,family,pinmap,capacity,fields,nominal,preserve_isp,sectors,sector_mask;
 unsigned char*idle;size_t idle_size;
};
#endif
