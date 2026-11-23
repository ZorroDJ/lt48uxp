#include <labtool48/labtool48.h>

#include "internal.h"
#include "dataman_id_policy.h"
#include "dataman_contact_policy.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LT48_MAX_PACKET_SIZE 512u
#define LT48_EP_COMMAND_OUT 0x02u
#define LT48_EP_DATA_OUT 0x04u
#define LT48_EP_IN 0x88u
#define LT48_MAX_QUEUE_LIMIT (LT48_MAX_PACKET_SIZE - 4u)
#define LT48_RUNTIME_EVENT_ARGUMENTS 64u

static const unsigned char dataman_m0_definition[LT48_SHA256_SIZE] = {
    0x2e,0x62,0x61,0xc8,0x67,0xd6,0x85,0x77,
    0x86,0x93,0x3c,0x0d,0x09,0x3e,0x5b,0xd6,
    0x69,0xf7,0x03,0x55,0xcc,0x21,0x8f,0xf0,
    0x06,0xc8,0x8c,0x4f,0xb9,0x19,0x63,0x6b
};

typedef struct transaction_owned {
    lt48_transfer_direction direction;
    uint8_t endpoint;
    unsigned char *data;
    int owns_data;
    size_t length;
    int captures_runtime_byte;
    uint8_t runtime_buffer;
    size_t response_offset;
    size_t runtime_capture_length;
    size_t runtime_pin_count;
    int validates_controller_response;
    uint8_t request_opcode;
    unsigned completion_data_width;
    int checks_dataman_m0_contacts;
    int reads_device_data;
    uint32_t device_data_offset;
    size_t device_data_length;
} transaction_owned;

typedef struct recovery_phase_owned {
    lt48_m37_cleanup_phase phase;
    size_t first_transaction;
    size_t transaction_count;
} recovery_phase_owned;

struct lt48_protocol_plan {
    transaction_owned *transactions;
    size_t transaction_count;
    size_t transaction_capacity;
    unsigned char queue[LT48_MAX_QUEUE_LIMIT];
    size_t queue_size;
    size_t packet_size;
    unsigned mode;
    recovery_phase_owned recovery_phases[12];
    size_t recovery_phase_count;
    int recovery_sealed;
    int dataman_id_cleanup_stage;
    int m37_read_sealed;
    int dataman_id_required;
    int dataman_id_match_required;
    size_t dataman_id_boundary;
    uint16_t dataman_manufacturer_id;
    uint16_t dataman_device_id;
    size_t m37_powered_first_transaction;
    unsigned char m37_resource_manifest_sha256[LT48_SHA256_SIZE];
};

struct lt48_m37_verify_plan_set {
    size_t count;
    uint16_t voltages[4];
    lt48_protocol_plan *plans[4];
    unsigned char sha256[LT48_SHA256_SIZE];
};

struct lt48_m37_program_plan_set {
    size_t count;
    lt48_protocol_plan *plans[4];
    unsigned char sha256[LT48_SHA256_SIZE];
};

static lt48_status append_recovery_phase(
    lt48_protocol_plan *plan, lt48_m37_cleanup_phase phase,
    size_t first_transaction
) {
    recovery_phase_owned *span;
    if (plan == NULL || first_transaction > plan->transaction_count ||
        plan->recovery_phase_count >=
            sizeof(plan->recovery_phases) / sizeof(plan->recovery_phases[0])) {
        return LT48_PROTOCOL_ERROR;
    }
    span = &plan->recovery_phases[plan->recovery_phase_count++];
    span->phase = phase;
    span->first_transaction = first_transaction;
    span->transaction_count = plan->transaction_count - first_transaction;
    return LT48_OK;
}

static void trim_transactions(lt48_protocol_plan *plan, size_t count) {
    while (plan->transaction_count > count) {
        --plan->transaction_count;
        if(plan->transactions[plan->transaction_count].owns_data)free(plan->transactions[plan->transaction_count].data);
    }
}

/* Streaming-only history collection. Completed endpoint records may be
 * dropped; the pending controller queue, mode and packet size must survive.
 * Sealed plans and contact/ID boundaries are never eligible. */
lt48_status lt48_protocol_plan_discard_completed_internal(lt48_protocol_plan *plan) {
    if(!plan)return LT48_INVALID_ARGUMENT;
    if(plan->recovery_sealed || plan->m37_read_sealed || plan->recovery_phase_count ||
       plan->dataman_id_required || plan->dataman_id_cleanup_stage)
        return LT48_SAFETY_REJECTED;
    for(size_t i=0;i<plan->transaction_count;i++)
        if(plan->transactions[i].checks_dataman_m0_contacts)return LT48_SAFETY_REJECTED;
    trim_transactions(plan,0);
    return LT48_OK;
}

static lt48_status append_transaction(
    lt48_protocol_plan *plan, lt48_transfer_direction direction,
    uint8_t endpoint, const unsigned char *data, size_t length
) {
    transaction_owned *transaction;
    if (direction == LT48_TRANSFER_OUT && length && !data) {
        return LT48_INVALID_ARGUMENT;
    }
    if (plan->transaction_count == plan->transaction_capacity) {
        size_t capacity = plan->transaction_capacity ?
                          plan->transaction_capacity * 2u : 16u;
        transaction_owned *transactions;
        if (capacity < plan->transaction_capacity ||
            capacity > SIZE_MAX / sizeof(*transactions)) return LT48_NO_MEMORY;
        transactions = realloc(plan->transactions,
                               capacity * sizeof(*transactions));
        if (!transactions) return LT48_NO_MEMORY;
        plan->transactions = transactions;
        plan->transaction_capacity = capacity;
    }
    transaction = &plan->transactions[plan->transaction_count];
    transaction->data = NULL;
    transaction->owns_data=0;
    if (direction == LT48_TRANSFER_OUT && length) {
        static const unsigned char compare_packet[512]={0x41,0xd0};
        static const unsigned char controlled_packet[512]={0x04};
        if(length==512 && !memcmp(data,compare_packet,512))
            transaction->data=(unsigned char *)compare_packet;
        else if(length==512 && !memcmp(data,controlled_packet,512))
            transaction->data=(unsigned char *)controlled_packet;
        else {
            transaction->data = malloc(length);
            if (!transaction->data) return LT48_NO_MEMORY;
            transaction->owns_data=1;memcpy(transaction->data,data,length);
        }
    }
    transaction->direction = direction;
    transaction->endpoint = endpoint;
    transaction->length = length;
    transaction->captures_runtime_byte = 0;
    transaction->runtime_buffer = 0u;
    transaction->response_offset = 0u;
    transaction->runtime_capture_length = 0u;
    transaction->runtime_pin_count = 0u;
    transaction->validates_controller_response = 0;
    transaction->request_opcode = 0u;
    transaction->completion_data_width = 0u;
    transaction->checks_dataman_m0_contacts = 0;
    transaction->reads_device_data = 0;
    transaction->device_data_offset = 0u;
    transaction->device_data_length = 0u;
    ++plan->transaction_count;
    return LT48_OK;
}

static lt48_status command(
    lt48_protocol_plan *plan, const unsigned char *payload,
    size_t payload_size, int response
) {
    unsigned char packet[LT48_MAX_PACKET_SIZE] = {0};
    lt48_status status;
    if (payload_size > plan->packet_size) return LT48_PROTOCOL_ERROR;
    if (payload_size) memcpy(packet, payload, payload_size);
    status = append_transaction(plan, LT48_TRANSFER_OUT,
                                LT48_EP_COMMAND_OUT, packet, plan->packet_size);
    if (status != LT48_OK || !response) return status;
    status = append_transaction(plan, LT48_TRANSFER_IN, LT48_EP_IN,
                                NULL, plan->packet_size);
    if (status == LT48_OK) {
        transaction_owned *reply =
            &plan->transactions[plan->transaction_count - 1u];
        reply->validates_controller_response = 1;
        reply->request_opcode = payload_size ? payload[0] : 0u;
    }
    return status;
}

static lt48_status flush_queue(lt48_protocol_plan *plan) {
    unsigned char payload[LT48_MAX_PACKET_SIZE];
    size_t payload_size;
    lt48_status status;
    if (!plan->queue_size) return LT48_OK;
    payload[0] = 0x2cu;
    memcpy(payload + 1u, plan->queue, plan->queue_size);
    payload[plan->queue_size + 1u] = 0u;
    payload_size = plan->queue_size + 2u;
    status = command(plan, payload, payload_size, 1);
    if (status == LT48_OK) plan->queue_size = 0u;
    return status;
}

static lt48_status pair(lt48_protocol_plan *plan, uint8_t kind, uint8_t value) {
    lt48_status status;
    if (plan->queue_size + 2u > plan->packet_size - 4u) {
        status = flush_queue(plan);
        if (status != LT48_OK) return status;
    }
    plan->queue[plan->queue_size++] = kind;
    plan->queue[plan->queue_size++] = value;
    return LT48_OK;
}

static lt48_status write_block(
    lt48_protocol_plan *plan, const unsigned char *data, size_t size
) {
    size_t position;
    lt48_status status;
    if (size && !data) return LT48_INVALID_ARGUMENT;
    if (size <= (SIZE_MAX - 1u) / 2u &&
        size * 2u < plan->packet_size - plan->queue_size - 1u) {
        for (position = 0; position < size; ++position) {
            status = pair(plan, 1u, data[position]);
            if (status != LT48_OK) return status;
        }
        return plan->mode == 2u ? flush_queue(plan) : LT48_OK;
    }
    status = flush_queue(plan);
    if (status != LT48_OK) return status;
    for (position = 0; position < size; position += plan->packet_size - 3u) {
        unsigned char payload[LT48_MAX_PACKET_SIZE];
        size_t chunk = size - position;
        if (chunk > plan->packet_size - 3u)
            chunk = plan->packet_size - 3u;
        payload[0] = 0x0fu;
        payload[1] = (unsigned char)chunk;
        payload[2] = (unsigned char)(chunk >> 8);
        memcpy(payload + 3u, data + position, chunk);
        status = command(plan, payload, chunk + 3u, 1);
        if (status != LT48_OK) return status;
    }
    return LT48_OK;
}

static lt48_status status_command(lt48_protocol_plan *plan) {
    static const unsigned char status_bytes[] = {0x54u, 0x54u};
    static const unsigned char opcode[] = {0x0du};
    lt48_status status = write_block(plan, status_bytes, sizeof(status_bytes));
    if (status == LT48_OK) status = flush_queue(plan);
    if (status == LT48_OK) status = command(plan, opcode, sizeof(opcode), 1);
    return status;
}

static lt48_status raw_write(
    lt48_protocol_plan *plan, const uint64_t *values, size_t size
) {
    unsigned char header[5];
    unsigned char data[48];
    size_t index;
    lt48_status status;
    if (size > sizeof(data)) return LT48_PROTOCOL_ERROR;
    for (index = 0; index < size; ++index) {
        if (values[index] > 0xffu) return LT48_PROTOCOL_ERROR;
        data[index] = (unsigned char)values[index];
    }
    status = flush_queue(plan);
    if (status != LT48_OK) return status;
    /* Original 004967c0 flushes the queue but sends no bulk command
     * when the control record contains zero pins. */
    if (!size) return LT48_OK;
    header[0] = 0x44u;
    header[1] = (unsigned char)(size / plan->packet_size);
    header[2] = (unsigned char)(size / plan->packet_size >> 8);
    header[3] = (unsigned char)(size % plan->packet_size);
    header[4] = (unsigned char)(size % plan->packet_size >> 8);
    status = command(plan, header, sizeof(header), 0);
    if (status != LT48_OK) return status;
    return append_transaction(plan, LT48_TRANSFER_OUT, LT48_EP_DATA_OUT,
                              data, size);
}

static lt48_status applet_address(
    const lt48_applet_address *applets, size_t applet_count,
    uint16_t wave, uint16_t *out_address
) {
    size_t index;
    const lt48_applet_address *found = NULL;
    for (index = 0; index < applet_count; ++index) {
        if (applets[index].wave == wave) {
            if (found) return LT48_PROTOCOL_ERROR;
            found = &applets[index];
        }
    }
    if (!found) return LT48_NOT_FOUND;
    *out_address = found->address;
    return LT48_OK;
}

static lt48_status create_plan_for_packet_size(
    size_t packet_size, lt48_protocol_plan **out_plan
) {
    lt48_protocol_plan *plan;
    if (!out_plan || (packet_size != 64u && packet_size != 512u))
        return LT48_INVALID_ARGUMENT;
    *out_plan = NULL;
    plan = calloc(1u, sizeof(*plan));
    if (!plan) return LT48_NO_MEMORY;
    plan->mode = 1u;
    plan->packet_size = packet_size;
    *out_plan = plan;
    return LT48_OK;
}

lt48_status lt48_protocol_plan_create(lt48_protocol_plan **out_plan) {
    return create_plan_for_packet_size(512u, out_plan);
}

lt48_status lt48_protocol_plan_create_offline_for_packet_size(
    size_t packet_size, lt48_protocol_plan **out_plan
) {
    return create_plan_for_packet_size(packet_size, out_plan);
}

size_t lt48_protocol_plan_packet_size_internal(const lt48_protocol_plan *plan) {
    return plan == NULL ? 0u : plan->packet_size;
}

void lt48_protocol_plan_close(lt48_protocol_plan *plan) {
    if (!plan) return;
    trim_transactions(plan, 0u);
    free(plan->transactions);
    free(plan);
}

