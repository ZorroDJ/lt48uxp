#include <labtool48/labtool48.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct jedec_writer {
    unsigned char *output;
    size_t capacity;
    size_t size;
    uint32_t sum;
    int overflow;
} jedec_writer;

static uint16_t map_checksum(const unsigned char *fuses, size_t count) {
    uint32_t sum = 0u;
    for (size_t base = 0u; base < count; base += 8u) {
        unsigned value = 0u;
        for (size_t bit = 0u; bit < 8u && base + bit < count; ++bit)
            if (fuses[base + bit] != 0u) value |= 1u << bit;
        sum += value;
    }
    return (uint16_t)sum;
}

static int hex_digit(unsigned char value) {
    if (value >= '0' && value <= '9') return value - '0';
    value = (unsigned char)toupper(value);
    return value >= 'A' && value <= 'F' ? value - 'A' + 10 : -1;
}

static int parse_number(
    const unsigned char **cursor, const unsigned char *end, unsigned base,
    uint32_t *out
) {
    uint32_t value = 0u;
    int have = 0;
    while (*cursor < end && isspace((unsigned char)**cursor)) ++*cursor;
    while (*cursor < end) {
        int digit = hex_digit(**cursor);
        if (digit < 0 || (unsigned)digit >= base) break;
        if (value > (UINT32_MAX - (uint32_t)digit) / base) return 0;
        value = value * base + (uint32_t)digit;
        ++*cursor;
        have = 1;
    }
    if (!have) return 0;
    *out = value;
    return 1;
}

static void trim(const unsigned char **start, const unsigned char **end) {
    while (*start < *end && isspace((unsigned char)**start)) ++*start;
    while (*end > *start && isspace((unsigned char)(*end)[-1])) --*end;
}

static lt48_status scan_header(
    const unsigned char *start, const unsigned char *end, size_t expected,
    unsigned char *default_value, int *have_qf
) {
    const unsigned char *record = start;
    *default_value = 0u;
    *have_qf = 0;
    while (record < end) {
        const unsigned char *tail = record;
        const unsigned char *record_end =
            memchr(record, '*', (size_t)(end - record));
        const unsigned char *cursor;
        uint32_t value;
        while (tail < end && isspace((unsigned char)*tail)) ++tail;
        if (tail == end) break;
        if (record_end == NULL) return LT48_FORMAT_ERROR;
        cursor = record;
        trim(&cursor, &record_end);
        if (cursor < record_end && cursor[0] == 'F') {
            ++cursor;
            if (!parse_number(&cursor, record_end, 10u, &value) || value > 1u)
                return LT48_FORMAT_ERROR;
            *default_value = (unsigned char)value;
        } else if ((size_t)(record_end - cursor) >= 2u &&
                   cursor[0] == 'Q' && cursor[1] == 'F') {
            cursor += 2;
            if (!parse_number(&cursor, record_end, 10u, &value) ||
                value != expected)
                return LT48_FORMAT_ERROR;
            *have_qf = 1;
        }
        record = record_end + 1u;
    }
    return LT48_OK;
}

