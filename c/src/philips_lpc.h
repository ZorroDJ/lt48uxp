#ifndef LT48_PHILIPS_LPC_H
#define LT48_PHILIPS_LPC_H
#include <labtool48/labtool48.h>
struct lt48_lpc_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[24];
 lt48_compile_variable vars[5];unsigned definition,family,pinmap,code_size;
 unsigned char *idle;size_t idle_size;
};
#endif
