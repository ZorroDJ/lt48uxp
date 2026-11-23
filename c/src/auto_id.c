#include <labtool48/labtool48.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

static const lt48_auto_id_eprom_recipe recipes[] = {
    {24u,500u,1200u,0x23u,0x17u,0x17u,0x21u,0x13u,0x00u,8u,
     {0x50u,0x54u,0x58u,0x60u,0x64u,0x68u,0x6cu,0x70u},0u,{0u}},
    {28u,500u,1200u,0x25u,0x17u,0x17u,0x21u,0x13u,0x00u,8u,
     {0x50u,0x54u,0x58u,0x60u,0x64u,0x68u,0x6cu,0x70u},1u,{0x24u}},
    {32u,500u,1200u,0x27u,0x17u,0x17u,0x21u,0x13u,0x25u,8u,
     {0x50u,0x54u,0x58u,0x60u,0x64u,0x68u,0x6cu,0x70u},2u,{0x26u,0x08u}},
    {40u,500u,1200u,0x2bu,0x0eu,0x21u,0x22u,0x18u,0x00u,16u,
     {0x58u,0x54u,0x50u,0x4cu,0x48u,0x44u,0x40u,0x3cu,
      0x34u,0x30u,0x2cu,0x28u,0x24u,0x20u,0x1cu,0x18u},0u,{0u}},
    {42u,500u,1200u,0x18u,0x0eu,0x21u,0x2au,0x0cu,0x00u,16u,
     {0x40u,0x48u,0x50u,0x58u,0x64u,0x6cu,0x74u,0x7cu,
      0x44u,0x4cu,0x54u,0x5cu,0x68u,0x70u,0x78u,0x80u},1u,{0x22u}},
    {44u,500u,1200u,0x18u,0x0eu,0x21u,0x2au,0x0cu,0x00u,16u,
     {0x40u,0x48u,0x50u,0x58u,0x64u,0x6cu,0x74u,0x7cu,
      0x44u,0x4cu,0x54u,0x5cu,0x68u,0x70u,0x78u,0x80u},1u,{0x2cu}}
};

static uint8_t reverse8(uint8_t value) {
    uint8_t result = 0u;
    unsigned index;
    for (index = 0u; index < 8u; ++index) {
        result = (uint8_t)((result << 1) | (value & 1u));
        value >>= 1;
    }
    return result;
}

static struct timespec auto_id_now(void) {
    struct timespec value = {0};
    (void)timespec_get(&value, TIME_UTC);
    return value;
}

static double auto_id_elapsed_ms(
    struct timespec start, struct timespec finish
) {
    time_t seconds = finish.tv_sec - start.tv_sec;
    long nanoseconds = finish.tv_nsec - start.tv_nsec;
    if (nanoseconds < 0) {
        --seconds;
        nanoseconds += 1000000000L;
    }
    if (seconds < 0) return 0.0;
    return (double)seconds * 1000.0 +
           (double)nanoseconds / 1000000.0;
}

size_t lt48_auto_id_eprom_recipe_count(void) {
    return sizeof(recipes) / sizeof(recipes[0]);
}

lt48_status lt48_auto_id_eprom_recipe_get(
    size_t index, lt48_auto_id_eprom_recipe *out_recipe
) {
    if (out_recipe == NULL) return LT48_INVALID_ARGUMENT;
    if (index >= lt48_auto_id_eprom_recipe_count()) return LT48_NOT_FOUND;
    *out_recipe = recipes[index];
    return LT48_OK;
}

lt48_status lt48_auto_id_eprom_recipe_find(
    uint16_t package_pin_count, lt48_auto_id_eprom_recipe *out_recipe
) {
    size_t index;
    if (out_recipe == NULL) return LT48_INVALID_ARGUMENT;
    for (index = 0u; index < lt48_auto_id_eprom_recipe_count(); ++index) {
        if (recipes[index].package_pin_count == package_pin_count) {
            *out_recipe = recipes[index];
            return LT48_OK;
        }
    }
    return LT48_NOT_FOUND;
}

