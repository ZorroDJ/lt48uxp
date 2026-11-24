#ifndef LABTOOL48_INTERNAL_H
#define LABTOOL48_INTERNAL_H
lt48_status lt48_word14_open_internal(const lt48_installation *,size_t,lt48_gaaf_serial_resources **);
unsigned lt48_gaaf_serial_is_word14_internal(const lt48_gaaf_serial_resources*);
int lt48_hitachi_is15_internal(const lt48_hitachi_resources*);
int lt48_w78legacy90_admitted_internal(unsigned);

/* Host interpreter buffer capacity; capture destinations use the same limit. */
#define LT48_RUNTIME_BUFFER_COUNT 81u

#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>

#include <libusb-1.0/libusb.h>

struct lt48_context {
    libusb_context *usb;
};

typedef lt48_status (*lt48_programmer_bulk_fn)(
    void *context,
    uint8_t endpoint,
    unsigned char *data,
    size_t length,
    unsigned timeout_ms,
    size_t *out_transferred
);

typedef lt48_status (*lt48_programmer_release_fn)(void *context);

struct lt48_programmer {
    libusb_device_handle *handle;
    uint16_t product_id;
    uint8_t bus_number;
    uint8_t device_address;
    int interface_claimed;
    void *backend_context;
    lt48_programmer_bulk_fn bulk;
    lt48_programmer_release_fn release;
    unsigned execution_timeout_ms;
    int execution_active;
    int transport_ready;
    int m37_read_armed;
    int m37_operation_armed;
    lt48_m37_operation armed_m37_operation;
    int auto_id_eprom_armed;
    lt48_cancel_check_fn cancel_check;
    void *cancel_context;
    lt48_m37_id_decision_fn id_decide;
    void *id_decision_context;
    atomic_uint_fast64_t progress_revision;
    atomic_int progress_stage;
    atomic_size_t progress_completed;
    atomic_size_t progress_total;
    atomic_uint progress_cleanup_phase;
    atomic_size_t progress_cleanup_index;
    atomic_size_t progress_cleanup_count;
    unsigned char armed_manifest_sha256[LT48_SHA256_SIZE];
    unsigned char armed_plan_sha256[LT48_SHA256_SIZE];
    unsigned char armed_auto_id_confirmation_sha256[LT48_SHA256_SIZE];
};

typedef struct lt48_sha256_context {
    uint32_t state[8];
    uint64_t total_size;
    unsigned char block[64];
    size_t block_size;
} lt48_sha256_context;

void lt48_sha256_init(lt48_sha256_context *context);
void lt48_sha256_update(
    lt48_sha256_context *context, const unsigned char *data, size_t size
);
void lt48_sha256_final(
    lt48_sha256_context *context, unsigned char digest[32]
);

lt48_status lt48_hash_file_internal(
    const char *path, unsigned char digest[LT48_SHA256_SIZE], size_t *out_size
);
lt48_status lt48_load_verified_idle_fpga_internal(
    const char *root, unsigned char **out_data, size_t *out_size
);
void lt48_buffer_set_image_provenance_internal(
    lt48_buffer *buffer, const lt48_image_import_options *options,
    const lt48_image_import_result *result
);
lt48_status lt48_component_evidence_verify_with_matrix_internal(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path, const lt48_component_plan_matrix *matrix,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_verification *out_verification
);

lt48_status from_libusb_error_internal(int result);

lt48_status lt48_protocol_plan_m37_recovery_state_internal(
    const lt48_protocol_plan *plan,
    const lt48_m37_resources *resources,
    size_t completed_transactions,
    lt48_status operation_status,
    lt48_m37_recovery_state *out_state
);

void lt48_programmer_progress_publish_internal(
    void *programmer, lt48_execution_stage stage,
    size_t completed_transactions, size_t total_transactions,
    unsigned cleanup_phase, size_t cleanup_phase_index,
    size_t cleanup_phase_count
);

size_t lt48_protocol_plan_packet_size_internal(const lt48_protocol_plan *plan);
lt48_status lt48_protocol_plan_select_id_policy_internal(lt48_protocol_plan *plan);

