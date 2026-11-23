/* Shared bounded JSON reader for authored bundles and NewDeviceLib. */
#ifndef LT48_JSON_READER_INTERNAL_H
#define LT48_JSON_READER_INTERNAL_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum json_token_type {
    JSON_OBJECT, JSON_ARRAY, JSON_STRING, JSON_PRIMITIVE
} json_token_type;

typedef struct json_token {
    json_token_type type;
    size_t start;
    size_t end;
    size_t parent;
} json_token;

typedef struct json_parser {
    const unsigned char *data;
    size_t size;
    size_t cursor;
    json_token *tokens;
    size_t count;
    size_t capacity;
    int failed;
} json_parser;

static inline void skip_space(json_parser *parser) {
    while (parser->cursor < parser->size &&
           isspace(parser->data[parser->cursor])) ++parser->cursor;
}

static inline size_t new_token(
    json_parser *parser, json_token_type type, size_t start, size_t parent
) {
    json_token *grown;
    size_t index;
    if (parser->count == parser->capacity) {
        size_t capacity = parser->capacity == 0u ? 128u : parser->capacity * 2u;
        if (capacity < parser->capacity ||
            capacity > SIZE_MAX / sizeof(*parser->tokens)) {
            parser->failed = 1;
            return SIZE_MAX;
        }
        grown = realloc(parser->tokens, capacity * sizeof(*parser->tokens));
        if (grown == NULL) {
            parser->failed = 1;
            return SIZE_MAX;
        }
        parser->tokens = grown;
        parser->capacity = capacity;
    }
    index = parser->count++;
    parser->tokens[index] = (json_token){type, start, 0u, parent};
    return index;
}

static inline int parse_value(json_parser *parser, size_t parent);

static inline int parse_string(json_parser *parser, size_t parent) {
    size_t token;
    size_t start;
    if (parser->data[parser->cursor++] != '"') return 0;
    start = parser->cursor;
    token = new_token(parser, JSON_STRING, start, parent);
    if (token == SIZE_MAX) return 0;
    while (parser->cursor < parser->size) {
        unsigned char value = parser->data[parser->cursor++];
        if (value == '"') {
            parser->tokens[token].end = parser->cursor - 1u;
            return 1;
        }
        if (value < 0x20u) return 0;
        if (value == '\\') {
            if (parser->cursor >= parser->size) return 0;
            value = parser->data[parser->cursor++];
            if (value == 'u') {
                for (unsigned index = 0u; index < 4u; ++index) {
                    if (parser->cursor >= parser->size ||
                        !isxdigit(parser->data[parser->cursor++])) return 0;
                }
            } else if (strchr("\"\\/bfnrt", value) == NULL) {
                return 0;
            }
        }
    }
    return 0;
}

static inline int parse_compound(
    json_parser *parser, size_t parent, json_token_type type,
    unsigned char open, unsigned char close
) {
    size_t token;
    if (parser->data[parser->cursor++] != open) return 0;
    token = new_token(parser, type, parser->cursor - 1u, parent);
    if (token == SIZE_MAX) return 0;
    skip_space(parser);
    if (parser->cursor < parser->size && parser->data[parser->cursor] == close) {
        parser->tokens[token].end = ++parser->cursor;
        return 1;
    }
    for (;;) {
        if (type == JSON_OBJECT) {
            if (parser->cursor >= parser->size ||
                parser->data[parser->cursor] != '"' ||
                !parse_string(parser, token)) return 0;
            skip_space(parser);
            if (parser->cursor >= parser->size ||
                parser->data[parser->cursor++] != ':') return 0;
            skip_space(parser);
        }
        if (!parse_value(parser, token)) return 0;
        skip_space(parser);
        if (parser->cursor >= parser->size) return 0;
        if (parser->data[parser->cursor] == close) {
            parser->tokens[token].end = ++parser->cursor;
            return 1;
        }
        if (parser->data[parser->cursor++] != ',') return 0;
        skip_space(parser);
    }
}

