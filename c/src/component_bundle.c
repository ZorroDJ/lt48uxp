#define _XOPEN_SOURCE 700

#include <labtool48/labtool48.h>

#include "internal.h"
#include "json_reader_internal.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct component_object {
    size_t id;
    size_t kind;
    size_t extends;
    size_t references;
} component_object;

static void report_issue(
    lt48_component_validation_report *report,
    lt48_component_validation_issue issue, size_t object_index,
    const char *message
) {
    report->issue = issue;
    report->issue_object_index = object_index;
    snprintf(report->diagnostic, sizeof(report->diagnostic), "%s", message);
}

const char *lt48_component_validation_issue_name(
    lt48_component_validation_issue issue
) {
    switch (issue) {
    case LT48_COMPONENT_VALID: return "valid";
    case LT48_COMPONENT_FILE_ERROR: return "file-error";
    case LT48_COMPONENT_JSON_ERROR: return "json-error";
    case LT48_COMPONENT_SCHEMA_ERROR: return "schema-error";
    case LT48_COMPONENT_DUPLICATE_ID: return "duplicate-id";
    case LT48_COMPONENT_MISSING_REFERENCE: return "missing-reference";
    case LT48_COMPONENT_KIND_MISMATCH: return "kind-mismatch";
    case LT48_COMPONENT_INHERITANCE_CYCLE: return "inheritance-cycle";
    default: return NULL;
    }
}

static int valid_kind(const json_parser *parser, size_t token) {
    static const char *const kinds[] = {
        "algorithm", "family", "variant", "package", "adapter",
        "payload", "evidence", "variable", "pin-map", "operation"
    };
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    for (size_t index = 0u; index < sizeof(kinds) / sizeof(kinds[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token], kinds[index]))
            return 1;
    return 0;
}

static int valid_kind_name(const char *kind) {
    static const char *const kinds[] = {
        "algorithm", "family", "variant", "package", "adapter",
        "payload", "evidence", "variable", "pin-map", "operation"
    };
    if (kind == NULL || kind[0] == '\0') return 0;
    for (size_t index = 0u; index < sizeof(kinds) / sizeof(kinds[0]); ++index)
        if (strcmp(kind, kinds[index]) == 0) return 1;
    return 0;
}

static int token_same(
    const json_parser *parser, size_t left, size_t right
) {
    const json_token *a = &parser->tokens[left];
    const json_token *b = &parser->tokens[right];
    size_t a_size = a->end - a->start;
    return a_size == b->end - b->start &&
           memcmp(parser->data + a->start, parser->data + b->start, a_size) == 0;
}

static size_t find_object(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t id
) {
    for (size_t index = 0u; index < object_count; ++index)
        if (token_same(parser, objects[index].id, id)) return index;
    return SIZE_MAX;
}

static int visit_inheritance(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t index, unsigned char *state
) {
    size_t parent;
    if (state[index] == 2u) return 1;
    if (state[index] == 1u) return 0;
    state[index] = 1u;
    if (objects[index].extends != SIZE_MAX) {
        parent = find_object(parser, objects, object_count,
                             objects[index].extends);
        if (parent == SIZE_MAX ||
            !visit_inheritance(parser, objects, object_count, parent, state))
            return 0;
    }
    state[index] = 2u;
    return 1;
}