lt48_status lt48_auto_id_eprom_decode_socket(
    const unsigned char raw[12],
    lt48_auto_id_socket_observation *out_observation
) {
    uint32_t left;
    uint32_t right;
    uint32_t occupied;
    unsigned low = 0u;
    unsigned high = 23u;
    if (raw == NULL || out_observation == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_observation, 0, sizeof(*out_observation));
    memcpy(out_observation->raw, raw, 12u);
    left = (uint32_t)reverse8(raw[2]) |
           (uint32_t)reverse8(raw[1]) << 8 |
           (uint32_t)reverse8(raw[0]) << 16;
    right = (uint32_t)raw[3] | (uint32_t)raw[4] << 8 |
            (uint32_t)raw[5] << 16;
    occupied = left | right;
    out_observation->left_contact_mask = left;
    out_observation->right_contact_mask = right;
    out_observation->occupied_contact_mask = occupied;
    if (occupied == 0u) return LT48_NOT_FOUND;
    while (low < 24u && (occupied & (UINT32_C(1) << low)) == 0u) ++low;
    while (high != 0u && (occupied & (UINT32_C(1) << high)) == 0u) --high;
    if (low >= 24u || high < low) return LT48_PROTOCOL_ERROR;
    out_observation->first_half_position = (uint16_t)(low + 1u);
    out_observation->last_half_position = (uint16_t)(high + 1u);
    out_observation->package_pin_count = (uint16_t)((high - low + 1u) * 2u);
    return LT48_OK;
}

lt48_status lt48_auto_id_eprom_plan(
    int has_explicit_pin_hint, uint16_t explicit_pin_hint,
    lt48_auto_id_eprom_plan_info *out_plan
) {
    static const unsigned char tag[] = "LT48-AUTO-SELECT-EPROM-v1";
    unsigned char serialized[32u * (sizeof(recipes) / sizeof(recipes[0]))];
    size_t position = 0u;
    size_t index;
    if (out_plan == NULL || (has_explicit_pin_hint != 0 &&
                             has_explicit_pin_hint != 1)) {
        return LT48_INVALID_ARGUMENT;
    }
    memset(out_plan, 0, sizeof(*out_plan));
    out_plan->recipe_count = lt48_auto_id_eprom_recipe_count();
    out_plan->has_explicit_pin_hint = has_explicit_pin_hint;
    out_plan->explicit_pin_hint = has_explicit_pin_hint ? explicit_pin_hint : 0u;
    out_plan->probe_always_first = 1;
    out_plan->selected_device_independent = 1;
    out_plan->cleanup_finalizer_required = 1;
    out_plan->maximum_operating_vcc_centivolts = 500u;
    out_plan->maximum_signature_vpp_centivolts = 1200u;
    for (index = 0u; index < out_plan->recipe_count; ++index) {
        const lt48_auto_id_eprom_recipe *recipe = &recipes[index];
        unsigned char *bytes = serialized + position;
        bytes[0] = (unsigned char)recipe->package_pin_count;
        bytes[1] = (unsigned char)(recipe->package_pin_count >> 8);
        bytes[2] = recipe->rail_pin;
        bytes[3] = recipe->power_pin_a;
        bytes[4] = recipe->power_pin_b;
        bytes[5] = recipe->read_enable_pin;
        bytes[6] = recipe->id_select_pin;
        bytes[7] = recipe->fixed_pin;
        bytes[8] = recipe->data_pin_count;
        memcpy(bytes + 9u, recipe->data_capture_codes, 16u);
        bytes[25] = recipe->address_pattern_pin_count;
        memcpy(bytes + 26u, recipe->address_pattern_pins, 2u);
        bytes[28] = (unsigned char)recipe->operating_vcc_centivolts;
        bytes[29] = (unsigned char)(recipe->operating_vcc_centivolts >> 8);
        bytes[30] = (unsigned char)recipe->signature_vpp_centivolts;
        bytes[31] = (unsigned char)(recipe->signature_vpp_centivolts >> 8);
        position += 32u;
        if (!has_explicit_pin_hint ||
            recipe->package_pin_count == explicit_pin_hint)
            ++out_plan->eligible_recipe_count;
    }
    if (has_explicit_pin_hint && out_plan->eligible_recipe_count == 0u)
        return LT48_NOT_FOUND;
    {
        unsigned char digest_input[sizeof(tag) - 1u + sizeof(serialized)];
        memcpy(digest_input, tag, sizeof(tag) - 1u);
        memcpy(digest_input + sizeof(tag) - 1u, serialized, sizeof(serialized));
        lt48_sha256(digest_input, sizeof(digest_input),
                    out_plan->provider_sha256);
    }
    return LT48_OK;
}

