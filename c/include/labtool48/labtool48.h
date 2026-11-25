#ifndef LABTOOL48_LABTOOL48_H
#define LABTOOL48_LABTOOL48_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && defined(LABTOOL48_BUILDING_LIBRARY)
#define LT48_API __declspec(dllexport)
#elif defined(_WIN32)
#define LT48_API __declspec(dllimport)
#else
#define LT48_API
#endif

#define LT48_USB_VENDOR_ID UINT16_C(0x11b5)
#define LT48_USB_PRODUCT_ID_READY UINT16_C(0x2003)
#define LT48_USB_PRODUCT_ID_COLD UINT16_C(0x2004)
#define LT48_USB_PRODUCT_ID_READY_ALT UINT16_C(0x2005)
#define LT48_SHA256_SIZE 32u
#define LT48_M37_READ_HARDWARE_AUTHORIZATION UINT32_C(0x4d333752)
#define LT48_M37_OPERATION_HARDWARE_AUTHORIZATION UINT32_C(0x4d33374f)
#define LT48_FX2_LOAD_HARDWARE_AUTHORIZATION UINT32_C(0x4658324c)
#define LT48_AUTO_ID_EPROM_HARDWARE_AUTHORIZATION UINT32_C(0x41494445)

typedef enum lt48_status {
    LT48_OK = 0,
    LT48_INVALID_ARGUMENT = 1,
    LT48_NO_MEMORY = 2,
    LT48_USB_ERROR = 3,
    LT48_NOT_FOUND = 4,
    LT48_PERMISSION_DENIED = 5,
    LT48_BUFFER_TOO_SMALL = 6,
    LT48_BUSY = 7,
    LT48_PROTOCOL_ERROR = 8,
    LT48_FILE_ERROR = 9,
    LT48_FORMAT_ERROR = 10,
    LT48_TIMEOUT = 11,
    LT48_UNSUPPORTED = 12,
    LT48_COMPILE_ERROR = 13,
    LT48_RUNTIME_ERROR = 14,
    LT48_CANCELLED = 15,
    LT48_CLEANUP_FAILED = 16,
    LT48_SAFETY_REJECTED = 17,
    LT48_FIRMWARE_REQUIRED = 18,
    LT48_DEVICE_MISMATCH = 19
} lt48_status;

typedef struct lt48_context lt48_context;
typedef struct lt48_programmer lt48_programmer;
typedef struct lt48_rez lt48_rez;
typedef struct lt48_installation lt48_installation;
typedef struct lt48_compact_wave lt48_compact_wave;
typedef struct lt48_pin_map lt48_pin_map;
typedef struct lt48_compiled_wave lt48_compiled_wave;
typedef struct lt48_runtime_result lt48_runtime_result;
typedef struct lt48_runtime_machine lt48_runtime_machine;
typedef struct lt48_protocol_plan lt48_protocol_plan;
typedef struct lt48_m37_verify_plan_set lt48_m37_verify_plan_set;
typedef struct lt48_m37_program_plan_set lt48_m37_program_plan_set;
typedef struct lt48_m37_resources lt48_m37_resources;
typedef struct lt48_palce_resources lt48_palce_resources;
typedef struct lt48_pal_resources lt48_pal_resources;
typedef struct lt48_gal_resources lt48_gal_resources;
typedef struct lt48_palce_operation_result lt48_palce_operation_result;
typedef struct lt48_m37_read_collector lt48_m37_read_collector;
typedef struct lt48_fx2_firmware lt48_fx2_firmware;
typedef struct lt48_buffer lt48_buffer;
typedef struct lt48_test_vectors lt48_test_vectors;
typedef struct lt48_jtag_plan lt48_jtag_plan;
typedef struct lt48_operation_report lt48_operation_report;
typedef struct lt48_transfer_trace lt48_transfer_trace;
typedef int (*lt48_cancel_check_fn)(void *context);

typedef enum lt48_jtag_tap_state {
    LT48_JTAG_TEST_LOGIC_RESET = 0,
    LT48_JTAG_RUN_TEST_IDLE = 1,
    LT48_JTAG_SELECT_DR_SCAN = 2,
    LT48_JTAG_CAPTURE_DR = 3,
    LT48_JTAG_SHIFT_DR = 4,
    LT48_JTAG_EXIT1_DR = 5,
    LT48_JTAG_PAUSE_DR = 6,
    LT48_JTAG_EXIT2_DR = 7,
    LT48_JTAG_UPDATE_DR = 8,
    LT48_JTAG_SELECT_IR_SCAN = 9,
    LT48_JTAG_CAPTURE_IR = 10,
    LT48_JTAG_SHIFT_IR = 11,
    LT48_JTAG_EXIT1_IR = 12,
    LT48_JTAG_PAUSE_IR = 13,
    LT48_JTAG_EXIT2_IR = 14,
    LT48_JTAG_UPDATE_IR = 15
} lt48_jtag_tap_state;

typedef enum lt48_jtag_scan_kind {
    LT48_JTAG_SCAN_IR = 1,
    LT48_JTAG_SCAN_DR = 2
} lt48_jtag_scan_kind;

typedef struct lt48_jtag_scan_request {
    lt48_jtag_scan_kind kind;
    const unsigned char *tdi;
    size_t tdi_size;
    size_t bit_count;
    uint32_t tck_hz;
    uint32_t profile_max_tck_hz;
    lt48_jtag_tap_state safe_end_state;
} lt48_jtag_scan_request;

typedef struct lt48_jtag_step {
    lt48_jtag_tap_state state_before;
    lt48_jtag_tap_state state_after;
    uint8_t tms;
    uint8_t tdi;
    int sample_tdo;
    size_t scan_bit_index;
} lt48_jtag_step;

typedef struct lt48_jtag_plan_info {
    lt48_jtag_scan_kind kind;
    size_t bit_count;
    size_t step_count;
    size_t sampled_step_count;
    size_t cancellation_recovery_step_count;
    uint32_t tck_hz;
    uint32_t profile_max_tck_hz;
    lt48_jtag_tap_state final_state;
    int transport_ready;
    unsigned char sha256[LT48_SHA256_SIZE];
} lt48_jtag_plan_info;

typedef struct lt48_jtag_result_request {
    const unsigned char *tdo;
    size_t tdo_size;
    const unsigned char *expected;
    size_t expected_size;
    const unsigned char *mask;
    size_t mask_size;
    size_t bit_count;
} lt48_jtag_result_request;

typedef struct lt48_jtag_result {
    int compared;
    int matched;
    int has_mismatch;
    size_t first_mismatch_bit;
    uint8_t expected_bit;
    uint8_t actual_bit;
    int stuck_low_observed;
    int stuck_high_observed;
    size_t compared_bit_count;
} lt48_jtag_result;

typedef enum lt48_jtag_chain_element_kind {
    LT48_JTAG_CHAIN_BYPASS = 1,
    LT48_JTAG_CHAIN_IDCODE = 2
} lt48_jtag_chain_element_kind;

typedef struct lt48_jtag_chain_element {
    lt48_jtag_chain_element_kind kind;
    size_t first_bit;
    size_t bit_count;
    uint32_t idcode;
    uint16_t manufacturer_id;
    uint16_t part_number;
    uint8_t version;
} lt48_jtag_chain_element;

typedef struct lt48_jtag_chain_report {
    size_t requested_element_count;
    size_t decoded_element_count;
    size_t consumed_bit_count;
    size_t trailing_bit_count;
    int complete;
} lt48_jtag_chain_report;

typedef enum lt48_jtag_power_ownership {
    LT48_JTAG_POWER_EXTERNAL = 1,
    LT48_JTAG_POWER_ADAPTER = 2,
    LT48_JTAG_POWER_PROGRAMMER = 3
} lt48_jtag_power_ownership;

typedef enum lt48_jtag_profile_assurance {
    LT48_JTAG_ASSURANCE_UNKNOWN = 0,
    LT48_JTAG_ASSURANCE_PRESENT = 1,
    LT48_JTAG_ASSURANCE_NOT_REQUIRED = 2
} lt48_jtag_profile_assurance;

typedef enum lt48_jtag_target_reset_policy {
    LT48_JTAG_RESET_NOT_CONNECTED = 1,
    LT48_JTAG_RESET_HOLD_DURING_CONNECT = 2,
    LT48_JTAG_RESET_RELEASED = 3
} lt48_jtag_target_reset_policy;

typedef struct lt48_jtag_target_profile {
    const char *adapter_id;
    const char *adapter_revision;
    uint8_t tck_contact;
    uint8_t tms_contact;
    uint8_t tdi_contact;
    uint8_t tdo_contact;
    uint8_t ground_contact;
    uint8_t vref_contact;
    uint8_t reset_contact;
    uint8_t target_power_contact;
    lt48_jtag_power_ownership power_ownership;
    uint32_t vref_min_millivolts;
    uint32_t vref_max_millivolts;
    uint32_t maximum_tck_hz;
    lt48_jtag_profile_assurance level_translation;
    lt48_jtag_profile_assurance contention_control;
    lt48_jtag_target_reset_policy target_reset;
    int disconnect_high_impedance;
    int disconnect_target_power_off;
} lt48_jtag_target_profile;

typedef enum lt48_jtag_profile_issue {
    LT48_JTAG_PROFILE_VALID = 0,
    LT48_JTAG_PROFILE_IDENTITY = 1,
    LT48_JTAG_PROFILE_CONTACT_RANGE = 2,
    LT48_JTAG_PROFILE_CONTACT_CONFLICT = 3,
    LT48_JTAG_PROFILE_POWER_OWNERSHIP = 4,
    LT48_JTAG_PROFILE_VREF_RANGE = 5,
    LT48_JTAG_PROFILE_TCK_LIMIT = 6,
    LT48_JTAG_PROFILE_LEVEL_TRANSLATION = 7,
    LT48_JTAG_PROFILE_CONTENTION_CONTROL = 8,
    LT48_JTAG_PROFILE_RESET_POLICY = 9,
    LT48_JTAG_PROFILE_DISCONNECT_STATE = 10
} lt48_jtag_profile_issue;

typedef struct lt48_jtag_profile_report {
    lt48_jtag_profile_issue issue;
    int complete;
    size_t mapped_contact_count;
    char diagnostic[192];
    unsigned char sha256[LT48_SHA256_SIZE];
} lt48_jtag_profile_report;

typedef struct lt48_buffer_checksum {
    uint32_t additive32;
    unsigned char sha256[LT48_SHA256_SIZE];
} lt48_buffer_checksum;

typedef enum lt48_buffer_short_input_policy {
    LT48_BUFFER_SHORT_REJECT = 0,
    LT48_BUFFER_SHORT_PAD = 1
} lt48_buffer_short_input_policy;

typedef enum lt48_buffer_long_input_policy {
    LT48_BUFFER_LONG_REJECT = 0,
    LT48_BUFFER_LONG_TRUNCATE = 1
} lt48_buffer_long_input_policy;

typedef struct lt48_buffer_import_options {
    size_t buffer_size;
    lt48_buffer_short_input_policy short_input;
    lt48_buffer_long_input_policy long_input;
    unsigned char pad_value;
    size_t word_size_bytes;
    int reverse_word_bytes;
} lt48_buffer_import_options;

typedef struct lt48_buffer_import_result {
    size_t input_size;
    size_t imported_size;
    size_t padded_size;
    size_t truncated_size;
} lt48_buffer_import_result;

typedef enum lt48_image_format {
    LT48_IMAGE_FORMAT_AUTO = 0,
    LT48_IMAGE_FORMAT_RAW_BINARY = 1,
    LT48_IMAGE_FORMAT_INTEL_HEX = 2,
    LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD = 3,
    LT48_IMAGE_FORMAT_STRAIGHT_HEX = 4,
    LT48_IMAGE_FORMAT_MOS_TECHNOLOGY = 5,
    LT48_IMAGE_FORMAT_TEKTRONIX_HEX = 6,
    LT48_IMAGE_FORMAT_HP64000_ABSOLUTE = 7
} lt48_image_format;

typedef enum lt48_image_gap_policy {
    LT48_IMAGE_GAPS_PRESERVE = 0,
    LT48_IMAGE_GAPS_FILL = 1
} lt48_image_gap_policy;

typedef struct lt48_image_import_options {
    lt48_image_format format;
    size_t buffer_size;
    uint64_t address_base;
    lt48_image_gap_policy gap_policy;
    unsigned char gap_fill_value;
} lt48_image_import_options;

typedef struct lt48_image_import_result {
    lt48_image_format format;
    size_t input_size;
    size_t record_count;
    size_t data_record_count;
    size_t encoded_data_byte_count;
    size_t mapped_data_byte_count;
    size_t out_of_range_byte_count;
    size_t overlap_byte_count;
    size_t gap_count;
    size_t gap_byte_count;
    uint64_t minimum_source_address;
    uint64_t maximum_source_address_exclusive;
    int has_data;
    int has_execution_start;
    uint64_t execution_start_address;
} lt48_image_import_result;

typedef enum lt48_image_byte_mode {
    LT48_IMAGE_BYTES_NORMAL = 0,
    LT48_IMAGE_BYTES_EVEN = 1,
    LT48_IMAGE_BYTES_ODD = 2,
    LT48_IMAGE_BYTES_LANE_0_OF_4 = 3,
    LT48_IMAGE_BYTES_LANE_1_OF_4 = 4,
    LT48_IMAGE_BYTES_LANE_2_OF_4 = 5,
    LT48_IMAGE_BYTES_LANE_3_OF_4 = 6
} lt48_image_byte_mode;

typedef struct lt48_image_export_options {
    lt48_image_format format;
    size_t buffer_offset;
    size_t buffer_length;
    uint64_t address_base;
    lt48_image_byte_mode byte_mode;
    int overwrite;
} lt48_image_export_options;

typedef struct lt48_image_export_result {
    lt48_image_format format;
    size_t source_byte_count;
    size_t output_data_byte_count;
    size_t record_count;
    uint64_t minimum_output_address;
    uint64_t maximum_output_address_exclusive;
} lt48_image_export_result;

typedef enum lt48_buffer_origin_kind {
    LT48_BUFFER_ORIGIN_GENERATED_FILL = 1,
    LT48_BUFFER_ORIGIN_FILE_IMPORT = 2,
    LT48_BUFFER_ORIGIN_DEVICE_READ = 3
} lt48_buffer_origin_kind;

/* Provenance retained with the owned image. mutation_count covers successful
 * writes, fills, and byte edits after creation/import. */
typedef struct lt48_buffer_provenance {
    lt48_buffer_origin_kind origin;
    size_t input_size;
    size_t imported_size;
    size_t padded_size;
    size_t truncated_size;
    lt48_buffer_short_input_policy short_input;
    lt48_buffer_long_input_policy long_input;
    unsigned char fill_value;
    size_t word_size_bytes;
    int reverse_word_bytes;
    size_t mutation_count;
    lt48_image_format image_format;
    uint64_t image_address_base;
    lt48_image_gap_policy image_gap_policy;
} lt48_buffer_provenance;

/* Canonical REQ-BUF-004 binding. binding_sha256 covers the fixed schema tag,
 * exact full/range hashes, selected range, definition geometry, all import
 * and fill rules, and the post-import mutation count. */
typedef struct lt48_buffer_binding {
    uint32_t schema_version;
    size_t definition_capacity;
    size_t range_offset;
    size_t range_length;
    unsigned char image_sha256[LT48_SHA256_SIZE];
    unsigned char range_sha256[LT48_SHA256_SIZE];
    lt48_buffer_provenance provenance;
    unsigned char binding_sha256[LT48_SHA256_SIZE];
} lt48_buffer_binding;

typedef struct lt48_buffer_compare_result {
    int equal;
    size_t compared_size;
    size_t first_mismatch_offset;
    unsigned char expected_value;
    unsigned char actual_value;
} lt48_buffer_compare_result;

typedef enum lt48_buffer_byte_order {
    LT48_BUFFER_LITTLE_ENDIAN = 0,
    LT48_BUFFER_BIG_ENDIAN = 1
} lt48_buffer_byte_order;

/* Portable representation of the original Function Test Vectors editor.
 * Each row contains exactly pin_count symbols. The accepted alphabet is the
 * one enforced by WLT48UXP.exe: 0, 1, B, C, D, F, H, K, L, N, P, U, X, Z.
 * This model is editor state only; it does not authorize a live function
 * test or assign electrical meaning to symbols whose engine is undecoded. */
typedef struct lt48_test_vector_info {
    size_t pin_count;
    size_t row_count;
    size_t current_row;
    int has_undo;
} lt48_test_vector_info;

typedef struct lt48_private_cache_report {
    size_t source_file_count;
    size_t object_count;
    size_t total_bytes;
    size_t verified_object_count;
    unsigned char installation_manifest_sha256[LT48_SHA256_SIZE];
    unsigned char cache_manifest_sha256[LT48_SHA256_SIZE];
} lt48_private_cache_report;

typedef enum lt48_component_validation_issue {
    LT48_COMPONENT_VALID = 0,
    LT48_COMPONENT_FILE_ERROR = 1,
    LT48_COMPONENT_JSON_ERROR = 2,
    LT48_COMPONENT_SCHEMA_ERROR = 3,
    LT48_COMPONENT_DUPLICATE_ID = 4,
    LT48_COMPONENT_MISSING_REFERENCE = 5,
    LT48_COMPONENT_KIND_MISMATCH = 6,
    LT48_COMPONENT_INHERITANCE_CYCLE = 7
} lt48_component_validation_issue;

typedef struct lt48_component_validation_report {
    uint32_t schema_version;
    size_t document_count;
    size_t object_count;
    size_t combination_count;
    size_t inherited_object_count;
    size_t reference_count;
    lt48_component_validation_issue issue;
    size_t issue_object_index;
    char diagnostic[192];
    unsigned char source_sha256[LT48_SHA256_SIZE];
} lt48_component_validation_report;

typedef struct lt48_component_resolution {
    unsigned char *effective_json;
    size_t effective_json_size;
    unsigned char effective_sha256[LT48_SHA256_SIZE];
    unsigned char *delta_json;
    size_t delta_json_size;
    unsigned char delta_sha256[LT48_SHA256_SIZE];
    size_t inheritance_depth;
    size_t effective_property_count;
    size_t changed_property_count;
} lt48_component_resolution;

typedef struct lt48_component_revision_report {
    size_t previous_object_count;
    size_t output_object_count;
    unsigned char previous_source_sha256[LT48_SHA256_SIZE];
    unsigned char object_record_sha256[LT48_SHA256_SIZE];
    unsigned char output_source_sha256[LT48_SHA256_SIZE];
} lt48_component_revision_report;

typedef struct lt48_component_semantic_report {
    size_t object_count;
    size_t validated_object_count;
    size_t blocker_count;
    size_t invalid_object_count;
    size_t combination_count;
    size_t validated_combination_count;
    size_t invalid_combination_count;
    size_t first_problem_object_index;
    size_t first_problem_combination_index;
    int complete;
    char first_problem_object_id[96];
    char diagnostic[192];
    unsigned char source_sha256[LT48_SHA256_SIZE];
} lt48_component_semantic_report;

typedef enum lt48_component_phase_kind {
    LT48_COMPONENT_PHASE_PREPARE = 1,
    LT48_COMPONENT_PHASE_PRECHECK = 2,
    LT48_COMPONENT_PHASE_POWER = 3,
    LT48_COMPONENT_PHASE_INITIALIZE = 4,
    LT48_COMPONENT_PHASE_TRANSFER = 5,
    LT48_COMPONENT_PHASE_VERIFY = 6,
    LT48_COMPONENT_PHASE_DELAY = 7,
    LT48_COMPONENT_PHASE_RESULT = 8,
    LT48_COMPONENT_PHASE_CLEANUP = 9
} lt48_component_phase_kind;

typedef enum lt48_component_rail_state {
    LT48_COMPONENT_RAIL_OFF = 1,
    LT48_COMPONENT_RAIL_NOMINAL = 2,
    LT48_COMPONENT_RAIL_PROGRAMMING = 3,
    LT48_COMPONENT_RAIL_VERIFY_LOW = 4,
    LT48_COMPONENT_RAIL_VERIFY_HIGH = 5
} lt48_component_rail_state;

typedef enum lt48_component_pin_state {
    LT48_COMPONENT_PIN_LOW = 1,
    LT48_COMPONENT_PIN_HIGH = 2,
    LT48_COMPONENT_PIN_INPUT = 3,
    LT48_COMPONENT_PIN_OUTPUT = 4,
    LT48_COMPONENT_PIN_BIDIRECTIONAL = 5,
    LT48_COMPONENT_PIN_HIGH_IMPEDANCE = 6
} lt48_component_pin_state;

typedef enum lt48_component_transition_kind {
    LT48_COMPONENT_TRANSITION_PHASE = 1,
    LT48_COMPONENT_TRANSITION_COMPLETE = 2,
    LT48_COMPONENT_TRANSITION_ERROR = 3,
    LT48_COMPONENT_TRANSITION_ABORT = 4
} lt48_component_transition_kind;

typedef struct lt48_component_plan_rail {
    char name[64];
    lt48_component_rail_state state;
    uint32_t centivolts;
} lt48_component_plan_rail;

typedef struct lt48_component_plan_pin {
    char logical[96];
    uint32_t package_pin;
    uint32_t programmer_pin;
    lt48_component_pin_state state;
} lt48_component_plan_pin;

typedef struct lt48_component_plan_payload {
    int present;
    char id[96];
    char declared_sha256[LT48_SHA256_SIZE * 2u + 1u];
    char origin[64];
    char payload_type[64];
    char format[64];
    char abi[96];
} lt48_component_plan_payload;

typedef struct lt48_component_plan_transition {
    lt48_component_transition_kind kind;
    size_t phase_index;
} lt48_component_plan_transition;

typedef struct lt48_component_plan_phase {
    char id[96];
    lt48_component_phase_kind kind;
    uint32_t timeout_ms;
    uint32_t retry_limit;
    lt48_component_plan_rail *rails;
    size_t rail_count;
    lt48_component_plan_pin *pins;
    size_t pin_count;
    lt48_component_plan_payload payload;
    lt48_component_plan_transition on_success;
    lt48_component_plan_transition on_failure;
} lt48_component_plan_phase;

typedef struct lt48_component_plan {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    size_t combination_index;
    size_t phase_count;
    size_t mapped_pin_state_count;
    size_t rail_state_count;
    int mutating;
    char operation_id[96];
    char completion_protocol[96];
    char error_protocol[96];
    size_t entry_phase_index;
    size_t cleanup_phase_index;
    lt48_component_plan_phase *phases;
} lt48_component_plan;

typedef struct lt48_component_plan_matrix {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    unsigned char bundle_sha256[LT48_SHA256_SIZE];
    size_t combination_count;
    size_t plan_count;
    size_t mutating_plan_count;
} lt48_component_plan_matrix;

typedef struct lt48_component_all_variants_report {
    lt48_component_validation_report structure;
    lt48_component_semantic_report semantics;
    unsigned char matrix_sha256[LT48_SHA256_SIZE];
    size_t plan_count;
    size_t mutating_plan_count;
    int structure_valid;
    int semantics_complete;
    int matrix_complete;
    int complete;
} lt48_component_all_variants_report;

typedef struct lt48_component_adapter_test_plan {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    size_t continuity_test_count;
    size_t isolation_test_count;
    int active_components;
    int automatic_continuity_ready;
} lt48_component_adapter_test_plan;

typedef struct lt48_component_payload_build_plan {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    unsigned char bundle_sha256[LT48_SHA256_SIZE];
    size_t payload_count;
    size_t authored_payload_count;
    size_t imported_payload_count;
} lt48_component_payload_build_plan;

typedef struct lt48_component_payload_input_verification {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    unsigned char build_plan_sha256[LT48_SHA256_SIZE];
    size_t authored_payload_count;
    size_t required_input_count;
    size_t verified_input_count;
    size_t rejected_input_count;
    int complete;
} lt48_component_payload_input_verification;

typedef struct lt48_component_payload_build_execution {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    unsigned char input_verification_sha256[LT48_SHA256_SIZE];
    size_t attempted_payload_count;
    size_t started_payload_count;
    size_t completed_payload_count;
    size_t verified_output_count;
    int complete;
    int cancelled;
} lt48_component_payload_build_execution;

typedef struct lt48_component_evidence_report {
    char evidence_id[96];
    char evidence_type[48];
    char method[64];
    char status[16];
    char performed_date[11];
    char operation_id[96];
    size_t combination_index;
    char bundle_sha256[LT48_SHA256_SIZE * 2u + 1u];
    char matrix_sha256[LT48_SHA256_SIZE * 2u + 1u];
    char plan_sha256[LT48_SHA256_SIZE * 2u + 1u];
    char artifact_sha256[LT48_SHA256_SIZE * 2u + 1u];
    int passed;
    int hardware_evidence;
    int complete;
    char diagnostic[192];
    unsigned char source_sha256[LT48_SHA256_SIZE];
} lt48_component_evidence_report;

typedef struct lt48_component_evidence_verification {
    lt48_component_evidence_report evidence;
    size_t artifact_size;
    unsigned char artifact_sha256[LT48_SHA256_SIZE];
    int artifact_verified;
    int bundle_verified;
    int matrix_verified;
    int plan_verified;
    int complete;
    char diagnostic[192];
} lt48_component_evidence_verification;

typedef struct lt48_component_evidence_import_report {
    char evidence_id[96];
    unsigned char record_sha256[LT48_SHA256_SIZE];
    unsigned char artifact_sha256[LT48_SHA256_SIZE];
    size_t record_size;
    size_t artifact_size;
    int record_created;
    int artifact_created;
    int evidence_passed;
} lt48_component_evidence_import_report;

typedef enum lt48_component_evidence_level {
    LT48_COMPONENT_EVIDENCE_PLANNED = 1,
    LT48_COMPONENT_EVIDENCE_SAFETY_VALIDATED = 2,
    LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED = 3,
    LT48_COMPONENT_EVIDENCE_ENGINE_HARDWARE_VERIFIED = 4,
    LT48_COMPONENT_EVIDENCE_DEVICE_HARDWARE_VERIFIED = 5
} lt48_component_evidence_level;

enum {
    LT48_COMPONENT_EVIDENCE_MISSING_SIMULATION = 1u << 0,
    LT48_COMPONENT_EVIDENCE_MISSING_FAULT_INJECTION = 1u << 1,
    LT48_COMPONENT_EVIDENCE_MISSING_BEHAVIOR = 1u << 2,
    LT48_COMPONENT_EVIDENCE_MISSING_ADAPTER_BENCH = 1u << 3,
    LT48_COMPONENT_EVIDENCE_MISSING_ENGINE_HARDWARE = 1u << 4,
    LT48_COMPONENT_EVIDENCE_MISSING_DEVICE_HARDWARE = 1u << 5
};

typedef struct lt48_component_evidence_plan_status {
    size_t combination_index;
    char operation_id[96];
    int mutating;
    lt48_component_evidence_level level;
    uint32_t missing_evidence;
} lt48_component_evidence_plan_status;

typedef struct lt48_component_evidence_audit {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    size_t stored_record_count;
    size_t verified_record_count;
    size_t invalid_record_count;
    size_t passing_record_count;
    size_t failing_record_count;
    size_t plan_count;
    size_t safety_validated_plan_count;
    size_t behavior_validated_plan_count;
    size_t engine_hardware_plan_count;
    size_t device_hardware_plan_count;
    size_t mutating_behavior_blocker_count;
    int store_integrity_valid;
    int release_ready;
    lt48_component_evidence_plan_status *plans;
} lt48_component_evidence_audit;

typedef struct lt48_component_release_check {
    unsigned char *json;
    size_t json_size;
    unsigned char sha256[LT48_SHA256_SIZE];
    size_t plan_count;
    size_t evidence_candidate_count;
    size_t transport_ready_plan_count;
    size_t blocker_count;
    int evidence_store_integrity_valid;
    int release_ready;
} lt48_component_release_check;

typedef struct lt48_operation_report_info {
    uint32_t schema_version;
    const char *run_id;
    const char *device_key;
    const char *operation;
    const char *status;
    const char *support_level;
    const char *plan_sha256;
    const char *input_image_sha256;
    int has_output;
    size_t output_size;
    const char *output_sha256;
    size_t resource_count;
    size_t phase_count;
    size_t session_event_count;
    size_t cleanup_phase_count;
    size_t cleanup_failed_phase_count;
    const char *transport_close_status;
    const char *verification_result;
    size_t progress_observer_error_count;
    int has_failure;
    int has_address_range;
    uint32_t address_start;
    uint32_t address_length;
    int mismatch_override;
    int cancelled;
    const char *observed_result;
    const char *failure_type;
    const char *failure_message;
    const char *report_sha256;
} lt48_operation_report_info;

typedef struct lt48_operation_report_resource {
    const char *name;
    size_t size;
    unsigned char sha256[LT48_SHA256_SIZE];
} lt48_operation_report_resource;

typedef enum lt48_trace_direction {
    LT48_TRACE_OUT = 0,
    LT48_TRACE_IN = 1
} lt48_trace_direction;

typedef struct lt48_trace_event {
    size_t sequence;
    lt48_trace_direction direction;
    uint8_t endpoint;
    size_t requested_length;
    const char *data_hex;
    size_t data_size;
    uint32_t timeout_ms;
    const char *status;
    const char *error;
} lt48_trace_event;

typedef struct lt48_trace_info {
    uint32_t schema_version;
    size_t event_count;
    const unsigned char *metadata_json;
    size_t metadata_json_size;
    int had_claimed_sha256;
    unsigned char trace_sha256[LT48_SHA256_SIZE];
} lt48_trace_info;

typedef struct lt48_trace_comparison {
    int equal;
    size_t first_difference;
    int has_left;
    int has_right;
    lt48_trace_event left;
    lt48_trace_event right;
} lt48_trace_comparison;

/*
 * Inputs for a payload-free schema-1 planned report. All strings and arrays
 * are borrowed for the duration of the call. Digests contain raw SHA-256
 * bytes. The writer creates a fresh run id and UTC timestamps and never
 * overwrites an existing path.
 */
typedef struct lt48_operation_report_plan {
    const char *device_key;
    const char *definition;
    uint16_t device_class;
    unsigned char source_sha256[LT48_SHA256_SIZE];
    const char *primary_engine;
    const char *semantic_family;
    const char *normalized_engine_profile;
    const char *operation;
    const char *support_level;
    uint32_t address_start;
    uint32_t address_length;
    unsigned char plan_sha256[LT48_SHA256_SIZE];
    int has_input_image;
    unsigned char input_image_sha256[LT48_SHA256_SIZE];
    const lt48_operation_report_resource *resources;
    size_t resource_count;
    const uint32_t *planned_voltages_centivolts;
    size_t planned_voltage_count;
} lt48_operation_report_plan;

typedef struct lt48_operation_report_phase_record {
    const char *phase;
    const char *category;
    double started_offset_ms;
    double duration_ms;
    const char *status;
    const char *error_type;
    const char *error_message;
} lt48_operation_report_phase_record;

typedef struct lt48_operation_report_result {
    const char *status;
    double duration_ms;
    const lt48_operation_report_phase_record *phases;
    size_t phase_count;
    int has_output;
    size_t output_size;
    unsigned char output_sha256[LT48_SHA256_SIZE];
    size_t cleanup_phase_count;
    size_t cleanup_failed_phase_count;
    const char *transport_close_status;
    const char *verification_result;
    size_t progress_observer_error_count;
    int mismatch_override;
    int cancelled;
    const char *observed_result;
    const char *failure_type;
    const char *failure_message;
} lt48_operation_report_result;

typedef struct lt48_programmer_info {
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t usb_version_bcd;
    uint8_t bus_number;
    uint8_t device_address;
    uint16_t device_version_bcd;
    char model[64];
    char serial_number[64];
} lt48_programmer_info;

typedef struct lt48_transport_info {
    uint8_t packet_size_ack;
    uint8_t mode_ack;
} lt48_transport_info;

typedef struct lt48_fx2_firmware_info {
    size_t record_count;
    size_t payload_size;
    size_t image_size;
    int verified_owner_image;
    unsigned char file_sha256[LT48_SHA256_SIZE];
    unsigned char image_sha256[LT48_SHA256_SIZE];
} lt48_fx2_firmware_info;

typedef struct lt48_fx2_load_result {
    int cpu_halted;
    int cpu_released;
    size_t records_written;
    size_t bytes_written;
} lt48_fx2_load_result;

