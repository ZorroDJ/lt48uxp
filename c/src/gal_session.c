#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdlib.h>
#include <string.h>

#define GAL_MAX_BUFFERS 81u
#define GAL_MAX_WAVES 64u

struct lt48_gal_wave_session {
    const lt48_gal_resources *resources;
    const lt48_protocol_transport *transport_api;
    void *transport_context;
    lt48_runtime_transport_session *transport;
    unsigned char buffers[GAL_MAX_BUFFERS][32];
    lt48_compile_variable *variables;
    size_t variable_count;
    uint32_t voltage[4];
    uint8_t buffer_count;
    uint8_t mode_buffer;
    uint8_t data_in;
    uint8_t data_out;
    int failed;
    int requires_cleanup;
    uint32_t lifecycle_mode;
    lt48_fuse_image *program_image;
    size_t verify_voltage_index,verify_margin_index;
    uint16_t verify_values[2];
    unsigned verify_passes;
    int verify_custom;

};

static int gal_mode_supported(uint32_t mode);

static lt48_status gal_result_check(const lt48_runtime_result *result) {
    if (!result) return LT48_INVALID_ARGUMENT;
    for (size_t index = 0u; index < lt48_runtime_event_count(result); ++index) {
        lt48_runtime_event event;
        lt48_status status = lt48_runtime_get_event(result, index, &event);
        if (status != LT48_OK) return status;
        if (event.kind == LT48_RUNTIME_RESULT_LATCH ||
            (event.kind == LT48_RUNTIME_RETURN_STATUS &&
             event.argument_count == 1u && event.arguments[0] == 0u))
            return LT48_RUNTIME_ERROR;
    }
    return LT48_OK;
}

lt48_status lt48_gal_wave_session_create(
    const lt48_gal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_gal_wave_session **out_session
) {
    lt48_gal_wave_session *session;
    const lt48_compile_variable *variables;
    lt48_status status;
    if (!resources || !transport || !out_session) return LT48_INVALID_ARGUMENT;
    *out_session = NULL;
    session = calloc(1u, sizeof(*session));
    if (!session) return LT48_NO_MEMORY;
    session->verify_voltage_index=SIZE_MAX;session->verify_margin_index=SIZE_MAX;session->verify_passes=1;
    session->resources = resources;
    session->transport_api = transport;
    session->transport_context = context;
    session->buffer_count = lt48_gal_resources_buffer_count(resources);
    if (!session->buffer_count || session->buffer_count > GAL_MAX_BUFFERS) {
        free(session);
        return LT48_UNSUPPORTED;
    }
    status = lt48_gal_resources_buffer(resources, "_BUF_Mode",
                                        &session->mode_buffer);
    if (status == LT48_OK)
        status = lt48_gal_resources_buffer(resources, "_BUF_DataIn",
                                            &session->data_in);
    if (status == LT48_OK)
        status = lt48_gal_resources_buffer(resources, "_BUF_DataOut",
                                            &session->data_out);
    variables = lt48_gal_resources_variables(resources, &session->variable_count);
    if (status == LT48_OK && session->variable_count != 0u && !variables)
        status = LT48_FORMAT_ERROR;
    if (status == LT48_OK && session->variable_count != 0u) {
        session->variables = calloc(session->variable_count,
                                     sizeof(*session->variables));
        if (!session->variables) status = LT48_NO_MEMORY;
        else memcpy(session->variables, variables,
                    session->variable_count * sizeof(*variables));
    }
    if(status==LT48_OK){
        uint8_t vcc,margin,custom[2];int has_custom=0;
        if(lt48_gal_resources_buffer(resources,"_VAR_VCC",&vcc)!=LT48_OK)vcc=255;
        if(lt48_gal_resources_buffer(resources,"_VAR_MarginVer",&margin)!=LT48_OK)margin=255;
        for(unsigned i=0;i<2;i++){
            const char *name=i?"_VAR_VerifyVCC1":"_VAR_VerifyVCC0";
            if(lt48_gal_resources_buffer(resources,name,custom+i)!=LT48_OK)custom[i]=255;
        }
        unsigned n=0;
        for(size_t i=0;i<session->variable_count;i++){
            const lt48_compile_variable *v=session->variables+i;
            if(v->variable_id==vcc)session->verify_voltage_index=i;
            if(v->variable_id==margin)session->verify_margin_index=i;
        }
        for(unsigned j=0;j<2;j++)for(size_t i=0;i<session->variable_count;i++)if(session->variables[i].variable_id==custom[j]){
            has_custom=1;if(session->variables[i].value)session->verify_values[n++]=session->variables[i].value;
        }
        session->verify_custom=has_custom;
        session->verify_passes=has_custom?n:session->verify_margin_index!=SIZE_MAX?2u:1u;
        if(has_custom && (!n||session->verify_voltage_index==SIZE_MAX))status=LT48_SAFETY_REJECTED;
    }
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_create(
            transport, context, &session->transport);
    if (status != LT48_OK) {
        free(session->variables);
        free(session);
        return status;
    }
    *out_session = session;
    return LT48_OK;
}

