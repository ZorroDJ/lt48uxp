#undef NDEBUG
#include <labtool48/labtool48.h>
#include "../c/src/internal.h"

#include <assert.h>
#include <string.h>

#ifdef ORIGINAL_IDENTITY_APPLET
typedef struct mock_monitor {
    unsigned char command[8];
    size_t command_size;
    size_t requested;
    int corrupt_banner;
} mock_monitor;

static lt48_status mock_bulk(void *context, uint8_t endpoint,
                             unsigned char *data, size_t size,
                             unsigned timeout, size_t *transferred) {
    static const unsigned char banner[] =
        "LabTool-48UXP Firmware Version 4.10";
    mock_monitor *mock = context;
    (void)timeout;
    if (endpoint == 0x02 && size == 64u) {
        if (data[0] == 0x0f && data[1] <= 61u) {
            assert(mock->command_size + data[1] <= sizeof(mock->command));
            memcpy(mock->command + mock->command_size, data + 3u, data[1]);
            mock->command_size += data[1];
            mock->requested = 0u;
        } else if (data[0] == 0x43) {
            mock->requested = (size_t)data[3] | ((size_t)data[4] << 8);
        } else return LT48_PROTOCOL_ERROR;
    } else if (endpoint == 0x88 && mock->requested == 0u && size == 64u) {
        memset(data, 0, size);
        data[0] = 1u;
    } else if (endpoint == 0x88 && mock->requested == size &&
               mock->command_size == 3u &&
               memcmp(mock->command, "\x51\xaa\x55", 3u) == 0) {
        if (size == 1u) data[0] = sizeof(banner);
        else if (size == sizeof(banner)) {
            memcpy(data, banner, size);
            if (mock->corrupt_banner) data[0] = 'X';
        } else return LT48_PROTOCOL_ERROR;
        mock->requested = 0u;
    } else return LT48_PROTOCOL_ERROR;
    *transferred = size;
    return LT48_OK;
}

int main(void) {
    mock_monitor monitor = {0};
    lt48_programmer programmer = {0};
    lt48_reader_identity identity = {0};
    programmer.bulk = mock_bulk;
    programmer.backend_context = &monitor;
    programmer.interface_claimed = 1;
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_OK);
    assert(strcmp(identity.model, "LabTool-48UXP") == 0);
    assert(strcmp(identity.firmware_version, "4.10") == 0);
    assert(identity.serial[0] == '\0');
    assert(identity.serial_status == LT48_NOT_FOUND);
    monitor = (mock_monitor){0};
    assert(lt48_programmer_identify(&programmer, __FILE__, 1000u,
                                    &identity) == LT48_OK);
    assert(identity.serial_status == LT48_FORMAT_ERROR);
    assert(monitor.command_size == 3u);
    monitor = (mock_monitor){.corrupt_banner = 1};
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_FORMAT_ERROR);
    programmer.interface_claimed = 0;
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_SAFETY_REJECTED);
    return 0;
}
#else
typedef struct mock_monitor {
    unsigned char response[9u + 65u + 3u];
    unsigned char upload[6u + 414u];
    unsigned char request[9u];
    size_t payload_size;
    size_t upload_size;
    size_t request_size;
    size_t response_pos;
    unsigned execute_args;
    unsigned signature_size;
    unsigned char command;
    unsigned char last_operation;
    unsigned char last_value;
    int saw_request;
    int saw_signature;
    int short_status;
    int corrupt_banner;
    int corrupt_checksum;
    int corrupt_nonce;
    int unknown_banner;
    int unsupported;
} mock_monitor;

static void make_response(mock_monitor *mock, unsigned char nonce) {
    const char *banner = mock->unknown_banner ?
        "LabTool-UNKNOWN Firmware Version 0.00" :
        "LabTool-48UXP Firmware Version 4.10";
    size_t size = strlen(banner) + 1u;
    unsigned char *response = mock->response;
    memcpy(response, "LTRS", 4u);
    response[4] = 1u;
    response[5] = nonce ^ (mock->corrupt_nonce ? 1u : 0u);
    response[6] = 1u;
    response[7] = mock->unsupported ? 1u : 0u;
    response[8] = mock->unsupported ? 0u : (unsigned char)(size + 1u);
    if (!mock->unsupported) {
        response[9] = (unsigned char)size;
        memcpy(response + 10u, banner, size);
        if (mock->corrupt_banner) response[10] = 'X';
    }
    mock->payload_size = mock->unsupported ? 0u : size + 1u;
    unsigned char checksum = 0u;
    for (size_t i = 0u; i < 9u + mock->payload_size; ++i)
        checksum ^= response[i];
    response[9u + mock->payload_size] =
        checksum ^ (mock->corrupt_checksum ? 1u : 0u);
    response[10u + mock->payload_size] = 0xa5u;
    response[11u + mock->payload_size] = 0x5au;
}

