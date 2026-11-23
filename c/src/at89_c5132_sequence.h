#ifndef LT48_AT89_C5132_SEQUENCE_H
#define LT48_AT89_C5132_SEQUENCE_H
#include "at89_tail.h"
/* Internal powered stages. No device admission or hardware qualification. */
typedef struct lt48_c5132_resources {
    lt48_at89_tail_resources *base;
    lt48_compiled_wave *extra_read,*extra_write;
    unsigned char guarded_read[265];size_t guarded_read_size;
} lt48_c5132_resources;
typedef struct lt48_c5132_frame {
    unsigned char buffers[32][32],extra[128];
    lt48_runtime_configuration_value config[5];size_t config_count;
    uint32_t volts[4];unsigned mode;
} lt48_c5132_frame;
typedef struct lt48_c5132_transport {
    const lt48_c5132_resources *resources;
    lt48_runtime_transport_session *wire;
    lt48_compile_variable variables[3];
    const lt48_protocol_transport *api;void *context;int guard_loaded;
} lt48_c5132_transport;
typedef lt48_status (*lt48_c5132_step_fn)(void *,unsigned,lt48_c5132_frame *);
lt48_status lt48_c5132_resources_open(const lt48_installation *,size_t,lt48_c5132_resources **);
void lt48_c5132_resources_close(lt48_c5132_resources *);
lt48_status lt48_c5132_transport_prepare(lt48_c5132_transport *);
lt48_status lt48_c5132_transport_step(void *,unsigned,lt48_c5132_frame *);
lt48_status lt48_c5132_memory_sequence(const lt48_c5132_resources *,lt48_c5132_step_fn,
    void *,lt48_c5132_frame *,int,uint32_t,size_t,const unsigned char *,unsigned char *,size_t *,lt48_status *);
/* Settings: flags5/4, lock7, numeric9/11. op13 reads the extra row; op8 runs the original write suffix only. */
lt48_status lt48_c5132_settings_sequence(const lt48_c5132_resources *,lt48_c5132_step_fn,
    void *,lt48_c5132_frame *,const unsigned char *,unsigned char *,lt48_status *);
typedef struct lt48_c5132_erase_state {
    unsigned char settings[5],extra[128];
    int restore_pending,erase_incomplete;
    size_t verified;uint32_t mismatch_address;
} lt48_c5132_erase_state;
lt48_status lt48_c5132_erase_sequence(const lt48_c5132_resources *,lt48_c5132_step_fn,
    void *,lt48_c5132_frame *,lt48_c5132_erase_state *,lt48_status *);
/* Restore an outstanding snapshot after completing an authorized erase. */
lt48_status lt48_c5132_restore_sequence(const lt48_c5132_resources *,lt48_c5132_step_fn,
    void *,lt48_c5132_frame *,lt48_c5132_erase_state *,lt48_status *);
#endif