lt48_status lt48_m37_resources_catalogue_ids_internal(
    const lt48_m37_resources *resources, uint16_t *manufacturer, uint16_t *device
);

/* Offline candidate until full mismatch/choice workflow qualification. */
/* Unapproved offline Retry candidate for the pinned captured profile. */
lt48_status lt48_m37_resources_build_id_retry_candidate_internal(
    const lt48_m37_resources *resources, lt48_protocol_plan **out_plan
);

/* Unapproved offline Ignore candidate for the pinned captured profile. */
lt48_status lt48_m37_resources_build_id_ignore_candidate_internal(
    const lt48_m37_resources *resources, lt48_protocol_plan **out_plan
);

lt48_status lt48_m37_resources_build_id_mismatch_cleanup_internal(
    const lt48_m37_resources *resources, int post_abort,
    lt48_protocol_plan **out_plan
);

lt48_status lt48_protocol_plan_execute_id_cleanup_internal(
    const lt48_protocol_plan *plan, const lt48_protocol_transport *transport,
    void *transport_context, lt48_m37_recovery_execution_result *out_result
);

/* Internal captured-profile decision loop; no live authorization is granted. */
typedef lt48_m37_id_choice lt48_m37_id_choice_internal;
typedef lt48_m37_id_decision_fn lt48_m37_id_decide_internal;
#define LT48_M37_ID_ABORT_INTERNAL LT48_M37_ID_ABORT
#define LT48_M37_ID_RETRY_INTERNAL LT48_M37_ID_RETRY
#define LT48_M37_ID_IGNORE_INTERNAL LT48_M37_ID_IGNORE
void lt48_m37_read_collector_replace_internal(
    lt48_m37_read_collector *target, lt48_m37_read_collector *source);
lt48_status lt48_m37_read_id_owned_internal(
    const lt48_m37_resources *resources, const lt48_protocol_plan *plan,
    const lt48_protocol_transport *transport, void *transport_context,
    lt48_m37_id_decision_fn decide, void *decision_context,
    lt48_m37_read_collector *collector, const lt48_cleanup_action *release,
    lt48_cleanup_result *results, size_t capacity,
    lt48_m37_snapshot_read_execution_result *out_result);
typedef struct lt48_m37_id_loop_result_internal {
    size_t attempts;
    size_t decisions;
    size_t operation_completed_transactions;
    size_t cleanup_completed_transactions;
    lt48_status operation_status;
    lt48_status cleanup_status;
    /* Recovery uses the last attempt's layout, never the cumulative count.
     * The caller still owns interface release. These fields do not authorize
     * an unqualified recovery sequence or hardware execution. */
    size_t last_attempt_completed_transactions;
    size_t last_attempt_transaction_count;
    int needs_failure_recovery;
    int recovery_state_valid;
    lt48_m37_recovery_state recovery_state;
} lt48_m37_id_loop_result_internal;
lt48_status lt48_m37_read_id_loop_candidate_internal(
    const lt48_m37_resources *resources,
    const lt48_protocol_transport *transport, void *transport_context,
    lt48_m37_id_decide_internal decide, void *decision_context,
    lt48_m37_read_collector **out_collector,
    lt48_m37_id_loop_result_internal *out_result);

/* Source range helper 0044e430: bytes/100, capped at 8192 and element-aligned.
 * Retain the captured legacy transaction partition for pre-existing profiles. */
static inline uint16_t lt48_eprom_chunk(const lt48_m37_resource_audit *a, uint32_t length) {
    if(a->capacity_bytes<131072u)return 163u;
    unsigned width=(a->device_class==2003u || a->device_class==2004u)?2u:1u;
    uint32_t n=length/100u;if(n>8192u)n=8192u;
    n-=n%width;return (uint16_t)(n?n:width);
}

lt48_status lt48_protocol_plan_discard_completed_internal(lt48_protocol_plan *plan);

lt48_status lt48_protocol_plan_sample_pin_internal(lt48_protocol_plan *, uint8_t, uint8_t);

