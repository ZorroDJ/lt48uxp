#ifndef LABTOOL48_NMOS68705_NUM_H
#define LABTOOL48_NMOS68705_NUM_H
#include <labtool48/labtool48.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Offline capture decoding only. This API does not enable a device or operate
 * a programmer. One sample is PB7..PB0, taken once per complete EXTAL cycle.
 * The capture must contain at least two complete address-space traversals.
 * A board-side acquisition path must separately prove these sample semantics.
 */
typedef struct lt48_nmos68705_num_contract {
    unsigned space_size;              /* 2048 or 4096 */
    unsigned bootstrap_start;
    unsigned bootstrap_size;
    unsigned char bootstrap_sha256[32];
    /* For each PB bit: the address bit it carries during an address phase.
     * Require eight distinct bits and coverage of address bits 0..5. */
    unsigned char address_bits[8];
} lt48_nmos68705_num_contract;

typedef struct lt48_nmos68705_num_result {
    unsigned sample_phase;
    unsigned first_frame_address;
    size_t frames_checked;
    size_t eprom_bytes;               /* Includes the MOR cell. */
} lt48_nmos68705_num_result;

/* Authenticate phase/address alignment against the immutable bootstrap hash,
 * check address progression and all repeated EPROM/bootstrap observations.
 * Return observed EPROM, MOR and vectors at their physical image addresses;
 * other addresses are explicit FF padding. Never echo expected image data.
 * Output image and result are unchanged on failure. Samples, output image,
 * contract and result must not overlap. No programmer operation is admitted
 * merely because this decoder succeeds on a supplied capture.
 */
LT48_API lt48_status lt48_nmos68705_num_decode(
    const lt48_nmos68705_num_contract *, const unsigned char *samples,
    size_t sample_count, unsigned char *image, size_t image_size,
    lt48_nmos68705_num_result *);

/* Transport-independent collection of indexed, consecutive sample pairs.
 * The callback must return actual frozen PB samples at the requested 16-bit
 * cycle index, with the target clock running continuously between calls.
 * Callback failures (including cancellation) propagate without output changes.
 * The operation owner must perform entry, power/clock setup and unconditional
 * stop/power cleanup. This API alone does not operate or admit a programmer.
 */
typedef lt48_status (*lt48_nmos68705_num_pair_reader)(
    void *context, uint16_t sample_index, unsigned char pair[2]);
LT48_API lt48_status lt48_nmos68705_num_collect(
    const lt48_nmos68705_num_contract *, lt48_nmos68705_num_pair_reader,
    void *context, unsigned char *image, size_t image_size,
    lt48_nmos68705_num_result *);

#ifdef __cplusplus
}
#endif
#endif
