#include <labtool48/labtool48.h>

#include "internal.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    LT48_FX2_MAX_FILE_SIZE = 1024 * 1024,
    LT48_FX2_ADDRESS_SPACE = 65536,
    LT48_FX2_CPUCS = 0xe600,
    LT48_FX2_VENDOR_REQUEST = 0xa0,
    LT48_FX2_CONTROL_TIMEOUT_MS = 2000
};

typedef struct lt48_fx2_record {
    uint16_t address;
    uint8_t length;
    unsigned char data[16];
} lt48_fx2_record;

struct lt48_fx2_firmware {
    lt48_fx2_record *records;
    size_t record_count;
    size_t payload_size;
    size_t image_size;
    unsigned char file_sha256[LT48_SHA256_SIZE];
    unsigned char image_sha256[LT48_SHA256_SIZE];
};

static const unsigned char verified_image_sha256[LT48_SHA256_SIZE] = {
    0xb9,0x4b,0x55,0x8c,0xab,0x47,0x8e,0x96,
    0xbb,0x5b,0xbe,0x68,0x09,0x99,0x47,0x50,
    0x6f,0x5d,0xa8,0xcf,0xdb,0xba,0x90,0xe9,
    0x9e,0xca,0x1f,0x01,0x55,0xb7,0xda,0x0b
};

/* Owned Dataman 13.10 image: audited delta is descriptor revision plus the
 * command-48 per-byte ready/result handshake. All other bytes are identical. */
static const unsigned char verified_dataman_1310_sha256[LT48_SHA256_SIZE] = {
    0x1e,0xb9,0x58,0x5e,0x4a,0xa5,0xf1,0xb8,0x0b,0xf5,0x0f,0x7a,0x82,0x1e,0x3e,0x66,0x5b,0x26,0x09,0x78,0x33,0x89,0xd3,0x02,0x6f,0x36,0x78,0x17,0x02,0x20,0x9a,0x77
};
/* AEC bridge firmware 1.14, recovered from the original control transfer. */
static const unsigned char verified_aec_114_sha256[LT48_SHA256_SIZE] = {
    0x71,0x0d,0x72,0x6f,0xa5,0x54,0x13,0x6e,0x05,0x65,0x77,0x60,0x78,0x20,0xdf,0x15,0x6c,0x49,0x0b,0x9e,0x8a,0xcc,0x45,0x00,0x59,0xd9,0x78,0x6e,0x61,0xb9,0x6a,0xee
};

static int hex_nibble(unsigned char value) {
    if (value >= '0' && value <= '9') return (int)(value - '0');
    value = (unsigned char)tolower(value);
    if (value >= 'a' && value <= 'f') return (int)(value - 'a' + 10);
    return -1;
}

static int parse_hex_byte(const unsigned char *text, unsigned char *out) {
    int high = hex_nibble(text[0]);
    int low = hex_nibble(text[1]);
    if (high < 0 || low < 0) return 0;
    *out = (unsigned char)((high << 4) | low);
    return 1;
}

