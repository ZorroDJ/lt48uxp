#include <labtool48/labtool48.h>
#include "internal.h"

#include <stddef.h>

static const lt48_m37_cleanup_phase powered_cleanup[] = {
    LT48_M37_CLEANUP_DEVICE_RESET,
    LT48_M37_CLEANUP_SETTLE_DELAY,
    LT48_M37_CLEANUP_SETTLE_STATUS,
    LT48_M37_CLEANUP_POWER_OFF,
    LT48_M37_CLEANUP_POWER_OFF_FLUSH,
    LT48_M37_CLEANUP_ZERO_ALL_RAILS,
    LT48_M37_CLEANUP_RESTORE_IDLE_HEADER,
    LT48_M37_CLEANUP_RESTORE_IDLE_DATA,
    LT48_M37_CLEANUP_FINALIZE_CONTROLLER,
    LT48_M37_CLEANUP_FINALIZE_FLUSH,
    LT48_M37_CLEANUP_SET_RESULT,
    LT48_M37_CLEANUP_RELEASE_INTERFACE
};

static const lt48_m37_cleanup_phase initialized_cleanup[] = {
    LT48_M37_CLEANUP_ZERO_ALL_RAILS,
    LT48_M37_CLEANUP_RESTORE_IDLE_HEADER,
    LT48_M37_CLEANUP_RESTORE_IDLE_DATA,
    LT48_M37_CLEANUP_FINALIZE_CONTROLLER,
    LT48_M37_CLEANUP_FINALIZE_FLUSH,
    LT48_M37_CLEANUP_SET_RESULT,
    LT48_M37_CLEANUP_RELEASE_INTERFACE
};

static const lt48_m37_cleanup_phase unopened_cleanup[] = {
    LT48_M37_CLEANUP_RELEASE_INTERFACE
};

lt48_status lt48_m37_cleanup_recipe(
    int controller_initialized,
    int power_started,
    lt48_m37_cleanup_phase *phases,
    size_t phase_capacity,
    size_t *out_phase_count
) {
    const lt48_m37_cleanup_phase *source;
    size_t count;
    size_t index;
    if (!out_phase_count || (phase_capacity && !phases) ||
        (power_started && !controller_initialized)) {
        return LT48_INVALID_ARGUMENT;
    }
    if (power_started) {
        source = powered_cleanup;
        count = sizeof(powered_cleanup) / sizeof(powered_cleanup[0]);
    } else if (controller_initialized) {
        source = initialized_cleanup;
        count = sizeof(initialized_cleanup) / sizeof(initialized_cleanup[0]);
    } else {
        source = unopened_cleanup;
        count = sizeof(unopened_cleanup) / sizeof(unopened_cleanup[0]);
    }
    *out_phase_count = count;
    if (phase_capacity < count) return LT48_BUFFER_TOO_SMALL;
    for (index = 0; index < count; ++index) phases[index] = source[index];
    return LT48_OK;
}

static int cleanup_matches(const lt48_m37_plan_evidence *evidence) {
    size_t index;
    if (!evidence->cleanup_phases ||
        evidence->cleanup_phase_count !=
            sizeof(powered_cleanup) / sizeof(powered_cleanup[0])) {
        return 0;
    }
    for (index = 0; index < evidence->cleanup_phase_count; ++index) {
        if (evidence->cleanup_phases[index] != powered_cleanup[index]) return 0;
    }
    return 1;
}

static int family_has_approved_16k_read_behavior(uint16_t family_code) {
    return family_code == 0x37u || family_code == 0x39u ||
           family_code == 0x3bu || family_code == 0x50u ||
           family_code == 0x51u || family_code == 0x52u;
}