lt48_status lt48_protocol_plan_apply_runtime_range(
    lt48_protocol_plan *plan,
    const lt48_runtime_result *runtime,
    size_t first_event,
    size_t event_count,
    const lt48_applet_address *applets,
    size_t applet_count
) {
    size_t saved_transaction_count;
    size_t saved_queue_size;
    unsigned saved_mode;
    unsigned char saved_queue[LT48_MAX_QUEUE_LIMIT];
    size_t event_index;
    lt48_status status = LT48_OK;
    if (!plan || !runtime || (applet_count && !applets)) {
        return LT48_INVALID_ARGUMENT;
    }
    if (plan->recovery_sealed || plan->m37_read_sealed) {
        return LT48_SAFETY_REJECTED;
    }
    if (first_event > lt48_runtime_event_count(runtime) ||
        event_count > lt48_runtime_event_count(runtime) - first_event) {
        return LT48_INVALID_ARGUMENT;
    }
    saved_transaction_count = plan->transaction_count;
    saved_queue_size = plan->queue_size;
    saved_mode = plan->mode;
    memcpy(saved_queue, plan->queue, saved_queue_size);
    plan->mode = 1u;
    for (event_index = first_event; event_index < first_event + event_count;
         ++event_index) {
        lt48_runtime_event event;
        status = lt48_runtime_get_event(runtime, event_index, &event);
        if (status != LT48_OK) break;
        switch (event.kind) {
        case LT48_RUNTIME_HOST_NOTIFICATION:
            if(event.argument_count==2u){
                if(!((event.arguments[0]==2u&&event.arguments[1]==2u)||(event.arguments[0]==3u&&event.arguments[1]==3u)||(event.arguments[0]==6u&&event.arguments[1]==3u)))status=LT48_UNSUPPORTED;
            }else{
                unsigned n=(unsigned)event.arguments[2];
                if(event.argument_count<3u || (n!=2u && n!=4u) ||
                   event.argument_count!=3u+n || event.arguments[1]!=3u ||
                   (event.arguments[0]!=4u && event.arguments[0]!=5u))status=LT48_UNSUPPORTED;
                else for(unsigned i=0;i<n;i++)if(event.arguments[3u+i]>255u)status=LT48_PROTOCOL_ERROR;
            }
            /* Host-only preservation warning. No controller transaction. */
            break;
        case LT48_RUNTIME_WAVE_WATCHDOG:
            if(event.argument_count!=2u || event.arguments[0]>UINT32_MAX ||
               event.arguments[1]>2u) status=LT48_PROTOCOL_ERROR;
            else status=lt48_protocol_plan_wave_watchdog(plan,(uint32_t)event.arguments[0],
                                                       0,0,event.arguments[1]==1u);
            break;
        case LT48_RUNTIME_Q1:
        case LT48_RUNTIME_Q2:
            if (event.argument_count != 1u || event.arguments[0] > 0xffu) {
                status = LT48_PROTOCOL_ERROR;
            } else {
                status = pair(plan, event.kind == LT48_RUNTIME_Q1 ? 1u : 2u,
                              (uint8_t)event.arguments[0]);
            }
            break;
        case LT48_RUNTIME_VOLTAGE: {
            unsigned char data[3];
            if (event.argument_count != 3u || event.arguments[0] > 3u ||
                event.arguments[2] > 0xffu) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            data[0] = 0x10u;
            data[1] = (unsigned char)event.arguments[0];
            data[2] = (unsigned char)event.arguments[2];
            status = write_block(plan, data, sizeof(data));
            break;
        }
        case LT48_RUNTIME_CONTROLLER_COMMAND_WRITE: {
            unsigned char data[LT48_RUNTIME_EVENT_ARGUMENTS];
            size_t index;
            if (!event.argument_count || event.argument_count > sizeof(data)) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            for (index = 0u; index < event.argument_count; ++index) {
                if (event.arguments[index] > UINT8_MAX) break;
                data[index] = (unsigned char)event.arguments[index];
            }
            status = index == event.argument_count ?
                write_block(plan, data, event.argument_count) :
                LT48_PROTOCOL_ERROR;
            break;
        }
        case LT48_RUNTIME_RAW_WRITE:
            status = raw_write(plan, event.arguments, event.argument_count);
            break;
        case LT48_RUNTIME_CONTROLLER_DELAY: {
            unsigned char data[3];
            size_t size;
            uint64_t delay;
            if (event.argument_count != 1u) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            delay = event.arguments[0];
            if (delay < 0x100u) {
                data[0] = 0x4eu;
                data[1] = (unsigned char)delay;
                size = 2u;
            } else {
                uint64_t scaled = delay / 100u;
                if (scaled > 0xffffu) {
                    status = LT48_PROTOCOL_ERROR;
                    break;
                }
                data[0] = 0x4fu;
                data[1] = (unsigned char)scaled;
                data[2] = (unsigned char)(scaled >> 8);
                size = 3u;
            }
            status = write_block(plan, data, size);
            break;
        }
        case LT48_RUNTIME_EXECUTE_WAVE: {
            unsigned char data[5];
            uint16_t address;
            if (event.argument_count != 1u || event.arguments[0] > 0xffffu) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            status = applet_address(applets, applet_count,
                                    (uint16_t)event.arguments[0], &address);
            if (status != LT48_OK) break;
            data[0] = 0x3bu; data[1] = 0xaau; data[2] = 0x55u;
            data[3] = (unsigned char)address;
            data[4] = (unsigned char)(address >> 8);
            status = write_block(plan, data, sizeof(data));
            break;
        }
        case LT48_RUNTIME_EVALUATE_VARIABLE:
        case LT48_RUNTIME_BUFFER_ARITHMETIC:
        case LT48_RUNTIME_BUFFER_BIT_COPY:
        case LT48_RUNTIME_BUFFER_MEMMOVE:
        case LT48_RUNTIME_BUFFER_BYTE_OPERATION:
        case LT48_RUNTIME_BUFFER_FILL:
        case LT48_RUNTIME_HOST_BITSORT:
        case LT48_RUNTIME_HOST_GAL_PES:
        case LT48_RUNTIME_FUSE_IMAGE_WRITE:
        case LT48_RUNTIME_FUSE_IMAGE_READ:
        case LT48_RUNTIME_FUSE_SECTION_SELECT:
        case LT48_RUNTIME_RETURN_STATUS:
        case LT48_RUNTIME_RESULT_LATCH:
            break;
        case LT48_RUNTIME_CONTROLLER_PACKED_WRITE: {
            unsigned char data[LT48_RUNTIME_EVENT_ARGUMENTS];
            size_t index;
            if (event.argument_count < 4u ||
                event.argument_count > sizeof(data)) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            for (index = 0u; index < event.argument_count; ++index) {
                if (event.arguments[index] > 0xffu) {
                    status = LT48_PROTOCOL_ERROR;
                    break;
                }
                data[index] = (unsigned char)event.arguments[index];
            }
            if (status == LT48_OK)
                status = write_block(plan, data, event.argument_count);
            break;
        }
        case LT48_RUNTIME_CONTROLLER_PIN_READ: {
            unsigned char data[50];
            size_t count;
            if (event.argument_count < 3u || event.arguments[0] >= LT48_RUNTIME_BUFFER_COUNT ||
                event.arguments[1] == 0u || event.arguments[1] > 48u ||
                event.argument_count != event.arguments[1] + 2u) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            count = (size_t)event.arguments[1];
            data[0] = 9u; data[1] = (unsigned char)count;
            for (size_t index = 0u; index < count; ++index) {
                if (event.arguments[index + 2u] > UINT8_MAX) {
                    status = LT48_PROTOCOL_ERROR;
                    break;
                }
                data[index + 2u] = (unsigned char)event.arguments[index + 2u];
            }
            if (status == LT48_OK) status = flush_queue(plan);
            if (status == LT48_OK) status = command(plan, data, count + 2u, 1);
            if (status == LT48_OK) {
                transaction_owned *capture = &plan->transactions[plan->transaction_count - 1u];
                capture->captures_runtime_byte = 1;
                capture->runtime_buffer = (uint8_t)event.arguments[0];
                capture->response_offset = 2u;
                capture->runtime_capture_length = (count + 7u) / 8u;
                capture->runtime_pin_count = count;
            }
            break;
        }
        case LT48_RUNTIME_RAW_READ: {
            if(event.argument_count!=6u || event.arguments[4]>255u || !event.arguments[5] || event.arguments[5]>32u){status=LT48_PROTOCOL_ERROR;break;}
            size_t n=(size_t)event.arguments[5];
            unsigned char header[]={0x43,0,0,(unsigned char)n,0};
            status=flush_queue(plan);
            if(status==LT48_OK)status=command(plan,header,sizeof(header),0);
            if(status==LT48_OK)status=append_transaction(plan,LT48_TRANSFER_IN,LT48_EP_IN,NULL,n);
            if(status==LT48_OK){transaction_owned *capture=&plan->transactions[plan->transaction_count-1];
                capture->captures_runtime_byte=1;capture->runtime_buffer=(uint8_t)event.arguments[4];
                capture->runtime_capture_length=n;capture->response_offset=0;}
            break;
        }
        case LT48_RUNTIME_CONTROLLER_PACKED_READ: {
            unsigned char data[4];
            unsigned char capture_request[3];
            size_t index;
            if (event.argument_count != 6u || event.arguments[4] >= LT48_RUNTIME_BUFFER_COUNT ||
                event.arguments[5] == 0u || event.arguments[5] > 32u) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            for (index = 0u; index < sizeof(data); ++index) {
                if (event.arguments[index] > 0xffu) {
                    status = LT48_PROTOCOL_ERROR;
                    break;
                }
                data[index] = (unsigned char)event.arguments[index];
            }
            if (status == LT48_OK) status = write_block(plan, data, sizeof(data));
            if (status == LT48_OK) status = flush_queue(plan);
            /* FUN_0045bb70 queues command 0x60, then transport slot +0x48
             * (FUN_0045b1b0) retrieves the captured bytes from the resident
             * controller's inline-result FIFO with command 0x0e.  The queue
             * acknowledgement is not the captured pin data. */
            capture_request[0] = 0x0eu;
            capture_request[1] = (unsigned char)event.arguments[5];
            capture_request[2] = 0u;
            if (status == LT48_OK)
                status = command(
                    plan, capture_request, sizeof(capture_request), 1
                );
            if (status == LT48_OK) {
                transaction_owned *capture =
                    &plan->transactions[plan->transaction_count - 1u];
                if (capture->direction != LT48_TRANSFER_IN ||
                    capture->endpoint != LT48_EP_IN ||
                    capture->length < (size_t)event.arguments[5]) {
                    status = LT48_PROTOCOL_ERROR;
                    break;
                }
                capture->captures_runtime_byte = 1;
                capture->runtime_buffer = (uint8_t)event.arguments[4];
                capture->response_offset = 0u;
                capture->runtime_capture_length =
                    (size_t)event.arguments[5];
            }
            break;
        }
        case LT48_RUNTIME_START_ENGINE: {
            static const unsigned char opcode[] = {3u};
            status = flush_queue(plan);
            if (status == LT48_OK) {
                status = command(plan, opcode, sizeof(opcode), 1);
            }
            break;
        }
        case LT48_RUNTIME_STATUS:
            status = status_command(plan);
            break;
        case LT48_RUNTIME_CAPTURE_RESPONSE_BYTE:
            if (event.argument_count != 1u || event.arguments[0] >= LT48_RUNTIME_BUFFER_COUNT) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            status = pair(plan, 3u, 0u);
            if (status == LT48_OK) status = flush_queue(plan);
            if (status == LT48_OK) {
                transaction_owned *capture =
                    &plan->transactions[plan->transaction_count - 1u];
                if (capture->direction != LT48_TRANSFER_IN ||
                    capture->endpoint != LT48_EP_IN ||
                    capture->length <= 2u) {
                    status = LT48_PROTOCOL_ERROR;
                    break;
                }
                capture->captures_runtime_byte = 1;
                capture->runtime_buffer = (uint8_t)event.arguments[0];
                capture->response_offset = 2u;
                capture->runtime_capture_length = 1u;
            }
            break;
        default:
            status = LT48_PROTOCOL_ERROR;
            break;
        }
        if (status != LT48_OK) break;
    }
    if (status == LT48_OK) {
        plan->mode = 2u;
        return LT48_OK;
    }
    trim_transactions(plan, saved_transaction_count);
    memcpy(plan->queue, saved_queue, saved_queue_size);
    plan->queue_size = saved_queue_size;
    plan->mode = saved_mode;
    return status;
}

lt48_status lt48_protocol_plan_apply_runtime(
    lt48_protocol_plan *plan,
    const lt48_runtime_result *runtime,
    const lt48_applet_address *applets,
    size_t applet_count
) {
    if (!runtime) return LT48_INVALID_ARGUMENT;
    return lt48_protocol_plan_apply_runtime_range(
        plan, runtime, 0u, lt48_runtime_event_count(runtime),
        applets, applet_count
    );
}

lt48_status lt48_protocol_plan_flush(lt48_protocol_plan *plan) {
    if (!plan) return LT48_INVALID_ARGUMENT;
    if (plan->recovery_sealed || plan->m37_read_sealed) {
        return LT48_SAFETY_REJECTED;
    }
    return flush_queue(plan);
}

lt48_status lt48_protocol_plan_controller_write(
    lt48_protocol_plan *plan, const unsigned char *bytes, size_t size
) {
    lt48_status status;
    if (!plan || !bytes || !size || size > 65536) return LT48_INVALID_ARGUMENT;
    if (plan->recovery_sealed || plan->m37_read_sealed) return LT48_SAFETY_REJECTED;
    plan->mode = 1;
    status = write_block(plan, bytes, size);
    plan->mode = 2;
    return status;
}

lt48_status lt48_protocol_plan_settle_status(lt48_protocol_plan *plan, int settle) {
    static const unsigned char delay[] = {0x4f, 0x64, 0};
    lt48_status status = LT48_OK;
    if (!plan || (settle != 0 && settle != 1)) return LT48_INVALID_ARGUMENT;
    if (plan->recovery_sealed || plan->m37_read_sealed) return LT48_SAFETY_REJECTED;
    plan->mode = 1;
    if (settle) status = write_block(plan, delay, sizeof(delay));
    if (status == LT48_OK) status = status_command(plan);
    plan->mode = 2;
    return status;
}
/* Private raw status collection for the source-bound PIC host. The caller
 * decodes ACK and fault flags; this cannot treat an unvalidated packet as an
 * operation success. Other status users retain their existing validation. */
lt48_status lt48_protocol_plan_status_capture_internal(lt48_protocol_plan *plan,int settle) {
    lt48_status status=lt48_protocol_plan_settle_status(plan,settle);
    if(status!=LT48_OK)return status;
    transaction_owned *reply=&plan->transactions[plan->transaction_count-1];
    if(reply->direction!=LT48_TRANSFER_IN || reply->request_opcode!=0x0d || reply->length<3)
        return LT48_PROTOCOL_ERROR;
    reply->validates_controller_response=0;
    reply->captures_runtime_byte=1;reply->runtime_buffer=0;
    reply->runtime_capture_length=3;reply->response_offset=0;
    return LT48_OK;
}

size_t lt48_protocol_plan_transaction_count(const lt48_protocol_plan *plan) {
    return plan ? plan->transaction_count : 0u;
}