/* NVRAM STORE power hold; no USB or nonvolatile programmer access. */
lt48_status lt48_nvram_store_hold_internal(void);
lt48_status lt48_timekeeper_power_hold_internal(void);
lt48_status lt48_pin_map_limit_group_internal(lt48_pin_map *, unsigned, size_t, size_t);

const lt48_compile_variable *lt48_palce_resource_variables_internal(const lt48_palce_resources *,size_t *);
lt48_status lt48_palce_machine_variables_internal(const lt48_palce_resources *,uint16_t,uint32_t,
    const uint32_t[4],const lt48_runtime_buffer_seed *,size_t,
    const lt48_compile_variable *,size_t,lt48_runtime_machine **);

/* Best effort indicator-only error report using a fresh queue after the
 * normal completion indicator failed. Never repeats device programming. */
lt48_status lt48_runtime_transport_error_indicator_internal(
    const lt48_protocol_transport *transport, void *context);

#define LT48_EPM_LEGACY_STATE_SIZE 814u
lt48_status lt48_compiled_host_words_internal(const uint16_t *words, size_t count, lt48_compiled_wave **out);
lt48_status lt48_epm_legacy_compile_internal(unsigned callback, lt48_compiled_wave **out);
lt48_status lt48_epm_legacy_step_internal(unsigned op, unsigned arg, unsigned char buffers[][32], lt48_compile_variable *variables, size_t count, unsigned char *state);
lt48_status lt48_pal_resources_machine_state_internal(const lt48_pal_resources *resources, uint16_t operation, uint32_t mode, const uint32_t voltage[4], const lt48_compile_variable *variables, size_t variable_count, const lt48_runtime_buffer_seed *seeds, size_t seed_count, const unsigned char *state, const lt48_runtime_configuration_value *configuration, size_t configuration_count, lt48_runtime_machine **out);
lt48_status lt48_pal_resources_machine_image_internal(const lt48_pal_resources *resources, uint16_t operation, uint32_t mode, const uint32_t voltage[4], const lt48_compile_variable *variables, size_t variable_count, const lt48_runtime_buffer_seed *seeds, size_t seed_count, const unsigned char *state, const lt48_runtime_configuration_value *configuration, size_t configuration_count, const lt48_runtime_fuse_image *image, lt48_runtime_machine **out);
int lt48_pal_resources_project_image_internal(const lt48_pal_resources *resources);
int lt48_pal_resources_legacy_epm_internal(const lt48_pal_resources *resources);
typedef struct lt48_pof_trs_source lt48_pof_trs_source;
lt48_status lt48_pof_trs_source_parse_internal(const unsigned char *data, size_t size,
    lt48_pof_trs_source **out);
lt48_status lt48_pof_trs_source_open_internal(const char *root,lt48_pof_trs_source **out);
lt48_status lt48_pof_source_bundle_internal(const lt48_pof_trs_source *,unsigned char **,size_t *);
void lt48_pof_trs_source_close_internal(lt48_pof_trs_source *source);
lt48_status lt48_pof_source_fpga_internal(const lt48_pof_trs_source *source,
    unsigned selector,const unsigned char **data,size_t *size);
lt48_status lt48_pal_resources_pof_source_internal(lt48_pal_resources *resources,
    const lt48_pof_trs_source *source);
lt48_status lt48_pal_resources_pof_fpga_internal(const lt48_pal_resources *resources,
    unsigned *selector,const unsigned char **data,size_t *size);
lt48_status lt48_pal_resources_parameter_internal(const lt48_pal_resources *resources,
    const char *name,uint16_t *id,uint32_t *value);
lt48_status lt48_pof_trs_source_coordinate_internal(const lt48_pof_trs_source *source,
    unsigned kind, uint16_t address, uint16_t position, unsigned bit, uint32_t out[3]);
typedef struct lt48_pof_trs_image lt48_pof_trs_image;
void lt48_pof_trs_image_close_internal(lt48_pof_trs_image *image);
const unsigned char *lt48_pof_trs_image_cells_internal(const lt48_pof_trs_image *image,
    size_t *size, uint32_t *columns, uint32_t *rows);