void lt48_gal_wave_session_close(lt48_gal_wave_session *session) {
    if (!session) return;
    lt48_fuse_image_destroy(session->program_image);
    lt48_runtime_transport_session_close(session->transport);
    free(session->variables);
    free(session);
}

static lt48_status gal_run_wave(
    lt48_gal_wave_session *session, uint16_t operation, uint32_t mode,
    const lt48_runtime_buffer_seed *overlays, size_t overlay_count,
    lt48_runtime_result **out_result
) {
    unsigned char buffers[GAL_MAX_BUFFERS][32];
    lt48_runtime_buffer_seed seeds[GAL_MAX_BUFFERS - 1u];
    lt48_runtime_machine *machine = NULL;
    lt48_runtime_result *result = NULL;
    size_t seed_count = 0u;
    lt48_status status;
    if (!session || !out_result || (overlay_count && !overlays))
        return LT48_INVALID_ARGUMENT;
    *out_result = NULL;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (overlay_count > session->buffer_count) return LT48_INVALID_ARGUMENT;
    memcpy(buffers, session->buffers, sizeof(buffers));
    for (size_t index = 0u; index < overlay_count; ++index) {
        const lt48_runtime_buffer_seed *seed = &overlays[index];
        if (seed->buffer >= session->buffer_count ||
            seed->buffer == session->mode_buffer || seed->size > 32u ||
            (seed->size && !seed->data)) return LT48_INVALID_ARGUMENT;
        for (size_t prior = 0u; prior < index; ++prior)
            if (overlays[prior].buffer == seed->buffer)
                return LT48_INVALID_ARGUMENT;
        memset(buffers[seed->buffer], 0, 32u);
        if (seed->size) memcpy(buffers[seed->buffer], seed->data, seed->size);
    }
    for (uint8_t buffer = 0u; buffer < session->buffer_count; ++buffer) {
        if (buffer == session->mode_buffer) continue;
        seeds[seed_count++] = (lt48_runtime_buffer_seed){
            buffer, buffers[buffer], 32u};
    }
    status = lt48_gal_resources_image_machine_internal(
        session->resources, operation, mode, session->voltage,
        session->variables, session->variable_count, seeds, seed_count,
        lt48_fuse_image_view(session->program_image), &machine);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_run(
            session->transport, machine, &result);
    if (status == LT48_OK)
        status = gal_result_check(result);
    if (status == LT48_OK) {
        for (uint8_t buffer = 0u; buffer < session->buffer_count; ++buffer) {
            const unsigned char *data;
            size_t size;
            status = lt48_runtime_get_buffer(
                result, buffer, &data, &size);
            if (status != LT48_OK || size != 32u) {
                status = LT48_RUNTIME_ERROR;
                break;
            }
            memcpy(buffers[buffer], data, 32u);
        }
    }
    if (status == LT48_OK) {
        for (size_t channel = 0u; channel < 4u; ++channel) {
            status = lt48_runtime_get_voltage(
                result, (uint8_t)channel, &session->voltage[channel]);
            if (status != LT48_OK) break;
        }
    }
    if (status == LT48_OK && session->variable_count != 0u) {
        for (size_t index = 0u; index < session->variable_count; ++index) {
            lt48_compile_variable updated;
            status = lt48_runtime_get_variable(
                result, session->variables[index].variable_id, &updated);
            if (status == LT48_OK) session->variables[index] = updated;
            else if (status == LT48_NOT_FOUND) status = LT48_OK;
            else break;
        }
    }
    if (status == LT48_OK) {
        memcpy(session->buffers, buffers, sizeof(buffers));
        *out_result = result;
        result = NULL;
    } else {
        session->failed = 1;
    }
    lt48_runtime_result_close(result);
    lt48_runtime_machine_close(machine);
    if (status != LT48_OK) return status;
    return LT48_OK;
}

