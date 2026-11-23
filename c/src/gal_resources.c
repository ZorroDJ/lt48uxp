#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../NewDeviceLib/sources/gal59/profiles.h"
#include "../../NewDeviceLib/sources/cpld69/profiles.h"
#include "../../NewDeviceLib/sources/palce54/profiles.h"
#include "../../NewDeviceLib/sources/palce8/profiles.h"
#include "../../NewDeviceLib/sources/mach11/profiles.h"
#include "../../NewDeviceLib/sources/mach11/callbacks.h"
#include "../../NewDeviceLib/sources/ejed9/profiles.h"
#include "../../NewDeviceLib/sources/ejed9/callbacks.h"
#include "../../NewDeviceLib/sources/ejed9/geometry.h"

/* This is deliberately a resource closure, rather than a GAL executor.  The
 * package digest, family and pin-map are all part of the admission boundary;
 * callers can inspect and compile the exact Windows resource without gaining
 * a path to USB execution. */
#define LT48_GAL_MAX_WAVES 64u

struct lt48_gal_resources {
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compile_variable *variables;
    size_t variable_count;
    lt48_compiled_wave *waves[LT48_GAL_MAX_WAVES];
    uint16_t wave_numbers[LT48_GAL_MAX_WAVES];
    uint16_t definition_resource;
    uint16_t family_code;
    uint16_t pinmap;
    size_t family_index;
    lt48_runtime_frame_profile profile;
    lt48_gal_pes_binding pes;
    int has_pes;
    int ejed;
    int palce;
    int mach11;
    int ejed9;
    unsigned char *mach11_fpga;
    lt48_runtime_embedded_image mach11_image;
    unsigned blank_fill;
    int requires_program_image;
    lt48_fuse_code fuse_code;
    lt48_fuse_code contexts[3];
    size_t context_offsets[3];
    uint16_t context_ids[3];
    size_t context_count;
    lt48_fuse_coordinate *coordinates;
    size_t coordinate_count;
    size_t names_index;
    lt48_defined_name *cached_names;size_t cached_name_count;
    const char *source_version;
};

static void gal_resources_destroy(lt48_gal_resources *resources) {
    if (!resources) return;
    for (size_t i = 0u; i < LT48_GAL_MAX_WAVES; ++i)
        lt48_compiled_wave_close(resources->waves[i]);
    free(resources->mach11_fpga);
    free(resources->cached_names);
    free(resources->coordinates);
    free(resources->variables);
    lt48_pin_map_close(resources->pins);
    lt48_rez_close(resources->rez);
    free(resources);
}

void lt48_gal_resources_close(lt48_gal_resources *resources) {
    gal_resources_destroy(resources);
}

