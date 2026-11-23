#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PAL_MAX_BUFFERS 76u
#define PAL_MAX_WAVES 35u

struct lt48_pal_wave_session {
    const lt48_pal_resources *resources;
    const lt48_protocol_transport *transport_api;
    void *transport_context;
    lt48_runtime_transport_session *transport;
    unsigned char buffers[PAL_MAX_BUFFERS][32];
    lt48_compile_variable *variables;
    size_t variable_count;
    uint32_t voltage[4];
    unsigned char epm_state[LT48_EPM_LEGACY_STATE_SIZE];
    lt48_runtime_configuration_value configuration[4];
    size_t configuration_count;
    uint8_t buffer_count;
    uint8_t mode_buffer;
    uint8_t data_in;
    uint8_t data_out;
    int failed;
    int requires_cleanup;
    int initialized;
    int read_protected;
    uint32_t lifecycle_mode;
    uint16_t power_on, power_off, initialize, reset;
    int pof_range_reset;
    unsigned fuse_section;
    lt48_fuse_image *project_image;
};

static int pal_mode_supported(uint32_t mode);

static lt48_status pal_result_check(const lt48_runtime_result *result) {
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

lt48_status lt48_pal_wave_session_create(
    const lt48_pal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_pal_wave_session **out_session
) {
    lt48_pal_wave_session *session;
    const lt48_compile_variable *variables;
    lt48_status status;
    if (!resources || !transport || !out_session) return LT48_INVALID_ARGUMENT;
    *out_session = NULL;
    session = calloc(1u, sizeof(*session));
    if (!session) return LT48_NO_MEMORY;
    session->resources = resources;
    session->transport_api = transport;
    session->transport_context = context;
    session->buffer_count = lt48_pal_resources_buffer_count(resources);
    if (!session->buffer_count || session->buffer_count > PAL_MAX_BUFFERS) {
        free(session);
        return LT48_UNSUPPORTED;
    }
    status = lt48_pal_resources_buffer(resources, "_BUF_Mode",
                                        &session->mode_buffer);
    if (status == LT48_OK)
        status = lt48_pal_resources_buffer(resources, "_BUF_DataIn",
                                            &session->data_in);
    if (status == LT48_OK)
        status = lt48_pal_resources_buffer(resources, "_BUF_DataOut",
                                            &session->data_out);
    session->power_on=0;session->power_off=1;session->initialize=2;session->reset=3;
    if (status==LT48_OK && lt48_pal_resources_pof_internal(resources)) {
        const char *names[]={"_FUN_POWERON","_FUN_POWEROFF","_FUN_DEVINIT","_FUN_DEVRESET"};
        uint16_t *ids[]={&session->power_on,&session->power_off,&session->initialize,&session->reset};
        for(unsigned i=0;i<4 && status==LT48_OK;i++)
            status=lt48_pal_resources_operation_internal(resources,names[i],ids[i]);
    }
    variables = lt48_pal_resources_variables(resources, &session->variable_count);
    if (status == LT48_OK && session->variable_count != 0u && !variables)
        status = LT48_FORMAT_ERROR;
    if (status == LT48_OK && session->variable_count != 0u) {
        session->variables = calloc(session->variable_count,
                                     sizeof(*session->variables));
        if (!session->variables) status = LT48_NO_MEMORY;
        else memcpy(session->variables, variables,
                    session->variable_count * sizeof(*variables));
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

void lt48_pal_wave_session_close(lt48_pal_wave_session *session) {
    if (!session) return;
    lt48_runtime_transport_session_close(session->transport);
    lt48_fuse_image_destroy(session->project_image);
    free(session->variables);
    free(session);
}

static lt48_status pal_run_wave(
    lt48_pal_wave_session *session, uint16_t operation, uint32_t mode,
    const lt48_runtime_buffer_seed *overlays, size_t overlay_count,
    lt48_runtime_result **out_result
) {
    unsigned char buffers[PAL_MAX_BUFFERS][32];
    lt48_runtime_buffer_seed seeds[PAL_MAX_BUFFERS - 1u];
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
        /* The old EPM callbacks access words even when the row worker
         * updates only one byte. Preserve the rest of that shared frame. */
        if (!lt48_pal_resources_legacy_epm_internal(session->resources) &&
            !lt48_pal_resources_pof_internal(session->resources))
            memset(buffers[seed->buffer], 0, 32u);
        if (seed->size) memcpy(buffers[seed->buffer], seed->data, seed->size);
    }
    for (uint8_t buffer = 0u; buffer < session->buffer_count; ++buffer) {
        if (buffer == session->mode_buffer) continue;
        seeds[seed_count++] = (lt48_runtime_buffer_seed){
            buffer, buffers[buffer], 32u};
    }
    if(session->project_image)status=lt48_pal_resources_machine_image_internal(
        session->resources,operation,mode,session->voltage,session->variables,session->variable_count,
        seeds,seed_count,session->epm_state,session->configuration,session->configuration_count,
        lt48_fuse_image_view(session->project_image),&machine);
    else status = lt48_pal_resources_machine_state_internal(
        session->resources, operation, mode, session->voltage,
        session->variables, session->variable_count, seeds, seed_count,
        session->epm_state, session->configuration,session->configuration_count,&machine);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_run(
            session->transport, machine, &result);
    if(status==LT48_OK)for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
        lt48_runtime_event event;
        if(lt48_runtime_get_event(result,i,&event)==LT48_OK && event.kind==LT48_RUNTIME_HOST_NOTIFICATION && event.argument_count==2u && event.arguments[0]==6u && event.arguments[1]==3u) {
            session->read_protected=1;status=LT48_PERMISSION_DENIED;break;
        }
    }
    if (status == LT48_OK)
        status = pal_result_check(result);
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
    if(status==LT48_OK && lt48_pal_resources_legacy_epm_internal(session->resources)) {
        const unsigned char *state;size_t size;
        status=lt48_runtime_get_image(result,&state,&size);
        if(status==LT48_OK && size!=sizeof(session->epm_state))status=LT48_FORMAT_ERROR;
        if(status==LT48_OK)memcpy(session->epm_state,state,size);
    }
    if(status==LT48_OK){
        unsigned section;lt48_status selected=lt48_runtime_get_fuse_section(result,&section);
        if(selected==LT48_OK)session->fuse_section=section;
        else if(selected!=LT48_NOT_FOUND)status=selected;
    }
    if(status==LT48_OK && lt48_pal_resources_project_image_internal(session->resources)==2) {
        const lt48_runtime_configuration_value *values;size_t count;
        status=lt48_runtime_get_configuration(result,&values,&count);
        if(status==LT48_OK && count>sizeof(session->configuration)/sizeof(*session->configuration))status=LT48_SAFETY_REJECTED;
        if(status==LT48_OK){if(count)memcpy(session->configuration,values,count*sizeof(*values));session->configuration_count=count;}
    }
    if(status==LT48_OK && session->project_image) {
        const lt48_runtime_fuse_image *updated=NULL;
        lt48_status changed=lt48_runtime_host_fuse_image_internal(result,&updated);
        if(changed==LT48_OK) {
            const lt48_runtime_fuse_image *owned=lt48_fuse_image_view(session->project_image);
            if(updated->size!=owned->size || updated->record_width!=owned->record_width ||
               updated->copy_width!=owned->copy_width || updated->stride!=owned->stride)status=LT48_FORMAT_ERROR;
            else memcpy((void*)owned->data,updated->data,owned->size);
        } else if(changed!=LT48_NOT_FOUND)status=changed;
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
    if (status != LT48_OK && getenv("LT48_SERIAL_TRACE"))fprintf(stderr,"PAL op%u mode%u status%d\n",operation,mode,status);
    if (status != LT48_OK) return status;
    return LT48_OK;
}

static lt48_status pal_reset_frame(lt48_pal_wave_session *session) {
    lt48_runtime_frame_profile profile;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    status = lt48_pal_resources_frame_profile(session->resources, &profile);
    if (status != LT48_OK) return status;
    status = lt48_runtime_frame_set_mode(
        session->buffers, profile.buffer_count, profile.reset_mask,
        profile.mode_buffer, (uint16_t)session->lifecycle_mode, 0, 0);
    return status;
}

lt48_status lt48_pal_wave_session_enter_mode(
    lt48_pal_wave_session *session, uint32_t mode
) {
    static const unsigned char controls[] = {0x56u, 0x0au, 1u, 0x40u, 1u};
    lt48_runtime_result *result = NULL;
    lt48_runtime_frame_profile profile;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (mode > UINT16_MAX || !pal_mode_supported(mode) ||
        session->failed || session->requires_cleanup)
        return session->failed ? LT48_SAFETY_REJECTED : LT48_BUSY;
    status = lt48_pal_resources_frame_profile(session->resources, &profile);
    if (status == LT48_OK)
        status = lt48_runtime_frame_set_mode(
            session->buffers, profile.buffer_count, profile.reset_mask,
            profile.mode_buffer, (uint16_t)mode, 0, 0);
    if (status == LT48_OK) {
        session->lifecycle_mode = mode;
        const unsigned char *idle=NULL;size_t idle_size=0;
        if(lt48_pal46_idle_internal(session->resources,&idle,&idle_size)==LT48_OK) {
            session->requires_cleanup=1;
            status=lt48_runtime_transport_session_wave_watchdog(session->transport,5000,0);
            if(status==LT48_OK)status=lt48_owner_load_fpga_internal(session->transport,idle,idle_size);
        }
        if(lt48_pal_resources_pof_internal(session->resources)) {
            const unsigned char *image;size_t size;unsigned selector;
            status=lt48_pal_resources_pof_fpga_internal(session->resources,&selector,&image,&size);
            if(status!=LT48_OK)return status;
            /* 4251A0/43D3F0 load the selected base image before PowerOn.
             * Always load here instead of trusting a previous operation's
             * FPGA state. This also applies compact 93's compile-time choice. */
            static const unsigned char header[]={0x38,0xaa,0x55};
            session->requires_cleanup=1;
            status=lt48_runtime_transport_session_wave_watchdog(session->transport,5000,0);
            if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(session->transport,header,sizeof header);
            if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(session->transport,image,size);
        }
        session->requires_cleanup = 1;
        if(status==LT48_OK)status = pal_run_wave(session, session->power_on, mode, NULL, 0u, &result);
    }
    lt48_runtime_result_close(result);
    result = NULL;
    for (unsigned wait = 0u; status == LT48_OK && wait < 3u; ++wait)
        status = lt48_runtime_transport_session_status(session->transport, 1);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_controller_write(
            session->transport, controls, sizeof(controls));
    session->initialized=0;
    if (status == LT48_OK)
        status = pal_run_wave(session, session->initialize, mode, NULL, 0u, &result);
    if(status==LT48_OK)session->initialized=1;
    lt48_runtime_result_close(result);
    if (status != LT48_OK) session->failed = 1;
    return status;
}

lt48_status lt48_pal_wave_session_switch_mode(
    lt48_pal_wave_session *session, uint32_t mode
) {
    lt48_runtime_result *result = NULL;
    lt48_status status;
    if (!session || mode > UINT16_MAX || !pal_mode_supported(mode))
        return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (!session->requires_cleanup) return LT48_INVALID_ARGUMENT;
    status = session->pof_range_reset ? LT48_OK : pal_run_wave(session, session->reset, session->lifecycle_mode,
                          NULL, 0u, &result);
    lt48_runtime_result_close(result);
    result = NULL;
    if (status == LT48_OK) {
        session->pof_range_reset=0;
        session->lifecycle_mode = mode;
        status = pal_reset_frame(session);
    }
    session->initialized=0;
    if (status == LT48_OK)
        status = pal_run_wave(session, session->initialize, mode, NULL, 0u, &result);
    if(status==LT48_OK)session->initialized=1;
    lt48_runtime_result_close(result);
    if (status != LT48_OK) session->failed = 1;
    return status;
}

lt48_status lt48_pal_wave_session_finish_mode(
    lt48_pal_wave_session *session
) {
    static const unsigned char finalize[] = {0x49u, 0xaau, 0x55u};
    lt48_runtime_result *result = NULL;
    lt48_status status;
    if (!session) return LT48_INVALID_ARGUMENT;
    if (session->failed) return LT48_SAFETY_REJECTED;
    if (!session->requires_cleanup) return LT48_INVALID_ARGUMENT;
    status = session->pof_range_reset ? LT48_OK : pal_run_wave(session, session->reset, session->lifecycle_mode,
                          NULL, 0u, &result);
    lt48_runtime_result_close(result);
    result = NULL;
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_status(session->transport, 1);
    if (status == LT48_OK)
        status = pal_run_wave(session, session->power_off, session->lifecycle_mode,
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

int lt48_pal_wave_session_requires_cleanup(
    const lt48_pal_wave_session *session
) {
    return session ? session->requires_cleanup : 0;
}

static int pal_mode_supported(uint32_t mode) {
    return mode == 1u || mode == 2u || mode == 3u || mode == 4u ||
           mode == 7u || mode == 8u || mode == 10u || mode == 11u;
}

static lt48_status pal_recovery_step(
    lt48_pal_wave_session *session, size_t phase
) {
    lt48_runtime_transport_session *transport = NULL;
    lt48_runtime_machine *machine = NULL;
    lt48_runtime_result *result = NULL;
    lt48_runtime_buffer_seed seeds[PAL_MAX_BUFFERS - 1u];
    size_t seed_count = 0u;
    lt48_status status;
    /* Cancellation ends the requested operation, not the bounded sequence
     * which removes programming voltages and restores idle pin control. */
    lt48_protocol_transport recovery_api = *session->transport_api;
    recovery_api.cancelled = NULL;
    status = lt48_runtime_transport_session_create(
        &recovery_api, session->transport_context, &transport);
    if (status != LT48_OK) return status;
    if (phase == 0u && !session->initialized) {
        /* A partial entry has no valid device-reset scratch frame. The
         * subsequent independent phases still remove all four rails and
         * restore idle pins. Do not execute dynamic shifts with zero widths. */
        status=LT48_OK;
    } else if (phase == 0u) {
        for (uint8_t buffer = 0u; buffer < session->buffer_count; ++buffer)
            if (buffer != session->mode_buffer)
                seeds[seed_count++] = (lt48_runtime_buffer_seed){
                    buffer, session->buffers[buffer], 32u};
        status = lt48_pal_resources_create_machine_with_variables(
            session->resources, session->reset, session->lifecycle_mode,
            session->voltage, session->variables, session->variable_count,
            seeds, seed_count, &machine);
        if (status == LT48_OK)
            status = lt48_runtime_transport_session_run(
                transport, machine, &result);
        if (status == LT48_OK)
            status = pal_result_check(result);
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

lt48_status lt48_pal_wave_session_recover(
    lt48_pal_wave_session *session, lt48_palce_recovery_result *out_result
) {
    lt48_status first_error = LT48_OK;
    if (!session || !out_result || !session->requires_cleanup)
        return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    session->failed = 1;
    for (size_t phase = 0u; phase < LT48_PALCE_RECOVERY_PHASE_COUNT; ++phase) {
        lt48_status status = pal_recovery_step(session, phase);
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
static lt48_status pal_exchange_row_once(void *context, uint16_t mode,
    uint16_t row, uint16_t column, uint8_t type, unsigned char *expected,
    const unsigned char *mask, size_t width, unsigned char *actual) {
    lt48_pal_wave_session *session = context;
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
        status = lt48_pal_resources_buffer(session->resources, names[i], &ids[i]);
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
    status = pal_run_wave(session, mode == 2 ? 5u : 4u, mode, seeds, seed_count, &result);
    if (status == LT48_OK) {
        status = lt48_runtime_get_buffer(result, session->data_in, &captured, &size);
        if (status == LT48_OK && size < width) status = LT48_RUNTIME_ERROR;
        if (status == LT48_OK) memcpy(actual, captured, width);
        /* Original M1 LOAD already returns positive sense, but PROGRAM
         * unconditionally complements it. MMI specifies HIGH for a blown
         * H-output fuse; cancel only that erroneous Program complement.
         * Mode11 deliberately retains the host's masked blank0 expectation,
         * ignoring original M1 LOAD's impossible DataOut=FF overwrite. */
        if (status == LT48_OK && mode == 2 &&
            (lt48_pal_resources_authored_corrections(session->resources) & LT48_PAL_POSITIVE_FUSE_SENSE))
            for(size_t i=0;i<width;i++)actual[i]^=0xffu;
        if (status == LT48_OK && (mode == 2 || mode == 3 || mode == 4) && !lt48_pal_resources_is_epm(session->resources)) {
            status=lt48_runtime_get_buffer(result,session->data_out,&captured,&size);
            if(status==LT48_OK && size<width)status=LT48_RUNTIME_ERROR;
            if(status==LT48_OK)memcpy(expected,captured,width);
        }
    }
    lt48_runtime_result_close(result);
    return status;
}

/* MMI1983 5.2.3.3: retry only underblow, at most three attempts.
 * Reinitialize the source bank state before revisiting the same physical cell.
 * Never retry unexpected blown bits or a transport failure. */
static lt48_status pal_exchange_row(void *context,uint16_t mode,uint16_t row,
    uint16_t column,uint8_t type,unsigned char *expected,const unsigned char *mask,
    size_t width,unsigned char *actual){
    lt48_pal_wave_session *s=context;
    if(mode!=2 || !(lt48_pal_resources_authored_corrections(s->resources)&LT48_PAL_MMI_ELECTRICAL))
        return pal_exchange_row_once(context,mode,row,column,type,expected,mask,width,actual);
    if(!width||width>32)return LT48_UNSUPPORTED;
    unsigned char wanted[32];memcpy(wanted,expected,width);
    for(unsigned attempt=0;attempt<3;attempt++){
        memcpy(expected,wanted,width);
        lt48_status st=pal_exchange_row_once(context,mode,row,column,type,expected,mask,width,actual);
        memcpy(expected,wanted,width);
        if(st!=LT48_OK)return st;
        unsigned missing=0,over=0;
        for(size_t i=0;i<width;i++){
            missing|=wanted[i]&~actual[i]&mask[i];
            over|=actual[i]&~wanted[i]&mask[i];
        }
        if(!missing||over||attempt==2)return LT48_OK;
        st=lt48_pal_wave_session_switch_mode(s,2u);if(st!=LT48_OK)return st;
    }
    return LT48_RUNTIME_ERROR;
}

/* Original 45DE40 uses complete retained words after copying one or two
 * image bytes. In particular, a byte stream must not clear the high byte
 * left by the preceding record. Col/Row are bound by 45D860. */
static lt48_status pof_gc_exchange(lt48_pal_wave_session *s,unsigned mode,
    uint16_t tag,uint16_t column,uint16_t row,uint16_t mask,uint32_t address,
    unsigned width,uint16_t *expected,uint16_t *actual) {
    if((tag!=7&&tag!=17)||!width||width>2||(tag==17&&width!=1))
        return LT48_UNSUPPORTED;
    lt48_runtime_buffer_seed seeds[5];size_t count=0;
    unsigned char col[2]={(unsigned char)column,(unsigned char)(column>>8)};
    unsigned char rows[2]={(unsigned char)row,(unsigned char)(row>>8)};
    unsigned char masks[2]={(unsigned char)mask,(unsigned char)(mask>>8)};
    unsigned char addr[4]={(unsigned char)address,(unsigned char)(address>>8),
        (unsigned char)(address>>16),(unsigned char)(address>>24)};
    uint8_t id;lt48_status status;
    if(tag==7) {
        const char *names[]={"_BUF_Col","_BUF_Row","_BUF_Mask"};
        const unsigned char *bytes[]={col,rows,masks};
        for(unsigned i=0;i<3;i++) {
            status=lt48_pal_resources_buffer(s->resources,names[i],&id);
            if(status!=LT48_OK)return status;
            seeds[count++]=(lt48_runtime_buffer_seed){id,bytes[i],2};
        }
    } else {
        status=lt48_pal_resources_buffer(s->resources,"_BUF_Address",&id);
        if(status!=LT48_OK)return status;
        seeds[count++]=(lt48_runtime_buffer_seed){id,addr,4};
    }
    uint16_t value=*expected;
    if(mode==2||mode==4) {
        if(width==1)value=(value&255u)|((uint16_t)s->buffers[s->data_out][1]<<8);
        if(tag==7)value&=mask;
    }
    unsigned char data[2]={(unsigned char)value,(unsigned char)(value>>8)};
    if(mode!=1)seeds[count++]=(lt48_runtime_buffer_seed){s->data_out,data,2};
    if(mode==2||mode==4)seeds[count++]=(lt48_runtime_buffer_seed){s->data_in,data,2};
    uint16_t operation;
    status=lt48_pal_resources_operation_internal(s->resources,
        mode==2?"_FUN_PROGRAM":"_FUN_LOAD",&operation);
    lt48_runtime_result *result=NULL;
    if(status==LT48_OK)status=pal_run_wave(s,operation,mode,seeds,count,&result);
    if(status==LT48_OK) {
        if(tag==7) {
            /* 45DE40 stores masked words back into the shared frame. */
            s->buffers[s->data_in][0]&=(unsigned char)mask;
            s->buffers[s->data_in][1]&=(unsigned char)(mask>>8);
            if(mode==2||mode==4) {
                s->buffers[s->data_out][0]&=(unsigned char)mask;
                s->buffers[s->data_out][1]&=(unsigned char)(mask>>8);
            }
        } else if(mode==11) s->buffers[s->data_in][1]=0;
        *actual=s->buffers[s->data_in][0]|(uint16_t)s->buffers[s->data_in][1]<<8;
        if(mode==2||mode==4)
            *expected=s->buffers[s->data_out][0]|(uint16_t)s->buffers[s->data_out][1]<<8;
    }
    lt48_runtime_result_close(result);return status;
}

/* Original 4629A0 binds A0A380=Col, A0A388=Row. The transformed
 * cell is one byte, while exception callbacks inspect retained words. */
lt48_status lt48_pof_session_exchange_internal(void *context,unsigned mode,
    uint16_t tag,uint16_t column,uint16_t row,uint16_t mask,uint32_t address,
    unsigned width,uint16_t *expected,uint16_t *actual) {
    lt48_pal_wave_session *s=context;
    (void)address;
    if(!s||!expected||!actual)return LT48_INVALID_ARGUMENT;
    if(!s->requires_cleanup||s->lifecycle_mode!=mode)return LT48_INVALID_ARGUMENT;
    if(mode!=1&&mode!=2&&mode!=4&&mode!=11)return LT48_UNSUPPORTED;
    int kind=lt48_pal_resources_pof_internal(s->resources);
    if(kind==1)return pof_gc_exchange(s,mode,tag,column,row,mask,address,width,expected,actual);
    if(kind!=2||tag!=7||width!=1)return LT48_UNSUPPORTED;
    const char *names[]={"_BUF_Col","_BUF_Row","_BUF_FuseType","_BUF_Mask"};
    uint8_t ids[4];lt48_status status;
    for(unsigned i=0;i<4;i++) {
        status=lt48_pal_resources_buffer(s->resources,names[i],&ids[i]);
        if(status!=LT48_OK)return status;
    }
    unsigned char col_bytes[2]={(unsigned char)column,(unsigned char)(column>>8)};
    unsigned char row_bytes[2]={(unsigned char)row,(unsigned char)(row>>8)};
    unsigned char type=0,mask_byte=(unsigned char)mask,data=(unsigned char)*expected;
    lt48_runtime_buffer_seed seeds[6]={{ids[0],col_bytes,2},{ids[1],row_bytes,2},{ids[2],&type,1}};
    size_t count=3;
    if(mode!=11)seeds[count++]=(lt48_runtime_buffer_seed){s->data_out,&data,1};
    if(mode==2) {
        seeds[count++]=(lt48_runtime_buffer_seed){s->data_in,&data,1};
        seeds[count++]=(lt48_runtime_buffer_seed){ids[3],&mask_byte,1};
    }
    uint16_t operation;
    status=lt48_pal_resources_operation_internal(s->resources,
        mode==2?"_FUN_PROGRAM":"_FUN_LOAD",&operation);
    lt48_runtime_result *result=NULL;
    if(status==LT48_OK)status=pal_run_wave(s,operation,mode,seeds,count,&result);
    if(status==LT48_OK) {
        *actual=s->buffers[s->data_in][0];
        if(mode!=11)*expected=s->buffers[s->data_out][0];
    }
    lt48_runtime_result_close(result);return status;
}

lt48_status lt48_pof_session_image_internal(lt48_pal_wave_session *session,
    const lt48_pof_trs_source *source,unsigned mode,const unsigned char *input,
    size_t size,unsigned char *output,lt48_pof_core_result *result) {
    if(!session)return LT48_INVALID_ARGUMENT;
    if(session->failed)return LT48_SAFETY_REJECTED;
    if(!session->requires_cleanup||session->lifecycle_mode!=mode)return LT48_INVALID_ARGUMENT;
    return lt48_pof_image_run_internal(session->resources,source,mode,input,size,
        output,lt48_pof_session_exchange_internal,session,result);
}

static lt48_status pal_rows(lt48_pal_wave_session *session, uint16_t mode,
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
    const lt48_fuse_coordinate *selected_coordinates;size_t selected_count;
    status=lt48_pal_resources_section_internal(session->resources,session->fuse_section,&code,&selected_coordinates,&selected_count);
    if(status!=LT48_OK)return status;
    if (!code || count != code->total_fuse_count) return LT48_INVALID_ARGUMENT;
    values = calloc(count, 1);
    if (!values) return LT48_NO_MEMORY;
    if (input) {
        for (size_t i = 0; i < count; ++i)
            if (input[i] > 1) { free(values); return LT48_INVALID_ARGUMENT; }
        memcpy(values, input, count);
    }
    /* ATV2500 source INIT may normalize its project configuration before
     * programming. Subsequent Program/Verify consume that same owned image. */
    if(session->project_image && lt48_pal_resources_project_image_internal(session->resources)==2 && (mode==2||mode==4)) {
        status=lt48_fuse_image_decode(code,lt48_fuse_image_view(session->project_image),values,count);
        if(status!=LT48_OK){free(values);return status;}
    }
    if (comparison) {
        memset(comparison, 0, sizeof(*comparison));
        comparison->first_mismatch = SIZE_MAX;
    }
    unsigned pia_bits=0;
    if(lt48_pal_resources_is_epm(session->resources))
        for(size_t j=0;j<session->variable_count;j++)if(session->variables[j].variable_id==13)pia_bits=session->variables[j].value;
    status = lt48_pof_fuse_image_build(code, values, count, pia_bits, &image);
    if (status == LT48_OK)
        status = lt48_fuse_image_run_rows(image, mode,
            lt48_pal_resources_blank_fuse(session->resources) ? 255 : 0,
            pal_exchange_row, NULL, session, &result);
    *completed = result.exchanges;
    if (status == LT48_OK) status = lt48_runtime_transport_session_flush(session->transport);
    if (status == LT48_OK && output) {
        status = lt48_pof_fuse_image_decode(code, lt48_fuse_image_view(image), values, count, pia_bits);
        if (status == LT48_OK) memcpy(output, values, count);
    }
    if (status == LT48_OK && comparison) {
        comparison->has_result = 1;
        comparison->matched = result.matched;
        comparison->completed_cells = result.exchanges;
        if (!result.matched) {
            size_t n=selected_count;
            const lt48_fuse_coordinate *coordinates=selected_coordinates;
            comparison->mismatch_count = 1;
            for (size_t i = 0; i < n; ++i) {
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

lt48_status lt48_pal_wave_session_read_fuses(lt48_pal_wave_session *session,
    uint32_t mode, unsigned char *out_fuses, size_t capacity, size_t *completed) {
    const lt48_fuse_code *code = lt48_pal_resources_fuse_code(session ? session->resources : NULL);
    if (!code || !out_fuses || capacity < code->total_fuse_count) return LT48_INVALID_ARGUMENT;
    /* Other modes need an expected image; do not silently verify against zeros. */
    if (mode != 1u) return LT48_UNSUPPORTED;
    return pal_rows(session, 1, NULL, out_fuses, code->total_fuse_count, NULL, completed);
}

lt48_status lt48_pal_wave_session_program_fuses(lt48_pal_wave_session *session,
    const unsigned char *fuses, size_t count, size_t *completed) {
    if (!fuses) return LT48_INVALID_ARGUMENT;
    return pal_rows(session, 2, fuses, NULL, count, NULL, completed);
}

lt48_status lt48_pal_wave_session_verify_fuses(lt48_pal_wave_session *session,
    uint32_t mode, const unsigned char *expected, size_t count,
    lt48_palce_fuse_comparison *comparison) {
    size_t completed;
    if (!expected || !comparison || (mode != 3u && mode != 4u)) return LT48_INVALID_ARGUMENT;
    return pal_rows(session, (uint16_t)mode, expected, NULL, count, comparison, &completed);
}

lt48_status lt48_pal_wave_session_blank_check(lt48_pal_wave_session *session,
    lt48_palce_fuse_comparison *comparison) {
    size_t completed;
    const lt48_fuse_code *code = lt48_pal_resources_fuse_code(session ? session->resources : NULL);
    if (!code || !comparison) return LT48_INVALID_ARGUMENT;
    return pal_rows(session, 11, NULL, NULL, code->total_fuse_count, comparison, &completed);
}

lt48_status lt48_pal_execute_operation(
    const lt48_pal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_operation_kind operation, const unsigned char *input_fuses,
    size_t input_count, unsigned char *read_fuses, size_t read_capacity,
    lt48_pal_operation_result *out_result
) {
    lt48_pal_wave_session *session = NULL;
    lt48_palce_recovery_result recovery;
    lt48_status status = LT48_OK;
    unsigned char *pending_read = NULL;
    size_t fuse_count = 0u;
    if (!out_result || !resources || !transport) return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    if (lt48_pal_resources_pof_internal(resources)) return LT48_UNSUPPORTED;
    if(lt48_pal_resources_xc95_internal(resources))
        return lt48_xc95_execute_internal(resources,transport,context,operation,
            input_fuses,input_count,read_fuses,read_capacity,
            operation==LT48_OPERATION_SECURE?lt48_pal_resources_xc95_protection_internal(resources):0,
            out_result);
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
    uint16_t erase_id=0,secure_id=0;
    lt48_status erase_status=lt48_pal_resources_operation_internal(resources,"_FUN_ERASE",&erase_id);
    lt48_status secure_status=lt48_pal_resources_operation_internal(resources,"_FUN_SECU",&secure_id);
    if(operation==LT48_OPERATION_ERASE && erase_status!=LT48_OK)return LT48_UNSUPPORTED;
    if(operation==LT48_OPERATION_SECURE && secure_status!=LT48_OK)return LT48_UNSUPPORTED;
    {
        (void)lt48_pal_resources_coordinates(resources, &fuse_count);
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
    unsigned mmi=lt48_pal_resources_authored_corrections(resources)&LT48_PAL_MMI_ELECTRICAL;
    if (operation == LT48_OPERATION_READ || (operation==LT48_OPERATION_SECURE && mmi)) {
        pending_read = calloc(fuse_count, 1u);
        if (!pending_read) return LT48_NO_MEMORY;
    }
    status = lt48_pal_wave_session_create(resources, transport, context,
                                           &session);
    if(status==LT48_OK && input_fuses && lt48_pal_resources_project_image_internal(resources))
        status=lt48_fuse_image_build(lt48_pal_resources_fuse_code(resources),input_fuses,input_count,&session->project_image);
    if (status == LT48_OK) {
        status = lt48_pal_wave_session_enter_mode(
            session, operation == LT48_OPERATION_SECURE ? 8u :
            operation == LT48_OPERATION_ERASE ? 7u :
            operation == LT48_OPERATION_PROGRAM ? 11u :
            operation == LT48_OPERATION_VERIFY ? 4u :
            operation == LT48_OPERATION_BLANK_CHECK ? 11u : 1u);
    }
    if (status == LT48_OK) {
        if (operation == LT48_OPERATION_READ) {
            status = lt48_pal_wave_session_read_fuses(
                session, 1u, pending_read, fuse_count,
                &out_result->completed_cells);
        } else if (operation == LT48_OPERATION_BLANK_CHECK) {
            status = lt48_pal_wave_session_blank_check(
                session, &out_result->comparison);
        } else if (operation == LT48_OPERATION_VERIFY) {
            status = lt48_pal_wave_session_verify_fuses(
                session, 4u, input_fuses, input_count,
                &out_result->comparison);
        } else if (operation == LT48_OPERATION_PROGRAM) {
            status = lt48_pal_wave_session_blank_check(
                session, &out_result->comparison);
            if (status == LT48_OK && out_result->comparison.matched) {
                status = lt48_pal_wave_session_switch_mode(session, 2u);
                if (status == LT48_OK)
                    status = lt48_pal_wave_session_program_fuses(
                        session, input_fuses, input_count,
                        &out_result->completed_cells);
                if (status == LT48_OK) {
                    out_result->programmed = 1;
                    status = lt48_pal_wave_session_switch_mode(session, 4u);
                }
                if (status == LT48_OK)
                    status = lt48_pal_wave_session_verify_fuses(
                        session, 4u, input_fuses, input_count,
                        &out_result->comparison);
                /* MMI1983 first5V, then high5.5V and low4.5V full-array
                 * verification. The normal Reset has restored the VCC relay. */
                if(lt48_pal_resources_authored_corrections(resources)&LT48_PAL_MMI_ELECTRICAL)
                    for(unsigned margin=0;status==LT48_OK && out_result->comparison.matched && margin<2;margin++){
                        lt48_runtime_result *r=NULL;
                        status=lt48_pal_wave_session_switch_mode(session,4u);
                        if(status==LT48_OK)status=pal_run_wave(session,30u+margin,4u,NULL,0,&r);
                        lt48_runtime_result_close(r);
                        if(status==LT48_OK)status=lt48_pal_wave_session_verify_fuses(session,4u,
                            input_fuses,input_count,&out_result->comparison);
                    }
            }
        } else if (operation == LT48_OPERATION_SECURE) {
            lt48_runtime_result *result = NULL;
            if(mmi){
                status=lt48_pal_wave_session_switch_mode(session,1u);
                if(status==LT48_OK)status=lt48_pal_wave_session_read_fuses(session,1u,pending_read,fuse_count,&out_result->completed_cells);
                for(unsigned margin=0;status==LT48_OK && margin<2;margin++){
                    status=lt48_pal_wave_session_switch_mode(session,4u);
                    if(status==LT48_OK)status=pal_run_wave(session,30u+margin,4u,NULL,0,&result);
                    lt48_runtime_result_close(result);result=NULL;
                    if(status==LT48_OK)status=lt48_pal_wave_session_verify_fuses(session,4u,pending_read,fuse_count,&out_result->comparison);
                    if(status==LT48_OK && !out_result->comparison.matched)status=LT48_DEVICE_MISMATCH;
                }
                if(status==LT48_OK)status=lt48_pal_wave_session_switch_mode(session,8u);
            }
            if(status==LT48_OK)status = pal_run_wave(session, secure_id, 8u, NULL, 0u, &result);
            lt48_runtime_result_close(result);
            if (status == LT48_OK)
                status = lt48_runtime_transport_session_flush(session->transport);
            if(status==LT48_OK && mmi){
                status=lt48_pal_wave_session_switch_mode(session,4u);
                for(unsigned margin=0;status==LT48_OK && margin<2;margin++){
                    result=NULL;status=lt48_pal_wave_session_switch_mode(session,4u);
                    if(status==LT48_OK)status=pal_run_wave(session,30u+margin,4u,NULL,0,&result);
                    lt48_runtime_result_close(result);
                    if(status==LT48_OK)status=lt48_pal_wave_session_verify_fuses(session,4u,pending_read,fuse_count,&out_result->comparison);
                    /* MMI5.3.5: either half must fail to verify at both margins.
                     * No invented protected fuse value is used. */
                    if(status==LT48_OK && out_result->comparison.matched)status=LT48_DEVICE_MISMATCH;
                }
                if(status==LT48_OK)memset(&out_result->comparison,0,sizeof out_result->comparison);
            }
            if (status == LT48_OK) out_result->secured = 1;
        } else if (operation == LT48_OPERATION_ERASE) {
            lt48_runtime_result *result = NULL;
            status = pal_run_wave(session, erase_id, 7u, NULL, 0u, &result);
            lt48_runtime_result_close(result);
            if (status == LT48_OK)
                status = lt48_pal_wave_session_switch_mode(session, 11u);
            if (status == LT48_OK) {
                out_result->erased = 1;
                status = lt48_pal_wave_session_blank_check(
                    session, &out_result->comparison);
            }
        }
    }
    /* Original 004447e0 checks option bit20 after Verify. GB INIT's
     * current-object field5 edits exactly that bit (Kernel+2c). Never apply
     * the JEDEC security request before successful array verification. */
    if(status==LT48_OK && operation==LT48_OPERATION_PROGRAM &&
       out_result->programmed && out_result->comparison.has_result && out_result->comparison.matched &&
       lt48_pal_resources_project_image_internal(resources)==2) {
        int requested=0;
        for(size_t i=0;i<session->configuration_count;i++){
            const lt48_runtime_configuration_value*c=session->configuration+i;
            if(c->object_id==-1 && c->category==0 && c->field==5)requested=!!c->value;
        }
        if(requested){
            lt48_runtime_result *result=NULL;
            status=secure_status==LT48_OK?lt48_pal_wave_session_switch_mode(session,8u):LT48_UNSUPPORTED;
            if(status==LT48_OK)status=pal_run_wave(session,secure_id,8u,NULL,0,&result);
            lt48_runtime_result_close(result);
            if(status==LT48_OK)status=lt48_runtime_transport_session_flush(session->transport);
            if(status==LT48_OK)out_result->secured=1;
        }
    }
    /* Source type5 LOAD substitutes expected data outside raw Read. Retain
     * its original callbacks, then independently read those configuration
     * cells in mode1 so a skipped comparison cannot report false success. */
    if(status==LT48_OK && lt48_pal_resources_ep35_internal(resources) &&
       out_result->comparison.has_result && out_result->comparison.matched &&
       (operation==LT48_OPERATION_PROGRAM || operation==LT48_OPERATION_VERIFY ||
        operation==LT48_OPERATION_BLANK_CHECK)) {
        const lt48_fuse_code *code;const lt48_fuse_coordinate *coords;size_t n;
        status=lt48_pal_resources_section_internal(resources,session->fuse_section,&code,&coords,&n);
        int switched=0;
        for(size_t i=0;status==LT48_OK && i<n;i++) {
            const lt48_fuse_coordinate*c=coords+i;if(c->fuse_type!=5u && !(lt48_pal_resources_project_image_internal(resources)==1&&c->row==31u))continue;
            if(!switched){status=lt48_pal_wave_session_switch_mode(session,1u);switched=1;}
            if(status!=LT48_OK)break;
            if(c->bit>7u){status=LT48_UNSUPPORTED;break;}
            unsigned char mask=(unsigned char)(1u<<c->bit),wanted=0,actual=0;
            unsigned expected=input_fuses?input_fuses[i]:lt48_pal_resources_blank_fuse(resources);
            if(expected)wanted=mask;
            status=pal_exchange_row_once(session,1u,c->row,c->column,5u,&wanted,&mask,1u,&actual);
            if(status==LT48_OK && !!(actual&mask)!=expected) {
                out_result->comparison.matched=0;out_result->comparison.mismatch_count=1;
                out_result->comparison.first_mismatch=i;out_result->comparison.expected=expected;
                out_result->comparison.actual=!!(actual&mask);
                if(operation==LT48_OPERATION_PROGRAM)out_result->programmed=0;
                break;
            }
        }
    }
    out_result->operation_status = status;
    if (session && lt48_pal_wave_session_requires_cleanup(session)) {
        if (status == LT48_OK) {
            out_result->cleanup_status =
                lt48_pal_wave_session_finish_mode(session);
            if (out_result->cleanup_status != LT48_OK) {
                lt48_status recovery_status =
                    lt48_pal_wave_session_recover(session, &recovery);
                out_result->cleanup_completed =
                    recovery_status == LT48_OK && recovery.completed;
            } else {
                out_result->cleanup_completed = 1;
            }
        } else {
            out_result->cleanup_status =
                lt48_pal_wave_session_recover(session, &recovery);
            out_result->cleanup_completed =
                out_result->cleanup_status == LT48_OK && recovery.completed;
        }
    }
    if (status == LT48_OK && out_result->cleanup_status != LT48_OK)
        status = out_result->cleanup_status;
    if (status == LT48_OK && out_result->cleanup_completed) {
        const unsigned char indicator[] = {0x40u,
            out_result->comparison.has_result && !out_result->comparison.matched ? 2u : 4u};
        status = lt48_runtime_transport_session_controller_write(session->transport, indicator, sizeof(indicator));
        if (status == LT48_OK) status = lt48_runtime_transport_session_flush(session->transport);
        if (status != LT48_OK) {
            out_result->cleanup_status = status;
            (void)lt48_runtime_transport_error_indicator_internal(transport,context);
        }
    }
    if (status == LT48_OK && pending_read && operation==LT48_OPERATION_READ)
        memcpy(read_fuses, pending_read, fuse_count);
    out_result->read_protected=session?session->read_protected:0;
    free(pending_read);
    lt48_pal_wave_session_close(session);
    return status;
}

#include "xc95_owner.inc"
#include "pof_owner.inc"

/* Functional test entry uses resident reset and the selected base FPGA,
 * followed by named POWEROFF in mode 10. Do not enter programming DevInit. */
lt48_status lt48_pof_vector_prepare_internal(lt48_pal_wave_session *s) {
    if(!s)return LT48_INVALID_ARGUMENT;
    if(s->failed||s->requires_cleanup)return LT48_SAFETY_REJECTED;
    lt48_vector_device_profile profile;
    lt48_status status=lt48_pal_resources_vector_profile_internal(s->resources,&profile);
    const unsigned char *image=NULL;size_t size=0;unsigned selector=0;
    if(status==LT48_OK)status=lt48_pal_resources_pof_fpga_internal(s->resources,&selector,&image,&size);
    if(status!=LT48_OK)return status;
    s->lifecycle_mode=10;
    status=pal_reset_frame(s);
    if(status!=LT48_OK)return status;
    s->requires_cleanup=1;
    const unsigned char reset[]={0x49,0xaa,0x55},load[]={0x38,0xaa,0x55};
    status=lt48_runtime_transport_session_wave_watchdog(s->transport,5000,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->transport,reset,sizeof reset);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->transport,load,sizeof load);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->transport,image,size);
    lt48_runtime_result *result=NULL;
    if(status==LT48_OK)status=pal_run_wave(s,profile.power_off,10,NULL,0,&result);
    lt48_runtime_result_close(result);
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->transport);
    if(status!=LT48_OK)s->failed=1;
    return status;
}

#include "pof_vectors_owner.inc"
