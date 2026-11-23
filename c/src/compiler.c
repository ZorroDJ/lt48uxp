#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct word_vector {
    uint16_t *items;
    size_t count;
    size_t capacity;
} word_vector;

typedef struct branch_record {
    int kind;
    size_t location;
} branch_record;

typedef struct branch_stack {
    branch_record *items;
    size_t count;
    size_t capacity;
} branch_stack;

struct lt48_compiled_wave {
    uint16_t *words;
    size_t word_count;
    lt48_compiled_expansion *expansions;
    size_t expansion_count;
    lt48_defaulted_logical_pin *defaulted_pins;
    size_t defaulted_pin_count;
    size_t defaulted_pin_capacity;
    uint8_t preserved_buffers[256];
    size_t preserved_buffer_count;
    uint16_t completion_message_index;
    int has_completion_message;
};

static const unsigned char power_codes[48] = {
    0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,
    0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1d,0x1e,
    0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1d,
    0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,
    0x1e,0x1d,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,
    0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e
};

static int append_word(word_vector *vector, uint16_t value) {
    if (vector->count == vector->capacity) {
        size_t capacity = vector->capacity == 0 ? 64 : vector->capacity * 2;
        uint16_t *items;
        if (capacity < vector->capacity || capacity > SIZE_MAX / sizeof(*items)) {
            return 0;
        }
        items = realloc(vector->items, capacity * sizeof(*items));
        if (items == NULL) {
            return 0;
        }
        vector->items = items;
        vector->capacity = capacity;
    }
    vector->items[vector->count++] = value;
    return 1;
}

static int append_words(word_vector *vector, const uint16_t *values, size_t count) {
    for (size_t index = 0; index < count; ++index) {
        if (!append_word(vector, values[index])) {
            return 0;
        }
    }
    return 1;
}

static int patch_word(word_vector *words, size_t index, long long value) {
    if (index >= words->count || value < -0x8000LL || value > 0xffffLL) {
        return 0;
    }
    words->items[index] = (uint16_t)value;
    return 1;
}

static int push_branch(branch_stack *stack, int kind, size_t location) {
    if (stack->count >= stack->capacity) {
        return 0;
    }
    stack->items[stack->count++] = (branch_record){kind, location};
    return 1;
}

static int pop_non_exit_branch(branch_stack *stack, branch_record *out_record) {
    size_t index = stack->count;
    while (index != 0 &&
           (stack->items[index - 1].kind == 7 ||
            stack->items[index - 1].kind == 8)) {
        --index;
    }
    if (index == 0) {
        return 0;
    }
    --index;
    *out_record = stack->items[index];
    memmove(stack->items + index, stack->items + index + 1,
            (stack->count - index - 1) * sizeof(*stack->items));
    --stack->count;
    return 1;
}

static int find_variable_channel(
    const lt48_compile_variable *variables,
    size_t variable_count,
    uint16_t variable_id,
    uint16_t *out_channel
) {
    size_t matches = 0;
    uint16_t channel = 0;
    for (size_t index = 0; index < variable_count; ++index) {
        if (variables[index].variable_id == variable_id) {
            channel = variables[index].channel;
            ++matches;
        }
    }
    if (matches != 1 || channel >= 4) {
        return 0;
    }
    *out_channel = channel;
    return 1;
}

static int append_defaulted_pin(
    lt48_compiled_wave *compiled,
    uint16_t number,
    const lt48_compact_instruction *instruction,
    lt48_defaulted_pin_class classification
) {
    if (compiled->defaulted_pin_count == compiled->defaulted_pin_capacity) {
        size_t capacity = compiled->defaulted_pin_capacity == 0 ? 8 :
                          compiled->defaulted_pin_capacity * 2;
        lt48_defaulted_logical_pin *items;
        if (capacity < compiled->defaulted_pin_capacity ||
            capacity > SIZE_MAX / sizeof(*items)) {
            return 0;
        }
        items = realloc(compiled->defaulted_pins, capacity * sizeof(*items));
        if (items == NULL) {
            return 0;
        }
        compiled->defaulted_pins = items;
        compiled->defaulted_pin_capacity = capacity;
    }
    compiled->defaulted_pins[compiled->defaulted_pin_count++] =
        (lt48_defaulted_logical_pin){
            number, instruction->offset, instruction->opcode, classification
        };
    return 1;
}