static lt48_status gal_reset_frame(lt48_gal_wave_session *session) {
    lt48_runtime_frame_profile profile;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    status = lt48_gal_resources_frame_profile(session->resources, &profile);
    if (status != LT48_OK) return status;
    status = lt48_runtime_frame_set_mode(
        session->buffers, (uint8_t)lt48_gal_source_buffer_count_internal(session->resources), profile.reset_mask,
        profile.mode_buffer, (uint16_t)session->lifecycle_mode, 0, 0);
    return status;
}

lt48_status lt48_gal_wave_session_enter_mode(
    lt48_gal_wave_session *session, uint32_t mode
) {
    static const unsigned char controls[] = {0x56u, 0x0au, 1u, 0x40u, 1u};
    lt48_runtime_result *result = NULL;
    lt48_runtime_frame_profile profile;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if(mode==2u && lt48_gal_resources_requires_image_internal(session->resources) && !session->program_image)
        return LT48_SAFETY_REJECTED;
    if (mode > UINT16_MAX || !gal_mode_supported(mode) ||
        session->failed || session->requires_cleanup)
        return session->failed ? LT48_SAFETY_REJECTED : LT48_BUSY;
    status = lt48_gal_resources_frame_profile(session->resources, &profile);
    if (status == LT48_OK)
        status = lt48_runtime_frame_set_mode(
            session->buffers, (uint8_t)lt48_gal_source_buffer_count_internal(session->resources), profile.reset_mask,
            profile.mode_buffer, (uint16_t)mode, 0, 0);
    if (status == LT48_OK) {
        session->lifecycle_mode = mode;
        session->requires_cleanup = 1;
        status = gal_run_wave(session, 0u, mode, NULL, 0u, &result);
    }
    lt48_runtime_result_close(result);
    result = NULL;
    for (unsigned wait = 0u; status == LT48_OK && wait < 3u; ++wait)
        status = lt48_runtime_transport_session_status(session->transport, 1);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_controller_write(
            session->transport, controls, sizeof(controls));
    if (status == LT48_OK)
        status = gal_run_wave(session, 2u, mode, NULL, 0u, &result);
    lt48_runtime_result_close(result);
    if (status != LT48_OK) session->failed = 1;
    return status;
}

static lt48_status gal_switch_mode(
    lt48_gal_wave_session *session, uint32_t mode,int verify_pass
) {
    lt48_runtime_result *result = NULL;
    lt48_status status;
    if (!session || mode > UINT16_MAX || !gal_mode_supported(mode))
        return LT48_INVALID_ARGUMENT;
    if(mode==2u && lt48_gal_resources_requires_image_internal(session->resources) && !session->program_image)
        return LT48_SAFETY_REJECTED;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (!session->requires_cleanup) return LT48_INVALID_ARGUMENT;
    status = gal_run_wave(session, 3u, session->lifecycle_mode,
                          NULL, 0u, &result);
    lt48_runtime_result_close(result);
    result = NULL;
    if (status == LT48_OK) {
        if(verify_pass>=0){
            if(session->verify_custom)session->variables[session->verify_voltage_index].value=session->verify_values[verify_pass];
            else if(session->verify_margin_index!=SIZE_MAX)session->variables[session->verify_margin_index].value=(uint16_t)verify_pass;
        }
        session->lifecycle_mode = mode;
        status = gal_reset_frame(session);
    }
    if (status == LT48_OK)
        status = gal_run_wave(session, 2u, mode, NULL, 0u, &result);
    lt48_runtime_result_close(result);
    if (status != LT48_OK) session->failed = 1;
    return status;
}

lt48_status lt48_gal_wave_session_switch_mode(lt48_gal_wave_session *session,uint32_t mode){return gal_switch_mode(session,mode,-1);}