typedef struct lt48_fx2_load_preparation {
    lt48_fx2_firmware_info firmware;
    uint8_t bus_number;
    uint8_t device_address;
    unsigned char confirmation_sha256[LT48_SHA256_SIZE];
} lt48_fx2_load_preparation;

typedef struct lt48_rez_info {
    uint32_t declared_size;
    uint32_t table_offset;
    size_t actual_size;
    size_t resource_count;
} lt48_rez_info;

typedef struct lt48_rez_resource {
    const char *name;
    const char *type_name;
    uint32_t offset;
    uint32_t size;
} lt48_rez_resource;

typedef struct lt48_runtime_frame_profile {
    uint8_t buffer_count;
    uint8_t mode_buffer;
    uint32_t reset_mask; /* Low 32 buffers; higher declared buffers reset. High preservation is rejected. */
} lt48_runtime_frame_profile;

#define LT48_LOGIC_MAX_FUSE_CODES 15u

/* Exact TFuseCode serialized header. The five 16-bit dimensions retain their
 * on-disk/deserializer order until the iterator roles are named conclusively.
 * bytecode contains instruction_count fixed three-byte instructions and is
 * borrowed from the owning lt48_rez. */
typedef struct lt48_fuse_code {
    uint16_t instruction_count;
    uint16_t dimensions[5];
    uint32_t total_fuse_count;
    const unsigned char *bytecode;
    size_t bytecode_size;
} lt48_fuse_code;

typedef struct lt48_logic_fuse_codes {
    size_t count;
    uint16_t identifiers[LT48_LOGIC_MAX_FUSE_CODES];
    /* Optional dialog-139 contexts serialized after the primary list. Value
     * counts are distinct from TFuseCode layout totals because a family may
     * include an additional control fuse in the layout. */
    uint32_t e_field_value_count;
    uint16_t e_field_identifier;
    int has_e_field;
    uint32_t user_code_value_count;
    uint16_t user_code_identifier;
    int has_user_code;
    uint32_t primary_fuse_count;
} lt48_logic_fuse_codes;

#define LT48_FUSE_LAYOUT_STREAM_COUNT 5u

typedef enum lt48_fuse_layout_stream {
    LT48_FUSE_STREAM_T = 0,
    LT48_FUSE_STREAM_P = 1,
    LT48_FUSE_STREAM_I = 2,
    LT48_FUSE_STREAM_O = 3,
    LT48_FUSE_STREAM_R = 4
} lt48_fuse_layout_stream;

/* Structural result of the original TFuseCode layout interpreter.  total is
 * the number of fuses generated by a stream after nested repetition.  R is
 * the special row/run stream; row_count is meaningful only for R. */
typedef struct lt48_fuse_layout_stream_info {
    uint32_t total;
    uint32_t row_count;
    uint16_t declared_minimum;
    size_t node_count;
    size_t maximum_nesting;
} lt48_fuse_layout_stream_info;

typedef struct lt48_fuse_layout {
    lt48_fuse_layout_stream_info streams[LT48_FUSE_LAYOUT_STREAM_COUNT];
    size_t instruction_count;
    size_t maximum_nesting;
} lt48_fuse_layout;

/* One exact row emitted by the TFuseCode R stream.  first_fuse is the linear
 * fuse number displayed at the left of dialog 139; length is the number of
 * editable fuse positions on that row. */
typedef struct lt48_fuse_row {
    uint32_t first_fuse;
    uint16_t length;
} lt48_fuse_row;

typedef struct lt48_fuse_coordinate {
    uint16_t fuse_type;
    uint16_t row;
    uint16_t column;
    uint16_t bit;
} lt48_fuse_coordinate;

typedef struct lt48_jedec_info {
    size_t fuse_count;
    uint16_t fuse_checksum;
    uint16_t transmission_checksum;
    int has_fuse_checksum;
    int has_transmission_checksum;
    int security_enabled;
} lt48_jedec_info;

/*
 * Frontend-neutral model for definition-provided property sheets.  The
 * recovered application uses the same three control classes for
 * Configuration and family-specific option pages: bounded numbers,
 * independent checkboxes, and one-of-N choices.  Frontends must edit a draft
 * copy and commit it only when the modal sheet is accepted.
 */
#define LT48_PROPERTY_MAX_PAGES 20u
#define LT48_PROPERTY_MAX_NUMBER_CONTROLS 32u
#define LT48_PROPERTY_MAX_CHECKBOX_BANKS 32u
#define LT48_PROPERTY_CHECKBOXES_PER_BANK 32u
#define LT48_PROPERTY_MAX_CHECKBOX_CONTROLS \
    (LT48_PROPERTY_MAX_CHECKBOX_BANKS * LT48_PROPERTY_CHECKBOXES_PER_BANK)
#define LT48_PROPERTY_MAX_CHOICE_CONTROLS 32u
#define LT48_PROPERTY_MAX_CHOICES 32u
#define LT48_PROPERTY_MAX_CONTROLS_PER_PAGE \
    (LT48_PROPERTY_MAX_NUMBER_CONTROLS + \
     LT48_PROPERTY_MAX_CHECKBOX_CONTROLS + \
     LT48_PROPERTY_MAX_CHOICE_CONTROLS)

typedef enum lt48_property_control_kind {
    LT48_PROPERTY_NUMBER = 1,
    LT48_PROPERTY_CHECKBOX = 2,
    LT48_PROPERTY_CHOICE = 3
} lt48_property_control_kind;

typedef struct lt48_property_number {
    uint32_t minimum;
    uint32_t maximum;
    uint8_t radix;
} lt48_property_number;

typedef struct lt48_property_choice {
    const char *const *labels;
    size_t count;
} lt48_property_choice;

typedef struct lt48_property_control {
    const char *key;
    const char *label;
    lt48_property_control_kind kind;
    uint32_t value;
    union {
        lt48_property_number number;
        lt48_property_choice choice;
    } metadata;
} lt48_property_control;

typedef struct lt48_property_page {
    const char *title;
    lt48_property_control *controls;
    size_t control_count;
} lt48_property_page;

typedef struct lt48_property_model {
    const char *title;
    lt48_property_page *pages;
    size_t page_count;
} lt48_property_model;

typedef struct lt48_property_value {
    const char *key;
    uint32_t value;
} lt48_property_value;

typedef struct lt48_property_document lt48_property_document;
typedef struct lt48_family_operation_options lt48_family_operation_options;

typedef enum lt48_property_sheet_kind {
    LT48_PROPERTY_SHEET_CONFIGURATION = 1,
    LT48_PROPERTY_SHEET_FUSE_MAP = 2,
    LT48_PROPERTY_SHEET_TEST_VECTOR = 3,
    LT48_PROPERTY_SHEET_OPERATION_OPTION = 4
} lt48_property_sheet_kind;

typedef struct lt48_property_validation_issue {
    size_t page_index;
    size_t control_index;
    const char *reason;
} lt48_property_validation_issue;

LT48_API lt48_status lt48_property_model_validate(
    const lt48_property_model *model,
    lt48_property_validation_issue *out_issue
);
/* Applies a complete, exact-key snapshot transactionally. No model value is
 * changed unless every model control occurs exactly once and the resulting
 * model remains valid. */
LT48_API lt48_status lt48_property_model_apply_values(
    lt48_property_model *model,
    const lt48_property_value *values,
    size_t value_count
);
LT48_API lt48_status lt48_property_document_open(
    const char *path,
    lt48_property_document **out_document
);
LT48_API lt48_status lt48_property_document_from_operation_options(
    uint16_t device_class,
    uint32_t capacity_bytes,
    uint32_t element_count,
    const lt48_family_operation_options *options,
    lt48_property_document **out_document
);
LT48_API void lt48_property_document_close(
    lt48_property_document *document
);
LT48_API lt48_property_model *lt48_property_document_model(
    lt48_property_document *document
);

typedef enum lt48_device_definition_kind {
    LT48_DEVICE_DEFINITION_MEMORY = 1,
    LT48_DEVICE_DEFINITION_LOGIC = 2,
    LT48_DEVICE_DEFINITION_POF = 3,
    LT48_DEVICE_DEFINITION_POF_FUSE = 4,
    LT48_DEVICE_DEFINITION_XILINX_MEMORY = 5,
    LT48_DEVICE_DEFINITION_DOUBLE_BUFFER_MEMORY = 6,
    LT48_DEVICE_DEFINITION_ADDRESS_TABLE_MEMORY = 7,
    LT48_DEVICE_DEFINITION_XP_MEMORY = 8
} lt48_device_definition_kind;

/*
 * Common, immutable header decoded from a device-definition resource. The
 * strings remain owned by rez. device_size is present only for TXPMemDev;
 * has_device_size distinguishes an encoded zero from an absent field.
 * Memory-derived definitions additionally have the primary element geometry
 * consumed by the common virtual-memory host engine. TDblBufMemDev appends a
 * second element geometry for its independently allocated host buffer.
 * auxiliary_value is retained without assigning semantics that have not yet
 * been proved for every device class.
 */
typedef struct lt48_device_definition {
    const char *resource_name;
    const char *type_name;
    lt48_device_definition_kind kind;
    uint16_t family_code;
    uint16_t pinmap;
    uint16_t value2;
    uint16_t value3;
    uint32_t device_size;
    uint32_t element_count;
    uint16_t element_bits;
    uint32_t auxiliary_value;
    uint32_t secondary_element_count;
    uint16_t secondary_element_bits;
    const unsigned char *trailing_data;
    size_t trailing_data_size;
    int has_device_size;
    int has_element_geometry;
    int has_secondary_element_geometry;
} lt48_device_definition;

typedef struct lt48_catalogue_entry {
    const char *vendor;
    const unsigned char *device_name;
    size_t device_name_length;
    uint16_t numeric_column_count;
    uint16_t table_index;
    uint16_t device_class;
    uint16_t device_type;
    uint16_t unknown3;
    uint16_t manufacturer_id;
    uint16_t device_id;
    uint16_t unknown6;
    uint16_t unknown7;
    uint16_t unknown8;
    uint16_t unknown9;
} lt48_catalogue_entry;

typedef struct lt48_family_info {
    uint16_t operation_count;
    uint16_t variable_count;
    /* Exact Windows Configuration map key serialized as TFamCode word 2.
     * Zero means that the family does not own a Configuration property set. */
    uint16_t configuration_set;
    /* Compatibility alias for callers which consumed this before its use was
     * recovered. Equal to configuration_set. */
    uint16_t header_value;
    size_t trailing_data_size;
} lt48_family_info;

/* Exact 0xd4-byte Device Operation Option record embedded in TFamCode. */
struct lt48_family_operation_options {
    uint32_t engine_flags;
    uint32_t checkbox_presence;
    uint32_t choice_presence;
    uint32_t number_presence;
    uint32_t checkbox_values;
    uint32_t number_values[32];
    uint16_t choice_values[32];
};

typedef struct lt48_family_operation {
    uint16_t operation_id;
    uint16_t wave;
} lt48_family_operation;

typedef struct lt48_family_variable {
    uint16_t values[5];
} lt48_family_variable;

/* Top-level DefineName/TDNCol namespace. Names are borrowed Pascal-string
 * payloads owned by rez and are not NUL terminated. IDs are scoped by the
 * textual prefix (_FUN_, _VAR_, _BUF_, ...), so callers must not merge the
 * namespaces by identifier alone. */
typedef struct lt48_defined_name_collection_info {
    uint32_t count;
    uint32_t value2;
    uint32_t value3;
    const unsigned char *trailing_data;
    size_t trailing_data_size;
} lt48_defined_name_collection_info;

typedef struct lt48_defined_name {
    const unsigned char *name;
    size_t name_length;
    uint16_t identifier;
} lt48_defined_name;

typedef struct lt48_pin_code {
    uint8_t pin_count;
    const unsigned char *data;
    size_t data_size;
} lt48_pin_code;

typedef struct lt48_wave_code {
    uint32_t wave_type;
    uint16_t length1;
    uint16_t load_address;
    uint16_t length2;
    const unsigned char *data;
    size_t data_size;
    int has_applet_header;
} lt48_wave_code;

typedef struct lt48_device_spec {
    lt48_catalogue_entry catalogue;
    const char *definition_stem;
    lt48_device_definition_kind definition_kind;
    uint16_t definition_resource;
    uint16_t family_code;
    uint16_t pinmap;
    uint8_t pin_count;
    uint32_t capacity_bytes;
    uint32_t element_count;
    uint16_t element_bits;
    uint32_t secondary_element_count;
    uint16_t secondary_element_bits;
    uint16_t operation_count;
    uint16_t variable_count;
    uint16_t configuration_set;
    uint16_t family_header_value;
} lt48_device_spec;

/* Exact TFamCode operation-to-resource closure for one selected definition.
 * symbol is copied from the _FUN_ namespace of DefineName/TDNCol. It is empty
 * when an older definition has no matching typed name. */
typedef struct lt48_device_operation_binding {
    uint16_t operation_id;
    uint16_t wave;
    uint32_t wave_type;
    uint16_t load_address;
    size_t stored_data_size;
    int has_symbol;
    int has_wave;
    int has_applet_header;
    char symbol[256];
} lt48_device_operation_binding;

typedef enum lt48_adapter_requirement {
    LT48_ADAPTER_UNKNOWN = 0,
    LT48_ADAPTER_POSSIBLY_DIRECT = 1,
    LT48_ADAPTER_REQUIRED = 2
} lt48_adapter_requirement;

typedef struct lt48_device_guidance {
    uint8_t definition_pin_count;
    int has_catalogue_package_hint;
    uint16_t catalogue_package_pin_count;
    char catalogue_package_code[16];
    lt48_adapter_requirement adapter_requirement;
    int has_socket_span;
    uint8_t first_socket_position;
    uint8_t last_socket_position;
    int package_pin_count_conflict;
    int orientation_known;
} lt48_device_guidance;

typedef struct lt48_device_compile_evidence {
    size_t operation_count;
    size_t strict_compact_count;
    size_t resident_noop_default_count;
    size_t unresolved_default_count;
    size_t rejected_compact_count;
    size_t local_resource_count;
} lt48_device_compile_evidence;

/* Clean-room host dispatcher identity recovered for every executable
 * device class. Addresses are provenance labels into the owner-supplied
 * Windows executable; native code never jumps to or executes them. */
typedef struct lt48_host_engine_info {
    uint16_t device_class;
    uint32_t constructor_address;
    uint32_t vtable_address;
    uint32_t primary_entry_address;
    const char *primary_engine;
    const char *semantic_family;
    const char *semantic_status;
    const char *normalized_engine_profile;
} lt48_host_engine_info;

LT48_API lt48_status lt48_host_engine_for_class(
    uint16_t device_class,
    lt48_host_engine_info *out_info
);

typedef struct lt48_installation_audit {
    size_t catalogue_device_count;
    size_t supported_device_count;
    size_t unsupported_class_count;
    size_t definition_file_count;
    size_t resolved_device_count;
    size_t failed_device_count;
    size_t zero_pin_device_count;
    size_t first_failed_device_index;
    lt48_status first_failure;
    unsigned char manifest_sha256[LT48_SHA256_SIZE];
} lt48_installation_audit;

/* Frontend-neutral, USB-free production-core diagnostic results. */
typedef struct lt48_diagnostics_report {
    uint32_t schema_version;
    int passed;
    int sha256_check_passed;
    int operation_surface_check_passed;
    int buffer_model_check_passed;
    int jtag_planner_check_passed;
    int progress_model_check_passed;
    int installation_requested;
    lt48_status installation_status;
    lt48_installation_audit installation_audit;
} lt48_diagnostics_report;

typedef enum lt48_operation_kind {
    LT48_OPERATION_READ = 1,
    LT48_OPERATION_BLANK_CHECK = 2,
    LT48_OPERATION_PROGRAM = 3,
    LT48_OPERATION_OVER_PROGRAM = 4,
    LT48_OPERATION_VERIFY = 5,
    LT48_OPERATION_COMPARE = 6,
    LT48_OPERATION_ERASE = 7,
    LT48_OPERATION_SECURE = 8,
    LT48_OPERATION_MEMORY_PROTECT = 9,
    LT48_OPERATION_CONFIGURATION = 10,
    LT48_OPERATION_FUNCTIONAL_TEST = 11,
    LT48_OPERATION_AUTO_ID = 12,
    LT48_OPERATION_MASS_PRODUCTION = 13,
    LT48_OPERATION_JTAG_ISP = 14
} lt48_operation_kind;

typedef enum lt48_operation_availability {
    LT48_OPERATION_UNAVAILABLE = 0,
    LT48_OPERATION_PLAN_ONLY = 1,
    LT48_OPERATION_EXECUTABLE = 2,
    LT48_OPERATION_HOST_ONLY = 3
} lt48_operation_availability;

typedef struct lt48_operation_support {
    lt48_operation_kind operation;
    const char *name;
    lt48_operation_availability availability;
    const char *support_level;
    const char *reason;
    int mutates_device;
    int definition_callback_present;
    uint16_t definition_wave;
    char definition_callback[256];
} lt48_operation_support;

#define LT48_OPERATION_KIND_COUNT 14u

LT48_API const char *lt48_operation_kind_name(lt48_operation_kind operation);
LT48_API lt48_status lt48_operation_kind_parse(
    const char *name, lt48_operation_kind *out_operation
);

typedef struct lt48_auto_id_observation {
    int has_manufacturer_id;
    uint16_t manufacturer_id;
    int has_device_id;
    uint16_t device_id;
    int has_pin_count_hint;
    uint16_t pin_count_hint;
    int has_device_class_hint;
    uint16_t device_class_hint;
} lt48_auto_id_observation;

typedef struct lt48_auto_id_candidate {
    size_t device_index;
    lt48_catalogue_entry catalogue;
    uint32_t score;
    int manufacturer_id_match;
    int device_id_match;
    int pin_count_match;
    int device_class_match;
    int has_resolved_pin_count;
    uint8_t resolved_pin_count;
} lt48_auto_id_candidate;

#define LT48_AUTO_ID_EPROM_MAX_DATA_PINS 16u
#define LT48_AUTO_ID_EPROM_MAX_ADDRESS_PINS 2u

typedef struct lt48_auto_id_eprom_recipe {
    uint16_t package_pin_count;
    uint16_t operating_vcc_centivolts;
    uint16_t signature_vpp_centivolts;
    uint8_t rail_pin;
    uint8_t power_pin_a;
    uint8_t power_pin_b;
    uint8_t read_enable_pin;
    uint8_t id_select_pin;
    uint8_t fixed_pin;
    uint8_t data_pin_count;
    uint8_t data_capture_codes[LT48_AUTO_ID_EPROM_MAX_DATA_PINS];
    uint8_t address_pattern_pin_count;
    uint8_t address_pattern_pins[LT48_AUTO_ID_EPROM_MAX_ADDRESS_PINS];
} lt48_auto_id_eprom_recipe;

typedef struct lt48_auto_id_socket_observation {
    unsigned char raw[12];
    uint32_t left_contact_mask;
    uint32_t right_contact_mask;
    uint32_t occupied_contact_mask;
    uint16_t first_half_position;
    uint16_t last_half_position;
    uint16_t package_pin_count;
} lt48_auto_id_socket_observation;

typedef struct lt48_auto_id_eprom_plan_info {
    size_t recipe_count;
    size_t eligible_recipe_count;
    int has_explicit_pin_hint;
    uint16_t explicit_pin_hint;
    int probe_always_first;
    int selected_device_independent;
    int cleanup_finalizer_required;
    uint16_t maximum_operating_vcc_centivolts;
    uint16_t maximum_signature_vpp_centivolts;
    unsigned char provider_sha256[LT48_SHA256_SIZE];
} lt48_auto_id_eprom_plan_info;

typedef struct lt48_auto_id_eprom_preparation {
    lt48_auto_id_eprom_plan_info plan;
    uint8_t bus_number;
    uint8_t device_address;
    int acquire_signature;
    unsigned char confirmation_sha256[LT48_SHA256_SIZE];
} lt48_auto_id_eprom_preparation;

typedef struct lt48_auto_id_eprom_execution_result {
    lt48_status operation_status;
    lt48_status finalizer_status;
    int finalizer_attempted;
    int signature_requested;
    int signature_attempted;
    int has_signature;
    int rails_may_be_active;
    int interface_release_attempted;
    int interface_released;
    lt48_status interface_release_status;
    size_t command_transfer_count;
    size_t address_patterns_attempted;
    double operation_duration_ms;
    double finalizer_duration_ms;
    double interface_release_duration_ms;
    double total_duration_ms;
    lt48_auto_id_socket_observation socket;
    lt48_auto_id_eprom_recipe recipe;
    uint16_t manufacturer_id;
    uint16_t device_id;
} lt48_auto_id_eprom_execution_result;

typedef enum lt48_verify_margin_selection {
    LT48_VERIFY_MARGIN_NOMINAL = 0,
    LT48_VERIFY_MARGIN_LOW = 1,
    LT48_VERIFY_MARGIN_HIGH = 2,
    LT48_VERIFY_MARGIN_ALL = 3
} lt48_verify_margin_selection;

typedef struct lt48_verify_voltage_profile {
    uint16_t nominal_centivolts;
    uint16_t low_centivolts;
    uint16_t high_centivolts;
    uint8_t margin_type;
    uint8_t explicit_value_count;
    uint16_t explicit_values_centivolts[4];
} lt48_verify_voltage_profile;

typedef struct lt48_compact_instruction {
    size_t offset;
    uint8_t opcode;
    const unsigned char *operands;
    size_t operand_size;
    uint32_t compiler_handler;
} lt48_compact_instruction;

typedef struct lt48_pin_group {
    uint16_t kind;
    const unsigned char *pins;
    size_t pin_count;
} lt48_pin_group;

typedef struct lt48_logical_pin {
    uint16_t number;
    uint8_t engine_pin;
} lt48_logical_pin;

typedef struct lt48_compile_variable {
    uint16_t kind;
    uint16_t variable_id;
    uint16_t enabled;
    uint16_t value;
    uint16_t channel;
} lt48_compile_variable;

typedef struct lt48_compiled_expansion {
    size_t compact_offset;
    uint8_t compact_opcode;
    uint32_t compiler_handler;
    size_t first_word;
    size_t word_count;
} lt48_compiled_expansion;

typedef enum lt48_pin_fallback_policy {
    LT48_PIN_FALLBACK_STRICT = 0,
    LT48_PIN_FALLBACK_RESIDENT_NOOP = 1,
    LT48_PIN_FALLBACK_ANALYSIS_ALL = 2,
    /* Also permits compact27 zero-pin selection: Q2(2) is a resident no-op;
     * its independently evaluated DAC ramp remains active. Opt-in only. */
    LT48_PIN_FALLBACK_RESIDENT_NOOP_WITH_RAMP = 3,
    /* Exact source compatibility: missing logical21 in compact11/2b.
     * Caller must validate every default site against its pinned source. */
    LT48_PIN_FALLBACK_SOURCE_ZERO_STROBE = 4,
    /* Pinned GBD logical17 supply command, not a guessed ZIF mapping.
     * Caller must validate source digest and every compact default site. */
    LT48_PIN_FALLBACK_SOURCE_ZERO_SUPPLY = 5,
    /* GB PEEL153/173/253/273 INIT offset48: absent logical13. */
    LT48_PIN_FALLBACK_PEEL_SUPPLY = 6,
    /* GAD COP8Cx source reset: absent logical1 at compact offset3. */
    LT48_PIN_FALLBACK_COP_RESET = 7
} lt48_pin_fallback_policy;

typedef enum lt48_defaulted_pin_class {
    LT48_DEFAULTED_PIN_UNRESOLVED = 0,
    LT48_DEFAULTED_PIN_RESIDENT_NOOP = 1,
    LT48_DEFAULTED_PIN_SOURCE_ZERO_STROBE = 2,
    LT48_DEFAULTED_PIN_SOURCE_ZERO_SUPPLY = 3
} lt48_defaulted_pin_class;

typedef struct lt48_defaulted_logical_pin {
    uint16_t logical_pin;
    size_t compact_offset;
    uint8_t compact_opcode;
    lt48_defaulted_pin_class classification;
} lt48_defaulted_logical_pin;

typedef enum lt48_runtime_event_kind {
    LT48_RUNTIME_Q1 = 1,
    LT48_RUNTIME_Q2 = 2,
    LT48_RUNTIME_VOLTAGE = 3,
    LT48_RUNTIME_RAW_WRITE = 4,
    LT48_RUNTIME_CONTROLLER_DELAY = 5,
    LT48_RUNTIME_EXECUTE_WAVE = 6,
    LT48_RUNTIME_EVALUATE_VARIABLE = 7,
    LT48_RUNTIME_BUFFER_ARITHMETIC = 8,
    LT48_RUNTIME_START_ENGINE = 9,
    LT48_RUNTIME_STATUS = 10,
    LT48_RUNTIME_CAPTURE_RESPONSE_BYTE = 11,
    LT48_RUNTIME_CONTROLLER_PACKED_WRITE = 12,
    LT48_RUNTIME_CONTROLLER_PACKED_READ = 13,
    LT48_RUNTIME_BUFFER_BIT_COPY = 14,
    LT48_RUNTIME_BUFFER_MEMMOVE = 15,
    LT48_RUNTIME_BUFFER_BYTE_OPERATION = 16,
    /* Buffered controller writer (+0x44), distinct from bulk RAW_WRITE.
     * Voltage-route support does not itself admit a device operation. */
    LT48_RUNTIME_CONTROLLER_COMMAND_WRITE = 17,
    LT48_RUNTIME_BUFFER_FILL = 18,
    LT48_RUNTIME_CONTROLLER_PIN_READ = 19,
    LT48_RUNTIME_RETURN_STATUS = 20,
    LT48_RUNTIME_RESULT_LATCH = 21,
    LT48_RUNTIME_HOST_BITSORT = 22,
    LT48_RUNTIME_HOST_GAL_PES = 23,
    /* Original procedure entry: milliseconds, global interpreter state.
     * Transport must implement this boundary before hardware admission. */
    LT48_RUNTIME_WAVE_WATCHDOG = 24,
    LT48_RUNTIME_FUSE_IMAGE_READ = 25,
    LT48_RUNTIME_CONTROLLER_PIN_POLL = 26,
    LT48_RUNTIME_RAW_READ = 27,
    /* Audited source notice2/severity2: factory word retained, no decision. */
    LT48_RUNTIME_HOST_NOTIFICATION = 28,
    LT48_RUNTIME_FUSE_SECTION_SELECT = 29,
    LT48_RUNTIME_FUSE_IMAGE_WRITE = 30
} lt48_runtime_event_kind;

typedef struct lt48_runtime_event {
    lt48_runtime_event_kind kind;
    const uint64_t *arguments;
    size_t argument_count;
} lt48_runtime_event;

typedef struct lt48_runtime_buffer_seed {
    uint8_t buffer;
    const unsigned char *data;
    size_t size;
} lt48_runtime_buffer_seed;

typedef struct lt48_gal_pes_binding lt48_gal_pes_binding;
typedef struct lt48_runtime_nested_wave {
    uint16_t operation;
    const lt48_compiled_wave *compiled;
} lt48_runtime_nested_wave;
typedef struct lt48_runtime_embedded_image {
    uint16_t selector;
    const unsigned char *data;
    size_t size;
} lt48_runtime_embedded_image;
/* Serialized original TFuseBuf records: two header bytes and two regions
 * each copy_width bytes long. Runtime6B copies the second region. */
typedef struct lt48_runtime_fuse_image {
    const unsigned char *data;
    size_t size;
    uint32_t stride;
    size_t record_width;
    size_t copy_width;
} lt48_runtime_fuse_image;

typedef struct lt48_fuse_image lt48_fuse_image;
/* Owns a fresh image built from this exact resource and one byte per fuse.
 * Nonzero input values encode as true, matching the original converter. */
/* Original TPofFuseGroup PIA conversion (00438e20/00439200).
 * pia_bits is the selected _VAR_PIAbitCnt; zero preserves direct mapping. */
LT48_API lt48_status lt48_pof_fuse_image_build(const lt48_fuse_code *code,
    const unsigned char *values,size_t count,unsigned pia_bits,lt48_fuse_image **out);
LT48_API lt48_status lt48_pof_fuse_image_decode(const lt48_fuse_code *code,
    const lt48_runtime_fuse_image *image,unsigned char *values,size_t count,unsigned pia_bits);
LT48_API lt48_status lt48_fuse_image_build(const lt48_fuse_code *code,
    const unsigned char *values, size_t value_count, lt48_fuse_image **out_image);
LT48_API const lt48_runtime_fuse_image *lt48_fuse_image_view(
    const lt48_fuse_image *image);
LT48_API void lt48_fuse_image_destroy(lt48_fuse_image *image);
/* Extract boolean values from the second plane. Failure leaves values intact. */
LT48_API lt48_status lt48_fuse_image_decode(const lt48_fuse_code *code,
    const lt48_runtime_fuse_image *image, unsigned char *values, size_t value_count);
typedef lt48_status (*lt48_fuse_row_exchange_fn)(void *context, uint16_t mode,
    uint16_t row, uint16_t column, uint8_t fuse_type,
    unsigned char *expected, const unsigned char *mask,
    size_t width, unsigned char *actual);
typedef struct lt48_fuse_row_result {
    size_t completed_rows;
    size_t exchanges;
    int matched;
    uint16_t mismatch_row;
    uint16_t mismatch_column;
    size_t mismatch_byte;
    unsigned char expected;
    unsigned char actual;
} lt48_fuse_row_result;
/* Original 004599f0 host traversal. Comparison mismatch returns OK with
 * matched=0; exchange/cancellation failure returns its status. Callback
 * binds LOAD/PROGRAM and preserves the shared device frame. No USB here. */
LT48_API lt48_status lt48_fuse_image_run_rows(lt48_fuse_image *image,
    uint16_t mode, unsigned char blank_fill, lt48_fuse_row_exchange_fn exchange,
    int (*cancelled)(void *context), void *context, lt48_fuse_row_result *out_result);
typedef struct lt48_runtime_configuration_value {
    int object_id; /* -1: current operation options; otherwise original object-table index. */
    uint16_t category, field;
    uint32_t value;
} lt48_runtime_configuration_value;
typedef struct lt48_runtime_options {
    uint32_t mode;
    uint32_t address;
    const unsigned char *parameter_buffer;
    size_t parameter_size;
    uint32_t initial_voltage_centivolts[4];
    size_t max_steps;
    const unsigned char *capture_response_bytes;
    size_t capture_response_size;
    int require_capture_responses;
    int use_explicit_buffer_ids;
    uint8_t address_buffer;
    uint8_t parameter_buffer_id;
    uint8_t mode_buffer;
    const lt48_runtime_buffer_seed *initial_buffers;
    size_t initial_buffer_count;
    /* Explicit audited CFUN binding; zero leaves ExecuteWave unchanged. */
    int use_host_bitsort;
    uint16_t host_bitsort_operation;
    uint8_t host_bitsort_buffer;
    /* Explicit family-local callback binding; NULL disables GAL PES. */
    const lt48_gal_pes_binding *host_gal_pes;
    /* Machine replay stops before interpreting beyond unavailable input. */
    int stop_at_missing_capture;
    /* Audited family-local compact callees, borrowed for the whole run.
     * Nonzero count enables procedure boundary events; missing callees remain
     * explicit ExecuteWave events. Duplicate operation IDs are rejected. */
    const lt48_runtime_nested_wave *nested_waves;
    size_t nested_wave_count;
    uint32_t root_timeout_seconds;
    /* NULL reproduces the original absent-host-image no-op. A complete
     * program owner must supply its selected image. Machines snapshot it. */
    const lt48_runtime_fuse_image *fuse_image;
    const lt48_runtime_configuration_value *configuration;
    size_t configuration_count;
    /* Some hosts keep the direct fuse bank separate from short-form options. */
    int use_explicit_direct_configuration_object;
    int direct_configuration_object;
    /* Legacy unnamed getters address Kernel+0x20, a different object from
     * the direct fuse bank. Named object-table indexes are unaffected. */
    int use_explicit_legacy_configuration_object;
    int legacy_configuration_object;
    /* Active byte image, snapshotted by the interpreter/machine. Image writes
     * are returned in the result; never mutate the caller during replay. */
    const unsigned char *device_image;
    size_t device_image_size;
    unsigned device_element_bytes;
    /* Audited GCA D0/D1 host callbacks; cache lives in snapshotted image. */
    int host_epm_legacy;
    /* Exact class-2017 source compatibility: original ramp interval WORD truncation. */
    int host_xc7000_ramp;
    /* Source-verified embedded FPGA images, immutable and borrowed for the
     * entire machine lifetime, like nested_waves. No vendor bytes built in. */
    const lt48_runtime_embedded_image *embedded_images;
    size_t embedded_image_count;
    int host_staged_shutdown; /* 4: source float explicit-voltage conversion plus policy3 shutdown; 1: staged stable order; 2: exact 443A50 pairwise exchanges. */ /* Audited 443a50: lower rails above 5 V before off. */
    int host_avr_parallel; /* Legacy name: audited opcode3 float DAC +443a50 shutdown; AVR/LPC/GAL owners. */
    /* Exact authored MMI PAL owner: defer status while VCC1 is at VIHH.
     * The enclosing pulse must fit one controller queue packet. */
    int host_pal_mmi_atomic;
    /* Exact M37 erase owner: retain source retry notices3/3; owner must
     * inspect final EVERIFY result and independently blank-check the array. */
    int host_m37_erase_notifications;
    /* Exact host-owned fuse sections. Zero reproduces an absent map object. */
    unsigned fuse_section_count;
} lt48_runtime_options;

