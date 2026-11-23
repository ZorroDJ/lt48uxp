#define _POSIX_C_SOURCE 200809L

#include <labtool48/labtool48.h>

#include "internal.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct image_parser {
    const lt48_image_import_options *options;
    lt48_image_import_result result;
    lt48_buffer *buffer;
    unsigned char *seen;
    int parse_error;
} image_parser;

static int cancelled_now(
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    return cancelled != NULL && cancelled(cancel_context);
}

const char *lt48_image_format_name(lt48_image_format format) {
    switch (format) {
    case LT48_IMAGE_FORMAT_RAW_BINARY: return "raw-binary";
    case LT48_IMAGE_FORMAT_INTEL_HEX: return "intel-hex";
    case LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD: return "motorola-s-record";
    case LT48_IMAGE_FORMAT_STRAIGHT_HEX: return "straight-hex";
    case LT48_IMAGE_FORMAT_MOS_TECHNOLOGY: return "mos-technology";
    case LT48_IMAGE_FORMAT_TEKTRONIX_HEX: return "tektronix-hex";
    case LT48_IMAGE_FORMAT_HP64000_ABSOLUTE: return "hp64000-absolute";
    default: return NULL;
    }
}

static int suffix_equals(const char *path, const char *suffix) {
    size_t path_size = strlen(path);
    size_t suffix_size = strlen(suffix);
    if (path_size < suffix_size) return 0;
    path += path_size - suffix_size;
    for (size_t index = 0u; index < suffix_size; ++index) {
        if (tolower((unsigned char)path[index]) !=
            tolower((unsigned char)suffix[index])) return 0;
    }
    return 1;
}

static lt48_image_format detect_format(const char *path) {
    if (suffix_equals(path, ".hex") || suffix_equals(path, ".ihx"))
        return LT48_IMAGE_FORMAT_INTEL_HEX;
    if (suffix_equals(path, ".srec") || suffix_equals(path, ".s19") ||
        suffix_equals(path, ".s28") || suffix_equals(path, ".s37"))
        return LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD;
    if (suffix_equals(path, ".shx") || suffix_equals(path, ".hexstr"))
        return LT48_IMAGE_FORMAT_STRAIGHT_HEX;
    if (suffix_equals(path, ".mos"))
        return LT48_IMAGE_FORMAT_MOS_TECHNOLOGY;
    if (suffix_equals(path, ".tek") || suffix_equals(path, ".tekhex"))
        return LT48_IMAGE_FORMAT_TEKTRONIX_HEX;
    if (suffix_equals(path, ".abs") || suffix_equals(path, ".hpabs"))
        return LT48_IMAGE_FORMAT_HP64000_ABSOLUTE;
    return LT48_IMAGE_FORMAT_RAW_BINARY;
}

