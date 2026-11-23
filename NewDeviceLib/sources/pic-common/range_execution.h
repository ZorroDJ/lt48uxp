#ifndef LT48_PIC_COMMON_RANGE_EXECUTION_H
#define LT48_PIC_COMMON_RANGE_EXECUTION_H
#include <labtool48/labtool48.h>
#include <string.h>
#include "traversal.h"
#include "transaction.h"

typedef enum {
    LT48_PIC_RANGE_MODE, LT48_PIC_RANGE_START, LT48_PIC_RANGE_PRE,
    LT48_PIC_RANGE_ELEMENT, LT48_PIC_RANGE_POST
} lt48_pic_range_phase;
typedef struct {
    uint16_t mode, element_bits, pre_role, element_role, post_role;
    uint8_t blank_byte;
} lt48_pic_range_contract;
/* DataOut/DataIn are the source host's expected/observed element buffers.
 * Mutable protocol state belongs to context and survives chunk boundaries.
 * A callback does not own the logical traversal cursor. */
typedef lt48_status (*lt48_pic_range_invoke)(void *, lt48_pic_range_phase,
    uint16_t, uint32_t, unsigned char *, unsigned char *, size_t);
typedef lt48_status (*lt48_pic_range_image_io)(void *, int, uint32_t,
    unsigned char *, size_t);
typedef int (*lt48_pic_range_cancelled)(void *);
typedef struct {
    uint32_t completed_elements, first_mismatch;
} lt48_pic_range_execution_result;

/* Shared compact host 0044e4e0, restricted to declared compact callbacks and
 * linear logical addresses. The selected procedure still owns all silicon
 * effects and protocol state. Caller must perform hardware cleanup on error.
 * Unlike Windows, failed image I/O and PostConfig cannot report success. */
static inline lt48_status lt48_pic_range_execute(const lt48_pic_range_contract *contract,
    uint32_t first, uint32_t last, int started, lt48_pic_range_invoke invoke,
    lt48_pic_range_image_io image_io, lt48_pic_range_cancelled cancelled,
    void *context, lt48_pic_range_execution_result *result) {
    if (!contract || !invoke || !result || !contract->element_bits ||
        contract->element_bits > 256 || contract->element_role == LT48_PIC_ABSENT_ROLE)
        return LT48_INVALID_ARGUMENT;
    unsigned mode = contract->mode, width = (contract->element_bits + 7) / 8;
    if (!(mode == 1 || mode == 2 || mode == 3 || mode == 4 || mode == 5 || mode == 11) ||
        ((mode >= 1 && mode <= 5) && !image_io)) return LT48_INVALID_ARGUMENT;
    lt48_pic_range_iterator iterator;
    if (!lt48_pic_range_begin(first, last, width, &iterator)) return LT48_INVALID_ARGUMENT;
    if ((uint64_t)iterator.plan.last * width + width > UINT32_MAX) return LT48_INVALID_ARGUMENT;
    lt48_pic_range_execution_result state = {0, UINT32_MAX};
    *result = state;
    unsigned char chunk[8192], data_out[32] = {0}, data_in[32] = {0};
    uint16_t runtime_mode = mode == 5 ? 1 : mode;
    lt48_status st = invoke(context, LT48_PIC_RANGE_MODE, runtime_mode,
        iterator.plan.first, data_out, data_in, width);
    if (st == LT48_OK && !started)
        st = invoke(context, LT48_PIC_RANGE_START, runtime_mode,
            iterator.plan.first, data_out, data_in, width);
    if (st != LT48_OK) return st;
    if (mode == 11) {
        memset(data_out, contract->blank_byte, width);
        unsigned remainder = contract->element_bits & 7;
        if (remainder) data_out[width - 1] &= (1u << remainder) - 1;
    }
    if (contract->pre_role != LT48_PIC_ABSENT_ROLE)
        st = invoke(context, LT48_PIC_RANGE_PRE, contract->pre_role,
            iterator.plan.first, data_out, data_in, width);
    if (st != LT48_OK) return st;
    lt48_pic_range_span span;
    while (st == LT48_OK && lt48_pic_range_next(&iterator, &span)) {
        if (mode >= 2 && mode <= 5) {
            st = image_io(context, 0, span.address * width, chunk, span.byte_count);
            if (st != LT48_OK) break;
        }
        for (uint32_t i = 0; st == LT48_OK && i < span.count; ++i) {
            uint32_t address = span.address + i;
            if (mode >= 2 && mode <= 5) memcpy(data_out, chunk + i * width, width);
            if (mode == 2 || mode == 3) memcpy(data_in, data_out, width);
            st = invoke(context, LT48_PIC_RANGE_ELEMENT, contract->element_role,
                address, data_out, data_in, width);
            /* Source callback failures skip PostConfig. Hardware cleanup is
             * still mandatory in the enclosing operation owner. */
            if (st != LT48_OK) { *result = state; return st; }
            if (mode == 1) memcpy(chunk + i * width, data_in, width);
            else if (memcmp(data_out, data_in, width)) {
                if (state.first_mismatch == UINT32_MAX) state.first_mismatch = address;
                /* Source Compare (5) reports differences and continues. Retain
                 * that traversal, but expose a mismatch status to the caller. */
                if (mode != 5) { st = LT48_DEVICE_MISMATCH; break; }
            }
            ++state.completed_elements;
        }
        if (st == LT48_OK && mode == 1)
            st = image_io(context, 1, span.address * width, chunk, span.byte_count);
        if (st == LT48_OK && cancelled && cancelled(context)) st = LT48_CANCELLED;
    }
    if (contract->post_role != LT48_PIC_ABSENT_ROLE) {
        lt48_status post = invoke(context, LT48_PIC_RANGE_POST, contract->post_role,
            iterator.plan.first + state.completed_elements, data_out, data_in, width);
        if (st == LT48_OK) st = post;
    }
    if (st == LT48_OK && state.first_mismatch != UINT32_MAX) st = LT48_DEVICE_MISMATCH;
    *result = state;
    return st;
}
#endif
