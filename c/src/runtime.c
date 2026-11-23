#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../NewDeviceLib/sources/gal59/pin-loop.h"
#include "../../NewDeviceLib/sources/palce54/remap.h"
#include "../../NewDeviceLib/sources/mach11/callbacks.h"
#include "../../NewDeviceLib/sources/ejed9/callbacks.h"

/* Dataman 13.10 GBD declares 76 buffers; its W72 writes buffer 70.
 * Storage capacity does not admit a device or extend the frame-profile API. */
#define LT48_RUNTIME_BUFFER_SIZE 32u
#define LT48_RUNTIME_EVENT_ARGUMENTS 64u
#define LT48_RUNTIME_DEFAULT_MAX_STEPS 1000000u

/* Original 443A50 reconstructs centivolts from cached DAC bytes before
 * sorting: channel0 has 5 cV steps, other channels have 10 cV steps. */
static uint32_t shutdown_voltage(uint32_t value,unsigned channel,int policy){
 unsigned step=channel?10u:5u;return (policy==3||policy==4)?value/step*step:value;
}

typedef struct runtime_event_owned {
    lt48_runtime_event_kind kind;
    uint64_t arguments[LT48_RUNTIME_EVENT_ARGUMENTS];
    size_t argument_count;
} runtime_event_owned;

struct lt48_runtime_result {
    runtime_event_owned *events;
    size_t event_count;
    size_t event_capacity;
    unsigned char buffers[LT48_RUNTIME_BUFFER_COUNT][LT48_RUNTIME_BUFFER_SIZE];
    uint32_t voltage[4];
    lt48_compile_variable *variables;
    size_t variable_count;
    unsigned char *device_image;
    size_t device_image_size;
    lt48_runtime_configuration_value configuration[1024];
    size_t configuration_count;
    unsigned fuse_section;
    int has_fuse_section;
    lt48_runtime_fuse_image host_fuse_image;
};

typedef struct runtime_call_frame {const uint16_t *words;size_t word_count,pc;} runtime_call_frame;
typedef struct runtime_pin_loop {int active;size_t start,end,index;uint32_t mask;lt48_pin_group group;} runtime_pin_loop;
/* Opt-in continuation for ordinary pin-group captures. It changes no public
 * options layout and leaves the established replay machine as the default. */
typedef struct runtime_pin_cursor {
    int valid,streaming,yielded;
    const uint16_t *words;
    size_t word_count,pc,steps,capture_index,depth,event_start;
    runtime_call_frame stack[64];
    runtime_pin_loop pin_loop;
    unsigned interpreter_state;
} runtime_pin_cursor;

struct lt48_runtime_machine {
    const lt48_compiled_wave *compiled;
    const lt48_pin_map *pin_map;
    const lt48_compile_variable *variables;
    size_t variable_count;
    lt48_runtime_options options;
    unsigned char parameter_buffer[LT48_RUNTIME_BUFFER_SIZE];
    lt48_runtime_buffer_seed *initial_buffers;
    unsigned char *initial_buffer_data;
    unsigned char *responses;
    size_t response_count;
    size_t response_capacity;
    lt48_runtime_result *result;
    lt48_runtime_machine_state state;
    uint8_t capture_buffer;
    size_t capture_length;
    size_t last_event_first;
    size_t last_event_count;
    lt48_gal_pes_binding gal_pes_binding;
    lt48_runtime_nested_wave *nested_waves;
    lt48_runtime_fuse_image fuse_image;
    unsigned char *fuse_image_data;
    unsigned char *device_image;
    lt48_runtime_configuration_value *configuration;
    int resume_pin_captures;
    lt48_status resume_error;
    runtime_pin_cursor cursor;
};

static lt48_status append_event(
    lt48_runtime_result *result, lt48_runtime_event_kind kind,
    const uint64_t *arguments, size_t argument_count
) {
    runtime_event_owned *event;
    if (argument_count > LT48_RUNTIME_EVENT_ARGUMENTS) {
        return LT48_RUNTIME_ERROR;
    }
    if (result->event_count == result->event_capacity) {
        size_t capacity = result->event_capacity ? result->event_capacity * 2u : 32u;
        runtime_event_owned *events;
        if (capacity < result->event_capacity ||
            capacity > SIZE_MAX / sizeof(*events)) {
            return LT48_NO_MEMORY;
        }
        events = realloc(result->events, capacity * sizeof(*events));
        if (!events) {
            return LT48_NO_MEMORY;
        }
        result->events = events;
        result->event_capacity = capacity;
    }
    event = &result->events[result->event_count++];
    event->kind = kind;
    event->argument_count = argument_count;
    if (argument_count) {
        memcpy(event->arguments, arguments, argument_count * sizeof(*arguments));
    }
    return LT48_OK;
}

static lt48_status find_variable(
    const lt48_compile_variable *variables, size_t variable_count,
    uint16_t variable_id, const lt48_compile_variable **out_variable
) {
    const lt48_compile_variable *found = NULL;
    size_t index;
    for (index = 0; index < variable_count; ++index) {
        if (variables[index].variable_id == variable_id) {
            if (found) {
                return LT48_RUNTIME_ERROR;
            }
            found = &variables[index];
        }
    }
    if (!found) {
        return LT48_RUNTIME_ERROR;
    }
    *out_variable = found;
    return LT48_OK;
}

static lt48_status evaluate_variable(
    const lt48_compile_variable *variables, size_t variable_count,
    uint16_t variable_id, uint64_t *out_value
) {
    static const uint32_t scales[] = {1u, 100u, 100000u};
    const lt48_compile_variable *variable;
    lt48_status status = find_variable(variables, variable_count, variable_id,
                                       &variable);
    if (status != LT48_OK) {
        return status;
    }
    if (variable->kind == 1u) {
        if (variable->value >= sizeof(scales) / sizeof(scales[0])) {
            return LT48_RUNTIME_ERROR;
        }
        *out_value = (uint64_t)variable->channel * scales[variable->value];
        return LT48_OK;
    }
    if (variable->kind == 2u) {
        /* Original loader 0041f8a0 reads the final two REZ words as
         * one DWORD for kind 2; 0041f580 returns that complete value. */
        *out_value = (uint32_t)variable->value | (uint32_t)variable->channel << 16;
        return LT48_OK;
    }
    return LT48_RUNTIME_ERROR;
}

static uint32_t load_dword(const unsigned char *buffer) {
    return (uint32_t)buffer[0] | (uint32_t)buffer[1] << 8 |
           (uint32_t)buffer[2] << 16 | (uint32_t)buffer[3] << 24;
}

static void store_dword(unsigned char *buffer, uint32_t value) {
    buffer[0] = (unsigned char)value;
    buffer[1] = (unsigned char)(value >> 8);
    buffer[2] = (unsigned char)(value >> 16);
    buffer[3] = (unsigned char)(value >> 24);
}

static int16_t displacement(uint16_t value) {
    return (int16_t)value;
}

static lt48_status branch_target(size_t placeholder, int16_t delta,
                                 size_t word_count, size_t *out_pc) {
    int64_t target = (int64_t)placeholder + delta;
    if (target < 1 || (uint64_t)target > word_count) {
        return LT48_RUNTIME_ERROR;
    }
    *out_pc = (size_t)target;
    return LT48_OK;
}

static int require_words(size_t pc, size_t count, size_t word_count) {
    return pc <= word_count && count <= word_count - pc;
}

static lt48_status runtime_interpret_cursor(
    const lt48_compiled_wave *compiled,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    const lt48_runtime_options *options,
    lt48_runtime_result **out_result,
    lt48_runtime_result *existing,
    runtime_pin_cursor *cursor
) {
    const uint16_t *words;
    size_t word_count;
    size_t pc = 1u;
    size_t steps = 0u;
    size_t capture_response_index = 0u;
    size_t max_steps;
    runtime_call_frame stack[64];
    size_t depth = 0;
    runtime_pin_loop pin_loop = {0};
    unsigned interpreter_state = 1;
    int procedure_boundaries = options && options->nested_wave_count;
    lt48_runtime_result *result;
    lt48_status status = LT48_OK;

    if (!compiled || !pin_map || !options || !out_result ||
        (variable_count && !variables) ||
        (options->parameter_size && !options->parameter_buffer) ||
        (options->capture_response_size &&
         !options->capture_response_bytes) ||
        options->parameter_size > LT48_RUNTIME_BUFFER_SIZE ||
        (options->initial_buffer_count && !options->initial_buffers) ||
        (options->nested_wave_count && !options->nested_waves) ||
        (options->configuration_count && (!options->configuration || options->configuration_count>1024u))) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_result = NULL;
    for (size_t i = 0; i < options->nested_wave_count; ++i) {
        if (!options->nested_waves[i].compiled) return LT48_INVALID_ARGUMENT;
        for (size_t j = 0; j < i; ++j)
            if (options->nested_waves[i].operation == options->nested_waves[j].operation)
                return LT48_INVALID_ARGUMENT;
    }
    words = lt48_compiled_wave_words(compiled, &word_count);
    if (!words || !word_count || words[0] != word_count - 1u) {
        return LT48_RUNTIME_ERROR;
    }
    max_steps = options->max_steps ? options->max_steps : LT48_RUNTIME_DEFAULT_MAX_STEPS;
    if(existing){
        if(!cursor || !cursor->valid)return LT48_INVALID_ARGUMENT;
        result=existing;variables=result->variables;variable_count=result->variable_count;
        words=cursor->words;word_count=cursor->word_count;pc=cursor->pc;
        steps=cursor->steps;capture_response_index=cursor->capture_index;
        depth=cursor->depth;memcpy(stack,cursor->stack,depth*sizeof(*stack));
        pin_loop=cursor->pin_loop;interpreter_state=cursor->interpreter_state;
        result->event_count=cursor->event_start;cursor->valid=0;
        goto execute_cursor;
    }
    if(cursor)cursor->valid=0;
    result = calloc(1u, sizeof(*result));
    if (!result) {
        return LT48_NO_MEMORY;
    }
    if (options->device_image_size) {
        if (!options->device_image || !options->device_element_bytes || options->device_element_bytes>32u) {
            status=LT48_INVALID_ARGUMENT; goto fail;
        }
        result->device_image=malloc(options->device_image_size);
        if (!result->device_image) { status=LT48_NO_MEMORY; goto fail; }
        memcpy(result->device_image,options->device_image,options->device_image_size);
        result->device_image_size=options->device_image_size;
    }
    result->configuration_count=options->configuration_count;
    if (options->configuration_count) memcpy(result->configuration,options->configuration,
        options->configuration_count*sizeof(*options->configuration));
    {
        if (variable_count > SIZE_MAX / sizeof(*result->variables)) {
            status = LT48_INVALID_ARGUMENT; goto fail;
        }
        if (variable_count) {
            result->variables = malloc(variable_count * sizeof(*result->variables));
            if (!result->variables) { status = LT48_NO_MEMORY; goto fail; }
            memcpy(result->variables, variables, variable_count * sizeof(*variables));
        }
        result->variable_count = variable_count;
        variables = result->variables;
    }
    {
        uint8_t address_buffer = options->use_explicit_buffer_ids ?
            options->address_buffer : 0u;
        uint8_t parameter_buffer = options->use_explicit_buffer_ids ?
            options->parameter_buffer_id : 1u;
        uint8_t mode_buffer = options->use_explicit_buffer_ids ?
            options->mode_buffer : 2u;
        if (address_buffer >= LT48_RUNTIME_BUFFER_COUNT ||
            parameter_buffer >= LT48_RUNTIME_BUFFER_COUNT ||
            mode_buffer >= LT48_RUNTIME_BUFFER_COUNT)
            { status = LT48_INVALID_ARGUMENT; goto fail; }
        store_dword(result->buffers[address_buffer], options->address);
        if (options->parameter_size)
            memcpy(result->buffers[parameter_buffer], options->parameter_buffer,
                   options->parameter_size);
        store_dword(result->buffers[mode_buffer], options->mode);
    }
    for (size_t index = 0u; index < options->initial_buffer_count; ++index) {
        const lt48_runtime_buffer_seed *seed = &options->initial_buffers[index];
        if (seed->buffer >= LT48_RUNTIME_BUFFER_COUNT ||
            seed->size > LT48_RUNTIME_BUFFER_SIZE ||
            (seed->size && !seed->data)) {
            status = LT48_INVALID_ARGUMENT;
            goto fail;
        }
        memset(result->buffers[seed->buffer], 0, LT48_RUNTIME_BUFFER_SIZE);
        memcpy(result->buffers[seed->buffer], seed->data, seed->size);
    }
    memcpy(result->voltage, options->initial_voltage_centivolts,
           sizeof(result->voltage));
    max_steps = options->max_steps ? options->max_steps :
                LT48_RUNTIME_DEFAULT_MAX_STEPS;

#define FAIL(code) do { status = (code); if(getenv("LT48_SERIAL_TRACE"))fprintf(stderr,"runtime line %d pc %zu opcode %x status %d\n",__LINE__,pc,pc<word_count?words[pc]:0,status); goto fail; } while (0)
#define REQUIRE(count) do { if (!require_words(pc, (count), word_count)) \
    FAIL(LT48_RUNTIME_ERROR); } while (0)
#define APPEND(kind, args, count) do { status = append_event(result, (kind), \
    (args), (count)); if (status != LT48_OK) goto fail; } while (0)

    if (procedure_boundaries) {
        uint32_t seconds = options->root_timeout_seconds;
        uint64_t entry[2];
        if (seconds < 5) seconds = 5;
        if (seconds > UINT32_MAX / 1000u) FAIL(LT48_INVALID_ARGUMENT);
        entry[0] = seconds * 1000u; entry[1] = interpreter_state;
        APPEND(LT48_RUNTIME_WAVE_WATCHDOG, entry, 2);
    }
