#include <labtool48/labtool48.h>
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../NewDeviceLib/sources/pal46/profiles.h"
#include "../../NewDeviceLib/sources/pal29/profiles.h"
#include "../../NewDeviceLib/sources/ep35/profiles.h"
#include "../../NewDeviceLib/sources/pal52/profiles.h"
#include "../../NewDeviceLib/sources/peel46/profiles.h"
#include "../../NewDeviceLib/sources/pz20/profiles.h"
#include "../../NewDeviceLib/sources/compact20/profiles.h"
#include "../../NewDeviceLib/sources/pls26/profiles.h"
#include "../../NewDeviceLib/sources/epx6/profiles.h"
#include "../../NewDeviceLib/sources/peel46/corrections.h"
/* EP35_RESIDENT_NOOP_BEGIN */
#include "../../NewDeviceLib/sources/ep35/resident-noop.h"
/* EP35_RESIDENT_NOOP_END */
#include "../../NewDeviceLib/sources/pal29/maps.h"
#include "../../NewDeviceLib/sources/pal29/voltage.h"
#include "../../NewDeviceLib/sources/pal46/corrections.h"
#include "../../NewDeviceLib/sources/pal46/operations.h"

#include "../../NewDeviceLib/sources/epm44/profiles.h"

#define LT48_PAL_MAX_OPERATIONS 35u

struct lt48_pal_resources {
    lt48_rez *rez;
    int pal46;
    int peel46;
    int compact20;
    unsigned authored_corrections;
    unsigned char *pal46_idle; size_t pal46_idle_size;
    int epm;
    int xc95;
    int pof; /* 1: GC row core, 2: GCB transformed core. */
    lt48_pof_core pof_core;
    unsigned char pof_fpga[2][0xada];
    lt48_runtime_embedded_image pof_images[2];
    size_t pof_image_count;
    unsigned xc95_protection;
    lt48_pin_map *pins;
    lt48_compile_variable *variables;
    size_t variable_count;
    lt48_compiled_wave *waves[LT48_PAL_MAX_OPERATIONS];
    uint16_t wave_numbers[LT48_PAL_MAX_OPERATIONS];
    unsigned char present[LT48_PAL_MAX_OPERATIONS];
    lt48_fuse_code fuse_code;
    unsigned fuse_section_count;
    lt48_fuse_code secondary_fuse_code;
    lt48_fuse_coordinate *secondary_coordinates;
    lt48_fuse_coordinate *coordinates;
    size_t coordinate_count;
    size_t names_index;
    lt48_defined_name *cached_names;size_t cached_name_count;
    uint8_t blank_fuse;
    unsigned char wave_types[LT48_PAL_MAX_OPERATIONS];
    uint16_t definition_resource;
    uint16_t family_code;
    uint16_t pinmap;
    lt48_runtime_frame_profile profile;
    const char *source_version;
};

static void pal_resources_destroy(lt48_pal_resources *resources) {
    if (!resources) return;
    for (size_t i = 0u; i < LT48_PAL_MAX_OPERATIONS; ++i)
        lt48_compiled_wave_close(resources->waves[i]);
    free(resources->pal46_idle);
    free(resources->cached_names);
    free(resources->coordinates);
    free(resources->secondary_coordinates);
    free(resources->variables);
    lt48_pin_map_close(resources->pins);
    lt48_rez_close(resources->rez);
    free(resources);
}

void lt48_pal_resources_close(lt48_pal_resources *resources) {
    pal_resources_destroy(resources);
}
lt48_status lt48_pal_resources_pof_source_internal(lt48_pal_resources *resources,
    const lt48_pof_trs_source *source) {
    if(!resources||!source)return LT48_INVALID_ARGUMENT;
    if(!resources->pof && !(resources->pal46 && resources->definition_resource==0x40u && resources->family_code==0x28u && resources->pinmap==0x1bu))return LT48_UNSUPPORTED;
    const unsigned char *data[2];size_t size[2];
    for(unsigned i=0;i<2;i++) {
        lt48_status status=lt48_pof_source_fpga_internal(source,i,&data[i],&size[i]);
        if(status!=LT48_OK)return status;
        if(size[i]!=sizeof(resources->pof_fpga[i]))return LT48_FORMAT_ERROR;
    }
    for(unsigned i=0;i<2;i++) {
        memcpy(resources->pof_fpga[i],data[i],size[i]);
        resources->pof_images[i]=(lt48_runtime_embedded_image){i,resources->pof_fpga[i],size[i]};
    }
    resources->pof_image_count=2;return LT48_OK;
}
lt48_status lt48_pal_resources_pof_fpga_internal(const lt48_pal_resources *resources,
    unsigned *selector,const unsigned char **data,size_t *size) {
    if(!resources||!selector||!data||!size)return LT48_INVALID_ARGUMENT;
    *data=NULL;*size=0;*selector=0;
    if(!resources->pof||resources->pof_image_count!=2)return LT48_UNSUPPORTED;
    int found=0;
    for(unsigned op=0;op<LT48_PAL_MAX_OPERATIONS;op++) {
        if(!resources->waves[op])continue;
        uint16_t value;
        /* The historical accessor name is misleading: compact 93 selects
         * the base FPGA pointer at compile time; it is not a message. */
        lt48_status status=lt48_compiled_wave_completion_message(resources->waves[op],&value);
        if(status==LT48_NOT_FOUND)continue;
        if(status!=LT48_OK)return status;
        if(value>1||(found&&value!=*selector))return LT48_FORMAT_ERROR;
        *selector=value;found=1;
    }
    *data=resources->pof_images[*selector].data;
    *size=resources->pof_images[*selector].size;return LT48_OK;
}

