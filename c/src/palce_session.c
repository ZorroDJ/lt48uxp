#include <labtool48/labtool48.h>
#include "internal.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

struct lt48_palce_wave_session {
    const lt48_palce_resources *resources;
    lt48_runtime_transport_session *transport;
    unsigned char buffers[32][32];
    lt48_compile_variable variables[32];
    size_t variable_count;
    uint32_t voltage[4];
    uint8_t mode_buffer;
    int failed;
    int has_device_id;
    unsigned char device_id;
    uint32_t identification_mode;
    int requires_cleanup;
    uint32_t lifecycle_mode;
    lt48_protocol_transport recovery_transport;
    void *transport_context;
};

lt48_status lt48_palce_wave_session_create(
    const lt48_palce_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_palce_wave_session **out_session
) {
    lt48_palce_wave_session *session;
    lt48_status status;
    if (!resources || !out_session) return LT48_INVALID_ARGUMENT;
    *out_session = NULL;
    session = calloc(1, sizeof(*session));
    if (!session) return LT48_NO_MEMORY;
    session->resources = resources;
    const lt48_compile_variable *variables=lt48_palce_resource_variables_internal(resources,&session->variable_count);
    if(session->variable_count>32){free(session);return LT48_FORMAT_ERROR;}
    memcpy(session->variables,variables,session->variable_count*sizeof(*variables));
    status = lt48_palce_resources_buffer(resources, "_BUF_Mode", &session->mode_buffer);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_create(transport, context, &session->transport);
    if (status != LT48_OK) { free(session); return status; }
    session->recovery_transport = *transport;
    session->recovery_transport.cancelled = NULL;
    session->recovery_transport.progress_stage = LT48_EXECUTION_READ_RECOVERY;
    session->transport_context = context;
    *out_session = session;
    return LT48_OK;
}

void lt48_palce_wave_session_close(lt48_palce_wave_session *session) {
    if (!session) return;
    lt48_runtime_transport_session_close(session->transport);
    free(session);
}

lt48_status lt48_palce_wave_session_run(
    lt48_palce_wave_session *session, uint16_t operation, uint32_t mode,
    const lt48_runtime_buffer_seed *overlays, size_t overlay_count,
    lt48_runtime_result **out_result
) {
    unsigned char buffers[32][32];
    lt48_runtime_buffer_seed seeds[31];
    lt48_runtime_machine *machine = NULL;
    lt48_runtime_result *result = NULL;
    size_t count = 0;
    uint32_t voltage[4];
    lt48_status status;
    if (!session || !out_result || (overlay_count && !overlays))
        return LT48_INVALID_ARGUMENT;
    *out_result = NULL;
    if (session->failed) return LT48_SAFETY_REJECTED;
    memcpy(buffers, session->buffers, sizeof(buffers));
    for (size_t index = 0; index < overlay_count; ++index) {
        const lt48_runtime_buffer_seed *seed = &overlays[index];
        if (seed->buffer >= 32 || seed->buffer == session->mode_buffer ||
            seed->size > 32 || (seed->size && !seed->data))
            return LT48_INVALID_ARGUMENT;
        for (size_t prior = 0; prior < index; ++prior)
            if (overlays[prior].buffer == seed->buffer) return LT48_INVALID_ARGUMENT;
        memset(buffers[seed->buffer], 0, 32);
        if (seed->size) memcpy(buffers[seed->buffer], seed->data, seed->size);
    }
    for (uint8_t buffer = 0; buffer < 32; ++buffer) {
        if (buffer == session->mode_buffer) continue;
        seeds[count++] = (lt48_runtime_buffer_seed){buffer, buffers[buffer], 32};
    }
    status = lt48_palce_machine_variables_internal(session->resources, operation,
        mode, session->voltage, seeds, count, session->variables, session->variable_count, &machine);
    /* Unsupported requests issue no I/O and do not invalidate the session. */
    if (status != LT48_OK) return status;
    status = lt48_runtime_transport_session_run(session->transport, machine, &result);
    if (status == LT48_OK) {
        for (uint8_t buffer = 0; buffer < 32; ++buffer) {
            const unsigned char *data;
            size_t size;
            status = lt48_runtime_get_buffer(result, buffer, &data, &size);
            if (status != LT48_OK || size != 32) { status = LT48_RUNTIME_ERROR; break; }
            memcpy(buffers[buffer], data, size);
        }
    }
    if (status == LT48_OK) {
        for (uint8_t channel = 0; channel < 4; ++channel) {
            status = lt48_runtime_get_voltage(result, channel, &voltage[channel]);
            if (status != LT48_OK) break;
        }
    }
    if (status == LT48_OK) {
        for(size_t i=0;i<session->variable_count;i++) {
            status=lt48_runtime_get_variable(result,session->variables[i].variable_id,&session->variables[i]);
            if(status!=LT48_OK){session->failed=1;lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;}
        }
        memcpy(session->buffers, buffers, sizeof(buffers));
        memcpy(session->voltage, voltage, sizeof(voltage));
        if (operation == 2) {
            uint8_t data_id;
            session->has_device_id = 0;
            if (lt48_palce_resources_buffer(session->resources,
                    lt48_palce_resources_definition(session->resources) == 2u ? "_BUF_iD" : "_BUF_DataIn", &data_id) == LT48_OK) {
                int valid = 1;
                for (size_t index = 0; index < lt48_runtime_event_count(result); ++index) {
                    lt48_runtime_event event;
                    if (lt48_runtime_get_event(result, index, &event) != LT48_OK ||
                        event.kind == LT48_RUNTIME_RESULT_LATCH ||
                        (event.kind == LT48_RUNTIME_RETURN_STATUS &&
                         event.argument_count == 1 && event.arguments[0] == 0)) {
                        valid = 0; break;
                    }
                }
                if (valid) {
                    session->device_id = buffers[data_id][0];
                    session->has_device_id = 1;
                    session->identification_mode = mode;
                }
            }
        }
        *out_result = result;
    } else {
        session->failed = 1;
        lt48_runtime_result_close(result);
    }
    lt48_runtime_machine_close(machine);
    return status;
}