lt48_status lt48_pof_trs_image_create_internal(const lt48_pof_trs_source *source,
    unsigned kind, const lt48_pof_core *core, const unsigned char *input, size_t size,
    lt48_pof_trs_image **out);
lt48_status lt48_pof_trs_image_rebuild_internal(const lt48_pof_trs_source *source,
    unsigned kind, const lt48_pof_core *core, lt48_pof_trs_image *image,
    unsigned char *output, size_t size);
lt48_status lt48_pof_trs_coordinate_internal(unsigned kind, uint16_t address,
    uint16_t position, unsigned bit, const unsigned char *const tables[6],
    const size_t sizes[6], uint32_t out[3]);
int lt48_pal_resources_pof_internal(const lt48_pal_resources *resources);
const lt48_pof_core *lt48_pal_resources_pof_core_internal(const lt48_pal_resources *resources);
lt48_status lt48_pal_resources_operation_internal(const lt48_pal_resources *resources, const char *name, uint16_t *out);
int lt48_pal_resources_compact20_internal(const lt48_pal_resources*);
int lt48_pal_resources_xc95_internal(const lt48_pal_resources *resources);
unsigned lt48_pal_resources_xc95_protection_internal(const lt48_pal_resources *r);
lt48_status lt48_compiled_word_table_internal(const unsigned char *,size_t,lt48_compiled_wave **);

lt48_status lt48_pin_map_complete_groups_internal(lt48_pin_map *map, size_t count);

/* Shared compact-owner mechanics. Family code supplies all electrical waves,
 * modes, applet addresses and cleanup prefix; these helpers select no pins. */
typedef lt48_status (*lt48_owner_cleanup_wave_internal)(void *, unsigned);
lt48_status lt48_owner_rez_open_internal(const char *, const char *, lt48_rez **);
lt48_status lt48_owner_load_fpga_internal(lt48_runtime_transport_session *, const unsigned char *, size_t);
lt48_status lt48_owner_load_applet_internal(lt48_runtime_transport_session *, uint16_t, const unsigned char *, size_t);
lt48_status lt48_protocol_plan_status_capture_internal(lt48_protocol_plan *,int);
lt48_status lt48_runtime_transport_session_controller_status_internal(
    lt48_runtime_transport_session *,int,uint16_t *);
lt48_status lt48_protocol_plan_controller_capture_internal(lt48_protocol_plan *,unsigned);
lt48_status lt48_runtime_transport_session_controller_capture_internal(lt48_runtime_transport_session *,unsigned char *,unsigned);
lt48_status lt48_owner_run_internal(lt48_runtime_transport_session *,
    const lt48_compiled_wave *, const lt48_pin_map *,
    const lt48_compile_variable *, size_t, const lt48_runtime_options *,
    const lt48_applet_address *, size_t, unsigned char [32][32],
    lt48_runtime_configuration_value *, size_t, size_t *, uint32_t [4]);
lt48_status lt48_owner_run_mutable_internal(lt48_runtime_transport_session *,
    const lt48_compiled_wave *, const lt48_pin_map *,
    lt48_compile_variable *, size_t, const lt48_runtime_options *,
    const lt48_applet_address *, size_t, unsigned char [32][32],
    lt48_runtime_configuration_value *, size_t, size_t *, uint32_t [4]);
/* Caller closes the optional finished runtime result, including on mismatch. */
lt48_status lt48_owner_run_result_internal(lt48_runtime_transport_session *,
    const lt48_compiled_wave *, const lt48_pin_map *,
    const lt48_compile_variable *, size_t, const lt48_runtime_options *,
    const lt48_applet_address *, size_t, unsigned char [32][32],
    lt48_runtime_configuration_value *, size_t, size_t *, uint32_t [4],lt48_runtime_result **);
lt48_status lt48_owner_finish_internal(const lt48_protocol_transport *, void *,
    lt48_runtime_transport_session **, lt48_owner_cleanup_wave_internal, void *,
    const unsigned *, size_t, lt48_status, lt48_status *, int *);
