#ifndef LT48_PIC_COMMON_MEMBER_H
#define LT48_PIC_COMMON_MEMBER_H
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Membership is independent of the executable procedure binding. Several
 * catalogue packages can share one procedure, without sharing qualification.
 * Names here are generated source membership data, never operation branches. */
typedef struct {
    uint16_t definition, family, pinmap;
    const char *manufacturer, *device;
} lt48_pic_batch_member;

static inline int lt48_pic_member_allowed(const lt48_pic_batch_member *members,
    size_t count, uint16_t definition, uint16_t family, uint16_t pinmap,
    const char *manufacturer, const unsigned char *device, size_t device_length) {
    if (!members || !count || !manufacturer || !device) return 0;
    for (size_t i=0;i<count;i++) {
        const lt48_pic_batch_member *m=members+i;
        if (m->definition==definition && m->family==family && m->pinmap==pinmap &&
            m->manufacturer && m->device && !strcmp(manufacturer,m->manufacturer) &&
            strlen(m->device)==device_length && !memcmp(device,m->device,device_length))
            return 1;
    }
    return 0;
}
#endif