lt48_status lt48_component_bundle_validate(
    const char *path, lt48_component_validation_report *out_report
) {
    static const char *const root_fields[] = {
        "schema_version", "bundle", "documents", "objects", "combinations"
    };
    static const char *const bundle_fields[] = {
        "id", "version", "author", "license", "created_date"
    };
    static const char *const document_fields[] = {"id", "revision", "sha256"};
    static const char *const object_fields_list[] = {
        "kind", "id", "version", "sha256", "extends", "references",
        "properties"
    };
    static const char *const combination_fields[] = {
        "variant", "package", "adapter"
    };
    unsigned char *data = NULL;
    size_t size = 0u;
    json_parser parser = {0};
    component_object *objects = NULL;
    unsigned char *state = NULL;
    size_t bundle;
    size_t documents;
    size_t object_array;
    size_t combinations;
    lt48_status status;
    if (out_report == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->issue_object_index = SIZE_MAX;
    status = read_file(path, &data, &size);
    if (status != LT48_OK) {
        report_issue(out_report, LT48_COMPONENT_FILE_ERROR, SIZE_MAX,
                     "cannot read component bundle");
        return status;
    }
    lt48_sha256(data, size, out_report->source_sha256);
    parser.data = data;
    parser.size = size;
    if (!parse_value(&parser, SIZE_MAX)) parser.failed = 1;
    skip_space(&parser);
    if (parser.failed || parser.cursor != size || parser.count == 0u ||
        parser.tokens[0].type != JSON_OBJECT) {
        report_issue(out_report, LT48_COMPONENT_JSON_ERROR, SIZE_MAX,
                     "bundle is not one strict JSON object");
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    if (!object_fields(&parser, 0u, root_fields,
                       sizeof(root_fields) / sizeof(root_fields[0])) ||
        (bundle = object_find(&parser, 0u, "bundle")) == SIZE_MAX ||
        (documents = object_find(&parser, 0u, "documents")) == SIZE_MAX ||
        (object_array = object_find(&parser, 0u, "objects")) == SIZE_MAX ||
        (combinations = object_find(&parser, 0u, "combinations")) == SIZE_MAX ||
        !token_equal(parser.data,
                     &parser.tokens[object_find(&parser, 0u, "schema_version")],
                     "1") ||
        !object_fields(&parser, bundle, bundle_fields,
                       sizeof(bundle_fields) / sizeof(bundle_fields[0])) ||
        !valid_identifier(&parser, object_find(&parser, bundle, "id")) ||
        !valid_version(&parser, object_find(&parser, bundle, "version")) ||
        parser.tokens[object_find(&parser, bundle, "author")].type != JSON_STRING ||
        parser.tokens[object_find(&parser, bundle, "license")].type != JSON_STRING) {
        report_issue(out_report, LT48_COMPONENT_SCHEMA_ERROR, SIZE_MAX,
                     "top-level or bundle manifest schema is invalid");
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    out_report->schema_version = 1u;
    out_report->document_count = array_count(&parser, documents);
    out_report->object_count = array_count(&parser, object_array);
    out_report->combination_count = array_count(&parser, combinations);
    if (out_report->document_count == SIZE_MAX ||
        out_report->object_count == SIZE_MAX ||
        out_report->combination_count == SIZE_MAX) {
        report_issue(out_report, LT48_COMPONENT_SCHEMA_ERROR, SIZE_MAX,
                     "documents, objects, and combinations must be arrays");
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    for (size_t index = 0u; index < out_report->document_count; ++index) {
        size_t item = array_item(&parser, documents, index);
        if (!object_fields(&parser, item, document_fields, 3u) ||
            !valid_identifier(&parser, object_find(&parser, item, "id")) ||
            parser.tokens[object_find(&parser, item, "revision")].type != JSON_STRING ||
            !valid_sha256(&parser, object_find(&parser, item, "sha256"))) {
            report_issue(out_report, LT48_COMPONENT_SCHEMA_ERROR, index,
                         "document record is invalid");
            status = LT48_FORMAT_ERROR;
            goto done;
        }
    }
    objects = calloc(out_report->object_count ? out_report->object_count : 1u,
                     sizeof(*objects));
    state = calloc(out_report->object_count ? out_report->object_count : 1u, 1u);
    if (objects == NULL || state == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < out_report->object_count; ++index) {
        size_t item = array_item(&parser, object_array, index);
        size_t sha;
        size_t version;
        size_t properties;
        if (!object_fields(&parser, item, object_fields_list, 7u)) {
            report_issue(out_report, LT48_COMPONENT_SCHEMA_ERROR, index,
                         "component object has missing or unknown fields");
            status = LT48_FORMAT_ERROR;
            goto done;
        }
        objects[index].kind = object_find(&parser, item, "kind");
        objects[index].id = object_find(&parser, item, "id");
        version = object_find(&parser, item, "version");
        sha = object_find(&parser, item, "sha256");
        objects[index].extends = object_find(&parser, item, "extends");
        objects[index].references = object_find(&parser, item, "references");
        properties = object_find(&parser, item, "properties");
        if (!valid_kind(&parser, objects[index].kind) ||
            !valid_identifier(&parser, objects[index].id) ||
            !valid_version(&parser, version) || !valid_sha256(&parser, sha) ||
            (parser.tokens[objects[index].extends].type != JSON_STRING &&
             !token_equal(parser.data, &parser.tokens[objects[index].extends],
                          "null")) ||
            array_count(&parser, objects[index].references) == SIZE_MAX ||
            parser.tokens[properties].type != JSON_OBJECT) {
            report_issue(out_report, LT48_COMPONENT_SCHEMA_ERROR, index,
                         "component object identity or structure is invalid");
            status = LT48_FORMAT_ERROR;
            goto done;
        }
        if (parser.tokens[objects[index].extends].type != JSON_STRING)
            objects[index].extends = SIZE_MAX;
        else
            ++out_report->inherited_object_count;
        for (size_t prior = 0u; prior < index; ++prior) {
            if (token_same(&parser, objects[prior].id, objects[index].id)) {
                report_issue(out_report, LT48_COMPONENT_DUPLICATE_ID, index,
                             "component object ID is duplicated");
                status = LT48_FORMAT_ERROR;
                goto done;
            }
        }
    }
    for (size_t index = 0u; index < out_report->object_count; ++index) {
        if (objects[index].extends != SIZE_MAX) {
            size_t parent = find_object(&parser, objects,
                                        out_report->object_count,
                                        objects[index].extends);
            if (parent == SIZE_MAX) {
                report_issue(out_report, LT48_COMPONENT_MISSING_REFERENCE,
                             index, "base object does not exist");
                status = LT48_FORMAT_ERROR;
                goto done;
            }
            if (!token_same(&parser, objects[index].kind, objects[parent].kind)) {
                report_issue(out_report, LT48_COMPONENT_KIND_MISMATCH, index,
                             "base object has a different kind");
                status = LT48_FORMAT_ERROR;
                goto done;
            }
        }
        {
            size_t count = array_count(&parser, objects[index].references);
            out_report->reference_count += count;
            for (size_t ref = 0u; ref < count; ++ref) {
                size_t id = array_item(&parser, objects[index].references, ref);
                if (!valid_identifier(&parser, id) ||
                    find_object(&parser, objects, out_report->object_count, id) ==
                        SIZE_MAX) {
                    report_issue(out_report,
                                 LT48_COMPONENT_MISSING_REFERENCE, index,
                                 "object reference does not resolve");
                    status = LT48_FORMAT_ERROR;
                    goto done;
                }
            }
        }
    }
    for (size_t index = 0u; index < out_report->object_count; ++index) {
        if (!visit_inheritance(&parser, objects, out_report->object_count,
                               index, state)) {
            report_issue(out_report, LT48_COMPONENT_INHERITANCE_CYCLE, index,
                         "inheritance graph contains a cycle");
            status = LT48_FORMAT_ERROR;
            goto done;
        }
    }
    for (size_t index = 0u; index < out_report->combination_count; ++index) {
        size_t item = array_item(&parser, combinations, index);
        size_t ids[3];
        const char *kinds[3] = {"variant", "package", "adapter"};
        if (!object_fields(&parser, item, combination_fields, 3u)) {
            report_issue(out_report, LT48_COMPONENT_SCHEMA_ERROR, index,
                         "combination record is invalid");
            status = LT48_FORMAT_ERROR;
            goto done;
        }
        ids[0] = object_find(&parser, item, "variant");
        ids[1] = object_find(&parser, item, "package");
        ids[2] = object_find(&parser, item, "adapter");
        for (size_t field = 0u; field < 3u; ++field) {
            size_t object;
            if (field == 2u && token_equal(
                    parser.data, &parser.tokens[ids[field]], "null")) continue;
            if (!valid_identifier(&parser, ids[field]) ||
                (object = find_object(&parser, objects,
                                      out_report->object_count,
                                      ids[field])) == SIZE_MAX) {
                report_issue(out_report, LT48_COMPONENT_MISSING_REFERENCE,
                             index, "combination reference does not resolve");
                status = LT48_FORMAT_ERROR;
                goto done;
            }
            if (!token_equal(parser.data, &parser.tokens[objects[object].kind],
                             kinds[field])) {
                report_issue(out_report, LT48_COMPONENT_KIND_MISMATCH, index,
                             "combination reference has the wrong object kind");
                status = LT48_FORMAT_ERROR;
                goto done;
            }
        }
    }
    out_report->issue = LT48_COMPONENT_VALID;
    out_report->issue_object_index = SIZE_MAX;
    snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
             "structural schema, identities, references, and inheritance are valid");
    status = LT48_OK;
done:
    free(state);
    free(objects);
    free(parser.tokens);
    free(data);
    return status;
}

typedef struct output_buffer {
    unsigned char *data;
    size_t size;
    size_t capacity;
} output_buffer;

typedef struct property_member {
    size_t key;
    size_t value;
} property_member;

static int output_append(
    output_buffer *output, const void *data, size_t size
) {
    unsigned char *grown;
    size_t required;
    size_t capacity;
    if (size > SIZE_MAX - output->size) return 0;
    required = output->size + size;
    if (required > output->capacity) {
        capacity = output->capacity == 0u ? 256u : output->capacity;
        while (capacity < required) {
            if (capacity > SIZE_MAX / 2u) return 0;
            capacity *= 2u;
        }
        grown = realloc(output->data, capacity);
        if (grown == NULL) return 0;
        output->data = grown;
        output->capacity = capacity;
    }
    if (size != 0u) memcpy(output->data + output->size, data, size);
    output->size = required;
    return 1;
}

static int output_literal(output_buffer *output, const char *text) {
    return output_append(output, text, strlen(text));
}

static int member_key_compare(
    const json_parser *parser, const property_member *left,
    const property_member *right
) {
    const json_token *a = &parser->tokens[left->key];
    const json_token *b = &parser->tokens[right->key];
    size_t a_size = a->end - a->start;
    size_t b_size = b->end - b->start;
    size_t common = a_size < b_size ? a_size : b_size;
    int result = memcmp(parser->data + a->start,
                        parser->data + b->start, common);
    if (result != 0) return result;
    return a_size < b_size ? -1 : a_size > b_size;
}

static int canonical_token(
    const json_parser *parser, size_t token, output_buffer *output
) {
    const json_token *value;
    if (token >= parser->count) return 0;
    value = &parser->tokens[token];
    if (value->type == JSON_STRING)
        return value->start != 0u && value->end < parser->size &&
               output_append(output, parser->data + value->start - 1u,
                             value->end - value->start + 2u);
    if (value->type == JSON_PRIMITIVE)
        return output_append(output, parser->data + value->start,
                             value->end - value->start);
    if (value->type == JSON_ARRAY) {
        size_t count = array_count(parser, token);
        if (!output_literal(output, "[")) return 0;
        for (size_t index = 0u; index < count; ++index) {
            if ((index != 0u && !output_literal(output, ",")) ||
                !canonical_token(parser, array_item(parser, token, index),
                                 output)) return 0;
        }
        return output_literal(output, "]");
    }
    if (value->type == JSON_OBJECT) {
        property_member *members = NULL;
        size_t count = 0u;
        int expecting_key = 1;
        size_t pending_key = SIZE_MAX;
        for (size_t index = token + 1u; index < parser->count &&
             parser->tokens[index].start < value->end; ++index) {
            property_member *grown;
            if (parser->tokens[index].parent != token) continue;
            if (expecting_key) {
                pending_key = index;
                expecting_key = 0;
                continue;
            }
            grown = realloc(members, (count + 1u) * sizeof(*members));
            if (grown == NULL) {
                free(members);
                return 0;
            }
            members = grown;
            members[count++] = (property_member){pending_key, index};
            expecting_key = 1;
        }
        for (size_t index = 1u; index < count; ++index) {
            property_member current = members[index];
            size_t position = index;
            while (position != 0u &&
                   member_key_compare(parser, &current,
                                      &members[position - 1u]) < 0) {
                members[position] = members[position - 1u];
                --position;
            }
            members[position] = current;
        }
        if (!output_literal(output, "{")) {
            free(members);
            return 0;
        }
        for (size_t index = 0u; index < count; ++index) {
            if ((index != 0u && !output_literal(output, ",")) ||
                !canonical_token(parser, members[index].key, output) ||
                !output_literal(output, ":") ||
                !canonical_token(parser, members[index].value, output)) {
                free(members);
                return 0;
            }
        }
        free(members);
        return output_literal(output, "}");
    }
    return 0;
}

static int property_set(
    const json_parser *parser, property_member **members,
    size_t *count, size_t key, size_t value
) {
    property_member *grown;
    for (size_t index = 0u; index < *count; ++index) {
        if (token_same(parser, (*members)[index].key, key)) {
            (*members)[index] = (property_member){key, value};
            return 1;
        }
    }
    grown = realloc(*members, (*count + 1u) * sizeof(**members));
    if (grown == NULL) return 0;
    *members = grown;
    (*members)[(*count)++] = (property_member){key, value};
    return 1;
}

static int collect_object_properties(
    const json_parser *parser, size_t properties,
    property_member **members, size_t *count
) {
    int expecting_key = 1;
    size_t key = SIZE_MAX;
    for (size_t index = properties + 1u; index < parser->count &&
         parser->tokens[index].start < parser->tokens[properties].end; ++index) {
        if (parser->tokens[index].parent != properties) continue;
        if (expecting_key) {
            key = index;
            expecting_key = 0;
        } else {
            if (!property_set(parser, members, count, key, index)) return 0;
            expecting_key = 1;
        }
    }
    return expecting_key;
}

static int canonical_equal(
    const json_parser *parser, size_t left, size_t right
) {
    output_buffer a = {0};
    output_buffer b = {0};
    int equal = canonical_token(parser, left, &a) &&
                canonical_token(parser, right, &b) &&
                a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
    free(a.data);
    free(b.data);
    return equal;
}

static int output_members(
    const json_parser *parser, property_member *members,
    size_t count, output_buffer *output
) {
    for (size_t index = 1u; index < count; ++index) {
        property_member current = members[index];
        size_t position = index;
        while (position != 0u &&
               member_key_compare(parser, &current,
                                  &members[position - 1u]) < 0) {
            members[position] = members[position - 1u];
            --position;
        }
        members[position] = current;
    }
    if (!output_literal(output, "{")) return 0;
    for (size_t index = 0u; index < count; ++index) {
        if ((index != 0u && !output_literal(output, ",")) ||
            !canonical_token(parser, members[index].key, output) ||
            !output_literal(output, ":") ||
            !canonical_token(parser, members[index].value, output)) return 0;
    }
    return output_literal(output, "}");
}

void lt48_component_resolution_clear(lt48_component_resolution *resolution) {
    if (resolution == NULL) return;
    free(resolution->effective_json);
    free(resolution->delta_json);
    memset(resolution, 0, sizeof(*resolution));
}

lt48_status lt48_component_bundle_resolve(
    const char *path, const char *object_id,
    lt48_component_resolution *out_resolution
) {
    lt48_component_validation_report validation;
    unsigned char *data = NULL;
    size_t size = 0u;
    json_parser parser = {0};
    component_object *objects = NULL;
    size_t *items = NULL;
    size_t *chain = NULL;
    property_member *base_members = NULL;
    property_member *effective_members = NULL;
    property_member *changed_members = NULL;
    size_t base_count = 0u;
    size_t effective_count = 0u;
    size_t changed_count = 0u;
    size_t object_array;
    size_t object_count;
    size_t target = SIZE_MAX;
    size_t chain_count = 0u;
    output_buffer effective = {0};
    output_buffer delta = {0};
    lt48_status status;
    if (path == NULL || object_id == NULL || object_id[0] == '\0' ||
        out_resolution == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_resolution, 0, sizeof(*out_resolution));
    status = lt48_component_bundle_validate(path, &validation);
    if (status != LT48_OK) return status;
    status = read_file(path, &data, &size);
    if (status != LT48_OK) return status;
    parser.data = data;
    parser.size = size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    object_array = object_find(&parser, 0u, "objects");
    object_count = array_count(&parser, object_array);
    objects = calloc(object_count ? object_count : 1u, sizeof(*objects));
    items = calloc(object_count ? object_count : 1u, sizeof(*items));
    chain = calloc(object_count ? object_count : 1u, sizeof(*chain));
    if (objects == NULL || items == NULL || chain == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < object_count; ++index) {
        size_t item = array_item(&parser, object_array, index);
        items[index] = item;
        objects[index].id = object_find(&parser, item, "id");
        objects[index].kind = object_find(&parser, item, "kind");
        objects[index].extends = object_find(&parser, item, "extends");
        if (parser.tokens[objects[index].extends].type != JSON_STRING)
            objects[index].extends = SIZE_MAX;
        if (token_equal(parser.data, &parser.tokens[objects[index].id],
                        object_id)) target = index;
    }
    if (target == SIZE_MAX) {
        status = LT48_NOT_FOUND;
        goto done;
    }
    for (size_t current = target;;) {
        size_t parent;
        chain[chain_count++] = current;
        if (objects[current].extends == SIZE_MAX) break;
        parent = find_object(&parser, objects, object_count,
                             objects[current].extends);
        if (parent == SIZE_MAX || chain_count >= object_count) {
            status = LT48_FORMAT_ERROR;
            goto done;
        }
        current = parent;
    }
    for (size_t index = 0u; index < chain_count / 2u; ++index) {
        size_t temporary = chain[index];
        chain[index] = chain[chain_count - index - 1u];
        chain[chain_count - index - 1u] = temporary;
    }
    for (size_t level = 0u; level + 1u < chain_count; ++level) {
        size_t properties = object_find(&parser, items[chain[level]],
                                        "properties");
        if (!collect_object_properties(&parser, properties,
                                       &base_members, &base_count)) {
            status = LT48_NO_MEMORY;
            goto done;
        }
    }
    for (size_t index = 0u; index < base_count; ++index)
        if (!property_set(&parser, &effective_members, &effective_count,
                          base_members[index].key, base_members[index].value)) {
            status = LT48_NO_MEMORY;
            goto done;
        }
    {
        size_t properties = object_find(&parser, items[target], "properties");
        property_member *local = NULL;
        size_t local_count = 0u;
        if (!collect_object_properties(&parser, properties, &local, &local_count)) {
            free(local);
            status = LT48_NO_MEMORY;
            goto done;
        }
        for (size_t index = 0u; index < local_count; ++index) {
            size_t base_value = SIZE_MAX;
            for (size_t prior = 0u; prior < base_count; ++prior)
                if (token_same(&parser, base_members[prior].key,
                               local[index].key)) {
                    base_value = base_members[prior].value;
                    break;
                }
            if (base_value == SIZE_MAX ||
                !canonical_equal(&parser, base_value, local[index].value)) {
                if (!property_set(&parser, &changed_members, &changed_count,
                                  local[index].key, local[index].value)) {
                    free(local);
                    status = LT48_NO_MEMORY;
                    goto done;
                }
            }
            if (!property_set(&parser, &effective_members, &effective_count,
                              local[index].key, local[index].value)) {
                free(local);
                status = LT48_NO_MEMORY;
                goto done;
            }
        }
        free(local);
    }
    if (!output_literal(&effective,
            "{\"schema_version\":1,\"object_id\":") ||
        !canonical_token(&parser, objects[target].id, &effective) ||
        !output_literal(&effective, ",\"kind\":") ||
        !canonical_token(&parser, objects[target].kind, &effective) ||
        !output_literal(&effective, ",\"version\":") ||
        !canonical_token(&parser,
            object_find(&parser, items[target], "version"), &effective) ||
        !output_literal(&effective, ",\"declared_sha256\":") ||
        !canonical_token(&parser,
            object_find(&parser, items[target], "sha256"), &effective) ||
        !output_literal(&effective, ",\"inheritance\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < chain_count; ++index) {
        if ((index != 0u && !output_literal(&effective, ",")) ||
            !canonical_token(&parser, objects[chain[index]].id, &effective)) {
            status = LT48_NO_MEMORY;
            goto done;
        }
    }
    if (!output_literal(&effective, "],\"properties\":") ||
        !output_members(&parser, effective_members, effective_count, &effective) ||
        !output_literal(&effective, "}")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    if (!output_literal(&delta, "{\"schema_version\":1,\"base\":") ||
        !(chain_count > 1u ? canonical_token(
              &parser, objects[chain[chain_count - 2u]].id, &delta) :
              output_literal(&delta, "null")) ||
        !output_literal(&delta, ",\"object_id\":") ||
        !canonical_token(&parser, objects[target].id, &delta) ||
        !output_literal(&delta, ",\"changed_properties\":") ||
        !output_members(&parser, changed_members, changed_count, &delta) ||
        !output_literal(&delta, "}")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    if (!output_append(&effective, "\0", 1u) ||
        !output_append(&delta, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_resolution->effective_json = effective.data;
    out_resolution->effective_json_size = effective.size - 1u;
    lt48_sha256(effective.data, effective.size - 1u,
                out_resolution->effective_sha256);
    out_resolution->delta_json = delta.data;
    out_resolution->delta_json_size = delta.size - 1u;
    lt48_sha256(delta.data, delta.size - 1u,
                out_resolution->delta_sha256);
    out_resolution->inheritance_depth = chain_count - 1u;
    out_resolution->effective_property_count = effective_count;
    out_resolution->changed_property_count = changed_count;
    effective.data = NULL;
    delta.data = NULL;
    status = LT48_OK;
done:
    free(delta.data);
    free(effective.data);
    free(changed_members);
    free(effective_members);
    free(base_members);
    free(chain);
    free(items);
    free(objects);
    free(parser.tokens);
    free(data);
    return status;
}

lt48_status lt48_component_bundle_diff(
    const char *path, const char *base_object_id, const char *object_id,
    lt48_component_resolution *out_resolution
) {
    lt48_component_validation_report validation;
    unsigned char *data = NULL;
    size_t size = 0u;
    json_parser parser = {0};
    size_t objects_token;
    size_t object_count;
    size_t base = SIZE_MAX;
    size_t target = SIZE_MAX;
    lt48_status status;
    if (path == NULL || base_object_id == NULL || base_object_id[0] == '\0' ||
        object_id == NULL || object_id[0] == '\0' ||
        out_resolution == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_resolution, 0, sizeof(*out_resolution));
    if (strcmp(base_object_id, object_id) == 0) return LT48_INVALID_ARGUMENT;
    status = lt48_component_bundle_validate(path, &validation);
    if (status != LT48_OK) return status;
    status = read_file(path, &data, &size);
    if (status != LT48_OK) return status;
    parser.data = data;
    parser.size = size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    objects_token = object_find(&parser, 0u, "objects");
    object_count = array_count(&parser, objects_token);
    for (size_t index = 0u; index < object_count; ++index) {
        size_t item = array_item(&parser, objects_token, index);
        size_t id = object_find(&parser, item, "id");
        if (token_equal(parser.data, &parser.tokens[id], base_object_id))
            base = item;
        if (token_equal(parser.data, &parser.tokens[id], object_id))
            target = item;
    }
    if (base == SIZE_MAX || target == SIZE_MAX) {
        status = LT48_NOT_FOUND;
        goto done;
    }
    if (!token_equal(parser.data,
                     &parser.tokens[object_find(&parser, base, "kind")],
                     "variant") ||
        !token_equal(parser.data,
                     &parser.tokens[object_find(&parser, target, "kind")],
                     "variant") ||
        !token_equal(parser.data,
                     &parser.tokens[object_find(&parser, target, "extends")],
                     base_object_id)) {
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    status = LT48_OK;
done:
    free(parser.tokens);
    free(data);
    if (status != LT48_OK) return status;
    return lt48_component_bundle_resolve(path, object_id, out_resolution);
}

static int c_identifier(const char *text) {
    const unsigned char *cursor = (const unsigned char *)text;
    if (cursor == NULL || !isalnum(*cursor)) return 0;
    for (++cursor; *cursor != 0u; ++cursor)
        if (!isalnum(*cursor) && strchr("._:/-", *cursor) == NULL) return 0;
    return 1;
}

static int c_version(const char *text) {
    const unsigned char *cursor = (const unsigned char *)text;
    for (unsigned component = 0u; component < 3u; ++component) {
        const unsigned char *begin = cursor;
        if (!isdigit(*cursor)) return 0;
        while (isdigit(*cursor)) ++cursor;
        if (cursor - begin > 1 && *begin == '0') return 0;
        if (component < 2u && *cursor++ != '.') return 0;
    }
    if (*cursor == '-') {
        const unsigned char *begin = ++cursor;
        while (*cursor != 0u && *cursor != '+') {
            if (!isalnum(*cursor) && *cursor != '.' && *cursor != '-') return 0;
            ++cursor;
        }
        if (cursor == begin) return 0;
    }
    if (*cursor == '+') {
        const unsigned char *begin = ++cursor;
        while (*cursor != 0u) {
            if (!isalnum(*cursor) && *cursor != '.' && *cursor != '-') return 0;
            ++cursor;
        }
        if (cursor == begin) return 0;
    }
    return *cursor == 0u;
}

static int c_date(const char *text) {
    static const unsigned days[] = {0u,31u,28u,31u,30u,31u,30u,
                                    31u,31u,30u,31u,30u,31u};
    unsigned year;
    unsigned month;
    unsigned day;
    unsigned limit;
    char trailing;
    if (text == NULL || strlen(text) != 10u ||
        sscanf(text, "%4u-%2u-%2u%c", &year, &month, &day, &trailing) != 3 ||
        text[4] != '-' || text[7] != '-' || year == 0u ||
        month == 0u || month > 12u) return 0;
    limit = days[month];
    if (month == 2u && ((year % 4u == 0u && year % 100u != 0u) ||
                       year % 400u == 0u)) ++limit;
    return day != 0u && day <= limit;
}

static int valid_utf8_text(const char *text) {
    const unsigned char *cursor = (const unsigned char *)text;
    if (cursor == NULL) return 0;
    while (*cursor != 0u) {
        uint32_t code;
        unsigned continuation;
        if (*cursor < 0x80u) {
            ++cursor;
            continue;
        }
        if ((*cursor & 0xe0u) == 0xc0u) {
            code = *cursor++ & 0x1fu;
            continuation = 1u;
            if (code < 2u) return 0;
        } else if ((*cursor & 0xf0u) == 0xe0u) {
            code = *cursor++ & 0x0fu;
            continuation = 2u;
        } else if ((*cursor & 0xf8u) == 0xf0u) {
            code = *cursor++ & 0x07u;
            continuation = 3u;
        } else {
            return 0;
        }
        for (unsigned index = 0u; index < continuation; ++index) {
            if ((*cursor & 0xc0u) != 0x80u) return 0;
            code = (code << 6u) | (*cursor++ & 0x3fu);
        }
        if ((continuation == 2u && code < 0x800u) ||
            (continuation == 3u && code < 0x10000u) ||
            (code >= 0xd800u && code <= 0xdfffu) || code > 0x10ffffu)
            return 0;
    }
    return 1;
}

static int output_json_string(output_buffer *output, const char *text) {
    static const char hex[] = "0123456789abcdef";
    const unsigned char *cursor = (const unsigned char *)text;
    if (!valid_utf8_text(text) || !output_literal(output, "\"")) return 0;
    while (*cursor != 0u) {
        unsigned char value = *cursor++;
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
            if (!output_append(output, escape, 2u)) return 0;
        } else if (value < 0x20u) {
            unsigned char encoded[] = {
                '\\','u','0','0',hex[value >> 4u],hex[value & 0x0fu]
            };
            if (!output_append(output, encoded, sizeof(encoded))) return 0;
        } else if (!output_append(output, &value, 1u)) {
            return 0;
        }
    }
    return output_literal(output, "\"");
}

lt48_status lt48_component_bundle_init(
    const char *path, const char *bundle_id, const char *version,
    const char *author, const char *license, const char *created_date,
    unsigned char out_source_sha256[LT48_SHA256_SIZE]
) {
    output_buffer output = {0};
    lt48_component_validation_report validation;
    FILE *file = NULL;
    lt48_status status = LT48_OK;
    if (path == NULL || path[0] == '\0' || !c_identifier(bundle_id) ||
        !c_version(version) || !valid_utf8_text(author) || author[0] == '\0' ||
        !valid_utf8_text(license) || license[0] == '\0' ||
        !c_date(created_date) || out_source_sha256 == NULL)
        return LT48_INVALID_ARGUMENT;
    if (!output_literal(&output, "{\"schema_version\":1,\"bundle\":{"
                                      "\"id\":") ||
        !output_json_string(&output, bundle_id) ||
        !output_literal(&output, ",\"version\":") ||
        !output_json_string(&output, version) ||
        !output_literal(&output, ",\"author\":") ||
        !output_json_string(&output, author) ||
        !output_literal(&output, ",\"license\":") ||
        !output_json_string(&output, license) ||
        !output_literal(&output, ",\"created_date\":") ||
        !output_json_string(&output, created_date) ||
        !output_literal(&output,
            "},\"documents\":[],\"objects\":[],\"combinations\":[]}\n")) {
        free(output.data);
        return LT48_NO_MEMORY;
    }
    file = fopen(path, "wx");
    if (file == NULL) {
        free(output.data);
        return LT48_FILE_ERROR;
    }
    if (fwrite(output.data, 1u, output.size, file) != output.size ||
        fflush(file) != 0 || fclose(file) != 0) {
        file = NULL;
        (void)remove(path);
        free(output.data);
        return LT48_FILE_ERROR;
    }
    file = NULL;
    status = lt48_component_bundle_validate(path, &validation);
    if (status != LT48_OK) {
        (void)remove(path);
    } else {
        memcpy(out_source_sha256, validation.source_sha256, LT48_SHA256_SIZE);
    }
    free(output.data);
    return status;
}

lt48_status lt48_component_bundle_add_object_of_kind(
    const char *input_path, const char *output_path,
    const char *new_bundle_version, const char *new_created_date,
    const char *object_record_path, const char *expected_kind,
    lt48_component_revision_report *out_report
) {
    static const char *const record_fields[] = {
        "kind", "id", "version", "sha256", "extends", "references",
        "properties"
    };
    lt48_component_validation_report input_validation;
    lt48_component_validation_report output_validation;
    unsigned char *input_data = NULL;
    unsigned char *record_data = NULL;
    size_t input_size = 0u;
    size_t record_size = 0u;
    json_parser input = {0};
    json_parser record = {0};
    component_object *objects = NULL;
    size_t object_array;
    size_t object_count;
    size_t record_kind;
    size_t record_id;
    size_t record_version;
    size_t record_sha;
    size_t record_extends;
    size_t record_references;
    size_t record_properties;
    output_buffer output = {0};
    FILE *file = NULL;
    int output_created = 0;
    lt48_status status;
    if (input_path == NULL || output_path == NULL ||
        object_record_path == NULL || out_report == NULL ||
        (expected_kind != NULL && !valid_kind_name(expected_kind)) ||
        !c_version(new_bundle_version) || !c_date(new_created_date))
        return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    if (strcmp(input_path, output_path) == 0) return LT48_INVALID_ARGUMENT;
    status = lt48_component_bundle_validate(input_path, &input_validation);
    if (status != LT48_OK) return status;
    status = read_file(input_path, &input_data, &input_size);
    if (status != LT48_OK) return status;
    status = read_file(object_record_path, &record_data, &record_size);
    if (status != LT48_OK) goto done;
    input.data = input_data;
    input.size = input_size;
    record.data = record_data;
    record.size = record_size;
    if (!parse_value(&input, SIZE_MAX) || !parse_value(&record, SIZE_MAX)) {
        status = input.failed || record.failed ? LT48_NO_MEMORY :
                                                LT48_FORMAT_ERROR;
        goto done;
    }
    skip_space(&input);
    skip_space(&record);
    if (input.cursor != input.size || record.cursor != record.size ||
        record.count == 0u || record.tokens[0].type != JSON_OBJECT ||
        !object_fields(&record, 0u, record_fields, 7u)) {
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    if (token_equal(input.data,
                    &input.tokens[object_find(
                        &input, object_find(&input, 0u, "bundle"), "version")],
                    new_bundle_version)) {
        status = LT48_INVALID_ARGUMENT;
        goto done;
    }
    record_kind = object_find(&record, 0u, "kind");
    record_id = object_find(&record, 0u, "id");
    record_version = object_find(&record, 0u, "version");
    record_sha = object_find(&record, 0u, "sha256");
    record_extends = object_find(&record, 0u, "extends");
    record_references = object_find(&record, 0u, "references");
    record_properties = object_find(&record, 0u, "properties");
    if (!valid_kind(&record, record_kind) ||
        (expected_kind != NULL &&
         !token_equal(record.data, &record.tokens[record_kind], expected_kind)) ||
        !valid_identifier(&record, record_id) ||
        !valid_version(&record, record_version) ||
        !valid_sha256(&record, record_sha) ||
        (record.tokens[record_extends].type != JSON_STRING &&
         !token_equal(record.data, &record.tokens[record_extends], "null")) ||
        array_count(&record, record_references) == SIZE_MAX ||
        record.tokens[record_properties].type != JSON_OBJECT) {
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    object_array = object_find(&input, 0u, "objects");
    object_count = array_count(&input, object_array);
    objects = calloc(object_count ? object_count : 1u, sizeof(*objects));
    if (objects == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < object_count; ++index) {
        size_t item = array_item(&input, object_array, index);
        objects[index].id = object_find(&input, item, "id");
        objects[index].kind = object_find(&input, item, "kind");
        if (input.tokens[objects[index].id].end -
                input.tokens[objects[index].id].start ==
                record.tokens[record_id].end - record.tokens[record_id].start &&
            memcmp(input.data + input.tokens[objects[index].id].start,
                   record.data + record.tokens[record_id].start,
                   record.tokens[record_id].end -
                       record.tokens[record_id].start) == 0) {
            status = LT48_FORMAT_ERROR;
            goto done;
        }
    }
    if (record.tokens[record_extends].type == JSON_STRING) {
        size_t parent = SIZE_MAX;
        for (size_t index = 0u; index < object_count; ++index) {
            const json_token *id = &input.tokens[objects[index].id];
            const json_token *base = &record.tokens[record_extends];
            if (id->end - id->start == base->end - base->start &&
                memcmp(input.data + id->start, record.data + base->start,
                       base->end - base->start) == 0) {
                parent = index;
                break;
            }
        }
        if (parent == SIZE_MAX) {
            status = LT48_NOT_FOUND;
            goto done;
        }
        {
            const json_token *parent_kind = &input.tokens[objects[parent].kind];
            const json_token *child_kind = &record.tokens[record_kind];
            if (parent_kind->end - parent_kind->start !=
                    child_kind->end - child_kind->start ||
                memcmp(input.data + parent_kind->start,
                       record.data + child_kind->start,
                       child_kind->end - child_kind->start) != 0) {
                status = LT48_FORMAT_ERROR;
                goto done;
            }
        }
    }
    {
        size_t reference_count = array_count(&record, record_references);
        for (size_t ref = 0u; ref < reference_count; ++ref) {
            size_t ref_token = array_item(&record, record_references, ref);
            int found = 0;
            if (!valid_identifier(&record, ref_token)) {
                status = LT48_FORMAT_ERROR;
                goto done;
            }
            for (size_t index = 0u; index < object_count; ++index) {
                const json_token *existing = &input.tokens[objects[index].id];
                const json_token *wanted = &record.tokens[ref_token];
                if (existing->end - existing->start ==
                        wanted->end - wanted->start &&
                    memcmp(input.data + existing->start,
                           record.data + wanted->start,
                           wanted->end - wanted->start) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                status = LT48_NOT_FOUND;
                goto done;
            }
        }
    }
    {
        size_t bundle = object_find(&input, 0u, "bundle");
        size_t documents = object_find(&input, 0u, "documents");
        size_t combinations = object_find(&input, 0u, "combinations");
        if (!output_literal(&output, "{\"schema_version\":1,\"bundle\":{"
                                     "\"id\":") ||
            !canonical_token(&input, object_find(&input, bundle, "id"),
                             &output) ||
            !output_literal(&output, ",\"version\":") ||
            !output_json_string(&output, new_bundle_version) ||
            !output_literal(&output, ",\"author\":") ||
            !canonical_token(&input, object_find(&input, bundle, "author"),
                             &output) ||
            !output_literal(&output, ",\"license\":") ||
            !canonical_token(&input, object_find(&input, bundle, "license"),
                             &output) ||
            !output_literal(&output, ",\"created_date\":") ||
            !output_json_string(&output, new_created_date) ||
            !output_literal(&output, "},\"documents\":") ||
            !canonical_token(&input, documents, &output) ||
            !output_literal(&output, ",\"objects\":[")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        for (size_t index = 0u; index < object_count; ++index) {
            if ((index != 0u && !output_literal(&output, ",")) ||
                !canonical_token(&input, array_item(&input, object_array, index),
                                 &output)) {
                status = LT48_NO_MEMORY;
                goto done;
            }
        }
        if ((object_count != 0u && !output_literal(&output, ",")) ||
            !canonical_token(&record, 0u, &output) ||
            !output_literal(&output, "],\"combinations\":") ||
            !canonical_token(&input, combinations, &output) ||
            !output_literal(&output, "}\n")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
    }
    file = fopen(output_path, "wx");
    if (file == NULL) {
        status = LT48_FILE_ERROR;
        goto done;
    }
    output_created = 1;
    if (fwrite(output.data, 1u, output.size, file) != output.size ||
        fflush(file) != 0 || fclose(file) != 0) {
        file = NULL;
        status = LT48_FILE_ERROR;
        goto done;
    }
    file = NULL;
    status = lt48_component_bundle_validate(output_path, &output_validation);
    if (status != LT48_OK) goto done;
    out_report->previous_object_count = input_validation.object_count;
    out_report->output_object_count = output_validation.object_count;
    memcpy(out_report->previous_source_sha256,
           input_validation.source_sha256, LT48_SHA256_SIZE);
    lt48_sha256(record_data, record_size, out_report->object_record_sha256);
    memcpy(out_report->output_source_sha256,
           output_validation.source_sha256, LT48_SHA256_SIZE);
done:
    if (file != NULL) (void)fclose(file);
    if (status != LT48_OK && output_created) (void)remove(output_path);
    free(output.data);
    free(objects);
    free(record.tokens);
    free(input.tokens);
    free(record_data);
    free(input_data);
    return status;
}

lt48_status lt48_component_bundle_add_object(
    const char *input_path, const char *output_path,
    const char *new_bundle_version, const char *new_created_date,
    const char *object_record_path,
    lt48_component_revision_report *out_report
) {
    return lt48_component_bundle_add_object_of_kind(
        input_path, output_path, new_bundle_version, new_created_date,
        object_record_path, NULL, out_report
    );
}

static property_member *member_find(
    const json_parser *parser, property_member *members, size_t count,
    const char *name
) {
    for (size_t index = 0u; index < count; ++index)
        if (token_equal(parser->data, &parser->tokens[members[index].key], name))
            return &members[index];
    return NULL;
}

static int exact_property_names(
    const json_parser *parser, property_member *members, size_t count,
    const char *const *names, size_t name_count
) {
    if (count != name_count) return 0;
    for (size_t index = 0u; index < name_count; ++index)
        if (member_find(parser, members, count, names[index]) == NULL) return 0;
    return 1;
}

static int effective_properties(
    const json_parser *parser, const component_object *objects,
    const size_t *items, size_t object_count, size_t target,
    property_member **out_members, size_t *out_count
) {
    size_t *chain;
    size_t chain_count = 0u;
    property_member *members = NULL;
    size_t count = 0u;
    chain = calloc(object_count ? object_count : 1u, sizeof(*chain));
    if (chain == NULL) return 0;
    for (size_t current = target;;) {
        size_t parent;
        chain[chain_count++] = current;
        if (objects[current].extends == SIZE_MAX) break;
        parent = find_object(parser, objects, object_count,
                             objects[current].extends);
        if (parent == SIZE_MAX || chain_count >= object_count) {
            free(chain);
            return 0;
        }
        current = parent;
    }
    while (chain_count != 0u) {
        size_t current = chain[--chain_count];
        size_t properties = object_find(parser, items[current], "properties");
        if (!collect_object_properties(parser, properties, &members, &count)) {
            free(members);
            free(chain);
            return 0;
        }
    }
    free(chain);
    *out_members = members;
    *out_count = count;
    return 1;
}

static int support_level_valid(const json_parser *parser, size_t token) {
    static const char *const levels[] = {
        "parsed", "planned", "behavior-validated",
        "engine-hardware-verified", "device-hardware-verified"
    };
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    for (size_t index = 0u; index < sizeof(levels) / sizeof(levels[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token], levels[index]))
            return 1;
    return 0;
}

static int operation_is_mutating(lt48_operation_kind operation) {
    switch (operation) {
    case LT48_OPERATION_PROGRAM:
    case LT48_OPERATION_OVER_PROGRAM:
    case LT48_OPERATION_ERASE:
    case LT48_OPERATION_SECURE:
    case LT48_OPERATION_MEMORY_PROTECT:
    case LT48_OPERATION_CONFIGURATION:
    case LT48_OPERATION_MASS_PRODUCTION:
    case LT48_OPERATION_JTAG_ISP:
        return 1;
    default:
        return 0;
    }
}

static int phase_kind_valid(const json_parser *parser, size_t token) {
    static const char *const kinds[] = {
        "prepare", "precheck", "power", "initialize", "transfer", "verify",
        "delay", "result", "cleanup"
    };
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    for (size_t index = 0u; index < sizeof(kinds) / sizeof(kinds[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token], kinds[index]))
            return 1;
    return 0;
}

static int state_map_valid(
    const json_parser *parser, size_t object, int rail_map
) {
    static const char *const rail_states[] = {
        "off", "nominal", "programming", "verify-low", "verify-high"
    };
    static const char *const pin_states[] = {
        "low", "high", "input", "output", "bidirectional", "high-impedance"
    };
    const char *const *states = rail_map ? rail_states : pin_states;
    size_t state_count = rail_map ?
        sizeof(rail_states) / sizeof(rail_states[0]) :
        sizeof(pin_states) / sizeof(pin_states[0]);
    property_member *entries = NULL;
    size_t count = 0u;
    size_t direct_tokens = 0u;
    int valid = 1;
    if (object >= parser->count || parser->tokens[object].type != JSON_OBJECT ||
        !collect_object_properties(parser, object, &entries, &count))
        return 0;
    for (size_t index = object + 1u; index < parser->count &&
         parser->tokens[index].start < parser->tokens[object].end; ++index)
        if (parser->tokens[index].parent == object) ++direct_tokens;
    if (direct_tokens % 2u != 0u || direct_tokens / 2u != count) valid = 0;
    for (size_t index = 0u; index < count && valid; ++index) {
        int state_valid = 0;
        if (!valid_identifier(parser, entries[index].key) ||
            parser->tokens[entries[index].value].type != JSON_STRING) {
            valid = 0;
            break;
        }
        for (size_t state = 0u; state < state_count; ++state)
            if (token_equal(parser->data,
                            &parser->tokens[entries[index].value],
                            states[state])) {
                state_valid = 1;
                break;
            }
        if (!state_valid) valid = 0;
    }
    free(entries);
    return valid;
}

static size_t phase_index_by_id(
    const json_parser *parser, const size_t *phases, size_t phase_count,
    size_t id
) {
    for (size_t index = 0u; index < phase_count; ++index)
        if (token_same(parser, object_find(parser, phases[index], "id"), id))
            return index;
    return SIZE_MAX;
}

static int phase_target_valid(
    const json_parser *parser, const size_t *phases, size_t phase_count,
    size_t target
) {
    return token_equal(parser->data, &parser->tokens[target], "complete") ||
           token_equal(parser->data, &parser->tokens[target], "error") ||
           token_equal(parser->data, &parser->tokens[target], "abort") ||
           phase_index_by_id(parser, phases, phase_count, target) != SIZE_MAX;
}

static int result_protocol_valid(const json_parser *parser, size_t object) {
    static const char *const names[] = {"completion", "error"};
    char completion[64];
    char error[64];
    if (!object_fields(parser, object, names, 2u)) return 0;
    return token_text(parser, object_find(parser, object, "completion"),
                      completion, sizeof(completion)) &&
           token_text(parser, object_find(parser, object, "error"),
                      error, sizeof(error));
}

static int validate_operation_properties(
    const json_parser *parser, property_member *members, size_t count
) {
    static const char *const names[] = {
        "name", "mutating", "support_level", "entry_phase", "cleanup_phase",
        "phases", "result_protocol"
    };
    static const char *const phase_names[] = {
        "id", "kind", "timeout_ms", "retry_limit", "rails", "pins",
        "payload", "on_success", "on_failure"
    };
    property_member *name;
    property_member *mutating;
    property_member *support;
    property_member *entry_phase;
    property_member *cleanup_phase;
    property_member *phases_member;
    property_member *result_protocol;
    lt48_operation_kind operation;
    char operation_name[48];
    int declared_mutating;
    size_t phase_count;
    size_t *phases = NULL;
    unsigned char *reachable = NULL;
    size_t entry_index;
    size_t cleanup_index;
    int has_transfer = 0;
    int valid = 0;
    if (!exact_property_names(parser, members, count, names, 7u)) return 0;
    name = member_find(parser, members, count, "name");
    mutating = member_find(parser, members, count, "mutating");
    support = member_find(parser, members, count, "support_level");
    entry_phase = member_find(parser, members, count, "entry_phase");
    cleanup_phase = member_find(parser, members, count, "cleanup_phase");
    phases_member = member_find(parser, members, count, "phases");
    result_protocol = member_find(parser, members, count, "result_protocol");
    if (!token_text(parser, name->value, operation_name,
                    sizeof(operation_name)) ||
        lt48_operation_kind_parse(operation_name, &operation) != LT48_OK ||
        !token_bool(parser, mutating->value, &declared_mutating) ||
        declared_mutating != operation_is_mutating(operation) ||
        !support_level_valid(parser, support->value) ||
        !valid_identifier(parser, entry_phase->value) ||
        !valid_identifier(parser, cleanup_phase->value) ||
        !result_protocol_valid(parser, result_protocol->value) ||
        (phase_count = array_count(parser, phases_member->value)) == SIZE_MAX ||
        phase_count < 2u || phase_count > 64u)
        return 0;
    phases = calloc(phase_count, sizeof(*phases));
    reachable = calloc(phase_count, sizeof(*reachable));
    if (phases == NULL || reachable == NULL) goto done;
    for (size_t index = 0u; index < phase_count; ++index) {
        uint64_t timeout;
        uint64_t retries;
        phases[index] = array_item(parser, phases_member->value, index);
        if (!object_fields(parser, phases[index], phase_names, 9u) ||
            !valid_identifier(parser, object_find(parser, phases[index], "id")) ||
            !phase_kind_valid(parser,
                              object_find(parser, phases[index], "kind")) ||
            !token_uint64(parser,
                          object_find(parser, phases[index], "timeout_ms"),
                          &timeout) || timeout == 0u || timeout > 3600000u ||
            !token_uint64(parser,
                          object_find(parser, phases[index], "retry_limit"),
                          &retries) || retries > 16u ||
            !state_map_valid(parser,
                             object_find(parser, phases[index], "rails"), 1) ||
            !state_map_valid(parser,
                             object_find(parser, phases[index], "pins"), 0) ||
            (!valid_identifier(parser,
                 object_find(parser, phases[index], "payload")) &&
             !token_equal(parser->data,
                 &parser->tokens[object_find(
                     parser, phases[index], "payload")], "null")))
            goto done;
        if (token_equal(parser->data,
                        &parser->tokens[object_find(
                            parser, phases[index], "kind")], "transfer"))
            has_transfer = 1;
        for (size_t prior = 0u; prior < index; ++prior)
            if (token_same(parser,
                    object_find(parser, phases[index], "id"),
                    object_find(parser, phases[prior], "id")))
                goto done;
    }
    entry_index = phase_index_by_id(parser, phases, phase_count,
                                    entry_phase->value);
    cleanup_index = phase_index_by_id(parser, phases, phase_count,
                                      cleanup_phase->value);
    if (entry_index == SIZE_MAX || cleanup_index == SIZE_MAX ||
        entry_index == cleanup_index || !has_transfer ||
        !token_equal(parser->data,
            &parser->tokens[object_find(parser, phases[cleanup_index], "kind")],
            "cleanup"))
        goto done;
    for (size_t index = 0u; index < phase_count; ++index) {
        size_t success = object_find(parser, phases[index], "on_success");
        size_t failure = object_find(parser, phases[index], "on_failure");
        if (!phase_target_valid(parser, phases, phase_count, success) ||
            !phase_target_valid(parser, phases, phase_count, failure) ||
            (index != cleanup_index &&
             !token_same(parser, failure, cleanup_phase->value)))
            goto done;
    }
    reachable[entry_index] = 1u;
    for (size_t pass = 0u; pass < phase_count; ++pass)
        for (size_t index = 0u; index < phase_count; ++index)
            if (reachable[index]) {
                size_t targets[] = {
                    object_find(parser, phases[index], "on_success"),
                    object_find(parser, phases[index], "on_failure")
                };
                for (size_t target = 0u; target < 2u; ++target) {
                    size_t next = phase_index_by_id(
                        parser, phases, phase_count, targets[target]);
                    if (next != SIZE_MAX) reachable[next] = 1u;
                }
            }
    for (size_t index = 0u; index < phase_count; ++index)
        if (!reachable[index]) goto done;
    valid = 1;
done:
    free(reachable);
    free(phases);
    return valid;
}

static int validate_variable_properties(
    const json_parser *parser, property_member *members, size_t count
) {
    static const char *const names[] = {
        "unit", "minimum", "nominal", "maximum", "subgroup_effect", "rail"
    };
    property_member *unit;
    property_member *minimum;
    property_member *nominal;
    property_member *maximum;
    property_member *subgroup;
    property_member *rail;
    uint64_t low;
    uint64_t normal;
    uint64_t high;
    int subgroup_effect;
    char unit_text[48];
    if (!exact_property_names(parser, members, count, names, 6u)) return 0;
    unit = member_find(parser, members, count, "unit");
    minimum = member_find(parser, members, count, "minimum");
    nominal = member_find(parser, members, count, "nominal");
    maximum = member_find(parser, members, count, "maximum");
    subgroup = member_find(parser, members, count, "subgroup_effect");
    rail = member_find(parser, members, count, "rail");
    return token_text(parser, unit->value, unit_text, sizeof(unit_text)) &&
           token_uint64(parser, minimum->value, &low) &&
           token_uint64(parser, nominal->value, &normal) &&
           token_uint64(parser, maximum->value, &high) &&
           low <= normal && normal <= high && high <= UINT32_MAX &&
           token_bool(parser, subgroup->value, &subgroup_effect) &&
           (valid_identifier(parser, rail->value) ||
            token_equal(parser->data, &parser->tokens[rail->value], "null"));
}

static int validate_pin_map_properties(
    const json_parser *parser, property_member *members, size_t count
) {
    static const char *const names[] = {"pin_count", "logical_pins"};
    property_member *pin_count_member;
    property_member *logical_pins;
    uint64_t pin_count;
    size_t logical_count;
    if (!exact_property_names(parser, members, count, names, 2u)) return 0;
    pin_count_member = member_find(parser, members, count, "pin_count");
    logical_pins = member_find(parser, members, count, "logical_pins");
    if (!token_uint64(parser, pin_count_member->value, &pin_count) ||
        pin_count == 0u || pin_count > UINT16_MAX ||
        (logical_count = array_count(parser, logical_pins->value)) == SIZE_MAX ||
        logical_count == 0u || logical_count > pin_count) return 0;
    for (size_t index = 0u; index < logical_count; ++index) {
        size_t item = array_item(parser, logical_pins->value, index);
        if (!valid_identifier(parser, item)) return 0;
        for (size_t prior = 0u; prior < index; ++prior)
            if (token_same(parser, item,
                           array_item(parser, logical_pins->value, prior)))
                return 0;
    }
    return 1;
}

static int validate_evidence_properties(
    const json_parser *parser, size_t documents, size_t references,
    property_member *members, size_t count
) {
    static const char *const names[] = {"document"};
    property_member *document;
    size_t document_count;
    if (!exact_property_names(parser, members, count, names, 1u) ||
        array_count(parser, references) != 0u ||
        (document_count = array_count(parser, documents)) == SIZE_MAX)
        return 0;
    document = member_find(parser, members, count, "document");
    if (!valid_identifier(parser, document->value)) return 0;
    for (size_t index = 0u; index < document_count; ++index) {
        size_t item = array_item(parser, documents, index);
        size_t id = object_find(parser, item, "id");
        if (id != SIZE_MAX && token_same(parser, id, document->value))
            return 1;
    }
    return 0;
}

static size_t reference_kind_count(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references, const char *kind,
    int *out_only_allowed, const char *const *allowed, size_t allowed_count
) {
    size_t count = 0u;
    size_t reference_count = array_count(parser, references);
    if (reference_count == SIZE_MAX) {
        *out_only_allowed = 0;
        return 0u;
    }
    for (size_t ref = 0u; ref < reference_count; ++ref) {
        size_t id = array_item(parser, references, ref);
        size_t target = find_object(parser, objects, object_count, id);
        int permitted = 0;
        if (target == SIZE_MAX) {
            *out_only_allowed = 0;
            continue;
        }
        if (token_equal(parser->data, &parser->tokens[objects[target].kind],
                        kind))
            ++count;
        for (size_t index = 0u; index < allowed_count; ++index)
            if (token_equal(parser->data,
                            &parser->tokens[objects[target].kind],
                            allowed[index])) {
                permitted = 1;
                break;
            }
        if (!permitted) *out_only_allowed = 0;
    }
    return count;
}

static int payload_relative_path(
    const json_parser *parser, size_t token
) {
    char path[512];
    const char *cursor;
    if (!token_text(parser, token, path, sizeof(path)) || path[0] == '/' ||
        path[0] == '\\' || strchr(path, ':') != NULL) return 0;
    for (cursor = path; *cursor != '\0';) {
        const char *end = strchr(cursor, '/');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        if (length == 0u || (length == 1u && cursor[0] == '.') ||
            (length == 2u && cursor[0] == '.' && cursor[1] == '.')) return 0;
        if (end == NULL) break;
        cursor = end + 1;
    }
    return 1;
}

static int payload_text_array(
    const json_parser *parser, size_t token, int allow_empty
) {
    size_t count = array_count(parser, token);
    char text[192];
    if (count == SIZE_MAX || count > 128u || (!allow_empty && count == 0u))
        return 0;
    for (size_t index = 0u; index < count; ++index)
        if (!token_text(parser, array_item(parser, token, index), text,
                        sizeof(text))) return 0;
    return 1;
}

static int validate_payload_build(
    const json_parser *parser, size_t build
) {
    static const char *const fields[] = {
        "source_path", "source_license", "source_sha256", "toolchain",
        "toolchain_path", "toolchain_version", "toolchain_sha256", "manifest_path",
        "manifest_sha256", "arguments", "environment", "output_path",
        "output_sha256", "output_size", "target", "resource_limits",
        "behavior"
    };
    static const char *const behavior_fields[] = {
        "inputs", "outputs", "error_codes", "entry", "completion",
        "timeout", "abort"
    };
    property_member *entries = NULL;
    size_t entry_count = 0u;
    char text[192];
    uint64_t number;
    int valid = 0;
    size_t environment;
    size_t limits;
    size_t behavior;
    if (!object_fields(parser, build, fields,
                       sizeof(fields) / sizeof(fields[0])) ||
        !payload_relative_path(parser, object_find(parser, build, "source_path")) ||
        !token_text(parser, object_find(parser, build, "source_license"), text,
                    sizeof(text)) ||
        !valid_sha256(parser, object_find(parser, build, "source_sha256")) ||
        !token_text(parser, object_find(parser, build, "toolchain"), text,
                    sizeof(text)) ||
        !payload_relative_path(parser,
            object_find(parser, build, "toolchain_path")) ||
        !token_text(parser, object_find(parser, build, "toolchain_version"),
                    text, sizeof(text)) ||
        !valid_sha256(parser, object_find(parser, build, "toolchain_sha256")) ||
        !payload_relative_path(parser,
            object_find(parser, build, "manifest_path")) ||
        !valid_sha256(parser, object_find(parser, build, "manifest_sha256")) ||
        !payload_text_array(parser, object_find(parser, build, "arguments"), 0) ||
        !token_same(parser,
            array_item(parser, object_find(parser, build, "arguments"), 0u),
            object_find(parser, build, "toolchain")) ||
        !payload_relative_path(parser, object_find(parser, build, "output_path")) ||
        !valid_sha256(parser, object_find(parser, build, "output_sha256")) ||
        !token_uint64(parser, object_find(parser, build, "output_size"),
                      &number) || number == 0u ||
        !token_text(parser, object_find(parser, build, "target"), text,
                    sizeof(text))) return 0;
    environment = object_find(parser, build, "environment");
    if (!collect_object_properties(parser, environment, &entries, &entry_count) ||
        entry_count > 64u)
        return 0;
    for (size_t index = 0u; index < entry_count; ++index)
        if (!valid_identifier(parser, entries[index].key) ||
            !token_text(parser, entries[index].value, text, sizeof(text)))
            goto done;
    free(entries);
    entries = NULL;
    entry_count = 0u;
    limits = object_find(parser, build, "resource_limits");
    if (!collect_object_properties(parser, limits, &entries, &entry_count) ||
        entry_count == 0u) goto done;
    for (size_t index = 0u; index < entry_count; ++index)
        if (!valid_identifier(parser, entries[index].key) ||
            !token_uint64(parser, entries[index].value, &number) || number == 0u)
            goto done;
    behavior = object_find(parser, build, "behavior");
    if (!object_fields(parser, behavior, behavior_fields,
                       sizeof(behavior_fields) / sizeof(behavior_fields[0])) ||
        !payload_text_array(parser, object_find(parser, behavior, "inputs"), 1) ||
        !payload_text_array(parser, object_find(parser, behavior, "outputs"), 0) ||
        !payload_text_array(parser, object_find(parser, behavior, "error_codes"), 0) ||
        !token_text(parser, object_find(parser, behavior, "entry"), text,
                    sizeof(text)) ||
        !token_text(parser, object_find(parser, behavior, "completion"), text,
                    sizeof(text)) ||
        !token_text(parser, object_find(parser, behavior, "timeout"), text,
                    sizeof(text)) ||
        !token_text(parser, object_find(parser, behavior, "abort"), text,
                    sizeof(text))) goto done;
    valid = 1;
done:
    free(entries);
    return valid;
}

static int validate_payload_properties(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references,
    property_member *members, size_t count
) {
    static const char *const names[] = {
        "origin", "payload_type", "format", "source_available",
        "reproducible_build", "abi", "side_effects"
    };
    static const char *const authored_names[] = {
        "origin", "payload_type", "format", "source_available",
        "reproducible_build", "abi", "side_effects", "build"
    };
    static const char *const allowed[] = {"evidence"};
    property_member *origin;
    property_member *payload_type;
    property_member *format;
    property_member *source_available;
    property_member *reproducible_build;
    property_member *abi;
    property_member *side_effects;
    property_member *build;
    char origin_text[48];
    char payload_type_text[48];
    char format_text[64];
    char abi_text[96];
    int has_source;
    int reproducible;
    int only_allowed = 1;
    size_t evidence_count;
    size_t effect_count;
    if (count != 7u && count != 8u) return 0;
    origin = member_find(parser, members, count, "origin");
    payload_type = member_find(parser, members, count, "payload_type");
    format = member_find(parser, members, count, "format");
    source_available = member_find(parser, members, count, "source_available");
    reproducible_build = member_find(parser, members, count,
                                     "reproducible_build");
    abi = member_find(parser, members, count, "abi");
    side_effects = member_find(parser, members, count, "side_effects");
    build = member_find(parser, members, count, "build");
    if (origin == NULL || payload_type == NULL || format == NULL ||
        source_available == NULL || reproducible_build == NULL || abi == NULL ||
        side_effects == NULL) return 0;
    evidence_count = reference_kind_count(
        parser, objects, object_count, references, "evidence",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    if (!token_text(parser, origin->value, origin_text, sizeof(origin_text)) ||
        (strcmp(origin_text, "imported-vendor") != 0 &&
         strcmp(origin_text, "authored") != 0) ||
        !token_text(parser, payload_type->value, payload_type_text,
                    sizeof(payload_type_text)) ||
        (strcmp(payload_type_text, "resident-monitor") != 0 &&
         strcmp(payload_type_text, "80c32-applet") != 0 &&
         strcmp(payload_type_text, "fpga-image") != 0) ||
        !token_text(parser, format->value, format_text, sizeof(format_text)) ||
        !token_bool(parser, source_available->value, &has_source) ||
        !token_bool(parser, reproducible_build->value, &reproducible) ||
        !token_text(parser, abi->value, abi_text, sizeof(abi_text)) ||
        !only_allowed || evidence_count == 0u ||
        (effect_count = array_count(parser, side_effects->value)) == SIZE_MAX ||
        effect_count == 0u || (reproducible && !has_source) ||
        (strcmp(origin_text, "authored") == 0 &&
         (!has_source || !reproducible || build == NULL ||
          !exact_property_names(parser, members, count, authored_names, 8u) ||
          !validate_payload_build(parser, build->value))) ||
        (strcmp(origin_text, "imported-vendor") == 0 &&
         (build != NULL || !exact_property_names(parser, members, count,
                                                 names, 7u))))
        return 0;
    for (size_t index = 0u; index < effect_count; ++index) {
        size_t item = array_item(parser, side_effects->value, index);
        if (!valid_identifier(parser, item)) return 0;
        for (size_t prior = 0u; prior < index; ++prior)
            if (token_same(parser, item,
                           array_item(parser, side_effects->value, prior)))
                return 0;
    }
    return 1;
}

static int package_role_valid(const json_parser *parser, size_t token) {
    static const char *const values[] = {
        "supply", "ground", "address", "data", "control", "no-connect",
        "other"
    };
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    for (size_t index = 0u; index < sizeof(values) / sizeof(values[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token], values[index]))
            return 1;
    return 0;
}

static int package_direction_valid(const json_parser *parser, size_t token) {
    static const char *const values[] = {
        "input", "output", "bidirectional", "power", "passive"
    };
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING)
        return 0;
    for (size_t index = 0u; index < sizeof(values) / sizeof(values[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token], values[index]))
            return 1;
    return 0;
}

static int validate_package_properties(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references,
    property_member *members, size_t count
) {
    static const char *const names[] = {"pin_count", "orientation", "pins"};
    static const char *const pin_names[] = {
        "number", "name", "role", "direction"
    };
    static const char *const allowed[] = {"evidence"};
    property_member *pin_count_member;
    property_member *orientation;
    property_member *pins;
    uint64_t pin_count;
    char orientation_text[64];
    size_t pins_count;
    int only_allowed = 1;
    size_t evidence_count;
    if (!exact_property_names(parser, members, count, names, 3u)) return 0;
    pin_count_member = member_find(parser, members, count, "pin_count");
    orientation = member_find(parser, members, count, "orientation");
    pins = member_find(parser, members, count, "pins");
    evidence_count = reference_kind_count(
        parser, objects, object_count, references, "evidence",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    if (!token_uint64(parser, pin_count_member->value, &pin_count) ||
        pin_count == 0u || pin_count > 512u ||
        !token_text(parser, orientation->value, orientation_text,
                    sizeof(orientation_text)) ||
        (strcmp(orientation_text, "pin-1-notch") != 0 &&
         strcmp(orientation_text, "pin-1-dot") != 0 &&
         strcmp(orientation_text, "package-drawing") != 0) ||
        (pins_count = array_count(parser, pins->value)) == SIZE_MAX ||
        pins_count != pin_count || !only_allowed || evidence_count == 0u)
        return 0;
    for (size_t index = 0u; index < pins_count; ++index) {
        size_t pin = array_item(parser, pins->value, index);
        size_t number;
        size_t name;
        size_t role;
        size_t direction;
        uint64_t number_value;
        if (!object_fields(parser, pin, pin_names, 4u)) return 0;
        number = object_find(parser, pin, "number");
        name = object_find(parser, pin, "name");
        role = object_find(parser, pin, "role");
        direction = object_find(parser, pin, "direction");
        if (!token_uint64(parser, number, &number_value) ||
            number_value == 0u || number_value > pin_count ||
            !valid_identifier(parser, name) ||
            !package_role_valid(parser, role) ||
            !package_direction_valid(parser, direction))
            return 0;
        for (size_t prior = 0u; prior < index; ++prior) {
            uint64_t prior_number;
            size_t prior_pin = array_item(parser, pins->value, prior);
            if (!token_uint64(parser, object_find(parser, prior_pin, "number"),
                              &prior_number) || prior_number == number_value)
                return 0;
        }
    }
    return 1;
}

static int validate_adapter_properties(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references,
    property_member *members, size_t count
) {
    static const char *const names[] = {
        "topology", "programmer_pin_count", "package_pin_count",
        "orientation", "mappings", "power_paths", "active_components",
        "safe_state", "maximum_voltage_millivolts"
    };
    static const char *const mapping_names[] = {
        "programmer_pin", "net", "package_pin"
    };
    static const char *const allowed[] = {"package", "evidence"};
    property_member *topology;
    property_member *programmer_pin_count;
    property_member *package_pin_count;
    property_member *orientation;
    property_member *mappings;
    property_member *power_paths;
    property_member *active_components;
    property_member *safe_state;
    property_member *maximum_voltage;
    char topology_text[32];
    char orientation_text[96];
    char safe_state_text[48];
    uint64_t programmer_pins;
    uint64_t package_pins;
    uint64_t maximum_millivolts;
    int active;
    int only_allowed = 1;
    size_t package_count;
    size_t evidence_count;
    size_t mapping_count;
    size_t power_path_count;
    if (!exact_property_names(parser, members, count, names, 9u)) return 0;
    topology = member_find(parser, members, count, "topology");
    programmer_pin_count = member_find(parser, members, count,
                                       "programmer_pin_count");
    package_pin_count = member_find(parser, members, count,
                                    "package_pin_count");
    orientation = member_find(parser, members, count, "orientation");
    mappings = member_find(parser, members, count, "mappings");
    power_paths = member_find(parser, members, count, "power_paths");
    active_components = member_find(parser, members, count,
                                    "active_components");
    safe_state = member_find(parser, members, count, "safe_state");
    maximum_voltage = member_find(parser, members, count,
                                  "maximum_voltage_millivolts");
    package_count = reference_kind_count(
        parser, objects, object_count, references, "package",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    evidence_count = reference_kind_count(
        parser, objects, object_count, references, "evidence",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    if (!token_text(parser, topology->value, topology_text,
                    sizeof(topology_text)) ||
        (strcmp(topology_text, "direct") != 0 &&
         strcmp(topology_text, "passive") != 0 &&
         strcmp(topology_text, "active") != 0 &&
         strcmp(topology_text, "isp") != 0) ||
        !token_uint64(parser, programmer_pin_count->value, &programmer_pins) ||
        programmer_pins == 0u || programmer_pins > 512u ||
        !token_uint64(parser, package_pin_count->value, &package_pins) ||
        package_pins == 0u || package_pins > 512u ||
        !token_text(parser, orientation->value, orientation_text,
                    sizeof(orientation_text)) ||
        !token_bool(parser, active_components->value, &active) ||
        !token_text(parser, safe_state->value, safe_state_text,
                    sizeof(safe_state_text)) ||
        (strcmp(safe_state_text, "all-high-impedance") != 0 &&
         strcmp(safe_state_text, "disconnected") != 0 &&
         strcmp(safe_state_text, "passive-only") != 0) ||
        !token_uint64(parser, maximum_voltage->value, &maximum_millivolts) ||
        maximum_millivolts == 0u || maximum_millivolts > 30000u ||
        !only_allowed || package_count != 1u || evidence_count == 0u ||
        ((strcmp(topology_text, "direct") == 0 ||
          strcmp(topology_text, "passive") == 0) && active) ||
        (strcmp(topology_text, "active") == 0 && !active) ||
        (mapping_count = array_count(parser, mappings->value)) == SIZE_MAX ||
        mapping_count != package_pins ||
        (power_path_count = array_count(parser, power_paths->value)) ==
            SIZE_MAX || power_path_count == 0u)
        return 0;
    for (size_t index = 0u; index < power_path_count; ++index) {
        size_t item = array_item(parser, power_paths->value, index);
        if (!valid_identifier(parser, item)) return 0;
        for (size_t prior = 0u; prior < index; ++prior)
            if (token_same(parser, item,
                           array_item(parser, power_paths->value, prior)))
                return 0;
    }
    for (size_t index = 0u; index < mapping_count; ++index) {
        size_t mapping = array_item(parser, mappings->value, index);
        size_t programmer_pin;
        size_t net;
        size_t package_pin;
        uint64_t programmer_pin_value;
        uint64_t package_pin_value;
        if (!object_fields(parser, mapping, mapping_names, 3u)) return 0;
        programmer_pin = object_find(parser, mapping, "programmer_pin");
        net = object_find(parser, mapping, "net");
        package_pin = object_find(parser, mapping, "package_pin");
        if (!token_uint64(parser, programmer_pin, &programmer_pin_value) ||
            programmer_pin_value == 0u || programmer_pin_value > programmer_pins ||
            !valid_identifier(parser, net) ||
            !token_uint64(parser, package_pin, &package_pin_value) ||
            package_pin_value == 0u || package_pin_value > package_pins)
            return 0;
        for (size_t prior = 0u; prior < index; ++prior) {
            size_t prior_mapping = array_item(parser, mappings->value, prior);
            uint64_t prior_programmer_pin;
            uint64_t prior_package_pin;
            if (!token_uint64(parser,
                    object_find(parser, prior_mapping, "programmer_pin"),
                    &prior_programmer_pin) ||
                !token_uint64(parser,
                    object_find(parser, prior_mapping, "package_pin"),
                    &prior_package_pin) ||
                prior_programmer_pin == programmer_pin_value ||
                prior_package_pin == package_pin_value)
                return 0;
        }
    }
    return 1;
}

static size_t referenced_object_of_kind(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references, const char *kind
) {
    size_t found = SIZE_MAX;
    size_t count = array_count(parser, references);
    if (count == SIZE_MAX) return SIZE_MAX;
    for (size_t index = 0u; index < count; ++index) {
        size_t target = find_object(parser, objects, object_count,
                                    array_item(parser, references, index));
        if (target != SIZE_MAX &&
            token_equal(parser->data, &parser->tokens[objects[target].kind],
                        kind)) {
            if (found != SIZE_MAX) return SIZE_MAX;
            found = target;
        }
    }
    return found;
}

static int token_array_contains(
    const json_parser *parser, size_t array, size_t wanted
) {
    size_t count = array_count(parser, array);
    if (count == SIZE_MAX) return 0;
    for (size_t index = 0u; index < count; ++index)
        if (token_same(parser, array_item(parser, array, index), wanted))
            return 1;
    return 0;
}

static int package_pins_contain_name(
    const json_parser *parser, size_t package_pins, size_t wanted
) {
    size_t count = array_count(parser, package_pins);
    if (count == SIZE_MAX) return 0;
    for (size_t index = 0u; index < count; ++index) {
        size_t pin = array_item(parser, package_pins, index);
        if (token_same(parser, object_find(parser, pin, "name"), wanted))
            return 1;
    }
    return 0;
}

static int operation_phase_requirements_fit(
    const json_parser *parser, const component_object *objects,
    const size_t *items, size_t object_count, size_t algorithm,
    size_t package_pins, size_t power_paths
) {
    size_t reference_count = array_count(parser, objects[algorithm].references);
    size_t operation_count = 0u;
    if (reference_count == SIZE_MAX) return 0;
    for (size_t reference = 0u; reference < reference_count; ++reference) {
        size_t operation = find_object(
            parser, objects, object_count,
            array_item(parser, objects[algorithm].references, reference));
        property_member *operation_members = NULL;
        size_t operation_member_count = 0u;
        property_member *phases;
        size_t phase_count;
        int valid = 1;
        if (operation == SIZE_MAX ||
            !token_equal(parser->data,
                         &parser->tokens[objects[operation].kind], "operation"))
            continue;
        ++operation_count;
        if (!effective_properties(parser, objects, items, object_count,
                                  operation, &operation_members,
                                  &operation_member_count))
            return 0;
        phases = member_find(parser, operation_members,
                             operation_member_count, "phases");
        phase_count = phases == NULL ? SIZE_MAX :
            array_count(parser, phases->value);
        if (phase_count == SIZE_MAX) valid = 0;
        for (size_t phase_index = 0u;
             valid && phase_index < phase_count; ++phase_index) {
            size_t phase = array_item(parser, phases->value, phase_index);
            size_t payload = object_find(parser, phase, "payload");
            size_t state_objects[] = {
                object_find(parser, phase, "rails"),
                object_find(parser, phase, "pins")
            };
            for (size_t state_kind = 0u; valid && state_kind < 2u;
                 ++state_kind) {
                property_member *states = NULL;
                size_t state_count = 0u;
                if (!collect_object_properties(parser, state_objects[state_kind],
                                               &states, &state_count)) {
                    valid = 0;
                    break;
                }
                for (size_t state = 0u; state < state_count; ++state)
                    if (state_kind == 0u ?
                        !token_array_contains(parser, power_paths,
                                              states[state].key) :
                        !package_pins_contain_name(parser, package_pins,
                                                   states[state].key)) {
                        valid = 0;
                        break;
                    }
                free(states);
            }
            if (valid && parser->tokens[payload].type == JSON_STRING) {
                int payload_found = 0;
                for (size_t ref = 0u; ref < reference_count; ++ref) {
                    size_t ref_id = array_item(
                        parser, objects[algorithm].references, ref);
                    size_t target = find_object(
                        parser, objects, object_count, ref_id);
                    if (target != SIZE_MAX && token_same(parser, ref_id, payload) &&
                        token_equal(parser->data,
                            &parser->tokens[objects[target].kind], "payload")) {
                        payload_found = 1;
                        break;
                    }
                }
                if (!payload_found) valid = 0;
            }
        }
        free(operation_members);
        if (!valid) return 0;
    }
    return operation_count != 0u;
}

static int validate_combination_composition(
    const json_parser *parser, const component_object *objects,
    const size_t *items, size_t object_count, size_t combination
) {
    size_t variant_id = object_find(parser, combination, "variant");
    size_t package_id = object_find(parser, combination, "package");
    size_t adapter_id = object_find(parser, combination, "adapter");
    size_t variant;
    size_t package;
    size_t adapter;
    size_t family;
    size_t algorithm;
    size_t pin_map;
    property_member *package_members = NULL;
    property_member *adapter_members = NULL;
    property_member *pin_map_members = NULL;
    size_t package_member_count = 0u;
    size_t adapter_member_count = 0u;
    size_t pin_map_member_count = 0u;
    property_member *package_pin_count;
    property_member *package_pins;
    property_member *adapter_pin_count;
    property_member *mappings;
    property_member *power_paths;
    property_member *logical_pins;
    uint64_t package_pin_count_value;
    uint64_t adapter_pin_count_value;
    int valid = 0;
    if (variant_id == SIZE_MAX || package_id == SIZE_MAX ||
        adapter_id == SIZE_MAX ||
        parser->tokens[adapter_id].type != JSON_STRING)
        goto done;
    variant = find_object(parser, objects, object_count, variant_id);
    package = find_object(parser, objects, object_count, package_id);
    adapter = find_object(parser, objects, object_count, adapter_id);
    if (variant == SIZE_MAX || package == SIZE_MAX || adapter == SIZE_MAX ||
        !token_array_contains(parser, objects[adapter].references, package_id))
        goto done;
    family = referenced_object_of_kind(
        parser, objects, object_count, objects[variant].references, "family");
    if (family == SIZE_MAX) goto done;
    algorithm = referenced_object_of_kind(
        parser, objects, object_count, objects[family].references, "algorithm");
    if (algorithm == SIZE_MAX) goto done;
    pin_map = referenced_object_of_kind(
        parser, objects, object_count, objects[algorithm].references, "pin-map");
    if (pin_map == SIZE_MAX ||
        !effective_properties(parser, objects, items, object_count, package,
                              &package_members, &package_member_count) ||
        !effective_properties(parser, objects, items, object_count, adapter,
                              &adapter_members, &adapter_member_count) ||
        !effective_properties(parser, objects, items, object_count, pin_map,
                              &pin_map_members, &pin_map_member_count))
        goto done;
    package_pin_count = member_find(parser, package_members,
                                    package_member_count, "pin_count");
    package_pins = member_find(parser, package_members,
                               package_member_count, "pins");
    adapter_pin_count = member_find(parser, adapter_members,
                                    adapter_member_count, "package_pin_count");
    mappings = member_find(parser, adapter_members, adapter_member_count,
                           "mappings");
    power_paths = member_find(parser, adapter_members, adapter_member_count,
                              "power_paths");
    logical_pins = member_find(parser, pin_map_members, pin_map_member_count,
                               "logical_pins");
    if (package_pin_count == NULL || package_pins == NULL ||
        adapter_pin_count == NULL || mappings == NULL || power_paths == NULL ||
        logical_pins == NULL ||
        !token_uint64(parser, package_pin_count->value,
                      &package_pin_count_value) ||
        !token_uint64(parser, adapter_pin_count->value,
                      &adapter_pin_count_value) ||
        package_pin_count_value != adapter_pin_count_value)
        goto done;
    {
        size_t mapping_count = array_count(parser, mappings->value);
        size_t pin_count = array_count(parser, package_pins->value);
        if (mapping_count == SIZE_MAX || pin_count == SIZE_MAX ||
            mapping_count != pin_count) goto done;
        for (size_t mapping_index = 0u; mapping_index < mapping_count;
             ++mapping_index) {
            size_t mapping = array_item(parser, mappings->value, mapping_index);
            uint64_t package_number;
            size_t mapping_net = object_find(parser, mapping, "net");
            int found = 0;
            if (!token_uint64(parser,
                    object_find(parser, mapping, "package_pin"),
                    &package_number)) goto done;
            for (size_t pin_index = 0u; pin_index < pin_count; ++pin_index) {
                size_t pin = array_item(parser, package_pins->value, pin_index);
                uint64_t number;
                if (!token_uint64(parser, object_find(parser, pin, "number"),
                                  &number)) goto done;
                if (number == package_number) {
                    if (!token_same(parser, mapping_net,
                                    object_find(parser, pin, "name")))
                        goto done;
                    found = 1;
                    break;
                }
            }
            if (!found) goto done;
        }
        for (size_t pin_index = 0u; pin_index < pin_count; ++pin_index) {
            size_t pin = array_item(parser, package_pins->value, pin_index);
            size_t role = object_find(parser, pin, "role");
            if ((token_equal(parser->data, &parser->tokens[role], "supply") ||
                 token_equal(parser->data, &parser->tokens[role], "ground")) &&
                !token_array_contains(parser, power_paths->value,
                                      object_find(parser, pin, "name")))
                goto done;
        }
        {
            size_t logical_count = array_count(parser, logical_pins->value);
            if (logical_count == SIZE_MAX) goto done;
            for (size_t logical = 0u; logical < logical_count; ++logical) {
                size_t wanted = array_item(parser, logical_pins->value, logical);
                int found = 0;
                for (size_t pin_index = 0u; pin_index < pin_count; ++pin_index) {
                    size_t pin = array_item(parser, package_pins->value, pin_index);
                    if (token_same(parser, wanted,
                                   object_find(parser, pin, "name"))) {
                        found = 1;
                        break;
                    }
                }
                if (!found) goto done;
            }
        }
        if (!operation_phase_requirements_fit(
                parser, objects, items, object_count, algorithm,
                package_pins->value, power_paths->value))
            goto done;
    }
    valid = 1;
done:
    free(pin_map_members);
    free(adapter_members);
    free(package_members);
    return valid;
}

static int validate_algorithm_properties(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references,
    property_member *members, size_t count
) {
    static const char *const names[] = {"technology", "address_unit_bits"};
    static const char *const allowed[] = {
        "operation", "variable", "pin-map", "payload", "evidence"
    };
    property_member *technology;
    property_member *address_unit_bits;
    char technology_text[64];
    uint64_t unit_bits;
    int only_allowed = 1;
    size_t operation_count;
    size_t variable_count;
    size_t pin_map_count;
    if (!exact_property_names(parser, members, count, names, 2u)) return 0;
    technology = member_find(parser, members, count, "technology");
    address_unit_bits = member_find(parser, members, count,
                                    "address_unit_bits");
    operation_count = reference_kind_count(
        parser, objects, object_count, references, "operation",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    variable_count = reference_kind_count(
        parser, objects, object_count, references, "variable",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    pin_map_count = reference_kind_count(
        parser, objects, object_count, references, "pin-map",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    return token_text(parser, technology->value, technology_text,
                      sizeof(technology_text)) &&
           token_uint64(parser, address_unit_bits->value, &unit_bits) &&
           (unit_bits == 1u || unit_bits == 4u || unit_bits == 8u ||
            unit_bits == 16u || unit_bits == 32u || unit_bits == 64u) &&
           only_allowed && operation_count >= 1u && variable_count >= 1u &&
           pin_map_count == 1u;
}

static int validate_family_properties(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references,
    property_member *members, size_t count
) {
    static const char *const names[] = {"blank_value", "identification_method"};
    static const char *const allowed[] = {"algorithm", "evidence"};
    property_member *blank_value;
    property_member *identification_method;
    uint64_t blank;
    char method[48];
    int only_allowed = 1;
    size_t algorithm_count;
    if (!exact_property_names(parser, members, count, names, 2u)) return 0;
    blank_value = member_find(parser, members, count, "blank_value");
    identification_method = member_find(parser, members, count,
                                        "identification_method");
    algorithm_count = reference_kind_count(
        parser, objects, object_count, references, "algorithm",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    return token_uint64(parser, blank_value->value, &blank) && blank <= 255u &&
           token_text(parser, identification_method->value, method,
                      sizeof(method)) &&
           (strcmp(method, "none") == 0 ||
            strcmp(method, "selected-device") == 0 ||
            strcmp(method, "general-auto-id") == 0) &&
           only_allowed && algorithm_count == 1u;
}

static int validate_variant_properties(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t references,
    property_member *members, size_t count
) {
    static const char *const names[] = {
        "manufacturer", "part", "capacity_bytes", "address_width", "bus_width"
    };
    static const char *const allowed[] = {"family", "evidence"};
    property_member *manufacturer;
    property_member *part;
    property_member *capacity_bytes;
    property_member *address_width;
    property_member *bus_width;
    char manufacturer_text[96];
    char part_text[96];
    uint64_t capacity;
    uint64_t width;
    uint64_t bus;
    uint64_t total_bits;
    uint64_t addressable_units;
    uint64_t maximum_units;
    int only_allowed = 1;
    size_t family_count;
    if (!exact_property_names(parser, members, count, names, 5u)) return 0;
    manufacturer = member_find(parser, members, count, "manufacturer");
    part = member_find(parser, members, count, "part");
    capacity_bytes = member_find(parser, members, count, "capacity_bytes");
    address_width = member_find(parser, members, count, "address_width");
    bus_width = member_find(parser, members, count, "bus_width");
    family_count = reference_kind_count(
        parser, objects, object_count, references, "family",
        &only_allowed, allowed, sizeof(allowed) / sizeof(allowed[0]));
    if (!token_text(parser, manufacturer->value, manufacturer_text,
                    sizeof(manufacturer_text)) ||
        !token_text(parser, part->value, part_text, sizeof(part_text)) ||
        !token_uint64(parser, capacity_bytes->value, &capacity) ||
        !token_uint64(parser, address_width->value, &width) ||
        !token_uint64(parser, bus_width->value, &bus) ||
        capacity == 0u || capacity > UINT32_MAX || width == 0u || width > 32u ||
        !(bus == 1u || bus == 4u || bus == 8u || bus == 16u || bus == 32u ||
          bus == 64u) || !only_allowed || family_count != 1u)
        return 0;
    total_bits = capacity * 8u;
    if (total_bits % bus != 0u) return 0;
    addressable_units = total_bits / bus;
    maximum_units = UINT64_C(1) << width;
    return addressable_units <= maximum_units;
}

static void semantic_problem(
    lt48_component_semantic_report *report, const json_parser *parser,
    const component_object *object, size_t object_index, const char *message
) {
    const json_token *id;
    size_t length;
    if (report->first_problem_object_index != SIZE_MAX) return;
    report->first_problem_object_index = object_index;
    id = &parser->tokens[object->id];
    length = id->end - id->start;
    if (length >= sizeof(report->first_problem_object_id))
        length = sizeof(report->first_problem_object_id) - 1u;
    memcpy(report->first_problem_object_id, parser->data + id->start, length);
    report->first_problem_object_id[length] = '\0';
    snprintf(report->diagnostic, sizeof(report->diagnostic), "%s", message);
}

lt48_status lt48_component_bundle_check_semantics(
    const char *path, lt48_component_semantic_report *out_report
) {
    lt48_component_validation_report structural;
    unsigned char *data = NULL;
    size_t size = 0u;
    json_parser parser = {0};
    component_object *objects = NULL;
    size_t *items = NULL;
    size_t object_array;
    size_t documents;
    size_t combinations;
    lt48_status status;
    if (path == NULL || out_report == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->first_problem_object_index = SIZE_MAX;
    out_report->first_problem_combination_index = SIZE_MAX;
    status = lt48_component_bundle_validate(path, &structural);
    if (status != LT48_OK) return status;
    memcpy(out_report->source_sha256, structural.source_sha256,
           LT48_SHA256_SIZE);
    out_report->object_count = structural.object_count;
    if (out_report->object_count == 0u) {
        out_report->blocker_count = 1u;
        snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
                 "bundle contains no component objects");
        return LT48_OK;
    }
    status = read_file(path, &data, &size);
    if (status != LT48_OK) return status;
    parser.data = data;
    parser.size = size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    object_array = object_find(&parser, 0u, "objects");
    documents = object_find(&parser, 0u, "documents");
    combinations = object_find(&parser, 0u, "combinations");
    out_report->combination_count = array_count(&parser, combinations);
    objects = calloc(out_report->object_count ? out_report->object_count : 1u,
                     sizeof(*objects));
    items = calloc(out_report->object_count ? out_report->object_count : 1u,
                   sizeof(*items));
    if (objects == NULL || items == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < out_report->object_count; ++index) {
        size_t item = array_item(&parser, object_array, index);
        items[index] = item;
        objects[index].id = object_find(&parser, item, "id");
        objects[index].kind = object_find(&parser, item, "kind");
        objects[index].extends = object_find(&parser, item, "extends");
        objects[index].references = object_find(&parser, item, "references");
        if (parser.tokens[objects[index].extends].type != JSON_STRING)
            objects[index].extends = SIZE_MAX;
    }
    for (size_t index = 0u; index < out_report->object_count; ++index) {
        property_member *members = NULL;
        size_t count = 0u;
        int valid = 0;
        int implemented = 1;
        if (!effective_properties(&parser, objects, items,
                                  out_report->object_count, index,
                                  &members, &count)) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        if (token_equal(parser.data, &parser.tokens[objects[index].kind],
                        "operation"))
            valid = validate_operation_properties(&parser, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "variable"))
            valid = validate_variable_properties(&parser, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "pin-map"))
            valid = validate_pin_map_properties(&parser, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "algorithm"))
            valid = validate_algorithm_properties(
                &parser, objects, out_report->object_count,
                objects[index].references, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "family"))
            valid = validate_family_properties(
                &parser, objects, out_report->object_count,
                objects[index].references, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "variant"))
            valid = validate_variant_properties(
                &parser, objects, out_report->object_count,
                objects[index].references, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "evidence"))
            valid = validate_evidence_properties(
                &parser, documents, objects[index].references, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "payload"))
            valid = validate_payload_properties(
                &parser, objects, out_report->object_count,
                objects[index].references, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "package"))
            valid = validate_package_properties(
                &parser, objects, out_report->object_count,
                objects[index].references, members, count);
        else if (token_equal(parser.data,
                             &parser.tokens[objects[index].kind], "adapter"))
            valid = validate_adapter_properties(
                &parser, objects, out_report->object_count,
                objects[index].references, members, count);
        else
            implemented = 0;
        free(members);
        if (!implemented) {
            ++out_report->blocker_count;
            semantic_problem(out_report, &parser, &objects[index], index,
                "kind-specific semantic validator is not implemented");
        } else if (!valid) {
            ++out_report->invalid_object_count;
            semantic_problem(out_report, &parser, &objects[index], index,
                "effective properties violate the kind-specific schema");
        } else {
            ++out_report->validated_object_count;
        }
    }
    for (size_t index = 0u; index < out_report->combination_count; ++index) {
        size_t combination = array_item(&parser, combinations, index);
        if (validate_combination_composition(
                &parser, objects, items, out_report->object_count,
                combination)) {
            ++out_report->validated_combination_count;
        } else {
            ++out_report->invalid_combination_count;
            if (out_report->first_problem_object_index == SIZE_MAX &&
                out_report->first_problem_combination_index == SIZE_MAX) {
                out_report->first_problem_combination_index = index;
                snprintf(out_report->diagnostic,
                         sizeof(out_report->diagnostic),
                         "variant/package/adapter composition is inconsistent");
            }
        }
    }
    out_report->complete = out_report->blocker_count == 0u &&
                           out_report->invalid_object_count == 0u &&
                           out_report->invalid_combination_count == 0u;
    if (out_report->complete) {
        snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
                 "all component objects and declared combinations pass semantic schemas");
    }
    status = out_report->invalid_object_count == 0u &&
             out_report->invalid_combination_count == 0u ? LT48_OK :
                                                           LT48_FORMAT_ERROR;
done:
    free(items);
    free(objects);
    free(parser.tokens);
    free(data);
    return status;
}

static int output_size_number(output_buffer *output, size_t value) {
    char text[32];
    int length = snprintf(text, sizeof(text), "%zu", value);
    return length > 0 && (size_t)length < sizeof(text) &&
           output_append(output, text, (size_t)length);
}

static int output_digest(
    output_buffer *output, const unsigned char digest[LT48_SHA256_SIZE]
) {
    static const char hex[] = "0123456789abcdef";
    char text[LT48_SHA256_SIZE * 2u];
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        text[index * 2u] = hex[digest[index] >> 4u];
        text[index * 2u + 1u] = hex[digest[index] & 0x0fu];
    }
    return output_append(output, text, sizeof(text));
}

static int mapped_pin_numbers(
    const json_parser *parser, size_t package_pins, size_t mappings,
    size_t logical_name, uint64_t *out_package_pin,
    uint64_t *out_programmer_pin
) {
    size_t pin_count = array_count(parser, package_pins);
    size_t mapping_count = array_count(parser, mappings);
    uint64_t package_pin = 0u;
    if (pin_count == SIZE_MAX || mapping_count == SIZE_MAX) return 0;
    for (size_t index = 0u; index < pin_count; ++index) {
        size_t pin = array_item(parser, package_pins, index);
        if (token_same(parser, object_find(parser, pin, "name"), logical_name) &&
            token_uint64(parser, object_find(parser, pin, "number"),
                         &package_pin))
            break;
    }
    if (package_pin == 0u) return 0;
    for (size_t index = 0u; index < mapping_count; ++index) {
        size_t mapping = array_item(parser, mappings, index);
        uint64_t mapped_package;
        uint64_t programmer_pin;
        if (token_uint64(parser, object_find(parser, mapping, "package_pin"),
                         &mapped_package) && mapped_package == package_pin &&
            token_uint64(parser,
                         object_find(parser, mapping, "programmer_pin"),
                         &programmer_pin)) {
            *out_package_pin = package_pin;
            *out_programmer_pin = programmer_pin;
            return 1;
        }
    }
    return 0;
}

static int resolve_rail_centivolts(
    const json_parser *parser, const component_object *objects,
    const size_t *items, size_t object_count, size_t algorithm,
    size_t rail, size_t state, uint64_t *out_centivolts
) {
    size_t reference_count;
    if (token_equal(parser->data, &parser->tokens[state], "off")) {
        *out_centivolts = 0u;
        return 1;
    }
    reference_count = array_count(parser, objects[algorithm].references);
    for (size_t ref = 0u; ref < reference_count; ++ref) {
        size_t variable = find_object(
            parser, objects, object_count,
            array_item(parser, objects[algorithm].references, ref));
        property_member *members = NULL;
        size_t count = 0u;
        property_member *bound_rail;
        property_member *unit;
        property_member *selected;
        int matched = 0;
        if (variable == SIZE_MAX || !token_equal(
                parser->data, &parser->tokens[objects[variable].kind],
                "variable"))
            continue;
        if (!effective_properties(parser, objects, items, object_count,
                                  variable, &members, &count))
            return 0;
        bound_rail = member_find(parser, members, count, "rail");
        unit = member_find(parser, members, count, "unit");
        if (bound_rail != NULL && unit != NULL &&
            token_same(parser, bound_rail->value, rail) &&
            token_equal(parser->data, &parser->tokens[unit->value],
                        "centivolt")) {
            if (token_equal(parser->data, &parser->tokens[state], "verify-low"))
                selected = member_find(parser, members, count, "minimum");
            else if (token_equal(parser->data, &parser->tokens[state],
                                 "verify-high"))
                selected = member_find(parser, members, count, "maximum");
            else
                selected = member_find(parser, members, count, "nominal");
            matched = selected != NULL &&
                      token_uint64(parser, selected->value, out_centivolts);
        }
        free(members);
        if (matched) return 1;
    }
    return 0;
}

static size_t find_algorithm_payload(
    const json_parser *parser, const component_object *objects,
    size_t object_count, size_t algorithm, size_t payload_id
) {
    size_t reference_count = array_count(parser, objects[algorithm].references);
    for (size_t ref = 0u; ref < reference_count; ++ref) {
        size_t id = array_item(parser, objects[algorithm].references, ref);
        size_t target = find_object(parser, objects, object_count, id);
        if (target != SIZE_MAX && token_same(parser, id, payload_id) &&
            token_equal(parser->data, &parser->tokens[objects[target].kind],
                        "payload"))
            return target;
    }
    return SIZE_MAX;
}

static void sort_property_members(
    const json_parser *parser, property_member *members, size_t count
) {
    for (size_t index = 1u; index < count; ++index) {
        property_member current = members[index];
        size_t position = index;
        while (position != 0u && member_key_compare(
                parser, &current, &members[position - 1u]) < 0) {
            members[position] = members[position - 1u];
            --position;
        }
        members[position] = current;
    }
}

static int output_phase_payload(
    const json_parser *parser, const component_object *objects,
    const size_t *items, size_t object_count, size_t algorithm,
    size_t payload_id, output_buffer *output
) {
    size_t payload;
    property_member *members = NULL;
    size_t count = 0u;
    int valid;
    if (token_equal(parser->data, &parser->tokens[payload_id], "null"))
        return output_literal(output, "null");
    payload = find_algorithm_payload(
        parser, objects, object_count, algorithm, payload_id);
    if (payload == SIZE_MAX ||
        !effective_properties(parser, objects, items, object_count, payload,
                              &members, &count))
        return 0;
    valid = output_literal(output, "{\"id\":") &&
            canonical_token(parser, objects[payload].id, output) &&
            output_literal(output, ",\"declared_sha256\":") &&
            canonical_token(parser,
                object_find(parser, items[payload], "sha256"), output) &&
            output_literal(output, ",\"origin\":") &&
            canonical_token(parser,
                member_find(parser, members, count, "origin")->value, output) &&
            output_literal(output, ",\"payload_type\":") &&
            canonical_token(parser,
                member_find(parser, members, count, "payload_type")->value,
                output) &&
            output_literal(output, ",\"format\":") &&
            canonical_token(parser,
                member_find(parser, members, count, "format")->value, output) &&
            output_literal(output, ",\"abi\":") &&
            canonical_token(parser,
                member_find(parser, members, count, "abi")->value, output) &&
            output_literal(output, "}");
    free(members);
    return valid;
}

static int component_phase_kind_from_token(
    const json_parser *parser, size_t token, lt48_component_phase_kind *out
) {
    static const struct {
        const char *name;
        lt48_component_phase_kind value;
    } values[] = {
        {"prepare", LT48_COMPONENT_PHASE_PREPARE},
        {"precheck", LT48_COMPONENT_PHASE_PRECHECK},
        {"power", LT48_COMPONENT_PHASE_POWER},
        {"initialize", LT48_COMPONENT_PHASE_INITIALIZE},
        {"transfer", LT48_COMPONENT_PHASE_TRANSFER},
        {"verify", LT48_COMPONENT_PHASE_VERIFY},
        {"delay", LT48_COMPONENT_PHASE_DELAY},
        {"result", LT48_COMPONENT_PHASE_RESULT},
        {"cleanup", LT48_COMPONENT_PHASE_CLEANUP}
    };
    for (size_t index = 0u; index < sizeof(values) / sizeof(values[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token],
                        values[index].name)) {
            *out = values[index].value;
            return 1;
        }
    return 0;
}

static int component_rail_state_from_token(
    const json_parser *parser, size_t token, lt48_component_rail_state *out
) {
    static const struct {
        const char *name;
        lt48_component_rail_state value;
    } values[] = {
        {"off", LT48_COMPONENT_RAIL_OFF},
        {"nominal", LT48_COMPONENT_RAIL_NOMINAL},
        {"programming", LT48_COMPONENT_RAIL_PROGRAMMING},
        {"verify-low", LT48_COMPONENT_RAIL_VERIFY_LOW},
        {"verify-high", LT48_COMPONENT_RAIL_VERIFY_HIGH}
    };
    for (size_t index = 0u; index < sizeof(values) / sizeof(values[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token],
                        values[index].name)) {
            *out = values[index].value;
            return 1;
        }
    return 0;
}

static int component_pin_state_from_token(
    const json_parser *parser, size_t token, lt48_component_pin_state *out
) {
    static const struct {
        const char *name;
        lt48_component_pin_state value;
    } values[] = {
        {"low", LT48_COMPONENT_PIN_LOW},
        {"high", LT48_COMPONENT_PIN_HIGH},
        {"input", LT48_COMPONENT_PIN_INPUT},
        {"output", LT48_COMPONENT_PIN_OUTPUT},
        {"bidirectional", LT48_COMPONENT_PIN_BIDIRECTIONAL},
        {"high-impedance", LT48_COMPONENT_PIN_HIGH_IMPEDANCE}
    };
    for (size_t index = 0u; index < sizeof(values) / sizeof(values[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[token],
                        values[index].name)) {
            *out = values[index].value;
            return 1;
        }
    return 0;
}

static int component_transition_from_token(
    const json_parser *parser, size_t phase_array, size_t token,
    lt48_component_plan_transition *out
) {
    size_t count;
    if (token_equal(parser->data, &parser->tokens[token], "complete")) {
        out->kind = LT48_COMPONENT_TRANSITION_COMPLETE;
        out->phase_index = SIZE_MAX;
        return 1;
    }
    if (token_equal(parser->data, &parser->tokens[token], "error")) {
        out->kind = LT48_COMPONENT_TRANSITION_ERROR;
        out->phase_index = SIZE_MAX;
        return 1;
    }
    if (token_equal(parser->data, &parser->tokens[token], "abort")) {
        out->kind = LT48_COMPONENT_TRANSITION_ABORT;
        out->phase_index = SIZE_MAX;
        return 1;
    }
    count = array_count(parser, phase_array);
    for (size_t index = 0u; index < count; ++index) {
        size_t phase = array_item(parser, phase_array, index);
        if (token_same(parser, object_find(parser, phase, "id"), token)) {
            out->kind = LT48_COMPONENT_TRANSITION_PHASE;
            out->phase_index = index;
            return 1;
        }
    }
    return 0;
}

static int populate_component_payload(
    const json_parser *parser, const component_object *objects,
    const size_t *items, size_t object_count, size_t algorithm,
    size_t payload_id, lt48_component_plan_payload *out
) {
    size_t payload;
    property_member *members = NULL;
    size_t count = 0u;
    int valid;
    memset(out, 0, sizeof(*out));
    if (token_equal(parser->data, &parser->tokens[payload_id], "null"))
        return 1;
    payload = find_algorithm_payload(
        parser, objects, object_count, algorithm, payload_id);
    if (payload == SIZE_MAX ||
        !effective_properties(parser, objects, items, object_count, payload,
                              &members, &count))
        return 0;
    valid = token_text(parser, objects[payload].id, out->id,
                       sizeof(out->id)) &&
            token_text(parser, object_find(parser, items[payload], "sha256"),
                       out->declared_sha256,
                       sizeof(out->declared_sha256)) &&
            token_text(parser, member_find(parser, members, count,
                                           "origin")->value,
                       out->origin, sizeof(out->origin)) &&
            token_text(parser, member_find(parser, members, count,
                                           "payload_type")->value,
                       out->payload_type, sizeof(out->payload_type)) &&
            token_text(parser, member_find(parser, members, count,
                                           "format")->value,
                       out->format, sizeof(out->format)) &&
            token_text(parser, member_find(parser, members, count,
                                           "abi")->value,
                       out->abi, sizeof(out->abi));
    free(members);
    if (valid) out->present = 1;
    return valid;
}

void lt48_component_plan_clear(lt48_component_plan *plan) {
    if (plan == NULL) return;
    for (size_t index = 0u; index < plan->phase_count; ++index) {
        free(plan->phases[index].rails);
        free(plan->phases[index].pins);
    }
    free(plan->phases);
    free(plan->json);
    memset(plan, 0, sizeof(*plan));
}

lt48_status lt48_component_bundle_compile_plan(
    const char *path, size_t combination_index, const char *operation_name,
    lt48_component_plan *out_plan
) {
    lt48_component_semantic_report semantics;
    unsigned char *data = NULL;
    size_t size = 0u;
    json_parser parser = {0};
    component_object *objects = NULL;
    size_t *items = NULL;
    property_member *operation_members = NULL;
    property_member *package_members = NULL;
    property_member *adapter_members = NULL;
    size_t operation_member_count = 0u;
    size_t package_member_count = 0u;
    size_t adapter_member_count = 0u;
    size_t object_array;
    size_t object_count;
    size_t combinations;
    size_t combination;
    size_t variant;
    size_t package;
    size_t adapter;
    size_t family;
    size_t algorithm;
    size_t operation = SIZE_MAX;
    property_member *phases;
    property_member *package_pins;
    property_member *mappings;
    size_t phase_count;
    output_buffer output = {0};
    lt48_status status;
    if (path == NULL || operation_name == NULL || operation_name[0] == '\0' ||
        out_plan == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_plan, 0, sizeof(*out_plan));
    status = lt48_component_bundle_check_semantics(path, &semantics);
    if (status != LT48_OK) return status;
    if (!semantics.complete) return LT48_UNSUPPORTED;
    status = read_file(path, &data, &size);
    if (status != LT48_OK) return status;
    parser.data = data;
    parser.size = size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    object_array = object_find(&parser, 0u, "objects");
    combinations = object_find(&parser, 0u, "combinations");
    object_count = array_count(&parser, object_array);
    if (combination_index >= array_count(&parser, combinations)) {
        status = LT48_NOT_FOUND;
        goto done;
    }
    objects = calloc(object_count ? object_count : 1u, sizeof(*objects));
    items = calloc(object_count ? object_count : 1u, sizeof(*items));
    if (objects == NULL || items == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < object_count; ++index) {
        size_t item = array_item(&parser, object_array, index);
        items[index] = item;
        objects[index].id = object_find(&parser, item, "id");
        objects[index].kind = object_find(&parser, item, "kind");
        objects[index].extends = object_find(&parser, item, "extends");
        objects[index].references = object_find(&parser, item, "references");
        if (parser.tokens[objects[index].extends].type != JSON_STRING)
            objects[index].extends = SIZE_MAX;
    }
    combination = array_item(&parser, combinations, combination_index);
    variant = find_object(&parser, objects, object_count,
                          object_find(&parser, combination, "variant"));
    package = find_object(&parser, objects, object_count,
                          object_find(&parser, combination, "package"));
    adapter = find_object(&parser, objects, object_count,
                          object_find(&parser, combination, "adapter"));
    family = referenced_object_of_kind(
        &parser, objects, object_count, objects[variant].references, "family");
    algorithm = referenced_object_of_kind(
        &parser, objects, object_count, objects[family].references, "algorithm");
    {
        size_t reference_count = array_count(&parser,
                                             objects[algorithm].references);
        for (size_t ref = 0u; ref < reference_count; ++ref) {
            size_t candidate = find_object(
                &parser, objects, object_count,
                array_item(&parser, objects[algorithm].references, ref));
            property_member *candidate_members = NULL;
            size_t candidate_count = 0u;
            property_member *name;
            if (candidate == SIZE_MAX ||
                !token_equal(parser.data,
                    &parser.tokens[objects[candidate].kind], "operation"))
                continue;
            if (!effective_properties(&parser, objects, items, object_count,
                                      candidate, &candidate_members,
                                      &candidate_count)) {
                status = LT48_NO_MEMORY;
                goto done;
            }
            name = member_find(&parser, candidate_members, candidate_count,
                               "name");
            if (name != NULL && token_equal(parser.data,
                    &parser.tokens[name->value], operation_name)) {
                operation = candidate;
                operation_members = candidate_members;
                operation_member_count = candidate_count;
                break;
            }
            free(candidate_members);
        }
    }
    if (operation == SIZE_MAX) {
        status = LT48_NOT_FOUND;
        goto done;
    }
    if (!effective_properties(&parser, objects, items, object_count, package,
                              &package_members, &package_member_count) ||
        !effective_properties(&parser, objects, items, object_count, adapter,
                              &adapter_members, &adapter_member_count)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    phases = member_find(&parser, operation_members, operation_member_count,
                         "phases");
    package_pins = member_find(&parser, package_members, package_member_count,
                               "pins");
    mappings = member_find(&parser, adapter_members, adapter_member_count,
                           "mappings");
    phase_count = array_count(&parser, phases->value);
    out_plan->phases = calloc(phase_count ? phase_count : 1u,
                              sizeof(*out_plan->phases));
    out_plan->phase_count = phase_count;
    if (out_plan->phases == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_plan->entry_phase_index = SIZE_MAX;
    out_plan->cleanup_phase_index = SIZE_MAX;
    for (size_t index = 0u; index < phase_count; ++index) {
        size_t phase = array_item(&parser, phases->value, index);
        if (token_same(&parser, object_find(&parser, phase, "id"),
                       member_find(&parser, operation_members,
                                   operation_member_count,
                                   "entry_phase")->value))
            out_plan->entry_phase_index = index;
        if (token_same(&parser, object_find(&parser, phase, "id"),
                       member_find(&parser, operation_members,
                                   operation_member_count,
                                   "cleanup_phase")->value))
            out_plan->cleanup_phase_index = index;
    }
    {
        size_t protocol = member_find(
            &parser, operation_members, operation_member_count,
            "result_protocol")->value;
        if (out_plan->entry_phase_index == SIZE_MAX ||
            out_plan->cleanup_phase_index == SIZE_MAX ||
            !token_text(&parser, object_find(&parser, protocol, "completion"),
                        out_plan->completion_protocol,
                        sizeof(out_plan->completion_protocol)) ||
            !token_text(&parser, object_find(&parser, protocol, "error"),
                        out_plan->error_protocol,
                        sizeof(out_plan->error_protocol))) {
            status = LT48_COMPILE_ERROR;
            goto done;
        }
    }
    if (!output_literal(&output,
            "{\"schema_version\":1,\"bundle_sha256\":\"") ||
        !output_digest(&output, semantics.source_sha256) ||
        !output_literal(&output, "\",\"combination_index\":") ||
        !output_size_number(&output, combination_index) ||
        !output_literal(&output, ",\"variant\":") ||
        !canonical_token(&parser, objects[variant].id, &output) ||
        !output_literal(&output, ",\"package\":") ||
        !canonical_token(&parser, objects[package].id, &output) ||
        !output_literal(&output, ",\"adapter\":") ||
        !canonical_token(&parser, objects[adapter].id, &output) ||
        !output_literal(&output, ",\"operation_id\":") ||
        !canonical_token(&parser, objects[operation].id, &output) ||
        !output_literal(&output, ",\"operation\":") ||
        !canonical_token(&parser,
            member_find(&parser, operation_members, operation_member_count,
                        "name")->value, &output) ||
        !output_literal(&output, ",\"mutating\":") ||
        !canonical_token(&parser,
            member_find(&parser, operation_members, operation_member_count,
                        "mutating")->value, &output) ||
        !output_literal(&output, ",\"support_level\":") ||
        !canonical_token(&parser,
            member_find(&parser, operation_members, operation_member_count,
                        "support_level")->value, &output) ||
        !output_literal(&output, ",\"entry_phase\":") ||
        !canonical_token(&parser,
            member_find(&parser, operation_members, operation_member_count,
                        "entry_phase")->value, &output) ||
        !output_literal(&output, ",\"cleanup_phase\":") ||
        !canonical_token(&parser,
            member_find(&parser, operation_members, operation_member_count,
                        "cleanup_phase")->value, &output) ||
        !output_literal(&output, ",\"result_protocol\":") ||
        !canonical_token(&parser,
            member_find(&parser, operation_members, operation_member_count,
                        "result_protocol")->value, &output) ||
        !output_literal(&output, ",\"phases\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t phase_index = 0u; phase_index < phase_count; ++phase_index) {
        size_t phase = array_item(&parser, phases->value, phase_index);
        lt48_component_plan_phase *typed = &out_plan->phases[phase_index];
        property_member *pin_states = NULL;
        property_member *rail_states = NULL;
        size_t pin_state_count = 0u;
        size_t rail_state_count = 0u;
        size_t rails = object_find(&parser, phase, "rails");
        size_t pins = object_find(&parser, phase, "pins");
        uint64_t timeout;
        uint64_t retries;
        if (!collect_object_properties(&parser, pins, &pin_states,
                                       &pin_state_count) ||
            !collect_object_properties(&parser, rails, &rail_states,
                                       &rail_state_count)) {
            free(rail_states);
            free(pin_states);
            status = LT48_NO_MEMORY;
            goto done;
        }
        sort_property_members(&parser, pin_states, pin_state_count);
        sort_property_members(&parser, rail_states, rail_state_count);
        if (!token_text(&parser, object_find(&parser, phase, "id"),
                        typed->id, sizeof(typed->id)) ||
            !component_phase_kind_from_token(
                &parser, object_find(&parser, phase, "kind"), &typed->kind) ||
            !token_uint64(&parser, object_find(&parser, phase, "timeout_ms"),
                          &timeout) || timeout > UINT32_MAX ||
            !token_uint64(&parser, object_find(&parser, phase, "retry_limit"),
                          &retries) || retries > UINT32_MAX) {
            free(rail_states);
            free(pin_states);
            status = LT48_COMPILE_ERROR;
            goto done;
        }
        typed->timeout_ms = (uint32_t)timeout;
        typed->retry_limit = (uint32_t)retries;
        typed->rail_count = rail_state_count;
        typed->pin_count = pin_state_count;
        if (rail_state_count != 0u) {
            typed->rails = calloc(rail_state_count, sizeof(*typed->rails));
            if (typed->rails == NULL) {
                free(rail_states);
                free(pin_states);
                status = LT48_NO_MEMORY;
                goto done;
            }
        }
        if (pin_state_count != 0u) {
            typed->pins = calloc(pin_state_count, sizeof(*typed->pins));
            if (typed->pins == NULL) {
                free(rail_states);
                free(pin_states);
                status = LT48_NO_MEMORY;
                goto done;
            }
        }
        if ((phase_index != 0u && !output_literal(&output, ",")) ||
            !output_literal(&output, "{\"id\":") ||
            !canonical_token(&parser, object_find(&parser, phase, "id"),
                             &output) ||
            !output_literal(&output, ",\"kind\":") ||
            !canonical_token(&parser, object_find(&parser, phase, "kind"),
                             &output) ||
            !output_literal(&output, ",\"timeout_ms\":") ||
            !canonical_token(&parser,
                             object_find(&parser, phase, "timeout_ms"),
                             &output) ||
            !output_literal(&output, ",\"retry_limit\":") ||
            !canonical_token(&parser,
                             object_find(&parser, phase, "retry_limit"),
                             &output) ||
            !output_literal(&output, ",\"rails\":[")) {
            free(rail_states);
            free(pin_states);
            status = LT48_NO_MEMORY;
            goto done;
        }
        for (size_t rail_index = 0u; rail_index < rail_state_count;
             ++rail_index) {
            uint64_t centivolts;
            if (!resolve_rail_centivolts(
                    &parser, objects, items, object_count, algorithm,
                    rail_states[rail_index].key,
                    rail_states[rail_index].value, &centivolts) ||
                centivolts > UINT32_MAX ||
                !token_text(&parser, rail_states[rail_index].key,
                            typed->rails[rail_index].name,
                            sizeof(typed->rails[rail_index].name)) ||
                !component_rail_state_from_token(
                    &parser, rail_states[rail_index].value,
                    &typed->rails[rail_index].state) ||
                (rail_index != 0u && !output_literal(&output, ",")) ||
                !output_literal(&output, "{\"rail\":") ||
                !canonical_token(&parser, rail_states[rail_index].key,
                                 &output) ||
                !output_literal(&output, ",\"state\":") ||
                !canonical_token(&parser, rail_states[rail_index].value,
                                 &output) ||
                !output_literal(&output, ",\"centivolts\":") ||
                !output_size_number(&output, (size_t)centivolts) ||
                !output_literal(&output, "}")) {
                free(rail_states);
                free(pin_states);
                status = LT48_COMPILE_ERROR;
                goto done;
            }
            typed->rails[rail_index].centivolts = (uint32_t)centivolts;
        }
        out_plan->rail_state_count += rail_state_count;
        free(rail_states);
        if (!output_literal(&output, "],\"mapped_pins\":[")) {
            free(pin_states);
            status = LT48_NO_MEMORY;
            goto done;
        }
        for (size_t pin_index = 0u; pin_index < pin_state_count; ++pin_index) {
            uint64_t package_pin;
            uint64_t programmer_pin;
            if (!mapped_pin_numbers(&parser, package_pins->value,
                                    mappings->value, pin_states[pin_index].key,
                                    &package_pin, &programmer_pin) ||
                package_pin > UINT32_MAX || programmer_pin > UINT32_MAX ||
                !token_text(&parser, pin_states[pin_index].key,
                            typed->pins[pin_index].logical,
                            sizeof(typed->pins[pin_index].logical)) ||
                !component_pin_state_from_token(
                    &parser, pin_states[pin_index].value,
                    &typed->pins[pin_index].state) ||
                (pin_index != 0u && !output_literal(&output, ",")) ||
                !output_literal(&output, "{\"logical\":") ||
                !canonical_token(&parser, pin_states[pin_index].key, &output) ||
                !output_literal(&output, ",\"package_pin\":") ||
                !output_size_number(&output, (size_t)package_pin) ||
                !output_literal(&output, ",\"programmer_pin\":") ||
                !output_size_number(&output, (size_t)programmer_pin) ||
                !output_literal(&output, ",\"state\":") ||
                !canonical_token(&parser, pin_states[pin_index].value,
                                 &output) ||
                !output_literal(&output, "}")) {
                free(pin_states);
                status = LT48_COMPILE_ERROR;
                goto done;
            }
            typed->pins[pin_index].package_pin = (uint32_t)package_pin;
            typed->pins[pin_index].programmer_pin = (uint32_t)programmer_pin;
            ++out_plan->mapped_pin_state_count;
        }
        free(pin_states);
        if (!populate_component_payload(
                &parser, objects, items, object_count, algorithm,
                object_find(&parser, phase, "payload"), &typed->payload) ||
            !component_transition_from_token(
                &parser, phases->value,
                object_find(&parser, phase, "on_success"),
                &typed->on_success) ||
            !component_transition_from_token(
                &parser, phases->value,
                object_find(&parser, phase, "on_failure"),
                &typed->on_failure) ||
            !output_literal(&output, "],\"payload\":") ||
            !output_phase_payload(
                &parser, objects, items, object_count, algorithm,
                object_find(&parser, phase, "payload"), &output) ||
            !output_literal(&output, ",\"on_success\":") ||
            !canonical_token(&parser,
                             object_find(&parser, phase, "on_success"),
                             &output) ||
            !output_literal(&output, ",\"on_failure\":") ||
            !canonical_token(&parser,
                             object_find(&parser, phase, "on_failure"),
                             &output) ||
            !output_literal(&output, "}")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
    }
    if (!output_literal(&output, "]}" ) ||
        !output_append(&output, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_plan->json = output.data;
    out_plan->json_size = output.size - 1u;
    lt48_sha256(output.data, output.size - 1u, out_plan->sha256);
    out_plan->combination_index = combination_index;
    {
        int mutating_value = 0;
        (void)token_bool(&parser,
            member_find(&parser, operation_members, operation_member_count,
                        "mutating")->value, &mutating_value);
        out_plan->mutating = mutating_value;
    }
    {
        const json_token *id = &parser.tokens[objects[operation].id];
        size_t length = id->end - id->start;
        if (length >= sizeof(out_plan->operation_id))
            length = sizeof(out_plan->operation_id) - 1u;
        memcpy(out_plan->operation_id, parser.data + id->start, length);
        out_plan->operation_id[length] = '\0';
    }
    output.data = NULL;
    status = LT48_OK;
done:
    if (status != LT48_OK) lt48_component_plan_clear(out_plan);
    free(output.data);
    free(adapter_members);
    free(package_members);
    free(operation_members);
    free(items);
    free(objects);
    free(parser.tokens);
    free(data);
    return status;
}

void lt48_component_plan_matrix_clear(lt48_component_plan_matrix *matrix) {
    if (matrix == NULL) return;
    free(matrix->json);
    memset(matrix, 0, sizeof(*matrix));
}

lt48_status lt48_component_bundle_compile_plan_matrix_cancellable(
    const char *path, lt48_cancel_check_fn cancelled,
    void *cancel_context, lt48_component_plan_matrix *out_matrix
) {
    lt48_component_semantic_report semantics;
    output_buffer output = {0};
    lt48_status status;
    size_t plan_count = 0u;
    size_t mutating_count = 0u;
    if (path == NULL || out_matrix == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_matrix, 0, sizeof(*out_matrix));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_bundle_check_semantics(path, &semantics);
    if (status != LT48_OK) return status;
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    if (!semantics.complete) return LT48_UNSUPPORTED;
    if (!output_literal(&output,
            "{\"schema_version\":1,\"bundle_sha256\":\"") ||
        !output_digest(&output, semantics.source_sha256) ||
        !output_literal(&output, "\",\"combination_count\":") ||
        !output_size_number(&output, semantics.combination_count) ||
        !output_literal(&output, ",\"transport_ready\":false,\"plans\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t combination = 0u;
         combination < semantics.combination_count; ++combination) {
        for (unsigned value = 1u; value <= LT48_OPERATION_KIND_COUNT;
             ++value) {
            const char *operation = lt48_operation_kind_name(
                (lt48_operation_kind)value);
            lt48_component_plan plan;
            if (cancelled != NULL && cancelled(cancel_context)) {
                status = LT48_CANCELLED;
                goto done;
            }
            status = lt48_component_bundle_compile_plan(
                path, combination, operation, &plan);
            if (status == LT48_NOT_FOUND) continue;
            if (status != LT48_OK) goto done;
            if ((plan_count != 0u && !output_literal(&output, ",")) ||
                !output_literal(&output, "{\"combination_index\":") ||
                !output_size_number(&output, combination) ||
                !output_literal(&output, ",\"operation\":") ||
                !output_json_string(&output, operation) ||
                !output_literal(&output, ",\"operation_id\":") ||
                !output_json_string(&output, plan.operation_id) ||
                !output_literal(&output, ",\"mutating\":") ||
                !output_literal(&output, plan.mutating ? "true" : "false") ||
                !output_literal(&output, ",\"phase_count\":") ||
                !output_size_number(&output, plan.phase_count) ||
                !output_literal(&output, ",\"entry_phase_index\":") ||
                !output_size_number(&output, plan.entry_phase_index) ||
                !output_literal(&output, ",\"cleanup_phase_index\":") ||
                !output_size_number(&output, plan.cleanup_phase_index) ||
                !output_literal(&output, ",\"plan_sha256\":\"") ||
                !output_digest(&output, plan.sha256) ||
                !output_literal(&output, "\",\"plan\":") ||
                !output_append(&output, plan.json, plan.json_size) ||
                !output_literal(&output, "}")) {
                lt48_component_plan_clear(&plan);
                status = LT48_NO_MEMORY;
                goto done;
            }
            if (plan.mutating) ++mutating_count;
            ++plan_count;
            lt48_component_plan_clear(&plan);
        }
    }
    if (plan_count == 0u) {
        status = LT48_COMPILE_ERROR;
        goto done;
    }
    if (!output_literal(&output, "],\"plan_count\":") ||
        !output_size_number(&output, plan_count) ||
        !output_literal(&output, ",\"mutating_plan_count\":") ||
        !output_size_number(&output, mutating_count) ||
        !output_literal(&output, "}") ||
        !output_append(&output, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_matrix->json = output.data;
    out_matrix->json_size = output.size - 1u;
    memcpy(out_matrix->bundle_sha256, semantics.source_sha256,
           LT48_SHA256_SIZE);
    lt48_sha256(output.data, output.size - 1u, out_matrix->sha256);
    out_matrix->combination_count = semantics.combination_count;
    out_matrix->plan_count = plan_count;
    out_matrix->mutating_plan_count = mutating_count;
    output.data = NULL;
    status = LT48_OK;
done:
    free(output.data);
    if (status != LT48_OK) lt48_component_plan_matrix_clear(out_matrix);
    return status;
}

lt48_status lt48_component_bundle_compile_plan_matrix(
    const char *path, lt48_component_plan_matrix *out_matrix
) {
    return lt48_component_bundle_compile_plan_matrix_cancellable(
        path, NULL, NULL, out_matrix);
}

lt48_status lt48_component_bundle_validate_all_variants_cancellable(
    const char *path, lt48_cancel_check_fn cancelled,
    void *cancel_context, lt48_component_all_variants_report *out_report
) {
    lt48_component_plan_matrix matrix;
    lt48_status status;
    if (path == NULL || out_report == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_bundle_validate(path, &out_report->structure);
    if (status != LT48_OK) return status;
    out_report->structure_valid = 1;
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_bundle_check_semantics(
        path, &out_report->semantics);
    if (status != LT48_OK) return status;
    out_report->semantics_complete = out_report->semantics.complete;
    if (!out_report->semantics_complete) return LT48_UNSUPPORTED;
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_bundle_compile_plan_matrix_cancellable(
        path, cancelled, cancel_context, &matrix);
    if (status != LT48_OK) return status;
    memcpy(out_report->matrix_sha256, matrix.sha256, LT48_SHA256_SIZE);
    out_report->plan_count = matrix.plan_count;
    out_report->mutating_plan_count = matrix.mutating_plan_count;
    out_report->matrix_complete =
        matrix.combination_count == out_report->semantics.combination_count;
    out_report->complete = out_report->structure_valid &&
        out_report->semantics_complete && out_report->matrix_complete;
    lt48_component_plan_matrix_clear(&matrix);
    return out_report->complete ? LT48_OK : LT48_COMPILE_ERROR;
}

lt48_status lt48_component_bundle_validate_all_variants(
    const char *path, lt48_component_all_variants_report *out_report
) {
    return lt48_component_bundle_validate_all_variants_cancellable(
        path, NULL, NULL, out_report);
}

void lt48_component_adapter_test_plan_clear(
    lt48_component_adapter_test_plan *plan
) {
    if (plan == NULL) return;
    free(plan->json);
    memset(plan, 0, sizeof(*plan));
}

lt48_status lt48_component_bundle_adapter_test_plan(
    const char *path, const char *adapter_id,
    lt48_component_adapter_test_plan *out_plan
) {
    lt48_component_semantic_report semantics;
    lt48_component_resolution resolution;
    json_parser parser = {0};
    output_buffer output = {0};
    size_t properties;
    size_t mappings;
    size_t mapping_count;
    int active = 0;
    lt48_status status;
    if (path == NULL || adapter_id == NULL || adapter_id[0] == '\0' ||
        out_plan == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_plan, 0, sizeof(*out_plan));
    status = lt48_component_bundle_check_semantics(path, &semantics);
    if (status != LT48_OK) return status;
    if (!semantics.complete) return LT48_UNSUPPORTED;
    status = lt48_component_bundle_resolve(path, adapter_id, &resolution);
    if (status != LT48_OK) return status;
    parser.data = resolution.effective_json;
    parser.size = resolution.effective_json_size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    if (!token_equal(parser.data,
                     &parser.tokens[object_find(&parser, 0u, "kind")],
                     "adapter")) {
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    properties = object_find(&parser, 0u, "properties");
    mappings = object_find(&parser, properties, "mappings");
    mapping_count = array_count(&parser, mappings);
    if (mapping_count == SIZE_MAX ||
        !token_bool(&parser,
                    object_find(&parser, properties, "active_components"),
                    &active)) {
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    if (!output_literal(&output,
            "{\"schema_version\":1,\"adapter_id\":") ||
        !output_json_string(&output, adapter_id) ||
        !output_literal(&output, ",\"topology\":") ||
        !canonical_token(&parser, object_find(&parser, properties, "topology"),
                         &output) ||
        !output_literal(&output, ",\"assembly_guidance\":{"
            "\"orientation\":") ||
        !canonical_token(&parser,
                         object_find(&parser, properties, "orientation"),
                         &output) ||
        !output_literal(&output, ",\"safe_state\":") ||
        !canonical_token(&parser,
                         object_find(&parser, properties, "safe_state"),
                         &output) ||
        !output_literal(&output, ",\"maximum_voltage_millivolts\":") ||
        !canonical_token(&parser, object_find(
            &parser, properties, "maximum_voltage_millivolts"), &output) ||
        !output_literal(&output,
            ",\"test_with_programmer_and_device_unpowered\":true,"
            "\"active_adapter_protocol_review_required\":") ||
        !output_literal(&output, active ? "true" : "false") ||
        !output_literal(&output, "},\"continuity_tests\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < mapping_count; ++index) {
        size_t mapping = array_item(&parser, mappings, index);
        if ((index != 0u && !output_literal(&output, ",")) ||
            !output_literal(&output, "{\"programmer_pin\":") ||
            !canonical_token(&parser, object_find(
                &parser, mapping, "programmer_pin"), &output) ||
            !output_literal(&output, ",\"net\":") ||
            !canonical_token(&parser, object_find(&parser, mapping, "net"),
                             &output) ||
            !output_literal(&output, ",\"package_pin\":") ||
            !canonical_token(&parser, object_find(
                &parser, mapping, "package_pin"), &output) ||
            !output_literal(&output,
                ",\"expectation\":\"continuity\"}")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        ++out_plan->continuity_test_count;
    }
    if (!output_literal(&output, "],\"isolation_tests\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t left = 0u; left < mapping_count; ++left) {
        size_t left_mapping = array_item(&parser, mappings, left);
        for (size_t right = left + 1u; right < mapping_count; ++right) {
            size_t right_mapping = array_item(&parser, mappings, right);
            size_t left_net = object_find(&parser, left_mapping, "net");
            size_t right_net = object_find(&parser, right_mapping, "net");
            if (token_same(&parser, left_net, right_net)) continue;
            if ((out_plan->isolation_test_count != 0u &&
                 !output_literal(&output, ",")) ||
                !output_literal(&output, "{\"left_net\":") ||
                !canonical_token(&parser, left_net, &output) ||
                !output_literal(&output, ",\"left_programmer_pin\":") ||
                !canonical_token(&parser, object_find(
                    &parser, left_mapping, "programmer_pin"), &output) ||
                !output_literal(&output, ",\"right_net\":") ||
                !canonical_token(&parser, right_net, &output) ||
                !output_literal(&output, ",\"right_programmer_pin\":") ||
                !canonical_token(&parser, object_find(
                    &parser, right_mapping, "programmer_pin"), &output) ||
                !output_literal(&output,
                    ",\"expectation\":\"isolated\"}")) {
                status = LT48_NO_MEMORY;
                goto done;
            }
            ++out_plan->isolation_test_count;
        }
    }
    if (!output_literal(&output, "],\"active_components\":") ||
        !output_literal(&output, active ? "true" : "false") ||
        !output_literal(&output, ",\"automatic_continuity_ready\":") ||
        !output_literal(&output, active ? "false" : "true") ||
        !output_literal(&output, "}") ||
        !output_append(&output, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_plan->json = output.data;
    out_plan->json_size = output.size - 1u;
    lt48_sha256(output.data, output.size - 1u, out_plan->sha256);
    out_plan->active_components = active;
    out_plan->automatic_continuity_ready = !active;
    output.data = NULL;
    status = LT48_OK;
done:
    free(output.data);
    free(parser.tokens);
    lt48_component_resolution_clear(&resolution);
    if (status != LT48_OK) lt48_component_adapter_test_plan_clear(out_plan);
    return status;
}

void lt48_component_payload_build_plan_clear(
    lt48_component_payload_build_plan *plan
) {
    if (plan == NULL) return;
    free(plan->json);
    memset(plan, 0, sizeof(*plan));
}

lt48_status lt48_component_bundle_plan_payload_builds(
    const char *path, lt48_component_payload_build_plan *out_plan
) {
    lt48_component_semantic_report semantics;
    unsigned char *data = NULL;
    size_t size = 0u;
    json_parser parser = {0};
    component_object *objects = NULL;
    size_t *items = NULL;
    size_t *payloads = NULL;
    size_t object_array;
    size_t object_count;
    size_t payload_count = 0u;
    output_buffer output = {0};
    lt48_status status;
    if (path == NULL || out_plan == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_plan, 0, sizeof(*out_plan));
    status = lt48_component_bundle_check_semantics(path, &semantics);
    if (status != LT48_OK) return status;
    if (!semantics.complete) return LT48_UNSUPPORTED;
    status = read_file(path, &data, &size);
    if (status != LT48_OK) return status;
    parser.data = data;
    parser.size = size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    object_array = object_find(&parser, 0u, "objects");
    object_count = array_count(&parser, object_array);
    objects = calloc(object_count ? object_count : 1u, sizeof(*objects));
    items = calloc(object_count ? object_count : 1u, sizeof(*items));
    payloads = calloc(object_count ? object_count : 1u, sizeof(*payloads));
    if (objects == NULL || items == NULL || payloads == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < object_count; ++index) {
        size_t item = array_item(&parser, object_array, index);
        items[index] = item;
        objects[index].id = object_find(&parser, item, "id");
        objects[index].kind = object_find(&parser, item, "kind");
        objects[index].extends = object_find(&parser, item, "extends");
        if (parser.tokens[objects[index].extends].type != JSON_STRING)
            objects[index].extends = SIZE_MAX;
        if (token_equal(parser.data, &parser.tokens[objects[index].kind],
                        "payload")) payloads[payload_count++] = index;
    }
    if (payload_count == 0u) {
        status = LT48_NOT_FOUND;
        goto done;
    }
    for (size_t index = 1u; index < payload_count; ++index) {
        size_t current = payloads[index];
        size_t position = index;
        const json_token *a = &parser.tokens[objects[current].id];
        while (position != 0u) {
            const json_token *b =
                &parser.tokens[objects[payloads[position - 1u]].id];
            size_t a_length = a->end - a->start;
            size_t b_length = b->end - b->start;
            size_t common = a_length < b_length ? a_length : b_length;
            int comparison = memcmp(parser.data + a->start,
                                    parser.data + b->start, common);
            if (comparison > 0 ||
                (comparison == 0 && a_length >= b_length)) break;
            payloads[position] = payloads[position - 1u];
            --position;
        }
        payloads[position] = current;
    }
    if (!output_literal(&output,
            "{\"schema_version\":1,\"bundle_sha256\":\"") ||
        !output_digest(&output, semantics.source_sha256) ||
        !output_literal(&output, "\",\"payloads\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t position = 0u; position < payload_count; ++position) {
        size_t index = payloads[position];
        property_member *members = NULL;
        size_t member_count = 0u;
        property_member *origin;
        int authored;
        if (!effective_properties(&parser, objects, items, object_count, index,
                                  &members, &member_count)) {
            free(members);
            status = LT48_NO_MEMORY;
            goto done;
        }
        origin = member_find(&parser, members, member_count, "origin");
        authored = origin != NULL && token_equal(
            parser.data, &parser.tokens[origin->value], "authored");
        if ((position != 0u && !output_literal(&output, ",")) ||
            !output_literal(&output, "{\"id\":") ||
            !canonical_token(&parser, objects[index].id, &output) ||
            !output_literal(&output, ",\"declared_sha256\":") ||
            !canonical_token(&parser, object_find(
                &parser, items[index], "sha256"), &output) ||
            !output_literal(&output, ",\"origin\":") ||
            !canonical_token(&parser, origin->value, &output) ||
            !output_literal(&output, ",\"payload_type\":") ||
            !canonical_token(&parser, member_find(
                &parser, members, member_count, "payload_type")->value,
                &output) ||
            !output_literal(&output, ",\"format\":") ||
            !canonical_token(&parser, member_find(
                &parser, members, member_count, "format")->value, &output) ||
            !output_literal(&output, ",\"abi\":") ||
            !canonical_token(&parser, member_find(
                &parser, members, member_count, "abi")->value, &output) ||
            !output_literal(&output, ",\"side_effects\":") ||
            !canonical_token(&parser, member_find(
                &parser, members, member_count, "side_effects")->value,
                &output) ||
            !output_literal(&output, ",\"build\":") ||
            !(authored ? canonical_token(&parser, member_find(
                  &parser, members, member_count, "build")->value, &output) :
                output_literal(&output, "null")) ||
            !output_literal(&output, "}")) {
            free(members);
            status = LT48_NO_MEMORY;
            goto done;
        }
        if (authored) ++out_plan->authored_payload_count;
        else ++out_plan->imported_payload_count;
        free(members);
    }
    if (!output_literal(&output, "],\"payload_count\":") ||
        !output_size_number(&output, payload_count) ||
        !output_literal(&output, ",\"authored_payload_count\":") ||
        !output_size_number(&output, out_plan->authored_payload_count) ||
        !output_literal(&output, ",\"imported_payload_count\":") ||
        !output_size_number(&output, out_plan->imported_payload_count) ||
        !output_literal(&output,
            ",\"execution_performed\":false,\"local_inputs_verified\":false}") ||
        !output_append(&output, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_plan->json = output.data;
    out_plan->json_size = output.size - 1u;
    lt48_sha256(output.data, output.size - 1u, out_plan->sha256);
    memcpy(out_plan->bundle_sha256, semantics.source_sha256, LT48_SHA256_SIZE);
    out_plan->payload_count = payload_count;
    output.data = NULL;
    status = LT48_OK;
done:
    free(output.data);
    free(payloads);
    free(items);
    free(objects);
    free(parser.tokens);
    free(data);
    if (status != LT48_OK) lt48_component_payload_build_plan_clear(out_plan);
    return status;
}

void lt48_component_payload_input_verification_clear(
    lt48_component_payload_input_verification *verification
) {
    if (verification == NULL) return;
    free(verification->json);
    memset(verification, 0, sizeof(*verification));
}

static lt48_status hash_payload_input_file(
    const char *path, lt48_cancel_check_fn cancelled, void *cancel_context,
    unsigned char digest[LT48_SHA256_SIZE], size_t *out_size
) {
    unsigned char buffer[65536];
    lt48_sha256_context hash;
    FILE *file;
    size_t total = 0u;
    size_t count;
    if (path == NULL || digest == NULL || out_size == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_size = 0u;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    lt48_sha256_init(&hash);
    for (;;) {
        if (cancelled != NULL && cancelled(cancel_context)) {
            fclose(file);
            return LT48_CANCELLED;
        }
        count = fread(buffer, 1u, sizeof(buffer), file);
        if (count != 0u) {
            if (total > 16u * 1024u * 1024u - count) {
                fclose(file);
                return LT48_FILE_ERROR;
            }
            lt48_sha256_update(&hash, buffer, count);
            total += count;
        }
        if (count != sizeof(buffer)) {
            if (ferror(file) || !feof(file) || fclose(file) != 0 ||
                total == 0u) return LT48_FILE_ERROR;
            break;
        }
    }
    lt48_sha256_final(&hash, digest);
    *out_size = total;
    return LT48_OK;
}

/* 1 = JSON result emitted, 0 = allocation/output failure, -1 = cancelled. */
static int verify_payload_input_file(
    const json_parser *parser, size_t build, const char *root,
    const char *kind, const char *path_field, const char *hash_field,
    lt48_cancel_check_fn cancelled, void *cancel_context, output_buffer *output,
    lt48_component_payload_input_verification *verification
) {
    char relative[512] = {0};
    char *joined = NULL;
    char *canonical = NULL;
    size_t size = 0u;
    unsigned char digest[LT48_SHA256_SIZE];
    char digest_text[LT48_SHA256_SIZE * 2u + 1u];
    const json_token *expected;
    size_t root_length = strlen(root);
    int verified = 0;
    lt48_status read_status = LT48_FILE_ERROR;
    if (cancelled != NULL && cancelled(cancel_context)) return -1;
    ++verification->required_input_count;
    if (token_text(parser, object_find(parser, build, path_field), relative,
                   sizeof(relative))) {
        size_t needed = root_length + 1u + strlen(relative) + 1u;
        joined = malloc(needed);
        if (joined != NULL) {
            (void)snprintf(joined, needed, "%s/%s", root, relative);
            canonical = realpath(joined, NULL);
        }
    }
    if (canonical != NULL &&
        ((root_length == 1u && root[0] == '/') ||
         (strncmp(canonical, root, root_length) == 0 &&
          canonical[root_length] == '/'))) {
        read_status = hash_payload_input_file(
            canonical, cancelled, cancel_context, digest, &size);
        if (read_status == LT48_CANCELLED) {
            free(canonical);
            free(joined);
            return -1;
        }
        if (read_status == LT48_OK) {
            for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
                static const char hex[] = "0123456789abcdef";
                digest_text[index * 2u] = hex[digest[index] >> 4u];
                digest_text[index * 2u + 1u] = hex[digest[index] & 0x0fu];
            }
            digest_text[LT48_SHA256_SIZE * 2u] = '\0';
            expected = &parser->tokens[object_find(parser, build, hash_field)];
            verified = token_equal(parser->data, expected, digest_text);
        }
    }
    if (!output_literal(output, "{\"kind\":") ||
        !output_json_string(output, kind) ||
        !output_literal(output, ",\"path\":") ||
        !output_json_string(output, relative) ||
        !output_literal(output, ",\"expected_sha256\":") ||
        !canonical_token(parser, object_find(parser, build, hash_field), output) ||
        !output_literal(output, ",\"actual_size\":") ||
        !(read_status == LT48_OK ? output_size_number(output, size) :
                                   output_literal(output, "null")) ||
        !output_literal(output, ",\"actual_sha256\":") ||
        !(read_status == LT48_OK ?
              (output_literal(output, "\"") &&
               output_append(output, digest_text, LT48_SHA256_SIZE * 2u) &&
               output_literal(output, "\"")) : output_literal(output, "null")) ||
        !output_literal(output, ",\"verified\":") ||
        !output_literal(output, verified ? "true" : "false") ||
        !output_literal(output, "}")) {
        free(canonical);
        free(joined);
        return 0;
    }
    if (verified) ++verification->verified_input_count;
    else ++verification->rejected_input_count;
    free(canonical);
    free(joined);
    return 1;
}

lt48_status lt48_component_bundle_verify_payload_inputs_cancellable(
    const char *path, const char *workspace_root,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_payload_input_verification *out_verification
) {
    lt48_component_payload_build_plan plan;
    json_parser parser = {0};
    output_buffer output = {0};
    char *root = NULL;
    struct stat root_stat;
    size_t payloads;
    size_t payload_count;
    lt48_status status;
    if (path == NULL || workspace_root == NULL || workspace_root[0] == '\0' ||
        out_verification == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_verification, 0, sizeof(*out_verification));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_bundle_plan_payload_builds(path, &plan);
    if (status != LT48_OK) return status;
    if (cancelled != NULL && cancelled(cancel_context)) {
        lt48_component_payload_build_plan_clear(&plan);
        return LT48_CANCELLED;
    }
    memcpy(out_verification->build_plan_sha256, plan.sha256, LT48_SHA256_SIZE);
    root = realpath(workspace_root, NULL);
    if (root == NULL || stat(root, &root_stat) != 0 ||
        !S_ISDIR(root_stat.st_mode)) {
        free(root);
        lt48_component_payload_build_plan_clear(&plan);
        return LT48_FILE_ERROR;
    }
    parser.data = plan.json;
    parser.size = plan.json_size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    payloads = object_find(&parser, 0u, "payloads");
    payload_count = array_count(&parser, payloads);
    if (!output_literal(&output,
            "{\"schema_version\":1,\"build_plan_sha256\":\"") ||
        !output_digest(&output, plan.sha256) ||
        !output_literal(&output,
            "\",\"workspace_root_verified\":true,\"payloads\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < payload_count; ++index) {
        size_t payload = array_item(&parser, payloads, index);
        size_t build = object_find(&parser, payload, "build");
        int source_result;
        int manifest_result;
        int toolchain_result;
        if (token_equal(parser.data, &parser.tokens[build], "null")) continue;
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            goto done;
        }
        if ((out_verification->authored_payload_count != 0u &&
             !output_literal(&output, ",")) ||
            !output_literal(&output, "{\"id\":") ||
            !canonical_token(&parser, object_find(&parser, payload, "id"),
                             &output) ||
            !output_literal(&output, ",\"inputs\":[")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        source_result = verify_payload_input_file(
            &parser, build, root, "source", "source_path", "source_sha256",
            cancelled, cancel_context, &output, out_verification);
        if (source_result < 0) { status = LT48_CANCELLED; goto done; }
        if (source_result == 0 || !output_literal(&output, ",")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        manifest_result = verify_payload_input_file(
            &parser, build, root, "manifest", "manifest_path",
            "manifest_sha256", cancelled, cancel_context, &output,
            out_verification);
        if (manifest_result < 0) { status = LT48_CANCELLED; goto done; }
        if (manifest_result == 0 || !output_literal(&output, ",")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        toolchain_result = verify_payload_input_file(
            &parser, build, root, "toolchain", "toolchain_path",
            "toolchain_sha256", cancelled, cancel_context, &output,
            out_verification);
        if (toolchain_result < 0) { status = LT48_CANCELLED; goto done; }
        if (toolchain_result == 0 || !output_literal(&output, "]}")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        ++out_verification->authored_payload_count;
    }
    out_verification->complete = out_verification->authored_payload_count != 0u &&
        out_verification->required_input_count ==
            out_verification->verified_input_count &&
        out_verification->rejected_input_count == 0u;
    if (!output_literal(&output, "],\"authored_payload_count\":") ||
        !output_size_number(&output, out_verification->authored_payload_count) ||
        !output_literal(&output, ",\"required_input_count\":") ||
        !output_size_number(&output, out_verification->required_input_count) ||
        !output_literal(&output, ",\"verified_input_count\":") ||
        !output_size_number(&output, out_verification->verified_input_count) ||
        !output_literal(&output, ",\"rejected_input_count\":") ||
        !output_size_number(&output, out_verification->rejected_input_count) ||
        !output_literal(&output, ",\"complete\":") ||
        !output_literal(&output,
                        out_verification->complete ? "true" : "false") ||
        !output_literal(&output, ",\"execution_performed\":false}") ||
        !output_append(&output, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_verification->json = output.data;
    out_verification->json_size = output.size - 1u;
    lt48_sha256(output.data, output.size - 1u, out_verification->sha256);
    output.data = NULL;
    status = out_verification->complete ? LT48_OK : LT48_SAFETY_REJECTED;
done:
    free(output.data);
    free(parser.tokens);
    free(root);
    lt48_component_payload_build_plan_clear(&plan);
    if (status != LT48_OK && status != LT48_SAFETY_REJECTED)
        lt48_component_payload_input_verification_clear(out_verification);
    return status;
}

lt48_status lt48_component_bundle_verify_payload_inputs(
    const char *path, const char *workspace_root,
    lt48_component_payload_input_verification *out_verification
) {
    return lt48_component_bundle_verify_payload_inputs_cancellable(
        path, workspace_root, NULL, NULL, out_verification);
}

void lt48_component_payload_build_execution_clear(
    lt48_component_payload_build_execution *execution
) {
    if (execution == NULL) return;
    free(execution->json);
    memset(execution, 0, sizeof(*execution));
}

static int payload_path_is_beneath(const char *root, const char *path) {
    size_t root_length = strlen(root);
    return (root_length == 1u && root[0] == '/') ||
        (strncmp(root, path, root_length) == 0 && path[root_length] == '/');
}

static char *payload_existing_path(
    const char *root, const char *relative
) {
    size_t needed = strlen(root) + 1u + strlen(relative) + 1u;
    char *joined = malloc(needed);
    char *canonical;
    if (joined == NULL) return NULL;
    (void)snprintf(joined, needed, "%s/%s", root, relative);
    canonical = realpath(joined, NULL);
    free(joined);
    if (canonical == NULL || !payload_path_is_beneath(root, canonical)) {
        free(canonical);
        return NULL;
    }
    return canonical;
}

static char *payload_new_output_path(
    const char *root, const char *relative
) {
    char path[512];
    char *slash;
    const char *base;
    char *parent;
    char *canonical_parent;
    char *output;
    size_t needed;
    struct stat existing;
    if (strlen(relative) >= sizeof(path)) return NULL;
    memcpy(path, relative, strlen(relative) + 1u);
    slash = strrchr(path, '/');
    if (slash == NULL) {
        parent = strdup(root);
        base = path;
    } else {
        *slash = '\0';
        base = slash + 1u;
        needed = strlen(root) + 1u + strlen(path) + 1u;
        parent = malloc(needed);
        if (parent != NULL)
            (void)snprintf(parent, needed, "%s/%s", root, path);
    }
    if (parent == NULL || base[0] == '\0') {
        free(parent);
        return NULL;
    }
    canonical_parent = realpath(parent, NULL);
    free(parent);
    if (canonical_parent == NULL ||
        (strcmp(canonical_parent, root) != 0 &&
         !payload_path_is_beneath(root, canonical_parent))) {
        free(canonical_parent);
        return NULL;
    }
    needed = strlen(canonical_parent) + 1u + strlen(base) + 1u;
    output = malloc(needed);
    if (output != NULL)
        (void)snprintf(output, needed, "%s/%s", canonical_parent, base);
    free(canonical_parent);
    if (output == NULL) return NULL;
    errno = 0;
    if (lstat(output, &existing) == 0 || errno != ENOENT) {
        free(output);
        return NULL;
    }
    return output;
}

static void payload_free_strings(char **strings, size_t count) {
    if (strings == NULL) return;
    for (size_t index = 0u; index < count; ++index) free(strings[index]);
    free(strings);
}

static int payload_wait_process(
    pid_t process, uint64_t timeout_ms,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    int *out_exit, int *out_timed_out, int *out_cancelled
) {
    struct timespec start;
    struct timespec now;
    struct timespec pause = {0, 10000000L};
    int wait_status;
    *out_exit = -1;
    *out_timed_out = 0;
    *out_cancelled = 0;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) return 0;
    for (;;) {
        if (cancelled != NULL && cancelled(cancel_context)) {
            *out_cancelled = 1;
            (void)kill(-process, SIGKILL);
            (void)kill(process, SIGKILL);
            while (waitpid(process, &wait_status, 0) < 0 && errno == EINTR) {}
            return 1;
        }
        pid_t waited = waitpid(process, &wait_status, WNOHANG);
        if (waited == process) {
            if (WIFEXITED(wait_status)) *out_exit = WEXITSTATUS(wait_status);
            else if (WIFSIGNALED(wait_status))
                *out_exit = 128 + WTERMSIG(wait_status);
            return 1;
        }
        if (waited < 0 && errno != EINTR) return 0;
        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) return 0;
        {
            uint64_t elapsed_ms =
                (uint64_t)(now.tv_sec - start.tv_sec) * 1000u +
                (uint64_t)(now.tv_nsec >= start.tv_nsec ?
                    now.tv_nsec - start.tv_nsec :
                    1000000000L + now.tv_nsec - start.tv_nsec) / 1000000u;
            if (now.tv_nsec < start.tv_nsec) elapsed_ms -= 1000u;
            if (elapsed_ms >= timeout_ms) {
                *out_timed_out = 1;
                (void)kill(-process, SIGKILL);
                (void)kill(process, SIGKILL);
                while (waitpid(process, &wait_status, 0) < 0 && errno == EINTR) {}
                return 1;
            }
        }
        (void)nanosleep(&pause, NULL);
    }
}

lt48_status lt48_component_bundle_execute_payload_builds_cancellable(
    const char *path, const char *workspace_root,
    const unsigned char confirmation_sha256[LT48_SHA256_SIZE],
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_payload_build_execution *out_execution
) {
    lt48_component_payload_input_verification verification = {0};
    lt48_component_payload_build_plan plan = {0};
    json_parser parser = {0};
    output_buffer output = {0};
    char *root = NULL;
    size_t payloads;
    size_t payload_count;
    size_t reported_payload_count = 0u;
    lt48_status execution_status = LT48_OK;
    lt48_status status;
    if (path == NULL || workspace_root == NULL ||
        confirmation_sha256 == NULL || out_execution == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_execution, 0, sizeof(*out_execution));
    status = lt48_component_bundle_verify_payload_inputs_cancellable(
        path, workspace_root, cancelled, cancel_context, &verification);
    if (status != LT48_OK) return status;
    if (memcmp(confirmation_sha256, verification.sha256,
               LT48_SHA256_SIZE) != 0) {
        lt48_component_payload_input_verification_clear(&verification);
        return LT48_PERMISSION_DENIED;
    }
    memcpy(out_execution->input_verification_sha256, verification.sha256,
           LT48_SHA256_SIZE);
    if (cancelled != NULL && cancelled(cancel_context)) {
        out_execution->cancelled = 1;
        status = LT48_CANCELLED;
        goto done;
    }
    status = lt48_component_bundle_plan_payload_builds(path, &plan);
    if (status != LT48_OK) goto done;
    root = realpath(workspace_root, NULL);
    if (root == NULL) {
        status = LT48_FILE_ERROR;
        goto done;
    }
    parser.data = plan.json;
    parser.size = plan.json_size;
    if (!parse_value(&parser, SIZE_MAX)) {
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    payloads = object_find(&parser, 0u, "payloads");
    payload_count = array_count(&parser, payloads);
    if (!output_literal(&output,
            "{\"schema_version\":1,\"input_verification_sha256\":\"") ||
        !output_digest(&output, verification.sha256) ||
        !output_literal(&output, "\",\"payloads\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < payload_count; ++index) {
        size_t payload = array_item(&parser, payloads, index);
        size_t build = object_find(&parser, payload, "build");
        size_t arguments;
        size_t argument_count = 0u;
        size_t environment;
        property_member *environment_members = NULL;
        size_t environment_count = 0u;
        char **argv = NULL;
        char **envp = NULL;
        char relative[512];
        char *toolchain = NULL;
        char *output_path = NULL;
        uint64_t timeout_ms;
        uint64_t expected_size;
        pid_t child;
        int exit_code = -1;
        int timed_out = 0;
        int process_cancelled = 0;
        int process_started = 0;
        unsigned char *output_data = NULL;
        size_t output_size = 0u;
        unsigned char output_digest_bytes[LT48_SHA256_SIZE];
        char output_digest_text[LT48_SHA256_SIZE * 2u + 1u];
        int output_verified = 0;
        if (token_equal(parser.data, &parser.tokens[build], "null")) continue;
        if (cancelled != NULL && cancelled(cancel_context)) {
            out_execution->cancelled = 1;
            execution_status = LT48_CANCELLED;
            break;
        }
        ++out_execution->attempted_payload_count;
        if (!token_text(&parser, object_find(&parser, build, "toolchain_path"),
                        relative, sizeof(relative))) {
            status = LT48_FORMAT_ERROR;
            goto payload_done;
        }
        toolchain = payload_existing_path(root, relative);
        if (toolchain == NULL || access(toolchain, X_OK) != 0) {
            status = LT48_PERMISSION_DENIED;
            goto payload_done;
        }
        if (!token_text(&parser, object_find(&parser, build, "output_path"),
                        relative, sizeof(relative))) {
            status = LT48_FORMAT_ERROR;
            goto payload_done;
        }
        output_path = payload_new_output_path(root, relative);
        if (output_path == NULL) {
            status = LT48_FILE_ERROR;
            goto payload_done;
        }
        arguments = object_find(&parser, build, "arguments");
        argument_count = array_count(&parser, arguments);
        argv = calloc(argument_count + 1u, sizeof(*argv));
        if (argv == NULL) {
            status = LT48_NO_MEMORY;
            goto payload_done;
        }
        argv[0] = strdup(toolchain);
        for (size_t argument = 1u; argument < argument_count; ++argument) {
            char text[512];
            if (!token_text(&parser, array_item(&parser, arguments, argument),
                            text, sizeof(text)) ||
                (argv[argument] = strdup(text)) == NULL) {
                status = LT48_NO_MEMORY;
                goto payload_done;
            }
        }
        environment = object_find(&parser, build, "environment");
        if (!collect_object_properties(&parser, environment,
                &environment_members, &environment_count)) {
            status = LT48_FORMAT_ERROR;
            goto payload_done;
        }
        envp = calloc(environment_count + 1u, sizeof(*envp));
        if (envp == NULL) {
            status = LT48_NO_MEMORY;
            goto payload_done;
        }
        for (size_t entry = 0u; entry < environment_count; ++entry) {
            const json_token *key =
                &parser.tokens[environment_members[entry].key];
            const json_token *value =
                &parser.tokens[environment_members[entry].value];
            size_t key_size = key->end - key->start;
            size_t value_size = value->end - value->start;
            envp[entry] = malloc(key_size + 1u + value_size + 1u);
            if (envp[entry] == NULL) {
                status = LT48_NO_MEMORY;
                goto payload_done;
            }
            memcpy(envp[entry], parser.data + key->start, key_size);
            envp[entry][key_size] = '=';
            memcpy(envp[entry] + key_size + 1u,
                   parser.data + value->start, value_size);
            envp[entry][key_size + 1u + value_size] = '\0';
        }
        {
            size_t limits = object_find(&parser, build, "resource_limits");
            if (!token_uint64(&parser,
                    object_find(&parser, limits, "maximum_runtime_ms"),
                    &timeout_ms) || timeout_ms == 0u || timeout_ms > 600000u ||
                !token_uint64(&parser,
                    object_find(&parser, build, "output_size"),
                    &expected_size)) {
                status = LT48_FORMAT_ERROR;
                goto payload_done;
            }
        }
        child = fork();
        if (child < 0) {
            status = LT48_RUNTIME_ERROR;
            goto payload_done;
        }
        if (child == 0) {
            int null_fd;
            (void)setpgid(0, 0);
            if (chdir(root) != 0) _exit(126);
            null_fd = open("/dev/null", O_RDWR);
            if (null_fd < 0) _exit(126);
            (void)dup2(null_fd, STDIN_FILENO);
            (void)dup2(null_fd, STDOUT_FILENO);
            (void)dup2(null_fd, STDERR_FILENO);
            if (null_fd > STDERR_FILENO) (void)close(null_fd);
            execve(toolchain, argv, envp);
            _exit(127);
        }
        process_started = 1;
        ++out_execution->started_payload_count;
        (void)setpgid(child, child);
        if (!payload_wait_process(
                child, timeout_ms, cancelled, cancel_context,
                &exit_code, &timed_out, &process_cancelled)) {
            status = LT48_RUNTIME_ERROR;
            goto payload_done;
        }
        if (process_cancelled) out_execution->cancelled = 1;
        if (!timed_out && !process_cancelled && exit_code == 0 &&
            read_file(output_path, &output_data, &output_size) == LT48_OK) {
            lt48_sha256(output_data, output_size, output_digest_bytes);
            for (size_t byte = 0u; byte < LT48_SHA256_SIZE; ++byte) {
                static const char hex[] = "0123456789abcdef";
                output_digest_text[byte * 2u] =
                    hex[output_digest_bytes[byte] >> 4u];
                output_digest_text[byte * 2u + 1u] =
                    hex[output_digest_bytes[byte] & 0x0fu];
            }
            output_digest_text[LT48_SHA256_SIZE * 2u] = '\0';
            output_verified = output_size == expected_size && token_equal(
                parser.data,
                &parser.tokens[object_find(&parser, build, "output_sha256")],
                output_digest_text);
        }
        if (!output_verified) {
            status = process_cancelled ? LT48_CANCELLED :
                timed_out ? LT48_TIMEOUT :
                exit_code != 0 ? LT48_RUNTIME_ERROR : LT48_SAFETY_REJECTED;
            goto payload_done;
        }
        if ((reported_payload_count != 0u &&
             !output_literal(&output, ",")) ||
            !output_literal(&output, "{\"id\":") ||
            !canonical_token(&parser, object_find(&parser, payload, "id"),
                             &output) ||
            !output_literal(&output, ",\"exit_code\":0,\"timed_out\":false,"
                "\"cancelled\":false,"
                "\"output_path\":") ||
            !canonical_token(&parser,
                             object_find(&parser, build, "output_path"),
                             &output) ||
            !output_literal(&output, ",\"output_size\":") ||
            !output_size_number(&output, output_size) ||
            !output_literal(&output, ",\"output_sha256\":\"") ||
            !output_append(&output, output_digest_text,
                           LT48_SHA256_SIZE * 2u) ||
            !output_literal(&output, "\",\"output_verified\":true}")) {
            status = LT48_NO_MEMORY;
            goto payload_done;
        }
        ++out_execution->completed_payload_count;
        ++out_execution->verified_output_count;
        ++reported_payload_count;
        status = LT48_OK;
payload_done:
        if (status != LT48_OK && process_started) {
            if ((reported_payload_count != 0u &&
                 !output_literal(&output, ",")) ||
                !output_literal(&output, "{\"id\":") ||
                !canonical_token(&parser,
                    object_find(&parser, payload, "id"), &output) ||
                !output_literal(&output, ",\"exit_code\":") ||
                !(exit_code >= 0 ?
                    output_size_number(&output, (size_t)exit_code) :
                    output_literal(&output, "null")) ||
                !output_literal(&output, ",\"timed_out\":") ||
                !output_literal(&output, timed_out ? "true" : "false") ||
                !output_literal(&output, ",\"cancelled\":") ||
                !output_literal(&output,
                                process_cancelled ? "true" : "false") ||
                !output_literal(&output,
                    ",\"output_verified\":false,\"status\":") ||
                !output_json_string(&output, lt48_status_string(status)) ||
                !output_literal(&output, "}")) {
                status = LT48_NO_MEMORY;
            } else {
                ++reported_payload_count;
            }
        }
        free(output_data);
        free(environment_members);
        payload_free_strings(envp, environment_count);
        payload_free_strings(argv, argument_count);
        if (status != LT48_OK && process_started && !output_verified &&
            output_path != NULL)
            (void)unlink(output_path);
        free(output_path);
        free(toolchain);
        if (status != LT48_OK) {
            if (status == LT48_NO_MEMORY) goto done;
            execution_status = status;
            break;
        }
    }
    out_execution->complete =
        out_execution->attempted_payload_count != 0u &&
        out_execution->attempted_payload_count ==
            out_execution->completed_payload_count &&
        out_execution->completed_payload_count ==
            out_execution->verified_output_count;
    if (!output_literal(&output, "],\"attempted_payload_count\":") ||
        !output_size_number(&output, out_execution->attempted_payload_count) ||
        !output_literal(&output, ",\"started_payload_count\":") ||
        !output_size_number(&output, out_execution->started_payload_count) ||
        !output_literal(&output, ",\"completed_payload_count\":") ||
        !output_size_number(&output, out_execution->completed_payload_count) ||
        !output_literal(&output, ",\"verified_output_count\":") ||
        !output_size_number(&output, out_execution->verified_output_count) ||
        !output_literal(&output, ",\"complete\":") ||
        !output_literal(&output, out_execution->complete ? "true" : "false") ||
        !output_literal(&output, ",\"cancelled\":") ||
        !output_literal(&output, out_execution->cancelled ? "true" : "false") ||
        !output_literal(&output, ",\"usb_opened\":false}") ||
        !output_append(&output, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_execution->json = output.data;
    out_execution->json_size = output.size - 1u;
    lt48_sha256(output.data, output.size - 1u, out_execution->sha256);
    output.data = NULL;
    status = execution_status != LT48_OK ? execution_status :
        out_execution->complete ? LT48_OK : LT48_RUNTIME_ERROR;
done:
    free(output.data);
    free(parser.tokens);
    free(root);
    lt48_component_payload_build_plan_clear(&plan);
    lt48_component_payload_input_verification_clear(&verification);
    if (status != LT48_OK && out_execution->json == NULL)
        lt48_component_payload_build_execution_clear(out_execution);
    return status;
}

lt48_status lt48_component_bundle_execute_payload_builds(
    const char *path, const char *workspace_root,
    const unsigned char confirmation_sha256[LT48_SHA256_SIZE],
    lt48_component_payload_build_execution *out_execution
) {
    return lt48_component_bundle_execute_payload_builds_cancellable(
        path, workspace_root, confirmation_sha256, NULL, NULL, out_execution);
}

static int evidence_type_method_valid(
    const json_parser *parser, size_t type, size_t method,
    int *out_hardware
) {
    static const struct {
        const char *type;
        const char *method;
        int hardware;
    } pairs[] = {
        {"simulation", "independent-model", 0},
        {"fault-injection", "phase-fault-injection", 0},
        {"windows-reference", "windows-reference-matched", 0},
        {"manufacturer-conformance", "manufacturer-spec-validated", 0},
        {"adapter-bench", "dummy-load-electrical", 1},
        {"engine-hardware", "representative-device", 1},
        {"device-hardware", "exact-device", 1}
    };
    for (size_t index = 0u; index < sizeof(pairs) / sizeof(pairs[0]); ++index)
        if (token_equal(parser->data, &parser->tokens[type],
                        pairs[index].type) &&
            token_equal(parser->data, &parser->tokens[method],
                        pairs[index].method)) {
            *out_hardware = pairs[index].hardware;
            return 1;
        }
    return 0;
}

static int nullable_nonempty_text(
    const json_parser *parser, size_t token, int *out_present
) {
    if (token_equal(parser->data, &parser->tokens[token], "null")) {
        *out_present = 0;
        return 1;
    }
    if (token >= parser->count || parser->tokens[token].type != JSON_STRING ||
        parser->tokens[token].end == parser->tokens[token].start)
        return 0;
    *out_present = 1;
    return 1;
}

lt48_status lt48_component_evidence_validate(
    const char *path, lt48_component_evidence_report *out_report
) {
    static const char *const root_fields[] = {
        "schema_version", "evidence", "subject", "environment"
    };
    static const char *const evidence_fields[] = {
        "id", "type", "method", "status", "performed_date",
        "artifact_sha256"
    };
    static const char *const subject_fields[] = {
        "bundle_sha256", "matrix_sha256", "plan_sha256",
        "combination_index", "operation_id"
    };
    static const char *const environment_fields[] = {
        "programmer_serial", "adapter_revision", "device_marking",
        "device_serial"
    };
    unsigned char *data = NULL;
    size_t size = 0u;
    json_parser parser = {0};
    size_t evidence;
    size_t subject;
    size_t environment;
    size_t type;
    size_t method;
    uint64_t combination;
    int programmer_present;
    int adapter_present;
    int marking_present;
    int serial_present;
    char date[11];
    lt48_status status;
    if (out_report == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    status = read_file(path, &data, &size);
    if (status != LT48_OK) {
        snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
                 "cannot read evidence record");
        return status;
    }
    lt48_sha256(data, size, out_report->source_sha256);
    parser.data = data;
    parser.size = size;
    if (!parse_value(&parser, SIZE_MAX)) parser.failed = 1;
    skip_space(&parser);
    if (parser.failed || parser.cursor != size || parser.count == 0u ||
        parser.tokens[0].type != JSON_OBJECT ||
        !object_fields(&parser, 0u, root_fields, 4u) ||
        !token_equal(parser.data,
            &parser.tokens[object_find(&parser, 0u, "schema_version")], "1") ||
        (evidence = object_find(&parser, 0u, "evidence")) == SIZE_MAX ||
        (subject = object_find(&parser, 0u, "subject")) == SIZE_MAX ||
        (environment = object_find(&parser, 0u, "environment")) == SIZE_MAX ||
        !object_fields(&parser, evidence, evidence_fields, 6u) ||
        !object_fields(&parser, subject, subject_fields, 5u) ||
        !object_fields(&parser, environment, environment_fields, 4u)) {
        snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
                 "evidence root or record schema is invalid");
        status = parser.failed ? LT48_NO_MEMORY : LT48_FORMAT_ERROR;
        goto done;
    }
    type = object_find(&parser, evidence, "type");
    method = object_find(&parser, evidence, "method");
    if (!valid_identifier(&parser, object_find(&parser, evidence, "id")) ||
        !evidence_type_method_valid(&parser, type, method,
                                    &out_report->hardware_evidence) ||
        (!token_equal(parser.data,
                      &parser.tokens[object_find(&parser, evidence, "status")],
                      "pass") &&
         !token_equal(parser.data,
                      &parser.tokens[object_find(&parser, evidence, "status")],
                      "fail")) ||
        !token_text(&parser, object_find(&parser, evidence, "performed_date"),
                    date, sizeof(date)) || !c_date(date) ||
        !valid_sha256(&parser,
                      object_find(&parser, evidence, "artifact_sha256")) ||
        !valid_sha256(&parser, object_find(&parser, subject, "bundle_sha256")) ||
        !valid_sha256(&parser, object_find(&parser, subject, "matrix_sha256")) ||
        !valid_sha256(&parser, object_find(&parser, subject, "plan_sha256")) ||
        !token_uint64(&parser,
                      object_find(&parser, subject, "combination_index"),
                      &combination) || combination > SIZE_MAX ||
        !valid_identifier(&parser,
                          object_find(&parser, subject, "operation_id")) ||
        !nullable_nonempty_text(
            &parser, object_find(&parser, environment, "programmer_serial"),
            &programmer_present) ||
        !nullable_nonempty_text(
            &parser, object_find(&parser, environment, "adapter_revision"),
            &adapter_present) ||
        !nullable_nonempty_text(
            &parser, object_find(&parser, environment, "device_marking"),
            &marking_present) ||
        !nullable_nonempty_text(
            &parser, object_find(&parser, environment, "device_serial"),
            &serial_present)) {
        snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
                 "evidence identity, binding, result, or environment is invalid");
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    (void)serial_present;
    if (out_report->hardware_evidence &&
        (!programmer_present || !adapter_present ||
         ((!token_equal(parser.data, &parser.tokens[type], "adapter-bench")) &&
          !marking_present))) {
        snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
                 "hardware evidence lacks programmer, adapter, or device identity");
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    if (!token_text(&parser, object_find(&parser, evidence, "id"),
                    out_report->evidence_id,
                    sizeof(out_report->evidence_id)) ||
        !token_text(&parser, type, out_report->evidence_type,
                    sizeof(out_report->evidence_type)) ||
        !token_text(&parser, method, out_report->method,
                    sizeof(out_report->method)) ||
        !token_text(&parser, object_find(&parser, evidence, "status"),
                    out_report->status, sizeof(out_report->status)) ||
        !token_text(&parser, object_find(&parser, evidence, "performed_date"),
                    out_report->performed_date,
                    sizeof(out_report->performed_date)) ||
        !token_text(&parser, object_find(&parser, evidence, "artifact_sha256"),
                    out_report->artifact_sha256,
                    sizeof(out_report->artifact_sha256)) ||
        !token_text(&parser, object_find(&parser, subject, "bundle_sha256"),
                    out_report->bundle_sha256,
                    sizeof(out_report->bundle_sha256)) ||
        !token_text(&parser, object_find(&parser, subject, "matrix_sha256"),
                    out_report->matrix_sha256,
                    sizeof(out_report->matrix_sha256)) ||
        !token_text(&parser, object_find(&parser, subject, "plan_sha256"),
                    out_report->plan_sha256,
                    sizeof(out_report->plan_sha256)) ||
        !token_text(&parser, object_find(&parser, subject, "operation_id"),
                    out_report->operation_id,
                    sizeof(out_report->operation_id))) {
        status = LT48_FORMAT_ERROR;
        goto done;
    }
    out_report->combination_index = (size_t)combination;
    out_report->passed = strcmp(out_report->status, "pass") == 0;
    out_report->complete = 1;
    snprintf(out_report->diagnostic, sizeof(out_report->diagnostic),
             "immutable evidence record is structurally complete");
    status = LT48_OK;
done:
    free(parser.tokens);
    free(data);
    return status;
}

static lt48_status hash_artifact_file_cancellable(
    const char *path, unsigned char digest[LT48_SHA256_SIZE],
    size_t *out_size, lt48_cancel_check_fn cancelled, void *cancel_context
) {
    unsigned char buffer[65536];
    lt48_sha256_context context;
    FILE *file;
    size_t total = 0u;
    if (path == NULL || path[0] == '\0' || digest == NULL || out_size == NULL)
        return LT48_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    lt48_sha256_init(&context);
    for (;;) {
        if (cancelled != NULL && cancelled(cancel_context)) {
            fclose(file);
            return LT48_CANCELLED;
        }
        size_t count = fread(buffer, 1u, sizeof(buffer), file);
        if (count != 0u) {
            if (count > SIZE_MAX - total) {
                fclose(file);
                return LT48_FILE_ERROR;
            }
            lt48_sha256_update(&context, buffer, count);
            total += count;
        }
        if (count != sizeof(buffer)) {
            if (ferror(file) || fclose(file) != 0) return LT48_FILE_ERROR;
            break;
        }
    }
    lt48_sha256_final(&context, digest);
    *out_size = total;
    return LT48_OK;
}

static int digest_matches_hex(
    const unsigned char digest[LT48_SHA256_SIZE], const char *expected
) {
    static const char hex[] = "0123456789abcdef";
    if (expected == NULL || strlen(expected) != LT48_SHA256_SIZE * 2u)
        return 0;
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        unsigned char high = (unsigned char)tolower(
            (unsigned char)expected[index * 2u]);
        unsigned char low = (unsigned char)tolower(
            (unsigned char)expected[index * 2u + 1u]);
        if (high != (unsigned char)hex[digest[index] >> 4u] ||
            low != (unsigned char)hex[digest[index] & 0x0fu])
            return 0;
    }
    return 1;
}

lt48_status lt48_component_evidence_verify_with_matrix_internal(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path, const lt48_component_plan_matrix *matrix,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_verification *out_verification
) {
    lt48_status status;
    int found_plan = 0;
    if (bundle_path == NULL || evidence_path == NULL || artifact_path == NULL ||
        matrix == NULL || out_verification == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_verification, 0, sizeof(*out_verification));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_evidence_validate(
        evidence_path, &out_verification->evidence);
    if (status != LT48_OK) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 "evidence record is not structurally valid");
        return status;
    }
    status = hash_artifact_file_cancellable(
        artifact_path, out_verification->artifact_sha256,
        &out_verification->artifact_size, cancelled, cancel_context);
    if (status != LT48_OK) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 status == LT48_CANCELLED ?
                    "evidence verification cancelled while hashing artifact" :
                    "cannot hash detailed evidence artifact");
        return status;
    }
    if (!digest_matches_hex(out_verification->artifact_sha256,
                            out_verification->evidence.artifact_sha256)) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 "detailed evidence artifact SHA-256 does not match record");
        return LT48_SAFETY_REJECTED;
    }
    out_verification->artifact_verified = 1;
    if (!digest_matches_hex(matrix->bundle_sha256,
                            out_verification->evidence.bundle_sha256)) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 "evidence bundle SHA-256 binding is stale");
        status = LT48_SAFETY_REJECTED;
        goto done;
    }
    out_verification->bundle_verified = 1;
    if (!digest_matches_hex(matrix->sha256,
                            out_verification->evidence.matrix_sha256)) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 "evidence matrix SHA-256 binding is stale");
        status = LT48_SAFETY_REJECTED;
        goto done;
    }
    out_verification->matrix_verified = 1;
    if (out_verification->evidence.combination_index >=
        matrix->combination_count) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 "evidence combination index does not exist");
        status = LT48_SAFETY_REJECTED;
        goto done;
    }
    for (unsigned value = 1u; value <= LT48_OPERATION_KIND_COUNT; ++value) {
        lt48_component_plan plan;
        const char *operation = lt48_operation_kind_name(
            (lt48_operation_kind)value);
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            goto done;
        }
        status = lt48_component_bundle_compile_plan(
            bundle_path, out_verification->evidence.combination_index,
            operation, &plan);
        if (status == LT48_NOT_FOUND) continue;
        if (status != LT48_OK) goto done;
        if (strcmp(plan.operation_id,
                   out_verification->evidence.operation_id) == 0) {
            found_plan = 1;
            if (digest_matches_hex(plan.sha256,
                                   out_verification->evidence.plan_sha256))
                out_verification->plan_verified = 1;
            lt48_component_plan_clear(&plan);
            break;
        }
        lt48_component_plan_clear(&plan);
    }
    if (!found_plan) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 "evidence operation is not selected by this combination");
        status = LT48_SAFETY_REJECTED;
        goto done;
    }
    if (!out_verification->plan_verified) {
        snprintf(out_verification->diagnostic,
                 sizeof(out_verification->diagnostic),
                 "evidence plan SHA-256 binding is stale");
        status = LT48_SAFETY_REJECTED;
        goto done;
    }
    out_verification->complete = 1;
    snprintf(out_verification->diagnostic,
             sizeof(out_verification->diagnostic),
             "artifact and all evidence subject bindings are verified");
    status = LT48_OK;
done:
    return status;
}

lt48_status lt48_component_evidence_verify_cancellable(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_verification *out_verification
) {
    lt48_component_plan_matrix matrix;
    lt48_status status;
    if (bundle_path == NULL || evidence_path == NULL || artifact_path == NULL ||
        out_verification == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_verification, 0, sizeof(*out_verification));
    status = lt48_component_bundle_compile_plan_matrix_cancellable(
        bundle_path, cancelled, cancel_context, &matrix);
    if (status != LT48_OK) {
        if (status != LT48_CANCELLED)
            snprintf(out_verification->diagnostic,
                     sizeof(out_verification->diagnostic),
                     "subject bundle no longer compiles into a complete matrix");
        return status;
    }
    status = lt48_component_evidence_verify_with_matrix_internal(
        bundle_path, evidence_path, artifact_path, &matrix, cancelled,
        cancel_context, out_verification);
    lt48_component_plan_matrix_clear(&matrix);
    return status;
}

lt48_status lt48_component_evidence_verify(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path,
    lt48_component_evidence_verification *out_verification
) {
    return lt48_component_evidence_verify_cancellable(
        bundle_path, evidence_path, artifact_path, NULL, NULL,
        out_verification);
}
