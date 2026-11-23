#include <labtool48/labtool48.h>

#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static struct timespec execution_now(void) {
    struct timespec value = {0};
    (void)timespec_get(&value, TIME_UTC);
    return value;
}

static double execution_elapsed_ms(
    struct timespec start, struct timespec finish
) {
    time_t seconds = finish.tv_sec - start.tv_sec;
    long nanoseconds = finish.tv_nsec - start.tv_nsec;
    if (nanoseconds < 0) {
        --seconds;
        nanoseconds += 1000000000L;
    }
    if (seconds < 0) return 0.0;
    return (double)seconds * 1000.0 + (double)nanoseconds / 1000000.0;
}

static void preparation_hex(
    const unsigned char *data, size_t size, char *out_text
) {
    static const char digits[] = "0123456789abcdef";
    for (size_t index = 0u; index < size; ++index) {
        out_text[index * 2u] = digits[data[index] >> 4];
        out_text[index * 2u + 1u] = digits[data[index] & 0x0fu];
    }
    out_text[size * 2u] = '\0';
}

lt48_status lt48_m37_prepare_read(
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *read_plan,
    const char *selector,
    const char *output_path,
    uint8_t bus,
    uint8_t address,
    uint32_t start,
    uint32_t length,
    uint32_t timeout_ms,
    lt48_m37_read_preparation *out_preparation
) {
    lt48_m37_read_preparation preparation = {0};
    lt48_protocol_plan *recovery[3] = {NULL,NULL,NULL};
    const lt48_m37_resource_audit *audit;
    lt48_m37_recovery_state unused_state;
    uint32_t expected_offset = start;
    char manifest_hex[LT48_SHA256_SIZE * 2u + 1u];
    char plan_hex[LT48_SHA256_SIZE * 2u + 1u];
    char recovery_hex[3][LT48_SHA256_SIZE * 2u + 1u];
    char *confirmation_text = NULL;
    int confirmation_size;
    lt48_status status;
    if (resources == NULL || read_plan == NULL || selector == NULL ||
        selector[0] == '\0' || output_path == NULL || output_path[0] == '\0' ||
        length == 0u || length > UINT32_MAX - start || timeout_ms == 0u ||
        out_preparation == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    audit = lt48_m37_resources_audit(resources);
    status = lt48_protocol_plan_m37_recovery_state_internal(
        read_plan, resources, 0u, LT48_CANCELLED, &unused_state
    );
    if (audit == NULL || status != LT48_OK) {
        return status == LT48_OK ? LT48_SAFETY_REJECTED : status;
    }
    preparation.transaction_count =
        lt48_protocol_plan_transaction_count(read_plan);
    for (size_t index = 0u; index < preparation.transaction_count; ++index) {
        lt48_protocol_transaction transaction;
        status = lt48_protocol_plan_get_transaction(
            read_plan, index, &transaction
        );
        if (status != LT48_OK) goto cleanup;
        if (!transaction.reads_device_data) continue;
        if (transaction.device_data_offset != expected_offset ||
            transaction.device_data_length > UINT32_MAX - expected_offset) {
            status = LT48_SAFETY_REJECTED;
            goto cleanup;
        }
        expected_offset += (uint32_t)transaction.device_data_length;
        if (preparation.read_byte_count >
            SIZE_MAX - transaction.device_data_length) {
            status = LT48_SAFETY_REJECTED;
            goto cleanup;
        }
        preparation.read_byte_count += transaction.device_data_length;
        ++preparation.read_chunk_count;
    }
    if (preparation.read_chunk_count == 0u ||
        preparation.read_byte_count != length ||
        expected_offset != start + length) {
        status = LT48_SAFETY_REJECTED;
        goto cleanup;
    }
    memcpy(preparation.definition_sha256, audit->definition_sha256,
           LT48_SHA256_SIZE);
    memcpy(preparation.resource_manifest_sha256,
           audit->resource_manifest_sha256, LT48_SHA256_SIZE);
    status = lt48_protocol_plan_sha256(
        read_plan, preparation.plan_sha256
    );
    if (status != LT48_OK) goto cleanup;
    for (size_t state = 0u; state < 3u; ++state) {
        status = lt48_m37_resources_build_recovery_plan(
            resources, (lt48_m37_recovery_state)state, &recovery[state]
        );
        if (status != LT48_OK) goto cleanup;
        preparation.recovery_transaction_count[state] =
            lt48_protocol_plan_transaction_count(recovery[state]);
        status = lt48_protocol_plan_sha256(
            recovery[state], preparation.recovery_sha256[state]
        );
        if (status != LT48_OK) goto cleanup;
    }
    preparation_hex(preparation.resource_manifest_sha256,
                    LT48_SHA256_SIZE, manifest_hex);
    preparation_hex(preparation.plan_sha256, LT48_SHA256_SIZE, plan_hex);
    for (size_t state = 0u; state < 3u; ++state)
        preparation_hex(preparation.recovery_sha256[state],
                        LT48_SHA256_SIZE, recovery_hex[state]);
    confirmation_size = snprintf(
        NULL, 0,
        "labtool48-m37-read-v2\nreadiness=transport-initialize\n"
        "selector=%s\noutput=%s\n"
        "bus=%u\naddress=%u\nstart=%u\nlength=%u\ntimeout=%u\n"
        "manifest=%s\nplan=%s\nrecovery-0=%s\nrecovery-1=%s\n"
        "recovery-2=%s\n",
        selector, output_path, bus, address, start, length, timeout_ms,
        manifest_hex, plan_hex, recovery_hex[0], recovery_hex[1],
        recovery_hex[2]
    );
    if (confirmation_size < 0) {
        status = LT48_FORMAT_ERROR;
        goto cleanup;
    }
    confirmation_text = malloc((size_t)confirmation_size + 1u);
    if (confirmation_text == NULL) {
        status = LT48_NO_MEMORY;
        goto cleanup;
    }
    (void)snprintf(
        confirmation_text, (size_t)confirmation_size + 1u,
        "labtool48-m37-read-v2\nreadiness=transport-initialize\n"
        "selector=%s\noutput=%s\n"
        "bus=%u\naddress=%u\nstart=%u\nlength=%u\ntimeout=%u\n"
        "manifest=%s\nplan=%s\nrecovery-0=%s\nrecovery-1=%s\n"
        "recovery-2=%s\n",
        selector, output_path, bus, address, start, length, timeout_ms,
        manifest_hex, plan_hex, recovery_hex[0], recovery_hex[1],
        recovery_hex[2]
    );
    lt48_sha256(
        (const unsigned char *)confirmation_text,
        (size_t)confirmation_size, preparation.confirmation_sha256
    );
    *out_preparation = preparation;
    status = LT48_OK;

cleanup:
    free(confirmation_text);
    for (size_t state = 0u; state < 3u; ++state)
        lt48_protocol_plan_close(recovery[state]);
    return status;
}

static lt48_status validate_fresh_collector(
    const lt48_m37_read_collector *collector
) {
    size_t chunks_received;
    size_t chunk_count;
    size_t bytes_received;
    size_t byte_count;
    if (collector == NULL) return LT48_INVALID_ARGUMENT;
    lt48_m37_read_collector_progress(
        collector, &chunks_received, &chunk_count,
        &bytes_received, &byte_count
    );
    return chunk_count != 0u && byte_count != 0u && chunks_received == 0u &&
           bytes_received == 0u ? LT48_OK : LT48_BUSY;
}

lt48_status lt48_m37_recovery_execute_injected(
    const lt48_protocol_plan *plan,
    const lt48_protocol_transport *transport,
    void *transport_context,
    const lt48_cleanup_action *release_interface,
    lt48_cleanup_result *phase_results,
    size_t phase_result_capacity,
    lt48_m37_recovery_execution_result *out_result
) {
    lt48_protocol_transport uncancellable;
    size_t phase_count;
    size_t index;
    size_t expected_first = 0u;
    lt48_m37_cleanup_phase previous_phase = 0;
    int saw_release = 0;
    struct timespec recovery_start;
    if (plan == NULL || transport == NULL || transport->write == NULL ||
        transport->read == NULL || release_interface == NULL ||
        release_interface->name == NULL || release_interface->run == NULL ||
        phase_results == NULL || out_result == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    phase_count = lt48_protocol_plan_recovery_phase_count(plan);
    if (phase_count == 0u || phase_result_capacity < phase_count) {
        return phase_count == 0u ? LT48_SAFETY_REJECTED : LT48_BUFFER_TOO_SMALL;
    }
    if (strcmp(release_interface->name,
               lt48_m37_cleanup_phase_name(
                   LT48_M37_CLEANUP_RELEASE_INTERFACE)) != 0) {
        return LT48_SAFETY_REJECTED;
    }
    for (index = 0u; index < phase_count; ++index) {
        lt48_m37_cleanup_phase phase;
        size_t first;
        size_t count;
        if (lt48_protocol_plan_get_recovery_phase(
            plan, index, &phase, &first, &count) != LT48_OK ||
            lt48_m37_cleanup_phase_name(phase) == NULL ||
            phase <= previous_phase || first != expected_first ||
            first > lt48_protocol_plan_transaction_count(plan) ||
            count > lt48_protocol_plan_transaction_count(plan) - first ||
            (index == 0u && phase != LT48_M37_CLEANUP_DEVICE_RESET &&
             phase != LT48_M37_CLEANUP_ZERO_ALL_RAILS &&
             phase != LT48_M37_CLEANUP_RELEASE_INTERFACE)) {
            return LT48_SAFETY_REJECTED;
        }
        previous_phase = phase;
        expected_first += count;
        if (phase == LT48_M37_CLEANUP_RELEASE_INTERFACE) {
            if (saw_release || index + 1u != phase_count || count != 0u ||
                first != lt48_protocol_plan_transaction_count(plan)) {
                return LT48_SAFETY_REJECTED;
            }
            saw_release = 1;
        }
    }
    if (!saw_release ||
        expected_first != lt48_protocol_plan_transaction_count(plan)) {
        return LT48_SAFETY_REJECTED;
    }

    memset(out_result, 0, sizeof(*out_result));
    out_result->recovery_status = LT48_OK;
    out_result->first_failure_status = LT48_OK;
    out_result->phase_count = phase_count;
    uncancellable = *transport;
    uncancellable.cancelled = NULL;
    uncancellable.progress_stage = LT48_EXECUTION_READ_RECOVERY;
    recovery_start = execution_now();
    for (index = 0u; index < phase_count; ++index) {
        lt48_m37_cleanup_phase phase;
        const char *name;
        size_t first;
        size_t count;
        size_t capture_count = 0u;
        size_t completed = 0u;
        lt48_status status;
        struct timespec phase_start = execution_now();
        (void)lt48_protocol_plan_get_recovery_phase(
            plan, index, &phase, &first, &count
        );
        name = lt48_m37_cleanup_phase_name(phase);
        if (uncancellable.progress != NULL)
            uncancellable.progress(
                transport_context,
                phase == LT48_M37_CLEANUP_RELEASE_INTERFACE ?
                    LT48_EXECUTION_INTERFACE_RELEASE :
                    LT48_EXECUTION_READ_RECOVERY,
                0u, count, (unsigned)phase, index + 1u, phase_count
            );
        if (phase == LT48_M37_CLEANUP_RELEASE_INTERFACE) {
            status = release_interface->run(release_interface->context);
            if (uncancellable.progress != NULL)
                uncancellable.progress(
                    transport_context, LT48_EXECUTION_INTERFACE_RELEASE,
                    status == LT48_OK ? 1u : 0u, 1u, (unsigned)phase,
                    index + 1u, phase_count
                );
        } else {
            status = lt48_protocol_plan_execute_range(
                plan, first, count, &uncancellable, transport_context,
                NULL, 0u, &capture_count, &completed
            );
            out_result->completed_transactions += completed;
        }
        phase_results[index].name = name;
        phase_results[index].status = status;
        phase_results[index].started_offset_ms =
            execution_elapsed_ms(recovery_start, phase_start);
        phase_results[index].duration_ms =
            execution_elapsed_ms(phase_start, execution_now());
        if (status != LT48_OK) {
            if (out_result->failure_count == 0u)
                out_result->first_failure_status = status;
            ++out_result->failure_count;
        }
    }
    if (out_result->failure_count != 0u) {
        out_result->recovery_status = LT48_CLEANUP_FAILED;
        return LT48_CLEANUP_FAILED;
    }
    return LT48_OK;
}

lt48_status lt48_m37_snapshot_read_execute_injected(
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *read_plan,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_m37_read_collector *collector,
    const lt48_cleanup_action *release_interface,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_snapshot_read_execution_result *out_result
) {
    lt48_protocol_plan *recovery_plans[3] = {NULL,NULL,NULL};
    lt48_m37_recovery_execution_result recovery_result;
    lt48_m37_cleanup_phase authorization_cleanup[12];
    lt48_m37_plan_evidence authorization = {0};
    uint64_t authorization_issues = 0u;
    size_t authorization_cleanup_count = 0u;
    size_t capture_count = 0u;
    size_t state;
    int operation_started = 0;
    lt48_status status;
    lt48_status return_status = LT48_OK;
    struct timespec execution_start;
    struct timespec operation_finish;
    const char *release_name = lt48_m37_cleanup_phase_name(
        LT48_M37_CLEANUP_RELEASE_INTERFACE
    );
    if (resources == NULL || read_plan == NULL || transport == NULL ||
        transport->write == NULL || transport->read == NULL ||
        release_interface == NULL ||
        release_interface->name == NULL || release_interface->run == NULL ||
        finalization_results == NULL || out_result == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (release_name == NULL ||
        strcmp(release_interface->name, release_name) != 0) {
        return LT48_SAFETY_REJECTED;
    }
    status = lt48_m37_cleanup_recipe(
        1, 1, authorization_cleanup,
        sizeof(authorization_cleanup) / sizeof(authorization_cleanup[0]),
        &authorization_cleanup_count
    );
    if (status != LT48_OK) return status;
    authorization.operation = LT48_M37_READ;
    authorization.start = 0u;
    const lt48_m37_resource_audit *audit=lt48_m37_resources_audit(resources);
    if(!audit)return LT48_SAFETY_REJECTED;
    authorization.length = (audit->device_class==2003u || audit->device_class==2004u)?2u:1u;
    authorization.chunk_size = lt48_eprom_chunk(audit,authorization.length);
    authorization.cleanup_phases = authorization_cleanup;
    authorization.cleanup_phase_count = authorization_cleanup_count;
    status = lt48_m37_resources_validate_plan(
        resources, &authorization, &authorization_issues
    );
    if (status != LT48_OK) return status;
    if (collector != NULL) {
        status = validate_fresh_collector(collector);
        if (status != LT48_OK) return status;
    }
    status = lt48_protocol_plan_m37_recovery_state_internal(
        read_plan, resources, 0u, LT48_CANCELLED,
        &out_result->recovery_state
    );
    if (status != LT48_OK) return status;
    for (state = LT48_M37_RECOVERY_UNOPENED;
         state <= LT48_M37_RECOVERY_POWERED; ++state) {
        status = lt48_m37_resources_build_recovery_plan(
            resources, (lt48_m37_recovery_state)state,
            &recovery_plans[state]
        );
        if (status != LT48_OK) goto cleanup;
        if (finalization_result_capacity <
            lt48_protocol_plan_recovery_phase_count(recovery_plans[state])) {
            status = LT48_BUFFER_TOO_SMALL;
            goto cleanup;
        }
    }

    memset(out_result, 0, sizeof(*out_result));
    operation_started = 1;
    execution_start = execution_now();
    if (collector != NULL)
        out_result->operation_status =
            lt48_protocol_plan_execute_range_collect(
                read_plan, 0u,
                lt48_protocol_plan_transaction_count(read_plan),
                transport, transport_context, collector, NULL, 0u,
                &capture_count,
                &out_result->operation_completed_transactions
            );
    else
        out_result->operation_status = lt48_protocol_plan_execute_range(
            read_plan, 0u,
            lt48_protocol_plan_transaction_count(read_plan),
            transport, transport_context, NULL, 0u, &capture_count,
            &out_result->operation_completed_transactions);
    operation_finish = execution_now();
    out_result->operation_duration_ms =
        execution_elapsed_ms(execution_start, operation_finish);
    if (out_result->operation_status == LT48_OK) {
        struct timespec release_start;
        if (transport->progress != NULL)
            transport->progress(
                transport_context, LT48_EXECUTION_INTERFACE_RELEASE,
                0u, 1u, (unsigned)LT48_M37_CLEANUP_RELEASE_INTERFACE,
                1u, 1u
            );
        release_start = execution_now();
        status = release_interface->run(release_interface->context);
        if (transport->progress != NULL)
            transport->progress(
                transport_context, LT48_EXECUTION_INTERFACE_RELEASE,
                status == LT48_OK ? 1u : 0u, 1u,
                (unsigned)LT48_M37_CLEANUP_RELEASE_INTERFACE, 1u, 1u
            );
        finalization_results[0].name = release_name;
        finalization_results[0].status = status;
        finalization_results[0].started_offset_ms =
            out_result->operation_duration_ms;
        finalization_results[0].duration_ms =
            execution_elapsed_ms(release_start, execution_now());
        out_result->finalization_phase_count = 1u;
        out_result->finalization_failure_count = status == LT48_OK ? 0u : 1u;
        out_result->finalization_status = status == LT48_OK ?
                                          LT48_OK : LT48_CLEANUP_FAILED;
        return_status = out_result->finalization_status;
        goto cleanup;
    }

    status = lt48_protocol_plan_m37_recovery_state_internal(
        read_plan, resources,
        out_result->operation_completed_transactions,
        out_result->operation_status, &out_result->recovery_state
    );
    if (status != LT48_OK) {
        return_status = status;
        goto cleanup;
    }
    out_result->used_recovery_plan = 1;
    status = lt48_m37_recovery_execute_injected(
        recovery_plans[out_result->recovery_state], transport,
        transport_context, release_interface, finalization_results,
        finalization_result_capacity, &recovery_result
    );
    out_result->finalization_status = recovery_result.recovery_status;
    out_result->recovery_completed_transactions =
        recovery_result.completed_transactions;
    out_result->finalization_phase_count = recovery_result.phase_count;
    out_result->finalization_failure_count = recovery_result.failure_count;
    for (size_t index = 0u;
         index < out_result->finalization_phase_count; ++index)
        finalization_results[index].started_offset_ms +=
            out_result->operation_duration_ms;
    return_status = status == LT48_OK ?
                    out_result->operation_status : LT48_CLEANUP_FAILED;

cleanup:
    if (operation_started)
        out_result->total_duration_ms =
            execution_elapsed_ms(execution_start, execution_now());
    for (state = 0u; state < 3u; ++state)
        lt48_protocol_plan_close(recovery_plans[state]);
    return operation_started ? return_status : status;
}

lt48_status lt48_m37_compare_execute_injected(
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *compare_plan,
    uint32_t expected_start,
    const unsigned char *expected,
    size_t expected_size,
    lt48_m37_compare_decision_fn decide,
    void *decision_context,
    const lt48_protocol_transport *transport,
    void *transport_context,
    const lt48_cleanup_action *release_interface,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_compare_execution_result *out_result
) {
    lt48_m37_read_collector *collector = NULL;
    const unsigned char *actual = NULL;
    uint32_t actual_start = 0u;
    size_t actual_size = 0u;
    lt48_status execution_status;
    lt48_status status;
    if (resources == NULL || compare_plan == NULL || expected == NULL ||
        expected_size == 0u || expected_size > UINT32_MAX ||
        (uint64_t)expected_start + (uint64_t)expected_size >
            UINT64_C(0x100000000) || transport == NULL ||
        release_interface == NULL || finalization_results == NULL ||
        out_result == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    memset(out_result, 0, sizeof(*out_result));
    status = lt48_m37_read_collector_create(compare_plan, &collector);
    if (status != LT48_OK) return status;
    execution_status = lt48_m37_snapshot_read_execute_injected(
        resources, compare_plan, transport, transport_context, collector,
        release_interface, finalization_results,
        finalization_result_capacity, &out_result->execution
    );
    if (out_result->execution.operation_status != LT48_OK) {
        lt48_m37_read_collector_close(collector);
        return execution_status;
    }
    status = lt48_m37_read_collector_data(
        collector, &actual_start, &actual, &actual_size
    );
    if (status == LT48_OK &&
        (actual_start != expected_start || actual_size != expected_size)) {
        status = LT48_SAFETY_REJECTED;
    }
    if (status == LT48_OK) {
        status = lt48_m37_compare_evaluate(
            expected_start, expected, actual, expected_size, decide,
            decision_context, &out_result->comparison
        );
    }
    if (status == LT48_OK) out_result->comparison_available = 1;
    lt48_m37_read_collector_close(collector);
    if (status != LT48_OK) return status;
    return execution_status;
}

static lt48_status validate_recovery(
    const lt48_cleanup_action *actions,
    size_t action_count,
    lt48_cleanup_result *results,
    size_t result_capacity
) {
    lt48_m37_cleanup_phase phases[12];
    size_t phase_count;
    size_t index;
    lt48_status status = lt48_m37_cleanup_recipe(
        1, 1, phases, sizeof(phases) / sizeof(phases[0]), &phase_count
    );
    if (status != LT48_OK) return status;
    if (action_count != phase_count || actions == NULL) {
        return LT48_SAFETY_REJECTED;
    }
    if (result_capacity < action_count) return LT48_BUFFER_TOO_SMALL;
    if (results == NULL) return LT48_INVALID_ARGUMENT;
    for (index = 0u; index < action_count; ++index) {
        const char *expected = lt48_m37_cleanup_phase_name(phases[index]);
        if (actions[index].name == NULL || actions[index].run == NULL ||
            expected == NULL || strcmp(actions[index].name, expected) != 0) {
            return LT48_SAFETY_REJECTED;
        }
    }
    return LT48_OK;
}

lt48_status lt48_m37_read_execute_injected(
    const lt48_protocol_plan *plan,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_m37_read_collector *collector,
    const lt48_cleanup_action *recovery_actions,
    size_t recovery_action_count,
    lt48_cleanup_result *recovery_results,
    size_t recovery_result_capacity,
    lt48_m37_read_execution_result *out_result
) {
    size_t chunks_received;
    size_t chunk_count;
    size_t bytes_received;
    size_t byte_count;
    size_t capture_count = 0u;
    size_t result_count = 0u;
    size_t failure_count = 0u;
    lt48_status status;
    if (plan == NULL || transport == NULL || collector == NULL ||
        out_result == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    memset(out_result, 0, sizeof(*out_result));
    out_result->operation_status = LT48_OK;
    out_result->recovery_status = LT48_OK;
    status = validate_recovery(
        recovery_actions, recovery_action_count, recovery_results,
        recovery_result_capacity
    );
    if (status != LT48_OK) return status;
    lt48_m37_read_collector_progress(
        collector, &chunks_received, &chunk_count,
        &bytes_received, &byte_count
    );
    if (chunk_count == 0u || byte_count == 0u || chunks_received != 0u ||
        bytes_received != 0u) {
        return LT48_BUSY;
    }
    out_result->operation_status = lt48_protocol_plan_execute_range_collect(
        plan, 0u, lt48_protocol_plan_transaction_count(plan),
        transport, transport_context, collector, NULL, 0u,
        &capture_count, &out_result->completed_transactions
    );
    if (out_result->operation_status == LT48_OK) return LT48_OK;
    out_result->recovery_status = lt48_cleanup_run(
        recovery_actions, recovery_action_count, recovery_results,
        recovery_result_capacity, &result_count, &failure_count
    );
    (void)result_count;
    out_result->recovery_failure_count = failure_count;
    if (out_result->recovery_status != LT48_OK) return LT48_CLEANUP_FAILED;
    return out_result->operation_status;
}

/* Offline captured-profile orchestration. The public snapshot executor still
 * performs live authorization independently; this helper does not bypass it.
 * Transport-failure recovery remains the caller's responsibility. */
lt48_status lt48_m37_read_id_loop_candidate_internal(
    const lt48_m37_resources *resources,
    const lt48_protocol_transport *transport, void *transport_context,
    lt48_m37_id_decide_internal decide, void *decision_context,
    lt48_m37_read_collector **out_collector,
    lt48_m37_id_loop_result_internal *out_result
) {
    lt48_protocol_plan *initial = NULL, *retry = NULL, *ignore = NULL;
    lt48_protocol_plan *before_choice = NULL, *after_abort = NULL;
    const lt48_protocol_plan *current;
    const lt48_protocol_plan *last_executed = NULL;
    int safely_cleaned = 0;
    lt48_m37_read_collector *collector = NULL;
    lt48_m37_recovery_execution_result cleanup_result;
    uint16_t expected_manufacturer, expected_device;
    lt48_status status;
    if (!resources || !transport || !transport->write || !transport->read ||
        !out_collector || !out_result) return LT48_INVALID_ARGUMENT;
    *out_collector = NULL;
    memset(out_result, 0, sizeof(*out_result));
    out_result->cleanup_status = LT48_OK;
    /* Retry/Ignore constructors pin the captured definition before any I/O. */
    status = lt48_m37_resources_build_id_retry_candidate_internal(resources, &retry);
    if (status != LT48_OK) goto done;
    status = lt48_m37_resources_build_id_ignore_candidate_internal(resources, &ignore);
    if (status != LT48_OK) goto done;
    status = lt48_m37_resources_build_offline_read_plan(resources, 0u, 16384u, &initial);
    if (status != LT48_OK) goto done;
    status = lt48_m37_resources_build_id_mismatch_cleanup_internal(resources, 0, &before_choice);
    if (status != LT48_OK) goto done;
    status = lt48_m37_resources_build_id_mismatch_cleanup_internal(resources, 1, &after_abort);
    if (status != LT48_OK) goto done;
    status = lt48_m37_resources_catalogue_ids_internal(resources, &expected_manufacturer, &expected_device);
    if (status != LT48_OK) goto done;
    current = initial;
    for (;;) {
        lt48_runtime_capture captures[2];
        size_t captured = 0u, completed = 0u;
        lt48_m37_id_choice_internal choice;
        int cancelled;
        lt48_m37_read_collector_close(collector);
        collector = NULL;
        status = lt48_m37_read_collector_create(current, &collector);
        if (status != LT48_OK) goto done;
        ++out_result->attempts;
        safely_cleaned = 0;
        last_executed = current;
        out_result->last_attempt_transaction_count =
            lt48_protocol_plan_transaction_count(current);
        status = lt48_protocol_plan_execute_range_collect(
            current, 0u, lt48_protocol_plan_transaction_count(current),
            transport, transport_context, collector, captures, 2u,
            &captured, &completed);
        out_result->operation_completed_transactions += completed;
        out_result->last_attempt_completed_transactions = completed;
        out_result->operation_status = status;
        if (status != LT48_DEVICE_MISMATCH) break;
        if (captured != 2u || captures[0].buffer != 3u || captures[1].buffer != 4u ||
            captures[0].length != 1u || captures[1].length != 1u) {
            status = LT48_PROTOCOL_ERROR;
            break;
        }
        status = lt48_protocol_plan_execute_id_cleanup_internal(
            before_choice, transport, transport_context, &cleanup_result);
        out_result->cleanup_completed_transactions += cleanup_result.completed_transactions;
        out_result->cleanup_status = status;
        if (status != LT48_OK) break;
        safely_cleaned = 1;
        cancelled = transport->cancelled && transport->cancelled(transport_context);
        choice = (!cancelled && decide) ? decide(
            decision_context, expected_manufacturer, expected_device,
            captures[0].values[0], captures[1].values[0]) : LT48_M37_ID_ABORT_INTERNAL;
        if (!cancelled && decide) ++out_result->decisions;
        /* A close/cancel during a dialog takes the same post-Abort cleanup. */
        cancelled = cancelled || (transport->cancelled && transport->cancelled(transport_context));
        if (cancelled || choice == LT48_M37_ID_ABORT_INTERNAL ||
            (choice != LT48_M37_ID_RETRY_INTERNAL && choice != LT48_M37_ID_IGNORE_INTERNAL)) {
            safely_cleaned = 0;
            status = lt48_protocol_plan_execute_id_cleanup_internal(
                after_abort, transport, transport_context, &cleanup_result);
            out_result->cleanup_completed_transactions += cleanup_result.completed_transactions;
            out_result->cleanup_status = status;
            if (status == LT48_OK) {
                safely_cleaned = 1;
                status = cancelled ? LT48_CANCELLED : LT48_DEVICE_MISMATCH;
            }
            break;
        }
        current = choice == LT48_M37_ID_RETRY_INTERNAL ? retry : ignore;
    }
done:
    if (out_result->attempts == 0u) out_result->operation_status = status;
    if (last_executed && status != LT48_OK && !safely_cleaned) {
        out_result->needs_failure_recovery = 1;
        lt48_status state_status = lt48_protocol_plan_m37_recovery_state_internal(
            last_executed, resources,
            out_result->last_attempt_completed_transactions, status,
            &out_result->recovery_state);
        out_result->recovery_state_valid = state_status == LT48_OK;
        if (state_status != LT48_OK) status = state_status;
    }
    *out_collector = collector;
    lt48_protocol_plan_close(initial);
    lt48_protocol_plan_close(retry);
    lt48_protocol_plan_close(ignore);
    lt48_protocol_plan_close(before_choice);
    lt48_protocol_plan_close(after_abort);
    return status;
}

/* Bind the recovered decision loop to the normal operation/release owner.
 * Range variants must be separately recovered before using this full-read loop. */
lt48_status lt48_m37_read_id_owned_internal(
    const lt48_m37_resources *resources, const lt48_protocol_plan *plan,
    const lt48_protocol_transport *transport, void *transport_context,
    lt48_m37_id_decision_fn decide, void *decision_context,
    lt48_m37_read_collector *collector, const lt48_cleanup_action *release,
    lt48_cleanup_result *results, size_t capacity,
    lt48_m37_snapshot_read_execution_result *out_result
) {
    lt48_protocol_plan *full = NULL, *recovery = NULL;
    lt48_m37_read_collector *received = NULL;
    lt48_m37_id_loop_result_internal loop = {0};
    lt48_m37_recovery_execution_result recovered = {0};
    unsigned char expected[LT48_SHA256_SIZE], actual[LT48_SHA256_SIZE];
    lt48_status status;
    if (!resources || !plan || !transport || !collector || !release ||
        !release->run || !results || capacity < 12 || !out_result)
        return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    status = validate_fresh_collector(collector);
    if (status != LT48_OK) return status;
    status = lt48_m37_resources_build_read_plan(resources, 0, 16384, &full);
    if (status == LT48_OK) status = lt48_protocol_plan_sha256(full, expected);
    if (status == LT48_OK) status = lt48_protocol_plan_sha256(plan, actual);
    lt48_protocol_plan_close(full);
    if (status != LT48_OK) return status;
    if (memcmp(actual, expected, sizeof(actual))) return LT48_UNSUPPORTED;
    status = lt48_m37_read_id_loop_candidate_internal(resources, transport,
        transport_context, decide, decision_context, &received, &loop);
    out_result->operation_status = loop.operation_status;
    if (status == LT48_CANCELLED) out_result->operation_status = LT48_CANCELLED;
    out_result->operation_completed_transactions = loop.operation_completed_transactions;
    if (status == LT48_OK && received) {
        lt48_m37_read_collector_replace_internal(collector, received);
        received = NULL;
    }
    lt48_m37_read_collector_close(received);
    if (loop.needs_failure_recovery && !loop.recovery_state_valid) {
        out_result->finalization_failure_count = 1;
        out_result->finalization_status = LT48_CLEANUP_FAILED;
    }
    if (loop.needs_failure_recovery && loop.recovery_state_valid) {
        out_result->used_recovery_plan = 1;
        out_result->recovery_state = loop.recovery_state;
        lt48_status recovery_status = lt48_m37_resources_build_recovery_plan(
            resources, loop.recovery_state, &recovery);
        if (recovery_status == LT48_OK) {
            recovery_status = lt48_m37_recovery_execute_injected(recovery,
                transport, transport_context, release, results, capacity, &recovered);
            out_result->recovery_completed_transactions = recovered.completed_transactions;
            out_result->finalization_status = recovered.recovery_status;
            out_result->finalization_phase_count = recovered.phase_count;
            out_result->finalization_failure_count = recovered.failure_count;
            lt48_protocol_plan_close(recovery);
            return recovery_status == LT48_OK ? status : LT48_CLEANUP_FAILED;
        }
        /* Construction failure still releases the interface below. */
        out_result->finalization_failure_count = 1;
        out_result->finalization_status = LT48_CLEANUP_FAILED;
    }
    lt48_status release_status = release->run(release->context);
    results[0].name = "release-interface";
    results[0].status = release_status;
    out_result->finalization_phase_count = 1;
    if (release_status != LT48_OK || loop.cleanup_status != LT48_OK) {
        ++out_result->finalization_failure_count;
        out_result->finalization_status = LT48_CLEANUP_FAILED;
    }
    return out_result->finalization_failure_count ? LT48_CLEANUP_FAILED : status;
}