lt48_status lt48_m37_validate_plan(
    const lt48_m37_plan_evidence *evidence,
    uint64_t *out_issue_mask
) {
    uint64_t issues = 0u;
    int mutating;
    if (!evidence || !out_issue_mask) return LT48_INVALID_ARGUMENT;
    if (evidence->device_class != 2001u ||
        !family_has_approved_16k_read_behavior(evidence->family_code) ||
        evidence->pin_count != 28u ||
        evidence->capacity_bytes != 0x4000u) {
        issues |= LT48_M37_SAFETY_ENGINE;
    }
    if (evidence->operation < LT48_M37_READ ||
        evidence->operation > LT48_M37_COMPARE) {
        issues |= LT48_M37_SAFETY_OPERATION;
    }
    if (!evidence->length || evidence->start >= evidence->capacity_bytes ||
        evidence->length > evidence->capacity_bytes - evidence->start) {
        issues |= LT48_M37_SAFETY_ADDRESS_RANGE;
    }
    if (evidence->chunk_size != 163u) issues |= LT48_M37_SAFETY_CHUNK;
    if (evidence->nominal_vcc_centivolts < 100u ||
        evidence->nominal_vcc_centivolts > 700u ||
        evidence->vcc_selector !=
            (evidence->nominal_vcc_centivolts <= 400u ? 0x07u : 0x0fu)) {
        issues |= LT48_M37_SAFETY_VCC;
    }
    if (!evidence->strict_pin_mapping) issues |= LT48_M37_SAFETY_PIN_MAPPING;
    if (!evidence->all_applets_hash_bound ||
        !evidence->applet_ranges_nonoverlapping) {
        issues |= LT48_M37_SAFETY_APPLETS;
    }
    if (!evidence->active_fpga_hash_bound) issues |= LT48_M37_SAFETY_FPGA;
    if (!cleanup_matches(evidence)) issues |= LT48_M37_SAFETY_CLEANUP;
    mutating = evidence->operation == LT48_M37_PROGRAM;
    if (evidence->operation == LT48_M37_PROGRAM ||
        evidence->operation == LT48_M37_VERIFY ||
        evidence->operation == LT48_M37_COMPARE) {
        if (!evidence->has_image_digest ||
            evidence->image_size != evidence->length) {
            issues |= LT48_M37_SAFETY_IMAGE;
        }
    }
    if (mutating && evidence->precheck != LT48_M37_PRECHECK_BLANK &&
        evidence->precheck != LT48_M37_PRECHECK_ONE_TO_ZERO) {
        issues |= LT48_M37_SAFETY_PRECHECK;
    }
    if (mutating && !evidence->mandatory_post_verify) {
        issues |= LT48_M37_SAFETY_VERIFICATION;
    }
    /* Selected catalogue resources define the operation. EPROM IDs are an
     * identification aid, not a prerequisite for programming that selection. */
    *out_issue_mask = issues;
    return issues ? LT48_SAFETY_REJECTED : LT48_OK;
}

static lt48_status validate_audited_plan(
    const lt48_m37_resource_audit *audit,
    const lt48_m37_plan_evidence *operation_evidence,
    uint64_t *out_issue_mask
) {
    lt48_m37_plan_evidence verified;
    uint64_t audit_issues = 0u;
    uint64_t plan_issues = 0u;
    lt48_status status;
    if (!audit || !operation_evidence || !out_issue_mask) {
        return LT48_INVALID_ARGUMENT;
    }
    verified = *operation_evidence;
    verified.device_class = audit->device_class;
    verified.family_code = audit->family_code;
    verified.pin_count = audit->pin_count;
    verified.capacity_bytes = audit->capacity_bytes;
    verified.nominal_vcc_centivolts = audit->nominal_vcc_centivolts;
    verified.vcc_selector = audit->expected_vcc_selector;
    verified.strict_pin_mapping = audit->strict_pin_mapping &&
                                  audit->required_compact_waves_strict;
    verified.all_applets_hash_bound = audit->applet_metadata_complete;
    verified.applet_ranges_nonoverlapping =
        audit->applet_ranges_nonoverlapping;
    verified.active_fpga_hash_bound = audit->active_fpga_exactly_one;
    if (!audit->exact_engine_envelope ||
        !audit->required_compact_waves_strict) {
        audit_issues |= LT48_M37_SAFETY_ENGINE;
    }
    if (!audit->strict_pin_mapping) {
        audit_issues |= LT48_M37_SAFETY_PIN_MAPPING;
    }
    if (!audit->applet_metadata_complete ||
        !audit->applet_ranges_nonoverlapping) {
        audit_issues |= LT48_M37_SAFETY_APPLETS;
    }
    if (!audit->active_fpga_exactly_one) {
        audit_issues |= LT48_M37_SAFETY_FPGA;
    }
    status = lt48_m37_validate_plan(&verified, &plan_issues);
    (void)status;
    /* Opening an immutable resource snapshot proves its complete read and
     * recovery transcript against the approved digest table.  That evidence
     * is stronger than the legacy 16 KiB-only public evidence shape, which
     * intentionally has no caller-settable transcript digest field. */
    if ((operation_evidence->operation == LT48_M37_READ &&
         audit->read_behavior_approved) ||
        audit->complete_group_equivalence_verified || audit->complete_source_audit_verified) {
        plan_issues &= ~LT48_M37_SAFETY_ENGINE;
    }
    if(operation_evidence->operation==LT48_M37_ELECTRICAL_ERASE &&
       audit->complete_source_audit_verified && ((audit->device_class==2006u && audit->device_type==5u &&
       audit->family_code==4u && audit->pinmap==3u)||lt48_eep21_definition_internal(audit->device_class,audit->device_type)) && operation_evidence->start==0 &&
       operation_evidence->length==audit->capacity_bytes && operation_evidence->mandatory_post_verify)
        plan_issues &= ~LT48_M37_SAFETY_OPERATION;
    if(audit->complete_source_audit_verified && audit->capacity_bytes>=131072u) {
        if(operation_evidence->chunk_size==lt48_eprom_chunk(audit,operation_evidence->length))
            plan_issues &= ~LT48_M37_SAFETY_CHUNK;
        else plan_issues |= LT48_M37_SAFETY_CHUNK;
    }
    if((audit->device_class==2003u || audit->device_class==2004u) && ((verified.start|verified.length)&1u))
        plan_issues |= LT48_M37_SAFETY_ADDRESS_RANGE;
    *out_issue_mask = audit_issues | plan_issues;
    return *out_issue_mask ? LT48_SAFETY_REJECTED : LT48_OK;
}

