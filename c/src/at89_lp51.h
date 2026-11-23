#ifndef LT48_AT89_LP51_INTERNAL_H
#define LT48_AT89_LP51_INTERNAL_H
#include <labtool48/labtool48.h>
enum lt48_lp51_fragment {
    LT48_LP51_FUSES, LT48_LP51_LOCKS, LT48_LP51_CHIP_ERASE
};
/* Exact-source fragments only; this is not device admission.
 * W652 fuse row is kept in one completed/polled transaction: 16 bytes for
 * ED2/RD2, 19 for ID2 (DEVICE variable 18 == 0x69). Locks are separate.
 * W651 is limited to its first chip erase, excluding fuse and calibration
 * mutation. The caller must preserve EEPROM/signature/configuration state.
 * Seed buffer 26 with the two-byte calibration offset 0x0180. */
lt48_status lt48_lp51_compile_fragment(const lt48_wave_code *,
    enum lt48_lp51_fragment,const lt48_pin_map *,
    const lt48_compile_variable *,size_t,lt48_compiled_wave **);
#endif
