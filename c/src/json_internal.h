#ifndef LT48_JSON_INTERNAL_H
#define LT48_JSON_INTERNAL_H

#include <labtool48/labtool48.h>

typedef struct lt48_json_document lt48_json_document;
typedef struct json_value lt48_json_value;

typedef enum lt48_json_type {
    LT48_JSON_NULL,
    LT48_JSON_BOOL,
    LT48_JSON_NUMBER,
    LT48_JSON_STRING,
    LT48_JSON_ARRAY,
    LT48_JSON_OBJECT
} lt48_json_type;

lt48_status lt48_json_document_open(
    const char *path, lt48_json_document **out_document
);
void lt48_json_document_close(lt48_json_document *document);
const lt48_json_value *lt48_json_document_root(
    const lt48_json_document *document
);
lt48_json_type lt48_json_value_type(const lt48_json_value *value);
const lt48_json_value *lt48_json_object_member(
    const lt48_json_value *object, const char *key
);
int lt48_json_object_exact(
    const lt48_json_value *object,
    const char *const *fields, size_t field_count
);
size_t lt48_json_array_count(const lt48_json_value *array);
const lt48_json_value *lt48_json_array_item(
    const lt48_json_value *array, size_t index
);
const char *lt48_json_string(const lt48_json_value *value);
int lt48_json_u64(const lt48_json_value *value, uint64_t *out_value);
int lt48_json_bool(const lt48_json_value *value, int *out_value);
lt48_status lt48_json_canonical_sha256(
    const lt48_json_value *value, const char *omit_root_member,
    unsigned char digest[LT48_SHA256_SIZE]
);
lt48_status lt48_json_canonical_copy(
    const lt48_json_value *value,
    unsigned char **out_data, size_t *out_size
);

#endif
