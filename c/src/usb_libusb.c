#include <labtool48/labtool48.h>

#include "internal.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

enum {
    LT48_PACKET_SIZE = 512,
    LT48_EP_COMMAND_OUT = 0x02,
    LT48_EP_IN = 0x88
};

static struct timespec owner_now(void) {
    struct timespec value = {0};
    (void)timespec_get(&value, TIME_UTC);
    return value;
}

static double owner_elapsed_ms(
    struct timespec start, struct timespec finish
) {
    time_t seconds = finish.tv_sec - start.tv_sec;
    long nanoseconds = finish.tv_nsec - start.tv_nsec;
    if (nanoseconds < 0) {
        --seconds;
        nanoseconds += 1000000000L;
    }
    if (seconds < 0) return 0.0;
    return (double)seconds * 1000.0 +
           (double)nanoseconds / 1000000.0;
}

lt48_status from_libusb_error_internal(int result) {
    if (result == LIBUSB_ERROR_TIMEOUT) {
        return LT48_TIMEOUT;
    }
    if (result == LIBUSB_ERROR_ACCESS) {
        return LT48_PERMISSION_DENIED;
    }
    if (result == LIBUSB_ERROR_NO_MEM) {
        return LT48_NO_MEMORY;
    }
    if (result == LIBUSB_ERROR_BUSY) {
        return LT48_BUSY;
    }
    if (result == LIBUSB_ERROR_NO_DEVICE || result == LIBUSB_ERROR_NOT_FOUND) {
        return LT48_NOT_FOUND;
    }
    return LT48_USB_ERROR;
}

static lt48_status libusb_bulk_backend(
    void *context, uint8_t endpoint, unsigned char *data,
    size_t length, unsigned timeout_ms, size_t *out_transferred
) {
    lt48_programmer *programmer = context;
    int transferred = 0;
    int result;
    if (programmer == NULL || programmer->handle == NULL ||
        out_transferred == NULL || length > INT_MAX) {
        return LT48_INVALID_ARGUMENT;
    }
    result = libusb_bulk_transfer(
        programmer->handle, endpoint, data, (int)length,
        &transferred, timeout_ms
    );
    if (result != LIBUSB_SUCCESS) return from_libusb_error_internal(result);
    if (transferred < 0) return LT48_PROTOCOL_ERROR;
    *out_transferred = (size_t)transferred;
    return LT48_OK;
}

