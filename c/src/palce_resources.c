#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lt48_palce_resources {
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compile_variable *variables;
    size_t variable_count;
    lt48_compiled_wave *waves[11];
    lt48_fuse_coordinate *coordinates;
    size_t coordinate_count;
    size_t names_index;
    uint8_t buffer_count;
    uint16_t definition_resource;
    lt48_runtime_nested_wave bit_map;
    const char *source_version;
};

void lt48_palce_resources_close(lt48_palce_resources *resources) {
    if (!resources) return;
    for (size_t index = 0u; index < 11u; ++index)
        lt48_compiled_wave_close(resources->waves[index]);
    free(resources->coordinates);
    free(resources->variables);
    lt48_pin_map_close(resources->pins);
    lt48_rez_close(resources->rez);
    free(resources);
}

lt48_status lt48_palce_resources_buffer(
    const lt48_palce_resources *resources, const char *name, uint8_t *out_id
) {
    lt48_defined_name_collection_info info;
    lt48_status status;
    int found = 0;
    uint16_t identifier = 0u;
    if (!resources || !name || !out_id || strncmp(name, "_BUF_", 5u))
        return LT48_INVALID_ARGUMENT;
    status = lt48_rez_get_defined_name_collection_info(
        resources->rez, resources->names_index, &info);
    if (status != LT48_OK) return status;
    for (size_t index = 0u; index < info.count; ++index) {
        lt48_defined_name item;
        status = lt48_rez_get_defined_name(resources->rez,
            resources->names_index, index, &item);
        if (status != LT48_OK) return status;
        if (item.name_length == strlen(name) &&
            !memcmp(item.name, name, item.name_length)) {
            if (found || item.identifier >= resources->buffer_count) return LT48_FORMAT_ERROR;
            identifier = item.identifier;
            found = 1;
        }
    }
    if (!found) return LT48_NOT_FOUND;
    *out_id = (uint8_t)identifier;
    return LT48_OK;
}