static int hex_nibble(unsigned char value) {
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

static int hex_byte(const char *text, unsigned char *out_value) {
    int high = hex_nibble((unsigned char)text[0]);
    int low = hex_nibble((unsigned char)text[1]);
    if (high < 0 || low < 0) return 0;
    *out_value = (unsigned char)((high << 4) | low);
    return 1;
}

static void note_source_byte(image_parser *parser, uint64_t address) {
    if (!parser->result.has_data) {
        parser->result.minimum_source_address = address;
        parser->result.maximum_source_address_exclusive = address + 1u;
        parser->result.has_data = 1;
    } else {
        if (address < parser->result.minimum_source_address)
            parser->result.minimum_source_address = address;
        if (address != UINT64_MAX &&
            address + 1u > parser->result.maximum_source_address_exclusive)
            parser->result.maximum_source_address_exclusive = address + 1u;
    }
}

static void place_byte(
    image_parser *parser, uint64_t address, unsigned char value
) {
    uint64_t offset;
    ++parser->result.encoded_data_byte_count;
    note_source_byte(parser, address);
    if (address < parser->options->address_base) {
        ++parser->result.out_of_range_byte_count;
        return;
    }
    offset = address - parser->options->address_base;
    if (offset >= parser->options->buffer_size) {
        ++parser->result.out_of_range_byte_count;
        return;
    }
    if (parser->seen[offset] != 0u) {
        ++parser->result.overlap_byte_count;
        return;
    }
    parser->seen[offset] = 1u;
    if (lt48_buffer_write(
            parser->buffer, (size_t)offset, &value, 1u) != LT48_OK) {
        parser->parse_error = 1;
        return;
    }
    ++parser->result.mapped_data_byte_count;
}

static size_t trim_line(char *line, size_t size) {
    while (size != 0u && (line[size - 1u] == '\n' ||
                          line[size - 1u] == '\r')) --size;
    line[size] = '\0';
    return size;
}

static lt48_status parse_intel_hex(
    FILE *file, image_parser *parser,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    char *line = NULL;
    size_t capacity = 0u;
    ssize_t received;
    uint64_t address_prefix = 0u;
    int eof_seen = 0;
    lt48_status status = LT48_OK;
    while ((received = getline(&line, &capacity, file)) >= 0) {
        unsigned char bytes[260];
        size_t size = trim_line(line, (size_t)received);
        unsigned char count;
        unsigned char type;
        uint16_t address;
        unsigned sum = 0u;
        if (cancelled_now(cancelled, cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        parser->result.input_size += (size_t)received;
        if (size == 0u) continue;
        if (eof_seen || size < 11u || line[0] != ':' ||
            !hex_byte(line + 1, &count) ||
            size != 11u + (size_t)count * 2u) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        for (size_t index = 0u; index < (size_t)count + 5u; ++index) {
            if (!hex_byte(line + 1u + index * 2u, &bytes[index])) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            sum += bytes[index];
        }
        if (status != LT48_OK) break;
        if ((sum & 0xffu) != 0u) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        address = (uint16_t)(((uint16_t)bytes[1] << 8) | bytes[2]);
        type = bytes[3];
        ++parser->result.record_count;
        if (type == 0u) {
            if ((uint64_t)address > UINT64_MAX - address_prefix ||
                (uint64_t)count >
                    UINT64_MAX - (address_prefix + address)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            ++parser->result.data_record_count;
            for (size_t index = 0u; index < count; ++index)
                place_byte(parser, address_prefix + address + index,
                           bytes[4u + index]);
        } else if (type == 1u) {
            if (count != 0u || address != 0u) status = LT48_FORMAT_ERROR;
            else eof_seen = 1;
        } else if (type == 2u || type == 4u) {
            if (count != 2u || address != 0u) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            address_prefix =
                ((uint64_t)bytes[4] << 8 | bytes[5]) <<
                (type == 2u ? 4u : 16u);
        } else if (type == 3u || type == 5u) {
            if (count != 4u || address != 0u ||
                parser->result.has_execution_start) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            parser->result.has_execution_start = 1;
            if (type == 3u) {
                uint32_t cs = (uint32_t)bytes[4] << 8 | bytes[5];
                uint32_t ip = (uint32_t)bytes[6] << 8 | bytes[7];
                parser->result.execution_start_address =
                    (uint64_t)cs * 16u + ip;
            } else {
                parser->result.execution_start_address =
                    (uint64_t)bytes[4] << 24 | (uint64_t)bytes[5] << 16 |
                    (uint64_t)bytes[6] << 8 | bytes[7];
            }
        } else {
            status = LT48_UNSUPPORTED;
            break;
        }
        if (parser->parse_error) {
            status = LT48_RUNTIME_ERROR;
            break;
        }
    }
    if (status == LT48_OK && ferror(file)) status = LT48_FILE_ERROR;
    if (status == LT48_OK && !eof_seen) status = LT48_FORMAT_ERROR;
    free(line);
    return status;
}

static unsigned srec_address_size(unsigned type) {
    switch (type) {
    case 0u: case 1u: case 5u: case 9u: return 2u;
    case 2u: case 6u: case 8u: return 3u;
    case 3u: case 7u: return 4u;
    default: return 0u;
    }
}

static lt48_status parse_s_record(
    FILE *file, image_parser *parser,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    char *line = NULL;
    size_t capacity = 0u;
    ssize_t received;
    size_t data_records = 0u;
    uint64_t declared_records = UINT64_MAX;
    unsigned data_type = 0u;
    int header_seen = 0;
    int termination_seen = 0;
    lt48_status status = LT48_OK;
    while ((received = getline(&line, &capacity, file)) >= 0) {
        unsigned char bytes[260];
        size_t size = trim_line(line, (size_t)received);
        unsigned type;
        unsigned address_size;
        unsigned char count;
        uint64_t address = 0u;
        unsigned sum = 0u;
        size_t data_size;
        if (cancelled_now(cancelled, cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        parser->result.input_size += (size_t)received;
        if (size == 0u) continue;
        if (termination_seen || size < 10u || line[0] != 'S' ||
            line[1] < '0' || line[1] > '9' ||
            !hex_byte(line + 2, &count) ||
            size != 4u + (size_t)count * 2u) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        type = (unsigned)(line[1] - '0');
        address_size = srec_address_size(type);
        if (address_size == 0u || count < address_size + 1u) {
            status = type == 4u ? LT48_UNSUPPORTED : LT48_FORMAT_ERROR;
            break;
        }
        for (size_t index = 0u; index < count; ++index) {
            if (!hex_byte(line + 4u + index * 2u, &bytes[index])) {
                status = LT48_FORMAT_ERROR;
                break;
            }
        }
        if (status != LT48_OK) break;
        sum = count;
        for (size_t index = 0u; index < count; ++index) sum += bytes[index];
        if ((sum & 0xffu) != 0xffu) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        for (unsigned index = 0u; index < address_size; ++index)
            address = address << 8 | bytes[index];
        data_size = (size_t)count - address_size - 1u;
        ++parser->result.record_count;
        if (type == 0u) {
            if (header_seen || data_records != 0u) status = LT48_FORMAT_ERROR;
            header_seen = 1;
        } else if (type >= 1u && type <= 3u) {
            if (data_type != 0u && data_type != type) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            data_type = type;
            ++data_records;
            ++parser->result.data_record_count;
            for (size_t index = 0u; index < data_size; ++index) {
                if (address > UINT64_MAX - index) {
                    status = LT48_FORMAT_ERROR;
                    break;
                }
                place_byte(parser, address + index,
                           bytes[address_size + index]);
            }
        } else if (type == 5u || type == 6u) {
            if (data_size != 0u || declared_records != UINT64_MAX)
                status = LT48_FORMAT_ERROR;
            else declared_records = address;
        } else if (type >= 7u && type <= 9u) {
            unsigned expected = data_type == 1u ? 9u :
                                data_type == 2u ? 8u :
                                data_type == 3u ? 7u : 0u;
            if (data_size != 0u || type != expected) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            termination_seen = 1;
            parser->result.has_execution_start = 1;
            parser->result.execution_start_address = address;
        } else {
            status = LT48_UNSUPPORTED;
        }
        if (status != LT48_OK || parser->parse_error) {
            if (status == LT48_OK) status = LT48_RUNTIME_ERROR;
            break;
        }
    }
    if (status == LT48_OK && ferror(file)) status = LT48_FILE_ERROR;
    if (status == LT48_OK && (!termination_seen || data_records == 0u ||
        (declared_records != UINT64_MAX && declared_records != data_records)))
        status = LT48_FORMAT_ERROR;
    free(line);
    return status;
}

static lt48_status parse_raw(
    FILE *file, image_parser *parser,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    unsigned char data[8192];
    uint64_t address = parser->options->address_base;
    for (;;) {
        size_t count;
        if (cancelled_now(cancelled, cancel_context)) return LT48_CANCELLED;
        count = fread(data, 1u, sizeof(data), file);
        if (count > SIZE_MAX - parser->result.input_size)
            return LT48_FORMAT_ERROR;
        parser->result.input_size += count;
        if (count != 0u) {
            ++parser->result.record_count;
            ++parser->result.data_record_count;
            for (size_t index = 0u; index < count; ++index) {
                if (address == UINT64_MAX && index != 0u)
                    return LT48_FORMAT_ERROR;
                place_byte(parser, address, data[index]);
                if (address != UINT64_MAX) ++address;
            }
        }
        if (count != sizeof(data)) {
            if (ferror(file)) return LT48_FILE_ERROR;
            break;
        }
    }
    return parser->result.has_data ? LT48_OK : LT48_FORMAT_ERROR;
}

static lt48_status parse_straight_hex(
    FILE *file, image_parser *parser,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    char *line = NULL;
    size_t capacity = 0u;
    ssize_t received;
    uint64_t address = parser->options->address_base;
    int address_exhausted = 0;
    lt48_status status = LT48_OK;
    while ((received = getline(&line, &capacity, file)) >= 0) {
        int high = -1;
        size_t line_bytes = 0u;
        if (cancelled_now(cancelled, cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        if ((size_t)received > SIZE_MAX - parser->result.input_size) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        parser->result.input_size += (size_t)received;
        for (ssize_t index = 0; index < received; ++index) {
            unsigned char character = (unsigned char)line[index];
            int nibble;
            if (isspace(character)) continue;
            nibble = hex_nibble(character);
            if (nibble < 0) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            if (high < 0) {
                high = nibble;
            } else {
                unsigned char value = (unsigned char)((high << 4) | nibble);
                if (address_exhausted) {
                    status = LT48_FORMAT_ERROR;
                    break;
                }
                place_byte(parser, address, value);
                if (parser->parse_error) {
                    status = LT48_RUNTIME_ERROR;
                    break;
                }
                if (address != UINT64_MAX) ++address;
                else address_exhausted = 1;
                high = -1;
                ++line_bytes;
            }
        }
        if (status != LT48_OK) break;
        if (high >= 0) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        if (line_bytes != 0u) {
            ++parser->result.record_count;
            ++parser->result.data_record_count;
        }
    }
    if (status == LT48_OK && ferror(file)) status = LT48_FILE_ERROR;
    if (status == LT48_OK && !parser->result.has_data)
        status = LT48_FORMAT_ERROR;
    free(line);
    return status;
}

static lt48_status parse_mos_technology(
    FILE *file, image_parser *parser,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    char *line = NULL;
    size_t capacity = 0u;
    ssize_t received;
    int termination_seen = 0;
    lt48_status status = LT48_OK;
    while ((received = getline(&line, &capacity, file)) >= 0) {
        char compact[523];
        size_t compact_size = 0u;
        unsigned char count;
        unsigned char address_high;
        unsigned char address_low;
        uint16_t address;
        uint16_t expected_checksum;
        unsigned checksum;
        if (cancelled_now(cancelled, cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        if ((size_t)received > SIZE_MAX - parser->result.input_size) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        parser->result.input_size += (size_t)received;
        for (ssize_t index = 0; index < received; ++index) {
            unsigned char character = (unsigned char)line[index];
            if (isspace(character)) continue;
            if (compact_size + 1u >= sizeof(compact)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            compact[compact_size++] = (char)character;
        }
        if (status != LT48_OK) break;
        if (compact_size == 0u) continue;
        compact[compact_size] = '\0';
        if (termination_seen || compact[0] != ';' || compact_size < 11u ||
            !hex_byte(compact + 1, &count) ||
            compact_size != 11u + (size_t)count * 2u ||
            !hex_byte(compact + 3, &address_high) ||
            !hex_byte(compact + 5, &address_low)) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        address = (uint16_t)((uint16_t)address_high << 8 | address_low);
        checksum = count + address_high + address_low;
        for (size_t index = 0u; index < count; ++index) {
            unsigned char value;
            if (!hex_byte(compact + 7u + index * 2u, &value)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            checksum += value;
            place_byte(parser, (uint64_t)address + index, value);
        }
        if (status != LT48_OK || parser->parse_error) {
            if (status == LT48_OK) status = LT48_RUNTIME_ERROR;
            break;
        }
        {
            unsigned char checksum_high;
            unsigned char checksum_low;
            const char *checksum_text = compact + 7u + (size_t)count * 2u;
            if (!hex_byte(checksum_text, &checksum_high) ||
                !hex_byte(checksum_text + 2, &checksum_low)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            expected_checksum = (uint16_t)(
                (uint16_t)checksum_high << 8 | checksum_low);
        }
        if ((uint16_t)checksum != expected_checksum) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        ++parser->result.record_count;
        if (count == 0u) {
            termination_seen = 1;
        } else {
            if ((uint32_t)address + count > UINT32_C(0x10000)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            ++parser->result.data_record_count;
        }
    }
    if (status == LT48_OK && ferror(file)) status = LT48_FILE_ERROR;
    if (status == LT48_OK && (!termination_seen || !parser->result.has_data))
        status = LT48_FORMAT_ERROR;
    free(line);
    return status;
}

static unsigned hex_nibble_sum(const char *text, size_t count) {
    unsigned sum = 0u;
    for (size_t index = 0u; index < count; ++index) {
        int nibble = hex_nibble((unsigned char)text[index]);
        if (nibble < 0) return UINT_MAX;
        sum += (unsigned)nibble;
    }
    return sum;
}

static lt48_status parse_tektronix_hex(
    FILE *file, image_parser *parser,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    char *line = NULL;
    size_t capacity = 0u;
    ssize_t received;
    int termination_seen = 0;
    lt48_status status = LT48_OK;
    while ((received = getline(&line, &capacity, file)) >= 0) {
        char compact[523];
        size_t compact_size = 0u;
        unsigned char address_high;
        unsigned char address_low;
        unsigned char count;
        unsigned char header_checksum;
        unsigned char data_checksum;
        uint16_t address;
        unsigned sum;
        if (cancelled_now(cancelled, cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        if ((size_t)received > SIZE_MAX - parser->result.input_size) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        parser->result.input_size += (size_t)received;
        for (ssize_t index = 0; index < received; ++index) {
            unsigned char character = (unsigned char)line[index];
            if (isspace(character)) continue;
            if (compact_size + 1u >= sizeof(compact)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            compact[compact_size++] = (char)character;
        }
        if (status != LT48_OK) break;
        if (compact_size == 0u) continue;
        compact[compact_size] = '\0';
        if (termination_seen || compact[0] != '/' || compact_size < 11u ||
            !hex_byte(compact + 1, &address_high) ||
            !hex_byte(compact + 3, &address_low) ||
            !hex_byte(compact + 5, &count) ||
            compact_size != 11u + (size_t)count * 2u ||
            !hex_byte(compact + 7, &header_checksum)) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        sum = hex_nibble_sum(compact + 1, 6u);
        if (sum == UINT_MAX || (unsigned char)sum != header_checksum) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        address = (uint16_t)((uint16_t)address_high << 8 | address_low);
        for (size_t index = 0u; index < count; ++index) {
            unsigned char value;
            if (!hex_byte(compact + 9u + index * 2u, &value)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            place_byte(parser, (uint64_t)address + index, value);
        }
        if (status != LT48_OK || parser->parse_error) {
            if (status == LT48_OK) status = LT48_RUNTIME_ERROR;
            break;
        }
        if (!hex_byte(compact + 9u + (size_t)count * 2u,
                      &data_checksum)) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        sum = hex_nibble_sum(compact + 9u, (size_t)count * 2u);
        if (sum == UINT_MAX || (unsigned char)sum != data_checksum) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        ++parser->result.record_count;
        if (count == 0u) {
            termination_seen = 1;
        } else {
            if ((uint32_t)address + count > UINT32_C(0x10000)) {
                status = LT48_FORMAT_ERROR;
                break;
            }
            ++parser->result.data_record_count;
        }
    }
    if (status == LT48_OK && ferror(file)) status = LT48_FILE_ERROR;
    if (status == LT48_OK && (!termination_seen || !parser->result.has_data))
        status = LT48_FORMAT_ERROR;
    free(line);
    return status;
}

static uint16_t read_be16(const unsigned char *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static uint32_t read_hp64000_address(const unsigned char *bytes) {
    uint32_t low = read_be16(bytes);
    uint32_t high = read_be16(bytes + 2);
    return low | high << 16;
}

static void write_be16(unsigned char *bytes, uint16_t value) {
    bytes[0] = (unsigned char)(value >> 8);
    bytes[1] = (unsigned char)value;
}

static void write_hp64000_address(unsigned char *bytes, uint32_t address) {
    write_be16(bytes, (uint16_t)address);
    write_be16(bytes + 2, (uint16_t)(address >> 16));
}

static lt48_status parse_hp64000_absolute(
    FILE *file, image_parser *parser,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    unsigned char header[28];
    unsigned char record[257];
    size_t received;
    lt48_status status = LT48_OK;
    received = fread(header, 1u, sizeof(header), file);
    parser->result.input_size += received;
    if (received != sizeof(header))
        return ferror(file) ? LT48_FILE_ERROR : LT48_FORMAT_ERROR;
    if (header[18] != 0u || read_be16(header + 18) != 7u ||
        read_be16(header + 20) != 8u || read_be16(header + 22) != 8u)
        return LT48_FORMAT_ERROR;
    parser->result.has_execution_start = 1;
    parser->result.execution_start_address =
        read_hp64000_address(header + 24);
    ++parser->result.record_count;
    for (;;) {
        unsigned char size_bytes[2];
        uint16_t record_size;
        uint16_t count;
        uint32_t address;
        if (cancelled_now(cancelled, cancel_context)) {
            status = LT48_CANCELLED;
            break;
        }
        received = fread(size_bytes, 1u, sizeof(size_bytes), file);
        parser->result.input_size += received;
        if (received == 0u && feof(file)) break;
        if (received != sizeof(size_bytes)) {
            status = ferror(file) ? LT48_FILE_ERROR : LT48_FORMAT_ERROR;
            break;
        }
        record_size = read_be16(size_bytes);
        if (record_size < 5u || record_size > 255u) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        received = fread(record, 1u, (size_t)record_size + 1u, file);
        parser->result.input_size += received;
        if (received != (size_t)record_size + 1u) {
            status = ferror(file) ? LT48_FILE_ERROR : LT48_FORMAT_ERROR;
            break;
        }
        count = read_be16(record);
        if (count > 250u || record_size != count + 5u) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        address = read_hp64000_address(record + 2);
        if ((uint64_t)address + count > UINT64_C(0x100000000)) {
            status = LT48_FORMAT_ERROR;
            break;
        }
        for (size_t index = 0u; index < count; ++index)
            place_byte(parser, (uint64_t)address + index, record[6u + index]);
        if (parser->parse_error) {
            status = LT48_RUNTIME_ERROR;
            break;
        }
        ++parser->result.record_count;
        ++parser->result.data_record_count;
    }
    if (status == LT48_OK && ferror(file)) status = LT48_FILE_ERROR;
    if (status == LT48_OK && !parser->result.has_data)
        status = LT48_FORMAT_ERROR;
    return status;
}

static void count_gaps(image_parser *parser) {
    int in_gap = 0;
    for (size_t index = 0u; index < parser->options->buffer_size; ++index) {
        if (parser->seen[index] == 0u) {
            ++parser->result.gap_byte_count;
            if (!in_gap) {
                ++parser->result.gap_count;
                in_gap = 1;
            }
        } else {
            in_gap = 0;
        }
    }
}

lt48_status lt48_buffer_load_image(
    const char *path, const lt48_image_import_options *options,
    lt48_image_import_result *out_result, lt48_buffer **out_buffer
) {
    return lt48_buffer_load_image_cancellable(
        path, options, NULL, NULL, out_result, out_buffer
    );
}

lt48_status lt48_buffer_load_image_cancellable(
    const char *path, const lt48_image_import_options *options,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_image_import_result *out_result, lt48_buffer **out_buffer
) {
    image_parser parser;
    lt48_image_import_options resolved;
    FILE *file = NULL;
    lt48_status status;
    if (path == NULL || path[0] == '\0' || options == NULL ||
        out_result == NULL || out_buffer == NULL ||
        options->buffer_size == 0u ||
        options->address_base > UINT64_MAX - options->buffer_size ||
        (options->format < LT48_IMAGE_FORMAT_AUTO ||
         options->format > LT48_IMAGE_FORMAT_HP64000_ABSOLUTE) ||
        (options->gap_policy != LT48_IMAGE_GAPS_PRESERVE &&
         options->gap_policy != LT48_IMAGE_GAPS_FILL))
        return LT48_INVALID_ARGUMENT;
    memset(&parser, 0, sizeof(parser));
    memset(out_result, 0, sizeof(*out_result));
    *out_buffer = NULL;
    resolved = *options;
    if (resolved.format == LT48_IMAGE_FORMAT_AUTO)
        resolved.format = detect_format(path);
    parser.options = &resolved;
    parser.result.format = resolved.format;
    parser.seen = calloc(resolved.buffer_size, 1u);
    if (parser.seen == NULL) return LT48_NO_MEMORY;
    status = resolved.gap_policy == LT48_IMAGE_GAPS_FILL ?
        lt48_buffer_create(resolved.buffer_size, resolved.gap_fill_value,
                           &parser.buffer) :
        lt48_buffer_create_sparse(resolved.buffer_size, &parser.buffer);
    if (status != LT48_OK) goto done;
    if (cancelled_now(cancelled, cancel_context)) {
        status = LT48_CANCELLED;
        goto done;
    }
    file = fopen(path,
        resolved.format == LT48_IMAGE_FORMAT_RAW_BINARY ||
        resolved.format == LT48_IMAGE_FORMAT_HP64000_ABSOLUTE ? "rb" : "r");
    if (file == NULL) {
        status = LT48_FILE_ERROR;
        goto done;
    }
    if (resolved.format == LT48_IMAGE_FORMAT_RAW_BINARY)
        status = parse_raw(file, &parser, cancelled, cancel_context);
    else if (resolved.format == LT48_IMAGE_FORMAT_INTEL_HEX)
        status = parse_intel_hex(file, &parser, cancelled, cancel_context);
    else if (resolved.format == LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD)
        status = parse_s_record(file, &parser, cancelled, cancel_context);
    else if (resolved.format == LT48_IMAGE_FORMAT_STRAIGHT_HEX)
        status = parse_straight_hex(
            file, &parser, cancelled, cancel_context);
    else if (resolved.format == LT48_IMAGE_FORMAT_MOS_TECHNOLOGY)
        status = parse_mos_technology(
            file, &parser, cancelled, cancel_context);
    else if (resolved.format == LT48_IMAGE_FORMAT_TEKTRONIX_HEX)
        status = parse_tektronix_hex(
            file, &parser, cancelled, cancel_context);
    else
        status = parse_hp64000_absolute(
            file, &parser, cancelled, cancel_context);
    if (fclose(file) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    file = NULL;
    count_gaps(&parser);
    if (status == LT48_OK && parser.result.overlap_byte_count != 0u)
        status = LT48_FORMAT_ERROR;
    if (status == LT48_OK && parser.result.out_of_range_byte_count != 0u)
        status = LT48_SAFETY_REJECTED;
    if (status == LT48_OK && !parser.result.has_data)
        status = LT48_FORMAT_ERROR;
    if (status == LT48_OK) {
        lt48_buffer_set_image_provenance_internal(
            parser.buffer, &resolved, &parser.result
        );
        *out_buffer = parser.buffer;
        parser.buffer = NULL;
    }

done:
    if (file != NULL) fclose(file);
    *out_result = parser.result;
    lt48_buffer_close(parser.buffer);
    free(parser.seen);
    return status;
}

static int image_export_lane(
    lt48_image_byte_mode mode, size_t *out_phase, size_t *out_stride
) {
    switch (mode) {
    case LT48_IMAGE_BYTES_NORMAL: *out_phase = 0u; *out_stride = 1u; return 1;
    case LT48_IMAGE_BYTES_EVEN: *out_phase = 0u; *out_stride = 2u; return 1;
    case LT48_IMAGE_BYTES_ODD: *out_phase = 1u; *out_stride = 2u; return 1;
    case LT48_IMAGE_BYTES_LANE_0_OF_4:
        *out_phase = 0u; *out_stride = 4u; return 1;
    case LT48_IMAGE_BYTES_LANE_1_OF_4:
        *out_phase = 1u; *out_stride = 4u; return 1;
    case LT48_IMAGE_BYTES_LANE_2_OF_4:
        *out_phase = 2u; *out_stride = 4u; return 1;
    case LT48_IMAGE_BYTES_LANE_3_OF_4:
        *out_phase = 3u; *out_stride = 4u; return 1;
    default: return 0;
    }
}

static lt48_status image_export_check_known(
    const lt48_buffer *buffer, size_t offset, size_t length,
    size_t phase, size_t stride,
    lt48_cancel_check_fn cancelled, void *cancel_context
) {
    for (size_t index = phase; index < length; index += stride) {
        int known = 0;
        if (cancelled_now(cancelled, cancel_context)) return LT48_CANCELLED;
        if (lt48_buffer_range_known(
                buffer, offset + index, 1u, &known) != LT48_OK)
            return LT48_INVALID_ARGUMENT;
        if (!known) return LT48_SAFETY_REJECTED;
    }
    return LT48_OK;
}

static unsigned char image_export_byte(
    const lt48_buffer *buffer, size_t offset, size_t phase,
    size_t stride, size_t output_index
) {
    return lt48_buffer_data(buffer)[offset + phase + output_index * stride];
}

static lt48_status write_intel_record(
    FILE *file, unsigned char type, uint16_t address,
    const unsigned char *data, size_t size
) {
    unsigned sum = (unsigned)size + (address >> 8) + (address & 0xffu) + type;
    if (fprintf(file, ":%02X%04X%02X", (unsigned)size,
                (unsigned)address, (unsigned)type) < 0)
        return LT48_FILE_ERROR;
    for (size_t index = 0u; index < size; ++index) {
        sum += data[index];
        if (fprintf(file, "%02X", (unsigned)data[index]) < 0)
            return LT48_FILE_ERROR;
    }
    if (fprintf(file, "%02X\n", (unsigned)((0u - sum) & 0xffu)) < 0)
        return LT48_FILE_ERROR;
    return LT48_OK;
}

static lt48_status write_s_record(
    FILE *file, unsigned type, uint32_t address, unsigned address_size,
    const unsigned char *data, size_t size
) {
    unsigned count = (unsigned)size + address_size + 1u;
    unsigned sum = count;
    if (fprintf(file, "S%u%02X", type, count) < 0) return LT48_FILE_ERROR;
    for (unsigned index = 0u; index < address_size; ++index) {
        unsigned shift = (address_size - index - 1u) * 8u;
        unsigned value = (address >> shift) & 0xffu;
        sum += value;
        if (fprintf(file, "%02X", value) < 0) return LT48_FILE_ERROR;
    }
    for (size_t index = 0u; index < size; ++index) {
        sum += data[index];
        if (fprintf(file, "%02X", (unsigned)data[index]) < 0)
            return LT48_FILE_ERROR;
    }
    if (fprintf(file, "%02X\n", (unsigned)(~sum & 0xffu)) < 0)
        return LT48_FILE_ERROR;
    return LT48_OK;
}

lt48_status lt48_buffer_save_image(
    const lt48_buffer *buffer, const char *path,
    const lt48_image_export_options *options,
    lt48_image_export_result *out_result
) {
    return lt48_buffer_save_image_cancellable(
        buffer, path, options, NULL, NULL, out_result);
}

lt48_status lt48_buffer_save_image_cancellable(
    const lt48_buffer *buffer, const char *path,
    const lt48_image_export_options *options,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_image_export_result *out_result
) {
    lt48_image_export_options resolved;
    lt48_image_export_result result;
    FILE *file = NULL;
    size_t phase;
    size_t stride;
    size_t output_size;
    lt48_status status;
    if (buffer == NULL || path == NULL || path[0] == '\0' ||
        options == NULL || out_result == NULL ||
        options->buffer_length == 0u ||
        options->buffer_offset > lt48_buffer_size(buffer) ||
        options->buffer_length >
            lt48_buffer_size(buffer) - options->buffer_offset ||
        !image_export_lane(options->byte_mode, &phase, &stride) ||
        options->buffer_length <= phase ||
        (options->format != LT48_IMAGE_FORMAT_AUTO &&
         options->format != LT48_IMAGE_FORMAT_RAW_BINARY &&
         options->format != LT48_IMAGE_FORMAT_INTEL_HEX &&
         options->format != LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD &&
         options->format != LT48_IMAGE_FORMAT_STRAIGHT_HEX &&
         options->format != LT48_IMAGE_FORMAT_MOS_TECHNOLOGY &&
         options->format != LT48_IMAGE_FORMAT_TEKTRONIX_HEX &&
         options->format != LT48_IMAGE_FORMAT_HP64000_ABSOLUTE) ||
        (options->overwrite != 0 && options->overwrite != 1) ||
        (options->overwrite && cancelled != NULL))
        return LT48_INVALID_ARGUMENT;
    memset(&result, 0, sizeof(result));
    *out_result = result;
    resolved = *options;
    if (resolved.format == LT48_IMAGE_FORMAT_AUTO)
        resolved.format = detect_format(path);
    output_size = (resolved.buffer_length - phase + stride - 1u) / stride;
    if (resolved.address_base > UINT64_MAX - output_size ||
        (resolved.format != LT48_IMAGE_FORMAT_RAW_BINARY &&
         resolved.format != LT48_IMAGE_FORMAT_STRAIGHT_HEX &&
         resolved.address_base + output_size > UINT64_C(0x100000000)))
        return LT48_INVALID_ARGUMENT;
    if (resolved.format == LT48_IMAGE_FORMAT_MOS_TECHNOLOGY &&
        (resolved.address_base > UINT16_MAX ||
         resolved.address_base + output_size > UINT32_C(0x10000)))
        return LT48_INVALID_ARGUMENT;
    if (resolved.format == LT48_IMAGE_FORMAT_TEKTRONIX_HEX &&
        (resolved.address_base > UINT16_MAX ||
         resolved.address_base + output_size > UINT32_C(0x10000)))
        return LT48_INVALID_ARGUMENT;
    status = image_export_check_known(
        buffer, resolved.buffer_offset, resolved.buffer_length, phase, stride,
        cancelled, cancel_context);
    if (status != LT48_OK) return status;
    if (cancelled_now(cancelled, cancel_context)) return LT48_CANCELLED;
    file = fopen(path, resolved.overwrite ? "wb" : "wbx");
    if (file == NULL) return LT48_FILE_ERROR;
    result.format = resolved.format;
    result.source_byte_count = resolved.buffer_length;
    result.output_data_byte_count = output_size;
    result.minimum_output_address = resolved.address_base;
    result.maximum_output_address_exclusive =
        resolved.address_base + output_size;
    if (resolved.format == LT48_IMAGE_FORMAT_RAW_BINARY) {
        for (size_t index = 0u; index < output_size; ++index) {
            if (cancelled_now(cancelled, cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            if (fputc(image_export_byte(
                    buffer, resolved.buffer_offset, phase, stride, index),
                    file) == EOF) {
                status = LT48_FILE_ERROR;
                break;
            }
        }
        if (status == LT48_OK) result.record_count = output_size != 0u;
    } else if (resolved.format == LT48_IMAGE_FORMAT_INTEL_HEX) {
        size_t index = 0u;
        uint32_t previous_upper = UINT32_MAX;
        while (index < output_size && status == LT48_OK) {
            unsigned char bytes[16];
            uint32_t address = (uint32_t)(resolved.address_base + index);
            uint32_t upper = address >> 16;
            size_t count = output_size - index;
            size_t before_boundary = UINT32_C(0x10000) - (address & 0xffffu);
            if (cancelled_now(cancelled, cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            if (upper != previous_upper) {
                unsigned char extended[2] = {
                    (unsigned char)(upper >> 8), (unsigned char)upper
                };
                status = write_intel_record(file, 4u, 0u, extended, 2u);
                if (status != LT48_OK) break;
                ++result.record_count;
                previous_upper = upper;
            }
            if (count > sizeof(bytes)) count = sizeof(bytes);
            if (count > before_boundary) count = before_boundary;
            for (size_t byte = 0u; byte < count; ++byte)
                bytes[byte] = image_export_byte(
                    buffer, resolved.buffer_offset, phase, stride,
                    index + byte);
            status = write_intel_record(
                file, 0u, (uint16_t)address, bytes, count);
            if (status == LT48_OK) ++result.record_count;
            index += count;
        }
        if (status == LT48_OK) {
            status = write_intel_record(file, 1u, 0u, NULL, 0u);
            if (status == LT48_OK) ++result.record_count;
        }
    } else if (resolved.format == LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD) {
        uint64_t maximum = resolved.address_base + output_size - 1u;
        unsigned address_size = maximum <= UINT16_MAX ? 2u :
                                maximum <= UINT32_C(0xffffff) ? 3u : 4u;
        unsigned data_type = address_size == 2u ? 1u :
                             address_size == 3u ? 2u : 3u;
        unsigned end_type = address_size == 2u ? 9u :
                            address_size == 3u ? 8u : 7u;
        size_t index = 0u;
        while (index < output_size && status == LT48_OK) {
            unsigned char bytes[16];
            size_t count = output_size - index;
            if (cancelled_now(cancelled, cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            if (count > sizeof(bytes)) count = sizeof(bytes);
            for (size_t byte = 0u; byte < count; ++byte)
                bytes[byte] = image_export_byte(
                    buffer, resolved.buffer_offset, phase, stride,
                    index + byte);
            status = write_s_record(
                file, data_type,
                (uint32_t)(resolved.address_base + index), address_size,
                bytes, count);
            if (status == LT48_OK) ++result.record_count;
            index += count;
        }
        if (status == LT48_OK) {
            status = write_s_record(
                file, end_type, (uint32_t)resolved.address_base,
                address_size, NULL, 0u);
            if (status == LT48_OK) ++result.record_count;
        }
    } else if (resolved.format == LT48_IMAGE_FORMAT_STRAIGHT_HEX) {
        for (size_t index = 0u; index < output_size && status == LT48_OK;
             ++index) {
            if (cancelled_now(cancelled, cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            if (fprintf(file, "%02X", (unsigned)image_export_byte(
                    buffer, resolved.buffer_offset, phase, stride, index)) < 0) {
                status = LT48_FILE_ERROR;
                break;
            }
            if ((index + 1u) % 16u != 0u && index + 1u != output_size &&
                fputc(' ', file) == EOF) {
                status = LT48_FILE_ERROR;
                break;
            }
            if ((index + 1u) % 16u == 0u || index + 1u == output_size) {
                if (fputs("\r\n", file) == EOF) {
                    status = LT48_FILE_ERROR;
                    break;
                }
                ++result.record_count;
            }
        }
    } else if (resolved.format == LT48_IMAGE_FORMAT_MOS_TECHNOLOGY) {
        size_t index = 0u;
        unsigned data_records = 0u;
        while (index < output_size && status == LT48_OK) {
            size_t count = output_size - index;
            uint16_t address = (uint16_t)(resolved.address_base + index);
            unsigned checksum;
            if (cancelled_now(cancelled, cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            if (count > 32u) count = 32u;
            checksum = (unsigned)count + (address >> 8) + (address & 0xffu);
            if (fprintf(file, ";%02X%04X", (unsigned)count,
                        (unsigned)address) < 0) {
                status = LT48_FILE_ERROR;
                break;
            }
            for (size_t byte = 0u; byte < count; ++byte) {
                unsigned value = image_export_byte(
                    buffer, resolved.buffer_offset, phase, stride,
                    index + byte);
                checksum += value;
                if (fprintf(file, "%02X", value) < 0) {
                    status = LT48_FILE_ERROR;
                    break;
                }
            }
            if (status != LT48_OK) break;
            if (fprintf(file, "%04X\r\n", checksum & 0xffffu) < 0) {
                status = LT48_FILE_ERROR;
                break;
            }
            ++data_records;
            ++result.record_count;
            index += count;
        }
        if (status == LT48_OK) {
            uint16_t address = (uint16_t)data_records;
            unsigned checksum = (address >> 8) + (address & 0xffu);
            if (fprintf(file, ";00%04X%04X\r\n", (unsigned)address,
                        checksum & 0xffffu) < 0)
                status = LT48_FILE_ERROR;
            else
                ++result.record_count;
        }
    } else if (resolved.format == LT48_IMAGE_FORMAT_TEKTRONIX_HEX) {
        size_t index = 0u;
        while (index < output_size && status == LT48_OK) {
            size_t count = output_size - index;
            uint16_t address = (uint16_t)(resolved.address_base + index);
            char header[7];
            unsigned header_checksum;
            unsigned data_checksum = 0u;
            if (cancelled_now(cancelled, cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            if (count > 32u) count = 32u;
            if (snprintf(header, sizeof(header), "%04X%02X",
                         (unsigned)address, (unsigned)count) != 6) {
                status = LT48_RUNTIME_ERROR;
                break;
            }
            header_checksum = hex_nibble_sum(header, 6u);
            if (header_checksum == UINT_MAX ||
                fprintf(file, "/%s%02X", header,
                        header_checksum & 0xffu) < 0) {
                status = LT48_FILE_ERROR;
                break;
            }
            for (size_t byte = 0u; byte < count; ++byte) {
                unsigned value = image_export_byte(
                    buffer, resolved.buffer_offset, phase, stride,
                    index + byte);
                data_checksum += (value >> 4) + (value & 0x0fu);
                if (fprintf(file, "%02X", value) < 0) {
                    status = LT48_FILE_ERROR;
                    break;
                }
            }
            if (status != LT48_OK) break;
            if (fprintf(file, "%02X\r\n", data_checksum & 0xffu) < 0) {
                status = LT48_FILE_ERROR;
                break;
            }
            ++result.record_count;
            index += count;
        }
        if (status == LT48_OK) {
            if (fputs("/0000000000\r\n", file) == EOF)
                status = LT48_FILE_ERROR;
            else
                ++result.record_count;
        }
    } else {
        static const unsigned char signature[] = "Programmer 99/2000";
        unsigned char header_tail[10] = {0u, 7u, 0u, 8u, 0u, 8u};
        size_t index = 0u;
        write_hp64000_address(
            header_tail + 6, (uint32_t)resolved.address_base);
        if (fwrite(signature, 1u, sizeof(signature) - 1u, file) !=
                sizeof(signature) - 1u ||
            fwrite(header_tail, 1u, sizeof(header_tail), file) !=
                sizeof(header_tail)) {
            status = LT48_FILE_ERROR;
        } else {
            ++result.record_count;
        }
        while (index < output_size && status == LT48_OK) {
            unsigned char record_header[8];
            size_t count = output_size - index;
            uint32_t address =
                (uint32_t)(resolved.address_base + index);
            if (cancelled_now(cancelled, cancel_context)) {
                status = LT48_CANCELLED;
                break;
            }
            if (count > 250u) count = 250u;
            write_be16(record_header, (uint16_t)(count + 5u));
            write_be16(record_header + 2, (uint16_t)count);
            write_hp64000_address(record_header + 4, address);
            if (fwrite(record_header, 1u, sizeof(record_header), file) !=
                    sizeof(record_header)) {
                status = LT48_FILE_ERROR;
                break;
            }
            for (size_t byte = 0u; byte < count; ++byte) {
                if (fputc(image_export_byte(
                        buffer, resolved.buffer_offset, phase, stride,
                        index + byte), file) == EOF) {
                    status = LT48_FILE_ERROR;
                    break;
                }
            }
            if (status != LT48_OK) break;
            ++result.record_count;
            index += count;
        }
    }
    if (status == LT48_OK && fflush(file) != 0) status = LT48_FILE_ERROR;
    if (fclose(file) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    if (status != LT48_OK && !resolved.overwrite) (void)remove(path);
    if (status == LT48_OK) *out_result = result;
    return status;
}
