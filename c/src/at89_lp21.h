#ifndef LT48_AT89_LP21_INTERNAL_H
#define LT48_AT89_LP21_INTERNAL_H
#include <labtool48/labtool48.h>

/* Internal building blocks. These do not admit a device or access USB. */
enum lt48_lp21_row { LT48_LP21_SIGNATURE, LT48_LP21_FUSES, LT48_LP21_LOCKS };
typedef struct lt48_lp21_settings {
    /* Source object -1: category 0 fields 0..10, category 1 field 0,
     * category 2 field 0. Flags describe programmed bytes, not GUI labels. */
    unsigned char value[13];
    unsigned char signature[64];
} lt48_lp21_settings;
typedef struct lt48_lp21_step {
    enum lt48_lp21_row row;
    lt48_lp21_settings expected;
} lt48_lp21_step;
typedef struct lt48_lp21_plan {
    size_t count;
    lt48_lp21_step steps[4];
} lt48_lp21_plan;

/* Split the exact audited W152 at complete command boundaries. Its source
 * bytes remain in the operator's imported library. Page buffer 12 must be
 * seeded to 32 before executing the signature fragment. */
lt48_status lt48_lp21_compile_row(const lt48_wave_code *, enum lt48_lp21_row,
    const lt48_pin_map *, const lt48_compile_variable *, size_t,
    lt48_compiled_wave **);

/* Bits 0..12 select value fields; bit 13 selects the signature. Unselected
 * input bytes are ignored. Each step must be independently read back before
 * the next step, especially before adding locks. No implicit chip erase. */
lt48_status lt48_lp21_plan_settings(const lt48_lp21_settings *,
    const lt48_lp21_settings *, unsigned, lt48_lp21_plan *);

typedef lt48_status (*lt48_lp21_read_settings_fn)(void *,lt48_lp21_settings *);
typedef lt48_status (*lt48_lp21_write_row_fn)(void *,enum lt48_lp21_row,
    const lt48_lp21_settings *);
typedef struct lt48_lp21_update_result {
    lt48_lp21_settings before,observed;
    unsigned writes_attempted,steps_verified;
    int observed_valid;
} lt48_lp21_update_result;
/* Callbacks are supplied by the serial operation owner. A failed write,
 * read or comparison stops execution before subsequent rows/locks. Failure
 * after a write can leave temporary settings: callers must report it and
 * perform ordinary power-off cleanup, not claim an atomic update. */
lt48_status lt48_lp21_update_settings(void *,lt48_lp21_read_settings_fn,
    lt48_lp21_write_row_fn,const lt48_lp21_settings *,unsigned,
    lt48_lp21_update_result *);
#endif