typedef enum lt48_runtime_machine_state {
    LT48_RUNTIME_MACHINE_READY = 1,
    LT48_RUNTIME_MACHINE_AWAITING_CAPTURE = 2,
    LT48_RUNTIME_MACHINE_FINISHED = 3
} lt48_runtime_machine_state;

typedef enum lt48_transfer_direction {
    LT48_TRANSFER_OUT = 1,
    LT48_TRANSFER_IN = 2
} lt48_transfer_direction;

typedef struct lt48_protocol_transaction {
    lt48_transfer_direction direction;
    uint8_t endpoint;
    const unsigned char *data;
    size_t length;
    int captures_runtime_byte;
    uint8_t runtime_buffer;
    size_t response_offset;
    size_t runtime_capture_length;
    int validates_controller_response;
    uint8_t request_opcode;
    int reads_device_data;
    uint32_t device_data_offset;
    size_t device_data_length;
} lt48_protocol_transaction;

typedef struct lt48_applet_address {
    uint16_t wave;
    uint16_t address;
} lt48_applet_address;

typedef enum lt48_execution_stage {
    LT48_EXECUTION_IDLE = 0,
    LT48_EXECUTION_READ_OPERATION = 1,
    LT48_EXECUTION_READ_RECOVERY = 2,
    LT48_EXECUTION_INTERFACE_RELEASE = 3,
    LT48_EXECUTION_COMPLETE = 4,
    LT48_EXECUTION_AUTO_ID_OPERATION = 5,
    LT48_EXECUTION_AUTO_ID_FINALIZATION = 6
} lt48_execution_stage;

typedef struct lt48_execution_progress {
    lt48_execution_stage stage;
    size_t completed_transactions;
    size_t total_transactions;
    unsigned cleanup_phase;
    size_t cleanup_phase_index;
    size_t cleanup_phase_count;
} lt48_execution_progress;

typedef enum lt48_progress_scope {
    LT48_PROGRESS_OPERATION = 1,
    LT48_PROGRESS_SESSION = 2,
    LT48_PROGRESS_PHASE = 3,
    LT48_PROGRESS_UNITS = 4
} lt48_progress_scope;

typedef enum lt48_progress_state_kind {
    LT48_PROGRESS_STARTED = 1,
    LT48_PROGRESS_SUCCEEDED = 2,
    LT48_PROGRESS_FAILED = 3,
    LT48_PROGRESS_CANCELLED = 4,
    LT48_PROGRESS_SAFETY_REJECTED = 5,
    LT48_PROGRESS_RUNNING = 6
} lt48_progress_state_kind;

typedef struct lt48_progress_event_input {
    lt48_progress_scope scope;
    const char *name;
    lt48_progress_state_kind state;
    const char *category;
    int has_units;
    size_t completed_units;
    size_t total_units;
    const char *unit;
    const char *error_type;
    const char *error_message;
} lt48_progress_event_input;

typedef struct lt48_progress_event {
    uint32_t schema_version;
    uint64_t sequence;
    lt48_progress_event_input value;
} lt48_progress_event;

typedef lt48_status (*lt48_progress_observer_fn)(
    void *context, const lt48_progress_event *event
);

typedef struct lt48_progress_emitter lt48_progress_emitter;

#define LT48_PROGRESS_NAME_CAPACITY 96u
#define LT48_PROGRESS_UNIT_CAPACITY 32u
#define LT48_PROGRESS_ERROR_CAPACITY 192u

typedef struct lt48_progress_state {
    int has_event;
    uint64_t last_sequence;
    lt48_progress_state_kind operation_state;
    char current_phase[LT48_PROGRESS_NAME_CAPACITY];
    int has_units;
    size_t completed_units;
    size_t total_units;
    char unit[LT48_PROGRESS_UNIT_CAPACITY];
    int has_error;
    char error_type[LT48_PROGRESS_NAME_CAPACITY];
    char error_message[LT48_PROGRESS_ERROR_CAPACITY];
} lt48_progress_state;

typedef void (*lt48_transport_progress_fn)(
    void *context, lt48_execution_stage stage,
    size_t completed_transactions, size_t total_transactions,
    unsigned cleanup_phase, size_t cleanup_phase_index,
    size_t cleanup_phase_count
);

typedef struct lt48_protocol_transport {
    lt48_status (*write)(
        void *context, uint8_t endpoint, const unsigned char *data,
        size_t length, size_t *out_transferred
    );
    lt48_status (*read)(
        void *context, uint8_t endpoint, unsigned char *data,
        size_t length, size_t *out_transferred
    );
    /* Optional. Checked only by forward execution before each transfer. */
    int (*cancelled)(void *context);
    /* Optional internal state sink; frontends must poll programmer progress. */
    lt48_transport_progress_fn progress;
    lt48_execution_stage progress_stage;
} lt48_protocol_transport;

typedef struct lt48_runtime_capture {
    uint8_t buffer;
    uint8_t value;
    unsigned char values[32];
    size_t length;
} lt48_runtime_capture;

typedef lt48_status (*lt48_cleanup_action_fn)(void *context);

typedef struct lt48_cleanup_action {
    const char *name;
    lt48_cleanup_action_fn run;
    void *context;
} lt48_cleanup_action;

typedef struct lt48_cleanup_result {
    const char *name;
    lt48_status status;
    double started_offset_ms;
    double duration_ms;
} lt48_cleanup_result;

typedef lt48_status (*lt48_session_operation_fn)(void *context);

typedef struct lt48_session_result {
    lt48_status operation_status;
    lt48_status cleanup_status;
    size_t cleanup_failure_count;
} lt48_session_result;

typedef struct lt48_m37_read_execution_result {
    lt48_status operation_status;
    lt48_status recovery_status;
    size_t recovery_failure_count;
    size_t completed_transactions;
} lt48_m37_read_execution_result;

typedef struct lt48_m37_recovery_execution_result {
    lt48_status recovery_status;
    lt48_status first_failure_status;
    size_t phase_count;
    size_t failure_count;
    size_t completed_transactions;
} lt48_m37_recovery_execution_result;

typedef enum lt48_m37_operation {
    LT48_M37_READ = 1,
    LT48_M37_BLANK_CHECK = 2,
    LT48_M37_VERIFY = 3,
    LT48_M37_PROGRAM = 4,
    LT48_M37_COMPARE = 5,
    LT48_M37_ELECTRICAL_ERASE = 6
} lt48_m37_operation;

typedef enum lt48_m37_compare_decision {
    LT48_M37_COMPARE_CONTINUE = 1,
    LT48_M37_COMPARE_ABORT = 2
} lt48_m37_compare_decision;

typedef lt48_m37_compare_decision (*lt48_m37_compare_decision_fn)(
    void *context, uint32_t address, uint8_t expected, uint8_t actual
);

typedef struct lt48_m37_compare_result {
    size_t compared_size;
    size_t mismatch_count;
    int matched;
    int aborted;
    uint32_t first_mismatch_address;
    uint8_t first_expected;
    uint8_t first_actual;
    uint32_t last_mismatch_address;
    uint8_t last_expected;
    uint8_t last_actual;
} lt48_m37_compare_result;

typedef enum lt48_m37_precheck {
    LT48_M37_PRECHECK_NONE = 0,
    LT48_M37_PRECHECK_BLANK = 1,
    LT48_M37_PRECHECK_ONE_TO_ZERO = 2
} lt48_m37_precheck;

typedef enum lt48_m37_recovery_state {
    LT48_M37_RECOVERY_UNOPENED = 0,
    LT48_M37_RECOVERY_INITIALIZED = 1,
    LT48_M37_RECOVERY_POWERED = 2
} lt48_m37_recovery_state;

typedef struct lt48_m37_snapshot_read_execution_result {
    lt48_status operation_status;
    lt48_status finalization_status;
    lt48_m37_recovery_state recovery_state;
    int used_recovery_plan;
    size_t operation_completed_transactions;
    size_t recovery_completed_transactions;
    size_t finalization_phase_count;
    size_t finalization_failure_count;
    double operation_duration_ms;
    double total_duration_ms;
} lt48_m37_snapshot_read_execution_result;

typedef struct lt48_m37_compare_execution_result {
    lt48_m37_snapshot_read_execution_result execution;
    lt48_m37_compare_result comparison;
    int comparison_available;
} lt48_m37_compare_execution_result;

typedef struct lt48_m37_read_preparation {
    size_t transaction_count;
    size_t read_chunk_count;
    size_t read_byte_count;
    size_t recovery_transaction_count[3];
    unsigned char definition_sha256[LT48_SHA256_SIZE];
    unsigned char resource_manifest_sha256[LT48_SHA256_SIZE];
    unsigned char plan_sha256[LT48_SHA256_SIZE];
    unsigned char recovery_sha256[3][LT48_SHA256_SIZE];
    unsigned char confirmation_sha256[LT48_SHA256_SIZE];
} lt48_m37_read_preparation;

typedef enum lt48_m37_cleanup_phase {
    LT48_M37_CLEANUP_DEVICE_RESET = 1,
    LT48_M37_CLEANUP_SETTLE_DELAY = 2,
    LT48_M37_CLEANUP_SETTLE_STATUS = 3,
    LT48_M37_CLEANUP_POWER_OFF = 4,
    LT48_M37_CLEANUP_POWER_OFF_FLUSH = 5,
    LT48_M37_CLEANUP_ZERO_ALL_RAILS = 6,
    LT48_M37_CLEANUP_RESTORE_IDLE_HEADER = 7,
    LT48_M37_CLEANUP_RESTORE_IDLE_DATA = 8,
    LT48_M37_CLEANUP_FINALIZE_CONTROLLER = 9,
    LT48_M37_CLEANUP_FINALIZE_FLUSH = 10,
    LT48_M37_CLEANUP_SET_RESULT = 11,
    LT48_M37_CLEANUP_RELEASE_INTERFACE = 12
} lt48_m37_cleanup_phase;

enum {
    LT48_M37_SAFETY_ENGINE = UINT64_C(1) << 0,
    LT48_M37_SAFETY_OPERATION = UINT64_C(1) << 1,
    LT48_M37_SAFETY_ADDRESS_RANGE = UINT64_C(1) << 2,
    LT48_M37_SAFETY_CHUNK = UINT64_C(1) << 3,
    LT48_M37_SAFETY_VCC = UINT64_C(1) << 4,
    LT48_M37_SAFETY_PIN_MAPPING = UINT64_C(1) << 5,
    LT48_M37_SAFETY_APPLETS = UINT64_C(1) << 6,
    LT48_M37_SAFETY_FPGA = UINT64_C(1) << 7,
    LT48_M37_SAFETY_CLEANUP = UINT64_C(1) << 8,
    LT48_M37_SAFETY_IMAGE = UINT64_C(1) << 9,
    LT48_M37_SAFETY_PRECHECK = UINT64_C(1) << 10,
    LT48_M37_SAFETY_VERIFICATION = UINT64_C(1) << 11,
    LT48_M37_SAFETY_DEVICE_IDENTITY = UINT64_C(1) << 12
};

typedef struct lt48_m37_plan_evidence {
    uint16_t device_class;
    uint16_t family_code;
    uint8_t pin_count;
    uint32_t capacity_bytes;
    lt48_m37_operation operation;
    uint32_t start;
    uint32_t length;
    uint16_t chunk_size;
    uint16_t nominal_vcc_centivolts;
    uint8_t vcc_selector;
    int strict_pin_mapping;
    int all_applets_hash_bound;
    int applet_ranges_nonoverlapping;
    int active_fpga_hash_bound;
    const lt48_m37_cleanup_phase *cleanup_phases;
    size_t cleanup_phase_count;
    int has_image_digest;
    uint32_t image_size;
    lt48_m37_precheck precheck;
    int mandatory_post_verify;
    /* Retained for ABI compatibility. Identification evidence does not gate
     * selected-device EPROM operations; these three fields are ignored. */
    int selected_device_check_planned;
    int selected_device_verified;
    int mismatch_override_for_this_operation;
} lt48_m37_plan_evidence;

/*
 * Immutable facts derived directly from one installed device definition.
 * The two digests bind both the complete REZ file and the exact selected
 * family-resource manifest used by the audit.  Boolean fields are results of
 * parsing/compilation checks, not caller assertions.
 */
typedef struct lt48_m37_resource_audit {
    uint16_t device_class;
    uint16_t device_type;
    uint16_t family_code;
    uint16_t pinmap;
    uint8_t pin_count;
    uint32_t capacity_bytes;
    uint16_t operation_count;
    uint16_t variable_count;
    uint16_t nominal_vcc_centivolts;
    uint8_t expected_vcc_selector;
    uint16_t compact_wave_count;
    uint16_t applet_count;
    uint16_t active_fpga_count;
    int exact_engine_envelope;
    int strict_pin_mapping;
    int required_compact_waves_strict;
    int applet_metadata_complete;
    int applet_ranges_nonoverlapping;
    int active_fpga_exactly_one;
    int read_behavior_approved;
    /* Complete Dataman Windows success-path plus native cleanup replay. */
    int read_original_replay_verified;
    unsigned char definition_sha256[LT48_SHA256_SIZE];
    unsigned char resource_manifest_sha256[LT48_SHA256_SIZE];
    /* Complete forward Read capture match; does not approve live recovery. */
    int read_forward_reference_verified;
    /* Exact source-bound 2764/27128 procedure equivalence; not new hardware evidence. */
    int complete_group_equivalence_verified;
    /* Complete source audit; physical qualification is recorded separately. */
    int complete_source_audit_verified;
} lt48_m37_resource_audit;

typedef struct lt48_m37_applet_resource {
    uint16_t operation_id;
    uint16_t wave;
    uint16_t load_address;
    const unsigned char *data;
    size_t data_size;
} lt48_m37_applet_resource;

LT48_API const char *lt48_version_string(void);
LT48_API const char *lt48_status_string(lt48_status status);
LT48_API int lt48_is_supported_usb_id(uint16_t vendor_id, uint16_t product_id);
/*
 * Describe only the role implied by the USB product ID.  In particular,
 * 0x2004 does not prove whether the volatile FX2 firmware is running.
 */
LT48_API const char *lt48_programmer_descriptor_role(uint16_t product_id);

/*
 * Select a descriptor without opening USB.  With no exact target, precisely
 * one item is required.  An empty/missing target returns LT48_NOT_FOUND and
 * an ambiguous unqualified selection returns LT48_BUSY.
 */
LT48_API lt48_status lt48_programmer_describe(
    uint8_t bus_number, uint8_t device_address,
    char *model, size_t model_size, char *serial, size_t serial_size,
    uint16_t *out_product_id, uint16_t *out_device_version_bcd
);
LT48_API lt48_status lt48_programmer_info_select(
    const lt48_programmer_info *infos, size_t count, int has_exact_target,
    uint8_t bus_number, uint8_t device_address,
    lt48_programmer_info *out_info
);
LT48_API lt48_status lt48_fx2_firmware_open_hex(
    const char *path, lt48_fx2_firmware **out_firmware
);
LT48_API void lt48_fx2_firmware_close(lt48_fx2_firmware *firmware);
LT48_API lt48_status lt48_fx2_firmware_info_get(
    const lt48_fx2_firmware *firmware, lt48_fx2_firmware_info *out_info
);
LT48_API lt48_status lt48_fx2_prepare_load(
    const lt48_fx2_firmware *firmware, uint8_t bus_number,
    uint8_t device_address, lt48_fx2_load_preparation *out_preparation
);
LT48_API lt48_status lt48_programmer_load_fx2(
    lt48_context *context, uint8_t bus_number, uint8_t device_address,
    const lt48_fx2_firmware *firmware, uint32_t authorization,
    lt48_fx2_load_result *out_result
);

LT48_API lt48_status lt48_context_create(lt48_context **out_context);
LT48_API void lt48_context_destroy(lt48_context *context);

/*
 * Descriptor-only discovery: this does not open, claim or transmit to a USB
 * device. Set infos to NULL and capacity to zero to query the required count.
 */
LT48_API lt48_status lt48_programmer_list(
    lt48_context *context,
    lt48_programmer_info *infos,
    size_t capacity,
    size_t *out_count
);

/*
 * Open one exact enumerated device, claim interface 0 and select alternate
 * setting 1. This establishes session ownership but sends no device command or
 * payload. A successful handle must be released with lt48_programmer_close().
 */
LT48_API lt48_status lt48_programmer_open(
    lt48_context *context,
    uint8_t bus_number,
    uint8_t device_address,
    lt48_programmer **out_programmer
);
LT48_API void lt48_programmer_close(lt48_programmer *programmer);

/*
 * Perform only the two proved 4.20 transport initialization exchanges. The
 * legacy standalone 0x1234 synchronization write and ready query are omitted:
 * both are known to disrupt this genuine unit. No socket, rail, applet or FPGA
 * operation is issued.
 */
/* Front-panel LEDs only: bit0 Busy, bit1 Error, bit2 Good; zero clears.
 * Does not load applets/FPGA, alter EEPROMs or energize socket supplies. */
LT48_API lt48_status lt48_programmer_set_indicators(
    lt48_programmer *programmer, unsigned mask, unsigned timeout_ms);

LT48_API lt48_status lt48_programmer_transport_initialize(
    lt48_programmer *programmer,
    unsigned timeout_ms,
    lt48_transport_info *out_info
);

/*
 * One-shot native hardware gate. Arming performs no USB I/O and binds the
 * claimed programmer to the exact immutable M37 resource manifest. The
 * authorization constant makes socket-energizing intent explicit at the call
 * site. Execution consumes the arm before transfer zero.
 */
LT48_API lt48_status lt48_programmer_arm_m37_read(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    uint32_t authorization
);
LT48_API lt48_status lt48_programmer_arm_m37_operation(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *plan,
    lt48_m37_operation operation,
    uint32_t authorization
);
/*
 * One-shot Auto Select EPROM owner gate. The preparation must match the exact
 * opened target and the same claimed handle must already have passed transport
 * initialization. Arming performs no USB transfer.
 */
LT48_API lt48_status lt48_programmer_arm_auto_id_eprom(
    lt48_programmer *programmer,
    const lt48_auto_id_eprom_preparation *preparation,
    uint32_t authorization
);
LT48_API lt48_status lt48_programmer_execute_auto_id_eprom(
    lt48_programmer *programmer,
    const lt48_auto_id_eprom_preparation *preparation,
    unsigned timeout_ms,
    lt48_auto_id_eprom_execution_result *out_result
);
LT48_API lt48_status lt48_programmer_set_cancel_check(
    lt48_programmer *programmer, lt48_cancel_check_fn check, void *context
);
typedef enum lt48_m37_id_choice {
    LT48_M37_ID_ABORT = 0, LT48_M37_ID_RETRY = 1, LT48_M37_ID_IGNORE = 2
} lt48_m37_id_choice;
typedef lt48_m37_id_choice (*lt48_m37_id_decision_fn)(
    void *context, uint16_t expected_manufacturer, uint16_t expected_device,
    uint16_t actual_manufacturer, uint16_t actual_device);
LT48_API lt48_status lt48_programmer_set_m37_id_decision(
    lt48_programmer *programmer, lt48_m37_id_decision_fn decide, void *context);
/* Thread-safe snapshot; polling cannot delay execution or mandatory cleanup. */
/* Nonblocking snapshot: LT48_BUSY means an update is in progress; retry later. */
LT48_API lt48_status lt48_programmer_get_progress(
    const lt48_programmer *programmer, lt48_execution_progress *out_progress
);
LT48_API lt48_status lt48_programmer_execute_m37_read(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *read_plan,
    lt48_m37_read_collector *collector,
    unsigned timeout_ms,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_snapshot_read_execution_result *out_result
);
LT48_API lt48_status lt48_programmer_execute_m37_operation(
    lt48_programmer *programmer,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *plan,
    lt48_m37_operation operation,
    lt48_m37_read_collector *collector,
    unsigned timeout_ms,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_snapshot_read_execution_result *out_result
);
/* Execute an admitted PALCE/GAL wave owner through an initialized native
 * programmer transport.  The resource object remains digest-locked by its
 * family owner; this wrapper adds USB ownership, cancellation, progress and
 * unconditional interface release. */
LT48_API lt48_status lt48_programmer_execute_palce_operation(
    lt48_programmer *programmer,
    const lt48_palce_resources *resources,
    lt48_operation_kind operation,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    unsigned timeout_ms,
    lt48_palce_operation_result *out_result
);

/* Bounds-checked, read-only FBPR/REZ container access. */
/* Offline catalogue evaluation only. Within a paired begin/end on one thread,
 * REZ opens share immutable parsed bytes and hashes until the outermost end.
 * Later scopes read fresh files. Handles remain valid after end until closed.
 * Never wrap hardware planning/execution: those must validate current files. */
LT48_API void lt48_rez_snapshot_begin(void);
LT48_API void lt48_rez_snapshot_end(void);

LT48_API lt48_status lt48_rez_open(const char *path, lt48_rez **out_rez);
LT48_API void lt48_rez_close(lt48_rez *rez);
LT48_API lt48_status lt48_rez_get_kernel_buffer_count(
    const lt48_rez *rez, size_t index, uint16_t *out_count
);
LT48_API lt48_status lt48_rez_get_kernel_configuration_value(
    const lt48_rez *rez, size_t index, unsigned category, unsigned field,
    uint32_t *out_value
);
LT48_API uint8_t lt48_palce_resources_buffer_count(const lt48_palce_resources *resources);

/* Source-bound GAL resource closure.  This loads one GBA family/pin-map
 * binding, compiles its declared compact waves and resolves the package-local
 * PES callback.  It is an offline resource owner only; it does not open USB
 * or authorize a GAL operation. */
LT48_API lt48_status lt48_gal_resources_open(
    const char *definition_path, uint16_t family_code, uint16_t pinmap,
    lt48_gal_resources **out_resources);
/* Same closure with the selected Dxx definition checked as well.  A value of
 * UINT16_MAX means that the caller has no definition-resource identity and
 * retains the family/pin-only compatibility behavior of the legacy entry. */
LT48_API lt48_status lt48_gal_resources_open_definition(
    const char *definition_path, uint16_t definition_resource,
    uint16_t family_code, uint16_t pinmap,
    lt48_gal_resources **out_resources);
LT48_API void lt48_gal_resources_close(lt48_gal_resources *resources);
LT48_API int lt48_gal_resources_live_qualified(const lt48_gal_resources *resources);
LT48_API const char *lt48_gal_resources_source_version(
    const lt48_gal_resources *resources);
LT48_API lt48_status lt48_gal_resources_frame_profile(
    const lt48_gal_resources *resources,
    lt48_runtime_frame_profile *out_profile);
LT48_API lt48_status lt48_gal_resources_pes_binding(
    const lt48_gal_resources *resources, lt48_gal_pes_binding *out_binding);
LT48_API lt48_status lt48_gal_resources_compiled_wave(
    const lt48_gal_resources *resources, uint16_t operation_id,
    uint16_t *out_wave, const lt48_compiled_wave **out_compiled);
LT48_API const lt48_compile_variable *lt48_gal_resources_variables(
    const lt48_gal_resources *resources, size_t *out_count);
LT48_API uint16_t lt48_gal_resources_family_code(
    const lt48_gal_resources *resources);
LT48_API uint16_t lt48_gal_resources_pinmap(
    const lt48_gal_resources *resources);
LT48_API uint8_t lt48_gal_resources_buffer_count(
    const lt48_gal_resources *resources);
LT48_API uint32_t lt48_gal_resources_reset_buffer_mask(
    const lt48_gal_resources *resources);
LT48_API lt48_status lt48_gal_resources_buffer(
    const lt48_gal_resources *resources, const char *name, uint8_t *out_id);
LT48_API const lt48_fuse_coordinate *lt48_gal_resources_coordinates(
    const lt48_gal_resources *resources, size_t *out_count);
LT48_API lt48_status lt48_gal_resources_create_machine(
    const lt48_gal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine);
LT48_API lt48_status lt48_gal_resources_create_machine_with_variables(
    const lt48_gal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_compile_variable *variables,
    size_t variable_count, const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine);
/* Source-bound GBB PAL closure: compact waves and host pin-control tables.
 * Other table formats remain explicit unresolved runtime dependencies. */
LT48_API lt48_status lt48_pal_resources_open_definition(
    const char *definition_path, uint16_t definition_resource,
    uint16_t family_code, uint16_t pinmap,
    lt48_pal_resources **out_resources);
LT48_API void lt48_pal_resources_close(lt48_pal_resources *resources);
/* XC9500 Secure choices: bit 0 read security, bit 1 write protection.
 * XC9536XL accepts only bit 0. Default is read security; Program does not
 * apply protection implicitly. Applies only to digest-bound XC95 resources. */
LT48_API lt48_status lt48_pal_resources_set_xc95_protection(
    lt48_pal_resources *resources, unsigned mask);

LT48_API const char *lt48_pal_resources_source_version(
    const lt48_pal_resources *resources);
LT48_API lt48_status lt48_pal_resources_frame_profile(
    const lt48_pal_resources *resources,
    lt48_runtime_frame_profile *out_profile);
/* Resolve a source-defined _FUN_ role; numeric identifiers vary by class. */
LT48_API lt48_status lt48_pal_resources_operation(
    const lt48_pal_resources *resources, const char *role, uint16_t *operation_id);
/* JEDEC delayed-Secure request marker; not physical protection readback. */
LT48_API lt48_status lt48_pal_resources_security_marker(
    const lt48_pal_resources *resources, size_t *fuse_index);
LT48_API int lt48_pal_resources_has_operation(
    const lt48_pal_resources *resources, uint16_t operation_id);
LT48_API lt48_status lt48_pal_resources_compiled_wave(
    const lt48_pal_resources *resources, uint16_t operation_id,
    uint16_t *out_wave, const lt48_compiled_wave **out_compiled);
LT48_API const lt48_compile_variable *lt48_pal_resources_variables(
    const lt48_pal_resources *resources, size_t *out_count);
LT48_API uint16_t lt48_pal_resources_family_code(
    const lt48_pal_resources *resources);
LT48_API uint16_t lt48_pal_resources_pinmap(
    const lt48_pal_resources *resources);
LT48_API uint8_t lt48_pal_resources_buffer_count(
    const lt48_pal_resources *resources);
LT48_API uint32_t lt48_pal_resources_reset_buffer_mask(
    const lt48_pal_resources *resources);
LT48_API lt48_status lt48_pal_resources_buffer(
    const lt48_pal_resources *resources, const char *name, uint8_t *out_id);
LT48_API const lt48_fuse_coordinate *lt48_pal_resources_coordinates(
    const lt48_pal_resources *resources, size_t *out_count);
LT48_API lt48_status lt48_pal_resources_create_machine(
    const lt48_pal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine);
LT48_API lt48_status lt48_pal_resources_create_machine_with_variables(
    const lt48_pal_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_compile_variable *variables,
    size_t variable_count, const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine);
LT48_API uint8_t lt48_pal_resources_blank_fuse(const lt48_pal_resources *resources);
LT48_API lt48_status lt48_rez_get_info(const lt48_rez *rez, lt48_rez_info *out_info);
/* Digest the exact immutable bytes owned by an open REZ object. */
LT48_API lt48_status lt48_rez_sha256(
    const lt48_rez *rez, unsigned char digest[LT48_SHA256_SIZE]
);
LT48_API lt48_status lt48_rez_get_resource(
    const lt48_rez *rez,
    size_t index,
    lt48_rez_resource *out_resource
);
LT48_API lt48_status lt48_rez_find_resource(
    const lt48_rez *rez,
    const char *name,
    size_t *out_index
);
/* Return LT48_NOT_FOUND when the indexed atom is not a device definition. */
LT48_API lt48_status lt48_rez_get_device_definition(
    const lt48_rez *rez,
    size_t index,
    lt48_device_definition *out_definition
);
/* Decode the TFuseCode identifiers serialized by a TLogDev definition. */
LT48_API lt48_status lt48_rez_get_logic_fuse_codes(
    const lt48_rez *rez, size_t definition_index,
    lt48_logic_fuse_codes *out_codes
);
/* Class-2026/2028 POF layout; borrowed data remain valid while REZ is open.
 * total_bytes is the operator image size, not a JEDEC fuse count. */
typedef struct lt48_pof_core {
    const unsigned char *units;
    size_t units_size;
    uint32_t unit_count;
    uint32_t total_bytes;
    uint16_t format;
} lt48_pof_core;
typedef struct lt48_pof_core_unit {
    uint16_t tag;
    uint32_t value;
    const unsigned char *payload;
    size_t payload_size;
} lt48_pof_core_unit;
LT48_API lt48_status lt48_rez_get_pof_core(const lt48_rez *rez,
    size_t definition_index, lt48_pof_core *out_core);
LT48_API lt48_status lt48_pof_core_get_unit(const lt48_pof_core *core,
    size_t index, lt48_pof_core_unit *out_unit);

/* Selected class-2027 POF core. Payloads remain owned by the open REZ. */
typedef struct lt48_pof_fuse_core {
    const unsigned char *device_name;
    size_t device_name_size;
    unsigned char header[12];
    uint32_t packed_size, fuse_count;
    uint16_t fuse_identifier;
} lt48_pof_fuse_core;
LT48_API lt48_status lt48_rez_get_pof_fuse_core(const lt48_rez *rez,
    size_t definition_index, lt48_pof_fuse_core *core);
LT48_API lt48_status lt48_pof_fuse_import(const lt48_pof_fuse_core *core,
    const unsigned char *data, size_t size, unsigned char *fuses, size_t count,
    int *security);
LT48_API lt48_status lt48_pof_fuse_export(const lt48_pof_fuse_core *core,
    const unsigned char *fuses, size_t count, int security,
    unsigned char *data, size_t capacity, size_t *size);
/* Decode one TFuseCode resource. Bytecode remains owned by rez. */
LT48_API lt48_status lt48_rez_get_fuse_code(
    const lt48_rez *rez, size_t resource_index,
    lt48_fuse_code *out_code
);
/* Validate and summarize the bounded iterator tree encoded by TFuseCode. */
LT48_API lt48_status lt48_fuse_layout_analyze(
    const lt48_fuse_code *code, lt48_fuse_layout *out_layout
);
/* Expand the exact dialog-139 row map. Passing rows=NULL queries row_count. */
LT48_API lt48_status lt48_fuse_layout_expand_rows(
    const lt48_fuse_code *code, lt48_fuse_row *rows, size_t row_capacity,
    size_t *out_row_count
);
/* Physical coordinates in serialized fuse order, recovered from synchronized
 * T/P/I/O streams. Full expansion rejects out-of-range and aliased positions.
 * coordinates=NULL with capacity=0 queries the required count. */
LT48_API lt48_status lt48_fuse_layout_expand_coordinates(
    const lt48_fuse_code *code, lt48_fuse_coordinate *coordinates,
    size_t capacity, size_t *out_count
);
/* Native PALCE16V8 D0 resource closure for the audited AEC 4.20 and
 * Dataman 13.10 definition packages. No USB I/O or hardware admission. */
LT48_API lt48_status lt48_palce_resources_open(
    const char *definition_path, lt48_palce_resources **out_resources
);
LT48_API lt48_status lt48_palce_resources_open_definition(
    const char *definition_path, uint16_t definition_resource,
    lt48_palce_resources **out_resources);