static lt48_status mock_bulk(void *context, uint8_t endpoint,
                             unsigned char *data, size_t size,
                             unsigned timeout, size_t *transferred) {
    mock_monitor *mock = context;
    (void)timeout;
    if (endpoint == 0x02 && size == 512u) {
        mock->last_operation = data[0];
        if (data[0] == 0u) {
            if (data[1] != 0x39u && data[1] != 0x3bu)
                return LT48_PROTOCOL_ERROR;
            mock->command = data[1];
            if (mock->command == 0x3bu)
                assert(mock->upload_size == sizeof(mock->upload));
        } else if (data[0] == 1u) {
            if (mock->command == 0x39u &&
                mock->upload_size < sizeof(mock->upload)) {
                mock->upload[mock->upload_size++] = data[1];
                if (mock->upload_size == sizeof(mock->upload)) {
                    assert(memcmp(mock->upload, "\xaa\x55\x00\xb0\x9d\x01", 6u) == 0);
                }
            } else if (mock->command == 0x3bu && mock->execute_args < 4u) {
                const unsigned char args[] = {0xaa,0x55,0x00,0xb0};
                assert(data[1] == args[mock->execute_args++]);
            } else if (mock->request_size < sizeof(mock->request)) {
                mock->request[mock->request_size++] = data[1];
                if (mock->request_size == sizeof(mock->request)) {
                    unsigned char checksum = 0u;
                    for (size_t i = 0u; i < 8u; ++i)
                        checksum ^= mock->request[i];
                    assert(memcmp(mock->request, "LTID", 4u) == 0);
                    assert(mock->request[4] == 1u && mock->request[6] == 1u &&
                           mock->request[7] == 0u && mock->request[8] == checksum);
                    make_response(mock, mock->request[5]);
                    mock->saw_request = 1;
                }
            } else {
                const unsigned char signature[] = {0xaa,0x55};
                if (mock->response_pos != 9u ||
                    mock->signature_size >= sizeof(signature) ||
                    data[1] != signature[mock->signature_size++])
                    return LT48_PROTOCOL_ERROR;
                mock->saw_signature = mock->signature_size == 2u;
            }
        } else if (data[0] == 4u && mock->saw_request &&
                   (mock->response_pos < 9u || mock->saw_signature ||
                    mock->unsupported) &&
                   mock->response_pos < 9u + mock->payload_size + 3u)
            mock->last_value = mock->response[mock->response_pos++];
        else return LT48_PROTOCOL_ERROR;
    } else if (endpoint == 0x88 && size == 512u) {
        memset(data, 0, size);
        data[0] = 1u;
        if (mock->last_operation == 4u) data[2] = mock->last_value;
        *transferred = mock->short_status ? 64u : 512u;
        return LT48_OK;
    } else return LT48_PROTOCOL_ERROR;
    *transferred = size;
    return LT48_OK;
}

int main(void) {
    mock_monitor monitor = {0};
    lt48_programmer programmer = {0};
    lt48_reader_identity identity = {0};
    programmer.bulk = mock_bulk;
    programmer.backend_context = &monitor;
    programmer.interface_claimed = 1;
    assert(lt48_programmer_identify(&programmer, "/nonexistent/original.bin",
                                    1000u, &identity) == LT48_OK);
    assert(monitor.saw_request && monitor.saw_signature);
    assert(strcmp(identity.model, "LabTool-48UXP") == 0);
    assert(strcmp(identity.firmware_version, "4.10") == 0);
    assert(identity.serial[0] == '\0');
    assert(identity.serial_status == LT48_UNSUPPORTED);
    assert(monitor.response_pos == 9u + monitor.payload_size + 3u);
    monitor = (mock_monitor){.short_status = 1};
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_OK);
    monitor = (mock_monitor){.corrupt_banner = 1};
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_FORMAT_ERROR);
    monitor = (mock_monitor){.corrupt_checksum = 1};
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_PROTOCOL_ERROR);
    monitor = (mock_monitor){.corrupt_nonce = 1};
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_PROTOCOL_ERROR);
    monitor = (mock_monitor){.unknown_banner = 1};
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_FORMAT_ERROR);
    monitor = (mock_monitor){.unsupported = 1};
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_UNSUPPORTED);
    assert(!monitor.saw_signature);
    programmer.interface_claimed = 0;
    assert(lt48_programmer_identify(&programmer, NULL, 1000u,
                                    &identity) == LT48_SAFETY_REJECTED);
    return 0;
}
#endif
