#include <labtool48/labtool48.h>

#include "json_internal.h"

#include <stdlib.h>
#include <string.h>

struct lt48_transfer_trace {
    lt48_json_document *document;
    lt48_trace_event *events;
    size_t event_count;
    unsigned char *metadata_json;
    size_t metadata_json_size;
    int had_claimed_sha256;
    unsigned char trace_sha256[LT48_SHA256_SIZE];
};

static int hex_digit(unsigned char value, unsigned char *out) {
    if (value >= '0' && value <= '9') *out = (unsigned char)(value - '0');
    else if (value >= 'a' && value <= 'f')
        *out = (unsigned char)(value - 'a' + 10u);
    else if (value >= 'A' && value <= 'F')
        *out = (unsigned char)(value - 'A' + 10u);
    else return 0;
    return 1;
}

static int parse_digest(
    const char *text, unsigned char digest[LT48_SHA256_SIZE]
) {
    if (text == NULL || strlen(text) != LT48_SHA256_SIZE * 2u) return 0;
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        unsigned char high;
        unsigned char low;
        if (!hex_digit((unsigned char)text[index * 2u], &high) ||
            !hex_digit((unsigned char)text[index * 2u + 1u], &low)) return 0;
        digest[index] = (unsigned char)((unsigned)high << 4u | low);
    }
    return 1;
}

static int validate_data_hex(const char *text, size_t *out_size) {
    size_t length;
    unsigned char ignored;
    if (text == NULL || out_size == NULL) return 0;
    length = strlen(text);
    if (length % 2u != 0u) return 0;
    for (size_t index = 0u; index < length; ++index)
        if (!hex_digit((unsigned char)text[index], &ignored)) return 0;
    *out_size = length / 2u;
    return 1;
}

static int event_from_json(
    const lt48_json_value *value, size_t expected_sequence,
    lt48_trace_event *out_event
) {
    static const char *const fields[] = {
        "sequence", "direction", "endpoint", "requested_length",
        "data_hex", "timeout_ms", "status", "error"
    };
    const char *direction;
    const char *status;
    const lt48_json_value *error;
    uint64_t sequence;
    uint64_t endpoint;
    uint64_t requested_length;
    uint64_t timeout_ms;
    size_t data_size;
    if (out_event == NULL ||
        !lt48_json_object_exact(value, fields,
                                sizeof(fields) / sizeof(fields[0])) ||
        !lt48_json_u64(lt48_json_object_member(value, "sequence"),
                       &sequence) || sequence != expected_sequence ||
        !lt48_json_u64(lt48_json_object_member(value, "endpoint"),
                       &endpoint) || endpoint > UINT8_MAX ||
        !lt48_json_u64(lt48_json_object_member(value, "requested_length"),
                       &requested_length) || requested_length > SIZE_MAX ||
        !lt48_json_u64(lt48_json_object_member(value, "timeout_ms"),
                       &timeout_ms) || timeout_ms > UINT32_MAX)
        return 0;
    direction = lt48_json_string(
        lt48_json_object_member(value, "direction")
    );
    status = lt48_json_string(lt48_json_object_member(value, "status"));
    out_event->data_hex = lt48_json_string(
        lt48_json_object_member(value, "data_hex")
    );
    error = lt48_json_object_member(value, "error");
    if (direction == NULL || status == NULL ||
        !validate_data_hex(out_event->data_hex, &data_size) ||
        (strcmp(direction, "out") != 0 && strcmp(direction, "in") != 0) ||
        (strcmp(status, "ok") != 0 && strcmp(status, "error") != 0) ||
        (lt48_json_value_type(error) != LT48_JSON_NULL &&
         lt48_json_value_type(error) != LT48_JSON_STRING) ||
        ((strcmp(status, "error") == 0) !=
         (lt48_json_value_type(error) == LT48_JSON_STRING)) ||
        (strcmp(direction, "out") == 0 && data_size != requested_length) ||
        (strcmp(direction, "in") == 0 && data_size > requested_length))
        return 0;
    memset(out_event, 0, sizeof(*out_event));
    out_event->sequence = expected_sequence;
    out_event->direction = strcmp(direction, "out") == 0 ?
        LT48_TRACE_OUT : LT48_TRACE_IN;
    out_event->endpoint = (uint8_t)endpoint;
    out_event->requested_length = (size_t)requested_length;
    out_event->data_hex = lt48_json_string(
        lt48_json_object_member(value, "data_hex")
    );
    out_event->data_size = data_size;
    out_event->timeout_ms = (uint32_t)timeout_ms;
    out_event->status = status;
    out_event->error = lt48_json_string(error);
    return 1;
}