lt48_status lt48_protocol_plan_get_transaction(
    const lt48_protocol_plan *plan, size_t index,
    lt48_protocol_transaction *out_transaction
) {
    const transaction_owned *transaction;
    if (!plan || !out_transaction) return LT48_INVALID_ARGUMENT;
    if (index >= plan->transaction_count) return LT48_NOT_FOUND;
    transaction = &plan->transactions[index];
    out_transaction->direction = transaction->direction;
    out_transaction->endpoint = transaction->endpoint;
    out_transaction->data = transaction->data;
    out_transaction->length = transaction->length;
    out_transaction->captures_runtime_byte = transaction->captures_runtime_byte;
    out_transaction->runtime_buffer = transaction->runtime_buffer;
    out_transaction->response_offset = transaction->response_offset;
    out_transaction->runtime_capture_length =
        transaction->runtime_capture_length;
    out_transaction->validates_controller_response =
        transaction->validates_controller_response;
    out_transaction->request_opcode = transaction->request_opcode;
    out_transaction->reads_device_data = transaction->reads_device_data;
    out_transaction->device_data_offset = transaction->device_data_offset;
    out_transaction->device_data_length = transaction->device_data_length;
    return LT48_OK;
}

lt48_status lt48_protocol_plan_select_id_policy_internal(lt48_protocol_plan *plan) {
    if (!plan || !plan->m37_read_sealed || !plan->dataman_id_required)
        return LT48_INVALID_ARGUMENT;
    plan->dataman_id_match_required = 0;
    return LT48_OK;
}

lt48_status lt48_protocol_plan_sha256(
    const lt48_protocol_plan *plan,
    unsigned char digest[LT48_SHA256_SIZE]
) {
    lt48_sha256_context hash;
    size_t index;
    if (plan == NULL || digest == NULL) return LT48_INVALID_ARGUMENT;
    lt48_sha256_init(&hash);
    for (index = 0u; index < plan->transaction_count; ++index) {
        const transaction_owned *transaction = &plan->transactions[index];
        unsigned char header[6];
        header[0] = transaction->direction == LT48_TRANSFER_OUT ? 'O' : 'I';
        header[1] = transaction->endpoint;
        header[2] = (unsigned char)transaction->length;
        header[3] = (unsigned char)(transaction->length >> 8);
        header[4] = (unsigned char)(transaction->length >> 16);
        header[5] = (unsigned char)(transaction->length >> 24);
        lt48_sha256_update(&hash, header, sizeof(header));
        if(plan->transactions[index].completion_data_width) {
            static const unsigned char completion_policy[]="WORD-COMPLETION-v1";
            lt48_sha256_update(&hash,completion_policy,sizeof(completion_policy)-1);
        }
        if (transaction->direction == LT48_TRANSFER_OUT) {
            lt48_sha256_update(
                &hash, transaction->data, transaction->length
            );
        }
    }
    if (plan->dataman_id_required) {
        unsigned char policy[12] = {'I','D',1,0};
        /* Preserve the oracle policy hash; distinguish selected-device mode. */
        if (!plan->dataman_id_match_required) policy[3] = 1u;
        uint32_t boundary = (uint32_t)plan->dataman_id_boundary;
        for (unsigned byte = 0u; byte < 4u; ++byte)
            policy[4u + byte] = (unsigned char)(boundary >> (8u * byte));
        if (plan->dataman_id_match_required) {
            policy[8] = (unsigned char)plan->dataman_manufacturer_id;
            policy[9] = (unsigned char)(plan->dataman_manufacturer_id >> 8);
            policy[10] = (unsigned char)plan->dataman_device_id;
            policy[11] = (unsigned char)(plan->dataman_device_id >> 8);
        }
        unsigned char power_boundary[8];
        for (unsigned byte = 0u; byte < 8u; ++byte)
            power_boundary[byte] = (unsigned char)(
                (uint64_t)plan->m37_powered_first_transaction >> (byte * 8u));
        lt48_sha256_update(&hash, policy, sizeof(policy));
        lt48_sha256_update(&hash, power_boundary, sizeof(power_boundary));
    }
    for (index = 0u; index < plan->transaction_count; ++index)
        if (plan->transactions[index].checks_dataman_m0_contacts) {
            static const unsigned char tag[] = "Dataman-D1-contact-normal-v1";
            unsigned char position[8];
            for (unsigned byte = 0u; byte < 8u; ++byte)
                position[byte] = (unsigned char)((uint64_t)index >> (byte * 8u));
            lt48_sha256_update(&hash, tag, sizeof(tag) - 1u);
            lt48_sha256_update(&hash, position, sizeof(position));
        }
    lt48_sha256_final(&hash, digest);
    return LT48_OK;
}

size_t lt48_protocol_plan_recovery_phase_count(
    const lt48_protocol_plan *plan
) {
    return plan && plan->recovery_sealed ? plan->recovery_phase_count : 0u;
}

lt48_status lt48_protocol_plan_get_recovery_phase(
    const lt48_protocol_plan *plan, size_t index,
    lt48_m37_cleanup_phase *out_phase,
    size_t *out_first_transaction,
    size_t *out_transaction_count
) {
    const recovery_phase_owned *span;
    if (plan == NULL || out_phase == NULL || out_first_transaction == NULL ||
        out_transaction_count == NULL) return LT48_INVALID_ARGUMENT;
    if (!plan->recovery_sealed || index >= plan->recovery_phase_count) {
        return LT48_NOT_FOUND;
    }
    span = &plan->recovery_phases[index];
    *out_phase = span->phase;
    *out_first_transaction = span->first_transaction;
    *out_transaction_count = span->transaction_count;
    return LT48_OK;
}

lt48_status lt48_protocol_plan_controller_capture_internal(lt48_protocol_plan *plan,unsigned count) {
    if(!plan||!count||count>32)return LT48_INVALID_ARGUMENT;
    const unsigned char request[]={0x0e,(unsigned char)count,0};
    lt48_status status=flush_queue(plan);
    if(status==LT48_OK)status=command(plan,request,sizeof(request),1);
    if(status==LT48_OK) {
        transaction_owned *capture=&plan->transactions[plan->transaction_count-1];
        capture->captures_runtime_byte=1;capture->runtime_buffer=0;
        capture->response_offset=0;capture->runtime_capture_length=count;
    }
    return status;
}
static lt48_status raw_read(
    lt48_protocol_plan *plan, uint16_t size, uint32_t device_offset, int controlled
) {
    unsigned char packet[LT48_MAX_PACKET_SIZE] = {0};
    lt48_status status;
    if (size == 0u) return LT48_INVALID_ARGUMENT;
    status = flush_queue(plan);
    if (status != LT48_OK) return status;
    packet[0] = controlled?0x43u:0x46u;
    packet[1] = (unsigned char)(size / plan->packet_size);
    packet[2] = (unsigned char)(size / plan->packet_size >> 8);
    packet[3] = (unsigned char)(size % plan->packet_size);
    packet[4] = (unsigned char)(size % plan->packet_size >> 8);
    packet[5] = controlled?0u:0xd0u;
    status = append_transaction(plan, LT48_TRANSFER_OUT,
                                LT48_EP_COMMAND_OUT, packet, plan->packet_size);
    if (status != LT48_OK) return status;
    status = append_transaction(plan, LT48_TRANSFER_IN, LT48_EP_IN, NULL, size);
    if (status == LT48_OK) {
        transaction_owned *transaction =
            &plan->transactions[plan->transaction_count - 1u];
        transaction->reads_device_data = 1;
        transaction->device_data_offset = device_offset;
        transaction->device_data_length = size;
    }
    return status;
}

static lt48_status program_block(
    lt48_protocol_plan *plan, const unsigned char *data, uint16_t size, unsigned width
) {
    unsigned char packet[LT48_MAX_PACKET_SIZE] = {0};
    lt48_status status;
    if (plan == NULL || data == NULL || size == 0u) {
        return LT48_INVALID_ARGUMENT;
    }
    status = flush_queue(plan);
    if (status != LT48_OK) return status;
    packet[0] = 0x47u;
    packet[1] = (unsigned char)(size / plan->packet_size);
    packet[2] = (unsigned char)(size / plan->packet_size >> 8);
    packet[3] = (unsigned char)(size % plan->packet_size);
    packet[4] = (unsigned char)(size % plan->packet_size >> 8);
    packet[5] = (unsigned char)width;
    status = append_transaction(plan, LT48_TRANSFER_OUT,
                                LT48_EP_COMMAND_OUT, packet, plan->packet_size);
    if (status == LT48_OK) {
        status = append_transaction(plan, LT48_TRANSFER_OUT,
                                    LT48_EP_DATA_OUT, data, size);
    }
    if (status == LT48_OK) {
        status = append_transaction(plan, LT48_TRANSFER_IN, LT48_EP_IN,
                                    NULL, plan->packet_size);
    }
    if (status == LT48_OK) {
        transaction_owned *reply =
            &plan->transactions[plan->transaction_count - 1u];
        reply->validates_controller_response = 1;
        reply->request_opcode = 0x47u;
    }
    return status;
}

#include "../../NewDeviceLib/sources/hitachipulse2/retry-policy.h"
static lt48_status apply_snapshot_operation(
    lt48_protocol_plan *plan, const lt48_m37_resources *resources,
    uint16_t operation_id, uint32_t mode, uint32_t address,
    uint32_t initial_voltage[4],
    uint16_t vcc_override_centivolts,
    const lt48_applet_address *applets, size_t applet_count
) {
    const lt48_compiled_wave *compiled;
    const lt48_pin_map *pin_map;
    const lt48_compile_variable *variables;
    lt48_compile_variable *overridden_variables = NULL;
    lt48_runtime_options options = {0};
    lt48_runtime_nested_wave nested[5];
    lt48_runtime_result *runtime = NULL;
    uint16_t wave;
    size_t variable_count;
    lt48_status status = lt48_m37_resources_compiled_wave(
        resources, operation_id, &wave, &compiled
    );
    if (status != LT48_OK) return status;
    (void)wave;
    pin_map = lt48_m37_resources_pin_map(resources);
    variables = lt48_m37_resources_variables(resources, &variable_count);
    if (pin_map == NULL || variables == NULL) return LT48_SAFETY_REJECTED;
    if(operation_id==3u&&(mode==2u||mode==7u)&&lt48_m37_eep21_write_voltage_internal(resources))vcc_override_centivolts=500u;
    if (vcc_override_centivolts != 0u) {
        size_t matches = 0u;
        overridden_variables = malloc(
            variable_count * sizeof(*overridden_variables)
        );
        if (overridden_variables == NULL) return LT48_NO_MEMORY;
        memcpy(overridden_variables, variables,
               variable_count * sizeof(*overridden_variables));
        for (size_t index = 0u; index < variable_count; ++index) {
            if (overridden_variables[index].variable_id != (lt48_m37_resources_audit(resources)->device_class==2004u?3u:0u)) continue;
            if (!overridden_variables[index].enabled ||
                (overridden_variables[index].channel != 0u &&
                 !(lt48_m37_resources_audit(resources)->complete_source_audit_verified &&
                   (lt48_m37_resources_audit(resources)->capacity_bytes>=131072u ||
                    (lt48_m37_resources_audit(resources)->device_class==2001u &&
                     lt48_m37_resources_audit(resources)->device_type>=0x3cu &&
                     lt48_m37_resources_audit(resources)->device_type<=0x40u)) &&
                   overridden_variables[index].channel<4u))) {
                free(overridden_variables);
                return LT48_SAFETY_REJECTED;
            }
            overridden_variables[index].value = vcc_override_centivolts;
            ++matches;
        }
        if (matches != 1u) {
            free(overridden_variables);
            return LT48_SAFETY_REJECTED;
        }
        variables = overridden_variables;
    }
    /* M1 DEVINIT invokes its POWERON/POWEROFF compact procedures.
     * Resolve those slots as compact code, never as applet addresses. */
    if (lt48_m37_resources_audit(resources)->complete_source_audit_verified &&
        lt48_m37_resources_audit(resources)->device_class == 2001u &&
        lt48_m37_resources_audit(resources)->family_code == 1u) {
        for (uint16_t id=0;id<5;++id) {
            const lt48_compiled_wave *child;uint16_t child_wave;
            status=lt48_m37_resources_compiled_wave(resources,id,&child_wave,&child);
            if(status!=LT48_OK){free(overridden_variables);return status;}
            nested[id]=(lt48_runtime_nested_wave){id,child};
        }
        options.nested_waves=nested;options.nested_wave_count=5;options.root_timeout_seconds=10;
    }
    options.host_staged_shutdown=lt48_m37_resources_is_eep21(resources);
    unsigned char retry_parameter=0;
    if(lt48_m37_resources_audit(resources)->device_class==2001&&lt48_m37_resources_audit(resources)->device_type==0x77&&operation_id==3&&mode==2){
        retry_parameter=(unsigned char)hitachipulse2_retry(variables,variable_count);
        if(!retry_parameter){free(overridden_variables);return LT48_SAFETY_REJECTED;}
        options.parameter_buffer=&retry_parameter;options.parameter_size=1;
    }
    options.mode = mode;
    options.address = address;
    if(lt48_m37_resources_audit(resources)->device_class==2004u) {
        options.use_explicit_buffer_ids=1;options.mode_buffer=3;
        options.address_buffer=1;options.parameter_buffer_id=2;
    }
    memcpy(options.initial_voltage_centivolts, initial_voltage,
           sizeof(options.initial_voltage_centivolts));
    status = lt48_runtime_interpret(
        compiled, pin_map, variables, variable_count, &options, &runtime
    );
    if (status == LT48_OK) {
        status = lt48_protocol_plan_apply_runtime(
            plan, runtime, applets, applet_count
        );
    }
    if (status==LT48_OK && lt48_m37_resources_audit(resources)->complete_source_audit_verified)
        for (uint8_t channel=0;channel<4 && status==LT48_OK;++channel)
            status=lt48_runtime_get_voltage(runtime,channel,&initial_voltage[channel]);
    lt48_runtime_result_close(runtime);
    free(overridden_variables);
    return status;
}

