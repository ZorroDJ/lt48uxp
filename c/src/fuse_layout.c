#include <labtool48/labtool48.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../NewDeviceLib/sources/epm44/maps.h"
#include "../../NewDeviceLib/sources/cpld69/maps.h"
#include "../../NewDeviceLib/sources/palce54/maps.h"
#include "../../NewDeviceLib/sources/mach11/maps.h"

struct lt48_fuse_image {
    lt48_runtime_fuse_image view;
};

void lt48_fuse_image_destroy(lt48_fuse_image *image) {
    if (!image) return;
    free((void *)image->view.data);
    free(image);
}

const lt48_runtime_fuse_image *lt48_fuse_image_view(const lt48_fuse_image *image) {
    return image ? &image->view : NULL;
}

lt48_status lt48_pof_fuse_image_decode(const lt48_fuse_code *code,
    const lt48_runtime_fuse_image *image, unsigned char *values, size_t value_count, unsigned pia_bits) {
    if(pia_bits>32)return LT48_INVALID_ARGUMENT;
    lt48_fuse_coordinate *coordinates;
    unsigned char *decoded;
    size_t count, rows, stride, width, record;
    lt48_status status;
    if (!code || !image || !image->data || !values ||
        value_count != code->total_fuse_count) return LT48_INVALID_ARGUMENT;
    for (size_t i = 0; i < 3; ++i)
        if (code->dimensions[i] > INT16_MAX) return LT48_FORMAT_ERROR;
    rows = (size_t)code->dimensions[0] + 1u;
    stride = (size_t)code->dimensions[1] + 1u;
    width = ((size_t)code->dimensions[2] + 8u) / 8u;
    record = width * 2u + 2u;
    if (rows > SIZE_MAX / stride || rows * stride > SIZE_MAX / record ||
        value_count > SIZE_MAX / sizeof(*coordinates)) return LT48_NO_MEMORY;
    if (image->stride != stride || image->copy_width != width ||
        image->record_width != record || image->size != rows * stride * record)
        return LT48_FORMAT_ERROR;
    coordinates = calloc(value_count ? value_count : 1u, sizeof(*coordinates));
    decoded = malloc(value_count ? value_count : 1u);
    if (!coordinates || !decoded) {
        free(coordinates); free(decoded); return LT48_NO_MEMORY;
    }
    status = lt48_fuse_layout_expand_coordinates(code, coordinates, value_count, &count);
    if (status == LT48_OK) {
        for (size_t i = 0; i < count; ++i) {
            const lt48_fuse_coordinate *c = coordinates + i;
            if(pia_bits && (c->fuse_type==12 || c->fuse_type==13 || c->fuse_type==15 || c->fuse_type==16)) {
                size_t end=i+1;
                while(end<count && coordinates[end].fuse_type==c->fuse_type && coordinates[end].column==c->column)end++;
                if(end-i>8){status=LT48_FORMAT_ERROR;break;}
                unsigned value=0;
                for(;value<pia_bits;value++) {
                    int row=(int)c->row+((c->fuse_type==12 || c->fuse_type==15)?(int)value:-(int)value);
                    if(row<0 || (size_t)row>=rows){status=LT48_FORMAT_ERROR;break;}
                    size_t off=((size_t)row*stride+c->column)*record;
                    if(!(image->data[off+width+2+c->bit/8]&(1u<<(c->bit%8))))break;
                }
                if(status!=LT48_OK)break;
                if(value==pia_bits)value=255;
                for(size_t j=end;j>i;j--){decoded[j-1]=value&1;value>>=1;}
                i=end-1;continue;
            }
            size_t offset = ((size_t)c->row * stride + c->column) * record;
            decoded[i] = !!(image->data[offset + width + 2u + c->bit / 8u] &
                           (1u << (c->bit & 7u)));
        }
        if(status==LT48_OK)memcpy(values, decoded, count);
    }
    free(coordinates); free(decoded);
    return status;
}

