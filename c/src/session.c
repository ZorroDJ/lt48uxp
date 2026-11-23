#include <labtool48/labtool48.h>

#include <stddef.h>

static lt48_status validate_cleanup(
    const lt48_cleanup_action *actions, size_t action_count,
    lt48_cleanup_result *results, size_t result_capacity
) {
    size_t index;
    if ((action_count && !actions) || (result_capacity && !results)) {
        return LT48_INVALID_ARGUMENT;
    }
    if (result_capacity < action_count) return LT48_BUFFER_TOO_SMALL;
    for (index = 0; index < action_count; ++index) {
        if (!actions[index].name || !actions[index].run) {
            return LT48_INVALID_ARGUMENT;
        }
    }
    return LT48_OK;
}

lt48_status lt48_cleanup_run(
    const lt48_cleanup_action *actions,
    size_t action_count,
    lt48_cleanup_result *results,
    size_t result_capacity,
    size_t *out_result_count,
    size_t *out_failure_count
) {
    size_t index;
    size_t failures = 0u;
    lt48_status validation;
    if (!out_result_count || !out_failure_count) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_result_count = action_count;
    *out_failure_count = 0u;
    validation = validate_cleanup(actions, action_count, results,
                                  result_capacity);
    if (validation != LT48_OK) return validation;
    for (index = 0; index < action_count; ++index) {
        lt48_status status = actions[index].run(actions[index].context);
        results[index].name = actions[index].name;
        results[index].status = status;
        results[index].started_offset_ms = 0.0;
        results[index].duration_ms = 0.0;
        if (status != LT48_OK) ++failures;
    }
    *out_failure_count = failures;
    return failures ? LT48_CLEANUP_FAILED : LT48_OK;
}

lt48_status lt48_session_run(
    lt48_session_operation_fn operation,
    void *operation_context,
    const lt48_cleanup_action *cleanup_actions,
    size_t cleanup_action_count,
    lt48_cleanup_result *cleanup_results,
    size_t cleanup_result_capacity,
    lt48_session_result *out_session
) {
    lt48_status status;
    size_t result_count = 0u;
    size_t failure_count = 0u;
    if (!operation || !out_session) return LT48_INVALID_ARGUMENT;
    out_session->operation_status = LT48_OK;
    out_session->cleanup_status = LT48_OK;
    out_session->cleanup_failure_count = 0u;
    status = validate_cleanup(cleanup_actions, cleanup_action_count,
                              cleanup_results, cleanup_result_capacity);
    if (status != LT48_OK) return status;
    out_session->operation_status = operation(operation_context);
    out_session->cleanup_status = lt48_cleanup_run(
        cleanup_actions, cleanup_action_count, cleanup_results,
        cleanup_result_capacity, &result_count, &failure_count
    );
    (void)result_count;
    out_session->cleanup_failure_count = failure_count;
    if (out_session->cleanup_status != LT48_OK) return LT48_CLEANUP_FAILED;
    return out_session->operation_status;
}