lt48_status lt48_jedec_import(
    const unsigned char *data, size_t data_size,
    unsigned char *fuses, size_t fuse_count, lt48_jedec_info *out_info
) {
    const unsigned char *stx;
    const unsigned char *etx;
    const unsigned char *records;
    const unsigned char *record;
    unsigned char default_value;
    uint32_t declared_map = 0u;
    int have_qf;
    int have_map = 0;
    lt48_status status;
    if (data == NULL || data_size == 0u || fuses == NULL ||
        fuse_count == 0u || out_info == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_info, 0, sizeof(*out_info));
    stx = memchr(data, 0x02, data_size);
    if (stx == NULL) return LT48_FORMAT_ERROR;
    etx = memchr(stx + 1u, 0x03, data_size - (size_t)(stx + 1u - data));
    if (etx == NULL) return LT48_FORMAT_ERROR;
    records = memchr(stx + 1u, '*', (size_t)(etx - (stx + 1u)));
    if (records == NULL) return LT48_FORMAT_ERROR;
    ++records;
    status = scan_header(records, etx, fuse_count,
                         &default_value, &have_qf);
    if (status != LT48_OK || !have_qf) return LT48_FORMAT_ERROR;
    memset(fuses, default_value, fuse_count);
    record = records;
    while (record < etx) {
        const unsigned char *tail = record;
        const unsigned char *record_end =
            memchr(record, '*', (size_t)(etx - record));
        const unsigned char *cursor;
        uint32_t address;
        while (tail < etx && isspace((unsigned char)*tail)) ++tail;
        if (tail == etx) break;
        if (record_end == NULL) return LT48_FORMAT_ERROR;
        cursor = record;
        trim(&cursor, &record_end);
        if (cursor < record_end && (*cursor == 'L' || *cursor == 'K')) {
            unsigned char kind = *cursor++;
            if (!parse_number(&cursor, record_end, 10u, &address) ||
                address >= fuse_count)
                return LT48_FORMAT_ERROR;
            while (cursor < record_end && isspace((unsigned char)*cursor))
                ++cursor;
            while (cursor < record_end) {
                if (isspace((unsigned char)*cursor)) { ++cursor; continue; }
                if (kind == 'L') {
                    if ((*cursor != '0' && *cursor != '1') ||
                        address >= fuse_count)
                        return LT48_FORMAT_ERROR;
                    fuses[address++] = (unsigned char)(*cursor++ - '0');
                } else {
                    int value = hex_digit(*cursor++);
                    if (value < 0 || fuse_count - address < 4u)
                        return LT48_FORMAT_ERROR;
                    for (int bit = 3; bit >= 0; --bit)
                        fuses[address++] = (unsigned char)((value >> bit) & 1);
                }
            }
        } else if (cursor < record_end && *cursor == 'C') {
            ++cursor;
            if (!parse_number(&cursor, record_end, 16u, &declared_map) ||
                declared_map > UINT16_MAX)
                return LT48_FORMAT_ERROR;
            have_map = 1;
        } else if (cursor < record_end && *cursor == 'G') {
            uint32_t security;
            ++cursor;
            if (!parse_number(&cursor, record_end, 10u, &security) ||
                security > 1u)
                return LT48_FORMAT_ERROR;
            out_info->security_enabled = security != 0u;
        }
        record = record_end + 1u;
    }
    out_info->fuse_count = fuse_count;
    out_info->fuse_checksum = map_checksum(fuses, fuse_count);
    out_info->has_fuse_checksum = have_map;
    if (have_map && out_info->fuse_checksum != (uint16_t)declared_map)
        return LT48_FORMAT_ERROR;
    if ((size_t)(etx + 5u - data) <= data_size) {
        const unsigned char *cursor = etx + 1u;
        const unsigned char *checksum_end = cursor + 4u;
        uint32_t declared_transport;
        if (parse_number(&cursor, checksum_end, 16u, &declared_transport) &&
            cursor == checksum_end) {
            uint32_t sum = 0u;
            for (cursor = stx; cursor <= etx; ++cursor) sum += *cursor;
            out_info->transmission_checksum = (uint16_t)sum;
            out_info->has_transmission_checksum = 1;
            if (declared_transport != 0u &&
                declared_transport != out_info->transmission_checksum)
                return LT48_FORMAT_ERROR;
        }
    }
    return LT48_OK;
}

static void writer_bytes(jedec_writer *writer, const void *data, size_t size) {
    const unsigned char *bytes = data;
    if (size > SIZE_MAX - writer->size) { writer->overflow = 1; return; }
    if (writer->output != NULL && writer->size + size <= writer->capacity)
        memcpy(writer->output + writer->size, data, size);
    else if (writer->output != NULL)
        writer->overflow = 1;
    for (size_t index = 0u; index < size; ++index) writer->sum += bytes[index];
    writer->size += size;
}