static lt48_status build_m37_read_or_compare_plan(
    const lt48_m37_resources *resources, uint32_t start, uint32_t length,
    const unsigned char *expected, lt48_m37_operation operation,
    int require_live_authorization, int resume_after_id_ignore,
    lt48_protocol_plan **out_plan
) {
    static const unsigned char initialize0[] = {0x11u,0xaau,0x55u,0,0,2};
    static const unsigned char initialize1[] = {0x11u,0xaau,0x55u,1,0,2};
    static const unsigned char fpga_header[] = {0x38u,0xaau,0x55u};
    static const unsigned char settle[] = {0x4fu,0x64u,0};
    static const unsigned char enter_operation[] = {0x56u,0x0au,1};
    static const unsigned char busy[] = {0x40u,1};
    static const unsigned char finalize[] = {0x49u,0xaau,0x55u};
    static const unsigned char good[] = {0x40u,4};
    static const unsigned char timeout500[] = {0x21,0x10,0x0a,0xf4,1,0,0};
    static const unsigned char timeout700[] = {0x21,0x10,0x0a,0xbc,2,0,0};
    static const unsigned char timeout1000[] = {0x21,0x10,0x0a,0xe8,3,0,0};
    static const unsigned char queue_reset[] = {0x2cu,0x08u,0x00u,0x00u};
    const lt48_m37_resource_audit *audit;
    const unsigned char *active_fpga;
    const unsigned char *idle_fpga;
    size_t active_fpga_size;
    size_t idle_fpga_size;
    lt48_m37_cleanup_phase cleanup[12];
    size_t cleanup_count;
    lt48_m37_plan_evidence evidence = {0};
    uint64_t issues;
    lt48_applet_address *applet_addresses = NULL;
    lt48_m37_applet_resource *applets = NULL;
    size_t applet_count;
    lt48_protocol_plan *plan = NULL;
    uint32_t initial_voltage[4] = {0,0,0,0};
    unsigned workflow_mode;
    int dataman_m0_read;
    size_t index;
    lt48_status status;
    if (resources == NULL || out_plan == NULL ||
        (operation != LT48_M37_READ && operation != LT48_M37_COMPARE) ||
        (operation == LT48_M37_COMPARE && expected == NULL)) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_plan = NULL;
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL) return LT48_SAFETY_REJECTED;
    unsigned width=(audit->device_class==2003u || audit->device_class==2004u)?2u:1u;
    if((start%width)||(length%width))return LT48_SAFETY_REJECTED;
    uint16_t chunk_limit=lt48_eprom_chunk(audit,length);
    dataman_m0_read = operation == LT48_M37_READ && audit->family_code == 0u &&
        audit->device_type == 1u && audit->pinmap == 5u && audit->pin_count == 28u &&
        audit->capacity_bytes == 16384u &&
        memcmp(audit->definition_sha256, dataman_m0_definition,
               sizeof(dataman_m0_definition)) == 0;
    if (resume_after_id_ignore && (!dataman_m0_read || require_live_authorization ||
        start != 0u || length != 16384u)) return LT48_SAFETY_REJECTED;
    status = lt48_m37_cleanup_recipe(
        1, 1, cleanup, sizeof(cleanup) / sizeof(cleanup[0]), &cleanup_count
    );
    if (status != LT48_OK) return status;
    evidence.operation = operation;
    evidence.start = start;
    evidence.length = length;
    evidence.chunk_size = lt48_eprom_chunk(audit,length);
    evidence.cleanup_phases = cleanup;
    evidence.cleanup_phase_count = cleanup_count;
    if (operation == LT48_M37_COMPARE) {
        evidence.has_image_digest = 1;
        evidence.image_size = length;
    }
    if (require_live_authorization) {
        status = lt48_m37_resources_validate_plan(
            resources, &evidence, &issues
        );
        if (status != LT48_OK) return status;
    } else if (operation != LT48_M37_READ || length == 0u ||
               start >= audit->capacity_bytes ||
               length > audit->capacity_bytes - start ||
               !audit->exact_engine_envelope ||
               !audit->strict_pin_mapping ||
               !audit->required_compact_waves_strict ||
               !audit->applet_metadata_complete ||
               !audit->applet_ranges_nonoverlapping ||
               !audit->active_fpga_exactly_one) {
        return LT48_SAFETY_REJECTED;
    }
    status = lt48_m37_resources_active_fpga(
        resources, &active_fpga, &active_fpga_size
    );
    if (status == LT48_OK) {
        status = lt48_m37_resources_idle_fpga(
            resources, &idle_fpga, &idle_fpga_size
        );
    }
    if (status != LT48_OK || active_fpga_size != 2778u ||
        idle_fpga_size != 2778u) {
        return status == LT48_OK ? LT48_SAFETY_REJECTED : status;
    }
    applet_count = lt48_m37_resources_applet_count(resources);
    applet_addresses = calloc(applet_count ? applet_count : 1u,
                              sizeof(*applet_addresses));
    applets = calloc(applet_count ? applet_count : 1u, sizeof(*applets));
    if (applet_addresses == NULL || applets == NULL) {
        status = LT48_NO_MEMORY;
        goto fail;
    }
    for (index = 0u; index < applet_count; ++index) {
        size_t position;
        status = lt48_m37_resources_get_applet(resources, index, &applets[index]);
        if (status != LT48_OK) goto fail;
        for (position = index; position > 0u &&
             applets[position - 1u].wave > applets[position].wave; --position) {
            lt48_m37_applet_resource swap = applets[position - 1u];
            applets[position - 1u] = applets[position];
            applets[position] = swap;
        }
    }
    for (index = 0u; index < applet_count; ++index) {
        applet_addresses[index].wave = applets[index].operation_id;
        applet_addresses[index].address = applets[index].load_address;
    }
    status = lt48_protocol_plan_create(&plan);
    if (status != LT48_OK) goto fail;
    if (dataman_m0_read) {
        /* Transport negotiation precedes arming. Original Read begins
         * controller queue preparation, not another speed/size probe. */
        status = command(plan, queue_reset, sizeof(queue_reset), 1);
    } else {
        status = command(plan, initialize0, sizeof(initialize0), 1);
        if (status == LT48_OK)
            status = command(plan, initialize1, sizeof(initialize1), 1);
    }
    /* Source 49b7b0/49e400 resets the host/FX2 window before controller
     * downloads. A previous native session may leave an expired deadline;
     * bridge size negotiation alone does not renew it. */
    if(status==LT48_OK && width==2u && audit->complete_source_audit_verified)
        status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
    for (index = 0u; status == LT48_OK && !resume_after_id_ignore &&
         index < applet_count; ++index) {
        unsigned char header[7];
        size_t last;
        if (applets[index].data_size == 0u) {
            status = LT48_SAFETY_REJECTED;
            break;
        }
        last = applets[index].data_size - 1u;
        if (last > UINT16_MAX) {
            status = LT48_SAFETY_REJECTED;
            break;
        }
        header[0] = 0x39u; header[1] = 0xaau; header[2] = 0x55u;
        header[3] = (unsigned char)applets[index].load_address;
        header[4] = (unsigned char)(applets[index].load_address >> 8);
        header[5] = (unsigned char)last;
        header[6] = (unsigned char)(last >> 8);
        status = write_block(plan, header, sizeof(header));
        /* Original M0 acknowledges each applet header and body separately,
         * then resets the queue before the next upload/operation. */
        if (status == LT48_OK && dataman_m0_read) status = flush_queue(plan);
        if (status == LT48_OK)
            status = write_block(plan, applets[index].data,
                                 applets[index].data_size);
        if (status == LT48_OK && dataman_m0_read) status = flush_queue(plan);
        if (status == LT48_OK && dataman_m0_read)
            status = command(plan, queue_reset, sizeof(queue_reset), 1);
    }
    if (status == LT48_OK && dataman_m0_read && !resume_after_id_ignore) {
        static const unsigned char contact[] = {0x47,0xaa,0x55};
        static const unsigned char ready[] = {0x29,0xaa,0x55,0};
        static const unsigned char fetch[] = {0x0e,12,0};
        status = write_block(plan, contact, sizeof(contact));
        if (status == LT48_OK) status = flush_queue(plan);
        if (status == LT48_OK) status = command(plan, ready, sizeof(ready), 1);
        if (status == LT48_OK) status = command(plan, fetch, sizeof(fetch), 1);
        if (status == LT48_OK) {
            plan->transactions[plan->transaction_count - 1u].checks_dataman_m0_contacts = 1;
            status = command(plan, queue_reset, sizeof(queue_reset), 1);
        }
    }
    if (status == LT48_OK)
        status = write_block(plan, fpga_header, sizeof(fpga_header));
    if (status == LT48_OK)
        status = write_block(plan, active_fpga, active_fpga_size);
    if (status == LT48_OK && dataman_m0_read)
        status = command(plan, queue_reset, sizeof(queue_reset), 1);
    if (status == LT48_OK) {
        unsigned char selector[] = {0x0fu, audit->expected_vcc_selector};
        status = write_block(plan, selector, sizeof(selector));
    }
    /* Original 0044f3c0 maps the Compare command (5) to device Read
     * mode (1). Compare retains its separate host acquisition method. */
    workflow_mode = 1u;
    if (status == LT48_OK && dataman_m0_read) {
        static const unsigned char watchdog[] = {0x21,0x10,0x0a,0xe8,3,0,0};
        status = flush_queue(plan);
        if (status == LT48_OK) status = command(plan, watchdog, sizeof(watchdog), 0);
        /* Failure anywhere in POWERON may leave rails/pins active. */
        plan->m37_powered_first_transaction = plan->transaction_count;
    }
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 0u, workflow_mode, 0u,
                                          initial_voltage, 0u, applet_addresses,
                                          applet_count);
    /* Original D1 Read emits three separately acknowledged settle cycles. */
    for (index = 0u; status == LT48_OK &&
         index < (dataman_m0_read ? 3u : 2u); ++index) {
        static const unsigned char timeout700[] = {0x21,0x10,0x0a,0xbc,2,0,0};
        static const unsigned char timeout500[] = {0x21,0x10,0x0a,0xf4,1,0,0};
        if (dataman_m0_read) {
            status = flush_queue(plan);
            if (status == LT48_OK)
                status = command(plan, timeout700, sizeof(timeout700), 0);
        }
        if (status == LT48_OK) status = write_block(plan, settle, sizeof(settle));
        if (status == LT48_OK && dataman_m0_read) {
            status = flush_queue(plan);
            if (status == LT48_OK)
                status = command(plan, timeout500, sizeof(timeout500), 0);
        }
        if (status == LT48_OK) status = status_command(plan);
    }
    if (status == LT48_OK && dataman_m0_read)
        status = command(plan, queue_reset, sizeof(queue_reset), 1);
    if (status == LT48_OK)
        status = write_block(plan, enter_operation, sizeof(enter_operation));
    if (status == LT48_OK && dataman_m0_read) {
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, queue_reset, sizeof(queue_reset), 1);
    }
    if (status == LT48_OK) status = write_block(plan, busy, sizeof(busy));
    if(audit->capacity_bytes<131072u)initial_voltage[0] = audit->nominal_vcc_centivolts;
    if (!dataman_m0_read)
        plan->m37_powered_first_transaction = plan->transaction_count;
    if (status == LT48_OK && dataman_m0_read && !resume_after_id_ignore) {
        /* Original 13.10 restores 1000 immediately before the ID wave,
         * even when POWERON already wrote that value. Normal UI capture
         * 20260927021418536991 and the earlier mismatch capture agree. */
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, timeout1000, sizeof(timeout1000), 0);
        if (status == LT48_OK)
            status = apply_snapshot_operation(plan, resources, 2u, workflow_mode, start,
                                          initial_voltage, 0u, applet_addresses,
                                          applet_count);
        if (status == LT48_OK) {
            status = lt48_m37_resources_catalogue_ids_internal(
                resources, &plan->dataman_manufacturer_id,
                &plan->dataman_device_id);
            plan->dataman_id_boundary = plan->transaction_count;
            plan->dataman_id_required = status == LT48_OK;
            /* A mismatch invokes Abort/Retry/Ignore in the operation owner.
             * Ignore retains the selected device procedure. */
            plan->dataman_id_match_required = 1;
        }
        /* ID's trailing route writes remain queued into the first Read
         * packet; the original inserts kind08 before Read's C4/C8 writes. */
        if (status == LT48_OK) status = pair(plan, 8u, 0u);
    }
    /* Original Ignore skips resident helper/contact/ID repetition. With
     * ID controls flushed before the dialog, Read begins without that prefix
     * and restores timeout1000 after the power/busy phase. */
    if (status == LT48_OK && resume_after_id_ignore) {
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, timeout1000, sizeof(timeout1000), 0);
    }
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 3u, workflow_mode, start/width,
                                          initial_voltage, 0u, applet_addresses,
                                          applet_count);
    if (status == LT48_OK && dataman_m0_read) {
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, timeout500, sizeof(timeout500), 0);
    }
    if (operation == LT48_M37_READ) {
        uint32_t device_offset = start;
        while (status == LT48_OK && length != 0u) {
            uint16_t chunk = length > chunk_limit ? chunk_limit : (uint16_t)length;
            /* Original 13.10 normal-UI M0 Read resets the queue and
             * consumes its acknowledgement before each subsequent input.
             * Existing approved profiles retain their transaction shape. */
            if (dataman_m0_read && device_offset != start)
                status = command(plan, queue_reset, sizeof(queue_reset), 1);
            if(status==LT48_OK && width==2u && audit->complete_source_audit_verified)
                status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
            if (status == LT48_OK)
                status = raw_read(plan, chunk, device_offset,audit->device_class==2004u);
            device_offset += chunk;
            length -= chunk;
        }
    } else {
        const unsigned char read_byte[] = {audit->device_class==2004u?0x04u:0x41u, audit->device_class==2004u?0u:0xd0u};
        if(status==LT48_OK)status=flush_queue(plan);
        for (index = 0u; status == LT48_OK && index < length; ++index) {
            if(width==2u && audit->complete_source_audit_verified && index%1024u==0)
                status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
            if(status==LT48_OK)status = command(plan, read_byte, sizeof(read_byte), 1);
            if (status == LT48_OK) {
                transaction_owned *reply =
                    &plan->transactions[plan->transaction_count - 1u];
                reply->reads_device_data = 1;
                reply->device_data_offset = start + (uint32_t)index;
                reply->response_offset = audit->device_class==2004u?2u:1u;
                reply->device_data_length = 1u;
            }
        }
    }
    if(status==LT48_OK && width==2u && audit->complete_source_audit_verified)
        status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
    if (!audit->complete_source_audit_verified)
        initial_voltage[3] = audit->nominal_vcc_centivolts;
    if (status == LT48_OK && dataman_m0_read) {
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, timeout1000, sizeof(timeout1000), 0);
    }
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 4u, workflow_mode, 0u,
                                          initial_voltage, 0u, applet_addresses,
                                          applet_count);
    /* Original M0 Read completes and acknowledges DEVRESET's queued
     * 2A/22/14 writes before beginning the settling phase. */
    if (status == LT48_OK && dataman_m0_read) {
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, timeout700, sizeof(timeout700), 0);
    }
    if (status == LT48_OK) status = write_block(plan, settle, sizeof(settle));
    if (status == LT48_OK && dataman_m0_read) {
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, timeout500, sizeof(timeout500), 0);
    }
    if (status == LT48_OK) status = status_command(plan);
    if (status == LT48_OK && dataman_m0_read)
        status = command(plan, timeout1000, sizeof(timeout1000), 0);
    memset(initial_voltage, 0, sizeof(initial_voltage));
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 1u, workflow_mode, 0u,
                                          initial_voltage, 0u, applet_addresses,
                                          applet_count);
    if (status == LT48_OK && dataman_m0_read) {
        status = flush_queue(plan);
        if (status == LT48_OK)
            status = command(plan, timeout500, sizeof(timeout500), 0);
    }
    if (status == LT48_OK)
        status = write_block(plan, fpga_header, sizeof(fpga_header));
    if (status == LT48_OK) status = write_block(plan, idle_fpga, idle_fpga_size);
    /* Captured 13.10 M0 Read acknowledges queue init after idle FPGA
     * restoration and before resident reset 49 AA 55. */
    if (status == LT48_OK && dataman_m0_read)
        status = command(plan, queue_reset, sizeof(queue_reset), 1);
    if (status == LT48_OK)
        status = write_block(plan, finalize, sizeof(finalize));
    if (status == LT48_OK) status = flush_queue(plan);
    /* Normal UI finalization after the original Read method returns:
     * queue init is acknowledged before the good-result command. */
    if (status == LT48_OK && dataman_m0_read)
        status = command(plan, queue_reset, sizeof(queue_reset), 1);
    if (status == LT48_OK) status = write_block(plan, good, sizeof(good));
    if (status != LT48_OK) goto fail;
    memcpy(plan->m37_resource_manifest_sha256,
           audit->resource_manifest_sha256, LT48_SHA256_SIZE);
    plan->m37_read_sealed = 1;
    free(applets);
    free(applet_addresses);
    *out_plan = plan;
    return LT48_OK;