lt48_status lt48_palce_wave_session_flush(lt48_palce_wave_session *session) {
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    status = lt48_runtime_transport_session_flush(session->transport);
    if (status != LT48_OK) session->failed = 1;
    return status;
}

typedef struct palce_cell_fuse {
    lt48_fuse_coordinate coordinate;
    size_t fuse;
} palce_cell_fuse;

static size_t palce_fuse_count(const lt48_palce_wave_session *session) {
    size_t count = 0u;
    if (session) (void)lt48_palce_resources_coordinates(session->resources, &count);
    return count;
}

static int compare_cell_fuses(const void *left, const void *right) {
    const palce_cell_fuse *a = left, *b = right;
    if (a->coordinate.row != b->coordinate.row)
        return a->coordinate.row < b->coordinate.row ? -1 : 1;
    if (a->coordinate.column != b->coordinate.column)
        return a->coordinate.column < b->coordinate.column ? -1 : 1;
    return a->fuse < b->fuse ? -1 : a->fuse != b->fuse;
}

static lt48_status transfer_fuses(
    lt48_palce_wave_session *session, uint32_t mode,
    const unsigned char *input, unsigned char *out_fuses, size_t capacity, size_t *out_completed_cells
) {
    const lt48_fuse_coordinate *coordinates;
    palce_cell_fuse *ordered;
    unsigned char image[5828] = {0};
    uint8_t ids[6];
    const char *names[] = {"_BUF_PLine", "_BUF_ILine", "_BUF_DataMask",
                          "_BUF_FuseType", "_BUF_DataIn", "_BUF_DataOut"};
    size_t count, first = 0, width = 0;
    lt48_status status = LT48_OK;
    if (!session || (!input && !out_fuses) || !out_completed_cells)
        return LT48_INVALID_ARGUMENT;
    *out_completed_cells = 0;
    coordinates = lt48_palce_resources_coordinates(session->resources, &count);
    if (!coordinates || !count || count > sizeof(image)) return LT48_FORMAT_ERROR;
    if (capacity < count) return LT48_BUFFER_TOO_SMALL;
    if (input ? mode != 2 : (mode != 1 && mode != 3 && mode != 4 && mode != 11))
        return LT48_UNSUPPORTED;
    if (input)
        for (size_t fuse = 0; fuse < count; ++fuse)
            if (input[fuse] > 1) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    coordinates = lt48_palce_resources_coordinates(session->resources, &count);

    for (size_t index = 0; index < 6; ++index) {
        status = lt48_palce_resources_buffer(session->resources, names[index], &ids[index]);
        if (status != LT48_OK) return status;
    }
    ordered = calloc(count, sizeof(*ordered));
    if (!ordered) return LT48_NO_MEMORY;
    for (size_t fuse = 0; fuse < count; ++fuse)
        ordered[fuse] = (palce_cell_fuse){coordinates[fuse], fuse};
    qsort(ordered, count, sizeof(*ordered), compare_cell_fuses);
    /* Validate the whole layout before issuing any cell operation. */
    for (size_t index = 0; index < count; ++index) {
        const lt48_fuse_coordinate *c = &ordered[index].coordinate;
        if (width < c->bit / 8u + 1u) width = c->bit / 8u + 1u;
        if (c->bit >= 256 || c->fuse_type > 255 ||
            (index && c->row == ordered[index-1].coordinate.row &&
             c->column == ordered[index-1].coordinate.column &&
             c->fuse_type != ordered[index-1].coordinate.fuse_type)) {
            free(ordered); return LT48_FORMAT_ERROR;
        }
    }
    while (first < count) {
        const lt48_fuse_coordinate *c = &ordered[first].coordinate;
        size_t end = first;
        unsigned char row[2] = {(unsigned char)c->row, (unsigned char)(c->row >> 8)};
        unsigned char column[2] = {(unsigned char)c->column, (unsigned char)(c->column >> 8)};
        unsigned char type = (unsigned char)c->fuse_type;
        unsigned char mask[32] = {0};
        unsigned char desired[32] = {0};
        lt48_runtime_buffer_seed seeds[] = {
            {ids[0], row, 2}, {ids[1], column, 2},
            {ids[2], mask, width}, {ids[3], &type, 1},
            {ids[4], desired, width}, {ids[5], desired, width}};
        lt48_runtime_result *result = NULL;
        const unsigned char *physical;
        size_t size;
        while (end < count && ordered[end].coordinate.row == c->row &&
               ordered[end].coordinate.column == c->column) {
            unsigned bit = ordered[end].coordinate.bit;
            mask[bit / 8] |= 1u << (bit % 8);
            if (input && input[ordered[end].fuse]) desired[bit / 8] |= 1u << (bit % 8);
            ++end;
        }
        status = lt48_palce_wave_session_run(session, input ? 5 : 4, mode, seeds, input ? 6 : 4, &result);
        if (status == LT48_OK) {
            for (size_t event = 0; event < lt48_runtime_event_count(result); ++event) {
                lt48_runtime_event value;
                status = lt48_runtime_get_event(result, event, &value);
                if (status != LT48_OK) break;
                if (value.kind == LT48_RUNTIME_RESULT_LATCH ||
                    (value.kind == LT48_RUNTIME_RETURN_STATUS &&
                     value.argument_count == 1 && value.arguments[0] == 0)) {
                    status = LT48_RUNTIME_ERROR; break;
                }
            }
        }
        if (status == LT48_OK && !input)
            status = lt48_runtime_get_buffer(result, ids[4], &physical, &size);
        if (status == LT48_OK && !input && size < width) status = LT48_RUNTIME_ERROR;
        if (status == LT48_OK) {
            for (size_t index = first; !input && index < end; ++index) {
                unsigned bit = ordered[index].coordinate.bit;
                image[ordered[index].fuse] = (physical[bit / 8] >> (bit % 8)) & 1u;
            }
            ++*out_completed_cells;
        }
        lt48_runtime_result_close(result);
        if (status != LT48_OK) { session->failed = 1; break; }
        first = end;
    }
    if (status == LT48_OK) status = lt48_palce_wave_session_flush(session);
    if (status == LT48_OK && !input) memcpy(out_fuses, image, count);
    free(ordered);
    return status;
}