static lt48_status resolve_logical_pin(
    const lt48_pin_map *pin_map,
    uint16_t number,
    lt48_pin_fallback_policy policy,
    lt48_compiled_wave *compiled,
    const lt48_compact_instruction *instruction,
    uint16_t *out_pin
) {
    lt48_logical_pin pin;
    lt48_status status = lt48_pin_map_find_logical(pin_map, number, &pin);
    if (status == LT48_OK) {
        *out_pin = pin.engine_pin;
        return LT48_OK;
    }
    if (policy == LT48_PIN_FALLBACK_STRICT) {
        return LT48_COMPILE_ERROR;
    }
    {
        lt48_defaulted_pin_class classification =
            ((instruction->opcode == 0x11u && instruction->operand_size >= 2u &&
              instruction->operands[1] <= 2u) ||
             (policy == LT48_PIN_FALLBACK_RESIDENT_NOOP_WITH_RAMP &&
              instruction->opcode == 0x27u && instruction->operand_size == 4u)) ?
                LT48_DEFAULTED_PIN_RESIDENT_NOOP :
                LT48_DEFAULTED_PIN_UNRESOLVED;
        if(policy==LT48_PIN_FALLBACK_SOURCE_ZERO_STROBE){
            if(number!=21u || instruction->operand_size!=2u ||
               !((instruction->opcode==0x11u&&instruction->operands[1]==0u)||
                 (instruction->opcode==0x2bu&&instruction->operands[1]==1u)))return LT48_COMPILE_ERROR;
            classification=LT48_DEFAULTED_PIN_SOURCE_ZERO_STROBE;
        }
        if(policy==LT48_PIN_FALLBACK_COP_RESET){
            if(number!=1u||instruction->opcode!=0x04u||instruction->offset!=3u||instruction->operand_size!=3u||instruction->operands[1]!=5u||instruction->operands[2]!=2u)return LT48_COMPILE_ERROR;
            classification=LT48_DEFAULTED_PIN_SOURCE_ZERO_SUPPLY;
        }
        if(policy==LT48_PIN_FALLBACK_PEEL_SUPPLY){
            if(number!=13u || instruction->opcode!=0x03u || instruction->operand_size!=2u || instruction->offset!=48u)return LT48_COMPILE_ERROR;
            classification=LT48_DEFAULTED_PIN_SOURCE_ZERO_SUPPLY;
        }
        if(policy==LT48_PIN_FALLBACK_SOURCE_ZERO_SUPPLY){
            if(number!=17u || !((instruction->opcode==0x03u && instruction->operand_size==2u) ||
                (instruction->opcode==0x04u && instruction->operand_size==3u)))return LT48_COMPILE_ERROR;
            classification=LT48_DEFAULTED_PIN_SOURCE_ZERO_SUPPLY;
        }
        if ((policy == LT48_PIN_FALLBACK_RESIDENT_NOOP ||
             policy == LT48_PIN_FALLBACK_RESIDENT_NOOP_WITH_RAMP) &&
            classification != LT48_DEFAULTED_PIN_RESIDENT_NOOP) {
            return LT48_COMPILE_ERROR;
        }
        if (!append_defaulted_pin(
                compiled, number, instruction, classification)) {
            return LT48_NO_MEMORY;
        }
    }
    *out_pin = 0;
    return LT48_OK;
}

static int emit_pin_channel(word_vector *words, uint16_t pin, uint16_t channel) {
    static const uint16_t channel_codes[] = {0x1c, 0x20, 0x21, 0x22};
    uint16_t values[9];
    if (channel >= sizeof(channel_codes) / sizeof(channel_codes[0])) {
        return 0;
    }
    values[0] = 0;
    values[1] = pin + 1;
    values[2] = 1;
    values[3] = channel_codes[channel];
    values[4] = 1;
    values[5] = (pin >> 1) + 1;
    values[6] = 0;
    values[7] = pin + 2;
    values[8] = 0x4f;
    return append_words(words, values, 9);
}

static int emit_pin_offset(
    word_vector *words, uint16_t pin, uint16_t channel, uint16_t offset
) {
    static const uint16_t channel_codes[] = {0x1c, 0x20, 0x21, 0x22};
    if (channel >= sizeof(channel_codes) / sizeof(channel_codes[0])) {
        return 0;
    }
    if (offset != 0) {
        uint16_t prefix[] = {0, (uint16_t)(pin + offset)};
        if (!append_words(words, prefix, 2)) {
            return 0;
        }
    }
    {
        uint16_t body[] = {1, channel_codes[channel], 1, (uint16_t)(pin >> 1)};
        if (!append_words(words, body, 4)) {
            return 0;
        }
    }
    if (offset == 0) {
        uint16_t suffix[] = {0, pin};
        if (!append_words(words, suffix, 2)) {
            return 0;
        }
    }
    return 1;
}

static uint16_t raw_u16(const unsigned char *raw) {
    return (uint16_t)((uint16_t)raw[0] | (uint16_t)raw[1] << 8);
}

static void preserve_buffer(lt48_compiled_wave *compiled, uint8_t buffer) {
    size_t position = 0;
    while (position < compiled->preserved_buffer_count &&
           compiled->preserved_buffers[position] < buffer) {
        ++position;
    }
    if (position < compiled->preserved_buffer_count &&
        compiled->preserved_buffers[position] == buffer) {
        return;
    }
    memmove(compiled->preserved_buffers + position + 1,
            compiled->preserved_buffers + position,
            compiled->preserved_buffer_count - position);
    compiled->preserved_buffers[position] = buffer;
    ++compiled->preserved_buffer_count;
}

void lt48_compiled_wave_close(lt48_compiled_wave *compiled) {
    if (compiled == NULL) {
        return;
    }
    free(compiled->words);
    free(compiled->expansions);
    free(compiled->defaulted_pins);
    free(compiled);
}