static inline int parse_primitive(json_parser *parser, size_t parent) {
    size_t start = parser->cursor;
    size_t check;
    size_t token;
    while (parser->cursor < parser->size &&
           strchr(" \t\r\n,]}", parser->data[parser->cursor]) == NULL)
        ++parser->cursor;
    if (parser->cursor == start) return 0;
    if ((parser->cursor - start == 4u &&
         (memcmp(parser->data + start, "true", 4u) == 0 ||
          memcmp(parser->data + start, "null", 4u) == 0)) ||
        (parser->cursor - start == 5u &&
         memcmp(parser->data + start, "false", 5u) == 0)) {
        /* valid literal */
    } else {
        check = start;
        if (parser->data[check] == '-') ++check;
        if (check >= parser->cursor) return 0;
        if (parser->data[check] == '0') {
            ++check;
        } else if (parser->data[check] >= '1' &&
                   parser->data[check] <= '9') {
            do { ++check; }
            while (check < parser->cursor && isdigit(parser->data[check]));
        } else {
            return 0;
        }
        if (check < parser->cursor && parser->data[check] == '.') {
            ++check;
            if (check >= parser->cursor || !isdigit(parser->data[check]))
                return 0;
            while (check < parser->cursor && isdigit(parser->data[check]))
                ++check;
        }
        if (check < parser->cursor &&
            (parser->data[check] == 'e' || parser->data[check] == 'E')) {
            ++check;
            if (check < parser->cursor &&
                (parser->data[check] == '+' || parser->data[check] == '-'))
                ++check;
            if (check >= parser->cursor || !isdigit(parser->data[check]))
                return 0;
            while (check < parser->cursor && isdigit(parser->data[check]))
                ++check;
        }
        if (check != parser->cursor) return 0;
    }
    token = new_token(parser, JSON_PRIMITIVE, start, parent);
    if (token == SIZE_MAX) return 0;
    parser->tokens[token].end = parser->cursor;
    return 1;
}

static inline int parse_value(json_parser *parser, size_t parent) {
    unsigned depth=0;
    for(size_t p=parent;p!=SIZE_MAX;p=parser->tokens[p].parent)if(++depth>64)return 0;
    skip_space(parser);
    if (parser->cursor >= parser->size) return 0;
    switch (parser->data[parser->cursor]) {
    case '{': return parse_compound(parser, parent, JSON_OBJECT, '{', '}');
    case '[': return parse_compound(parser, parent, JSON_ARRAY, '[', ']');
    case '"': return parse_string(parser, parent);
    default: return parse_primitive(parser, parent);
    }
}

static inline int token_equal(
    const unsigned char *data, const json_token *token, const char *text
) {
    size_t length = strlen(text);
    return token->end - token->start == length &&
           memcmp(data + token->start, text, length) == 0;
}

static inline size_t object_find(
    const json_parser *parser, size_t object, const char *key
) {
    int expecting_key = 1;
    if (object >= parser->count || parser->tokens[object].type != JSON_OBJECT)
        return SIZE_MAX;
    for (size_t index = object + 1u; index + 1u < parser->count &&
         parser->tokens[index].start < parser->tokens[object].end; ++index) {
        if (parser->tokens[index].parent != object) continue;
        if (expecting_key && parser->tokens[index].type == JSON_STRING &&
            token_equal(parser->data, &parser->tokens[index], key))
            return index + 1u;
        expecting_key = !expecting_key;
    }
    return SIZE_MAX;
}

static inline int object_fields(
    const json_parser *parser, size_t object,
    const char *const *fields, size_t field_count
) {
    size_t found = 0u;
    int expecting_key = 1;
    if (object >= parser->count || parser->tokens[object].type != JSON_OBJECT)
        return 0;
    for (size_t index = object + 1u; index + 1u < parser->count &&
         parser->tokens[index].start < parser->tokens[object].end; ++index) {
        int known = 0;
        if (parser->tokens[index].parent != object) continue;
        if (!expecting_key) {
            expecting_key = 1;
            continue;
        }
        expecting_key = 0;
        if (parser->tokens[index].type != JSON_STRING) return 0;
        for (size_t field = 0u; field < field_count; ++field) {
            if (token_equal(parser->data, &parser->tokens[index], fields[field])) {
                known = 1;
                break;
            }
        }
        if (!known) return 0;
        ++found;
    }
    if (found != field_count) return 0;
    for (size_t field = 0u; field < field_count; ++field)
        if (object_find(parser, object, fields[field]) == SIZE_MAX) return 0;
    return 1;
}

static inline size_t array_count(const json_parser *parser, size_t array) {
    size_t count = 0u;
    if (array >= parser->count || parser->tokens[array].type != JSON_ARRAY)
        return SIZE_MAX;
    for (size_t index = array + 1u; index < parser->count &&
         parser->tokens[index].start < parser->tokens[array].end; ++index)
        if (parser->tokens[index].parent == array) ++count;
    return count;
}

static inline size_t array_item(
    const json_parser *parser, size_t array, size_t wanted
) {
    size_t count = 0u;
    for (size_t index = array + 1u; index < parser->count &&
         parser->tokens[index].start < parser->tokens[array].end; ++index) {
        if (parser->tokens[index].parent != array) continue;
        if (count++ == wanted) return index;
    }
    return SIZE_MAX;
}

