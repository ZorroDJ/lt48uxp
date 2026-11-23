#include <labtool48/labtool48.h>

#include "internal.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

struct lt48_jtag_plan {
    lt48_jtag_step *steps;
    size_t step_count;
    size_t step_capacity;
    unsigned char *json;
    size_t json_size;
    lt48_jtag_plan_info info;
};

static lt48_jtag_tap_state next_state(lt48_jtag_tap_state state, uint8_t tms) {
    static const lt48_jtag_tap_state transitions[16][2] = {
        {LT48_JTAG_RUN_TEST_IDLE, LT48_JTAG_TEST_LOGIC_RESET},
        {LT48_JTAG_RUN_TEST_IDLE, LT48_JTAG_SELECT_DR_SCAN},
        {LT48_JTAG_CAPTURE_DR, LT48_JTAG_SELECT_IR_SCAN},
        {LT48_JTAG_SHIFT_DR, LT48_JTAG_EXIT1_DR},
        {LT48_JTAG_SHIFT_DR, LT48_JTAG_EXIT1_DR},
        {LT48_JTAG_PAUSE_DR, LT48_JTAG_UPDATE_DR},
        {LT48_JTAG_PAUSE_DR, LT48_JTAG_EXIT2_DR},
        {LT48_JTAG_SHIFT_DR, LT48_JTAG_UPDATE_DR},
        {LT48_JTAG_RUN_TEST_IDLE, LT48_JTAG_SELECT_DR_SCAN},
        {LT48_JTAG_CAPTURE_IR, LT48_JTAG_TEST_LOGIC_RESET},
        {LT48_JTAG_SHIFT_IR, LT48_JTAG_EXIT1_IR},
        {LT48_JTAG_SHIFT_IR, LT48_JTAG_EXIT1_IR},
        {LT48_JTAG_PAUSE_IR, LT48_JTAG_UPDATE_IR},
        {LT48_JTAG_PAUSE_IR, LT48_JTAG_EXIT2_IR},
        {LT48_JTAG_SHIFT_IR, LT48_JTAG_UPDATE_IR},
        {LT48_JTAG_RUN_TEST_IDLE, LT48_JTAG_SELECT_DR_SCAN}
    };
    return transitions[(unsigned)state][tms ? 1u : 0u];
}

const char *lt48_jtag_tap_state_name(lt48_jtag_tap_state state) {
    static const char *const names[16] = {
        "test-logic-reset", "run-test-idle", "select-dr-scan", "capture-dr",
        "shift-dr", "exit1-dr", "pause-dr", "exit2-dr", "update-dr",
        "select-ir-scan", "capture-ir", "shift-ir", "exit1-ir", "pause-ir",
        "exit2-ir", "update-ir"
    };
    return (unsigned)state < 16u ? names[(unsigned)state] : NULL;
}