lt48_status lt48_palce_wave_session_read_fuses(
    lt48_palce_wave_session *session, uint32_t mode,
    unsigned char *out_fuses, size_t capacity, size_t *out_completed_cells
) {
    return transfer_fuses(session, mode, NULL, out_fuses, capacity, out_completed_cells);
}

lt48_status lt48_palce_wave_session_program_fuses(
    lt48_palce_wave_session *session, const unsigned char *fuses,
    size_t count, size_t *out_completed_cells
) {
    if (!fuses || count != palce_fuse_count(session)) return LT48_INVALID_ARGUMENT;
    return transfer_fuses(session, 2, fuses, NULL, count, out_completed_cells);
}

static lt48_status compare_fuses(
    lt48_palce_wave_session *session, uint32_t mode,
    const unsigned char *expected, lt48_palce_fuse_comparison *out_comparison
) {
    unsigned char actual[5828];
    size_t count = palce_fuse_count(session);
    lt48_status status;
    if (!out_comparison) return LT48_INVALID_ARGUMENT;
    memset(out_comparison, 0, sizeof(*out_comparison));
    out_comparison->first_mismatch = SIZE_MAX;
    if (!session || !count || count > sizeof(actual)) return LT48_INVALID_ARGUMENT;
    if (expected)
        for (size_t fuse = 0; fuse < count; ++fuse)
            if (expected[fuse] > 1) return LT48_INVALID_ARGUMENT;
    status = lt48_palce_wave_session_read_fuses(session, mode, actual,
        count, &out_comparison->completed_cells);
    if (status != LT48_OK) return status;
    for (size_t fuse = 0; fuse < count; ++fuse) {
        unsigned char desired = expected ? expected[fuse] : 1;
        if (actual[fuse] == desired) continue;
        if (!out_comparison->mismatch_count) {
            out_comparison->first_mismatch = fuse;
            out_comparison->expected = desired;
            out_comparison->actual = actual[fuse];
        }
        ++out_comparison->mismatch_count;
    }
    out_comparison->has_result = 1;
    out_comparison->matched = out_comparison->mismatch_count == 0;
    return LT48_OK;
}