static void writer_format(jedec_writer *writer, const char *format, ...) {
    char text[64];
    va_list arguments;
    int length;
    va_start(arguments, format);
    length = vsnprintf(text, sizeof(text), format, arguments);
    va_end(arguments);
    if (length < 0 || (size_t)length >= sizeof(text)) {
        writer->overflow = 1;
        return;
    }
    writer_bytes(writer, text, (size_t)length);
}

static lt48_status jedec_export(
    const unsigned char *fuses, size_t fuse_count,
    const lt48_fuse_row *rows, size_t row_count, int security_enabled,
    unsigned char *output, size_t output_capacity, size_t *out_size,
    lt48_jedec_info *out_info
) {
    jedec_writer writer = {output, output_capacity, 0u, 0u, 0};
    uint16_t fuse_sum;
    uint16_t transport_sum;
    const unsigned char stx = 0x02u;
    const unsigned char etx = 0x03u;
    if (fuses == NULL || fuse_count == 0u || rows == NULL || row_count == 0u ||
        out_size == NULL || out_info == NULL ||
        (output == NULL && output_capacity != 0u) ||
        security_enabled < -1 || security_enabled > 1)
        return LT48_INVALID_ARGUMENT;
    memset(out_info, 0, sizeof(*out_info));
    fuse_sum = map_checksum(fuses, fuse_count);
    writer_bytes(&writer, &stx, 1u);
    writer_bytes(
        &writer, "LabTool-48UXP portable JEDEC*\r\n",
        sizeof("LabTool-48UXP portable JEDEC*\r\n") - 1u);
    writer_format(&writer, "QF%zu*\r\nF1*\r\n", fuse_count);
    for (size_t row = 0u; row < row_count; ++row) {
        size_t first = rows[row].first_fuse;
        size_t length = rows[row].length;
        if (first > fuse_count || length > fuse_count - first)
            return LT48_FORMAT_ERROR;
        writer_format(&writer, "L%06u ", rows[row].first_fuse);
        for (size_t bit = 0u; bit < length; ++bit) {
            unsigned char value = fuses[first + bit] ? '1' : '0';
            writer_bytes(&writer, &value, 1u);
        }
        writer_bytes(&writer, "*\r\n", 3u);
    }
    if (security_enabled >= 0)
        writer_format(&writer, "G%d*\r\n", security_enabled);
    writer_format(&writer, "C%04X*\r\n", fuse_sum);
    writer_bytes(&writer, &etx, 1u);
    transport_sum = (uint16_t)writer.sum;
    writer_format(&writer, "%04X", transport_sum);
    *out_size = writer.size;
    out_info->security_enabled = security_enabled == 1;
    out_info->fuse_count = fuse_count;
    out_info->fuse_checksum = fuse_sum;
    out_info->transmission_checksum = transport_sum;
    out_info->has_fuse_checksum = 1;
    out_info->has_transmission_checksum = 1;
    return writer.overflow ? LT48_BUFFER_TOO_SMALL : LT48_OK;
}

lt48_status lt48_jedec_export(
    const unsigned char *fuses, size_t fuse_count,
    const lt48_fuse_row *rows, size_t row_count,
    unsigned char *output, size_t output_capacity, size_t *out_size,
    lt48_jedec_info *out_info
) {
    return jedec_export(fuses, fuse_count, rows, row_count, -1,
                        output, output_capacity, out_size, out_info);
}

lt48_status lt48_jedec_export_with_security(
    const unsigned char *fuses, size_t fuse_count,
    const lt48_fuse_row *rows, size_t row_count, int security_enabled,
    unsigned char *output, size_t output_capacity, size_t *out_size,
    lt48_jedec_info *out_info
) {
    if (security_enabled != 0 && security_enabled != 1)
        return LT48_INVALID_ARGUMENT;
    return jedec_export(fuses, fuse_count, rows, row_count, security_enabled,
                        output, output_capacity, out_size, out_info);
}