fail:
    lt48_protocol_plan_close(plan);
    free(applets);
    free(applet_addresses);
    return status;
}

lt48_status lt48_m37_resources_build_read_plan(
    const lt48_m37_resources *resources, uint32_t start, uint32_t length,
    lt48_protocol_plan **out_plan
) {
    return build_m37_read_or_compare_plan(
        resources, start, length, NULL, LT48_M37_READ, 1, 0, out_plan
    );
}

lt48_status lt48_m37_resources_build_offline_read_plan(
    const lt48_m37_resources *resources, uint32_t start, uint32_t length,
    lt48_protocol_plan **out_plan
) {
    return build_m37_read_or_compare_plan(
        resources, start, length, NULL, LT48_M37_READ, 0, 0, out_plan
    );
}

/* Offline projection of the captured Retry restart. Helpers remain resident.
 * This does not authorize execution or provide an operator decision loop. */
lt48_status lt48_m37_resources_build_id_retry_candidate_internal(
    const lt48_m37_resources *resources, lt48_protocol_plan **out_plan
) {
    static const unsigned char contact[] = {0x2c,1,0x47,1,0xaa,1,0x55,0};
    static const unsigned char reset[] = {0x2c,8,0,0};
    lt48_protocol_plan *plan = NULL;
    size_t first = SIZE_MAX;
    lt48_status status;
    if (!out_plan) return LT48_INVALID_ARGUMENT;
    *out_plan = NULL;
    status = build_m37_read_or_compare_plan(
        resources, 0u, 16384u, NULL, LT48_M37_READ, 0, 0, &plan);
    if (status != LT48_OK) return status;
    if (!plan->dataman_id_required) goto reject;
    for (size_t i = 2u; i < plan->transaction_count; ++i) {
        const transaction_owned *t = &plan->transactions[i];
        if (t->direction == LT48_TRANSFER_OUT && t->length >= sizeof(contact) &&
            memcmp(t->data, contact, sizeof(contact)) == 0) {
            if (first != SIZE_MAX) goto reject;
            first = i - 2u;
        }
    }
    if (first == SIZE_MAX ||
        plan->transactions[first].direction != LT48_TRANSFER_OUT ||
        plan->transactions[first].length < sizeof(reset) ||
        memcmp(plan->transactions[first].data, reset, sizeof(reset)) != 0 ||
        plan->transactions[first + 1u].direction != LT48_TRANSFER_IN ||
        !plan->transactions[first + 1u].validates_controller_response ||
        plan->dataman_id_boundary <= first ||
        plan->m37_powered_first_transaction <= first) goto reject;
    /* Retain the complete suffix, including the contact gate and cleanup. */
    for (size_t i = 0u; i < first; ++i) if(plan->transactions[i].owns_data)free(plan->transactions[i].data);
    memmove(plan->transactions, plan->transactions + first,
            (plan->transaction_count - first) * sizeof(*plan->transactions));
    plan->transaction_count -= first;
    plan->dataman_id_boundary -= first;
    plan->m37_powered_first_transaction -= first;
    *out_plan = plan;
    return LT48_OK;
reject:
    lt48_protocol_plan_close(plan);
    return LT48_SAFETY_REJECTED;
}

/* Private candidate; normal authorization and GUI do not expose this path. */
lt48_status lt48_m37_resources_build_id_ignore_candidate_internal(
    const lt48_m37_resources *resources, lt48_protocol_plan **out_plan
) {
    return build_m37_read_or_compare_plan(
        resources, 0u, 16384u, NULL, LT48_M37_READ, 0, 1, out_plan);
}

lt48_status lt48_m37_resources_build_compare_plan(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *expected, size_t length,
    lt48_protocol_plan **out_plan
) {
    if (length > UINT32_MAX) return LT48_SAFETY_REJECTED;
    return build_m37_read_or_compare_plan(
        resources, start, (uint32_t)length, expected,
        LT48_M37_COMPARE, 1, 0, out_plan
    );
}

lt48_status lt48_m37_compare_evaluate(
    uint32_t start, const unsigned char *expected,
    const unsigned char *actual, size_t length,
    lt48_m37_compare_decision_fn decide, void *decision_context,
    lt48_m37_compare_result *out_result
) {
    size_t index;
    if (expected == NULL || actual == NULL || out_result == NULL ||
        length == 0u || length > UINT32_MAX ||
        (uint64_t)start + (uint64_t)length > UINT64_C(0x100000000)) {
        return LT48_INVALID_ARGUMENT;
    }
    memset(out_result, 0, sizeof(*out_result));
    out_result->matched = 1;
    out_result->first_mismatch_address = UINT32_MAX;
    out_result->last_mismatch_address = UINT32_MAX;
    for (index = 0u; index < length; ++index) {
        uint32_t address = start + (uint32_t)index;
        lt48_m37_compare_decision decision;
        ++out_result->compared_size;
        if (expected[index] == actual[index]) continue;
        out_result->matched = 0;
        if (out_result->mismatch_count == 0u) {
            out_result->first_mismatch_address = address;
            out_result->first_expected = expected[index];
            out_result->first_actual = actual[index];
        }
        ++out_result->mismatch_count;
        out_result->last_mismatch_address = address;
        out_result->last_expected = expected[index];
        out_result->last_actual = actual[index];
        decision = decide == NULL ? LT48_M37_COMPARE_CONTINUE :
                   decide(decision_context, address,
                          expected[index], actual[index]);
        if (decision == LT48_M37_COMPARE_ABORT) {
            out_result->aborted = 1;
            break;
        }
        if (decision != LT48_M37_COMPARE_CONTINUE)
            return LT48_INVALID_ARGUMENT;
    }
    return LT48_OK;
}

static lt48_status build_m37_compare_plan(
    const lt48_m37_resources *resources, lt48_m37_operation operation,
    uint32_t start, const unsigned char *expected, uint32_t length,
    uint16_t verify_vcc_centivolts, unsigned forced_mode,
    lt48_protocol_plan **out_plan
) {
    static const unsigned char initialize0[] = {0x11u,0xaau,0x55u,0,0,2};
    static const unsigned char initialize1[] = {0x11u,0xaau,0x55u,1,0,2};
    static const unsigned char fpga_header[] = {0x38u,0xaau,0x55u};
    static const unsigned char settle[] = {0x4fu,0x64u,0};
    static const unsigned char enter_operation[] = {0x56u,0x0au,1};
    static const unsigned char busy[] = {0x40u,1};
    static const unsigned char finalize[] = {0x49u,0xaau,0x55u};
    static const unsigned char good[] = {0x40u,4};
    const lt48_m37_resource_audit *audit;
    const unsigned char *active_fpga;
    const unsigned char *idle_fpga;
    size_t active_fpga_size;
    size_t idle_fpga_size;
    lt48_m37_cleanup_phase cleanup[12];
    size_t cleanup_count;
    lt48_m37_plan_evidence evidence = {0};
    uint64_t issues;
    lt48_applet_address *applet_addresses = NULL;
    lt48_m37_applet_resource *applets = NULL;
    size_t applet_count;
    lt48_protocol_plan *plan = NULL;
    uint32_t initial_voltage[4] = {0,0,0,0};
    unsigned mode;
    size_t index;
    lt48_status status;
    if (resources == NULL || out_plan == NULL || expected == NULL ||
        (operation != LT48_M37_BLANK_CHECK &&
         operation != LT48_M37_VERIFY &&
         operation != LT48_M37_PROGRAM && operation != LT48_M37_ELECTRICAL_ERASE)) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_plan = NULL;
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL) return LT48_SAFETY_REJECTED;
    /* Exact source mask-ROM definition: no voltage/programming path. */
    if(audit->device_class==2006u && audit->device_type==0x0eu &&
       operation!=LT48_M37_VERIFY)return LT48_UNSUPPORTED;
    int erasing=operation==LT48_M37_ELECTRICAL_ERASE;
    if(erasing&&lt48_m37_has_dynamic_erase(resources))return LT48_UNSUPPORTED;
    if(erasing && !(audit->complete_source_audit_verified && ((audit->device_class==2006u && audit->device_type==5u)||lt48_m37_resources_is_eep21(resources)) && start==0 && length==audit->capacity_bytes))return LT48_SAFETY_REJECTED;
    unsigned width=(audit->device_class==2003u || audit->device_class==2004u)?2u:1u;
    if((start%width)||(length%width))return LT48_SAFETY_REJECTED;
    uint16_t chunk_limit=lt48_eprom_chunk(audit,length);
    if (operation == LT48_M37_BLANK_CHECK || erasing ||
        operation == LT48_M37_PROGRAM) {
        verify_vcc_centivolts = audit->nominal_vcc_centivolts;
    } else if (verify_vcc_centivolts < 100u ||
               verify_vcc_centivolts > 700u ||
               (verify_vcc_centivolts <= 400u ? 0x07u : 0x0fu) !=
                   audit->expected_vcc_selector) {
        return LT48_SAFETY_REJECTED;
    }
    status = lt48_m37_cleanup_recipe(
        1, 1, cleanup, sizeof(cleanup) / sizeof(cleanup[0]), &cleanup_count
    );
    if (status != LT48_OK) return status;
    evidence.operation = operation;
    if(erasing)evidence.mandatory_post_verify=1;
    evidence.start = start;
    evidence.length = length;
    evidence.chunk_size = lt48_eprom_chunk(audit,length);
    evidence.cleanup_phases = cleanup;
    evidence.cleanup_phase_count = cleanup_count;
    if (operation == LT48_M37_VERIFY ||
        operation == LT48_M37_PROGRAM) {
        evidence.has_image_digest = 1;
        evidence.image_size = length;
    }
    if (operation == LT48_M37_PROGRAM) {
        /*
         * This builder is reachable only through the composite program set,
         * which binds these mandatory surrounding phases. The selected
         * definition supplies the procedure independently of Auto-ID.
         */
        evidence.precheck = LT48_M37_PRECHECK_BLANK;
        evidence.mandatory_post_verify = 1;
    }
    status = lt48_m37_resources_validate_plan(resources, &evidence, &issues);
    if (status != LT48_OK) return status;
    status = lt48_m37_resources_active_fpga(
        resources, &active_fpga, &active_fpga_size
    );
    if (status == LT48_OK) {
        status = lt48_m37_resources_idle_fpga(
            resources, &idle_fpga, &idle_fpga_size
        );
    }
    if (status != LT48_OK || active_fpga_size != 2778u ||
        idle_fpga_size != 2778u) {
        return status == LT48_OK ? LT48_SAFETY_REJECTED : status;
    }
    applet_count = lt48_m37_resources_applet_count(resources);
    applet_addresses = calloc(applet_count ? applet_count : 1u,
                              sizeof(*applet_addresses));
    applets = calloc(applet_count ? applet_count : 1u, sizeof(*applets));
    if (applet_addresses == NULL || applets == NULL) {
        status = LT48_NO_MEMORY;
        goto compare_fail;
    }
    for (index = 0u; index < applet_count; ++index) {
        size_t position;
        status = lt48_m37_resources_get_applet(resources, index, &applets[index]);
        if (status != LT48_OK) goto compare_fail;
        for (position = index; position > 0u &&
             applets[position - 1u].wave > applets[position].wave; --position) {
            lt48_m37_applet_resource swap = applets[position - 1u];
            applets[position - 1u] = applets[position];
            applets[position] = swap;
        }
    }
    for (index = 0u; index < applet_count; ++index) {
        applet_addresses[index].wave = applets[index].operation_id;
        applet_addresses[index].address = applets[index].load_address;
    }
    status = lt48_protocol_plan_create(&plan);
    if (status != LT48_OK) goto compare_fail;
    status = command(plan, initialize0, sizeof(initialize0), 1);
    if (status == LT48_OK)
        status = command(plan, initialize1, sizeof(initialize1), 1);
    if(status==LT48_OK && width==2u && audit->complete_source_audit_verified)
        status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
    for (index = 0u; status == LT48_OK && index < applet_count; ++index) {
        unsigned char header[7];
        size_t last;
        if (applets[index].data_size == 0u) {
            status = LT48_SAFETY_REJECTED;
            break;
        }
        last = applets[index].data_size - 1u;
        if (last > UINT16_MAX) {
            status = LT48_SAFETY_REJECTED;
            break;
        }
        header[0] = 0x39u; header[1] = 0xaau; header[2] = 0x55u;
        header[3] = (unsigned char)applets[index].load_address;
        header[4] = (unsigned char)(applets[index].load_address >> 8);
        header[5] = (unsigned char)last;
        header[6] = (unsigned char)(last >> 8);
        status = write_block(plan, header, sizeof(header));
        if (status == LT48_OK)
            status = write_block(plan, applets[index].data,
                                 applets[index].data_size);
    }
    if (status == LT48_OK)
        status = write_block(plan, fpga_header, sizeof(fpga_header));
    if (status == LT48_OK)
        status = write_block(plan, active_fpga, active_fpga_size);
    if (status == LT48_OK) {
        unsigned char selector[] = {0x0fu, (unsigned char)((operation==LT48_M37_PROGRAM||erasing)&&lt48_m37_eep21_write_voltage_internal(resources)?0x0f:audit->expected_vcc_selector)};
        status = write_block(plan, selector, sizeof(selector));
    }
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 0u, 2u, 0u,
                                          initial_voltage, 0u, applet_addresses,
                                          applet_count);
    for (index = 0u; status == LT48_OK && index < 2u; ++index) {
        status = write_block(plan, settle, sizeof(settle));
        if (status == LT48_OK) status = status_command(plan);
    }
    if (status == LT48_OK)
        status = write_block(plan, enter_operation, sizeof(enter_operation));
    if (status == LT48_OK) status = write_block(plan, busy, sizeof(busy));
    mode = operation == LT48_M37_BLANK_CHECK || erasing ? 11u :
           operation == LT48_M37_PROGRAM ? (forced_mode ? forced_mode : 2u) : 4u;
    if(audit->capacity_bytes<131072u)initial_voltage[0] = verify_vcc_centivolts;
    plan->m37_powered_first_transaction = plan->transaction_count;
    if(status==LT48_OK && erasing) {
        status=apply_snapshot_operation(plan,resources,3u,7u,0u,initial_voltage,0u,applet_addresses,applet_count);
        if(status==LT48_OK)status=apply_snapshot_operation(plan,resources,6u,7u,0u,initial_voltage,0u,applet_addresses,applet_count);
        if(status==LT48_OK)status=apply_snapshot_operation(plan,resources,4u,7u,0u,initial_voltage,0u,applet_addresses,applet_count);
        memset(initial_voltage,0,sizeof(initial_voltage));
        if(status==LT48_OK)status=apply_snapshot_operation(plan,resources,1u,7u,0u,initial_voltage,0u,applet_addresses,applet_count);
        if(status==LT48_OK)status=write_block(plan,settle,sizeof(settle));
        if(status==LT48_OK)status=apply_snapshot_operation(plan,resources,0u,11u,0u,initial_voltage,0u,applet_addresses,applet_count);
        if(status==LT48_OK)status=status_command(plan);
    }
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 3u, mode, start/width,
                                          initial_voltage,
                                          operation == LT48_M37_VERIFY ?
                                              verify_vcc_centivolts : 0u,
                                          applet_addresses,
                                          applet_count);
    if(status==LT48_OK)status=flush_queue(plan);
    if(status==LT48_OK && width==2u && audit->complete_source_audit_verified)
        status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
    for(unsigned byte=0;status==LT48_OK && byte<width;byte++) {
        unsigned char first[] = {0x04u, expected[byte]};
        status = command(plan, first, sizeof(first), 1);
    }
    for (index = width; status == LT48_OK && index < length; index += chunk_limit) {
        uint16_t chunk = (uint16_t)(length - index > chunk_limit ?
                                    chunk_limit : length - index);
        /* Original 451dae..451de6 renews a minimum five-second window
         * for each word transfer block. */
        if(width==2u && audit->complete_source_audit_verified)
            status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
        if(status==LT48_OK)status = program_block(plan, expected + index, chunk, width);
    }
    if (status == LT48_OK) {
        static const unsigned char complete[] = {0x40u,0};
        status = command(plan, complete, sizeof(complete), 1);
        if(status==LT48_OK && width==2u && audit->complete_source_audit_verified)
            plan->transactions[plan->transaction_count-1].completion_data_width=width;
    }
    if(status==LT48_OK && width==2u && audit->complete_source_audit_verified)
        status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
    if (!audit->complete_source_audit_verified) initial_voltage[3] = verify_vcc_centivolts;
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 4u, mode, 0u,
                                          initial_voltage,
                                          operation == LT48_M37_VERIFY ?
                                              verify_vcc_centivolts : 0u,
                                          applet_addresses,
                                          applet_count);
    if (status == LT48_OK) status = write_block(plan, settle, sizeof(settle));
    if (status == LT48_OK) status = status_command(plan);
    memset(initial_voltage, 0, sizeof(initial_voltage));
    if (status == LT48_OK)
        status = apply_snapshot_operation(plan, resources, 1u, 2u, 0u,
                                          initial_voltage, 0u, applet_addresses,
                                          applet_count);
    if (status == LT48_OK)
        status = write_block(plan, fpga_header, sizeof(fpga_header));
    if (status == LT48_OK) status = write_block(plan, idle_fpga, idle_fpga_size);
    if (status == LT48_OK)
        status = write_block(plan, finalize, sizeof(finalize));
    if (status == LT48_OK) status = flush_queue(plan);
    if (status == LT48_OK) status = write_block(plan, good, sizeof(good));
    if (status != LT48_OK) goto compare_fail;
    memcpy(plan->m37_resource_manifest_sha256,
           audit->resource_manifest_sha256, LT48_SHA256_SIZE);
    plan->m37_read_sealed = 1;
    free(applets);
    free(applet_addresses);
    *out_plan = plan;
    return LT48_OK;

