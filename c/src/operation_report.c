#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif

#include <labtool48/labtool48.h>

#include "json_internal.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct report_programmer_identity {
    char model[64];
    char serial[64];
    char firmware_version[32];
    unsigned bus_number;
    unsigned device_address;
    unsigned product_id;
    unsigned device_version_bcd;
    int valid;
} report_programmer_identity;

static _Thread_local report_programmer_identity report_programmer;

void lt48_operation_report_set_programmer_identity(
    const char *model, const char *serial, const char *firmware_version,
    unsigned bus_number,
    unsigned device_address, unsigned product_id,
    unsigned device_version_bcd
) {
    if (model == NULL || serial == NULL) {
        report_programmer.valid = 0;
        return;
    }
    snprintf(report_programmer.model, sizeof(report_programmer.model),
             "%s", model);
    snprintf(report_programmer.serial, sizeof(report_programmer.serial),
             "%s", serial);
    snprintf(report_programmer.firmware_version,
             sizeof(report_programmer.firmware_version), "%s",
             firmware_version == NULL ? "" : firmware_version);
    report_programmer.bus_number = bus_number;
    report_programmer.device_address = device_address;
    report_programmer.product_id = product_id;
    report_programmer.device_version_bcd = device_version_bcd;
    report_programmer.valid = 1;
}

typedef enum json_type {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} json_type;

typedef struct json_string {
    unsigned char *data;
    size_t size;
} json_string;

typedef struct json_value json_value;

typedef struct json_member {
    json_string key;
    json_value *value;
} json_member;

struct json_value {
    json_type type;
    union {
        int boolean;
        json_string text;
        struct {
            json_value **items;
            size_t count;
        } array;
        struct {
            json_member *items;
            size_t count;
        } object;
    } value;
};

typedef struct json_parser {
    const unsigned char *cursor;
    const unsigned char *end;
    unsigned depth;
    lt48_status status;
} json_parser;

typedef struct byte_buffer {
    unsigned char *data;
    size_t size;
    size_t capacity;
} byte_buffer;

struct lt48_operation_report {
    unsigned char *source;
    size_t source_size;
    json_value *root;
};

static void json_free(json_value *value) {
    if (value == NULL) return;
    if (value->type == JSON_STRING || value->type == JSON_NUMBER) {
        free(value->value.text.data);
    } else if (value->type == JSON_ARRAY) {
        for (size_t index = 0u; index < value->value.array.count; ++index)
            json_free(value->value.array.items[index]);
        free(value->value.array.items);
    } else if (value->type == JSON_OBJECT) {
        for (size_t index = 0u; index < value->value.object.count; ++index) {
            free(value->value.object.items[index].key.data);
            json_free(value->value.object.items[index].value);
        }
        free(value->value.object.items);
    }
    free(value);
}

static int buffer_reserve(byte_buffer *buffer, size_t extra) {
    size_t required;
    size_t capacity;
    unsigned char *resized;
    if (extra > SIZE_MAX - buffer->size) return 0;
    required = buffer->size + extra;
    if (required <= buffer->capacity) return 1;
    capacity = buffer->capacity == 0u ? 64u : buffer->capacity;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2u) {
            capacity = required;
            break;
        }
        capacity *= 2u;
    }
    resized = realloc(buffer->data, capacity);
    if (resized == NULL) return 0;
    buffer->data = resized;
    buffer->capacity = capacity;
    return 1;
}

static int buffer_append(byte_buffer *buffer, const void *data, size_t size) {
    if (!buffer_reserve(buffer, size)) return 0;
    if (size != 0u) memcpy(buffer->data + buffer->size, data, size);
    buffer->size += size;
    return 1;
}

static int buffer_byte(byte_buffer *buffer, unsigned char value) {
    return buffer_append(buffer, &value, 1u);
}

static void skip_space(json_parser *parser) {
    while (parser->cursor < parser->end &&
           (*parser->cursor == ' ' || *parser->cursor == '\t' ||
            *parser->cursor == '\r' || *parser->cursor == '\n'))
        ++parser->cursor;
}

static int hex_digit(unsigned char value, unsigned *out) {
    if (value >= '0' && value <= '9') *out = (unsigned)(value - '0');
    else if (value >= 'a' && value <= 'f') *out = (unsigned)(value - 'a' + 10u);
    else if (value >= 'A' && value <= 'F') *out = (unsigned)(value - 'A' + 10u);
    else return 0;
    return 1;
}

static int parse_u16_escape(json_parser *parser, unsigned *out) {
    unsigned result = 0u;
    if ((size_t)(parser->end - parser->cursor) < 4u) return 0;
    for (size_t index = 0u; index < 4u; ++index) {
        unsigned digit;
        if (!hex_digit(parser->cursor[index], &digit)) return 0;
        result = result * 16u + digit;
    }
    parser->cursor += 4u;
    *out = result;
    return 1;
}

static int append_utf8(byte_buffer *buffer, unsigned codepoint) {
    unsigned char bytes[4];
    size_t size;
    if (codepoint <= 0x7fu) {
        bytes[0] = (unsigned char)codepoint;
        size = 1u;
    } else if (codepoint <= 0x7ffu) {
        bytes[0] = (unsigned char)(0xc0u | (codepoint >> 6));
        bytes[1] = (unsigned char)(0x80u | (codepoint & 0x3fu));
        size = 2u;
    } else if (codepoint <= 0xffffu) {
        if (codepoint >= 0xd800u && codepoint <= 0xdfffu) return 0;
        bytes[0] = (unsigned char)(0xe0u | (codepoint >> 12));
        bytes[1] = (unsigned char)(0x80u | ((codepoint >> 6) & 0x3fu));
        bytes[2] = (unsigned char)(0x80u | (codepoint & 0x3fu));
        size = 3u;
    } else if (codepoint <= 0x10ffffu) {
        bytes[0] = (unsigned char)(0xf0u | (codepoint >> 18));
        bytes[1] = (unsigned char)(0x80u | ((codepoint >> 12) & 0x3fu));
        bytes[2] = (unsigned char)(0x80u | ((codepoint >> 6) & 0x3fu));
        bytes[3] = (unsigned char)(0x80u | (codepoint & 0x3fu));
        size = 4u;
    } else {
        return 0;
    }
    return buffer_append(buffer, bytes, size);
}

static int parse_string(json_parser *parser, json_string *out) {
    byte_buffer result = {0};
    if (parser->cursor >= parser->end || *parser->cursor++ != '"') return 0;
    while (parser->cursor < parser->end) {
        unsigned char value = *parser->cursor++;
        if (value == '"') {
            if (!buffer_byte(&result, 0u)) goto memory_error;
            out->data = result.data;
            out->size = result.size - 1u;
            return 1;
        }
        if (value < 0x20u) goto format_error;
        if (value != '\\') {
            if (!buffer_byte(&result, value)) goto memory_error;
            continue;
        }
        if (parser->cursor >= parser->end) goto format_error;
        value = *parser->cursor++;
        switch (value) {
        case '"': case '\\': case '/':
            if (!buffer_byte(&result, value)) goto memory_error;
            break;
        case 'b': if (!buffer_byte(&result, '\b')) goto memory_error; break;
        case 'f': if (!buffer_byte(&result, '\f')) goto memory_error; break;
        case 'n': if (!buffer_byte(&result, '\n')) goto memory_error; break;
        case 'r': if (!buffer_byte(&result, '\r')) goto memory_error; break;
        case 't': if (!buffer_byte(&result, '\t')) goto memory_error; break;
        case 'u': {
            unsigned first;
            unsigned codepoint;
            if (!parse_u16_escape(parser, &first)) goto format_error;
            codepoint = first;
            if (first >= 0xd800u && first <= 0xdbffu) {
                unsigned second;
                if ((size_t)(parser->end - parser->cursor) < 6u ||
                    parser->cursor[0] != '\\' || parser->cursor[1] != 'u')
                    goto format_error;
                parser->cursor += 2u;
                if (!parse_u16_escape(parser, &second) ||
                    second < 0xdc00u || second > 0xdfffu)
                    goto format_error;
                codepoint = 0x10000u + ((first - 0xd800u) << 10) +
                            (second - 0xdc00u);
            } else if (first >= 0xdc00u && first <= 0xdfffu) {
                goto format_error;
            }
            if (!append_utf8(&result, codepoint)) goto memory_error;
            break;
        }
        default:
            goto format_error;
        }
    }
format_error:
    free(result.data);
    return 0;
memory_error:
    free(result.data);
    parser->status = LT48_NO_MEMORY;
    return 0;
}

static json_value *parse_value(json_parser *parser);

static json_value *new_value(json_parser *parser, json_type type) {
    json_value *value = calloc(1u, sizeof(*value));
    if (value == NULL) parser->status = LT48_NO_MEMORY;
    else value->type = type;
    return value;
}

static int append_array_item(json_parser *parser, json_value *array,
                             json_value *item) {
    json_value **items;
    if (array->value.array.count == SIZE_MAX / sizeof(*items)) return 0;
    items = realloc(array->value.array.items,
        (array->value.array.count + 1u) * sizeof(*items));
    if (items == NULL) {
        parser->status = LT48_NO_MEMORY;
        return 0;
    }
    array->value.array.items = items;
    items[array->value.array.count++] = item;
    return 1;
}

static int string_equal(const json_string *string, const char *text) {
    size_t size = strlen(text);
    return string->size == size && memcmp(string->data, text, size) == 0;
}

static int append_object_member(json_parser *parser, json_value *object,
                                json_string key, json_value *value) {
    json_member *items;
    for (size_t index = 0u; index < object->value.object.count; ++index) {
        json_string *existing = &object->value.object.items[index].key;
        if (existing->size == key.size &&
            memcmp(existing->data, key.data, key.size) == 0)
            return 0;
    }
    if (object->value.object.count == SIZE_MAX / sizeof(*items)) return 0;
    items = realloc(object->value.object.items,
        (object->value.object.count + 1u) * sizeof(*items));
    if (items == NULL) {
        parser->status = LT48_NO_MEMORY;
        return 0;
    }
    object->value.object.items = items;
    items[object->value.object.count].key = key;
    items[object->value.object.count].value = value;
    ++object->value.object.count;
    return 1;
}

