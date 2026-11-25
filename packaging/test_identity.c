#undef NDEBUG
#include <labtool48/labtool48.h>
#include "../c/src/internal.h"

#include <assert.h>
#include <string.h>

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