lt48_status lt48_gal_wave_session_finish_mode(
    lt48_gal_wave_session *session
) {
    static const unsigned char finalize[] = {0x49u, 0xaau, 0x55u};
    lt48_runtime_result *result = NULL;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (!session->requires_cleanup) return LT48_INVALID_ARGUMENT;
    status = gal_run_wave(session, 3u, session->lifecycle_mode,
                          NULL, 0u, &result);
    lt48_runtime_result_close(result);
    result = NULL;
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_status(session->transport, 1);
    if (status == LT48_OK)
        status = gal_run_wave(session, 1u, session->lifecycle_mode,
                              NULL, 0u, &result);
    lt48_runtime_result_close(result);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_controller_write(
            session->transport, finalize, sizeof(finalize));
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_flush(session->transport);
    if (status == LT48_OK) session->requires_cleanup = 0;
    else session->failed = 1;
    return status;
}

int lt48_gal_wave_session_requires_cleanup(
    const lt48_gal_wave_session *session
) {
    return session ? session->requires_cleanup : 0;
}

static int gal_mode_supported(uint32_t mode) {
    return mode == 1u || mode == 2u || mode == 3u || mode == 4u ||
           mode == 7u || mode == 8u || mode == 10u || mode == 11u;
}

static lt48_status gal_recovery_step(
    lt48_gal_wave_session *session, size_t phase
) {
    lt48_runtime_transport_session *transport = NULL;
    lt48_runtime_machine *machine = NULL;
    lt48_runtime_result *result = NULL;
    lt48_runtime_buffer_seed seeds[GAL_MAX_BUFFERS - 1u];
    size_t seed_count = 0u;
    lt48_status status;
    status = lt48_runtime_transport_session_create(
        session->transport_api, session->transport_context, &transport);
    if (status != LT48_OK) return status;
    if (phase == 0u) {
        for (uint8_t buffer = 0u; buffer < session->buffer_count; ++buffer)
            if (buffer != session->mode_buffer)
                seeds[seed_count++] = (lt48_runtime_buffer_seed){
                    buffer, session->buffers[buffer], 32u};
        status = lt48_gal_resources_image_machine_internal(
            session->resources, 3u, session->lifecycle_mode,
            session->voltage, session->variables, session->variable_count,
            seeds, seed_count, lt48_fuse_image_view(session->program_image), &machine);
        if (status == LT48_OK)
            status = lt48_runtime_transport_session_run(
                transport, machine, &result);
        if (status == LT48_OK)
            status = gal_result_check(result);
    } else if (phase == 1u) {
        status = lt48_runtime_transport_session_status(transport, 1);
    } else {
        unsigned char bytes[3];
        size_t length = sizeof(bytes);
        if (phase >= 3u && phase <= 6u) {
            bytes[0] = 0x10u;
            bytes[1] = (unsigned char)(6u - phase);
            bytes[2] = 0u;
        } else if (phase == 7u) {
            bytes[0] = 0x49u; bytes[1] = 0xaau; bytes[2] = 0x55u;
        } else {
            bytes[0] = 0x40u; bytes[1] = 2u;
            length = 2u;
        }
        status = lt48_runtime_transport_session_controller_write(
            transport, bytes, length);
    }
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_flush(transport);
    lt48_runtime_result_close(result);
    lt48_runtime_machine_close(machine);
    lt48_runtime_transport_session_close(transport);
    return status;
}

lt48_status lt48_gal_wave_session_recover(
    lt48_gal_wave_session *session, lt48_palce_recovery_result *out_result
) {
    lt48_status first_error = LT48_OK;
    if (!session || !out_result || !session->requires_cleanup)
        return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    session->failed = 1;
    for (size_t phase = 0u; phase < LT48_PALCE_RECOVERY_PHASE_COUNT; ++phase) {
        lt48_status status = gal_recovery_step(session, phase);
        out_result->phases[phase] = status;
        ++out_result->attempted_phases;
        if (first_error == LT48_OK && status != LT48_OK)
            first_error = status;
    }
    if (first_error == LT48_OK) {
        memset(session->voltage, 0, sizeof(session->voltage));
        session->requires_cleanup = 0;
        out_result->completed = 1;
    }
    return first_error;
}

/* Shared original 004599f0 traversal supplies the complete fixed-width cell,
 * including expected data in Verify and per-cell readback during Program. */