lt48_status lt48_gal_resources_open_definition(
    const char *definition_path, uint16_t definition_resource,
    uint16_t family_code, uint16_t pinmap,
    lt48_gal_resources **out_resources
) {
    static const char expected_digest[] =
        "2b82c3e23dde22e68484fa6de0bf93f4ac957486158ab2d0d91a160a3416a380";
    static const uint16_t required_operations[] = {
        0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u
    };
    unsigned char digest[LT48_SHA256_SIZE];
    char digest_hex[LT48_SHA256_SIZE * 2u + 1u];
    char family_name[16];
    char pin_name[16];
    lt48_gal_resources *resources = NULL;
    lt48_status status;
    size_t index, family_index;
    lt48_family_info family;
    lt48_pin_code pin_code;
    lt48_device_definition definition;
    lt48_logic_fuse_codes fuse_bindings;
    lt48_fuse_code fuse_code;
    lt48_runtime_frame_profile profile;
    int present[LT48_GAL_MAX_WAVES] = {0};

    if (!definition_path || !out_resources) return LT48_INVALID_ARGUMENT;
    *out_resources = NULL;
    resources = calloc(1u, sizeof(*resources));
    if (!resources) return LT48_NO_MEMORY;
#define GAL_TRY(call) do { \
        status = (call); \
        if (status != LT48_OK) goto fail; \
    } while (0)

    GAL_TRY(lt48_rez_open(definition_path, &resources->rez));
    GAL_TRY(lt48_rez_sha256(resources->rez, digest));
    for (size_t byte = 0u; byte < sizeof(digest); ++byte)
        snprintf(digest_hex + byte * 2u, 3u, "%02x", digest[byte]);
    resources->ejed = !strcmp(digest_hex, "88cd45163f6ffac436959a4659a6f782aea294d0add67c87bf1f4336fb5d07f1");
    resources->palce = !strcmp(digest_hex, "65753e664d77d1cfab853ae51f6b58de07fc98ab73f88e618a31d0261fd1d846");
    resources->mach11=resources->palce&&mach11_definition(definition_resource,family_code,pinmap);
    if(resources->palce && !resources->mach11 && !(palce54_definition(definition_resource,family_code,pinmap)||palce8_definition(definition_resource,family_code,pinmap))){status=LT48_UNSUPPORTED;goto fail;}
    resources->ejed9=resources->ejed&&ejed9_definition(definition_resource,family_code,pinmap);
    if (resources->ejed && !resources->ejed9 && !cpld69_definition(definition_resource,family_code,pinmap)) {
        status=LT48_UNSUPPORTED;goto fail;
    }
    if (!resources->ejed && !resources->palce && strcmp(digest_hex, expected_digest) != 0) {
        status = LT48_SAFETY_REJECTED;
        goto fail;
    }
    if((resources->mach11 && definition_resource==0x11u)||(resources->ejed9&&definition_resource>=0x2au)){
        const char *slash=strrchr(definition_path,'/');
        if(!slash){status=LT48_NOT_FOUND;goto fail;}
        size_t length=(size_t)(slash-definition_path);char *root=malloc(length+1);
        if(!root){status=LT48_NO_MEMORY;goto fail;}memcpy(root,definition_path,length);root[length]=0;
        lt48_pof_trs_source *source=NULL;status=lt48_pof_trs_source_open_internal(root,&source);free(root);
        const unsigned char *image=NULL;size_t bytes=0;
        if(status==LT48_OK)status=lt48_pof_source_fpga_internal(source,1,&image,&bytes);
        if(status==LT48_OK&&bytes!=0xada)status=LT48_FORMAT_ERROR;
        if(status==LT48_OK){resources->mach11_fpga=malloc(bytes);if(!resources->mach11_fpga)status=LT48_NO_MEMORY;}
        if(status==LT48_OK){memcpy(resources->mach11_fpga,image,bytes);resources->mach11_image=(lt48_runtime_embedded_image){1,resources->mach11_fpga,bytes};}
        lt48_pof_trs_source_close_internal(source);if(status!=LT48_OK)goto fail;
    }
    resources->source_version = "Dataman 13.10";
    resources->definition_resource = definition_resource;
    resources->blank_fill=255u;

    if (definition_resource != UINT16_MAX) {
        snprintf(family_name, sizeof(family_name), "D%x", definition_resource);
        GAL_TRY(lt48_rez_find_resource(
            resources->rez, family_name, &index));
        GAL_TRY(lt48_rez_get_device_definition(
            resources->rez, index, &definition));
        resources->blank_fill=definition.value3;
        if (definition.kind != LT48_DEVICE_DEFINITION_LOGIC ||
            definition.family_code != family_code ||
            definition.pinmap != pinmap) {
            status = LT48_DEVICE_MISMATCH;
            goto fail;
        }
    }

    snprintf(family_name, sizeof(family_name), "M%x", family_code);
    snprintf(pin_name, sizeof(pin_name), "P%x", pinmap);
    GAL_TRY(lt48_rez_find_resource(resources->rez, family_name, &family_index));
    GAL_TRY(lt48_rez_get_family_info(resources->rez, family_index, &family));
    if (!family.operation_count || family.operation_count > LT48_GAL_MAX_WAVES ||
        family.variable_count > 256u) {
        status = LT48_UNSUPPORTED;
        goto fail;
    }
    GAL_TRY(lt48_rez_find_resource(resources->rez, pin_name, &index));
    GAL_TRY(lt48_rez_get_pin_code(resources->rez, index, &pin_code));
    GAL_TRY(lt48_pin_map_parse(&pin_code, &resources->pins));
    GAL_TRY(lt48_rez_get_family_frame_profile(resources->rez, family_index, &profile));
    if (!profile.buffer_count || profile.mode_buffer >= profile.buffer_count) {
        status = LT48_FORMAT_ERROR;
        goto fail;
    }
    resources->profile = profile;
    if(resources->ejed9){if(profile.buffer_count!=76){status=LT48_FORMAT_ERROR;goto fail;}resources->profile.buffer_count=81;}
    /* Source declares17 named buffers. Extra private scratch replaces its heap row. */
    if(resources->mach11){if(profile.buffer_count!=17){status=LT48_FORMAT_ERROR;goto fail;}resources->profile.buffer_count=76;}

    /* A selected logic definition owns the primary JEDEC geometry. Keep the
     * exact fuse order alongside the family resources so the future session
     * owner cannot accidentally use a shared family with another Dxx layout. */
    if (definition_resource != UINT16_MAX) {
        snprintf(family_name, sizeof(family_name), "D%x", definition_resource);
        GAL_TRY(lt48_rez_find_resource(resources->rez, family_name, &index));
        GAL_TRY(lt48_rez_get_logic_fuse_codes(
            resources->rez, index, &fuse_bindings));
        if (fuse_bindings.count == 0u) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        snprintf(family_name, sizeof(family_name), "F%x",
                 fuse_bindings.identifiers[0]);
        GAL_TRY(lt48_rez_find_resource(resources->rez, family_name, &index));
        GAL_TRY(lt48_rez_get_fuse_code(resources->rez, index, &fuse_code));
        resources->fuse_code = fuse_code;
        resources->contexts[0]=fuse_code;resources->context_ids[0]=fuse_bindings.identifiers[0];resources->context_count=1;
        if (fuse_code.total_fuse_count == 0u ||
            fuse_code.total_fuse_count > 1000000u) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        resources->coordinates = calloc(
            fuse_code.total_fuse_count, sizeof(*resources->coordinates));
        if (!resources->coordinates) {
            status = LT48_NO_MEMORY;
            goto fail;
        }
        GAL_TRY(lt48_fuse_layout_expand_coordinates(
            &fuse_code, resources->coordinates, fuse_code.total_fuse_count,
            &resources->coordinate_count));
        if (resources->coordinate_count != fuse_code.total_fuse_count) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
    }

    if(resources->ejed){
        const uint16_t ids[]={fuse_bindings.e_field_identifier,fuse_bindings.user_code_identifier};
        const uint32_t counts[]={fuse_bindings.e_field_value_count,fuse_bindings.user_code_value_count};
        const int present[]={fuse_bindings.has_e_field,fuse_bindings.has_user_code};
        for(unsigned i=0;i<2;i++)if(present[i]){
            size_t slot=resources->context_count;lt48_fuse_code code;
            snprintf(family_name,sizeof family_name,"F%x",ids[i]);
            GAL_TRY(lt48_rez_find_resource(resources->rez,family_name,&index));
            GAL_TRY(lt48_rez_get_fuse_code(resources->rez,index,&code));
            /* Authored finite repair: the source iterator and layout own73
             * coordinates; its descriptor allocates only72 editable values.
             * Keep all source cycles, with73 bounded values in C and GUI. */
            unsigned expected=resources->ejed9 ? ejed9_aux_count(definition_resource,
                family_code,pinmap,i,ids[i],counts[i],code.total_fuse_count) : counts[i];
            if(code.total_fuse_count!=expected || code.total_fuse_count>1000000u-resources->coordinate_count){status=LT48_SAFETY_REJECTED;goto fail;}
            size_t total=resources->coordinate_count+code.total_fuse_count;
            void *memory=realloc(resources->coordinates,total*sizeof(*resources->coordinates));
            if(!memory){status=LT48_NO_MEMORY;goto fail;}resources->coordinates=memory;
            size_t n=0;GAL_TRY(lt48_fuse_layout_expand_coordinates(&code,resources->coordinates+resources->coordinate_count,code.total_fuse_count,&n));
            if(n!=code.total_fuse_count){status=LT48_FORMAT_ERROR;goto fail;}
            resources->contexts[slot]=code;resources->context_ids[slot]=ids[i];resources->context_offsets[slot]=resources->coordinate_count;
            resources->coordinate_count=total;resources->context_count++;
        }
    }

    GAL_TRY(lt48_rez_find_resource(resources->rez, "DefineName",
                                   &resources->names_index));
    if((resources->palce && palce8_definition(definition_resource,family_code,pinmap)) ||
       resources->ejed9) {
        lt48_defined_name_collection_info info;GAL_TRY(lt48_rez_get_defined_name_collection_info(resources->rez,resources->names_index,&info));
        if(!info.count||info.count>4096u){status=LT48_FORMAT_ERROR;goto fail;}
        resources->cached_names=calloc(info.count,sizeof(*resources->cached_names));if(!resources->cached_names){status=LT48_NO_MEMORY;goto fail;}
        GAL_TRY(lt48_rez_get_defined_names(resources->rez,resources->names_index,resources->cached_names,info.count,&resources->cached_name_count));
        if(info.count!=resources->cached_name_count){status=LT48_FORMAT_ERROR;goto fail;}
    }

    resources->family_code = family_code;
    resources->pinmap = pinmap;
    resources->family_index = family_index;
    resources->variable_count = family.variable_count;
    resources->variables = calloc(
        family.variable_count ? family.variable_count : 1u,
        sizeof(*resources->variables));
    if (!resources->variables) {
        status = LT48_NO_MEMORY;
        goto fail;
    }
    for (size_t variable = 0u; variable < family.variable_count; ++variable) {
        lt48_family_variable item;
        GAL_TRY(lt48_rez_get_family_variable(
            resources->rez, family_index, variable, &item));
        resources->variables[variable] = (lt48_compile_variable){
            item.values[0], item.values[1], item.values[2],
            item.values[3], item.values[4]};
    }

    status = (resources->ejed || resources->palce) ? LT48_NOT_FOUND : lt48_gal_pes_resolve_binding(resources->rez, family_index, &resources->pes);
    if (status == LT48_OK) resources->has_pes = 1;
    else if (status != LT48_NOT_FOUND ||
        (!resources->ejed && !resources->palce && !gal59_stage(definition_resource,family_code,pinmap) &&
         (definition_resource != 0x32u || family_code != 0x2fu || pinmap != 0xbu)))
        goto fail; /* D32/M2f/Pb has no PES callback in the original package. */
    for (size_t operation_index = 0u;
         operation_index < family.operation_count; ++operation_index) {
        lt48_family_operation operation;
        lt48_wave_code wave_code;
        char wave_name[16];
        lt48_compact_wave *wave = NULL;
        GAL_TRY(lt48_rez_get_family_operation(
            resources->rez, family_index, operation_index, &operation));
        if (operation.operation_id >= LT48_GAL_MAX_WAVES ||
            present[operation.operation_id]) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        present[operation.operation_id] = 1;
        resources->wave_numbers[operation.operation_id] = operation.wave;
        snprintf(wave_name, sizeof(wave_name), "W%x", operation.wave);
        GAL_TRY(lt48_rez_find_resource(resources->rez, wave_name, &index));
        GAL_TRY(lt48_rez_get_wave_code(resources->rez, index, &wave_code));
        if (resources->has_pes && operation.operation_id == resources->pes.operation) {
            if (wave_code.wave_type != 3u) {
                status = LT48_FORMAT_ERROR;
                goto fail;
            }
            continue;
        }
        /* M10 retains an unused legacy PES callback without its parameters.
         * Every compact caller is checked below; no missing field is invented. */
        int unused_pes=definition_resource==0x14u && family_code==0x10u && pinmap==2u;
        if(unused_pes && operation.operation_id==14u && operation.wave==3u &&
           wave_code.wave_type==3u && wave_code.data_size==4u &&
           wave_code.data[0]==2u && !wave_code.data[1] &&
           !wave_code.data[2] && !wave_code.data[3]) continue;
        if(resources->palce && palce8_definition(definition_resource,family_code,pinmap) && wave_code.wave_type==1u && operation.operation_id==26u){
            GAL_TRY(lt48_compiled_word_table_internal(wave_code.data,wave_code.data_size,&resources->waves[operation.operation_id]));continue;
        }
        if(resources->palce && wave_code.wave_type==5u){
            GAL_TRY(lt48_compiled_map_create(wave_code.data,wave_code.data_size,&resources->waves[operation.operation_id]));
            continue;
        }
        if(resources->ejed9 && wave_code.wave_type==3u){
            if(wave_code.data_size!=4u||wave_code.data[0]!=2u||wave_code.data[1]||wave_code.data[3]){status=LT48_FORMAT_ERROR;goto fail;}
            unsigned cb=wave_code.data[2],expected=operation.operation_id==11?0:operation.operation_id==5?1:operation.operation_id==12?2:operation.operation_id>=36&&operation.operation_id<=44?operation.operation_id-33:99;
            if(cb!=expected){status=LT48_FORMAT_ERROR;goto fail;}
            GAL_TRY(ejed9_compile(cb,resources->fuse_code.dimensions[1]+1,&resources->waves[operation.operation_id]));continue;
        }
        if(resources->mach11 && wave_code.wave_type==3u){
            if(wave_code.data_size!=4u||wave_code.data[0]!=2u||wave_code.data[1]||wave_code.data[3]){status=LT48_FORMAT_ERROR;goto fail;}
            unsigned cb=wave_code.data[2],expected=operation.operation_id==5?2:operation.operation_id==14?1:operation.operation_id==15?3:operation.operation_id==22?4:operation.operation_id==23?5:operation.operation_id==24?6:operation.operation_id==25?7:0;
            if(!expected||cb!=expected){status=LT48_FORMAT_ERROR;goto fail;}
            GAL_TRY(mach11_compile(cb,resources->fuse_code.dimensions[1]+1,&resources->waves[operation.operation_id]));continue;
        }
        if (wave_code.wave_type != 0u) {
            status = LT48_UNSUPPORTED;
            goto fail;
        }
        GAL_TRY(lt48_compact_decode(wave_code.data, wave_code.data_size, &wave));
        if(operation.operation_id==2u)for(size_t k=0;k<lt48_compact_instruction_count(wave);k++){
            lt48_compact_instruction instruction;
            status=lt48_compact_get_instruction(wave,k,&instruction);
            if(status==LT48_OK && instruction.opcode==0x89u)resources->requires_program_image=1;
        }
        if(unused_pes)for(size_t k=0;k<lt48_compact_instruction_count(wave);k++){
            lt48_compact_instruction instruction;
            status=lt48_compact_get_instruction(wave,k,&instruction);
            if(status!=LT48_OK || (instruction.opcode==0x3du && instruction.operands[0]==14u)){
                lt48_compact_wave_close(wave);wave=NULL;
                if(status==LT48_OK)status=LT48_SAFETY_REJECTED;
                goto fail;
            }
        }
        if(resources->ejed &&
           ((definition_resource==7u && family_code==7u && pinmap==6u) ||
            (definition_resource==9u && family_code==8u && pinmap==9u) ||
            (definition_resource==11u && family_code==10u && pinmap==9u)) &&
           (operation.operation_id==2u || operation.operation_id==3u)){
            /* Original compiler's zero-filled logical17 is retained at only
             * these six source sites. It is not assigned a physical ZIF pin. */
            status=lt48_compact_compile_with_pin_policy(wave,resources->pins,
                resources->variables,resources->variable_count,1u,
                LT48_PIN_FALLBACK_SOURCE_ZERO_SUPPLY,&resources->waves[operation.operation_id]);
            if(status==LT48_OK){
                lt48_defaulted_logical_pin missing;
                if(lt48_compiled_wave_defaulted_pin_count(resources->waves[operation.operation_id])!=1u ||
                   lt48_compiled_wave_get_defaulted_pin(resources->waves[operation.operation_id],0,&missing)!=LT48_OK ||
                   missing.logical_pin!=17u || missing.compact_offset!=(operation.operation_id==2u?66u:16u) ||
                   missing.compact_opcode!=(operation.operation_id==2u?3u:4u) ||
                   missing.classification!=LT48_DEFAULTED_PIN_SOURCE_ZERO_SUPPLY)status=LT48_SAFETY_REJECTED;
            }
        } else if (!resources->ejed && !resources->palce && definition_resource==0x2au && family_code==0x27u && pinmap==8u &&
            (operation.operation_id==2u || operation.operation_id==6u)) {
            /* P8 omits logical 12. The original zero-filled lookup emits
             * resident RET, not a socket-pin write; preserve only these sites. */
            status=lt48_compact_compile_with_pin_policy(wave,resources->pins,
                resources->variables,resources->variable_count,1u,
                LT48_PIN_FALLBACK_RESIDENT_NOOP,&resources->waves[operation.operation_id]);
            if(status==LT48_OK){
                size_t wanted=operation.operation_id==2u?1u:2u;
                lt48_compiled_wave *compiled=resources->waves[operation.operation_id];
                if(lt48_compiled_wave_defaulted_pin_count(compiled)!=wanted)status=LT48_SAFETY_REJECTED;
                for(size_t k=0;status==LT48_OK && k<wanted;k++){
                    lt48_defaulted_logical_pin missing;
                    size_t offset=operation.operation_id==2u?16u:k?30u:8u;
                    if(lt48_compiled_wave_get_defaulted_pin(compiled,k,&missing)!=LT48_OK ||
                       missing.logical_pin!=12u || missing.compact_offset!=offset ||
                       missing.compact_opcode!=0x11u || missing.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP)
                        status=LT48_SAFETY_REJECTED;
                }
            }
        } else {
        status = lt48_compact_compile(
            wave, resources->pins, resources->variables,
            resources->variable_count, 1u, 0, &resources->waves[operation.operation_id]);
        }
        lt48_compact_wave_close(wave);
        wave = NULL;
        if (status != LT48_OK) {
            if(getenv("LT48_RESOURCE_TRACE"))fprintf(stderr,"GBD/GBA D%x M%x P%x operation%u compile status%d\n",definition_resource,family_code,pinmap,operation.operation_id,status);
            goto fail;
        }
    }
    for (size_t required = 0u;
         required < sizeof(required_operations) / sizeof(required_operations[0]);
         ++required) {
        if (!present[required_operations[required]]) {
            status = LT48_NOT_FOUND;
            goto fail;
        }
    }
    *out_resources = resources;
