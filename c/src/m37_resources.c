#include <labtool48/labtool48.h>

#include "internal.h"

/* The only forward Read procedure reconstructed from a complete Windows
 * capture is the pinned Dataman M0 definition.  Capacity/family alone is not
 * an identity certificate: another 16 KiB M0 definition may use different
 * contacts, applets or cleanup. */
static const unsigned char dataman_m0_forward_definition[LT48_SHA256_SIZE] = {
    0x2e,0x62,0x61,0xc8,0x67,0xd6,0x85,0x77,
    0x86,0x93,0x3c,0x0d,0x09,0x3e,0x5b,0xd6,
    0x69,0xf7,0x03,0x55,0xcc,0x21,0x8f,0xf0,
    0x06,0xc8,0x8c,0x4f,0xb9,0x19,0x63,0x6b
};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct applet_range {
    uint32_t start;
    uint32_t end;
} applet_range;

typedef struct compiled_resource {
    uint16_t operation_id;
    uint16_t wave;
    lt48_compiled_wave *compiled;
} compiled_resource;

struct lt48_m37_resources {
    lt48_m37_resource_audit audit;
    uint16_t catalogue_manufacturer_id;
    uint16_t catalogue_device_id;
    lt48_rez *rez;
    lt48_pin_map *pin_map;
    lt48_compile_variable *variables;
    size_t variable_count;
    compiled_resource *compiled;
    size_t compiled_count;
    lt48_m37_applet_resource *applets;
    size_t applet_count;
    const unsigned char *active_fpga;
    size_t active_fpga_size;
    unsigned read_nominal_override;
    unsigned char *idle_fpga;
    size_t idle_fpga_size;
};

static void hash_u16(lt48_sha256_context *hash, uint16_t value) {
    unsigned char bytes[2] = {
        (unsigned char)value, (unsigned char)(value >> 8)
    };
    lt48_sha256_update(hash, bytes, sizeof(bytes));
}

static void hash_u32(lt48_sha256_context *hash, uint32_t value) {
    unsigned char bytes[4] = {
        (unsigned char)value, (unsigned char)(value >> 8),
        (unsigned char)(value >> 16), (unsigned char)(value >> 24)
    };
    lt48_sha256_update(hash, bytes, sizeof(bytes));
}

static lt48_status make_definition_path(
    const lt48_installation *installation, uint16_t device_class,
    char **out_path
) {
    const char *root = lt48_installation_path(installation);
    const char *stem = lt48_definition_stem_for_class(device_class);
    size_t length;
    char *path;
    if (root == NULL || stem == NULL || out_path == NULL) return LT48_NOT_FOUND;
    if (strlen(root) > SIZE_MAX - strlen(stem) - 6u) {
        return LT48_INVALID_ARGUMENT;
    }
    length = strlen(root) + strlen(stem) + 6u;
    path = malloc(length);
    if (path == NULL) return LT48_NO_MEMORY;
    snprintf(path, length, "%s/%s.rez", root, stem);
    *out_path = path;
    return LT48_OK;
}

static lt48_status load_verified_idle_fpga(
    const lt48_installation *installation,
    unsigned char **out_data,
    size_t *out_size
) {
    const char *root = lt48_installation_path(installation);
    return lt48_load_verified_idle_fpga_internal(root, out_data, out_size);
}

/* Exact Dataman 13.10 byte-wide EPROM closure, audited in
 * docs/audit-qualification/EPROM_64_512_GROUPS.md. No caller-supplied evidence can extend it. */
#include "large_eprom_groups.h"
#include "../../NewDeviceLib/sources/eep21/profiles.h"
#include "../../NewDeviceLib/sources/holtek040one/profiles.h"
#include "../../NewDeviceLib/sources/holtek040one/admission.h"
#include "../../NewDeviceLib/sources/m26c201one/profiles.h"
#include "../../NewDeviceLib/sources/m26c201one/admission.h"
#include "../../NewDeviceLib/sources/macronixmtp2/profiles.h"
#include "../../NewDeviceLib/sources/macronixmtp2/admission.h"
#include "../../NewDeviceLib/sources/eprom3/profiles.h"
#include "../../NewDeviceLib/sources/novatek-umc6/profiles.h"
#include "../../NewDeviceLib/sources/hitachipulse2/profiles.h"
static int electrical_eprom_group(const lt48_m37_resource_audit*a){return (LT48_M26C201ONE_ADMITTED&&m26c201one_group(a))||eep21_group(a)||eprom3_electrical(a)||(LT48_MACRONIXMTP2_ADMITTED&&macronixmtp2_group(a));}
#include "../../NewDeviceLib/sources/green14/profiles.h"
int lt48_eep21_definition_internal(unsigned cls,unsigned d){return (LT48_M26C201ONE_ADMITTED&&m26c201one_definition(cls,d))||(LT48_MACRONIXMTP2_ADMITTED&&macronixmtp2_definition(cls,d))||eep21_definition(cls,d)||(eprom3_definition(cls,d)&&d!=0x25);}
int lt48_m37_resources_is_eep21(const lt48_m37_resources*r){return r&&electrical_eprom_group(&r->audit);}