static lt48_status gal_exchange_row(void *context, uint16_t mode,
    uint16_t row, uint16_t column, uint8_t type, unsigned char *expected,
    const unsigned char *mask, size_t width, unsigned char *actual) {
    lt48_gal_wave_session *session = context;
    static const char *names[] = {"_BUF_PLine", "_BUF_ILine", "_BUF_DataMask",
        "_BUF_FuseType", "_BUF_DataOut", "_BUF_DataIn"};
    uint8_t ids[6];
    unsigned char row_bytes[2] = {(unsigned char)row, (unsigned char)(row >> 8)};
    unsigned char column_bytes[2] = {(unsigned char)column, (unsigned char)(column >> 8)};
    lt48_runtime_buffer_seed seeds[6];
    lt48_runtime_result *result = NULL;
    lt48_status status;
    const unsigned char *captured;
    size_t size;
    if (!width || width > 32u) return LT48_UNSUPPORTED;
    for (size_t i = 0; i < 6; ++i) {
        status = lt48_gal_resources_buffer(session->resources, names[i], &ids[i]);
        if (status != LT48_OK) return status;
    }
    seeds[0] = (lt48_runtime_buffer_seed){ids[0], row_bytes, 2};
    seeds[1] = (lt48_runtime_buffer_seed){ids[1], column_bytes, 2};
    seeds[2] = (lt48_runtime_buffer_seed){ids[2], mask, width};
    seeds[3] = (lt48_runtime_buffer_seed){ids[3], &type, 1};
    size_t seed_count = 4;
    if (mode == 2 || mode == 3 || mode == 4)
        seeds[seed_count++] = (lt48_runtime_buffer_seed){ids[4], expected, width};
    if (mode == 2)
        seeds[seed_count++] = (lt48_runtime_buffer_seed){ids[5], expected, width};
    status = gal_run_wave(session, mode == 2 ? 5u : 4u, mode, seeds, seed_count, &result);
    if (status == LT48_OK) {
        status = lt48_runtime_get_buffer(result, session->data_in, &captured, &size);
        if (status == LT48_OK && size < width) status = LT48_RUNTIME_ERROR;
        if (status == LT48_OK) memcpy(actual, captured, width);
        if (status == LT48_OK && (mode == 2 || mode == 3 || mode == 4)) {
            status=lt48_runtime_get_buffer(result,session->data_out,&captured,&size);
            if(status==LT48_OK && size<width)status=LT48_RUNTIME_ERROR;
            if(status==LT48_OK)memcpy(expected,captured,width);
        }
    }
    lt48_runtime_result_close(result);
    return status;
}

static lt48_status gal_rows_one(lt48_gal_wave_session *session, size_t region, uint16_t mode,
    const unsigned char *input, unsigned char *output, size_t count,
    lt48_palce_fuse_comparison *comparison, size_t *completed) {
    lt48_fuse_image *image = NULL;
    lt48_fuse_row_result result = {0};
    const lt48_fuse_code *code;
    unsigned char *values;
    lt48_status status;
    if (!session || !completed) return LT48_INVALID_ARGUMENT;
    *completed = 0;
    if (session->failed) return LT48_SAFETY_REJECTED;
    size_t region_offset=0;
    code = lt48_gal_resources_fuse_context(session->resources,region,&region_offset,NULL);
    if (!code || count != code->total_fuse_count) return LT48_INVALID_ARGUMENT;
    values = calloc(count, 1);
    if (!values) return LT48_NO_MEMORY;
    if (input) {
        for (size_t i = 0; i < count; ++i)
            if (input[i] > 1) { free(values); return LT48_INVALID_ARGUMENT; }
        memcpy(values, input, count);
    }
    if (comparison) {
        memset(comparison, 0, sizeof(*comparison));
        comparison->first_mismatch = SIZE_MAX;
    }
    status = lt48_fuse_image_build(code, values, count, &image);
    if (status == LT48_OK)
        status = lt48_fuse_image_run_rows(image, mode,
            (unsigned char)lt48_gal_resources_blank_fill_internal(session->resources),
            gal_exchange_row, NULL, session, &result);
    *completed = result.exchanges;
    if (status == LT48_OK) status = lt48_runtime_transport_session_flush(session->transport);
    if (status == LT48_OK && output) {
        status = lt48_fuse_image_decode(code, lt48_fuse_image_view(image), values, count);
        if (status == LT48_OK) memcpy(output, values, count);
    }
    if (status == LT48_OK && comparison) {
        comparison->has_result = 1;
        comparison->matched = result.matched;
        comparison->completed_cells = result.exchanges;
        if (!result.matched) {
            size_t n;
            const lt48_fuse_coordinate *coordinates = lt48_gal_resources_coordinates(session->resources, &n);
            comparison->mismatch_count = 1;
            for (size_t i = region_offset; i < region_offset+count && i<n; ++i) {
                const lt48_fuse_coordinate *c = coordinates + i;
                if (c->row == result.mismatch_row && c->column == result.mismatch_column &&
                    c->bit / 8u == result.mismatch_byte &&
                    ((result.expected ^ result.actual) & (1u << (c->bit % 8u)))) {
                    comparison->first_mismatch = i;
                    comparison->expected = (result.expected >> (c->bit % 8u)) & 1u;
                    comparison->actual = (result.actual >> (c->bit % 8u)) & 1u;
                    break;
                }
            }
        }
    }
    if (status == LT48_OK && mode == 2 && !result.matched) status = LT48_DEVICE_MISMATCH;
    lt48_fuse_image_destroy(image);
    free(values);
    return status;
}

