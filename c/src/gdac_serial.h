#ifndef LT48_GDAC_SERIAL_H
#define LT48_GDAC_SERIAL_H
#include <labtool48/labtool48.h>
/* Internal resource owner. Catalogue admission awaits complete operations. */
struct lt48_gdac_serial_resources {
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *waves[28];
    lt48_wave_code applets[28];
    lt48_applet_address addresses[28];
    size_t address_count;
    lt48_compile_variable variables[8];
    size_t variable_count;
    const unsigned char *fpga;
    unsigned char *idle;
    size_t idle_size;
    uint32_t capacity;
    uint16_t definition, nominal, man, dev;
    const uint32_t *sectors;size_t sector_count;
};
#endif