static int source_eprom_group(const lt48_m37_resource_audit *a) {
    static const struct {uint16_t definition,family,pins;uint32_t capacity;} groups[] = {
        {0x0u,0x0u,0x0u,0x2000u},
        {0x1u,0x0u,0x5u,0x4000u},
        {0x2u,0x2u,0x6u,0x8000u},
        {0x3u,0x1u,0x0u,0x2000u},
        {0x11u,0x4u,0x6u,0x8000u},
        {0x12u,0xeu,0x8u,0x10000u},
        {0x1du,0x16u,0x0u,0x2000u},
        {0x1eu,0x16u,0x5u,0x4000u},
        {0x1fu,0x17u,0x6u,0x8000u},
        {0x2au,0x20u,0x8u,0x10000u},
        {0x2du,0x22u,0x8u,0x10000u},
        {0x31u,0x25u,0x8u,0x10000u},
        {0x32u,0x25u,0x9u,0x10000u},
        {0x33u,0x26u,0x6u,0x8000u},
        {0x34u,0x27u,0x8u,0x10000u},
        {0x47u,0x38u,0x0u,0x2000u},
        {0x49u,0x3au,0x0u,0x2000u},
        {0x50u,0x3du,0x6u,0x8000u},
        {0x51u,0x3eu,0x6u,0x8000u},
        {0x52u,0x3fu,0x6u,0x8000u},
        {0x53u,0x40u,0x6u,0x8000u},
        {0x54u,0x41u,0x6u,0x8000u},
        {0x55u,0x42u,0x8u,0x10000u},
        {0x56u,0x43u,0x8u,0x10000u},
        {0x6au,0x53u,0x6u,0x8000u},
        {0x6bu,0x54u,0x6u,0x8000u},
        {0x6cu,0x55u,0x6u,0x8000u},
        {0x6du,0x56u,0x8u,0x10000u},
        {0x7au,0x5eu,0x8u,0x10000u},
    };
    static const unsigned char rapid_sha[32]={0xe2,0x82,0xaa,0x2c,0x1a,0x8f,0xbc,0xc1,0x30,0x70,0xfe,0xe6,0x2d,0xae,0x55,0x1f,0xe9,0xdb,0x38,0x09,0x63,0x9a,0x90,0x52,0x9b,0x5d,0x42,0x3f,0x99,0xc9,0x01,0x80};
    static const unsigned char cypress_sha[32]={0x9f,0xdc,0x0c,0x7a,0x88,0xbb,0x96,0xaa,0xea,0xd0,0x31,0x4f,0xb4,0xf8,0x5c,0x08,0x00,0xab,0x98,0x7a,0xbf,0x37,0x09,0x08,0x00,0x04,0x93,0xcc,0x27,0x2d,0x13,0xf6};
    if((LT48_HOLTEK040ONE_ADMITTED&&holtek040one_group(a))||large_eprom_group(a)||electrical_eprom_group(a)||eprom3_group(a)||novatek_umc6_group(a)||hitachipulse2_group(a))return 1;
    if(a->device_class==2006u && !memcmp(a->definition_sha256,rapid_sha,32) && a->device_type==5u && a->family_code==4u && a->pinmap==3u && a->pin_count==28u && a->capacity_bytes==65536u)return 1;
    if(a->device_class==2006u && !memcmp(a->definition_sha256,rapid_sha,32) &&
       a->device_type<=3u && a->pinmap==a->device_type && a->pin_count==28u &&
       a->family_code==(a->device_type<2u?0u:a->device_type-1u) &&
       a->capacity_bytes==(8192u<<a->device_type)) return 1;
    if(a->device_class==2009u && !memcmp(a->definition_sha256,cypress_sha,32) &&
       a->device_type==0u && a->pinmap==0u && a->family_code==0u &&
       a->pin_count==28u && a->capacity_bytes==32768u) return 1;
    if (a->device_class != 2001u ||
        memcmp(a->definition_sha256, dataman_m0_forward_definition,32)) return 0;
    for (size_t i=0;i<sizeof(groups)/sizeof(groups[0]);++i)
        if (a->device_type==groups[i].definition && a->family_code==groups[i].family &&
            a->pinmap==groups[i].pins && a->capacity_bytes==groups[i].capacity &&
            a->pin_count==(groups[i].pins==9u?32u:28u)) return 1;
    return 0;
}

static int compiled_pins_resolved(const lt48_compiled_wave *wave) {
    for (size_t i=0;i<lt48_compiled_wave_defaulted_pin_count(wave);++i) {
        lt48_defaulted_logical_pin pin;
        if (lt48_compiled_wave_get_defaulted_pin(wave,i,&pin)!=LT48_OK ||
            pin.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP) return 0;
    }
    return 1;
}

static int ranges_do_not_overlap(const applet_range *ranges, size_t count) {
    size_t left;
    size_t right;
    for (left = 0; left < count; ++left) {
        for (right = left + 1u; right < count; ++right) {
            if (ranges[left].start < ranges[right].end &&
                ranges[right].start < ranges[left].end) {
                return 0;
            }
        }
    }
    return 1;
}

static int pin_map_matches_reference(
    const lt48_pin_map *pin_map, uint16_t pinmap, uint32_t capacity_bytes
) {
    lt48_pin_group group;
    size_t expected_logical_count = capacity_bytes == 0x8000u ? 4u : 5u;
    size_t expected_address_count = capacity_bytes == 0x2000u ? 13u :
                                    capacity_bytes == 0x4000u ? 14u :
                                    capacity_bytes == 0x8000u ? 15u : 16u;
    size_t expected_data_count = pinmap == 7u ? 5u : 8u;
    if (lt48_pin_map_package_pin_count(pin_map) != (pinmap == 9u ? 32u : 28u) ||
        lt48_pin_map_logical_count(pin_map) != expected_logical_count) {
        return 0;
    }
    if (lt48_pin_map_find_group(pin_map, 1u, &group) != LT48_OK ||
        group.pin_count != 1u || group.pins[0] != 92u) return 0;
    if (lt48_pin_map_find_group(pin_map, 2u, &group) != LT48_OK ||
        group.pin_count != 1u || group.pins[0] != (pinmap == 9u ? 156u : 148u)) return 0;
    if (lt48_pin_map_find_group(pin_map, 4u, &group) != LT48_OK ||
        group.pin_count != expected_address_count) return 0;
    if (lt48_pin_map_find_group(pin_map, 5u, &group) != LT48_OK ||
        group.pin_count != expected_data_count) return 0;
    if (capacity_bytes == 0x2000u &&
        (lt48_pin_map_find_group(pin_map, 3u, &group) != LT48_OK ||
         group.pin_count != 1u || group.pins[0] != 140u)) return 0;
    if (pinmap == 7u &&
        (lt48_pin_map_find_group(pin_map, 3u, &group) != LT48_OK ||
         group.pin_count != 3u || group.pins[0] != 104u ||
         group.pins[1] != 108u || group.pins[2] != 112u)) return 0;
    return 1;
}