static void auto_id_digest_hex(
    const unsigned char digest[LT48_SHA256_SIZE],
    char text[LT48_SHA256_SIZE * 2u + 1u]
) {
    static const char digits[] = "0123456789abcdef";
    size_t index;
    for (index = 0u; index < LT48_SHA256_SIZE; ++index) {
        text[index * 2u] = digits[digest[index] >> 4];
        text[index * 2u + 1u] = digits[digest[index] & 0x0fu];
    }
    text[LT48_SHA256_SIZE * 2u] = '\0';
}

lt48_status lt48_auto_id_eprom_prepare(
    uint8_t bus_number,
    uint8_t device_address,
    int acquire_signature,
    int has_explicit_pin_hint,
    uint16_t explicit_pin_hint,
    lt48_auto_id_eprom_preparation *out_preparation
) {
    lt48_auto_id_eprom_preparation preparation;
    char provider_hex[LT48_SHA256_SIZE * 2u + 1u];
    char binding[512];
    int binding_length;
    lt48_status status;
    if (out_preparation == NULL ||
        (acquire_signature != 0 && acquire_signature != 1))
        return LT48_INVALID_ARGUMENT;
    memset(&preparation, 0, sizeof(preparation));
    status = lt48_auto_id_eprom_plan(
        has_explicit_pin_hint, explicit_pin_hint, &preparation.plan);
    if (status != LT48_OK) return status;
    preparation.bus_number = bus_number;
    preparation.device_address = device_address;
    preparation.acquire_signature = acquire_signature;
    auto_id_digest_hex(preparation.plan.provider_sha256, provider_hex);
    binding_length = snprintf(
        binding, sizeof(binding),
        "labtool48-auto-select-eprom-v1\n"
        "readiness=transport-initialize\n"
        "bus=%u\naddress=%u\nmode=%s\n"
        "has_pin_hint=%d\npin_hint=%u\n"
        "provider_sha256=%s\nrecipes=%zu\neligible=%zu\n"
        "maximum_vcc_centivolts=%u\n"
        "maximum_signature_vpp_centivolts=%u\n"
        "probe_always_first=1\nfinalizer_required=1\n",
        bus_number, device_address,
        acquire_signature ? "signature" : "probe-only",
        preparation.plan.has_explicit_pin_hint,
        preparation.plan.explicit_pin_hint,
        provider_hex, preparation.plan.recipe_count,
        preparation.plan.eligible_recipe_count,
        preparation.plan.maximum_operating_vcc_centivolts,
        preparation.plan.maximum_signature_vpp_centivolts
    );
    if (binding_length < 0 || (size_t)binding_length >= sizeof(binding))
        return LT48_RUNTIME_ERROR;
    lt48_sha256((const unsigned char *)binding, (size_t)binding_length,
                preparation.confirmation_sha256);
    *out_preparation = preparation;
    return LT48_OK;
}

lt48_status lt48_auto_id_eprom_execution_observation(
    const lt48_auto_id_eprom_execution_result *execution,
    lt48_auto_id_observation *out_observation
) {
    lt48_auto_id_eprom_recipe recipe;
    if (execution == NULL || out_observation == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_observation, 0, sizeof(*out_observation));
    if (execution->operation_status != LT48_OK)
        return execution->operation_status;
    if (!execution->finalizer_attempted ||
        execution->finalizer_status != LT48_OK)
        return LT48_CLEANUP_FAILED;
    if (execution->interface_release_attempted &&
        execution->interface_release_status != LT48_OK)
        return LT48_CLEANUP_FAILED;
    if (lt48_auto_id_eprom_recipe_find(
            execution->socket.package_pin_count, &recipe) != LT48_OK)
        return LT48_NOT_FOUND;
    out_observation->has_pin_count_hint = 1;
    out_observation->pin_count_hint = execution->socket.package_pin_count;
    if (execution->has_signature) {
        if (!execution->signature_requested ||
            !execution->signature_attempted)
            return LT48_PROTOCOL_ERROR;
        out_observation->has_manufacturer_id = 1;
        out_observation->manufacturer_id = execution->manufacturer_id;
        out_observation->has_device_id = 1;
        out_observation->device_id = execution->device_id;
    }
    return LT48_OK;
}