lt48_status lt48_pal_resources_open_definition(
    const char *definition_path, uint16_t definition_resource,
    uint16_t family_code, uint16_t pinmap,
    lt48_pal_resources **out_resources
) {
    static const char expected_digest[] =
        "85ca2609c5d2691cd435b8edd8550523d7bac5c0e8107b599f189692baeeafa7";
    static const uint16_t required[] = {0u, 1u, 2u, 3u, 4u, 5u};
    unsigned char digest[LT48_SHA256_SIZE];
    char digest_hex[LT48_SHA256_SIZE * 2u + 1u];
    char name[16];
    lt48_pal_resources *resources = NULL;
    lt48_status status;
    size_t index, family_index;
    lt48_device_definition definition;
    lt48_family_info family;
    lt48_pin_code pin_code;
    lt48_logic_fuse_codes fuse_bindings;
    lt48_fuse_code fuse_code;
    int present[LT48_PAL_MAX_OPERATIONS] = {0};

    if (!definition_path || !out_resources) return LT48_INVALID_ARGUMENT;
    *out_resources = NULL;
    resources = calloc(1u, sizeof(*resources));
    if (!resources) return LT48_NO_MEMORY;
#define PAL_TRY(call) do { \
        status = (call); \
        if (status != LT48_OK) goto fail; \
    } while (0)
    PAL_TRY(lt48_rez_open(definition_path, &resources->rez));
    PAL_TRY(lt48_rez_sha256(resources->rez, digest));
    for (size_t byte = 0u; byte < sizeof(digest); ++byte)
        snprintf(digest_hex + byte * 2u, 3u, "%02x", digest[byte]);
    resources->epm = strcmp(digest_hex,
        "fe960eb458355e267848f82bd9d86dd05c54d9979c40a80036c3d3a57a7c4b32") == 0;
    resources->xc95 = !strcmp(digest_hex,"a7d97ec76f79911804983d3faee0bc0fc8ecfb1bcc2fb137305de06022b5556a");
    if (!strcmp(digest_hex,"fc6529b7b6c6443dc9a962052b5f18e7f2928ae1bdfc3248d373a9a9fec32c20")) resources->pof = 1;
    if (!strcmp(digest_hex,"9bd0f6c9cd3e5e6888a23f8750c6acc6da7fafb64501f477e064db3c8bb74b12")) resources->pof = 2;
    resources->compact20=!strcmp(digest_hex,"c9c29035544d72ea92a281b7a8def753b035889635f82fdb1f521fc0330f8c76") && (compact20_definition(definition_resource,family_code,pinmap)||pls26_definition(definition_resource,family_code,pinmap)||epx6_definition(definition_resource,family_code,pinmap));
    resources->peel46=!strcmp(digest_hex,"913000514454bd9ee191bc4167efa52aff3969ff46e84081d9ddfbe2635c6f7f") && (peel46_definition(definition_resource,family_code,pinmap)||pz20_definition(definition_resource,family_code,pinmap));
    if (strcmp(digest_hex, expected_digest) != 0 && !resources->epm && !resources->xc95 && !resources->pof && !resources->peel46 && !resources->compact20) {
        status = LT48_SAFETY_REJECTED;
        goto fail;
    }
    resources->source_version = "Dataman 13.10";
    resources->pal46 = !strcmp(digest_hex,expected_digest) && (pal46_definition(definition_resource,family_code,pinmap)||pal29_definition(definition_resource,family_code,pinmap)||ep35_definition(definition_resource,family_code,pinmap)||pal52_definition(definition_resource,family_code,pinmap));
    if (resources->pal46) {
        resources->authored_corrections = pal46_correction_flags(definition_resource,family_code,pinmap);
        if(family_code==2 && ((definition_resource==9&&pinmap==0)||(definition_resource==14&&pinmap==1)))resources->authored_corrections=LT48_PAL_MMI_ELECTRICAL;
    }
    resources->definition_resource=definition_resource;resources->family_code=family_code;resources->pinmap=pinmap;
    if (resources->pal46 || resources->peel46 || resources->compact20) {
        size_t n=strlen(definition_path);char *root=malloc(n+2u);
        if (!root) {status=LT48_NO_MEMORY;goto fail;}
        memcpy(root,definition_path,n+1u);char *slash=strrchr(root,'/');
        if(slash)*slash='\0';else {root[0]='.';root[1]='\0';}
        status=lt48_load_verified_idle_fpga_internal(root,&resources->pal46_idle,&resources->pal46_idle_size);
        if(status==LT48_OK && pal52_definition(definition_resource,family_code,pinmap) && definition_resource==0x40u) {
            lt48_pof_trs_source *source=NULL;
            status=lt48_pof_trs_source_open_internal(root,&source);
            if(status==LT48_OK)status=lt48_pal_resources_pof_source_internal(resources,source);
            lt48_pof_trs_source_close_internal(source);
        }
        free(root);if(status!=LT48_OK)goto fail;
    }


    snprintf(name, sizeof(name), "D%x", definition_resource);
    PAL_TRY(lt48_rez_find_resource(resources->rez, name, &index));
    PAL_TRY(lt48_rez_get_device_definition(resources->rez, index, &definition));
    if (definition.kind != (resources->pof ? LT48_DEVICE_DEFINITION_POF : resources->epm ? LT48_DEVICE_DEFINITION_POF_FUSE : LT48_DEVICE_DEFINITION_LOGIC) ||
        definition.family_code != family_code || definition.pinmap != pinmap) {
        status = LT48_DEVICE_MISMATCH;
        goto fail;
    }
    /* A selected logic definition owns the primary JEDEC geometry. Keep the
     * exact fuse order alongside the family resources so the future session
     * owner cannot accidentally use a shared family with another Dxx layout. */
    if (resources->pof) {
        PAL_TRY(lt48_rez_get_pof_core(resources->rez, index, &resources->pof_core));
    } else if (definition_resource != UINT16_MAX) {
        snprintf(name, sizeof(name), "D%x", definition_resource);
        PAL_TRY(lt48_rez_find_resource(resources->rez, name, &index));
        if (resources->epm) {
            lt48_pof_fuse_core core;
            PAL_TRY(lt48_rez_get_pof_fuse_core(resources->rez,index,&core));
            if (definition.trailing_data_size != 4u) { status = LT48_FORMAT_ERROR; goto fail; }
            memset(&fuse_bindings, 0, sizeof(fuse_bindings));
            fuse_bindings.count = 1;
            fuse_bindings.identifiers[0] = definition.trailing_data[2] |
                ((uint16_t)definition.trailing_data[3] << 8);
        } else {
            PAL_TRY(lt48_rez_get_logic_fuse_codes(resources->rez, index, &fuse_bindings));
        }
        if (fuse_bindings.count == 0u) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        snprintf(name, sizeof(name), "F%x",
                 fuse_bindings.identifiers[0]);
        PAL_TRY(lt48_rez_find_resource(resources->rez, name, &index));
        PAL_TRY(lt48_rez_get_fuse_code(resources->rez, index, &fuse_code));
        if (resources->authored_corrections & LT48_PAL_CORRECTED_GEOMETRY)
            if (!pal46_corrected_map(definition_resource,family_code,pinmap,&fuse_code)) {
                status=LT48_FORMAT_ERROR;goto fail;
            }
        if(resources->pal46)pal29_corrected_map(definition_resource,family_code,pinmap,&fuse_code);
        resources->fuse_code = fuse_code;
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
        PAL_TRY(lt48_fuse_layout_expand_coordinates(
            &fuse_code, resources->coordinates, fuse_code.total_fuse_count,
            &resources->coordinate_count));
        if (resources->epm) {
            for (size_t i = 0; i < resources->coordinate_count; ++i)
                if (resources->coordinates[i].fuse_type != 18u &&
                    !(definition_resource == 0 && resources->coordinates[i].fuse_type == 11u) &&
                    !(definition_resource >= 1 && definition_resource <= 4 && resources->coordinates[i].fuse_type == 12u) &&
                    !(epm44_stage(definition_resource,family_code,pinmap) && ((resources->coordinates[i].fuse_type>=11u && resources->coordinates[i].fuse_type<=16u) || resources->coordinates[i].fuse_type==1u || resources->coordinates[i].fuse_type==7u))) {
                    status = LT48_UNSUPPORTED; goto fail;
                }
        }
        if (resources->coordinate_count != fuse_code.total_fuse_count) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
    }

    if(resources->compact20 || resources->peel46 || (resources->pal46 && (ep35_definition(definition_resource,family_code,pinmap)||pal52_definition(definition_resource,family_code,pinmap)))) {
        if(fuse_bindings.count<1 || fuse_bindings.count>2){status=LT48_UNSUPPORTED;goto fail;}
        resources->fuse_section_count=fuse_bindings.count;
        if(fuse_bindings.count==2){
            snprintf(name,sizeof name,"F%x",fuse_bindings.identifiers[1]);
            PAL_TRY(lt48_rez_find_resource(resources->rez,name,&index));
            PAL_TRY(lt48_rez_get_fuse_code(resources->rez,index,&resources->secondary_fuse_code));
            if(resources->secondary_fuse_code.total_fuse_count!=resources->coordinate_count){status=LT48_FORMAT_ERROR;goto fail;}
            resources->secondary_coordinates=calloc(resources->coordinate_count,sizeof(*resources->secondary_coordinates));
            if(!resources->secondary_coordinates){status=LT48_NO_MEMORY;goto fail;}
            size_t n=0;PAL_TRY(lt48_fuse_layout_expand_coordinates(&resources->secondary_fuse_code,
                resources->secondary_coordinates,resources->coordinate_count,&n));
            if(n!=resources->coordinate_count){status=LT48_FORMAT_ERROR;goto fail;}
        }
    }

    PAL_TRY(lt48_rez_find_resource(resources->rez, "DefineName",
                                   &resources->names_index));

    /* Reuse the immutable namespace for new table-driven profiles. The old
     * accessor reparses the complete collection for every item of every row. */
    if((resources->compact20 && (pls26_definition(definition_resource,family_code,pinmap)||epx6_definition(definition_resource,family_code,pinmap))) || (resources->peel46&&pz20_definition(definition_resource,family_code,pinmap))) {
        lt48_defined_name_collection_info names;
        PAL_TRY(lt48_rez_get_defined_name_collection_info(resources->rez,resources->names_index,&names));
        if(!names.count || names.count>4096u){status=LT48_FORMAT_ERROR;goto fail;}
        resources->cached_names=calloc(names.count,sizeof(*resources->cached_names));
        if(!resources->cached_names){status=LT48_NO_MEMORY;goto fail;}
        PAL_TRY(lt48_rez_get_defined_names(resources->rez,resources->names_index,resources->cached_names,names.count,&resources->cached_name_count));
        if(resources->cached_name_count!=names.count){status=LT48_FORMAT_ERROR;goto fail;}
    }
    resources->blank_fuse = (uint8_t)(definition.value3 & 1u);
    /* MMI's programming convention is 0=intact, 1=blown for both senses.
     * GBB M1's FF blank is inconsistent with its positive LOAD and pulse mask. */
    if (resources->authored_corrections & LT48_PAL_POSITIVE_FUSE_SENSE)
        resources->blank_fuse = 0;
    snprintf(name, sizeof(name), "M%x", family_code);
    PAL_TRY(lt48_rez_find_resource(resources->rez, name, &family_index));
    PAL_TRY(lt48_rez_get_family_info(resources->rez, family_index, &family));
    if (family.operation_count > LT48_PAL_MAX_OPERATIONS ||
        family.variable_count > 256u) {
        status = LT48_UNSUPPORTED;
        goto fail;
    }
    snprintf(name, sizeof(name), "P%x", pinmap);
    PAL_TRY(lt48_rez_find_resource(resources->rez, name, &index));
    PAL_TRY(lt48_rez_get_pin_code(resources->rez, index, &pin_code));
    PAL_TRY(lt48_pin_map_parse(&pin_code, &resources->pins));
    /* Exact GCA Kernel declares 15 zero-initialised control records.
     * S procedures intentionally reference absent groups, including group 3.
     * Reuse the original Kernel/PinCode initialization contract. */
    if (resources->epm && epm44_stage(definition_resource,family_code,pinmap))
        PAL_TRY(lt48_pin_map_complete_groups_internal(resources->pins,15));
    if(resources->peel46 && pz20_definition(definition_resource,family_code,pinmap))
        PAL_TRY(lt48_pin_map_complete_groups_internal(resources->pins,16)); /* Exact GB Kernel control count. */
    PAL_TRY(lt48_rez_get_family_frame_profile(
        resources->rez, family_index, &resources->profile));
    resources->definition_resource = definition_resource;
    resources->family_code = family_code;
    resources->pinmap = pinmap;
    resources->variable_count = 0;
    resources->variables = calloc(
        family.variable_count + 5u,
        sizeof(*resources->variables));
    if (!resources->variables) {
        status = LT48_NO_MEMORY;
        goto fail;
    }
    for (size_t variable = 0u; variable < family.variable_count; ++variable) {
        lt48_family_variable item;
        PAL_TRY(lt48_rez_get_family_variable(
            resources->rez, family_index, variable, &item));
        size_t slot = 0;
        while (slot < resources->variable_count && resources->variables[slot].variable_id != item.values[1]) ++slot;
        if (slot == resources->variable_count) ++resources->variable_count;
        resources->variables[slot] = (lt48_compile_variable){
            item.values[0], item.values[1], item.values[2],
            item.values[3], item.values[4]};
    }
    if(resources->pal46 && pal29_security_voltage(definition_resource,family_code,pinmap))
        for(size_t i=0;i<resources->variable_count;i++)
            if(resources->variables[i].kind==0 && resources->variables[i].variable_id==16)
                resources->variables[i].value=(uint16_t)pal29_security_voltage(definition_resource,family_code,pinmap);
    if(resources->authored_corrections & LT48_PAL_MMI_ELECTRICAL){
        for(size_t i=0;i<resources->variable_count;i++){
            if(resources->variables[i].variable_id==22)resources->variables[i].channel=20; /* Tp us */
            if(resources->variables[i].variable_id==16)resources->variables[i].value=1850;
        }
        const lt48_compile_variable own[]={
            {0,40,1,550,0},{0,41,1,450,0},{0,42,1,600,1},
            {1,43,1,0,40},{1,44,1,0,100}};
        for(size_t i=0;i<5;i++)resources->variables[resources->variable_count++]=own[i];
    }
    for (size_t op_index = 0u; op_index < family.operation_count; ++op_index) {
        lt48_family_operation operation;
        lt48_wave_code wave_code;
        lt48_compact_wave *wave = NULL;
        PAL_TRY(lt48_rez_get_family_operation(
            resources->rez, family_index, op_index, &operation));
        if (operation.operation_id >= LT48_PAL_MAX_OPERATIONS ||
            present[operation.operation_id]) {
            status = LT48_FORMAT_ERROR;
            goto fail;
        }
        present[operation.operation_id] = 1;
        resources->present[operation.operation_id] = 1u;
        resources->wave_numbers[operation.operation_id] = operation.wave;
        snprintf(name, sizeof(name), "W%x", operation.wave);
        PAL_TRY(lt48_rez_find_resource(resources->rez, name, &index));
        PAL_TRY(lt48_rez_get_wave_code(resources->rez, index, &wave_code));
        resources->wave_types[operation.operation_id] = wave_code.wave_type;
        /* Type-2 resources are host pin-control matrices, consumed by the
         * original 004437f0 helper through compact 0x54 / expanded 0x3f. */
        if (wave_code.wave_type == 1u && ((ep22four_definition(definition_resource,family_code,pinmap)&&operation.operation_id==14u) || (resources->xc95 && operation.operation_id==10u) || (resources->peel46 && pz20_definition(definition_resource,family_code,pinmap) && (operation.operation_id==9u||operation.operation_id==10u)) ||
            (resources->compact20 && pls26_definition(definition_resource,family_code,pinmap) &&
             (operation.operation_id==15u||operation.operation_id==16u||operation.operation_id==20u||operation.operation_id==21u)))) {
            PAL_TRY(lt48_compiled_word_table_internal(wave_code.data,wave_code.data_size,&resources->waves[operation.operation_id]));
            continue;
        }
        if (wave_code.wave_type == 5u && resources->compact20 && epx6_definition(definition_resource,family_code,pinmap) && operation.operation_id==8u) {
            PAL_TRY(lt48_compiled_map_create(wave_code.data,wave_code.data_size,&resources->waves[operation.operation_id]));
            continue;
        }
        if (wave_code.wave_type == 5u && resources->peel46 && pz20_definition(definition_resource,family_code,pinmap) && operation.operation_id==13u) {
            PAL_TRY(lt48_compiled_map_create(wave_code.data,wave_code.data_size,&resources->waves[operation.operation_id]));continue;
        }
        if (wave_code.wave_type == 2u) {
            PAL_TRY(lt48_compiled_control_table_create(wave_code.data,
                wave_code.data_size, &resources->waves[operation.operation_id]));
            continue;
        }
        if (resources->pof && wave_code.wave_type == 3u) {
            PAL_TRY(lt48_pof_exception_bind_internal(resources->rez,index,&resources->waves[operation.operation_id]));
            continue;
        }
        if (resources->epm && (definition_resource <= 4 || epm44_stage(definition_resource,family_code,pinmap)==4) && wave_code.wave_type == 3u) {
            if(wave_code.data_size!=4 || wave_code.data[0]!=2 || wave_code.data[1] || wave_code.data[3]){status=LT48_FORMAT_ERROR;goto fail;}
            PAL_TRY(lt48_epm_legacy_compile_internal(wave_code.data[2],&resources->waves[operation.operation_id]));
            continue;
        }
        /* Other table formats remain available for planning only. Their
         * NULL compiled binding prevents a runtime from consuming them. */
        if (wave_code.wave_type != 0u) continue;
        unsigned char *amended=NULL;size_t amended_size=0;
        if(resources->peel46){
            status=peel46_security_wave(definition_resource,family_code,pinmap,operation.operation_id,
                wave_code.data,wave_code.data_size,&amended,&amended_size);
            if(status!=LT48_OK && status!=LT48_NOT_FOUND)goto fail;
        }
        if(resources->authored_corrections & LT48_PAL_MMI_ELECTRICAL){
            status=pal46_amend_wave(operation.operation_id,wave_code.data,wave_code.data_size,&amended,&amended_size);
            if(status!=LT48_OK && status!=LT48_NOT_FOUND)goto fail;
        }
        status=lt48_compact_decode(amended?amended:wave_code.data,
            amended?amended_size:wave_code.data_size,&wave);
        free(amended);if(status!=LT48_OK)goto fail;
        status = lt48_compact_compile(
            wave, resources->pins, resources->variables,
            resources->variable_count, 1u, 0,
            &resources->waves[operation.operation_id]);
        /* Original GB logical table is zero initialized. These four INITs
         * deliberately retain exactly its missing13 supply-route emission.
         * This is source compatibility, not a newly inferred socket mapping. */
        if(status==LT48_COMPILE_ERROR && resources->peel46 && operation.operation_id==2u &&
           ((definition_resource==8u&&family_code==7u&&pinmap==3u&&operation.wave==0x14u)||
            (definition_resource==10u&&family_code==7u&&pinmap==4u&&operation.wave==0x14u)||
            (definition_resource==9u&&family_code==8u&&pinmap==3u&&operation.wave==0x1au)||
            (definition_resource==11u&&family_code==8u&&pinmap==4u&&operation.wave==0x1au))) {
            status=lt48_compact_compile_with_pin_policy(wave,resources->pins,resources->variables,
                resources->variable_count,1u,LT48_PIN_FALLBACK_PEEL_SUPPLY,&resources->waves[operation.operation_id]);
            if(status==LT48_OK){lt48_defaulted_logical_pin missing;
                if(lt48_compiled_wave_defaulted_pin_count(resources->waves[operation.operation_id])!=1u ||
                   lt48_compiled_wave_get_defaulted_pin(resources->waves[operation.operation_id],0,&missing)!=LT48_OK ||
                   missing.logical_pin!=13u || missing.compact_offset!=48u || missing.compact_opcode!=3u ||
                   missing.classification!=LT48_DEFAULTED_PIN_SOURCE_ZERO_SUPPLY)status=LT48_COMPILE_ERROR;
            }
        }
        /* EP35_RESIDENT_NOOP_BEGIN */
        /* Exact D39 INIT offset9: original lookup defaults logical4 to zero;
         * compact11/state1 emits resident command1, an immediate RET. */
        if(status==LT48_COMPILE_ERROR && resources->pal46 &&
           ep35_resident_noop(definition_resource,family_code,pinmap,
                             operation.operation_id,operation.wave)) {
            status=lt48_compact_compile_with_pin_policy(wave,resources->pins,
                resources->variables,resources->variable_count,1u,
                LT48_PIN_FALLBACK_RESIDENT_NOOP,&resources->waves[operation.operation_id]);
            if(status==LT48_OK){
                lt48_defaulted_logical_pin missing;
                const lt48_compiled_wave *compiled=resources->waves[operation.operation_id];
                if(lt48_compiled_wave_defaulted_pin_count(compiled)!=1 ||
                   lt48_compiled_wave_get_defaulted_pin(compiled,0,&missing)!=LT48_OK ||
                   missing.logical_pin!=4 || missing.compact_offset!=9 ||
                   missing.compact_opcode!=0x11 || missing.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP)
                    status=LT48_COMPILE_ERROR;
            }
        }
        /* EP35_RESIDENT_NOOP_END */
        /* GC D3/D4 W14 uses missing logical zero solely to ramp VCC.
         * Original 442df0 emits Q2(0), 10/0/start, 1C/1, Q2(2), ramp.
         * Resident 0/2 return immediately; 1C/1 matches no relay. Preserve
         * the DAC ramp, and admit only this exact hash-bound instruction. */
        if (status == LT48_COMPILE_ERROR && resources->pof == 1 &&
            (definition_resource == 3 || definition_resource == 4) &&
            operation.operation_id == 0 && operation.wave == 0x14) {
            status = lt48_compact_compile(wave,resources->pins,resources->variables,
                resources->variable_count,1u,1,&resources->waves[operation.operation_id]);
            if (status == LT48_OK) {
                lt48_defaulted_logical_pin missing;
                const lt48_compiled_wave *compiled=resources->waves[operation.operation_id];
                if (lt48_compiled_wave_defaulted_pin_count(compiled)!=1 ||
                    lt48_compiled_wave_get_defaulted_pin(compiled,0,&missing)!=LT48_OK ||
                    missing.logical_pin!=0 || missing.compact_opcode!=0x28 ||
                    missing.compact_offset!=13) status=LT48_COMPILE_ERROR;
            }
        }
        lt48_compact_wave_close(wave);
        if (status != LT48_OK) goto fail;
    }
    if(resources->authored_corrections & LT48_PAL_MMI_ELECTRICAL){
        for(unsigned i=0;i<2;i++){
            const unsigned char own[]={5,0,2,0,(unsigned char)(40+i),0x31,44};
            lt48_compact_wave *wave=NULL;
            if(resources->present[30+i]){status=LT48_FORMAT_ERROR;goto fail;}
            PAL_TRY(lt48_compact_decode(own,sizeof own,&wave));
            status=lt48_compact_compile(wave,resources->pins,resources->variables,
                resources->variable_count,1u,0,&resources->waves[30+i]);
            lt48_compact_wave_close(wave);if(status!=LT48_OK)goto fail;
            resources->present[30+i]=1;
        }
    }
    for (size_t required_index = 0u;
         required_index < sizeof(required) / sizeof(required[0]);
         ++required_index) {
        uint16_t operation = required[required_index];
        if (resources->pof) {
            static const char *const names[] = {"_FUN_POWERON", "_FUN_POWEROFF", "_FUN_DEVINIT", "_FUN_DEVRESET", "_FUN_LOAD", "_FUN_PROGRAM"};
            PAL_TRY(lt48_pal_resources_operation_internal(resources,names[required_index],&operation));
        }
        if (!resources->present[operation]) {
            status = LT48_NOT_FOUND;
            goto fail;
        }
    }
    *out_resources = resources;
