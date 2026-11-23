#include <labtool48/labtool48.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct compact_instruction_owned {
    size_t offset;
    size_t operand_offset;
    size_t operand_size;
    uint8_t opcode;
} compact_instruction_owned;

struct lt48_compact_wave {
    unsigned char *data;
    size_t size;
    compact_instruction_owned *instructions;
    size_t instruction_count;
};

static const int8_t operand_widths[] = {
    1,2,2,2,3,2,3,3,4,2,3,2,3,2,3,3,4,2,2,3,2,2,2,5,2,5,8,5,5,8,5,5,
    8,5,5,8,5,4,3,4,4,4,3,2,3,4,3,4,2,1,4,1,1,1,0,0,0,1,1,1,1,1,2,2,
    -1,3,7,4,4,5,5,5,5,5,4,7,3,6,1,1,2,2,3,1,3,1,3,2,5,4,9,8,0,3,6,0,
    0,3,6,0,0,3,6,0,0,1,1,1,2,0,4,4,4,4,4,4,3,3,3,3,3,3,2,0,1,2,2,2,
    2,1,2,3,6,3,6,1,1,3,5,3,5,1,1,1,2,2,2,2,5,5,5,2,1,5,2,2,1,1,1,1,
    4,3,4,3,4
};

static const uint32_t compiler_handlers[] = {
    UINT32_C(0x44b4c0),UINT32_C(0x44b560),UINT32_C(0x44b500),UINT32_C(0x44b740),UINT32_C(0x44b790),UINT32_C(0x44b7f0),UINT32_C(0x44b8c0),UINT32_C(0x44b9a0),
    UINT32_C(0x44ba10),UINT32_C(0x44b860),UINT32_C(0x44b940),UINT32_C(0x44ba90),UINT32_C(0x44bad0),UINT32_C(0x44bb20),UINT32_C(0x44bb80),UINT32_C(0x44bbf0),
    UINT32_C(0x44bc50),UINT32_C(0x44bcc0),UINT32_C(0x44bd10),UINT32_C(0x44be00),UINT32_C(0x44bd60),UINT32_C(0x44be70),UINT32_C(0x44bec0),UINT32_C(0x44bf10),
    UINT32_C(0x44bf60),UINT32_C(0x44bff0),UINT32_C(0x44c0a0),UINT32_C(0x44c150),UINT32_C(0x44c200),UINT32_C(0x44c2b0),UINT32_C(0x44c360),UINT32_C(0x44c480),
    UINT32_C(0x44c530),UINT32_C(0x44c5e0),UINT32_C(0x44c690),UINT32_C(0x44c740),UINT32_C(0x44c7f0),UINT32_C(0x44c410),UINT32_C(0x44c8a0),UINT32_C(0x44c910),
    UINT32_C(0x44c9a0),UINT32_C(0x44ca30),UINT32_C(0x44cac0),UINT32_C(0x44cb40),UINT32_C(0x44cbb0),UINT32_C(0x44cc20),UINT32_C(0x44cca0),UINT32_C(0x44cd10),
    UINT32_C(0x44cd90),UINT32_C(0x44cde0),UINT32_C(0x44ce20),UINT32_C(0x44cea0),UINT32_C(0x44cfa0),UINT32_C(0x44d040),UINT32_C(0x44d090),UINT32_C(0x44d0b0),
    UINT32_C(0x44d0d0),UINT32_C(0x44d130),UINT32_C(0x44d170),UINT32_C(0x44d1c0),UINT32_C(0x44d200),UINT32_C(0x44d250),UINT32_C(0x44d290),UINT32_C(0x44d2e0),
    UINT32_C(0x44d330),UINT32_C(0x44d3b0),UINT32_C(0x44d410),UINT32_C(0x44d490),UINT32_C(0x44d510),UINT32_C(0x44d590),UINT32_C(0x44d6c0),UINT32_C(0x44d630),
    UINT32_C(0x44d750),UINT32_C(0x44d870),UINT32_C(0x44dab0),UINT32_C(0x44db30),UINT32_C(0x44dbb0),UINT32_C(0x44dc10),UINT32_C(0x44dc70),UINT32_C(0x44dcb0),
    UINT32_C(0x44dcf0),UINT32_C(0x44dd40),UINT32_C(0x44dd90),UINT32_C(0x44ddf0),UINT32_C(0x44dea0),UINT32_C(0x44df00),UINT32_C(0x44df40),UINT32_C(0x44dfa0),
    UINT32_C(0x44dff0),UINT32_C(0x44e080),UINT32_C(0x44e100),UINT32_C(0x44e190),UINT32_C(0x44e220),UINT32_C(0x44e240),UINT32_C(0x44e2d0),UINT32_C(0x44e3d0),
    UINT32_C(0x44e430),UINT32_C(0x44e460),UINT32_C(0x44e4f0),UINT32_C(0x44e580),UINT32_C(0x44e650),UINT32_C(0x44e710),UINT32_C(0x44e7a0),UINT32_C(0x44e830),
    UINT32_C(0x44e870),UINT32_C(0x44e930),UINT32_C(0x44e970),UINT32_C(0x44e9b0),UINT32_C(0x44e9f0),UINT32_C(0x44eb10),UINT32_C(0x44ec90),UINT32_C(0x44ed20),
    UINT32_C(0x44edb0),UINT32_C(0x44ee40),UINT32_C(0x44eed0),UINT32_C(0x44ef60),UINT32_C(0x44eff0),UINT32_C(0x44f060),UINT32_C(0x44f0d0),UINT32_C(0x44f140),
    UINT32_C(0x44f1b0),UINT32_C(0x44f220),UINT32_C(0x44f290),UINT32_C(0x44f300),UINT32_C(0x44bfb0),UINT32_C(0x44f340),UINT32_C(0x44f390),UINT32_C(0x44f3e0),
    UINT32_C(0x44bdb0),UINT32_C(0x44f460),UINT32_C(0x44f470),UINT32_C(0x44f480),UINT32_C(0x44f4e0),UINT32_C(0x44f540),UINT32_C(0x44f5a0),UINT32_C(0x44f600),
    UINT32_C(0x44f640),UINT32_C(0x44f680),UINT32_C(0x44f6e0),UINT32_C(0x44f750),UINT32_C(0x44f7b0),UINT32_C(0x44f820),UINT32_C(0x44f860),UINT32_C(0x44cff0),
    UINT32_C(0x44ea40),UINT32_C(0x44f880),UINT32_C(0x44de30),UINT32_C(0x44de70),UINT32_C(0x44d900),UINT32_C(0x44d990),UINT32_C(0x44da20),UINT32_C(0x44f8e0),
    UINT32_C(0x44ce60),UINT32_C(0x44d7e0),UINT32_C(0x44f9d0),UINT32_C(0x44f9d0),UINT32_C(0x44b480),UINT32_C(0x44e8f0),UINT32_C(0x44e8b0),UINT32_C(0x44e8f0),
    UINT32_C(0x44ebe0),UINT32_C(0x44ebe0),UINT32_C(0x44eb30),UINT32_C(0x44eb30),UINT32_C(0x44ea90)
};