#undef GAL_TRY
    return LT48_OK;

fail:
#undef GAL_TRY
    gal_resources_destroy(resources);
    return status;
}

lt48_status lt48_gal_resources_open(
    const char *definition_path, uint16_t family_code, uint16_t pinmap,
    lt48_gal_resources **out_resources
) {
    return lt48_gal_resources_open_definition(
        definition_path, UINT16_MAX, family_code, pinmap, out_resources);
}

const char *lt48_gal_resources_source_version(
    const lt48_gal_resources *resources
) {
    return resources ? resources->source_version : NULL;
}

lt48_status lt48_gal_resources_buffer(
    const lt48_gal_resources *resources, const char *name, uint8_t *out_id
) {
    lt48_defined_name_collection_info info;
    lt48_status status=LT48_OK;
    uint16_t identifier = 0u;
    int found = 0;
    if (!resources || !name || !out_id || strncmp(name, "_BUF_", 5u))
        return LT48_INVALID_ARGUMENT;
    if(resources->cached_names)info.count=resources->cached_name_count;
    else status = lt48_rez_get_defined_name_collection_info(
        resources->rez, resources->names_index, &info);
    if (status != LT48_OK) return status;
    for (size_t index = 0u; index < info.count; ++index) {
        lt48_defined_name item;
        if(resources->cached_names)item=resources->cached_names[index];
        else status = lt48_rez_get_defined_name(
            resources->rez, resources->names_index, index, &item);
        if (status != LT48_OK) return status;
        if (item.name_length != strlen(name) ||
            memcmp(item.name, name, item.name_length) != 0) continue;
        if (found || item.identifier >= resources->profile.buffer_count)
            return LT48_FORMAT_ERROR;
        identifier = item.identifier;
        found = 1;
    }
    if (!found) return LT48_NOT_FOUND;
    *out_id = (uint8_t)identifier;
    return LT48_OK;
}