LT48_API uint16_t lt48_palce_resources_definition(const lt48_palce_resources *resources);
LT48_API void lt48_palce_resources_close(lt48_palce_resources *resources);
/* Kernel-declared buffers minus compiler reset exemptions. */
LT48_API uint32_t lt48_palce_resources_reset_buffer_mask(const lt48_palce_resources *resources);
LT48_API const char *lt48_palce_resources_source_version(
    const lt48_palce_resources *resources
);
LT48_API const lt48_fuse_coordinate *lt48_palce_resources_coordinates(
    const lt48_palce_resources *resources, size_t *out_count
);
LT48_API lt48_status lt48_palce_resources_buffer(
    const lt48_palce_resources *resources, const char *name, uint8_t *out_id
);
/* Close all machines before their resources; modes 1/2/3/4/10/11 only.
 * Secure is unavailable. Seed buffers are copied by the machine. */
LT48_API lt48_status lt48_palce_resources_create_machine(
    const lt48_palce_resources *resources, uint16_t operation, uint32_t mode,
    const uint32_t initial_voltage[4], const lt48_runtime_buffer_seed *seeds,
    size_t seed_count, lt48_runtime_machine **out_machine
);
/* Parse/export the JEDEC fuse records used by the original logic workflow.
 * Fuse values are one byte per fuse: zero is blown and one is intact. */
LT48_API lt48_status lt48_jedec_import(
    const unsigned char *data, size_t data_size,
    unsigned char *fuses, size_t fuse_count, lt48_jedec_info *out_info
);
LT48_API lt48_status lt48_jedec_export(
    const unsigned char *fuses, size_t fuse_count,
    const lt48_fuse_row *rows, size_t row_count,
    unsigned char *output, size_t output_capacity, size_t *out_size,
    lt48_jedec_info *out_info
);
/* Preserve a JEDEC G flag in file output. This does not authorize or
 * perform the separate hardware Secure operation. */
LT48_API lt48_status lt48_jedec_export_with_security(
    const unsigned char *fuses, size_t fuse_count,
    const lt48_fuse_row *rows, size_t row_count, int security_enabled,
    unsigned char *output, size_t output_capacity, size_t *out_size,
    lt48_jedec_info *out_info
);
/* Access one TDevPool entry. device_name remains owned by rez and is not NUL-terminated. */
LT48_API lt48_status lt48_rez_get_catalogue_entry(
    const lt48_rez *rez,
    size_t resource_index,
    size_t device_index,
    lt48_catalogue_entry *out_entry,
    size_t *out_device_count
);
/* Return the installed REZ filename stem for a catalogue class, or NULL. */
LT48_API const char *lt48_definition_stem_for_class(uint16_t device_class);
/* Select the class table matching a 10-column legacy or 15-column catalogue. */
LT48_API const char *lt48_definition_stem_for_class_format(
    uint16_t device_class, size_t numeric_column_count
);

/* Indexed, read-only view of an owned LabTool installation directory. */
LT48_API lt48_status lt48_installation_open(
    const char *path, lt48_installation **out_installation
);

/* NewDeviceLib: authored definitions/resources, independent of vendor imports.
 * Validation/storage never grants executable support or accesses USB. */
typedef struct lt48_new_device_lib lt48_new_device_lib;
typedef struct lt48_new_device_record {
    char id[128], version[48], kind[24], status[32];
    char manufacturer[128], part[160], base_key[288], base_file[32], base_sha256[65];
    uint32_t base_class, base_definition, base_family, base_pinmap, base_pins, base_capacity;
    uint32_t override_mask, device_type, definition, pinmap, pins, capacity, manufacturer_id, device_id;
    size_t variable_count;
    struct { uint16_t id, kind; uint32_t expected, value; } variables[32];
    char source_path[512], source_sha256[65];
} lt48_new_device_record;
typedef struct lt48_new_device_lib_report {
    size_t records, resources, bundles;
    char root[4096], sha256[65], diagnostic[512];
} lt48_new_device_lib_report;
LT48_API lt48_status lt48_new_device_lib_open(const char *root,
    lt48_new_device_lib **out, lt48_new_device_lib_report *report);
LT48_API void lt48_new_device_lib_close(lt48_new_device_lib *library);
LT48_API size_t lt48_new_device_lib_count(const lt48_new_device_lib *library);
LT48_API const lt48_new_device_record *lt48_new_device_lib_record(
    const lt48_new_device_lib *library, size_t index);
LT48_API lt48_status lt48_new_device_lib_default_path(char *path, size_t capacity);
LT48_API lt48_status lt48_new_device_lib_read_resource(const lt48_new_device_lib *library,
    const char *id, const char *kind, const char *abi, unsigned char **data, size_t *size);
LT48_API const char *lt48_installation_new_device_lib_path(const lt48_installation *installation);
LT48_API const char *lt48_installation_new_device_lib_sha256(const lt48_installation *installation);
LT48_API const lt48_new_device_record *lt48_installation_new_device_record(
    const lt48_installation *installation, size_t device);
LT48_API lt48_status lt48_installation_apply_new_device_variables(
    const lt48_installation *installation, size_t device,
    lt48_compile_variable *variables, size_t count);
/*
 * Import an owned installation into a private, content-addressed directory.
 * The destination must not exist. Publication is by a final directory rename;
 * WLT48UXP.exe is verified but never copied. The proved idle FPGA slice is
 * stored as its own hash-addressed runtime object. No USB access occurs.
 */
LT48_API lt48_status lt48_private_cache_import(
    const char *source_path, const char *destination_path,
    lt48_private_cache_report *out_report
);
/* Import catalogue/definition REZ files from definition_source_path while
 * obtaining only the independently verified idle runtime object from
 * runtime_source_path. This permits a newer owned REZ-only catalogue to be
 * paired with the proved runtime payload from an older owned installation.
 * Both sources remain private; the executable is verified but never copied. */
LT48_API lt48_status lt48_private_cache_import_with_runtime(
    const char *definition_source_path, const char *runtime_source_path,
    const char *destination_path, lt48_private_cache_report *out_report
);
/* Verify the manifest object plus every content-addressed logical object. */
LT48_API lt48_status lt48_private_cache_verify(
    const char *path, lt48_private_cache_report *out_report
);
/* Strict, offline structural validation for authored component bundle schema
 * 1. It performs no USB access and never enables an operation. */
LT48_API lt48_status lt48_component_bundle_validate(
    const char *path, lt48_component_validation_report *out_report
);
LT48_API lt48_status lt48_component_bundle_init(
    const char *path, const char *bundle_id, const char *version,
    const char *author, const char *license, const char *created_date,
    unsigned char out_source_sha256[LT48_SHA256_SIZE]
);
LT48_API lt48_status lt48_component_bundle_add_object(
    const char *input_path, const char *output_path,
    const char *new_bundle_version, const char *new_created_date,
    const char *object_record_path,
    lt48_component_revision_report *out_report
);
/* Create an immutable revision while requiring the standalone record to have
 * exactly expected_kind. This prevents a purpose-specific authoring action
 * from silently accepting another valid object kind. */
LT48_API lt48_status lt48_component_bundle_add_object_of_kind(
    const char *input_path, const char *output_path,
    const char *new_bundle_version, const char *new_created_date,
    const char *object_record_path, const char *expected_kind,
    lt48_component_revision_report *out_report
);
/* Kind-specific semantic schema and declared-combination composition gate.
 * Completion remains authored/parsed evidence and cannot enable execution. */
LT48_API lt48_status lt48_component_bundle_check_semantics(
    const char *path, lt48_component_semantic_report *out_report
);
LT48_API const char *lt48_component_validation_issue_name(
    lt48_component_validation_issue issue
);
LT48_API lt48_status lt48_component_bundle_resolve(
    const char *path, const char *object_id,
    lt48_component_resolution *out_resolution
);
/* Resolve a child and return its canonical effective definition and delta,
 * but only when base_object_id is its declared immediate base. */
LT48_API lt48_status lt48_component_bundle_diff(
    const char *path, const char *base_object_id, const char *object_id,
    lt48_component_resolution *out_resolution
);
LT48_API void lt48_component_resolution_clear(
    lt48_component_resolution *resolution
);
/* Compile one semantically valid declared combination and operation into a
 * deterministic, mapped, payload-byte-free authored plan and typed common
 * phase IR. This is offline evidence, not hardware-execution authorization. */
LT48_API lt48_status lt48_component_bundle_compile_plan(
    const char *path, size_t combination_index, const char *operation_name,
    lt48_component_plan *out_plan
);
LT48_API void lt48_component_plan_clear(lt48_component_plan *plan);
/* Compile every declared combination and each operation selected by its
 * algorithm. The matrix is deterministic, offline and never execution-ready. */
LT48_API lt48_status lt48_component_bundle_compile_plan_matrix(
    const char *path, lt48_component_plan_matrix *out_matrix
);
LT48_API lt48_status lt48_component_bundle_compile_plan_matrix_cancellable(
    const char *path, lt48_cancel_check_fn cancelled,
    void *cancel_context, lt48_component_plan_matrix *out_matrix
);
LT48_API void lt48_component_plan_matrix_clear(
    lt48_component_plan_matrix *matrix
);
/* Apply the structural, semantic/composition, and full plan-matrix gates to
 * every declared variant/package/adapter combination. Offline only. */
LT48_API lt48_status lt48_component_bundle_validate_all_variants(
    const char *path, lt48_component_all_variants_report *out_report
);
LT48_API lt48_status lt48_component_bundle_validate_all_variants_cancellable(
    const char *path, lt48_cancel_check_fn cancelled,
    void *cancel_context, lt48_component_all_variants_report *out_report
);
/* Generate a deterministic, de-energized continuity/isolation procedure and
 * insertion guidance from a semantically validated adapter mapping. Active
 * adapters are reported but require a separate protocol-aware bench plan. */
LT48_API lt48_status lt48_component_bundle_adapter_test_plan(
    const char *path, const char *adapter_id,
    lt48_component_adapter_test_plan *out_plan
);
LT48_API void lt48_component_adapter_test_plan_clear(
    lt48_component_adapter_test_plan *plan
);
/* Canonicalize every semantically valid payload and authored build descriptor
 * into one bundle-bound plan. This does not spawn a process or verify local
 * source/toolchain/output files; those are separate execution gates. */
LT48_API lt48_status lt48_component_bundle_plan_payload_builds(
    const char *path, lt48_component_payload_build_plan *out_plan
);
LT48_API void lt48_component_payload_build_plan_clear(
    lt48_component_payload_build_plan *plan
);
LT48_API lt48_status lt48_component_bundle_verify_payload_inputs(
    const char *path, const char *workspace_root,
    lt48_component_payload_input_verification *out_verification
);
LT48_API lt48_status lt48_component_bundle_verify_payload_inputs_cancellable(
    const char *path, const char *workspace_root,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_payload_input_verification *out_verification
);
LT48_API void lt48_component_payload_input_verification_clear(
    lt48_component_payload_input_verification *verification
);
LT48_API lt48_status lt48_component_bundle_execute_payload_builds(
    const char *path, const char *workspace_root,
    const unsigned char confirmation_sha256[LT48_SHA256_SIZE],
    lt48_component_payload_build_execution *out_execution
);
LT48_API lt48_status lt48_component_bundle_execute_payload_builds_cancellable(
    const char *path, const char *workspace_root,
    const unsigned char confirmation_sha256[LT48_SHA256_SIZE],
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_payload_build_execution *out_execution
);
LT48_API void lt48_component_payload_build_execution_clear(
    lt48_component_payload_build_execution *execution
);
/* Validate a separate immutable evidence record bound to an existing bundle,
 * matrix and plan. Validation is offline and does not promote support. */
LT48_API lt48_status lt48_component_evidence_validate(
    const char *path, lt48_component_evidence_report *out_report
);
/* Hash the detailed artifact, rebuild the full matrix, and verify all subject
 * bindings. This remains an offline evidence check and cannot promote support. */
LT48_API lt48_status lt48_component_evidence_verify(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path,
    lt48_component_evidence_verification *out_verification
);
LT48_API lt48_status lt48_component_evidence_verify_cancellable(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path, lt48_cancel_check_fn cancelled,
    void *cancel_context,
    lt48_component_evidence_verification *out_verification
);
/* Verify and import an immutable record/artifact pair into STORE/records and
 * STORE/objects under content hashes. Existing matching objects are reused;
 * existing mismatches fail and are never overwritten. */
LT48_API lt48_status lt48_component_evidence_import(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path, const char *store_path,
    lt48_component_evidence_import_report *out_report
);
LT48_API lt48_status lt48_component_evidence_import_cancellable(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path, const char *store_path,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_import_report *out_report
);
LT48_API const char *lt48_component_evidence_level_name(
    lt48_component_evidence_level level
);
/* Reverify every stored record and conservatively evaluate evidence for every
 * compiled plan. This reports a proposal only; release_ready remains false
 * until an explicit reviewed registry-change workflow exists. */
LT48_API lt48_status lt48_component_evidence_audit_store(
    const char *bundle_path, const char *store_path,
    lt48_component_evidence_audit *out_audit
);
LT48_API lt48_status lt48_component_evidence_audit_store_cancellable(
    const char *bundle_path, const char *store_path,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_audit *out_audit
);
LT48_API void lt48_component_evidence_audit_clear(
    lt48_component_evidence_audit *audit
);
/* Produce a deterministic review proposal. It never applies registry changes;
 * transport lowering, independent review, and explicit approval remain hard
 * blockers until separately implemented and recorded. */
LT48_API lt48_status lt48_component_release_check_create(
    const char *bundle_path, const char *store_path,
    lt48_component_release_check *out_check
);
LT48_API lt48_status lt48_component_release_check_create_cancellable(
    const char *bundle_path, const char *store_path,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_release_check *out_check
);
LT48_API void lt48_component_release_check_clear(
    lt48_component_release_check *check
);
/* Retain immutable parsed catalogue metadata; owners still revalidate execution resources. */
LT48_API lt48_installation *lt48_installation_retain(lt48_installation *installation);
LT48_API void lt48_installation_close(lt48_installation *installation);
LT48_API size_t lt48_installation_device_count(
    const lt48_installation *installation
);
LT48_API lt48_status lt48_installation_get_device(
    const lt48_installation *installation,
    size_t index,
    lt48_catalogue_entry *out_entry
);
LT48_API lt48_status lt48_installation_resolve_device(
    const lt48_installation *installation,
    size_t index,
    lt48_device_spec *out_spec
);
/*
 * Open typed UI metadata owned by one exact selected database definition.
 * Files resolve below ui-properties/STEM/D<type>/ and never fall back to a
 * different database, device class, family, or catalogue entry. Absence is
 * reported as LT48_NOT_FOUND. This describes controls only; it does not by
 * itself authorize or implement configuration/fuse/vector execution.
 */
LT48_API lt48_status lt48_installation_open_device_property_document(
    const lt48_installation *installation,
    size_t device_index,
    lt48_property_sheet_kind kind,
    lt48_property_document **out_document
);
/* Resolve the definition's exact callback symbols and waves without USB I/O.
 * A NULL items/zero capacity call reports the required count. */
LT48_API lt48_status lt48_installation_device_operations(
    const lt48_installation *installation, size_t device_index,
    lt48_device_operation_binding *items, size_t capacity,
    size_t *out_count
);
/* Library-owned wiring note for an exactly qualified source binding, or NULL.
 * This additive API preserves the existing guidance structure ABI. */
LT48_API const char *lt48_installation_device_adapter_wiring_note(
    const lt48_installation *installation, size_t device_index
);
/* Authored source-bound display correction; raw catalogue identity is retained.
 * Returns NULL when no independently qualified correction applies. */
LT48_API const char *lt48_installation_device_corrected_name(
    const lt48_installation *installation, size_t device_index
);

LT48_API lt48_status lt48_installation_device_guidance(
    const lt48_installation *installation, size_t device_index,
    lt48_device_guidance *out_guidance
);
/* Compile every bound operation for one definition without USB access.
 * Opaque non-type-0 resources are counted as local resources, not as proved
 * executable behavior. */
LT48_API lt48_status lt48_installation_device_compile_evidence(
    const lt48_installation *installation, size_t device_index,
    lt48_device_compile_evidence *out_evidence
);
/* Complete offline installation audit. It opens each referenced class file
 * once, resolves every catalogue D/M/P resource closure, and hashes the
 * catalogue plus class-file digests in class order. It never opens USB. */
LT48_API lt48_status lt48_installation_audit_all(
    const lt48_installation *installation,
    lt48_installation_audit *out_audit
);
LT48_API lt48_status lt48_installation_audit_all_cancellable(
    const lt48_installation *installation,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_installation_audit *out_audit
);
/*
 * Return the frontend-neutral operation matrix for one selected device.
 * This is local-definition inspection only and never opens USB. String
 * pointers are static. A NULL items/zero capacity call queries the fixed
 * required count. LT48_BUFFER_TOO_SMALL reports the required count without
 * returning a partial matrix.
 */
LT48_API lt48_status lt48_installation_operation_support(
    const lt48_installation *installation, size_t device_index,
    lt48_operation_support *items, size_t capacity, size_t *out_count
);
LT48_API lt48_status lt48_installation_operation_support_one(
    const lt48_installation *installation, size_t device_index,
    lt48_operation_kind operation, lt48_operation_support *out_support
);
/* Complete device support is stricter than an individual executable matrix
 * cell.  Every operation applicable to the device class must be executable;
 * partial rows are deliberately excluded from supported-device counts. */
LT48_API const char *lt48_installation_complete_operation_profile(const lt48_installation *,size_t);
LT48_API const char *lt48_complete_operation_profile(uint16_t device_class);
/* Offline, uncached support snapshot. Profile is a static library string.
 * Does not open USB. All applicable operations must be executable. */
LT48_API lt48_status lt48_installation_support_summary(
    const lt48_installation *, size_t device_index,
    lt48_operation_support *, size_t capacity, size_t *out_count,
    const char **out_profile, int *out_complete);

LT48_API int lt48_installation_device_complete_support(
    const lt48_installation *installation, size_t device_index
);
/* Definition-backed verify voltages. margin_type 0 is 95/105%; type 1 is
 * 90/110%. Enabled VerifyVCC0..3 values replace computed defaults. */
LT48_API lt48_status lt48_installation_verify_voltage_profile(
    const lt48_installation *installation, size_t device_index,
    unsigned margin_type, lt48_verify_voltage_profile *out_profile
);
LT48_API lt48_status lt48_verify_voltage_profile_select(
    const lt48_verify_voltage_profile *profile,
    lt48_verify_margin_selection selection,
    uint16_t *voltages, size_t capacity, size_t *out_count
);
/*
 * Rank the complete catalogue from acquired observations. Manufacturer and
 * device IDs, when present, are exact filters. Package pin count and device
 * class are optional ranking hints and never silently exclude a candidate.
 * The function is offline and does not depend on current device selection.
 */
LT48_API lt48_status lt48_installation_rank_auto_id(
    const lt48_installation *installation,
    const lt48_auto_id_observation *observation,
    lt48_auto_id_candidate *items, size_t capacity, size_t *out_count
);
LT48_API lt48_status lt48_auto_id_eprom_execution_observation(
    const lt48_auto_id_eprom_execution_result *execution,
    lt48_auto_id_observation *out_observation
);
LT48_API lt48_status lt48_installation_rank_auto_id_eprom_execution(
    const lt48_installation *installation,
    const lt48_auto_id_eprom_execution_result *execution,
    lt48_auto_id_candidate *items, size_t capacity, size_t *out_count
);
/* Recovered Auto Select EPROM provider.  It is one bounded provider for six
 * package sizes, not universal Auto-ID and not selected-device W6/get-ID. */
LT48_API size_t lt48_auto_id_eprom_recipe_count(void);
LT48_API lt48_status lt48_auto_id_eprom_recipe_get(
    size_t index, lt48_auto_id_eprom_recipe *out_recipe
);
LT48_API lt48_status lt48_auto_id_eprom_recipe_find(
    uint16_t package_pin_count, lt48_auto_id_eprom_recipe *out_recipe
);
LT48_API lt48_status lt48_auto_id_eprom_decode_socket(
    const unsigned char raw[12],
    lt48_auto_id_socket_observation *out_observation
);
LT48_API lt48_status lt48_auto_id_eprom_plan(
    int has_explicit_pin_hint, uint16_t explicit_pin_hint,
    lt48_auto_id_eprom_plan_info *out_plan
);
/* Pure offline target/mode/provider binding; performs no USB access. */
LT48_API lt48_status lt48_auto_id_eprom_prepare(
    uint8_t bus_number,
    uint8_t device_address,
    int acquire_signature,
    int has_explicit_pin_hint,
    uint16_t explicit_pin_hint,
    lt48_auto_id_eprom_preparation *out_preparation
);
/*
 * Execute the recovered provider over an already owned and initialized
 * transport. This API does not open USB. It always attempts controller
 * finalizer 49 aa 55, including probe, signature and transport-failure exits.
 */
LT48_API lt48_status lt48_auto_id_eprom_execute(
    const lt48_protocol_transport *transport,
    void *transport_context,
    int acquire_signature,
    int has_explicit_pin_hint,
    uint16_t explicit_pin_hint,
    uint32_t authorization,
    lt48_auto_id_eprom_execution_result *out_result
);
/* Borrowed installation path; valid until lt48_installation_close(). */
/* Exact catalogue identity; this does not grant device operation support. */
LT48_API const char *lt48_installation_source_profile(const lt48_installation *installation);
/* Selected-definition insertion check, distinct from device-ID matching.
 * Capture uses resident47, source mask semantics and independent finalizer49.
 * Loads the verified idle FPGA first. Alternate05 additionally requires the
 * verified owner-supplied B000 dispatcher; neither payload is distributed.
 * A successful check is momentary contact evidence, not electrical qualification. */
/* Standalone diagnostics; empty-socket testing requires explicit operator
 * confirmation and exact owned idle image. No device operation is performed. */
typedef enum lt48_hardware_test_kind {
    LT48_HARDWARE_TEST_ECHO = 0,
    LT48_HARDWARE_TEST_EMPTY_SOCKET = 1
} lt48_hardware_test_kind;
typedef struct lt48_hardware_test_result {
    lt48_status operation_status, cleanup_status;
    unsigned echo_checks, pin_checks, patterns_completed;
    unsigned failed_pattern, failed_pin, expected, observed;
    unsigned empty_capture_completed, cleanup_completed;
    unsigned char empty_raw[12];
    uint64_t failed_pins;
} lt48_hardware_test_result;
LT48_API lt48_status lt48_hardware_test_execute(lt48_hardware_test_kind kind,
    const unsigned char *idle, size_t idle_size,
    const lt48_protocol_transport *transport, void *context,
    lt48_hardware_test_result *out);
LT48_API lt48_status lt48_programmer_hardware_test(lt48_programmer *programmer,
    lt48_hardware_test_kind kind, const unsigned char *idle, size_t idle_size,
    unsigned timeout_ms, lt48_hardware_test_result *out);
LT48_API lt48_status lt48_installation_idle_fpga(const char *path,
    unsigned char **out_data, size_t *out_size);

typedef struct lt48_contact_check_profile {
    uint32_t required[2], forbidden[2], auxiliary[2];
    unsigned pin_count, alternate;
    size_t idle_size, alternate_size;
    unsigned char idle_fpga[2778], alternate_payload[476];
} lt48_contact_check_profile;
typedef struct lt48_contact_check_result {
    lt48_status operation_status, cleanup_status;
    unsigned attempted, capture_completed, cleanup_completed, matched;
    unsigned char raw[12];
    uint32_t observed[2], missing[2], unexpected[2];
} lt48_contact_check_result;
LT48_API lt48_status lt48_installation_contact_check_profile(
    const lt48_installation *, size_t, lt48_contact_check_profile *);
LT48_API lt48_status lt48_contact_check_execute(const lt48_contact_check_profile *,
    const lt48_protocol_transport *, void *, lt48_contact_check_result *);
LT48_API lt48_status lt48_programmer_check_contacts(lt48_programmer *,
    const lt48_contact_check_profile *, unsigned, lt48_contact_check_result *);

LT48_API const char *lt48_installation_path(
    const lt48_installation *installation
);
/* Portable utility used for image and resource provenance. */
LT48_API void lt48_sha256(
    const unsigned char *data, size_t size,
    unsigned char digest[LT48_SHA256_SIZE]
);
/* Run deterministic core checks without enumerating or opening USB. When
 * installation_path is non-NULL/nonempty, also perform the complete offline
 * installation/cache audit. A completed check returns LT48_OK; inspect
 * out_report->passed for the diagnostic outcome. */
LT48_API lt48_status lt48_diagnostics_self_test(
    const char *installation_path, lt48_diagnostics_report *out_report
);
/* Offline audit only: parses and compiles resources but performs no USB I/O. */
LT48_API lt48_status lt48_installation_audit_m37(
    const lt48_installation *installation,
    size_t device_index,
    lt48_m37_resource_audit *out_audit
);
/*
 * Open an immutable, safety-audited M37 execution snapshot. The constructor
 * accepts only the narrow proved engine envelope. All returned pointers are
 * borrowed and remain valid until lt48_m37_resources_close().
 */
LT48_API lt48_status lt48_m37_resources_open(
    const lt48_installation *installation,
    size_t device_index,
    lt48_m37_resources **out_resources
);
LT48_API void lt48_m37_resources_close(lt48_m37_resources *resources);
LT48_API const lt48_m37_resource_audit *lt48_m37_resources_audit(
    const lt48_m37_resources *resources
);
LT48_API const lt48_pin_map *lt48_m37_resources_pin_map(
    const lt48_m37_resources *resources
);
LT48_API const lt48_compile_variable *lt48_m37_resources_variables(
    const lt48_m37_resources *resources, size_t *out_count
);
LT48_API lt48_status lt48_m37_resources_compiled_wave(
    const lt48_m37_resources *resources,
    uint16_t operation_id,
    uint16_t *out_wave,
    const lt48_compiled_wave **out_compiled
);
LT48_API size_t lt48_m37_resources_applet_count(
    const lt48_m37_resources *resources
);
LT48_API lt48_status lt48_m37_resources_get_applet(
    const lt48_m37_resources *resources,
    size_t index,
    lt48_m37_applet_resource *out_applet
);
LT48_API lt48_status lt48_m37_resources_active_fpga(
    const lt48_m37_resources *resources,
    const unsigned char **out_data,
    size_t *out_size
);
LT48_API lt48_status lt48_m37_resources_idle_fpga(
    const lt48_m37_resources *resources,
    const unsigned char **out_data,
    size_t *out_size
);
LT48_API lt48_status lt48_rez_get_family_frame_profile(
    const lt48_rez *rez, size_t family_index, lt48_runtime_frame_profile *out_profile
);
LT48_API lt48_status lt48_rez_get_family_info(
    const lt48_rez *rez, size_t index, lt48_family_info *out_info
);
LT48_API lt48_status lt48_rez_get_family_operation_options(
    const lt48_rez *rez, size_t index,
    lt48_family_operation_options *out_options
);
LT48_API lt48_status lt48_rez_get_family_operation(
    const lt48_rez *rez, size_t index, size_t operation_index,
    lt48_family_operation *out_operation
);
LT48_API lt48_status lt48_rez_get_family_variable(
    const lt48_rez *rez, size_t index, size_t variable_index,
    lt48_family_variable *out_variable
);
LT48_API lt48_status lt48_rez_get_defined_name_collection_info(
    const lt48_rez *rez, size_t index,
    lt48_defined_name_collection_info *out_info
);
/* Bulk form: one validated traversal, same ordering and contents as single reads. */
LT48_API lt48_status lt48_rez_get_defined_names(const lt48_rez *rez, size_t index,
    lt48_defined_name *names, size_t capacity, size_t *count);

LT48_API lt48_status lt48_rez_get_defined_name(
    const lt48_rez *rez, size_t index, size_t name_index,
    lt48_defined_name *out_name
);
LT48_API lt48_status lt48_rez_get_pin_code(
    const lt48_rez *rez, size_t index, lt48_pin_code *out_pin_code
);
LT48_API lt48_status lt48_rez_get_wave_code(
    const lt48_rez *rez, size_t index, lt48_wave_code *out_wave_code
);
LT48_API lt48_status lt48_rez_get_variable_name(
    const lt48_rez *rez, size_t index, size_t name_index,
    const unsigned char **out_name, size_t *out_name_length,
    size_t *out_name_count
);

/* Syntax-only decoder for a length-prefixed type-0 compact waveform. */
LT48_API lt48_status lt48_compact_decode(
    const unsigned char *data,
    size_t size,
    lt48_compact_wave **out_wave
);
LT48_API void lt48_compact_wave_close(lt48_compact_wave *wave);
LT48_API size_t lt48_compact_instruction_count(const lt48_compact_wave *wave);
LT48_API lt48_status lt48_compact_get_instruction(
    const lt48_compact_wave *wave,
    size_t index,
    lt48_compact_instruction *out_instruction
);
LT48_API lt48_status lt48_pin_map_parse(
    const lt48_pin_code *pin_code, lt48_pin_map **out_pin_map
);
LT48_API void lt48_pin_map_close(lt48_pin_map *pin_map);
LT48_API uint8_t lt48_pin_map_package_pin_count(const lt48_pin_map *pin_map);
LT48_API size_t lt48_pin_map_group_count(const lt48_pin_map *pin_map);
LT48_API size_t lt48_pin_map_logical_count(const lt48_pin_map *pin_map);
LT48_API lt48_status lt48_pin_map_get_group(
    const lt48_pin_map *pin_map, size_t index, lt48_pin_group *out_group
);
LT48_API lt48_status lt48_pin_map_find_group(
    const lt48_pin_map *pin_map, uint16_t kind, lt48_pin_group *out_group
);
LT48_API lt48_status lt48_pin_map_get_logical(
    const lt48_pin_map *pin_map, size_t index, lt48_logical_pin *out_pin
);
LT48_API lt48_status lt48_pin_map_find_logical(
    const lt48_pin_map *pin_map, uint16_t number, lt48_logical_pin *out_pin
);
LT48_API lt48_status lt48_compact_compile(
    const lt48_compact_wave *wave,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    uint16_t selected_group,
    int analysis_default_unmapped_pins,
    lt48_compiled_wave **out_compiled
);
/* Policy-aware compiler entry point. RESIDENT_NOOP admits only an unmapped
 * opcode-0x11 reference whose encoded q2 byte is 0, 1, or 2; those commands
 * are proved immediate RET entries in the resident 80C32 dispatcher.
 * ANALYSIS_ALL reproduces Windows' zero-filled lookup table but is not an
 * execution policy. The legacy boolean API above maps false to STRICT and
 * true to ANALYSIS_ALL. */
LT48_API lt48_status lt48_compact_compile_with_pin_policy(
    const lt48_compact_wave *wave,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    uint16_t selected_group,
    lt48_pin_fallback_policy pin_policy,
    lt48_compiled_wave **out_compiled
);
LT48_API void lt48_compiled_wave_close(lt48_compiled_wave *compiled);
/* Decode a length-prefixed type-5 bit mapping table; no executable code. */
/* Type-2 pin-control matrix: length prefix, row/column counts, byte cells. */
LT48_API lt48_status lt48_compiled_control_table_create(
    const unsigned char *data, size_t size, lt48_compiled_wave **out_table);
LT48_API lt48_status lt48_compiled_map_create(
    const unsigned char *data, size_t size, lt48_compiled_wave **out_map);
LT48_API const uint16_t *lt48_compiled_wave_words(
    const lt48_compiled_wave *compiled, size_t *out_word_count
);
LT48_API size_t lt48_compiled_wave_expansion_count(
    const lt48_compiled_wave *compiled
);
LT48_API lt48_status lt48_compiled_wave_get_expansion(
    const lt48_compiled_wave *compiled, size_t index,
    lt48_compiled_expansion *out_expansion
);
LT48_API size_t lt48_compiled_wave_defaulted_pin_count(
    const lt48_compiled_wave *compiled
);
LT48_API lt48_status lt48_compiled_wave_get_defaulted_pin(
    const lt48_compiled_wave *compiled, size_t index,
    lt48_defaulted_logical_pin *out_pin
);
LT48_API size_t lt48_compiled_wave_preserved_buffer_count(
    const lt48_compiled_wave *compiled
);
LT48_API lt48_status lt48_compiled_wave_get_preserved_buffer(
    const lt48_compiled_wave *compiled, size_t index, uint8_t *out_buffer
);
LT48_API lt48_status lt48_compiled_wave_completion_message(
    const lt48_compiled_wave *compiled, uint16_t *out_message_index
);