static json_value *parse_array(json_parser *parser) {
    json_value *array = new_value(parser, JSON_ARRAY);
    if (array == NULL) return NULL;
    ++parser->cursor;
    skip_space(parser);
    if (parser->cursor < parser->end && *parser->cursor == ']') {
        ++parser->cursor;
        return array;
    }
    for (;;) {
        json_value *item = parse_value(parser);
        if (item == NULL || !append_array_item(parser, array, item)) {
            json_free(item);
            json_free(array);
            return NULL;
        }
        skip_space(parser);
        if (parser->cursor >= parser->end) break;
        if (*parser->cursor == ']') {
            ++parser->cursor;
            return array;
        }
        if (*parser->cursor++ != ',') break;
        skip_space(parser);
    }
    json_free(array);
    return NULL;
}

static json_value *parse_object(json_parser *parser) {
    json_value *object = new_value(parser, JSON_OBJECT);
    if (object == NULL) return NULL;
    ++parser->cursor;
    skip_space(parser);
    if (parser->cursor < parser->end && *parser->cursor == '}') {
        ++parser->cursor;
        return object;
    }
    for (;;) {
        json_string key = {0};
        json_value *value;
        if (!parse_string(parser, &key) || memchr(key.data, 0, key.size) != NULL)
            break;
        skip_space(parser);
        if (parser->cursor >= parser->end || *parser->cursor++ != ':') {
            free(key.data);
            break;
        }
        skip_space(parser);
        value = parse_value(parser);
        if (value == NULL || !append_object_member(parser, object, key, value)) {
            free(key.data);
            json_free(value);
            break;
        }
        skip_space(parser);
        if (parser->cursor >= parser->end) break;
        if (*parser->cursor == '}') {
            ++parser->cursor;
            return object;
        }
        if (*parser->cursor++ != ',') break;
        skip_space(parser);
    }
    json_free(object);
    return NULL;
}

static json_value *parse_number(json_parser *parser) {
    const unsigned char *start = parser->cursor;
    json_value *number;
    if (*parser->cursor == '-') ++parser->cursor;
    if (parser->cursor >= parser->end) return NULL;
    if (*parser->cursor == '0') {
        ++parser->cursor;
        if (parser->cursor < parser->end && isdigit(*parser->cursor)) return NULL;
    } else if (*parser->cursor >= '1' && *parser->cursor <= '9') {
        do { ++parser->cursor; }
        while (parser->cursor < parser->end && isdigit(*parser->cursor));
    } else {
        return NULL;
    }
    if (parser->cursor < parser->end && *parser->cursor == '.') {
        ++parser->cursor;
        if (parser->cursor >= parser->end || !isdigit(*parser->cursor)) return NULL;
        do { ++parser->cursor; }
        while (parser->cursor < parser->end && isdigit(*parser->cursor));
    }
    if (parser->cursor < parser->end &&
        (*parser->cursor == 'e' || *parser->cursor == 'E')) {
        ++parser->cursor;
        if (parser->cursor < parser->end &&
            (*parser->cursor == '+' || *parser->cursor == '-')) ++parser->cursor;
        if (parser->cursor >= parser->end || !isdigit(*parser->cursor)) return NULL;
        do { ++parser->cursor; }
        while (parser->cursor < parser->end && isdigit(*parser->cursor));
    }
    number = new_value(parser, JSON_NUMBER);
    if (number == NULL) return NULL;
    number->value.text.size = (size_t)(parser->cursor - start);
    number->value.text.data = malloc(number->value.text.size + 1u);
    if (number->value.text.data == NULL) {
        parser->status = LT48_NO_MEMORY;
        json_free(number);
        return NULL;
    }
    memcpy(number->value.text.data, start, number->value.text.size);
    number->value.text.data[number->value.text.size] = 0u;
    return number;
}

static json_value *parse_value(json_parser *parser) {
    json_value *value = NULL;
    if (++parser->depth > 64u) {
        --parser->depth;
        return NULL;
    }
    skip_space(parser);
    if (parser->cursor >= parser->end) goto done;
    if (*parser->cursor == '{') value = parse_object(parser);
    else if (*parser->cursor == '[') value = parse_array(parser);
    else if (*parser->cursor == '"') {
        value = new_value(parser, JSON_STRING);
        if (value != NULL && !parse_string(parser, &value->value.text)) {
            json_free(value);
            value = NULL;
        }
    } else if (*parser->cursor == '-' || isdigit(*parser->cursor)) {
        value = parse_number(parser);
    } else if ((size_t)(parser->end - parser->cursor) >= 4u &&
               memcmp(parser->cursor, "null", 4u) == 0) {
        parser->cursor += 4u;
        value = new_value(parser, JSON_NULL);
    } else if ((size_t)(parser->end - parser->cursor) >= 4u &&
               memcmp(parser->cursor, "true", 4u) == 0) {
        parser->cursor += 4u;
        value = new_value(parser, JSON_BOOL);
        if (value != NULL) value->value.boolean = 1;
    } else if ((size_t)(parser->end - parser->cursor) >= 5u &&
               memcmp(parser->cursor, "false", 5u) == 0) {
        parser->cursor += 5u;
        value = new_value(parser, JSON_BOOL);
    }
done:
    --parser->depth;
    return value;
}

static json_value *object_find(const json_value *object, const char *key) {
    if (object == NULL || object->type != JSON_OBJECT) return NULL;
    for (size_t index = 0u; index < object->value.object.count; ++index)
        if (string_equal(&object->value.object.items[index].key, key))
            return object->value.object.items[index].value;
    return NULL;
}

static int exact_fields(const json_value *object,
                        const char *const *fields, size_t field_count) {
    if (object == NULL || object->type != JSON_OBJECT ||
        object->value.object.count != field_count) return 0;
    for (size_t index = 0u; index < field_count; ++index)
        if (object_find(object, fields[index]) == NULL) return 0;
    return 1;
}

static int canonical_string(byte_buffer *output, const json_string *string) {
    static const char digits[] = "0123456789abcdef";
    if (!buffer_byte(output, '"')) return 0;
    for (size_t index = 0u; index < string->size; ++index) {
        unsigned char value = string->data[index];
        const char *escape = NULL;
        switch (value) {
        case '"': escape = "\\\""; break;
        case '\\': escape = "\\\\"; break;
        case '\b': escape = "\\b"; break;
        case '\f': escape = "\\f"; break;
        case '\n': escape = "\\n"; break;
        case '\r': escape = "\\r"; break;
        case '\t': escape = "\\t"; break;
        default: break;
        }
        if (escape != NULL) {
            if (!buffer_append(output, escape, 2u)) return 0;
        } else if (value < 0x20u) {
            unsigned char encoded[] = {
                '\\','u','0','0',
                (unsigned char)digits[value >> 4],
                (unsigned char)digits[value & 0x0fu]
            };
            if (!buffer_append(output, encoded, sizeof(encoded))) return 0;
        } else if (!buffer_byte(output, value)) {
            return 0;
        }
    }
    return buffer_byte(output, '"');
}

static int member_compare(const void *left, const void *right) {
    const json_member *a = *(const json_member *const *)left;
    const json_member *b = *(const json_member *const *)right;
    size_t common = a->key.size < b->key.size ? a->key.size : b->key.size;
    int comparison = memcmp(a->key.data, b->key.data, common);
    if (comparison != 0) return comparison;
    return a->key.size < b->key.size ? -1 : a->key.size > b->key.size;
}

static int canonical_value(const json_value *value, byte_buffer *output,
                           const char *omit_root_member) {
    if (value == NULL) return 0;
    switch (value->type) {
    case JSON_NULL: return buffer_append(output, "null", 4u);
    case JSON_BOOL: return buffer_append(output,
        value->value.boolean ? "true" : "false",
        value->value.boolean ? 4u : 5u);
    case JSON_NUMBER:
        return buffer_append(output, value->value.text.data,
                             value->value.text.size);
    case JSON_STRING:
        return canonical_string(output, &value->value.text);
    case JSON_ARRAY:
        if (!buffer_byte(output, '[')) return 0;
        for (size_t index = 0u; index < value->value.array.count; ++index) {
            if (index != 0u && !buffer_byte(output, ',')) return 0;
            if (!canonical_value(value->value.array.items[index], output, NULL))
                return 0;
        }
        return buffer_byte(output, ']');
    case JSON_OBJECT: {
        json_member **members;
        size_t emitted = 0u;
        if (!buffer_byte(output, '{')) return 0;
        if (value->value.object.count == 0u) return buffer_byte(output, '}');
        members = malloc(value->value.object.count * sizeof(*members));
        if (members == NULL) return 0;
        for (size_t index = 0u; index < value->value.object.count; ++index)
            members[index] = &value->value.object.items[index];
        qsort(members, value->value.object.count, sizeof(*members),
              member_compare);
        for (size_t index = 0u; index < value->value.object.count; ++index) {
            json_member *member = members[index];
            if (omit_root_member != NULL &&
                string_equal(&member->key, omit_root_member))
                continue;
            if (emitted++ != 0u && !buffer_byte(output, ',')) goto failed;
            if (!canonical_string(output, &member->key) ||
                !buffer_byte(output, ':') ||
                !canonical_value(member->value, output, NULL)) goto failed;
        }
        free(members);
        return buffer_byte(output, '}');
failed:
        free(members);
        return 0;
    }
    }
    return 0;
}

static int is_string_or_null(const json_value *value) {
    return value != NULL &&
           (value->type == JSON_STRING || value->type == JSON_NULL);
}

static int is_number(const json_value *value) {
    return value != NULL && value->type == JSON_NUMBER;
}

static int is_bool(const json_value *value) {
    return value != NULL && value->type == JSON_BOOL;
}

static int is_digest(const json_value *value) {
    if (value == NULL || value->type != JSON_STRING ||
        value->value.text.size != LT48_SHA256_SIZE * 2u) return 0;
    for (size_t index = 0u; index < value->value.text.size; ++index)
        if (!((value->value.text.data[index] >= '0' &&
               value->value.text.data[index] <= '9') ||
              (value->value.text.data[index] >= 'a' &&
               value->value.text.data[index] <= 'f'))) return 0;
    return 1;
}

static int validate_error(const json_value *value, unsigned depth) {
    static const char *const two[] = {"type", "message"};
    static const char *const three[] = {"type", "message", "children"};
    json_value *children;
    if (value == NULL || depth > 32u) return 0;
    if (value->type == JSON_NULL) return 1;
    if (!exact_fields(value, two, 2u) && !exact_fields(value, three, 3u))
        return 0;
    if (object_find(value, "type")->type != JSON_STRING ||
        object_find(value, "message")->type != JSON_STRING) return 0;
    children = object_find(value, "children");
    if (children == NULL) return 1;
    if (children->type != JSON_ARRAY) return 0;
    for (size_t index = 0u; index < children->value.array.count; ++index)
        if (!validate_error(children->value.array.items[index], depth + 1u))
            return 0;
    return 1;
}