lt48_status lt48_pof_fuse_image_build(const lt48_fuse_code *code,
    const unsigned char *values, size_t value_count, unsigned pia_bits, lt48_fuse_image **out_image) {
    if(pia_bits>32)return LT48_INVALID_ARGUMENT;
    lt48_fuse_image *image = NULL;
    lt48_fuse_coordinate *coordinates = NULL;
    unsigned char *data;
    size_t count, rows, stride, width, record, size;
    lt48_status status;
    if (!out_image) return LT48_INVALID_ARGUMENT;
    *out_image = NULL;
    if (!code || !values || value_count != code->total_fuse_count)
        return LT48_INVALID_ARGUMENT;
    /* Original constructor sign-extends these serialized shorts. */
    for (size_t i = 0; i < 3; ++i)
        if (code->dimensions[i] > INT16_MAX) return LT48_FORMAT_ERROR;
    rows = (size_t)code->dimensions[0] + 1u;
    stride = (size_t)code->dimensions[1] + 1u;
    width = ((size_t)code->dimensions[2] + 8u) / 8u;
    record = width * 2u + 2u;
    if (rows > SIZE_MAX / stride || rows * stride > SIZE_MAX / record ||
        value_count > SIZE_MAX / sizeof(*coordinates)) return LT48_NO_MEMORY;
    size = rows * stride * record;
    coordinates = calloc(value_count ? value_count : 1u, sizeof(*coordinates));
    if (!coordinates) return LT48_NO_MEMORY;
    status = lt48_fuse_layout_expand_coordinates(code, coordinates, value_count, &count);
    if (status != LT48_OK) { free(coordinates); return status; }
    image = calloc(1, sizeof(*image));
    data = calloc(size, 1);
    if (!image || !data) {
        free(image); free(data); free(coordinates); return LT48_NO_MEMORY;
    }
    image->view = (lt48_runtime_fuse_image){data, size, (uint32_t)stride, record, width};
    for (size_t i = 0; i < count; ++i) {
        const lt48_fuse_coordinate *c = coordinates + i;
        if(pia_bits && (c->fuse_type==12 || c->fuse_type==13 || c->fuse_type==15 || c->fuse_type==16)) {
            size_t end=i+1;unsigned value=values[i]!=0,all=values[i]!=0;
            while(end<count && coordinates[end].fuse_type==c->fuse_type && coordinates[end].column==c->column){value=(value<<1)|(values[end]!=0);all&=values[end]!=0;end++;}
            if(end-i>8){free(coordinates);lt48_fuse_image_destroy(image);return LT48_FORMAT_ERROR;}
            uint32_t encoded=all && (c->fuse_type==15 || c->fuse_type==16)?0xffffu:~(UINT32_C(1)<<(value&31));
            for(unsigned j=0;j<pia_bits;j++) {
                int row=(int)c->row+((c->fuse_type==12 || c->fuse_type==15)?(int)j:-(int)j);
                if(row<0 || (size_t)row>=rows){free(coordinates);lt48_fuse_image_destroy(image);return LT48_FORMAT_ERROR;}
                size_t off=((size_t)row*stride+c->column)*record;
                unsigned char bit=(unsigned char)(1u<<(c->bit%8));
                data[off]=12;data[off+1]=1;data[off+2+c->bit/8]|=bit;
                if(encoded&1)data[off+width+2+c->bit/8]|=bit;
                encoded>>=1;
            }
            i=end-1;continue;
        }
        size_t offset = ((size_t)c->row * stride + c->column) * record;
        unsigned char mask = (unsigned char)(1u << (c->bit & 7u));
        data[offset] = (unsigned char)c->fuse_type;
        data[offset + 1u] = 1;
        data[offset + 2u + c->bit / 8u] |= mask;
        if (values[i]) data[offset + width + 2u + c->bit / 8u] |= mask;
    }
    free(coordinates);
    *out_image = image;
    return LT48_OK;
}

lt48_status lt48_fuse_image_build(const lt48_fuse_code *code,
    const unsigned char *values,size_t count,lt48_fuse_image **out) {
    return lt48_pof_fuse_image_build(code,values,count,0,out);
}
lt48_status lt48_fuse_image_decode(const lt48_fuse_code *code,
    const lt48_runtime_fuse_image *image,unsigned char *values,size_t count) {
    return lt48_pof_fuse_image_decode(code,image,values,count,0);
}