/* Build the original seven-byte controller command 0x12 without USB I/O.
 * Positive signed-32-bit duration only; unrepresentable fallback intervals
 * are rejected rather than silently truncated. Output is unchanged on error.
 * The selected PALCE default and positive unit-zero edit envelope have an
 * independent original-host arithmetic audit. This alone does not authorize
 * a voltage, timing edit, device selection, or hardware operation. */
LT48_API lt48_status lt48_dac_ramp_packet(
    uint8_t channel, uint8_t from_code, uint8_t to_code, uint32_t duration,
    unsigned char out_packet[7]
);

/*
 * Side-effect-free interpreter for the independently proved runtime subset.
 * It never opens or writes USB. Events and final buffers remain owned by the
 * result until lt48_runtime_result_close(). A zero max_steps selects 1,000,000.
 */
LT48_API lt48_status lt48_runtime_interpret(
    const lt48_compiled_wave *compiled,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    const lt48_runtime_options *options,
    lt48_runtime_result **out_result
);
LT48_API void lt48_runtime_result_close(lt48_runtime_result *result);
LT48_API size_t lt48_runtime_event_count(const lt48_runtime_result *result);
LT48_API lt48_status lt48_runtime_get_event(
    const lt48_runtime_result *result, size_t index,
    lt48_runtime_event *out_event
);
LT48_API lt48_status lt48_runtime_get_buffer(
    const lt48_runtime_result *result, uint8_t buffer_id,
    const unsigned char **out_data, size_t *out_size
);
LT48_API lt48_status lt48_runtime_get_voltage(
    const lt48_runtime_result *result, uint8_t channel,
    uint32_t *out_centivolts
);
/* Mutable host-callback frame, present only when such a binding is enabled.
 * Returned values belong to this result; the original resource frame remains
 * unchanged. The owning session must retain them for subsequent waves. */
LT48_API lt48_status lt48_runtime_get_image(const lt48_runtime_result *result,
    const unsigned char **data, size_t *size);
LT48_API lt48_status lt48_runtime_get_configuration(const lt48_runtime_result *result,
    const lt48_runtime_configuration_value **values, size_t *count);
LT48_API lt48_status lt48_runtime_get_variable(
    const lt48_runtime_result *result, uint16_t identifier,
    lt48_compile_variable *out_variable);
LT48_API lt48_status lt48_runtime_get_fuse_section(
    const lt48_runtime_result *result, unsigned *out_section);

/*
 * Resumable interpreter. The compiled wave, pin map and variable array are
 * borrowed and must outlive the machine. Partial results expose only committed
 * events; final buffers and voltages become authoritative at FINISHED.
 */
/* Source mode transition: marked 32-byte buffers are cleared unless an
 * operation is active and option 0x200 is set; mode is stored as signed LE32.
 * Buffer count and IDs must come from the selected resource package. */
LT48_API lt48_status lt48_runtime_frame_set_mode(
    unsigned char buffers[][32], uint8_t buffer_count, uint32_t reset_mask,
    uint8_t mode_buffer, uint16_t mode, int operation_active, uint32_t option_flags
);

LT48_API lt48_status lt48_runtime_machine_create(
    const lt48_compiled_wave *compiled,
    const lt48_pin_map *pin_map,
    const lt48_compile_variable *variables,
    size_t variable_count,
    const lt48_runtime_options *options,
    lt48_runtime_machine **out_machine
);
LT48_API void lt48_runtime_machine_close(lt48_runtime_machine *machine);
LT48_API lt48_status lt48_runtime_machine_advance(
    lt48_runtime_machine *machine,
    lt48_runtime_machine_state *out_state
);
LT48_API lt48_runtime_machine_state lt48_runtime_machine_get_state(
    const lt48_runtime_machine *machine
);
LT48_API lt48_status lt48_runtime_machine_capture_buffer(
    const lt48_runtime_machine *machine, uint8_t *out_buffer
);
LT48_API lt48_status lt48_runtime_machine_capture_request(
    const lt48_runtime_machine *machine, uint8_t *out_buffer,
    size_t *out_length
);
LT48_API lt48_status lt48_runtime_machine_supply_capture(
    lt48_runtime_machine *machine, uint8_t value
);
LT48_API lt48_status lt48_runtime_machine_supply_capture_bytes(
    lt48_runtime_machine *machine, const unsigned char *values, size_t length
);
LT48_API const lt48_runtime_result *lt48_runtime_machine_result(
    const lt48_runtime_machine *machine
);
LT48_API void lt48_runtime_machine_last_event_range(
    const lt48_runtime_machine *machine, size_t *out_first, size_t *out_count
);
LT48_API lt48_status lt48_runtime_machine_take_result(
    lt48_runtime_machine *machine, lt48_runtime_result **out_result
);

/*
 * Offline protocol encoder. It records exact endpoint transactions and never
 * opens USB. Queue state is retained across apply calls. Unsupported events
 * fail atomically, leaving the plan unchanged.
 */
LT48_API lt48_status lt48_protocol_plan_create(lt48_protocol_plan **out_plan);
/* Offline-only framing profile for 64- or 512-byte command packets. Native
 * hardware arming currently accepts only 512-byte plans. This generic
 * encoder is not a complete original-software operation workflow. */
LT48_API lt48_status lt48_protocol_plan_create_offline_for_packet_size(
    size_t packet_size, lt48_protocol_plan **out_plan
);
LT48_API void lt48_protocol_plan_close(lt48_protocol_plan *plan);
LT48_API lt48_status lt48_protocol_plan_apply_runtime(
    lt48_protocol_plan *plan,
    const lt48_runtime_result *runtime,
    const lt48_applet_address *applets,
    size_t applet_count
);
LT48_API lt48_status lt48_protocol_plan_apply_runtime_range(
    lt48_protocol_plan *plan,
    const lt48_runtime_result *runtime,
    size_t first_event,
    size_t event_count,
    const lt48_applet_address *applets,
    size_t applet_count
);
LT48_API lt48_status lt48_protocol_plan_flush(lt48_protocol_plan *plan);
/* Optionally queue 4f6400, then the original 5454 / status-read exchange. */
LT48_API lt48_status lt48_protocol_plan_settle_status(lt48_protocol_plan *plan, int settle);
/* Original nested-dispatch boundary: changed timeout flushes and sends2110
 * without a reply read; unchanged timeout queues control pair0800. */
LT48_API lt48_status lt48_protocol_plan_wave_watchdog(
    lt48_protocol_plan *plan, uint32_t milliseconds, int cached_valid,
    uint32_t cached_milliseconds, int interpreter_active);

LT48_API size_t lt48_protocol_plan_transaction_count(
    const lt48_protocol_plan *plan
);
LT48_API lt48_status lt48_protocol_plan_get_transaction(
    const lt48_protocol_plan *plan, size_t index,
    lt48_protocol_transaction *out_transaction
);
/* Canonical digest: direction byte, endpoint, LE32 length, then OUT data. */
LT48_API lt48_status lt48_protocol_plan_sha256(
    const lt48_protocol_plan *plan,
    unsigned char digest[LT48_SHA256_SIZE]
);
/* Recovery-phase metadata exists only on snapshot-derived recovery plans. */
LT48_API size_t lt48_protocol_plan_recovery_phase_count(
    const lt48_protocol_plan *plan
);
LT48_API lt48_status lt48_protocol_plan_get_recovery_phase(
    const lt48_protocol_plan *plan,
    size_t index,
    lt48_m37_cleanup_phase *out_phase,
    size_t *out_first_transaction,
    size_t *out_transaction_count
);
/* Execute an already-built range only through caller-injected I/O callbacks. */
LT48_API lt48_status lt48_protocol_plan_execute_range(
    const lt48_protocol_plan *plan,
    size_t first_transaction,
    size_t transaction_count,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_runtime_capture *captures,
    size_t capture_capacity,
    size_t *out_capture_count,
    size_t *out_completed_transactions
);
LT48_API lt48_status lt48_protocol_plan_execute_range_collect(
    const lt48_protocol_plan *plan,
    size_t first_transaction,
    size_t transaction_count,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_m37_read_collector *collector,
    lt48_runtime_capture *captures,
    size_t capture_capacity,
    size_t *out_capture_count,
    size_t *out_completed_transactions
);
/* Injected-I/O waveform session. Preserves the protocol queue across waves
 * and resumes controller captures. This is not device admission or cleanup;
 * the owning engine must inspect host result events and preserve buffers and
 * voltage state. After failure the session refuses further forward work.
 * Closing only releases memory, and does not perform hardware cleanup. */
typedef struct lt48_runtime_transport_session lt48_runtime_transport_session;
LT48_API lt48_status lt48_runtime_transport_session_wave_watchdog(
    lt48_runtime_transport_session *session, uint32_t milliseconds,
    int interpreter_active);

LT48_API lt48_status lt48_runtime_transport_session_create(
    const lt48_protocol_transport *transport, void *context,
    lt48_runtime_transport_session **out_session);
LT48_API void lt48_runtime_transport_session_close(lt48_runtime_transport_session *session);
LT48_API lt48_status lt48_runtime_transport_session_run(
    lt48_runtime_transport_session *session, lt48_runtime_machine *machine,
    lt48_runtime_result **out_result);
LT48_API lt48_status lt48_runtime_transport_session_run_with_applets(
    lt48_runtime_transport_session *session, lt48_runtime_machine *machine,
    const lt48_applet_address *applets, size_t applet_count,
    lt48_runtime_result **out_result);
LT48_API lt48_status lt48_runtime_transport_session_flush(lt48_runtime_transport_session *session);
LT48_API lt48_status lt48_runtime_transport_session_status(
    lt48_runtime_transport_session *session, int settle);

/* PALCE wave frame: persistent named buffers, voltage cache and queue.
 * resources must outlive the session. The caller owns returned results and
 * must inspect return/latch events: LT48_OK means wave execution completed,
 * not that a device operation succeeded. No mode-entry or hardware cleanup
 * is implied, and this primitive does not admit any device operation. */
typedef struct lt48_palce_wave_session lt48_palce_wave_session;
LT48_API lt48_status lt48_palce_wave_session_create(
    const lt48_palce_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_palce_wave_session **out_session);
LT48_API void lt48_palce_wave_session_close(lt48_palce_wave_session *session);
/* Original reset-helper behavior: clear unexempted buffers, invalidate ID.
 * No I/O; voltage cache and controller queue remain intact. The lifecycle
 * owner calls this at the original mode-setting boundary. */
LT48_API lt48_status lt48_palce_wave_session_reset_frame(lt48_palce_wave_session *session);
LT48_API lt48_status lt48_palce_wave_session_run(
    lt48_palce_wave_session *session, uint16_t operation, uint32_t mode,
    const lt48_runtime_buffer_seed *overlays, size_t overlay_count,
    lt48_runtime_result **out_result);
LT48_API lt48_status lt48_palce_wave_session_flush(lt48_palce_wave_session *session);
/* Execute LOAD over all primary cells in the caller-established mode.
 * Output is one byte per fuse in JEDEC order and is committed only on success.
 * Does not establish power/mode or perform cleanup. */
LT48_API lt48_status lt48_palce_wave_session_read_fuses(
    lt48_palce_wave_session *session, uint32_t mode,
    unsigned char *out_fuses, size_t capacity, size_t *out_completed_cells);


/* PROGRAM all cells in the caller-established programming mode (2).
 * Input is exactly 2194 one-byte Boolean fuses. Verification is a separate
 * operation cycle; this call does not claim verified programming success. */
LT48_API lt48_status lt48_palce_wave_session_program_fuses(
    lt48_palce_wave_session *session, const unsigned char *fuses,
    size_t count, size_t *out_completed_cells);

typedef struct lt48_palce_fuse_comparison {
    int has_result;
    int matched;
    size_t completed_cells;
    size_t mismatch_count;
    size_t first_mismatch; /* SIZE_MAX if no mismatch was found. */
    unsigned char expected;
    unsigned char actual;
} lt48_palce_fuse_comparison;
/* Both calls perform a fresh complete LOAD traversal in the established mode.
 * LT48_OK denotes a valid comparison; matched distinguishes its outcome.
 * Transport/interpretation errors return without claiming a comparison result. */
LT48_API lt48_status lt48_palce_wave_session_verify_fuses(
    lt48_palce_wave_session *session, uint32_t mode,
    const unsigned char *expected, size_t count,
    lt48_palce_fuse_comparison *out_comparison);
LT48_API lt48_status lt48_palce_wave_session_blank_check(
    lt48_palce_wave_session *session,
    lt48_palce_fuse_comparison *out_comparison);

/* ERASE direct-ID paths after a successful DEVINIT in caller-established
 * mode 7 (original erase worker) or legacy mode 10, including ID 80's
 * four audited SubErase pulses. No blank
 * result or cleanup is implied by successful erase-wave execution. */
LT48_API lt48_status lt48_palce_wave_session_erase(lt48_palce_wave_session *session);

/* Mutable injected-I/O plan/session controller writer. Retains queued bytes;
 * it does not itself admit USB hardware or perform a device operation. */
LT48_API lt48_status lt48_protocol_plan_controller_write(
    lt48_protocol_plan *plan, const unsigned char *bytes, size_t size);
LT48_API lt48_status lt48_runtime_transport_session_controller_write(
    lt48_runtime_transport_session *session, const unsigned char *bytes, size_t size);
/* Normal PALCE entry after caller transport initialization and contact-check
 * decision. Clears the mode frame, POWERON, three settle/status exchanges,
 * controller option/LED and DEVINIT. Failure retains cleanup-required state.
 * Caller must recover/clean up before another entry or destroying the session. */
LT48_API lt48_status lt48_palce_wave_session_enter_mode(
    lt48_palce_wave_session *session, uint32_t mode, unsigned char *out_id);
/* After a successfully completed cell stage, execute DEVRESET, reset the
 * mode frame and DEVINIT for the next stage while retaining the original
 * operation-active state. No intervening POWEROFF/POWERON or extra completion
 * queries are inserted. Failure requires independent cleanup. */
LT48_API lt48_status lt48_palce_wave_session_switch_mode(
    lt48_palce_wave_session *session, uint32_t mode, unsigned char *out_id);
LT48_API int lt48_palce_wave_session_requires_cleanup(const lt48_palce_wave_session *session);
/* Successful operation exit: DEVRESET, completion query, POWEROFF and
 * controller finalizer. A failure retains cleanup-required state and poisons
 * forward execution. This is not recovery for a failed session. */
LT48_API lt48_status lt48_palce_wave_session_finish_mode(lt48_palce_wave_session *session);

typedef struct lt48_palce_operation_result {
    lt48_status operation_status;
    lt48_status cleanup_status;
    size_t completed_cells;
    int programmed;
    int erased;
    int cleanup_completed;
    lt48_palce_fuse_comparison comparison;
    int secured; /* SECU waveform completed; cleanup status is separate. */
} lt48_palce_operation_result;
/* Complete injected-transport owner for the admitted PALCE resources.
 * Caller establishes transport readiness and contact-check decision first.
 * Program requires blank comparison, then programs and freshly verifies.
 * Erase uses original mode7 then freshly checks blank in mode11.
 * OK means execution/cleanup completed: comparison.matched must also be
 * checked for Blank, Verify, Program and Erase. A nonblank Program stops
 * before programming. Read output is committed only after successful cleanup.
 * No USB opening, GUI admission, or new resource/device admission is implied. */
LT48_API lt48_status lt48_palce_execute_operation(
    const lt48_palce_resources *resources, const lt48_protocol_transport *transport,
    void *context, lt48_operation_kind operation,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    lt48_palce_operation_result *out_result);

/* Source-bound GBA GAL session.  The session keeps the selected family
 * frame, nested waves, PES variables and fuse-coordinate order across the
 * original mode transitions.  It is an injected-transport owner; USB
 * admission remains a separate policy decision. */
enum { LT48_PALCE_RECOVERY_PHASE_COUNT = 9 };
typedef struct lt48_palce_recovery_result {
    /* DEVRESET, completion, POWEROFF, zero rails 3/2/1/0,
     * controller finalizer, failure indicator, in that order. */
    lt48_status phases[LT48_PALCE_RECOVERY_PHASE_COUNT];
    size_t attempted_phases;
    int completed;
} lt48_palce_recovery_result;
typedef struct lt48_gal_wave_session lt48_gal_wave_session;
LT48_API lt48_status lt48_gal_wave_session_create(
    const lt48_gal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_gal_wave_session **out_session);
LT48_API void lt48_gal_wave_session_close(lt48_gal_wave_session *session);
LT48_API lt48_status lt48_gal_wave_session_enter_mode(
    lt48_gal_wave_session *session, uint32_t mode);
LT48_API lt48_status lt48_gal_wave_session_switch_mode(
    lt48_gal_wave_session *session, uint32_t mode);
LT48_API lt48_status lt48_gal_wave_session_finish_mode(
    lt48_gal_wave_session *session);
LT48_API lt48_status lt48_gal_wave_session_read_fuses(
    lt48_gal_wave_session *session, uint32_t mode,
    unsigned char *out_fuses, size_t capacity, size_t *out_completed_cells);
/* Prepare the immutable program image before entering mode2. Required by
 * source procedures that program architecture/configuration during DEVINIT.
 * The complete operation owner performs this preparation automatically. */
LT48_API lt48_status lt48_gal_wave_session_prepare_program_fuses(
    lt48_gal_wave_session *,const unsigned char *,size_t);
LT48_API lt48_status lt48_gal_wave_session_program_fuses(
    lt48_gal_wave_session *session, const unsigned char *fuses,
    size_t count, size_t *out_completed_cells);
LT48_API lt48_status lt48_gal_wave_session_verify_fuses(
    lt48_gal_wave_session *session, uint32_t mode,
    const unsigned char *expected, size_t count,
    lt48_palce_fuse_comparison *out_comparison);
LT48_API lt48_status lt48_gal_wave_session_blank_check(
    lt48_gal_wave_session *session,
    lt48_palce_fuse_comparison *out_comparison);
LT48_API int lt48_gal_wave_session_requires_cleanup(
    const lt48_gal_wave_session *session);
LT48_API lt48_status lt48_gal_wave_session_recover(
    lt48_gal_wave_session *session, lt48_palce_recovery_result *out_result);

typedef struct lt48_gal_operation_result {
    lt48_status operation_status;
    lt48_status cleanup_status;
    size_t completed_cells;
    int programmed;
    int erased;
    int cleanup_completed;
    lt48_palce_fuse_comparison comparison;
    int secured;
} lt48_gal_operation_result;
LT48_API lt48_status lt48_programmer_execute_gal_operation(
    lt48_programmer *programmer,
    const lt48_gal_resources *resources,
    lt48_operation_kind operation,
    const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity,
    unsigned timeout_ms,
    lt48_gal_operation_result *out_result
);
LT48_API lt48_status lt48_gal_execute_operation(
    const lt48_gal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_operation_kind operation, const unsigned char *input_fuses,
    size_t input_count, unsigned char *read_fuses, size_t read_capacity,
    lt48_gal_operation_result *out_result);

typedef struct lt48_pal_wave_session lt48_pal_wave_session;
LT48_API lt48_status lt48_pal_wave_session_create(
    const lt48_pal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_pal_wave_session **out_session);
LT48_API void lt48_pal_wave_session_close(lt48_pal_wave_session *session);
LT48_API lt48_status lt48_pal_wave_session_enter_mode(
    lt48_pal_wave_session *session, uint32_t mode);
LT48_API lt48_status lt48_pal_wave_session_switch_mode(
    lt48_pal_wave_session *session, uint32_t mode);
LT48_API lt48_status lt48_pal_wave_session_finish_mode(
    lt48_pal_wave_session *session);
LT48_API lt48_status lt48_pal_wave_session_read_fuses(
    lt48_pal_wave_session *session, uint32_t mode,
    unsigned char *out_fuses, size_t capacity, size_t *out_completed_cells);
LT48_API lt48_status lt48_pal_wave_session_program_fuses(
    lt48_pal_wave_session *session, const unsigned char *fuses,
    size_t count, size_t *out_completed_cells);
LT48_API lt48_status lt48_pal_wave_session_verify_fuses(
    lt48_pal_wave_session *session, uint32_t mode,
    const unsigned char *expected, size_t count,
    lt48_palce_fuse_comparison *out_comparison);
LT48_API lt48_status lt48_pal_wave_session_blank_check(
    lt48_pal_wave_session *session,
    lt48_palce_fuse_comparison *out_comparison);
LT48_API int lt48_pal_wave_session_requires_cleanup(
    const lt48_pal_wave_session *session);
LT48_API lt48_status lt48_pal_wave_session_recover(
    lt48_pal_wave_session *session, lt48_palce_recovery_result *out_result);

LT48_API const lt48_fuse_code *lt48_pal_resources_fuse_code(
    const lt48_pal_resources *resources);

enum { LT48_PAL_CORRECTED_GEOMETRY=1u, LT48_PAL_POSITIVE_FUSE_SENSE=2u, LT48_PAL_MMI_ELECTRICAL=4u };
LT48_API unsigned lt48_pal_resources_authored_corrections(const lt48_pal_resources *resources);
/* On success, bytecode has library lifetime. NOT_FOUND leaves out_code intact.
 * Exact source digest and definition/family/pin-map binding are mandatory. */
LT48_API lt48_status lt48_pal_resources_corrected_fuse_code(lt48_rez *source,
    uint16_t definition, uint16_t family, uint16_t pinmap, lt48_fuse_code *out_code);

LT48_API int lt48_gal_resources_source_audited(const lt48_gal_resources *resources);
/* Ordered flat image regions: primary, optional E Field, optional User Code.
 * Offsets are byte-per-fuse offsets into complete operation inputs/results. */
LT48_API size_t lt48_gal_resources_fuse_context_count(const lt48_gal_resources *);
LT48_API const lt48_fuse_code *lt48_gal_resources_fuse_context(
    const lt48_gal_resources *,size_t,size_t *,uint16_t *);
LT48_API const lt48_fuse_code *lt48_gal_resources_fuse_code(
    const lt48_gal_resources *resources);

typedef struct lt48_pal_operation_result {
    lt48_status operation_status;
    lt48_status cleanup_status;
    size_t completed_cells;
    int programmed;
    int erased;
    int cleanup_completed;
    lt48_palce_fuse_comparison comparison;
    int secured;
    int read_protected; /* Source reports secured readback; image not committed. */
} lt48_pal_operation_result;
LT48_API int lt48_pal_resources_is_epm(const lt48_pal_resources *resources);
LT48_API int lt48_pal_resources_source_audited(const lt48_pal_resources *resources);
LT48_API lt48_status lt48_pal_execute_operation(
    const lt48_pal_resources *resources,
    const lt48_protocol_transport *transport, void *context,
    lt48_operation_kind operation, const unsigned char *input_fuses,
    size_t input_count, unsigned char *read_fuses, size_t read_capacity,
    lt48_pal_operation_result *out_result);

LT48_API lt48_status lt48_programmer_execute_pal_operation(
    lt48_programmer *programmer, const lt48_pal_resources *resources,
    lt48_operation_kind operation, const unsigned char *input_fuses, size_t input_count,
    unsigned char *read_fuses, size_t read_capacity, unsigned timeout_ms,
    lt48_pal_operation_result *out_result);


typedef enum lt48_gal_pes_kind {
    LT48_GAL_PES_ACCESS = 0,
    LT48_GAL_PES_ACCESS_PLSI = 1,
    LT48_GAL_PES_ACCESS_LV = 2
} lt48_gal_pes_kind;
typedef enum lt48_gal_pes_parameter {
    LT48_GAL_PES_VIEP = 0,
    LT48_GAL_PES_VIEV = 1,
    LT48_GAL_PES_VIEP1 = 2,
    LT48_GAL_PES_VIEV1 = 3,
    LT48_GAL_PES_TPWP = 4,
    LT48_GAL_PES_TPWP_BE = 5,
    LT48_GAL_PES_TPWV = 6,
    LT48_GAL_PES_VCCP = 7,
    LT48_GAL_PES_VIH = 8,
    LT48_GAL_PES_PARAMETER_COUNT = 9
} lt48_gal_pes_parameter;
typedef struct lt48_gal_pes_result {
    uint32_t updated_parameters;
    uint32_t values[LT48_GAL_PES_PARAMETER_COUNT];
    uint16_t original_status; /* Original identity failures: 5 or 6. */
} lt48_gal_pes_result;
struct lt48_gal_pes_binding {
    uint16_t operation;
    lt48_gal_pes_kind kind;
    uint8_t data_in_buffer;
    uint16_t manufacturer_variable;
    uint16_t device_variable;
    uint16_t old_parameter_variable;
    uint16_t parameter_variables[LT48_GAL_PES_PARAMETER_COUNT];
};
/* Resolve and validate the package-local PES callback and variable bindings
 * from the pinned Dataman13.10 GAL resource. This does not admit hardware or
 * prove complete nested operation closure. Missing branch variables fail. */
LT48_API lt48_status lt48_gal_pes_resolve_binding(
    const lt48_rez *rez, size_t family_index, lt48_gal_pes_binding *out_binding);
/* Pure class-2020 Dataman 13.10 parameter callback port. Values are original
 * variable magnitudes, not DAC bytes; untouched variables are absent from
 * the update mask. No USB, admission or implicit family binding. Unknown
 * legacy indices are rejected rather than reading adjacent executable data. */
LT48_API lt48_status lt48_gal_pes_decode(
    lt48_gal_pes_kind kind, const unsigned char *data_in, size_t size,
    uint8_t manufacturer, uint8_t device, uint8_t old_parameter_index,
    lt48_gal_pes_result *out_result);
/* Apply decoded magnitudes to an explicitly named family variable frame.
 * parameter_variables maps each PES parameter to its package-local variable
 * ID. Voltage updates change value; timing updates change channel (magnitude)
 * and retain value (unit index). Missing/duplicate IDs, incompatible kinds or
 * out-of-range magnitudes are rejected before any frame mutation. */
LT48_API lt48_status lt48_gal_pes_apply(
    const lt48_gal_pes_result *decoded,
    const uint16_t parameter_variables[LT48_GAL_PES_PARAMETER_COUNT],
    lt48_compile_variable *variables, size_t variable_count);

/* Independent best-effort failure cleanup through the original injected
 * callbacks. Drops the poisoned forward queue, ignores cancellation, attempts
 * every phase even after errors, and returns the first cleanup error. This
 * does not restore transport synchronization or authorize forward reuse.
 * The caller separately retains its original operation error and releases
 * its USB interface. Closing a session never substitutes for this cleanup. */
LT48_API lt48_status lt48_palce_wave_session_recover(
    lt48_palce_wave_session *session, lt48_palce_recovery_result *out_result);

/*
 * Build the complete proved M37 read transcript from one immutable snapshot.
 * The returned plan is offline and is not attached to a transport or USB.
 * The selected snapshot determines the procedure. Recovered EPROM ID
 * mismatch handling allows Abort/Retry/Ignore without changing selection.
 */
LT48_API lt48_status lt48_m37_resources_build_read_plan(
    const lt48_m37_resources *resources,
    uint32_t start,
    uint32_t length,
    lt48_protocol_plan **out_plan
);
/* Build an immutable offline Read plan for a digest-approved reference-match
 * snapshot that has not yet passed the live hardware gate.  This does not
 * authorize transport execution; the executor independently reapplies the
 * live gate before the first transaction. Dataman reference candidates retain
 * original ID-mismatch branching for controlled oracle comparisons; use the
 * selected-device builder above for operator workflows. */
LT48_API lt48_status lt48_m37_resources_build_offline_read_plan(
    const lt48_m37_resources *resources,
    uint32_t start,
    uint32_t length,
    lt48_protocol_plan **out_plan
);
/* Build the recovered mode-5 device Compare transcript.  Unlike Verify,
 * Compare reads one byte at a time with command 41 d0 and compares on the
 * host.  This builder is offline and performs no USB access. */
LT48_API lt48_status lt48_m37_resources_build_compare_plan(
    const lt48_m37_resources *resources,
    uint32_t start,
    const unsigned char *expected,
    size_t length,
    lt48_protocol_plan **out_plan
);
/* Evaluate bytes returned by mode-5 Compare.  A NULL decision callback means
 * continue after every mismatch.  The callback models the recovered Windows
 * "Error found, do you want to continue?" choice; Compare has no retry step. */
LT48_API lt48_status lt48_m37_compare_evaluate(
    uint32_t start,
    const unsigned char *expected,
    const unsigned char *actual,
    size_t length,
    lt48_m37_compare_decision_fn decide,
    void *decision_context,
    lt48_m37_compare_result *out_result
);
LT48_API int lt48_m37_resources_is_eep21(const lt48_m37_resources *resources);
/* Exact source-locked electrical erase, followed by a power-cycle
 * and full-array blank check. Other definitions are rejected. */
LT48_API lt48_status lt48_m37_resources_build_electrical_erase_plan(
    const lt48_m37_resources *resources, uint32_t start, uint32_t length,
    lt48_protocol_plan **out_plan);
LT48_API lt48_status lt48_m37_resources_build_blank_check_plan(
    const lt48_m37_resources *resources,
    uint32_t start,
    uint32_t length,
    lt48_protocol_plan **out_plan
);
LT48_API lt48_status lt48_m37_resources_build_verify_plan(
    const lt48_m37_resources *resources,
    uint32_t start,
    const unsigned char *expected,
    size_t length,
    lt48_protocol_plan **out_plan
);
LT48_API lt48_status lt48_m37_resources_build_verify_voltage_plan(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *expected, size_t length,
    uint16_t verify_vcc_centivolts, lt48_protocol_plan **out_plan
);
/* A margin plan set preserves independent initialization/cleanup per voltage
 * and a deterministic combined identity. It is offline and performs no USB. */
LT48_API lt48_status lt48_m37_verify_plan_set_create(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *expected, size_t length,
    const uint16_t *voltages, size_t voltage_count,
    lt48_m37_verify_plan_set **out_set
);
LT48_API void lt48_m37_verify_plan_set_close(
    lt48_m37_verify_plan_set *set
);
LT48_API size_t lt48_m37_verify_plan_set_count(
    const lt48_m37_verify_plan_set *set
);
LT48_API lt48_status lt48_m37_verify_plan_set_get(
    const lt48_m37_verify_plan_set *set, size_t index,
    uint16_t *out_voltage_centivolts, const lt48_protocol_plan **out_plan
);
LT48_API lt48_status lt48_m37_verify_plan_set_sha256(
    const lt48_m37_verify_plan_set *set,
    unsigned char digest[LT48_SHA256_SIZE]
);
/*
 * Independent powered phases: blank precheck, mode-2 programming, optional
 * Rapid mode 3, then nominal-VCC readback. Use plan_set_count, not a fixed 3.
 * This offline artifact does not itself authorize live programming.
 */
LT48_API lt48_status lt48_m37_program_plan_set_create(
    const lt48_m37_resources *resources, uint32_t start,
    const unsigned char *image, size_t length,
    lt48_m37_program_plan_set **out_set
);
LT48_API void lt48_m37_program_plan_set_close(
    lt48_m37_program_plan_set *set
);
LT48_API size_t lt48_m37_program_plan_set_count(
    const lt48_m37_program_plan_set *set
);
LT48_API lt48_status lt48_m37_program_plan_set_get(
    const lt48_m37_program_plan_set *set, size_t index,
    lt48_m37_operation *out_operation, const lt48_protocol_plan **out_plan
);
LT48_API lt48_status lt48_m37_program_plan_set_sha256(
    const lt48_m37_program_plan_set *set,
    unsigned char digest[LT48_SHA256_SIZE]
);
LT48_API lt48_status lt48_m37_resources_build_recovery_plan(
    const lt48_m37_resources *resources,
    lt48_m37_recovery_state state,
    lt48_protocol_plan **out_plan
);
/* Pure offline preparation shared by CLI and GUI; performs no USB access. */
LT48_API lt48_status lt48_m37_prepare_read(
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *read_plan,
    const char *selector,
    const char *output_path,
    uint8_t bus,
    uint8_t address,
    uint32_t start,
    uint32_t length,
    uint32_t timeout_ms,
    lt48_m37_read_preparation *out_preparation
);
/*
 * Execute every concrete recovery phase even after an earlier phase fails.
 * Transport cancellation is deliberately ignored. The release-interface
 * phase is the sole non-protocol action and must be supplied explicitly.
 */
LT48_API lt48_status lt48_m37_recovery_execute_injected(
    const lt48_protocol_plan *plan,
    const lt48_protocol_transport *transport,
    void *transport_context,
    const lt48_cleanup_action *release_interface,
    lt48_cleanup_result *phase_results,
    size_t phase_result_capacity,
    lt48_m37_recovery_execution_result *out_result
);
/*
 * Snapshot-bound read owner. It prebuilds all recovery states before I/O,
 * selects the state from authoritative read-plan progress after failure, and
 * always releases the interface. No native USB handle is accepted.
 */