static inline int valid_identifier(const json_parser *parser, size_t token) {
    const json_token *value;
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    value = &parser->tokens[token];
    if (value->end == value->start ||
        !isalnum(parser->data[value->start])) return 0;
    for (size_t index = value->start; index < value->end; ++index) {
        unsigned char c = parser->data[index];
        if (!isalnum(c) && strchr("._:/-", c) == NULL) return 0;
    }
    return 1;
}

static inline int valid_version(const json_parser *parser, size_t token) {
    const json_token *value;
    size_t cursor;
    unsigned component = 0u;
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    value = &parser->tokens[token];
    cursor = value->start;
    while (component < 3u) {
        size_t begin = cursor;
        if (cursor >= value->end || !isdigit(parser->data[cursor])) return 0;
        while (cursor < value->end && isdigit(parser->data[cursor])) ++cursor;
        if (cursor - begin > 1u && parser->data[begin] == '0') return 0;
        ++component;
        if (component < 3u) {
            if (cursor >= value->end || parser->data[cursor++] != '.') return 0;
        }
    }
    if (cursor < value->end && parser->data[cursor] == '-') {
        size_t begin = ++cursor;
        while (cursor < value->end && parser->data[cursor] != '+') {
            unsigned char c = parser->data[cursor++];
            if (!isalnum(c) && c != '.' && c != '-') return 0;
        }
        if (cursor == begin) return 0;
    }
    if (cursor < value->end && parser->data[cursor] == '+') {
        size_t begin = ++cursor;
        while (cursor < value->end) {
            unsigned char c = parser->data[cursor++];
            if (!isalnum(c) && c != '.' && c != '-') return 0;
        }
        if (cursor == begin) return 0;
    }
    return cursor == value->end;
}

static inline int valid_sha256(const json_parser *parser, size_t token) {
    const json_token *value;
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    value = &parser->tokens[token];
    if (value->end - value->start != LT48_SHA256_SIZE * 2u) return 0;
    for (size_t index = value->start; index < value->end; ++index)
        if (!isxdigit(parser->data[index])) return 0;
    return 1;
}

static inline lt48_status read_file(
    const char *path, unsigned char **out_data, size_t *out_size
) {
    FILE *file;
    long length;
    unsigned char *data;
    if (path == NULL || path[0] == '\0' || out_data == NULL || out_size == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_data = NULL;
    *out_size = 0u;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    if (fseek(file, 0, SEEK_END) != 0 || (length = ftell(file)) <= 0 ||
        length > 16 * 1024 * 1024 || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return LT48_FILE_ERROR;
    }
    data = malloc((size_t)length);
    if (data == NULL) {
        fclose(file);
        return LT48_NO_MEMORY;
    }
    if (fread(data, 1u, (size_t)length, file) != (size_t)length ||
        fclose(file) != 0) {
        free(data);
        return LT48_FILE_ERROR;
    }
    *out_data = data;
    *out_size = (size_t)length;
    return LT48_OK;
}

static inline int token_uint64(
    const json_parser *parser, size_t token, uint64_t *out_value
) {
    const json_token *value;
    uint64_t parsed = 0u;
    if (token >= parser->count || out_value == NULL ||
        parser->tokens[token].type != JSON_PRIMITIVE) return 0;
    value = &parser->tokens[token];
    if (value->start == value->end) return 0;
    for (size_t index = value->start; index < value->end; ++index) {
        unsigned char digit = parser->data[index];
        if (!isdigit(digit) || parsed > (UINT64_MAX - (digit - '0')) / 10u)
            return 0;
        parsed = parsed * 10u + (digit - '0');
    }
    *out_value = parsed;
    return 1;
}

static inline int token_bool(
    const json_parser *parser, size_t token, int *out_value
) {
    if (token >= parser->count || out_value == NULL ||
        parser->tokens[token].type != JSON_PRIMITIVE) return 0;
    if (token_equal(parser->data, &parser->tokens[token], "true")) {
        *out_value = 1;
        return 1;
    }
    if (token_equal(parser->data, &parser->tokens[token], "false")) {
        *out_value = 0;
        return 1;
    }
    return 0;
}

static inline int token_text(
    const json_parser *parser, size_t token, char *output, size_t capacity
) {
    const json_token *value;
    size_t length;
    if (token >= parser->count || output == NULL || capacity == 0u ||
        parser->tokens[token].type != JSON_STRING) return 0;
    value = &parser->tokens[token];
    length = value->end - value->start;
    if (length == 0u || length >= capacity ||
        memchr(parser->data + value->start, '\\', length) != NULL)
        return 0;
    memcpy(output, parser->data + value->start, length);
    output[length] = '\0';
    return 1;
}


#endif