lt48_status lt48_installation_validate_m37_plan(
    const lt48_installation *installation,
    size_t device_index,
    const lt48_m37_plan_evidence *operation_evidence,
    uint64_t *out_issue_mask
) {
    lt48_m37_resource_audit audit;
    lt48_status status;
    if (!installation || !operation_evidence || !out_issue_mask) {
        return LT48_INVALID_ARGUMENT;
    }
    status = lt48_installation_audit_m37(installation, device_index, &audit);
    if (status != LT48_OK) return status;
    if (audit.device_class == 2001u || audit.device_class == 2006u ||
        audit.device_class == 2009u || audit.device_class == 2003u || audit.device_class == 2004u) {
        /* The group certificate belongs to the immutable resource snapshot,
         * not the preliminary parse audit or caller-supplied evidence. */
        lt48_m37_resources *resources = NULL;
        if (lt48_m37_resources_open(installation, device_index, &resources) ==
            LT48_OK) {
            status = validate_audited_plan(lt48_m37_resources_audit(resources),
                                          operation_evidence, out_issue_mask);
            lt48_m37_resources_close(resources);
            return status;
        }
    }
    return validate_audited_plan(&audit, operation_evidence, out_issue_mask);
}

lt48_status lt48_m37_resources_validate_plan(
    const lt48_m37_resources *resources,
    const lt48_m37_plan_evidence *operation_evidence,
    uint64_t *out_issue_mask
) {
    return validate_audited_plan(
        lt48_m37_resources_audit(resources), operation_evidence,
        out_issue_mask
    );
}

const char *lt48_m37_cleanup_phase_name(lt48_m37_cleanup_phase phase) {
    switch (phase) {
    case LT48_M37_CLEANUP_DEVICE_RESET: return "device-reset";
    case LT48_M37_CLEANUP_SETTLE_DELAY: return "settle-delay";
    case LT48_M37_CLEANUP_SETTLE_STATUS: return "settle-status";
    case LT48_M37_CLEANUP_POWER_OFF: return "power-off";
    case LT48_M37_CLEANUP_POWER_OFF_FLUSH: return "power-off-flush";
    case LT48_M37_CLEANUP_ZERO_ALL_RAILS: return "zero-all-rails";
    case LT48_M37_CLEANUP_RESTORE_IDLE_HEADER: return "restore-idle-fpga-header";
    case LT48_M37_CLEANUP_RESTORE_IDLE_DATA: return "restore-idle-fpga-data";
    case LT48_M37_CLEANUP_FINALIZE_CONTROLLER: return "finalize-controller";
    case LT48_M37_CLEANUP_FINALIZE_FLUSH: return "finalize-controller-flush";
    case LT48_M37_CLEANUP_SET_RESULT: return "set-result";
    case LT48_M37_CLEANUP_RELEASE_INTERFACE: return "release-interface";
    default: return NULL;
    }
}

const char *lt48_m37_safety_issue_name(uint64_t single_issue_bit) {
    switch (single_issue_bit) {
    case LT48_M37_SAFETY_ENGINE: return "engine-envelope";
    case LT48_M37_SAFETY_OPERATION: return "operation";
    case LT48_M37_SAFETY_ADDRESS_RANGE: return "address-range";
    case LT48_M37_SAFETY_CHUNK: return "chunk-size";
    case LT48_M37_SAFETY_VCC: return "vcc-selection";
    case LT48_M37_SAFETY_PIN_MAPPING: return "strict-pin-mapping";
    case LT48_M37_SAFETY_APPLETS: return "applet-provenance";
    case LT48_M37_SAFETY_FPGA: return "fpga-provenance";
    case LT48_M37_SAFETY_CLEANUP: return "cleanup-recipe";
    case LT48_M37_SAFETY_IMAGE: return "image-binding";
    case LT48_M37_SAFETY_PRECHECK: return "program-precheck";
    case LT48_M37_SAFETY_VERIFICATION: return "mandatory-verification";
    case LT48_M37_SAFETY_DEVICE_IDENTITY: return "device-identity";
    default: return NULL;
    }
}