lt48_status lt48_fuse_image_run_rows(lt48_fuse_image *image,
    uint16_t mode, unsigned char blank_fill, lt48_fuse_row_exchange_fn exchange,
    int (*cancelled)(void *context), void *context, lt48_fuse_row_result *out_result) {
    unsigned char *scratch, *actual, *expected;
    size_t width, record, rows;
    if (!image || !exchange || !out_result) return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    out_result->matched = 1;
    out_result->mismatch_byte = SIZE_MAX;
    if (mode != 1 && mode != 2 && mode != 3 && mode != 4 && mode != 11)
        return LT48_UNSUPPORTED;
    width = image->view.copy_width; record = image->view.record_width;
    rows = image->view.size / record / image->view.stride;
    scratch = malloc(width * 2u);
    if (!scratch) return LT48_NO_MEMORY;
    actual = scratch; expected = scratch + width;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < image->view.stride; ++column) {
            unsigned char *cell = (unsigned char *)image->view.data +
                (row * image->view.stride + column) * record;
            const unsigned char *mask = cell + 2u;
            if (!cell[1]) continue;
            if (mode == 11) {
                for (size_t i = 0; i < width; ++i)
                    expected[i] = blank_fill & mask[i];
            } else memcpy(expected, cell + width + 2u, width);
            /* PROGRAM starts DataIn with expected; other modes overwrite it
             * through LOAD. The bound callback owns runtime buffer details. */
            memcpy(actual, expected, width);
            lt48_status status = exchange(context, mode, (uint16_t)row,
                (uint16_t)column, cell[0], expected, mask, width, actual);
            ++out_result->exchanges;
            if (status != LT48_OK) { free(scratch); return status; }
            for (size_t i = 0; i < width; ++i) actual[i] &= mask[i];
            if (mode == 1) memcpy(cell + width + 2u, actual, width);
            else {
                if (mode == 2)
                    for (size_t i = 0; i < width; ++i) expected[i] &= mask[i];
                for (size_t i = 0; i < width; ++i) {
                    if (actual[i] == expected[i]) continue;
                    out_result->matched = 0;
                    out_result->mismatch_row = (uint16_t)row;
                    out_result->mismatch_column = (uint16_t)column;
                    out_result->mismatch_byte = i;
                    out_result->expected = expected[i];
                    out_result->actual = actual[i];
                    free(scratch);
                    return LT48_OK;
                }
            }
        }
        ++out_result->completed_rows;
        if (cancelled && cancelled(context)) { free(scratch); return LT48_CANCELLED; }
    }
    free(scratch);
    return LT48_OK;
}

typedef struct layout_accumulator {
    uint64_t total;
    uint64_t rows;
    size_t nodes;
} layout_accumulator;

typedef struct layout_group {
    layout_accumulator value;
    lt48_fuse_layout_stream stream;
} layout_group;

typedef struct layout_descriptor {
    uint16_t start;
    int16_t step;
    uint16_t change_interval;
    uint16_t reset_interval;
} layout_descriptor;

typedef struct row_group {
    lt48_fuse_layout_stream stream;
    size_t row_start;
} row_group;

static uint16_t read_u16(const unsigned char *bytes) {
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8u));
}

static int selector_stream(unsigned char selector,
                           lt48_fuse_layout_stream *out_stream) {
    switch (selector) {
    case 'T': *out_stream = LT48_FUSE_STREAM_T; return 1;
    case 'P': *out_stream = LT48_FUSE_STREAM_P; return 1;
    case 'I': *out_stream = LT48_FUSE_STREAM_I; return 1;
    case 'O': *out_stream = LT48_FUSE_STREAM_O; return 1;
    case 'R': *out_stream = LT48_FUSE_STREAM_R; return 1;
    default: return 0;
    }
}

static int add_value(layout_accumulator *target,
                     const layout_accumulator *value, uint16_t repeat) {
    uint64_t total = value->total * (uint64_t)repeat;
    uint64_t rows = value->rows * (uint64_t)repeat;
    if (total > UINT32_MAX || rows > UINT32_MAX ||
        target->total > UINT32_MAX - total ||
        target->rows > UINT32_MAX - rows)
        return 0;
    target->total += total;
    target->rows += rows;
    target->nodes += value->nodes + 1u;
    return 1;
}