/* Optional verified idle FPGA is loaded after all rail-off attempts and before
 * release/result indication; a failed phase never suppresses later phases. */
lt48_status lt48_owner_finish_fpga_internal(const lt48_protocol_transport *,void *,
    lt48_runtime_transport_session **,lt48_owner_cleanup_wave_internal,void *,
    const unsigned *,size_t,const unsigned char *,size_t,lt48_status,lt48_status *,int *);


const char *lt48_config_memory_profile_internal(const lt48_config_memory_resources *);
unsigned lt48_config_memory_mask_internal(const lt48_config_memory_resources *);
const char *lt48_xc17_profile_internal(const lt48_xc17_resources *);
unsigned lt48_xc17_mask_internal(const lt48_xc17_resources *);

lt48_status lt48_xc95_execute_internal(const lt48_pal_resources *,const lt48_protocol_transport *,void *,lt48_operation_kind,const unsigned char *,size_t,unsigned char *,size_t,unsigned,lt48_pal_operation_result *);

lt48_status lt48_pof_core_import_internal(const lt48_pof_core *,
    const unsigned char *,size_t,unsigned char *,size_t,int *);
lt48_status lt48_pof_core_export_internal(const lt48_pof_core *,
    const unsigned char *,size_t,int,unsigned char *,size_t,size_t *);
lt48_status lt48_pof_core_parse_internal(const unsigned char *body,
    size_t size, lt48_pof_core *out);

lt48_status lt48_pof_execute_internal(const lt48_pal_resources *resources,
    const lt48_pof_trs_source *source,const lt48_protocol_transport *transport,
    void *context,lt48_operation_kind operation,const unsigned char *input,
    size_t input_size,unsigned char *output,size_t output_size,
    lt48_pof_operation_result *result);
typedef lt48_status (*lt48_pof_core_exchange)(void *,unsigned,uint16_t,
    uint16_t,uint16_t,uint16_t,uint32_t,unsigned,uint16_t *,uint16_t *);
lt48_status lt48_pof_image_run_internal(const lt48_pal_resources *resources,
    const lt48_pof_trs_source *source,unsigned mode,const unsigned char *input,
    size_t size,unsigned char *output,lt48_pof_core_exchange exchange,void *context,
    lt48_pof_core_result *result);
lt48_status lt48_pof_session_image_internal(lt48_pal_wave_session *session,
    const lt48_pof_trs_source *source,unsigned mode,const unsigned char *input,
    size_t size,unsigned char *output,lt48_pof_core_result *result);
lt48_status lt48_pof_session_exchange_internal(void *context,unsigned mode,
    uint16_t tag,uint16_t column,uint16_t row,uint16_t mask,uint32_t address,
    unsigned width,uint16_t *expected,uint16_t *actual);
lt48_status lt48_pof_trs_image_run_internal(lt48_pof_trs_image *image,unsigned mode,
    unsigned char blank,lt48_pof_core_exchange exchange,void *context,
    lt48_pof_core_result *out);

lt48_status lt48_pof_core_run_internal(const lt48_pof_core *,unsigned,
    unsigned char,const unsigned char *,size_t,unsigned char *,
    lt48_pof_core_exchange,void *,lt48_pof_core_result *);

lt48_status lt48_pof_exception_internal(unsigned callback,uint16_t row,
    uint16_t column,uint16_t *data_in,uint16_t *data_out,uint16_t *mask);

lt48_status lt48_pof_exception_compile_internal(unsigned callback,
    const uint8_t buffers[5],lt48_compiled_wave **out);

lt48_status lt48_pof_exception_bind_internal(const lt48_rez *rez,size_t wave_index,
    lt48_compiled_wave **out);

lt48_status lt48_pof_core_import_metadata_internal(const lt48_pof_core *,const unsigned char *,size_t,unsigned char *,size_t,lt48_pof_file_metadata *);
lt48_status lt48_pof_core_export_metadata_internal(const lt48_pof_core *,const unsigned char *,size_t,const lt48_pof_file_metadata *,unsigned char *,size_t,size_t *);