execute_cursor:
    for (;;) {
        uint16_t op;
        if (pc >= word_count) {
            interpreter_state = 2;
            if (!depth) break;
            --depth;
            words = stack[depth].words; word_count = stack[depth].word_count;
            pc = stack[depth].pc;
            continue;
        }
        if(cursor&&cursor->streaming&&result->event_count>=4096u){
            cursor->valid=1;cursor->yielded=1;cursor->words=words;cursor->word_count=word_count;cursor->pc=pc;cursor->steps=steps;cursor->capture_index=capture_response_index;cursor->depth=depth;memcpy(cursor->stack,stack,depth*sizeof(*stack));cursor->pin_loop=pin_loop;cursor->interpreter_state=interpreter_state;cursor->event_start=result->event_count;goto complete;
        }
        if (++steps > max_steps) {
            FAIL(LT48_RUNTIME_ERROR);
        }
        op = words[pc];
        if (op == 0u || op == 1u) {
            uint64_t arguments[1];
            REQUIRE(2u);
            arguments[0] = words[pc + 1u] & 0xffu;
            APPEND(op ? LT48_RUNTIME_Q1 : LT48_RUNTIME_Q2, arguments, 1u);
            pc += 2u;
        } else if (op == 2u || op == 3u) {
            /* Opcode 3 has an explicit destination channel; the value still
             * comes from a selected voltage variable. PIC18 DEVINIT supplies
             * VCC to both DAC1 and DAC2 this way. */
            const lt48_compile_variable *variable;
            uint64_t arguments[3];
            REQUIRE(op==2u?2u:3u);
            status=find_variable(variables,variable_count,words[pc+(op==2u?1:2)],&variable);
            if(status!=LT48_OK || (op==3u && variable->kind!=0u)) FAIL(LT48_RUNTIME_ERROR);
            unsigned channel=op==2u?variable->channel:words[pc+1];
            if(channel>=4u) FAIL(LT48_RUNTIME_ERROR);
            uint32_t scale=channel==0u?20u:10u;
            uint32_t calibration=(uint32_t[]){0u,13u,13u,20u}[channel];
            result->voltage[channel]=variable->value;
            arguments[0]=channel;arguments[1]=variable->value;
            uint32_t dac_value=variable->value;
            if(op==3u&&(options->host_avr_parallel||options->host_staged_shutdown==4)){float volts=(float)((double)(float)dac_value/100.0);dac_value=(uint32_t)((double)volts*100.0);}
            arguments[2]=dac_value?scale*dac_value/100u+calibration:0u;
            if(arguments[2]>255u) FAIL(LT48_SAFETY_REJECTED);
            APPEND(LT48_RUNTIME_VOLTAGE,arguments,3u);
            pc+=op==2u?2u:3u;
        } else if (op == 4u || op == 5u) {
            static const uint8_t channels[] = {0x1c, 0x20, 0x21, 0x22};
            const lt48_compile_variable *variable;
            lt48_pin_group group;
            uint64_t packet[2], q2[1];
            REQUIRE(op == 4u ? 3u : 4u);
            if (!pin_loop.active) FAIL(LT48_RUNTIME_ERROR);
            status = lt48_pin_map_find_group(pin_map, words[pc + 1u], &group);
            if (status != LT48_OK || pin_loop.index >= group.pin_count) FAIL(LT48_RUNTIME_ERROR);
            status = find_variable(variables, variable_count, words[pc + 2u], &variable);
            if (status != LT48_OK || variable->channel >= 4u) FAIL(LT48_RUNTIME_ERROR);
            unsigned pin = group.pins[pin_loop.index];
            unsigned offset = op == 4u ? 1u : words[pc + 3u] & 255u;
            packet[0] = channels[variable->channel];
            packet[1] = (pin >> 1u) + (op == 4u ? 1u : 0u);
            if (offset) { q2[0] = (pin + offset) & 255u; APPEND(LT48_RUNTIME_Q2, q2, 1u); }
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, packet, 2u);
            if (!offset || op == 4u) { q2[0] = (pin + (op == 4u ? 2u : 0u)) & 255u; APPEND(LT48_RUNTIME_Q2, q2, 1u); }
            pc += op == 4u ? 3u : 4u;
        } else if (op == 6u) {
            /* Original4411DC: active I/O-list entry + byte offset -> Q2. */
            REQUIRE(3u);
            if(!pin_loop.active)FAIL(LT48_RUNTIME_ERROR);
            uint64_t argument;
            status=gal59_loop_ttl(pin_map,words[pc+1u],pin_loop.index,
                words[pc+2u]&255u,&argument);
            if(status!=LT48_OK)goto fail;
            APPEND(LT48_RUNTIME_Q2,&argument,1u);
            pc+=3u;
        } else if (op == 0x5du) {
            REQUIRE(4u);
            if (!pin_loop.active) {
                if (words[pc + 2u] >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
                status = lt48_pin_map_find_group(pin_map, words[pc + 1u], &pin_loop.group);
                if (status != LT48_OK) goto fail;
                pin_loop.active = 1; pin_loop.start = pc; pin_loop.index = 0u;
                pin_loop.end = pc + 3u + words[pc + 3u];
                if (pin_loop.end <= pc + 4u || pin_loop.end > word_count) FAIL(LT48_RUNTIME_ERROR);
                pin_loop.mask = load_dword(result->buffers[words[pc + 2u]]);
            } else {
                if (pc != pin_loop.start) FAIL(LT48_UNSUPPORTED);
                ++pin_loop.index;
                /* The x86 original uses arithmetic right shift. */
                pin_loop.mask = (pin_loop.mask >> 1u) | (pin_loop.mask & 0x80000000u);
            }
            if (pin_loop.index >= pin_loop.group.pin_count) {
                pc = pin_loop.end; pin_loop.active = 0;
            } else if (pin_loop.group.pins[pin_loop.index] > 0xfbu || !(pin_loop.mask & 1u)) {
                pc = pin_loop.start;
            } else pc += 4u;
        } else if (op == 0x5eu) {
            if (!pin_loop.active) FAIL(LT48_RUNTIME_ERROR);
            pc = pin_loop.start;
        } else if (op == 0x62u) {
            /* Original compact80: current I/O-list bit -> TPIC sink route.
             * Digital driver is floated before the sink is switched. The
             * source TI PROM programming wave uses offsets 1 (on), 0 (off). */
            lt48_pin_group group;
            uint64_t q2[1], packet[2];
            REQUIRE(3u);
            if (!pin_loop.active || words[pc + 2u] > 255u)
                FAIL(LT48_RUNTIME_ERROR);
            status = lt48_pin_map_find_group(pin_map, words[pc + 1u], &group);
            if (status != LT48_OK || pin_loop.index >= group.pin_count ||
                group.pins[pin_loop.index] > 0xfbu) FAIL(LT48_RUNTIME_ERROR);
            unsigned pin = group.pins[pin_loop.index];
            q2[0] = pin + 2u;
            APPEND(LT48_RUNTIME_Q2, q2, 1u);
            packet[0] = 0x1eu;
            packet[1] = ((pin >> 1u) + words[pc + 2u]) & 255u;
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, packet, 2u);
            pc += 3u;
        } else if (op == 0x1au) {
            /* Original00442d40: first/second voltage plus duration; seed
             * first DAC, select Q2, then00442c40 ramps to the second. */
            static const uint8_t calibration[] = {0,13,13,20};
            const lt48_compile_variable *first, *second;
            uint32_t from, to;
            uint64_t duration, arguments[7];
            unsigned char packet[7];
            uint8_t channel;
            REQUIRE(6u);
            if (words[pc + 5u] != 0x4fu || words[pc + 1u] > UINT8_MAX)
                FAIL(LT48_RUNTIME_ERROR);
            status = find_variable(variables,variable_count,words[pc + 2u],&first);
            if (status != LT48_OK) goto fail;
            status = find_variable(variables,variable_count,words[pc + 3u],&second);
            if (status != LT48_OK) goto fail;
            if (first->kind != 0 || second->kind != 0 || first->channel >= 4 ||
                second->channel != first->channel) FAIL(LT48_SAFETY_REJECTED);
            channel = (uint8_t)first->channel;
            from = first->value ? (channel ? 10u : 20u) * first->value / 100u + calibration[channel] : 0;
            to = second->value ? (channel ? 10u : 20u) * second->value / 100u + calibration[channel] : 0;
            if (from > UINT8_MAX || to > UINT8_MAX) FAIL(LT48_SAFETY_REJECTED);
            status = evaluate_variable(variables,variable_count,words[pc + 4u],&duration);
            if (status != LT48_OK) goto fail;
            if (duration > INT32_MAX) FAIL(LT48_SAFETY_REJECTED);
            status = lt48_dac_ramp_packet(channel,(uint8_t)from,(uint8_t)to,(uint32_t)duration,packet);
            if(status==LT48_SAFETY_REJECTED && options->host_xc7000_ramp && channel==3 &&
               from==120 && to==155 && duration==1750000) {
                /* GAE M0/M1 tWOUT: original 442c40 serializes 99990 as a
                 * 16-bit interval (34454). Preserve its actual wire packet,
                 * not an invented 1.75-second ramp. No generic relaxation. */
                static const unsigned char original[]={0x12,3,120,155,0,0x96,0x86};
                memcpy(packet,original,sizeof original);status=LT48_OK;
            }
            if (status != LT48_OK) goto fail;
            arguments[0] = 0x10; arguments[1] = channel; arguments[2] = from;
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,arguments,3);
            arguments[0] = (uint8_t)(words[pc + 1u] + 2);
            APPEND(LT48_RUNTIME_Q2,arguments,1);
            for (size_t i = 0; i < sizeof(packet); ++i) arguments[i] = packet[i];
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,arguments,7);
            result->voltage[channel] = second->value;
            pc += 5u;
        } else if (op == 0x1du) {
            /* Dataman00442fa0: first variable in EDX, second in ECX.
             * Both byte evaluations precede four checked transport stages. */
            static const uint8_t selectors[] = {0x1c,0x20,0x21,0x22};
            static const uint8_t calibration[] = {0,13,13,20};
            const lt48_compile_variable *first, *second;
            uint32_t first_code, second_code;
            uint64_t arguments[4];
            uint8_t pin, channel;
            REQUIRE(5u);
            if (words[pc + 4u] != 0x4fu || words[pc + 1u] > UINT8_MAX)
                FAIL(LT48_RUNTIME_ERROR);
            status = find_variable(variables,variable_count,words[pc + 2u],&first);
            if (status != LT48_OK) goto fail;
            status = find_variable(variables,variable_count,words[pc + 3u],&second);
            if (status != LT48_OK) goto fail;
            if (first->kind != 0 || second->kind != 0 || first->channel >= 4 ||
                second->channel != first->channel) FAIL(LT48_SAFETY_REJECTED);
            channel = (uint8_t)first->channel; pin = (uint8_t)words[pc + 1u];
            first_code = first->value ? (channel ? 10u : 20u) * first->value / 100u + calibration[channel] : 0;
            second_code = second->value ? (channel ? 10u : 20u) * second->value / 100u + calibration[channel] : 0;
            if (first_code > UINT8_MAX || second_code > UINT8_MAX) FAIL(LT48_SAFETY_REJECTED);
            /* Original host DAC shadow is finally assigned first_code. */
            result->voltage[channel] = first->value;
            arguments[0] = 0x10; arguments[1] = channel; arguments[2] = first_code;
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,arguments,3);
            arguments[0] = selectors[channel]; arguments[1] = (pin >> 1) + 1;
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,arguments,2);
            arguments[0] = (uint8_t)(pin + 2);
            APPEND(LT48_RUNTIME_Q2,arguments,1);
            arguments[0] = 0x13; arguments[1] = channel;
            arguments[2] = first_code; arguments[3] = second_code;
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,arguments,4);
            pc += 4u; /* Process trailing4F status independently. */
        } else if (op == 0x1bu || op == 0x1cu) {
            /* AEC 0041ab50 / Dataman 00442df0: select pin BEFORE the
             * starting DAC command. Every payload uses transport +0x44;
             * the rail route is not the bulk/raw transport +0x20. */
            static const uint8_t selectors[] = {0x1cu, 0x20u, 0x21u, 0x22u};
            static const uint8_t calibration[] = {0u, 13u, 13u, 20u};
            const lt48_compile_variable *voltage_variable;
            uint64_t arguments[7];
            uint64_t duration;
            uint32_t target;
            uint8_t pin, flag, channel, start;
            unsigned char packet[7];
            REQUIRE(6u);
            pin = (uint8_t)words[pc + 1u];
            flag = (uint8_t)words[pc + (op == 0x1bu ? 2u : 3u)];
            /* 442df0 uses an arbitrary byte selector (W64 uses 108),
             * tests it for zero when choosing the initial voltage, and adds
             * it to the mapped pin with byte wrapping. */
            if (words[pc + (op == 0x1bu ? 2u : 3u)] > 255u || words[pc + 5u] != 0x4fu)
                FAIL(LT48_RUNTIME_ERROR);
            status = find_variable(variables, variable_count,
                                   words[pc + (op == 0x1bu ? 3u : 2u)],
                                   &voltage_variable);
            if (status != LT48_OK || voltage_variable->kind != 0u ||
                voltage_variable->channel >= 4u)
                FAIL(LT48_RUNTIME_ERROR);
            channel = (uint8_t)voltage_variable->channel;
            target = voltage_variable->value ?
                (channel == 0u ? 20u : 10u) *
                    (uint32_t)voltage_variable->value / 100u +
                    calibration[channel] : 0u;
            if (target > UINT8_MAX) FAIL(LT48_SAFETY_REJECTED);
            status = evaluate_variable(variables, variable_count,
                                       words[pc + 4u], &duration);
            if (status != LT48_OK) goto fail;
            if (duration > INT32_MAX) FAIL(LT48_SAFETY_REJECTED);
            start = (uint8_t)((channel == 0u ? 20u : 10u) * (flag ? 5u : 0u));
            status = lt48_dac_ramp_packet(channel,
                                          op == 0x1bu ? start : (uint8_t)target,
                                          op == 0x1bu ? (uint8_t)target : start,
                                          (uint32_t)duration, packet);
            if (status != LT48_OK) goto fail;
            if (op == 0x1bu) {
                arguments[0] = (pin + flag) & 0xffu;
                APPEND(LT48_RUNTIME_Q2, arguments, 1u);
                arguments[0] = 0x10u; arguments[1] = channel;
                arguments[2] = start;
                APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, arguments, 3u);
                arguments[0] = selectors[channel];
                arguments[1] = (pin >> 1) + 1u;
                APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, arguments, 2u);
                arguments[0] = (pin + 2u) & 0xffu;
                APPEND(LT48_RUNTIME_Q2, arguments, 1u);
            }
            for (size_t index = 0u; index < sizeof(packet); ++index)
                arguments[index] = packet[index];
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, arguments, 7u);
            if (op == 0x1cu) {
                /* AEC 0041ac20 / Dataman 00442ef0: ramp FIRST, then
                 * select and disconnect the rail through buffered +0x44. */
                arguments[0] = (pin + flag) & 0xffu;
                APPEND(LT48_RUNTIME_Q2, arguments, 1u);
                arguments[0] = selectors[channel];
                arguments[1] = pin >> 1;
                APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, arguments, 2u);
                result->voltage[channel] = start <= calibration[channel] ?
                    0u : (start - calibration[channel]) * 100u /
                    (channel == 0u ? 20u : 10u);
            } else {
                result->voltage[channel] = voltage_variable->value;
            }
            /* The trailing4F is a separate status instruction. Original
             * 00442df0/00442ef0 return to it after ramp/route setup. Skipping
             * it loses the rail-fault check (KA DEVRESET x86 comparison). */
            pc += 5u;
        } else if (op == 0x12u || op == 0x19u) {
            /* AEC 0045bcb0 / Dataman 00497b20: batches of at most
             * 32 repeated controller words, through buffered +0x44. */
            uint64_t arguments[64];
            uint16_t remaining;
            uint8_t selected;
            REQUIRE(4u);
            remaining = words[pc + 2u];
            if(op==0x19u) {
                if(remaining>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
                remaining=(uint16_t)(result->buffers[remaining][0] | (unsigned)result->buffers[remaining][1]<<8);
            }
            selected = (uint8_t)(words[pc + 1u] + words[pc + 3u]);
            while (remaining) {
                size_t count = remaining > 32u ? 32u : remaining;
                for (size_t index = 0u; index < count; ++index) {
                    arguments[index * 2u] = 0x18u;
                    arguments[index * 2u + 1u] = selected;
                }
                APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, arguments, count * 2u);
                remaining = (uint16_t)(remaining - count);
            }
            pc += 4u;
        } else if (op == 0x1fu || op == 0x21u) {
            /* Dataman 00440f71 -> 00443170: timed source-bank pulse.
             * The voltage object's channel chooses the controller opcode;
             * the duration is evaluated independently. Long ticks subtract 1. */
            static const uint8_t short_op[] = {0,0x28,0x29,0x2a};
            static const uint8_t long_op[] = {0,0x30,0x31,0x32};
            const lt48_compile_variable *voltage;
            uint64_t duration, arguments[4];size_t count;
            unsigned pin;
            REQUIRE(5u);
            pin = words[pc+1u];
            if (op == 0x21u) {
                lt48_pin_group group;
                if (!pin_loop.active) FAIL(LT48_RUNTIME_ERROR);
                status = lt48_pin_map_find_group(pin_map, words[pc+1u], &group);
                if (status != LT48_OK || pin_loop.index >= group.pin_count) FAIL(LT48_RUNTIME_ERROR);
                pin = group.pins[pin_loop.index];
            }
            status=find_variable(variables,variable_count,words[pc+3u],&voltage);
            if(status!=LT48_OK)goto fail;
            if(voltage->kind!=0 || voltage->channel<1 || voltage->channel>3 ||
               pin>255 || words[pc+4u]>1)FAIL(LT48_SAFETY_REJECTED);
            status=evaluate_variable(variables,variable_count,words[pc+2u],&duration);
            if(status!=LT48_OK)goto fail;
            arguments[1]=((pin>>1)+words[pc+4u])&255;
            if(duration<258) {
                arguments[0]=short_op[voltage->channel];
                arguments[2]=duration>=3?duration-3:duration;count=3;
            } else {
                uint64_t ticks=duration/100-1;
                if(ticks>UINT16_MAX)FAIL(LT48_SAFETY_REJECTED);
                arguments[0]=long_op[voltage->channel];
                arguments[2]=ticks&255;arguments[3]=ticks>>8;count=4;
            }
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,arguments,count);
            pc+=5u;
        } else if (op == 0x1eu) {
            /* Compact 0x2c, AEC 0041bc00 -> 0041ad40: buffered
             * pin-duration command, not the bulk/raw transport. */
            uint64_t duration, arguments[4];
            size_t count;
            REQUIRE(4u);
            status = evaluate_variable(variables, variable_count,
                                       words[pc + 2u], &duration);
            if (status != LT48_OK) goto fail;
            arguments[1] = (words[pc + 1u] + words[pc + 3u]) & 0xffu;
            if (duration < 0x102u) {
                arguments[0] = 0x19u;
                arguments[2] = duration > 2u ? duration - 3u : duration;
                count = 3u;
            } else {
                uint64_t scaled = duration / 100u;
                if (scaled > UINT16_MAX) FAIL(LT48_SAFETY_REJECTED);
                arguments[0] = 0x1au;
                arguments[2] = scaled & 0xffu;
                arguments[3] = scaled >> 8;
                count = 4u;
            }
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, arguments, count);
            pc += 4u;
        } else if (op == 7u) {
            lt48_pin_group group;
            uint64_t arguments[LT48_RUNTIME_EVENT_ARGUMENTS];
            size_t index;
            REQUIRE(3u);
            status = lt48_pin_map_find_group(pin_map, words[pc + 1u], &group);
            /* Original488AE0 preinitializes all36 IO-group records empty.
             * Source SST DIP setup deliberately references absent group3. */
            if(status==LT48_NOT_FOUND && words[pc+1u]<36u){group.pins=NULL;group.pin_count=0;status=LT48_OK;}
            if (status != LT48_OK || group.pin_count > LT48_RUNTIME_EVENT_ARGUMENTS) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            size_t emitted = 0;
            for (index = 0; index < group.pin_count; ++index) {
                /* FC..FF denote unmapped entries, not socket commands.
                 * Original +6C skips them but transmits the unadjusted count,
                 * exposing an uninitialised tail. Do not reproduce that tail. */
                if (group.pins[index] < 0xfcu)
                    arguments[emitted++] = (group.pins[index] + words[pc + 2u]) & 0xffu;
            }
            APPEND(LT48_RUNTIME_RAW_WRITE, arguments, emitted);
            pc += 3u;
        } else if (op == 8u || op == 9u || op == 0x0au) {
            /* Controller +0x74: one LSB-first source bit per mapped
             * control-record entry, followed by the bulk/raw +0x20 path. */
            lt48_pin_group group;
            unsigned char inline_bitmap[4];
            const unsigned char *bitmap;
            uint64_t arguments[48];
            size_t count = 0u;
            REQUIRE(op != 0x0au ? 3u : 4u);
            status = lt48_pin_map_find_group(pin_map, words[pc + 1u], &group);
            if (status != LT48_OK || group.pin_count > 48u)
                FAIL(LT48_RUNTIME_ERROR);
            if (op != 0x0au) {
                if (words[pc + 2u] >= LT48_RUNTIME_BUFFER_COUNT)
                    FAIL(LT48_RUNTIME_ERROR);
                bitmap = result->buffers[words[pc + 2u]];
            } else {
                if (group.pin_count > 32u) FAIL(LT48_RUNTIME_ERROR);
                inline_bitmap[0] = (unsigned char)words[pc + 2u];
                inline_bitmap[1] = (unsigned char)(words[pc + 2u] >> 8);
                inline_bitmap[2] = (unsigned char)words[pc + 3u];
                inline_bitmap[3] = (unsigned char)(words[pc + 3u] >> 8);
                bitmap = inline_bitmap;
            }
            for (size_t index = 0u; index < group.pin_count; ++index) {
                if (group.pins[index] < 0xfcu)
                    arguments[count++] = group.pins[index] +
                        ((bitmap[index / 8u] >> (index & 7u)) & 1u);
                /* Original controller +0x70 (00497710) updates the binary
                 * carry chain only, ending after the first set address bit. */
                if (op == 8u && ((bitmap[index / 8u] >> (index & 7u)) & 1u)) break;
            }
            APPEND(LT48_RUNTIME_RAW_WRITE, arguments, count);
            pc += op != 0x0au ? 3u : 4u;
        } else if (op == 0x71u) {
            /* Dataman 00497870: sample (encoded pin | 2), store AL in
             * destination byte zero. Use the existing single-element pin
             * group transport so zero is valid data, not a failed ACK. */
            REQUIRE(3u);uint16_t dest=words[pc+2];
            if(dest>=LT48_RUNTIME_BUFFER_COUNT || words[pc+1]>255u)FAIL(LT48_RUNTIME_ERROR);
            int missing=capture_response_index>=options->capture_response_size;
            if(!missing)result->buffers[dest][0]=options->capture_response_bytes[capture_response_index++];
            else if(options->require_capture_responses && !options->stop_at_missing_capture)FAIL(LT48_RUNTIME_ERROR);
            uint64_t args[3]={dest,1u,words[pc+1]|2u};
            APPEND(LT48_RUNTIME_CONTROLLER_PIN_READ,args,3u);
            if(missing && options->stop_at_missing_capture)goto complete;
            pc+=3u;
        } else if (op == 0x0bu) {
            lt48_pin_group group;
            uint64_t arguments[50];
            uint16_t buffer_id;
            size_t byte_count;
            REQUIRE(3u);
            buffer_id = words[pc + 2u];
            status = lt48_pin_map_find_group(pin_map, words[pc + 1u], &group);
            if (status != LT48_OK || !group.pin_count || group.pin_count > 48u ||
                buffer_id >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
            byte_count = (group.pin_count + 7u) / 8u;
            int missing_capture = capture_response_index + byte_count > options->capture_response_size;
            if (capture_response_index + byte_count <= options->capture_response_size) {
                memcpy(result->buffers[buffer_id],
                       options->capture_response_bytes + capture_response_index,
                       byte_count);
                capture_response_index += byte_count;
                /* Original pin-group reader packs only the selected pins.
                 * Explicit offline captures obey the same unused-bit rule. */
                if(group.pin_count & 7u)result->buffers[buffer_id][byte_count-1u] &=
                    (unsigned char)((1u << (group.pin_count & 7u))-1u);
            } else if (options->require_capture_responses && !options->stop_at_missing_capture)
                FAIL(LT48_RUNTIME_ERROR);
            arguments[0] = buffer_id;
            arguments[1] = group.pin_count;
            for (size_t index = 0u; index < group.pin_count; ++index)
                arguments[2u + index] = group.pins[index];
            APPEND(LT48_RUNTIME_CONTROLLER_PIN_READ, arguments, 2u + group.pin_count);
            if (missing_capture && options->stop_at_missing_capture) {
                if(cursor){
                    cursor->valid=1;cursor->yielded=0;cursor->words=words;cursor->word_count=word_count;
                    cursor->pc=pc;cursor->steps=steps-1u;cursor->capture_index=capture_response_index;
                    cursor->depth=depth;memcpy(cursor->stack,stack,depth*sizeof(*stack));
                    cursor->pin_loop=pin_loop;cursor->interpreter_state=interpreter_state;
                    cursor->event_start=result->event_count-1u;
                }
                goto complete;
            }
            pc += 3u;
        } else if (op == 0x0cu || op == 0x0du ||
                   op == 0x0fu || op == 0x10u || op == 0x13u || op == 0x14u || op == 0x16u || op == 0x17u) {
            uint16_t pin_a;
            uint16_t pin_b;
            uint16_t buffer_id = 0u;
            uint16_t bit_count;
            uint16_t flag;
            unsigned char packed[32] = {0};
            size_t byte_count;
            size_t index;
            uint64_t arguments[LT48_RUNTIME_EVENT_ARGUMENTS];
            if (op == 0x0cu || op == 0x0fu || op == 0x13u || op == 0x16u) {
                REQUIRE(6u);
                pin_a = words[pc + 1u] & 0xffu;
                pin_b = words[pc + 2u] & 0xffu;
                buffer_id = words[pc + 3u];
                if (op == 0x13u || op == 0x16u) {
                    if (words[pc + 4u] >= LT48_RUNTIME_BUFFER_COUNT)
                        FAIL(LT48_RUNTIME_ERROR);
                    bit_count = result->buffers[words[pc + 4u]][0];
                    if (!bit_count) FAIL(LT48_SAFETY_REJECTED);
                } else bit_count = words[pc + 4u] & 0xffu;
                flag = words[pc + 5u] & 0xffu;
                byte_count = ((size_t)bit_count + 7u) / 8u;
                if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT ||
                    byte_count > LT48_RUNTIME_BUFFER_SIZE) {
                    FAIL(LT48_RUNTIME_ERROR);
                }
                pc += 6u;
            } else {
                REQUIRE(7u);
                pin_a = words[pc + 1u] & 0xffu;
                pin_b = words[pc + 2u] & 0xffu;
                if(op == 0x14u || op == 0x17u) {
                    if(words[pc+5u]>=LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
                    bit_count=result->buffers[words[pc+5u]][0];
                } else bit_count = words[pc + 5u] & 0xffu;
                if(!bit_count) FAIL(LT48_SAFETY_REJECTED);
                flag = words[pc + 6u] & 0xffu;
                byte_count = ((size_t)bit_count + 7u) / 8u;
                if (byte_count > sizeof(packed)) FAIL(LT48_RUNTIME_ERROR);
                /* The original passes a pointer into the word stream to
                 * 00497890/00497930, even for >32-bit inline transfers (GCA
                 * W75 Secure uses94). Preserve those adjacent source bytes,
                 * but reject reads beyond the owned compiled stream. */
                REQUIRE(3u + (byte_count + 1u) / 2u);
                for (size_t j = 0; j < byte_count; ++j)
                    packed[j] = (unsigned char)(words[pc+3u+j/2u] >> ((j&1u)*8u));
                pc += 7u;
            }
            arguments[0] = (op == 0x0fu || op == 0x10u || op == 0x16u || op == 0x17u) ? 0x59u : 0x58u;
            arguments[1] = (pin_a + (byte_count ?
                ((op == 0x0cu || op == 0x0fu || op == 0x13u || op == 0x16u) ?
                    result->buffers[buffer_id][0] : packed[0]) & 1u
                : 0u)) & 0xffu;
            arguments[2] = (pin_b + ((~flag) & 1u)) & 0xffu;
            arguments[3] = (bit_count - 1u) & 0xffu;
            for (index = 0u; index < byte_count; ++index) {
                size_t source_index = (op == 0x0fu || op == 0x10u || op == 0x16u || op == 0x17u) ?
                    byte_count - 1u - index : index;
                arguments[4u + index] = (op == 0x0cu || op == 0x0fu || op == 0x13u || op == 0x16u) ?
                    result->buffers[buffer_id][source_index] : packed[source_index];
            }
            APPEND(LT48_RUNTIME_CONTROLLER_PACKED_WRITE, arguments,
                   4u + byte_count);
        } else if (op == 0x0eu || op == 0x11u || op == 0x15u || op == 0x18u) {
            uint16_t buffer_id;
            uint16_t bit_count;
            size_t byte_count;
            uint64_t arguments[6];
            REQUIRE(6u);
            buffer_id = words[pc + 3u];
            if (op == 0x15u || op == 0x18u) {
                if (words[pc + 4u] >= LT48_RUNTIME_BUFFER_COUNT)
                    FAIL(LT48_RUNTIME_ERROR);
                bit_count = result->buffers[words[pc + 4u]][0];
            } else bit_count = words[pc + 4u] & 0xffu;
            if (!bit_count) FAIL(LT48_SAFETY_REJECTED);
            byte_count = ((size_t)bit_count + 7u) / 8u;
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT ||
                byte_count > LT48_RUNTIME_BUFFER_SIZE) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            arguments[0] = (op==0x11u||op==0x18u)?0x61u:0x60u;
            arguments[1] = (words[pc + 1u] + 2u) & 0xffu;
            arguments[2] = (words[pc + 2u] +
                            ((words[pc + 5u] & 0xffu) == 0u)) & 0xffu;
            arguments[3] = (bit_count - 1u) & 0xffu;
            arguments[4] = buffer_id;
            arguments[5] = byte_count;
            int missing_capture = capture_response_index + byte_count > options->capture_response_size;
            if (capture_response_index + byte_count <=
                options->capture_response_size) {
                memcpy(result->buffers[buffer_id],
                       options->capture_response_bytes + capture_response_index,
                       byte_count);
                if(op==0x11u||op==0x18u)for(size_t j=0;j<byte_count/2;j++){unsigned char v=result->buffers[buffer_id][j];result->buffers[buffer_id][j]=result->buffers[buffer_id][byte_count-1-j];result->buffers[buffer_id][byte_count-1-j]=v;}
                capture_response_index += byte_count;
            } else if (options->require_capture_responses && !options->stop_at_missing_capture) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            APPEND(LT48_RUNTIME_CONTROLLER_PACKED_READ, arguments, 6u);
            if (missing_capture && options->stop_at_missing_capture) {
                if(cursor && cursor->streaming){
                    cursor->valid=1;cursor->yielded=0;cursor->words=words;cursor->word_count=word_count;
                    cursor->pc=pc;cursor->steps=steps-1u;cursor->capture_index=capture_response_index;
                    cursor->depth=depth;memcpy(cursor->stack,stack,depth*sizeof(*stack));
                    cursor->pin_loop=pin_loop;cursor->interpreter_state=interpreter_state;
                    cursor->event_start=result->event_count-1u;
                }
                goto complete;
            }
            pc += 6u;
        } else if (op == 0x7au) {
            /* Original 004405D0 -> 0041F580: send a voltage variable as two
             * applet FIFO parameters. This does not set the host DAC state. */
            const lt48_compile_variable *v;
            REQUIRE(2u);
            status=find_variable(variables,variable_count,words[pc+1u],&v);
            if(status!=LT48_OK || v->kind!=0u || v->channel>=4u)
                FAIL(LT48_UNSUPPORTED);
            unsigned channel=v->channel;
            uint64_t value=v->value ? (channel?10u:20u)*(uint32_t)v->value/100u+
                (unsigned[]){0u,13u,13u,20u}[channel] : 0u;
            if(value>255u)FAIL(LT48_SAFETY_REJECTED);
            uint64_t arg=channel;
            APPEND(LT48_RUNTIME_Q2,&arg,1u);
            APPEND(LT48_RUNTIME_Q2,&value,1u);
            pc+=2u;
        } else if (op == 0x78u || op == 0x79u) {
            REQUIRE(3u);
            unsigned buffer=words[pc+1u],n=words[pc+2u];
            if(buffer>=LT48_RUNTIME_BUFFER_COUNT || !n || n>LT48_RUNTIME_BUFFER_SIZE)FAIL(LT48_RUNTIME_ERROR);
            if(op==0x78u){
                uint64_t data[32];for(unsigned i=0;i<n;i++)data[i]=result->buffers[buffer][i];
                APPEND(LT48_RUNTIME_RAW_WRITE,data,n);
            }else{
                uint64_t args[6]={0,0,0,0,buffer,n};
                int missing=capture_response_index+n>options->capture_response_size;
                if(!missing){memcpy(result->buffers[buffer],options->capture_response_bytes+capture_response_index,n);capture_response_index+=n;}
                else if(options->require_capture_responses && !options->stop_at_missing_capture)FAIL(LT48_RUNTIME_ERROR);
                APPEND(LT48_RUNTIME_RAW_READ,args,6u);
                if(missing && options->stop_at_missing_capture)goto complete;
            }
            pc+=3u;
        } else if(op==0x22u){
            /* Dataman CDrvUSB +98, 00497bf0: reverse exactly N bits.
             * Define unused scratch bits as zero rather than copying the
             * original helper's uninitialized stack bits. */
            REQUIRE(3u);unsigned id=words[pc+1],bits=words[pc+2]&255u;
            if(id>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
            unsigned char scratch[32]={0};
            for(unsigned i=0;i<bits;i++)scratch[i/8]|=((result->buffers[id][(bits-1-i)/8]>>((bits-1-i)%8))&1u)<<(i%8);
            memcpy(result->buffers[id],scratch,(bits+7)/8);pc+=3u;
        } else if (op == 0x23u || op == 0x24u || op == 0x76u) {
            uint64_t arguments[1];
            if (op == 0x76u) {
                /* Original interpreter reads a buffer DWORD, then invokes
                 * the same 00443280 delay emitter as immediate delays. */
                REQUIRE(2u);
                if (words[pc + 1u] >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
                arguments[0] = load_dword(result->buffers[words[pc + 1u]]);
                pc += 2u;
            } else if (op == 0x23u) {
                REQUIRE(2u);
                status = evaluate_variable(variables, variable_count,
                                           words[pc + 1u], &arguments[0]);
                if (status != LT48_OK) goto fail;
                pc += 2u;
            } else {
                REQUIRE(3u);
                arguments[0] = words[pc + 1u] |
                               (uint32_t)words[pc + 2u] << 16;
                pc += 3u;
            }
            APPEND(LT48_RUNTIME_CONTROLLER_DELAY, arguments, 1u);
        } else if(op==0xf006u){
            REQUIRE(3u);status=ejed9_step(words[pc+1],words[pc+2],result->buffers);
            if(status!=LT48_OK)goto fail;
            pc+=3;
        } else if(op==0xf007u){
            REQUIRE(3u);unsigned row=ejed9_word(result->buffers[1]),stage=ejed9_word(result->buffers[15]);
            if(words[pc+1]>1u)FAIL(LT48_FORMAT_ERROR);
            size_t next=(words[pc+1]?row==0:row==stage)?words[pc+2]:pc+3;
            if(next>=word_count)FAIL(LT48_FORMAT_ERROR);
            pc=next;
        } else if(op==0xf004u){
            REQUIRE(3u);status=mach11_step(words[pc+1],words[pc+2],result->buffers);
            if(status!=LT48_OK)goto fail;
            pc+=3;
        } else if(op==0xf005u){
            REQUIRE(3u);unsigned row=result->buffers[1][0]|(unsigned)result->buffers[1][1]<<8;
            unsigned stage=result->buffers[7][0]|(unsigned)result->buffers[7][1]<<8;
            if(words[pc+1]>1u)FAIL(LT48_FORMAT_ERROR);
            size_t next=(words[pc+1]?row==0:row==stage)?words[pc+2]:pc+3;
            if(next>=word_count)FAIL(LT48_FORMAT_ERROR);
            pc=next;
        } else if (op == 0xf002u) {
            REQUIRE(7u);
            unsigned char *b[5];uint16_t value[5];
            for(unsigned i=0;i<5;i++) {
                unsigned id=words[pc+2+i];
                if(id>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_FORMAT_ERROR);
                for(unsigned j=0;j<i;j++)if(words[pc+2+j]==id)FAIL(LT48_FORMAT_ERROR);
                b[i]=result->buffers[id];value[i]=(uint16_t)(b[i][0]|(uint16_t)b[i][1]<<8);
            }
            status=lt48_pof_exception_internal(words[pc+1],value[0],value[1],&value[2],&value[3],&value[4]);
            if(status!=LT48_OK)goto fail;
            for(unsigned i=2;i<5;i++){b[i][0]=(unsigned char)value[i];b[i][1]=(unsigned char)(value[i]>>8);}
            pc+=7;
        } else if (op == 0xf000u || op == 0xf001u || op == 0xf003u) {
            if (!options->host_epm_legacy || result->device_image_size != LT48_EPM_LEGACY_STATE_SIZE)
                FAIL(LT48_UNSUPPORTED);
            if (op == 0xf000u) {
                REQUIRE(3u);
                status=lt48_epm_legacy_step_internal(words[pc+1],words[pc+2],result->buffers,
                    result->variables,result->variable_count,result->device_image);
                if(status!=LT48_OK)goto fail;
                pc+=3;
            } else {
                REQUIRE(2u);
                unsigned row=result->buffers[1][0]|(unsigned)result->buffers[1][1]<<8;
                size_t next=(op==0xf003u ? row>=128 : (row<88 || row==124))?pc+2:words[pc+1];
                if(next>=word_count)FAIL(LT48_FORMAT_ERROR);
                pc=next;
            }
        } else if (op == 0x25u) {
            uint64_t arguments[1];
            REQUIRE(2u);
            arguments[0] = words[pc + 1u];
            for (size_t index = 0; index < options->nested_wave_count; ++index) {
                const lt48_runtime_nested_wave *callee = &options->nested_waves[index];
                size_t count;
                const uint16_t *child;
                uint64_t entry[2] = {10000, 1};
                if (callee->operation != arguments[0]) continue;
                if (depth == sizeof(stack) / sizeof(stack[0])) FAIL(LT48_RUNTIME_ERROR);
                child = lt48_compiled_wave_words(callee->compiled, &count);
                if (!child || !count || child[0] != count - 1u) FAIL(LT48_RUNTIME_ERROR);
                stack[depth].words = words; stack[depth].word_count = word_count;
                stack[depth++].pc = pc + 2;
                words = child; word_count = count; pc = 1;
                interpreter_state = 1;
                APPEND(LT48_RUNTIME_WAVE_WATCHDOG, entry, 2);
                goto next_instruction;
            }
            if (options->host_gal_pes && arguments[0] == options->host_gal_pes->operation) {
                const lt48_gal_pes_binding *binding = options->host_gal_pes;
                const lt48_compile_variable *variable;
                uint8_t manufacturer = 0, device = 0, old_index = 0;
                lt48_gal_pes_result decoded;
                uint64_t host_arguments[4];
                if (procedure_boundaries) {
                    uint64_t entry[2] = {10000, interpreter_state};
                    APPEND(LT48_RUNTIME_WAVE_WATCHDOG, entry, 2);
                }
                if (binding->data_in_buffer >= LT48_RUNTIME_BUFFER_COUNT)
                    FAIL(LT48_INVALID_ARGUMENT);
                if (binding->kind != LT48_GAL_PES_ACCESS_PLSI) {
                    status = find_variable(variables, variable_count, binding->manufacturer_variable, &variable);
                    if (status != LT48_OK || variable->kind != 2) FAIL(LT48_FORMAT_ERROR);
                    if (variable->value > UINT8_MAX) FAIL(LT48_FORMAT_ERROR);
                    manufacturer = (uint8_t)variable->value;
                }
                if (binding->kind == LT48_GAL_PES_ACCESS_LV &&
                    result->buffers[binding->data_in_buffer][3] == manufacturer) {
                    status = find_variable(variables, variable_count, binding->device_variable, &variable);
                    if (status != LT48_OK || variable->kind != 2) FAIL(LT48_FORMAT_ERROR);
                    if (variable->value > UINT8_MAX) FAIL(LT48_FORMAT_ERROR);
                    device = (uint8_t)variable->value;
                }
                if (binding->kind == LT48_GAL_PES_ACCESS &&
                    result->buffers[binding->data_in_buffer][3] == manufacturer &&
                    result->buffers[binding->data_in_buffer][1] != 5) {
                    status = find_variable(variables, variable_count, binding->old_parameter_variable, &variable);
                    if (status != LT48_OK || variable->kind != 2) FAIL(LT48_FORMAT_ERROR);
                    if (variable->value > UINT8_MAX) FAIL(LT48_FORMAT_ERROR);
                    old_index = (uint8_t)variable->value;
                }
                status = lt48_gal_pes_decode(binding->kind,
                    result->buffers[binding->data_in_buffer], LT48_RUNTIME_BUFFER_SIZE,
                    manufacturer, device, old_index, &decoded);
                if (status != LT48_OK && status != LT48_DEVICE_MISMATCH) goto fail;
                host_arguments[0] = binding->operation;
                host_arguments[1] = binding->kind;
                host_arguments[2] = decoded.updated_parameters;
                host_arguments[3] = decoded.original_status;
                APPEND(LT48_RUNTIME_HOST_GAL_PES, host_arguments, 4);
                if (decoded.original_status) {
                    uint64_t failure[] = {decoded.original_status, 0};
                    uint64_t returned[] = {0};
                    APPEND(LT48_RUNTIME_RESULT_LATCH, failure, 2);
                    APPEND(LT48_RUNTIME_RETURN_STATUS, returned, 1);
                    goto complete;
                }
                status = lt48_gal_pes_apply(&decoded, binding->parameter_variables,
                    result->variables, result->variable_count);
                if (status != LT48_OK) goto fail;
            } else if (options->use_host_bitsort &&
                arguments[0] == options->host_bitsort_operation) {
                unsigned char transposed[8] = {0};
                uint8_t buffer = options->host_bitsort_buffer;
                uint64_t host_arguments[2] = {arguments[0], buffer};
                if (procedure_boundaries) {
                    uint64_t entry[2] = {10000, interpreter_state};
                    APPEND(LT48_RUNTIME_WAVE_WATCHDOG, entry, 2);
                }
                if (buffer >= LT48_RUNTIME_BUFFER_COUNT)
                    FAIL(LT48_INVALID_ARGUMENT);
                for (unsigned bit = 0u; bit < 8u; ++bit)
                    for (unsigned byte = 0u; byte < 8u; ++byte)
                        transposed[bit] |=
                            ((result->buffers[buffer][byte] >> bit) & 1u)
                            << (7u - byte);
                memcpy(result->buffers[buffer], transposed, sizeof(transposed));
                APPEND(LT48_RUNTIME_HOST_BITSORT, host_arguments, 2u);
            } else {
                APPEND(LT48_RUNTIME_EXECUTE_WAVE, arguments, 1u);
            }
            pc += 2u;
        } else if (op == 0x26u || op == 0x27u) {
            uint16_t buffer_id;
            uint16_t offset;
            uint64_t arguments[1];
            REQUIRE(3u);
            buffer_id = words[pc + 1u];
            offset = words[pc + 2u];
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT ||
                offset >= LT48_RUNTIME_BUFFER_SIZE) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            arguments[0] = result->buffers[buffer_id][offset];
            APPEND(op == 0x26u ? LT48_RUNTIME_Q2 : LT48_RUNTIME_Q1,
                   arguments, 1u);
            pc += 3u;
        } else if (op == 0x28u) {
            uint16_t buffer_id;
            uint16_t byte_count;
            size_t payload_words;
            size_t index;
            REQUIRE(3u);
            buffer_id = words[pc + 1u];
            byte_count = words[pc + 2u];
            payload_words = ((size_t)byte_count + 1u) / 2u;
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT ||
                byte_count > LT48_RUNTIME_BUFFER_SIZE) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            REQUIRE(3u + payload_words);
            for (index = 0; index < byte_count; ++index) {
                uint16_t packed = words[pc + 3u + index / 2u];
                result->buffers[buffer_id][index] =
                    (unsigned char)(packed >> ((index & 1u) * 8u));
            }
            pc += 3u + payload_words;
        } else if (op == 0x2bu || op == 0x2cu || op == 0x31u || op == 0x32u) {
            /* Original 00441502..0044160c: byte-sized index/count wrap.
             * Long form uses value bit0; single-bit form tests nonzero. */
            int single = op == 0x31u || op == 0x32u;
            int indirect = op == 0x2bu || op == 0x31u;
            uint16_t buffer_id, index_operand;
            uint8_t bit, value, count;
            REQUIRE(single ? 4u : 5u);
            buffer_id = words[pc + 1u];
            index_operand = words[pc + 2u];
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT ||
                (indirect && index_operand >= LT48_RUNTIME_BUFFER_COUNT))
                FAIL(LT48_RUNTIME_ERROR);
            bit = indirect ? result->buffers[index_operand][0] : (uint8_t)index_operand;
            value = (uint8_t)words[pc + 3u];
            count = single ? 1u : (uint8_t)words[pc + 4u];
            for (unsigned i = 0u; i < count; ++i, ++bit) {
                unsigned char mask = (unsigned char)(1u << (bit & 7u));
                if (single ? value != 0u : (value & 1u) != 0u)
                    result->buffers[buffer_id][bit >> 3] |= mask;
                else
                    result->buffers[buffer_id][bit >> 3] &= (unsigned char)~mask;
            }
            pc += single ? 4u : 5u;
        } else if (op == 0x29u) {
            uint16_t buffer_id;
            uint16_t length;
            uint16_t variable_id;
            uint64_t value;
            uint64_t arguments[4];
            size_t index;
            REQUIRE(4u);
            buffer_id = words[pc + 1u];
            length = words[pc + 2u];
            variable_id = words[pc + 3u];
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT ||
                length > LT48_RUNTIME_BUFFER_SIZE) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            status = evaluate_variable(variables, variable_count, variable_id,
                                       &value);
            if (status != LT48_OK) goto fail;
            if ((length == 0u && value != 0u) ||
                (length < 8u && value >= (UINT64_C(1) << (length * 8u)))) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            for (index = 0; index < length; ++index) {
                result->buffers[buffer_id][index] =
                    index < 8u ? (unsigned char)(value >> (index * 8u)) : 0u;
            }
            arguments[0] = variable_id;
            arguments[1] = value;
            arguments[2] = buffer_id;
            arguments[3] = length;
            APPEND(LT48_RUNTIME_EVALUATE_VARIABLE, arguments, 4u);
            pc += 4u;
        } else if ((op >= 0x2du && op <= 0x30u) || op == 0x33u || op == 0x77u) {
            uint16_t destination_id;
            uint16_t destination_bit;
            uint16_t source_id;
            uint16_t source_bit;
            uint16_t bit_count;
            unsigned char source[LT48_RUNTIME_BUFFER_SIZE];
            uint64_t arguments[5];
            REQUIRE(op == 0x33u ? 5u : 6u);
            destination_id = words[pc + 1u];
            destination_bit = words[pc + 2u] & 0xffu;
            source_id = words[pc + 3u];
            source_bit = words[pc + 4u] & 0xffu;
            bit_count = op == 0x33u ? 1u : words[pc + 5u];
            /* Compact99/expanded77 keeps both offsets immediate and reads
             * only the count from its buffer (original00441778).
             * Original 004433d0 snapshots the source, using byte-sized dynamic
             * offsets/count. Reject zero and out-of-bounds before its unsafe loop. */
            if((op>=0x2eu && op<=0x30u) || op==0x77u) {
                if(bit_count>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
                bit_count=result->buffers[bit_count][0];
                if(!bit_count)FAIL(LT48_RUNTIME_ERROR);
                if(op==0x2eu || op==0x30u){if(destination_bit>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);destination_bit=result->buffers[destination_bit][0];}
                if(op==0x2fu || op==0x30u){if(source_bit>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);source_bit=result->buffers[source_bit][0];}
            }
            if (destination_id >= LT48_RUNTIME_BUFFER_COUNT ||
                source_id >= LT48_RUNTIME_BUFFER_COUNT ||
                destination_bit + bit_count > LT48_RUNTIME_BUFFER_SIZE * 8u ||
                source_bit + bit_count > LT48_RUNTIME_BUFFER_SIZE * 8u) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            memcpy(source, result->buffers[source_id], sizeof(source));
            for (size_t index = 0u; index < bit_count; ++index) {
                unsigned value = (source[(source_bit + index) / 8u] >>
                                  ((source_bit + index) & 7u)) & 1u;
                unsigned char *destination =
                    &result->buffers[destination_id]
                        [(destination_bit + index) / 8u];
                unsigned char mask = (unsigned char)(
                    1u << ((destination_bit + index) & 7u));
                *destination = value ?
                    (unsigned char)(*destination | mask) :
                    (unsigned char)(*destination & (unsigned char)~mask);
            }
            arguments[0] = destination_id;
            arguments[1] = destination_bit;
            arguments[2] = source_id;
            arguments[3] = source_bit;
            arguments[4] = bit_count;
            APPEND(LT48_RUNTIME_BUFFER_BIT_COPY, arguments, 5u);
            pc += op == 0x33u ? 5u : 6u;
        } else if (op == 0x34u || op == 0x73u || op == 0x74u || op == 0x75u) {
            uint16_t destination_offset;
            uint16_t source_offset;
            uint16_t source_id;
            uint16_t destination_id;
            uint16_t length;
            uint64_t arguments[5];
            REQUIRE(6u);
            destination_id = words[pc + 1u];
            destination_offset = words[pc + 2u] & 0xffu;
            source_id = words[pc + 3u];
            source_offset = words[pc + 4u] & 0xffu;
            length = words[pc + 5u] & 0xffu;
            /* Original 004417e5..00441827: indexed destination/source
             * offsets use the index buffer's LOW BYTE. Both are memmove. */
            if(op==0x73u){
                if(words[pc+2]>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
                destination_offset=result->buffers[words[pc+2]][0];
            }
            if(op==0x74u){
                if(words[pc+4]>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
                source_offset=result->buffers[words[pc+4]][0];
            }
            if (op == 0x75u) {
                /* 004405d0 case75: offsets and length are the LOW BYTE of
                 * three buffers, not their dwords (factory data may remain
                 * above the overwritten low bytes). */
                if (words[pc+2] >= LT48_RUNTIME_BUFFER_COUNT ||
                    words[pc+4] >= LT48_RUNTIME_BUFFER_COUNT ||
                    words[pc+5] >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
                destination_offset = result->buffers[words[pc+2]][0];
                source_offset = result->buffers[words[pc+4]][0];
                length = result->buffers[words[pc+5]][0];
            }
            if (source_id >= LT48_RUNTIME_BUFFER_COUNT ||
                destination_id >= LT48_RUNTIME_BUFFER_COUNT ||
                source_offset + length > LT48_RUNTIME_BUFFER_SIZE ||
                destination_offset + length > LT48_RUNTIME_BUFFER_SIZE) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            memmove(result->buffers[destination_id] + destination_offset,
                    result->buffers[source_id] + source_offset, length);
            arguments[0] = destination_id;
            arguments[1] = destination_offset;
            arguments[2] = source_id;
            arguments[3] = source_offset;
            arguments[4] = length;
            APPEND(LT48_RUNTIME_BUFFER_MEMMOVE, arguments, 5u);
            pc += 6u;
        } else if (op == 0x35u || op == 0x36u) {
            uint16_t destination_id;
            uint16_t length;
            uint16_t source_id;
            uint16_t operation;
            unsigned char source[LT48_RUNTIME_BUFFER_SIZE];
            uint64_t arguments[4];
            REQUIRE(op == 0x35u ? 5u : 6u);
            destination_id = words[pc + 2u];
            length = words[pc + (op == 0x35u ? 4u : 5u)] & 0xffu;
            source_id = op == 0x35u ? words[pc + 3u] : UINT16_MAX;
            operation = words[pc + 1u] & 0xffu;
            if (destination_id >= LT48_RUNTIME_BUFFER_COUNT ||
                (op == 0x35u && source_id >= LT48_RUNTIME_BUFFER_COUNT) ||
                length > LT48_RUNTIME_BUFFER_SIZE) FAIL(LT48_RUNTIME_ERROR);
            if (op == 0x35u) memcpy(source, result->buffers[source_id], length);
            else {
                /* Original source points into the compiled little-endian
                 * word stream, including the operation word and zero padding. */
                for (size_t i = 0u; i < length; ++i) {
                    size_t word = pc + 3u + i / 2u;
                    source[i] = word < word_count ?
                        (unsigned char)(words[word] >> ((i & 1u) * 8u)) : 0u;
                }
            }
            for (size_t index = 0u; index < length; ++index) {
                if (operation == 1u)
                    result->buffers[destination_id][index] &= source[index];
                else if (operation == 2u)
                    result->buffers[destination_id][index] |= source[index];
                else if (operation == 3u)
                    result->buffers[destination_id][index] ^= source[index];
                else
                    result->buffers[destination_id][index] = source[index];
            }
            arguments[0] = operation;
            arguments[1] = destination_id;
            arguments[2] = source_id;
            arguments[3] = length;
            APPEND(LT48_RUNTIME_BUFFER_BYTE_OPERATION, arguments, 4u);
            pc += op == 0x35u ? 5u : 6u;
        } else if (op == 0x37u) {
            /* Dataman 13.10 441994: destination dword arithmetic with a
             * source buffer; division/remainder use x86 signed IDIV. */
            REQUIRE(4u);
            unsigned operation=words[pc+1]&255u,dst=words[pc+2],src=words[pc+3];
            if(dst>=LT48_RUNTIME_BUFFER_COUNT || src>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
            uint32_t a=load_dword(result->buffers[dst]),b=load_dword(result->buffers[src]),value=a;
            if(operation==0)value=a+b;
            else if(operation==1)value=a-b;
            else if(operation==2)value=a*b;
            else if(operation==3 || operation==4) {
                int64_t sa=a<=INT32_MAX?(int64_t)a:(int64_t)a-INT64_C(4294967296);
                int64_t sb=b<=INT32_MAX?(int64_t)b:(int64_t)b-INT64_C(4294967296);
                if(!sb || (sa==INT32_MIN && sb==-1))FAIL(LT48_RUNTIME_ERROR);
                value=(uint32_t)(operation==3?sa/sb:sa%sb);
            }
            store_dword(result->buffers[dst],value);
            uint64_t arguments[]={operation,dst,b,value};
            APPEND(LT48_RUNTIME_BUFFER_ARITHMETIC,arguments,4u);pc+=4u;
        } else if (op == 0x38u) {
            uint16_t arithmetic;
            uint16_t buffer_id;
            uint32_t constant;
            uint32_t current;
            uint32_t value;
            uint64_t arguments[4];
            REQUIRE(5u);
            arithmetic = words[pc + 1u];
            buffer_id = words[pc + 2u];
            constant = words[pc + 3u] | (uint32_t)words[pc + 4u] << 16;
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
            current = load_dword(result->buffers[buffer_id]);
            switch (arithmetic) {
            case 0u: value = current + constant; break;
            case 1u: value = current - constant; break;
            case 2u: value = current * constant; break;
            case 3u:
                if (!constant) FAIL(LT48_RUNTIME_ERROR);
                value = current / constant; break;
            case 4u:
                if (!constant) FAIL(LT48_RUNTIME_ERROR);
                value = current % constant; break;
            default: FAIL(LT48_RUNTIME_ERROR);
            }
            store_dword(result->buffers[buffer_id], value);
            arguments[0] = arithmetic;
            arguments[1] = buffer_id;
            arguments[2] = constant;
            arguments[3] = value;
            APPEND(LT48_RUNTIME_BUFFER_ARITHMETIC, arguments, 4u);
            pc += 5u;
        } else if (op == 0x39u || op == 0x3au) {
            uint16_t buffer_id;
            uint32_t current;
            uint32_t value;
            uint64_t arguments[4];
            REQUIRE(2u);
            buffer_id = words[pc + 1u];
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT)
                FAIL(LT48_RUNTIME_ERROR);
            current = load_dword(result->buffers[buffer_id]);
            value = op == 0x39u ? current + 1u : current - 1u;
            store_dword(result->buffers[buffer_id], value);
            arguments[0] = op == 0x39u ? 0u : 1u;
            arguments[1] = buffer_id;
            arguments[2] = current;
            arguments[3] = value;
            APPEND(LT48_RUNTIME_BUFFER_ARITHMETIC, arguments, 4u);
            pc += 2u;
        } else if (op == 0x43u) {
            REQUIRE(5u);
            uint16_t buffer=words[pc+4];
            if(buffer>=LT48_RUNTIME_BUFFER_COUNT || words[pc+2]>1u) FAIL(LT48_RUNTIME_ERROR);
            uint64_t args[4]={words[pc+1]&255u,words[pc+2],(uint32_t)words[pc+3]*56u,buffer};
            int missing=capture_response_index>=options->capture_response_size;
            if(!missing)store_dword(result->buffers[buffer],options->capture_response_bytes[capture_response_index++]);
            else if(options->require_capture_responses && !options->stop_at_missing_capture) FAIL(LT48_RUNTIME_ERROR);
            APPEND(LT48_RUNTIME_CONTROLLER_PIN_POLL,args,4);
            if(missing && options->stop_at_missing_capture)goto complete;
            pc+=5;
        } else if (op == 0x3fu || op == 0x40u) {
            lt48_pin_group group = {0};
            const uint16_t *table = NULL;
            size_t table_size = 0u, row = 0u, columns = 0u;
            REQUIRE(op == 0x3fu ? 4u : 2u);
            uint16_t group_id = words[pc + (op == 0x3fu ? 2u : 1u)];
            status = lt48_pin_map_find_group(pin_map, group_id, &group);
            /* The original allocates empty IO lists for unbound entries. */
            if (status == LT48_NOT_FOUND) { group.pins = NULL; group.pin_count = 0u; }
            else if (status != LT48_OK) goto fail;
            if (op == 0x3fu) {
                uint16_t buffer = words[pc + 1u], operation = words[pc + 3u];
                if (buffer >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
                for (size_t i = 0u; i < options->nested_wave_count; ++i)
                    if (options->nested_waves[i].operation == operation)
                        table = lt48_compiled_wave_words(options->nested_waves[i].compiled, &table_size);
                if (!table || table_size < 3u || table_size != (size_t)table[0] + 1u)
                    FAIL(LT48_UNSUPPORTED);
                row = result->buffers[buffer][0]; columns = table[2];
                if (row >= table[1] || group.pin_count > columns ||
                    table_size != 3u + (size_t)table[1] * columns)
                    FAIL(LT48_RUNTIME_ERROR);
            }
            for (size_t i = 0u; i < group.pin_count; ++i) {
                unsigned pin = group.pins[i];
                uint64_t packet[2] = {0x22u, pin >> 1u};
                uint64_t q2[1];
                unsigned control = op == 0x3fu ? table[3u + row * columns + i] : 0u;
                if (pin >= 0xfcu) continue;
                if (control > 3u) FAIL(LT48_UNSUPPORTED);
                if (op == 0x3fu && control == 3u) {
                    q2[0] = (pin + 2u) & 255u;
                    APPEND(LT48_RUNTIME_Q2, q2, 1u);
                    packet[1] = (pin >> 1u) + 1u;
                }
                APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE, packet, 2u);
                if (op == 0x3fu && control <= 2u) {
                    q2[0] = (pin + control) & 255u;
                    APPEND(LT48_RUNTIME_Q2, q2, 1u);
                }
            }
            pc += op == 0x3fu ? 4u : 2u;
        } else if (op == 0x41u) {
            /* Original443930: look up PAD[low-word index] and drive its
             * zero-extended word through transport +74. The index buffer is
             * unchanged. Bounds are explicit here; source has no such guard. */
            REQUIRE(4u);uint16_t group_id=words[pc+1],buffer=words[pc+2],role=words[pc+3];
            if(buffer>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
            const uint16_t*table=NULL;size_t count=0;lt48_pin_group group={0};
            for(size_t i=0;i<options->nested_wave_count;i++)if(options->nested_waves[i].operation==role) {
                if(table)FAIL(LT48_FORMAT_ERROR);
                table=lt48_compiled_wave_words(options->nested_waves[i].compiled,&count);
            }
            if(!table||!count||count!=(size_t)table[0]+1u)FAIL(LT48_FORMAT_ERROR);
            unsigned index=result->buffers[buffer][0]|(unsigned)result->buffers[buffer][1]<<8;
            if(index>=table[0])FAIL(LT48_SAFETY_REJECTED);
            status=lt48_pin_map_find_group(pin_map,group_id,&group);
            if(status==LT48_NOT_FOUND)group.pin_count=0;
            else if(status!=LT48_OK)goto fail;
            if(group.pin_count>32u)FAIL(LT48_SAFETY_REJECTED);
            uint32_t value=table[index+1u];uint64_t args[32];size_t n=0;
            for(size_t i=0;i<group.pin_count;i++)if(group.pins[i]<0xfcu)args[n++]=group.pins[i]+((value>>i)&1u);
            APPEND(LT48_RUNTIME_RAW_WRITE,args,n);pc+=4u;
        } else if (op == 0x42u) {
            /* Original compact57: replace the selected buffer's low word
             * with PAD[index]. Keep the remaining bytes unchanged. */
            REQUIRE(3u);
            uint16_t buffer=words[pc+1],operation=words[pc+2];
            const uint16_t *table=NULL;size_t count=0;
            if(buffer>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
            for(size_t i=0;i<options->nested_wave_count;i++)
                if(options->nested_waves[i].operation==operation)
                    table=lt48_compiled_wave_words(options->nested_waves[i].compiled,&count);
            if(!table||!count||count!=(size_t)table[0]+1)FAIL(LT48_FORMAT_ERROR);
            unsigned index=result->buffers[buffer][0]|(unsigned)result->buffers[buffer][1]<<8;
            if(index>=table[0])FAIL(LT48_SAFETY_REJECTED);
            uint16_t value=table[index+1];
            result->buffers[buffer][0]=(unsigned char)value;
            result->buffers[buffer][1]=(unsigned char)(value>>8);
            interpreter_state=1;
            pc+=3u;
        } else if (op == 0x3bu || op == 0x3cu) {
            /* Original 00443580: successive source bits scatter into the
             * low-byte destination indices of the selected map table. */
            const uint16_t *map = NULL;
            size_t map_size = 0u;
            unsigned char scratch[LT48_RUNTIME_BUFFER_SIZE] = {0};
            REQUIRE(3u);
            uint16_t buffer = words[pc + 1u], operation = words[pc + 2u];
            if (buffer >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
            for (size_t i = 0u; i < options->nested_wave_count; ++i)
                if (options->nested_waves[i].operation == operation)
                    map = lt48_compiled_wave_words(options->nested_waves[i].compiled, &map_size);
            if (!map || !map_size || map[0] > 256u || map_size != (size_t)map[0] + 1u)
                FAIL(LT48_UNSUPPORTED);
            for (size_t bit = 0u; bit < map[0]; ++bit) {
                if(op==0x3cu){
                    if(palce54_gather_bit(map[bit+1u],result->buffers[buffer]))
                        scratch[bit/8u]|=(unsigned char)(1u<<(bit&7u));
                    continue;
                }
                unsigned target = map[bit + 1u] & 255u;
                unsigned char mask = (unsigned char)(1u << (target % 8u));
                if (result->buffers[buffer][bit / 8u] & (1u << (bit % 8u)))
                    scratch[target / 8u] |= mask;
                else scratch[target / 8u] &= (unsigned char)~mask;
            }
            memcpy(result->buffers[buffer], scratch, sizeof(scratch));
            pc += 3u;
        } else if (op == 0x3du) {
            uint64_t arguments[3];
            uint16_t buffer_id;
            size_t count;
            REQUIRE(4u);
            buffer_id = words[pc + 1u];
            count = words[pc + 3u] & 0xffu;
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT ||
                count > LT48_RUNTIME_BUFFER_SIZE) FAIL(LT48_RUNTIME_ERROR);
            memset(result->buffers[buffer_id], words[pc + 2u] & 0xffu, count);
            arguments[0] = buffer_id;
            arguments[1] = words[pc + 2u] & 0xffu;
            arguments[2] = count;
            APPEND(LT48_RUNTIME_BUFFER_FILL, arguments, 3u);
            pc += 4u;
        } else if (op >= 0x65u && op <= 0x68u) {
            int immediate=(op==0x66u || op==0x68u);
            REQUIRE(immediate?5u:4u);
            if (!immediate && words[pc+1]>=LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
            uint32_t index=immediate ? (uint32_t)words[pc+1] | (uint32_t)words[pc+2]<<16 : load_dword(result->buffers[words[pc+1]]);
            unsigned buffer=words[pc+(immediate?3:2)], width=words[pc+(immediate?4:3)];
            if (!width) width=options->device_element_bytes;
            uint64_t offset=(uint64_t)index*width;
            if (!result->device_image || !width || width>32u || buffer>=LT48_RUNTIME_BUFFER_COUNT ||
                offset>result->device_image_size || width>result->device_image_size-offset) FAIL(LT48_RUNTIME_ERROR);
            if (op==0x65u || op==0x66u) memcpy(result->buffers[buffer],result->device_image+(size_t)offset,width);
            else memcpy(result->device_image+(size_t)offset,result->buffers[buffer],width);
            pc+=immediate?5u:4u;
        } else if (op==0x60u || op==0x61u) {
            REQUIRE(3u);
            unsigned buffer=words[pc+1], id=words[pc+2];
            if (buffer>=LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
            const lt48_compile_variable *v=NULL;
            if (find_variable(result->variables,result->variable_count,id,&v)!=LT48_OK) FAIL(LT48_RUNTIME_ERROR);
            /* Original variable object +4 holds the numeric value. In the
             * serialized/native time declaration, value is the unit selector
             * and channel holds that numeric value (0041f580). */
            if (op==0x60u) {
                uint32_t numeric = load_dword(result->buffers[buffer]);
                if (numeric > UINT16_MAX) FAIL(LT48_UNSUPPORTED);
                if (v->kind == 1u) result->variables[v-result->variables].channel=(uint16_t)numeric;
                else result->variables[v-result->variables].value=(uint16_t)numeric;
            } else store_dword(result->buffers[buffer],v->kind == 1u ? v->channel : v->value);
            pc+=3u;
        } else if ((op>=0x55u && op<=0x58u) || op==0x5au || op==0x5cu || (op>=0x7eu && op<=0x81u)) {
            int direct=op>=0x7eu, object=-1;
            if(direct && options->use_explicit_direct_configuration_object)object=options->direct_configuration_object;
            if(!direct && options->use_explicit_legacy_configuration_object)object=options->legacy_configuration_object;
            unsigned category,field,buffer; size_t length;
            REQUIRE(direct?4u:5u);
            if (direct) {
                category=words[pc+1];field=words[pc+2];buffer=words[pc+3];length=4u;
                if (op==0x7fu || op==0x81u) {
                    if (field>=LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
                    field=load_dword(result->buffers[field]);
                }
            } else {
                unsigned named=words[pc+1]; if(named>1u) FAIL(LT48_RUNTIME_ERROR);
                length=named?6u:5u; REQUIRE(length);
                if(named)object=words[pc+2];
                size_t base=pc+2u+named;
                category=words[base];field=words[base+1];buffer=words[base+2];
            }
            /* Original 443CD0/443D90: indexed fields use only the low
             * byte of their source buffer, including 57/58/5C. */
            if(op==0x57u || op==0x58u || op==0x5cu) {
                if(field>=LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
                field=result->buffers[field][0];
            }
            /* Direct option helpers 00444110/00444060 select a DWORD
             * bank with bits 8..15 and a bit with bits 0..4. The legacy
             * object helpers above address a single checkbox DWORD. */
            if(category==0u)field&=direct?0xff1fu:31u;
            int immediate=op==0x55u || op==0x57u;
            int writing=(op>=0x55u && op<=0x58u) || op==0x7eu || op==0x7fu;
            if(category>2u || field>65535u || (!immediate && buffer>=LT48_RUNTIME_BUFFER_COUNT)) FAIL(LT48_RUNTIME_ERROR);
            size_t found=SIZE_MAX;
            for(size_t i=0;i<result->configuration_count;i++) {
                const lt48_runtime_configuration_value *v=&result->configuration[i];
                if(v->object_id==object && v->category==category && v->field==field) {
                    if(found!=SIZE_MAX) FAIL(LT48_RUNTIME_ERROR);
                    found=i;
                }
            }
            if(found==SIZE_MAX) {
                if(!writing) FAIL(LT48_UNSUPPORTED);
                if(result->configuration_count==1024u) FAIL(LT48_RUNTIME_ERROR);
                found=result->configuration_count++;
                result->configuration[found]=(lt48_runtime_configuration_value){object,(uint16_t)category,(uint16_t)field,0};
            }
            if(writing) {
                uint32_t value=immediate?buffer:load_dword(result->buffers[buffer]);
                result->configuration[found].value=category==0u?!!value:category==1u?value&65535u:value;
            } else {
                uint32_t value=result->configuration[found].value;
                store_dword(result->buffers[buffer],category==0u?!!value:category==1u?value&65535u:value);
            }
            pc+=length;
        } else if (op == 0x82u) {
            /* Original44208A/442A30: display selected bytes on an identity
             * mismatch. Formatting/UI only; it does not change wave status. */
            REQUIRE(5u);
            unsigned buffer=words[pc+1u]&255u,n=words[pc+2u]&255u;
            unsigned message=words[pc+3u],level=words[pc+4u];
            if(buffer>=LT48_RUNTIME_BUFFER_COUNT || (n!=2u && n!=4u) ||
               (message!=4u && message!=5u) || level!=3u)FAIL(LT48_UNSUPPORTED);
            uint64_t args[7]={message,level,n,0,0,0,0};
            for(unsigned i=0;i<n;i++)args[3u+i]=result->buffers[buffer][i];
            APPEND(LT48_RUNTIME_HOST_NOTIFICATION,args,3u+n);pc+=5u;
        } else if (op == 0x70u) {
            /* Original interpreter441435 case70 calls44c200 with the
             * selected message and severity; its return does not alter the
             * procedure outcome. The factory notice and explicitly scoped M37 erase retry
             * notice, XCR source notice6/3 and MSP430 fuse notice9/0 are admitted. Other messages remain unsupported. */
            REQUIRE(3u);
            if((words[pc+1u]!=2u||words[pc+2u]!=2u)&&
               (words[pc+1u]!=6u||words[pc+2u]!=3u)&&
               (words[pc+1u]!=9u||words[pc+2u]!=0u)&&
               !(options->host_avr_parallel&&options->use_explicit_buffer_ids&&options->mode_buffer==7u&&
                 words[pc+1u]==8u&&words[pc+2u]==3u)&&
               !(options->host_m37_erase_notifications==1&&options->use_explicit_buffer_ids&&options->mode_buffer==2u&&
                 load_dword(result->buffers[2])==7u&&words[pc+1u]==3u&&words[pc+2u]==3u))FAIL(LT48_UNSUPPORTED);
            uint64_t arguments[]={words[pc+1u],words[pc+2u]};
            APPEND(LT48_RUNTIME_HOST_NOTIFICATION,arguments,2u);pc+=3u;
        } else if (op == 0x52u || op == 0x53u) {
            uint64_t arguments[2];
            REQUIRE(op == 0x52u ? 2u : 3u);
            arguments[0] = words[pc + 1u];
            if (op == 0x53u) arguments[1] = words[pc + 2u];
            APPEND(op == 0x52u ? LT48_RUNTIME_RETURN_STATUS :
                   LT48_RUNTIME_RESULT_LATCH, arguments, op == 0x52u ? 1u : 2u);
            if (op == 0x52u && !arguments[0]) goto complete;
            pc = op == 0x52u ? word_count : pc + 3u;
        } else if (op == 0x47u || op == 0x4fu) {
            /* A USB status exchange inside a <=60us MMI VCC pulse would
             * stretch it by host latency. Exact authored PAL profiles defer
             * it until VCC returns to nominal; the low-DAC trailing status
             * then flushes/checks the complete bounded controller packet. */
            if(!(op==0x4fu && options->host_pal_mmi_atomic &&
                 result->voltage[1]>=1150u && result->voltage[1]<=1200u))
                APPEND(op == 0x47u ? LT48_RUNTIME_START_ENGINE : LT48_RUNTIME_STATUS,
                       NULL, 0u);
            pc += 1u;
        } else if (op == 0x4au || op == 0x4bu) {
            uint16_t buffer_id;
            uint16_t comparator;
            uint32_t constant;
            uint32_t current;
            int condition;
            REQUIRE(7u);
            buffer_id = words[pc + 1u];
            comparator = words[pc + 3u] & 0xffu;
            if (op == 0x4au) {
                uint16_t right_id = words[pc + 4u];
                if (right_id >= LT48_RUNTIME_BUFFER_COUNT)
                    FAIL(LT48_RUNTIME_ERROR);
                constant = load_dword(result->buffers[right_id]);
            } else {
                constant = words[pc + 4u] |
                           (uint32_t)words[pc + 5u] << 16;
            }
            if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
            current = load_dword(result->buffers[buffer_id]);
            switch (comparator) {
            case 0u: condition = current == constant; break;
            case 1u: condition = current != constant; break;
            case 2u: condition = current < constant; break;
            case 3u: condition = current <= constant; break;
            case 4u: condition = constant < current; break;
            case 5u: condition = constant <= current; break;
            case 6u: condition = (constant & current) != 0u; break;
            case 7u: condition = (constant | current) != 0u; break;
            case 8u:
                if (!constant) FAIL(LT48_RUNTIME_ERROR);
                condition = current % constant != 0u; break;
            default: FAIL(LT48_RUNTIME_ERROR);
            }
            if (condition) {
                pc += 7u;
            } else {
                size_t placeholder = pc + 6u;
                status = branch_target(placeholder, displacement(words[placeholder]),
                                       word_count, &pc);
                if (status != LT48_OK) goto fail;
            }
        } else if (op == 0x4cu) {
            size_t placeholder;
            REQUIRE(2u);
            placeholder = pc + 1u;
            status = branch_target(placeholder, displacement(words[placeholder]),
                                   word_count, &pc);
            if (status != LT48_OK) goto fail;
        } else if (op == 0x50u) {
            unsigned char order[] = {3u, 2u, 1u, 0u};
            size_t left;
            /* 443A50 uses pairwise exchanges, not a stable sort: moving a
             * larger rail can reverse equal lower rails. Value 2 opts the
             * audited GAL owner into that exact source ordering. */
            if (options->host_staged_shutdown == 2 || options->host_staged_shutdown == 3 || options->host_staged_shutdown == 4) {
                for (left = 0; left < 4u; ++left)
                    for (size_t right = left + 1u; right < 4u; ++right)
                        if (shutdown_voltage(result->voltage[order[left]],order[left],options->host_staged_shutdown) <
                            shutdown_voltage(result->voltage[order[right]],order[right],options->host_staged_shutdown)) {
                            unsigned char swap = order[left];
                            order[left] = order[right]; order[right] = swap;
                        }
            } else for (left = 1u; left < 4u; ++left) {
                unsigned char item = order[left];
                size_t insertion = left;
                while (insertion > 0u &&
                       result->voltage[item] >
                       result->voltage[order[insertion - 1u]]) {
                    order[insertion] = order[insertion - 1u];
                    --insertion;
                }
                order[insertion] = item;
            }
            if(options->host_avr_parallel || options->host_staged_shutdown)for(left=0;left<4u;left++){
                unsigned channel=order[left];if(shutdown_voltage(result->voltage[channel],channel,options->host_staged_shutdown)<=500)continue;
                uint64_t args[]={channel,500,(channel?50u:100u)+(uint32_t[]){0,13,13,20}[channel]};
                APPEND(LT48_RUNTIME_VOLTAGE,args,3u);
            }
            for (left = 0; left < 4u; ++left) {
                uint64_t arguments[3];
                if (!shutdown_voltage(result->voltage[order[left]],order[left],options->host_staged_shutdown)) continue;
                arguments[0] = order[left];
                arguments[1] = 0u;
                arguments[2] = 0u;
                result->voltage[order[left]] = 0u;
                APPEND(LT48_RUNTIME_VOLTAGE, arguments, 3u);
            }
            pc += 1u;
        } else if (op == 0x6fu) {
            /* Compact8D: original438A60 selects a host-owned map section.
             * Storage is owned by the PAL session, not the transport. */
            REQUIRE(2u);
            unsigned section=words[pc+1u];
            if(options->fuse_section_count){
                if(section>=options->fuse_section_count)FAIL(LT48_SAFETY_REJECTED);
                result->fuse_section=section;result->has_fuse_section=1;
            }
            uint64_t arguments[]={section,options->fuse_section_count!=0};
            APPEND(LT48_RUNTIME_FUSE_SECTION_SELECT,arguments,2u);
            pc+=2u;
        } else if (op == 0x6bu || op == 0x6cu || op == 0x6du || op == 0x6eu) {
            const lt48_runtime_fuse_image *image = result->host_fuse_image.data ? &result->host_fuse_image : options->fuse_image;
            uint16_t row_buffer, column_buffer, destination;
            uint64_t row, column, offset = 0, arguments[6];
            REQUIRE(4u);
            row_buffer = words[pc + 1u]; column_buffer = words[pc + 2u];
            destination = words[pc + 3u];
            if (destination >= LT48_RUNTIME_BUFFER_COUNT) FAIL(LT48_RUNTIME_ERROR);
            /* Original expanded6C uses immediate coordinates; 6B takes them
             * from buffers. Both copy the selected host map's data field. */
            if(op==0x6cu || op==0x6eu){row=row_buffer;column=column_buffer;}
            else {
                if(row_buffer>=LT48_RUNTIME_BUFFER_COUNT || column_buffer>=LT48_RUNTIME_BUFFER_COUNT)FAIL(LT48_RUNTIME_ERROR);
                row=result->buffers[row_buffer][0]|(uint16_t)result->buffers[row_buffer][1]<<8;
                column=result->buffers[column_buffer][0]|(uint16_t)result->buffers[column_buffer][1]<<8;
            }
            if (image) {
                if (!image->data || !image->stride || !image->copy_width ||
                    image->copy_width > LT48_RUNTIME_BUFFER_SIZE ||
                    image->record_width != image->copy_width * 2u + 2u ||
                    image->size % image->record_width || column >= image->stride)
                    FAIL(LT48_FORMAT_ERROR);
                offset = row * image->stride + column;
                if (offset > SIZE_MAX / image->record_width) FAIL(LT48_SAFETY_REJECTED);
                offset *= image->record_width;
                if (offset > image->size || image->record_width > image->size - (size_t)offset)
                    FAIL(LT48_SAFETY_REJECTED);
                if(op==0x6du || op==0x6eu) {
                    /* Original 004425c0: read the whole record, replace only
                     * its data bytes, write the record back. Keep masks/type. */
                    if(!result->host_fuse_image.data) {
                        unsigned char *copy=malloc(image->size);
                        if(!copy)FAIL(LT48_NO_MEMORY);
                        memcpy(copy,image->data,image->size);
                        result->host_fuse_image=*image;
                        result->host_fuse_image.data=copy;
                        image=&result->host_fuse_image;
                    }
                    memcpy((unsigned char*)image->data+(size_t)offset+image->copy_width+2u,
                        result->buffers[destination],image->copy_width);
                } else memcpy(result->buffers[destination],image->data + (size_t)offset + image->copy_width + 2u,
                       image->copy_width);
            }
            arguments[0] = row; arguments[1] = column; arguments[2] = destination;
            arguments[3] = offset; arguments[4] = image ? image->copy_width : 0;
            arguments[5] = image != NULL;
            APPEND((op==0x6du||op==0x6eu)?LT48_RUNTIME_FUSE_IMAGE_WRITE:LT48_RUNTIME_FUSE_IMAGE_READ,arguments,6);
            pc += 4u;
        } else if (op == 0x4du) {
            /* Original 0049b7b0: compact 69 resets the timeout window,
             * in seconds with a five-second minimum, then calls 49e400. */
            REQUIRE(2u);
            uint32_t seconds=words[pc+1u];
            if(seconds>32767u) FAIL(LT48_SAFETY_REJECTED);
            if(seconds<5u)seconds=5u;
            uint64_t arguments[2]={seconds*1000u,1u};
            APPEND(LT48_RUNTIME_WAVE_WATCHDOG,arguments,2u);
            pc+=2u;
        } else if (op == 0x51u) {
            /* Original interpreter +44(009F8AD4,3): controller reset/idle. */
            uint64_t packet[3]={0x49,0xaa,0x55};
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,packet,3);
            pc++;
        } else if (op == 0x72u) {
            /* Original interpreter: transport +44(header,3), then
             * +44(selected embedded FPGA,0xada). Both use buffered writes. */
            REQUIRE(2u);
            const lt48_runtime_embedded_image *selected=NULL;
            if(options->embedded_image_count&&!options->embedded_images)FAIL(LT48_INVALID_ARGUMENT);
            for(size_t i=0;i<options->embedded_image_count;i++) {
                if(options->embedded_images[i].selector!=words[pc+1])continue;
                if(selected)FAIL(LT48_FORMAT_ERROR);
                selected=&options->embedded_images[i];
            }
            if(!selected)FAIL(LT48_UNSUPPORTED);
            if(!selected->data||selected->size!=0xada)FAIL(LT48_FORMAT_ERROR);
            uint64_t packet[LT48_RUNTIME_EVENT_ARGUMENTS]={0x38,0xaa,0x55};
            APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,packet,3);
            for(size_t at=0;at<selected->size;) {
                size_t count=selected->size-at;
                if(count>LT48_RUNTIME_EVENT_ARGUMENTS)count=LT48_RUNTIME_EVENT_ARGUMENTS;
                for(size_t i=0;i<count;i++)packet[i]=selected->data[at+i];
                APPEND(LT48_RUNTIME_CONTROLLER_COMMAND_WRITE,packet,count);
                at+=count;
            }
            pc+=2u;
        } else if (op == 0x5fu) {
            uint64_t arguments[1];
            REQUIRE(2u);
            if (words[pc + 1u] >= LT48_RUNTIME_BUFFER_COUNT) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            arguments[0] = words[pc + 1u];
            int missing_capture = capture_response_index >= options->capture_response_size;
            if (capture_response_index < options->capture_response_size) {
                result->buffers[words[pc + 1u]][0] =
                    options->capture_response_bytes[capture_response_index++];
            } else if (options->require_capture_responses && !options->stop_at_missing_capture) {
                FAIL(LT48_RUNTIME_ERROR);
            }
            APPEND(LT48_RUNTIME_CAPTURE_RESPONSE_BYTE, arguments, 1u);
            if (missing_capture && options->stop_at_missing_capture) {
                if(cursor&&cursor->streaming){cursor->valid=1;cursor->yielded=0;cursor->words=words;cursor->word_count=word_count;cursor->pc=pc;cursor->steps=steps-1u;cursor->capture_index=capture_response_index;cursor->depth=depth;memcpy(cursor->stack,stack,depth*sizeof(*stack));cursor->pin_loop=pin_loop;cursor->interpreter_state=interpreter_state;cursor->event_start=result->event_count-1u;}
                goto complete;
            }
            pc += 2u;
        } else {
            FAIL(LT48_RUNTIME_ERROR);
        }
        if (pc < 1u || pc > word_count) FAIL(LT48_RUNTIME_ERROR);
next_instruction:
        ;
    }
    if (options->require_capture_responses &&
        capture_response_index != options->capture_response_size) {
        FAIL(LT48_RUNTIME_ERROR);
    }
complete:
    if(cursor && !cursor->valid && pc<word_count && result->event_count){
        lt48_runtime_event_kind k=result->events[result->event_count-1u].kind;
        if(k==LT48_RUNTIME_CAPTURE_RESPONSE_BYTE || k==LT48_RUNTIME_CONTROLLER_PACKED_READ ||
           k==LT48_RUNTIME_CONTROLLER_PIN_READ || k==LT48_RUNTIME_CONTROLLER_PIN_POLL ||
           k==LT48_RUNTIME_RAW_READ){status=LT48_UNSUPPORTED;goto fail;}
    }
    *out_result = result;
    return LT48_OK;

fail:
    if(cursor){cursor->valid=0;*out_result=result;}
    else lt48_runtime_result_close(result);
    return status;
#undef FAIL
#undef REQUIRE
#undef APPEND
}

lt48_status lt48_runtime_interpret(const lt48_compiled_wave *compiled,
    const lt48_pin_map *pin_map,const lt48_compile_variable *variables,
    size_t variable_count,const lt48_runtime_options *options,
    lt48_runtime_result **out_result){
    return runtime_interpret_cursor(compiled,pin_map,variables,variable_count,
        options,out_result,NULL,NULL);
}

void lt48_runtime_result_close(lt48_runtime_result *result) {
    if (!result) return;
    free(result->events);
    free(result->variables);
    free(result->device_image);
    free((void*)result->host_fuse_image.data);
    free(result);
}

lt48_status lt48_runtime_host_fuse_image_internal(const lt48_runtime_result*r,const lt48_runtime_fuse_image**out) {
    if(!r||!out)return LT48_INVALID_ARGUMENT;
    *out=&r->host_fuse_image;
    return r->host_fuse_image.data?LT48_OK:LT48_NOT_FOUND;
}
lt48_status lt48_runtime_get_image(const lt48_runtime_result *result,const unsigned char **data,size_t *size) {
    if(!result || !data || !size)return LT48_INVALID_ARGUMENT;
    *data=result->device_image;*size=result->device_image_size;return LT48_OK;
}
lt48_status lt48_runtime_get_configuration(const lt48_runtime_result *result,
    const lt48_runtime_configuration_value **values,size_t *count) {
    if(!result || !values || !count)return LT48_INVALID_ARGUMENT;
    *values=result->configuration;*count=result->configuration_count;return LT48_OK;
}

lt48_status lt48_runtime_get_fuse_section(const lt48_runtime_result *result,unsigned *out_section){
    if(!result||!out_section)return LT48_INVALID_ARGUMENT;
    if(!result->has_fuse_section)return LT48_NOT_FOUND;
    *out_section=result->fuse_section;return LT48_OK;
}

lt48_status lt48_runtime_get_variable(
    const lt48_runtime_result *result, uint16_t identifier,
    lt48_compile_variable *out_variable
) {
    const lt48_compile_variable *found = NULL;
    if (!result || !out_variable) return LT48_INVALID_ARGUMENT;
    for (size_t index = 0; index < result->variable_count; ++index) {
        if (result->variables[index].variable_id != identifier) continue;
        if (found) return LT48_FORMAT_ERROR;
        found = &result->variables[index];
    }
    if (!found) return LT48_NOT_FOUND;
    *out_variable = *found;
    return LT48_OK;
}

size_t lt48_runtime_event_count(const lt48_runtime_result *result) {
    return result ? result->event_count : 0u;
}

lt48_status lt48_runtime_get_event(
    const lt48_runtime_result *result, size_t index,
    lt48_runtime_event *out_event
) {
    const runtime_event_owned *event;
    if (!result || !out_event) return LT48_INVALID_ARGUMENT;
    if (index >= result->event_count) return LT48_NOT_FOUND;
    event = &result->events[index];
    out_event->kind = event->kind;
    out_event->arguments = event->arguments;
    out_event->argument_count = event->argument_count;
    return LT48_OK;
}

lt48_status lt48_runtime_get_buffer(
    const lt48_runtime_result *result, uint8_t buffer_id,
    const unsigned char **out_data, size_t *out_size
) {
    if (!result || !out_data || !out_size) return LT48_INVALID_ARGUMENT;
    if (buffer_id >= LT48_RUNTIME_BUFFER_COUNT) return LT48_NOT_FOUND;
    *out_data = result->buffers[buffer_id];
    *out_size = LT48_RUNTIME_BUFFER_SIZE;
    return LT48_OK;
}

lt48_status lt48_runtime_get_voltage(
    const lt48_runtime_result *result, uint8_t channel,
    uint32_t *out_centivolts
) {
    if (!result || !out_centivolts) return LT48_INVALID_ARGUMENT;
    if (channel >= 4u) return LT48_NOT_FOUND;
    *out_centivolts = result->voltage[channel];
    return LT48_OK;
}

static int events_equal(const runtime_event_owned *left,
                        const runtime_event_owned *right) {
    return left->kind == right->kind &&
           left->argument_count == right->argument_count &&
           (!left->argument_count ||
            memcmp(left->arguments, right->arguments,
                   left->argument_count * sizeof(left->arguments[0])) == 0);
}

lt48_status lt48_runtime_machine_create(
    const lt48_compiled_wave *compiled,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    const lt48_runtime_options *options,
    lt48_runtime_machine **out_machine
) {
    lt48_runtime_machine *machine;
    size_t word_count;
    const uint16_t *words;
    if (!compiled || !pin_map || !options || !out_machine ||
        (variable_count && !variables) ||
        (options->parameter_size && !options->parameter_buffer) ||
        options->parameter_size > LT48_RUNTIME_BUFFER_SIZE ||
        (options->initial_buffer_count && !options->initial_buffers) ||
        options->capture_response_size) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_machine = NULL;
    words = lt48_compiled_wave_words(compiled, &word_count);
    if (!words || !word_count || words[0] != word_count - 1u) {
        return LT48_RUNTIME_ERROR;
    }
    machine = calloc(1u, sizeof(*machine));
    if (!machine) return LT48_NO_MEMORY;
    machine->result = calloc(1u, sizeof(*machine->result));
    if (!machine->result) {
        free(machine);
        return LT48_NO_MEMORY;
    }
    machine->compiled = compiled;
    machine->pin_map = pin_map;
    machine->variables = variables;
    machine->variable_count = variable_count;
    machine->options = *options;
    if(options->device_image_size) {
        if(!options->device_image || !options->device_element_bytes || options->device_element_bytes>32u) {
            lt48_runtime_machine_close(machine);return LT48_INVALID_ARGUMENT;
        }
        machine->device_image=malloc(options->device_image_size);
        if(!machine->device_image){lt48_runtime_machine_close(machine);return LT48_NO_MEMORY;}
        memcpy(machine->device_image,options->device_image,options->device_image_size);
        machine->options.device_image=machine->device_image;
    }
    if (options->configuration_count) {
        if (!options->configuration || options->configuration_count > 1024u) {
            lt48_runtime_machine_close(machine); return LT48_INVALID_ARGUMENT;
        }
        machine->configuration = malloc(options->configuration_count * sizeof(*machine->configuration));
        if (!machine->configuration) { lt48_runtime_machine_close(machine); return LT48_NO_MEMORY; }
        memcpy(machine->configuration, options->configuration,
               options->configuration_count * sizeof(*machine->configuration));
        machine->options.configuration = machine->configuration;
    }
    if (options->fuse_image) {
        if (options->fuse_image->size && !options->fuse_image->data) {
            lt48_runtime_machine_close(machine); return LT48_INVALID_ARGUMENT;
        }
        machine->fuse_image = *options->fuse_image;
        if (machine->fuse_image.size) {
            machine->fuse_image_data = malloc(machine->fuse_image.size);
            if (!machine->fuse_image_data) { lt48_runtime_machine_close(machine); return LT48_NO_MEMORY; }
            memcpy(machine->fuse_image_data,machine->fuse_image.data,machine->fuse_image.size);
        }
        machine->fuse_image.data = machine->fuse_image_data;
        machine->options.fuse_image = &machine->fuse_image;
    }
    if (options->nested_wave_count) {
        if (!options->nested_waves ||
            options->nested_wave_count > SIZE_MAX / sizeof(*machine->nested_waves)) {
            lt48_runtime_machine_close(machine); return LT48_INVALID_ARGUMENT;
        }
        machine->nested_waves = malloc(options->nested_wave_count * sizeof(*machine->nested_waves));
        if (!machine->nested_waves) {
            lt48_runtime_machine_close(machine); return LT48_NO_MEMORY;
        }
        memcpy(machine->nested_waves, options->nested_waves,
               options->nested_wave_count * sizeof(*machine->nested_waves));
        machine->options.nested_waves = machine->nested_waves;
    }
    if (options->host_gal_pes) {
        machine->gal_pes_binding = *options->host_gal_pes;
        machine->options.host_gal_pes = &machine->gal_pes_binding;
    }
    machine->options.capture_response_bytes = NULL;
    machine->options.capture_response_size = 0u;
    machine->options.require_capture_responses = 0;
    if (options->parameter_size) {
        memcpy(machine->parameter_buffer, options->parameter_buffer,
               options->parameter_size);
        machine->options.parameter_buffer = machine->parameter_buffer;
    }
    if (options->initial_buffer_count) {
        size_t total = options->initial_buffer_count * LT48_RUNTIME_BUFFER_SIZE;
        machine->initial_buffers = calloc(options->initial_buffer_count,
                                          sizeof(*machine->initial_buffers));
        machine->initial_buffer_data = calloc(total, 1u);
        if (!machine->initial_buffers || !machine->initial_buffer_data) {
            lt48_runtime_machine_close(machine);
            return LT48_NO_MEMORY;
        }
        for (size_t index = 0u; index < options->initial_buffer_count; ++index) {
            if (options->initial_buffers[index].size > LT48_RUNTIME_BUFFER_SIZE ||
                (options->initial_buffers[index].size &&
                 !options->initial_buffers[index].data)) {
                lt48_runtime_machine_close(machine);
                return LT48_INVALID_ARGUMENT;
            }
            machine->initial_buffers[index] = options->initial_buffers[index];
            machine->initial_buffers[index].data = machine->initial_buffer_data +
                index * LT48_RUNTIME_BUFFER_SIZE;
            memcpy((unsigned char *)machine->initial_buffers[index].data,
                   options->initial_buffers[index].data,
                   options->initial_buffers[index].size);
        }
        machine->options.initial_buffers = machine->initial_buffers;
    }
    machine->state = LT48_RUNTIME_MACHINE_READY;
    *out_machine = machine;
    return LT48_OK;
}

void lt48_runtime_machine_close(lt48_runtime_machine *machine) {
    if (!machine) return;
    lt48_runtime_result_close(machine->result);
    free(machine->responses);
    free(machine->initial_buffer_data);
    free(machine->initial_buffers);
    free(machine->nested_waves);
    free(machine->fuse_image_data);
    free(machine->device_image);
    free(machine->configuration);
    free(machine);
}

lt48_status lt48_runtime_machine_pin_resume_internal(lt48_runtime_machine *machine){
    if(!machine || machine->state!=LT48_RUNTIME_MACHINE_READY ||
       !machine->result || machine->result->event_count || machine->response_count)return LT48_INVALID_ARGUMENT;
    machine->resume_pin_captures=1;
    return LT48_OK;
}

lt48_status lt48_runtime_machine_stream_resume_internal(lt48_runtime_machine*machine){
    lt48_status st=lt48_runtime_machine_pin_resume_internal(machine);if(!st)machine->cursor.streaming=1;return st;
}

static lt48_status machine_advance_pin_cursor(lt48_runtime_machine *machine,
    lt48_runtime_machine_state *out_state){
    if(machine->resume_error!=LT48_OK)return machine->resume_error;
    size_t previous=machine->result->event_count;
    int continuing=machine->cursor.valid;
    runtime_event_owned request={0};int capture_resume=continuing&&!machine->cursor.yielded;
    if(continuing&&machine->cursor.streaming){
        if(capture_resume){if(!previous||machine->cursor.event_start!=previous-1u)return LT48_RUNTIME_ERROR;machine->result->events[0]=machine->result->events[previous-1u];previous=1;machine->result->event_count=1;machine->cursor.event_start=0;}
        else {previous=0;machine->result->event_count=0;machine->cursor.event_start=0;}
    }
    if(capture_resume){
        if(!previous || machine->cursor.event_start!=previous-1u)return LT48_RUNTIME_ERROR;
        request=machine->result->events[previous-1u];
    }
    lt48_runtime_result *old=machine->result,*run=NULL;
    machine->options.capture_response_bytes=machine->responses;
    machine->options.capture_response_size=machine->response_count;
    machine->options.stop_at_missing_capture=1;
    lt48_status status=runtime_interpret_cursor(machine->compiled,machine->pin_map,
        machine->variables,machine->variable_count,&machine->options,&run,
        continuing?old:NULL,&machine->cursor);
    if(run){if(!continuing)lt48_runtime_result_close(old);machine->result=run;}
    if(status==LT48_OK && (run->event_count<previous ||
       (capture_resume && !events_equal(&request,&run->events[previous-1u]))))status=LT48_RUNTIME_ERROR;
    if(status!=LT48_OK){machine->resume_error=status;return status;}
    machine->last_event_first=previous;machine->last_event_count=run->event_count-previous;
    if(machine->cursor.valid){
        if(machine->cursor.yielded)machine->state=LT48_RUNTIME_MACHINE_READY;
        else {const runtime_event_owned*capture=&run->events[run->event_count-1u];
            if(capture->kind==LT48_RUNTIME_CAPTURE_RESPONSE_BYTE&&capture->argument_count==1){machine->capture_buffer=(uint8_t)capture->arguments[0];machine->capture_length=1;}
            else if(capture->kind==LT48_RUNTIME_CONTROLLER_PIN_READ&&capture->argument_count>=3){machine->capture_buffer=(uint8_t)capture->arguments[0];machine->capture_length=(capture->arguments[1]+7u)/8u;}
            else if(capture->kind==LT48_RUNTIME_CONTROLLER_PACKED_READ&&capture->argument_count==6){machine->capture_buffer=(uint8_t)capture->arguments[4];machine->capture_length=capture->arguments[5];}
            else return LT48_RUNTIME_ERROR;
            machine->state=LT48_RUNTIME_MACHINE_AWAITING_CAPTURE;
        }
    }else machine->state=LT48_RUNTIME_MACHINE_FINISHED;
    *out_state=machine->state;return LT48_OK;
}

lt48_status lt48_runtime_machine_advance(
    lt48_runtime_machine *machine,
    lt48_runtime_machine_state *out_state
) {
    lt48_runtime_result *run = NULL;
    size_t capture_ordinal = 0u;
    size_t stop = 0u;
    size_t index;
    size_t previous_count;
    int found_capture = 0;
    lt48_status status;
    if (!machine || !out_state) return LT48_INVALID_ARGUMENT;
    previous_count = machine->result->event_count;
    if (machine->state == LT48_RUNTIME_MACHINE_AWAITING_CAPTURE) {
        return LT48_BUSY;
    }
    if (machine->state == LT48_RUNTIME_MACHINE_FINISHED) {
        *out_state = machine->state;
        machine->last_event_first = machine->result->event_count;
        machine->last_event_count = 0u;
        return LT48_OK;
    }
    if(machine->resume_pin_captures)return machine_advance_pin_cursor(machine,out_state);
    machine->options.capture_response_bytes = machine->responses;
    machine->options.capture_response_size = machine->response_count;
    machine->options.stop_at_missing_capture = 1;
    status = lt48_runtime_interpret(
        machine->compiled, machine->pin_map, machine->variables,
        machine->variable_count, &machine->options, &run
    );
    if (status != LT48_OK) return status;
    if (run->event_count < machine->result->event_count) {
        status = LT48_RUNTIME_ERROR;
        goto cleanup;
    }
    for (index = 0; index < machine->result->event_count; ++index) {
        if (!events_equal(&machine->result->events[index],
                          &run->events[index])) {
            status = LT48_RUNTIME_ERROR;
            goto cleanup;
        }
    }
    for (index = 0; index < run->event_count; ++index) {
        if (run->events[index].kind == LT48_RUNTIME_CAPTURE_RESPONSE_BYTE ||
            run->events[index].kind == LT48_RUNTIME_CONTROLLER_PACKED_READ ||
            run->events[index].kind == LT48_RUNTIME_RAW_READ ||
            run->events[index].kind == LT48_RUNTIME_CONTROLLER_PIN_READ ||
            run->events[index].kind == LT48_RUNTIME_CONTROLLER_PIN_POLL) {
            if (capture_ordinal == machine->response_count) {
                stop = index + 1u;
                if (run->events[index].kind ==
                    LT48_RUNTIME_CAPTURE_RESPONSE_BYTE) {
                    machine->capture_buffer =
                        (uint8_t)run->events[index].arguments[0];
                    machine->capture_length = 1u;
                } else if(run->events[index].kind==LT48_RUNTIME_CONTROLLER_PIN_POLL) {
                    machine->capture_buffer=(uint8_t)run->events[index].arguments[3];machine->capture_length=1;
                } else if (run->events[index].kind == LT48_RUNTIME_CONTROLLER_PIN_READ) {
                    machine->capture_buffer = (uint8_t)run->events[index].arguments[0];
                    machine->capture_length = (run->events[index].arguments[1] + 7u) / 8u;
                } else {
                    machine->capture_buffer =
                        (uint8_t)run->events[index].arguments[4];
                    machine->capture_length =
                        (size_t)run->events[index].arguments[5];
                }
                found_capture = 1;
                break;
            }
            capture_ordinal += (run->events[index].kind ==
                LT48_RUNTIME_CAPTURE_RESPONSE_BYTE || run->events[index].kind==LT48_RUNTIME_CONTROLLER_PIN_POLL) ? 1u :
                run->events[index].kind == LT48_RUNTIME_CONTROLLER_PIN_READ ?
                (run->events[index].arguments[1] + 7u) / 8u :
                (size_t)run->events[index].arguments[5];
        }
    }
    if (!found_capture) {
        if (capture_ordinal != machine->response_count) {
            status = LT48_RUNTIME_ERROR;
            goto cleanup;
        }
        stop = run->event_count;
    }
    for (index = previous_count; index < stop; ++index) {
        status = append_event(machine->result, run->events[index].kind,
                              run->events[index].arguments,
                              run->events[index].argument_count);
        if (status != LT48_OK) goto cleanup;
    }
    machine->last_event_first = previous_count;
    machine->last_event_count = stop - previous_count;
    if (found_capture) {
        machine->state = LT48_RUNTIME_MACHINE_AWAITING_CAPTURE;
    } else {
        memcpy(machine->result->buffers, run->buffers,
               sizeof(machine->result->buffers));
        memcpy(machine->result->voltage, run->voltage,
               sizeof(machine->result->voltage));
        free(machine->result->variables);
        machine->result->variables = run->variables;
        machine->result->variable_count = run->variable_count;
        run->variables = NULL;
        free(machine->result->device_image);
        machine->result->device_image=run->device_image;
        machine->result->device_image_size=run->device_image_size;
        run->device_image=NULL;
        free((void*)machine->result->host_fuse_image.data);
        machine->result->host_fuse_image=run->host_fuse_image;
        run->host_fuse_image.data=NULL;
        machine->result->fuse_section=run->fuse_section;
        machine->result->has_fuse_section=run->has_fuse_section;
        machine->result->configuration_count=run->configuration_count;
        memcpy(machine->result->configuration,run->configuration,run->configuration_count*sizeof(*run->configuration));
        machine->state = LT48_RUNTIME_MACHINE_FINISHED;
    }
    *out_state = machine->state;
    status = LT48_OK;
cleanup:
    if (status != LT48_OK) machine->result->event_count = previous_count;
    lt48_runtime_result_close(run);
    return status;
}

lt48_runtime_machine_state lt48_runtime_machine_get_state(
    const lt48_runtime_machine *machine
) {
    return machine ? machine->state : 0;
}

lt48_status lt48_runtime_machine_capture_buffer(
    const lt48_runtime_machine *machine, uint8_t *out_buffer
) {
    if (!machine || !out_buffer) return LT48_INVALID_ARGUMENT;
    if (machine->state != LT48_RUNTIME_MACHINE_AWAITING_CAPTURE) {
        return LT48_BUSY;
    }
    *out_buffer = machine->capture_buffer;
    return LT48_OK;
}

lt48_status lt48_runtime_machine_capture_request(
    const lt48_runtime_machine *machine, uint8_t *out_buffer,
    size_t *out_length
) {
    if (!machine || !out_buffer || !out_length) return LT48_INVALID_ARGUMENT;
    if (machine->state != LT48_RUNTIME_MACHINE_AWAITING_CAPTURE)
        return LT48_BUSY;
    *out_buffer = machine->capture_buffer;
    *out_length = machine->capture_length;
    return LT48_OK;
}

lt48_status lt48_runtime_machine_supply_capture(
    lt48_runtime_machine *machine, uint8_t value
) {
    return lt48_runtime_machine_supply_capture_bytes(machine, &value, 1u);
}

lt48_status lt48_runtime_machine_supply_capture_bytes(
    lt48_runtime_machine *machine, const unsigned char *values, size_t length
) {
    if (!machine) return LT48_INVALID_ARGUMENT;
    if (machine->state != LT48_RUNTIME_MACHINE_AWAITING_CAPTURE) {
        return LT48_BUSY;
    }
    if ((length && !values) || length != machine->capture_length)
        return LT48_INVALID_ARGUMENT;
    if (machine->response_count > SIZE_MAX - length) return LT48_NO_MEMORY;
    if (machine->response_count + length > machine->response_capacity) {
        size_t capacity = machine->response_capacity ?
                          machine->response_capacity * 2u : 4u;
        unsigned char *responses;
        while (capacity < machine->response_count + length) {
            if (capacity > SIZE_MAX / 2u) return LT48_NO_MEMORY;
            capacity *= 2u;
        }
        responses = realloc(machine->responses, capacity);
        if (!responses) return LT48_NO_MEMORY;
        machine->responses = responses;
        machine->response_capacity = capacity;
    }
    memcpy(machine->responses + machine->response_count, values, length);
    machine->response_count += length;
    machine->state = LT48_RUNTIME_MACHINE_READY;
    return LT48_OK;
}

const lt48_runtime_result *lt48_runtime_machine_result(
    const lt48_runtime_machine *machine
) {
    return machine ? machine->result : NULL;
}

void lt48_runtime_machine_last_event_range(
    const lt48_runtime_machine *machine, size_t *out_first, size_t *out_count
) {
    if (out_first) *out_first = machine ? machine->last_event_first : 0u;
    if (out_count) *out_count = machine ? machine->last_event_count : 0u;
}

lt48_status lt48_runtime_machine_take_result(
    lt48_runtime_machine *machine, lt48_runtime_result **out_result
) {
    if (!machine || !out_result) return LT48_INVALID_ARGUMENT;
    *out_result = NULL;
    if (machine->state != LT48_RUNTIME_MACHINE_FINISHED) return LT48_BUSY;
    *out_result = machine->result;
    machine->result = NULL;
    return LT48_OK;
}

lt48_status lt48_runtime_frame_set_mode(
    unsigned char buffers[][32], uint8_t buffer_count, uint32_t reset_mask,
    uint8_t mode_buffer, uint16_t mode, int operation_active, uint32_t option_flags
) {
    uint32_t valid_mask, value;
    if (!buffers || !buffer_count || buffer_count > LT48_RUNTIME_BUFFER_COUNT || mode_buffer >= buffer_count ||
        (operation_active != 0 && operation_active != 1)) return LT48_INVALID_ARGUMENT;
    valid_mask = buffer_count >= 32 ? UINT32_MAX :
        (UINT32_C(1) << buffer_count) - 1u;
    if (reset_mask & ~valid_mask) return LT48_INVALID_ARGUMENT;
    if (!(operation_active && (option_flags & UINT32_C(0x200))))
        for (uint8_t index = 0; index < buffer_count; ++index)
            if (index >= 32 || (reset_mask & (UINT32_C(1) << index))) memset(buffers[index], 0, 32);
    value = mode & UINT16_C(0x8000) ? UINT32_C(0xffff0000) | mode : mode;
    for (unsigned index = 0; index < 4; ++index)
        buffers[mode_buffer][index] = (unsigned char)(value >> (index * 8));
    return LT48_OK;
}

lt48_status lt48_runtime_device_image_internal(const lt48_runtime_result*r,const unsigned char**p,size_t*n){if(!r||!p||!n)return LT48_INVALID_ARGUMENT;*p=r->device_image;*n=r->device_image_size;return LT48_OK;}