static int strings_have_no_nul(const json_value *value) {
    if (value == NULL) return 0;
    if (value->type == JSON_STRING) {
        return memchr(value->value.text.data, 0, value->value.text.size) == NULL;
    }
    if (value->type == JSON_ARRAY) {
        for (size_t index = 0u; index < value->value.array.count; ++index)
            if (!strings_have_no_nul(value->value.array.items[index])) return 0;
    } else if (value->type == JSON_OBJECT) {
        for (size_t index = 0u; index < value->value.object.count; ++index) {
            const json_member *member = &value->value.object.items[index];
            if (memchr(member->key.data, 0, member->key.size) != NULL ||
                !strings_have_no_nul(member->value)) return 0;
        }
    }
    return 1;
}

static int validate_report_schema(const json_value *root) {
    static const char *const root_fields_v1[] = {
        "schema_version", "run_id", "device", "operation", "status",
        "support_level", "plan_sha256", "input_image_sha256", "output",
        "resource_objects", "planned_voltages_centivolts", "started_at_utc",
        "finished_at_utc", "duration_ms", "phases", "session_events",
        "cleanup", "verification_result", "progress_observer_error_count",
        "failure", "report_sha256"
    };
    static const char *const root_fields_v2[] = {
        "schema_version", "run_id", "device", "operation", "status",
        "support_level", "plan_sha256", "input_image_sha256", "output",
        "resource_objects", "planned_voltages_centivolts", "started_at_utc",
        "finished_at_utc", "duration_ms", "phases", "session_events",
        "cleanup", "verification_result", "progress_observer_error_count",
        "failure", "report_sha256", "address_range", "mismatch_override",
        "cancelled", "observed_result"
    };
    static const char *const root_fields_v3[] = {
        "schema_version", "run_id", "device", "operation", "status",
        "support_level", "plan_sha256", "input_image_sha256", "output",
        "resource_objects", "planned_voltages_centivolts", "started_at_utc",
        "finished_at_utc", "duration_ms", "phases", "session_events",
        "cleanup", "verification_result", "progress_observer_error_count",
        "failure", "report_sha256", "address_range", "mismatch_override",
        "cancelled", "observed_result", "programmer"
    };
    static const char *const range_fields[] = {"length", "start"};
    static const char *const device_fields[] = {
        "key", "definition", "device_class", "source_sha256",
        "primary_engine", "semantic_family", "normalized_engine_profile"
    };
    static const char *const output_fields[] = {"size", "sha256"};
    static const char *const resource_fields[] = {"size", "sha256"};
    static const char *const phase_fields[] = {
        "sequence", "phase", "category", "started_offset_ms", "duration_ms",
        "status", "error_type", "error_message"
    };
    static const char *const event_fields[] = {
        "sequence", "component", "state", "offset_ms", "error_type",
        "error_message"
    };
    static const char *const cleanup_fields[] = {
        "phase_count", "failed_phase_count", "transport_close_status"
    };
    json_value *schema;
    json_value *device;
    json_value *output;
    json_value *resources;
    json_value *phases;
    json_value *events;
    json_value *cleanup;
    unsigned schema_version;
    if (!strings_have_no_nul(root) || root->type != JSON_OBJECT) return 0;
    schema = object_find(root, "schema_version");
    if (!is_number(schema)) return 0;
    if (string_equal(&schema->value.text, "1")) schema_version = 1u;
    else if (string_equal(&schema->value.text, "2")) schema_version = 2u;
    else if (string_equal(&schema->value.text, "3")) schema_version = 3u;
    else if (string_equal(&schema->value.text, "4")) schema_version = 4u;
    else return 0;
    if (schema_version == 1u) {
        if (!exact_fields(root, root_fields_v1,
                          sizeof(root_fields_v1) /
                          sizeof(root_fields_v1[0]))) return 0;
    } else {
        json_value *range;
        if (!exact_fields(root,
                          schema_version >= 3u ? root_fields_v3 : root_fields_v2,
                          sizeof(root_fields_v2) /
                          sizeof(root_fields_v2[0]) +
                              (schema_version >= 3u ? 1u : 0u))) return 0;
        range = object_find(root, "address_range");
        if (!exact_fields(range, range_fields, 2u) ||
            !is_number(object_find(range, "start")) ||
            !is_number(object_find(range, "length")) ||
            !is_bool(object_find(root, "mismatch_override")) ||
            !is_bool(object_find(root, "cancelled")) ||
            !is_string_or_null(object_find(root, "observed_result"))) return 0;
        if (schema_version >= 3u) {
            json_value *programmer = object_find(root, "programmer");
            static const char *const programmer_fields[] = {
                "bus", "address", "product_id", "device_version_bcd",
                "model", "serial"
            };
            static const char *const programmer_fields_v4[] = {
                "bus", "address", "product_id", "device_version_bcd",
                "firmware_version", "model", "serial"
            };
            if (programmer->type != JSON_NULL &&
                (!exact_fields(programmer,
                    schema_version == 4u ? programmer_fields_v4 : programmer_fields,
                    schema_version == 4u ? 7u : 6u) ||
                 !is_number(object_find(programmer, "bus")) ||
                 !is_number(object_find(programmer, "address")) ||
                 !is_number(object_find(programmer, "product_id")) ||
                 !is_number(object_find(programmer, "device_version_bcd")) ||
                 (schema_version == 4u &&
                  object_find(programmer, "firmware_version")->type != JSON_STRING) ||
                 object_find(programmer, "model")->type != JSON_STRING ||
                 object_find(programmer, "serial")->type != JSON_STRING))
                return 0;
        }
    }
    if (object_find(root, "run_id")->type != JSON_STRING ||
        object_find(root, "operation")->type != JSON_STRING ||
        object_find(root, "status")->type != JSON_STRING ||
        object_find(root, "support_level")->type != JSON_STRING ||
        !is_digest(object_find(root, "plan_sha256")) ||
        !is_string_or_null(object_find(root, "input_image_sha256")) ||
        object_find(root, "planned_voltages_centivolts")->type != JSON_ARRAY ||
        object_find(root, "started_at_utc")->type != JSON_STRING ||
        !is_string_or_null(object_find(root, "finished_at_utc")) ||
        !is_number(object_find(root, "duration_ms")) ||
        object_find(root, "verification_result")->type != JSON_STRING ||
        !is_number(object_find(root, "progress_observer_error_count")) ||
        !is_digest(object_find(root, "report_sha256")) ||
        !validate_error(object_find(root, "failure"), 0u)) return 0;
    device = object_find(root, "device");
    if (!exact_fields(device, device_fields,
                      sizeof(device_fields) / sizeof(device_fields[0])) ||
        object_find(device, "key")->type != JSON_STRING ||
        object_find(device, "definition")->type != JSON_STRING ||
        !is_number(object_find(device, "device_class")) ||
        !is_digest(object_find(device, "source_sha256")) ||
        object_find(device, "primary_engine")->type != JSON_STRING ||
        !is_string_or_null(object_find(device, "semantic_family")) ||
        !is_string_or_null(object_find(device, "normalized_engine_profile")))
        return 0;
    output = object_find(root, "output");
    if (!exact_fields(output, output_fields, 2u) ||
        !(is_number(object_find(output, "size")) ||
          object_find(output, "size")->type == JSON_NULL) ||
        !(is_digest(object_find(output, "sha256")) ||
          object_find(output, "sha256")->type == JSON_NULL)) return 0;
    resources = object_find(root, "resource_objects");
    if (resources->type != JSON_OBJECT) return 0;
    for (size_t index = 0u; index < resources->value.object.count; ++index) {
        json_value *resource = resources->value.object.items[index].value;
        if (!exact_fields(resource, resource_fields, 2u) ||
            !is_number(object_find(resource, "size")) ||
            !is_digest(object_find(resource, "sha256"))) return 0;
    }
    phases = object_find(root, "phases");
    if (phases->type != JSON_ARRAY) return 0;
    for (size_t index = 0u; index < phases->value.array.count; ++index) {
        json_value *phase = phases->value.array.items[index];
        if (!exact_fields(phase, phase_fields, 8u) ||
            !is_number(object_find(phase, "sequence")) ||
            object_find(phase, "phase")->type != JSON_STRING ||
            object_find(phase, "category")->type != JSON_STRING ||
            !is_number(object_find(phase, "started_offset_ms")) ||
            !(is_number(object_find(phase, "duration_ms")) ||
              object_find(phase, "duration_ms")->type == JSON_NULL) ||
            object_find(phase, "status")->type != JSON_STRING ||
            !is_string_or_null(object_find(phase, "error_type")) ||
            !is_string_or_null(object_find(phase, "error_message"))) return 0;
    }
    events = object_find(root, "session_events");
    if (events->type != JSON_ARRAY) return 0;
    for (size_t index = 0u; index < events->value.array.count; ++index) {
        json_value *event = events->value.array.items[index];
        if (!exact_fields(event, event_fields, 6u) ||
            !is_number(object_find(event, "sequence")) ||
            object_find(event, "component")->type != JSON_STRING ||
            object_find(event, "state")->type != JSON_STRING ||
            !is_number(object_find(event, "offset_ms")) ||
            !is_string_or_null(object_find(event, "error_type")) ||
            !is_string_or_null(object_find(event, "error_message"))) return 0;
    }
    cleanup = object_find(root, "cleanup");
    return exact_fields(cleanup, cleanup_fields, 3u) &&
           is_number(object_find(cleanup, "phase_count")) &&
           is_number(object_find(cleanup, "failed_phase_count")) &&
           object_find(cleanup, "transport_close_status")->type == JSON_STRING;
}

static int digest_equals(const unsigned char digest[LT48_SHA256_SIZE],
                         const json_string *claimed) {
    static const char digits[] = "0123456789abcdef";
    if (claimed->size != LT48_SHA256_SIZE * 2u) return 0;
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index)
        if (claimed->data[index * 2u] !=
                (unsigned char)digits[digest[index] >> 4] ||
            claimed->data[index * 2u + 1u] !=
                (unsigned char)digits[digest[index] & 0x0fu]) return 0;
    return 1;
}

