#include <labtool48/labtool48.h>

#include <string.h>

static int sha256_check(void) {
    static const unsigned char expected[LT48_SHA256_SIZE] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };
    unsigned char actual[LT48_SHA256_SIZE];
    static const unsigned char input[] = {'a', 'b', 'c'};
    lt48_sha256(input, sizeof(input), actual);
    return memcmp(actual, expected, sizeof(expected)) == 0;
}

static int operation_surface_check(void) {
    for (unsigned value = 1u; value <= LT48_OPERATION_KIND_COUNT; ++value) {
        lt48_operation_kind parsed = (lt48_operation_kind)0;
        const char *name = lt48_operation_kind_name(
            (lt48_operation_kind)value
        );
        if (name == NULL || name[0] == '\0' ||
            lt48_operation_kind_parse(name, &parsed) != LT48_OK ||
            parsed != (lt48_operation_kind)value)
            return 0;
    }
    return lt48_operation_kind_name((lt48_operation_kind)0) == NULL;
}

static int buffer_model_check(void) {
    static const unsigned char expected[16] = {
        0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x12, 0x34, 0x56, 0x78, 0xff, 0xff, 0xff, 0xff
    };
    lt48_buffer *buffer = NULL;
    uint64_t value = 0u;
    int passed =
        lt48_buffer_create(sizeof(expected), 0xffu, &buffer) == LT48_OK &&
        lt48_buffer_fill(buffer, 4u, 4u, 0x00u) == LT48_OK &&
        lt48_buffer_set_word(buffer, 8u, 4u, LT48_BUFFER_BIG_ENDIAN,
                             0x12345678u) == LT48_OK &&
        lt48_buffer_get_word(buffer, 8u, 4u, LT48_BUFFER_BIG_ENDIAN,
                             &value) == LT48_OK &&
        value == 0x12345678u &&
        lt48_buffer_size(buffer) == sizeof(expected) &&
        memcmp(lt48_buffer_data(buffer), expected, sizeof(expected)) == 0;
    lt48_buffer_close(buffer);
    return passed;
}

static int jtag_planner_check(void) {
    static const unsigned char tdi[] = {0x0bu};
    const lt48_jtag_scan_request request = {
        LT48_JTAG_SCAN_IR, tdi, sizeof(tdi), 4u,
        100000u, 250000u, LT48_JTAG_TEST_LOGIC_RESET
    };
    lt48_jtag_plan *plan = NULL;
    lt48_jtag_plan_info info;
    int passed = lt48_jtag_plan_scan(&request, &plan) == LT48_OK &&
        lt48_jtag_plan_get_info(plan, &info) == LT48_OK &&
        info.bit_count == 4u && info.step_count == 20u &&
        info.sampled_step_count == 4u &&
        info.cancellation_recovery_step_count == 5u &&
        info.final_state == LT48_JTAG_TEST_LOGIC_RESET &&
        !info.transport_ready;
    lt48_jtag_plan_close(plan);
    return passed;
}

static lt48_status failing_progress_observer(
    void *context, const lt48_progress_event *event
) {
    size_t *calls = context;
    if (calls != NULL) ++*calls;
    (void)event;
    return LT48_RUNTIME_ERROR;
}

static int progress_model_check(void) {
    lt48_progress_emitter *emitter = NULL;
    lt48_progress_state state;
    lt48_progress_event event;
    lt48_progress_event_input input = {
        LT48_PROGRESS_OPERATION, "read", LT48_PROGRESS_STARTED,
        NULL, 0, 0u, 0u, NULL, NULL, NULL
    };
    size_t observer_calls = 0u;
    int passed = lt48_progress_emitter_create(
        failing_progress_observer, &observer_calls, &emitter
    ) == LT48_OK;
    lt48_progress_state_init(&state);
    if (passed)
        passed = lt48_progress_emit(emitter, &input, &event) == LT48_OK &&
            event.sequence == 0u &&
            lt48_progress_state_consume(&state, &event) == LT48_OK;
    input.scope = LT48_PROGRESS_UNITS;
    input.name = "read-data";
    input.state = LT48_PROGRESS_RUNNING;
    input.has_units = 1;
    input.completed_units = 4u;
    input.total_units = 16u;
    input.unit = "transactions";
    if (passed)
        passed = lt48_progress_emit(emitter, &input, &event) == LT48_OK &&
            event.sequence == 1u &&
            lt48_progress_state_consume(&state, &event) == LT48_OK;
    if (passed)
        passed = observer_calls == 2u &&
            lt48_progress_observer_error_count(emitter) == 2u &&
            state.operation_state == LT48_PROGRESS_STARTED &&
            state.has_units && state.completed_units == 4u &&
            state.total_units == 16u &&
            strcmp(state.current_phase, "read-data") == 0 &&
            strcmp(state.unit, "transactions") == 0;
    lt48_progress_emitter_close(emitter);
    return passed;
}

lt48_status lt48_diagnostics_self_test(
    const char *installation_path, lt48_diagnostics_report *out_report
) {
    lt48_installation *installation = NULL;
    if (out_report == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->schema_version = 1u;
    out_report->sha256_check_passed = sha256_check();
    out_report->operation_surface_check_passed = operation_surface_check();
    out_report->buffer_model_check_passed = buffer_model_check();
    out_report->jtag_planner_check_passed = jtag_planner_check();
    out_report->progress_model_check_passed = progress_model_check();
    out_report->installation_status = LT48_OK;
    if (installation_path != NULL && installation_path[0] != '\0') {
        out_report->installation_requested = 1;
        out_report->installation_status = lt48_installation_open(
            installation_path, &installation
        );
        if (out_report->installation_status == LT48_OK)
            out_report->installation_status = lt48_installation_audit_all(
                installation, &out_report->installation_audit
            );
        lt48_installation_close(installation);
    }
    out_report->passed = out_report->sha256_check_passed &&
        out_report->operation_surface_check_passed &&
        out_report->buffer_model_check_passed &&
        out_report->jtag_planner_check_passed &&
        out_report->progress_model_check_passed &&
        (!out_report->installation_requested ||
         (out_report->installation_status == LT48_OK &&
          out_report->installation_audit.catalogue_device_count != 0u &&
          out_report->installation_audit.failed_device_count == 0u));
    return LT48_OK;
}