lt48_status lt48_installation_audit_m37(
    const lt48_installation *installation,
    size_t device_index,
    lt48_m37_resource_audit *out_audit
) {
    static const unsigned char manifest_tag[] = "LT48-M37-MANIFEST-v1";
    lt48_catalogue_entry entry;
    lt48_device_spec spec;
    lt48_device_definition definition;
    lt48_family_info family;
    lt48_pin_code pin_code;
    lt48_pin_map *pin_map = NULL;
    lt48_compile_variable *variables = NULL;
    applet_range *ranges = NULL;
    lt48_rez *rez = NULL;
    char *path = NULL;
    char name[16];
    size_t definition_index;
    size_t family_index;
    size_t pin_index;
    size_t operation_index;
    size_t range_count = 0u;
    uint16_t required_mask = 0u;
    unsigned nominal_count = 0u;
    int compact_strict = 1;
    int applet_metadata = 1;
    int fpga_framing = 1;
    lt48_sha256_context manifest;
    lt48_status status;

    if (installation == NULL || out_audit == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    memset(out_audit, 0, sizeof(*out_audit));
    status = lt48_installation_get_device(installation, device_index, &entry);
    if (status != LT48_OK) return status;
    status = lt48_installation_resolve_device(installation, device_index, &spec);
    if (status != LT48_OK) return status;
    status = make_definition_path(installation, entry.device_class, &path);
    if (status != LT48_OK) return status;
    status = lt48_rez_open(path, &rez);
    free(path);
    if (status != LT48_OK) return status;
    status = lt48_rez_sha256(rez, out_audit->definition_sha256);
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return status;
    }

    snprintf(name, sizeof(name), "D%x", entry.device_type);
    status = lt48_rez_find_resource(rez, name, &definition_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_device_definition(
            rez, definition_index, &definition
        );
    }
    snprintf(name, sizeof(name), "M%x", spec.family_code);
    if (status == LT48_OK) {
        status = lt48_rez_find_resource(rez, name, &family_index);
    }
    if (status == LT48_OK) {
        status = lt48_rez_get_family_info(rez, family_index, &family);
    }
    snprintf(name, sizeof(name), "P%x", spec.pinmap);
    if (status == LT48_OK) status = lt48_rez_find_resource(rez, name, &pin_index);
    if (status == LT48_OK) status = lt48_rez_get_pin_code(rez, pin_index, &pin_code);
    if (status == LT48_OK) status = lt48_pin_map_parse(&pin_code, &pin_map);
    if (status != LT48_OK) goto cleanup;

    variables = calloc(family.variable_count == 0u ? 1u : family.variable_count,
                       sizeof(*variables));
    ranges = calloc(family.operation_count == 0u ? 1u : family.operation_count,
                    sizeof(*ranges));
    if (variables == NULL || ranges == NULL) {
        status = LT48_NO_MEMORY;
        goto cleanup;
    }

    lt48_sha256_init(&manifest);
    lt48_sha256_update(&manifest, manifest_tag, sizeof(manifest_tag) - 1u);
    hash_u32(&manifest, (uint32_t)strlen(entry.vendor));
    lt48_sha256_update(
        &manifest, (const unsigned char *)entry.vendor, strlen(entry.vendor)
    );
    hash_u32(&manifest, (uint32_t)entry.device_name_length);
    lt48_sha256_update(&manifest, entry.device_name, entry.device_name_length);
    hash_u16(&manifest, entry.table_index);
    hash_u16(&manifest, entry.device_class);
    hash_u16(&manifest, entry.device_type);
    hash_u16(&manifest, entry.unknown3);
    hash_u16(&manifest, entry.manufacturer_id);
    hash_u16(&manifest, entry.device_id);
    hash_u16(&manifest, entry.unknown6);
    hash_u16(&manifest, entry.unknown7);
    hash_u16(&manifest, entry.unknown8);
    hash_u16(&manifest, entry.unknown9);
    hash_u16(&manifest, (uint16_t)definition.kind);
    hash_u16(&manifest, definition.family_code);
    hash_u16(&manifest, definition.pinmap);
    hash_u16(&manifest, definition.value2);
    hash_u16(&manifest, definition.value3);
    hash_u32(&manifest, definition.device_size);
    hash_u16(&manifest, (uint16_t)definition.has_device_size);
    hash_u32(&manifest, (uint32_t)definition.trailing_data_size);
    lt48_sha256_update(
        &manifest, definition.trailing_data, definition.trailing_data_size
    );
    hash_u16(&manifest, spec.family_code);
    hash_u16(&manifest, spec.pinmap);
    hash_u32(&manifest, spec.capacity_bytes);
    hash_u16(&manifest, pin_code.pin_count);
    hash_u32(&manifest, (uint32_t)pin_code.data_size);
    lt48_sha256_update(&manifest, pin_code.data, pin_code.data_size);

    for (operation_index = 0; operation_index < family.variable_count;
         ++operation_index) {
        lt48_family_variable source;
        status = lt48_rez_get_family_variable(
            rez, family_index, operation_index, &source
        );
        if (status != LT48_OK) goto cleanup;
        variables[operation_index].kind = source.values[0];
        variables[operation_index].variable_id = source.values[1];
        variables[operation_index].enabled = source.values[2];
        variables[operation_index].value = source.values[3];
        variables[operation_index].channel = source.values[4];
        for (size_t value_index = 0u; value_index < 5u; ++value_index) {
            hash_u16(&manifest, source.values[value_index]);
        }
        if (source.values[1] == (entry.device_class==2004u?3u:0u) && source.values[2] != 0u) {
            ++nominal_count;
            out_audit->nominal_vcc_centivolts = source.values[3];
        }
    }

    for (operation_index = 0; operation_index < family.operation_count;
         ++operation_index) {
        lt48_family_operation operation;
        lt48_wave_code wave;
        size_t wave_index;
        status = lt48_rez_get_family_operation(
            rez, family_index, operation_index, &operation
        );
        if (status != LT48_OK) goto cleanup;
        snprintf(name, sizeof(name), "W%x", operation.wave);
        status = lt48_rez_find_resource(rez, name, &wave_index);
        if (status == LT48_OK) {
            status = lt48_rez_get_wave_code(rez, wave_index, &wave);
        }
        if (status != LT48_OK) goto cleanup;
        hash_u16(&manifest, operation.operation_id);
        hash_u16(&manifest, operation.wave);
        hash_u32(&manifest, wave.wave_type);
        hash_u16(&manifest, wave.length1);
        hash_u16(&manifest, wave.load_address);
        hash_u16(&manifest, wave.length2);
        hash_u32(&manifest, (uint32_t)wave.data_size);
        lt48_sha256_update(&manifest, wave.data, wave.data_size);

        if (wave.wave_type == 0u) {
            lt48_compact_wave *compact = NULL;
            lt48_compiled_wave *compiled = NULL;
            ++out_audit->compact_wave_count;
            if (operation.operation_id <= 4u) {
                uint16_t bit = (uint16_t)(1u << operation.operation_id);
                /* Repeated slots use the original loader's last binding. */
                required_mask = (uint16_t)(required_mask | bit);
            }
            status = lt48_compact_decode(wave.data, wave.data_size, &compact);
            if (status == LT48_OK) {
                status = lt48_compact_compile_with_pin_policy(
                    compact, pin_map, variables, family.variable_count,
                    1u, LT48_PIN_FALLBACK_RESIDENT_NOOP, &compiled
                );
            }
            if (status != LT48_OK ||
                (compiled != NULL &&
                 !compiled_pins_resolved(compiled))) {
                compact_strict = 0;
                status = LT48_OK;
            }
            lt48_compiled_wave_close(compiled);
            lt48_compact_wave_close(compact);
        } else if (wave.wave_type == 6u) {
            uint32_t end = (uint32_t)wave.load_address +
                           (uint32_t)wave.data_size;
            ++out_audit->applet_count;
            if (wave.data_size > UINT16_MAX ||
                wave.length1 != wave.data_size + 4u ||
                wave.length2 != wave.data_size || end > UINT16_MAX + 1u) {
                applet_metadata = 0;
            }
            ranges[range_count].start = wave.load_address;
            ranges[range_count].end = end;
            ++range_count;
        } else if (wave.wave_type == 4u) {
            uint16_t framed_size = 0u;
            ++out_audit->active_fpga_count;
            if (wave.data_size >= 2u) {
                framed_size = (uint16_t)(wave.data[0] |
                                         (uint16_t)wave.data[1] << 8);
            }
            if (wave.data_size != 2780u || framed_size != 2778u) {
                fpga_framing = 0;
            }
        }
    }
    lt48_sha256_final(&manifest, out_audit->resource_manifest_sha256);

    out_audit->device_class = entry.device_class;
    out_audit->device_type = entry.device_type;
    out_audit->family_code = spec.family_code;
    out_audit->pinmap = spec.pinmap;
    out_audit->pin_count = spec.pin_count;
    out_audit->capacity_bytes = spec.capacity_bytes;
    out_audit->operation_count = family.operation_count;
    out_audit->variable_count = family.variable_count;
    out_audit->expected_vcc_selector =
        out_audit->nominal_vcc_centivolts <= 400u ? 0x07u : 0x0fu;
    out_audit->exact_engine_envelope =
        entry.device_class == 2001u &&
        definition.kind == LT48_DEVICE_DEFINITION_MEMORY &&
        spec.pin_count == 28u &&
        /* Dataman 13.10 Am27C128 uses M0, P5, 28 pins and 16 KiB.
         * Its normal-UI Read was captured through success and cleanup.
         * This recognizes the binding only: validate_approved_read_behavior
         * still requires a separately approved native transaction digest. */
        (((spec.family_code == 0u || spec.family_code == 0x37u || spec.family_code == 0x39u ||
           spec.family_code == 0x3bu || spec.family_code == 0x50u ||
           spec.family_code == 0x51u || spec.family_code == 0x52u) &&
          spec.pinmap == 5u && spec.capacity_bytes == 0x4000u) ||
         ((spec.family_code == 0x37u || spec.family_code == 0x38u ||
           spec.family_code == 0x39u || spec.family_code == 0x3au ||
           spec.family_code == 0x3bu || spec.family_code == 0x50u ||
           spec.family_code == 0x52u) &&
          spec.pinmap == 0u && spec.capacity_bytes == 0x2000u) ||
         ((spec.pinmap == 6u || spec.pinmap == 7u) &&
          spec.capacity_bytes == 0x8000u) ||
         (spec.pinmap == 8u && spec.capacity_bytes == 0x10000u));
    if (source_eprom_group(out_audit)) out_audit->exact_engine_envelope = 1;
    out_audit->strict_pin_mapping =
        pin_map_matches_reference(pin_map, spec.pinmap, spec.capacity_bytes);
    if (source_eprom_group(out_audit) && entry.device_class!=2001u) {
        lt48_pin_group addr,data,gnd,vcc;
        unsigned address_group=entry.device_class==2009u?5u:4u;
        unsigned bits=spec.capacity_bytes==8192u?13u:spec.capacity_bytes==16384u?14u:
                      spec.capacity_bytes==32768u?15u:16u;
        out_audit->strict_pin_mapping=
            lt48_pin_map_find_group(pin_map,address_group,&addr)==LT48_OK && ((eprom3_group(out_audit)||novatek_umc6_group(out_audit)||hitachipulse2_group(out_audit))?addr.pin_count>=bits&&addr.pin_count<=24u:addr.pin_count==bits) &&
            lt48_pin_map_find_group(pin_map,address_group+1u,&data)==LT48_OK && data.pin_count==8u &&
            lt48_pin_map_find_group(pin_map,1,&gnd)==LT48_OK && gnd.pin_count==1 && gnd.pins[0]==92 &&
            lt48_pin_map_find_group(pin_map,2,&vcc)==LT48_OK && vcc.pin_count==1 && vcc.pins[0]==148;
    }
    if((LT48_HOLTEK040ONE_ADMITTED&&holtek040one_group(out_audit))||large_eprom_group(out_audit)||electrical_eprom_group(out_audit)||eprom3_group(out_audit)||novatek_umc6_group(out_audit)||hitachipulse2_group(out_audit)) {
        lt48_pin_group addr,data,gnd,vcc;
        unsigned width=(entry.device_class==2003u || entry.device_class==2004u)?2u:1u;
        unsigned address_group=width==2?5u:4u, bits=0;
        for(uint32_t n=(spec.capacity_bytes/width)-1;n;n>>=1)bits++;
        /* MBM27C1028 multiplexes 16 data/address lines; P5 includes an
         * additional address/control line, as bound in the original REZ. */
        if(entry.device_class==2004u && (entry.device_type==7u || spec.pinmap==23u))bits++;
        out_audit->strict_pin_mapping=
            spec.element_bits==width*8u &&
            lt48_pin_map_package_pin_count(pin_map)==spec.pin_count &&
            lt48_pin_map_find_group(pin_map,address_group,&addr)==LT48_OK && ((eprom3_group(out_audit)||novatek_umc6_group(out_audit)||hitachipulse2_group(out_audit))?addr.pin_count>=bits&&addr.pin_count<=24u:addr.pin_count==bits) &&
            lt48_pin_map_find_group(pin_map,address_group+1,&data)==LT48_OK && data.pin_count==width*8u &&
            lt48_pin_map_find_group(pin_map,1,&gnd)==LT48_OK && gnd.pin_count>0 &&
            lt48_pin_map_find_group(pin_map,2,&vcc)==LT48_OK && vcc.pin_count>0;
    }
    out_audit->required_compact_waves_strict =
        compact_strict && (required_mask == 0x1fu ||
                          ((large_eprom_group(out_audit)||eprom3_group(out_audit)||novatek_umc6_group(out_audit)||hitachipulse2_group(out_audit)) && required_mask == 0x1bu));
    out_audit->applet_metadata_complete = applet_metadata;
    out_audit->applet_ranges_nonoverlapping =
        ranges_do_not_overlap(ranges, range_count);
    out_audit->active_fpga_exactly_one =
        out_audit->active_fpga_count == 1u && fpga_framing;
    if (nominal_count != 1u || out_audit->nominal_vcc_centivolts < 100u ||
        out_audit->nominal_vcc_centivolts > 700u) {
        out_audit->exact_engine_envelope = 0;
    }
    status = LT48_OK;

cleanup:
    free(ranges);
    free(variables);
    lt48_pin_map_close(pin_map);
    lt48_rez_close(rez);
    return status;
}