static lt48_status gal_rows(lt48_gal_wave_session *session,uint16_t mode,
    const unsigned char *input,unsigned char *output,size_t count,
    lt48_palce_fuse_comparison *comparison,size_t *completed){
    if(!session||!completed)return LT48_INVALID_ARGUMENT;
    size_t total=0;lt48_gal_resources_coordinates(session->resources,&total);
    if(count!=total)return LT48_INVALID_ARGUMENT;
    *completed=0;if(comparison){memset(comparison,0,sizeof *comparison);comparison->first_mismatch=SIZE_MAX;comparison->has_result=1;comparison->matched=1;}
    unsigned char *pending=output?malloc(total):NULL;
    if(output&&!pending)return LT48_NO_MEMORY;
    lt48_status st=LT48_OK;
    for(size_t i=0;i<lt48_gal_resources_fuse_context_count(session->resources);i++){
        size_t offset=0,n=0;const lt48_fuse_code *code=lt48_gal_resources_fuse_context(session->resources,i,&offset,NULL);
        lt48_palce_fuse_comparison part={0};
        st=gal_rows_one(session,i,mode,input?input+offset:NULL,pending?pending+offset:NULL,code->total_fuse_count,comparison?&part:NULL,&n);
        *completed+=n;
        if(comparison){if(!part.matched)*comparison=part;comparison->completed_cells=*completed;}
        if(st!=LT48_OK || (comparison&&!part.matched))break;
    }
    if(st==LT48_OK&&output)memcpy(output,pending,total);
    free(pending);return st;
}

lt48_status lt48_gal_wave_session_read_fuses(lt48_gal_wave_session *session,
    uint32_t mode, unsigned char *out_fuses, size_t capacity, size_t *completed) {
    const lt48_fuse_code *code = lt48_gal_resources_fuse_code(session ? session->resources : NULL);
    size_t total=0;lt48_gal_resources_coordinates(session?session->resources:NULL,&total);
    if (!code || !out_fuses || capacity < total) return LT48_INVALID_ARGUMENT;
    /* Other modes need an expected image; do not silently verify against zeros. */
    if (mode != 1u) return LT48_UNSUPPORTED;
    return gal_rows(session, 1, NULL, out_fuses, total, NULL, completed);
}

lt48_status lt48_gal_wave_session_prepare_program_fuses(lt48_gal_wave_session *session,
    const unsigned char *fuses,size_t count){
    if(!session || !fuses)return LT48_INVALID_ARGUMENT;
    if(session->failed)return LT48_SAFETY_REJECTED;
    if(session->requires_cleanup)return LT48_BUSY;
    const lt48_fuse_code *code=lt48_gal_resources_fuse_code(session->resources);
    size_t total=0;lt48_gal_resources_coordinates(session->resources,&total);
    if(!code || count!=total)return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<count;i++)if(fuses[i]>1u)return LT48_INVALID_ARGUMENT;
    lt48_fuse_image *image=NULL;
    lt48_status st=lt48_fuse_image_build(code,fuses,code->total_fuse_count,&image);
    if(st==LT48_OK){lt48_fuse_image_destroy(session->program_image);session->program_image=image;}
    return st;
}

lt48_status lt48_gal_wave_session_program_fuses(lt48_gal_wave_session *session,
    const unsigned char *fuses, size_t count, size_t *completed) {
    if (!fuses) return LT48_INVALID_ARGUMENT;
    return gal_rows(session, 2, fuses, NULL, count, NULL, completed);
}