lt48_status lt48_fuse_layout_analyze(
    const lt48_fuse_code *code, lt48_fuse_layout *out_layout
) {
    layout_accumulator roots[LT48_FUSE_LAYOUT_STREAM_COUNT];
    layout_descriptor descriptor;
    layout_group *groups = NULL;
    lt48_fuse_layout_stream active = LT48_FUSE_STREAM_T;
    size_t depth = 0u;
    size_t index = 0u;
    int have_active = 0;
    int terminated = 0;

    if (code == NULL || out_layout == NULL || code->bytecode == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_layout, 0, sizeof(*out_layout));
    memset(roots, 0, sizeof(roots));
    memset(&descriptor, 0, sizeof(descriptor));
    if (code->bytecode_size != (size_t)code->instruction_count * 3u)
        return LT48_FORMAT_ERROR;
    if (code->instruction_count != 0u) {
        groups = calloc((size_t)code->instruction_count, sizeof(*groups));
        if (groups == NULL) return LT48_NO_MEMORY;
    }

    while (index < code->instruction_count) {
        const unsigned char *instruction = code->bytecode + index * 3u;
        uint16_t operand = read_u16(instruction + 1u);
        layout_accumulator value;
        layout_accumulator *target;

        memset(&value, 0, sizeof(value));
        switch (instruction[0]) {
        case 0xffu:
            if (depth != 0u || index + 1u != code->instruction_count)
                goto format_error;
            terminated = 1;
            ++index;
            break;
        case '@':
            if (depth != 0u || index + 1u >= code->instruction_count ||
                !selector_stream(code->bytecode[(index + 1u) * 3u],
                                 &active))
                goto format_error;
            have_active = 1;
            index += 2u;
            continue;
        case '#':
            memset(&descriptor, 0, sizeof(descriptor));
            descriptor.start = operand;
            ++index;
            continue;
        case '+':
            descriptor.step = 1;
            descriptor.change_interval = operand;
            ++index;
            continue;
        case '-':
            descriptor.step = -1;
            descriptor.change_interval = operand;
            ++index;
            continue;
        case '^':
            descriptor.reset_interval = operand;
            ++index;
            continue;
        case 'x':
            if (!have_active) goto format_error;
            out_layout->streams[active].declared_minimum = operand;
            ++index;
            continue;
        case ':':
            if (!have_active || depth >= code->instruction_count)
                goto format_error;
            memset(&groups[depth], 0, sizeof(groups[depth]));
            groups[depth].stream = active;
            ++depth;
            if (depth > out_layout->maximum_nesting)
                out_layout->maximum_nesting = depth;
            if (depth > out_layout->streams[active].maximum_nesting)
                out_layout->streams[active].maximum_nesting = depth;
            ++index;
            continue;
        case '!':
            if (!have_active || depth == 0u ||
                groups[depth - 1u].stream != active)
                goto format_error;
            value = groups[--depth].value;
            target = depth == 0u ? &roots[active] : &groups[depth - 1u].value;
            if (!add_value(target, &value, operand)) goto format_error;
            ++index;
            continue;
        case '>':
            if (!have_active) goto format_error;
            if (active == LT48_FUSE_STREAM_R) {
                value.total = (uint64_t)descriptor.start * operand;
                value.rows = operand;
            } else {
                value.total = operand;
            }
            value.nodes = 1u;
            target = depth == 0u ? &roots[active] : &groups[depth - 1u].value;
            /* A leaf is already one node; add directly rather than count the
             * group wrapper used by add_value(). */
            if (value.total > UINT32_MAX || value.rows > UINT32_MAX ||
                target->total > UINT32_MAX - value.total ||
                target->rows > UINT32_MAX - value.rows)
                goto format_error;
            target->total += value.total;
            target->rows += value.rows;
            target->nodes += value.nodes;
            memset(&descriptor, 0, sizeof(descriptor));
            ++index;
            continue;
        default:
            goto format_error;
        }
        if (terminated) break;
    }

    if (!terminated || index != code->instruction_count) goto format_error;
    for (size_t stream = 0u; stream < LT48_FUSE_LAYOUT_STREAM_COUNT; ++stream) {
        if (roots[stream].total != code->total_fuse_count)
            goto format_error;
        out_layout->streams[stream].total = (uint32_t)roots[stream].total;
        out_layout->streams[stream].row_count = (uint32_t)roots[stream].rows;
        out_layout->streams[stream].node_count = roots[stream].nodes;
    }
    out_layout->instruction_count = code->instruction_count;
    free(groups);
    return LT48_OK;

format_error:
    free(groups);
    memset(out_layout, 0, sizeof(*out_layout));
    return LT48_FORMAT_ERROR;
}

