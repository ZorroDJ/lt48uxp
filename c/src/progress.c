#include <labtool48/labtool48.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lt48_progress_emitter {
    uint64_t next_sequence;
    lt48_progress_observer_fn observer;
    void *observer_context;
    size_t observer_error_count;
};

static const unsigned char progress_manifest[] =
    "{\"encoding\":\"one JSON object per event\","
    "\"fields\":[\"schema_version\",\"sequence\",\"scope\",\"name\","
    "\"state\",\"category\",\"completed_units\",\"total_units\",\"unit\","
    "\"error_type\",\"error_message\"],\"payload_bytes_allowed\":false,"
    "\"schema_version\":1,\"scopes\":[\"operation\",\"session\",\"phase\","
    "\"progress\"],\"sequence\":\"zero-based, strictly increasing within one operation\","
    "\"states\":{\"operation\":[\"started\",\"succeeded\",\"failed\","
    "\"cancelled\",\"safety-rejected\"],\"phase\":[\"started\",\"succeeded\","
    "\"failed\"],\"progress\":[\"running\"],\"session\":[\"started\","
    "\"succeeded\",\"failed\"]}}";

const char *lt48_progress_scope_name(lt48_progress_scope scope) {
    switch (scope) {
    case LT48_PROGRESS_OPERATION: return "operation";
    case LT48_PROGRESS_SESSION: return "session";
    case LT48_PROGRESS_PHASE: return "phase";
    case LT48_PROGRESS_UNITS: return "progress";
    }
    return NULL;
}

const char *lt48_progress_state_name(lt48_progress_state_kind state) {
    switch (state) {
    case LT48_PROGRESS_STARTED: return "started";
    case LT48_PROGRESS_SUCCEEDED: return "succeeded";
    case LT48_PROGRESS_FAILED: return "failed";
    case LT48_PROGRESS_CANCELLED: return "cancelled";
    case LT48_PROGRESS_SAFETY_REJECTED: return "safety-rejected";
    case LT48_PROGRESS_RUNNING: return "running";
    }
    return NULL;
}

lt48_status lt48_progress_manifest_json(
    const unsigned char **out_json, size_t *out_size
) {
    if (out_json == NULL || out_size == NULL) return LT48_INVALID_ARGUMENT;
    *out_json = progress_manifest;
    *out_size = sizeof(progress_manifest) - 1u;
    return LT48_OK;
}

static int state_allowed(
    lt48_progress_scope scope, lt48_progress_state_kind state
) {
    if (scope == LT48_PROGRESS_OPERATION)
        return state >= LT48_PROGRESS_STARTED &&
               state <= LT48_PROGRESS_SAFETY_REJECTED;
    if (scope == LT48_PROGRESS_SESSION || scope == LT48_PROGRESS_PHASE)
        return state == LT48_PROGRESS_STARTED ||
               state == LT48_PROGRESS_SUCCEEDED ||
               state == LT48_PROGRESS_FAILED;
    return scope == LT48_PROGRESS_UNITS && state == LT48_PROGRESS_RUNNING;
}

static int input_valid(const lt48_progress_event_input *input) {
    if (input == NULL || lt48_progress_scope_name(input->scope) == NULL ||
        lt48_progress_state_name(input->state) == NULL ||
        !state_allowed(input->scope, input->state) || input->name == NULL ||
        input->name[0] == '\0' ||
        ((input->error_type == NULL) != (input->error_message == NULL)))
        return 0;
    if (input->has_units) {
        if (input->completed_units > input->total_units ||
            input->unit == NULL || input->unit[0] == '\0') return 0;
    } else if (input->completed_units != 0u || input->total_units != 0u ||
               input->unit != NULL) {
        return 0;
    }
    return 1;
}

typedef struct progress_json_writer {
    unsigned char *buffer;
    size_t capacity;
    size_t size;
} progress_json_writer;