compare_fail:
    lt48_protocol_plan_close(plan);
    free(applets);
    free(applet_addresses);
    return status;
}

lt48_status lt48_m37_resources_build_electrical_erase_plan(
    const lt48_m37_resources *resources,uint32_t start,uint32_t length,lt48_protocol_plan **out_plan) {
    if(!out_plan)return LT48_INVALID_ARGUMENT;
    *out_plan=NULL;
    const lt48_m37_resource_audit*a=lt48_m37_resources_audit(resources);
    if(!a||start||length!=a->capacity_bytes)return LT48_SAFETY_REJECTED;
    unsigned char *blank=malloc(length);if(!blank)return LT48_NO_MEMORY;memset(blank,255,length);
    lt48_status status=build_m37_compare_plan(resources,LT48_M37_ELECTRICAL_ERASE,start,blank,length,0,0,out_plan);
    free(blank);return status;
}

lt48_status lt48_m37_resources_build_blank_check_plan(
    const lt48_m37_resources *resources, uint32_t start, uint32_t length,
    lt48_protocol_plan **out_plan
) {
    const lt48_m37_resource_audit *audit;
    unsigned char *expected;
    lt48_protocol_plan *plan = NULL;
    lt48_status status;
    if (out_plan == NULL) return LT48_INVALID_ARGUMENT;
    *out_plan = NULL;
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL || length == 0u || start >= audit->capacity_bytes ||
        length > audit->capacity_bytes - start) {
        return LT48_SAFETY_REJECTED;
    }
    /* M37/W14 is proved to compare against the technology blank value 0xff. */
    expected = malloc(length);
    if (expected == NULL) return LT48_NO_MEMORY;
    memset(expected, 0xff, length);
    status = build_m37_compare_plan(
        resources, LT48_M37_BLANK_CHECK, start, expected, length, 0u, 0u, &plan
    );
    free(expected);
    if (status == LT48_OK) *out_plan = plan;
    return status;
}

lt48_status lt48_m37_resources_build_verify_plan(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *expected, size_t length,
    lt48_protocol_plan **out_plan
) {
    const lt48_m37_resource_audit *audit =
        lt48_m37_resources_audit(resources);
    if (audit == NULL) return LT48_INVALID_ARGUMENT;
    return lt48_m37_resources_build_verify_voltage_plan(
        resources, start, expected, length,
        audit->nominal_vcc_centivolts, out_plan
    );
}

lt48_status lt48_m37_resources_build_verify_voltage_plan(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *expected, size_t length,
    uint16_t verify_vcc_centivolts, lt48_protocol_plan **out_plan
) {
    if (length > UINT32_MAX) return LT48_SAFETY_REJECTED;
    return build_m37_compare_plan(
        resources, LT48_M37_VERIFY, start, expected, (uint32_t)length,
        verify_vcc_centivolts, 0u, out_plan
    );
}

lt48_status lt48_m37_verify_plan_set_create(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *expected, size_t length,
    const uint16_t *voltages, size_t voltage_count,
    lt48_m37_verify_plan_set **out_set
) {
    static const unsigned char tag[] = "LT48-M37-VERIFY-MARGINS-v1";
    lt48_m37_verify_plan_set *set;
    lt48_sha256_context hash;
    lt48_status status = LT48_OK;
    if (resources == NULL || expected == NULL || voltages == NULL ||
        out_set == NULL || voltage_count == 0u || voltage_count > 4u) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_set = NULL;
    for (size_t index = 0u; index < voltage_count; ++index) {
        if (voltages[index] < 100u || voltages[index] > 700u ||
            (index != 0u && voltages[index] <= voltages[index - 1u])) {
            return LT48_SAFETY_REJECTED;
        }
    }
    set = calloc(1u, sizeof(*set));
    if (set == NULL) return LT48_NO_MEMORY;
    set->count = voltage_count;
    memcpy(set->voltages, voltages, voltage_count * sizeof(*voltages));
    lt48_sha256_init(&hash);
    lt48_sha256_update(&hash, tag, sizeof(tag) - 1u);
    for (size_t index = 0u; index < voltage_count; ++index) {
        unsigned char plan_digest[LT48_SHA256_SIZE];
        unsigned char voltage_bytes[2] = {
            (unsigned char)voltages[index],
            (unsigned char)(voltages[index] >> 8)
        };
        status = lt48_m37_resources_build_verify_voltage_plan(
            resources, start, expected, length, voltages[index],
            &set->plans[index]
        );
        if (status == LT48_OK)
            status = lt48_protocol_plan_sha256(
                set->plans[index], plan_digest
            );
        if (status != LT48_OK) {
            lt48_m37_verify_plan_set_close(set);
            return status;
        }
        lt48_sha256_update(&hash, voltage_bytes, sizeof(voltage_bytes));
        lt48_sha256_update(&hash, plan_digest, sizeof(plan_digest));
    }
    lt48_sha256_final(&hash, set->sha256);
    *out_set = set;
    return LT48_OK;
}

void lt48_m37_verify_plan_set_close(lt48_m37_verify_plan_set *set) {
    if (set == NULL) return;
    for (size_t index = 0u; index < set->count; ++index)
        lt48_protocol_plan_close(set->plans[index]);
    free(set);
}

size_t lt48_m37_verify_plan_set_count(
    const lt48_m37_verify_plan_set *set
) {
    return set == NULL ? 0u : set->count;
}

lt48_status lt48_m37_verify_plan_set_get(
    const lt48_m37_verify_plan_set *set, size_t index,
    uint16_t *out_voltage_centivolts, const lt48_protocol_plan **out_plan
) {
    if (set == NULL || out_voltage_centivolts == NULL || out_plan == NULL)
        return LT48_INVALID_ARGUMENT;
    if (index >= set->count) return LT48_NOT_FOUND;
    *out_voltage_centivolts = set->voltages[index];
    *out_plan = set->plans[index];
    return LT48_OK;
}

lt48_status lt48_m37_verify_plan_set_sha256(
    const lt48_m37_verify_plan_set *set,
    unsigned char digest[LT48_SHA256_SIZE]
) {
    if (set == NULL || digest == NULL) return LT48_INVALID_ARGUMENT;
    memcpy(digest, set->sha256, LT48_SHA256_SIZE);
    return LT48_OK;
}

lt48_status lt48_m37_program_plan_set_create(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *image, size_t length,
    lt48_m37_program_plan_set **out_set
) {
    static const unsigned char tag[] = "LT48-M37-PROGRAM-PLAN-v1";

    lt48_m37_program_plan_set *set;
    lt48_sha256_context hash;
    const lt48_m37_resource_audit *audit;
    lt48_status status;
    if (resources == NULL || image == NULL || out_set == NULL ||
        length == 0u || length > UINT32_MAX) return LT48_INVALID_ARGUMENT;
    *out_set = NULL;
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL || start >= audit->capacity_bytes ||
        length > audit->capacity_bytes - start)
        return LT48_SAFETY_REJECTED;
    set = calloc(1u, sizeof(*set));
    if (set == NULL) return LT48_NO_MEMORY;
    set->count = audit->device_class==2006u ? 4u : 3u;
    status = lt48_m37_resources_build_blank_check_plan(
        resources, start, (uint32_t)length, &set->plans[0]
    );
    if (status == LT48_OK)
        status = build_m37_compare_plan(
            resources, LT48_M37_PROGRAM, start, image, (uint32_t)length,
            audit->nominal_vcc_centivolts, 0u, &set->plans[1]
        );
    if (status==LT48_OK && set->count==4u)
        status=build_m37_compare_plan(resources,LT48_M37_PROGRAM,start,image,
            (uint32_t)length,audit->nominal_vcc_centivolts,3u,&set->plans[2]);
    if (status == LT48_OK)
        status = lt48_m37_resources_build_verify_voltage_plan(
            resources, start, image, length, audit->nominal_vcc_centivolts,
            &set->plans[set->count-1]
        );
    if (status != LT48_OK) {
        lt48_m37_program_plan_set_close(set);
        return status;
    }
    lt48_sha256_init(&hash);
    lt48_sha256_update(&hash, tag, sizeof(tag) - 1u);
    for (size_t index = 0u; index < set->count; ++index) {
        unsigned char digest[LT48_SHA256_SIZE];
        unsigned char operation = (unsigned char)(index==0 ? LT48_M37_BLANK_CHECK :
            index==set->count-1 ? LT48_M37_VERIFY : LT48_M37_PROGRAM);
        status = lt48_protocol_plan_sha256(set->plans[index], digest);
        if (status != LT48_OK) {
            lt48_m37_program_plan_set_close(set);
            return status;
        }
        lt48_sha256_update(&hash, &operation, 1u);
        lt48_sha256_update(&hash, digest, sizeof(digest));
    }
    lt48_sha256_final(&hash, set->sha256);
    *out_set = set;
    return LT48_OK;
}

void lt48_m37_program_plan_set_close(lt48_m37_program_plan_set *set) {
    if (set == NULL) return;
    for (size_t index = 0u; index < set->count; ++index)
        lt48_protocol_plan_close(set->plans[index]);
    free(set);
}

size_t lt48_m37_program_plan_set_count(
    const lt48_m37_program_plan_set *set
) {
    return set == NULL ? 0u : set->count;
}

