#ifndef LT48_NUM_TRANSPORT_INTERNAL_H
#define LT48_NUM_TRANSPORT_INTERNAL_H
#include <labtool48/nmos68705_num.h>
/* Draft private transport, not an admitted device owner. Caller must have
 * loaded the exact qualified capture image and authored worker at 6000.
 * A failed/partial transport requires the owner's separate recovery path.
 */
lt48_status lt48_num_start_internal(lt48_runtime_transport_session *);
/* With target rails off and preparation applet at 6E00, BEFORE loading the
 * capture image. A nonzero result prohibits configuring/driving ready P14. */
lt48_status lt48_num_prepare_ready_internal(lt48_runtime_transport_session *);
lt48_status lt48_num_stop_internal(lt48_runtime_transport_session *);
lt48_status lt48_num_pair_internal(void *, uint16_t, unsigned char [2]);
/* Fresh per-collection context. Batch callback follows the collector's 1024
 * stride and checks its exact next index; never reuse after a failed call. */
typedef struct {
    lt48_runtime_transport_session *wire;
    size_t main_pairs, delivered;
    uint16_t next_index;
    unsigned char cache[32];
    unsigned used, count;
    int failed;
} lt48_num_reader;
lt48_status lt48_num_reader_init_internal(lt48_num_reader *,
    lt48_runtime_transport_session *, unsigned space_size);
lt48_status lt48_num_cached_pair_internal(void *, uint16_t, unsigned char [2]);
#endif