lt48_status lt48_gal_wave_session_verify_fuses(lt48_gal_wave_session *session,
    uint32_t mode, const unsigned char *expected, size_t count,
    lt48_palce_fuse_comparison *comparison) {
    size_t completed;
    if (!expected || !comparison || (mode != 3u && mode != 4u)) return LT48_INVALID_ARGUMENT;
    return gal_rows(session, (uint16_t)mode, expected, NULL, count, comparison, &completed);
}

lt48_status lt48_gal_wave_session_blank_check(lt48_gal_wave_session *session,
    lt48_palce_fuse_comparison *comparison) {
    size_t completed;
    const lt48_fuse_code *code = lt48_gal_resources_fuse_code(session ? session->resources : NULL);
    if (!code || !comparison) return LT48_INVALID_ARGUMENT;
    size_t total=0;lt48_gal_resources_coordinates(session->resources,&total);
    return gal_rows(session, 11, NULL, NULL, total, comparison, &completed);
}

static lt48_status gal_verify_passes(lt48_gal_wave_session *session,const unsigned char *expected,size_t count,lt48_palce_fuse_comparison *comparison){
    lt48_status st=LT48_OK;
    for(unsigned i=0;i<session->verify_passes;i++){
        if(i)st=gal_switch_mode(session,4u,(int)i);
        if(st==LT48_OK)st=lt48_gal_wave_session_verify_fuses(session,4u,expected,count,comparison);
        if(st!=LT48_OK || !comparison->matched)break;
    }
    return st;
}