#undef PAL_TRY
    return LT48_OK;
fail:
#undef PAL_TRY
    pal_resources_destroy(resources);
    return status;
}

const char *lt48_pal_resources_source_version(
    const lt48_pal_resources *resources
) {
    return resources ? resources->source_version : NULL;
}

lt48_status lt48_pal_resources_frame_profile(
    const lt48_pal_resources *resources,
    lt48_runtime_frame_profile *out_profile
) {
    if (!resources || !out_profile) return LT48_INVALID_ARGUMENT;
    *out_profile = resources->profile;
    return LT48_OK;
}

int lt48_pal_resources_has_operation(
    const lt48_pal_resources *resources, uint16_t operation_id
) {
    return resources && operation_id < LT48_PAL_MAX_OPERATIONS &&
           resources->present[operation_id] != 0u;
}

lt48_status lt48_pal_resources_compiled_wave(
    const lt48_pal_resources *resources, uint16_t operation_id,
    uint16_t *out_wave, const lt48_compiled_wave **out_compiled
) {
    if (!resources || !out_wave || !out_compiled) return LT48_INVALID_ARGUMENT;
    if (operation_id >= LT48_PAL_MAX_OPERATIONS ||
        !resources->waves[operation_id]) return LT48_NOT_FOUND;
    *out_wave = resources->wave_numbers[operation_id];
    *out_compiled = resources->waves[operation_id];
    return LT48_OK;
}