lt48_status lt48_palce_wave_session_verify_fuses(
    lt48_palce_wave_session *session, uint32_t mode,
    const unsigned char *expected, size_t count,
    lt48_palce_fuse_comparison *out_comparison
) {
    if (!out_comparison) return LT48_INVALID_ARGUMENT;
    memset(out_comparison, 0, sizeof(*out_comparison));
    out_comparison->first_mismatch = SIZE_MAX;
    if (!expected || count != palce_fuse_count(session) || (mode != 3 && mode != 4))
        return LT48_INVALID_ARGUMENT;
    return compare_fuses(session, mode, expected, out_comparison);
}

lt48_status lt48_palce_wave_session_blank_check(
    lt48_palce_wave_session *session,
    lt48_palce_fuse_comparison *out_comparison
) {
    return compare_fuses(session, 11, NULL, out_comparison);
}

lt48_status lt48_palce_wave_session_erase(lt48_palce_wave_session *session) {
    lt48_runtime_result *result = NULL;
    lt48_runtime_buffer_seed seed;
    uint8_t data_id;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (!session->has_device_id || (session->identification_mode != 7 && session->identification_mode != 10) ||
        session->buffers[session->mode_buffer][0] != session->identification_mode)
        return LT48_SAFETY_REJECTED;
    /* Audited W6 ID 80 branch consists of four SubErase calls. */
    if (lt48_palce_resources_definition(session->resources) == 0u) switch (session->device_id) {
        case 0x40: case 0xc0: case 0xe0: case 0x90: case 0xa0: case 0x80: break;
        default: return LT48_DEVICE_MISMATCH;
    }
    status = lt48_palce_resources_buffer(session->resources, "_BUF_DataIn", &data_id);
    if (status != LT48_OK) return status;
    seed = (lt48_runtime_buffer_seed){data_id, &session->device_id, 1};
    int suberase = lt48_palce_resources_definition(session->resources) == 0u && session->device_id == 0x80;
    for (unsigned pulse = 0; pulse < (suberase ? 4u : 1u); ++pulse) {
        status = lt48_palce_wave_session_run(session,
            suberase ? 10 : 6, session->identification_mode,
            pulse == 0 ? &seed : NULL, pulse == 0 ? 1 : 0, &result);
        if (status == LT48_OK) {
            for (size_t index = 0; index < lt48_runtime_event_count(result); ++index) {
                lt48_runtime_event event;
                status = lt48_runtime_get_event(result, index, &event);
                if (status != LT48_OK) break;
                if (event.kind == LT48_RUNTIME_RESULT_LATCH ||
                    (event.kind == LT48_RUNTIME_RETURN_STATUS &&
                     event.argument_count == 1 && event.arguments[0] == 0)) {
                    status = LT48_RUNTIME_ERROR; break;
                }
            }
        }
        lt48_runtime_result_close(result);
        result = NULL;
        if (status != LT48_OK) break;
    }
    if (status == LT48_OK) status = lt48_palce_wave_session_flush(session);
    if (status != LT48_OK) session->failed = 1;
    return status;
}

