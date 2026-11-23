#include <labtool48/labtool48.h>
#include <string.h>

static void update(lt48_gal_pes_result *result, lt48_gal_pes_parameter parameter,
                   uint32_t value) {
    result->values[parameter] = value;
    result->updated_parameters |= UINT32_C(1) << parameter;
}

lt48_status lt48_gal_pes_decode(
    lt48_gal_pes_kind kind, const unsigned char *data, size_t size,
    uint8_t manufacturer, uint8_t device, uint8_t old_index,
    lt48_gal_pes_result *out_result
) {
    static const uint32_t tpwp[16] = {
        5,10,50,100,200,300,400,500,600,700,800,900,1000,2000,100,250};
    static const uint32_t tpwp_be[8] = {100,250,500,1000,2000,4000,8000,126};
    static const uint32_t tpwv[4] = {5,10,50,100};
    static const uint32_t legacy_viep[2][5] = {
        {1575,1575,1650,1450,1400}, {1650,1575,1450,1400,0}};
    static const uint32_t legacy_tpwp[2][5] = {
        {800,800,100,400,800}, {100,800,400,800,0}};
    lt48_gal_pes_result result = {0};
    uint32_t word;
    if (!out_result) return LT48_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    if (!data || size < 8 || kind < LT48_GAL_PES_ACCESS || kind > LT48_GAL_PES_ACCESS_LV)
        return LT48_INVALID_ARGUMENT;
    if (kind != LT48_GAL_PES_ACCESS_PLSI && data[3] != manufacturer) {
        out_result->original_status = 5;
        return LT48_DEVICE_MISMATCH;
    }
    if (kind == LT48_GAL_PES_ACCESS_LV) {
        if (data[2] != device && data[2] != 0x1e && data[2] != 0x1f &&
            data[2] != 0x1b && data[2] != 0x3e && data[2] != 0x3f && data[2] != 0x3b) {
            out_result->original_status = 6;
            return LT48_DEVICE_MISMATCH;
        }
        if (data[1] != 0x25 && data[1] != 0x15) {
            out_result->original_status = 6;
            return LT48_DEVICE_MISMATCH;
        }
        update(&result, LT48_GAL_PES_VCCP, data[1] == 0x25 ? 330 : 500);
        update(&result, LT48_GAL_PES_VIH, data[1] == 0x25 ? 330 : 500);
    } else if (kind == LT48_GAL_PES_ACCESS && data[1] != 5) {
        /* Original pointer tables have five and four parameter pairs. Their
         * unchecked byte indices can otherwise reach text/pointers, which
         * cannot define a device parameter in the native implementation. */
        if (old_index > 1 || data[1] >= (old_index ? 4 : 5)) return LT48_FORMAT_ERROR;
        update(&result, LT48_GAL_PES_VIEP, legacy_viep[old_index][data[1]]);
        update(&result, LT48_GAL_PES_VIEV, 1200);
        update(&result, LT48_GAL_PES_TPWP, legacy_tpwp[old_index][data[1]]);
        update(&result, LT48_GAL_PES_TPWP_BE, 1000);
        update(&result, LT48_GAL_PES_TPWV, 10);
        *out_result = result;
        return LT48_OK;
    }
    word = (uint32_t)data[4] | (uint32_t)data[5] << 8 |
           (uint32_t)data[6] << 16 | (uint32_t)data[7] << 24;
    update(&result, LT48_GAL_PES_VIEP, 500 + 50 * ((word >> 9) & 31));
    update(&result, LT48_GAL_PES_VIEV, 500 + 50 * ((word >> 14) & 31));
    update(&result, LT48_GAL_PES_VIEP1, result.values[LT48_GAL_PES_VIEP]);
    update(&result, LT48_GAL_PES_VIEV1, result.values[LT48_GAL_PES_VIEV]);
    update(&result, LT48_GAL_PES_TPWP, tpwp[(word >> 5) & 15]);
    update(&result, LT48_GAL_PES_TPWP_BE, tpwp_be[(word >> 2) & 7]);
    update(&result, LT48_GAL_PES_TPWV, tpwv[word & 3]);
    *out_result = result;
    return LT48_OK;
}