lt48_status lt48_palce_resources_open_definition(
    const char *definition_path, uint16_t definition_resource,
    lt48_palce_resources **out_resources
) {
    static const char *digests[] = {
        "b1756512d722ef017b6c41688bea0d5184eb3052fb8fecc89d980c7bf0eda072",
        "65753e664d77d1cfab853ae51f6b58de07fc98ab73f88e618a31d0261fd1d846"
    };
    static const char *required_buffers[] = {
        "_BUF_DataIn", "_BUF_DataOut", "_BUF_DataMask", "_BUF_Mode",
        "_BUF_PLine", "_BUF_ILine", "_BUF_FuseType"
    };
    lt48_palce_resources *resources;
    lt48_status status;
    lt48_device_definition definition;
    lt48_family_info family;
    lt48_logic_fuse_codes fuse_bindings;
    char resource_name[16];
    lt48_pin_code pin_code;
    lt48_fuse_code fuse_code;
    unsigned char digest[32];
    char digest_hex[65];
    size_t index, family_index;
    if (!definition_path || !out_resources) return LT48_INVALID_ARGUMENT;
    *out_resources = NULL;
    resources = calloc(1u, sizeof(*resources));
    if (!resources) return LT48_NO_MEMORY;
    resources->definition_resource = definition_resource;
    if (definition_resource > 2u) {
        free(resources); return LT48_UNSUPPORTED;
    }
#define TRY(expression) do { status = (expression); if (status != LT48_OK) goto fail; } while (0)
    TRY(lt48_rez_open(definition_path, &resources->rez));
    TRY(lt48_rez_sha256(resources->rez, digest));
    for (size_t byte = 0u; byte < sizeof(digest); ++byte)
        snprintf(digest_hex + byte * 2u, 3u, "%02x", digest[byte]);
    if (!strcmp(digest_hex, digests[0])) resources->source_version = "AEC 7.60";
    else if (!strcmp(digest_hex, digests[1])) resources->source_version = "Dataman 13.10";
    else { status = LT48_SAFETY_REJECTED; goto fail; }
    snprintf(resource_name, sizeof(resource_name), "D%x", definition_resource);
    TRY(lt48_rez_find_resource(resources->rez, resource_name, &index));
    TRY(lt48_rez_get_logic_fuse_codes(resources->rez, index, &fuse_bindings));
    if (fuse_bindings.count != 1u) { status = LT48_FORMAT_ERROR; goto fail; }
    TRY(lt48_rez_get_device_definition(resources->rez, index, &definition));
    if (definition.kind != LT48_DEVICE_DEFINITION_LOGIC ||
        definition.family_code != definition_resource ||
        definition.pinmap != (definition_resource == 2u ? 4u : definition_resource)) {
        status = LT48_FORMAT_ERROR; goto fail;
    }
    {
        uint16_t count;
        TRY(lt48_rez_find_resource(resources->rez, "Kernel", &index));
        TRY(lt48_rez_get_kernel_buffer_count(resources->rez, index, &count));
        if (!count || count > 32) { status = LT48_UNSUPPORTED; goto fail; }
        resources->buffer_count = (uint8_t)count;
    }
    TRY(lt48_rez_find_resource(resources->rez, "DefineName", &resources->names_index));
    for (size_t buffer = 0u; buffer < sizeof(required_buffers)/sizeof(required_buffers[0]); ++buffer) {
        uint8_t identifier;
        TRY(lt48_palce_resources_buffer(resources, required_buffers[buffer], &identifier));
    }
    snprintf(resource_name, sizeof(resource_name), "P%x", definition.pinmap);
    TRY(lt48_rez_find_resource(resources->rez, resource_name, &index));
    TRY(lt48_rez_get_pin_code(resources->rez, index, &pin_code));
    if (pin_code.pin_count != (definition_resource == 0u ? 20u : 24u)) { status = LT48_FORMAT_ERROR; goto fail; }
    TRY(lt48_pin_map_parse(&pin_code, &resources->pins));
    snprintf(resource_name, sizeof(resource_name), "M%x", definition.family_code);
    TRY(lt48_rez_find_resource(resources->rez, resource_name, &family_index));
    TRY(lt48_rez_get_family_info(resources->rez, family_index, &family));
    resources->variable_count = family.variable_count;
    resources->variables = calloc(family.variable_count ? family.variable_count : 1u,
                                  sizeof(*resources->variables));
    if (!resources->variables) { status = LT48_NO_MEMORY; goto fail; }
    for (size_t variable = 0u; variable < family.variable_count; ++variable) {
        lt48_family_variable item;
        TRY(lt48_rez_get_family_variable(resources->rez, family_index, variable, &item));
        resources->variables[variable] = (lt48_compile_variable){
            item.values[0],item.values[1],item.values[2],item.values[3],item.values[4]};
    }
    for (size_t binding = 0u; binding < family.operation_count; ++binding) {
        lt48_family_operation operation;
        lt48_wave_code wave_code;
        lt48_compact_wave *wave = NULL;
        char name[16];
        TRY(lt48_rez_get_family_operation(resources->rez, family_index, binding, &operation));
        snprintf(name, sizeof(name), "W%x", operation.wave);
        TRY(lt48_rez_find_resource(resources->rez, name, &index));
        TRY(lt48_rez_get_wave_code(resources->rez, index, &wave_code));
        if (operation.operation_id == 8u && wave_code.wave_type == 3u) {
            /* This immutable profile's W8 is CFUN 0, not a controller applet.
             * Callback numbers are local to each resource package. */
            static const unsigned char bitsort_binding[] = {2u, 0u, 0u, 0u};
            if (wave_code.data_size != sizeof(bitsort_binding) ||
                memcmp(wave_code.data, bitsort_binding, sizeof(bitsort_binding))) {
                status = LT48_FORMAT_ERROR; goto fail;
            }
            continue;
        }
        if (operation.operation_id == 9u && wave_code.wave_type == 5u) {
            TRY(lt48_compiled_map_create(wave_code.data, wave_code.data_size,
                                         &resources->waves[9]));
            resources->bit_map = (lt48_runtime_nested_wave){9u, resources->waves[9]};
            continue;
        }
        if (operation.operation_id >= 11u || resources->waves[operation.operation_id] ||
            wave_code.wave_type != 0u) { status = LT48_FORMAT_ERROR; goto fail; }
        TRY(lt48_compact_decode(wave_code.data, wave_code.data_size, &wave));
        /* GBC DefineName binds _IO_GND=1 and _IO_ALL=0. Compact 0x33
         * must select the ground group; group zero grounds VCC as well. */
        status = lt48_compact_compile(wave, resources->pins, resources->variables,
            resources->variable_count, 1u, 0, &resources->waves[operation.operation_id]);
        lt48_compact_wave_close(wave);
        if (status != LT48_OK) goto fail;
    }
    snprintf(resource_name, sizeof(resource_name), "F%x", fuse_bindings.identifiers[0]);
    TRY(lt48_rez_find_resource(resources->rez, resource_name, &index));
    TRY(lt48_rez_get_fuse_code(resources->rez, index, &fuse_code));
    if (fuse_code.total_fuse_count != (definition_resource == 0u ? 2194u : definition_resource == 1u ? 2706u : 5828u)) { status = LT48_FORMAT_ERROR; goto fail; }
    resources->coordinates = calloc(fuse_code.total_fuse_count, sizeof(*resources->coordinates));
    if (!resources->coordinates) { status = LT48_NO_MEMORY; goto fail; }
    TRY(lt48_fuse_layout_expand_coordinates(&fuse_code, resources->coordinates,
                                           fuse_code.total_fuse_count, &resources->coordinate_count));
    *out_resources = resources;
    return LT48_OK;
fail:
    lt48_palce_resources_close(resources);
    return status;
#undef TRY
}