#define AUTO_ID_PACKET_SIZE 512u
#define AUTO_ID_QUEUE_LIMIT (AUTO_ID_PACKET_SIZE - 2u)

typedef struct auto_id_session {
    const lt48_protocol_transport *transport;
    void *transport_context;
    unsigned char queue[AUTO_ID_QUEUE_LIMIT];
    size_t queue_size;
    size_t command_transfer_count;
    size_t planned_command_transfer_count;
    int rails_requested;
    lt48_execution_stage progress_stage;
} auto_id_session;

static lt48_status auto_id_exchange(
    auto_id_session *session, const unsigned char request[AUTO_ID_PACKET_SIZE],
    unsigned char response[AUTO_ID_PACKET_SIZE]
) {
    size_t transferred = 0u;
    lt48_status status = session->transport->write(
        session->transport_context, 0x02u, request, AUTO_ID_PACKET_SIZE,
        &transferred
    );
    if (status != LT48_OK) return status;
    if (transferred != AUTO_ID_PACKET_SIZE) return LT48_PROTOCOL_ERROR;
    memset(response, 0, AUTO_ID_PACKET_SIZE);
    transferred = 0u;
    status = session->transport->read(
        session->transport_context, 0x88u, response, AUTO_ID_PACKET_SIZE,
        &transferred
    );
    if (status != LT48_OK) return status;
    if (transferred != AUTO_ID_PACKET_SIZE) return LT48_PROTOCOL_ERROR;
    ++session->command_transfer_count;
    if (session->transport->progress != NULL) {
        int finalizing = session->progress_stage ==
                         LT48_EXECUTION_AUTO_ID_FINALIZATION;
        session->transport->progress(
            session->transport_context, session->progress_stage,
            session->command_transfer_count,
            session->planned_command_transfer_count,
            finalizing ? 1u : 0u, finalizing ? 1u : 0u,
            finalizing ? 1u : 0u
        );
    }
    return LT48_OK;
}

static lt48_status auto_id_flush(auto_id_session *session) {
    unsigned char request[AUTO_ID_PACKET_SIZE] = {0u};
    unsigned char response[AUTO_ID_PACKET_SIZE];
    lt48_status status;
    if (session->queue_size == 0u) return LT48_OK;
    request[0] = 0x2cu;
    memcpy(request + 1u, session->queue, session->queue_size);
    request[session->queue_size + 1u] = 0u;
    status = auto_id_exchange(session, request, response);
    session->queue_size = 0u;
    if (status != LT48_OK) return status;
    if (response[0] == 0u && response[1] == 0u)
        return LT48_PROTOCOL_ERROR;
    return LT48_OK;
}

static lt48_status auto_id_queue_pair(
    auto_id_session *session, uint8_t kind, uint8_t value
) {
    lt48_status status;
    if (session->queue_size + 2u > AUTO_ID_QUEUE_LIMIT) {
        status = auto_id_flush(session);
        if (status != LT48_OK) return status;
    }
    session->queue[session->queue_size++] = kind;
    session->queue[session->queue_size++] = value;
    return LT48_OK;
}

static lt48_status auto_id_q1(auto_id_session *session, uint8_t value) {
    return auto_id_queue_pair(session, 1u, value);
}

static lt48_status auto_id_q2(auto_id_session *session, uint8_t value) {
    return auto_id_queue_pair(session, 2u, value);
}

static lt48_status auto_id_write_block(
    auto_id_session *session, const unsigned char *bytes, size_t count
) {
    size_t index;
    for (index = 0u; index < count; ++index) {
        lt48_status status = auto_id_q1(session, bytes[index]);
        if (status != LT48_OK) return status;
    }
    return LT48_OK;
}