lt48_status lt48_gal_pes_apply(
    const lt48_gal_pes_result *decoded,
    const uint16_t identifiers[LT48_GAL_PES_PARAMETER_COUNT],
    lt48_compile_variable *variables, size_t count
) {
    size_t indices[LT48_GAL_PES_PARAMETER_COUNT];
    const uint32_t valid_mask = (UINT32_C(1) << LT48_GAL_PES_PARAMETER_COUNT) - 1;
    if (!decoded || !identifiers || (count && !variables) || decoded->original_status)
        return LT48_INVALID_ARGUMENT;
    if (decoded->updated_parameters & ~valid_mask) return LT48_FORMAT_ERROR;
    for (size_t parameter = 0; parameter < LT48_GAL_PES_PARAMETER_COUNT; ++parameter) {
        int timing = parameter == LT48_GAL_PES_TPWP ||
                     parameter == LT48_GAL_PES_TPWP_BE || parameter == LT48_GAL_PES_TPWV;
        indices[parameter] = SIZE_MAX;
        if (!(decoded->updated_parameters & (UINT32_C(1) << parameter))) continue;
        if (decoded->values[parameter] > UINT16_MAX) return LT48_SAFETY_REJECTED;
        for (size_t prior = 0; prior < parameter; ++prior)
            if ((decoded->updated_parameters & (UINT32_C(1) << prior)) &&
                identifiers[prior] == identifiers[parameter]) return LT48_FORMAT_ERROR;
        for (size_t index = 0; index < count; ++index) {
            if (variables[index].variable_id != identifiers[parameter]) continue;
            if (indices[parameter] != SIZE_MAX) return LT48_FORMAT_ERROR;
            indices[parameter] = index;
        }
        if (indices[parameter] == SIZE_MAX) return LT48_NOT_FOUND;
        if (variables[indices[parameter]].kind != (timing ? 1 : 0))
            return LT48_FORMAT_ERROR;
    }
    for (size_t parameter = 0; parameter < LT48_GAL_PES_PARAMETER_COUNT; ++parameter) {
        if (indices[parameter] == SIZE_MAX) continue;
        lt48_compile_variable *variable = &variables[indices[parameter]];
        if (variable->kind == 1) variable->channel = (uint16_t)decoded->values[parameter];
        else variable->value = (uint16_t)decoded->values[parameter];
    }
    return LT48_OK;
}

static lt48_status pes_name(const lt48_rez *rez, size_t names, const char *name,
                            uint16_t *identifier) {
    lt48_defined_name_collection_info info;
    lt48_status status = lt48_rez_get_defined_name_collection_info(rez, names, &info);
    int found = 0;
    if (status != LT48_OK) return status;
    for (size_t i = 0; i < info.count; ++i) {
        lt48_defined_name entry;
        status = lt48_rez_get_defined_name(rez, names, i, &entry);
        if (status != LT48_OK) return status;
        if (entry.name_length != strlen(name) || memcmp(entry.name, name, entry.name_length)) continue;
        if (found) return LT48_FORMAT_ERROR;
        *identifier = entry.identifier;
        found = 1;
    }
    return found ? LT48_OK : LT48_NOT_FOUND;
}