const char *lt48_palce_resources_source_version(const lt48_palce_resources *resources) {
    return resources ? resources->source_version : NULL;
}

const lt48_fuse_coordinate *lt48_palce_resources_coordinates(
    const lt48_palce_resources *resources, size_t *out_count
) {
    if (out_count) *out_count = resources ? resources->coordinate_count : 0u;
    return resources ? resources->coordinates : NULL;
}

const lt48_compile_variable *lt48_palce_resource_variables_internal(const lt48_palce_resources *r,size_t *count) {
    *count=r->variable_count;return r->variables;
}
lt48_status lt48_palce_resources_create_machine(
    const lt48_palce_resources *r,uint16_t operation,uint32_t mode,
    const uint32_t voltage[4],const lt48_runtime_buffer_seed *seeds,size_t seed_count,
    lt48_runtime_machine **out) {
    if(!r)return LT48_INVALID_ARGUMENT;
    return lt48_palce_machine_variables_internal(r,operation,mode,voltage,seeds,
        seed_count,r->variables,r->variable_count,out);
}
lt48_status lt48_palce_machine_variables_internal(
    const lt48_palce_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, const lt48_compile_variable *variables, size_t variable_count,
    lt48_runtime_machine **out_machine
) {
    lt48_runtime_options options = {0};
    lt48_runtime_nested_wave nested[3];
    size_t nested_count = 0u;
    lt48_status status;
    if (!resources || !initial_voltage || !out_machine || (seed_count && !seeds))
        return LT48_INVALID_ARGUMENT;
    *out_machine = NULL;
    if ((mode != 1u && mode != 2u && mode != 3u && mode != 4u && mode != 7u && mode != 8u && mode != 10u && mode != 11u) ||
        operation >= 11u ||
        (resources->definition_resource == 2u && operation == 9u) ||
        !resources->waves[operation])
        return LT48_UNSUPPORTED;
    options.mode = mode;
    if (resources->bit_map.compiled)
        nested[nested_count++] = resources->bit_map;
    if (resources->definition_resource == 1u && operation == 2u)
        nested[nested_count++] = (lt48_runtime_nested_wave){6u, resources->waves[6]};
    if (operation == 7u) {
        /* Original mode-8 SECU (D0 W17 / D2 W19) calls PROGRAM after
         * selecting its security row. Preserve the original ten-second budget. */
        if (mode != 8u || !resources->waves[5]) return LT48_UNSUPPORTED;
        nested[nested_count++] = (lt48_runtime_nested_wave){5u, resources->waves[5]};
        options.root_timeout_seconds = 10u;
    }
    options.nested_waves = nested_count ? nested : NULL;
    options.nested_wave_count = nested_count;
    options.use_host_bitsort = 1;
    options.host_bitsort_operation = 8u;
    options.use_explicit_buffer_ids = 1;
    options.address_buffer = 0u;
    status = lt48_palce_resources_buffer(resources, "_BUF_DataIn", &options.parameter_buffer_id);
    if (status != LT48_OK) return status;
    options.host_bitsort_buffer = options.parameter_buffer_id;
    status = lt48_palce_resources_buffer(resources, "_BUF_Mode", &options.mode_buffer);
    if (status != LT48_OK) return status;
    memcpy(options.initial_voltage_centivolts, initial_voltage,
           sizeof(options.initial_voltage_centivolts));
    options.initial_buffers = seeds;
    options.initial_buffer_count = seed_count;
    return lt48_runtime_machine_create(resources->waves[operation], resources->pins,
        variables, variable_count, &options, out_machine);
}

uint32_t lt48_palce_resources_reset_buffer_mask(const lt48_palce_resources *resources) {
    uint32_t mask;
    if (!resources) return 0;
    mask = resources->buffer_count == 32 ? UINT32_MAX :
        (UINT32_C(1) << resources->buffer_count) - 1u;
    for (size_t operation = 0; operation < 11; ++operation) {
        const lt48_compiled_wave *wave = resources->waves[operation];
        for (size_t index = 0; index < lt48_compiled_wave_preserved_buffer_count(wave); ++index) {
            uint8_t buffer;
            if (lt48_compiled_wave_get_preserved_buffer(wave, index, &buffer) == LT48_OK && buffer < 32)
                mask &= ~(UINT32_C(1) << buffer);
        }
    }
    return mask;
}

uint8_t lt48_palce_resources_buffer_count(const lt48_palce_resources *resources) {
    return resources ? resources->buffer_count : 0;
}

lt48_status lt48_palce_resources_open(const char *path, lt48_palce_resources **out) {
    return lt48_palce_resources_open_definition(path, 0u, out);
}
uint16_t lt48_palce_resources_definition(const lt48_palce_resources *resources) {
    return resources ? resources->definition_resource : UINT16_MAX;
}
