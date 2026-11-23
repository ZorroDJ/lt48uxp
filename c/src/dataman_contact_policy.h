#ifndef LT48_DATAMAN_CONTACT_POLICY_H
#define LT48_DATAMAN_CONTACT_POLICY_H
#include <stdint.h>
/* Original 13.10 0043B450 decoding and captured D1 mask table.
 * This is the normal (43D670) route, not the alternate contact procedure. */
static inline uint32_t lt48_dataman_contact_reverse8(uint8_t value) {
    uint32_t result = 0u;
    for (unsigned i = 0u; i < 8u; ++i)
        result |= ((uint32_t)(value >> i) & 1u) << (7u - i);
    return result;
}
static inline uint32_t lt48_dataman_contact_left(const unsigned char *raw) {
    return lt48_dataman_contact_reverse8(raw[2]) |
           lt48_dataman_contact_reverse8(raw[1]) << 8 |
           lt48_dataman_contact_reverse8(raw[0]) << 16;
}
static inline uint32_t lt48_dataman_contact_right(const unsigned char *raw) {
    return (uint32_t)raw[3] | (uint32_t)raw[4] << 8 | (uint32_t)raw[5] << 16;
}
static inline int lt48_dataman_am27c128_contacts_match(const unsigned char raw[12]) {
    uint32_t left = lt48_dataman_contact_left(raw);
    uint32_t right = lt48_dataman_contact_right(raw);
    /* Original43B5F0 adds the second sample through aux0/aux1 masks (1/0). */
    left |= lt48_dataman_contact_left(raw + 6) & 1u;
    return (left & 0x3fffu) == 0x3fffu &&
           (right & 0x3fffu) == 0x3fffu &&
           (left & 0xffc000u) == 0u && (right & 0xffc000u) == 0u;
}
#endif