static lt48_status auto_id_read_inline(
    auto_id_session *session, unsigned char *data, uint16_t count
) {
    unsigned char request[AUTO_ID_PACKET_SIZE] = {0u};
    unsigned char response[AUTO_ID_PACKET_SIZE];
    lt48_status status;
    if (data == NULL || count > AUTO_ID_PACKET_SIZE)
        return LT48_INVALID_ARGUMENT;
    status = auto_id_flush(session);
    if (status != LT48_OK) return status;
    request[0] = 0x0eu;
    request[1] = (unsigned char)count;
    request[2] = (unsigned char)(count >> 8);
    status = auto_id_exchange(session, request, response);
    if (status != LT48_OK) return status;
    memcpy(data, response, count);
    return LT48_OK;
}

static lt48_status auto_id_read_pin_bits(
    auto_id_session *session, const lt48_auto_id_eprom_recipe *recipe,
    uint16_t *out_value
) {
    unsigned char request[AUTO_ID_PACKET_SIZE] = {0u};
    unsigned char response[AUTO_ID_PACKET_SIZE];
    uint16_t value = 0u;
    uint8_t bit;
    lt48_status status;
    if (recipe == NULL || out_value == NULL || recipe->data_pin_count == 0u ||
        recipe->data_pin_count > LT48_AUTO_ID_EPROM_MAX_DATA_PINS)
        return LT48_INVALID_ARGUMENT;
    status = auto_id_flush(session);
    if (status != LT48_OK) return status;
    request[0] = 0x09u;
    request[1] = recipe->data_pin_count;
    memcpy(request + 2u, recipe->data_capture_codes,
           recipe->data_pin_count);
    status = auto_id_exchange(session, request, response);
    if (status != LT48_OK) return status;
    if (response[0] == 0u) return LT48_PROTOCOL_ERROR;
    for (bit = 0u; bit < recipe->data_pin_count; ++bit) {
        if (response[(size_t)bit + 2u] != 0u)
            value |= (uint16_t)(UINT16_C(1) << bit);
    }
    *out_value = value;
    return LT48_OK;
}

static lt48_status auto_id_set_power_pin(
    auto_id_session *session, uint8_t pin, uint8_t state
) {
    static const uint8_t pin_power_opcode[48] = {
        0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,
        0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1d,0x1e,
        0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1d,
        0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,
        0x1e,0x1d,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,
        0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e
    };
    lt48_status status;
    if (pin >= sizeof(pin_power_opcode)) return LT48_INVALID_ARGUMENT;
    if (pin_power_opcode[pin] == 0x1du) {
        status = auto_id_q1(session, 0x1du);
        if (status != LT48_OK) return status;
        status = auto_id_q1(session, (uint8_t)(pin * 2u + state));
        if (status != LT48_OK) return status;
    }
    status = auto_id_q1(session, 0x1eu);
    if (status != LT48_OK) return status;
    return auto_id_q1(session, (uint8_t)(pin * 2u + state));
}

static lt48_status auto_id_set_rail_pin(
    auto_id_session *session, unsigned rail, uint8_t pin, uint8_t state
) {
    static const uint8_t rail_opcode[] = {0x1cu,0x20u,0x21u,0x22u};
    lt48_status status;
    if (rail >= sizeof(rail_opcode)) return LT48_INVALID_ARGUMENT;
    status = auto_id_q1(session, rail_opcode[rail]);
    if (status != LT48_OK) return status;
    return auto_id_q1(session, (uint8_t)(pin * 2u + state));
}

static lt48_status auto_id_set_voltage(
    auto_id_session *session, unsigned channel, unsigned centivolts
) {
    static const int calibration[] = {0,13,13,20};
    unsigned char bytes[3];
    int scale;
    int dac;
    if (channel > 3u || centivolts > (channel == 3u ? 1200u : 500u))
        return LT48_SAFETY_REJECTED;
    scale = channel != 0u ? 10 : 20;
    dac = centivolts != 0u ?
          (int)(scale * centivolts / 100u) + calibration[channel] : 0;
    if (dac > 255) return LT48_SAFETY_REJECTED;
    bytes[0] = 0x10u;
    bytes[1] = (unsigned char)channel;
    bytes[2] = (unsigned char)dac;
    if (centivolts != 0u) session->rails_requested = 1;
    return auto_id_write_block(session, bytes, sizeof(bytes));
}