lt48_status lt48_palce_wave_session_reset_frame(lt48_palce_wave_session *session) {
    uint32_t mask;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    mask = lt48_palce_resources_reset_buffer_mask(session->resources);
    for (uint8_t buffer = 0; buffer < 32; ++buffer)
        if (mask & (UINT32_C(1) << buffer)) memset(session->buffers[buffer], 0, 32);
    session->has_device_id = 0;
    return LT48_OK;
}

int lt48_palce_wave_session_requires_cleanup(const lt48_palce_wave_session *session) {
    return session ? session->requires_cleanup : 0;
}

lt48_status lt48_palce_wave_session_enter_mode(
    lt48_palce_wave_session *session, uint32_t mode, unsigned char *out_id
) {
    const unsigned char controls[] = {0x56, 0x0a, 1, 0x40, 1};
    lt48_runtime_result *result = NULL;
    lt48_status status;
    if (!session || !out_id) return LT48_INVALID_ARGUMENT;
    *out_id = 0;
    if (mode != 1 && mode != 2 && mode != 3 && mode != 4 && mode != 7 && mode != 8 && mode != 10 && mode != 11)
        return LT48_UNSUPPORTED;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (session->requires_cleanup) return LT48_BUSY;
    status = lt48_runtime_frame_set_mode(session->buffers,
        lt48_palce_resources_buffer_count(session->resources),
        lt48_palce_resources_reset_buffer_mask(session->resources),
        session->mode_buffer, (uint16_t)mode, 0, 0);
    if (status != LT48_OK) return status;
    session->has_device_id = 0;
    /* Once POWERON can issue I/O, cleanup is required even if entry fails. */
    session->requires_cleanup = 1;
    session->lifecycle_mode = mode;
    /* Re-entering after idle must clear the FX2 expired-watchdog latch before
     * the first controller queue. Original dispatch does this at wave entry:
     * 21/10 clears bit77, installs the timer and restarts it (FX2 CODE:0a99).
     * A fresh transport session deliberately has no reusable timeout cache. */
    status = lt48_runtime_transport_session_wave_watchdog(session->transport, 10000u, 1);
    if (status == LT48_OK)
        status = lt48_palce_wave_session_run(session, 0, mode, NULL, 0, &result);
    lt48_runtime_result_close(result);
    result = NULL;
    for (unsigned wait = 0; status == LT48_OK && wait < 3; ++wait)
        status = lt48_runtime_transport_session_status(session->transport, 1);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_controller_write(session->transport,
            controls, sizeof(controls));
    if (status == LT48_OK)
        status = lt48_palce_wave_session_run(session, 2, mode, NULL, 0, &result);
    lt48_runtime_result_close(result);
    if (status == LT48_OK && !session->has_device_id) status = LT48_DEVICE_MISMATCH;
    if (status == LT48_OK) *out_id = session->device_id;
    else session->failed = 1;
    return status;
}

static lt48_status palce_wave_result_status(const lt48_runtime_result *result) {
    for (size_t i = 0u; i < lt48_runtime_event_count(result); ++i) {
        lt48_runtime_event event;
        lt48_status status = lt48_runtime_get_event(result, i, &event);
        if (status != LT48_OK) return status;
        if (event.kind == LT48_RUNTIME_RESULT_LATCH ||
            (event.kind == LT48_RUNTIME_RETURN_STATUS &&
             event.argument_count == 1u && event.arguments[0] == 0u))
            return LT48_RUNTIME_ERROR;
    }
    return LT48_OK;
}