LT48_API lt48_status lt48_m37_snapshot_read_execute_injected(
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *read_plan,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_m37_read_collector *collector,
    const lt48_cleanup_action *release_interface,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_snapshot_read_execution_result *out_result
);
/*
 * Execute a mode-5 Compare plan through injected transport, finalize the
 * programmer using the same state-aware owner as Read, and evaluate the
 * collected device bytes against the caller's immutable expected image.
 * This API owns no USB handle and does not enable frontend live execution.
 */
LT48_API lt48_status lt48_m37_compare_execute_injected(
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *compare_plan,
    uint32_t expected_start,
    const unsigned char *expected,
    size_t expected_size,
    lt48_m37_compare_decision_fn decide,
    void *decision_context,
    const lt48_protocol_transport *transport,
    void *transport_context,
    const lt48_cleanup_action *release_interface,
    lt48_cleanup_result *finalization_results,
    size_t finalization_result_capacity,
    lt48_m37_compare_execution_result *out_result
);
/*
 * Offline response collector derived from read-transaction metadata. Chunks
 * may be supplied in any order, exactly once each. The completed data buffer
 * is owned by the collector.
 */
LT48_API lt48_status lt48_m37_read_collector_create(
    const lt48_protocol_plan *plan,
    lt48_m37_read_collector **out_collector
);
LT48_API void lt48_m37_read_collector_close(
    lt48_m37_read_collector *collector
);
LT48_API lt48_status lt48_m37_read_collector_accept(
    lt48_m37_read_collector *collector,
    size_t transaction_index,
    const unsigned char *data,
    size_t size
);
LT48_API void lt48_m37_read_collector_progress(
    const lt48_m37_read_collector *collector,
    size_t *out_chunks_received,
    size_t *out_chunk_count,
    size_t *out_bytes_received,
    size_t *out_byte_count
);
LT48_API lt48_status lt48_m37_read_collector_data(
    const lt48_m37_read_collector *collector,
    uint32_t *out_start,
    const unsigned char **out_data,
    size_t *out_size
);
/*
 * Execute only through injected callbacks. On failure or cancellation, all
 * twelve prevalidated recovery actions run in order without cancellation.
 */
LT48_API lt48_status lt48_m37_read_execute_injected(
    const lt48_protocol_plan *plan,
    const lt48_protocol_transport *transport,
    void *transport_context,
    lt48_m37_read_collector *collector,
    const lt48_cleanup_action *recovery_actions,
    size_t recovery_action_count,
    lt48_cleanup_result *recovery_results,
    size_t recovery_result_capacity,
    lt48_m37_read_execution_result *out_result
);

/*
 * Run every cleanup phase in order without consulting operation cancellation.
 * Names remain borrowed from actions. Insufficient result capacity is detected
 * before any action runs.
 */
LT48_API lt48_status lt48_cleanup_run(
    const lt48_cleanup_action *actions,
    size_t action_count,
    lt48_cleanup_result *results,
    size_t result_capacity,
    size_t *out_result_count,
    size_t *out_failure_count
);
/* Validate cleanup ownership first, then run operation and always run cleanup. */
LT48_API lt48_status lt48_session_run(
    lt48_session_operation_fn operation,
    void *operation_context,
    const lt48_cleanup_action *cleanup_actions,
    size_t cleanup_action_count,
    lt48_cleanup_result *cleanup_results,
    size_t cleanup_result_capacity,
    lt48_session_result *out_session
);
LT48_API const char *lt48_progress_scope_name(lt48_progress_scope scope);
LT48_API const char *lt48_progress_state_name(lt48_progress_state_kind state);
LT48_API lt48_status lt48_progress_manifest_json(
    const unsigned char **out_json, size_t *out_size
);
/*
 * Serialize one payload-free event using the stable manifest field order.
 * out_size excludes the trailing NUL. A NULL buffer with zero capacity is a
 * size query; a non-NULL buffer requires capacity of at least out_size + 1.
 */
LT48_API lt48_status lt48_progress_event_json(
    const lt48_progress_event *event, unsigned char *buffer, size_t capacity,
    size_t *out_size
);
LT48_API lt48_status lt48_progress_emitter_create(
    lt48_progress_observer_fn observer, void *observer_context,
    lt48_progress_emitter **out_emitter
);
LT48_API void lt48_progress_emitter_close(lt48_progress_emitter *emitter);
LT48_API lt48_status lt48_progress_emit(
    lt48_progress_emitter *emitter,
    const lt48_progress_event_input *input,
    lt48_progress_event *out_event
);
LT48_API size_t lt48_progress_observer_error_count(
    const lt48_progress_emitter *emitter
);
LT48_API void lt48_progress_state_init(lt48_progress_state *state);
LT48_API lt48_status lt48_progress_state_consume(
    lt48_progress_state *state, const lt48_progress_event *event
);
LT48_API lt48_status lt48_m37_cleanup_recipe(
    int controller_initialized,
    int power_started,
    lt48_m37_cleanup_phase *phases,
    size_t phase_capacity,
    size_t *out_phase_count
);
LT48_API lt48_status lt48_m37_validate_plan(
    const lt48_m37_plan_evidence *evidence,
    uint64_t *out_issue_mask
);
/*
 * Authoritative gate: re-audit the selected installed definition, replace all
 * resource-derived assertions with those results, then validate the operation.
 */
LT48_API lt48_status lt48_installation_validate_m37_plan(
    const lt48_installation *installation,
    size_t device_index,
    const lt48_m37_plan_evidence *operation_evidence,
    uint64_t *out_issue_mask
);
/* Validate against one already-open immutable execution snapshot. */
LT48_API lt48_status lt48_m37_resources_validate_plan(
    const lt48_m37_resources *resources,
    const lt48_m37_plan_evidence *operation_evidence,
    uint64_t *out_issue_mask
);
LT48_API const char *lt48_m37_cleanup_phase_name(
    lt48_m37_cleanup_phase phase
);
LT48_API const char *lt48_m37_safety_issue_name(uint64_t single_issue_bit);

/*
 * Hardware-independent device image. Bytes are always stored in increasing
 * device-address order; no implicit word packing or byte-order conversion is
 * performed. Exact file import rejects both short and long input. Range APIs
 * reject overflow and preserve every byte outside the selected range.
 */
LT48_API lt48_status lt48_buffer_create(
    size_t size, unsigned char fill, lt48_buffer **out_buffer
);
/* Create a device-sized image with no fabricated bytes. A sparse buffer has
 * storage for every device offset, but reads, hashes, comparisons, saves and
 * mutating-input bindings reject ranges containing bytes not yet acquired or
 * explicitly written. */
LT48_API lt48_status lt48_buffer_create_sparse(
    size_t size, lt48_buffer **out_buffer
);
LT48_API lt48_status lt48_buffer_load_file_exact(
    const char *path, size_t expected_size, lt48_buffer **out_buffer
);
LT48_API lt48_status lt48_buffer_load_file(
    const char *path, const lt48_buffer_import_options *options,
    lt48_buffer_import_result *out_result, lt48_buffer **out_buffer
);
LT48_API lt48_status lt48_buffer_load_file_cancellable(
    const char *path, const lt48_buffer_import_options *options,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_buffer_import_result *out_result, lt48_buffer **out_buffer
);
LT48_API const char *lt48_image_format_name(lt48_image_format format);
/* Programming-image import. AUTO recognizes the documented filename suffixes
 * and validates the selected syntax. Address-bearing input is translated by
 * address_base into device offsets; raw and Straight HEX data begin there.
 * Out-of-range or overlapping data fails closed. PRESERVE produces a sparse
 * buffer; FILL materializes every gap with gap_fill_value. */
LT48_API lt48_status lt48_buffer_load_image(
    const char *path, const lt48_image_import_options *options,
    lt48_image_import_result *out_result, lt48_buffer **out_buffer
);
LT48_API lt48_status lt48_buffer_load_image_cancellable(
    const char *path, const lt48_image_import_options *options,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_image_import_result *out_result, lt48_buffer **out_buffer
);
/* Export a known resident-buffer range as raw binary, Intel HEX, Motorola
 * S-record, address-free Straight HEX, MOS Technology HEX, Tektronix HEX, or
 * HP64000 Absolute. Byte modes select interleaved lanes from the source range
 * and pack selected bytes contiguously in the output address space. */
LT48_API lt48_status lt48_buffer_save_image(
    const lt48_buffer *buffer, const char *path,
    const lt48_image_export_options *options,
    lt48_image_export_result *out_result
);
LT48_API lt48_status lt48_buffer_save_image_cancellable(
    const lt48_buffer *buffer, const char *path,
    const lt48_image_export_options *options,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_image_export_result *out_result
);
LT48_API void lt48_buffer_close(lt48_buffer *buffer);
LT48_API size_t lt48_buffer_size(const lt48_buffer *buffer);
/* Raw read-only storage view. Sparse-buffer callers must prove the intended
 * range with lt48_buffer_range_known() before interpreting these bytes. */
LT48_API const unsigned char *lt48_buffer_data(const lt48_buffer *buffer);
LT48_API size_t lt48_buffer_known_size(const lt48_buffer *buffer);
LT48_API lt48_status lt48_buffer_range_known(
    const lt48_buffer *buffer, size_t offset, size_t length, int *out_known
);
LT48_API lt48_status lt48_buffer_read(
    const lt48_buffer *buffer, size_t offset,
    unsigned char *destination, size_t length
);
LT48_API lt48_status lt48_buffer_write(
    lt48_buffer *buffer, size_t offset,
    const unsigned char *source, size_t length
);
/* Merge bytes acquired from a device at their absolute device offsets. */
LT48_API lt48_status lt48_buffer_write_acquired(
    lt48_buffer *buffer, size_t offset,
    const unsigned char *source, size_t length
);
LT48_API lt48_status lt48_buffer_fill(
    lt48_buffer *buffer, size_t offset, size_t length, unsigned char value
);
LT48_API lt48_status lt48_buffer_get_byte(
    const lt48_buffer *buffer, size_t offset, unsigned char *out_value
);
LT48_API lt48_status lt48_buffer_set_byte(
    lt48_buffer *buffer, size_t offset, unsigned char value
);
/* Explicit typed access to 1, 2, 4, or 8 bytes at a byte offset. The value
 * must fit the selected width; no host byte-order or alignment assumption is
 * made. */
LT48_API lt48_status lt48_buffer_get_word(
    const lt48_buffer *buffer, size_t offset, size_t width,
    lt48_buffer_byte_order order, uint64_t *out_value
);
LT48_API lt48_status lt48_buffer_set_word(
    lt48_buffer *buffer, size_t offset, size_t width,
    lt48_buffer_byte_order order, uint64_t value
);
LT48_API lt48_status lt48_buffer_checksum_range(
    const lt48_buffer *buffer, size_t offset, size_t length,
    lt48_buffer_checksum *out_checksum
);
LT48_API lt48_status lt48_buffer_checksum_range_cancellable(
    const lt48_buffer *buffer, size_t offset, size_t length,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_buffer_checksum *out_checksum
);
LT48_API lt48_status lt48_buffer_compare_range(
    const lt48_buffer *buffer, size_t offset,
    const unsigned char *expected, size_t length,
    lt48_buffer_compare_result *out_result
);
LT48_API lt48_status lt48_buffer_compare_range_cancellable(
    const lt48_buffer *buffer, size_t offset,
    const unsigned char *expected, size_t length,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_buffer_compare_result *out_result
);
LT48_API lt48_status lt48_buffer_get_provenance(
    const lt48_buffer *buffer, lt48_buffer_provenance *out_provenance
);
LT48_API lt48_status lt48_buffer_bind_mutating_input(
    const lt48_buffer *buffer, size_t definition_capacity,
    size_t range_offset, size_t range_length,
    lt48_buffer_binding *out_binding
);
LT48_API lt48_status lt48_buffer_save_file_range(
    const lt48_buffer *buffer, const char *path,
    size_t offset, size_t length, int overwrite
);
/* Cooperative cancellation is available for exclusive creation. Overwrite
 * plus cancellation is rejected because an interrupted overwrite cannot be
 * made transactional by this API. */
LT48_API lt48_status lt48_buffer_save_file_range_cancellable(
    const lt48_buffer *buffer, const char *path,
    size_t offset, size_t length, int overwrite,
    lt48_cancel_check_fn cancelled, void *cancel_context
);

/* Hardware-independent Function Test Vectors document. A new document starts
 * with one all-N row and an all-N clipboard, matching the recovered Windows
 * editor initialization. Insert places the clipboard before current_row;
 * copy updates the clipboard; cut copies then removes the current row. Undo
 * restores the most recently changed row contents. */
LT48_API lt48_status lt48_test_vectors_create(
    size_t pin_count, lt48_test_vectors **out_vectors
);
LT48_API void lt48_test_vectors_close(lt48_test_vectors *vectors);
LT48_API int lt48_test_vector_symbol_valid(char symbol);
LT48_API lt48_status lt48_test_vectors_get_info(
    const lt48_test_vectors *vectors, lt48_test_vector_info *out_info
);
LT48_API lt48_status lt48_test_vectors_get_row(
    const lt48_test_vectors *vectors, size_t row,
    char *destination, size_t destination_size
);
LT48_API lt48_status lt48_test_vectors_get_clipboard(
    const lt48_test_vectors *vectors,
    char *destination, size_t destination_size
);
LT48_API lt48_status lt48_test_vectors_set_row(
    lt48_test_vectors *vectors, size_t row, const char *symbols
);
LT48_API lt48_status lt48_test_vectors_set_current(
    lt48_test_vectors *vectors, size_t row
);
LT48_API lt48_status lt48_test_vectors_insert(lt48_test_vectors *vectors);
LT48_API lt48_status lt48_test_vectors_copy(lt48_test_vectors *vectors);
LT48_API lt48_status lt48_test_vectors_cut(lt48_test_vectors *vectors);
LT48_API lt48_status lt48_test_vectors_undo(lt48_test_vectors *vectors);

/* Offline JTAG TAP planning. Plans contain logical rising-edge steps only;
 * they do not select socket pins, voltage levels, or a hardware transport. */
LT48_API const char *lt48_jtag_tap_state_name(lt48_jtag_tap_state state);
LT48_API lt48_status lt48_jtag_plan_scan(
    const lt48_jtag_scan_request *request, lt48_jtag_plan **out_plan
);
LT48_API void lt48_jtag_plan_close(lt48_jtag_plan *plan);
LT48_API lt48_status lt48_jtag_plan_get_info(
    const lt48_jtag_plan *plan, lt48_jtag_plan_info *out_info
);
LT48_API lt48_status lt48_jtag_plan_get_step(
    const lt48_jtag_plan *plan, size_t index, lt48_jtag_step *out_step
);
LT48_API lt48_status lt48_jtag_plan_json(
    const lt48_jtag_plan *plan, const unsigned char **out_json,
    size_t *out_size
);
/* IEEE 1149.1 reset recovery: five consecutive rising TCK edges with TMS=1
 * reach TEST_LOGIC_RESET from every valid TAP state. */
LT48_API lt48_status lt48_jtag_cancel_recovery(
    lt48_jtag_tap_state start_state, lt48_jtag_step *steps,
    size_t step_capacity, size_t *out_step_count
);
LT48_API lt48_status lt48_jtag_evaluate_result(
    const lt48_jtag_result_request *request, lt48_jtag_result *out_result
);
/* Decode a caller-bounded reset/IDCODE scan. A zero first bit is one BYPASS
 * element; a one first bit begins a 32-bit IDCODE, all LSB-first. */
LT48_API lt48_status lt48_jtag_decode_chain(
    const unsigned char *tdo, size_t tdo_size, size_t bit_count,
    size_t expected_element_count, lt48_jtag_chain_element *elements,
    size_t element_capacity, lt48_jtag_chain_report *out_report
);
LT48_API const char *lt48_jtag_profile_issue_name(
    lt48_jtag_profile_issue issue
);
LT48_API lt48_status lt48_jtag_validate_target_profile(
    const lt48_jtag_target_profile *profile,
    lt48_jtag_profile_report *out_report
);

/* Strict supported-schema, payload-free operation-report inspection. */
LT48_API void lt48_operation_report_set_programmer_identity(
    const char *model, const char *serial, unsigned bus_number,
    unsigned device_address, unsigned product_id,
    unsigned device_version_bcd
);
LT48_API lt48_status lt48_operation_report_open(
    const char *path, lt48_operation_report **out_report
);
LT48_API void lt48_operation_report_close(lt48_operation_report *report);
LT48_API lt48_status lt48_operation_report_get_info(
    const lt48_operation_report *report, lt48_operation_report_info *out_info
);
LT48_API lt48_status lt48_operation_report_json(
    const lt48_operation_report *report,
    const unsigned char **out_json, size_t *out_size
);
LT48_API lt48_status lt48_operation_report_write_planned(
    const char *path, const lt48_operation_report_plan *plan
);
LT48_API lt48_status lt48_operation_report_write_result(
    const char *path,
    const lt48_operation_report_plan *plan,
    const lt48_operation_report_result *result
);
LT48_API lt48_status lt48_auto_id_eprom_operation_report_write_planned(
    const char *path,
    const lt48_auto_id_eprom_preparation *preparation
);
LT48_API lt48_status lt48_auto_id_eprom_operation_report_write_execution(
    const char *path,
    const lt48_auto_id_eprom_preparation *preparation,
    const lt48_auto_id_eprom_execution_result *execution
);
/* Strict schema-1 transport traces. A present trace_sha256 is verified over
 * canonical JSON with that root member omitted. Metadata is preserved as
 * canonical JSON and event sequences must be contiguous. */
LT48_API lt48_status lt48_trace_open(
    const char *path, lt48_transfer_trace **out_trace
);
LT48_API void lt48_trace_close(lt48_transfer_trace *trace);
LT48_API lt48_status lt48_trace_get_info(
    const lt48_transfer_trace *trace, lt48_trace_info *out_info
);
LT48_API lt48_status lt48_trace_get_event(
    const lt48_transfer_trace *trace, size_t index, lt48_trace_event *out_event
);
LT48_API lt48_status lt48_trace_compare(
    const lt48_transfer_trace *left, const lt48_transfer_trace *right,
    lt48_trace_comparison *out_comparison
);
/* Build and write a planned report from one audited M37 snapshot. */
LT48_API lt48_status lt48_m37_operation_report_write_planned(
    const char *path,
    const lt48_installation *installation,
    size_t device_index,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *protocol_plan,
    lt48_m37_operation operation,
    uint32_t address_start,
    uint32_t address_length,
    const unsigned char *input_image_sha256
);
/* Write one payload-free Verify report for a complete, ordered margin plan.
 * The report plan digest binds every pass digest and requested voltage. */
LT48_API lt48_status lt48_m37_operation_report_write_verify_plan_set(
    const char *path,
    const lt48_installation *installation,
    size_t device_index,
    const lt48_m37_resources *resources,
    const lt48_m37_verify_plan_set *plan_set,
    uint32_t address_start,
    uint32_t address_length,
    const unsigned char input_image_sha256[LT48_SHA256_SIZE]
);
/* Write one payload-free Program report for every ordered plan phase.
 * The digest binds blank precheck, programming (including Rapid mode 3), and
 * mandatory readback; the image is represented only by its SHA-256 digest. */
LT48_API lt48_status lt48_m37_operation_report_write_program_plan_set(
    const char *path,
    const lt48_installation *installation,
    size_t device_index,
    const lt48_m37_resources *resources,
    const lt48_m37_program_plan_set *plan_set,
    uint32_t address_start,
    uint32_t address_length,
    const unsigned char input_image_sha256[LT48_SHA256_SIZE]
);
LT48_API lt48_status lt48_m37_operation_report_write_execution(
    const char *path,
    const lt48_installation *installation,
    size_t device_index,
    const lt48_m37_resources *resources,
    const lt48_protocol_plan *protocol_plan,
    uint32_t address_start,
    uint32_t address_length,
    lt48_status command_status,
    const lt48_m37_snapshot_read_execution_result *execution,
    const lt48_cleanup_result *finalization_results,
    size_t finalization_result_count,
    const unsigned char *output,
    size_t output_size
);

/* Source-audited Dataman GAD 2716/2732, X20C04 NVRAM and GA Philips 82S/82LS
 * compact-wave engines. NVRAM writes include STORE and fresh-power readback. x4 PROM images use one low-nibble byte per address. No hardware
 * qualification is implied. Opening resources and validating ranges use no USB. */