static lt48_status libusb_release_backend(void *context) {
    lt48_programmer *programmer = context;
    int result;
    if (programmer == NULL || programmer->handle == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    result = libusb_release_interface(programmer->handle, 0);
    return result == LIBUSB_SUCCESS ? LT48_OK : from_libusb_error_internal(result);
}

static lt48_status release_claim(lt48_programmer *programmer) {
    lt48_status status;
    if (programmer == NULL) return LT48_INVALID_ARGUMENT;
    if (!programmer->interface_claimed) return LT48_OK;
    if (programmer->release == NULL) return LT48_SAFETY_REJECTED;
    status = programmer->release(programmer->backend_context);
    if (status == LT48_OK) programmer->interface_claimed = 0;
    return status;
}

lt48_status lt48_context_create(lt48_context **out_context) {
    lt48_context *context;
    int result;

    if (out_context == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_context = NULL;
    context = calloc(1, sizeof(*context));
    if (context == NULL) {
        return LT48_NO_MEMORY;
    }
    result = libusb_init(&context->usb);
    if (result != LIBUSB_SUCCESS) {
        free(context);
        return from_libusb_error_internal(result);
    }
    *out_context = context;
    return LT48_OK;
}

void lt48_context_destroy(lt48_context *context) {
    if (context == NULL) {
        return;
    }
    libusb_exit(context->usb);
    free(context);
}

lt48_status lt48_programmer_list(
    lt48_context *context,
    lt48_programmer_info *infos,
    size_t capacity,
    size_t *out_count
) {
    libusb_device **devices = NULL;
    ssize_t device_count;
    size_t found = 0;

    if (context == NULL || out_count == NULL || (capacity != 0 && infos == NULL)) {
        return LT48_INVALID_ARGUMENT;
    }
    device_count = libusb_get_device_list(context->usb, &devices);
    if (device_count < 0) {
        return from_libusb_error_internal((int)device_count);
    }
    for (ssize_t index = 0; index < device_count; ++index) {
        struct libusb_device_descriptor descriptor;
        int result = libusb_get_device_descriptor(devices[index], &descriptor);
        if (result != LIBUSB_SUCCESS ||
            !lt48_is_supported_usb_id(descriptor.idVendor, descriptor.idProduct)) {
            continue;
        }
        if (found < capacity) {
            infos[found].vendor_id = descriptor.idVendor;
            infos[found].product_id = descriptor.idProduct;
            infos[found].usb_version_bcd = descriptor.bcdUSB;
            infos[found].bus_number = libusb_get_bus_number(devices[index]);
            infos[found].device_address = libusb_get_device_address(devices[index]);
        }
        ++found;
    }
    libusb_free_device_list(devices, 1);
    *out_count = found;
    return found > capacity ? LT48_BUFFER_TOO_SMALL : LT48_OK;
}

lt48_status lt48_programmer_open(
    lt48_context *context,
    uint8_t bus_number,
    uint8_t device_address,
    lt48_programmer **out_programmer
) {
    libusb_device **devices = NULL;
    libusb_device *selected = NULL;
    libusb_device_handle *handle = NULL;
    lt48_programmer *programmer = NULL;
    uint16_t selected_product_id = 0u;
    ssize_t device_count;
    int result;

    if (context == NULL || out_programmer == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_programmer = NULL;
    device_count = libusb_get_device_list(context->usb, &devices);
    if (device_count < 0) {
        return from_libusb_error_internal((int)device_count);
    }
    for (ssize_t index = 0; index < device_count; ++index) {
        struct libusb_device_descriptor descriptor;
        result = libusb_get_device_descriptor(devices[index], &descriptor);
        if (result == LIBUSB_SUCCESS &&
            libusb_get_bus_number(devices[index]) == bus_number &&
            libusb_get_device_address(devices[index]) == device_address &&
            lt48_is_supported_usb_id(descriptor.idVendor, descriptor.idProduct)) {
            selected = devices[index];
            selected_product_id = descriptor.idProduct;
            break;
        }
    }
    if (selected == NULL) {
        libusb_free_device_list(devices, 1);
        return LT48_NOT_FOUND;
    }
    result = libusb_open(selected, &handle);
    libusb_free_device_list(devices, 1);
    if (result != LIBUSB_SUCCESS) {
        return from_libusb_error_internal(result);
    }
    result = libusb_set_auto_detach_kernel_driver(handle, 1);
    if (result != LIBUSB_SUCCESS && result != LIBUSB_ERROR_NOT_SUPPORTED) {
        libusb_close(handle);
        return from_libusb_error_internal(result);
    }
    result = libusb_claim_interface(handle, 0);
    if (result != LIBUSB_SUCCESS) {
        libusb_close(handle);
        return from_libusb_error_internal(result);
    }
    result = libusb_set_interface_alt_setting(handle, 0, 1);
    if (result != LIBUSB_SUCCESS) {
        libusb_release_interface(handle, 0);
        libusb_close(handle);
        return from_libusb_error_internal(result);
    }
    programmer = calloc(1, sizeof(*programmer));
    if (programmer == NULL) {
        libusb_release_interface(handle, 0);
        libusb_close(handle);
        return LT48_NO_MEMORY;
    }
    programmer->handle = handle;
    programmer->product_id = selected_product_id;
    programmer->bus_number = bus_number;
    programmer->device_address = device_address;
    programmer->interface_claimed = 1;
    programmer->backend_context = programmer;
    programmer->bulk = libusb_bulk_backend;
    programmer->release = libusb_release_backend;
    atomic_init(&programmer->progress_revision, 0u);
    atomic_init(&programmer->progress_stage, LT48_EXECUTION_IDLE);
    atomic_init(&programmer->progress_completed, 0u);
    atomic_init(&programmer->progress_total, 0u);
    atomic_init(&programmer->progress_cleanup_phase, 0u);
    atomic_init(&programmer->progress_cleanup_index, 0u);
    atomic_init(&programmer->progress_cleanup_count, 0u);
    *out_programmer = programmer;
    return LT48_OK;
}

void lt48_programmer_close(lt48_programmer *programmer) {
    if (programmer == NULL) {
        return;
    }
    (void)release_claim(programmer);
    if (programmer->handle != NULL) libusb_close(programmer->handle);
    free(programmer);
}

static lt48_status exchange_packet(
    lt48_programmer *programmer,
    unsigned char packet[LT48_PACKET_SIZE],
    size_t packet_size,
    unsigned timeout_ms
) {
    size_t transferred = 0u;
    lt48_status status;
    if (programmer == NULL || programmer->bulk == NULL ||
        (packet_size != 64u && packet_size != LT48_PACKET_SIZE)) {
        return LT48_INVALID_ARGUMENT;
    }
    status = programmer->bulk(
        programmer->backend_context, LT48_EP_COMMAND_OUT, packet,
        packet_size, timeout_ms, &transferred
    );
    if (status != LT48_OK) return status;
    if (transferred != packet_size) {
        return LT48_PROTOCOL_ERROR;
    }
    memset(packet, 0, packet_size);
    status = programmer->bulk(
        programmer->backend_context, LT48_EP_IN, packet,
        packet_size, timeout_ms, &transferred
    );
    if (status != LT48_OK) return status;
    if (transferred != packet_size) {
        return LT48_PROTOCOL_ERROR;
    }
    return LT48_OK;
}

static lt48_status native_write(
    void *context, uint8_t endpoint, const unsigned char *data,
    size_t length, size_t *out_transferred
) {
    lt48_programmer *programmer = context;
    if (programmer == NULL || programmer->bulk == NULL ||
        !programmer->execution_active) return LT48_SAFETY_REJECTED;
    return programmer->bulk(
        programmer->backend_context, endpoint, (unsigned char *)data,
        length, programmer->execution_timeout_ms, out_transferred
    );
}

static lt48_status native_read(
    void *context, uint8_t endpoint, unsigned char *data,
    size_t length, size_t *out_transferred
) {
    lt48_programmer *programmer = context;
    if (programmer == NULL || programmer->bulk == NULL ||
        !programmer->execution_active) return LT48_SAFETY_REJECTED;
    return programmer->bulk(
        programmer->backend_context, endpoint, data, length,
        programmer->execution_timeout_ms, out_transferred
    );
}

static lt48_status native_release(void *context) {
    return release_claim(context);
}

static int native_cancelled(void *context) {
    lt48_programmer *programmer = context;
    return programmer != NULL && programmer->cancel_check != NULL &&
           programmer->cancel_check(programmer->cancel_context);
}

void lt48_programmer_progress_publish_internal(
    void *context, lt48_execution_stage stage,
    size_t completed_transactions, size_t total_transactions,
    unsigned cleanup_phase, size_t cleanup_phase_index,
    size_t cleanup_phase_count
) {
    lt48_programmer *programmer = context;
    if (programmer == NULL) return;
    (void)atomic_fetch_add_explicit(
        &programmer->progress_revision, 1u, memory_order_acq_rel);
    atomic_store_explicit(&programmer->progress_completed,
                          completed_transactions, memory_order_relaxed);
    atomic_store_explicit(&programmer->progress_total,
                          total_transactions, memory_order_relaxed);
    if (stage != LT48_EXECUTION_READ_RECOVERY || cleanup_phase != 0u) {
        atomic_store_explicit(&programmer->progress_cleanup_phase,
                              cleanup_phase, memory_order_relaxed);
        atomic_store_explicit(&programmer->progress_cleanup_index,
                              cleanup_phase_index, memory_order_relaxed);
        atomic_store_explicit(&programmer->progress_cleanup_count,
                              cleanup_phase_count, memory_order_relaxed);
    }
    atomic_store_explicit(&programmer->progress_stage, stage,
                          memory_order_relaxed);
    (void)atomic_fetch_add_explicit(
        &programmer->progress_revision, 1u, memory_order_release);
}

lt48_status lt48_programmer_get_progress(
    const lt48_programmer *programmer, lt48_execution_progress *out_progress
) {
    uint_fast64_t before;
    uint_fast64_t after = 0u;
    lt48_execution_progress progress;
    if (programmer == NULL || out_progress == NULL)
        return LT48_INVALID_ARGUMENT;
    /* Never spin on the UI thread if the publisher is descheduled. */
    for (unsigned attempt = 0; attempt < 16u; ++attempt) {
        before = atomic_load_explicit(
            &programmer->progress_revision, memory_order_acquire);
        if ((before & 1u) != 0u) continue;
        progress.stage = (lt48_execution_stage)atomic_load_explicit(
            &programmer->progress_stage, memory_order_relaxed);
        progress.completed_transactions = atomic_load_explicit(
            &programmer->progress_completed, memory_order_relaxed);
        progress.total_transactions = atomic_load_explicit(
            &programmer->progress_total, memory_order_relaxed);
        progress.cleanup_phase = atomic_load_explicit(
            &programmer->progress_cleanup_phase, memory_order_relaxed);
        progress.cleanup_phase_index = atomic_load_explicit(
            &programmer->progress_cleanup_index, memory_order_relaxed);
        progress.cleanup_phase_count = atomic_load_explicit(
            &programmer->progress_cleanup_count, memory_order_relaxed);
        after = atomic_load_explicit(
            &programmer->progress_revision, memory_order_acquire);
        if (before == after && (after & 1u) == 0u) {
            *out_progress = progress;
            return LT48_OK;
        }
    }
    return LT48_BUSY;
}

lt48_status lt48_programmer_set_cancel_check(
    lt48_programmer *programmer, lt48_cancel_check_fn check, void *context
) {
    if (programmer == NULL || (check == NULL && context != NULL)) {
        return LT48_INVALID_ARGUMENT;
    }
    if (programmer->execution_active || programmer->m37_read_armed ||
        programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed) {
        return LT48_BUSY;
    }
    programmer->cancel_check = check;
    programmer->cancel_context = context;
    return LT48_OK;
}

lt48_status lt48_programmer_set_m37_id_decision(
    lt48_programmer *programmer, lt48_m37_id_decision_fn decide, void *context
) {
    if (!programmer || (!decide && context)) return LT48_INVALID_ARGUMENT;
    if (programmer->execution_active || programmer->m37_read_armed ||
        programmer->m37_operation_armed || programmer->auto_id_eprom_armed)
        return LT48_BUSY;
    programmer->id_decide = decide;
    programmer->id_decision_context = context;
    return LT48_OK;
}

lt48_status lt48_programmer_arm_m37_read(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    uint32_t authorization
) {
    const lt48_m37_resource_audit *audit;
    if (programmer == NULL || resources == NULL) return LT48_INVALID_ARGUMENT;
    if (authorization != LT48_M37_READ_HARDWARE_AUTHORIZATION ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        programmer->release == NULL) return LT48_SAFETY_REJECTED;
    if (programmer->execution_active || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed)
        return LT48_BUSY;
    if (programmer->product_id == LT48_USB_PRODUCT_ID_COLD &&
        !programmer->transport_ready) {
        return LT48_FIRMWARE_REQUIRED;
    }
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL || !audit->exact_engine_envelope ||
        !audit->read_behavior_approved) {
        return LT48_SAFETY_REJECTED;
    }
    memcpy(programmer->armed_manifest_sha256,
           audit->resource_manifest_sha256, LT48_SHA256_SIZE);
    programmer->m37_read_armed = 1;
    return LT48_OK;
}

lt48_status lt48_programmer_arm_m37_operation(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *plan,
    lt48_m37_operation operation,
    uint32_t authorization
) {
    const lt48_m37_resource_audit *audit;
    lt48_status status;
    if (programmer == NULL || resources == NULL || plan == NULL)
        return LT48_INVALID_ARGUMENT;
    if (authorization != LT48_M37_OPERATION_HARDWARE_AUTHORIZATION ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        programmer->release == NULL) return LT48_SAFETY_REJECTED;
    if (operation != LT48_M37_BLANK_CHECK &&
        operation != LT48_M37_VERIFY && operation != LT48_M37_PROGRAM &&
        operation != LT48_M37_COMPARE && operation != LT48_M37_ELECTRICAL_ERASE) return LT48_SAFETY_REJECTED;
    if (programmer->execution_active || programmer->m37_read_armed ||
        programmer->m37_operation_armed || programmer->auto_id_eprom_armed)
        return LT48_BUSY;
    if (!programmer->transport_ready)
        return programmer->product_id == LT48_USB_PRODUCT_ID_COLD ?
               LT48_FIRMWARE_REQUIRED : LT48_SAFETY_REJECTED;
    if (lt48_protocol_plan_packet_size_internal(plan) != 512u)
        return LT48_UNSUPPORTED;
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL || !audit->exact_engine_envelope)
        return LT48_SAFETY_REJECTED;
    status = lt48_protocol_plan_sha256(plan, programmer->armed_plan_sha256);
    if (status != LT48_OK) return status;
    memcpy(programmer->armed_manifest_sha256,
           audit->resource_manifest_sha256, LT48_SHA256_SIZE);
    programmer->armed_m37_operation = operation;
    programmer->m37_operation_armed = 1;
    return LT48_OK;
}

lt48_status lt48_programmer_arm_auto_id_eprom(
    lt48_programmer *programmer,
    const lt48_auto_id_eprom_preparation *preparation,
    uint32_t authorization
) {
    lt48_auto_id_eprom_preparation rebuilt;
    lt48_status status;
    if (programmer == NULL || preparation == NULL)
        return LT48_INVALID_ARGUMENT;
    if (authorization != LT48_AUTO_ID_EPROM_HARDWARE_AUTHORIZATION ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        programmer->release == NULL)
        return LT48_SAFETY_REJECTED;
    if (programmer->execution_active || programmer->m37_read_armed ||
        programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed)
        return LT48_BUSY;
    if (!programmer->transport_ready)
        return programmer->product_id == LT48_USB_PRODUCT_ID_COLD ?
               LT48_FIRMWARE_REQUIRED : LT48_SAFETY_REJECTED;
    if (preparation->bus_number != programmer->bus_number ||
        preparation->device_address != programmer->device_address)
        return LT48_SAFETY_REJECTED;
    status = lt48_auto_id_eprom_prepare(
        preparation->bus_number, preparation->device_address,
        preparation->acquire_signature,
        preparation->plan.has_explicit_pin_hint,
        preparation->plan.explicit_pin_hint, &rebuilt
    );
    if (status != LT48_OK) return status;
    if (memcmp(&rebuilt.plan, &preparation->plan, sizeof(rebuilt.plan)) != 0 ||
        memcmp(rebuilt.confirmation_sha256, preparation->confirmation_sha256,
               LT48_SHA256_SIZE) != 0)
        return LT48_SAFETY_REJECTED;
    memcpy(programmer->armed_auto_id_confirmation_sha256,
           preparation->confirmation_sha256, LT48_SHA256_SIZE);
    programmer->auto_id_eprom_armed = 1;
    return LT48_OK;
}

lt48_status lt48_programmer_execute_auto_id_eprom(
    lt48_programmer *programmer,
    const lt48_auto_id_eprom_preparation *preparation,
    unsigned timeout_ms,
    lt48_auto_id_eprom_execution_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, NULL,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_AUTO_ID_OPERATION
    };
    lt48_auto_id_eprom_preparation rebuilt;
    lt48_status status;
    lt48_status release_status;
    struct timespec release_started;
    struct timespec release_finished;
    if (programmer == NULL || preparation == NULL || out_result == NULL ||
        timeout_ms == 0u)
        return LT48_INVALID_ARGUMENT;
    if (programmer->execution_active) return LT48_BUSY;
    status = lt48_auto_id_eprom_prepare(
        preparation->bus_number, preparation->device_address,
        preparation->acquire_signature,
        preparation->plan.has_explicit_pin_hint,
        preparation->plan.explicit_pin_hint, &rebuilt
    );
    if (!programmer->interface_claimed ||
        !programmer->auto_id_eprom_armed || status != LT48_OK ||
        memcmp(&rebuilt.plan, &preparation->plan, sizeof(rebuilt.plan)) != 0 ||
        memcmp(rebuilt.confirmation_sha256, preparation->confirmation_sha256,
               LT48_SHA256_SIZE) != 0 ||
        memcmp(programmer->armed_auto_id_confirmation_sha256,
               preparation->confirmation_sha256, LT48_SHA256_SIZE) != 0)
        return status == LT48_OK ? LT48_SAFETY_REJECTED : status;
    programmer->auto_id_eprom_armed = 0;
    memset(programmer->armed_auto_id_confirmation_sha256, 0,
           sizeof(programmer->armed_auto_id_confirmation_sha256));
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
                    programmer, LT48_EXECUTION_AUTO_ID_OPERATION, 0u, 0u,
                    0u, 0u, 0u);
    status = lt48_auto_id_eprom_execute(
        &transport, programmer, preparation->acquire_signature,
        preparation->plan.has_explicit_pin_hint,
        preparation->plan.explicit_pin_hint,
        LT48_AUTO_ID_EPROM_HARDWARE_AUTHORIZATION, out_result
    );
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    out_result->interface_release_attempted = 1;
    lt48_programmer_progress_publish_internal(
                    programmer, LT48_EXECUTION_INTERFACE_RELEASE,
                    out_result->command_transfer_count,
                    out_result->command_transfer_count, 1u, 1u, 1u);
    release_started = owner_now();
    release_status = release_claim(programmer);
    release_finished = owner_now();
    out_result->interface_release_duration_ms = owner_elapsed_ms(
        release_started, release_finished);
    out_result->total_duration_ms +=
        out_result->interface_release_duration_ms;
    out_result->interface_release_status = release_status;
    out_result->interface_released = release_status == LT48_OK;
    if (release_status == LT48_OK) programmer->transport_ready = 0;
    if (release_status != LT48_OK) status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
                    programmer, LT48_EXECUTION_COMPLETE,
                    out_result->command_transfer_count,
                    out_result->command_transfer_count, 0u, 0u, 0u);
    return status;
}