lt48_status lt48_gal_resources_create_machine(
    const lt48_gal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine
) {
    return lt48_gal_resources_create_machine_with_variables(
        resources, operation, mode, initial_voltage, resources ?
        resources->variables : NULL, resources ? resources->variable_count : 0u,
        seeds, seed_count, out_machine);
}

lt48_status lt48_gal_resources_create_machine_with_variables(
    const lt48_gal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_compile_variable *variables,
    size_t variable_count, const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine
) {
    return lt48_gal_resources_image_machine_internal(resources,operation,mode,
        initial_voltage,variables,variable_count,seeds,seed_count,NULL,out_machine);
}

lt48_status lt48_gal_resources_image_machine_internal(
    const lt48_gal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_compile_variable *variables,
    size_t variable_count, const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, const lt48_runtime_fuse_image *image, lt48_runtime_machine **out_machine
) {
    lt48_runtime_nested_wave nested[LT48_GAL_MAX_WAVES];
    lt48_runtime_options options = {0};
    size_t nested_count = 0u;
    lt48_status status;
    if (!resources || !initial_voltage || !out_machine ||
        (variable_count && !variables) ||
        (seed_count && !seeds)) return LT48_INVALID_ARGUMENT;
    *out_machine = NULL;
    if (operation >= LT48_GAL_MAX_WAVES || !resources->waves[operation])
        return LT48_UNSUPPORTED;
    for (uint16_t id = 0u; id < LT48_GAL_MAX_WAVES; ++id) {
        if (!resources->waves[id]) continue;
        nested[nested_count++] = (lt48_runtime_nested_wave){
            id, resources->waves[id]};
    }
    if(operation==2u && mode==2u && resources->requires_program_image && !image)
        return LT48_SAFETY_REJECTED;
    /* Shared logic Kernel bit8 enables identity checking, as in TPofFuseGroup. */
    const lt48_runtime_configuration_value ejed_configuration[]={{-1,0,8,1}};
    if(resources->ejed){options.configuration=ejed_configuration;options.configuration_count=1;}
    options.fuse_image=image;
    if(resources->mach11_fpga){options.embedded_images=&resources->mach11_image;options.embedded_image_count=1;}
    options.mode = mode;
    options.host_avr_parallel = resources->ejed || resources->palce || gal59_definition(resources->definition_resource,
        resources->family_code,resources->pinmap);
    options.host_staged_shutdown = resources->palce ? 3 : 2 * (resources->ejed || gal59_definition(resources->definition_resource,
        resources->family_code,resources->pinmap));
    options.use_explicit_buffer_ids = 1;
    status = lt48_gal_resources_buffer(
        resources, "_BUF_DataIn", &options.parameter_buffer_id);
    if (status != LT48_OK) return status;
    status = lt48_gal_resources_buffer(
        resources, "_BUF_Mode", &options.mode_buffer);
    if (status != LT48_OK) return status;
    options.address_buffer = 0u;
    options.initial_voltage_centivolts[0] = initial_voltage[0];
    options.initial_voltage_centivolts[1] = initial_voltage[1];
    options.initial_voltage_centivolts[2] = initial_voltage[2];
    options.initial_voltage_centivolts[3] = initial_voltage[3];
    options.initial_buffers = seeds;
    options.initial_buffer_count = seed_count;
    options.host_gal_pes = resources->has_pes ? &resources->pes : NULL;
    options.nested_waves = nested;
    options.nested_wave_count = nested_count;
    options.root_timeout_seconds = 30u;
    status=lt48_runtime_machine_create(
        resources->waves[operation], resources->pins,
        variables, variable_count,
        &options, out_machine);
    /* These finite new profiles retain the instruction cursor at pin reads;
     * transport acknowledgement must not replay every earlier row pulse. */
    if(status==LT48_OK && resources->ejed9 &&
       ejed9_stage(resources->definition_resource,resources->family_code,resources->pinmap)>=2u &&
       (operation==4u || operation==5u))
        status=lt48_runtime_machine_stream_resume_internal(*out_machine);
    if(status!=LT48_OK){lt48_runtime_machine_close(*out_machine);*out_machine=NULL;}
    return status;
}

