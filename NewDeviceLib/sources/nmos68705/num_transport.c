#include "num_transport.h"
#include "../../../c/src/internal.h"
#include <string.h>

lt48_status lt48_num_prepare_ready_internal(lt48_runtime_transport_session *wire) {
    if (!wire) return LT48_INVALID_ARGUMENT;
    const unsigned char packet[] = {0x3b,0xaa,0x55,0,0x6e};
    unsigned char ack;
    lt48_status st = lt48_runtime_transport_session_controller_write(wire,packet,sizeof packet);
    if (st == LT48_OK)
        st = lt48_runtime_transport_session_controller_capture_internal(wire,&ack,1);
    if (st == LT48_OK && ack != 1) st = LT48_SAFETY_REJECTED;
    return st;
}

static lt48_status exchange(lt48_runtime_transport_session *wire,
    unsigned char command, uint16_t index, unsigned char *reply, unsigned n) {
    if (!wire || !reply || (command > 2) || n != (command == 2 ? 3u : 1u))
        return LT48_INVALID_ARGUMENT;
    const unsigned char packet[] = {0x3b,0xaa,0x55,0,0x60,command,
        (unsigned char)index,(unsigned char)(index >> 8)};
    lt48_status st = lt48_runtime_transport_session_controller_write(
        wire, packet, command == 2 ? sizeof packet : sizeof packet - 2u);
    if (st == LT48_OK)
        st = lt48_runtime_transport_session_controller_capture_internal(wire,reply,n);
    if (st == LT48_OK && reply[0] != 1) st = LT48_PROTOCOL_ERROR;
    return st;
}
lt48_status lt48_num_start_internal(lt48_runtime_transport_session *wire) {
    unsigned char ack;
    return exchange(wire,1,0,&ack,1);
}
lt48_status lt48_num_stop_internal(lt48_runtime_transport_session *wire) {
    unsigned char ack;
    return exchange(wire,0,0,&ack,1);
}
lt48_status lt48_num_pair_internal(void *context,uint16_t index,unsigned char pair[2]) {
    if (!context || !pair) return LT48_INVALID_ARGUMENT;
    unsigned char reply[3];
    lt48_status st = exchange(context,2,index,reply,sizeof reply);
    if (st == LT48_OK) { pair[0] = reply[1]; pair[1] = reply[2]; }
    return st;
}

lt48_status lt48_num_reader_init_internal(lt48_num_reader *r,
    lt48_runtime_transport_session *wire, unsigned space_size) {
    if (!r || !wire || (space_size != 2048 && space_size != 4096))
        return LT48_INVALID_ARGUMENT;
    memset(r,0,sizeof *r);
    r->wire = wire;
    r->main_pairs = space_size == 4096 ? 32768u : 16388u;
    return LT48_OK;
}

lt48_status lt48_num_cached_pair_internal(void *context, uint16_t index,
    unsigned char pair[2]) {
    lt48_num_reader *r = context;
    if (!r || !r->wire || !pair || r->failed) return LT48_INVALID_ARGUMENT;
    lt48_status st;
    if (r->delivered >= r->main_pairs) {
        /* Only R/U has four fresh pairs after the 16-bit clock wrap. */
        if (r->main_pairs != 32768u || r->delivered >= 32772u ||
            index != 2u * (r->delivered - r->main_pairs)) {
            r->failed = 1;
            return LT48_INVALID_ARGUMENT;
        }
        st = lt48_num_pair_internal(r->wire,index,pair);
        if (st == LT48_OK) ++r->delivered;
        else r->failed = 1;
        return st;
    }
    /* Fail closed if a caller changes the collector traversal. */
    if (index != r->next_index) { r->failed = 1; return LT48_INVALID_ARGUMENT; }
    const size_t main_count = r->main_pairs * 2u;
    if (r->used == r->count) {
        unsigned count = (unsigned)((main_count - 1u - index) / 1024u + 1u);
        if (count > 16u) count = 16u;
        const unsigned char packet[] = {0x3b,0xaa,0x55,0,0x60,3,
            (unsigned char)index,(unsigned char)(index >> 8),(unsigned char)count};
        unsigned char ack;
        st = lt48_runtime_transport_session_controller_write(r->wire,packet,sizeof packet);
        if (st == LT48_OK)
            st = lt48_runtime_transport_session_controller_capture_internal(r->wire,&ack,1);
        if (st == LT48_OK && ack != 1) st = LT48_PROTOCOL_ERROR;
        if (st == LT48_OK)
            st = lt48_runtime_transport_session_controller_capture_internal(r->wire,r->cache,2u*count);
        if (st == LT48_OK)
            st = lt48_runtime_transport_session_controller_capture_internal(r->wire,&ack,1);
        if (st == LT48_OK && ack != 1) st = LT48_PROTOCOL_ERROR;
        if (st != LT48_OK) { r->failed = 1; return st; }
        r->used = 0;
        r->count = count;
    }
    pair[0] = r->cache[2u*r->used];
    pair[1] = r->cache[2u*r->used+1u];
    ++r->used;
    ++r->delivered;
    size_t next = (size_t)index + 1024u;
    r->next_index = (uint16_t)(next < main_count ? next : (index % 1024u) + 2u);
    return LT48_OK;
}