static lt48_status read_file(const char *path, unsigned char **out_data,
                             size_t *out_size) {
    FILE *file;
    long length;
    unsigned char *data;
    if (path == NULL || path[0] == '\0' || out_data == NULL || out_size == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_data = NULL;
    *out_size = 0u;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    if (fseek(file, 0, SEEK_END) != 0 || (length = ftell(file)) < 0 ||
        length > 16L * 1024L * 1024L || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return LT48_FILE_ERROR;
    }
    data = malloc((size_t)length + 1u);
    if (data == NULL) {
        fclose(file);
        return LT48_NO_MEMORY;
    }
    if (fread(data, 1u, (size_t)length, file) != (size_t)length ||
        fclose(file) != 0) {
        free(data);
        return LT48_FILE_ERROR;
    }
    data[length] = 0u;
    *out_data = data;
    *out_size = (size_t)length;
    return LT48_OK;
}

lt48_status lt48_operation_report_open(
    const char *path, lt48_operation_report **out_report
) {
    lt48_operation_report *report;
    json_parser parser = {0};
    byte_buffer canonical = {0};
    json_value *claimed;
    unsigned char digest[LT48_SHA256_SIZE];
    lt48_status status;
    if (out_report == NULL) return LT48_INVALID_ARGUMENT;
    *out_report = NULL;
    report = calloc(1u, sizeof(*report));
    if (report == NULL) return LT48_NO_MEMORY;
    status = read_file(path, &report->source, &report->source_size);
    if (status != LT48_OK) {
        lt48_operation_report_close(report);
        return status;
    }
    parser.cursor = report->source;
    parser.end = report->source + report->source_size;
    parser.status = LT48_FORMAT_ERROR;
    report->root = parse_value(&parser);
    skip_space(&parser);
    if (report->root == NULL || parser.cursor != parser.end ||
        !validate_report_schema(report->root)) {
        status = parser.status;
        lt48_operation_report_close(report);
        return status;
    }
    claimed = object_find(report->root, "report_sha256");
    if (!canonical_value(report->root, &canonical, "report_sha256")) {
        free(canonical.data);
        lt48_operation_report_close(report);
        return LT48_NO_MEMORY;
    }
    lt48_sha256(canonical.data, canonical.size, digest);
    free(canonical.data);
    if (!digest_equals(digest, &claimed->value.text)) {
        lt48_operation_report_close(report);
        return LT48_FORMAT_ERROR;
    }
    *out_report = report;
    return LT48_OK;
}

void lt48_operation_report_close(lt48_operation_report *report) {
    if (report == NULL) return;
    json_free(report->root);
    free(report->source);
    free(report);
}

static const char *string_value(const json_value *value) {
    return value != NULL && value->type == JSON_STRING ?
           (const char *)value->value.text.data : NULL;
}

static int size_value(const json_value *value, size_t *out) {
    char *end = NULL;
    unsigned long long parsed;
    if (!is_number(value) || value->value.text.size == 0u ||
        value->value.text.data[0] == '-') return 0;
    errno = 0;
    parsed = strtoull((const char *)value->value.text.data, &end, 10);
    if (errno == ERANGE || end == NULL || *end != '\0' || parsed > SIZE_MAX)
        return 0;
    *out = (size_t)parsed;
    return 1;
}

struct lt48_json_document {
    unsigned char *source;
    size_t source_size;
    json_value *root;
};

lt48_status lt48_json_document_open(
    const char *path, lt48_json_document **out_document
) {
    lt48_json_document *document;
    json_parser parser = {0};
    lt48_status status;
    if (path == NULL || out_document == NULL) return LT48_INVALID_ARGUMENT;
    *out_document = NULL;
    document = calloc(1u, sizeof(*document));
    if (document == NULL) return LT48_NO_MEMORY;
    status = read_file(path, &document->source, &document->source_size);
    if (status != LT48_OK) {
        lt48_json_document_close(document);
        return status;
    }
    parser.cursor = document->source;
    parser.end = document->source + document->source_size;
    parser.status = LT48_FORMAT_ERROR;
    document->root = parse_value(&parser);
    skip_space(&parser);
    if (document->root == NULL || parser.cursor != parser.end ||
        !strings_have_no_nul(document->root)) {
        status = parser.status;
        lt48_json_document_close(document);
        return status;
    }
    *out_document = document;
    return LT48_OK;
}

void lt48_json_document_close(lt48_json_document *document) {
    if (document == NULL) return;
    json_free(document->root);
    free(document->source);
    free(document);
}

const lt48_json_value *lt48_json_document_root(
    const lt48_json_document *document
) {
    return document == NULL ? NULL : document->root;
}

lt48_json_type lt48_json_value_type(const lt48_json_value *value) {
    if (value == NULL) return (lt48_json_type)-1;
    switch (value->type) {
    case JSON_NULL: return LT48_JSON_NULL;
    case JSON_BOOL: return LT48_JSON_BOOL;
    case JSON_NUMBER: return LT48_JSON_NUMBER;
    case JSON_STRING: return LT48_JSON_STRING;
    case JSON_ARRAY: return LT48_JSON_ARRAY;
    case JSON_OBJECT: return LT48_JSON_OBJECT;
    }
    return (lt48_json_type)-1;
}

const lt48_json_value *lt48_json_object_member(
    const lt48_json_value *object, const char *key
) {
    return object_find(object, key);
}

int lt48_json_object_exact(
    const lt48_json_value *object,
    const char *const *fields, size_t field_count
) {
    return exact_fields(object, fields, field_count);
}

size_t lt48_json_array_count(const lt48_json_value *array) {
    return array != NULL && array->type == JSON_ARRAY ?
        array->value.array.count : 0u;
}

const lt48_json_value *lt48_json_array_item(
    const lt48_json_value *array, size_t index
) {
    if (array == NULL || array->type != JSON_ARRAY ||
        index >= array->value.array.count) return NULL;
    return array->value.array.items[index];
}

const char *lt48_json_string(const lt48_json_value *value) {
    return string_value(value);
}

int lt48_json_u64(const lt48_json_value *value, uint64_t *out_value) {
    char *end = NULL;
    unsigned long long parsed;
    if (out_value == NULL || !is_number(value) ||
        value->value.text.size == 0u || value->value.text.data[0] == '-')
        return 0;
    errno = 0;
    parsed = strtoull((const char *)value->value.text.data, &end, 10);
    if (errno == ERANGE || end == NULL || *end != '\0' ||
        parsed > UINT64_MAX) return 0;
    *out_value = (uint64_t)parsed;
    return 1;
}

int lt48_json_bool(const lt48_json_value *value, int *out_value) {
    if (value == NULL || value->type != JSON_BOOL || out_value == NULL)
        return 0;
    *out_value = value->value.boolean;
    return 1;
}

lt48_status lt48_json_canonical_sha256(
    const lt48_json_value *value, const char *omit_root_member,
    unsigned char digest[LT48_SHA256_SIZE]
) {
    byte_buffer canonical = {0};
    if (value == NULL || digest == NULL) return LT48_INVALID_ARGUMENT;
    if (!canonical_value(value, &canonical, omit_root_member)) {
        free(canonical.data);
        return LT48_NO_MEMORY;
    }
    lt48_sha256(canonical.data, canonical.size, digest);
    free(canonical.data);
    return LT48_OK;
}

lt48_status lt48_json_canonical_copy(
    const lt48_json_value *value,
    unsigned char **out_data, size_t *out_size
) {
    byte_buffer canonical = {0};
    if (value == NULL || out_data == NULL || out_size == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_data = NULL;
    *out_size = 0u;
    if (!canonical_value(value, &canonical, NULL)) {
        free(canonical.data);
        return LT48_NO_MEMORY;
    }
    *out_data = canonical.data;
    *out_size = canonical.size;
    return LT48_OK;
}

lt48_status lt48_operation_report_get_info(
    const lt48_operation_report *report, lt48_operation_report_info *out_info
) {
    json_value *root;
    json_value *output;
    json_value *cleanup;
    json_value *output_size;
    size_t schema_number;
    if (report == NULL || report->root == NULL || out_info == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_info, 0, sizeof(*out_info));
    root = report->root;
    output = object_find(root, "output");
    cleanup = object_find(root, "cleanup");
    if (!size_value(object_find(root, "schema_version"), &schema_number) ||
        schema_number > UINT32_MAX)
        return LT48_FORMAT_ERROR;
    out_info->schema_version = (uint32_t)schema_number;
    out_info->run_id = string_value(object_find(root, "run_id"));
    out_info->device_key = string_value(
        object_find(object_find(root, "device"), "key")
    );
    out_info->operation = string_value(object_find(root, "operation"));
    out_info->status = string_value(object_find(root, "status"));
    out_info->support_level = string_value(object_find(root, "support_level"));
    out_info->plan_sha256 = string_value(object_find(root, "plan_sha256"));
    out_info->input_image_sha256 = string_value(
        object_find(root, "input_image_sha256")
    );
    output_size = object_find(output, "size");
    if (output_size->type != JSON_NULL) {
        if (!size_value(output_size, &out_info->output_size))
            return LT48_FORMAT_ERROR;
        out_info->has_output = 1;
    }
    out_info->output_sha256 = string_value(object_find(output, "sha256"));
    out_info->resource_count =
        object_find(root, "resource_objects")->value.object.count;
    out_info->phase_count = object_find(root, "phases")->value.array.count;
    out_info->session_event_count =
        object_find(root, "session_events")->value.array.count;
    if (!size_value(object_find(cleanup, "phase_count"),
                    &out_info->cleanup_phase_count) ||
        !size_value(object_find(cleanup, "failed_phase_count"),
                    &out_info->cleanup_failed_phase_count) ||
        !size_value(object_find(root, "progress_observer_error_count"),
                    &out_info->progress_observer_error_count))
        return LT48_FORMAT_ERROR;
    out_info->transport_close_status = string_value(
        object_find(cleanup, "transport_close_status")
    );
    out_info->verification_result = string_value(
        object_find(root, "verification_result")
    );
    out_info->has_failure = object_find(root, "failure")->type != JSON_NULL;
    if (out_info->has_failure) {
        json_value *failure = object_find(root, "failure");
        out_info->failure_type = string_value(object_find(failure, "type"));
        out_info->failure_message = string_value(
            object_find(failure, "message")
        );
    }
    {
        json_value *range = object_find(root, "address_range");
        if (range != NULL) {
            size_t start;
            size_t length;
            if (!size_value(object_find(range, "start"), &start) ||
                !size_value(object_find(range, "length"), &length) ||
                start > UINT32_MAX || length > UINT32_MAX)
                return LT48_FORMAT_ERROR;
            out_info->has_address_range = 1;
            out_info->address_start = (uint32_t)start;
            out_info->address_length = (uint32_t)length;
            out_info->mismatch_override =
                object_find(root, "mismatch_override")->value.boolean;
            out_info->cancelled = object_find(root, "cancelled")->value.boolean;
            out_info->observed_result = string_value(
                object_find(root, "observed_result")
            );
        }
    }
    out_info->report_sha256 = string_value(object_find(root, "report_sha256"));
    return LT48_OK;
}

lt48_status lt48_operation_report_json(
    const lt48_operation_report *report,
    const unsigned char **out_json, size_t *out_size
) {
    if (report == NULL || out_json == NULL || out_size == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_json = report->source;
    *out_size = report->source_size;
    return LT48_OK;
}

static int append_literal(byte_buffer *buffer, const char *text) {
    return buffer_append(buffer, text, strlen(text));
}

static int append_c_string(byte_buffer *buffer, const char *text) {
    json_string value;
    if (text == NULL) return append_literal(buffer, "null");
    value.data = (unsigned char *)text;
    value.size = strlen(text);
    return canonical_string(buffer, &value);
}

static int append_size(byte_buffer *buffer, size_t value) {
    char text[3u * sizeof(size_t) + 2u];
    int length = snprintf(text, sizeof(text), "%zu", value);
    return length > 0 && (size_t)length < sizeof(text) &&
           buffer_append(buffer, text, (size_t)length);
}

static int append_u32(byte_buffer *buffer, uint32_t value) {
    char text[16];
    int length = snprintf(text, sizeof(text), "%u", value);
    return length > 0 && (size_t)length < sizeof(text) &&
           buffer_append(buffer, text, (size_t)length);
}

static int append_milliseconds(byte_buffer *buffer, double value) {
    char text[64];
    int length;
    if (!isfinite(value) || value < 0.0) return 0;
    length = snprintf(text, sizeof(text), "%.3f", value);
    return length > 0 && (size_t)length < sizeof(text) &&
           buffer_append(buffer, text, (size_t)length);
}

static int append_digest(byte_buffer *buffer,
                         const unsigned char digest[LT48_SHA256_SIZE]) {
    static const char digits[] = "0123456789abcdef";
    unsigned char text[LT48_SHA256_SIZE * 2u];
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        text[index * 2u] = (unsigned char)digits[digest[index] >> 4];
        text[index * 2u + 1u] =
            (unsigned char)digits[digest[index] & 0x0fu];
    }
    return buffer_byte(buffer, '"') &&
           buffer_append(buffer, text, sizeof(text)) &&
           buffer_byte(buffer, '"');
}

static int report_plan_valid(const lt48_operation_report_plan *plan) {
    if (plan == NULL || plan->device_key == NULL ||
        plan->definition == NULL || plan->primary_engine == NULL ||
        plan->operation == NULL || plan->support_level == NULL ||
        (plan->resource_count != 0u && plan->resources == NULL) ||
        (plan->planned_voltage_count != 0u &&
         plan->planned_voltages_centivolts == NULL)) return 0;
    for (size_t index = 0u; index < plan->resource_count; ++index)
        if (plan->resources[index].name == NULL ||
            plan->resources[index].name[0] == '\0') return 0;
    return 1;
}

static int resource_pointer_compare(const void *left, const void *right) {
    const lt48_operation_report_resource *const *a = left;
    const lt48_operation_report_resource *const *b = right;
    return strcmp((*a)->name, (*b)->name);
}

static int format_utc_timestamp(struct timespec value, char timestamp[40]) {
    struct tm utc;
#if defined(_WIN32)
    if (gmtime_s(&utc, &value.tv_sec) != 0) return 0;
#else
    if (gmtime_r(&value.tv_sec, &utc) == NULL) return 0;
#endif
    if (strftime(timestamp, 24u, "%Y-%m-%dT%H:%M:%S", &utc) == 0u) return 0;
    snprintf(timestamp + 19u, 21u, ".%09ld+00:00", value.tv_nsec);
    return 1;
}

static int make_report_identity(
    const lt48_operation_report_plan *plan, double duration_ms,
    char run_id[37], char started_at[40], char finished_at[40]
) {
    static atomic_uint_fast64_t sequence = 0;
    struct timespec now;
    struct timespec started;
    uint64_t serial;
    unsigned char seed[sizeof(now) + sizeof(serial) + LT48_SHA256_SIZE];
    unsigned char digest[LT48_SHA256_SIZE];
    size_t offset = 0u;
    if (timespec_get(&now, TIME_UTC) != TIME_UTC) return 0;
    started = now;
    if (duration_ms > 0.0) {
        double seconds = duration_ms / 1000.0;
        time_t whole = (time_t)seconds;
        long nanos = (long)((seconds - (double)whole) * 1000000000.0);
        if (whole > now.tv_sec) return 0;
        started.tv_sec -= whole;
        started.tv_nsec -= nanos;
        if (started.tv_nsec < 0) {
            --started.tv_sec;
            started.tv_nsec += 1000000000L;
        }
    }
    if (!format_utc_timestamp(started, started_at) ||
        !format_utc_timestamp(now, finished_at)) return 0;
    serial = atomic_fetch_add_explicit(&sequence, 1u, memory_order_relaxed);
    memcpy(seed + offset, &now, sizeof(now));
    offset += sizeof(now);
    memcpy(seed + offset, &serial, sizeof(serial));
    offset += sizeof(serial);
    memcpy(seed + offset, plan->plan_sha256, LT48_SHA256_SIZE);
    offset += LT48_SHA256_SIZE;
    lt48_sha256(seed, offset, digest);
    snprintf(run_id, 37u,
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
             "%02x%02x%02x%02x%02x%02x",
             digest[0], digest[1], digest[2], digest[3],
             digest[4], digest[5], digest[6], digest[7],
             digest[8], digest[9], digest[10], digest[11],
             digest[12], digest[13], digest[14], digest[15]);
    return 1;
}

static int append_report_resources(
    byte_buffer *buffer, const lt48_operation_report_plan *plan
) {
    const lt48_operation_report_resource **sorted = NULL;
    int ok = 0;
    if (!buffer_byte(buffer, '{')) return 0;
    if (plan->resource_count != 0u) {
        if (plan->resource_count > SIZE_MAX / sizeof(*sorted)) return 0;
        sorted = malloc(plan->resource_count * sizeof(*sorted));
        if (sorted == NULL) return 0;
        for (size_t index = 0u; index < plan->resource_count; ++index)
            sorted[index] = &plan->resources[index];
        qsort(sorted, plan->resource_count, sizeof(*sorted),
              resource_pointer_compare);
        for (size_t index = 0u; index < plan->resource_count; ++index) {
            if ((index != 0u && !buffer_byte(buffer, ',')) ||
                !append_c_string(buffer, sorted[index]->name) ||
                !append_literal(buffer, ":{\"sha256\":") ||
                !append_digest(buffer, sorted[index]->sha256) ||
                !append_literal(buffer, ",\"size\":") ||
                !append_size(buffer, sorted[index]->size) ||
                !buffer_byte(buffer, '}')) goto done;
            if (index != 0u &&
                strcmp(sorted[index - 1u]->name, sorted[index]->name) == 0)
                goto done;
        }
    }
    ok = buffer_byte(buffer, '}');
done:
    free(sorted);
    return ok;
}

static int report_result_valid(const lt48_operation_report_result *result) {
    if (result == NULL || result->status == NULL ||
        result->transport_close_status == NULL ||
        result->verification_result == NULL ||
        !isfinite(result->duration_ms) || result->duration_ms < 0.0 ||
        result->duration_ms > 31536000000.0 ||
        (result->phase_count != 0u && result->phases == NULL) ||
        ((result->failure_type == NULL) !=
         (result->failure_message == NULL))) return 0;
    for (size_t index = 0u; index < result->phase_count; ++index) {
        const lt48_operation_report_phase_record *phase =
            &result->phases[index];
        if (phase->phase == NULL || phase->category == NULL ||
            phase->status == NULL || !isfinite(phase->started_offset_ms) ||
            phase->started_offset_ms < 0.0 || !isfinite(phase->duration_ms) ||
            phase->duration_ms < 0.0) return 0;
    }
    return 1;
}

static int append_report_phases(
    byte_buffer *buffer, const lt48_operation_report_result *result
) {
    if (!buffer_byte(buffer, '[')) return 0;
    for (size_t index = 0u; index < result->phase_count; ++index) {
        const lt48_operation_report_phase_record *phase =
            &result->phases[index];
        if ((index != 0u && !buffer_byte(buffer, ',')) ||
            !append_literal(buffer, "{\"category\":") ||
            !append_c_string(buffer, phase->category) ||
            !append_literal(buffer, ",\"duration_ms\":") ||
            !append_milliseconds(buffer, phase->duration_ms) ||
            !append_literal(buffer, ",\"error_message\":") ||
            !append_c_string(buffer, phase->error_message) ||
            !append_literal(buffer, ",\"error_type\":") ||
            !append_c_string(buffer, phase->error_type) ||
            !append_literal(buffer, ",\"phase\":") ||
            !append_c_string(buffer, phase->phase) ||
            !append_literal(buffer, ",\"sequence\":") ||
            !append_size(buffer, index) ||
            !append_literal(buffer, ",\"started_offset_ms\":") ||
            !append_milliseconds(buffer, phase->started_offset_ms) ||
            !append_literal(buffer, ",\"status\":") ||
            !append_c_string(buffer, phase->status) ||
            !buffer_byte(buffer, '}')) return 0;
    }
    return buffer_byte(buffer, ']');
}

static int build_report_body(
    byte_buffer *buffer, const lt48_operation_report_plan *plan,
    const lt48_operation_report_result *result,
    const char *run_id, const char *started_at, const char *finished_at
) {
    if (!append_literal(buffer, "{\"address_range\":{\"length\":") ||
        !append_u32(buffer, plan->address_length) ||
        !append_literal(buffer, ",\"start\":") ||
        !append_u32(buffer, plan->address_start) ||
        !append_literal(buffer, "},\"cancelled\":") ||
        !append_literal(buffer, result->cancelled ? "true" : "false") ||
        !append_literal(buffer, ",\"cleanup\":{\"failed_phase_count\":") ||
        !append_size(buffer, result->cleanup_failed_phase_count) ||
        !append_literal(buffer, ",\"phase_count\":") ||
        !append_size(buffer, result->cleanup_phase_count) ||
        !append_literal(buffer, ",\"transport_close_status\":") ||
        !append_c_string(buffer, result->transport_close_status) ||
        !append_literal(buffer, "},\"device\":{\"definition\":") ||
        !append_c_string(buffer, plan->definition) ||
        !append_literal(buffer, ",\"device_class\":") ||
        !append_u32(buffer, plan->device_class) ||
        !append_literal(buffer, ",\"key\":") ||
        !append_c_string(buffer, plan->device_key) ||
        !append_literal(buffer, ",\"normalized_engine_profile\":") ||
        !append_c_string(buffer, plan->normalized_engine_profile) ||
        !append_literal(buffer, ",\"primary_engine\":") ||
        !append_c_string(buffer, plan->primary_engine) ||
        !append_literal(buffer, ",\"semantic_family\":") ||
        !append_c_string(buffer, plan->semantic_family) ||
        !append_literal(buffer, ",\"source_sha256\":") ||
        !append_digest(buffer, plan->source_sha256) ||
        !append_literal(buffer, "},\"duration_ms\":") ||
        !append_milliseconds(buffer, result->duration_ms) ||
        !append_literal(buffer, ",\"failure\":") ||
        !(result->failure_type == NULL ? append_literal(buffer, "null") :
          (append_literal(buffer, "{\"message\":") &&
           append_c_string(buffer, result->failure_message) &&
           append_literal(buffer, ",\"type\":") &&
           append_c_string(buffer, result->failure_type) &&
           buffer_byte(buffer, '}'))) ||
        !append_literal(buffer, ",\"finished_at_utc\":") ||
        !append_c_string(buffer, finished_at) ||
        !append_literal(buffer, ",\"input_image_sha256\":") ||
        !(plan->has_input_image ?
          append_digest(buffer, plan->input_image_sha256) :
          append_literal(buffer, "null")) ||
        !append_literal(buffer, ",\"mismatch_override\":") ||
        !append_literal(buffer, result->mismatch_override ? "true" : "false") ||
        !append_literal(buffer, ",\"observed_result\":") ||
        !append_c_string(buffer, result->observed_result) ||
        !append_literal(buffer, ",\"operation\":") ||
        !append_c_string(buffer, plan->operation) ||
        !append_literal(buffer, ",\"output\":{\"sha256\":") ||
        !(result->has_output ? append_digest(buffer, result->output_sha256) :
                              append_literal(buffer, "null")) ||
        !append_literal(buffer, ",\"size\":") ||
        !(result->has_output ? append_size(buffer, result->output_size) :
                              append_literal(buffer, "null")) ||
        !append_literal(buffer, "},\"phases\":") ||
        !append_report_phases(buffer, result) ||
        !append_literal(buffer, ",\"plan_sha256\":") ||
        !append_digest(buffer, plan->plan_sha256) ||
        !append_literal(buffer, ",\"planned_voltages_centivolts\":["))
        return 0;
    for (size_t index = 0u; index < plan->planned_voltage_count; ++index) {
        if ((index != 0u && !buffer_byte(buffer, ',')) ||
            !append_u32(buffer, plan->planned_voltages_centivolts[index]))
            return 0;
    }
    if (!append_literal(buffer, "],\"programmer\":") ||
        !(report_programmer.valid && strcmp(result->status, "planned") != 0 ?
          (append_literal(buffer, "{\"address\":") &&
           append_u32(buffer, report_programmer.device_address) &&
           append_literal(buffer, ",\"bus\":") &&
           append_u32(buffer, report_programmer.bus_number) &&
           append_literal(buffer, ",\"device_version_bcd\":") &&
           append_u32(buffer, report_programmer.device_version_bcd) &&
           append_literal(buffer, ",\"firmware_version\":") &&
           append_c_string(buffer, report_programmer.firmware_version) &&
           append_literal(buffer, ",\"model\":") &&
           append_c_string(buffer, report_programmer.model) &&
           append_literal(buffer, ",\"product_id\":") &&
           append_u32(buffer, report_programmer.product_id) &&
           append_literal(buffer, ",\"serial\":") &&
           append_c_string(buffer, report_programmer.serial) &&
           buffer_byte(buffer, '}')) :
          append_literal(buffer, "null")) ||
        !append_literal(buffer, ",\"progress_observer_error_count\":") ||
        !append_size(buffer, result->progress_observer_error_count) ||
        !append_literal(buffer, ",\"resource_objects\":") ||
        !append_report_resources(buffer, plan) ||
        !append_literal(buffer, ",\"run_id\":") ||
        !append_c_string(buffer, run_id) ||
        !append_literal(buffer,
            ",\"schema_version\":4,\"session_events\":[],"
            "\"started_at_utc\":") ||
        !append_c_string(buffer, started_at) ||
        !append_literal(buffer, ",\"status\":") ||
        !append_c_string(buffer, result->status) ||
        !append_literal(buffer, ",\"support_level\":") ||
        !append_c_string(buffer, plan->support_level) ||
        !append_literal(buffer, ",\"verification_result\":") ||
        !append_c_string(buffer, result->verification_result) ||
        !buffer_byte(buffer, '}')) return 0;
    return 1;
}

lt48_status lt48_operation_report_write_result(
    const char *path, const lt48_operation_report_plan *plan,
    const lt48_operation_report_result *result
) {
    byte_buffer unhashed = {0};
    byte_buffer report = {0};
    unsigned char digest[LT48_SHA256_SIZE];
    char run_id[37];
    char started_at[40];
    char finished_at[40];
    FILE *file = NULL;
    lt48_status status = LT48_OK;
    if (path == NULL || path[0] == '\0' || !report_plan_valid(plan) ||
        !report_result_valid(result))
        return LT48_INVALID_ARGUMENT;
    if (!make_report_identity(plan, result->duration_ms, run_id,
                              started_at, finished_at))
        return LT48_RUNTIME_ERROR;
    if (!build_report_body(&unhashed, plan, result, run_id,
                           started_at, finished_at)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    lt48_sha256(unhashed.data, unhashed.size, digest);
    if (unhashed.size == 0u || unhashed.data[unhashed.size - 1u] != '}' ||
        !buffer_append(&report, unhashed.data, unhashed.size - 1u) ||
        !append_literal(&report, ",\"report_sha256\":") ||
        !append_digest(&report, digest) || !append_literal(&report, "}\n")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    file = fopen(path, "wx");
    if (file == NULL) {
        status = LT48_FILE_ERROR;
        goto done;
    }
    if (fwrite(report.data, 1u, report.size, file) != report.size ||
        fclose(file) != 0) {
        file = NULL;
        (void)remove(path);
        status = LT48_FILE_ERROR;
        goto done;
    }
    file = NULL;
done:
    if (file != NULL) {
        (void)fclose(file);
        (void)remove(path);
    }
    free(report.data);
    free(unhashed.data);
    return status;
}

lt48_status lt48_operation_report_write_planned(
    const char *path, const lt48_operation_report_plan *plan
) {
    const lt48_operation_report_result result = {
        "planned", 0.0, NULL, 0u, 0, 0u, {0}, 0u, 0u,
        "not-attempted", "not-applicable", 0u, 0, 0, NULL, NULL, NULL
    };
    return lt48_operation_report_write_result(path, plan, &result);
}

static lt48_status report_file_size(const char *path, size_t *out_size) {
    FILE *file;
    long size;
    if (path == NULL || out_size == NULL) return LT48_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    if (fseek(file, 0, SEEK_END) != 0 || (size = ftell(file)) < 0) {
        (void)fclose(file);
        return LT48_FILE_ERROR;
    }
    if (fclose(file) != 0) return LT48_FILE_ERROR;
    *out_size = (size_t)size;
    return LT48_OK;
}

static lt48_status write_m37_operation_report(
    const char *path, const lt48_installation *installation,
    size_t device_index, const lt48_m37_resources *resources,
    const lt48_protocol_plan *protocol_plan, lt48_m37_operation operation,
    uint32_t address_start, uint32_t address_length,
    const unsigned char *input_image_sha256,
    const unsigned char *plan_sha256_override,
    const uint32_t *planned_voltages_override,
    size_t planned_voltage_count,
    const lt48_operation_report_result *result
) {
    lt48_device_spec spec;
    lt48_catalogue_entry entry;
    const lt48_m37_resource_audit *audit;
    lt48_operation_report_plan report = {0};
    lt48_operation_report_resource *objects = NULL;
    char (*names)[48] = NULL;
    char *device_name = NULL;
    char *device_key = NULL;
    char definition[96];
    char definition_path[1024];
    const unsigned char *payload;
    size_t payload_size;
    size_t object_count = 0u;
    size_t capacity;
    uint32_t voltage;
    const char *operation_name;
    lt48_status status;
    if (path == NULL || installation == NULL || resources == NULL ||
        (protocol_plan == NULL && plan_sha256_override == NULL) ||
        (planned_voltage_count != 0u && planned_voltages_override == NULL))
        return LT48_INVALID_ARGUMENT;
    switch (operation) {
    case LT48_M37_ELECTRICAL_ERASE: operation_name = "electrical-erase"; break;
    case LT48_M37_READ: operation_name = "read"; break;
    case LT48_M37_BLANK_CHECK: operation_name = "blank-check"; break;
    case LT48_M37_VERIFY: operation_name = "verify"; break;
    case LT48_M37_PROGRAM: operation_name = "program"; break;
    case LT48_M37_COMPARE: operation_name = "compare"; break;
    default: return LT48_UNSUPPORTED;
    }
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL) return LT48_INVALID_ARGUMENT;
    status = lt48_installation_resolve_device(installation, device_index, &spec);
    if (status != LT48_OK) return status;
    status = lt48_installation_get_device(installation, device_index, &entry);
    if (status != LT48_OK) return status;
    device_name = malloc(entry.device_name_length + 1u);
    if (device_name == NULL) return LT48_NO_MEMORY;
    memcpy(device_name, entry.device_name, entry.device_name_length);
    device_name[entry.device_name_length] = '\0';
    if (strlen(entry.vendor) > SIZE_MAX - entry.device_name_length - 2u) {
        status = LT48_INVALID_ARGUMENT;
        goto done;
    }
    device_key = malloc(strlen(entry.vendor) + entry.device_name_length + 2u);
    if (device_key == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    sprintf(device_key, "%s:%s", entry.vendor, device_name);
    capacity = 3u + lt48_m37_resources_applet_count(resources);
    objects = calloc(capacity, sizeof(*objects));
    names = calloc(capacity, sizeof(*names));
    if (objects == NULL || names == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    snprintf(definition, sizeof(definition), "%s.rez:D%x",
             spec.definition_stem, spec.catalogue.device_type);
    if (snprintf(definition_path, sizeof(definition_path), "%s/%s.rez",
                 lt48_installation_path(installation), spec.definition_stem) >=
        (int)sizeof(definition_path)) {
        status = LT48_INVALID_ARGUMENT;
        goto done;
    }
    strcpy(names[object_count], "definition-rez");
    objects[object_count].name = names[object_count];
    status = report_file_size(definition_path, &objects[object_count].size);
    if (status != LT48_OK) goto done;
    memcpy(objects[object_count].sha256, audit->definition_sha256,
           LT48_SHA256_SIZE);
    ++object_count;
    status = lt48_m37_resources_active_fpga(resources, &payload, &payload_size);
    if (status != LT48_OK) goto done;
    strcpy(names[object_count], "active-fpga");
    objects[object_count].name = names[object_count];
    objects[object_count].size = payload_size;
    lt48_sha256(payload, payload_size, objects[object_count].sha256);
    ++object_count;
    status = lt48_m37_resources_idle_fpga(resources, &payload, &payload_size);
    if (status != LT48_OK) goto done;
    strcpy(names[object_count], "idle-fpga");
    objects[object_count].name = names[object_count];
    objects[object_count].size = payload_size;
    lt48_sha256(payload, payload_size, objects[object_count].sha256);
    ++object_count;
    for (size_t index = 0u;
         index < lt48_m37_resources_applet_count(resources); ++index) {
        lt48_m37_applet_resource applet;
        status = lt48_m37_resources_get_applet(resources, index, &applet);
        if (status != LT48_OK) goto done;
        snprintf(names[object_count], sizeof(names[object_count]),
                 "applet-%u-%u", applet.operation_id, applet.wave);
        objects[object_count].name = names[object_count];
        objects[object_count].size = applet.data_size;
        lt48_sha256(applet.data, applet.data_size,
                    objects[object_count].sha256);
        ++object_count;
    }
    voltage = audit->nominal_vcc_centivolts;
    report.device_key = device_key;
    report.definition = definition;
    report.device_class = spec.catalogue.device_class;
    memcpy(report.source_sha256, audit->definition_sha256, LT48_SHA256_SIZE);
    report.primary_engine = "range-02-00425c50";
    report.semantic_family = "byte-stream-memory";
    report.normalized_engine_profile = audit->complete_source_audit_verified ?
        "byte-stream-eprom/source-audited" : "byte-stream-memory/direct-erase";
    report.operation = operation_name;
    report.support_level = audit->complete_source_audit_verified ?
        (audit->capacity_bytes>=131072u?"large_eprom_source_audited":"byte_eprom_source_audited") : audit->complete_group_equivalence_verified ?
        "source_group_equivalence_verified" : operation == LT48_M37_READ ?
        (audit->read_original_replay_verified ? "original_windows_replay_verified" :
         audit->read_behavior_approved ? "engine_hardware_verified" :
         audit->read_forward_reference_verified ? "original_read_forward_verified" :
         "workflow_reconstructed") : "planned";
    report.address_start = address_start;
    report.address_length = address_length;
    if (plan_sha256_override != NULL) {
        memcpy(report.plan_sha256, plan_sha256_override, LT48_SHA256_SIZE);
    } else {
        status = lt48_protocol_plan_sha256(protocol_plan, report.plan_sha256);
        if (status != LT48_OK) goto done;
    }
    if (input_image_sha256 != NULL) {
        report.has_input_image = 1;
        memcpy(report.input_image_sha256, input_image_sha256,
               LT48_SHA256_SIZE);
    }
    report.resources = objects;
    report.resource_count = object_count;
    report.planned_voltages_centivolts = planned_voltage_count != 0u ?
        planned_voltages_override : &voltage;
    report.planned_voltage_count = planned_voltage_count != 0u ?
        planned_voltage_count : 1u;
    status = result == NULL ?
        lt48_operation_report_write_planned(path, &report) :
        lt48_operation_report_write_result(path, &report, result);
done:
    free(names);
    free(objects);
    free(device_key);
    free(device_name);
    return status;
}

lt48_status lt48_m37_operation_report_write_planned(
    const char *path, const lt48_installation *installation,
    size_t device_index, const lt48_m37_resources *resources,
    const lt48_protocol_plan *protocol_plan, lt48_m37_operation operation,
    uint32_t address_start, uint32_t address_length,
    const unsigned char *input_image_sha256
) {
    return write_m37_operation_report(
        path, installation, device_index, resources, protocol_plan, operation,
        address_start, address_length, input_image_sha256, NULL, NULL, 0u, NULL
    );
}

lt48_status lt48_m37_operation_report_write_verify_plan_set(
    const char *path, const lt48_installation *installation,
    size_t device_index, const lt48_m37_resources *resources,
    const lt48_m37_verify_plan_set *plan_set,
    uint32_t address_start, uint32_t address_length,
    const unsigned char input_image_sha256[LT48_SHA256_SIZE]
) {
    unsigned char digest[LT48_SHA256_SIZE];
    uint32_t voltages[4];
    char phase_names[4][40];
    lt48_operation_report_phase_record phases[4] = {0};
    lt48_operation_report_result result = {
        "planned", 0.0, phases, 0u, 0, 0u, {0}, 0u, 0u,
        "not-attempted", "planned", 0u, 0, 0, NULL, NULL, NULL
    };
    size_t count = lt48_m37_verify_plan_set_count(plan_set);
    lt48_status status;
    if (path == NULL || plan_set == NULL || input_image_sha256 == NULL ||
        count == 0u || count > 4u) return LT48_INVALID_ARGUMENT;
    status = lt48_m37_verify_plan_set_sha256(plan_set, digest);
    for (size_t index = 0u; status == LT48_OK && index < count; ++index) {
        const lt48_protocol_plan *pass;
        uint16_t voltage;
        status = lt48_m37_verify_plan_set_get(
            plan_set, index, &voltage, &pass
        );
        if (status == LT48_OK) {
            voltages[index] = voltage;
            snprintf(phase_names[index], sizeof(phase_names[index]),
                     "verify-%u-centivolts", (unsigned)voltage);
            phases[index].phase = phase_names[index];
            phases[index].category = "verification";
            phases[index].status = "planned";
        }
    }
    if (status != LT48_OK) return status;
    result.phase_count = count;
    return write_m37_operation_report(
        path, installation, device_index, resources, NULL, LT48_M37_VERIFY,
        address_start, address_length, input_image_sha256, digest,
        voltages, count, &result
    );
}

lt48_status lt48_m37_operation_report_write_program_plan_set(
    const char *path, const lt48_installation *installation,
    size_t device_index, const lt48_m37_resources *resources,
    const lt48_m37_program_plan_set *plan_set,
    uint32_t address_start, uint32_t address_length,
    const unsigned char input_image_sha256[LT48_SHA256_SIZE]
) {
    lt48_operation_report_phase_record phases[] = {
        {"blank-precheck", "precheck", 0.0, 0.0, "planned", NULL, NULL},
        {"program", "execution", 0.0, 0.0, "planned", NULL, NULL},
        {"program-verify", "execution", 0.0, 0.0, "planned", NULL, NULL},
        {"mandatory-readback", "verification", 0.0, 0.0, "planned", NULL, NULL}
    };
    size_t count = lt48_m37_program_plan_set_count(plan_set);
    lt48_operation_report_result result = {
        "planned", 0.0, phases, count,
        0, 0u, {0}, 0u, 0u, "not-attempted", "planned", 0u,
        0, 0, NULL, NULL, NULL
    };
    if (count == 3u) phases[2] = phases[3];
    unsigned char digest[LT48_SHA256_SIZE];
    lt48_status status;
    if (path == NULL || plan_set == NULL || input_image_sha256 == NULL ||
        (lt48_m37_program_plan_set_count(plan_set) != 3u &&
         lt48_m37_program_plan_set_count(plan_set) != 4u))
        return LT48_INVALID_ARGUMENT;
    status = lt48_m37_program_plan_set_sha256(plan_set, digest);
    if (status != LT48_OK) return status;
    return write_m37_operation_report(
        path, installation, device_index, resources, NULL, LT48_M37_PROGRAM,
        address_start, address_length, input_image_sha256, digest,
        NULL, 0u, &result
    );
}

static const char *report_status_for(lt48_status status) {
    if (status == LT48_OK) return "succeeded";
    if (status == LT48_CANCELLED) return "cancelled";
    if (status == LT48_SAFETY_REJECTED ||
        status == LT48_FIRMWARE_REQUIRED) return "safety-rejected";
    return "failed";
}

static lt48_status build_auto_id_report_plan(
    const lt48_auto_id_eprom_preparation *preparation,
    lt48_operation_report_plan *out_plan,
    lt48_operation_report_resource *out_resource,
    uint32_t voltages[2],
    char device_key[48]
) {
    lt48_auto_id_eprom_preparation rebuilt;
    int key_length;
    lt48_status status;
    if (preparation == NULL || out_plan == NULL || out_resource == NULL ||
        voltages == NULL || device_key == NULL)
        return LT48_INVALID_ARGUMENT;
    status = lt48_auto_id_eprom_prepare(
        preparation->bus_number, preparation->device_address,
        preparation->acquire_signature,
        preparation->plan.has_explicit_pin_hint,
        preparation->plan.explicit_pin_hint, &rebuilt
    );
    if (status != LT48_OK) return status;
    if (memcmp(&rebuilt, preparation, sizeof(rebuilt)) != 0)
        return LT48_SAFETY_REJECTED;
    key_length = snprintf(
        device_key, 48u, "auto-select-eprom@%03u:%03u",
        preparation->bus_number, preparation->device_address
    );
    if (key_length < 0 || key_length >= 48) return LT48_RUNTIME_ERROR;
    memset(out_resource, 0, sizeof(*out_resource));
    out_resource->name = "auto-select-eprom-provider";
    out_resource->size = preparation->plan.recipe_count * 32u;
    memcpy(out_resource->sha256, preparation->plan.provider_sha256,
           LT48_SHA256_SIZE);
    voltages[0] = preparation->plan.maximum_operating_vcc_centivolts;
    voltages[1] = preparation->plan.maximum_signature_vpp_centivolts;
    memset(out_plan, 0, sizeof(*out_plan));
    out_plan->device_key = device_key;
    out_plan->definition = "auto-select-eprom-six-recipe";
    memcpy(out_plan->source_sha256, preparation->plan.provider_sha256,
           LT48_SHA256_SIZE);
    out_plan->primary_engine = "auto-select-eprom";
    out_plan->semantic_family = "parallel-memory-identification";
    out_plan->normalized_engine_profile =
        preparation->acquire_signature ?
        "six-package-high-voltage-signature-v1" :
        "socket-contact-probe-v1";
    out_plan->operation = "auto-id";
    out_plan->support_level = "plan-only";
    memcpy(out_plan->plan_sha256, preparation->confirmation_sha256,
           LT48_SHA256_SIZE);
    out_plan->resources = out_resource;
    out_plan->resource_count = 1u;
    out_plan->planned_voltages_centivolts = voltages;
    out_plan->planned_voltage_count = preparation->acquire_signature ? 2u : 0u;
    return LT48_OK;
}

lt48_status lt48_auto_id_eprom_operation_report_write_planned(
    const char *path,
    const lt48_auto_id_eprom_preparation *preparation
) {
    lt48_operation_report_plan plan;
    lt48_operation_report_resource resource;
    uint32_t voltages[2];
    char device_key[48];
    lt48_status status;
    if (path == NULL) return LT48_INVALID_ARGUMENT;
    status = build_auto_id_report_plan(
        preparation, &plan, &resource, voltages, device_key);
    if (status != LT48_OK) return status;
    return lt48_operation_report_write_planned(path, &plan);
}

lt48_status lt48_auto_id_eprom_operation_report_write_execution(
    const char *path,
    const lt48_auto_id_eprom_preparation *preparation,
    const lt48_auto_id_eprom_execution_result *execution
) {
    lt48_operation_report_plan plan;
    lt48_operation_report_resource resource;
    lt48_operation_report_phase_record phases[3] = {{0}};
    lt48_operation_report_result result = {0};
    uint32_t voltages[2];
    char device_key[48];
    char observed[128];
    size_t phase_count = 2u;
    lt48_status command_status;
    lt48_status status;
    if (path == NULL || execution == NULL ||
        !isfinite(execution->operation_duration_ms) ||
        !isfinite(execution->finalizer_duration_ms) ||
        !isfinite(execution->interface_release_duration_ms) ||
        !isfinite(execution->total_duration_ms) ||
        execution->operation_duration_ms < 0.0 ||
        execution->finalizer_duration_ms < 0.0 ||
        execution->interface_release_duration_ms < 0.0 ||
        execution->total_duration_ms < 0.0 ||
        fabs(execution->total_duration_ms -
             (execution->operation_duration_ms +
              execution->finalizer_duration_ms +
              execution->interface_release_duration_ms)) > 0.001)
        return LT48_INVALID_ARGUMENT;
    status = build_auto_id_report_plan(
        preparation, &plan, &resource, voltages, device_key);
    if (status != LT48_OK) return status;
    command_status = execution->operation_status;
    if (command_status == LT48_OK && execution->finalizer_status != LT48_OK)
        command_status = LT48_CLEANUP_FAILED;
    if (command_status == LT48_OK && execution->interface_release_attempted &&
        execution->interface_release_status != LT48_OK)
        command_status = LT48_CLEANUP_FAILED;
    phases[0].phase = "auto-id";
    phases[0].category = "execution";
    phases[0].duration_ms = execution->operation_duration_ms;
    phases[0].status = report_status_for(execution->operation_status);
    if (execution->operation_status != LT48_OK) {
        phases[0].error_type = lt48_status_string(execution->operation_status);
        phases[0].error_message = lt48_status_string(execution->operation_status);
    }
    phases[1].phase = "controller-finalizer";
    phases[1].category = "cleanup";
    phases[1].started_offset_ms = execution->operation_duration_ms;
    phases[1].duration_ms = execution->finalizer_duration_ms;
    phases[1].status = report_status_for(execution->finalizer_status);
    if (execution->finalizer_status != LT48_OK) {
        phases[1].error_type = lt48_status_string(execution->finalizer_status);
        phases[1].error_message = lt48_status_string(execution->finalizer_status);
    }
    if (execution->interface_release_attempted) {
        phases[2].phase = "release-interface";
        phases[2].category = "cleanup";
        phases[2].started_offset_ms =
            execution->operation_duration_ms +
            execution->finalizer_duration_ms;
        phases[2].duration_ms = execution->interface_release_duration_ms;
        phases[2].status = report_status_for(
            execution->interface_release_status);
        if (execution->interface_release_status != LT48_OK) {
            phases[2].error_type = lt48_status_string(
                execution->interface_release_status);
            phases[2].error_message = lt48_status_string(
                execution->interface_release_status);
        }
        phase_count = 3u;
    }
    if (execution->has_signature) {
        snprintf(
            observed, sizeof(observed),
            "pins=%u manufacturer=0x%04x device=0x%04x",
            execution->socket.package_pin_count,
            execution->manufacturer_id, execution->device_id
        );
    } else {
        snprintf(
            observed, sizeof(observed), "pins=%u contact-probe-only",
            execution->socket.package_pin_count
        );
    }
    result.status = report_status_for(command_status);
    result.duration_ms = execution->total_duration_ms;
    result.phases = phases;
    result.phase_count = phase_count;
    result.cleanup_phase_count = phase_count - 1u;
    result.cleanup_failed_phase_count =
        (execution->finalizer_status != LT48_OK ? 1u : 0u) +
        (execution->interface_release_attempted &&
         execution->interface_release_status != LT48_OK ? 1u : 0u);
    result.transport_close_status = execution->interface_release_attempted ?
        report_status_for(execution->interface_release_status) :
        "not-attempted";
    result.verification_result = command_status == LT48_OK ?
        (execution->has_signature ? "identified" : "socket-observed") :
        "failed";
    result.observed_result = execution->operation_status == LT48_OK ?
        observed : NULL;
    if (command_status != LT48_OK) {
        result.failure_type = lt48_status_string(command_status);
        result.failure_message = lt48_status_string(command_status);
    }
    return lt48_operation_report_write_result(path, &plan, &result);
}

lt48_status lt48_m37_operation_report_write_execution(
    const char *path, const lt48_installation *installation,
    size_t device_index, const lt48_m37_resources *resources,
    const lt48_protocol_plan *protocol_plan,
    uint32_t address_start, uint32_t address_length,
    lt48_status command_status,
    const lt48_m37_snapshot_read_execution_result *execution,
    const lt48_cleanup_result *finalization_results,
    size_t finalization_result_count,
    const unsigned char *output, size_t output_size
) {
    lt48_operation_report_phase_record *phases = NULL;
    lt48_operation_report_result result = {0};
    const char *transport_close_status = "not-attempted";
    size_t phase_count;
    lt48_status status;
    if (path == NULL || installation == NULL || resources == NULL ||
        protocol_plan == NULL || execution == NULL ||
        finalization_result_count != execution->finalization_phase_count ||
        (finalization_result_count != 0u && finalization_results == NULL) ||
        (output_size != 0u && output == NULL))
        return LT48_INVALID_ARGUMENT;
    if (finalization_result_count == SIZE_MAX) return LT48_INVALID_ARGUMENT;
    phase_count = finalization_result_count + 1u;
    phases = calloc(phase_count, sizeof(*phases));
    if (phases == NULL) return LT48_NO_MEMORY;
    phases[0].phase = "read";
    phases[0].category = "execution";
    phases[0].started_offset_ms = 0.0;
    phases[0].duration_ms = execution->operation_duration_ms;
    phases[0].status = report_status_for(execution->operation_status);
    if (execution->operation_status != LT48_OK) {
        phases[0].error_type = lt48_status_string(execution->operation_status);
        phases[0].error_message =
            lt48_status_string(execution->operation_status);
    }
    for (size_t index = 0u; index < finalization_result_count; ++index) {
        const lt48_cleanup_result *cleanup = &finalization_results[index];
        lt48_operation_report_phase_record *phase = &phases[index + 1u];
        if (cleanup->name == NULL || !isfinite(cleanup->started_offset_ms) ||
            cleanup->started_offset_ms < 0.0 ||
            !isfinite(cleanup->duration_ms) || cleanup->duration_ms < 0.0) {
            free(phases);
            return LT48_INVALID_ARGUMENT;
        }
        phase->phase = cleanup->name;
        phase->category = "cleanup";
        phase->started_offset_ms = cleanup->started_offset_ms;
        phase->duration_ms = cleanup->duration_ms;
        phase->status = report_status_for(cleanup->status);
        if (cleanup->status != LT48_OK) {
            phase->error_type = lt48_status_string(cleanup->status);
            phase->error_message = lt48_status_string(cleanup->status);
        }
        if (strcmp(cleanup->name, "release-interface") == 0)
            transport_close_status = report_status_for(cleanup->status);
    }
    result.status = report_status_for(command_status);
    result.duration_ms = execution->total_duration_ms;
    result.phases = phases;
    result.phase_count = phase_count;
    result.has_output = output != NULL;
    result.output_size = result.has_output ? output_size : 0u;
    if (result.has_output)
        lt48_sha256(output, output_size, result.output_sha256);
    result.cleanup_phase_count = finalization_result_count;
    result.cleanup_failed_phase_count = execution->finalization_failure_count;
    result.transport_close_status = transport_close_status;
    result.verification_result = "not-applicable";
    result.cancelled = command_status == LT48_CANCELLED ||
                       execution->operation_status == LT48_CANCELLED;
    result.observed_result = lt48_status_string(command_status);
    if (command_status != LT48_OK) {
        result.failure_type = lt48_status_string(command_status);
        result.failure_message = lt48_status_string(command_status);
    }
    status = write_m37_operation_report(
        path, installation, device_index, resources, protocol_plan,
        LT48_M37_READ, address_start, address_length, NULL, NULL, NULL, 0u,
        &result
    );
    free(phases);
    return status;
}