/* Original 00440160 row semantics. Callbacks are host-method boundaries,
 * not USB commands; electrical setup and cleanup belong to the caller. */
typedef struct lt48_vector_cycle_io {
    lt48_status (*drive)(void *,unsigned);
    lt48_status (*control)(void *,unsigned);
    lt48_status (*sample)(void *,unsigned,unsigned char *);
} lt48_vector_cycle_io;

lt48_status lt48_vector_cycle_internal(const char *,size_t,unsigned,
    const lt48_vector_cycle_io *,void *,lt48_vector_cycle_result *);

lt48_status lt48_protocol_plan_pair_internal(lt48_protocol_plan *,uint8_t,uint8_t);
/* Requires a caller-owned, electrically prepared vector session. */
lt48_status lt48_vector_cycle_transport_internal(const char *,size_t,unsigned,
    const lt48_protocol_transport *,void *,lt48_vector_cycle_result *);

typedef lt48_status (*lt48_vector_packet_fn)(void *,const unsigned char *,size_t);
lt48_status lt48_vector_power_internal(const unsigned char *,size_t,const unsigned char *,size_t,
    unsigned,int,lt48_vector_packet_fn,void *);

typedef struct lt48_vector_device_profile {
    uint16_t power_off, supply_variable;
    unsigned pin_count;
    uint32_t centivolts;
    unsigned char ground[48], supply[48];
    size_t ground_count, supply_count;
} lt48_vector_device_profile;
lt48_status lt48_pal_resources_vector_profile_internal(const lt48_pal_resources *,lt48_vector_device_profile *);

lt48_status lt48_pof_vector_prepare_internal(lt48_pal_wave_session *);


lt48_status lt48_pof_vectors_validate_internal(const lt48_pal_resources *,const lt48_pof_file_metadata *,unsigned,unsigned);
lt48_status lt48_pof_vectors_execute_internal(const lt48_pal_resources *,const lt48_pof_file_metadata *,
    unsigned,unsigned,const lt48_protocol_transport *,void *,lt48_vector_operation_result *);

lt48_status lt48_rez_xilinx_table_internal(const lt48_rez *,size_t,const unsigned char **,size_t *);
/* Private source-audit integration; never used for public device admission. */
lt48_status lt48_addtab_tail_word_open_internal(const lt48_installation *,size_t,lt48_addtab_flash_resources **);
lt48_status lt48_addtab_amd_mx6_open_internal(const lt48_installation *,size_t,lt48_addtab_flash_resources **);
lt48_status lt48_addtab_byte8_open_internal(const lt48_installation *,size_t,lt48_addtab_flash_resources **);
uint32_t lt48_pic_operation_mask_internal(const lt48_pic_resources *);
int lt48_pic_is_hms77two_internal(const lt48_pic_resources*);
int lt48_pic_is_hyundai4_internal(const lt48_pic_resources *);
int lt48_pic_is_baseline12_otp_internal(const lt48_pic_resources *);
int lt48_pic_is_tail14_internal(const lt48_pic_resources *);
int lt48_pic_is_older17_internal(const lt48_pic_resources *);
int lt48_pic_is_pic18c_internal(const lt48_pic_resources *);
int lt48_pic_is_remaining14_internal(const lt48_pic_resources *);
int lt48_pic_is_calibrated14_internal(const lt48_pic_resources *);
const char *lt48_pic_otp_adapter_note_internal(const lt48_installation *,size_t);
uint32_t lt48_pic_image_capacity_internal(const lt48_installation *,const lt48_device_spec *);
uint32_t lt48_pic24_image_capacity_internal(const lt48_installation *,const lt48_device_spec *);
lt48_status lt48_pal46_idle_internal(const lt48_pal_resources*,const unsigned char**,size_t*);
lt48_status lt48_pal_resources_section_internal(const lt48_pal_resources *r,unsigned section,const lt48_fuse_code **code,const lt48_fuse_coordinate **coordinates,size_t *count);