lt48_status lt48_programmer_execute_m37_read(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *read_plan,
    lt48_m37_read_collector *collector,
    unsigned timeout_ms,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_snapshot_read_execution_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    const lt48_m37_resource_audit *audit;
    lt48_cleanup_action release_interface = {
        "release-interface", native_release, NULL
    };
    lt48_status status;
    if (programmer == NULL || resources == NULL || read_plan == NULL ||
        collector == NULL || finalization_results == NULL ||
        out_result == NULL || timeout_ms == 0u) {
        return LT48_INVALID_ARGUMENT;
    }
    if (programmer->execution_active) return LT48_BUSY;
    audit = lt48_m37_resources_audit(resources);
    if (!programmer->interface_claimed || !programmer->m37_read_armed ||
        audit == NULL || memcmp(programmer->armed_manifest_sha256,
                                audit->resource_manifest_sha256,
                                LT48_SHA256_SIZE) != 0) {
        return LT48_SAFETY_REJECTED;
    }
    if (lt48_protocol_plan_packet_size_internal(read_plan) != 512u)
        return LT48_UNSUPPORTED;
    programmer->m37_read_armed = 0;
    memset(programmer->armed_manifest_sha256, 0,
           sizeof(programmer->armed_manifest_sha256));
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
                    programmer, LT48_EXECUTION_READ_OPERATION, 0u,
                    lt48_protocol_plan_transaction_count(read_plan),
                    0u, 0u, 0u);
    release_interface.context = programmer;
    memset(out_result, 0, sizeof(*out_result));
    if (programmer->id_decide && audit->read_original_replay_verified) {
        status = lt48_m37_read_id_owned_internal(
            resources, read_plan, &transport, programmer,
            programmer->id_decide, programmer->id_decision_context, collector,
            &release_interface, finalization_results,
            finalization_result_capacity, out_result);
    } else {
        status = lt48_m37_snapshot_read_execute_injected(
            resources, read_plan, &transport, programmer, collector,
            &release_interface, finalization_results,
            finalization_result_capacity, out_result);
    }
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    if (programmer->interface_claimed &&
        out_result->finalization_phase_count == 0u) {
        lt48_status release_status = release_claim(programmer);
        out_result->finalization_phase_count = 1u;
        out_result->finalization_status = release_status == LT48_OK ?
                                          LT48_OK : LT48_CLEANUP_FAILED;
        out_result->finalization_failure_count =
            release_status == LT48_OK ? 0u : 1u;
        if (finalization_result_capacity != 0u) {
            finalization_results[0].name = "release-interface";
            finalization_results[0].status = release_status;
            finalization_results[0].started_offset_ms =
                out_result->operation_duration_ms;
            finalization_results[0].duration_ms = 0.0;
        }
        if (release_status != LT48_OK) status = LT48_CLEANUP_FAILED;
    }
    lt48_programmer_progress_publish_internal(
                    programmer, LT48_EXECUTION_COMPLETE,
                    out_result->operation_completed_transactions,
                    lt48_protocol_plan_transaction_count(read_plan),
                    0u, out_result->finalization_phase_count,
                    out_result->finalization_phase_count);
    return status;
}