_Static_assert(sizeof(operand_widths) / sizeof(operand_widths[0]) == 0xa5,
               "compact operand table must cover 0x00..0xa4");
_Static_assert(sizeof(compiler_handlers) / sizeof(compiler_handlers[0]) == 0xa5,
               "compact handler table must cover 0x00..0xa4");

void lt48_compact_wave_close(lt48_compact_wave *wave) {
    if (wave == NULL) {
        return;
    }
    free(wave->instructions);
    free(wave->data);
    free(wave);
}

lt48_status lt48_compact_decode(
    const unsigned char *data,
    size_t size,
    lt48_compact_wave **out_wave
) {
    lt48_compact_wave *wave;
    size_t position = 2;
    if (data == NULL || out_wave == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_wave = NULL;
    if (size < 2 || ((size_t)data[0] | (size_t)data[1] << 8) != size - 2) {
        return LT48_FORMAT_ERROR;
    }
    wave = calloc(1, sizeof(*wave));
    if (wave == NULL) {
        return LT48_NO_MEMORY;
    }
    wave->data = malloc(size == 0 ? 1 : size);
    wave->instructions = calloc(size > 2 ? size - 2 : 1,
                                sizeof(*wave->instructions));
    if (wave->data == NULL || wave->instructions == NULL) {
        lt48_compact_wave_close(wave);
        return LT48_NO_MEMORY;
    }
    memcpy(wave->data, data, size);
    wave->size = size;
    while (position < size) {
        compact_instruction_owned *instruction =
            &wave->instructions[wave->instruction_count];
        size_t offset = position;
        uint8_t opcode = wave->data[position++];
        int width;
        if (opcode >= sizeof(operand_widths) / sizeof(operand_widths[0])) {
            lt48_compact_wave_close(wave);
            return LT48_FORMAT_ERROR;
        }
        width = operand_widths[opcode];
        if (width < 0) {
            if (size - position < 2) {
                lt48_compact_wave_close(wave);
                return LT48_FORMAT_ERROR;
            }
            width = 2 + wave->data[position + 1];
        }
        if ((size_t)width > size - position) {
            lt48_compact_wave_close(wave);
            return LT48_FORMAT_ERROR;
        }
        instruction->offset = offset;
        instruction->opcode = opcode;
        instruction->operand_offset = position;
        instruction->operand_size = (size_t)width;
        ++wave->instruction_count;
        position += (size_t)width;
    }
    *out_wave = wave;
    return LT48_OK;
}

size_t lt48_compact_instruction_count(const lt48_compact_wave *wave) {
    return wave == NULL ? 0 : wave->instruction_count;
}

lt48_status lt48_compact_get_instruction(
    const lt48_compact_wave *wave,
    size_t index,
    lt48_compact_instruction *out_instruction
) {
    const compact_instruction_owned *instruction;
    if (wave == NULL || out_instruction == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= wave->instruction_count) {
        return LT48_NOT_FOUND;
    }
    instruction = &wave->instructions[index];
    out_instruction->offset = instruction->offset;
    out_instruction->opcode = instruction->opcode;
    out_instruction->operands = wave->data + instruction->operand_offset;
    out_instruction->operand_size = instruction->operand_size;
    out_instruction->compiler_handler = compiler_handlers[instruction->opcode];
    return LT48_OK;
}