int lt48_pal_resources_ep35_internal(const lt48_pal_resources *resources);
/* Shared GAL host image/blank state; original resources remain digest-bound. */
unsigned lt48_gal_resources_blank_fill_internal(const lt48_gal_resources *);
int lt48_gal_resources_requires_image_internal(const lt48_gal_resources *);
lt48_status lt48_gal_resources_image_machine_internal(
 const lt48_gal_resources *,uint16_t,uint32_t,const uint32_t[4],
 const lt48_compile_variable *,size_t,const lt48_runtime_buffer_seed *,size_t,
 const lt48_runtime_fuse_image *,lt48_runtime_machine **);

#endif

int lt48_eep21_definition_internal(unsigned cls,unsigned definition);

unsigned lt48_m37_eep21_write_voltage_internal(const lt48_m37_resources *resources);

unsigned lt48_eep21_read_nominal_internal(const lt48_device_spec *spec);

/* Owned changed host image, available only when expanded6D/6E wrote it. */
lt48_status lt48_runtime_host_fuse_image_internal(const lt48_runtime_result*,const lt48_runtime_fuse_image**);
/* Opt-in, fail-closed continuation for ordinary pin-group capture callbacks.
 * Only a freshly created machine can enable it; public options ABI unchanged. */
lt48_status lt48_runtime_machine_stream_resume_internal(lt48_runtime_machine *);
lt48_status lt48_runtime_machine_pin_resume_internal(lt48_runtime_machine*);

lt48_status lt48_runtime_device_image_internal(const lt48_runtime_result*,const unsigned char**,size_t*);

lt48_status lt48_parallel20_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_parallel20_member_internal(unsigned,unsigned);
int lt48_parallel20_admitted_internal(unsigned);

lt48_status lt48_sharp13_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_sharp13_member_internal(unsigned,unsigned);
int lt48_sharp13_admitted_internal(unsigned);

lt48_status lt48_nxp18_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_nxp18_member_internal(unsigned,unsigned);
int lt48_nxp18_admitted_internal(unsigned);

lt48_status lt48_emtc18_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_emtc18_member_internal(unsigned,unsigned);
int lt48_emtc18_admitted_internal(unsigned);

lt48_status lt48_lpc16_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_lpc16_member_internal(unsigned,unsigned);
int lt48_lpc16_admitted_internal(unsigned);

lt48_status lt48_nand15_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_nand15_member_internal(unsigned,unsigned);
int lt48_nand15_admitted_internal(unsigned);

int lt48_page6_member_internal(unsigned,unsigned);

lt48_status lt48_xp13_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_xp13_binding_internal(const lt48_catalogue_entry*);
int lt48_xp13_admitted_internal(void);

lt48_status lt48_tpi15_open_private(const lt48_installation*,size_t,lt48_xp_flash_resources**);
int lt48_tpi15_binding_internal(const lt48_catalogue_entry*);
int lt48_tpi15_admitted_internal(void);

unsigned lt48_addtab_spi_is_spiw6_internal(const lt48_addtab_spi_resources*);

int lt48_spiw6_binding_internal(const lt48_catalogue_entry*);

unsigned lt48_gal_source_buffer_count_internal(const lt48_gal_resources*);

unsigned lt48_addtab_spi_is_sst49c7_internal(const lt48_addtab_spi_resources*);

unsigned lt48_addtab_spi_is_df28_internal(const lt48_addtab_spi_resources*);

unsigned lt48_addtab_spi_is_sstfwh_internal(const lt48_addtab_spi_resources*);

lt48_status lt48_utron4_open_internal(const lt48_installation*,size_t,lt48_gaaf_serial_resources**);

unsigned lt48_mcu51_is_hy24_internal(const lt48_mcu51_resources*);

unsigned lt48_small_eprom_is_hyundai25_internal(const lt48_small_eprom_resources*);

unsigned lt48_small_eprom_is_hyundai18_internal(const lt48_small_eprom_resources*);

unsigned lt48_small_eprom_is_hyundai13_internal(const lt48_small_eprom_resources*);

lt48_status lt48_contact_dispatcher_load_internal(const char *, unsigned char [476]);