void lt48_m37_resources_close(lt48_m37_resources *resources) {
    size_t index;
    if (resources == NULL) return;
    for (index = 0u; index < resources->compiled_count; ++index) {
        lt48_compiled_wave_close(resources->compiled[index].compiled);
    }
    free(resources->applets);
    free(resources->compiled);
    free(resources->variables);
    free(resources->idle_fpga);
    lt48_pin_map_close(resources->pin_map);
    lt48_rez_close(resources->rez);
    free(resources);
}

static int audit_is_executable(const lt48_m37_resource_audit *audit) {
    return audit->exact_engine_envelope && audit->strict_pin_mapping &&
           audit->required_compact_waves_strict &&
           audit->applet_metadata_complete &&
           audit->applet_ranges_nonoverlapping &&
           audit->active_fpga_exactly_one;
}

static lt48_status validate_approved_read_behavior(
    lt48_m37_resources *resources
) {
    static const struct {
        uint32_t capacity_bytes;
        unsigned char digest[LT48_SHA256_SIZE];
        unsigned recovery_profile;
    } approved_read[] = {
        {
            0x2000u,
            {
                0x55,0x78,0x63,0xfa,0xa3,0xa5,0x74,0x2a,
                0x5c,0xe2,0x26,0x2d,0x14,0x0f,0x25,0x68,
                0x6c,0x89,0xa1,0xcf,0x9f,0xa5,0x60,0xc2,
                0xd7,0x56,0x6f,0xb0,0x90,0x46,0x2f,0xa9
            },
            0u
        },
        {
            0x4000u,
            {
                0x01,0x2c,0xad,0xc6,0x60,0x8f,0xa1,0xf1,
                0x6c,0x15,0xe3,0x79,0xa4,0x43,0x41,0x39,
                0xda,0x80,0x05,0xf0,0xd4,0xf6,0xd2,0xab,
                0xd3,0x7d,0x6e,0x72,0x49,0xb0,0x1c,0xda
            },
            0u
        },
        /* Dataman 13.10 M0/D1 16 KiB Read. This is the exact selected-
         * device procedure replayed against the complete original Windows
         * success capture plus native cleanup; it is deliberately a
         * Read-only admission. */
        {
            0x4000u,
            {
                0xbf,0x1f,0x0d,0xe2,0xa3,0x9d,0xe2,0x95,
                0x51,0xe3,0xc6,0x71,0xc6,0x95,0xb4,0xba,
                0xb7,0x32,0x12,0x39,0x92,0xcf,0x46,0x8c,
                0x49,0x6c,0x57,0xd7,0xc0,0x5a,0x40,0xbd
            },
            3u
        },
        {
            0x8000u,
            {
                0xc1,0x0c,0xb9,0x7f,0x48,0xca,0x5d,0xa5,
                0xec,0x69,0x6c,0x0b,0xd0,0xe5,0x13,0xcf,
                0x1b,0x0a,0xd4,0x82,0x75,0x0c,0xf9,0x41,
                0xfb,0x6c,0xe0,0xdd,0x4c,0xb0,0x59,0x5e
            },
            0u
        },
        {
            0x8000u,
            {
                0xb2,0x23,0xe6,0xa3,0xac,0xac,0x60,0x95,
                0x02,0xcf,0xbd,0xae,0xa7,0xc6,0xee,0x6e,
                0xb1,0x0e,0xdc,0x68,0xd5,0x6e,0xf2,0xc4,
                0xb6,0x76,0xf7,0x92,0xe6,0x03,0x38,0xeb
            },
            0u
        },
        {
            0x8000u,
            {
                0xa2,0x86,0x74,0xaf,0x96,0x70,0x26,0x6c,
                0xe2,0x1a,0xc4,0xf1,0x6c,0xe3,0x57,0x7c,
                0x49,0xa1,0x72,0x2a,0x08,0x6a,0x33,0x45,
                0x5d,0x31,0x0d,0x60,0x84,0x23,0xc5,0x5a
            },
            0u
        },
        {
            0x8000u,
            {
                0x14,0x8b,0xa1,0x79,0x76,0x0b,0x8e,0x88,
                0xc2,0x24,0xa8,0xbf,0x0b,0xca,0x8e,0x19,
                0xa7,0xa6,0x60,0xd5,0x86,0x79,0x99,0x36,
                0xaf,0xb5,0x83,0x64,0x46,0xce,0x08,0x26
            },
            0u
        },
        {
            0x10000u,
            {
                0x37,0x79,0x17,0xf8,0xc1,0x3a,0xf3,0x27,
                0xbf,0x5d,0xb0,0x18,0x73,0x3b,0x75,0x3e,
                0xd8,0x3c,0xb8,0x06,0xd8,0x33,0xe4,0xcd,
                0x38,0x8c,0xc8,0x4f,0x31,0x17,0xd3,0xed
            },
            1u
        },
        {
            0x10000u,
            {
                0x3d,0x5e,0x49,0x7e,0x74,0x00,0xa8,0x7e,
                0xa3,0xec,0x50,0xd9,0x06,0x22,0x1e,0x37,
                0xf5,0xbc,0x5c,0xcb,0x1a,0xb1,0x88,0xc3,
                0x07,0xb4,0x99,0x5d,0xd7,0xd8,0x2b,0x5d
            },
            1u
        },
        {
            0x10000u,
            {
                0x31,0x2c,0xf5,0x7b,0xa7,0x28,0x39,0x8b,
                0x5e,0x7a,0x39,0x42,0xb1,0x6b,0xb2,0xa1,
                0x77,0xe6,0xf7,0x8d,0x0a,0xbf,0x30,0x05,
                0x14,0x08,0xf2,0xa0,0xfb,0xb5,0x75,0xa6
            },
            2u
        },
        {
            0x10000u,
            {
                0xb5,0x89,0xf6,0x7c,0x76,0x42,0x08,0x3e,
                0x59,0x7c,0xbe,0x21,0x5d,0x73,0xa7,0x87,
                0xbe,0xe5,0x45,0x02,0xd3,0xea,0x6a,0x42,
                0x97,0x3d,0xd7,0x04,0x29,0x45,0xfc,0x81
            },
            1u
        }
    };
    static const unsigned char powered_recovery_64k[LT48_SHA256_SIZE] = {
        0x23,0x5f,0x4d,0x86,0xf8,0x39,0x97,0xd2,
        0x47,0xce,0x93,0xa5,0x6b,0x77,0x7c,0x46,
        0xce,0x94,0xde,0xdf,0x1f,0xf5,0x24,0xc0,
        0xb6,0xff,0xb8,0x8d,0x9f,0xa8,0xb9,0x8f
    };
    static const unsigned char powered_recovery_mx26c512a[LT48_SHA256_SIZE] = {
        0x73,0xc7,0xa8,0x4f,0x71,0x38,0xf9,0x3a,
        0x60,0xff,0x9b,0xf3,0xcd,0xa5,0xf6,0x5c,
        0x49,0x47,0xef,0x55,0x31,0x60,0xfa,0x23,
        0x33,0xbf,0x55,0x96,0x88,0x4a,0x01,0x14
    };
    static const unsigned char dataman_m0_initialized_recovery[LT48_SHA256_SIZE] = {
        0x18,0x14,0x21,0xc8,0x9e,0x14,0xfa,0x07,
        0x20,0x4c,0x71,0x74,0xf3,0xdb,0xc1,0x39,
        0xc0,0x7a,0xcc,0x20,0x55,0xbf,0x3f,0x89,
        0x37,0xb7,0x12,0x01,0x66,0x47,0x1c,0x48
    };
    static const unsigned char dataman_m0_powered_recovery[LT48_SHA256_SIZE] = {
        0x7a,0x04,0xf9,0x1a,0x27,0xce,0xd9,0xfb,
        0x09,0xb0,0xe3,0x7a,0x70,0x68,0x20,0x95,
        0x7b,0x7c,0x5a,0x55,0x34,0x9c,0xa4,0x16,
        0x6e,0x47,0x9c,0xa3,0x5e,0xee,0xf0,0x99
    };
    static const struct {
        lt48_m37_recovery_state state;
        unsigned char digest[LT48_SHA256_SIZE];
    } approved_recovery[] = {
        {
            LT48_M37_RECOVERY_INITIALIZED,
            {
                0xf5,0x07,0x46,0x07,0xa7,0x8f,0x9a,0x2a,
                0xf3,0xf3,0x8a,0x86,0x06,0x4c,0x0c,0x15,
                0xeb,0x8a,0xcd,0x93,0xea,0x1f,0x56,0x16,
                0xfa,0xfc,0x26,0xc9,0x3d,0xcc,0x6f,0xd7
            }
        },
        {
            LT48_M37_RECOVERY_POWERED,
            {
                0xd6,0x9a,0x52,0x1e,0x23,0x3d,0x57,0xb1,
                0x47,0xed,0x64,0xc6,0x72,0x20,0x96,0x58,
                0x70,0xbb,0x9b,0x39,0xa6,0x34,0x3d,0xb2,
                0xdd,0x5a,0xce,0x15,0xce,0x7c,0x19,0xa6
            }
        }
    };
    const lt48_m37_resource_audit *audit =
        lt48_m37_resources_audit(resources);
    lt48_protocol_plan *plan = NULL;
    unsigned char digest[LT48_SHA256_SIZE];
    lt48_status status;
    if (audit == NULL || audit->capacity_bytes == 0u)
        return LT48_SAFETY_REJECTED;
    status = lt48_m37_resources_build_offline_read_plan(
        resources, 0u, audit->capacity_bytes, &plan);
    if (status == LT48_OK)
        status = lt48_protocol_plan_sha256(plan, digest);
    lt48_protocol_plan_close(plan);
    plan = NULL;
    if (status != LT48_OK) return status;
    int read_approved = 0;
    unsigned recovery_profile = 0u;
    for (size_t index = 0u;
         index < sizeof(approved_read) / sizeof(approved_read[0]); ++index) {
        if (audit->capacity_bytes == approved_read[index].capacity_bytes &&
            memcmp(digest, approved_read[index].digest, sizeof(digest)) == 0) {
            read_approved = 1;
            recovery_profile = approved_read[index].recovery_profile;
            break;
        }
    }
    if (!read_approved) {
        /* Complete original M0/D1 16KiB forward capture, normalized to
         * selected-device policy. This certificate does not approve recovery. */
        static const unsigned char captured_forward[LT48_SHA256_SIZE] = {
            0x0a,0x6e,0xc7,0xcc,0xd6,0x4c,0x8a,0x1f,
            0xda,0x28,0x26,0xb6,0xe6,0x54,0xdb,0x87,
            0x72,0x6d,0x37,0x5a,0x0d,0x02,0x40,0xe7,
            0xb3,0xb2,0x9f,0x2f,0x4d,0xc0,0x4c,0x0f
        };
        if (audit->device_class != 2001u || audit->family_code != 0u ||
            audit->capacity_bytes != 16384u ||
            memcmp(audit->definition_sha256, dataman_m0_forward_definition,
                   sizeof(dataman_m0_forward_definition)) != 0)
            return LT48_SAFETY_REJECTED;
        status = lt48_m37_resources_build_offline_read_plan(
            resources, 0u, audit->capacity_bytes, &plan);
        if (status == LT48_OK)
            status = lt48_protocol_plan_select_id_policy_internal(plan);
        if (status == LT48_OK) status = lt48_protocol_plan_sha256(plan, digest);
        lt48_protocol_plan_close(plan);
        if (status != LT48_OK || memcmp(digest, captured_forward, sizeof(digest)))
            return LT48_SAFETY_REJECTED;
        resources->audit.read_forward_reference_verified = 1;
        return LT48_OK;
    }
    for (size_t index = 0u;
         index < sizeof(approved_recovery) / sizeof(approved_recovery[0]);
         ++index) {
        status = lt48_m37_resources_build_recovery_plan(
            resources, approved_recovery[index].state, &plan);
        if (status == LT48_OK)
            status = lt48_protocol_plan_sha256(plan, digest);
        lt48_protocol_plan_close(plan);
        plan = NULL;
        if (status != LT48_OK) return status;
        const unsigned char *expected_digest = approved_recovery[index].digest;
        if (approved_recovery[index].state == LT48_M37_RECOVERY_POWERED &&
            recovery_profile == 1u) {
            expected_digest = powered_recovery_64k;
        } else if (approved_recovery[index].state == LT48_M37_RECOVERY_POWERED &&
                   recovery_profile == 2u) {
            expected_digest = powered_recovery_mx26c512a;
        } else if (recovery_profile == 3u) {
            expected_digest = approved_recovery[index].state ==
                LT48_M37_RECOVERY_INITIALIZED ?
                dataman_m0_initialized_recovery : dataman_m0_powered_recovery;
        }
        if (memcmp(digest, expected_digest,
                   sizeof(digest)) != 0)
            return LT48_SAFETY_REJECTED;
    }
    resources->audit.read_original_replay_verified = recovery_profile == 3u;
    return LT48_OK;
}