lt48_status lt48_m37_program_plan_set_get(
    const lt48_m37_program_plan_set *set, size_t index,
    lt48_m37_operation *out_operation, const lt48_protocol_plan **out_plan
) {

    if (set == NULL || out_operation == NULL || out_plan == NULL)
        return LT48_INVALID_ARGUMENT;
    if (index >= set->count) return LT48_NOT_FOUND;
    *out_operation = index==0 ? LT48_M37_BLANK_CHECK :
        index==set->count-1 ? LT48_M37_VERIFY : LT48_M37_PROGRAM;
    *out_plan = set->plans[index];
    return LT48_OK;
}

lt48_status lt48_m37_program_plan_set_sha256(
    const lt48_m37_program_plan_set *set,
    unsigned char digest[LT48_SHA256_SIZE]
) {
    if (set == NULL || digest == NULL) return LT48_INVALID_ARGUMENT;
    memcpy(digest, set->sha256, LT48_SHA256_SIZE);
    return LT48_OK;
}

lt48_status lt48_protocol_plan_m37_recovery_state_internal(
    const lt48_protocol_plan *plan,
    const lt48_m37_resources *resources,
    size_t completed_transactions,
    lt48_status operation_status,
    lt48_m37_recovery_state *out_state
) {
    const lt48_m37_resource_audit *audit;
    if (plan == NULL || resources == NULL || out_state == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    audit = lt48_m37_resources_audit(resources);
    if (!plan->m37_read_sealed || audit == NULL ||
        completed_transactions > plan->transaction_count ||
        memcmp(plan->m37_resource_manifest_sha256,
               audit->resource_manifest_sha256,
               LT48_SHA256_SIZE) != 0) {
        return LT48_SAFETY_REJECTED;
    }
    if (operation_status == LT48_CANCELLED && completed_transactions == 0u) {
        *out_state = LT48_M37_RECOVERY_UNOPENED;
    } else if (completed_transactions < plan->m37_powered_first_transaction) {
        *out_state = LT48_M37_RECOVERY_INITIALIZED;
    } else {
        *out_state = LT48_M37_RECOVERY_POWERED;
    }
    return LT48_OK;
}

lt48_status lt48_m37_resources_build_recovery_plan(
    const lt48_m37_resources *resources,
    lt48_m37_recovery_state state,
    lt48_protocol_plan **out_plan
) {
    static const unsigned char settle[] = {0x4fu,0x64u,0};
    static const unsigned char fpga_header[] = {0x38u,0xaau,0x55u};
    static const unsigned char finalize[] = {0x49u,0xaau,0x55u};
    static const unsigned char error_result[] = {0x40u,2};
    static const unsigned char queue_reset[] = {0x2cu,0x08u,0,0};
    static const unsigned char timeout700[] = {0x21,0x10,0x0a,0xbc,2,0,0};
    static const unsigned char timeout500[] = {0x21,0x10,0x0a,0xf4,1,0,0};
    static const unsigned char timeout1000[] = {0x21,0x10,0x0a,0xe8,3,0,0};
    const lt48_m37_resource_audit *audit;
    const unsigned char *idle_fpga;
    size_t idle_fpga_size;
    lt48_protocol_plan *plan = NULL;
    lt48_applet_address *applet_addresses = NULL;
    size_t applet_count;
    uint32_t initial_voltage[4] = {0,0,0,0};
    size_t index;
    lt48_status status;
    if (resources == NULL || out_plan == NULL ||
        state < LT48_M37_RECOVERY_UNOPENED ||
        state > LT48_M37_RECOVERY_POWERED) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_plan = NULL;
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL) return LT48_SAFETY_REJECTED;
    int dataman_m0_recovery = audit->family_code == 0u &&
        audit->capacity_bytes == 16384u &&
        memcmp(audit->definition_sha256, dataman_m0_definition,
               sizeof(dataman_m0_definition)) == 0;
    status = lt48_protocol_plan_create(&plan);
    if (status != LT48_OK) return status;
    if (state == LT48_M37_RECOVERY_UNOPENED) {
        status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_RELEASE_INTERFACE, 0u
        );
        if (status != LT48_OK) goto fail;
        plan->recovery_sealed = 1;
        *out_plan = plan;
        return LT48_OK;
    }
    status = lt48_m37_resources_idle_fpga(
        resources, &idle_fpga, &idle_fpga_size
    );
    if (status != LT48_OK || idle_fpga_size != 2778u) {
        status = status == LT48_OK ? LT48_SAFETY_REJECTED : status;
        goto fail;
    }
    applet_count = lt48_m37_resources_applet_count(resources);
    applet_addresses = calloc(
        applet_count ? applet_count : 1u, sizeof(*applet_addresses)
    );
    if (applet_addresses == NULL) {
        status = LT48_NO_MEMORY;
        goto fail;
    }
    for (index = 0u; index < applet_count; ++index) {
        lt48_m37_applet_resource applet;
        status = lt48_m37_resources_get_applet(resources, index, &applet);
        if (status != LT48_OK) goto fail;
        applet_addresses[index].wave = applet.operation_id;
        applet_addresses[index].address = applet.load_address;
    }
    if (state == LT48_M37_RECOVERY_POWERED) {
        size_t first = plan->transaction_count;
        initial_voltage[0] = audit->nominal_vcc_centivolts;
        initial_voltage[3] = audit->nominal_vcc_centivolts;
        if(audit->complete_source_audit_verified && (audit->device_class==2003u||audit->device_class==2004u))
            status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
        if(status==LT48_OK)status = apply_snapshot_operation(
            plan, resources, 4u, 1u, 0u, initial_voltage, 0u,
            applet_addresses, applet_count
        );
        if (status == LT48_OK) status = flush_queue(plan);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_DEVICE_RESET, first
        );
        first = plan->transaction_count;
        if (status == LT48_OK && dataman_m0_recovery)
            status = command(plan, timeout700, sizeof(timeout700), 0);
        if (status == LT48_OK) status = write_block(
            plan, settle, sizeof(settle)
        );
        if (status == LT48_OK) status = flush_queue(plan);
        if (status == LT48_OK && dataman_m0_recovery)
            status = command(plan, timeout500, sizeof(timeout500), 0);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_SETTLE_DELAY, first
        );
        first = plan->transaction_count;
        if (status == LT48_OK) status = status_command(plan);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_SETTLE_STATUS, first
        );
        memset(initial_voltage, 0, sizeof(initial_voltage));
        first = plan->transaction_count;
        if (status == LT48_OK && dataman_m0_recovery)
            status = command(plan, timeout1000, sizeof(timeout1000), 0);
        if (status == LT48_OK)
            status = apply_snapshot_operation(
                plan, resources, 1u, 1u, 0u, initial_voltage, 0u,
                applet_addresses, applet_count
            );
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_POWER_OFF, first
        );
        first = plan->transaction_count;
        if (status == LT48_OK) status = flush_queue(plan);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_POWER_OFF_FLUSH, first
        );
    }
    {
        size_t first = plan->transaction_count;
        if (status == LT48_OK && dataman_m0_recovery &&
            state == LT48_M37_RECOVERY_POWERED)
            status = command(plan, timeout500, sizeof(timeout500), 0);
        if(status==LT48_OK && audit->complete_source_audit_verified && (audit->device_class==2003u||audit->device_class==2004u))
            status=lt48_protocol_plan_wave_watchdog(plan,5000u,0,0,0);
        if (status == LT48_OK) {
            for (index = 4u; index > 0u; --index) {
                unsigned char zero_rail[] = {
                    0x10u, (unsigned char)(index - 1u), 0u
                };
                status = write_block(plan, zero_rail, sizeof(zero_rail));
                if (status != LT48_OK) break;
            }
        }
        if (status == LT48_OK) status = flush_queue(plan);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_ZERO_ALL_RAILS, first
        );
    }
    {
        size_t first = plan->transaction_count;
        if (status == LT48_OK) status = write_block(
            plan, fpga_header, sizeof(fpga_header)
        );
        if (status == LT48_OK) status = flush_queue(plan);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_RESTORE_IDLE_HEADER, first
        );
    }
    {
        size_t first = plan->transaction_count;
        if (status == LT48_OK) status = write_block(
            plan, idle_fpga, idle_fpga_size
        );
        /* Original Read tail transfers 608/609 initialize the queue before
         * controller reset. Keep the established other-source profiles. */
        if (status == LT48_OK && dataman_m0_recovery)
            status = command(plan, queue_reset, sizeof(queue_reset), 1);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_RESTORE_IDLE_DATA, first
        );
    }
    {
        size_t first = plan->transaction_count;
        if (status == LT48_OK) status = write_block(
            plan, finalize, sizeof(finalize)
        );
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_FINALIZE_CONTROLLER, first
        );
    }
    {
        size_t first = plan->transaction_count;
        if (status == LT48_OK) status = flush_queue(plan);
        /* Original tail transfers 612/613 initialize it again before the
         * result indicator. This is distinct from flushing pending bytes. */
        if (status == LT48_OK && dataman_m0_recovery)
            status = command(plan, queue_reset, sizeof(queue_reset), 1);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_FINALIZE_FLUSH, first
        );
    }
    {
        size_t first = plan->transaction_count;
        if (status == LT48_OK) status = write_block(
            plan, error_result, sizeof(error_result)
        );
        if (status == LT48_OK) status = flush_queue(plan);
        if (status == LT48_OK) status = append_recovery_phase(
            plan, LT48_M37_CLEANUP_SET_RESULT, first
        );
    }
    if (status == LT48_OK) status = append_recovery_phase(
        plan, LT48_M37_CLEANUP_RELEASE_INTERFACE,
        plan->transaction_count
    );
    if (status != LT48_OK) goto fail;
    plan->recovery_sealed = 1;
    free(applet_addresses);
    *out_plan = plan;
    return LT48_OK;

fail:
    free(applet_addresses);
    lt48_protocol_plan_close(plan);
    return status;
}

/* Captured Dataman 13.10 Am27C128 ID mismatch cleanup, either before the
 * operator choice or after Abort. Wire timeout counts are retained exactly
 * as captured; this does not infer host timer units from their values. */
lt48_status lt48_m37_resources_build_id_mismatch_cleanup_internal(
    const lt48_m37_resources *resources, int post_abort,
    lt48_protocol_plan **out_plan
) {
    static const unsigned char definition[LT48_SHA256_SIZE] = {
        0x2e,0x62,0x61,0xc8,0x67,0xd6,0x85,0x77,
        0x86,0x93,0x3c,0x0d,0x09,0x3e,0x5b,0xd6,
        0x69,0xf7,0x03,0x55,0xcc,0x21,0x8f,0xf0,
        0x06,0xc8,0x8c,0x4f,0xb9,0x19,0x63,0x6b
    };
    static const unsigned char timeout700[] = {0x21,0x10,0x0a,0xbc,2,0,0};
    static const unsigned char timeout500[] = {0x21,0x10,0x0a,0xf4,1,0,0};
    static const unsigned char timeout1000[] = {0x21,0x10,0x0a,0xe8,3,0,0};
    static const unsigned char settle[] = {0x4f,0x64,0};
    static const unsigned char fpga[] = {0x38,0xaa,0x55};
    static const unsigned char init[] = {0x2c,8,0,0};
    static const unsigned char reset[] = {0x49,0xaa,0x55};
    const lt48_m37_resource_audit *audit;
    const unsigned char *idle;
    size_t idle_size, first;
    uint32_t voltage[4] = {0};
    lt48_protocol_plan *plan = NULL;
    lt48_status status;
    if (!resources || !out_plan || (post_abort != 0 && post_abort != 1))
        return LT48_INVALID_ARGUMENT;
    *out_plan = NULL;
    audit = lt48_m37_resources_audit(resources);
    if (!audit || audit->family_code != 0u ||
        memcmp(audit->definition_sha256, definition, sizeof(definition)) != 0)
        return LT48_SAFETY_REJECTED;
    status = lt48_m37_resources_idle_fpga(resources, &idle, &idle_size);
    if (status != LT48_OK) return status;
    if (idle_size != 2778u) return LT48_SAFETY_REJECTED;
    status = lt48_protocol_plan_create(&plan);
    if (status != LT48_OK) return status;
    first = 0u;
    if (!post_abort) {
        status = pair(plan, 1u, 0x20u);
        if (status == LT48_OK) status = pair(plan, 1u, 0x42u);
        if (status == LT48_OK) status = pair(plan, 2u, 0x84u);
        if (status == LT48_OK) status = flush_queue(plan);
    }
    if (status == LT48_OK) status = command(plan, timeout700, sizeof(timeout700), 0);
    if (status == LT48_OK) status = write_block(plan, settle, sizeof(settle));
    if (status == LT48_OK) status = flush_queue(plan);
    if (status == LT48_OK) status = command(plan, timeout500, sizeof(timeout500), 0);
    if (status == LT48_OK) status = append_recovery_phase(
        plan, LT48_M37_CLEANUP_SETTLE_DELAY, first);
    first = plan->transaction_count;
    if (status == LT48_OK) status = status_command(plan);
    if (status == LT48_OK) status = append_recovery_phase(
        plan, LT48_M37_CLEANUP_SETTLE_STATUS, first);
    first = plan->transaction_count;
    if (status == LT48_OK) status = command(plan, timeout1000, sizeof(timeout1000), 0);
    if (!post_abort) voltage[0] = audit->nominal_vcc_centivolts;
    if (status == LT48_OK) status = apply_snapshot_operation(
        plan, resources, 1u, 1u, 0u, voltage, 0u, NULL, 0u);
    if (status == LT48_OK) status = flush_queue(plan);
    if (status == LT48_OK) status = append_recovery_phase(
        plan, LT48_M37_CLEANUP_POWER_OFF, first);
    first = plan->transaction_count;
    if (status == LT48_OK) status = command(plan, timeout500, sizeof(timeout500), 0);
    if (status == LT48_OK) status = write_block(plan, fpga, sizeof(fpga));
    if (status == LT48_OK) status = flush_queue(plan);
    if (status == LT48_OK) status = append_recovery_phase(
        plan, LT48_M37_CLEANUP_RESTORE_IDLE_HEADER, first);
    first = plan->transaction_count;
    if (status == LT48_OK) status = write_block(plan, idle, idle_size);
    if (status == LT48_OK) status = append_recovery_phase(
        plan, LT48_M37_CLEANUP_RESTORE_IDLE_DATA, first);
    first = plan->transaction_count;
    if (status == LT48_OK) status = command(plan, init, sizeof(init), 1);
    if (status == LT48_OK) status = write_block(plan, reset, sizeof(reset));
    if (status == LT48_OK) status = flush_queue(plan);
    if (status == LT48_OK) status = append_recovery_phase(
        plan, LT48_M37_CLEANUP_FINALIZE_CONTROLLER, first);
    if (status != LT48_OK) {
        lt48_protocol_plan_close(plan);
        return status;
    }
    plan->dataman_id_cleanup_stage = 1;
    plan->recovery_sealed = 1;
    *out_plan = plan;
    return LT48_OK;
}

