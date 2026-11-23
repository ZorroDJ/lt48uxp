#include <labtool48/labtool48.h>

#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lt48_buffer {
    unsigned char *data;
    unsigned char *known;
    size_t size;
    size_t known_size;
    lt48_buffer_provenance provenance;
};

static void record_mutation(lt48_buffer *buffer, size_t length) {
    if (length != 0u && buffer->provenance.mutation_count != SIZE_MAX)
        ++buffer->provenance.mutation_count;
}

static void append_u64(unsigned char **cursor, uint64_t value) {
    for (unsigned shift = 56u;; shift -= 8u) {
        *(*cursor)++ = (unsigned char)(value >> shift);
        if (shift == 0u) break;
    }
}

static int valid_range(const lt48_buffer *buffer, size_t offset, size_t length) {
    return buffer != NULL && offset <= buffer->size &&
           length <= buffer->size - offset;
}

static int known_range(
    const lt48_buffer *buffer, size_t offset, size_t length
) {
    if (!valid_range(buffer, offset, length)) return 0;
    if (buffer->known == NULL || length == 0u) return 1;
    return memchr(buffer->known + offset, 0, length) == NULL;
}

static void mark_known(lt48_buffer *buffer, size_t offset, size_t length) {
    if (buffer->known == NULL) return;
    for (size_t index = 0u; index < length; ++index) {
        if (buffer->known[offset + index] == 0u) {
            buffer->known[offset + index] = 1u;
            ++buffer->known_size;
        }
    }
    if (buffer->known_size == buffer->size) {
        free(buffer->known);
        buffer->known = NULL;
    }
}

lt48_status lt48_buffer_create(
    size_t size, unsigned char fill, lt48_buffer **out_buffer
) {
    lt48_buffer *buffer;
    if (out_buffer == NULL || size == 0u) return LT48_INVALID_ARGUMENT;
    *out_buffer = NULL;
    buffer = calloc(1u, sizeof(*buffer));
    if (buffer == NULL) return LT48_NO_MEMORY;
    buffer->data = malloc(size);
    if (buffer->data == NULL) {
        free(buffer);
        return LT48_NO_MEMORY;
    }
    memset(buffer->data, fill, size);
    buffer->size = size;
    buffer->known_size = size;
    buffer->provenance.origin = LT48_BUFFER_ORIGIN_GENERATED_FILL;
    buffer->provenance.padded_size = size;
    buffer->provenance.short_input = LT48_BUFFER_SHORT_PAD;
    buffer->provenance.long_input = LT48_BUFFER_LONG_REJECT;
    buffer->provenance.fill_value = fill;
    buffer->provenance.word_size_bytes = 1u;
    *out_buffer = buffer;
    return LT48_OK;
}

lt48_status lt48_buffer_create_sparse(
    size_t size, lt48_buffer **out_buffer
) {
    lt48_buffer *buffer;
    if (out_buffer == NULL || size == 0u) return LT48_INVALID_ARGUMENT;
    *out_buffer = NULL;
    buffer = calloc(1u, sizeof(*buffer));
    if (buffer == NULL) return LT48_NO_MEMORY;
    buffer->data = calloc(size, 1u);
    buffer->known = calloc(size, 1u);
    if (buffer->data == NULL || buffer->known == NULL) {
        free(buffer->known);
        free(buffer->data);
        free(buffer);
        return LT48_NO_MEMORY;
    }
    buffer->size = size;
    buffer->provenance.origin = LT48_BUFFER_ORIGIN_DEVICE_READ;
    buffer->provenance.short_input = LT48_BUFFER_SHORT_REJECT;
    buffer->provenance.long_input = LT48_BUFFER_LONG_REJECT;
    buffer->provenance.word_size_bytes = 1u;
    *out_buffer = buffer;
    return LT48_OK;
}

