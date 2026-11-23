#include <labtool48/nmos68705_num.h>
#include "../../../c/src/internal.h"
#include <string.h>
#include <stdlib.h>

static unsigned address_byte(const lt48_nmos68705_num_contract *c, unsigned a) {
    unsigned b = 0;
    for (unsigned i = 0; i < 8; ++i) b |= ((a >> c->address_bits[i]) & 1u) << i;
    return b;
}

static int eprom(unsigned n, unsigned a) {
    return (a >= 0x80u && a <= (n == 4096u ? 0xf38u : 0x784u)) || a >= n - 8u;
}

static int stable(const lt48_nmos68705_num_contract *c, unsigned a) {
    return eprom(c->space_size, a) ||
        (a >= c->bootstrap_start && a < c->bootstrap_start + c->bootstrap_size);
}

static int valid(const lt48_nmos68705_num_contract *c) {
    if (!c || (c->space_size != 2048u && c->space_size != 4096u)) return 0;
    if (c->bootstrap_start != (c->space_size == 4096u ? 0xf80u : 0x785u) ||
        c->bootstrap_size != (c->space_size == 4096u ? 120u : 115u)) return 0;
    unsigned mask = 0, bits = c->space_size == 4096u ? 12u : 11u;
    for (unsigned i = 0; i < 8; ++i) {
        if (c->address_bits[i] >= bits || (mask & (1u << c->address_bits[i]))) return 0;
        mask |= 1u << c->address_bits[i];
    }
    return (mask & 63u) == 63u;
}

lt48_status lt48_nmos68705_num_decode(
    const lt48_nmos68705_num_contract *c, const unsigned char *samples,
    size_t count, unsigned char *image, size_t size,
    lt48_nmos68705_num_result *result) {
    if (!valid(c) || !samples || !image || !result) return LT48_INVALID_ARGUMENT;
    const unsigned n = c->space_size, mask = n - 1u;
    if (size != n) return LT48_INVALID_ARGUMENT;
    if (count < (size_t)n * 16u || count > (size_t)n * 32u + 7u)
        return LT48_FORMAT_ERROR;
    unsigned char data[4096], candidate[4096], accepted[4096];
    lt48_nmos68705_num_result chosen = {0};
    int found = 0;
    for (unsigned phase = 0; phase < 8; ++phase) {
        const size_t frames = (count - phase) / 8u;
        if (frames < (size_t)n * 2u) continue;
        for (unsigned i = 0; i < n; ++i) data[i] = samples[phase + (size_t)i * 8u + 2u];
        for (unsigned anchor = 0; anchor < n; ++anchor) {
            if (samples[phase + (size_t)anchor * 8u] != address_byte(c, c->bootstrap_start)) continue;
            unsigned char h[32];
            lt48_sha256_context sha;
            lt48_sha256_init(&sha);
            for (unsigned i = 0; i < c->bootstrap_size; ++i)
                lt48_sha256_update(&sha, &data[(anchor + i) & mask], 1);
            lt48_sha256_final(&sha, h);
            if (memcmp(h, c->bootstrap_sha256, 32)) continue;
            const unsigned base = (c->bootstrap_start - anchor) & mask;
            int ok = 1;
            for (size_t i = 0; i < frames; ++i) {
                const unsigned a = (base + (unsigned)i) & mask;
                const unsigned char *w = samples + phase + i * 8u;
                unsigned expected_address = address_byte(c, a);
                if (w[0] != expected_address || w[1] != expected_address ||
                    w[4] != expected_address || w[5] != expected_address) { ok = 0; break; }
                /* Reads of I/O (notably TDR) can change within a cycle. Those
                 * bytes never enter the public EPROM image. */
                if (stable(c, a) && (w[2] != w[3] || w[2] != w[6] || w[2] != w[7] ||
                                     w[2] != data[i & mask])) { ok = 0; break; }
            }
            if (!ok) continue;
            memset(candidate, 0xff, n);
            size_t bytes = 0;
            for (unsigned i = 0; i < n; ++i) {
                unsigned a = (base + i) & mask;
                if (eprom(n, a)) { candidate[a] = data[i]; ++bytes; }
            }
            /* Multiple phases may agree when a sampling point lies inside a
             * stable interval. Conflicting complete interpretations fail. */
            if (found && memcmp(candidate, accepted, n)) return LT48_PROTOCOL_ERROR;
            if (!found) {
                memcpy(accepted, candidate, n);
                chosen.sample_phase = phase;
                chosen.first_frame_address = base;
                chosen.frames_checked = frames;
                chosen.eprom_bytes = bytes;
                found = 1;
            }
        }
    }
    if (!found) return LT48_PROTOCOL_ERROR;
    memcpy(image, accepted, n);
    *result = chosen;
    return LT48_OK;
}

lt48_status lt48_nmos68705_num_collect(
    const lt48_nmos68705_num_contract *c, lt48_nmos68705_num_pair_reader read_pair,
    void *context, unsigned char *image, size_t size,
    lt48_nmos68705_num_result *result) {
    if (!valid(c) || !read_pair || !image || !result || size != c->space_size)
        return LT48_INVALID_ARGUMENT;
    const size_t count = (size_t)c->space_size * 16u + 8u;
    unsigned char *samples = malloc(count);
    if (!samples) return LT48_NO_MEMORY;
    lt48_status st = LT48_OK;
    /* Interleave the requests to leave 1024 target clocks between adjacent
     * requests in each column. A slow controller may wait for the next wrap;
     * it must never substitute samples from a different requested index. */
    const size_t main_count = count < 65536u ? count : 65536u;
    for (unsigned base = 0; base < 1024u && st == LT48_OK; base += 2u) {
        for (size_t index = base; index < main_count && st == LT48_OK; index += 1024u)
            st = read_pair(context, (uint16_t)index, samples + index);
    }
    /* R/U requires eight fresh samples after the 16-bit counter wraps, to
     * retain two complete traversals at every possible eight-clock phase. */
    for (size_t index = main_count; index < count && st == LT48_OK; index += 2u)
        st = read_pair(context, (uint16_t)index, samples + index);
    if (st == LT48_OK)
        st = lt48_nmos68705_num_decode(c, samples, count, image, size, result);
    free(samples);
    return st;
}