lt48_status lt48_fuse_layout_expand_rows(
    const lt48_fuse_code *code, lt48_fuse_row *rows, size_t row_capacity,
    size_t *out_row_count
) {
    lt48_fuse_layout layout;
    layout_descriptor descriptor;
    row_group *groups = NULL;
    lt48_fuse_layout_stream active = LT48_FUSE_STREAM_T;
    size_t row_count;
    size_t written = 0u;
    size_t depth = 0u;
    size_t index = 0u;
    uint32_t first = 0u;
    int have_active = 0;
    lt48_status status;

    if (out_row_count == NULL || code == NULL) return LT48_INVALID_ARGUMENT;
    *out_row_count = 0u;
    status = lt48_fuse_layout_analyze(code, &layout);
    if (status != LT48_OK) return status;
    row_count = layout.streams[LT48_FUSE_STREAM_R].row_count;
    *out_row_count = row_count;
    if (rows == NULL) return row_capacity == 0u ? LT48_OK : LT48_INVALID_ARGUMENT;
    if (row_capacity < row_count) return LT48_BUFFER_TOO_SMALL;
    memset(&descriptor, 0, sizeof(descriptor));
    if (code->instruction_count != 0u) {
        groups = calloc((size_t)code->instruction_count, sizeof(*groups));
        if (groups == NULL) return LT48_NO_MEMORY;
    }

    while (index < code->instruction_count) {
        const unsigned char *instruction = code->bytecode + index * 3u;
        uint16_t operand = read_u16(instruction + 1u);
        switch (instruction[0]) {
        case 0xffu:
            index = code->instruction_count;
            continue;
        case '@':
            if (!selector_stream(code->bytecode[(index + 1u) * 3u], &active))
                goto format_error;
            have_active = 1;
            index += 2u;
            continue;
        case '#':
            memset(&descriptor, 0, sizeof(descriptor));
            descriptor.start = operand;
            break;
        case '+':
            descriptor.step = 1;
            descriptor.change_interval = operand;
            break;
        case '-':
            descriptor.step = -1;
            descriptor.change_interval = operand;
            break;
        case '^':
            descriptor.reset_interval = operand;
            break;
        case 'x':
            break;
        case ':':
            if (!have_active || depth >= code->instruction_count)
                goto format_error;
            groups[depth].stream = active;
            groups[depth].row_start = written;
            ++depth;
            break;
        case '!':
            if (depth == 0u || groups[depth - 1u].stream != active)
                goto format_error;
            --depth;
            if (active == LT48_FUSE_STREAM_R) {
                size_t start = groups[depth].row_start;
                size_t body = written - start;
                if (operand == 0u) {
                    written = start;
                } else {
                    for (uint16_t repeat = 1u; repeat < operand; ++repeat) {
                        if (body > row_count - written) goto format_error;
                        memcpy(rows + written, rows + start,
                               body * sizeof(*rows));
                        written += body;
                    }
                }
            }
            break;
        case '>':
            if (!have_active) goto format_error;
            if (active == LT48_FUSE_STREAM_R) {
                if ((size_t)operand > row_count - written)
                    goto format_error;
                for (uint16_t row = 0u; row < operand; ++row) {
                    rows[written].first_fuse = 0u;
                    rows[written].length = descriptor.start;
                    ++written;
                }
            }
            memset(&descriptor, 0, sizeof(descriptor));
            break;
        default:
            goto format_error;
        }
        ++index;
    }
    if (depth != 0u || written != row_count) goto format_error;
    for (size_t row = 0u; row < written; ++row) {
        rows[row].first_fuse = first;
        if ((uint32_t)rows[row].length > UINT32_MAX - first)
            goto format_error;
        first += rows[row].length;
    }
    if (first != code->total_fuse_count) goto format_error;
    free(groups);
    return LT48_OK;

format_error:
    free(groups);
    return LT48_FORMAT_ERROR;
}

static int compare_coordinate(const void *left, const void *right) {
    const lt48_fuse_coordinate *a = left, *b = right;
    if (a->row != b->row) return a->row < b->row ? -1 : 1;
    if (a->column != b->column) return a->column < b->column ? -1 : 1;
    if (a->bit != b->bit) return a->bit < b->bit ? -1 : 1;
    return 0;
}