static lt48_status read_file(
    const char *path, unsigned char **out_data, size_t *out_size
) {
    FILE *file;
    long length;
    unsigned char *data;
    if (path == NULL || out_data == NULL || out_size == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_data = NULL;
    *out_size = 0u;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    if (fseek(file, 0, SEEK_END) != 0 || (length = ftell(file)) < 0 ||
        length > LT48_FX2_MAX_FILE_SIZE || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return LT48_FILE_ERROR;
    }
    data = malloc((size_t)length == 0u ? 1u : (size_t)length);
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

lt48_status lt48_fx2_firmware_open_hex(
    const char *path, lt48_fx2_firmware **out_firmware
) {
    unsigned char *file_data = NULL;
    unsigned char *present = NULL;
    unsigned char *image = NULL;
    lt48_fx2_firmware *firmware = NULL;
    size_t file_size = 0u;
    size_t position = 0u;
    size_t capacity = 0u;
    int saw_eof = 0;
    uint32_t address_base = 0;
    lt48_status status;

    if (path == NULL || out_firmware == NULL) return LT48_INVALID_ARGUMENT;
    *out_firmware = NULL;
    status = read_file(path, &file_data, &file_size);
    if (status != LT48_OK) return status;
    firmware = calloc(1u, sizeof(*firmware));
    present = calloc(LT48_FX2_ADDRESS_SPACE, 1u);
    image = malloc(LT48_FX2_ADDRESS_SPACE);
    if (firmware == NULL || present == NULL || image == NULL) {
        status = LT48_NO_MEMORY;
        goto fail;
    }
    memset(image, 0xff, LT48_FX2_ADDRESS_SPACE);
    lt48_sha256(file_data, file_size, firmware->file_sha256);

    while (position < file_size) {
        size_t line_start = position;
        size_t line_end;
        size_t hex_length;
        unsigned char bytes[260];
        unsigned char checksum = 0u;
        size_t byte_count;
        uint16_t address;
        unsigned char type;

        while (position < file_size && file_data[position] != '\n' &&
               file_data[position] != '\r') ++position;
        line_end = position;
        while (position < file_size &&
               (file_data[position] == '\n' || file_data[position] == '\r')) {
            ++position;
        }
        if (line_end == line_start) continue;
        if (saw_eof || file_data[line_start] != ':') {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        hex_length = line_end - line_start - 1u;
        if ((hex_length & 1u) != 0u || hex_length < 10u ||
            hex_length > sizeof(bytes) * 2u) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        byte_count = hex_length / 2u;
        for (size_t index = 0u; index < byte_count; ++index) {
            if (!parse_hex_byte(file_data + line_start + 1u + index * 2u,
                                &bytes[index])) {
                status = LT48_FORMAT_ERROR;
                goto fail;
            }
            checksum = (unsigned char)(checksum + bytes[index]);
        }
        if (checksum != 0u || byte_count != (size_t)bytes[0] + 5u) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        address = (uint16_t)((uint16_t)bytes[1] << 8 | bytes[2]);
        type = bytes[3];
        if (type == 1u) {
            if (bytes[0] != 0u || address != 0u) {
                status = LT48_FORMAT_ERROR;
                goto fail;
            }
            saw_eof = 1;
            continue;
        }
        /* Intel HEX address records are metadata, never device writes. */
        if (type == 2u || type == 4u) {
            if (bytes[0] != 2u || address != 0u) {
                status = LT48_FORMAT_ERROR;
                goto fail;
            }
            address_base = ((uint32_t)bytes[4] << 8 | bytes[5]) <<
                           (type == 2u ? 4u : 16u);
            continue;
        }
        uint64_t target_address = (uint64_t)address_base + address;
        if (type != 0u || bytes[0] == 0u ||
            target_address + bytes[0] > LT48_FX2_ADDRESS_SPACE ||
            (target_address <= LT48_FX2_CPUCS &&
             target_address + bytes[0] > LT48_FX2_CPUCS)) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        address = (uint16_t)target_address;
        /* Keep USB writes bounded regardless of the HEX producer's line size. */
        size_t chunks = ((size_t)bytes[0] + 15u) / 16u;
        if (firmware->record_count + chunks > capacity) {
            size_t next_capacity = capacity == 0u ? 64u : capacity * 2u;
            lt48_fx2_record *next = realloc(
                firmware->records, next_capacity * sizeof(*next));
            if (next == NULL) { status = LT48_NO_MEMORY; goto fail; }
            firmware->records = next;
            capacity = next_capacity;
        }
        for (size_t offset = 0; offset < bytes[0]; offset += 16u) {
            size_t n = bytes[0] - offset;
            if (n > 16u) n = 16u;
            lt48_fx2_record *record = &firmware->records[firmware->record_count++];
            record->address = (uint16_t)(address + offset);
            record->length = (uint8_t)n;
            memcpy(record->data, bytes + 4u + offset, n);
        }
        firmware->payload_size += bytes[0];
        for (size_t offset = 0u; offset < bytes[0]; ++offset) {
            size_t target = (size_t)address + offset;
            if (present[target]) {
                status = LT48_FORMAT_ERROR;
                goto fail;
            }
            present[target] = 1u;
            image[target] = bytes[4u + offset];
            if (target + 1u > firmware->image_size) {
                firmware->image_size = target + 1u;
            }
        }
    }
    if (!saw_eof || firmware->record_count == 0u) {
        status = LT48_FORMAT_ERROR;
        goto fail;
    }
    lt48_sha256(image, firmware->image_size, firmware->image_sha256);
    free(file_data);
    free(present);
    free(image);
    *out_firmware = firmware;
    return LT48_OK;

fail:
    free(file_data);
    free(present);
    free(image);
    lt48_fx2_firmware_close(firmware);
    return status;
}

void lt48_fx2_firmware_close(lt48_fx2_firmware *firmware) {
    if (firmware == NULL) return;
    free(firmware->records);
    free(firmware);
}

lt48_status lt48_fx2_firmware_info_get(
    const lt48_fx2_firmware *firmware, lt48_fx2_firmware_info *out_info
) {
    if (firmware == NULL || out_info == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_info, 0, sizeof(*out_info));
    out_info->record_count = firmware->record_count;
    out_info->payload_size = firmware->payload_size;
    out_info->image_size = firmware->image_size;
    memcpy(out_info->file_sha256, firmware->file_sha256, LT48_SHA256_SIZE);
    memcpy(out_info->image_sha256, firmware->image_sha256, LT48_SHA256_SIZE);
    out_info->verified_owner_image =
        firmware->image_size == 5319u &&
        (memcmp(firmware->image_sha256, verified_image_sha256,
               LT48_SHA256_SIZE) == 0 ||
         memcmp(firmware->image_sha256, verified_dataman_1310_sha256,
               LT48_SHA256_SIZE) == 0 ||
         memcmp(firmware->image_sha256, verified_aec_114_sha256,
               LT48_SHA256_SIZE) == 0);
    return LT48_OK;
}

static void digest_hex(
    const unsigned char digest[LT48_SHA256_SIZE],
    char text[LT48_SHA256_SIZE * 2u + 1u]
) {
    static const char digits[] = "0123456789abcdef";
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        text[index * 2u] = digits[digest[index] >> 4];
        text[index * 2u + 1u] = digits[digest[index] & 0x0fu];
    }
    text[LT48_SHA256_SIZE * 2u] = '\0';
}

lt48_status lt48_fx2_prepare_load(
    const lt48_fx2_firmware *firmware, uint8_t bus_number,
    uint8_t device_address, lt48_fx2_load_preparation *out_preparation
) {
    lt48_fx2_load_preparation preparation = {0};
    char image_hex[LT48_SHA256_SIZE * 2u + 1u];
    char binding[256];
    int binding_length;
    lt48_status status;
    if (firmware == NULL || out_preparation == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = lt48_fx2_firmware_info_get(firmware, &preparation.firmware);
    if (status != LT48_OK) return status;
    if (!preparation.firmware.verified_owner_image) {
        return LT48_SAFETY_REJECTED;
    }
    preparation.bus_number = bus_number;
    preparation.device_address = device_address;
    digest_hex(preparation.firmware.image_sha256, image_hex);
    binding_length = snprintf(
        binding, sizeof(binding),
        "labtool48-fx2-load-v1\nbus=%u\naddress=%u\n"
        "image_sha256=%s\nrecords=%zu\npayload=%zu\n",
        bus_number, device_address, image_hex,
        preparation.firmware.record_count,
        preparation.firmware.payload_size
    );
    if (binding_length < 0 || (size_t)binding_length >= sizeof(binding)) {
        return LT48_RUNTIME_ERROR;
    }
    lt48_sha256((const unsigned char *)binding, (size_t)binding_length,
                preparation.confirmation_sha256);
    *out_preparation = preparation;
    return LT48_OK;
}

static lt48_status fx2_control_write(
    libusb_device_handle *handle, uint16_t address,
    unsigned char *data, uint16_t length
) {
    int result = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR |
            LIBUSB_RECIPIENT_DEVICE,
        LT48_FX2_VENDOR_REQUEST, address, 0u, data, length,
        LT48_FX2_CONTROL_TIMEOUT_MS
    );
    if (result < 0) return from_libusb_error_internal(result);
    return result == length ? LT48_OK : LT48_PROTOCOL_ERROR;
}

lt48_status lt48_programmer_load_fx2(
    lt48_context *context, uint8_t bus_number, uint8_t device_address,
    const lt48_fx2_firmware *firmware, uint32_t authorization,
    lt48_fx2_load_result *out_result
) {
    libusb_device **devices = NULL;
    libusb_device *selected = NULL;
    libusb_device_handle *handle = NULL;
    lt48_fx2_firmware_info info;
    ssize_t device_count;
    unsigned char cpu_halt = 1u;
    unsigned char cpu_run = 0u;
    lt48_status status;

    if (context == NULL || firmware == NULL || out_result == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    memset(out_result, 0, sizeof(*out_result));
    if (authorization != LT48_FX2_LOAD_HARDWARE_AUTHORIZATION) {
        return LT48_SAFETY_REJECTED;
    }
    status = lt48_fx2_firmware_info_get(firmware, &info);
    if (status != LT48_OK) return status;
    if (!info.verified_owner_image) return LT48_SAFETY_REJECTED;

    device_count = libusb_get_device_list(context->usb, &devices);
    if (device_count < 0) return from_libusb_error_internal((int)device_count);
    for (ssize_t index = 0; index < device_count; ++index) {
        struct libusb_device_descriptor descriptor;
        int result = libusb_get_device_descriptor(devices[index], &descriptor);
        if (result == LIBUSB_SUCCESS &&
            libusb_get_bus_number(devices[index]) == bus_number &&
            libusb_get_device_address(devices[index]) == device_address &&
            descriptor.idVendor == LT48_USB_VENDOR_ID &&
            descriptor.idProduct == LT48_USB_PRODUCT_ID_COLD) {
            selected = devices[index];
            break;
        }
    }
    if (selected == NULL) {
        libusb_free_device_list(devices, 1);
        return LT48_NOT_FOUND;
    }
    {
        int result = libusb_open(selected, &handle);
        libusb_free_device_list(devices, 1);
        if (result != LIBUSB_SUCCESS) return from_libusb_error_internal(result);
    }
    status = fx2_control_write(handle, LT48_FX2_CPUCS, &cpu_halt, 1u);
    if (status != LT48_OK) goto done;
    out_result->cpu_halted = 1;
    for (size_t index = 0u; index < firmware->record_count; ++index) {
        const lt48_fx2_record *record = &firmware->records[index];
        status = fx2_control_write(
            handle, record->address, (unsigned char *)record->data,
            record->length
        );
        if (status != LT48_OK) goto done;
        ++out_result->records_written;
        out_result->bytes_written += record->length;
    }
    status = fx2_control_write(handle, LT48_FX2_CPUCS, &cpu_run, 1u);
    if (status == LT48_OK) out_result->cpu_released = 1;

done:
    libusb_close(handle);
    return status;
}