lt48_status lt48_m37_resources_open(
    const lt48_installation *installation,
    size_t device_index,
    lt48_m37_resources **out_resources
) {
    lt48_m37_resources *resources = NULL;
    lt48_catalogue_entry entry;
    lt48_device_spec spec;
    lt48_family_info family;
    lt48_pin_code pin_code;
    unsigned char snapshot_digest[LT48_SHA256_SIZE];
    char *path = NULL;
    char name[16];
    size_t family_index;
    size_t pin_index;
    size_t operation_index;
    lt48_status status;
    if (installation == NULL || out_resources == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_resources = NULL;
    resources = calloc(1, sizeof(*resources));
    if (resources == NULL) return LT48_NO_MEMORY;
    status = lt48_installation_audit_m37(
        installation, device_index, &resources->audit
    );
    if (status != LT48_OK) goto fail;
    if (!audit_is_executable(&resources->audit)) {
        status = LT48_SAFETY_REJECTED;
        goto fail;
    }
    status = lt48_installation_get_device(installation, device_index, &entry);
    if (status == LT48_OK) {
        status = lt48_installation_resolve_device(
            installation, device_index, &spec
        );
    }
    if(status==LT48_OK&&holtek040one_group(&resources->audit)&&strcmp(entry.vendor,"HOLTEK")){status=LT48_SAFETY_REJECTED;goto fail;}
    if(status==LT48_OK&&electrical_eprom_group(&resources->audit)&&strcmp(entry.vendor,"SST")&&strcmp(entry.vendor,"Winbond")&&!(m26c201one_group(&resources->audit)&&!strcmp(entry.vendor,"ST"))&&!(macronixmtp2_group(&resources->audit)&&!strcmp(entry.vendor,"Macronix"))&&!green14_binding(&entry)&&!(eprom3_electrical(&resources->audit)&&((resources->audit.device_type==0x3a&&!strcmp(entry.vendor,"ALi"))||(resources->audit.device_type==0x3b&&!strcmp(entry.vendor,"PTC"))))){status=LT48_SAFETY_REJECTED;goto fail;}
    if (status == LT48_OK) {
        resources->catalogue_manufacturer_id = entry.manufacturer_id;
        resources->catalogue_device_id = entry.device_id;
        status = make_definition_path(installation, entry.device_class, &path);
    }
    if (status == LT48_OK) status = lt48_rez_open(path, &resources->rez);
    free(path);
    path = NULL;
    if (status == LT48_OK) {
        status = lt48_rez_sha256(resources->rez, snapshot_digest);
    }
    if (status != LT48_OK) goto fail;
    if (memcmp(snapshot_digest, resources->audit.definition_sha256,
               LT48_SHA256_SIZE) != 0) {
        status = LT48_SAFETY_REJECTED;
        goto fail;
    }
    status = load_verified_idle_fpga(
        installation, &resources->idle_fpga, &resources->idle_fpga_size
    );
    if (status != LT48_OK) goto fail;

    snprintf(name, sizeof(name), "M%x", spec.family_code);
    status = lt48_rez_find_resource(resources->rez, name, &family_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_family_info(
            resources->rez, family_index, &family
        );
    }
    snprintf(name, sizeof(name), "P%x", spec.pinmap);
    if (status == LT48_OK) {
        status = lt48_rez_find_resource(resources->rez, name, &pin_index);
    }
    if (status == LT48_OK) {
        status = lt48_rez_get_pin_code(resources->rez, pin_index, &pin_code);
    }
    if (status == LT48_OK) {
        status = lt48_pin_map_parse(&pin_code, &resources->pin_map);
    }
    if (status != LT48_OK) goto fail;

    resources->variables = calloc(
        family.variable_count == 0u ? 1u : family.variable_count,
        sizeof(*resources->variables)
    );
    resources->compiled = calloc(
        family.operation_count == 0u ? 1u : family.operation_count,
        sizeof(*resources->compiled)
    );
    resources->applets = calloc(
        family.operation_count == 0u ? 1u : family.operation_count,
        sizeof(*resources->applets)
    );
    if (resources->variables == NULL || resources->compiled == NULL ||
        resources->applets == NULL) {
        status = LT48_NO_MEMORY;
        goto fail;
    }
    resources->read_nominal_override=electrical_eprom_group(&resources->audit)?eep21_read_nominal(&spec):0;
    if(resources->read_nominal_override){resources->audit.nominal_vcc_centivolts=resources->read_nominal_override;resources->audit.expected_vcc_selector=7;}
    resources->variable_count = family.variable_count;
    for (operation_index = 0u; operation_index < family.variable_count;
         ++operation_index) {
        lt48_family_variable source;
        status = lt48_rez_get_family_variable(
            resources->rez, family_index, operation_index, &source
        );
        if (status != LT48_OK) goto fail;
        resources->variables[operation_index].kind = source.values[0];
        resources->variables[operation_index].variable_id = source.values[1];
        resources->variables[operation_index].enabled = source.values[2];
        resources->variables[operation_index].value = source.values[3];
        resources->variables[operation_index].channel = source.values[4];
    }
    if(resources->read_nominal_override)for(size_t vi=0;vi<resources->variable_count;vi++){
        lt48_compile_variable*v=&resources->variables[vi];
        if(v->variable_id==0||v->variable_id==4){if(v->value!=500){status=LT48_SAFETY_REJECTED;goto fail;}v->value=330;}
    }
    for (operation_index = 0u; operation_index < family.operation_count;
         ++operation_index) {
        lt48_family_operation operation;
        lt48_wave_code wave;
        size_t wave_index;
        status = lt48_rez_get_family_operation(
            resources->rez, family_index, operation_index, &operation
        );
        if (status != LT48_OK) goto fail;
        snprintf(name, sizeof(name), "W%x", operation.wave);
        status = lt48_rez_find_resource(resources->rez, name, &wave_index);
        if (status == LT48_OK) {
            status = lt48_rez_get_wave_code(
                resources->rez, wave_index, &wave
            );
        }
        if (status != LT48_OK) goto fail;
        if (wave.wave_type == 0u) {
            compiled_resource *target =
                &resources->compiled[resources->compiled_count];
            lt48_compact_wave *compact = NULL;
            status = lt48_compact_decode(
                wave.data, wave.data_size, &compact
            );
            if (status == LT48_OK) {
                status = lt48_compact_compile_with_pin_policy(
                    compact, resources->pin_map, resources->variables,
                    resources->variable_count, 1u, LT48_PIN_FALLBACK_RESIDENT_NOOP, &target->compiled
                );
            }
            lt48_compact_wave_close(compact);
            if (status != LT48_OK ||
                (target->compiled != NULL &&
                 !compiled_pins_resolved(target->compiled))) {
                if (status == LT48_OK) {
                    status = LT48_SAFETY_REJECTED;
                    lt48_compiled_wave_close(target->compiled);
                    target->compiled = NULL;
                }
                goto fail;
            }
            target->operation_id = operation.operation_id;
            target->wave = operation.wave;
            ++resources->compiled_count;
        } else if (wave.wave_type == 6u) {
            lt48_m37_applet_resource *target =
                &resources->applets[resources->applet_count++];
            target->operation_id = operation.operation_id;
            target->wave = operation.wave;
            target->load_address = wave.load_address;
            target->data = wave.data;
            target->data_size = wave.data_size;
        } else if (wave.wave_type == 4u) {
            resources->active_fpga = wave.data + 2u;
            resources->active_fpga_size = wave.data_size - 2u;
        }
    }
    if (resources->compiled_count != resources->audit.compact_wave_count ||
        resources->applet_count != resources->audit.applet_count ||
        resources->active_fpga == NULL ||
        resources->active_fpga_size != 2778u) {
        status = LT48_SAFETY_REJECTED;
        goto fail;
    }
    resources->audit.complete_source_audit_verified = source_eprom_group(&resources->audit);
    status = resources->audit.device_class==2001u ? validate_approved_read_behavior(resources) : LT48_SAFETY_REJECTED;
    if (resources->audit.complete_source_audit_verified) status = LT48_OK;
    if (status != LT48_OK) goto fail;
    /* The other digest-locked capacities have reconstructed plans, but no
     * captured Windows run or representative hardware qualification. */
    /* These five P0 8 KiB definitions have source-identical P5
     * 16 KiB family counterparts (listed in the group-equivalence audit). All five compiled waves, all applets, FPGA,
     * electrical variables and transactions for the common address range
     * are identical. Restrict inheritance to the audited source definition;
     * another 8 KiB family is not admitted by its Read digest alone.
     * See docs/audit-qualification/EPROM_2764_GROUP.md and the group-equivalence test. */
    static const unsigned char group_source[LT48_SHA256_SIZE] = {
        0x2e,0x62,0x61,0xc8,0x67,0xd6,0x85,0x77,
        0x86,0x93,0x3c,0x0d,0x09,0x3e,0x5b,0xd6,
        0x69,0xf7,0x03,0x55,0xcc,0x21,0x8f,0xf0,
        0x06,0xc8,0x8c,0x4f,0xb9,0x19,0x63,0x6b
    };
    resources->audit.complete_group_equivalence_verified =
        ((resources->audit.device_type == 0x46u &&
          resources->audit.family_code == 0x37u) ||
         (resources->audit.device_type == 0x48u &&
          resources->audit.family_code == 0x39u) ||
         (resources->audit.device_type == 0x4au &&
          resources->audit.family_code == 0x3bu) ||
         (resources->audit.device_type == 0x65u &&
          resources->audit.family_code == 0x50u) ||
         (resources->audit.device_type == 0x66u &&
          resources->audit.family_code == 0x52u)) &&
        resources->audit.pinmap == 0u &&
        resources->audit.capacity_bytes == 0x2000u &&
        memcmp(resources->audit.definition_sha256, group_source,
               sizeof(group_source)) == 0;
    resources->audit.read_behavior_approved =
        (resources->audit.capacity_bytes == 0x4000u &&
         !resources->audit.read_forward_reference_verified) ||
        resources->audit.complete_group_equivalence_verified ||
        resources->audit.complete_source_audit_verified;
    *out_resources = resources;
    return LT48_OK;

fail:
    free(path);
    lt48_m37_resources_close(resources);
    return status;
}

const lt48_m37_resource_audit *lt48_m37_resources_audit(
    const lt48_m37_resources *resources
) {
    return resources == NULL ? NULL : &resources->audit;
}

const lt48_pin_map *lt48_m37_resources_pin_map(
    const lt48_m37_resources *resources
) {
    return resources == NULL ? NULL : resources->pin_map;
}

const lt48_compile_variable *lt48_m37_resources_variables(
    const lt48_m37_resources *resources, size_t *out_count
) {
    if (out_count != NULL) {
        *out_count = resources == NULL ? 0u : resources->variable_count;
    }
    return resources == NULL ? NULL : resources->variables;
}

lt48_status lt48_m37_resources_compiled_wave(
    const lt48_m37_resources *resources,
    uint16_t operation_id,
    uint16_t *out_wave,
    const lt48_compiled_wave **out_compiled
) {
    size_t index;
    if (resources == NULL || out_wave == NULL || out_compiled == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_compiled = NULL;
    /* Dataman 13.10 replaces a compiled slot as each family binding is
       loaded. Retained source entries must therefore resolve last-first. */
    for (index = resources->compiled_count; index > 0u; --index) {
        if (resources->compiled[index - 1u].operation_id == operation_id) {
            *out_wave = resources->compiled[index - 1u].wave;
            *out_compiled = resources->compiled[index - 1u].compiled;
            return LT48_OK;
        }
    }
    return LT48_NOT_FOUND;
}

size_t lt48_m37_resources_applet_count(
    const lt48_m37_resources *resources
) {
    return resources == NULL ? 0u : resources->applet_count;
}

lt48_status lt48_m37_resources_get_applet(
    const lt48_m37_resources *resources,
    size_t index,
    lt48_m37_applet_resource *out_applet
) {
    if (resources == NULL || out_applet == NULL) return LT48_INVALID_ARGUMENT;
    if (index >= resources->applet_count) return LT48_NOT_FOUND;
    *out_applet = resources->applets[index];
    return LT48_OK;
}

lt48_status lt48_m37_resources_active_fpga(
    const lt48_m37_resources *resources,
    const unsigned char **out_data,
    size_t *out_size
) {
    if (resources == NULL || out_data == NULL || out_size == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_data = resources->active_fpga;
    *out_size = resources->active_fpga_size;
    return LT48_OK;
}

lt48_status lt48_m37_resources_idle_fpga(
    const lt48_m37_resources *resources,
    const unsigned char **out_data,
    size_t *out_size
) {
    if (resources == NULL || out_data == NULL || out_size == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_data = resources->idle_fpga;
    *out_size = resources->idle_fpga_size;
    return LT48_OK;
}

lt48_status lt48_m37_resources_catalogue_ids_internal(
    const lt48_m37_resources *resources, uint16_t *manufacturer, uint16_t *device
) {
    if (!resources || !manufacturer || !device) return LT48_INVALID_ARGUMENT;
    *manufacturer = resources->catalogue_manufacturer_id;
    *device = resources->catalogue_device_id;
    return LT48_OK;
}

unsigned lt48_m37_eep21_write_voltage_internal(const lt48_m37_resources*r){return r&&r->read_nominal_override?500:0;}

unsigned lt48_eep21_read_nominal_internal(const lt48_device_spec*s){return eep21_definition(s->catalogue.device_class,s->definition_resource)?eep21_read_nominal(s):0;}