lt48_status lt48_buffer_load_file_exact(
    const char *path, size_t expected_size, lt48_buffer **out_buffer
) {
    lt48_buffer_import_options options = {
        expected_size,
        LT48_BUFFER_SHORT_REJECT,
        LT48_BUFFER_LONG_REJECT,
        0u,
        1u,
        0
    };
    return lt48_buffer_load_file(path, &options, NULL, out_buffer);
}

lt48_status lt48_buffer_load_file(
    const char *path, const lt48_buffer_import_options *options,
    lt48_buffer_import_result *out_result, lt48_buffer **out_buffer
) {
    return lt48_buffer_load_file_cancellable(
        path, options, NULL, NULL, out_result, out_buffer);
}

lt48_status lt48_buffer_load_file_cancellable(
    const char *path, const lt48_buffer_import_options *options,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_buffer_import_result *out_result, lt48_buffer **out_buffer
) {
    lt48_buffer *buffer = NULL;
    FILE *file;
    size_t received = 0u;
    size_t input_size;
    unsigned char discard[4096];
    lt48_buffer_import_result result = {0};
    lt48_status status = LT48_OK;
    if (path == NULL || path[0] == '\0' || options == NULL ||
        out_buffer == NULL || options->buffer_size == 0u ||
        (options->short_input != LT48_BUFFER_SHORT_REJECT &&
         options->short_input != LT48_BUFFER_SHORT_PAD) ||
        (options->long_input != LT48_BUFFER_LONG_REJECT &&
         options->long_input != LT48_BUFFER_LONG_TRUNCATE) ||
        (options->word_size_bytes != 1u &&
         options->word_size_bytes != 2u &&
         options->word_size_bytes != 4u &&
         options->word_size_bytes != 8u) ||
        (options->reverse_word_bytes != 0 &&
         options->reverse_word_bytes != 1) ||
        options->buffer_size % options->word_size_bytes != 0u)
        return LT48_INVALID_ARGUMENT;
    *out_buffer = NULL;
    if (out_result != NULL) memset(out_result, 0, sizeof(*out_result));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    status = lt48_buffer_create(options->buffer_size, options->pad_value,
                                &buffer);
    if (status != LT48_OK) {
        fclose(file);
        return status;
    }
    while (received < options->buffer_size) {
        size_t remaining = options->buffer_size - received;
        size_t request = remaining > sizeof(discard) ? sizeof(discard) : remaining;
        size_t count;
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        count = fread(buffer->data + received, 1u, request, file);
        received += count;
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        if (count != request) break;
    }
    input_size = received;
    while (status == LT48_OK && !ferror(file)) {
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        size_t extra = fread(discard, 1u, sizeof(discard), file);
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        if (extra == 0u) break;
        if (input_size > SIZE_MAX - extra) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        input_size += extra;
    }
    if (status == LT48_OK && ferror(file)) status = LT48_FILE_ERROR;
    if (fclose(file) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    result.input_size = input_size;
    result.imported_size = received;
    if (input_size < options->buffer_size)
        result.padded_size = options->buffer_size - input_size;
    else if (input_size > options->buffer_size)
        result.truncated_size = input_size - options->buffer_size;
    if (out_result != NULL) *out_result = result;
    if (status == LT48_OK && result.padded_size != 0u &&
        options->short_input == LT48_BUFFER_SHORT_REJECT)
        status = LT48_FORMAT_ERROR;
    if (status == LT48_OK && result.truncated_size != 0u &&
        options->long_input == LT48_BUFFER_LONG_REJECT)
        status = LT48_FORMAT_ERROR;
    if (status == LT48_OK && options->reverse_word_bytes) {
        for (size_t offset = 0u; offset < options->buffer_size;
             offset += options->word_size_bytes) {
            if ((offset & 0xffffu) == 0u && cancelled != NULL &&
                cancelled(cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            for (size_t low = 0u, high = options->word_size_bytes - 1u;
                 low < high; ++low, --high) {
                unsigned char temporary = buffer->data[offset + low];
                buffer->data[offset + low] = buffer->data[offset + high];
                buffer->data[offset + high] = temporary;
            }
        }
    }
    if (status != LT48_OK) {
        if (status == LT48_CANCELLED && out_result != NULL)
            memset(out_result, 0, sizeof(*out_result));
        lt48_buffer_close(buffer);
        return status;
    }
    buffer->provenance.origin = LT48_BUFFER_ORIGIN_FILE_IMPORT;
    buffer->provenance.input_size = result.input_size;
    buffer->provenance.imported_size = result.imported_size;
    buffer->provenance.padded_size = result.padded_size;
    buffer->provenance.truncated_size = result.truncated_size;
    buffer->provenance.short_input = options->short_input;
    buffer->provenance.long_input = options->long_input;
    buffer->provenance.fill_value = options->pad_value;
    buffer->provenance.word_size_bytes = options->word_size_bytes;
    buffer->provenance.reverse_word_bytes = options->reverse_word_bytes;
    buffer->provenance.mutation_count = 0u;
    *out_buffer = buffer;
    return LT48_OK;
}

void lt48_buffer_set_image_provenance_internal(
    lt48_buffer *buffer, const lt48_image_import_options *options,
    const lt48_image_import_result *result
) {
    if (buffer == NULL || options == NULL || result == NULL) return;
    memset(&buffer->provenance, 0, sizeof(buffer->provenance));
    buffer->provenance.origin = LT48_BUFFER_ORIGIN_FILE_IMPORT;
    buffer->provenance.input_size = result->input_size;
    buffer->provenance.imported_size = result->mapped_data_byte_count;
    buffer->provenance.padded_size =
        options->gap_policy == LT48_IMAGE_GAPS_FILL ?
        result->gap_byte_count : 0u;
    buffer->provenance.short_input =
        options->gap_policy == LT48_IMAGE_GAPS_FILL ?
        LT48_BUFFER_SHORT_PAD : LT48_BUFFER_SHORT_REJECT;
    buffer->provenance.long_input = LT48_BUFFER_LONG_REJECT;
    buffer->provenance.fill_value = options->gap_fill_value;
    buffer->provenance.word_size_bytes = 1u;
    buffer->provenance.image_format = result->format;
    buffer->provenance.image_address_base = options->address_base;
    buffer->provenance.image_gap_policy = options->gap_policy;
}

void lt48_buffer_close(lt48_buffer *buffer) {
    if (buffer == NULL) return;
    free(buffer->known);
    free(buffer->data);
    free(buffer);
}

size_t lt48_buffer_size(const lt48_buffer *buffer) {
    return buffer == NULL ? 0u : buffer->size;
}

const unsigned char *lt48_buffer_data(const lt48_buffer *buffer) {
    return buffer == NULL ? NULL : buffer->data;
}

size_t lt48_buffer_known_size(const lt48_buffer *buffer) {
    return buffer == NULL ? 0u : buffer->known_size;
}

lt48_status lt48_buffer_range_known(
    const lt48_buffer *buffer, size_t offset, size_t length, int *out_known
) {
    if (!valid_range(buffer, offset, length) || out_known == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_known = known_range(buffer, offset, length);
    return LT48_OK;
}

lt48_status lt48_buffer_read(
    const lt48_buffer *buffer, size_t offset,
    unsigned char *destination, size_t length
) {
    if (!valid_range(buffer, offset, length) ||
        (length != 0u && destination == NULL)) return LT48_INVALID_ARGUMENT;
    if (!known_range(buffer, offset, length)) return LT48_SAFETY_REJECTED;
    if (length != 0u) memcpy(destination, buffer->data + offset, length);
    return LT48_OK;
}

lt48_status lt48_buffer_write(
    lt48_buffer *buffer, size_t offset,
    const unsigned char *source, size_t length
) {
    if (!valid_range(buffer, offset, length) ||
        (length != 0u && source == NULL)) return LT48_INVALID_ARGUMENT;
    if (length != 0u) memmove(buffer->data + offset, source, length);
    mark_known(buffer, offset, length);
    record_mutation(buffer, length);
    return LT48_OK;
}

lt48_status lt48_buffer_write_acquired(
    lt48_buffer *buffer, size_t offset,
    const unsigned char *source, size_t length
) {
    if (!valid_range(buffer, offset, length) ||
        (length != 0u && source == NULL)) return LT48_INVALID_ARGUMENT;
    if (length != 0u) memmove(buffer->data + offset, source, length);
    mark_known(buffer, offset, length);
    if (buffer->provenance.origin == LT48_BUFFER_ORIGIN_DEVICE_READ)
        buffer->provenance.imported_size = buffer->known_size;
    record_mutation(buffer, length);
    return LT48_OK;
}

lt48_status lt48_buffer_fill(
    lt48_buffer *buffer, size_t offset, size_t length, unsigned char value
) {
    if (!valid_range(buffer, offset, length)) return LT48_INVALID_ARGUMENT;
    if (length != 0u) memset(buffer->data + offset, value, length);
    mark_known(buffer, offset, length);
    record_mutation(buffer, length);
    return LT48_OK;
}

lt48_status lt48_buffer_get_byte(
    const lt48_buffer *buffer, size_t offset, unsigned char *out_value
) {
    if (!valid_range(buffer, offset, 1u) || out_value == NULL)
        return LT48_INVALID_ARGUMENT;
    if (!known_range(buffer, offset, 1u)) return LT48_SAFETY_REJECTED;
    *out_value = buffer->data[offset];
    return LT48_OK;
}

lt48_status lt48_buffer_set_byte(
    lt48_buffer *buffer, size_t offset, unsigned char value
) {
    if (!valid_range(buffer, offset, 1u)) return LT48_INVALID_ARGUMENT;
    buffer->data[offset] = value;
    mark_known(buffer, offset, 1u);
    record_mutation(buffer, 1u);
    return LT48_OK;
}

static int valid_word_parameters(
    const lt48_buffer *buffer, size_t offset, size_t width,
    lt48_buffer_byte_order order
) {
    return valid_range(buffer, offset, width) &&
           (width == 1u || width == 2u || width == 4u || width == 8u) &&
           (order == LT48_BUFFER_LITTLE_ENDIAN ||
            order == LT48_BUFFER_BIG_ENDIAN);
}

lt48_status lt48_buffer_get_word(
    const lt48_buffer *buffer, size_t offset, size_t width,
    lt48_buffer_byte_order order, uint64_t *out_value
) {
    uint64_t value = 0u;
    if (!valid_word_parameters(buffer, offset, width, order) ||
        out_value == NULL)
        return LT48_INVALID_ARGUMENT;
    if (!known_range(buffer, offset, width)) return LT48_SAFETY_REJECTED;
    for (size_t index = 0u; index < width; ++index) {
        size_t byte_index = order == LT48_BUFFER_LITTLE_ENDIAN ?
                            width - index - 1u : index;
        value = (value << 8u) | buffer->data[offset + byte_index];
    }
    *out_value = value;
    return LT48_OK;
}

lt48_status lt48_buffer_set_word(
    lt48_buffer *buffer, size_t offset, size_t width,
    lt48_buffer_byte_order order, uint64_t value
) {
    uint64_t maximum;
    if (!valid_word_parameters(buffer, offset, width, order))
        return LT48_INVALID_ARGUMENT;
    maximum = width == 8u ? UINT64_MAX : ((UINT64_C(1) << (width * 8u)) - 1u);
    if (value > maximum) return LT48_INVALID_ARGUMENT;
    for (size_t index = 0u; index < width; ++index) {
        size_t byte_index = order == LT48_BUFFER_LITTLE_ENDIAN ?
                            index : width - index - 1u;
        buffer->data[offset + byte_index] = (unsigned char)(value & 0xffu);
        value >>= 8u;
    }
    mark_known(buffer, offset, width);
    record_mutation(buffer, width);
    return LT48_OK;
}

lt48_status lt48_buffer_checksum_range(
    const lt48_buffer *buffer, size_t offset, size_t length,
    lt48_buffer_checksum *out_checksum
) {
    return lt48_buffer_checksum_range_cancellable(
        buffer, offset, length, NULL, NULL, out_checksum);
}

lt48_status lt48_buffer_checksum_range_cancellable(
    const lt48_buffer *buffer, size_t offset, size_t length,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_buffer_checksum *out_checksum
) {
    lt48_sha256_context hash;
    uint32_t sum = 0u;
    size_t completed = 0u;
    if (!valid_range(buffer, offset, length) || out_checksum == NULL)
        return LT48_INVALID_ARGUMENT;
    if (!known_range(buffer, offset, length)) return LT48_SAFETY_REJECTED;
    memset(out_checksum, 0, sizeof(*out_checksum));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    lt48_sha256_init(&hash);
    while (completed < length) {
        size_t chunk = length - completed;
        if (chunk > 65536u) chunk = 65536u;
        for (size_t index = 0u; index < chunk; ++index)
            sum += buffer->data[offset + completed + index];
        lt48_sha256_update(
            &hash, buffer->data + offset + completed, chunk);
        completed += chunk;
        if (cancelled != NULL && cancelled(cancel_context)) {
            memset(&hash, 0, sizeof(hash));
            return LT48_CANCELLED;
        }
    }
    out_checksum->additive32 = sum;
    lt48_sha256_final(&hash, out_checksum->sha256);
    return LT48_OK;
}

lt48_status lt48_buffer_compare_range(
    const lt48_buffer *buffer, size_t offset,
    const unsigned char *expected, size_t length,
    lt48_buffer_compare_result *out_result
) {
    return lt48_buffer_compare_range_cancellable(
        buffer, offset, expected, length, NULL, NULL, out_result);
}

lt48_status lt48_buffer_compare_range_cancellable(
    const lt48_buffer *buffer, size_t offset,
    const unsigned char *expected, size_t length,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_buffer_compare_result *out_result
) {
    if (!valid_range(buffer, offset, length) || out_result == NULL ||
        (length != 0u && expected == NULL)) return LT48_INVALID_ARGUMENT;
    if (!known_range(buffer, offset, length)) return LT48_SAFETY_REJECTED;
    memset(out_result, 0, sizeof(*out_result));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    out_result->equal = 1;
    out_result->compared_size = length;
    out_result->first_mismatch_offset = SIZE_MAX;
    out_result->expected_value = 0u;
    out_result->actual_value = 0u;
    for (size_t index = 0u; index < length; ++index) {
        if ((index & 0xffffu) == 0u && cancelled != NULL &&
            cancelled(cancel_context)) {
            memset(out_result, 0, sizeof(*out_result));
            return LT48_CANCELLED;
        }
        if (buffer->data[offset + index] != expected[index]) {
            out_result->equal = 0;
            out_result->first_mismatch_offset = offset + index;
            out_result->expected_value = expected[index];
            out_result->actual_value = buffer->data[offset + index];
            break;
        }
    }
    return LT48_OK;
}

lt48_status lt48_buffer_get_provenance(
    const lt48_buffer *buffer, lt48_buffer_provenance *out_provenance
) {
    if (buffer == NULL || out_provenance == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_provenance = buffer->provenance;
    return LT48_OK;
}

lt48_status lt48_buffer_bind_mutating_input(
    const lt48_buffer *buffer, size_t definition_capacity,
    size_t range_offset, size_t range_length,
    lt48_buffer_binding *out_binding
) {
    static const unsigned char tag_v1[8] = {
        'L','T','4','8','B','U','F','1'
    };
    static const unsigned char tag_v2[8] = {
        'L','T','4','8','B','U','F','2'
    };
    unsigned char canonical[8u + 18u * 8u + 2u * LT48_SHA256_SIZE];
    unsigned char *cursor = canonical;
    const lt48_buffer_provenance *provenance;
    if (buffer == NULL || out_binding == NULL ||
        definition_capacity != buffer->size || range_length == 0u ||
        !valid_range(buffer, range_offset, range_length))
        return LT48_INVALID_ARGUMENT;
    if (!known_range(buffer, 0u, buffer->size)) return LT48_SAFETY_REJECTED;
    memset(out_binding, 0, sizeof(*out_binding));
    out_binding->schema_version =
        buffer->provenance.image_format == LT48_IMAGE_FORMAT_AUTO ? 1u : 2u;
    out_binding->definition_capacity = definition_capacity;
    out_binding->range_offset = range_offset;
    out_binding->range_length = range_length;
    out_binding->provenance = buffer->provenance;
    lt48_sha256(buffer->data, buffer->size, out_binding->image_sha256);
    lt48_sha256(buffer->data + range_offset, range_length,
                out_binding->range_sha256);
    provenance = &out_binding->provenance;
    memcpy(cursor, out_binding->schema_version == 1u ? tag_v1 : tag_v2,
           sizeof(tag_v1));
    cursor += sizeof(tag_v1);
    append_u64(&cursor, out_binding->schema_version);
    append_u64(&cursor, definition_capacity);
    append_u64(&cursor, range_offset);
    append_u64(&cursor, range_length);
    memcpy(cursor, out_binding->image_sha256, LT48_SHA256_SIZE);
    cursor += LT48_SHA256_SIZE;
    memcpy(cursor, out_binding->range_sha256, LT48_SHA256_SIZE);
    cursor += LT48_SHA256_SIZE;
    append_u64(&cursor, (uint64_t)provenance->origin);
    append_u64(&cursor, provenance->input_size);
    append_u64(&cursor, provenance->imported_size);
    append_u64(&cursor, provenance->padded_size);
    append_u64(&cursor, provenance->truncated_size);
    append_u64(&cursor, (uint64_t)provenance->short_input);
    append_u64(&cursor, (uint64_t)provenance->long_input);
    append_u64(&cursor, provenance->fill_value);
    append_u64(&cursor, provenance->word_size_bytes);
    append_u64(&cursor, provenance->reverse_word_bytes ? 1u : 0u);
    append_u64(&cursor, provenance->mutation_count);
    if (out_binding->schema_version == 2u) {
        append_u64(&cursor, (uint64_t)provenance->image_format);
        append_u64(&cursor, provenance->image_address_base);
        append_u64(&cursor, (uint64_t)provenance->image_gap_policy);
    }
    lt48_sha256(canonical, (size_t)(cursor - canonical),
                out_binding->binding_sha256);
    return LT48_OK;
}

lt48_status lt48_buffer_save_file_range(
    const lt48_buffer *buffer, const char *path,
    size_t offset, size_t length, int overwrite
) {
    return lt48_buffer_save_file_range_cancellable(
        buffer, path, offset, length, overwrite, NULL, NULL);
}

lt48_status lt48_buffer_save_file_range_cancellable(
    const lt48_buffer *buffer, const char *path,
    size_t offset, size_t length, int overwrite,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    FILE *file;
    size_t written = 0u;
    lt48_status status = LT48_OK;
    if (!valid_range(buffer, offset, length) || path == NULL || path[0] == '\0')
        return LT48_INVALID_ARGUMENT;
    if (!known_range(buffer, offset, length)) return LT48_SAFETY_REJECTED;
    if (overwrite && cancelled != NULL) return LT48_INVALID_ARGUMENT;
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    file = fopen(path, overwrite ? "wb" : "wbx");
    if (file == NULL) return LT48_FILE_ERROR;
    while (written < length) {
        size_t remaining = length - written;
        size_t request = remaining > 65536u ? 65536u : remaining;
        size_t count;
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        count = fwrite(buffer->data + offset + written, 1u, request, file);
        written += count;
        if (count != request) {
            status = LT48_FILE_ERROR;
            break;
        }
    }
    if (status == LT48_OK && fflush(file) != 0) status = LT48_FILE_ERROR;
    if (fclose(file) != 0) status = LT48_FILE_ERROR;
    if (status != LT48_OK && !overwrite) (void)remove(path);
    return status;
}