lt48_status lt48_gal_pes_resolve_binding(const lt48_rez *rez, size_t family,
                                         lt48_gal_pes_binding *out_binding) {
    static const char expected[] = "2b82c3e23dde22e68484fa6de0bf93f4ac957486158ab2d0d91a160a3416a380";
    static const char *parameters[] = {"_VAR_VIEP", "_VAR_VIEV", "_VAR_VIEP1", "_VAR_VIEV1",
        "_VAR_TPWP", "_VAR_TPWP_BE", "_VAR_TPWV", "_VAR_VCCP", "_VAR_VIH"};
    static const char *callbacks[] = {"_CFUN_PES_Access", "_CFUN_PES_AccessPLSI", "_CFUN_PES_Access_LV"};
    unsigned char digest[32];
    static const char hex[] = "0123456789abcdef";
    size_t names, wave_index;
    uint16_t buffer, callback_id = 0;
    lt48_family_info info;
    lt48_wave_code wave;
    lt48_gal_pes_binding binding = {0};
    lt48_status status;
    int found = 0, selected = 0;
    if (!out_binding) return LT48_INVALID_ARGUMENT;
    memset(out_binding, 0, sizeof(*out_binding));
    if (!rez) return LT48_INVALID_ARGUMENT;
#define PES_TRY(x) do { status = (x); if (status != LT48_OK) return status; } while (0)
    PES_TRY(lt48_rez_sha256(rez, digest));
    for (size_t i = 0; i < 32; ++i)
        if (expected[i * 2] != hex[digest[i] >> 4] || expected[i * 2 + 1] != hex[digest[i] & 15])
            return LT48_SAFETY_REJECTED;
    PES_TRY(lt48_rez_find_resource(rez, "DefineName", &names));
    PES_TRY(pes_name(rez, names, "_FUN_PES_Access", &binding.operation));
    PES_TRY(lt48_rez_get_family_info(rez, family, &info));
    for (size_t i = 0; i < info.operation_count; ++i) {
        lt48_family_operation operation;
        PES_TRY(lt48_rez_get_family_operation(rez, family, i, &operation));
        if (operation.operation_id != binding.operation) continue;
        char wave_name[16];
        if (found) return LT48_FORMAT_ERROR;
        /* Resource names encode their local numeric index in hexadecimal. */
        char digits[4]; size_t count = 0; uint16_t value = operation.wave;
        do { digits[count++] = hex[value & 15]; value >>= 4; } while (value);
        wave_name[0] = 'W';
        for (size_t j = 0; j < count; ++j) wave_name[j + 1] = digits[count - j - 1];
        wave_name[count + 1] = 0;
        PES_TRY(lt48_rez_find_resource(rez, wave_name, &wave_index));
        PES_TRY(lt48_rez_get_wave_code(rez, wave_index, &wave));
        if (wave.wave_type != 3) return LT48_NOT_FOUND;
        if (wave.data_size != 4 || wave.data[0] != 2 || wave.data[1])
            return LT48_FORMAT_ERROR;
        callback_id = (uint16_t)(wave.data[2] | (uint16_t)wave.data[3] << 8);
        found = 1;
    }
    if (!found) return LT48_NOT_FOUND;
    for (size_t i = 0; i < 3; ++i) {
        uint16_t id;
        PES_TRY(pes_name(rez, names, callbacks[i], &id));
        if (id != callback_id) continue;
        if (selected) return LT48_FORMAT_ERROR;
        binding.kind = (lt48_gal_pes_kind)i;
        selected = 1;
    }
    if (!selected) return LT48_NOT_FOUND;
    PES_TRY(pes_name(rez, names, "_BUF_DataIn", &buffer));
    if (buffer >= 32) return LT48_FORMAT_ERROR;
    binding.data_in_buffer = (uint8_t)buffer;
    PES_TRY(pes_name(rez, names, "_VAR_manID", &binding.manufacturer_variable));
    PES_TRY(pes_name(rez, names, "_VAR_devID", &binding.device_variable));
    PES_TRY(pes_name(rez, names, "_VAR_oldParaIdx", &binding.old_parameter_variable));
    for (size_t i = 0; i < LT48_GAL_PES_PARAMETER_COUNT; ++i)
        PES_TRY(pes_name(rez, names, parameters[i], &binding.parameter_variables[i]));
    /* Validate every branch this binding exposes; no missing field defaults. */
    for (size_t p = 0; p < 12; ++p) {
        uint16_t id, kind;
        if (p < 9) {
            if (p >= 7 && binding.kind != LT48_GAL_PES_ACCESS_LV) continue;
            id = binding.parameter_variables[p]; kind = p >= 4 && p <= 6 ? 1 : 0;
        } else {
            if (binding.kind == LT48_GAL_PES_ACCESS_PLSI) continue;
            if (p == 10 && binding.kind != LT48_GAL_PES_ACCESS_LV) continue;
            if (p == 11 && binding.kind != LT48_GAL_PES_ACCESS) continue;
            id = p == 9 ? binding.manufacturer_variable : p == 10 ? binding.device_variable : binding.old_parameter_variable;
            kind = 2;
        }
        int present = 0;
        for (size_t v = 0; v < info.variable_count; ++v) {
            lt48_family_variable variable;
            PES_TRY(lt48_rez_get_family_variable(rez, family, v, &variable));
            if (variable.values[1] != id) continue;
            if (present || variable.values[0] != kind) return LT48_FORMAT_ERROR;
            present = 1;
        }
        if (!present) return LT48_NOT_FOUND;
    }
    *out_binding = binding;
#undef PES_TRY
    return LT48_OK;
}