lt48_status lt48_palce_wave_session_finish_mode(lt48_palce_wave_session *session) {
    const unsigned char finalize[] = {0x49, 0xaa, 0x55};
    lt48_runtime_result *result = NULL;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (!session->requires_cleanup) return LT48_INVALID_ARGUMENT;
    /* The row worker's successful exit calls DEVRESET before the operation
     * reset wrapper's completion query, POWEROFF and controller finalizer. */
    status = lt48_palce_wave_session_run(session, 3, session->lifecycle_mode,
        NULL, 0, &result);
    if (status == LT48_OK) status = palce_wave_result_status(result);
    lt48_runtime_result_close(result);
    result = NULL;
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_status(session->transport, 1);
    if (status == LT48_OK)
        status = lt48_palce_wave_session_run(session, 1, session->lifecycle_mode,
            NULL, 0, &result);
    if (status == LT48_OK) status = palce_wave_result_status(result);
    lt48_runtime_result_close(result);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_controller_write(session->transport,
            finalize, sizeof(finalize));
    if (status == LT48_OK) status = lt48_palce_wave_session_flush(session);
    if (status == LT48_OK) {
        session->requires_cleanup = 0;
        session->has_device_id = 0;
    } else session->failed = 1;
    return status;
}

lt48_status lt48_palce_wave_session_switch_mode(
    lt48_palce_wave_session *session, uint32_t mode, unsigned char *out_id
) {
    lt48_runtime_result *result = NULL;
    lt48_status status;
    if (!session || !out_id) return LT48_INVALID_ARGUMENT;
    *out_id = 0;
    if (mode != 1 && mode != 2 && mode != 3 && mode != 4 && mode != 7 && mode != 8 && mode != 10 && mode != 11)
        return LT48_UNSUPPORTED;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (!session->requires_cleanup) return LT48_INVALID_ARGUMENT;
    /* 45a060 ends a successful row stage with DEVRESET. The next invocation
     * runs the mode setter and DEVINIT, bypassing POWERON while +90 is set. */
    status = lt48_palce_wave_session_run(session, 3, session->lifecycle_mode,
        NULL, 0, &result);
    lt48_runtime_result_close(result);
    result = NULL;
    if (status == LT48_OK) status = lt48_palce_wave_session_reset_frame(session);
    if (status == LT48_OK) {
        session->lifecycle_mode = mode;
        status = lt48_palce_wave_session_run(session, 2, mode, NULL, 0, &result);
    }
    lt48_runtime_result_close(result);
    if (status == LT48_OK && !session->has_device_id) status = LT48_DEVICE_MISMATCH;
    if (status == LT48_OK) *out_id = session->device_id;
    else session->failed = 1;
    return status;
}

static lt48_status palce_recovery_step(lt48_palce_wave_session *session, size_t phase) {
    lt48_runtime_transport_session *transport = NULL;
    lt48_runtime_machine *machine = NULL;
    lt48_runtime_result *result = NULL;
    lt48_runtime_buffer_seed seeds[31];
    size_t seed_count = 0;
    lt48_status status = lt48_runtime_transport_session_create(
        &session->recovery_transport, session->transport_context, &transport);
    if (status != LT48_OK) return status;
    if (phase == 0 || phase == 2) {
        for (uint8_t buffer = 0; buffer < 32; ++buffer)
            if (buffer != session->mode_buffer)
                seeds[seed_count++] = (lt48_runtime_buffer_seed){
                    buffer, session->buffers[buffer], 32};
        status = lt48_palce_resources_create_machine(session->resources,
            phase == 0 ? 3 : 1, session->lifecycle_mode,
            session->voltage, seeds, seed_count, &machine);
        if (status == LT48_OK)
            status = lt48_runtime_transport_session_run(transport, machine, &result);
        if (status == LT48_OK) {
            for (size_t index = 0; index < lt48_runtime_event_count(result); ++index) {
                lt48_runtime_event event;
                status = lt48_runtime_get_event(result, index, &event);
                if (status != LT48_OK) break;
                if (event.kind == LT48_RUNTIME_RESULT_LATCH ||
                    (event.kind == LT48_RUNTIME_RETURN_STATUS &&
                     event.argument_count == 1 && event.arguments[0] == 0)) {
                    status = LT48_RUNTIME_ERROR;
                    break;
                }
            }
        }
    } else if (phase == 1) {
        status = lt48_runtime_transport_session_status(transport, 1);
    } else {
        unsigned char bytes[3];
        size_t length = sizeof(bytes);
        if (phase >= 3 && phase <= 6) {
            bytes[0] = 0x10;
            bytes[1] = (unsigned char)(6 - phase);
            bytes[2] = 0;
        } else if (phase == 7) {
            bytes[0] = 0x49; bytes[1] = 0xaa; bytes[2] = 0x55;
        } else {
            bytes[0] = 0x40; bytes[1] = 2;
            length = 2;
        }
        status = lt48_runtime_transport_session_controller_write(transport, bytes, length);
    }
    if (status == LT48_OK) status = lt48_runtime_transport_session_flush(transport);
    lt48_runtime_result_close(result);
    lt48_runtime_machine_close(machine);
    lt48_runtime_transport_session_close(transport);
    return status;
}