static lt48_status auto_id_delay_us(
    auto_id_session *session, unsigned microseconds
) {
    unsigned char bytes[3];
    unsigned units;
    if (microseconds < 256u) {
        bytes[0] = 0x4eu;
        bytes[1] = (unsigned char)microseconds;
        return auto_id_write_block(session, bytes, 2u);
    }
    units = microseconds / 100u;
    if (units > UINT16_MAX) return LT48_INVALID_ARGUMENT;
    bytes[0] = 0x4fu;
    bytes[1] = (unsigned char)units;
    bytes[2] = (unsigned char)(units >> 8);
    return auto_id_write_block(session, bytes, 3u);
}

static lt48_status auto_id_probe(
    auto_id_session *session, lt48_auto_id_socket_observation *out_socket
) {
    static const unsigned char probe[] = {0x47u,0xaau,0x55u};
    unsigned char raw[12];
    lt48_status status = auto_id_write_block(session, probe, sizeof(probe));
    if (status == LT48_OK)
        status = auto_id_read_inline(session, raw, (uint16_t)sizeof(raw));
    if (status == LT48_OK)
        status = lt48_auto_id_eprom_decode_socket(raw, out_socket);
    return status;
}

static lt48_status auto_id_acquire_signature(
    auto_id_session *session, const lt48_auto_id_eprom_recipe *recipe,
    lt48_auto_id_eprom_execution_result *result
) {
    unsigned attempts = 1u << recipe->address_pattern_pin_count;
    unsigned pattern;
    for (pattern = 0u; pattern < attempts; ++pattern) {
        size_t bit;
        lt48_status status;
        ++result->address_patterns_attempted;
        status = auto_id_set_power_pin(session, recipe->power_pin_a, 1u);
        if (status != LT48_OK) return status;
        status = auto_id_set_power_pin(session, recipe->power_pin_b, 1u);
        if (status != LT48_OK) return status;
        status = auto_id_set_rail_pin(session, 1u, recipe->rail_pin, 1u);
        if (status != LT48_OK) return status;
        status = auto_id_set_rail_pin(session, 2u, recipe->rail_pin, 1u);
        if (status != LT48_OK) return status;
        status = auto_id_set_voltage(
            session, 1u, recipe->operating_vcc_centivolts);
        if (status != LT48_OK) return status;
        status = auto_id_set_voltage(
            session, 2u, recipe->operating_vcc_centivolts);
        if (status != LT48_OK) return status;
        status = auto_id_set_voltage(
            session, 3u, recipe->signature_vpp_centivolts);
        if (status != LT48_OK) return status;
        for (bit = 0u; bit < recipe->address_pattern_pin_count; ++bit) {
            status = auto_id_q2(
                session,
                (uint8_t)(recipe->address_pattern_pins[bit] * 4u +
                          ((pattern >> bit) & 1u))
            );
            if (status != LT48_OK) return status;
        }
        status = auto_id_q2(
            session, (uint8_t)(recipe->fixed_pin * 4u + 1u));
        if (status != LT48_OK) return status;
        status = auto_id_delay_us(session, 1000u);
        if (status != LT48_OK) return status;
        status = auto_id_set_rail_pin(
            session, 3u, recipe->read_enable_pin, 1u);
        if (status != LT48_OK) return status;
        status = auto_id_read_pin_bits(
            session, recipe, &result->manufacturer_id);
        if (status != LT48_OK) return status;
        status = auto_id_q2(
            session, (uint8_t)(recipe->id_select_pin * 4u + 1u));
        if (status != LT48_OK) return status;
        status = auto_id_delay_us(session, 100u);
        if (status != LT48_OK) return status;
        status = auto_id_read_pin_bits(session, recipe, &result->device_id);
        if (status != LT48_OK) return status;
        if (result->manufacturer_id != 0u &&
            result->manufacturer_id != 0xffu) break;
    }
    result->has_signature = 1;
    return LT48_OK;
}