lt48_status lt48_gal_execute_operation(
    const lt48_gal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_operation_kind operation, const unsigned char *input_fuses,
    size_t input_count, unsigned char *read_fuses, size_t read_capacity,
    lt48_gal_operation_result *out_result
) {
    lt48_gal_wave_session *session = NULL;
    lt48_palce_recovery_result recovery;
    lt48_status status = LT48_OK;
    unsigned char *pending_read = NULL;
    size_t fuse_count = 0u;
    if (!out_result || !resources || !transport) return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    out_result->comparison.first_mismatch = SIZE_MAX;
    if (operation == LT48_OPERATION_READ && !read_fuses)
        return LT48_INVALID_ARGUMENT;
    if ((operation == LT48_OPERATION_PROGRAM ||
         operation == LT48_OPERATION_VERIFY) && !input_fuses)
        return LT48_INVALID_ARGUMENT;
    if (operation != LT48_OPERATION_READ &&
        operation != LT48_OPERATION_BLANK_CHECK &&
        operation != LT48_OPERATION_PROGRAM &&
        operation != LT48_OPERATION_VERIFY &&
        operation != LT48_OPERATION_ERASE && operation != LT48_OPERATION_SECURE)
        return LT48_UNSUPPORTED;
    {

        (void)lt48_gal_resources_coordinates(resources, &fuse_count);
        if ((operation == LT48_OPERATION_PROGRAM ||
             operation == LT48_OPERATION_VERIFY) &&
            input_count != fuse_count)
            return LT48_INVALID_ARGUMENT;
        if (operation == LT48_OPERATION_READ && read_capacity < fuse_count)
            return LT48_BUFFER_TOO_SMALL;
    }
    if (input_fuses && (operation == LT48_OPERATION_PROGRAM || operation == LT48_OPERATION_VERIFY))
        for (size_t i = 0u; i < input_count; ++i)
            if (input_fuses[i] > 1u) return LT48_INVALID_ARGUMENT;
    if (operation == LT48_OPERATION_READ) {
        pending_read = calloc(fuse_count, 1u);
        if (!pending_read) return LT48_NO_MEMORY;
    }
    status = lt48_gal_wave_session_create(resources, transport, context,
                                           &session);
    if(status==LT48_OK && operation==LT48_OPERATION_PROGRAM)
        status=lt48_gal_wave_session_prepare_program_fuses(session,input_fuses,input_count);
    if(status==LT48_OK && operation==LT48_OPERATION_VERIFY){
        if(session->verify_custom)session->variables[session->verify_voltage_index].value=session->verify_values[0];
        else if(session->verify_margin_index!=SIZE_MAX)session->variables[session->verify_margin_index].value=0;
    }
    if (status == LT48_OK) {
        status = lt48_gal_wave_session_enter_mode(
            session, operation == LT48_OPERATION_ERASE ? 7u :
            operation == LT48_OPERATION_SECURE ? 8u :
            operation == LT48_OPERATION_PROGRAM ? 11u :
            operation == LT48_OPERATION_VERIFY ? 4u :
            operation == LT48_OPERATION_BLANK_CHECK ? 11u : 1u);
    }
    if (status == LT48_OK) {
        if (operation == LT48_OPERATION_READ) {
            status = lt48_gal_wave_session_read_fuses(
                session, 1u, pending_read, fuse_count,
                &out_result->completed_cells);
        } else if (operation == LT48_OPERATION_BLANK_CHECK) {
            status = lt48_gal_wave_session_blank_check(
                session, &out_result->comparison);
        } else if (operation == LT48_OPERATION_VERIFY) {
            status = gal_verify_passes(session,input_fuses,input_count,&out_result->comparison);
        } else if (operation == LT48_OPERATION_PROGRAM) {
            status = lt48_gal_wave_session_blank_check(
                session, &out_result->comparison);
            if (status == LT48_OK && out_result->comparison.matched) {
                status = lt48_gal_wave_session_switch_mode(session, 2u);
                if (status == LT48_OK)
                    status = lt48_gal_wave_session_program_fuses(
                        session, input_fuses, input_count,
                        &out_result->completed_cells);
                if (status == LT48_OK) {
                    out_result->programmed = 1;
                    status = gal_switch_mode(session, 4u,0);
                }
                if (status == LT48_OK)
                    status = gal_verify_passes(session,input_fuses,input_count,&out_result->comparison);
            }
        } else if (operation == LT48_OPERATION_SECURE) {
            lt48_runtime_result *result = NULL;
            status = gal_run_wave(session, 7u, 8u, NULL, 0u, &result);
            lt48_runtime_result_close(result);
            if (status == LT48_OK)
                status = lt48_runtime_transport_session_flush(session->transport);
            if (status == LT48_OK) out_result->secured = 1;
        } else if (operation == LT48_OPERATION_ERASE) {
            lt48_runtime_result *result = NULL;
            status = gal_run_wave(session, 6u, 7u, NULL, 0u, &result);
            lt48_runtime_result_close(result);
            if (status == LT48_OK)
                status = lt48_gal_wave_session_switch_mode(session, 11u);
            if (status == LT48_OK) {
                out_result->erased = 1;
                status = lt48_gal_wave_session_blank_check(
                    session, &out_result->comparison);
            }
        }
    }
    out_result->operation_status = status;
    if (session && lt48_gal_wave_session_requires_cleanup(session)) {
        if (status == LT48_OK) {
            out_result->cleanup_status =
                lt48_gal_wave_session_finish_mode(session);
            if (out_result->cleanup_status != LT48_OK) {
                lt48_status recovery_status =
                    lt48_gal_wave_session_recover(session, &recovery);
                out_result->cleanup_completed =
                    recovery_status == LT48_OK && recovery.completed;
            } else {
                out_result->cleanup_completed = 1;
            }
        } else {
            out_result->cleanup_status =
                lt48_gal_wave_session_recover(session, &recovery);
            out_result->cleanup_completed =
                out_result->cleanup_status == LT48_OK && recovery.completed;
        }
    }
    if (status == LT48_OK && out_result->cleanup_status != LT48_OK)
        status = out_result->cleanup_status;
    if (status == LT48_OK && out_result->cleanup_completed) {
        /* Original UI result command: 4=Good, 2=Error; both clear Busy.
         * Report success only after power-off and controller finalization. */
        const unsigned char indicator[] = {0x40u,
            out_result->comparison.has_result && !out_result->comparison.matched ? 2u : 4u};
        status = lt48_runtime_transport_session_controller_write(
            session->transport, indicator, sizeof(indicator));
        if (status == LT48_OK) status = lt48_runtime_transport_session_flush(session->transport);
        if (status != LT48_OK) {
            out_result->cleanup_status = status;
            (void)lt48_runtime_transport_error_indicator_internal(transport,context);
        }
    }
    if (status == LT48_OK && pending_read)
        memcpy(read_fuses, pending_read, fuse_count);
    free(pending_read);
    lt48_gal_wave_session_close(session);
    return status;
}