lt48_status lt48_trace_open(
    const char *path, lt48_transfer_trace **out_trace
) {
    static const char *const root_fields[] = {
        "schema_version", "metadata", "events"
    };
    static const char *const root_fields_hashed[] = {
        "schema_version", "metadata", "events", "trace_sha256"
    };
    lt48_transfer_trace *trace;
    const lt48_json_value *root;
    const lt48_json_value *events;
    const lt48_json_value *metadata;
    const lt48_json_value *claimed;
    uint64_t schema;
    unsigned char claimed_digest[LT48_SHA256_SIZE];
    lt48_status status;
    if (path == NULL || out_trace == NULL) return LT48_INVALID_ARGUMENT;
    *out_trace = NULL;
    trace = calloc(1u, sizeof(*trace));
    if (trace == NULL) return LT48_NO_MEMORY;
    status = lt48_json_document_open(path, &trace->document);
    if (status != LT48_OK) goto failed;
    root = lt48_json_document_root(trace->document);
    if ((!lt48_json_object_exact(root, root_fields,
                                 sizeof(root_fields) / sizeof(root_fields[0])) &&
         !lt48_json_object_exact(
             root, root_fields_hashed,
             sizeof(root_fields_hashed) / sizeof(root_fields_hashed[0]))) ||
        !lt48_json_u64(lt48_json_object_member(root, "schema_version"),
                       &schema) || schema != 1u) {
        status = LT48_FORMAT_ERROR;
        goto failed;
    }
    metadata = lt48_json_object_member(root, "metadata");
    events = lt48_json_object_member(root, "events");
    if (lt48_json_value_type(metadata) != LT48_JSON_OBJECT ||
        lt48_json_value_type(events) != LT48_JSON_ARRAY) {
        status = LT48_FORMAT_ERROR;
        goto failed;
    }
    trace->event_count = lt48_json_array_count(events);
    if (trace->event_count > SIZE_MAX / sizeof(*trace->events)) {
        status = LT48_NO_MEMORY;
        goto failed;
    }
    if (trace->event_count != 0u) {
        trace->events = calloc(trace->event_count, sizeof(*trace->events));
        if (trace->events == NULL) {
            status = LT48_NO_MEMORY;
            goto failed;
        }
    }
    for (size_t index = 0u; index < trace->event_count; ++index) {
        if (!event_from_json(lt48_json_array_item(events, index), index,
                             &trace->events[index])) {
            status = LT48_FORMAT_ERROR;
            goto failed;
        }
    }
    status = lt48_json_canonical_copy(
        metadata, &trace->metadata_json, &trace->metadata_json_size
    );
    if (status != LT48_OK) goto failed;
    status = lt48_json_canonical_sha256(
        root, "trace_sha256", trace->trace_sha256
    );
    if (status != LT48_OK) goto failed;
    claimed = lt48_json_object_member(root, "trace_sha256");
    if (claimed != NULL) {
        trace->had_claimed_sha256 = 1;
        if (!parse_digest(lt48_json_string(claimed), claimed_digest) ||
            memcmp(claimed_digest, trace->trace_sha256,
                   LT48_SHA256_SIZE) != 0) {
            status = LT48_FORMAT_ERROR;
            goto failed;
        }
    }
    *out_trace = trace;
    return LT48_OK;
failed:
    lt48_trace_close(trace);
    return status;
}

void lt48_trace_close(lt48_transfer_trace *trace) {
    if (trace == NULL) return;
    free(trace->metadata_json);
    free(trace->events);
    lt48_json_document_close(trace->document);
    free(trace);
}

lt48_status lt48_trace_get_info(
    const lt48_transfer_trace *trace, lt48_trace_info *out_info
) {
    if (trace == NULL || out_info == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_info, 0, sizeof(*out_info));
    out_info->schema_version = 1u;
    out_info->event_count = trace->event_count;
    out_info->metadata_json = trace->metadata_json;
    out_info->metadata_json_size = trace->metadata_json_size;
    out_info->had_claimed_sha256 = trace->had_claimed_sha256;
    memcpy(out_info->trace_sha256, trace->trace_sha256, LT48_SHA256_SIZE);
    return LT48_OK;
}

lt48_status lt48_trace_get_event(
    const lt48_transfer_trace *trace, size_t index, lt48_trace_event *out_event
) {
    if (trace == NULL || out_event == NULL) return LT48_INVALID_ARGUMENT;
    if (index >= trace->event_count) return LT48_NOT_FOUND;
    *out_event = trace->events[index];
    return LT48_OK;
}

static int nullable_equal(const char *left, const char *right) {
    return left == NULL ? right == NULL :
        right != NULL && strcmp(left, right) == 0;
}

static int event_equal(
    const lt48_trace_event *left, const lt48_trace_event *right
) {
    return left->sequence == right->sequence &&
        left->direction == right->direction &&
        left->endpoint == right->endpoint &&
        left->requested_length == right->requested_length &&
        left->data_size == right->data_size &&
        strcmp(left->data_hex, right->data_hex) == 0 &&
        left->timeout_ms == right->timeout_ms &&
        strcmp(left->status, right->status) == 0 &&
        nullable_equal(left->error, right->error);
}

lt48_status lt48_trace_compare(
    const lt48_transfer_trace *left, const lt48_transfer_trace *right,
    lt48_trace_comparison *out_comparison
) {
    size_t common;
    if (left == NULL || right == NULL || out_comparison == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_comparison, 0, sizeof(*out_comparison));
    common = left->event_count < right->event_count ?
        left->event_count : right->event_count;
    for (size_t index = 0u; index < common; ++index) {
        if (!event_equal(&left->events[index], &right->events[index])) {
            out_comparison->first_difference = index;
            out_comparison->has_left = 1;
            out_comparison->has_right = 1;
            out_comparison->left = left->events[index];
            out_comparison->right = right->events[index];
            return LT48_OK;
        }
    }
    if (left->event_count != right->event_count) {
        out_comparison->first_difference = common;
        if (common < left->event_count) {
            out_comparison->has_left = 1;
            out_comparison->left = left->events[common];
        }
        if (common < right->event_count) {
            out_comparison->has_right = 1;
            out_comparison->right = right->events[common];
        }
        return LT48_OK;
    }
    out_comparison->equal = 1;
    out_comparison->first_difference = SIZE_MAX;
    return LT48_OK;
}