lt48_status lt48_programmer_execute_m37_operation(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *plan,
    lt48_m37_operation operation,
    lt48_m37_read_collector *collector,
    unsigned timeout_ms,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_snapshot_read_execution_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    const lt48_m37_resource_audit *audit;
    unsigned char plan_sha256[LT48_SHA256_SIZE];
    lt48_cleanup_action release_interface = {
        "release-interface", native_release, NULL
    };
    lt48_status status;
    if (programmer == NULL || resources == NULL || plan == NULL ||
        finalization_results == NULL || out_result == NULL ||
        timeout_ms == 0u) return LT48_INVALID_ARGUMENT;
    if (operation == LT48_M37_COMPARE && collector == NULL)
        return LT48_INVALID_ARGUMENT;
    if (operation != LT48_M37_COMPARE && collector != NULL)
        return LT48_INVALID_ARGUMENT;
    if (programmer->execution_active) return LT48_BUSY;
    if (lt48_protocol_plan_packet_size_internal(plan) != 512u)
        return LT48_UNSUPPORTED;
    audit = lt48_m37_resources_audit(resources);
    status = lt48_protocol_plan_sha256(plan, plan_sha256);
    if (!programmer->interface_claimed || !programmer->m37_operation_armed ||
        status != LT48_OK || audit == NULL ||
        programmer->armed_m37_operation != operation ||
        memcmp(programmer->armed_manifest_sha256,
               audit->resource_manifest_sha256, LT48_SHA256_SIZE) != 0 ||
        memcmp(programmer->armed_plan_sha256, plan_sha256,
               LT48_SHA256_SIZE) != 0)
        return status == LT48_OK ? LT48_SAFETY_REJECTED : status;
    programmer->m37_operation_armed = 0;
    programmer->armed_m37_operation = 0;
    memset(programmer->armed_manifest_sha256, 0, LT48_SHA256_SIZE);
    memset(programmer->armed_plan_sha256, 0, LT48_SHA256_SIZE);
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    release_interface.context = programmer;
    memset(out_result, 0, sizeof(*out_result));
    status = lt48_m37_snapshot_read_execute_injected(
        resources, plan, &transport, programmer, collector,
        &release_interface, finalization_results,
        finalization_result_capacity, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    if (programmer->interface_claimed &&
        out_result->finalization_phase_count == 0u) {
        lt48_status release_status = release_claim(programmer);
        out_result->finalization_phase_count = 1u;
        out_result->finalization_status = release_status == LT48_OK ?
                                          LT48_OK : LT48_CLEANUP_FAILED;
        out_result->finalization_failure_count =
            release_status == LT48_OK ? 0u : 1u;
        if (finalization_result_capacity != 0u) {
            finalization_results[0].name = "release-interface";
            finalization_results[0].status = release_status;
        }
        if (release_status != LT48_OK) status = LT48_CLEANUP_FAILED;
    }
    return status;
}

lt48_status lt48_programmer_set_indicators(
    lt48_programmer *programmer, unsigned mask, unsigned timeout_ms
) {
    lt48_runtime_transport_session *session = NULL;
    lt48_protocol_transport transport = {native_write, native_read, NULL, NULL, 0};
    unsigned char command[] = {0x40, (unsigned char)mask};
    if (!programmer || mask > 7 || !timeout_ms) return LT48_INVALID_ARGUMENT;
    if (!programmer->interface_claimed || !programmer->transport_ready ||
        programmer->execution_active || programmer->m37_read_armed ||
        programmer->m37_operation_armed || programmer->auto_id_eprom_armed ||
        !programmer->bulk) return LT48_SAFETY_REJECTED;
    programmer->execution_active = 1;
    programmer->execution_timeout_ms = timeout_ms;
    lt48_status status = lt48_runtime_transport_session_create(&transport, programmer, &session);
    if (status == LT48_OK)
        status = lt48_runtime_transport_session_controller_write(session, command, sizeof(command));
    if (status == LT48_OK) status = lt48_runtime_transport_session_flush(session);
    lt48_runtime_transport_session_close(session);
    programmer->execution_active = 0;
    if (status != LT48_OK) programmer->transport_ready = 0;
    return status;
}

lt48_status lt48_programmer_transport_initialize(
    lt48_programmer *programmer,
    unsigned timeout_ms,
    lt48_transport_info *out_info
) {
    unsigned char packet[LT48_PACKET_SIZE] = {0};
    lt48_status status;

    if (programmer == NULL || out_info == NULL || timeout_ms == 0) {
        return LT48_INVALID_ARGUMENT;
    }
    if (!programmer->interface_claimed || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed ||
        programmer->bulk == NULL) {
        return LT48_SAFETY_REJECTED;
    }
    programmer->transport_ready = 0;
    memset(out_info, 0, sizeof(*out_info));
    packet[0] = 0x11;
    packet[1] = 0xaa;
    packet[2] = 0x55;
    packet[3] = 0x00;
    packet[4] = 0x40;
    packet[5] = 0x00;
    status = exchange_packet(programmer, packet, 64u, timeout_ms);
    if (status != LT48_OK) {
        return status;
    }
    out_info->packet_size_ack = (packet[0] | packet[1]) != 0u;
    if (out_info->packet_size_ack == 0) {
        return LT48_PROTOCOL_ERROR;
    }

    memset(packet, 0, sizeof(packet));
    packet[0] = 0x11;
    packet[1] = 0xaa;
    packet[2] = 0x55;
    packet[3] = 0x01;
    /* Original 13.10 speed helper changes bytes 0..3 only, retaining
       the initial size word 0x0040 (live and offline original evidence). */
    packet[4] = 0x40;
    status = exchange_packet(programmer, packet, 64u, timeout_ms);
    if (status != LT48_OK) {
        return status;
    }
    out_info->mode_ack = (packet[0] | packet[1]) != 0u;
    if (out_info->mode_ack == 0) return LT48_PROTOCOL_ERROR;
    /* Windows selects 512 only when speed-probe reply byte 2 has its sign
       bit set. Native hardware plans currently contain fixed 512-byte
       packets, so a 64-byte unit must fail before any plan is armed. */
    if ((packet[2] & 0x80u) == 0u) return LT48_UNSUPPORTED;
    memset(packet, 0, sizeof(packet));
    packet[0] = 0x11;
    packet[1] = 0xaa;
    packet[2] = 0x55;
    packet[5] = 0x02;
    status = exchange_packet(programmer, packet, 512u, timeout_ms);
    if (status != LT48_OK) return status;
    if ((packet[0] | packet[1]) == 0u) return LT48_PROTOCOL_ERROR;
    programmer->transport_ready = 1;
    return LT48_OK;
}

lt48_status lt48_programmer_execute_palce_operation(
    lt48_programmer *programmer,
    const lt48_palce_resources *resources,
    lt48_operation_kind operation,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    unsigned timeout_ms,
    lt48_palce_operation_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_palce_execute_operation(
        resources, &transport, programmer, operation,
        input_fuses, input_count, read_fuses, read_capacity, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_pal_operation(
    lt48_programmer *programmer,
    const lt48_pal_resources *resources,
    lt48_operation_kind operation,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    unsigned timeout_ms,
    lt48_pal_operation_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    if (!lt48_pal_resources_source_audited(resources)) return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_pal_execute_operation(
        resources, &transport, programmer, operation,
        input_fuses, input_count, read_fuses, read_capacity, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_gal_operation(
    lt48_programmer *programmer,
    const lt48_gal_resources *resources,
    lt48_operation_kind operation,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    unsigned timeout_ms,
    lt48_gal_operation_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    if (!lt48_gal_resources_live_qualified(resources) && !lt48_gal_resources_source_audited(resources))
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_gal_execute_operation(
        resources, &transport, programmer, operation,
        input_fuses, input_count, read_fuses, read_capacity, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_microwire(
    lt48_programmer *programmer,
    const lt48_microwire_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_microwire_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_microwire_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_compact_i2c(
    lt48_programmer *programmer,
    const lt48_compact_i2c_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_compact_i2c_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_compact_i2c_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_config_memory(
    lt48_programmer *programmer,
    const lt48_config_memory_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_config_memory_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_config_memory_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_xc17(
    lt48_programmer *programmer,
    const lt48_xc17_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_xc17_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_xc17_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_avr(
    lt48_programmer *programmer,
    const lt48_avr_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_avr_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_avr_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_pic(
    lt48_programmer *programmer,
    const lt48_pic_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_pic_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_pic_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_pic24(
    lt48_programmer *programmer,
    const lt48_pic24_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    const lt48_pic24_execution_options *options, lt48_pic24_diagnostics *diagnostics,
    unsigned timeout_ms,
    lt48_pic24_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_pic24_execution_options bound=options?*options:(lt48_pic24_execution_options){2,NULL,NULL};
    if(!bound.id_decision){bound.id_decision=programmer->id_decide;bound.id_context=programmer->id_decision_context;}
    status = lt48_pic24_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, &bound, diagnostics, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_pic18_with_options(
    lt48_programmer *programmer,
    const lt48_pic18_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    const lt48_pic18_execution_options *options, lt48_pic18_diagnostics *diagnostics,
    unsigned timeout_ms,
    lt48_pic18_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_pic18_execution_options bound=options?*options:(lt48_pic18_execution_options){0};
    if(!bound.id_decision){bound.id_decision=programmer->id_decide;bound.id_context=programmer->id_decision_context;}
    status = lt48_pic18_execute_with_options(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, &bound, diagnostics, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_pic18(lt48_programmer *p,const lt48_pic18_resources *r,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,
 unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,unsigned timeout,lt48_pic18_result *out) {
 return lt48_programmer_execute_pic18_with_options(p,r,op,start,length,image,size,read,read_size,margin,type,NULL,NULL,timeout,out);
}

lt48_status lt48_programmer_execute_compact_spi(
    lt48_programmer *programmer,
    const lt48_compact_spi_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_compact_spi_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_compact_spi_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_stk10c68(
    lt48_programmer *programmer,
    const lt48_stk10c68_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_stk10c68_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_stk10c68_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_mcu51(
    lt48_programmer *programmer,
    const lt48_mcu51_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    const lt48_mcu51_options *options, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_mcu51_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_mcu51_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, options, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_xc7000(
    lt48_programmer *programmer,
    const lt48_xc7000_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, unsigned timeout_ms,
    lt48_xc7000_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_xc7000_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_small_eprom(
    lt48_programmer *programmer,
    const lt48_small_eprom_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_small_eprom_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_small_eprom_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_eep21_erase(lt48_programmer *programmer,
    const lt48_m37_resources *resources,uint32_t start,uint32_t length,
    unsigned timeout_ms,lt48_small_eprom_result *out_result) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_m37_dynamic_erase_execute(resources,&transport,programmer,start,length,out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    if(release_status!=LT48_OK)out_result->cleanup_completed=0;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_xp_eprom(
    lt48_programmer *programmer,
    const lt48_xp_eprom_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_xp_eprom_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_xp_eprom_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_xp_flash(
    lt48_programmer *programmer,
    const lt48_xp_flash_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_xp_flash_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 120000u ? 120000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_xp_flash_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_addtab_flash(
    lt48_programmer *programmer,
    const lt48_addtab_flash_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_addtab_flash_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 120000u ? 120000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_addtab_flash_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_addtab_spi(
    lt48_programmer *programmer,
    const lt48_addtab_spi_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_addtab_spi_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* The source can wait over two minutes for chip erase and then erase
     * several special regions. Do not truncate its audited tES timer. */
    unsigned minimum_ms=operation==LT48_OPERATION_ERASE?900000u:120000u;
    programmer->execution_timeout_ms = timeout_ms < minimum_ms ? minimum_ms : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_addtab_spi_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_page_eeprom(
    lt48_programmer *programmer,
    const lt48_page_eeprom_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_page_eeprom_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_page_eeprom_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_serial_page(
    lt48_programmer *programmer,
    const lt48_serial_page_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_serial_page_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_ed2(
    lt48_programmer *programmer,
    const lt48_ed2_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_ed2_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_lp51_basic(
    lt48_programmer *programmer,
    const lt48_lp51_basic_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_lp51_basic_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_s2051(
    lt48_programmer *programmer,
    const lt48_s2051_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_s2051_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_s8253(
    lt48_programmer *programmer,
    const lt48_s8253_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_s8253_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_st7(
    lt48_programmer *programmer,
    const lt48_st7_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_small_eprom_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_st7_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_tms370(
    lt48_programmer *programmer,
    const lt48_tms370_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_small_eprom_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_tms370_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_tms87(
    lt48_programmer *programmer,
    const lt48_tms87_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_small_eprom_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_tms87_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_lpc(
    lt48_programmer *programmer,
    const lt48_lpc_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Allow bounded source pulses and controller replies within each transfer. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_lpc_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_lpcflash(
    lt48_programmer *programmer,
    const lt48_lpcflash_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Allow bounded source pulses and controller replies within each transfer. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_lpcflash_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}
lt48_status lt48_programmer_execute_at89_c5114(
    lt48_programmer *programmer,
    const lt48_c5114_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Allow bounded source pulses and controller replies within each transfer. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_c5114_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}
lt48_status lt48_programmer_execute_at89_c5132(
    lt48_programmer *programmer,
    const lt48_c5132_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Allow bounded source pulses and controller replies within each transfer. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_c5132_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}
lt48_status lt48_programmer_execute_at89_ic2(
    lt48_programmer *programmer,
    const lt48_ic2_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Allow bounded source pulses and controller replies within each transfer. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_ic2_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_re2(
    lt48_programmer *programmer,
    const lt48_re2_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_re2_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_lp2052(
    lt48_programmer *programmer,
    const lt48_lp2052_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_lp2052_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_usb51(
    lt48_programmer *programmer,
    const lt48_usb51_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_usb51_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_lp51(
    lt48_programmer *programmer,
    const lt48_at89_lp51_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_at89_lp51_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_at89_lp_large(
    lt48_programmer *programmer,
    const lt48_at89_lp_large_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_verify_margin_selection margin, unsigned margin_type, lt48_m37_id_decision_fn decide, void *decision_context,
    unsigned timeout_ms,
    lt48_serial_page_result *out_result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    lt48_status status;
    lt48_status release_status;
    if (programmer == NULL || resources == NULL || out_result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    /* Original XP worker permits 60 seconds for a programming block. */
    programmer->execution_timeout_ms = timeout_ms < 60000u ? 60000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    status = lt48_at89_lp_large_execute(
        resources, &transport, programmer, operation, start, length,
        input_fuses, input_count, read_fuses, read_capacity, margin, margin_type, decide, decision_context, out_result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    release_status = release_claim(programmer);
    if (status == LT48_OK && release_status != LT48_OK)
        status = LT48_CLEANUP_FAILED;
    if (release_status != LT48_OK) out_result->cleanup_completed = 0;
    if (out_result->cleanup_status == LT48_OK && release_status != LT48_OK)
        out_result->cleanup_status = LT48_CLEANUP_FAILED;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_hitachi(
    lt48_programmer *programmer,
    const lt48_hitachi_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_hitachi_result *result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hitachi_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_hc11(
    lt48_programmer *programmer,
    const lt48_hc11_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_hc11_result *result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->operation.first_mismatch = UINT32_MAX;
        result->operation.operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc11_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->operation.cleanup_status == LT48_OK)
            result->operation.cleanup_status = LT48_CLEANUP_FAILED;
        result->operation.cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_nmos68705(
    lt48_programmer *programmer,
    const lt48_nmos68705_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_nmos68705_qualified(resources))
        return lt48_nmos68705_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_nmos68705_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}


lt48_status lt48_programmer_execute_hc705_compact(
    lt48_programmer *programmer,
    const lt48_hc705_compact_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_hc705_compact_qualified(resources))
        return lt48_hc705_compact_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc705_compact_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}


lt48_status lt48_programmer_execute_hc705_bootstrap(
    lt48_programmer *programmer,
    const lt48_hc705_bootstrap_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_hc705_bootstrap_qualified(resources))
        return lt48_hc705_bootstrap_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc705_bootstrap_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_hc705_jp(
    lt48_programmer *programmer,
    const lt48_hc705_jp_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_hc705_jp_qualified(resources))
        return lt48_hc705_jp_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc705_jp_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}


lt48_status lt48_programmer_execute_hc711(
    lt48_programmer *programmer,
    const lt48_hc711_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc711_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_hc908(
    lt48_programmer *programmer,
    const lt48_hc908_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_hc908_result *result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc908_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_legacy_dataflash(
    lt48_programmer *programmer,
    const lt48_legacy_dataflash_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_legacy_dataflash_result *result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_legacy_dataflash_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_gdac_serial(
    lt48_programmer *programmer,
    const lt48_gdac_serial_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_m37_id_decision_fn decide, void *decision_context,
    lt48_gdac_serial_result *result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 300000u ? 300000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_gdac_serial_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, decide, decision_context, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_gaaf_serial(
    lt48_programmer *programmer,
    const lt48_gaaf_serial_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_m37_id_decision_fn decide, void *decision_context,
    lt48_gaaf_serial_result *result
) {
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 300000u ? 300000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_gaaf_serial_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, decide, decision_context, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_pof_device(
    lt48_programmer *programmer,const lt48_pof_device *device,
    lt48_operation_kind operation,const unsigned char *input,size_t input_size,
    unsigned char *output,size_t output_size,unsigned timeout_ms,
    lt48_pof_operation_result *result
) {
    if(result){
        memset(result,0,sizeof(*result));
        result->operation_status=LT48_SAFETY_REJECTED;
        result->traversal.first_mismatch=SIZE_MAX;
    }
    if(!programmer||!device||!result||!programmer->interface_claimed||
       !programmer->bulk||!programmer->transport_ready||programmer->execution_active||
       programmer->m37_read_armed||programmer->m37_operation_armed||
       programmer->auto_id_eprom_armed||!timeout_ms)return LT48_SAFETY_REJECTED;
    lt48_status status=lt48_pof_device_validate(device,operation,input,input_size);
    size_t size=lt48_pof_device_capacity(device);
    int read=operation==LT48_OPERATION_READ;
    if(status==LT48_OK&&read&&(!output||output_size<size))status=LT48_BUFFER_TOO_SMALL;
    if(status!=LT48_OK){result->operation_status=status;return status;}
    unsigned char *pending=read?malloc(size):NULL;
    if(read&&!pending){result->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    lt48_protocol_transport transport={native_write,native_read,native_cancelled,
        lt48_programmer_progress_publish_internal,LT48_EXECUTION_READ_OPERATION};
    programmer->execution_timeout_ms=timeout_ms;
    programmer->execution_active=1;
    lt48_programmer_progress_publish_internal(programmer,LT48_EXECUTION_READ_OPERATION,0,0,0,0,0);
    status=lt48_pof_device_execute(device,&transport,programmer,operation,input,input_size,
        pending,read?size:0,result);
    programmer->execution_active=0;
    programmer->execution_timeout_ms=0;
    lt48_status released=release_claim(programmer);
    if(released!=LT48_OK){
        if(status==LT48_OK)status=LT48_CLEANUP_FAILED;
        if(result->cleanup_status==LT48_OK)result->cleanup_status=LT48_CLEANUP_FAILED;
        result->cleanup_completed=0;
    }
    /* Keep the caller's image intact until both electrical cleanup and USB
     * interface release have succeeded. */
    if(status==LT48_OK&&read)memcpy(output,pending,size);
    free(pending);result->operation_status=status;
    lt48_programmer_progress_publish_internal(programmer,LT48_EXECUTION_INTERFACE_RELEASE,0,0,1,1,1);
    lt48_programmer_progress_publish_internal(programmer,LT48_EXECUTION_COMPLETE,0,0,1,1,1);
    return status;
}

lt48_status lt48_programmer_execute_pof_vectors(lt48_programmer *programmer,
    const lt48_pof_device *device,const lt48_pof_file_metadata *vectors,
    unsigned margin,unsigned x_mode,unsigned timeout_ms,lt48_vector_operation_result *result) {
    if(result){
        memset(result,0,sizeof(*result));result->operation_status=LT48_SAFETY_REJECTED;
        result->first_mismatch_row=SIZE_MAX;
    }
    if(!programmer||!device||!result||!programmer->interface_claimed||
       !programmer->bulk||!programmer->transport_ready||programmer->execution_active||
       programmer->m37_read_armed||programmer->m37_operation_armed||
       programmer->auto_id_eprom_armed||!timeout_ms)return LT48_SAFETY_REJECTED;
    lt48_status status=lt48_pof_device_validate_vectors(device,vectors,margin,x_mode);
    if(status!=LT48_OK){result->operation_status=status;return status;}
    lt48_protocol_transport transport={native_write,native_read,native_cancelled,
        lt48_programmer_progress_publish_internal,LT48_EXECUTION_READ_OPERATION};
    programmer->execution_timeout_ms=timeout_ms;programmer->execution_active=1;
    lt48_programmer_progress_publish_internal(programmer,LT48_EXECUTION_READ_OPERATION,0,0,0,0,0);
    status=lt48_pof_device_execute_vectors(device,vectors,margin,x_mode,&transport,programmer,result);
    programmer->execution_active=0;programmer->execution_timeout_ms=0;
    lt48_status released=release_claim(programmer);
    if(released!=LT48_OK){
        if(status==LT48_OK)status=LT48_CLEANUP_FAILED;
        if(result->cleanup_status==LT48_OK)result->cleanup_status=LT48_CLEANUP_FAILED;
        result->cleanup_completed=0;
    }
    result->operation_status=status;
    lt48_programmer_progress_publish_internal(programmer,LT48_EXECUTION_INTERFACE_RELEASE,0,0,1,1,1);
    lt48_programmer_progress_publish_internal(programmer,LT48_EXECUTION_COMPLETE,0,0,1,1,1);
    return status;
}

lt48_status lt48_programmer_execute_hc705_ca(
    lt48_programmer *programmer,
    const lt48_hc705_ca_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_hc705_ca_qualified(resources))
        return lt48_hc705_ca_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    programmer->execution_timeout_ms = timeout_ms < 30000u ? 30000u : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc705_ca_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_hc705_bd(
    lt48_programmer *programmer,
    const lt48_hc705_bd_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_hc705_bd_qualified(resources))
        return lt48_hc705_bd_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    unsigned minimum=lt48_hc705_bd_minimum_timeout_ms(resources);
    programmer->execution_timeout_ms = timeout_ms < minimum ? minimum : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc705_bd_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}


lt48_status lt48_programmer_execute_hc705_bx(
    lt48_programmer *programmer,
    const lt48_hc705_bx_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_hc705_bx_qualified(resources))
        return lt48_hc705_bx_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    unsigned minimum=lt48_hc705_bx_minimum_timeout_ms(resources);
    programmer->execution_timeout_ms = timeout_ms < minimum ? minimum : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc705_bx_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

lt48_status lt48_programmer_execute_hc705_jjsrpl(
    lt48_programmer *programmer,
    const lt48_hc705_jjsrpl_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_hc705_jjsrpl_qualified(resources))
        return lt48_hc705_jjsrpl_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    unsigned minimum=lt48_hc705_jjsrpl_minimum_timeout_ms(resources);
    programmer->execution_timeout_ms = timeout_ms < minimum ? minimum : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_hc705_jjsrpl_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}


lt48_status lt48_programmer_execute_s08(
    lt48_programmer *programmer,
    const lt48_s08_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_s08_qualified(resources))
        return lt48_s08_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    unsigned minimum=lt48_s08_minimum_timeout_ms(resources);
    programmer->execution_timeout_ms = timeout_ms < minimum ? minimum : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_s08_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}
lt48_status lt48_programmer_execute_rs08(
    lt48_programmer *programmer,
    const lt48_rs08_resources *resources,
    lt48_operation_kind operation, uint32_t start, size_t length,
    const unsigned char *input, size_t input_size,
    unsigned char *read, size_t read_size, unsigned timeout_ms,
    lt48_verify_margin_selection margin, unsigned margin_type,
    lt48_small_eprom_result *result
) {
    /* Qualification is checked before transport state or interface work. */
    if (!lt48_rs08_qualified(resources))
        return lt48_rs08_execute(resources,NULL,NULL,operation,start,length,
            input,input_size,read,read_size,margin,margin_type,result);
    lt48_protocol_transport transport = {
        native_write, native_read, native_cancelled,
        lt48_programmer_progress_publish_internal,
        LT48_EXECUTION_READ_OPERATION
    };
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
        result->first_mismatch = UINT32_MAX;
        result->operation_status = LT48_SAFETY_REJECTED;
    }
    if (programmer == NULL || resources == NULL || result == NULL ||
        !programmer->interface_claimed || programmer->bulk == NULL ||
        !programmer->transport_ready || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || timeout_ms == 0u)
        return LT48_SAFETY_REJECTED;
    unsigned minimum=lt48_rs08_minimum_timeout_ms(resources);
    programmer->execution_timeout_ms = timeout_ms < minimum ? minimum : timeout_ms;
    programmer->execution_active = 1;
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_READ_OPERATION, 0u, 0u, 0u, 0u, 0u);
    lt48_status status = lt48_rs08_execute(
        resources, &transport, programmer, operation, start, length,
        input, input_size, read, read_size, margin, margin_type, result);
    programmer->execution_active = 0;
    programmer->execution_timeout_ms = 0u;
    lt48_status released = release_claim(programmer);
    if (released != LT48_OK) {
        if (status == LT48_OK) status = LT48_CLEANUP_FAILED;
        if (result->cleanup_status == LT48_OK)
            result->cleanup_status = LT48_CLEANUP_FAILED;
        result->cleanup_completed = 0;
    }
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_INTERFACE_RELEASE, 0u, 0u, 1u, 1u, 1u);
    lt48_programmer_progress_publish_internal(
        programmer, LT48_EXECUTION_COMPLETE, 0u, 0u, 1u, 1u, 1u);
    return status;
}