static lt48_status auto_id_finalize(auto_id_session *session) {
    static const unsigned char finalizer[] = {0x49u,0xaau,0x55u};
    session->progress_stage = LT48_EXECUTION_AUTO_ID_FINALIZATION;
    if (session->transport->progress != NULL)
        session->transport->progress(
            session->transport_context, session->progress_stage,
            session->command_transfer_count,
            session->planned_command_transfer_count, 1u, 1u, 1u);
    session->queue_size = 0u;
    if (auto_id_write_block(session, finalizer, sizeof(finalizer)) != LT48_OK)
        return LT48_PROTOCOL_ERROR;
    return auto_id_flush(session);
}

lt48_status lt48_auto_id_eprom_execute(
    const lt48_protocol_transport *transport,
    void *transport_context,
    int acquire_signature,
    int has_explicit_pin_hint,
    uint16_t explicit_pin_hint,
    uint32_t authorization,
    lt48_auto_id_eprom_execution_result *out_result
) {
    auto_id_session session;
    struct timespec operation_started;
    struct timespec operation_finished;
    struct timespec finalizer_started;
    struct timespec finalizer_finished;
    lt48_status status;
    lt48_status finalizer_status;
    if (transport == NULL || transport->write == NULL ||
        transport->read == NULL || out_result == NULL ||
        (acquire_signature != 0 && acquire_signature != 1) ||
        (has_explicit_pin_hint != 0 && has_explicit_pin_hint != 1))
        return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    if (authorization != LT48_AUTO_ID_EPROM_HARDWARE_AUTHORIZATION)
        return LT48_SAFETY_REJECTED;
    if (has_explicit_pin_hint &&
        lt48_auto_id_eprom_recipe_find(explicit_pin_hint,
                                       &out_result->recipe) != LT48_OK)
        return LT48_NOT_FOUND;
    memset(&session, 0, sizeof(session));
    session.transport = transport;
    session.transport_context = transport_context;
    session.progress_stage = LT48_EXECUTION_AUTO_ID_OPERATION;
    out_result->signature_requested = acquire_signature;
    if (acquire_signature && has_explicit_pin_hint) {
        session.planned_command_transfer_count =
            3u + 4u * (1u << out_result->recipe.address_pattern_pin_count);
    } else if (!acquire_signature) {
        session.planned_command_transfer_count = 3u;
    }
    if (transport->progress != NULL)
        transport->progress(
            transport_context, session.progress_stage, 0u,
            session.planned_command_transfer_count, 0u, 0u, 0u);
    operation_started = auto_id_now();
    status = auto_id_probe(&session, &out_result->socket);
    if (status == LT48_OK && has_explicit_pin_hint &&
        out_result->socket.package_pin_count != explicit_pin_hint)
        status = LT48_SAFETY_REJECTED;
    if (status == LT48_OK && acquire_signature) {
        status = lt48_auto_id_eprom_recipe_find(
            out_result->socket.package_pin_count, &out_result->recipe);
        if (status == LT48_OK) {
            session.planned_command_transfer_count =
                3u + 4u *
                (1u << out_result->recipe.address_pattern_pin_count);
            out_result->signature_attempted = 1;
            status = auto_id_acquire_signature(
                &session, &out_result->recipe, out_result);
        }
    }
    operation_finished = auto_id_now();
    out_result->operation_duration_ms = auto_id_elapsed_ms(
        operation_started, operation_finished);
    out_result->operation_status = status;
    out_result->finalizer_attempted = 1;
    finalizer_started = auto_id_now();
    finalizer_status = auto_id_finalize(&session);
    finalizer_finished = auto_id_now();
    out_result->finalizer_duration_ms = auto_id_elapsed_ms(
        finalizer_started, finalizer_finished);
    out_result->total_duration_ms = out_result->operation_duration_ms +
                                    out_result->finalizer_duration_ms;
    out_result->finalizer_status = finalizer_status;
    out_result->command_transfer_count = session.command_transfer_count;
    out_result->rails_may_be_active =
        session.rails_requested && finalizer_status != LT48_OK;
    if (finalizer_status != LT48_OK) return LT48_CLEANUP_FAILED;
    return status;
}