static void json_bytes(
    progress_json_writer *writer, const char *data, size_t size
) {
    if (writer->buffer != NULL && writer->size < writer->capacity) {
        size_t available = writer->capacity - writer->size;
        size_t copied = size < available ? size : available;
        memcpy(writer->buffer + writer->size, data, copied);
    }
    writer->size += size;
}

static void json_literal(progress_json_writer *writer, const char *text) {
    json_bytes(writer, text, strlen(text));
}

static void json_string(progress_json_writer *writer, const char *text) {
    static const char hex[] = "0123456789abcdef";
    json_literal(writer, "\"");
    for (const unsigned char *cursor = (const unsigned char *)text;
         *cursor != '\0'; ++cursor) {
        char escaped[6];
        switch (*cursor) {
        case '"': json_literal(writer, "\\\""); break;
        case '\\': json_literal(writer, "\\\\"); break;
        case '\b': json_literal(writer, "\\b"); break;
        case '\f': json_literal(writer, "\\f"); break;
        case '\n': json_literal(writer, "\\n"); break;
        case '\r': json_literal(writer, "\\r"); break;
        case '\t': json_literal(writer, "\\t"); break;
        default:
            if (*cursor < 0x20u) {
                escaped[0] = '\\'; escaped[1] = 'u';
                escaped[2] = '0'; escaped[3] = '0';
                escaped[4] = hex[*cursor >> 4];
                escaped[5] = hex[*cursor & 0x0fu];
                json_bytes(writer, escaped, sizeof(escaped));
            } else {
                json_bytes(writer, (const char *)cursor, 1u);
            }
            break;
        }
    }
    json_literal(writer, "\"");
}

static void json_nullable_string(
    progress_json_writer *writer, const char *text
) {
    if (text == NULL) json_literal(writer, "null");
    else json_string(writer, text);
}

lt48_status lt48_progress_event_json(
    const lt48_progress_event *event, unsigned char *buffer, size_t capacity,
    size_t *out_size
) {
    progress_json_writer writer = {buffer, capacity, 0u};
    char number[32];
    int length;
    const char *scope;
    const char *state;

    if (event == NULL || out_size == NULL ||
        (buffer == NULL && capacity != 0u) || event->schema_version != 1u ||
        !input_valid(&event->value)) return LT48_INVALID_ARGUMENT;
    scope = lt48_progress_scope_name(event->value.scope);
    state = lt48_progress_state_name(event->value.state);
    json_literal(&writer, "{\"schema_version\":1,\"sequence\":");
    length = snprintf(number, sizeof(number), "%" PRIu64, event->sequence);
    if (length < 0 || (size_t)length >= sizeof(number))
        return LT48_RUNTIME_ERROR;
    json_bytes(&writer, number, (size_t)length);
    json_literal(&writer, ",\"scope\":"); json_string(&writer, scope);
    json_literal(&writer, ",\"name\":"); json_string(&writer, event->value.name);
    json_literal(&writer, ",\"state\":"); json_string(&writer, state);
    json_literal(&writer, ",\"category\":");
    json_nullable_string(&writer, event->value.category);
    json_literal(&writer, ",\"completed_units\":");
    if (event->value.has_units) {
        length = snprintf(number, sizeof(number), "%zu",
                          event->value.completed_units);
        if (length < 0 || (size_t)length >= sizeof(number))
            return LT48_RUNTIME_ERROR;
        json_bytes(&writer, number, (size_t)length);
    } else json_literal(&writer, "null");
    json_literal(&writer, ",\"total_units\":");
    if (event->value.has_units) {
        length = snprintf(number, sizeof(number), "%zu",
                          event->value.total_units);
        if (length < 0 || (size_t)length >= sizeof(number))
            return LT48_RUNTIME_ERROR;
        json_bytes(&writer, number, (size_t)length);
    } else json_literal(&writer, "null");
    json_literal(&writer, ",\"unit\":");
    json_nullable_string(&writer, event->value.unit);
    json_literal(&writer, ",\"error_type\":");
    json_nullable_string(&writer, event->value.error_type);
    json_literal(&writer, ",\"error_message\":");
    json_nullable_string(&writer, event->value.error_message);
    json_literal(&writer, "}");
    *out_size = writer.size;
    if (buffer == NULL) return LT48_OK;
    if (capacity <= writer.size) return LT48_BUFFER_TOO_SMALL;
    buffer[writer.size] = '\0';
    return LT48_OK;
}