lt48_status lt48_fuse_layout_expand_coordinates(
    const lt48_fuse_code *code, lt48_fuse_coordinate *coordinates,
    size_t capacity, size_t *out_count
) {
    lt48_fuse_layout layout;
    lt48_status status;
    uint16_t *values = NULL;
    lt48_fuse_coordinate *sorted = NULL;
    row_group *groups = NULL;
    layout_descriptor descriptor = {0};
    size_t written[LT48_FUSE_LAYOUT_STREAM_COUNT] = {0};
    size_t total, index = 0u, depth = 0u;
    lt48_fuse_layout_stream active = LT48_FUSE_STREAM_T;
    if (!code || !out_count) return LT48_INVALID_ARGUMENT;
    *out_count = 0u;
    status = lt48_fuse_layout_analyze(code, &layout);
    if (status != LT48_OK) return status;
    total = code->total_fuse_count;
    *out_count = total;
    if (!coordinates) return capacity ? LT48_INVALID_ARGUMENT : LT48_OK;
    if (capacity < total) return LT48_BUFFER_TOO_SMALL;
    if (total > SIZE_MAX / (4u * sizeof(*values)) ||
        total > SIZE_MAX / sizeof(*sorted)) return LT48_NO_MEMORY;
    values = calloc(total ? total : 1u, 4u * sizeof(*values));
    sorted = calloc(total ? total : 1u, sizeof(*sorted));
    groups = calloc(code->instruction_count ? code->instruction_count : 1u,
                    sizeof(*groups));
    if (!values || !sorted || !groups) { status = LT48_NO_MEMORY; goto cleanup; }
    while (index < code->instruction_count) {
        const unsigned char *instruction = code->bytecode + index * 3u;
        uint16_t operand = read_u16(instruction + 1u);
        switch (instruction[0]) {
        case 0xffu: index = code->instruction_count; continue;
        case '@':
            if (!selector_stream(code->bytecode[(index + 1u) * 3u], &active))
                goto format_error;
            index += 2u; continue;
        case '#':
            memset(&descriptor, 0, sizeof(descriptor));
            descriptor.start = operand; break;
        case '+': descriptor.step = 1; descriptor.change_interval = operand; break;
        case '-': descriptor.step = -1; descriptor.change_interval = operand; break;
        case '^': descriptor.reset_interval = operand; break;
        case 'x': break;
        case ':':
            groups[depth].stream = active;
            groups[depth].row_start = written[active];
            ++depth; break;
        case '!': {
            size_t start, body;
            --depth;
            if (active == LT48_FUSE_STREAM_R) break;
            start = groups[depth].row_start;
            body = written[active] - start;
            if (!operand) written[active] = start;
            else for (uint16_t repeat = 1u; repeat < operand; ++repeat) {
                if (body > total - written[active]) goto format_error;
                memcpy(values + (size_t)active * total + written[active],
                       values + (size_t)active * total + start,
                       body * sizeof(*values));
                written[active] += body;
            }
            break;
        }
        case '>': {
            uint16_t value = descriptor.start;
            uint16_t change_left = descriptor.change_interval;
            uint16_t reset_left = descriptor.reset_interval;
            if (active != LT48_FUSE_STREAM_R) {
                if (operand > total - written[active]) goto format_error;
                for (uint16_t item = 0u; item < operand; ++item) {
                    values[(size_t)active * total + written[active]++] = value;
                    if (descriptor.change_interval && --change_left == 0u) {
                        value = (uint16_t)(value + descriptor.step);
                        change_left = descriptor.change_interval;
                    }
                    if (descriptor.reset_interval && --reset_left == 0u) {
                        value = descriptor.start;
                        reset_left = descriptor.reset_interval;
                    }
                }
            }
            memset(&descriptor, 0, sizeof(descriptor));
            break;
        }
        default: goto format_error;
        }
        ++index;
    }
    for (size_t stream = 0u; stream < 4u; ++stream)
        if (written[stream] != total) goto format_error;
    for (size_t fuse = 0u; fuse < total; ++fuse) {
        sorted[fuse].fuse_type = values[fuse];
        sorted[fuse].row = values[total + fuse];
        sorted[fuse].column = values[2u * total + fuse];
        sorted[fuse].bit = values[3u * total + fuse];
        if (sorted[fuse].row > code->dimensions[0] ||
            sorted[fuse].column > code->dimensions[1] ||
            sorted[fuse].bit > code->dimensions[2]) goto format_error;
    }
    qsort(sorted, total, sizeof(*sorted), compare_coordinate);
    unsigned aliases=0;
    for (size_t fuse = 1u; fuse < total; ++fuse)
        if (!compare_coordinate(sorted + fuse - 1u, sorted + fuse)) ++aliases;
    if (aliases && aliases!=epm44_layout_aliases(code) && aliases!=cpld69_layout_aliases(code) && aliases!=palce54_layout_aliases(code) && aliases!=mach11_layout_aliases(code)) goto format_error;
    for (size_t fuse = 0u; fuse < total; ++fuse) {
        coordinates[fuse].fuse_type = values[fuse];
        coordinates[fuse].row = values[total + fuse];
        coordinates[fuse].column = values[2u * total + fuse];
        coordinates[fuse].bit = values[3u * total + fuse];
    }
    status = LT48_OK;
    goto cleanup;
format_error:
    status = LT48_FORMAT_ERROR;
cleanup:
    free(groups); free(sorted); free(values);
    return status;
}