lt48_status lt48_palce_wave_session_recover(
    lt48_palce_wave_session *session, lt48_palce_recovery_result *out_result
) {
    lt48_status first_error = LT48_OK;
    if (!out_result) return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    if (!session || !session->requires_cleanup) return LT48_INVALID_ARGUMENT;
    /* Cleanup never makes a failed/aborted forward frame reusable. Every
     * phase gets an independent queue so an earlier failure cannot suppress
     * the zero-rail or finalization attempts or replay pending program data. */
    session->failed = 1;
    session->has_device_id = 0;
    for (size_t phase = 0; phase < LT48_PALCE_RECOVERY_PHASE_COUNT; ++phase) {
        lt48_status status = palce_recovery_step(session, phase);
        out_result->phases[phase] = status;
        ++out_result->attempted_phases;
        if (first_error == LT48_OK && status != LT48_OK) first_error = status;
    }
    if (first_error == LT48_OK) {
        memset(session->voltage, 0, sizeof(session->voltage));
        session->requires_cleanup = 0;
        out_result->completed = 1;
    }
    return first_error;
}

lt48_status lt48_palce_execute_operation(
    const lt48_palce_resources *resources, const lt48_protocol_transport *transport,
    void *context, lt48_operation_kind operation,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_palce_operation_result *out_result
) {
    lt48_palce_wave_session *session = NULL;
    lt48_palce_recovery_result recovery;
    unsigned char temporary[5828], id;
    size_t fuse_count = 0u;
    uint32_t mode;
    lt48_status status;
    if (!out_result) return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    out_result->operation_status = LT48_INVALID_ARGUMENT;
    out_result->comparison.first_mismatch = SIZE_MAX;
    if (!resources || !transport) return LT48_INVALID_ARGUMENT;
    (void)lt48_palce_resources_coordinates(resources, &fuse_count);
    if (!fuse_count || fuse_count > sizeof(temporary)) return LT48_FORMAT_ERROR;
    switch (operation) {
        case LT48_OPERATION_READ: mode = 1; break;
        case LT48_OPERATION_BLANK_CHECK: mode = 11; break;
        case LT48_OPERATION_VERIFY: mode = 4; break;
        case LT48_OPERATION_PROGRAM: mode = 11; break;
        case LT48_OPERATION_ERASE: mode = 7; break;
        case LT48_OPERATION_SECURE: mode = 8; break;
        default: out_result->operation_status = LT48_UNSUPPORTED; return LT48_UNSUPPORTED;
    }
    if (operation == LT48_OPERATION_READ && (!read_fuses || read_capacity < fuse_count))
        return LT48_INVALID_ARGUMENT;
    if (operation == LT48_OPERATION_VERIFY || operation == LT48_OPERATION_PROGRAM) {
        if (!input_fuses || input_count != fuse_count) return LT48_INVALID_ARGUMENT;
        for (size_t i = 0; i < input_count; ++i)
            if (input_fuses[i] > 1) return LT48_INVALID_ARGUMENT;
    } else if (input_fuses || input_count) return LT48_INVALID_ARGUMENT;
    status = lt48_palce_wave_session_create(resources, transport, context, &session);
    if (status == LT48_OK) status = lt48_palce_wave_session_enter_mode(session, mode, &id);
    if (status == LT48_OK) {
        if (operation == LT48_OPERATION_READ)
            status = lt48_palce_wave_session_read_fuses(session, 1, temporary,
                fuse_count, &out_result->completed_cells);
        else if (operation == LT48_OPERATION_VERIFY)
            status = lt48_palce_wave_session_verify_fuses(session, 4, input_fuses,
                input_count, &out_result->comparison);
        else if (operation == LT48_OPERATION_SECURE) {
            lt48_runtime_result *secure_result = NULL;
            status = lt48_palce_wave_session_run(session, 7u, 8u, NULL, 0u,
                                                  &secure_result);
            if (status == LT48_OK) status = palce_wave_result_status(secure_result);
            lt48_runtime_result_close(secure_result);
            if (status == LT48_OK) status = lt48_palce_wave_session_flush(session);
            if (status == LT48_OK) out_result->secured = 1;
            else session->failed = 1;
        }
        else if (operation == LT48_OPERATION_ERASE) {
            status = lt48_palce_wave_session_erase(session);
            if (status == LT48_OK) {
                out_result->erased = 1;
                status = lt48_palce_wave_session_switch_mode(session, 11, &id);
            }
            if (status == LT48_OK)
                status = lt48_palce_wave_session_blank_check(session, &out_result->comparison);
        } else {
            status = lt48_palce_wave_session_blank_check(session, &out_result->comparison);
            if (status == LT48_OK && operation == LT48_OPERATION_PROGRAM && out_result->comparison.matched) {
                status = lt48_palce_wave_session_switch_mode(session, 2, &id);
                if (status == LT48_OK) status = lt48_palce_wave_session_program_fuses(session,
                    input_fuses, input_count, &out_result->completed_cells);
                if (status == LT48_OK) {
                    out_result->programmed = 1;
                    /* Do not retain a preprogram blank result as verification. */
                    memset(&out_result->comparison, 0, sizeof(out_result->comparison));
                    out_result->comparison.first_mismatch = SIZE_MAX;
                    status = lt48_palce_wave_session_switch_mode(session, 4, &id);
                }
                if (status == LT48_OK) status = lt48_palce_wave_session_verify_fuses(session,
                    4, input_fuses, input_count, &out_result->comparison);
            }
        }
    }
    out_result->operation_status = status;
    if (session && lt48_palce_wave_session_requires_cleanup(session)) {
        if (status == LT48_OK) {
            out_result->cleanup_status = lt48_palce_wave_session_finish_mode(session);
            if (out_result->cleanup_status != LT48_OK) {
                /* Preserve normal-finalization failure even if recovery succeeds. */
                lt48_status recovery_status = lt48_palce_wave_session_recover(session, &recovery);
                out_result->cleanup_completed = recovery_status == LT48_OK && recovery.completed;
            } else out_result->cleanup_completed = 1;
        } else {
            out_result->cleanup_status = lt48_palce_wave_session_recover(session, &recovery);
            out_result->cleanup_completed = out_result->cleanup_status == LT48_OK && recovery.completed;
        }
    }
    if (status == LT48_OK) status = out_result->cleanup_status;
    if (status == LT48_OK && out_result->cleanup_completed) {
        /* Original UI result command: 4=Good, 2=Error; both clear Busy.
         * Report success only after power-off and controller finalization. */
        const unsigned char indicator[] = {0x40u,
            out_result->comparison.has_result && !out_result->comparison.matched ? 2u : 4u};
        status = lt48_runtime_transport_session_controller_write(
            session->transport, indicator, sizeof(indicator));
        if (status == LT48_OK) status = lt48_palce_wave_session_flush(session);
        if (status != LT48_OK) {
            out_result->cleanup_status = status;
            (void)lt48_runtime_transport_error_indicator_internal(transport,context);
        }
    }
    if (status == LT48_OK && operation == LT48_OPERATION_READ)
        memcpy(read_fuses, temporary, fuse_count);
    lt48_palce_wave_session_close(session);
    return status;
}