lt48_status lt48_progress_emitter_create(
    lt48_progress_observer_fn observer, void *observer_context,
    lt48_progress_emitter **out_emitter
) {
    lt48_progress_emitter *emitter;
    if (out_emitter == NULL) return LT48_INVALID_ARGUMENT;
    *out_emitter = NULL;
    emitter = calloc(1u, sizeof(*emitter));
    if (emitter == NULL) return LT48_NO_MEMORY;
    emitter->observer = observer;
    emitter->observer_context = observer_context;
    *out_emitter = emitter;
    return LT48_OK;
}

void lt48_progress_emitter_close(lt48_progress_emitter *emitter) {
    free(emitter);
}

lt48_status lt48_progress_emit(
    lt48_progress_emitter *emitter,
    const lt48_progress_event_input *input,
    lt48_progress_event *out_event
) {
    lt48_progress_event event;
    if (emitter == NULL || out_event == NULL || !input_valid(input))
        return LT48_INVALID_ARGUMENT;
    if (emitter->next_sequence == UINT64_MAX) return LT48_RUNTIME_ERROR;
    event.schema_version = 1u;
    event.sequence = emitter->next_sequence++;
    event.value = *input;
    *out_event = event;
    if (emitter->observer != NULL &&
        emitter->observer(emitter->observer_context, &event) != LT48_OK)
        ++emitter->observer_error_count;
    return LT48_OK;
}

size_t lt48_progress_observer_error_count(
    const lt48_progress_emitter *emitter
) {
    return emitter == NULL ? 0u : emitter->observer_error_count;
}

void lt48_progress_state_init(lt48_progress_state *state) {
    if (state != NULL) memset(state, 0, sizeof(*state));
}

static int copy_text(char *destination, size_t capacity, const char *source) {
    size_t size;
    if (destination == NULL || capacity == 0u || source == NULL) return 0;
    size = strlen(source);
    if (size >= capacity) return 0;
    memcpy(destination, source, size + 1u);
    return 1;
}

lt48_status lt48_progress_state_consume(
    lt48_progress_state *state, const lt48_progress_event *event
) {
    lt48_progress_state next;
    if (state == NULL || event == NULL || event->schema_version != 1u ||
        !input_valid(&event->value) ||
        (!state->has_event && event->sequence != 0u) ||
        (state->has_event && event->sequence <= state->last_sequence))
        return LT48_FORMAT_ERROR;
    next = *state;
    next.has_event = 1;
    next.last_sequence = event->sequence;
    if (event->value.scope == LT48_PROGRESS_OPERATION)
        next.operation_state = event->value.state;
    if ((event->value.scope == LT48_PROGRESS_PHASE &&
         event->value.state == LT48_PROGRESS_STARTED) ||
        event->value.scope == LT48_PROGRESS_UNITS) {
        if (!copy_text(next.current_phase, sizeof(next.current_phase),
                       event->value.name)) return LT48_BUFFER_TOO_SMALL;
    }
    if (event->value.scope == LT48_PROGRESS_UNITS) {
        next.has_units = event->value.has_units;
        next.completed_units = event->value.completed_units;
        next.total_units = event->value.total_units;
        if (event->value.has_units &&
            !copy_text(next.unit, sizeof(next.unit), event->value.unit))
            return LT48_BUFFER_TOO_SMALL;
        if (!event->value.has_units) next.unit[0] = '\0';
    }
    if (event->value.error_type != NULL) {
        if (!copy_text(next.error_type, sizeof(next.error_type),
                       event->value.error_type) ||
            !copy_text(next.error_message, sizeof(next.error_message),
                       event->value.error_message))
            return LT48_BUFFER_TOO_SMALL;
        next.has_error = 1;
    }
    *state = next;
    return LT48_OK;
}
