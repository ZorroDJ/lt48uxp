#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdlib.h>
#include <string.h>

typedef struct read_chunk {
    size_t transaction_index;
    uint32_t device_offset;
    size_t response_offset;
    size_t response_size;
    size_t data_size;
    int received;
} read_chunk;

struct lt48_m37_read_collector {
    read_chunk *chunks;
    size_t chunk_count;
    size_t chunks_received;
    size_t bytes_received;
    uint32_t start;
    size_t size;
    unsigned char *data;
};

void lt48_m37_read_collector_close(lt48_m37_read_collector *collector) {
    if (collector == NULL) return;
    free(collector->data);
    free(collector->chunks);
    free(collector);
}

void lt48_m37_read_collector_replace_internal(
    lt48_m37_read_collector *target, lt48_m37_read_collector *source) {
    lt48_m37_read_collector old = *target;
    *target = *source;
    *source = old;
    lt48_m37_read_collector_close(source);
}

lt48_status lt48_m37_read_collector_create(
    const lt48_protocol_plan *plan,
    lt48_m37_read_collector **out_collector
) {
    lt48_m37_read_collector *collector;
    size_t transaction_count;
    size_t index;
    size_t next = 0u;
    uint32_t minimum = UINT32_MAX;
    uint32_t maximum = 0u;
    size_t total = 0u;
    if (plan == NULL || out_collector == NULL) return LT48_INVALID_ARGUMENT;
    *out_collector = NULL;
    transaction_count = lt48_protocol_plan_transaction_count(plan);
    for (index = 0u; index < transaction_count; ++index) {
        lt48_protocol_transaction transaction;
        lt48_status status = lt48_protocol_plan_get_transaction(
            plan, index, &transaction
        );
        if (status != LT48_OK) return status;
        if (transaction.reads_device_data) ++next;
    }
    if (next == 0u) return LT48_NOT_FOUND;
    collector = calloc(1u, sizeof(*collector));
    if (collector == NULL) return LT48_NO_MEMORY;
    collector->chunks = calloc(next, sizeof(*collector->chunks));
    if (collector->chunks == NULL) {
        lt48_m37_read_collector_close(collector);
        return LT48_NO_MEMORY;
    }
    collector->chunk_count = next;
    next = 0u;
    for (index = 0u; index < transaction_count; ++index) {
        lt48_protocol_transaction transaction;
        uint32_t end;
        lt48_status status = lt48_protocol_plan_get_transaction(
            plan, index, &transaction
        );
        if (status != LT48_OK) {
            lt48_m37_read_collector_close(collector);
            return status;
        }
        if (!transaction.reads_device_data) continue;
        if (transaction.direction != LT48_TRANSFER_IN ||
            transaction.device_data_length == 0u ||
            transaction.device_data_length > UINT32_MAX ||
            transaction.response_offset > transaction.length ||
            transaction.device_data_length >
                transaction.length - transaction.response_offset ||
            transaction.device_data_offset >
                UINT32_MAX - (uint32_t)transaction.device_data_length) {
            lt48_m37_read_collector_close(collector);
            return LT48_PROTOCOL_ERROR;
        }
        end = transaction.device_data_offset +
              (uint32_t)transaction.device_data_length;
        collector->chunks[next].transaction_index = index;
        collector->chunks[next].device_offset = transaction.device_data_offset;
        collector->chunks[next].response_offset = transaction.response_offset;
        collector->chunks[next].response_size = transaction.length;
        collector->chunks[next].data_size = transaction.device_data_length;
        if (transaction.device_data_offset < minimum)
            minimum = transaction.device_data_offset;
        if (end > maximum) maximum = end;
        if (transaction.device_data_length > SIZE_MAX - total) {
            lt48_m37_read_collector_close(collector);
            return LT48_PROTOCOL_ERROR;
        }
        total += transaction.device_data_length;
        ++next;
    }
    if ((uint64_t)maximum - minimum != total) {
        lt48_m37_read_collector_close(collector);
        return LT48_PROTOCOL_ERROR;
    }
    for (index = 0u; index < collector->chunk_count; ++index) {
        size_t other;
        uint32_t left_end = collector->chunks[index].device_offset +
                            (uint32_t)collector->chunks[index].data_size;
        for (other = index + 1u; other < collector->chunk_count; ++other) {
            uint32_t right_end = collector->chunks[other].device_offset +
                                 (uint32_t)collector->chunks[other].data_size;
            if (collector->chunks[index].device_offset < right_end &&
                collector->chunks[other].device_offset < left_end) {
                lt48_m37_read_collector_close(collector);
                return LT48_PROTOCOL_ERROR;
            }
        }
    }
    collector->data = malloc(total);
    if (collector->data == NULL) {
        lt48_m37_read_collector_close(collector);
        return LT48_NO_MEMORY;
    }
    collector->start = minimum;
    collector->size = total;
    *out_collector = collector;
    return LT48_OK;
}

lt48_status lt48_m37_read_collector_accept(
    lt48_m37_read_collector *collector,
    size_t transaction_index,
    const unsigned char *data,
    size_t size
) {
    size_t index;
    if (collector == NULL || data == NULL) return LT48_INVALID_ARGUMENT;
    for (index = 0u; index < collector->chunk_count; ++index) {
        read_chunk *chunk = &collector->chunks[index];
        if (chunk->transaction_index != transaction_index) continue;
        if (chunk->received || size != chunk->response_size)
            return LT48_PROTOCOL_ERROR;
        memcpy(collector->data + (chunk->device_offset - collector->start),
               data + chunk->response_offset, chunk->data_size);
        chunk->received = 1;
        ++collector->chunks_received;
        collector->bytes_received += chunk->data_size;
        return LT48_OK;
    }
    return LT48_NOT_FOUND;
}

void lt48_m37_read_collector_progress(
    const lt48_m37_read_collector *collector,
    size_t *out_chunks_received,
    size_t *out_chunk_count,
    size_t *out_bytes_received,
    size_t *out_byte_count
) {
    if (out_chunks_received != NULL)
        *out_chunks_received = collector == NULL ? 0u : collector->chunks_received;
    if (out_chunk_count != NULL)
        *out_chunk_count = collector == NULL ? 0u : collector->chunk_count;
    if (out_bytes_received != NULL)
        *out_bytes_received = collector == NULL ? 0u : collector->bytes_received;
    if (out_byte_count != NULL)
        *out_byte_count = collector == NULL ? 0u : collector->size;
}

lt48_status lt48_m37_read_collector_data(
    const lt48_m37_read_collector *collector,
    uint32_t *out_start,
    const unsigned char **out_data,
    size_t *out_size
) {
    if (collector == NULL || out_start == NULL || out_data == NULL ||
        out_size == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (collector->chunks_received != collector->chunk_count) return LT48_BUSY;
    *out_start = collector->start;
    *out_data = collector->data;
    *out_size = collector->size;
    return LT48_OK;
}