lt48_status lt48_pal_resources_buffer(
    const lt48_pal_resources *resources, const char *name, uint8_t *out_id
) {
    lt48_defined_name_collection_info info;
    lt48_status status;
    uint16_t identifier = 0u;
    int found = 0;
    if (!resources || !name || !out_id || strncmp(name, "_BUF_", 5u))
        return LT48_INVALID_ARGUMENT;
    if(resources->cached_names)info.count=(uint32_t)resources->cached_name_count;
    else {
        status = lt48_rez_get_defined_name_collection_info(resources->rez, resources->names_index, &info);
        if(status!=LT48_OK)return status;
    }
    for (size_t index = 0u; index < info.count; ++index) {
        lt48_defined_name item;
        if(resources->cached_names)item=resources->cached_names[index];
        else {
            status=lt48_rez_get_defined_name(resources->rez,resources->names_index,index,&item);
            if(status!=LT48_OK)return status;
        }
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

lt48_status lt48_pal_resources_create_machine(
    const lt48_pal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine
) {
    return lt48_pal_resources_create_machine_with_variables(
        resources, operation, mode, initial_voltage, resources ?
        resources->variables : NULL, resources ? resources->variable_count : 0u,
        seeds, seed_count, out_machine);
}

lt48_status lt48_pal_resources_create_machine_with_variables(
    const lt48_pal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_compile_variable *variables,
    size_t variable_count, const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine
) {
    return lt48_pal_resources_machine_state_internal(resources,operation,mode,initial_voltage,variables,variable_count,seeds,seed_count,NULL,NULL,0,out_machine);
}

int lt48_pal_resources_legacy_epm_internal(const lt48_pal_resources *r) {
    return r && r->epm && (r->definition_resource<=4 || epm44_stage(r->definition_resource,r->family_code,r->pinmap)==4);
}
lt48_status lt48_pal_resources_machine_state_internal(
    const lt48_pal_resources *resources,uint16_t operation,uint32_t mode,
    const uint32_t initial_voltage[4],const lt48_compile_variable *variables,
    size_t variable_count,const lt48_runtime_buffer_seed *seeds,size_t seed_count,
    const unsigned char *state,const lt48_runtime_configuration_value *configuration,size_t configuration_count,lt48_runtime_machine **out_machine) {
    return lt48_pal_resources_machine_image_internal(resources,operation,mode,initial_voltage,variables,variable_count,seeds,seed_count,state,configuration,configuration_count,NULL,out_machine);
}
int lt48_pal_resources_project_image_internal(const lt48_pal_resources *r){
 return !r?0:r->peel46&&(r->family_code==13u||r->family_code==14u)?2:r->pal46&&r->definition_resource==0x13u&&r->family_code==4u&&r->pinmap==0u;
}
lt48_status lt48_pal_resources_machine_image_internal(
    const lt48_pal_resources *resources,uint16_t operation,uint32_t mode,
    const uint32_t initial_voltage[4],const lt48_compile_variable *variables,
    size_t variable_count,const lt48_runtime_buffer_seed *seeds,size_t seed_count,
    const unsigned char *state,const lt48_runtime_configuration_value *configuration,size_t configuration_count,const lt48_runtime_fuse_image *image,lt48_runtime_machine **out_machine) {
    lt48_runtime_nested_wave nested[LT48_PAL_MAX_OPERATIONS];
    lt48_runtime_options options = {0};
    size_t nested_count = 0u;
    lt48_status status;
    if (!resources || !initial_voltage || !out_machine ||
        (variable_count && !variables) ||
        (seed_count && !seeds)) return LT48_INVALID_ARGUMENT;
    *out_machine = NULL;
    options.fuse_image=image;
    options.fuse_section_count=resources->fuse_section_count;
    if (operation >= LT48_PAL_MAX_OPERATIONS || !resources->waves[operation] || (resources->wave_types[operation] != 0u && !lt48_pal_resources_legacy_epm_internal(resources) && !(resources->pof && resources->wave_types[operation] == 3u)))
        return LT48_UNSUPPORTED;
    for (uint16_t id = 0u; id < LT48_PAL_MAX_OPERATIONS; ++id)
        if (resources->present[id] && !resources->waves[id]) return LT48_UNSUPPORTED;
    for (uint16_t id = 0u; id < LT48_PAL_MAX_OPERATIONS; ++id) {
        if (!resources->waves[id]) continue;
        nested[nested_count++] = (lt48_runtime_nested_wave){
            id, resources->waves[id]};
    }
    /* TPofFuseGroup's default Kernel bit 8 enables the device-ID check.
     * Keep it enabled; unsupported configuration reads fail closed. */
    const lt48_runtime_configuration_value epm_configuration[] = {{-1,0,8,1}};
    if (resources->epm) {
        options.configuration = epm_configuration;
        options.configuration_count = 1;
    }
    if(resources->xc95 || resources->peel46 || resources->compact20) {
        options.configuration=configuration;
        options.configuration_count=configuration_count;
    }
    /* Explicit Linux policy: retain source XCR ID checking rather than
     * silently choosing the unchecked branch when no project is supplied. */
    const lt48_runtime_configuration_value pz20_id_check[]={{-1,0,8,1}};
    if(resources->peel46 && (resources->definition_resource==0x29u||resources->definition_resource==0x2au) && !configuration_count) {
        options.configuration=pz20_id_check;options.configuration_count=1;
    }
    unsigned char empty_state[LT48_EPM_LEGACY_STATE_SIZE]={0};
    if(lt48_pal_resources_legacy_epm_internal(resources)) {
        options.host_epm_legacy=1;
        options.device_image=state?state:empty_state;
        options.device_image_size=sizeof(empty_state);
        options.device_element_bytes=1;
    }
    options.host_staged_shutdown = (resources->compact20 || resources->peel46 || (resources->pal46 && pal52_definition(resources->definition_resource,resources->family_code,resources->pinmap)))?3:resources->pal46 || (resources->epm && epm44_stage(resources->definition_resource,resources->family_code,resources->pinmap));
    options.mode = mode;
    options.use_explicit_buffer_ids = 1;
    status = lt48_pal_resources_buffer(
        resources, "_BUF_DataIn", &options.parameter_buffer_id);
    if (status != LT48_OK) return status;
    status = lt48_pal_resources_buffer(
        resources, "_BUF_Mode", &options.mode_buffer);
    if (status != LT48_OK) return status;
    options.address_buffer = 0u;
    options.initial_voltage_centivolts[0] = initial_voltage[0];
    options.initial_voltage_centivolts[1] = initial_voltage[1];
    options.initial_voltage_centivolts[2] = initial_voltage[2];
    options.initial_voltage_centivolts[3] = initial_voltage[3];
    options.host_pal_mmi_atomic=operation==5 &&
        (resources->authored_corrections & LT48_PAL_MMI_ELECTRICAL);
    options.initial_buffers = seeds;
    options.initial_buffer_count = seed_count;
    options.nested_waves = nested;
    options.nested_wave_count = nested_count;
    options.root_timeout_seconds = 30u;
    options.embedded_images=resources->pof_images;
    options.embedded_image_count=resources->pof_image_count;
    return lt48_runtime_machine_create(
        resources->waves[operation], resources->pins,
        variables, variable_count,
        &options, out_machine);
}

const lt48_compile_variable *lt48_pal_resources_variables(
    const lt48_pal_resources *resources, size_t *out_count
) {
    if (out_count) *out_count = resources ? resources->variable_count : 0u;
    return resources ? resources->variables : NULL;
}

uint16_t lt48_pal_resources_family_code(
    const lt48_pal_resources *resources
) {
    return resources ? resources->family_code : 0u;
}

uint16_t lt48_pal_resources_pinmap(const lt48_pal_resources *resources) {
    return resources ? resources->pinmap : 0u;
}

uint8_t lt48_pal_resources_buffer_count(
    const lt48_pal_resources *resources
) {
    return resources ? resources->profile.buffer_count : 0u;
}

uint32_t lt48_pal_resources_reset_buffer_mask(
    const lt48_pal_resources *resources
) {
    return resources ? resources->profile.reset_mask : 0u;
}

const lt48_fuse_coordinate *lt48_pal_resources_coordinates(
    const lt48_pal_resources *resources, size_t *out_count
) {
    if (out_count) *out_count = resources ? resources->coordinate_count : 0u;
    return resources ? resources->coordinates : NULL;
}


uint8_t lt48_pal_resources_blank_fuse(const lt48_pal_resources *resources) {
    return resources ? resources->blank_fuse : 0u;
}

const lt48_fuse_code *lt48_pal_resources_fuse_code(const lt48_pal_resources *resources) {
    return resources ? &resources->fuse_code : NULL;
}

unsigned lt48_pal_resources_authored_corrections(const lt48_pal_resources *resources) {
    return resources ? resources->authored_corrections : 0u;
}

lt48_status lt48_pal_resources_corrected_fuse_code(lt48_rez *rez,
    uint16_t definition, uint16_t family, uint16_t pinmap, lt48_fuse_code *out) {
    static const unsigned char expected[32]={
        0x85,0xca,0x26,0x09,0xc5,0xd2,0x69,0x1c,0xd4,0x35,0xb8,0xed,0xd8,0x55,0x05,0x23,
        0xd7,0xba,0xc5,0xc0,0xe8,0x10,0x7b,0x59,0x9f,0x18,0x96,0x92,0xba,0xee,0xaf,0xa7};
    unsigned char digest[32];char name[16];size_t index;lt48_device_definition d;
    if(!rez||!out)return LT48_INVALID_ARGUMENT;
    lt48_fuse_code own;
    int has_pal29=pal29_definition(definition,family,pinmap)&&pal29_corrected_map(definition,family,pinmap,&own);
    if(!(pal46_correction_flags(definition,family,pinmap)&1u)&&!has_pal29)return LT48_NOT_FOUND;
    lt48_status status=lt48_rez_sha256(rez,digest);
    if(status!=LT48_OK)return status;
    if(memcmp(digest,expected,sizeof expected))return LT48_NOT_FOUND;
    snprintf(name,sizeof name,"D%x",definition);
    status=lt48_rez_find_resource(rez,name,&index);
    if(status==LT48_OK)status=lt48_rez_get_device_definition(rez,index,&d);
    if(status!=LT48_OK)return status;
    if(d.kind!=LT48_DEVICE_DEFINITION_LOGIC||d.family_code!=family||d.pinmap!=pinmap)
        return LT48_DEVICE_MISMATCH;
    if(has_pal29){*out=own;return LT48_OK;}
    return pal46_corrected_map(definition,family,pinmap,out)?LT48_OK:LT48_NOT_FOUND;
}

int lt48_pal_resources_source_audited(const lt48_pal_resources *resources) {
    if (!resources || resources->pof) return 0;
    if (resources->xc95) return resources->definition_resource <= 4; /* GBE digest, complete XC95 owner. */
    if (resources->epm) {unsigned stage=epm44_stage(resources->definition_resource,resources->family_code,resources->pinmap);if(stage)return stage<=EPM44_STAGE;}
    if (resources->epm) return resources->definition_resource <= 4 || (resources->definition_resource >= 0x20 && resources->definition_resource <= 0x23) || resources->definition_resource == 0x25 || resources->definition_resource == 0x26;
    if (resources->pal46 || resources->peel46 || resources->compact20) return 1;
    switch (resources->definition_resource) {
    case 0x14: case 0x15: case 0x1f: case 0x23: case 0x25: case 0x2b:
        return 1;
    default: return 0;
    }
}

int lt48_pal_resources_compact20_internal(const lt48_pal_resources*r){return r&&r->compact20;}

int lt48_pal_resources_is_epm(const lt48_pal_resources *r) { return r && r->epm; }

int lt48_pal_resources_xc95_internal(const lt48_pal_resources *r) {return r&&r->xc95;}

lt48_status lt48_pal_resources_set_xc95_protection(lt48_pal_resources *r,unsigned mask) {
    if(!r||!r->xc95)return LT48_UNSUPPORTED;
    if(!mask||mask>(r->family_code==4?1u:3u))return LT48_INVALID_ARGUMENT;
    r->xc95_protection=mask;return LT48_OK;
}
unsigned lt48_pal_resources_xc95_protection_internal(const lt48_pal_resources *r) {
    return r&&r->xc95_protection?r->xc95_protection:1u;
}

/* Bind operation names per source: GC and GCB number even power operations
 * differently. Never infer an operation identifier from the sibling class. */
lt48_status lt48_pal_resources_operation_internal(const lt48_pal_resources *r,
    const char *name, uint16_t *out) {
    if (!r || !name || !out || strncmp(name,"_FUN_",5)) return LT48_INVALID_ARGUMENT;
    lt48_defined_name_collection_info info;
    lt48_status status = lt48_rez_get_defined_name_collection_info(r->rez,r->names_index,&info);
    if (status != LT48_OK) return status;
    int found = 0; uint16_t id = 0;
    for (size_t i=0;i<info.count;i++) {
        lt48_defined_name item;
        status = lt48_rez_get_defined_name(r->rez,r->names_index,i,&item);
        if (status != LT48_OK) return status;
        if (item.name_length != strlen(name) || memcmp(item.name,name,item.name_length)) continue;
        if (found || item.identifier >= LT48_PAL_MAX_OPERATIONS) return LT48_FORMAT_ERROR;
        id=item.identifier; found=1;
    }
    if (!found || !r->present[id]) return LT48_NOT_FOUND;
    *out=id; return LT48_OK;
}
lt48_status lt48_pal_resources_operation(const lt48_pal_resources *r,const char *role,uint16_t *id) {
    return lt48_pal_resources_operation_internal(r,role,id);
}
int lt48_pal_resources_pof_internal(const lt48_pal_resources *r) { return r ? r->pof : 0; }
lt48_status lt48_pal_resources_parameter_internal(const lt48_pal_resources *r,
    const char *name,uint16_t *id,uint32_t *value) {
    if(!r||!name||!id||!value||strncmp(name,"_VAR_",5))return LT48_INVALID_ARGUMENT;
    lt48_defined_name_collection_info info;
    lt48_status status=lt48_rez_get_defined_name_collection_info(r->rez,r->names_index,&info);
    if(status!=LT48_OK)return status;
    int found=0;uint16_t selected=0;
    for(size_t i=0;i<info.count;i++) {
        lt48_defined_name item;
        status=lt48_rez_get_defined_name(r->rez,r->names_index,i,&item);
        if(status!=LT48_OK)return status;
        if(item.name_length!=strlen(name)||memcmp(item.name,name,item.name_length))continue;
        if(found)return LT48_FORMAT_ERROR;
        selected=item.identifier;found=1;
    }
    if(!found)return LT48_NOT_FOUND;
    for(size_t i=0;i<r->variable_count;i++)if(r->variables[i].variable_id==selected) {
        *id=selected;*value=r->variables[i].value;return LT48_OK;
    }
    return LT48_NOT_FOUND;
}
const lt48_pof_core *lt48_pal_resources_pof_core_internal(const lt48_pal_resources *r) {
    return r && r->pof ? &r->pof_core : NULL;
}

/* Functional entry 00444d50 uses the named VCC and POWEROFF bindings,
 * plus groups 1/2 from the selected pin map; not programming VPP/DEVRESET. */
lt48_status lt48_pal_resources_vector_profile_internal(const lt48_pal_resources *r,
    lt48_vector_device_profile *out) {
    if(!r||!out)return LT48_INVALID_ARGUMENT;
    if(!r->pof)return LT48_UNSUPPORTED;
    lt48_vector_device_profile pending={0};lt48_pin_group ground,supply;
    lt48_status st=lt48_pal_resources_operation_internal(r,"_FUN_POWEROFF",&pending.power_off);
    if(st==LT48_OK)st=lt48_pal_resources_parameter_internal(r,"_VAR_VCC",&pending.supply_variable,&pending.centivolts);
    if(st==LT48_OK)st=lt48_pin_map_find_group(r->pins,1,&ground);
    if(st==LT48_OK)st=lt48_pin_map_find_group(r->pins,2,&supply);
    if(st!=LT48_OK)return st;
    if(!pending.centivolts||pending.centivolts>600||!ground.pin_count||!supply.pin_count||
       ground.pin_count>48||supply.pin_count>48)return LT48_FORMAT_ERROR;
    unsigned char occupied[48]={0};
    for(unsigned group=0;group<2;group++) {
        const lt48_pin_group *g=group?&supply:&ground;
        for(size_t i=0;i<g->pin_count;i++) {
            unsigned code=g->pins[i];
            if(code>188||(code&3)||occupied[code/4])return LT48_FORMAT_ERROR;
            occupied[code/4]=1;
        }
    }
    pending.pin_count=lt48_pin_map_package_pin_count(r->pins);
    pending.ground_count=ground.pin_count;pending.supply_count=supply.pin_count;
    memcpy(pending.ground,ground.pins,ground.pin_count);memcpy(pending.supply,supply.pins,supply.pin_count);
    *out=pending;return LT48_OK;
}

lt48_status lt48_pal46_idle_internal(const lt48_pal_resources*r,const unsigned char**data,size_t*size){
 if(!r||!data||!size)return LT48_INVALID_ARGUMENT;
 *data=r->pal46_idle;*size=r->pal46_idle_size;
 return *data&&*size==2778u?LT48_OK:LT48_NOT_FOUND;
}

lt48_status lt48_pal_resources_section_internal(const lt48_pal_resources*r,unsigned section,const lt48_fuse_code **code,const lt48_fuse_coordinate **coordinates,size_t *count){
 if(!r||!code||!coordinates||!count)return LT48_INVALID_ARGUMENT;
 if(section>1 || (section && r->fuse_section_count!=2))return LT48_SAFETY_REJECTED;
 *code=section?&r->secondary_fuse_code:&r->fuse_code;
 *coordinates=section?r->secondary_coordinates:r->coordinates;*count=r->coordinate_count;return LT48_OK;
}

int lt48_pal_resources_ep35_internal(const lt48_pal_resources*r){return r&&r->fuse_section_count!=0;}

lt48_status lt48_pal_resources_security_marker(const lt48_pal_resources*r,size_t*out) {
 if(!r||!out)return LT48_INVALID_ARGUMENT;
 if(!r->peel46 || (r->definition_resource!=0x12u&&r->definition_resource!=0x13u))return LT48_NOT_FOUND;
 size_t found=SIZE_MAX;
 for(size_t i=0;i<r->coordinate_count;i++){
  const lt48_fuse_coordinate*c=r->coordinates+i;
  if(c->fuse_type==10u && c->row==39u && c->column==191u && c->bit==10u){if(found!=SIZE_MAX)return LT48_FORMAT_ERROR;found=i;}
 }
 if(found==SIZE_MAX)return LT48_NOT_FOUND;
 *out=found;return LT48_OK;
}