typedef struct lt48_small_eprom_resources lt48_small_eprom_resources;
typedef struct lt48_small_eprom_result {
    lt48_status operation_status, cleanup_status;
    size_t completed_bytes, mismatch_count;
    uint32_t first_mismatch;
    int cleanup_completed;
} lt48_small_eprom_result;
/* Source-bound W27 dynamic erase; whole-chip only, mandatory fresh blank check. */
LT48_API int lt48_m37_has_dynamic_erase(const lt48_m37_resources *);
LT48_API lt48_status lt48_m37_dynamic_erase_validate(const lt48_m37_resources *,uint32_t,uint32_t);
LT48_API lt48_status lt48_m37_dynamic_erase_execute(const lt48_m37_resources *,const lt48_protocol_transport *,void *,uint32_t,uint32_t,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_eep21_erase(lt48_programmer *,const lt48_m37_resources *,uint32_t,uint32_t,unsigned,lt48_small_eprom_result *);

LT48_API lt48_status lt48_small_eprom_open(const lt48_installation *, size_t,
    lt48_small_eprom_resources **);
LT48_API void lt48_small_eprom_close(lt48_small_eprom_resources *);
/* Timekeepers: 1=DS1386, 2=DS1486, 3=ST top-eight, 4=DS164x,
 * 5=DS12887-compatible bank-zero RTC, 6=ST M48T129 top-sixteen.
 * Array writes/checks skip registers; Read snapshots them. region() returns
 * the exact file offset and count for Configuration. Status flags are
 * read-only. Phantom clocks are outside the RAM image. No HW test implied. */
LT48_API int lt48_small_eprom_is_hc711_parallel(const lt48_small_eprom_resources *);
LT48_API int lt48_small_eprom_is_st62(const lt48_small_eprom_resources *resources);
/* ST62 Configuration returns two decoded option bytes; only bits in mask()
 * are meaningful. Selected bits are set permanently; unselected preserved. */
LT48_API size_t lt48_small_eprom_hyundai13_configuration_size(const lt48_small_eprom_resources*);
lt48_status lt48_small_eprom_set_hyundai13_extended_configuration(lt48_small_eprom_resources*,const unsigned char*,size_t);
unsigned lt48_small_eprom_is_sm2965four(const lt48_small_eprom_resources*);
lt48_status lt48_small_eprom_set_sm2965four_configuration(lt48_small_eprom_resources*,unsigned mask,unsigned value);
unsigned lt48_small_eprom_is_elan5(const lt48_small_eprom_resources *resources);
lt48_status lt48_small_eprom_set_elan5_configuration(lt48_small_eprom_resources *resources,uint16_t clear_first,uint16_t clear_second);
unsigned lt48_small_eprom_is_syntek4(const lt48_small_eprom_resources *resources);
lt48_status lt48_small_eprom_set_syntek4_configuration(lt48_small_eprom_resources *resources,unsigned clear_bits);
unsigned lt48_small_eprom_is_scenix6(const lt48_small_eprom_resources*);
lt48_status lt48_small_eprom_set_scenix6_configuration(lt48_small_eprom_resources*,uint16_t,uint16_t);
uint16_t lt48_small_eprom_hyundai13_option_mask(const lt48_small_eprom_resources*);
LT48_API lt48_status lt48_small_eprom_set_hyundai13_configuration(lt48_small_eprom_resources*,uint16_t);
LT48_API uint16_t lt48_small_eprom_st62_option_mask(const lt48_small_eprom_resources*);
LT48_API lt48_status lt48_small_eprom_set_st62_configuration(lt48_small_eprom_resources*,uint16_t set_bits);

LT48_API int lt48_small_eprom_is_zilog70(const lt48_small_eprom_resources *);
/* Z86E61/E63 explicit OTP RAM-protection write choice; no state readback. */
LT48_API int lt48_small_eprom_is_z8tail(const lt48_small_eprom_resources*);
LT48_API lt48_status lt48_small_eprom_set_z8_ram_protection(lt48_small_eprom_resources*,unsigned enable);
LT48_API int lt48_small_eprom_is_zilog_word(const lt48_small_eprom_resources *resources);
/* Nine MUZE active-high choices. CONFIG start=0/length=2 returns raw OTP word
 * through optional read output; unselected and reserved cells are preserved. */
lt48_status lt48_small_eprom_set_zilog_configuration(lt48_small_eprom_resources *resources,
    uint16_t mask,uint16_t values);

LT48_API int lt48_small_eprom_is_cypress55(const lt48_small_eprom_resources *);
LT48_API int lt48_small_eprom_is_holtek19(const lt48_small_eprom_resources *resources);
LT48_API int lt48_small_eprom_is_cop48(const lt48_small_eprom_resources *resources);
LT48_API int lt48_small_eprom_is_compact40(const lt48_small_eprom_resources *resources);
int lt48_small_eprom_is_at89(const lt48_small_eprom_resources *resources);
LT48_API lt48_status lt48_small_eprom_set_protection(lt48_small_eprom_resources *resources, unsigned level);
LT48_API lt48_status lt48_timekeeper_region(const lt48_small_eprom_resources *, uint32_t *, size_t *);
LT48_API int lt48_small_eprom_is_timekeeper(const lt48_small_eprom_resources *);
LT48_API lt48_status lt48_timekeeper_validate_registers(int, const unsigned char *, size_t);
LT48_API int lt48_small_eprom_is_sram(const lt48_small_eprom_resources *resources);
LT48_API int lt48_small_eprom_is_eeprom(const lt48_small_eprom_resources *resources);
LT48_API int lt48_small_eprom_is_mcm68766(const lt48_small_eprom_resources *);
LT48_API int lt48_small_eprom_is_nvram(const lt48_small_eprom_resources *);
LT48_API uint32_t lt48_small_eprom_capacity(const lt48_small_eprom_resources *);
LT48_API lt48_status lt48_small_eprom_validate(const lt48_small_eprom_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t);
LT48_API lt48_status lt48_small_eprom_execute(const lt48_small_eprom_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t,
    size_t, const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, lt48_m37_id_decision_fn, void *,
    lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_small_eprom(lt48_programmer *,
    const lt48_small_eprom_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, lt48_m37_id_decision_fn, void *, unsigned,
    lt48_small_eprom_result *);

/* Digest-bound GA serial owner: Microwire 93C and X24C44 NVRAM.
 * Byte ranges/images; x16 words are LE. 93C provides whole-device Erase.
 * NVRAM is cleared with FF Program, including STORE and persistence checking. */
typedef struct lt48_compact_i2c_resources lt48_compact_i2c_resources;
typedef lt48_small_eprom_result lt48_compact_i2c_result;
LT48_API lt48_status lt48_compact_i2c_open(const lt48_installation *,size_t,lt48_compact_i2c_resources **);
LT48_API void lt48_compact_i2c_close(lt48_compact_i2c_resources *);
LT48_API lt48_status lt48_compact_i2c_validate_verify_options(const lt48_compact_i2c_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API int lt48_compact_i2c_has_protection(const lt48_compact_i2c_resources *);
/* Secure protects 0000..007f; this source procedure provides no unlock. */
LT48_API int lt48_compact_i2c_has_half_protection(const lt48_compact_i2c_resources *);
/* 24C65: Configuration takes one HE block byte; Secure takes start,count. */
LT48_API int lt48_compact_i2c_has_endurance_config(const lt48_compact_i2c_resources *);
/* SPD control payload is one mask byte: four block flags, or STTS PWP/RSWP. */
/* AT17 kind: 1 small, 2 single-bank large, 3 dual-bank 020, 4 RAM-applet 002/040.
 * Configuration mask: RESET active-high bit(s) first, one per bank;
 * then oscillator enabled, only on source-supported A variants. */
LT48_API unsigned lt48_compact_i2c_at17_configuration_bits(const lt48_compact_i2c_resources *resources);
LT48_API int lt48_compact_i2c_is_at17(const lt48_compact_i2c_resources *resources);
unsigned lt48_compact_i2c_spd_protection_bits(const lt48_compact_i2c_resources *);
LT48_API uint32_t lt48_compact_i2c_capacity(const lt48_compact_i2c_resources *);
LT48_API lt48_status lt48_compact_i2c_validate(const lt48_compact_i2c_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_compact_i2c_execute(const lt48_compact_i2c_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_compact_i2c_result *);
LT48_API lt48_status lt48_programmer_execute_compact_i2c(lt48_programmer *,const lt48_compact_i2c_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_compact_i2c_result *);
/* GC/GCB POF logic. Offline open/file support is available without the
 * private runtime. Validate reports LT48_NOT_FOUND when it must be imported.
 * This API does not by itself admit entries to the supported-device list. */
typedef struct lt48_pof_device lt48_pof_device;
/* Owned optional file metadata. Initialize to zero; clear when finished.
 * vectors contains row_count NUL-terminated rows of pin_count characters.
 * Preserving these rows does not execute a functional test. */
typedef struct lt48_pof_file_metadata {
    int security;
    uint32_t pin_count, row_count;
    char *vectors;
} lt48_pof_file_metadata;
LT48_API void lt48_pof_file_metadata_clear(lt48_pof_file_metadata *);
LT48_API lt48_status lt48_pof_device_import_metadata(const lt48_pof_device *,const unsigned char *,size_t,unsigned char *,size_t,lt48_pof_file_metadata *);
LT48_API lt48_status lt48_pof_device_export_metadata(const lt48_pof_device *,const unsigned char *,size_t,const lt48_pof_file_metadata *,unsigned char *,size_t,size_t *);

/* Functional test: margin 0 = +/-5%, 1 = +/-10%, 2 = nominal;
 * x_mode 0 = drive low, 1 = drive high, 2 = float for X positions.
 * Rows must match the selected socket pin width and remain valid throughout
 * the synchronous call. Stops at the first mismatch; row indices are zero
 * based, SIZE_MAX means none. completed_rows includes both margin passes.
 * Validation performs no I/O. Execution owns preparation and cleanup. */
typedef struct lt48_vector_cycle_result {
    char actual[49], errors[49];
    size_t mismatch_count;
} lt48_vector_cycle_result;
typedef struct lt48_vector_operation_result {
    lt48_status operation_status,cleanup_status;
    size_t completed_rows,first_mismatch_row;
    unsigned completed_passes,first_mismatch_centivolts;
    int cleanup_completed;
    lt48_vector_cycle_result mismatch;
} lt48_vector_operation_result;
LT48_API lt48_status lt48_pof_device_validate_vectors(const lt48_pof_device *,const lt48_pof_file_metadata *,unsigned,unsigned);
LT48_API lt48_status lt48_pof_device_execute_vectors(const lt48_pof_device *,const lt48_pof_file_metadata *,unsigned,unsigned,const lt48_protocol_transport *,void *,lt48_vector_operation_result *);
LT48_API lt48_status lt48_programmer_execute_pof_vectors(lt48_programmer *,const lt48_pof_device *,const lt48_pof_file_metadata *,unsigned,unsigned,unsigned,lt48_vector_operation_result *);

typedef struct lt48_pof_core_result {
    size_t exchanges, completed_bytes, first_mismatch;
    uint16_t expected, actual;
} lt48_pof_core_result;
typedef struct lt48_pof_operation_result {
    lt48_status operation_status,cleanup_status;
    lt48_pof_core_result traversal;
    unsigned verify_passes;
    int programmed,secured,cleanup_completed;
} lt48_pof_operation_result;
LT48_API lt48_status lt48_pof_device_open(const lt48_installation *,size_t,lt48_pof_device **);
LT48_API void lt48_pof_device_close(lt48_pof_device *);
LT48_API size_t lt48_pof_device_capacity(const lt48_pof_device *);
/* Definition blank byte for initializing a packed POF image. */
LT48_API lt48_status lt48_pof_device_blank_value(const lt48_pof_device *,unsigned char *);
LT48_API lt48_status lt48_pof_device_runtime_status(const lt48_pof_device *);
LT48_API lt48_status lt48_pof_device_validate(const lt48_pof_device *,lt48_operation_kind,const unsigned char *,size_t);
LT48_API lt48_status lt48_pof_device_execute(const lt48_pof_device *,const lt48_protocol_transport *,void *,lt48_operation_kind,const unsigned char *,size_t,unsigned char *,size_t,lt48_pof_operation_result *);
LT48_API lt48_status lt48_programmer_execute_pof_device(lt48_programmer *,const lt48_pof_device *,lt48_operation_kind,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_pof_operation_result *);
LT48_API lt48_status lt48_pof_device_import(const lt48_pof_device *,const unsigned char *,size_t,unsigned char *,size_t,int *);
LT48_API lt48_status lt48_pof_device_export(const lt48_pof_device *,const unsigned char *,size_t,int,unsigned char *,size_t,size_t *);

typedef struct lt48_config_memory_resources lt48_config_memory_resources;
typedef struct lt48_config_memory_result {
    lt48_status operation_status,cleanup_status;
    size_t completed_bytes,mismatch_count;
    uint32_t first_mismatch;
    int cleanup_completed,configuration_valid;
    unsigned configuration; uint32_t usercode;
} lt48_config_memory_result;
LT48_API lt48_status lt48_config_memory_open(const lt48_installation *,size_t,lt48_config_memory_resources **);
LT48_API void lt48_config_memory_close(lt48_config_memory_resources *);
LT48_API unsigned lt48_config_memory_configuration_bits(const lt48_config_memory_resources *);
LT48_API uint32_t lt48_config_memory_capacity(const lt48_config_memory_resources *);
LT48_API lt48_status lt48_config_memory_validate(const lt48_config_memory_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_config_memory_validate_verify_options(const lt48_config_memory_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_config_memory_execute(const lt48_config_memory_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_config_memory_result *);
LT48_API lt48_status lt48_programmer_execute_config_memory(lt48_programmer *,const lt48_config_memory_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_config_memory_result *);
LT48_API lt48_status lt48_config_memory_pof_import(const lt48_config_memory_resources *,const unsigned char *,size_t,unsigned char *,size_t);
LT48_API lt48_status lt48_config_memory_pof_export(const lt48_config_memory_resources *,const unsigned char *,size_t,unsigned char *,size_t,size_t *);
LT48_API unsigned lt48_config_memory_configuration_mask(const lt48_config_memory_resources *);

typedef struct lt48_xc17_resources lt48_xc17_resources;
typedef struct lt48_xc17_result {
    lt48_status operation_status,cleanup_status;
    size_t completed_bytes,mismatch_count;
    uint32_t first_mismatch;
    int cleanup_completed,configuration_valid;
    unsigned configuration;
} lt48_xc17_result;
LT48_API lt48_status lt48_xc17_open(const lt48_installation *,size_t,lt48_xc17_resources **);
LT48_API void lt48_xc17_close(lt48_xc17_resources *);
LT48_API unsigned lt48_xc17_configuration_bits(const lt48_xc17_resources *);
LT48_API uint32_t lt48_xc17_capacity(const lt48_xc17_resources *);
LT48_API lt48_status lt48_xc17_validate(const lt48_xc17_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_xc17_validate_verify_options(const lt48_xc17_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_xc17_execute(const lt48_xc17_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_xc17_result *);
LT48_API lt48_status lt48_programmer_execute_xc17(lt48_programmer *,const lt48_xc17_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_xc17_result *);
typedef struct lt48_avr_resources lt48_avr_resources;
typedef struct lt48_avr_result {
    lt48_status operation_status,cleanup_status;
    size_t completed_bytes,mismatch_count;
    uint32_t first_mismatch;
    int cleanup_completed,configuration_valid;
    unsigned lock_mode,spien,aux_fuse;
    int fuse_byte_valid; unsigned fuse_byte,calibration;
    unsigned fuse_byte_count,fuse_high,fuse_extended;
    unsigned boot_lock[2]; /* ATmega BLB0/BLB1 source dropdown values. */
} lt48_avr_result;
/* Fuse values: -1 preserve, 0 disabled, 1 enabled. Applied by Program. */
LT48_API lt48_status lt48_avr_set_fuses(lt48_avr_resources *,int spien,int aux_fuse);
/* ATtiny12 full fuse byte: -1 preserves every bit. */
/* SPIEN is not writable in the 2323/2343 SPI procedure. */
LT48_API int lt48_avr_has_spien(const lt48_avr_resources *);
LT48_API unsigned lt48_avr_nominal_centivolts(const lt48_avr_resources *);
LT48_API int lt48_avr_is_atmega(const lt48_avr_resources *);
LT48_API int lt48_avr_has_boot_locks(const lt48_avr_resources *resources);
LT48_API unsigned lt48_avr_fuse_byte_count(const lt48_avr_resources *);
/* ATmega boot lock source selections: -1 preserve, 0 unlocked, 1..3 source modes. */
LT48_API lt48_status lt48_avr_set_boot_locks(lt48_avr_resources *,int,int);
/* Physical LOW/HIGH[/EXT] bytes: -1 preserves. For two-byte parts EXT
 * must be -1; query lt48_avr_fuse_byte_count before composing edits. */
LT48_API lt48_status lt48_avr_set_fuse_bytes(lt48_avr_resources *,int low,int high,int extended);
LT48_API int lt48_avr_has_fuse_byte(const lt48_avr_resources *);
LT48_API lt48_status lt48_avr_set_fuse_byte(lt48_avr_resources *,int value);
/* Auxiliary fuse: EESAVE on ATmega; RCEN on1200, RSTDISBL on tiny12, otherwise FSTRT. */
LT48_API const char *lt48_avr_aux_fuse_name(const lt48_avr_resources *);
LT48_API lt48_status lt48_avr_open(const lt48_installation *,size_t,lt48_avr_resources **);
LT48_API void lt48_avr_close(lt48_avr_resources *);
LT48_API lt48_status lt48_avr_validate_verify_options(const lt48_avr_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API uint32_t lt48_avr_capacity(const lt48_avr_resources *);
LT48_API lt48_status lt48_avr_validate(const lt48_avr_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_avr_execute(const lt48_avr_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_avr_result *);
LT48_API lt48_status lt48_programmer_execute_avr(lt48_programmer *,const lt48_avr_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_avr_result *);
typedef struct lt48_pic_resources lt48_pic_resources;
typedef lt48_small_eprom_result lt48_pic_result;
LT48_API lt48_status lt48_pic_open(const lt48_installation *,size_t,lt48_pic_resources **);
LT48_API void lt48_pic_close(lt48_pic_resources *);
LT48_API lt48_status lt48_pic_validate_verify_options(const lt48_pic_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API uint32_t lt48_pic_capacity(const lt48_pic_resources *);
typedef struct lt48_pic_legacy16_policy_info {
 uint32_t code_words,eeprom_bytes;
 uint16_t protection_mask,maximum_verify_centivolts;
 int program_erases_first,over_program_clears_only;
} lt48_pic_legacy16_policy_info;
LT48_API lt48_status lt48_pic_legacy16_policy(const lt48_pic_resources *,lt48_pic_legacy16_policy_info *);
/* Whole-image metadata for the source-audited 14-bit OTP owner. No I/O. */
typedef struct lt48_pic_otp14_layout_info {
 uint32_t image_bytes,code_words,calibration_word,calibration_words;
} lt48_pic_otp14_layout_info;
LT48_API lt48_status lt48_pic_otp14_layout(const lt48_pic_resources *,lt48_pic_otp14_layout_info *);
typedef struct lt48_pic_configuration_word {
 uint32_t image_offset;
 uint16_t value_mask;
} lt48_pic_configuration_word;
LT48_API uint16_t lt48_pic_configuration_encoding_mask(const lt48_pic_resources *);
LT48_API size_t lt48_pic_configuration_word_count(const lt48_pic_resources *);
LT48_API lt48_status lt48_pic_configuration_word_info(const lt48_pic_resources *,size_t,lt48_pic_configuration_word *);
/* Source-layout metadata for the baseline 12-bit engine. Other engines return
 * UNSUPPORTED. This read-only query does not grant device admission. */
typedef struct lt48_pic_baseline12_layout_info {
 uint32_t image_bytes,code_words,data_words,user_id_word,configuration_word;
 uint32_t calibration_word,backup_word,backup_words;
 uint16_t element_bits,unused_word;
} lt48_pic_baseline12_layout_info;
LT48_API lt48_status lt48_pic_baseline12_layout(
 const lt48_pic_resources *,lt48_pic_baseline12_layout_info *);

/* Read-only authored midrange image layout. Other PIC engines return UNSUPPORTED. */
typedef struct lt48_pic_midrange14_layout_info {
 uint32_t image_bytes,code_words,user_id_word,configuration_word,identity_word;
 uint32_t revision_word,calibration_word,calibration_words;
 uint16_t configuration_mask,identity_mask,identity_value;
} lt48_pic_midrange14_layout_info;
LT48_API lt48_status lt48_pic_midrange14_layout(const lt48_pic_resources *,lt48_pic_midrange14_layout_info *);

LT48_API lt48_status lt48_pic_validate(const lt48_pic_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_pic_execute(const lt48_pic_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_pic_result *);
LT48_API lt48_status lt48_programmer_execute_pic(lt48_programmer *,const lt48_pic_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_pic_result *);
/* Authored functional carrier facts; do not imply operation admission or
 * manufactured continuity. Pin numbers are one-based. */
typedef struct lt48_device_carrier_info {
 const char *physical_package,*note;
 unsigned physical_pins,carrier_pins,dip1_zif;
 size_t connection_count;
 int requires_confirmation;
} lt48_device_carrier_info;
typedef struct lt48_device_carrier_connection {
 unsigned physical_pin,carrier_pin;const char *function;
} lt48_device_carrier_connection;
LT48_API lt48_status lt48_installation_device_carrier_info(const lt48_installation *,size_t,lt48_device_carrier_info *);
LT48_API lt48_status lt48_installation_device_carrier_connection(const lt48_installation *,size_t,size_t,lt48_device_carrier_connection *);

/* PIC24FJ source-bound whole-image C engine. Configuration is represented
 * by low16 words in four-byte instruction slots at the Flash image tail.
 * Inventory admission is a separate installation/GUI decision. */
typedef struct lt48_pic24_resources lt48_pic24_resources;
typedef lt48_small_eprom_result lt48_pic24_result;
typedef struct lt48_pic24_execution_options {
 unsigned startup_checks; /* source flags: ID=2, forced ID on erase=0x80000 */
 lt48_m37_id_decision_fn id_decision;void *id_context;
} lt48_pic24_execution_options;
typedef struct lt48_pic24_diagnostics {
 int configuration_readback_confirmed,protection_write_completed;
 uint16_t observed_manufacturer,observed_device;
} lt48_pic24_diagnostics;
LT48_API lt48_status lt48_pic24_open(const lt48_installation *,size_t,lt48_pic24_resources **);
LT48_API void lt48_pic24_close(lt48_pic24_resources *);
LT48_API uint32_t lt48_pic24_capacity(const lt48_pic24_resources *);
LT48_API lt48_status lt48_pic24_configuration_get(const lt48_pic24_resources *,const unsigned char *,size_t,lt48_runtime_configuration_value *,size_t,size_t *);
LT48_API lt48_status lt48_pic24_configuration_set(const lt48_pic24_resources *,unsigned char *,size_t,const lt48_runtime_configuration_value *,size_t);
LT48_API lt48_status lt48_pic24_validate(const lt48_pic24_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_pic24_validate_verify_options(const lt48_pic24_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_pic24_execute(const lt48_pic24_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,const lt48_pic24_execution_options *,lt48_pic24_diagnostics *,lt48_pic24_result *);

LT48_API lt48_status lt48_programmer_execute_pic24(lt48_programmer *,const lt48_pic24_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,const lt48_pic24_execution_options *,lt48_pic24_diagnostics *,unsigned,lt48_pic24_result *);

typedef struct lt48_pic18_resources lt48_pic18_resources;
typedef lt48_small_eprom_result lt48_pic18_result;
LT48_API lt48_status lt48_pic18_open(const lt48_installation *,size_t,lt48_pic18_resources **);
LT48_API void lt48_pic18_close(lt48_pic18_resources *);
LT48_API lt48_status lt48_pic18_validate_verify_options(const lt48_pic18_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API uint32_t lt48_pic18_capacity(const lt48_pic18_resources *);
/* Pure buffer configuration conversion; no programmer transport. */
LT48_API lt48_status lt48_pic18_configuration_values(const lt48_pic18_resources *,
    const unsigned char *,size_t,lt48_runtime_configuration_value *,size_t,size_t *);
LT48_API lt48_status lt48_pic18_configuration_edit(const lt48_pic18_resources *,
    unsigned char *,size_t,const lt48_runtime_configuration_value *,size_t);
LT48_API lt48_status lt48_pic18_validate(const lt48_pic18_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_pic18_execute(const lt48_pic18_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_pic18_result *);
LT48_API lt48_status lt48_programmer_execute_pic18(lt48_programmer *,const lt48_pic18_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_pic18_result *);

/* J-series configuration occupies the final eight physical Flash bytes.
 * Program performs a full erase; Over Program and Secure preserve a stable
 * backup before erase/rewrite. Authorization is explicit and per execution. */
enum { LT48_PIC18_ERASE_AUTHORIZED=1u, LT48_PIC18_IGNORE_ID_MISMATCH=2u };
typedef struct lt48_pic18_execution_options {
 unsigned flags;
 unsigned char *preservation_backup;size_t preservation_backup_size;
 lt48_m37_id_decision_fn id_decision;void *id_context;
} lt48_pic18_execution_options;
typedef struct lt48_pic18_diagnostics {
 uint16_t observed_id;
 int id_mismatch,protected_read,configuration_from_mirrors,backup_valid;
 uint32_t missing_contacts[2],unexpected_contacts[2];
} lt48_pic18_diagnostics;
LT48_API int lt48_pic18_is_j_series(const lt48_pic18_resources *);
LT48_API lt48_status lt48_pic18_execute_with_options(const lt48_pic18_resources *,
 const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,
 const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,
 const lt48_pic18_execution_options *,lt48_pic18_diagnostics *,lt48_pic18_result *);
LT48_API lt48_status lt48_programmer_execute_pic18_with_options(lt48_programmer *,
 const lt48_pic18_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,
 unsigned char *,size_t,lt48_verify_margin_selection,unsigned,
 const lt48_pic18_execution_options *,lt48_pic18_diagnostics *,unsigned,lt48_pic18_result *);

typedef struct lt48_compact_spi_resources lt48_compact_spi_resources;
typedef lt48_small_eprom_result lt48_compact_spi_result;
LT48_API lt48_status lt48_compact_spi_open(const lt48_installation *,size_t,lt48_compact_spi_resources **);
LT48_API void lt48_compact_spi_close(lt48_compact_spi_resources *);
LT48_API lt48_status lt48_compact_spi_validate_verify_options(const lt48_compact_spi_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API uint32_t lt48_compact_spi_capacity(const lt48_compact_spi_resources *);
LT48_API int lt48_compact_spi_has_protection(const lt48_compact_spi_resources *);
/* X25057/X25097: Secure takes one byte 1..7; Configuration takes 0..7
 * (0 unlocks). Both use a full-array range and preserve array contents. */
LT48_API int lt48_compact_spi_has_idlock(const lt48_compact_spi_resources *);
/* Raw SRWD/BP mask (bits7,3,2); explicit Configuration/Secure payload byte. */
LT48_API int lt48_compact_spi_has_raw_status(const lt48_compact_spi_resources *);
LT48_API int lt48_compact_spi_has_incremental_registers(const lt48_compact_spi_resources *);
LT48_API lt48_status lt48_compact_spi_validate(const lt48_compact_spi_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_compact_spi_execute(const lt48_compact_spi_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_compact_spi_result *);
LT48_API lt48_status lt48_programmer_execute_compact_spi(lt48_programmer *,const lt48_compact_spi_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_compact_spi_result *);
typedef struct lt48_stk10c68_resources lt48_stk10c68_resources;
typedef lt48_small_eprom_result lt48_stk10c68_result;
LT48_API lt48_status lt48_stk10c68_open(const lt48_installation *,size_t,lt48_stk10c68_resources **);
LT48_API void lt48_stk10c68_close(lt48_stk10c68_resources *);
LT48_API lt48_status lt48_stk10c68_validate_verify_options(const lt48_stk10c68_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API uint32_t lt48_stk10c68_capacity(const lt48_stk10c68_resources *);
LT48_API lt48_status lt48_stk10c68_validate(const lt48_stk10c68_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_stk10c68_execute(const lt48_stk10c68_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_stk10c68_result *);
LT48_API lt48_status lt48_programmer_execute_stk10c68(lt48_programmer *,const lt48_stk10c68_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_stk10c68_result *);

typedef struct lt48_microwire_resources lt48_microwire_resources;
typedef lt48_small_eprom_result lt48_microwire_result;
LT48_API lt48_status lt48_microwire_open(const lt48_installation *,size_t,lt48_microwire_resources **);
LT48_API void lt48_microwire_close(lt48_microwire_resources *);
LT48_API int lt48_microwire_is_nvram(const lt48_microwire_resources *);
LT48_API lt48_status lt48_microwire_validate_verify_options(const lt48_microwire_resources *,lt48_operation_kind,lt48_verify_margin_selection,unsigned);
LT48_API uint32_t lt48_microwire_capacity(const lt48_microwire_resources *);
LT48_API lt48_status lt48_microwire_validate(const lt48_microwire_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_microwire_execute(const lt48_microwire_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_microwire_result *);
LT48_API lt48_status lt48_programmer_execute_microwire(lt48_programmer *,const lt48_microwire_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_microwire_result *);

/* XC72xx/XC73xx use sparse byte planes, not interleaved words or JEDEC fuse indices. */
typedef struct lt48_xc7000_resources lt48_xc7000_resources;
typedef struct lt48_xc7000_row { uint16_t address,mask; } lt48_xc7000_row;
typedef struct lt48_xc7000_result {
    lt48_status operation_status,cleanup_status;
    size_t completed_rows,first_mismatch,mismatch_count;
    unsigned attempts; int cleanup_completed;
} lt48_xc7000_result;
LT48_API lt48_status lt48_xc7000_open(const lt48_installation *,size_t,lt48_xc7000_resources **);
LT48_API void lt48_xc7000_close(lt48_xc7000_resources *);
LT48_API size_t lt48_xc7000_capacity(const lt48_xc7000_resources *);
LT48_API size_t lt48_xc7000_row_count(const lt48_xc7000_resources *);
LT48_API lt48_status lt48_xc7000_get_row(const lt48_xc7000_resources *,size_t,lt48_xc7000_row *);
LT48_API lt48_status lt48_xc7000_validate(const lt48_xc7000_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_xc7000_execute(const lt48_xc7000_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_xc7000_result *);
LT48_API lt48_status lt48_programmer_execute_xc7000(lt48_programmer *,const lt48_xc7000_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_xc7000_result *);

typedef struct lt48_mcu51_resources lt48_mcu51_resources;
typedef lt48_small_eprom_result lt48_mcu51_result;
typedef struct lt48_mcu51_options {
    lt48_verify_margin_selection margin;
    unsigned margin_type;
    unsigned protection_level; /* Explicit cumulative level; Secure only. */
    const unsigned char *encryption; /* Optional, after verified Program. */
    size_t encryption_size;
} lt48_mcu51_options;
LT48_API lt48_status lt48_mcu51_open(const lt48_installation *,size_t,lt48_mcu51_resources **);
LT48_API void lt48_mcu51_close(lt48_mcu51_resources *);
LT48_API uint32_t lt48_mcu51_capacity(const lt48_mcu51_resources *);
/* 0: OTP/MCS-48; 1: LV autoerase; 2: S/LS with configuration.
 * S/LS Configuration output: [lock mask (LB1 bit0), raw SPI fuse (0 enabled)].
 * Optional input: [edit mask (locks bit0, SPI bit1), lock mask, raw SPI fuse]. */
/* UPI Configuration: input [edit mask bit0/bit1, user12, user13];
 * output full 32-byte signature row. Reserved bytes cannot be written. */
/* Philips39: 0 other; 1 RA+; 2 RA2 write-only X2-enable request;
 * 3 X2 readable locks/clock. Configuration [mask,lockmask,clock] as above;
 * RA2 accepts only mask2/clock1 and returns FF FF (no readback). */
/* Additional exact MCU70 settings kinds: 4 independent OTP flags;
 * 5 protection only; 6 raw OTP configuration bytes; 7 Dallas watchdog;
 * 8 RD2 boot bytes; 9 T87C5103 read-only [protected,FF];
 * 10 T87C5111 read-only [source lock selector 0..3, raw CONF].
 * Kinds 9/10 use Secure for protection; configuration writes are rejected.
 * T87C5111 reserved bits are retained; changing an existing selector is rejected. */
LT48_API unsigned lt48_mcu51_philips_options(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_id_can_ignore(const lt48_mcu51_resources *);
/* Source configuration bytes; zero for owners using other settings formats.
 * Configuration write: lock-edit, desired lock mask, byte-edit mask, N bytes.
 * Configuration read: lock mask followed by N bytes. Unselected bytes retained. */
LT48_API unsigned lt48_mcu51_configuration_bytes(const lt48_mcu51_resources *);
LT48_API uint32_t lt48_mcu51_configuration_address(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_is_flash51_57(const lt48_mcu51_resources *resources);
LT48_API unsigned lt48_mcu51_is_flash51_41(const lt48_mcu51_resources *resources);
LT48_API unsigned lt48_mcu51_cl38(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_cl38_mask(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_cl38_family(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_cl38_admitted(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_is_mcu70(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_is_otp20(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_lm20_kind(const lt48_mcu51_resources*);
unsigned lt48_mcu51_lm20_admitted(const lt48_mcu51_resources*);
unsigned lt48_mcu51_lm20_mask(const lt48_mcu51_resources*);
unsigned lt48_mcu51_legacy14_kind(const lt48_mcu51_resources *resources);
LT48_API unsigned lt48_mcu51_legacy14_admitted(const lt48_mcu51_resources *resources);
LT48_API unsigned lt48_mcu51_is_upi41ah(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_is_at89(const lt48_mcu51_resources *);
LT48_API unsigned lt48_mcu51_security_levels(const lt48_mcu51_resources *);
/* Source-specific protection selector; NULL uses normal level labels. */
LT48_API const char *lt48_mcu51_protection_choice_name(const lt48_mcu51_resources *, unsigned);
LT48_API lt48_status lt48_mcu51_validate(const lt48_mcu51_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_mcu51_validate_options(const lt48_mcu51_resources *,lt48_operation_kind,const lt48_mcu51_options *);
LT48_API lt48_status lt48_mcu51_execute(const lt48_mcu51_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,const lt48_mcu51_options *,lt48_m37_id_decision_fn,void *,lt48_mcu51_result *);
LT48_API lt48_status lt48_programmer_execute_mcu51(lt48_programmer *,const lt48_mcu51_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,const lt48_mcu51_options *,lt48_m37_id_decision_fn,void *,unsigned,lt48_mcu51_result *);

/* Digest-bound Hitachi Dataman 13.10 XP block engine; offline qualification. */
typedef struct lt48_xp_eprom_resources lt48_xp_eprom_resources;
typedef lt48_small_eprom_result lt48_xp_eprom_result;
LT48_API lt48_status lt48_xp_eprom_open(const lt48_installation *, size_t,
    lt48_xp_eprom_resources **);
LT48_API void lt48_xp_eprom_close(lt48_xp_eprom_resources *);
LT48_API int lt48_xp_eprom_is_at89(const lt48_xp_eprom_resources *resources);
/* CONFIGURATION returns one protection-level byte in read, after cleanup. */
/* RB2 Configuration: read seven fields [XRAM, BLJB, X2, lock, BSB, SBV, SSB].
 * Optional input: eight bytes [edit mask, seven values]; unselected fields are preserved. */
LT48_API int lt48_xp_eprom_is_philips(const lt48_xp_eprom_resources *resources);
LT48_API unsigned lt48_xp_eprom_extended_fields(const lt48_xp_eprom_resources *resources);
LT48_API unsigned lt48_xp_eprom_extended_flags(const lt48_xp_eprom_resources *resources);
LT48_API const char *lt48_xp_eprom_extended_label(const lt48_xp_eprom_resources *resources,unsigned field);
LT48_API unsigned lt48_xp_eprom_flag_fields(const lt48_xp_eprom_resources *resources);
LT48_API unsigned lt48_xp_eprom_w78_fields(const lt48_xp_eprom_resources *resources);
LT48_API /* RA2 configuration: clock flag, lock level, status byte, boot vector. */
int lt48_xp_eprom_is_ra2(const lt48_xp_eprom_resources *resources);
int lt48_xp_eprom_is_rb2(const lt48_xp_eprom_resources *resources);
LT48_API int lt48_xp_eprom_has_configuration(const lt48_xp_eprom_resources *resources);
/* AT89 source protection level 1..3; no automatic protection after programming. */
LT48_API lt48_status lt48_xp_eprom_set_protection(lt48_xp_eprom_resources *resources, unsigned level);
LT48_API int lt48_xp_eprom_is_fram(const lt48_xp_eprom_resources *);
LT48_API uint32_t lt48_xp_eprom_capacity(const lt48_xp_eprom_resources *);
LT48_API lt48_status lt48_xp_eprom_validate(const lt48_xp_eprom_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t);
LT48_API lt48_status lt48_xp_eprom_validate_verify_options(const lt48_xp_eprom_resources *,
    lt48_operation_kind, lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_xp_eprom_execute(const lt48_xp_eprom_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t,
    size_t, const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *,
    lt48_xp_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_xp_eprom(lt48_programmer *,
    const lt48_xp_eprom_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *, unsigned,
    lt48_xp_eprom_result *);

/* Source-bound XP flash engine. Operation/protection availability is profile
 * specific. Original package adapters are required: D4 AM29DL800BT uses
 * TSOP37 -> socket25, NOT 1:1; D42 AM29F100B SO44 uses straight DIP44.
 * Catalogue admission and hardware qualification are distinct. */
typedef struct lt48_xp_flash_resources lt48_xp_flash_resources;
typedef lt48_small_eprom_result lt48_xp_flash_result;
LT48_API lt48_status lt48_xp_flash_open(const lt48_installation *, size_t,
    lt48_xp_flash_resources **);
LT48_API void lt48_xp_flash_close(lt48_xp_flash_resources *);
/* 0: unavailable, 1: main-array/sector protection,
 * 2: permanent boot-block protection (x8) or OTP-register locks (Page/word),
 * 3: reversible boot-block protection (x8),
 * 4: permanent OTP-register and Parameter Block #0 locks (ST word NOR). */
LT48_API unsigned lt48_xp_flash_secure_kind(const lt48_xp_flash_resources *resources);
/* Choice-based boot protection: 1=SST bottom/top, 2=Winbond 16K/64K.
 * These two source choices are mutually exclusive. Select before Secure. */
LT48_API unsigned lt48_xp_flash_protection_choices(const lt48_xp_flash_resources *);
LT48_API const char *lt48_xp_flash_protection_choice_name(const lt48_xp_flash_resources *, unsigned);
LT48_API lt48_status lt48_xp_flash_set_protection_choice(lt48_xp_flash_resources *, unsigned);
LT48_API uint32_t lt48_xp_flash_capacity(const lt48_xp_flash_resources *);
LT48_API lt48_status lt48_xp_flash_validate(const lt48_xp_flash_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t);
LT48_API lt48_status lt48_xp_flash_validate_verify_options(const lt48_xp_flash_resources *,
    lt48_operation_kind, lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_xp_flash_execute(const lt48_xp_flash_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t,
    size_t, const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *,
    lt48_xp_flash_result *);
LT48_API lt48_status lt48_programmer_execute_xp_flash(lt48_programmer *,
    const lt48_xp_flash_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *, unsigned,
    lt48_xp_flash_result *);

/* Segmented AMD AddTab word flash. Secure covers main-array protection;
 * permanent special-region locks are not exposed by this owner. */
typedef struct lt48_addtab_flash_resources lt48_addtab_flash_resources;
typedef lt48_small_eprom_result lt48_addtab_flash_result;
LT48_API lt48_status lt48_addtab_flash_open(const lt48_installation *, size_t,
    lt48_addtab_flash_resources **);
LT48_API void lt48_addtab_flash_close(lt48_addtab_flash_resources *);
/* 0: no Secure; 1: main-array protection; 2: includes permanent locks. */
LT48_API unsigned lt48_addtab_flash_secure_kind(const lt48_addtab_flash_resources *);
LT48_API uint32_t lt48_addtab_flash_capacity(const lt48_addtab_flash_resources *);
LT48_API lt48_status lt48_addtab_flash_validate(const lt48_addtab_flash_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t);
LT48_API lt48_status lt48_addtab_flash_validate_verify_options(const lt48_addtab_flash_resources *,
    lt48_operation_kind, lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_addtab_flash_execute(const lt48_addtab_flash_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t,
    size_t, const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *,
    lt48_addtab_flash_result *);
LT48_API lt48_status lt48_programmer_execute_addtab_flash(lt48_programmer *,
    const lt48_addtab_flash_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *, unsigned,
    lt48_addtab_flash_result *);

/* Class2044 SPI owner: not admitted until the offline audit is complete. */
typedef struct lt48_addtab_spi_resources lt48_addtab_spi_resources;
typedef lt48_small_eprom_result lt48_addtab_spi_result;
LT48_API lt48_status lt48_addtab_spi_open(const lt48_installation *, size_t,
    lt48_addtab_spi_resources **);
LT48_API void lt48_addtab_spi_close(lt48_addtab_spi_resources *);
/* 0 SPI NOR, 1 serial/parallel DataFlash, 2 I2C RRAM, 3 legacy DataFlash. */
LT48_API unsigned lt48_addtab_spi_is_spi26(const lt48_addtab_spi_resources *resources);
LT48_API unsigned lt48_addtab_spi_is_spi23(const lt48_addtab_spi_resources *resources);
LT48_API unsigned lt48_addtab_spi_is_spi54(const lt48_addtab_spi_resources *resources);
LT48_API unsigned lt48_addtab_spi_is_mac29(const lt48_addtab_spi_resources *resources);
LT48_API unsigned lt48_addtab_spi_is_micron40(const lt48_addtab_spi_resources *resources);
LT48_API unsigned lt48_addtab_spi_memory_kind(const lt48_addtab_spi_resources *resources);
LT48_API int lt48_addtab_spi_secure_is_permanent(const lt48_addtab_spi_resources *resources);
uint32_t lt48_addtab_spi_capacity(const lt48_addtab_spi_resources *);
LT48_API lt48_status lt48_addtab_spi_validate(const lt48_addtab_spi_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t);
LT48_API lt48_status lt48_addtab_spi_validate_verify_options(const lt48_addtab_spi_resources *,
    lt48_operation_kind, lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_addtab_spi_execute(const lt48_addtab_spi_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t,
    size_t, const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *,
    lt48_addtab_spi_result *);
LT48_API lt48_status lt48_programmer_execute_addtab_spi(lt48_programmer *,
    const lt48_addtab_spi_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *, unsigned,
    lt48_addtab_spi_result *);

/* Exact page EEPROM owner; EEPROM clear writes FF, Secure enables SDP. */
typedef struct lt48_page_eeprom_resources lt48_page_eeprom_resources;
typedef lt48_small_eprom_result lt48_page_eeprom_result;
LT48_API lt48_status lt48_page_eeprom_open(const lt48_installation *, size_t,
    lt48_page_eeprom_resources **);
LT48_API void lt48_page_eeprom_close(lt48_page_eeprom_resources *);
LT48_API unsigned lt48_page_eeprom_is_word(const lt48_page_eeprom_resources *);
LT48_API lt48_status lt48_page_eeprom_set_sdp_choice(lt48_page_eeprom_resources *,unsigned);
LT48_API uint32_t lt48_page_eeprom_capacity(const lt48_page_eeprom_resources *);
LT48_API lt48_status lt48_page_eeprom_validate(const lt48_page_eeprom_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t);
LT48_API lt48_status lt48_page_eeprom_validate_verify_options(const lt48_page_eeprom_resources *,
    lt48_operation_kind, lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_page_eeprom_execute(const lt48_page_eeprom_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t,
    size_t, const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *,
    lt48_page_eeprom_result *);
LT48_API lt48_status lt48_programmer_execute_page_eeprom(lt48_programmer *,
    const lt48_page_eeprom_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *, unsigned,
    lt48_page_eeprom_result *);

/* Small serial Page EEPROM owner; clear writes FF; no software Secure. */
typedef struct lt48_serial_page_resources lt48_serial_page_resources;
typedef lt48_small_eprom_result lt48_serial_page_result;
LT48_API lt48_status lt48_serial_page_open(const lt48_installation *, size_t,
    lt48_serial_page_resources **);
LT48_API void lt48_serial_page_close(lt48_serial_page_resources *);
/* True for audited SPI NOR profiles (whole-chip erase, monotonic programming). */
/* Secure locks OTP data permanently on these original source profiles. */
LT48_API int lt48_serial_page_secure_is_otp(const lt48_serial_page_resources *);
/* 0 ordinary; 1 permanent lower-half; 2 permanent/reversible; 3 CAT config.
 * Kinds 2/3 require a one-byte image for Secure/Configuration, full-array range:
 * kind 2: bit0 permanent, bit1 reversible; Configuration 0 clears reversible.
 * kind 3: bits0..3 NB/AB0/AB1/DDC-WE, as in the original source config sheet.
 * Ordinary memory writes preserve these settings. Kind 1 Secure needs no image. */
LT48_API unsigned lt48_serial_page_i2c_config_kind(const lt48_serial_page_resources *);
LT48_API int lt48_serial_page_is_nor(const lt48_serial_page_resources *);
/* AT89LP213/214/216: Configuration reads 77 bytes: 11 programmed flags,
 * lock level, trim, 64 signature bytes. Optional input is a LE16 edit mask
 * followed by those 77 values; bits 0..12 select fields, bit 13 signature.
 * Secure optionally takes one byte (level 1/2), default 2. Code writes and
 * Erase require the full 2048-byte range. Hardware qualification pending. */
LT48_API int lt48_serial_page_is_lp21(const lt48_serial_page_resources *);
LT48_API uint32_t lt48_serial_page_capacity(const lt48_serial_page_resources *);
LT48_API lt48_status lt48_serial_page_validate(const lt48_serial_page_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t);
LT48_API lt48_status lt48_serial_page_validate_verify_options(const lt48_serial_page_resources *,
    lt48_operation_kind, lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_serial_page_execute(const lt48_serial_page_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t,
    size_t, const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *,
    lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_serial_page(lt48_programmer *,
    const lt48_serial_page_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_m37_id_decision_fn, void *, unsigned,
    lt48_serial_page_result *);


/* Audited AT89LP428/828/3240/6440 operation owner; hardware qualification pending.
 * Configuration input: LE16 mask + twelve flags + lock level (15 bytes).
 * Configuration output: twelve flags + lock level (13 bytes).
 * Memory image: code, data flash, user signature, analog/signature page.
 * The analog/signature page is preserved; FF in a program file means preserve.
 */
typedef struct lt48_at89_lp_large_resources lt48_at89_lp_large_resources;
LT48_API lt48_status lt48_at89_lp_large_open(const lt48_installation *,size_t,lt48_at89_lp_large_resources **);
LT48_API void lt48_at89_lp_large_close(lt48_at89_lp_large_resources *);
LT48_API uint32_t lt48_at89_lp_large_capacity(const lt48_at89_lp_large_resources *);
LT48_API lt48_status lt48_at89_lp_large_validate(const lt48_at89_lp_large_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_at89_lp_large_execute(const lt48_at89_lp_large_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);

/* Source-audited LP51 operation engine; hardware qualification pending.
 * Configuration input: LE16 edit mask plus 14 fields (ED2/RD2) or 16 (ID2).
 * Fields: ten/eleven flags, then Clock A, startup, power mode, locks, and
 * ID2 Clock B. Output omits the mask. Signature page 3 is preserved. */
typedef struct lt48_ed2_resources lt48_ed2_resources;
LT48_API lt48_status lt48_ed2_open(const lt48_installation *,size_t,lt48_ed2_resources **);
LT48_API void lt48_ed2_close(lt48_ed2_resources *);
LT48_API uint32_t lt48_ed2_capacity(const lt48_ed2_resources *);
LT48_API unsigned lt48_ed2_configuration_count(const lt48_ed2_resources *);
LT48_API unsigned lt48_ed2_configuration_flags(const lt48_ed2_resources *);
LT48_API lt48_status lt48_ed2_validate(const lt48_ed2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_ed2_execute(const lt48_ed2_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_ed2(lt48_programmer *,const lt48_ed2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
/* LP2052/4052: code followed by a 32-byte user signature row.
 * Configuration input: six-bit change mask, then six 0/1 fields: lock1,
 * lock2, ISP enable, oscillator bypass, user-row programming, clock output.
 * Output omits the mask. Checked (1) corresponds to a cleared hardware bit. */
typedef struct lt48_lp2052_resources lt48_lp2052_resources;
LT48_API lt48_status lt48_lp2052_open(const lt48_installation *,size_t,lt48_lp2052_resources **);
LT48_API void lt48_lp2052_close(lt48_lp2052_resources *);
LT48_API lt48_status lt48_lp2052_validate(const lt48_lp2052_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_lp2052_execute(const lt48_lp2052_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_lp2052(lt48_programmer *,const lt48_lp2052_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
/* S2051/4051: code plus a 32-byte user row. Configuration input is a
 * six-bit change mask, then lock1, lock2, SPI enable, x2 clock enable,
 * user-row enable and oscillator bypass (0/1). Output omits the mask. */
typedef struct lt48_s2051_resources lt48_s2051_resources;
LT48_API lt48_status lt48_s2051_open(const lt48_installation *,size_t,lt48_s2051_resources **);
LT48_API void lt48_s2051_close(lt48_s2051_resources *);
LT48_API lt48_status lt48_s2051_validate(const lt48_s2051_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_s2051_execute(const lt48_s2051_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_s2051(lt48_programmer *,const lt48_s2051_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
typedef struct lt48_s8253_resources lt48_s8253_resources;
LT48_API lt48_status lt48_s8253_open(const lt48_installation *,size_t,lt48_s8253_resources **);
LT48_API void lt48_s8253_close(lt48_s8253_resources *);
LT48_API lt48_status lt48_s8253_validate(const lt48_s8253_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_s8253_execute(const lt48_s8253_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
/* ST7: source-compatible 64-KiB sparse image, FF holes. Configuration input
 * is byte-change mask followed by 2/3 raw option bytes in source return order;
 * output is those option bytes. No irreversible FMP_W programming. */
typedef struct lt48_st7_resources lt48_st7_resources;
LT48_API lt48_status lt48_st7_open(const lt48_installation*,size_t,lt48_st7_resources**);
LT48_API void lt48_st7_close(lt48_st7_resources*);
LT48_API unsigned lt48_st7_option_bytes(const lt48_st7_resources*);
LT48_API lt48_status lt48_st7_validate(const lt48_st7_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_st7_execute(const lt48_st7_resources*,const lt48_protocol_transport*,void*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result*);
LT48_API lt48_status lt48_programmer_execute_st7(lt48_programmer*,const lt48_st7_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_small_eprom_result*);
typedef struct lt48_tms370_resources lt48_tms370_resources;
LT48_API lt48_status lt48_tms370_open(const lt48_installation*,size_t,lt48_tms370_resources**);
LT48_API void lt48_tms370_close(lt48_tms370_resources*);
LT48_API lt48_status lt48_tms370_validate(const lt48_tms370_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_tms370_execute(const lt48_tms370_resources*,const lt48_protocol_transport*,void*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result*);
LT48_API lt48_status lt48_programmer_execute_tms370(lt48_programmer*,const lt48_tms370_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_small_eprom_result*);
typedef struct lt48_tms87_resources lt48_tms87_resources;
LT48_API lt48_status lt48_tms87_open(const lt48_installation*,size_t,lt48_tms87_resources**);
LT48_API void lt48_tms87_close(lt48_tms87_resources*);
LT48_API lt48_status lt48_tms87_validate(const lt48_tms87_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_tms87_execute(const lt48_tms87_resources*,const lt48_protocol_transport*,void*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result*);
LT48_API lt48_status lt48_programmer_execute_tms87(lt48_programmer*,const lt48_tms87_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_small_eprom_result*);
LT48_API lt48_status lt48_programmer_execute_at89_s8253(lt48_programmer *,const lt48_s8253_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
/* P89LPC Flash: sector-CRC code verification; raw code Read unavailable.
 * Configuration edit packet: little-endian uint32 mask, then 20 raw values.
 * Read configuration returns field_count bytes: UCFG1,UCFG2,boot,status,SECx. */
typedef struct lt48_lpcflash_resources lt48_lpcflash_resources;
LT48_API lt48_status lt48_lpcflash_open(const lt48_installation*,size_t,lt48_lpcflash_resources**);
LT48_API void lt48_lpcflash_close(lt48_lpcflash_resources*);
LT48_API int lt48_lpcflash_has_isp_choice(const lt48_lpcflash_resources*);
LT48_API unsigned lt48_lpcflash_field_max(const lt48_lpcflash_resources*,unsigned);
LT48_API unsigned lt48_lpcflash_field_count(const lt48_lpcflash_resources*);
LT48_API lt48_status lt48_lpcflash_set_erase(lt48_lpcflash_resources*,unsigned,int);
LT48_API lt48_status lt48_lpcflash_validate(const lt48_lpcflash_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
LT48_API lt48_status lt48_lpcflash_execute(const lt48_lpcflash_resources*,const lt48_protocol_transport*,void*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void*,lt48_serial_page_result*);
LT48_API lt48_status lt48_programmer_execute_lpcflash(lt48_programmer*,const lt48_lpcflash_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,unsigned char*,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void*,unsigned,lt48_serial_page_result*);

typedef struct lt48_lpc_resources lt48_lpc_resources;
LT48_API lt48_status lt48_lpc_open(const lt48_installation *,size_t,lt48_lpc_resources **);
LT48_API void lt48_lpc_close(lt48_lpc_resources *);
LT48_API lt48_status lt48_lpc_validate(const lt48_lpc_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_lpc_execute(const lt48_lpc_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_lpc(lt48_programmer *,const lt48_lpc_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
typedef struct lt48_at89_tail_resources lt48_c5114_resources;
LT48_API lt48_status lt48_c5114_open(const lt48_installation *,size_t,lt48_c5114_resources **);
LT48_API void lt48_c5114_close(lt48_c5114_resources *);
LT48_API lt48_status lt48_c5114_validate(const lt48_c5114_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_c5114_execute(const lt48_c5114_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_c5114(lt48_programmer *,const lt48_c5114_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
typedef struct lt48_c5132_resources lt48_c5132_resources;
LT48_API lt48_status lt48_c5132_open(const lt48_installation *,size_t,lt48_c5132_resources **);
LT48_API void lt48_c5132_close(lt48_c5132_resources *);
LT48_API lt48_status lt48_c5132_validate(const lt48_c5132_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_c5132_execute(const lt48_c5132_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_c5132(lt48_programmer *,const lt48_c5132_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
typedef struct lt48_at89_tail_resources lt48_ic2_resources;
LT48_API lt48_status lt48_ic2_open(const lt48_installation *,size_t,lt48_ic2_resources **);
LT48_API void lt48_ic2_close(lt48_ic2_resources *);
LT48_API lt48_status lt48_ic2_validate(const lt48_ic2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_ic2_execute(const lt48_ic2_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_ic2(lt48_programmer *,const lt48_ic2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);

typedef struct lt48_re2_resources lt48_re2_resources;
LT48_API lt48_status lt48_re2_open(const lt48_installation *,size_t,lt48_re2_resources **);
LT48_API void lt48_re2_close(lt48_re2_resources *);
LT48_API lt48_status lt48_re2_validate(const lt48_re2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_re2_execute(const lt48_re2_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_re2(lt48_programmer *,const lt48_re2_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
/* Basic LP51/52: code + 256-byte data + 256-byte user signature.
 * Configuration: little-endian 9-bit mask, six 0/1 flags then oscillator,
 * start-up and lock choices (0..3). Nine-byte output omits the mask. */
typedef struct lt48_lp51_basic_resources lt48_lp51_basic_resources;
LT48_API lt48_status lt48_lp51_basic_open(const lt48_installation *,size_t,lt48_lp51_basic_resources **);
LT48_API void lt48_lp51_basic_close(lt48_lp51_basic_resources *);
LT48_API lt48_status lt48_lp51_basic_validate(const lt48_lp51_basic_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_lp51_basic_execute(const lt48_lp51_basic_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_lp51_basic(lt48_programmer *,const lt48_lp51_basic_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
/* C5130/5131: 64 KiB image; unused code-to-EFFF range must be FF.
 * Configuration input: little-endian 10-bit change mask (2 bytes), then
 * BLJB, X2, oscillator 0..3, lock 0..3, six bytes at extra-row addresses 0..5.
 * Configuration output is those ten fields without the mask. */
typedef struct lt48_usb51_resources lt48_usb51_resources;
LT48_API lt48_status lt48_usb51_open(const lt48_installation *,size_t,lt48_usb51_resources **);
LT48_API void lt48_usb51_close(lt48_usb51_resources *);
LT48_API lt48_status lt48_usb51_validate(const lt48_usb51_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_usb51_execute(const lt48_usb51_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_serial_page_result *);
LT48_API lt48_status lt48_programmer_execute_at89_usb51(lt48_programmer *,const lt48_usb51_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,unsigned,lt48_serial_page_result *);

typedef struct lt48_at89_lp51_resources lt48_at89_lp51_resources;
LT48_API lt48_status lt48_at89_lp51_open(const lt48_installation *,size_t,lt48_at89_lp51_resources **);
LT48_API void lt48_at89_lp51_close(lt48_at89_lp51_resources *);
LT48_API uint32_t lt48_at89_lp51_capacity(const lt48_at89_lp51_resources *);
LT48_API lt48_status lt48_at89_lp51_validate(const lt48_at89_lp51_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t);
LT48_API lt48_status lt48_at89_lp51_execute(const lt48_at89_lp51_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_serial_page_result *);

LT48_API lt48_status lt48_programmer_execute_at89_lp51(lt48_programmer *,const lt48_at89_lp51_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);
LT48_API unsigned lt48_at89_lp51_configuration_count(const lt48_at89_lp51_resources *);
LT48_API lt48_status lt48_programmer_execute_at89_lp_large(lt48_programmer *,const lt48_at89_lp_large_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,unsigned,lt48_serial_page_result *);

/* Hitachi class-2009 M1/M2 parallel MCU source procedures.
 * Program requires a blank range aligned to 128-byte blocks. Erase is whole-chip.
 * Package-specific original adapters are required; hardware qualification pending. */
typedef struct lt48_hitachi_resources lt48_hitachi_resources;
typedef struct lt48_hitachi_result {
    lt48_status operation_status, cleanup_status;
    size_t completed_bytes, mismatch_count;
    uint32_t first_mismatch;
    int cleanup_completed;
} lt48_hitachi_result;
LT48_API lt48_status lt48_hitachi_open(const lt48_installation *, size_t,
    lt48_hitachi_resources **);
LT48_API void lt48_hitachi_close(lt48_hitachi_resources *);
LT48_API uint32_t lt48_hitachi_capacity(const lt48_hitachi_resources *);
LT48_API unsigned lt48_hitachi_is_fujitsu(const lt48_hitachi_resources*);
unsigned lt48_hitachi_qualified(const lt48_hitachi_resources*);
lt48_status lt48_hitachi_validate(const lt48_hitachi_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_hitachi_execute(const lt48_hitachi_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_hitachi_result *);
/* Requires initialized transport; always releases its interface after execution.
 * Timeout is raised to at least the original 30-second programming watchdog. */
LT48_API lt48_status lt48_programmer_execute_hitachi(lt48_programmer *,
    const lt48_hitachi_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t, unsigned,
    lt48_verify_margin_selection, unsigned, lt48_hitachi_result *);


/* Legacy DataFlash source-applet engine. Availability
 * through this API does not imply complete-support catalogue/GUI admission. */
typedef struct lt48_legacy_dataflash_resources lt48_legacy_dataflash_resources;
typedef struct lt48_legacy_dataflash_result {
    lt48_status operation_status, cleanup_status;
    size_t completed_bytes, mismatch_count;
    uint32_t first_mismatch;
    int cleanup_completed;
} lt48_legacy_dataflash_result;
LT48_API lt48_status lt48_legacy_dataflash_open(const lt48_installation *, size_t,
    lt48_legacy_dataflash_resources **);
LT48_API void lt48_legacy_dataflash_close(lt48_legacy_dataflash_resources *);
LT48_API uint32_t lt48_legacy_dataflash_capacity(const lt48_legacy_dataflash_resources *);
LT48_API unsigned lt48_legacy_dataflash_page_size(const lt48_legacy_dataflash_resources *);
/* Array byte offset to the original non-power-of-two page SPI address. */
LT48_API lt48_status lt48_legacy_dataflash_address(const lt48_legacy_dataflash_resources *,
    uint32_t, uint32_t *);
LT48_API lt48_status lt48_legacy_dataflash_validate(const lt48_legacy_dataflash_resources *,
    lt48_operation_kind, uint32_t, size_t, const unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned);
LT48_API lt48_status lt48_legacy_dataflash_execute(const lt48_legacy_dataflash_resources *,
    const lt48_protocol_transport *, void *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t,
    lt48_verify_margin_selection, unsigned, lt48_legacy_dataflash_result *);
/* Requires initialized transport; always releases its interface after execution.
 * Timeout is raised to at least the original 30-second programming watchdog. */
LT48_API lt48_status lt48_programmer_execute_legacy_dataflash(lt48_programmer *,
    const lt48_legacy_dataflash_resources *, lt48_operation_kind, uint32_t, size_t,
    const unsigned char *, size_t, unsigned char *, size_t, unsigned,
    lt48_verify_margin_selection, unsigned, lt48_legacy_dataflash_result *);


/* Audited GDAC serial memory applets. Hardware qualification is separate. */
typedef struct lt48_gdac_serial_resources lt48_gdac_serial_resources;
typedef lt48_small_eprom_result lt48_gdac_serial_result;
LT48_API lt48_status lt48_gdac_serial_open(const lt48_installation *,size_t,lt48_gdac_serial_resources **);
LT48_API void lt48_gdac_serial_close(lt48_gdac_serial_resources *);
LT48_API int lt48_gdac_serial_is_otp(const lt48_gdac_serial_resources *);
LT48_API uint32_t lt48_gdac_serial_capacity(const lt48_gdac_serial_resources *);
LT48_API lt48_status lt48_gdac_serial_validate(const lt48_gdac_serial_resources *,lt48_operation_kind,
    uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_gdac_serial_execute(const lt48_gdac_serial_resources *,
    const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,
    const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,
    lt48_m37_id_decision_fn,void *,lt48_gdac_serial_result *);
LT48_API lt48_status lt48_programmer_execute_gdac_serial(lt48_programmer *,
    const lt48_gdac_serial_resources *,lt48_operation_kind,uint32_t,size_t,
    const unsigned char *,size_t,unsigned char *,size_t,unsigned,
    lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_gdac_serial_result *);

/* Audited GAAF serial memory applets. Hardware qualification is separate.
 * CONFIGURATION input is one raw byte for NexFlash, two little-endian bytes
 * for W45D (only bit0 of the second byte is valid). It deliberately changes
 * the nonvolatile configuration. SECURE preserves control bits and protects
 * the whole array. Ordinary Program/Erase restore the saved configuration. */
typedef struct lt48_gaaf_serial_resources lt48_gaaf_serial_resources;
typedef lt48_small_eprom_result lt48_gaaf_serial_result;
LT48_API lt48_status lt48_gaaf_serial_open(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
LT48_API void lt48_gaaf_serial_close(lt48_gaaf_serial_resources *);
LT48_API unsigned lt48_gaaf_serial_is_combined36(const lt48_gaaf_serial_resources *);
LT48_API unsigned lt48_gaaf_serial_qualified(const lt48_gaaf_serial_resources *);
LT48_API unsigned lt48_gaaf_serial_configuration_bits(const lt48_gaaf_serial_resources *);
LT48_API uint32_t lt48_gaaf_serial_capacity(const lt48_gaaf_serial_resources *);
LT48_API lt48_status lt48_gaaf_serial_validate(const lt48_gaaf_serial_resources *,lt48_operation_kind,
    uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_gaaf_serial_execute(const lt48_gaaf_serial_resources *,
    const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,
    const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,
    lt48_m37_id_decision_fn,void *,lt48_gaaf_serial_result *);
LT48_API lt48_status lt48_programmer_execute_gaaf_serial(lt48_programmer *,
    const lt48_gaaf_serial_resources *,lt48_operation_kind,uint32_t,size_t,
    const unsigned char *,size_t,unsigned char *,size_t,unsigned,
    lt48_verify_margin_selection,unsigned,lt48_m37_id_decision_fn,void *,lt48_gaaf_serial_result *);

/* Ordinary HC908 QT/QY, sparse 65536-byte image. Factory trim is preserved.
 * The eight-byte MON08 entry key is separate from the new image's vectors.
 * Blank key selects FF. Failed authentication stops all memory operations. */
/* HC11 A/E/L: 512-byte EEPROM plus separately selected CONFIG fields.
 * E2/F1/K extensions: 2048/512/640-byte EEPROM; six memory operations
 * preserve CONFIG by avoiding its writes. Extension CONFIG editing deferred.
 * Bootstrap entry on a security-enabled mask with NOSEC=0 inherently erases
 * EEPROM/RAM/CONFIG. Read cannot bypass that silicon behavior. */
/* NMOS68705 source profiles and offline operation preparation. These twelve
 * entries are NOT admitted: normal execute rejects before any I/O until NUM
 * electrical/variant contracts are qualified. Erase is external UV only. */
typedef struct lt48_nmos68705_resources lt48_nmos68705_resources;
LT48_API lt48_status lt48_nmos68705_open(const lt48_installation *,size_t,lt48_nmos68705_resources **);
LT48_API void lt48_nmos68705_close(lt48_nmos68705_resources *);
LT48_API int lt48_nmos68705_qualified(const lt48_nmos68705_resources *);
LT48_API const char *lt48_nmos68705_read_contract_status(const lt48_nmos68705_resources *);
LT48_API lt48_status lt48_nmos68705_validate(const lt48_nmos68705_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_nmos68705_execute(const lt48_nmos68705_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_nmos68705(lt48_programmer *,const lt48_nmos68705_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);

/* Compact HC705 source owner. Unqualified entries expose offline plans only;
 * all production execution paths reject before I/O until admission. */
typedef struct lt48_hc705_bootstrap_resources lt48_hc705_bootstrap_resources;
LT48_API lt48_status lt48_hc705_bootstrap_open(const lt48_installation *,size_t,lt48_hc705_bootstrap_resources **);
LT48_API void lt48_hc705_bootstrap_close(lt48_hc705_bootstrap_resources *);
LT48_API int lt48_hc705_bootstrap_qualified(const lt48_hc705_bootstrap_resources *);
LT48_API lt48_status lt48_hc705_bootstrap_validate(const lt48_hc705_bootstrap_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc705_bootstrap_execute(const lt48_hc705_bootstrap_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc705_bootstrap(lt48_programmer *,const lt48_hc705_bootstrap_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API unsigned lt48_hc705_bootstrap_blank_value(const lt48_hc705_bootstrap_resources *);
typedef struct lt48_hc705_ca_resources lt48_hc705_ca_resources;
LT48_API lt48_status lt48_hc705_ca_open(const lt48_installation *,size_t,lt48_hc705_ca_resources **);
LT48_API void lt48_hc705_ca_close(lt48_hc705_ca_resources *);
LT48_API int lt48_hc705_ca_qualified(const lt48_hc705_ca_resources *);
LT48_API lt48_status lt48_hc705_ca_validate(const lt48_hc705_ca_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc705_ca_execute(const lt48_hc705_ca_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc705_ca(lt48_programmer *,const lt48_hc705_ca_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API unsigned lt48_hc705_ca_blank_value(const lt48_hc705_ca_resources *);
typedef struct lt48_hc705_bd_resources lt48_hc705_bd_resources;
LT48_API lt48_status lt48_hc705_bd_open(const lt48_installation *,size_t,lt48_hc705_bd_resources **);
LT48_API void lt48_hc705_bd_close(lt48_hc705_bd_resources *);
LT48_API int lt48_hc705_bd_qualified(const lt48_hc705_bd_resources *);
LT48_API lt48_status lt48_hc705_bd_validate(const lt48_hc705_bd_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc705_bd_execute(const lt48_hc705_bd_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc705_bd(lt48_programmer *,const lt48_hc705_bd_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
typedef struct lt48_hc705_bx_resources lt48_hc705_bx_resources;
LT48_API lt48_status lt48_hc705_bx_open(const lt48_installation *,size_t,lt48_hc705_bx_resources **);
LT48_API void lt48_hc705_bx_close(lt48_hc705_bx_resources *);
LT48_API int lt48_hc705_bx_qualified(const lt48_hc705_bx_resources *);
LT48_API lt48_status lt48_hc705_bx_validate(const lt48_hc705_bx_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc705_bx_execute(const lt48_hc705_bx_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc705_bx(lt48_programmer *,const lt48_hc705_bx_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API unsigned lt48_hc705_bd_minimum_timeout_ms(const lt48_hc705_bd_resources *);
LT48_API unsigned lt48_hc705_bd_blank_value(const lt48_hc705_bd_resources *);
LT48_API unsigned lt48_hc705_bx_minimum_timeout_ms(const lt48_hc705_bx_resources *);
LT48_API unsigned lt48_hc705_bx_blank_value(const lt48_hc705_bx_resources *);

typedef struct lt48_hc705_jjsrpl_resources lt48_hc705_jjsrpl_resources;
LT48_API lt48_status lt48_hc705_jjsrpl_open(const lt48_installation *,size_t,lt48_hc705_jjsrpl_resources **);
LT48_API void lt48_hc705_jjsrpl_close(lt48_hc705_jjsrpl_resources *);
LT48_API int lt48_hc705_jjsrpl_qualified(const lt48_hc705_jjsrpl_resources *);
LT48_API lt48_status lt48_hc705_jjsrpl_validate(const lt48_hc705_jjsrpl_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc705_jjsrpl_execute(const lt48_hc705_jjsrpl_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc705_jjsrpl(lt48_programmer *,const lt48_hc705_jjsrpl_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API unsigned lt48_hc705_bd_minimum_timeout_ms(const lt48_hc705_bd_resources *);
LT48_API unsigned lt48_hc705_bd_blank_value(const lt48_hc705_bd_resources *);
LT48_API unsigned lt48_hc705_jjsrpl_minimum_timeout_ms(const lt48_hc705_jjsrpl_resources *);
LT48_API unsigned lt48_hc705_jjsrpl_blank_value(const lt48_hc705_jjsrpl_resources *);

typedef struct lt48_s08_resources lt48_s08_resources;
LT48_API lt48_status lt48_s08_open(const lt48_installation *,size_t,lt48_s08_resources **);
LT48_API void lt48_s08_close(lt48_s08_resources *);
LT48_API int lt48_s08_qualified(const lt48_s08_resources *);
LT48_API lt48_status lt48_s08_validate(const lt48_s08_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_s08_execute(const lt48_s08_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_s08(lt48_programmer *,const lt48_s08_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API unsigned lt48_hc705_bd_minimum_timeout_ms(const lt48_hc705_bd_resources *);
LT48_API unsigned lt48_hc705_bd_blank_value(const lt48_hc705_bd_resources *);
LT48_API unsigned lt48_s08_minimum_timeout_ms(const lt48_s08_resources *);
LT48_API unsigned lt48_s08_blank_value(const lt48_s08_resources *);
/* RS08 KA2: 16KiB address image, Flash3800..3FFF; trim preserved, unlocked only. */
typedef struct lt48_rs08_resources lt48_rs08_resources;
LT48_API lt48_status lt48_rs08_open(const lt48_installation *,size_t,lt48_rs08_resources **);
LT48_API void lt48_rs08_close(lt48_rs08_resources *);
LT48_API int lt48_rs08_qualified(const lt48_rs08_resources *);
LT48_API lt48_status lt48_rs08_validate(const lt48_rs08_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_rs08_execute(const lt48_rs08_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_rs08(lt48_programmer *,const lt48_rs08_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);


LT48_API unsigned lt48_rs08_minimum_timeout_ms(const lt48_rs08_resources *);
LT48_API unsigned lt48_rs08_blank_value(const lt48_rs08_resources *);
typedef struct lt48_hc705_jp_resources lt48_hc705_jp_resources;
LT48_API lt48_status lt48_hc705_jp_open(const lt48_installation *,size_t,lt48_hc705_jp_resources **);
LT48_API void lt48_hc705_jp_close(lt48_hc705_jp_resources *);
LT48_API int lt48_hc705_jp_qualified(const lt48_hc705_jp_resources *);
LT48_API lt48_status lt48_hc705_jp_validate(const lt48_hc705_jp_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc705_jp_execute(const lt48_hc705_jp_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc705_jp(lt48_programmer *,const lt48_hc705_jp_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API unsigned lt48_hc705_jp_blank_value(const lt48_hc705_jp_resources *);

typedef struct lt48_hc705_compact_resources lt48_hc705_compact_resources;
LT48_API lt48_status lt48_hc705_compact_open(const lt48_installation *,size_t,lt48_hc705_compact_resources **);
LT48_API void lt48_hc705_compact_close(lt48_hc705_compact_resources *);
LT48_API int lt48_hc705_compact_qualified(const lt48_hc705_compact_resources *);
LT48_API lt48_status lt48_hc705_compact_validate(const lt48_hc705_compact_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc705_compact_execute(const lt48_hc705_compact_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc705_compact(lt48_programmer *,const lt48_hc705_compact_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);

/* HC711 bootstrap EPROM: whole source-addressed image; no electrical erase.
 * Nonvolatile CONFIG is preserved; secured bootstrap may inherently erase EEPROM. */
typedef struct lt48_hc711_resources lt48_hc711_resources;
LT48_API lt48_status lt48_hc711_open(const lt48_installation *,size_t,lt48_hc711_resources **);
LT48_API void lt48_hc711_close(lt48_hc711_resources *);
LT48_API lt48_status lt48_hc711_validate(const lt48_hc711_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc711_execute(const lt48_hc711_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);
LT48_API lt48_status lt48_programmer_execute_hc711(lt48_programmer *,const lt48_hc711_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_small_eprom_result *);

typedef struct lt48_hc11_resources lt48_hc11_resources;
typedef struct lt48_hc11_result {lt48_small_eprom_result operation; unsigned configuration_valid; unsigned char configuration;} lt48_hc11_result;
LT48_API int lt48_hc11_is_extension(const lt48_hc11_resources *resources);
LT48_API lt48_status lt48_hc11_open(const lt48_installation *,size_t,lt48_hc11_resources **);
LT48_API void lt48_hc11_close(lt48_hc11_resources *);
/* mask/value select EEON, ROMON, NOCOP, NOSEC (bits0..3). mask0 reads only.
 * Clearing NOSEC is not admitted: reset verification can erase secured parts. */
LT48_API lt48_status lt48_hc11_set_configuration(lt48_hc11_resources *,unsigned,unsigned);
LT48_API lt48_status lt48_hc11_validate(const lt48_hc11_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc11_execute(const lt48_hc11_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_hc11_result *);
LT48_API lt48_status lt48_programmer_execute_hc11(lt48_programmer *,const lt48_hc11_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_hc11_result *);

typedef struct lt48_hc908_resources lt48_hc908_resources;
typedef lt48_small_eprom_result lt48_hc908_result;
LT48_API lt48_status lt48_hc908_open(const lt48_installation *,size_t,lt48_hc908_resources **);
LT48_API void lt48_hc908_close(lt48_hc908_resources *);
/* Selected Flash erased value; GP20 uses00, later HC908 usesFF. */
LT48_API unsigned lt48_hc908_blank_value(const lt48_hc908_resources *);
LT48_API unsigned lt48_hc908_flash_start(const lt48_hc908_resources *);
LT48_API lt48_status lt48_hc908_set_security_key(lt48_hc908_resources *,const unsigned char *,size_t);
LT48_API lt48_status lt48_hc908_validate(const lt48_hc908_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,lt48_verify_margin_selection,unsigned);
LT48_API lt48_status lt48_hc908_execute(const lt48_hc908_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,lt48_verify_margin_selection,unsigned,lt48_hc908_result *);
LT48_API lt48_status lt48_programmer_execute_hc908(lt48_programmer *,const lt48_hc908_resources *,lt48_operation_kind,uint32_t,size_t,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_verify_margin_selection,unsigned,lt48_hc908_result *);

#ifdef __cplusplus
}
#endif

#endif