lt48_status lt48_gal_resources_frame_profile(
    const lt48_gal_resources *resources,
    lt48_runtime_frame_profile *out_profile
) {
    if (!resources || !out_profile) return LT48_INVALID_ARGUMENT;
    *out_profile = resources->profile;
    return LT48_OK;
}

lt48_status lt48_gal_resources_pes_binding(
    const lt48_gal_resources *resources, lt48_gal_pes_binding *out_binding
) {
    if (!resources || !out_binding) return LT48_INVALID_ARGUMENT;
    if (!resources->has_pes) return LT48_NOT_FOUND;
    *out_binding = resources->pes;
    return LT48_OK;
}

lt48_status lt48_gal_resources_compiled_wave(
    const lt48_gal_resources *resources, uint16_t operation_id,
    uint16_t *out_wave, const lt48_compiled_wave **out_compiled
) {
    if (!resources || !out_wave || !out_compiled)
        return LT48_INVALID_ARGUMENT;
    if (operation_id >= LT48_GAL_MAX_WAVES || !resources->waves[operation_id])
        return LT48_NOT_FOUND;
    *out_wave = resources->wave_numbers[operation_id];
    *out_compiled = resources->waves[operation_id];
    return LT48_OK;
}

const lt48_compile_variable *lt48_gal_resources_variables(
    const lt48_gal_resources *resources, size_t *out_count
) {
    if (out_count) *out_count = resources ? resources->variable_count : 0u;
    return resources ? resources->variables : NULL;
}

