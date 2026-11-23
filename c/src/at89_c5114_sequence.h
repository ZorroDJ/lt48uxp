#ifndef LT48_AT89_C5114_SEQUENCE_H
#define LT48_AT89_C5114_SEQUENCE_H
#include "at89_tail.h"
/* Internal powered-access stage. Caller owns power, transport, configuration
 * preservation and independent verification. This does not admit a device. */
typedef struct lt48_c5114_frame {
    unsigned char buffers[32][32];
    lt48_runtime_configuration_value config[7];
    size_t config_count;
    uint32_t volts[4];
    unsigned mode;
    unsigned id_address,id_failure; /* Last completed DEVINIT diagnostic. */
} lt48_c5114_frame;
typedef struct lt48_c5114_transport {
    const lt48_at89_tail_resources *resources;
    lt48_runtime_transport_session *wire;
    lt48_compile_variable variables[1];
} lt48_c5114_transport;
lt48_status lt48_c5114_transport_step(void *,unsigned,lt48_c5114_frame *);
typedef lt48_status (*lt48_c5114_step_fn)(void *,unsigned,lt48_c5114_frame *);
lt48_status lt48_c5114_memory_sequence(const lt48_at89_tail_resources *,
    lt48_c5114_step_fn,void *,lt48_c5114_frame *,int,uint32_t,size_t,
    const unsigned char *,unsigned char *,size_t *,lt48_status *);
/* Settings order: flags17,18,19,20,4; lock choice1.
 * Write is a low-level complete-field commit; caller owns transition policy. */
lt48_status lt48_c5114_settings_sequence(const lt48_at89_tail_resources *,
    lt48_c5114_step_fn,void *,lt48_c5114_frame *,const unsigned char *,
    unsigned char *,lt48_status *);
/* Kept by the operation owner through failure/cancellation. A pending restore
 * requires a fresh, uncancellable transport before final power cleanup. */
typedef struct lt48_c5114_erase_state {
    unsigned char settings[6];
    int restore_pending;
    int erase_incomplete;
    size_t verified;
    uint32_t mismatch_address;
} lt48_c5114_erase_state;
lt48_status lt48_c5114_erase_sequence(const lt48_at89_tail_resources *,
    lt48_c5114_step_fn,void *,lt48_c5114_frame *,lt48_c5114_erase_state *,lt48_status *);
#endif