lt48_status lt48_jtag_cancel_recovery(
    lt48_jtag_tap_state start_state, lt48_jtag_step *steps,
    size_t step_capacity, size_t *out_step_count
) {
    lt48_jtag_tap_state state = start_state;
    if ((unsigned)start_state >= 16u || steps == NULL ||
        step_capacity < 5u || out_step_count == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_step_count = 0u;
    for (size_t index = 0u; index < 5u; ++index) {
        steps[index].state_before = state;
        steps[index].state_after = next_state(state, 1u);
        steps[index].tms = 1u;
        steps[index].tdi = 0u;
        steps[index].sample_tdo = 0;
        steps[index].scan_bit_index = SIZE_MAX;
        state = steps[index].state_after;
    }
    *out_step_count = 5u;
    return state == LT48_JTAG_TEST_LOGIC_RESET ? LT48_OK : LT48_PROTOCOL_ERROR;
}

static lt48_status append_step(
    lt48_jtag_plan *plan, lt48_jtag_tap_state *state, uint8_t tms,
    uint8_t tdi, int sample_tdo, size_t scan_bit_index
) {
    lt48_jtag_step *step;
    if (plan->step_count >= plan->step_capacity) return LT48_NO_MEMORY;
    step = &plan->steps[plan->step_count++];
    step->state_before = *state;
    step->state_after = next_state(*state, tms);
    step->tms = tms;
    step->tdi = tdi;
    step->sample_tdo = sample_tdo;
    step->scan_bit_index = scan_bit_index;
    *state = step->state_after;
    if (sample_tdo) ++plan->info.sampled_step_count;
    return LT48_OK;
}

static int append_text(
    unsigned char *buffer, size_t capacity, size_t *used,
    const char *format, ...
) {
    va_list arguments;
    int count;
    if (*used >= capacity) return 0;
    va_start(arguments, format);
    count = vsnprintf((char *)buffer + *used, capacity - *used,
                      format, arguments);
    va_end(arguments);
    if (count < 0 || (size_t)count >= capacity - *used) return 0;
    *used += (size_t)count;
    return 1;
}

static lt48_status build_json(lt48_jtag_plan *plan) {
    size_t capacity;
    size_t used = 0u;
    const char *kind = plan->info.kind == LT48_JTAG_SCAN_IR ? "ir" : "dr";
    const char *final_name = lt48_jtag_tap_state_name(plan->info.final_state);
    if (plan->step_count > (SIZE_MAX - 512u) / 192u) return LT48_NO_MEMORY;
    capacity = 512u + plan->step_count * 192u;
    plan->json = malloc(capacity);
    if (plan->json == NULL) return LT48_NO_MEMORY;
    if (!append_text(plan->json, capacity, &used,
        "{\"schema\":1,\"kind\":\"%s\",\"bit_count\":%zu,"
        "\"tck_hz\":%u,\"profile_max_tck_hz\":%u,"
        "\"safe_end_state\":\"%s\",\"transport_ready\":false,"
        "\"cancellation_recovery\":{\"policy\":\"five-tms-high-clocks\","
        "\"step_count\":5,\"final_state\":\"test-logic-reset\"},\"steps\":[",
        kind, plan->info.bit_count, plan->info.tck_hz,
        plan->info.profile_max_tck_hz, final_name)) return LT48_NO_MEMORY;
    for (size_t index = 0u; index < plan->step_count; ++index) {
        const lt48_jtag_step *step = &plan->steps[index];
        if (!append_text(plan->json, capacity, &used,
            "%s{\"state_before\":\"%s\",\"tms\":%u,\"tdi\":%u,"
            "\"sample_tdo\":%s,\"scan_bit_index\":",
            index == 0u ? "" : ",",
            lt48_jtag_tap_state_name(step->state_before), step->tms, step->tdi,
            step->sample_tdo ? "true" : "false")) return LT48_NO_MEMORY;
        if (step->sample_tdo) {
            if (!append_text(plan->json, capacity, &used, "%zu",
                             step->scan_bit_index)) return LT48_NO_MEMORY;
        } else if (!append_text(plan->json, capacity, &used, "null")) {
            return LT48_NO_MEMORY;
        }
        if (!append_text(plan->json, capacity, &used,
            ",\"state_after\":\"%s\"}",
            lt48_jtag_tap_state_name(step->state_after))) return LT48_NO_MEMORY;
    }
    if (!append_text(plan->json, capacity, &used, "]}")) return LT48_NO_MEMORY;
    plan->json_size = used;
    lt48_sha256(plan->json, plan->json_size, plan->info.sha256);
    return LT48_OK;
}

lt48_status lt48_jtag_plan_scan(
    const lt48_jtag_scan_request *request, lt48_jtag_plan **out_plan
) {
    lt48_jtag_plan *plan;
    lt48_jtag_tap_state state = LT48_JTAG_TEST_LOGIC_RESET;
    size_t required_size;
    lt48_status status = LT48_OK;
    if (request == NULL || out_plan == NULL) return LT48_INVALID_ARGUMENT;
    *out_plan = NULL;
    if ((request->kind != LT48_JTAG_SCAN_IR &&
         request->kind != LT48_JTAG_SCAN_DR) ||
        request->bit_count == 0u || request->tdi == NULL ||
        request->tck_hz == 0u || request->profile_max_tck_hz == 0u ||
        request->tck_hz > request->profile_max_tck_hz ||
        (request->safe_end_state != LT48_JTAG_TEST_LOGIC_RESET &&
         request->safe_end_state != LT48_JTAG_RUN_TEST_IDLE))
        return LT48_INVALID_ARGUMENT;
    required_size = request->bit_count / 8u +
                    (request->bit_count % 8u != 0u ? 1u : 0u);
    if (request->tdi_size != required_size || request->bit_count > SIZE_MAX - 20u)
        return LT48_INVALID_ARGUMENT;
    if (request->bit_count % 8u != 0u &&
        (request->tdi[required_size - 1u] >> (request->bit_count % 8u)) != 0u)
        return LT48_INVALID_ARGUMENT;
    plan = calloc(1u, sizeof(*plan));
    if (plan == NULL) return LT48_NO_MEMORY;
    plan->step_capacity = request->bit_count + 20u;
    plan->steps = calloc(plan->step_capacity, sizeof(*plan->steps));
    if (plan->steps == NULL) {
        free(plan);
        return LT48_NO_MEMORY;
    }
    plan->info.kind = request->kind;
    plan->info.bit_count = request->bit_count;
    plan->info.tck_hz = request->tck_hz;
    plan->info.profile_max_tck_hz = request->profile_max_tck_hz;
    plan->info.transport_ready = 0;
    plan->info.cancellation_recovery_step_count = 5u;
    for (size_t index = 0u; index < 5u && status == LT48_OK; ++index)
        status = append_step(plan, &state, 1u, 0u, 0, SIZE_MAX);
    if (status == LT48_OK)
        status = append_step(plan, &state, 0u, 0u, 0, SIZE_MAX);
    if (status == LT48_OK)
        status = append_step(plan, &state, 1u, 0u, 0, SIZE_MAX);
    if (request->kind == LT48_JTAG_SCAN_IR && status == LT48_OK)
        status = append_step(plan, &state, 1u, 0u, 0, SIZE_MAX);
    if (status == LT48_OK)
        status = append_step(plan, &state, 0u, 0u, 0, SIZE_MAX);
    if (status == LT48_OK)
        status = append_step(plan, &state, 0u, 0u, 0, SIZE_MAX);
    for (size_t bit = 0u; bit < request->bit_count && status == LT48_OK; ++bit) {
        uint8_t tdi = (request->tdi[bit / 8u] >> (bit % 8u)) & 1u;
        status = append_step(plan, &state,
                             bit + 1u == request->bit_count ? 1u : 0u,
                             tdi, 1, bit);
    }
    if (status == LT48_OK)
        status = append_step(plan, &state, 1u, 0u, 0, SIZE_MAX);
    if (status == LT48_OK && request->safe_end_state == LT48_JTAG_RUN_TEST_IDLE)
        status = append_step(plan, &state, 0u, 0u, 0, SIZE_MAX);
    if (status == LT48_OK && request->safe_end_state == LT48_JTAG_TEST_LOGIC_RESET)
        for (size_t index = 0u; index < 5u && status == LT48_OK; ++index)
            status = append_step(plan, &state, 1u, 0u, 0, SIZE_MAX);
    if (status != LT48_OK || state != request->safe_end_state) {
        lt48_jtag_plan_close(plan);
        return status == LT48_OK ? LT48_PROTOCOL_ERROR : status;
    }
    plan->info.step_count = plan->step_count;
    plan->info.final_state = state;
    status = build_json(plan);
    if (status != LT48_OK) {
        lt48_jtag_plan_close(plan);
        return status;
    }
    *out_plan = plan;
    return LT48_OK;
}

void lt48_jtag_plan_close(lt48_jtag_plan *plan) {
    if (plan == NULL) return;
    free(plan->steps);
    free(plan->json);
    free(plan);
}

lt48_status lt48_jtag_plan_get_info(
    const lt48_jtag_plan *plan, lt48_jtag_plan_info *out_info
) {
    if (plan == NULL || out_info == NULL) return LT48_INVALID_ARGUMENT;
    *out_info = plan->info;
    return LT48_OK;
}

lt48_status lt48_jtag_plan_get_step(
    const lt48_jtag_plan *plan, size_t index, lt48_jtag_step *out_step
) {
    if (plan == NULL || out_step == NULL) return LT48_INVALID_ARGUMENT;
    if (index >= plan->step_count) return LT48_NOT_FOUND;
    *out_step = plan->steps[index];
    return LT48_OK;
}

lt48_status lt48_jtag_plan_json(
    const lt48_jtag_plan *plan, const unsigned char **out_json,
    size_t *out_size
) {
    if (plan == NULL || out_json == NULL || out_size == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_json = plan->json;
    *out_size = plan->json_size;
    return LT48_OK;
}

static int valid_bit_buffer(
    const unsigned char *data, size_t size, size_t bit_count
) {
    size_t required;
    if (data == NULL || bit_count == 0u) return 0;
    required = bit_count / 8u + (bit_count % 8u != 0u ? 1u : 0u);
    if (size != required) return 0;
    return bit_count % 8u == 0u ||
           (data[required - 1u] >> (bit_count % 8u)) == 0u;
}

static uint8_t bit_at(const unsigned char *data, size_t bit) {
    return (data[bit / 8u] >> (bit % 8u)) & 1u;
}

lt48_status lt48_jtag_evaluate_result(
    const lt48_jtag_result_request *request, lt48_jtag_result *out_result
) {
    int any_low = 0;
    int any_high = 0;
    if (request == NULL || out_result == NULL ||
        !valid_bit_buffer(request->tdo, request->tdo_size,
                          request->bit_count)) return LT48_INVALID_ARGUMENT;
    if ((request->expected == NULL) != (request->expected_size == 0u) ||
        (request->mask == NULL) != (request->mask_size == 0u) ||
        (request->mask != NULL && request->expected == NULL) ||
        (request->expected != NULL &&
         !valid_bit_buffer(request->expected, request->expected_size,
                           request->bit_count)) ||
        (request->mask != NULL &&
         !valid_bit_buffer(request->mask, request->mask_size,
                           request->bit_count))) return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    out_result->first_mismatch_bit = SIZE_MAX;
    out_result->compared = request->expected != NULL;
    out_result->matched = request->expected != NULL;
    for (size_t bit = 0u; bit < request->bit_count; ++bit) {
        uint8_t actual = bit_at(request->tdo, bit);
        any_high |= actual != 0u;
        any_low |= actual == 0u;
        if (request->expected != NULL &&
            (request->mask == NULL || bit_at(request->mask, bit) != 0u)) {
            uint8_t expected = bit_at(request->expected, bit);
            ++out_result->compared_bit_count;
            if (actual != expected && !out_result->has_mismatch) {
                out_result->has_mismatch = 1;
                out_result->matched = 0;
                out_result->first_mismatch_bit = bit;
                out_result->expected_bit = expected;
                out_result->actual_bit = actual;
            }
        }
    }
    out_result->stuck_low_observed = !any_high;
    out_result->stuck_high_observed = !any_low;
    return LT48_OK;
}

lt48_status lt48_jtag_decode_chain(
    const unsigned char *tdo, size_t tdo_size, size_t bit_count,
    size_t expected_element_count, lt48_jtag_chain_element *elements,
    size_t element_capacity, lt48_jtag_chain_report *out_report
) {
    size_t cursor = 0u;
    if (out_report == NULL || expected_element_count == 0u ||
        elements == NULL || element_capacity < expected_element_count ||
        !valid_bit_buffer(tdo, tdo_size, bit_count))
        return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    memset(elements, 0, expected_element_count * sizeof(*elements));
    out_report->requested_element_count = expected_element_count;
    for (size_t index = 0u; index < expected_element_count; ++index) {
        lt48_jtag_chain_element *element = &elements[index];
        if (cursor >= bit_count) break;
        element->first_bit = cursor;
        if (bit_at(tdo, cursor) == 0u) {
            element->kind = LT48_JTAG_CHAIN_BYPASS;
            element->bit_count = 1u;
            ++cursor;
        } else {
            uint32_t idcode = 0u;
            if (bit_count - cursor < 32u) break;
            for (size_t bit = 0u; bit < 32u; ++bit)
                idcode |= (uint32_t)bit_at(tdo, cursor + bit) << bit;
            element->kind = LT48_JTAG_CHAIN_IDCODE;
            element->bit_count = 32u;
            element->idcode = idcode;
            element->manufacturer_id = (uint16_t)((idcode >> 1u) & 0x7ffu);
            element->part_number = (uint16_t)((idcode >> 12u) & 0xffffu);
            element->version = (uint8_t)((idcode >> 28u) & 0x0fu);
            cursor += 32u;
        }
        ++out_report->decoded_element_count;
    }
    out_report->consumed_bit_count = cursor;
    out_report->trailing_bit_count = bit_count - cursor;
    out_report->complete =
        out_report->decoded_element_count == expected_element_count &&
        cursor == bit_count;
    return out_report->complete ? LT48_OK : LT48_FORMAT_ERROR;
}

const char *lt48_jtag_profile_issue_name(lt48_jtag_profile_issue issue) {
    static const char *const names[] = {
        "valid", "identity", "contact-range", "contact-conflict",
        "power-ownership", "vref-range", "tck-limit", "level-translation",
        "contention-control", "reset-policy", "disconnect-state"
    };
    return (unsigned)issue < sizeof(names) / sizeof(names[0]) ?
           names[(unsigned)issue] : NULL;
}

static int valid_profile_identity(const char *text) {
    size_t length;
    if (text == NULL || text[0] == '\0') return 0;
    length = strlen(text);
    if (length > 95u) return 0;
    for (size_t index = 0u; index < length; ++index) {
        unsigned char value = (unsigned char)text[index];
        if (!((value >= 'a' && value <= 'z') ||
              (value >= 'A' && value <= 'Z') ||
              (value >= '0' && value <= '9') || value == '.' ||
              value == '_' || value == '-')) return 0;
    }
    return 1;
}

static lt48_status profile_failure(
    lt48_jtag_profile_report *report, lt48_jtag_profile_issue issue,
    const char *diagnostic
) {
    report->issue = issue;
    snprintf(report->diagnostic, sizeof(report->diagnostic), "%s", diagnostic);
    return LT48_SAFETY_REJECTED;
}

lt48_status lt48_jtag_validate_target_profile(
    const lt48_jtag_target_profile *profile,
    lt48_jtag_profile_report *out_report
) {
    uint8_t contacts[8];
    size_t contact_count = 0u;
    unsigned char canonical[1024];
    int count;
    if (profile == NULL || out_report == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    if (!valid_profile_identity(profile->adapter_id) ||
        !valid_profile_identity(profile->adapter_revision))
        return profile_failure(out_report, LT48_JTAG_PROFILE_IDENTITY,
            "adapter id and revision must be 1..95 stable ASCII identifier characters");
    contacts[contact_count++] = profile->tck_contact;
    contacts[contact_count++] = profile->tms_contact;
    contacts[contact_count++] = profile->tdi_contact;
    contacts[contact_count++] = profile->tdo_contact;
    contacts[contact_count++] = profile->ground_contact;
    contacts[contact_count++] = profile->vref_contact;
    for (size_t index = 0u; index < contact_count; ++index)
        if (contacts[index] == 0u || contacts[index] > 48u)
            return profile_failure(out_report, LT48_JTAG_PROFILE_CONTACT_RANGE,
                "TCK/TMS/TDI/TDO/ground/VREF contacts must be in 1..48");
    if (profile->reset_contact > 48u || profile->target_power_contact > 48u)
        return profile_failure(out_report, LT48_JTAG_PROFILE_CONTACT_RANGE,
            "optional reset and target-power contacts must be zero or in 1..48");
    if (profile->reset_contact != 0u)
        contacts[contact_count++] = profile->reset_contact;
    if (profile->target_power_contact != 0u)
        contacts[contact_count++] = profile->target_power_contact;
    for (size_t left = 0u; left < contact_count; ++left)
        for (size_t right = left + 1u; right < contact_count; ++right)
            if (contacts[left] == contacts[right])
                return profile_failure(out_report,
                    LT48_JTAG_PROFILE_CONTACT_CONFLICT,
                    "each declared signal, ground, reference and power contact must be distinct");
    if ((profile->power_ownership != LT48_JTAG_POWER_EXTERNAL &&
         profile->power_ownership != LT48_JTAG_POWER_ADAPTER &&
         profile->power_ownership != LT48_JTAG_POWER_PROGRAMMER) ||
        (profile->power_ownership == LT48_JTAG_POWER_PROGRAMMER) !=
            (profile->target_power_contact != 0u))
        return profile_failure(out_report, LT48_JTAG_PROFILE_POWER_OWNERSHIP,
            "programmer power requires one mapped power contact; external/adapter power forbids it");
    if (profile->vref_min_millivolts == 0u ||
        profile->vref_max_millivolts < profile->vref_min_millivolts)
        return profile_failure(out_report, LT48_JTAG_PROFILE_VREF_RANGE,
            "VREF minimum must be nonzero and not exceed the maximum");
    if (profile->maximum_tck_hz == 0u)
        return profile_failure(out_report, LT48_JTAG_PROFILE_TCK_LIMIT,
            "maximum TCK must be explicitly bounded and nonzero");
    if (profile->level_translation == LT48_JTAG_ASSURANCE_UNKNOWN)
        return profile_failure(out_report, LT48_JTAG_PROFILE_LEVEL_TRANSLATION,
            "level translation must be present or explicitly proved unnecessary");
    if (profile->contention_control == LT48_JTAG_ASSURANCE_UNKNOWN)
        return profile_failure(out_report, LT48_JTAG_PROFILE_CONTENTION_CONTROL,
            "contention control must be present or explicitly proved unnecessary");
    if ((profile->target_reset == LT48_JTAG_RESET_NOT_CONNECTED) !=
            (profile->reset_contact == 0u) ||
        (profile->target_reset != LT48_JTAG_RESET_NOT_CONNECTED &&
         profile->target_reset != LT48_JTAG_RESET_HOLD_DURING_CONNECT &&
         profile->target_reset != LT48_JTAG_RESET_RELEASED))
        return profile_failure(out_report, LT48_JTAG_PROFILE_RESET_POLICY,
            "reset policy and optional reset contact are inconsistent");
    if (!profile->disconnect_high_impedance ||
        (profile->power_ownership == LT48_JTAG_POWER_PROGRAMMER &&
         !profile->disconnect_target_power_off))
        return profile_failure(out_report, LT48_JTAG_PROFILE_DISCONNECT_STATE,
            "disconnect must high-impedance signals and switch off programmer-owned target power");
    count = snprintf((char *)canonical, sizeof(canonical),
        "lt48-jtag-target-profile-v1\n%s\n%s\n"
        "%u,%u,%u,%u,%u,%u,%u,%u\n%u\n%u,%u\n%u\n%u,%u\n%u\n%d,%d\n",
        profile->adapter_id, profile->adapter_revision,
        profile->tck_contact, profile->tms_contact, profile->tdi_contact,
        profile->tdo_contact, profile->ground_contact, profile->vref_contact,
        profile->reset_contact, profile->target_power_contact,
        (unsigned)profile->power_ownership, profile->vref_min_millivolts,
        profile->vref_max_millivolts, profile->maximum_tck_hz,
        (unsigned)profile->level_translation,
        (unsigned)profile->contention_control, (unsigned)profile->target_reset,
        profile->disconnect_high_impedance,
        profile->disconnect_target_power_off);
    if (count < 0 || (size_t)count >= sizeof(canonical))
        return LT48_NO_MEMORY;
    out_report->issue = LT48_JTAG_PROFILE_VALID;
    out_report->complete = 1;
    out_report->mapped_contact_count = contact_count;
    snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
             "complete external-target electrical profile");
    lt48_sha256(canonical, (size_t)count, out_report->sha256);
    return LT48_OK;
}