uint16_t lt48_gal_resources_family_code(
    const lt48_gal_resources *resources
) {
    return resources ? resources->family_code : 0u;
}

uint16_t lt48_gal_resources_pinmap(const lt48_gal_resources *resources) {
    return resources ? resources->pinmap : 0u;
}

uint8_t lt48_gal_resources_buffer_count(
    const lt48_gal_resources *resources
) {
    return resources ? resources->profile.buffer_count : 0u;
}

uint32_t lt48_gal_resources_reset_buffer_mask(
    const lt48_gal_resources *resources
) {
    return resources ? resources->profile.reset_mask : 0u;
}

const lt48_fuse_coordinate *lt48_gal_resources_coordinates(
    const lt48_gal_resources *resources, size_t *out_count
) {
    if (out_count) *out_count = resources ? resources->coordinate_count : 0u;
    return resources ? resources->coordinates : NULL;
}

int lt48_gal_resources_live_qualified(const lt48_gal_resources *resources) {
    /* Exact source hash is checked at open. D19/M18/P3 completed the native
     * six-operation hardware cycle, including Secure and erase/reprogram recovery. */
    return resources && !resources->ejed && !resources->palce && resources->definition_resource == 0x19u &&
        resources->family_code == 0x18u && resources->pinmap == 3u;
}