lt48_status lt48_compact_compile_with_pin_policy(
    const lt48_compact_wave *wave,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    uint16_t selected_group,
    lt48_pin_fallback_policy pin_policy,
    lt48_compiled_wave **out_compiled
) {
    lt48_compiled_wave *compiled;
    word_vector words = {0};
    branch_stack branches = {0};
    size_t instruction_count;
    lt48_status status = LT48_OK;
    (void)selected_group;
    if (wave == NULL || pin_map == NULL || out_compiled == NULL ||
        pin_policy > LT48_PIN_FALLBACK_COP_RESET ||
        (variable_count != 0 && variables == NULL)) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_compiled = NULL;
    instruction_count = lt48_compact_instruction_count(wave);
    compiled = calloc(1, sizeof(*compiled));
    if (compiled == NULL) {
        return LT48_NO_MEMORY;
    }
    compiled->expansions = calloc(instruction_count == 0 ? 1 : instruction_count,
                                  sizeof(*compiled->expansions));
    branches.items = calloc(instruction_count == 0 ? 1 : instruction_count,
                            sizeof(*branches.items));
    branches.capacity = instruction_count;
    if (compiled->expansions == NULL || branches.items == NULL) {
        free(branches.items);
        lt48_compiled_wave_close(compiled);
        return LT48_NO_MEMORY;
    }
    for (size_t index = 0; index < instruction_count; ++index) {
        lt48_compact_instruction instruction;
        const unsigned char *raw;
        size_t first = words.count;
        uint16_t pin;
        uint16_t pin2;
        uint16_t channel;
        lt48_pin_group group;
        status = lt48_compact_get_instruction(wave, index, &instruction);
        if (status != LT48_OK) {
            break;
        }
        raw = instruction.operands;
#define EMIT(...) do { \
    const uint16_t values_[] = {__VA_ARGS__}; \
    if (!append_words(&words, values_, sizeof(values_) / sizeof(values_[0]))) { \
        status = LT48_NO_MEMORY; \
    } \
} while (0)
        switch (instruction.opcode) {
        case 0x00:
            EMIT(2, raw[0], 0x4f);
            break;
        case 0x01:
            EMIT(1, 0x10, 1, raw[0], 1, raw[1], 0x4f);
            break;
        case 0x02:
            EMIT(3, raw[0], raw[1], 0x4f);
            break;
        case 0x03:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK &&
                !find_variable_channel(variables, variable_count, raw[1], &channel)) {
                status = LT48_COMPILE_ERROR;
            }
            if (status == LT48_OK && !emit_pin_channel(&words, pin, channel)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x04:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK &&
                !find_variable_channel(variables, variable_count, raw[1], &channel)) {
                status = LT48_COMPILE_ERROR;
            }
            if (status == LT48_OK &&
                !emit_pin_offset(&words, pin, channel, raw[2])) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x05:
        case 0x06:
            status = lt48_pin_map_find_group(pin_map, raw[0], &group);
            if (status != LT48_OK ||
                !find_variable_channel(variables, variable_count, raw[1], &channel)) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            for (size_t pin_index = 0; pin_index < group.pin_count; ++pin_index) {
                int ok = instruction.opcode == 0x05 ?
                    emit_pin_channel(&words, group.pins[pin_index], channel) :
                    emit_pin_offset(&words, group.pins[pin_index], channel, raw[2]);
                if (!ok) {
                    status = LT48_NO_MEMORY;
                    break;
                }
            }
            break;
        case 0x09:
            EMIT(4, raw[0], raw[1], 0x4f);
            break;
        case 0x0a:
            EMIT(5, raw[0], raw[1], raw[2]);
            break;
        case 0x0b:
        case 0x0c:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            channel = raw[1];
            if (channel >= 4) {
                status = LT48_COMPILE_ERROR;
            }
            if (status == LT48_OK) {
                int ok = instruction.opcode == 0x0b ?
                    emit_pin_channel(&words, pin, channel) :
                    emit_pin_offset(&words, pin, channel, raw[2]);
                if (!ok) {
                    status = LT48_NO_MEMORY;
                }
            }
            break;
        case 0x0d:
        case 0x0e:
            status = lt48_pin_map_find_group(pin_map, raw[0], &group);
            channel = raw[1];
            if (status != LT48_OK || channel >= 4) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            for (size_t pin_index = 0; pin_index < group.pin_count; ++pin_index) {
                int ok;
                if (group.pins[pin_index] == 0xfc) {
                    break;
                }
                ok = instruction.opcode == 0x0d ?
                    emit_pin_channel(&words, group.pins[pin_index], channel) :
                    emit_pin_offset(&words, group.pins[pin_index], channel, raw[2]);
                if (!ok) {
                    status = LT48_NO_MEMORY;
                    break;
                }
            }
            break;
        case 0x0f:
        case 0x10:
            /* Dataman 13.10 00483C10: operands are group, index,
             * channel. The normal-UI Am27C128 ID capture selects only
             * P5 group4 index9, not every address pin. */
            status = lt48_pin_map_find_group(pin_map, raw[0], &group);
            channel = raw[2];
            if (status != LT48_OK || channel >= 4) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            if (raw[1] < group.pin_count && group.pins[raw[1]] != 0xfc) {
                int ok = instruction.opcode == 0x0f ?
                    emit_pin_channel(&words, group.pins[raw[1]], channel) :
                    emit_pin_offset(&words, group.pins[raw[1]], channel, raw[3]);
                if (!ok) {
                    status = LT48_NO_MEMORY;
                }
            }
            break;
        case 0x11:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0, (uint16_t)(pin + raw[1]));
            }
            break;
        case 0x12:
            EMIT(7, raw[0], raw[1]);
            break;
        case 0x13:
            status = lt48_pin_map_find_group(pin_map, raw[0], &group);
            if (status != LT48_OK) {
                status = LT48_COMPILE_ERROR;
            } else if (raw[1] < group.pin_count && group.pins[raw[1]] != 0xfc) {
                EMIT(0, (uint16_t)(group.pins[raw[1]] + raw[2]));
            }
            break;
        case 0x14:
            EMIT(6, raw[0], raw[1]);
            break;
        case 0x15:
            EMIT(8, raw[0], raw[1]);
            break;
        case 0x16:
            EMIT(9, raw[0], raw[1]);
            break;
        case 0x17:
            EMIT(0x0a, raw[0], raw_u16(raw + 1), raw_u16(raw + 3));
            break;
        case 0x18:
            EMIT(0x0b, raw[0], raw[1]);
            break;
        case 0x19:
        case 0x1b:
        case 0x1c:
        case 0x1e:
        case 0x1f:
        case 0x21:
        case 0x22: {
            static const uint16_t runtime_codes[] = {
                0x0c, 0x0e, 0x0f, 0x11, 0x13, 0x15, 0x16
            };
            static const uint8_t compact_codes[] = {
                0x19, 0x1b, 0x1c, 0x1e, 0x1f, 0x21, 0x22
            };
            uint16_t runtime_code = 0;
            for (size_t code = 0; code < sizeof(compact_codes); ++code) {
                if (compact_codes[code] == instruction.opcode) {
                    runtime_code = runtime_codes[code];
                    break;
                }
            }
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                status = resolve_logical_pin(pin_map, raw[1],
                    pin_policy, compiled, &instruction, &pin2);
            }
            if (status == LT48_OK) {
                EMIT(runtime_code, pin, pin2, raw[2], raw[3], raw[4]);
            }
            break;
        }
        case 0x1a:
        case 0x1d:
        case 0x20: {
            uint16_t runtime_code = instruction.opcode == 0x1a ? 0x0d :
                                    instruction.opcode == 0x1d ? 0x10 : 0x14;
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                status = resolve_logical_pin(pin_map, raw[1],
                    pin_policy, compiled, &instruction, &pin2);
            }
            if (status == LT48_OK) {
                EMIT(runtime_code, pin, pin2, raw_u16(raw + 2),
                     raw_u16(raw + 4), raw[6], raw[7]);
            }
            break;
        }
        case 0x24:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                status = resolve_logical_pin(pin_map, raw[1],
                    pin_policy, compiled, &instruction, &pin2);
            }
            if (status == LT48_OK) {
                EMIT(0x18, pin, pin2, raw[2], raw[3], raw[4]);
            }
            break;
        case 0x25:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0x12, pin, raw_u16(raw + 1), raw[3]);
            }
            break;
        case 0x26:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0x19, pin, raw[1], raw[2]);
            }
            break;
        case 0x27:
        case 0x28:
        case 0x29: {
            uint16_t runtime_code = instruction.opcode == 0x27 ? 0x1a :
                                    instruction.opcode == 0x28 ? 0x1b : 0x1c;
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(runtime_code, pin, raw[1], raw[2], raw[3], 0x4f);
            }
            break;
        }
        case 0x2a:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0x1d, pin, raw[1], raw[2], 0x4f);
            }
            break;
        case 0x2b:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(1, 0x18, 1, (uint16_t)(pin + raw[1]));
            }
            break;
        case 0x2c:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0x1e, pin, raw[1], raw[2]);
            }
            break;
        case 0x2d:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0x1f, pin, raw[1], raw[2], raw[3]);
            }
            break;
        case 0x2f:
            EMIT(0x21, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x30:
            EMIT(0x22, raw[0], raw[1]);
            break;
        case 0x31:
            EMIT(0x23, raw[0]);
            break;
        case 0x32:
            EMIT(0x24, raw_u16(raw), raw_u16(raw + 2));
            break;
        case 0x33: {
            status = lt48_pin_map_find_group(pin_map, selected_group, &group);
            if (status != LT48_OK) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            for (size_t pin_index = 0; pin_index < group.pin_count; ++pin_index) {
                size_t power_index = group.pins[pin_index] >> 2;
                if (power_index >= sizeof(power_codes)) {
                    status = LT48_COMPILE_ERROR;
                    break;
                }
                if (power_codes[power_index] == 0x1d) {
                    uint16_t values[] = {
                        1,0x1d,1,(uint16_t)(power_index * 2 + raw[0])
                    };
                    if (!append_words(&words, values, 4)) {
                        status = LT48_NO_MEMORY;
                        break;
                    }
                }
                {
                    uint16_t values[] = {
                        1,0x1e,1,(uint16_t)(power_index * 2 + raw[0])
                    };
                    if (!append_words(&words, values, 4)) {
                        status = LT48_NO_MEMORY;
                        break;
                    }
                }
            }
            if (status == LT48_OK && !append_word(&words, 0x4f)) {
                status = LT48_NO_MEMORY;
            }
            break;
        }
        case 0x34:
            EMIT(1, 0x11, 1, raw[0]);
            break;
        case 0x36:
            EMIT(0x50);
            break;
        case 0x37:
            EMIT(0x51);
            break;
        case 0x39:
            EMIT(1, raw[0]);
            break;
        case 0x3b:
            EMIT(0, raw[0]);
            break;
        case 0x3d:
            EMIT(0x25, raw[0]);
            break;
        case 0x3e:
            EMIT(0x26, raw[0], raw[1]);
            break;
        case 0x3f:
            EMIT(0x27, raw[0], raw[1]);
            break;
        case 0x40:
            if (raw[1] != instruction.operand_size - 2) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            EMIT(0x28, raw[0], raw[1]);
            for (size_t offset = 2; offset < instruction.operand_size; offset += 2) {
                uint16_t value = raw[offset];
                if (offset + 1 < instruction.operand_size) {
                    value |= (uint16_t)raw[offset + 1] << 8;
                }
                if (!append_word(&words, value)) {
                    status = LT48_NO_MEMORY;
                    break;
                }
            }
            break;
        case 0x41:
            EMIT(0x29, raw[0], raw[1], raw[2]);
            break;
        case 0x43:
        case 0x44: {
            uint16_t short_code = instruction.opcode == 0x43 ? 0x31 : 0x32;
            uint16_t long_code = instruction.opcode == 0x43 ? 0x2b : 0x2c;
            if (raw[3] == 1) {
                EMIT(short_code, raw[0], raw[1], raw[2]);
            } else {
                EMIT(long_code, raw[0], raw[1], raw[2], raw[3]);
            }
            break;
        }
        case 0x45:
            if (raw[4] == 1) {
                EMIT(0x33, raw[0], raw[1], raw[2], raw[3]);
            } else {
                EMIT(0x2d, raw[0], raw[1], raw[2], raw[3], raw[4]);
            }
            break;
        case 0x46:
            EMIT(0x2e, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x47:
            EMIT(0x2f, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x48:
            EMIT(0x30, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x49:
            EMIT(0x34, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x4a:
            EMIT(0x35, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x4b:
            EMIT(0x36, raw[0], raw[1], raw_u16(raw + 2),
                 raw_u16(raw + 4), raw[6]);
            break;
        case 0x4c:
            EMIT(0x37, raw[0], raw[1], raw[2]);
            break;
        case 0x4d:
            EMIT(0x38, raw[0], raw[1], raw_u16(raw + 2), raw_u16(raw + 4));
            break;
        case 0x4e:
            EMIT(0x39, raw[0]);
            break;
        case 0x4f:
            EMIT(0x3a, raw[0]);
            break;
        case 0x50:
            EMIT(0x3b, raw[0], raw[1]);
            break;
        case 0x51:
            EMIT(0x3c, raw[0], raw[1]);
            break;
        case 0x52:
            EMIT(0x3d, raw[0], raw[1], raw[2]);
            break;
        case 0x53:
            EMIT(0x3e, raw[0]);
            break;
        case 0x54:
            EMIT(0x3f, raw[0], raw[1], raw[2]);
            break;
        case 0x55:
            EMIT(0x40, raw[0]);
            break;
        case 0x56:
            EMIT(0x41, raw[0], raw[1], raw[2]);
            break;
        case 0x57:
            EMIT(0x42, raw[0], raw[1]);
            break;
        case 0x58:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0x43, pin, raw[1], raw_u16(raw + 2), raw[4]);
            }
            break;
        case 0x5a:
            EMIT(0x45, raw[0], raw[1], raw_u16(raw + 2), raw_u16(raw + 4),
                 raw_u16(raw + 6), raw[8]);
            break;
        case 0x5b:
            EMIT(0x45, raw[0], raw[1], raw_u16(raw + 2), raw_u16(raw + 4),
                 raw[6], raw[7]);
            break;
        case 0x5c:
            EMIT(0x47);
            break;
        case 0x5d:
            EMIT(0x4a, raw[0], 0, raw[1], raw[2], 0, 0);
            if (status == LT48_OK &&
                !push_branch(&branches, 0, words.count - 1)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x5e:
            EMIT(0x4b, raw[0], 0, raw[1], raw_u16(raw + 2),
                 raw_u16(raw + 4), 0);
            if (status == LT48_OK &&
                !push_branch(&branches, 0, words.count - 1)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x5f: {
            branch_record previous;
            if (!pop_non_exit_branch(&branches, &previous) ||
                (previous.kind != 0 && previous.kind != 1)) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            EMIT(0x4c, 0);
            if (status == LT48_OK &&
                (!push_branch(&branches, 1, words.count - 1) ||
                 !patch_word(&words, previous.location,
                             (long long)words.count -
                             (long long)previous.location))) {
                status = LT48_COMPILE_ERROR;
            }
            break;
        }
        case 0x60: {
            branch_record previous;
            if (!pop_non_exit_branch(&branches, &previous) ||
                (previous.kind != 0 && previous.kind != 1) ||
                !patch_word(&words, previous.location,
                            (long long)words.count -
                            (long long)previous.location)) {
                status = LT48_COMPILE_ERROR;
            }
            break;
        }
        case 0x61:
            EMIT(0x4a, raw[0], 0, raw[1], raw[2], 0, 0);
            if (status == LT48_OK &&
                !push_branch(&branches, 3, words.count - 1)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x62:
            EMIT(0x4b, raw[0], 0, raw[1], raw_u16(raw + 2),
                 raw_u16(raw + 4), 0);
            if (status == LT48_OK &&
                !push_branch(&branches, 3, words.count - 1)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x63: {
            branch_record *exits = calloc(
                branches.count == 0 ? 1 : branches.count, sizeof(*exits)
            );
            size_t exit_count = 0;
            size_t loop_location;
            if (exits == NULL) {
                status = LT48_NO_MEMORY;
                break;
            }
            while (branches.count != 0 &&
                   branches.items[branches.count - 1].kind != 3) {
                branch_record record = branches.items[--branches.count];
                if (record.kind == 8) {
                    exits[exit_count++] = record;
                } else if (!patch_word(&words, record.location,
                        (long long)words.count - (long long)record.location + 2)) {
                    status = LT48_COMPILE_ERROR;
                    break;
                }
            }
            if (status == LT48_OK && branches.count == 0) {
                status = LT48_COMPILE_ERROR;
            }
            if (status == LT48_OK) {
                loop_location = branches.items[--branches.count].location;
                for (size_t exit_index = exit_count; exit_index != 0;
                     --exit_index) {
                    size_t location = exits[exit_index - 1].location;
                    if (!patch_word(&words, location,
                            (long long)loop_location - (long long)location - 6)) {
                        status = LT48_COMPILE_ERROR;
                        break;
                    }
                }
                if (status == LT48_OK) {
                    size_t backward_location;
                    EMIT(0x4c, 0);
                    backward_location = words.count - 1;
                    if (status == LT48_OK &&
                        (!patch_word(&words, backward_location,
                            (long long)loop_location -
                            (long long)backward_location - 6) ||
                         !patch_word(&words, loop_location,
                            (long long)words.count -
                            (long long)loop_location))) {
                        status = LT48_COMPILE_ERROR;
                    }
                }
            }
            free(exits);
            break;
        }
        case 0x64:
            if (!push_branch(&branches, 5, words.count)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x66: {
            size_t marker = branches.count;
            size_t region_location;
            while (marker != 0 && branches.items[marker - 1].kind != 5) {
                --marker;
            }
            if (marker == 0) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            region_location = branches.items[marker - 1].location;
            for (size_t deferred = marker; deferred < branches.count; ++deferred) {
                branch_record record = branches.items[deferred];
                long long displacement =
                    (long long)words.count - (long long)record.location;
                if (record.kind == 7) {
                    displacement += 7;
                }
                if (!patch_word(&words, record.location, displacement)) {
                    status = LT48_COMPILE_ERROR;
                    break;
                }
            }
            branches.count = marker - 1;
            if (status == LT48_OK) {
                EMIT(0x4b, raw[0], 0, raw[1], raw_u16(raw + 2),
                     raw_u16(raw + 4));
            }
            if (status == LT48_OK && !append_word(&words,
                    (uint16_t)((long long)region_location -
                               (long long)words.count))) {
                status = LT48_NO_MEMORY;
            }
            break;
        }
        case 0x67:
        case 0x68:
            EMIT(0x4c, 0);
            if (status == LT48_OK &&
                !push_branch(&branches, instruction.opcode == 0x67 ? 7 : 8,
                             words.count - 1)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x69:
            EMIT(0x4d, raw[0]);
            break;
        case 0x6b:
            EMIT(0x52, raw[0]);
            break;
        case 0x6c:
            EMIT(0x53, raw[0], raw[1]);
            break;
        case 0x6e:
            EMIT(0x55, 1, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x6f:
            EMIT(0x56, 1, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x70:
            EMIT(0x57, 1, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x71:
            EMIT(0x58, 1, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x72:
            EMIT(0x5a, 1, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x73:
            EMIT(0x5c, 1, raw[0], raw[1], raw[2], raw[3]);
            break;
        case 0x74:
            EMIT(0x55, 0, raw[0], raw[1], raw[2]);
            break;
        case 0x75:
            EMIT(0x56, 0, raw[0], raw[1], raw[2]);
            break;
        case 0x78:
            EMIT(0x5a, 0, raw[0], raw[1], raw[2]);
            break;
        case 0x7a:
            EMIT(0x5d, raw[0], raw[1], 0);
            if (status == LT48_OK &&
                !push_branch(&branches, 9, words.count - 1)) {
                status = LT48_NO_MEMORY;
            }
            break;
        case 0x7b: {
            branch_record previous;
            EMIT(0x5e);
            if (status == LT48_OK &&
                (!pop_non_exit_branch(&branches, &previous) ||
                 previous.kind != 9 ||
                 !patch_word(&words, previous.location,
                     (long long)words.count - (long long)previous.location))) {
                status = LT48_COMPILE_ERROR;
            }
            break;
        }
        case 0x7c:
            EMIT(0x5f, raw[0]);
            break;
        case 0x7d:
            EMIT(0x61, raw[0], raw[1]);
            break;
        case 0x7e:
            EMIT(0x60, raw[0], raw[1]);
            break;
        case 0x7f:
            EMIT(1, 0x56, 1, raw[0], 1, raw[1]);
            break;
        case 0x80:
            EMIT(0x62, raw[0], raw[1]);
            break;
        case 0x81:
        case 0x82:
            break;
        case 0x83:
            EMIT(0x65, raw[0], raw[1], raw[2]);
            break;
        case 0x84:
            EMIT(0x66, raw_u16(raw), raw_u16(raw + 2), raw[4], raw[5]);
            break;
        case 0x85:
            EMIT(0x67, raw[0], raw[1], raw[2]);
            break;
        case 0x86:
            EMIT(0x68, raw_u16(raw), raw_u16(raw + 2), raw[4], raw[5]);
            break;
        case 0x89:
            EMIT(0x6b, raw[0], raw[1], raw[2]);
            break;
        case 0x8a:
            EMIT(0x6c, raw_u16(raw), raw_u16(raw + 2), raw[4]);
            break;
        case 0x8c:
            EMIT(0x6e, raw_u16(raw), raw_u16(raw + 2), raw[4]);
            break;
        case 0x8d:
            EMIT(0x6f, raw[0]);
            break;
        case 0x8e:
            preserve_buffer(compiled, raw[0]);
            break;
        case 0x8f:
            EMIT(1, 0x0f, 1, raw[0]);
            break;
        case 0x90:
            EMIT(0x70, raw[0], raw[1]);
            break;
        case 0x91:
            status = resolve_logical_pin(pin_map, raw[0],
                pin_policy, compiled, &instruction, &pin);
            if (status == LT48_OK) {
                EMIT(0x71, pin, raw[1]);
            }
            break;
        case 0x92:
            EMIT(0x72, raw_u16(raw));
            break;
        case 0x93:
            compiled->completion_message_index = raw_u16(raw);
            compiled->has_completion_message = 1;
            break;
        case 0x94:
            EMIT(0x73, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x95:
            EMIT(0x74, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x96:
            EMIT(0x75, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x97:
            status = lt48_pin_map_find_group(pin_map, raw[0], &group);
            if (status != LT48_OK) {
                status = LT48_COMPILE_ERROR;
                break;
            }
            for (size_t pin_index = 0; pin_index < group.pin_count; ++pin_index) {
                size_t power_index = group.pins[pin_index] >> 2;
                if (power_index >= sizeof(power_codes)) {
                    status = LT48_COMPILE_ERROR;
                    break;
                }
                if (power_codes[power_index] == 0x1d) {
                    uint16_t values[] = {
                        1,0x1d,1,(uint16_t)(power_index * 2 + raw[1])
                    };
                    if (!append_words(&words, values, 4)) {
                        status = LT48_NO_MEMORY;
                        break;
                    }
                }
                {
                    uint16_t values[] = {
                        1,0x1e,1,(uint16_t)(power_index * 2 + raw[1])
                    };
                    if (!append_words(&words, values, 4)) {
                        status = LT48_NO_MEMORY;
                        break;
                    }
                }
            }
            break;
        case 0x98:
            EMIT(0x76, raw[0]);
            break;
        case 0x99:
            EMIT(0x77, raw[0], raw[1], raw[2], raw[3], raw[4]);
            break;
        case 0x9a:
            EMIT(0x78, raw[0], raw[1]);
            break;
        case 0x9b:
            EMIT(0x79, raw[0], raw[1]);
            break;
        case 0x9c:
            EMIT(0x7a, raw[0]);
            break;
        case 0xa0:
            EMIT(0x7e, raw[0], raw_u16(raw + 1), raw[3]);
            break;
        case 0xa1:
            EMIT(0x7f, raw[0], raw[1], raw[2]);
            break;
        case 0xa2:
            EMIT(0x80, raw[0], raw_u16(raw + 1), raw[3]);
            break;
        case 0xa3:
            EMIT(0x81, raw[0], raw[1], raw[2]);
            break;
        case 0xa4:
            EMIT(0x82, raw[0], raw[1], raw[2], raw[3]);
            break;
        default:
            status = LT48_UNSUPPORTED;
            break;
        }
#undef EMIT
        if (status != LT48_OK) {
            break;
        }
        compiled->expansions[index] = (lt48_compiled_expansion){
            instruction.offset, instruction.opcode, instruction.compiler_handler,
            first + 1, words.count - first
        };
        ++compiled->expansion_count;
    }
    if (status == LT48_OK && branches.count != 0) {
        status = LT48_COMPILE_ERROR;
    }
    if (status == LT48_OK && words.count > UINT16_MAX) {
        status = LT48_COMPILE_ERROR;
    }
    if (status != LT48_OK) {
        free(branches.items);
        free(words.items);
        lt48_compiled_wave_close(compiled);
        return status;
    }
    compiled->words = malloc((words.count + 1) * sizeof(*compiled->words));
    if (compiled->words == NULL) {
        free(branches.items);
        free(words.items);
        lt48_compiled_wave_close(compiled);
        return LT48_NO_MEMORY;
    }
    compiled->words[0] = (uint16_t)words.count;
    memcpy(compiled->words + 1, words.items, words.count * sizeof(*words.items));
    compiled->word_count = words.count + 1;
    free(words.items);
    free(branches.items);
    *out_compiled = compiled;
    return LT48_OK;
}

lt48_status lt48_compact_compile(
    const lt48_compact_wave *wave,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    uint16_t selected_group,
    int analysis_default_unmapped_pins,
    lt48_compiled_wave **out_compiled
) {
    return lt48_compact_compile_with_pin_policy(
        wave, pin_map, variables, variable_count, selected_group,
        analysis_default_unmapped_pins ? LT48_PIN_FALLBACK_ANALYSIS_ALL :
                                         LT48_PIN_FALLBACK_STRICT,
        out_compiled
    );
}

const uint16_t *lt48_compiled_wave_words(
    const lt48_compiled_wave *compiled, size_t *out_word_count
) {
    if (out_word_count != NULL) {
        *out_word_count = compiled == NULL ? 0 : compiled->word_count;
    }
    return compiled == NULL ? NULL : compiled->words;
}

size_t lt48_compiled_wave_expansion_count(const lt48_compiled_wave *compiled) {
    return compiled == NULL ? 0 : compiled->expansion_count;
}

lt48_status lt48_compiled_wave_get_expansion(
    const lt48_compiled_wave *compiled, size_t index,
    lt48_compiled_expansion *out_expansion
) {
    if (compiled == NULL || out_expansion == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= compiled->expansion_count) {
        return LT48_NOT_FOUND;
    }
    *out_expansion = compiled->expansions[index];
    return LT48_OK;
}

size_t lt48_compiled_wave_defaulted_pin_count(
    const lt48_compiled_wave *compiled
) {
    return compiled == NULL ? 0 : compiled->defaulted_pin_count;
}

lt48_status lt48_compiled_wave_get_defaulted_pin(
    const lt48_compiled_wave *compiled, size_t index,
    lt48_defaulted_logical_pin *out_pin
) {
    if (compiled == NULL || out_pin == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= compiled->defaulted_pin_count) {
        return LT48_NOT_FOUND;
    }
    *out_pin = compiled->defaulted_pins[index];
    return LT48_OK;
}

size_t lt48_compiled_wave_preserved_buffer_count(
    const lt48_compiled_wave *compiled
) {
    return compiled == NULL ? 0 : compiled->preserved_buffer_count;
}

lt48_status lt48_compiled_wave_get_preserved_buffer(
    const lt48_compiled_wave *compiled, size_t index, uint8_t *out_buffer
) {
    if (compiled == NULL || out_buffer == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= compiled->preserved_buffer_count) {
        return LT48_NOT_FOUND;
    }
    *out_buffer = compiled->preserved_buffers[index];
    return LT48_OK;
}

lt48_status lt48_compiled_wave_completion_message(
    const lt48_compiled_wave *compiled, uint16_t *out_message_index
) {
    if (compiled == NULL || out_message_index == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (!compiled->has_completion_message) {
        return LT48_NOT_FOUND;
    }
    *out_message_index = compiled->completion_message_index;
    return LT48_OK;
}

lt48_status lt48_compiled_map_create(
    const unsigned char *data, size_t size, lt48_compiled_wave **out_map
) {
    if (!data || !out_map) return LT48_INVALID_ARGUMENT;
    *out_map = NULL;
    if (size < 4u || size % 2u ||
        ((size_t)data[0] | (size_t)data[1] << 8u) != size - 2u)
        return LT48_FORMAT_ERROR;
    size_t count = (size_t)data[2] | (size_t)data[3] << 8u;
    if (count > 256u || size != 4u + count * 2u) return LT48_FORMAT_ERROR;
    lt48_compiled_wave *map = calloc(1u, sizeof(*map));
    if (!map) return LT48_NO_MEMORY;
    map->word_count = count + 1u;
    map->words = calloc(map->word_count, sizeof(*map->words));
    if (!map->words) { free(map); return LT48_NO_MEMORY; }
    for (size_t i = 0u; i <= count; ++i)
        map->words[i] = (uint16_t)data[2u + 2u*i] | (uint16_t)data[3u + 2u*i] << 8u;
    *out_map = map;
    return LT48_OK;
}

lt48_status lt48_compiled_control_table_create(
    const unsigned char *data, size_t size, lt48_compiled_wave **out_table
) {
    if (!data || !out_table) return LT48_INVALID_ARGUMENT;
    *out_table = NULL;
    if (size < 4u || ((size_t)data[0] | (size_t)data[1] << 8u) != size - 2u ||
        !data[2] || !data[3] || size != 4u + (size_t)data[2] * data[3])
        return LT48_FORMAT_ERROR;
    for (size_t i = 4u; i < size; ++i)
        if (data[i] > 3u) return LT48_UNSUPPORTED;
    lt48_compiled_wave *table = calloc(1u, sizeof(*table));
    if (!table) return LT48_NO_MEMORY;
    table->word_count = size - 1u;
    table->words = calloc(table->word_count, sizeof(*table->words));
    if (!table->words) { free(table); return LT48_NO_MEMORY; }
    table->words[0] = (uint16_t)(size - 2u);
    for (size_t i = 2u; i < size; ++i) table->words[i - 1u] = data[i];
    *out_table = table;
    return LT48_OK;
}

lt48_status lt48_compiled_host_words_internal(const uint16_t *words, size_t count, lt48_compiled_wave **out) {
    if(!words || !out || count<2 || count>UINT16_MAX || words[0]!=count-1)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_compiled_wave *w=calloc(1,sizeof(*w));if(!w)return LT48_NO_MEMORY;
    w->words=malloc(count*sizeof(*words));if(!w->words){free(w);return LT48_NO_MEMORY;}
    memcpy(w->words,words,count*sizeof(*words));w->word_count=count;*out=w;return LT48_OK;
}

/* Type-1 W resources: 16-bit byte length followed directly by LE words. */
lt48_status lt48_compiled_word_table_internal(const unsigned char *data,size_t size,
 lt48_compiled_wave **out) {
 if(!data||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(size<4||size%2||((size_t)data[0]|(size_t)data[1]<<8)!=size-2)return LT48_FORMAT_ERROR;
 size_t count=(size-2)/2;
 lt48_compiled_wave *table=calloc(1,sizeof(*table));if(!table)return LT48_NO_MEMORY;
 table->words=calloc(count+1,sizeof(*table->words));if(!table->words){free(table);return LT48_NO_MEMORY;}
 table->word_count=count+1;table->words[0]=(uint16_t)count;
 for(size_t i=0;i<count;i++)table->words[i+1]=(uint16_t)data[2+2*i]|(uint16_t)data[3+2*i]<<8;
 *out=table;return LT48_OK;
}
