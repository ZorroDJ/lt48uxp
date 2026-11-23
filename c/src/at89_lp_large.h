#ifndef LT48_AT89_LP_LARGE_INTERNAL_H
#define LT48_AT89_LP_LARGE_INTERNAL_H
#include <labtool48/labtool48.h>
/* Internal source fragments. These helpers do not admit devices or access USB.
 * The operation owner must snapshot and independently verify fuses, locks and
 * the complete signature array around destructive operations. */
enum lt48_lp_large_fragment {
    LT48_LP_LARGE_FUSES,
    LT48_LP_LARGE_LOCKS,
    LT48_LP_LARGE_CHIP_ERASE
};
/* W5d3: split fuse/lock transactions to apply locks only after verification.
 * W5d2: first chip-erase transaction only; exclude its later calibration-page
 * auto-erase. Byte 8 (user-signature enable) remains caller-owned state.
 * Source runtime seeds: buffer 16=64, buffer 26=64 or 128 as appropriate. */
lt48_status lt48_lp_large_compile_fragment(const lt48_wave_code *,
    enum lt48_lp_large_fragment,const lt48_pin_map *,
    const lt48_compile_variable *,size_t,lt48_compiled_wave **);

#endif