const lt48_fuse_code *lt48_gal_resources_fuse_code(const lt48_gal_resources *resources) {
    return resources ? &resources->fuse_code : NULL;
}

int lt48_gal_resources_source_audited(const lt48_gal_resources *resources) {
    if (!resources) return 0;
    if(resources->mach11)return mach11_stage(resources->definition_resource,resources->family_code,resources->pinmap)<=MACH11_STAGE;
    if(resources->palce)return palce54_definition(resources->definition_resource,resources->family_code,resources->pinmap)||palce8_definition(resources->definition_resource,resources->family_code,resources->pinmap);
    if(resources->ejed)return resources->ejed9 || cpld69_definition(resources->definition_resource,resources->family_code,resources->pinmap);
    if (gal59_definition(resources->definition_resource, resources->family_code, resources->pinmap)) return 1;
    switch (resources->definition_resource) {
    case 0x18: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2b:
    case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x2c: case 0x32:
        return 1;
    default: return 0;
    }
}

unsigned lt48_gal_resources_blank_fill_internal(const lt48_gal_resources*r){return r?r->blank_fill:255u;}
int lt48_gal_resources_requires_image_internal(const lt48_gal_resources*r){return r&&r->requires_program_image;}

size_t lt48_gal_resources_fuse_context_count(const lt48_gal_resources*r){return r?r->context_count:0;}
const lt48_fuse_code *lt48_gal_resources_fuse_context(const lt48_gal_resources*r,size_t i,size_t*offset,uint16_t*identifier){
    if(!r || i>=r->context_count)return NULL;
    if(offset)*offset=r->context_offsets[i];
    if(identifier)*identifier=r->context_ids[i];
    return &r->contexts[i];
}

unsigned lt48_gal_source_buffer_count_internal(const lt48_gal_resources*r){return r?(r->ejed9?76u:r->profile.buffer_count):0u;}