static lt48_status validate_response(
    const transaction_owned *transaction, const unsigned char *data
) {
    if (transaction->checks_dataman_m0_contacts) {
        if (transaction->length < 12u) return LT48_PROTOCOL_ERROR;
        return lt48_dataman_am27c128_contacts_match(data) ?
            LT48_OK : LT48_SAFETY_REJECTED;
    }

    if (!transaction->validates_controller_response) return LT48_OK;
    if (!transaction->length) return LT48_PROTOCOL_ERROR;
    if (transaction->captures_runtime_byte) {
        if (transaction->runtime_capture_length == 0u ||
            transaction->response_offset > transaction->length ||
            transaction->runtime_capture_length >
                transaction->length - transaction->response_offset ||
            transaction->runtime_pin_count >
                transaction->length - transaction->response_offset ||
            (transaction->request_opcode != 0x0eu &&
             (transaction->length < 2u ||
              ((uint16_t)data[0] | (uint16_t)data[1] << 8) == 0u))) {
            return LT48_PROTOCOL_ERROR;
        }
        return LT48_OK;
    }
    if (transaction->request_opcode == 0x0du) {
        if (transaction->length < 3u) return LT48_PROTOCOL_ERROR;
        if (((uint16_t)data[0] | (uint16_t)data[1] << 8) == 0u)
            return LT48_DEVICE_MISMATCH;
        return (data[2] & 0x08u) ? LT48_PROTOCOL_ERROR : LT48_OK;
    }
    if(transaction->reads_device_data && transaction->request_opcode==0x04u) {
        /* Original 4964c0: two status bytes followed by arbitrary device data.
         * Programming's 04 acknowledgement has a different payload contract. */
        if(transaction->length<3u)return LT48_PROTOCOL_ERROR;
        return ((uint16_t)data[0] | (uint16_t)data[1]<<8)?LT48_OK:LT48_DEVICE_MISMATCH;
    }
    if(transaction->completion_data_width) {
        /* Original 4972c0 returns transport status separately from data[2];
         * 4520c9 tests that completion byte. ACK alone is not verification. */
        if(transaction->length<3u || !(data[0]|data[1]))return LT48_PROTOCOL_ERROR;
        return data[2]?LT48_OK:LT48_DEVICE_MISMATCH;
    }
    if (data[0] == 0u) return LT48_DEVICE_MISMATCH;
    if (transaction->request_opcode == 0x04u &&
        (transaction->length < 3u || (data[2] & 1u) == 0u)) {
        return LT48_PROTOCOL_ERROR;
    }
    return LT48_OK;
}

static lt48_status execute_range(
    const lt48_protocol_plan *plan,
    size_t first_transaction,
    size_t transaction_count,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_m37_read_collector *collector,
    lt48_runtime_capture *captures,
    size_t capture_capacity,
    size_t *out_capture_count,
    size_t *out_completed_transactions
) {
    lt48_runtime_capture local_id_captures[2];
    size_t capture_count = 0u;
    size_t completed = 0u;
    size_t index;
    lt48_status status = LT48_OK;
    if (!plan || !transport || !transport->write || !transport->read ||
        !out_capture_count || !out_completed_transactions ||
        (capture_capacity && !captures)) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_capture_count = 0u;
    *out_completed_transactions = 0u;
    if (first_transaction > plan->transaction_count ||
        transaction_count > plan->transaction_count - first_transaction) {
        return LT48_INVALID_ARGUMENT;
    }
    /* A separate range cannot bypass the ID decision or reuse stale captures.
     * Recovery is executed through independently constructed recovery plans. */
    if (plan->dataman_id_required && first_transaction != 0u &&
        first_transaction + transaction_count > plan->dataman_id_boundary)
        return LT48_SAFETY_REJECTED;
    for (index = 0u; index < plan->transaction_count; ++index)
        if (plan->transactions[index].checks_dataman_m0_contacts &&
            first_transaction != 0u &&
            first_transaction + transaction_count > index)
            return LT48_SAFETY_REJECTED;
    if (plan->dataman_id_required && captures == NULL && capture_capacity == 0u) {
        captures = local_id_captures;
        capture_capacity = 2u;
    }
    for (index = first_transaction;
         index < first_transaction + transaction_count; ++index) {
        if (plan->transactions[index].captures_runtime_byte) ++capture_count;
    }
    if (capture_count > capture_capacity) {
        *out_capture_count = capture_count;
        return LT48_BUFFER_TOO_SMALL;
    }
    capture_count = 0u;
    if (transport->progress != NULL)
        transport->progress(
            transport_context, transport->progress_stage,
            0u, transaction_count, 0u, 0u, 0u
        );
    for (index = first_transaction;
         index < first_transaction + transaction_count; ++index) {
        const transaction_owned *transaction = &plan->transactions[index];
        size_t transferred = 0u;
        if (plan->dataman_id_required && index == plan->dataman_id_boundary) {
            if (capture_count != 2u || captures[0].buffer != 3u ||
                captures[1].buffer != 4u || captures[0].length != 1u ||
                captures[1].length != 1u) {
                status = LT48_PROTOCOL_ERROR;
                break;
            }
            if (plan->dataman_id_match_required &&
                !lt48_dataman_selector_zero_id_matches(
                    plan->dataman_manufacturer_id, plan->dataman_device_id,
                    captures[0].values[0], captures[1].values[0])) {
                status = LT48_DEVICE_MISMATCH;
                break;
            }
        }
        if (transport->cancelled && transport->cancelled(transport_context)) {
            status = LT48_CANCELLED;
            break;
        }
        if (transaction->direction == LT48_TRANSFER_OUT) {
            status = transport->write(
                transport_context, transaction->endpoint, transaction->data,
                transaction->length, &transferred
            );
        } else if (transaction->direction == LT48_TRANSFER_IN) {
            unsigned char *data = malloc(transaction->length ?
                                         transaction->length : 1u);
            if (!data) {
                status = LT48_NO_MEMORY;
                break;
            }
            memset(data, 0, transaction->length);
            status = transport->read(
                transport_context, transaction->endpoint, data,
                transaction->length, &transferred
            );
            if (status == LT48_OK) {
                if (transferred != transaction->length) {
                    status = LT48_PROTOCOL_ERROR;
                } else {
                    status = validate_response(transaction, data);
                }
            }
            if(status==LT48_DEVICE_MISMATCH && transaction->completion_data_width) {
                /* Original 4520d4..4520f5 consumes the actual low/high bytes
                 * with two further backend+8 (40 00) calls before shutdown. */
                unsigned char request[LT48_MAX_PACKET_SIZE]={0x40,0};
                for(unsigned n=0;n<transaction->completion_data_width;n++) {
                    size_t done=0;
                    lt48_status drained=transport->write(transport_context,LT48_EP_COMMAND_OUT,
                        request,plan->packet_size,&done);
                    if(drained!=LT48_OK || done!=plan->packet_size){status=LT48_PROTOCOL_ERROR;break;}
                    drained=transport->read(transport_context,LT48_EP_IN,data,transaction->length,&done);
                    if(drained!=LT48_OK || done!=transaction->length || !(data[0]|data[1])) {
                        status=LT48_PROTOCOL_ERROR;break;
                    }
                }
            }
            if (status == LT48_OK && transaction->captures_runtime_byte) {
                if (capture_count >= capture_capacity) {
                    status = LT48_BUFFER_TOO_SMALL;
                } else {
                    captures[capture_count].buffer = transaction->runtime_buffer;
                    captures[capture_count].value =
                        data[transaction->response_offset];
                    captures[capture_count].length =
                        transaction->runtime_capture_length;
                    if (transaction->runtime_pin_count) {
                        memset(captures[capture_count].values, 0,
                               transaction->runtime_capture_length);
                        for (size_t pin = 0u; pin < transaction->runtime_pin_count; ++pin)
                            if (data[transaction->response_offset + pin])
                                captures[capture_count].values[pin / 8u] |=
                                    (unsigned char)(1u << (pin & 7u));
                        captures[capture_count].value = captures[capture_count].values[0];
                    } else {
                        memcpy(captures[capture_count].values,
                               data + transaction->response_offset,
                               transaction->runtime_capture_length);
                    }
                    ++capture_count;
                }
            }
            if (status == LT48_OK && transaction->reads_device_data &&
                collector != NULL) {
                status = lt48_m37_read_collector_accept(
                    collector, index, data, transaction->length
                );
            }
            free(data);
        } else {
            status = LT48_PROTOCOL_ERROR;
        }
        if (status != LT48_OK) break;
        if (transferred != transaction->length) {
            status = LT48_PROTOCOL_ERROR;
            break;
        }
        ++completed;
        if (transport->progress != NULL)
            transport->progress(
                transport_context, transport->progress_stage,
                completed, transaction_count, 0u, 0u, 0u
            );
    }
    *out_capture_count = capture_count;
    *out_completed_transactions = completed;
    return status;
}

/* Preserve the existing Linux best-effort recovery rule: attempt later
 * cleanup phases after a failure and suppress operator continuation unless
 * every phase succeeds. This is not a claim about untraced Windows errors.
 * Interface release belongs to the outer operation after operator choice. */
lt48_status lt48_protocol_plan_execute_id_cleanup_internal(
    const lt48_protocol_plan *plan, const lt48_protocol_transport *transport,
    void *transport_context, lt48_m37_recovery_execution_result *out_result
) {
    lt48_protocol_transport recovery_transport;
    size_t expected_first = 0u;
    if (!plan || !transport || !transport->read || !transport->write || !out_result)
        return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    if (!plan->dataman_id_cleanup_stage || !plan->recovery_sealed ||
        plan->recovery_phase_count != 6u)
        return LT48_SAFETY_REJECTED;
    for (size_t i = 0u; i < plan->recovery_phase_count; ++i) {
        const recovery_phase_owned *phase = &plan->recovery_phases[i];
        if (phase->first_transaction != expected_first ||
            phase->transaction_count > plan->transaction_count - expected_first)
            return LT48_SAFETY_REJECTED;
        expected_first += phase->transaction_count;
    }
    if (expected_first != plan->transaction_count) return LT48_SAFETY_REJECTED;
    recovery_transport = *transport;
    recovery_transport.cancelled = NULL;
    recovery_transport.progress_stage = LT48_EXECUTION_READ_RECOVERY;
    out_result->phase_count = plan->recovery_phase_count;
    for (size_t i = 0u; i < plan->recovery_phase_count; ++i) {
        const recovery_phase_owned *phase = &plan->recovery_phases[i];
        size_t captures = 0u, completed = 0u;
        lt48_status status = execute_range(
            plan, phase->first_transaction, phase->transaction_count,
            &recovery_transport, transport_context, NULL, NULL, 0u,
            &captures, &completed);
        out_result->completed_transactions += completed;
        if (status != LT48_OK) {
            if (out_result->failure_count == 0u)
                out_result->first_failure_status = status;
            ++out_result->failure_count;
        }
    }
    out_result->recovery_status = out_result->failure_count ?
        LT48_CLEANUP_FAILED : LT48_OK;
    return out_result->recovery_status;
}

lt48_status lt48_protocol_plan_execute_range(
    const lt48_protocol_plan *plan,
    size_t first_transaction,
    size_t transaction_count,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_runtime_capture *captures,
    size_t capture_capacity,
    size_t *out_capture_count,
    size_t *out_completed_transactions
) {
    return execute_range(
        plan, first_transaction, transaction_count, transport,
        transport_context, NULL, captures, capture_capacity,
        out_capture_count, out_completed_transactions
    );
}

lt48_status lt48_protocol_plan_execute_range_collect(
    const lt48_protocol_plan *plan,
    size_t first_transaction,
    size_t transaction_count,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_m37_read_collector *collector,
    lt48_runtime_capture *captures,
    size_t capture_capacity,
    size_t *out_capture_count,
    size_t *out_completed_transactions
) {
    if (collector == NULL) return LT48_INVALID_ARGUMENT;
    return execute_range(
        plan, first_transaction, transaction_count, transport,
        transport_context, collector, captures, capture_capacity,
        out_capture_count, out_completed_transactions
    );
}

lt48_status lt48_protocol_plan_wave_watchdog(
    lt48_protocol_plan *plan, uint32_t milliseconds,
    int cached_valid, uint32_t cached_milliseconds, int interpreter_active
) {
    lt48_status status;
    unsigned char payload[7] = {0x21,0x10,10,0,0,0,0};
    if (!plan || (cached_valid != 0 && cached_valid != 1) ||
        (interpreter_active != 0 && interpreter_active != 1))
        return LT48_INVALID_ARGUMENT;
    if (plan->recovery_sealed || plan->m37_read_sealed) return LT48_SAFETY_REJECTED;
    if (!cached_valid || cached_milliseconds != milliseconds) {
        uint32_t count = milliseconds < 10 ? 1 : milliseconds / 10;
        for (unsigned i = 0; i < 4; ++i) payload[i + 3] = (unsigned char)(count >> (i * 8));
        status = flush_queue(plan);
        /* Original49df20 calls backend+40 only: no backend+50 reply read. */
        if (status == LT48_OK) status = command(plan, payload, sizeof(payload), 0);
        return status;
    }
    /* This is a queue control pair, not controller bytes written via kind1. */
    status = pair(plan, 8, 0);
    if (status == LT48_OK && !interpreter_active) status = flush_queue(plan);
    return status;
}

lt48_status lt48_protocol_plan_sample_pin_internal(lt48_protocol_plan *plan,uint8_t pin,uint8_t buffer) {
    unsigned char request[]={4,pin};
    if(!plan || buffer>=70)return LT48_INVALID_ARGUMENT;
    lt48_status status=flush_queue(plan);
    if(status==LT48_OK)status=command(plan,request,sizeof(request),1);
    if(status==LT48_OK){transaction_owned *c=&plan->transactions[plan->transaction_count-1];
        c->captures_runtime_byte=1;c->runtime_buffer=buffer;c->runtime_capture_length=1;c->response_offset=2;}
    return status;
}

lt48_status lt48_protocol_plan_pair_internal(lt48_protocol_plan *plan,uint8_t kind,uint8_t value) {
    if(!plan||(kind!=1&&kind!=2))return LT48_INVALID_ARGUMENT;
    return pair(plan,kind,value);
}
