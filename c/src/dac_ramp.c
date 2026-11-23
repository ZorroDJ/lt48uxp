#include <labtool48/labtool48.h>

#include <stdint.h>
#include <string.h>

lt48_status lt48_dac_ramp_packet(
    uint8_t channel, uint8_t from_code, uint8_t to_code, uint32_t duration,
    unsigned char out_packet[7]
) {
    uint32_t delta, scale;
    uint64_t rounded, interval = 0u;
    unsigned char packet[7];
    if (out_packet == NULL) return LT48_INVALID_ARGUMENT;
    if (channel >= 4u || duration == 0u || duration > INT32_MAX)
        return LT48_SAFETY_REJECTED;
    delta = from_code > to_code ? (uint32_t)from_code - to_code :
                                 (uint32_t)to_code - from_code;
    scale = channel == 0u ? 20u : 10u;
    /* Exact positive round-to-nearest, ties upward, followed by subtract
     * one and clamp. The executable audit checks this rational model against
     * both host conversions in the selected PALCE timing envelope. */
    rounded = ((uint64_t)delta * scale * 2u + duration) /
              ((uint64_t)duration * 2u);
    uint64_t code = rounded == 0u ? 0u : rounded - 1u;
    if (code > 255u) code = 255u;
    if (code == 0u && delta != 0u) {
        uint64_t candidate = ((uint64_t)duration / delta) * 2u;
        if (candidate >= 10u) interval = candidate - 10u;
        if (interval > UINT16_MAX) return LT48_SAFETY_REJECTED;
    }
    packet[0] = 0x12u;
    packet[1] = channel;
    packet[2] = from_code;
    packet[3] = to_code;
    packet[4] = (unsigned char)code;
    packet[5] = (unsigned char)interval;
    packet[6] = (unsigned char)(interval >> 8u);
    memcpy(out_packet, packet, sizeof(packet));
    return LT48_OK;
}
