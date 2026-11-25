#include <labtool48/labtool48.h>

#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { ID_PACKET_SIZE = 64, ID_RESPONSE_SIZE = 13, ID_APPLET_SIZE = 600 };

static const unsigned char serial_applet_sha256[LT48_SHA256_SIZE] = {
    0xaa,0xca,0x6c,0xb5,0x8a,0xdb,0xbf,0xa6,0x6e,0x5b,0x42,0xa7,0x8c,0x33,0x89,0xf4,
    0xb1,0x4b,0x92,0x71,0x7b,0x55,0xf3,0x33,0x94,0x2a,0x06,0xb3,0x8d,0x8e,0x94,0x2c
};

static lt48_status transfer(lt48_programmer *p, uint8_t endpoint,
                            unsigned char *data, size_t size, unsigned timeout) {
    size_t done = 0u;
    lt48_status status = p->bulk(p->backend_context, endpoint, data, size,
                                 timeout, &done);
    return status != LT48_OK ? status :
           done == size ? LT48_OK : LT48_PROTOCOL_ERROR;
}

static lt48_status monitor_write(lt48_programmer *p,
                                 const unsigned char *data, size_t size,
                                 unsigned timeout) {
    while (size != 0u) {
        unsigned char packet[ID_PACKET_SIZE] = {0};
        unsigned char response[ID_PACKET_SIZE] = {0};
        size_t part = size < 61u ? size : 61u;
        lt48_status status;
        packet[0] = 0x0f;
        packet[1] = (unsigned char)part;
        packet[2] = (unsigned char)(part >> 8);
        memcpy(packet + 3u, data, part);
        status = transfer(p, 0x02, packet, sizeof(packet), timeout);
        if (status != LT48_OK) return status;
        status = transfer(p, 0x88, response, sizeof(response), timeout);
        if (status != LT48_OK) return status;
        if (response[0] != 1u || response[1] != 0u)
            return LT48_PROTOCOL_ERROR;
        data += part;
        size -= part;
    }
    return LT48_OK;
}

static lt48_status monitor_read(lt48_programmer *p, unsigned char *data,
                                size_t size, unsigned timeout) {
    unsigned char packet[ID_PACKET_SIZE] = {0};
    lt48_status status;
    if (size == 0u || size > 512u) return LT48_INVALID_ARGUMENT;
    packet[0] = 0x43;
    packet[3] = (unsigned char)size;
    packet[4] = (unsigned char)(size >> 8);
    status = transfer(p, 0x02, packet, sizeof(packet), timeout);
    return status != LT48_OK ? status : transfer(p, 0x88, data, size, timeout);
}

static unsigned char reverse_rotation(unsigned char value,
                                      unsigned char rotation) {
    unsigned n = (rotation ^ 0x80u) & 7u;
    if ((rotation & 0x80u) == 0u) n = (8u - n) & 7u;
    return n == 0u ? value :
           (unsigned char)((value << n) | (value >> (8u - n)));
}

static lt48_status read_serial(lt48_programmer *p, const char *path,
                               unsigned timeout, char serial[16]) {
    unsigned char code[ID_APPLET_SIZE];
    unsigned char digest[LT48_SHA256_SIZE];
    unsigned char command[7 + ID_APPLET_SIZE] = {
        0x39,0xaa,0x55,0x00,0xb0,0x57,0x02
    };
    unsigned char execute[] = {0x3b,0xaa,0x55,0x00,0xb0,0x5a,0x83};
    unsigned char response[ID_RESPONSE_SIZE];
    unsigned char plain[ID_RESPONSE_SIZE];
    FILE *file;
    lt48_status status;
    if (path == NULL || path[0] == '\0') return LT48_NOT_FOUND;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_NOT_FOUND;
    size_t count = fread(code, 1u, sizeof(code), file);
    int extra = fgetc(file);
    int error = ferror(file);
    fclose(file);
    if (error || count != sizeof(code) || extra != EOF)
        return LT48_FORMAT_ERROR;
    lt48_sha256(code, sizeof(code), digest);
    if (memcmp(digest, serial_applet_sha256, sizeof(digest)) != 0)
        return LT48_SAFETY_REJECTED;
    memcpy(command + 7u, code, sizeof(code));
    status = monitor_write(p, command, sizeof(command), timeout);
    if (status == LT48_OK)
        status = monitor_write(p, execute, sizeof(execute), timeout);
    if (status == LT48_OK)
        status = monitor_read(p, response, sizeof(response), timeout);
    if (status != LT48_OK) return status;
    for (size_t i = 0u; i < sizeof(plain); ++i)
        plain[i] = reverse_rotation(response[i], execute[6]) ^ execute[5];
    if (plain[0] != 'A' || plain[1] != 'E') return LT48_FORMAT_ERROR;
    for (size_t i = 2u; i < 7u; ++i)
        if ((plain[i] & 15u) > 9u || (plain[i] >> 4) > 9u)
            return LT48_FORMAT_ERROR;
    snprintf(serial, 16u, "AE%02X%02X%02X%02X%02X",
             plain[2], plain[3], plain[4], plain[5], plain[6]);
    return LT48_OK;
}

lt48_status lt48_programmer_identify(lt48_programmer *programmer,
                                     const char *serial_applet_path,
                                     unsigned timeout_ms,
                                     lt48_reader_identity *out_identity) {
    static const unsigned char request[] = {0x51,0xaa,0x55};
    static const char separator[] = " Firmware Version ";
    unsigned char length = 0u;
    unsigned char banner[65] = {0};
    lt48_status status;
    char *version;
    size_t model_length;
    if (programmer == NULL || out_identity == NULL || timeout_ms == 0u)
        return LT48_INVALID_ARGUMENT;
    memset(out_identity, 0, sizeof(*out_identity));
    out_identity->serial_status = LT48_NOT_FOUND;
    if (!programmer->interface_claimed || programmer->execution_active ||
        programmer->m37_read_armed || programmer->m37_operation_armed ||
        programmer->auto_id_eprom_armed || programmer->bulk == NULL)
        return LT48_SAFETY_REJECTED;
    status = monitor_write(programmer, request, sizeof(request), timeout_ms);
    if (status == LT48_OK)
        status = monitor_read(programmer, &length, 1u, timeout_ms);
    if (status != LT48_OK) return status;
    if (length < 20u || length >= sizeof(banner)) return LT48_FORMAT_ERROR;
    status = monitor_read(programmer, banner, length, timeout_ms);
    if (status != LT48_OK) return status;
    if (banner[length - 1u] != 0u ||
        strlen((char *)banner) != (size_t)length - 1u)
        return LT48_FORMAT_ERROR;
    version = strstr((char *)banner, separator);
    if (version == NULL || strncmp((char *)banner, "LabTool-", 8u) != 0)
        return LT48_FORMAT_ERROR;
    model_length = (size_t)(version - (char *)banner);
    version += sizeof(separator) - 1u;
    if (model_length == 0u || model_length >= sizeof(out_identity->model) ||
        *version == '\0' || strlen(version) >= sizeof(out_identity->firmware_version))
        return LT48_FORMAT_ERROR;
    memcpy(out_identity->model, banner, model_length);
    memcpy(out_identity->firmware_version, version, strlen(version) + 1u);
    out_identity->serial_status = read_serial(programmer, serial_applet_path,
                                              timeout_ms, out_identity->serial);
    return LT48_OK;
}
