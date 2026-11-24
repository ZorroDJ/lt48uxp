#include <labtool48/labtool48.h>

const char *lt48_version_string(void) {
    return "0.1.0";
}

const char *lt48_status_string(lt48_status status) {
    switch (status) {
    case LT48_OK:
        return "success";
    case LT48_INVALID_ARGUMENT:
        return "invalid argument";
    case LT48_NO_MEMORY:
        return "out of memory";
    case LT48_USB_ERROR:
        return "USB error";
    case LT48_NOT_FOUND:
        return "programmer not found";
    case LT48_PERMISSION_DENIED:
        return "permission denied";
    case LT48_BUFFER_TOO_SMALL:
        return "buffer too small";
    case LT48_BUSY:
        return "programmer is busy";
    case LT48_PROTOCOL_ERROR:
        return "programmer protocol error";
    case LT48_FILE_ERROR:
        return "file error";
    case LT48_FORMAT_ERROR:
        return "invalid resource format";
    case LT48_TIMEOUT:
        return "programmer timed out";
    case LT48_UNSUPPORTED:
        return "operation is not implemented";
    case LT48_COMPILE_ERROR:
        return "definition compilation failed";
    case LT48_RUNTIME_ERROR:
        return "runtime interpretation failed";
    case LT48_CANCELLED:
        return "operation cancelled";
    case LT48_CLEANUP_FAILED:
        return "one or more cleanup phases failed";
    case LT48_SAFETY_REJECTED:
        return "execution plan failed safety validation";
    case LT48_FIRMWARE_REQUIRED:
        return "verified volatile FX2 firmware is required";
    case LT48_DEVICE_MISMATCH:
        return "device contents do not match";
    default:
        return "unknown error";
    }
}

int lt48_is_supported_usb_id(uint16_t vendor_id, uint16_t product_id) {
    return vendor_id == LT48_USB_VENDOR_ID &&
           (product_id == LT48_USB_PRODUCT_ID_READY ||
            product_id == LT48_USB_PRODUCT_ID_COLD ||
            product_id == LT48_USB_PRODUCT_ID_READY_ALT);
}

const char *lt48_programmer_descriptor_role(uint16_t product_id) {
    switch (product_id) {
    case LT48_USB_PRODUCT_ID_READY:
        return "runtime-identity";
    case LT48_USB_PRODUCT_ID_COLD:
        return "cold-or-runtime-undetermined";
    case LT48_USB_PRODUCT_ID_READY_ALT:
        return "runtime-alternate-identity";
    default:
        return "unsupported-identity";
    }
}

lt48_status lt48_programmer_info_select(
    const lt48_programmer_info *infos, size_t count, int has_exact_target,
    uint8_t bus_number, uint8_t device_address,
    lt48_programmer_info *out_info
) {
    size_t match = 0u;
    size_t selected = 0u;

    if (out_info == NULL || (count != 0u && infos == NULL) ||
        (has_exact_target != 0 && has_exact_target != 1)) {
        return LT48_INVALID_ARGUMENT;
    }
    if (!has_exact_target) {
        if (count == 0u) return LT48_NOT_FOUND;
        if (count != 1u) return LT48_BUSY;
        *out_info = infos[0];
        return LT48_OK;
    }
    for (size_t index = 0u; index < count; ++index) {
        if (infos[index].bus_number == bus_number &&
            infos[index].device_address == device_address) {
            selected = index;
            ++match;
        }
    }
    if (match == 0u) return LT48_NOT_FOUND;
    if (match != 1u) return LT48_FORMAT_ERROR;
    *out_info = infos[selected];
    return LT48_OK;
}
