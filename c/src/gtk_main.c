#include <labtool48/labtool48.h>

#include "property_sheet.h"

#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>

typedef struct operation_options_snapshot {
    char *start;
    char *length;
    char *output;
    char *timeout;
    char *image;
    char *image_base;
    char *image_gap_fill;
    char *plan_report;
    char *execution_report;
    guint image_format;
    guint image_gap_policy;
    guint verify_passes;
    guint verify_margin_policy;
    guint verify_margin_type;
    gboolean use_resident_buffer;
    gboolean identity_override;
    int valid;
} operation_options_snapshot;

typedef struct app_widgets {
    GtkWindow *window;
    GtkWindow *device_select_window;
    GtkWindow *auto_id_window;
    GtkWindow *operation_options_window;
    GtkWindow *programming_parameters_window;
    GtkWindow *buffer_edit_window;
    GtkWindow *buffer_fill_window;
    GtkWindow *buffer_copy_window;
    GtkWindow *buffer_swap_window;
    GtkWindow *load_image_window;
    GtkWindow *save_image_window;
    GtkWindow *database_manager_window;
    GtkWindow *machine_select_window;
    GtkWindow *project_description_window;
    GtkWindow *operation_progress_window;
    GtkWindow *statistics_window;
    GtkWindow *mass_setup_window;
    GtkWindow *mass_monitor_window;
    GtkWindow *self_test_window;
    GtkWindow *help_window;
    GtkWidget *main_notebook;
    GtkWidget *device_workflow;
    GtkWidget *toolbar_save_button;
    GtkWidget *toolbar_edit_button;
    GtkWidget *toolbar_vector_button;
    GtkWidget *toolbar_option_button;
    GtkWidget *quick_operation_buttons[LT48_OPERATION_KIND_COUNT];
    GtkWidget *programmer_list;
    GtkWidget *programmer_status;
    GtkWidget *machine_list;
    GtkWidget *machine_status;
    GtkWidget *project_path;
    GtkWidget *project_description;
    GtkWidget *project_buffer_policy;
    GtkWidget *project_buffer_status;
    GtkWidget *project_description_status;
    GtkWidget *statistics_target_entry;
    GtkWidget *statistics_max_failure_entry;
    GtkWidget *statistics_enabled_check;
    GtkWidget *statistics_alarm_check;
    GtkWidget *statistics_status;
    GtkWidget *operation_progress_heading;
    GtkWidget *operation_progress_status;
    GtkWidget *operation_progress_bar;
    GtkWidget *operation_progress_cancel;
    GtkWidget *mass_unlimited_check;
    GtkWidget *mass_success_check;
    GtkWidget *mass_success_entry;
    GtkWidget *mass_failure_check;
    GtkWidget *mass_failure_entry;
    GtkWidget *mass_setup_status;
    GtkWidget *mass_monitor_status;
    GtkWidget *mass_monitor_stop_total;
    GtkWidget *mass_monitor_stop_failure;
    GtkWidget *programmer_bus_entry;
    GtkWidget *programmer_address_entry;
    GtkWidget *programmer_timeout_entry;
    GtkWidget *programmer_initialize_button;
    GtkWidget *programmer_fx2_path_entry;
    GtkWidget *programmer_fx2_confirmation_entry;
    GtkWidget *programmer_fx2_prepare_button;
    GtkWidget *programmer_fx2_execute_button;
    GtkWidget *selected_programmer_status;
    GtkWidget *operation_device_status;
    GtkWidget *installation_entry;
    GtkWidget *database_dropdown;
    GtkWidget *database_manage_button;
    GtkWidget *database_list;
    GtkWidget *database_manager_status;
    GtkWidget *cache_import_button;
    GtkWidget *catalogue_verify_button;
    GtkWidget *catalogue_search_button;
    GtkWidget *catalogue_rank_button;
    GtkWidget *auto_id_button;
    GtkWidget *auto_id_vendor_list;
    GtkWidget *auto_id_list;
    GtkWidget *auto_id_status;
    GtkWidget *auto_id_accept_button;
    GtkWidget *catalogue_cancel_button;
    GtkWidget *query_entry;
    GtkWidget *device_type_filters[5];
    GtkWidget *auto_manufacturer_entry;
    GtkWidget *auto_device_entry;
    GtkWidget *auto_pins_entry;
    GtkWidget *auto_class_entry;
    GtkWidget *auto_eprom_mode_dropdown;
    GtkWidget *vendor_list;
    GtkWidget *device_list;
    GtkWidget *device_status;
    GtkWidget *device_details;
    GtkWidget *device_type_value;
    GtkWidget *device_adapter_value;
    GtkWidget *device_manufacturer_value;
    GtkWidget *device_code_value;
    GtkWidget *device_note_value;
    GtkWidget *selection_accept_button;
    GtkWidget *socket_diagram;
    lt48_device_guidance socket_guidance;
    int has_socket_guidance;
    GtkWidget *read_start_entry;
    GtkWidget *read_length_entry;
    GtkWidget *read_output_entry;
    GtkWidget *read_bus_entry;
    GtkWidget *read_address_entry;
    GtkWidget *read_timeout_entry;
    GtkWidget *operation_dropdown;
    GtkWidget *verify_image_entry;
    GtkWidget *verify_image_browse_button;
    GtkWidget *image_inspect_button;
    GtkWidget *image_status;
    GtkWidget *image_format_dropdown;
    GtkWidget *image_base_entry;
    GtkWidget *image_gap_dropdown;
    GtkWidget *image_gap_fill_entry;
    GtkWidget *verify_use_buffer_check;
    GtkWidget *verify_margin_dropdown;
    GtkWidget *verify_margin_type_dropdown;
    GtkWidget *parameters_margin_dropdown;
    GtkWidget *parameters_nominal;
    GtkWidget *parameters_low;
    GtkWidget *parameters_high;
    GtkWidget *parameters_status;
    GtkWidget *program_identity_override_check;
    GtkWidget *operation_report_entry;
    GtkWidget *execution_report_entry;
    GtkWidget *read_preparation;
    GtkWidget *central_log;
    GtkWidget *read_confirmation_entry;
    GtkWidget *read_execute_button;
    GtkWidget *read_cancel_button;
    GtkWidget *operation_prepare_button;
    GtkWidget *operation_prepare_cancel_button;
    GtkWidget *buffer_input_entry;
    GtkWidget *buffer_size_entry;
    GtkWidget *buffer_start_entry;
    GtkWidget *buffer_length_entry;
    GtkWidget *buffer_value_entry;
    GtkWidget *buffer_output_entry;
    GtkWidget *buffer_compare_entry;
    GtkWidget *buffer_short_dropdown;
    GtkWidget *buffer_long_dropdown;
    GtkWidget *buffer_word_entry;
    GtkWidget *buffer_byte_order_dropdown;
    GtkWidget *buffer_reverse_check;
    GtkWidget *buffer_status;
    GtkWidget *buffer_preview;
    GtkWidget *buffer_edit_address;
    GtkWidget *buffer_edit_hex;
    GtkWidget *buffer_edit_view;
    GtkWidget *buffer_edit_search;
    GtkWidget *buffer_edit_status;
    GtkWidget *buffer_fill_start;
    GtkWidget *buffer_fill_end;
    GtkWidget *buffer_fill_value;
    GtkWidget *buffer_copy_source;
    GtkWidget *buffer_copy_destination;
    GtkWidget *buffer_copy_length;
    GtkWidget *buffer_swap_start;
    GtkWidget *buffer_swap_length;
    GtkWidget *buffer_swap_word_size;
    GtkWidget *buffer_controls;
    GtkWidget *buffer_cancel_button;
    GtkWidget *load_image_path;
    GtkWidget *load_image_format;
    GtkWidget *load_image_size;
    GtkWidget *load_image_base;
    GtkWidget *load_image_target;
    GtkWidget *load_image_clear;
    GtkWidget *load_image_gap_policy;
    GtkWidget *load_image_gap_fill;
    GtkWidget *load_image_status;
    GtkWidget *load_image_controls;
    GtkWidget *load_image_apply;
    GtkWidget *load_image_cancel;
    GtkWidget *save_image_path;
    GtkWidget *save_image_format;
    GtkWidget *save_image_start;
    GtkWidget *save_image_length;
    GtkWidget *save_image_base;
    GtkWidget *save_image_byte_mode;
    GtkWidget *save_image_status;
    GtkWidget *save_image_controls;
    GtkWidget *save_image_apply;
    GtkWidget *save_image_cancel;
    GtkWindow *active_buffer_dialog;
    GtkWidget *report_path_entry;
    GtkWidget *report_status;
    GtkWidget *report_preview;
    GtkWidget *diagnostics_installation_entry;
    GtkWidget *diagnostics_run_button;
    GtkWidget *diagnostics_status;
    GtkWidget *trace_left_entry;
    GtkWidget *trace_right_entry;
    GtkWidget *trace_inspect_button;
    GtkWidget *trace_compare_button;
    GtkWidget *trace_status;
    GtkWidget *trace_preview;
    GtkWidget *component_path_entry;
    GtkWidget *component_object_entry;
    GtkWidget *component_base_object_entry;
    GtkWidget *component_combination_entry;
    GtkWidget *component_operation_entry;
    GtkWidget *component_id_entry;
    GtkWidget *component_version_entry;
    GtkWidget *component_author_entry;
    GtkWidget *component_license_entry;
    GtkWidget *component_date_entry;
    GtkWidget *component_revision_output_entry;
    GtkWidget *component_object_record_entry;
    GtkWidget *component_object_kind_dropdown;
    GtkWidget *component_payload_root_entry;
    GtkWidget *component_payload_confirmation_entry;
    GtkWidget *component_payload_execute_button;
    GtkWidget *component_payload_verify_button;
    GtkWidget *component_validate_all_button;
    GtkWidget *component_validate_button;
    GtkWidget *component_semantic_button;
    GtkWidget *component_compile_plan_button;
    GtkWidget *component_evidence_validate_button;
    GtkWidget *component_resolve_button;
    GtkWidget *component_diff_button;
    GtkWidget *component_adapter_test_button;
    GtkWidget *component_payload_plan_button;
    GtkWidget *component_compile_matrix_button;
    GtkWidget *component_cancel_button;
    GtkWidget *component_evidence_entry;
    GtkWidget *component_evidence_artifact_entry;
    GtkWidget *component_evidence_store_entry;
    GtkWidget *component_evidence_verify_button;
    GtkWidget *component_evidence_import_button;
    GtkWidget *component_evidence_audit_button;
    GtkWidget *component_release_check_button;
    GtkWidget *component_status;
    GtkWidget *component_preview;
    GtkWidget *jtag_kind_dropdown;
    GtkWidget *jtag_tdi_entry;
    GtkWidget *jtag_bits_entry;
    GtkWidget *jtag_tck_entry;
    GtkWidget *jtag_max_tck_entry;
    GtkWidget *jtag_end_dropdown;
    GtkWidget *jtag_tdo_entry;
    GtkWidget *jtag_expected_entry;
    GtkWidget *jtag_mask_entry;
    GtkWidget *jtag_chain_count_entry;
    GtkWidget *jtag_adapter_entry;
    GtkWidget *jtag_revision_entry;
    GtkWidget *jtag_contacts_entry;
    GtkWidget *jtag_power_dropdown;
    GtkWidget *jtag_vref_min_entry;
    GtkWidget *jtag_vref_max_entry;
    GtkWidget *jtag_level_dropdown;
    GtkWidget *jtag_contention_dropdown;
    GtkWidget *jtag_reset_dropdown;
    GtkWidget *jtag_disconnect_dropdown;
    GtkWidget *jtag_status;
    GtkWidget *jtag_preview;
    GtkWidget *footer_database;
    GtkWidget *footer_device;
    GtkWidget *footer_package;
    GtkWidget *footer_pin;
    GtkWidget *footer_capacity;
    GtkWidget *footer_voltage;
    GtkWidget *footer_vpp;
    GtkWidget *footer_checksum;
    GtkWidget *footer_manufacturer_code;
    GtkWidget *footer_device_code;
    GtkWidget *footer_note;
    GtkWidget *footer_file;
    GtkWidget *footer_programmer;
    GtkWidget *footer_current_count;
    GtkWidget *footer_target_count;
    GtkWidget *footer_current_failure;
    GtkWidget *footer_max_failure;
    GtkWidget *footer_statistics_toggle;
    GCancellable *read_cancellable;
    GCancellable *component_cancellable;
    GCancellable *buffer_cancellable;
    GCancellable *catalogue_cancellable;
    GCancellable *operation_prepare_cancellable;
    GCancellable *image_inspect_cancellable;
    guint active_background_tasks;
    guint close_wait_source_id;
    int close_pending;
    GMutex progress_mutex;
    lt48_programmer *active_programmer;
    guint progress_source_id;
    lt48_installation *installation;
    lt48_property_document *operation_option_document;
    size_t operation_option_device_index;
    GArray *device_result_indices;
    GArray *auto_id_candidate_evidence;
    lt48_buffer *device_buffer;
    lt48_buffer *secondary_buffer;
    lt48_test_vectors *test_vectors;
    GPtrArray *fuse_documents;
    unsigned char *buffer_edit_undo_data;
    size_t buffer_edit_undo_size;
    uint32_t buffer_edit_undo_offset;
    int buffer_edit_has_undo;
    int buffer_edit_secondary;
    int secondary_buffer_kind;
    GPtrArray *database_paths;
    int database_selector_refreshing;
    int database_manager_return_to_selection;
    char database_manifest_hex[LT48_SHA256_SIZE * 2u + 1u];
    char expected_project_manifest_hex[LT48_SHA256_SIZE * 2u + 1u];
    char *pending_project_query;
    char *pending_project_device_vendor;
    char *pending_project_device_name;
    GHashTable *pending_project_option_values;
    char *pending_project_option_vendor;
    char *pending_project_option_device;
    int database_manifest_verified;
    int auto_browse_after_audit;
    uint64_t buffer_revision;
    uint64_t secondary_buffer_revision;
    uint64_t operation_input_revision;
    size_t selected_device_index;
    int has_selected_device;
    size_t pending_device_index;
    int has_pending_device;
    uint8_t selected_programmer_bus;
    uint8_t selected_programmer_address;
    uint16_t selected_programmer_product_id;
    int has_selected_programmer;
    int programmer_ready;
    int programmer_initializing;
    int operation_model_refreshing;
    int automatic_fx2_attempted;
    int read_after_prepare;
    int pending_hardware_action;
    int read_overwrite_authorized;
    uint64_t statistic_current_count;
    uint64_t statistic_current_failure;
    uint32_t statistic_target_count;
    uint32_t statistic_max_failure;
    int statistic_enabled;
    int statistic_alarm_enabled;
    operation_options_snapshot options_snapshot;
    struct operation_prepare_task *prepared_operation;
} app_widgets;

typedef struct vector_editor {
    app_widgets *owner;
    GtkWindow *window;
    GtkWidget *number;
    GtkWidget *row;
    GtkWidget *total;
    GtkWidget *clipboard;
    GtkWidget *current_pin;
    GtkWidget *maximum_pin;
    GtkWidget *status;
    GtkWidget *undo;
    GtkWidget *cut;
    int refreshing;
} vector_editor;

typedef struct fuse_session_document {
    size_t device_index;
    uint16_t code_id;
    size_t fuse_count;
    unsigned char *bits;
    unsigned char *undo_bits;
    int has_undo;
} fuse_session_document;

typedef struct fuse_editor {
    app_widgets *owner;
    fuse_session_document *document;
    GtkWindow *window;
    lt48_rez *rez;
    lt48_fuse_code code;
    lt48_fuse_row *rows;
    size_t row_count;
    GtkWidget *radix;
    GtkWidget *status;
    GtkWidget *undo;
    GtkAdjustment *row_adjustment;
    GPtrArray *addresses;
    GPtrArray *entries;
    size_t first_row;
    int refreshing;
} fuse_editor;

typedef struct fuse_fill_dialog {
    fuse_editor *editor;
    GtkWindow *window;
    GtkWidget *from;
    GtkWidget *to;
    GtkWidget *intact;
} fuse_fill_dialog;

typedef enum fuse_context_action {
    FUSE_CONTEXT_EDIT = 0,
    FUSE_CONTEXT_LOAD_JEDEC = 1,
    FUSE_CONTEXT_SAVE_JEDEC = 2
} fuse_context_action;

typedef enum fuse_binding_kind {
    FUSE_BINDING_PRIMARY = 0,
    FUSE_BINDING_E_FIELD = 1,
    FUSE_BINDING_USER_CODE = 2
} fuse_binding_kind;

typedef struct fuse_context_chooser {
    app_widgets *owner;
    GtkWindow *window;
    fuse_context_action action;
    GFile *file;
} fuse_context_chooser;

static gboolean selected_fuse_editor_available(app_widgets *widgets);
static gboolean selected_auxiliary_fuse_editor_available(
    app_widgets *widgets, fuse_binding_kind kind);
static void open_fuse_editor(app_widgets *widgets);
static void open_fuse_editor_context(app_widgets *widgets, size_t ordinal);
static void open_auxiliary_fuse_editor(
    app_widgets *widgets, fuse_binding_kind kind);
static void open_fuse_context_action(
    app_widgets *widgets, fuse_context_action action, GFile *file);
static void fuse_session_reset(app_widgets *widgets);

enum {
    PAGE_START = 0,
    PAGE_DEVICES = 1,
    PAGE_BUFFER = 2,
    PAGE_REPORTS = 3,
    PAGE_TRACES = 4,
    PAGE_DIAGNOSTICS = 5,
    PAGE_HARDWARE_DEBUG = 6,
    PAGE_COMPONENTS = 7,
    PAGE_JTAG = 8
};

enum {
    PENDING_HARDWARE_NONE = 0,
    PENDING_HARDWARE_PREPARE_READ = 1,
    PENDING_HARDWARE_EXECUTE = 2,
    PENDING_HARDWARE_AUTO_ID = 3
};

typedef struct cache_import_task {
    char *source;
    char *runtime_source;
    char *destination;
} cache_import_task;

typedef struct cache_import_result {
    lt48_status status;
    lt48_private_cache_report report;
    char *source;
    char *runtime_source;
    char *destination;
} cache_import_result;

typedef enum catalogue_task_kind {
    CATALOGUE_TASK_SEARCH = 1,
    CATALOGUE_TASK_RANK = 2,
    CATALOGUE_TASK_AUDIT = 3
} catalogue_task_kind;

typedef struct catalogue_task {
    catalogue_task_kind kind;
    char *path;
    char *query;
    guint type_filter;
    lt48_auto_id_observation observation;
} catalogue_task;

typedef struct catalogue_task_result {
    catalogue_task_kind kind;
    lt48_status status;
    lt48_installation *installation;
    GArray *matching_indices;
    lt48_auto_id_candidate *candidates;
    size_t candidate_count;
    lt48_installation_audit audit;
} catalogue_task_result;

typedef struct eprom_auto_id_task {
    char *installation_path;
    uint8_t bus;
    uint8_t address;
    unsigned timeout_ms;
} eprom_auto_id_task;

typedef struct eprom_auto_id_result {
    lt48_status status;
    lt48_auto_id_eprom_execution_result execution;
    lt48_installation *installation;
    lt48_auto_id_candidate *candidates;
    size_t candidate_count;
} eprom_auto_id_result;

typedef struct auto_id_candidate_evidence {
    size_t device_index;
    uint32_t score;
    int manufacturer_id_match;
    int device_id_match;
    int pin_count_match;
    int device_class_match;
    int has_resolved_pin_count;
    uint8_t resolved_pin_count;
    size_t rank;
} auto_id_candidate_evidence;

typedef struct fx2_load_task {
    char *firmware_path;
    char *confirmation;
    uint8_t bus;
    uint8_t address;
} fx2_load_task;

typedef struct fx2_load_task_result {
    lt48_status status;
    lt48_fx2_load_preparation preparation;
    lt48_fx2_load_result load;
} fx2_load_task_result;

typedef struct programmer_init_task {
    uint8_t bus;
    uint8_t address;
    uint32_t timeout_ms;
} programmer_init_task;

typedef struct programmer_init_result {
    lt48_status status;
    lt48_transport_info transport;
} programmer_init_result;

typedef struct prepared_read {
    lt48_m37_resources *resources;
    lt48_protocol_plan *plan;
    lt48_m37_read_preparation preparation;
    char *selector;
    char *output;
    char *report;
    char *installation_path;
    size_t device_index;
    uint32_t capacity;
    uint32_t start;
    uint32_t length;
    uint32_t timeout;
    uint8_t bus;
    uint8_t address;
    int overwrite_output;
} prepared_read;

typedef struct read_task {
    prepared_read request;
    app_widgets *widgets;
} read_task;

typedef struct read_task_result {
    lt48_status status;
    char *message;
    lt48_buffer *buffer;
    uint32_t device_capacity;
    uint32_t device_start;
    size_t size;
} read_task_result;

typedef struct read_prepare_task {
    prepared_read request;
    char *plan_report;
    uint64_t input_revision;
} read_prepare_task;

typedef struct read_prepare_result {
    lt48_status status;
    char *message;
    char confirmation[LT48_SHA256_SIZE * 2u + 1u];
    uint64_t input_revision;
} read_prepare_result;

typedef struct operation_prepare_task {
    lt48_operation_kind operation;
    char *installation_path;
    size_t device_index;
    uint32_t start;
    uint32_t length;
    unsigned char *image;
    size_t image_size;
    char *image_path;
    char *image_source;
    lt48_image_format image_format;
    uint64_t image_address_base;
    lt48_image_gap_policy image_gap_policy;
    unsigned char image_gap_fill;
    char *report_path;
    guint margin_choice;
    guint margin_type;
    uint64_t input_revision;
    uint64_t buffer_revision;
    int uses_buffer;
    int selected_device_override;
} operation_prepare_task;

typedef struct operation_execute_task {
    operation_prepare_task *request;
    app_widgets *widgets;
    uint8_t bus;
    uint8_t address;
    uint32_t timeout;
} operation_execute_task;

typedef struct operation_execute_result {
    lt48_status status;
    char *message;
} operation_execute_result;

typedef struct operation_prepare_result {
    lt48_operation_kind operation;
    lt48_status status;
    char *message;
    uint64_t input_revision;
    uint64_t buffer_revision;
    int uses_buffer;
} operation_prepare_result;

typedef struct image_inspect_task {
    char *path;
    lt48_image_import_options options;
    uint32_t selected_start;
    uint32_t selected_length;
    uint64_t input_revision;
} image_inspect_task;

typedef struct image_inspect_result {
    lt48_status status;
    char *message;
    uint64_t input_revision;
    int selected_range_known;
} image_inspect_result;

typedef struct diagnostics_task {
    char *installation_path;
} diagnostics_task;

typedef struct diagnostics_task_result {
    lt48_status status;
    lt48_diagnostics_report report;
    char *installation_path;
} diagnostics_task_result;

typedef struct payload_build_task {
    char *bundle_path;
    char *workspace_root;
    unsigned char confirmation[LT48_SHA256_SIZE];
} payload_build_task;

typedef struct payload_build_task_result {
    lt48_status status;
    lt48_component_payload_build_execution execution;
} payload_build_task_result;

typedef struct payload_verify_task {
    char *bundle_path;
    char *workspace_root;
} payload_verify_task;

typedef struct payload_verify_task_result {
    lt48_status status;
    lt48_component_payload_input_verification verification;
} payload_verify_task_result;

typedef enum buffer_io_kind {
    BUFFER_IO_LOAD = 1,
    BUFFER_IO_SAVE = 2,
    BUFFER_IO_COMPARE = 3,
    BUFFER_IO_LOAD_IMAGE = 4
} buffer_io_kind;

typedef struct buffer_io_task {
    buffer_io_kind kind;
    char *path;
    lt48_buffer_import_options options;
    lt48_image_import_options image_options;
    lt48_image_export_options export_options;
    lt48_buffer *snapshot;
    uint32_t start;
    uint32_t length;
    uint32_t target_offset;
    guint load_clear_mode;
    uint64_t source_revision;
    int extract;
} buffer_io_task;

typedef struct buffer_io_result {
    buffer_io_kind kind;
    lt48_status status;
    char *path;
    lt48_buffer_import_options options;
    lt48_buffer_import_result import_result;
    lt48_image_import_options image_options;
    lt48_image_import_result image_import_result;
    lt48_image_export_options export_options;
    lt48_image_export_result export_result;
    lt48_buffer *loaded;
    lt48_buffer_compare_result comparison;
    uint32_t start;
    uint32_t length;
    uint32_t target_offset;
    guint load_clear_mode;
    uint64_t source_revision;
    int extract;
} buffer_io_result;

typedef enum component_long_task_kind {
    COMPONENT_LONG_VALIDATE_ALL = 1,
    COMPONENT_LONG_COMPILE_MATRIX = 2,
    COMPONENT_LONG_AUDIT_EVIDENCE = 3,
    COMPONENT_LONG_RELEASE_CHECK = 4,
    COMPONENT_LONG_VERIFY_EVIDENCE = 5,
    COMPONENT_LONG_IMPORT_EVIDENCE = 6,
    COMPONENT_LONG_VALIDATE_STRUCTURE = 7,
    COMPONENT_LONG_CHECK_SEMANTICS = 8,
    COMPONENT_LONG_COMPILE_PLAN = 9,
    COMPONENT_LONG_VALIDATE_EVIDENCE = 10,
    COMPONENT_LONG_RESOLVE = 11,
    COMPONENT_LONG_DIFF = 12,
    COMPONENT_LONG_ADAPTER_TEST = 13,
    COMPONENT_LONG_PAYLOAD_PLAN = 14
} component_long_task_kind;

typedef struct component_long_task {
    char *bundle_path;
    char *store_path;
    char *evidence_path;
    char *artifact_path;
    char *operation_id;
    char *object_id;
    char *base_object_id;
    size_t combination_index;
    component_long_task_kind kind;
} component_long_task;

typedef struct component_long_task_result {
    lt48_status status;
    component_long_task_kind kind;
    lt48_component_all_variants_report validation;
    lt48_component_plan_matrix matrix;
    lt48_component_evidence_audit audit;
    lt48_component_release_check release_check;
    lt48_component_evidence_verification verification;
    lt48_component_evidence_import_report import_report;
    lt48_component_validation_report structure;
    lt48_component_semantic_report semantics;
    lt48_component_plan plan;
    lt48_component_evidence_report evidence;
    lt48_component_resolution resolution;
    lt48_component_adapter_test_plan adapter_plan;
    lt48_component_payload_build_plan payload_plan;
} component_long_task_result;

typedef struct trace_task {
    char *left_path;
    char *right_path;
    int compare;
} trace_task;

typedef struct trace_task_result {
    lt48_status status;
    int comparison_difference;
    char *summary;
    char *preview;
} trace_task_result;

static const lt48_operation_kind gtk_operation_kinds[] = {
    LT48_OPERATION_READ,
    LT48_OPERATION_BLANK_CHECK,
    LT48_OPERATION_PROGRAM,
    LT48_OPERATION_OVER_PROGRAM,
    LT48_OPERATION_VERIFY,
    LT48_OPERATION_COMPARE,
    LT48_OPERATION_ERASE,
    LT48_OPERATION_SECURE,
    LT48_OPERATION_MEMORY_PROTECT,
    LT48_OPERATION_CONFIGURATION,
    LT48_OPERATION_FUNCTIONAL_TEST,
    LT48_OPERATION_AUTO_ID,
    LT48_OPERATION_MASS_PRODUCTION,
    LT48_OPERATION_JTAG_ISP
};

static const char *const gtk_operation_labels[] = {
    "Read", "Blank check", "Program", "Over-program", "Verify",
    "Compare", "Erase", "Secure", "Memory protect", "Configuration",
    "Functional test", "Auto-ID", "Mass production", "JTAG/ISP"
};

static const char *const gtk_operation_action_names[] = {
    "read", "blank", "program", "over-program", "verify", "compare", "erase",
    "secure", "memory-protect", "config", "function-test", NULL, NULL, NULL
};

G_STATIC_ASSERT(G_N_ELEMENTS(gtk_operation_kinds) ==
                G_N_ELEMENTS(gtk_operation_labels));
G_STATIC_ASSERT(G_N_ELEMENTS(gtk_operation_kinds) ==
                G_N_ELEMENTS(gtk_operation_action_names));

static const char *const component_object_kinds[] = {
    "algorithm", "family", "variant", "package", "adapter",
    "payload", "evidence", "variable", "pin-map", "operation", NULL
};

static const char *operation_availability_label(
    lt48_operation_availability availability
);
static int gio_cancelled(void *context);
static void execute_read(GtkButton *button, gpointer user_data);
static void prepare_read(GtkButton *button, gpointer user_data);
static void execute_prepared_operation(app_widgets *widgets);
static void plan_eprom_auto_id(GtkButton *button, gpointer user_data);
static void fx2_start_automatic(app_widgets *widgets);
static void refresh_programmers(GtkButton *button, gpointer user_data);
static void operation_progress_present(
    app_widgets *widgets, const char *operation, const char *status);
static void operation_progress_finish(app_widgets *widgets);
static void programmer_initialize(GtkButton *button, gpointer user_data);
static void inspect_programming_image(
    GtkButton *button, gpointer user_data
);
static void browse_programming_image(
    GtkButton *button, gpointer user_data
);
static void operation_prepare_task_free(gpointer data);
static void refresh_programming_parameters(app_widgets *widgets);
static void hide_auxiliary_window(GtkButton *button, gpointer user_data);
static void present_database_manager(GtkButton *button, gpointer user_data);
static gboolean database_registry_contains(
    const app_widgets *widgets, const char *path
);
static void database_registry_save(app_widgets *widgets);
static void database_manager_rebuild(app_widgets *widgets);
static void buffer_edit_discard_undo(app_widgets *widgets);
static void search_devices(GtkButton *button, gpointer user_data);
static void verify_installation(GtkButton *button, gpointer user_data);
static void project_status(app_widgets *widgets, const char *message);
static void commit_device_row(
    GtkListBox *box, GtkListBoxRow *row, gpointer user_data
);
static void restore_pending_project_operation_options(
    app_widgets *widgets, const char *vendor, const char *device
);

static void clear_pending_project_operation_options(app_widgets *widgets) {
    g_clear_pointer(&widgets->pending_project_option_values,
                    g_hash_table_unref);
    g_clear_pointer(&widgets->pending_project_option_vendor, g_free);
    g_clear_pointer(&widgets->pending_project_option_device, g_free);
}

static void clear_pending_project_device(app_widgets *widgets) {
    g_clear_pointer(&widgets->pending_project_device_vendor, g_free);
    g_clear_pointer(&widgets->pending_project_device_name, g_free);
}

static void tracked_task_finalized(gpointer user_data) {
    app_widgets *widgets = user_data;
    g_assert(widgets->active_background_tasks > 0u);
    --widgets->active_background_tasks;
}

static GTask *tracked_task_new(
    app_widgets *widgets, GCancellable *cancellable,
    GAsyncReadyCallback callback
) {
    GTask *task = g_task_new(
        widgets->window, cancellable, callback, widgets);
    ++widgets->active_background_tasks;
    g_object_set_data_full(
        G_OBJECT(task), "lt48-tracked-background-task", widgets,
        tracked_task_finalized);
    return task;
}

static lt48_operation_kind selected_operation_kind(app_widgets *widgets) {
    guint selected = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->operation_dropdown)
    );
    return selected < G_N_ELEMENTS(gtk_operation_kinds) ?
        gtk_operation_kinds[selected] : (lt48_operation_kind)0;
}

static void update_operation_controls(app_widgets *widgets) {
    lt48_operation_kind operation = selected_operation_kind(widgets);
    gboolean is_read = operation == LT48_OPERATION_READ;
    gboolean is_verify = operation == LT48_OPERATION_VERIFY;
    gboolean is_program = operation == LT48_OPERATION_PROGRAM ||
                          operation == LT48_OPERATION_OVER_PROGRAM;
    gboolean has_image = is_verify || operation == LT48_OPERATION_PROGRAM ||
                         operation == LT48_OPERATION_OVER_PROGRAM ||
                         operation == LT48_OPERATION_COMPARE;
    gboolean use_buffer = widgets->verify_use_buffer_check != NULL &&
        gtk_check_button_get_active(
            GTK_CHECK_BUTTON(widgets->verify_use_buffer_check));
    gboolean has_file_image = has_image && !use_buffer;

    gtk_widget_set_sensitive(widgets->read_output_entry, is_read);
    gtk_widget_set_sensitive(widgets->read_bus_entry, is_read);
    gtk_widget_set_sensitive(widgets->read_address_entry, is_read);
    gtk_widget_set_sensitive(widgets->read_timeout_entry, TRUE);
    gtk_widget_set_sensitive(widgets->execution_report_entry, TRUE);
    gtk_widget_set_sensitive(widgets->read_confirmation_entry, is_read);
    gtk_widget_set_sensitive(widgets->verify_image_entry, has_file_image);
    gtk_widget_set_sensitive(widgets->verify_image_browse_button,
                             has_file_image);
    gtk_widget_set_sensitive(widgets->image_inspect_button,
                             has_file_image &&
                             widgets->image_inspect_cancellable == NULL);
    gtk_widget_set_sensitive(widgets->image_format_dropdown, has_file_image);
    gtk_widget_set_sensitive(widgets->image_base_entry, has_file_image);
    gtk_widget_set_sensitive(widgets->image_gap_dropdown, has_file_image);
    gtk_widget_set_sensitive(widgets->image_gap_fill_entry, has_file_image);
    gtk_widget_set_sensitive(widgets->verify_use_buffer_check, has_image);
    gtk_widget_set_sensitive(widgets->verify_margin_dropdown, is_verify);
    gtk_widget_set_sensitive(widgets->verify_margin_type_dropdown, is_verify);
    gtk_widget_set_sensitive(
        widgets->program_identity_override_check, is_program);
}

static void invalidate_operation_preparation(
    app_widgets *widgets, int show_message
) {
    if (widgets->operation_input_revision != UINT64_MAX)
        ++widgets->operation_input_revision;
    operation_prepare_task_free(widgets->prepared_operation);
    widgets->prepared_operation = NULL;
    if (widgets->read_execute_button != NULL)
        gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    if (widgets->read_confirmation_entry != NULL)
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->read_confirmation_entry), "");
    if (widgets->operation_prepare_cancellable != NULL) {
        g_cancellable_cancel(widgets->operation_prepare_cancellable);
        if (widgets->operation_prepare_cancel_button != NULL)
            gtk_widget_set_sensitive(
                widgets->operation_prepare_cancel_button, FALSE);
    }
    if (widgets->image_inspect_cancellable != NULL)
        g_cancellable_cancel(widgets->image_inspect_cancellable);
    if (widgets->image_status != NULL)
        gtk_label_set_text(
            GTK_LABEL(widgets->image_status),
            "Image mapping changed; inspect the current image again");
    if (show_message && widgets->read_preparation != NULL &&
        widgets->read_cancellable == NULL)
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Operation settings changed; the next action will use the current values");
}

static void operation_input_changed(
    GObject *object, gpointer user_data
) {
    (void)object;
    invalidate_operation_preparation(user_data, 1);
    update_operation_controls(user_data);
}

static void operation_input_notify(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    (void)object;
    (void)parameter;
    invalidate_operation_preparation(user_data, 1);
    update_operation_controls(user_data);
}

static void operation_selection_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    app_widgets *widgets = user_data;
    lt48_operation_support support;
    lt48_operation_kind operation = selected_operation_kind(widgets);
    lt48_status status;
    char *message;
    char *definition_evidence;
    (void)object;
    (void)parameter;
    if (widgets->operation_model_refreshing) return;
    invalidate_operation_preparation(widgets, 0);
    update_operation_controls(widgets);
    if (widgets->installation == NULL || !widgets->has_selected_device ||
        lt48_operation_kind_name(operation) == NULL) {
        gtk_button_set_label(
            GTK_BUTTON(widgets->operation_prepare_button),
            "Select a device");
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Select a device, then choose the required operation");
        return;
    }
    status = lt48_installation_operation_support_one(
        widgets->installation, widgets->selected_device_index,
        operation, &support
    );
    if (status != LT48_OK) {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           lt48_status_string(status));
        return;
    }
    if (support.availability == LT48_OPERATION_EXECUTABLE) {
        const char *action = operation == LT48_OPERATION_READ ? "Read device" :
            operation == LT48_OPERATION_BLANK_CHECK ? "Blank check" :
            operation == LT48_OPERATION_PROGRAM ? "Program device" :
            operation == LT48_OPERATION_OVER_PROGRAM ? "Over-program device" :
            operation == LT48_OPERATION_VERIFY ? "Verify device" :
            operation == LT48_OPERATION_COMPARE ? "Compare device" :
            "Run operation";
        gtk_button_set_label(
            GTK_BUTTON(widgets->operation_prepare_button), action);
    }
    else if (support.availability == LT48_OPERATION_PLAN_ONLY)
        gtk_button_set_label(
            GTK_BUTTON(widgets->operation_prepare_button),
            "Prepare offline plan");
    else if (support.availability == LT48_OPERATION_HOST_ONLY)
        gtk_button_set_label(
            GTK_BUTTON(widgets->operation_prepare_button),
            "Show host action");
    else
        gtk_button_set_label(
            GTK_BUTTON(widgets->operation_prepare_button), "Show blocker");
    definition_evidence = support.definition_callback_present ?
        g_strdup_printf(
            "Definition evidence: %s → W%x. This identifies the original "
            "resource only; it does not enable hardware execution.",
            support.definition_callback, support.definition_wave
        ) :
        g_strdup(
            "No direct definition callback has been associated with this "
            "public operation."
        );
    message = g_strdup_printf(
        "Selected %s — %s (%s): %s\n%s\n%s",
        support.name, operation_availability_label(support.availability),
        support.support_level, support.reason,
        support.availability == LT48_OPERATION_EXECUTABLE ?
            "The toolbar action is available; execution applies the current "
            "safety and confirmation gates." :
        support.availability == LT48_OPERATION_PLAN_ONLY ?
            "An offline plan can be inspected, but live execution is disabled." :
        support.availability == LT48_OPERATION_HOST_ONLY ?
            "This is a host-only action and does not operate the programmer." :
            "No native plan or execution is available for this definition.",
        definition_evidence
    );
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
    g_free(definition_evidence);
    g_free(message);
}

enum {
    SECONDARY_BUFFER_NONE = 0,
    SECONDARY_BUFFER_DATA = 1,
    SECONDARY_BUFFER_ENCRYPTION = 2
};

static int selected_secondary_buffer_kind(
    app_widgets *widgets, const lt48_device_spec *resolved_spec
) {
    lt48_device_spec local_spec;
    const lt48_device_spec *spec = resolved_spec;
    lt48_device_operation_binding *bindings = NULL;
    size_t binding_count = 0u;
    int kind = SECONDARY_BUFFER_DATA;
    if (widgets->installation == NULL || !widgets->has_selected_device)
        return SECONDARY_BUFFER_NONE;
    if (spec == NULL) {
        if (lt48_installation_resolve_device(
                widgets->installation, widgets->selected_device_index,
                &local_spec) != LT48_OK)
            return SECONDARY_BUFFER_NONE;
        spec = &local_spec;
    }
    if (spec->definition_kind !=
            LT48_DEVICE_DEFINITION_DOUBLE_BUFFER_MEMORY ||
        spec->secondary_element_count == 0u ||
        spec->secondary_element_bits == 0u)
        return SECONDARY_BUFFER_NONE;
    if (lt48_installation_device_operations(
            widgets->installation, widgets->selected_device_index,
            NULL, 0u, &binding_count) != LT48_BUFFER_TOO_SMALL)
        return SECONDARY_BUFFER_NONE;
    bindings = g_new0(lt48_device_operation_binding, binding_count);
    if (lt48_installation_device_operations(
            widgets->installation, widgets->selected_device_index,
            bindings, binding_count, &binding_count) != LT48_OK) {
        g_free(bindings);
        return SECONDARY_BUFFER_NONE;
    }
    for (size_t index = 0u; index < binding_count; ++index)
        if (bindings[index].has_symbol &&
            strcmp(bindings[index].symbol, "_FUN_ProgEncrypt") == 0) {
            kind = SECONDARY_BUFFER_ENCRYPTION;
            break;
        }
    g_free(bindings);
    return kind;
}

static void refresh_operation_dropdown(app_widgets *widgets) {
    lt48_operation_support support[LT48_OPERATION_KIND_COUNT];
    size_t support_count = 0u;
    guint selected;
    GtkStringList *model;
    lt48_status status = LT48_NOT_FOUND;
    gboolean mass_production_available = FALSE;
    gboolean configuration_editor_available = FALSE;
    gboolean vector_editor_available = FALSE;
    gboolean fuse_editor_available = FALSE;
    gboolean e_field_editor_available = FALSE;
    gboolean user_code_editor_available = FALSE;
    gboolean data_buffer_editor_available = FALSE;
    gboolean encryption_editor_available = FALSE;
    gboolean secure_available = FALSE;
    gboolean memory_protect_available = FALSE;
    if (widgets->operation_dropdown == NULL) return;
    selected = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->operation_dropdown));
    if (widgets->installation != NULL && widgets->has_selected_device)
        status = lt48_installation_operation_support(
            widgets->installation, widgets->selected_device_index,
            support, G_N_ELEMENTS(support), &support_count);
    if (widgets->installation != NULL && widgets->has_selected_device) {
        lt48_property_document *document = NULL;
        lt48_device_spec spec;
        configuration_editor_available =
            lt48_installation_open_device_property_document(
                widgets->installation, widgets->selected_device_index,
                LT48_PROPERTY_SHEET_CONFIGURATION, &document) == LT48_OK;
        lt48_property_document_close(document);
        vector_editor_available = lt48_installation_resolve_device(
            widgets->installation, widgets->selected_device_index,
            &spec) == LT48_OK && spec.catalogue.device_class >= 2019u &&
            spec.catalogue.device_class <= 2028u && spec.pin_count != 0u;
        fuse_editor_available = selected_fuse_editor_available(widgets);
        e_field_editor_available = selected_auxiliary_fuse_editor_available(
            widgets, FUSE_BINDING_E_FIELD);
        user_code_editor_available = selected_auxiliary_fuse_editor_available(
            widgets, FUSE_BINDING_USER_CODE);
        data_buffer_editor_available = widgets->secondary_buffer != NULL &&
            widgets->secondary_buffer_kind == SECONDARY_BUFFER_DATA;
        encryption_editor_available = widgets->secondary_buffer != NULL &&
            widgets->secondary_buffer_kind == SECONDARY_BUFFER_ENCRYPTION;
    }
    model = gtk_string_list_new(NULL);
    for (size_t index = 0u; index < G_N_ELEMENTS(gtk_operation_kinds);
         ++index) {
        char *label;
        gboolean enabled;
        if (status == LT48_OK && index < support_count &&
            support[index].operation == gtk_operation_kinds[index])
            label = g_strdup_printf(
                "%s — %s", gtk_operation_labels[index],
                operation_availability_label(support[index].availability));
        else
            label = g_strdup_printf(
                "%s — select a device", gtk_operation_labels[index]);
        gtk_string_list_append(model, label);
        g_free(label);
        enabled = status == LT48_OK && index < support_count &&
            support[index].operation == gtk_operation_kinds[index] &&
            support[index].availability == LT48_OPERATION_EXECUTABLE;
        if (gtk_operation_kinds[index] == LT48_OPERATION_CONFIGURATION)
            enabled = configuration_editor_available;
        if (gtk_operation_kinds[index] == LT48_OPERATION_PROGRAM)
            mass_production_available = enabled;
        if (gtk_operation_kinds[index] == LT48_OPERATION_SECURE)
            secure_available = enabled;
        if (gtk_operation_kinds[index] == LT48_OPERATION_MEMORY_PROTECT)
            memory_protect_available = enabled;
        if (widgets->quick_operation_buttons[index] != NULL)
        {
            char *tooltip;
            gtk_widget_set_sensitive(
                widgets->quick_operation_buttons[index], enabled);
            if (gtk_operation_kinds[index] == LT48_OPERATION_CONFIGURATION &&
                configuration_editor_available)
                tooltip = g_strdup(
                    "Edit the exact Configuration set selected by this "
                    "definition in the active database.");
            else if (status == LT48_OK && index < support_count &&
                support[index].operation == gtk_operation_kinds[index])
                tooltip = g_strdup_printf(
                    "%s for the selected device in the active database. %s",
                    enabled ? "Available" : "Unavailable",
                    support[index].reason);
            else
                tooltip = g_strdup(
                    "Select a device from the active database first.");
            gtk_widget_set_tooltip_text(
                widgets->quick_operation_buttons[index], tooltip);
            g_free(tooltip);
        }
        if (widgets->window != NULL &&
            gtk_operation_action_names[index] != NULL) {
            GAction *action = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window),
                gtk_operation_action_names[index]);
            if (action != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(action), enabled);
        }
    }
    if (widgets->window != NULL) {
        GAction *data_buffer = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "edit-data-buffer");
        GAction *encryption = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "edit-encryption-array");
        if (data_buffer != NULL)
            g_simple_action_set_enabled(
                G_SIMPLE_ACTION(data_buffer), data_buffer_editor_available);
        if (encryption != NULL)
            g_simple_action_set_enabled(
                G_SIMPLE_ACTION(encryption), encryption_editor_available);
    }
    if (widgets->quick_operation_buttons[8] != NULL) {
        gboolean protect_available = secure_available ||
            memory_protect_available;
        gtk_widget_set_sensitive(
            widgets->quick_operation_buttons[8], protect_available);
        gtk_widget_set_tooltip_text(
            widgets->quick_operation_buttons[8],
            memory_protect_available ?
                "Run the selected definition's Memory Protect/Program "
                "Configuration operation." :
            secure_available ?
                "Run the selected definition's Secure operation." :
                "The selected definition has no executable protection operation.");
    }
    if (widgets->toolbar_option_button != NULL)
        gtk_widget_set_sensitive(
            widgets->toolbar_option_button,
            widgets->installation != NULL && widgets->has_selected_device);
    if (widgets->toolbar_save_button != NULL) {
        gboolean save_available = widgets->device_buffer != NULL ||
            fuse_editor_available;
        GAction *save_action;
        gtk_widget_set_sensitive(
            widgets->toolbar_save_button, save_available);
        gtk_widget_set_tooltip_text(
            widgets->toolbar_save_button,
            fuse_editor_available ?
                "Save the selected logic fuse context as JEDEC." :
            widgets->device_buffer != NULL ?
                "Save the resident device buffer." :
                "Load a buffer or select a logic definition first.");
        save_action = widgets->window == NULL ? NULL :
            g_action_map_lookup_action(G_ACTION_MAP(widgets->window), "save");
        if (save_action != NULL)
            g_simple_action_set_enabled(
                G_SIMPLE_ACTION(save_action), save_available);
    }
    if (widgets->toolbar_edit_button != NULL) {
        gboolean editable = widgets->device_buffer != NULL ||
            fuse_editor_available;
        gtk_widget_set_sensitive(widgets->toolbar_edit_button, editable);
        gtk_widget_set_tooltip_text(
            widgets->toolbar_edit_button,
            fuse_editor_available ?
                "Edit the exact fuse map for the selected logic definition." :
            widgets->device_buffer != NULL ?
                "Edit the resident device buffer." :
                "Load a buffer or select a logic definition with a decoded fuse map.");
        if (widgets->window != NULL) {
            GAction *action = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), "edit");
            if (action != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(action), editable);
        }
    }
    if (widgets->toolbar_vector_button != NULL) {
        gtk_widget_set_sensitive(
            widgets->toolbar_vector_button, vector_editor_available);
        gtk_widget_set_tooltip_text(
            widgets->toolbar_vector_button,
            vector_editor_available ?
                "Edit the selected logic device's function-test vectors." :
                "The selected definition does not own the recovered logic-vector editor.");
    }
    if (widgets->window != NULL) {
        static const char *const setting_actions[] = {
            "parameters", "operation"
        };
        GAction *project_save;
        for (size_t index = 0u; index < G_N_ELEMENTS(setting_actions);
             ++index) {
            GAction *action = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), setting_actions[index]);
            if (action != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(action),
                    widgets->installation != NULL &&
                    widgets->has_selected_device);
        }
        project_save = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "project-save");
        if (project_save != NULL)
            g_simple_action_set_enabled(
                G_SIMPLE_ACTION(project_save),
                widgets->installation != NULL &&
                widgets->has_selected_device &&
                widgets->database_manifest_verified);
        {
            GAction *vector = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), "vector");
            if (vector != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(vector), vector_editor_available);
        }
        {
            GAction *e_field = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), "edit-e-field");
            GAction *user_code = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), "edit-user-code");
            if (e_field != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(e_field), e_field_editor_available);
            if (user_code != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(user_code), user_code_editor_available);
        }
        {
            GAction *mass_production = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), "mass-production");
            if (mass_production != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(mass_production),
                    mass_production_available);
        }
        {
            GAction *protect = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), "protect");
            if (protect != NULL)
                g_simple_action_set_enabled(
                    G_SIMPLE_ACTION(protect),
                    secure_available || memory_protect_available);
        }
    }
    widgets->operation_model_refreshing = 1;
    gtk_drop_down_set_model(
        GTK_DROP_DOWN(widgets->operation_dropdown), G_LIST_MODEL(model));
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(widgets->operation_dropdown),
        selected < G_N_ELEMENTS(gtk_operation_kinds) ? selected : 0u);
    widgets->operation_model_refreshing = 0;
    g_object_unref(model);
    operation_selection_changed(NULL, NULL, widgets);
}

static void start_change_device_browse(app_widgets *widgets) {
    if (widgets->vendor_list == NULL ||
        gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(widgets->vendor_list), 0) != NULL ||
        widgets->catalogue_cancellable != NULL)
        return;
    if (!widgets->database_manifest_verified) {
        widgets->auto_browse_after_audit = 1;
        verify_installation(NULL, widgets);
    } else {
        search_devices(NULL, widgets);
    }
}

static void quick_select_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    if (widgets->main_notebook != NULL)
        gtk_notebook_set_current_page(
            GTK_NOTEBOOK(widgets->main_notebook), PAGE_DEVICES);
    if (widgets->device_select_window != NULL)
        gtk_window_present(widgets->device_select_window);
    start_change_device_browse(widgets);
}

static void quick_auto_id_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    if (widgets->auto_id_window != NULL)
        gtk_window_present(widgets->auto_id_window);
    plan_eprom_auto_id(button, widgets);
}

static void operation_options_snapshot_clear(app_widgets *widgets) {
    operation_options_snapshot *snapshot = &widgets->options_snapshot;
    g_clear_pointer(&snapshot->start, g_free);
    g_clear_pointer(&snapshot->length, g_free);
    g_clear_pointer(&snapshot->output, g_free);
    g_clear_pointer(&snapshot->timeout, g_free);
    g_clear_pointer(&snapshot->image, g_free);
    g_clear_pointer(&snapshot->image_base, g_free);
    g_clear_pointer(&snapshot->image_gap_fill, g_free);
    g_clear_pointer(&snapshot->plan_report, g_free);
    g_clear_pointer(&snapshot->execution_report, g_free);
    snapshot->valid = 0;
}

static char *editable_text_copy(GtkWidget *widget) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(widget));
    return g_strdup(text == NULL ? "" : text);
}

static void operation_options_snapshot_capture(app_widgets *widgets) {
    operation_options_snapshot *snapshot = &widgets->options_snapshot;
    operation_options_snapshot_clear(widgets);
    snapshot->start = editable_text_copy(widgets->read_start_entry);
    snapshot->length = editable_text_copy(widgets->read_length_entry);
    snapshot->output = editable_text_copy(widgets->read_output_entry);
    snapshot->timeout = editable_text_copy(widgets->read_timeout_entry);
    snapshot->image = editable_text_copy(widgets->verify_image_entry);
    snapshot->image_base = editable_text_copy(widgets->image_base_entry);
    snapshot->image_gap_fill = editable_text_copy(
        widgets->image_gap_fill_entry);
    snapshot->plan_report = editable_text_copy(
        widgets->operation_report_entry);
    snapshot->execution_report = editable_text_copy(
        widgets->execution_report_entry);
    snapshot->image_format = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->image_format_dropdown));
    snapshot->image_gap_policy = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->image_gap_dropdown));
    snapshot->verify_passes = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->verify_margin_dropdown));
    snapshot->verify_margin_policy = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->parameters_margin_dropdown));
    snapshot->verify_margin_type = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->verify_margin_type_dropdown));
    snapshot->use_resident_buffer = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->verify_use_buffer_check));
    snapshot->identity_override = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->program_identity_override_check));
    snapshot->valid = 1;
}

static void operation_options_snapshot_restore(app_widgets *widgets) {
    operation_options_snapshot *snapshot = &widgets->options_snapshot;
    if (!snapshot->valid) return;
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_start_entry), snapshot->start);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_length_entry), snapshot->length);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_output_entry), snapshot->output);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_timeout_entry), snapshot->timeout);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->verify_image_entry), snapshot->image);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->image_base_entry), snapshot->image_base);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->image_gap_fill_entry),
        snapshot->image_gap_fill);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->operation_report_entry),
        snapshot->plan_report);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->execution_report_entry),
        snapshot->execution_report);
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(widgets->image_format_dropdown),
        snapshot->image_format);
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(widgets->image_gap_dropdown),
        snapshot->image_gap_policy);
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(widgets->verify_margin_dropdown),
        snapshot->verify_passes);
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(widgets->parameters_margin_dropdown),
        snapshot->verify_margin_policy);
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(widgets->verify_margin_type_dropdown),
        snapshot->verify_margin_type);
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(widgets->verify_use_buffer_check),
        snapshot->use_resident_buffer);
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(widgets->program_identity_override_check),
        snapshot->identity_override);
}

static void present_settings_dialog(
    app_widgets *widgets, GtkWindow *window
) {
    operation_options_snapshot_capture(widgets);
    gtk_window_present(window);
}

static void settings_dialog_accept(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    operation_options_snapshot_clear(widgets);
    if (GTK_IS_WINDOW(root))
        gtk_widget_set_visible(GTK_WIDGET(root), FALSE);
}

static void settings_dialog_cancel(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    operation_options_snapshot_restore(widgets);
    operation_options_snapshot_clear(widgets);
    if (GTK_IS_WINDOW(root))
        gtk_widget_set_visible(GTK_WIDGET(root), FALSE);
}

static void settings_dialog_help(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    gtk_window_set_transient_for(
        widgets->help_window,
        GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : widgets->window);
    gtk_window_present(widgets->help_window);
}

static gboolean settings_dialog_close_requested(
    GtkWindow *window, gpointer user_data
) {
    app_widgets *widgets = user_data;
    operation_options_snapshot_restore(widgets);
    operation_options_snapshot_clear(widgets);
    gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
    return TRUE;
}

static void operation_option_editor_committed(
    lt48_property_model *model, gpointer user_data
) {
    app_widgets *widgets = user_data;
    uint32_t start = 0u;
    uint32_t end = 0u;
    int have_start = 0;
    int have_end = 0;
    size_t page_index;
    for (page_index = 0u; page_index < model->page_count; ++page_index) {
        lt48_property_page *page = &model->pages[page_index];
        size_t control_index;
        for (control_index = 0u; control_index < page->control_count;
             ++control_index) {
            lt48_property_control *control = &page->controls[control_index];
            if (strcmp(control->key, "option.number.0") == 0) {
                start = control->value;
                have_start = 1;
            } else if (strcmp(control->key, "option.number.1") == 0) {
                end = control->value;
                have_end = 1;
            } else if (strcmp(control->key, "option.choice.0") == 0) {
                if (control->value == 0u || control->value == 1u) {
                    gtk_drop_down_set_selected(
                        GTK_DROP_DOWN(widgets->verify_margin_dropdown), 0u);
                    gtk_drop_down_set_selected(
                        GTK_DROP_DOWN(widgets->verify_margin_type_dropdown),
                        control->value);
                } else if (control->value == 2u) {
                    gtk_drop_down_set_selected(
                        GTK_DROP_DOWN(widgets->verify_margin_dropdown), 1u);
                }
            }
        }
    }
    if (have_start) {
        char text[32];
        g_snprintf(text, sizeof(text), "%u", start);
        gtk_editable_set_text(GTK_EDITABLE(widgets->read_start_entry), text);
    }
    if (have_start && have_end && end >= start) {
        char text[32];
        g_snprintf(text, sizeof(text), "%u", end - start + 1u);
        gtk_editable_set_text(GTK_EDITABLE(widgets->read_length_entry), text);
    }
    gtk_label_set_text(
        GTK_LABEL(widgets->read_preparation),
        "Device Operation Option accepted. Range and verify selections are "
        "applied to subsequent operations; the remaining recovered family "
        "settings are retained for this selected definition.");
}

static lt48_status ensure_operation_option_document(app_widgets *widgets) {
    lt48_status status;
    if (widgets->installation == NULL || !widgets->has_selected_device)
        return LT48_NOT_FOUND;
    if (widgets->operation_option_document != NULL &&
        widgets->operation_option_device_index !=
            widgets->selected_device_index) {
        lt48_property_document_close(widgets->operation_option_document);
        widgets->operation_option_document = NULL;
    }
    if (widgets->operation_option_document != NULL) return LT48_OK;
    status = lt48_installation_open_device_property_document(
        widgets->installation, widgets->selected_device_index,
        LT48_PROPERTY_SHEET_OPERATION_OPTION,
        &widgets->operation_option_document);
    if (status == LT48_OK)
        widgets->operation_option_device_index =
            widgets->selected_device_index;
    return status;
}

static void restore_pending_project_operation_options(
    app_widgets *widgets, const char *vendor, const char *device
) {
    GHashTableIter iterator;
    gpointer key;
    gpointer value;
    lt48_property_value *values;
    lt48_property_model *model;
    size_t index = 0u;
    size_t count;
    lt48_status status;
    if (widgets->pending_project_option_values == NULL) return;
    if (g_strcmp0(vendor, widgets->pending_project_option_vendor) != 0 ||
        g_strcmp0(device, widgets->pending_project_option_device) != 0) {
        project_status(
            widgets,
            "Saved Device Operation Option values were not applied because "
            "a different device definition was selected");
        clear_pending_project_operation_options(widgets);
        return;
    }
    status = ensure_operation_option_document(widgets);
    if (status != LT48_OK) {
        char *message = g_strdup_printf(
            "Project rejected its Device Operation Option state: the exact "
            "selected definition has no compatible option model (%s)",
            lt48_status_string(status));
        project_status(widgets, message);
        g_free(message);
        clear_pending_project_operation_options(widgets);
        return;
    }
    count = g_hash_table_size(widgets->pending_project_option_values);
    values = g_new0(lt48_property_value, count);
    g_hash_table_iter_init(
        &iterator, widgets->pending_project_option_values);
    while (g_hash_table_iter_next(&iterator, &key, &value)) {
        values[index].key = key;
        values[index].value = *(const guint32 *)value;
        ++index;
    }
    model = lt48_property_document_model(
        widgets->operation_option_document);
    status = lt48_property_model_apply_values(model, values, count);
    g_free(values);
    if (status == LT48_OK) {
        operation_option_editor_committed(model, widgets);
        project_status(
            widgets,
            "Project device and exact Device Operation Option values restored");
    } else {
        project_status(
            widgets,
            "Project rejected its Device Operation Option state because its "
            "keys or values do not match the verified device definition");
    }
    clear_pending_project_operation_options(widgets);
}

static void open_operation_option_editor(app_widgets *widgets) {
    lt48_property_model *model;
    GtkWindow *sheet;
    lt48_status status;
    if (widgets->main_notebook != NULL)
        gtk_notebook_set_current_page(
            GTK_NOTEBOOK(widgets->main_notebook), PAGE_DEVICES);
    if (widgets->installation == NULL || !widgets->has_selected_device) return;
    status = ensure_operation_option_document(widgets);
    if (status != LT48_OK && status != LT48_NOT_FOUND) {
            char *message = g_strdup_printf(
                "Device Operation Option metadata could not be opened: %s",
                lt48_status_string(status));
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
            g_free(message);
            return;
    }
    if (widgets->operation_option_document == NULL) {
        /* No optional TFamCode record means that the original material does
         * not provide exact masks/defaults for this family. */
        if (widgets->operation_options_window != NULL)
            present_settings_dialog(
                widgets, widgets->operation_options_window);
        return;
    }
    model = lt48_property_document_model(
        widgets->operation_option_document);
    sheet = lt48_gtk_property_sheet_new(
        widgets->window, widgets->help_window, model,
        operation_option_editor_committed, widgets);
    if (sheet == NULL) {
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Device Operation Option failed portable-model validation");
        return;
    }
    gtk_window_present(sheet);
}

static void quick_option_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    open_operation_option_editor(user_data);
}

static void configuration_document_destroyed(
    GtkWidget *window, gpointer user_data
) {
    (void)window;
    lt48_property_document_close(user_data);
}

static void configuration_editor_committed(
    lt48_property_model *model, gpointer user_data
) {
    app_widgets *widgets = user_data;
    char *message = g_strdup_printf(
        "%s values accepted in the editor. The decoded property binding is "
        "retained for the selected definition; live device application remains "
        "disabled until that definition's write-back callback is decoded.",
        model->title);
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
    g_free(message);
}

static void open_configuration_editor(app_widgets *widgets) {
    lt48_property_document *document = NULL;
    lt48_property_model *model;
    GtkWindow *sheet;
    lt48_status status;
    if (widgets->installation == NULL || !widgets->has_selected_device) return;
    status = lt48_installation_open_device_property_document(
        widgets->installation, widgets->selected_device_index,
        LT48_PROPERTY_SHEET_CONFIGURATION, &document);
    if (status != LT48_OK) {
        char *message = g_strdup_printf(
            "Configuration metadata could not be opened: %s",
            lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
        g_free(message);
        return;
    }
    model = lt48_property_document_model(document);
    sheet = lt48_gtk_property_sheet_new(
        widgets->window, widgets->help_window, model,
        configuration_editor_committed, widgets);
    if (sheet == NULL) {
        lt48_property_document_close(document);
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Configuration metadata failed portable-model validation");
        return;
    }
    g_signal_connect(
        sheet, "destroy", G_CALLBACK(configuration_document_destroyed),
        document);
    gtk_window_present(sheet);
}

static void fuse_session_reset(app_widgets *widgets) {
    g_clear_pointer(&widgets->fuse_documents, g_ptr_array_unref);
}

static void fuse_session_document_free(gpointer data) {
    fuse_session_document *document = data;
    if (document == NULL) return;
    g_free(document->bits);
    g_free(document->undo_bits);
    g_free(document);
}

static fuse_session_document *fuse_session_document_get(
    app_widgets *widgets, uint16_t code_id, size_t fuse_count
) {
    fuse_session_document *document;
    if (widgets->fuse_documents == NULL)
        widgets->fuse_documents = g_ptr_array_new_with_free_func(
            fuse_session_document_free);
    for (guint index = 0u; index < widgets->fuse_documents->len; ++index) {
        document = g_ptr_array_index(widgets->fuse_documents, index);
        if (document->device_index == widgets->selected_device_index &&
            document->code_id == code_id &&
            document->fuse_count == fuse_count)
            return document;
    }
    document = g_new0(fuse_session_document, 1);
    document->device_index = widgets->selected_device_index;
    document->code_id = code_id;
    document->fuse_count = fuse_count;
    document->bits = g_malloc(fuse_count);
    memset(document->bits, 1, fuse_count);
    g_ptr_array_add(widgets->fuse_documents, document);
    return document;
}

static lt48_status selected_fuse_code_open(
    app_widgets *widgets, lt48_rez **out_rez, lt48_fuse_code *out_code,
    fuse_binding_kind kind, size_t ordinal, uint16_t *out_identifier,
    size_t *out_binding_count
) {
    lt48_device_spec spec;
    lt48_logic_fuse_codes bindings;
    lt48_rez *rez = NULL;
    size_t resource_index;
    char *filename;
    char *path;
    char name[24];
    lt48_status status;
    if (widgets == NULL || out_rez == NULL || out_code == NULL ||
        out_identifier == NULL || out_binding_count == NULL ||
        widgets->installation == NULL || !widgets->has_selected_device)
        return LT48_INVALID_ARGUMENT;
    *out_rez = NULL;
    *out_binding_count = 0u;
    status = lt48_installation_resolve_device(
        widgets->installation, widgets->selected_device_index, &spec);
    if (status != LT48_OK || spec.definition_stem == NULL) return status;
    filename = g_strdup_printf("%s.rez", spec.definition_stem);
    path = g_build_filename(
        lt48_installation_path(widgets->installation), filename, NULL);
    g_free(filename);
    status = lt48_rez_open(path, &rez);
    g_free(path);
    if (status != LT48_OK) return status;
    g_snprintf(name, sizeof(name), "D%x", spec.catalogue.device_type);
    status = lt48_rez_find_resource(rez, name, &resource_index);
    if (status == LT48_OK)
        status = lt48_rez_get_logic_fuse_codes(
            rez, resource_index, &bindings);
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return status;
    }
    if (kind == FUSE_BINDING_PRIMARY) {
        *out_binding_count = bindings.count;
        if (ordinal >= bindings.count) {
            lt48_rez_close(rez);
            return LT48_NOT_FOUND;
        }
        *out_identifier = bindings.identifiers[ordinal];
    } else if (kind == FUSE_BINDING_E_FIELD && bindings.has_e_field) {
        *out_binding_count = 1u;
        if (ordinal != 0u) {
            lt48_rez_close(rez);
            return LT48_NOT_FOUND;
        }
        *out_identifier = bindings.e_field_identifier;
    } else if (kind == FUSE_BINDING_USER_CODE && bindings.has_user_code) {
        *out_binding_count = 1u;
        if (ordinal != 0u) {
            lt48_rez_close(rez);
            return LT48_NOT_FOUND;
        }
        *out_identifier = bindings.user_code_identifier;
    } else {
        lt48_rez_close(rez);
        return LT48_NOT_FOUND;
    }
    g_snprintf(name, sizeof(name), "F%x", *out_identifier);
    status = lt48_rez_find_resource(rez, name, &resource_index);
    if (status == LT48_OK)
        status = lt48_rez_get_fuse_code(rez, resource_index, out_code);
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return status;
    }
    *out_rez = rez;
    return LT48_OK;
}

static gboolean selected_fuse_editor_available(app_widgets *widgets) {
    lt48_rez *rez = NULL;
    lt48_fuse_code code;
    uint16_t identifier;
    size_t binding_count;
    lt48_status status = selected_fuse_code_open(
        widgets, &rez, &code, FUSE_BINDING_PRIMARY, 0u, &identifier,
        &binding_count);
    if (status == LT48_OK)
        status = lt48_fuse_layout_analyze(&code, &(lt48_fuse_layout){0});
    lt48_rez_close(rez);
    return status == LT48_OK && binding_count != 0u;
}

static gboolean selected_auxiliary_fuse_editor_available(
    app_widgets *widgets, fuse_binding_kind kind
) {
    lt48_rez *rez = NULL;
    lt48_fuse_code code;
    uint16_t identifier;
    size_t binding_count;
    lt48_status status;
    if (kind != FUSE_BINDING_E_FIELD && kind != FUSE_BINDING_USER_CODE)
        return FALSE;
    status = selected_fuse_code_open(
        widgets, &rez, &code, kind, 0u, &identifier, &binding_count);
    if (status == LT48_OK)
        status = lt48_fuse_layout_analyze(&code, &(lt48_fuse_layout){0});
    lt48_rez_close(rez);
    return status == LT48_OK && binding_count == 1u;
}

static int fuse_editor_capture_undo(fuse_session_document *document) {
    unsigned char *copy;
    if (document == NULL || document->bits == NULL ||
        document->fuse_count == 0u) return 0;
    copy = g_memdup2(document->bits, document->fuse_count);
    if (copy == NULL) return 0;
    g_free(document->undo_bits);
    document->undo_bits = copy;
    document->has_undo = 1;
    return 1;
}

static void fuse_editor_refresh(fuse_editor *editor) {
    guint radix_index;
    unsigned radix;
    char address[32];
    if (editor == NULL || editor->document == NULL ||
        editor->document->bits == NULL) return;
    radix_index = gtk_drop_down_get_selected(GTK_DROP_DOWN(editor->radix));
    radix = radix_index == 0u ? 16u : 10u;
    editor->refreshing = 1;
    for (size_t slot = 0u; slot < editor->entries->len; ++slot) {
        size_t row = editor->first_row + slot;
        GtkWidget *address_widget = g_ptr_array_index(editor->addresses, slot);
        GtkWidget *entry_widget = g_ptr_array_index(editor->entries, slot);
        if (row >= editor->row_count) {
            gtk_widget_set_visible(address_widget, FALSE);
            gtk_widget_set_visible(entry_widget, FALSE);
            continue;
        }
        const lt48_fuse_row *map = &editor->rows[row];
        char *text = g_malloc((size_t)map->length + 1u);
        gtk_widget_set_visible(address_widget, TRUE);
        gtk_widget_set_visible(entry_widget, TRUE);
        if (radix == 16u)
            g_snprintf(address, sizeof(address), "%08X", map->first_fuse);
        else
            g_snprintf(address, sizeof(address), "%u", map->first_fuse);
        gtk_label_set_text(
            GTK_LABEL(address_widget), address);
        for (uint16_t column = 0u; column < map->length; ++column)
            text[column] = editor->document->bits[
                (size_t)map->first_fuse + column] ? '-' : 'X';
        text[map->length] = '\0';
        gtk_editable_set_text(
            GTK_EDITABLE(entry_widget), text);
        gtk_entry_set_max_length(GTK_ENTRY(entry_widget), map->length);
        g_object_set_data(
            G_OBJECT(entry_widget), "labtool48-fuse-row",
            GSIZE_TO_POINTER(row + 1u));
        g_free(text);
    }
    gtk_widget_set_sensitive(editor->undo, editor->document->has_undo);
    editor->refreshing = 0;
}

static void fuse_editor_scroll_changed(
    GtkAdjustment *adjustment, gpointer user_data
) {
    fuse_editor *editor = user_data;
    editor->first_row = (size_t)gtk_adjustment_get_value(adjustment);
    fuse_editor_refresh(editor);
}

static void fuse_editor_row_changed(
    GtkEditable *editable, gpointer user_data
) {
    fuse_editor *editor = user_data;
    size_t encoded = GPOINTER_TO_SIZE(
        g_object_get_data(G_OBJECT(editable), "labtool48-fuse-row"));
    const char *text = gtk_editable_get_text(editable);
    const lt48_fuse_row *row;
    int changed = 0;
    if (editor->refreshing || encoded == 0u || encoded > editor->row_count)
        return;
    row = &editor->rows[encoded - 1u];
    if (strlen(text) != row->length) {
        gtk_label_set_text(
            GTK_LABEL(editor->status),
            "A fuse row must retain its exact definition-provided width.");
        return;
    }
    for (uint16_t column = 0u; column < row->length; ++column) {
        if (text[column] != '-' && text[column] != 'X' && text[column] != 'x') {
            gtk_label_set_text(
                GTK_LABEL(editor->status),
                "Use '-' for an intact fuse and 'X' for a blown fuse.");
            return;
        }
        if ((editor->document->bits[(size_t)row->first_fuse + column] != 0u) !=
            (text[column] == '-'))
            changed = 1;
    }
    if (!changed) return;
    if (!fuse_editor_capture_undo(editor->document)) {
        gtk_label_set_text(GTK_LABEL(editor->status), "Undo snapshot failed.");
        return;
    }
    for (uint16_t column = 0u; column < row->length; ++column)
        editor->document->bits[(size_t)row->first_fuse + column] =
            text[column] == '-' ? 1u : 0u;
    gtk_widget_set_sensitive(editor->undo, TRUE);
    gtk_label_set_text(
        GTK_LABEL(editor->status),
        "Fuse row accepted in the selected device's session document.");
}

static void fuse_editor_radix_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    (void)object;
    (void)parameter;
    fuse_editor_refresh(user_data);
}

static void fuse_editor_undo(GtkButton *button, gpointer user_data) {
    fuse_editor *editor = user_data;
    (void)button;
    if (!editor->document->has_undo || editor->document->undo_bits == NULL)
        return;
    memcpy(editor->document->bits, editor->document->undo_bits,
           editor->document->fuse_count);
    g_clear_pointer(&editor->document->undo_bits, g_free);
    editor->document->has_undo = 0;
    fuse_editor_refresh(editor);
    gtk_label_set_text(
        GTK_LABEL(editor->status), "The last fuse-map change was undone.");
}

static void fuse_fill_destroyed(GtkWidget *window, gpointer user_data) {
    (void)window;
    g_free(user_data);
}

static void fuse_fill_apply(GtkButton *button, gpointer user_data) {
    fuse_fill_dialog *fill = user_data;
    size_t from = (size_t)gtk_spin_button_get_value_as_int(
        GTK_SPIN_BUTTON(fill->from));
    size_t to = (size_t)gtk_spin_button_get_value_as_int(
        GTK_SPIN_BUTTON(fill->to));
    unsigned char value = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(fill->intact)) ? 1u : 0u;
    (void)button;
    if (from > to || to >= fill->editor->document->fuse_count) {
        gtk_label_set_text(
            GTK_LABEL(fill->editor->status), "Fill range is invalid.");
        return;
    }
    if (!fuse_editor_capture_undo(fill->editor->document)) return;
    memset(fill->editor->document->bits + from, value, to - from + 1u);
    fuse_editor_refresh(fill->editor);
    gtk_label_set_text(
        GTK_LABEL(fill->editor->status),
        value ? "Selected fuses set intact." : "Selected fuses blown.");
    gtk_window_destroy(fill->window);
}

static void fuse_editor_fill(GtkButton *button, gpointer user_data) {
    fuse_editor *editor = user_data;
    fuse_fill_dialog *fill = g_new0(fuse_fill_dialog, 1);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *blown = gtk_check_button_new_with_label("Blown (X)");
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *apply = gtk_button_new_with_label("Apply");
    (void)button;
    fill->editor = editor;
    fill->window = GTK_WINDOW(gtk_window_new());
    fill->from = gtk_spin_button_new_with_range(
        0.0, (double)editor->document->fuse_count - 1.0, 1.0);
    fill->to = gtk_spin_button_new_with_range(
        0.0, (double)editor->document->fuse_count - 1.0, 1.0);
    fill->intact = gtk_check_button_new_with_label("Intact (-)");
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(fill->to), (double)editor->document->fuse_count - 1.0);
    gtk_check_button_set_group(
        GTK_CHECK_BUTTON(blown), GTK_CHECK_BUTTON(fill->intact));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(fill->intact), TRUE);
    gtk_window_set_title(fill->window, "Fill Fuse Map");
    gtk_window_set_transient_for(fill->window, editor->window);
    gtk_window_set_modal(fill->window, TRUE);
    gtk_window_set_destroy_with_parent(fill->window, TRUE);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("From:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), fill->from, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("To:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), fill->to, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), fill->intact, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), blown, 0, 3, 2, 1);
    gtk_widget_set_hexpand(actions, TRUE);
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(actions), cancel);
    gtk_box_append(GTK_BOX(actions), apply);
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(fill->window, box);
    g_signal_connect_swapped(
        cancel, "clicked", G_CALLBACK(gtk_window_destroy), fill->window);
    g_signal_connect(apply, "clicked", G_CALLBACK(fuse_fill_apply), fill);
    g_signal_connect(
        fill->window, "destroy", G_CALLBACK(fuse_fill_destroyed), fill);
    gtk_window_present(fill->window);
}

static void fuse_editor_help(GtkButton *button, gpointer user_data) {
    fuse_editor *editor = user_data;
    (void)button;
    gtk_window_set_transient_for(editor->owner->help_window, editor->window);
    gtk_window_present(editor->owner->help_window);
}

static void fuse_editor_destroyed(GtkWidget *window, gpointer user_data) {
    fuse_editor *editor = user_data;
    (void)window;
    lt48_rez_close(editor->rez);
    g_free(editor->rows);
    g_ptr_array_free(editor->addresses, TRUE);
    g_ptr_array_free(editor->entries, TRUE);
    g_free(editor);
}

static void open_fuse_editor_context_kind(
    app_widgets *widgets, fuse_binding_kind kind, size_t ordinal
) {
    fuse_editor *editor;
    lt48_rez *rez = NULL;
    lt48_fuse_code code;
    uint16_t identifier;
    size_t binding_count;
    size_t row_count = 0u;
    lt48_status status;
    GtkWidget *box;
    GtkWidget *header;
    GtkWidget *scroll;
    GtkWidget *map_box;
    GtkWidget *row_scroll;
    GtkWidget *grid;
    GtkWidget *actions;
    GtkWidget *fill;
    GtkWidget *help;
    GtkWidget *done;
    status = selected_fuse_code_open(
        widgets, &rez, &code, kind, ordinal, &identifier, &binding_count);
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "The selected definition has no decoded fuse-map layout.");
        return;
    }
    status = lt48_fuse_layout_expand_rows(&code, NULL, 0u, &row_count);
    editor = g_new0(fuse_editor, 1);
    editor->rows = g_new0(lt48_fuse_row, row_count);
    if (status == LT48_OK)
        status = lt48_fuse_layout_expand_rows(
            &code, editor->rows, row_count, &row_count);
    if (status != LT48_OK || row_count == 0u) {
        lt48_rez_close(rez);
        g_free(editor->rows);
        g_free(editor);
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "The selected fuse-map row layout failed validation.");
        return;
    }
    editor->owner = widgets;
    editor->document = fuse_session_document_get(
        widgets, identifier, code.total_fuse_count);
    editor->rez = rez;
    editor->code = code;
    editor->row_count = row_count;
    editor->addresses = g_ptr_array_new();
    editor->entries = g_ptr_array_new();
    editor->window = GTK_WINDOW(gtk_window_new());
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    scroll = gtk_scrolled_window_new();
    map_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    grid = gtk_grid_new();
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    editor->radix = gtk_drop_down_new_from_strings(
        (const char *const[]){"Hexadecimal", "Decimal", NULL});
    editor->status = gtk_label_new(
        "'-' is intact and 'X' is blown. Changes remain in this selected-device session.");
    editor->undo = gtk_button_new_with_label("Undo");
    fill = gtk_button_new_with_label("Fill");
    help = gtk_button_new_with_label("Help");
    done = gtk_button_new_with_label("Done");
    gtk_window_set_title(
        editor->window,
        kind == FUSE_BINDING_E_FIELD ? "E-Field Fuse Edit" :
        kind == FUSE_BINDING_USER_CODE ? "User-Code Fuse Edit" :
        "Fuse Edit");
    gtk_window_set_transient_for(editor->window, widgets->window);
    gtk_window_set_modal(editor->window, TRUE);
    gtk_window_set_destroy_with_parent(editor->window, TRUE);
    gtk_window_set_default_size(editor->window, 920, 640);
    gtk_widget_set_margin_top(box, 10);
    gtk_widget_set_margin_bottom(box, 10);
    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_box_append(GTK_BOX(header), gtk_label_new("Address radix:"));
    gtk_box_append(GTK_BOX(header), editor->radix);
    gtk_box_append(GTK_BOX(header), gtk_label_new("Address"));
    gtk_box_append(GTK_BOX(header), gtk_label_new("Fuse map"));
    gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    for (size_t row = 0u; row < MIN(row_count, 16u); ++row) {
        GtkWidget *address = gtk_label_new("");
        GtkWidget *entry = gtk_entry_new();
        char *accessible_name = g_strdup_printf("Fuse row %zu", row + 1u);
        gtk_label_set_xalign(GTK_LABEL(address), 1.0F);
        gtk_widget_add_css_class(address, "monospace");
        gtk_widget_add_css_class(entry, "monospace");
        gtk_accessible_update_property(
            GTK_ACCESSIBLE(entry), GTK_ACCESSIBLE_PROPERTY_LABEL,
            accessible_name, -1);
        g_free(accessible_name);
        gtk_widget_set_hexpand(entry, TRUE);
        g_object_set_data(
            G_OBJECT(entry), "labtool48-fuse-row",
            GSIZE_TO_POINTER(row + 1u));
        g_signal_connect(entry, "changed",
                         G_CALLBACK(fuse_editor_row_changed), editor);
        gtk_grid_attach(GTK_GRID(grid), address, 0, (int)row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), entry, 1, (int)row, 1, 1);
        g_ptr_array_add(editor->addresses, address);
        g_ptr_array_add(editor->entries, entry);
    }
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_hexpand(scroll, TRUE);
    editor->row_adjustment = gtk_adjustment_new(
        0.0, 0.0, (double)row_count, 1.0, 16.0,
        (double)MIN(row_count, 16u));
    row_scroll = gtk_scrollbar_new(
        GTK_ORIENTATION_VERTICAL, editor->row_adjustment);
    gtk_box_append(GTK_BOX(map_box), scroll);
    gtk_box_append(GTK_BOX(map_box), row_scroll);
    gtk_label_set_xalign(GTK_LABEL(editor->status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(editor->status), TRUE);
    gtk_widget_set_hexpand(editor->status, TRUE);
    gtk_box_append(GTK_BOX(actions), help);
    gtk_box_append(GTK_BOX(actions), editor->status);
    gtk_box_append(GTK_BOX(actions), editor->undo);
    gtk_box_append(GTK_BOX(actions), fill);
    gtk_box_append(GTK_BOX(actions), done);
    gtk_box_append(GTK_BOX(box), header);
    gtk_box_append(GTK_BOX(box), map_box);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(editor->window, box);
    g_signal_connect(editor->radix, "notify::selected",
                     G_CALLBACK(fuse_editor_radix_changed), editor);
    g_signal_connect(editor->row_adjustment, "value-changed",
                     G_CALLBACK(fuse_editor_scroll_changed), editor);
    g_signal_connect(editor->undo, "clicked",
                     G_CALLBACK(fuse_editor_undo), editor);
    g_signal_connect(fill, "clicked", G_CALLBACK(fuse_editor_fill), editor);
    g_signal_connect(help, "clicked", G_CALLBACK(fuse_editor_help), editor);
    g_signal_connect_swapped(
        done, "clicked", G_CALLBACK(gtk_window_destroy), editor->window);
    g_signal_connect(
        editor->window, "destroy", G_CALLBACK(fuse_editor_destroyed), editor);
    fuse_editor_refresh(editor);
    gtk_window_present(editor->window);
}

static void open_fuse_editor_context(app_widgets *widgets, size_t ordinal) {
    open_fuse_editor_context_kind(
        widgets, FUSE_BINDING_PRIMARY, ordinal);
}

static void open_auxiliary_fuse_editor(
    app_widgets *widgets, fuse_binding_kind kind
) {
    if (kind != FUSE_BINDING_E_FIELD && kind != FUSE_BINDING_USER_CODE)
        return;
    open_fuse_editor_context_kind(widgets, kind, 0u);
}

static void fuse_context_chooser_destroyed(
    GtkWidget *window, gpointer user_data
) {
    fuse_context_chooser *chooser = user_data;
    (void)window;
    g_clear_object(&chooser->file);
    g_free(chooser);
}

static void fuse_jedec_load_context(
    app_widgets *widgets, size_t ordinal, GFile *file
) {
    lt48_rez *rez = NULL;
    lt48_fuse_code code;
    lt48_jedec_info info;
    fuse_session_document *document;
    unsigned char *loaded = NULL;
    gchar *contents = NULL;
    gsize size = 0u;
    uint16_t identifier = 0u;
    size_t binding_count;
    GError *error = NULL;
    char *path = NULL;
    char *message;
    lt48_status status = selected_fuse_code_open(
        widgets, &rez, &code, FUSE_BINDING_PRIMARY, ordinal, &identifier,
        &binding_count);
    if (status == LT48_OK &&
        !g_file_load_contents(file, NULL, &contents, &size, NULL, &error))
        status = LT48_FILE_ERROR;
    if (status == LT48_OK) {
        loaded = g_malloc(code.total_fuse_count);
        status = lt48_jedec_import(
            (const unsigned char *)contents, size, loaded,
            code.total_fuse_count, &info);
    }
    if (status == LT48_OK) {
        document = fuse_session_document_get(
            widgets, identifier, code.total_fuse_count);
        if (!fuse_editor_capture_undo(document))
            status = LT48_NO_MEMORY;
        else
            memcpy(document->bits, loaded, document->fuse_count);
    }
    path = g_file_get_path(file);
    if (status == LT48_OK) {
        if (widgets->footer_file != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->footer_file),
                path == NULL ? "JEDEC file" : path);
        message = g_strdup_printf(
            "Loaded JEDEC into fuse context F%X: %u fuses, checksum %04X%s.",
            identifier, code.total_fuse_count, info.fuse_checksum,
            info.security_enabled ? ", security enabled" : "");
    } else {
        message = g_strdup_printf(
            "JEDEC load failed for fuse context F%X: %s", identifier,
            error == NULL ? lt48_status_string(status) : error->message);
    }
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
    g_free(message);
    g_free(path);
    g_free(loaded);
    g_free(contents);
    g_clear_error(&error);
    lt48_rez_close(rez);
}

static void fuse_jedec_save_context(
    app_widgets *widgets, size_t ordinal, GFile *file
) {
    lt48_rez *rez = NULL;
    lt48_fuse_code code;
    lt48_jedec_info info;
    lt48_fuse_row *rows = NULL;
    fuse_session_document *document = NULL;
    unsigned char *contents = NULL;
    uint16_t identifier = 0u;
    size_t binding_count;
    size_t row_count = 0u;
    size_t size = 0u;
    GError *error = NULL;
    char *path = NULL;
    char *message;
    lt48_status status = selected_fuse_code_open(
        widgets, &rez, &code, FUSE_BINDING_PRIMARY, ordinal, &identifier,
        &binding_count);
    if (status == LT48_OK)
        status = lt48_fuse_layout_expand_rows(
            &code, NULL, 0u, &row_count);
    if (status == LT48_OK) {
        rows = g_new0(lt48_fuse_row, row_count);
        status = lt48_fuse_layout_expand_rows(
            &code, rows, row_count, &row_count);
    }
    if (status == LT48_OK)
        document = fuse_session_document_get(
            widgets, identifier, code.total_fuse_count);
    if (status == LT48_OK)
        status = lt48_jedec_export(
            document->bits, document->fuse_count, rows, row_count,
            NULL, 0u, &size, &info);
    if (status == LT48_OK) {
        contents = g_malloc(size);
        status = lt48_jedec_export(
            document->bits, document->fuse_count, rows, row_count,
            contents, size, &size, &info);
    }
    if (status == LT48_OK && !g_file_replace_contents(
            file, (const char *)contents, size, NULL, FALSE,
            G_FILE_CREATE_REPLACE_DESTINATION, NULL, NULL, &error))
        status = LT48_FILE_ERROR;
    path = g_file_get_path(file);
    if (status == LT48_OK) {
        if (widgets->footer_file != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->footer_file),
                path == NULL ? "JEDEC file" : path);
        message = g_strdup_printf(
            "Saved fuse context F%X as JEDEC: %u fuses, checksum %04X.",
            identifier, code.total_fuse_count, info.fuse_checksum);
    } else {
        message = g_strdup_printf(
            "JEDEC save failed for fuse context F%X: %s", identifier,
            error == NULL ? lt48_status_string(status) : error->message);
    }
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
    g_free(message);
    g_free(path);
    g_free(contents);
    g_free(rows);
    g_clear_error(&error);
    lt48_rez_close(rez);
}

static void fuse_context_chosen(GtkButton *button, gpointer user_data) {
    fuse_context_chooser *chooser = user_data;
    size_t encoded = GPOINTER_TO_SIZE(
        g_object_get_data(G_OBJECT(button), "labtool48-fuse-context"));
    app_widgets *owner = chooser->owner;
    fuse_context_action action = chooser->action;
    GFile *file = chooser->file == NULL ? NULL : g_object_ref(chooser->file);
    if (encoded == 0u) return;
    gtk_window_destroy(chooser->window);
    if (action == FUSE_CONTEXT_EDIT)
        open_fuse_editor_context(owner, encoded - 1u);
    else if (action == FUSE_CONTEXT_LOAD_JEDEC)
        fuse_jedec_load_context(owner, encoded - 1u, file);
    else
        fuse_jedec_save_context(owner, encoded - 1u, file);
    g_clear_object(&file);
}

static void open_fuse_context_action(
    app_widgets *widgets, fuse_context_action action, GFile *file
) {
    lt48_rez *rez = NULL;
    lt48_fuse_code code;
    lt48_logic_fuse_codes bindings;
    lt48_device_spec spec;
    uint16_t identifier;
    size_t binding_count;
    size_t definition_index;
    char definition_name[24];
    char *filename;
    char *path;
    lt48_status status;
    status = selected_fuse_code_open(
        widgets, &rez, &code, FUSE_BINDING_PRIMARY, 0u, &identifier,
        &binding_count);
    lt48_rez_close(rez);
    if (status != LT48_OK || binding_count == 0u) return;
    if (binding_count == 1u) {
        if (action == FUSE_CONTEXT_EDIT)
            open_fuse_editor_context(widgets, 0u);
        else if (action == FUSE_CONTEXT_LOAD_JEDEC)
            fuse_jedec_load_context(widgets, 0u, file);
        else
            fuse_jedec_save_context(widgets, 0u, file);
        return;
    }
    status = lt48_installation_resolve_device(
        widgets->installation, widgets->selected_device_index, &spec);
    if (status != LT48_OK) return;
    filename = g_strdup_printf("%s.rez", spec.definition_stem);
    path = g_build_filename(
        lt48_installation_path(widgets->installation), filename, NULL);
    g_free(filename);
    status = lt48_rez_open(path, &rez);
    g_free(path);
    g_snprintf(
        definition_name, sizeof(definition_name), "D%x",
        spec.catalogue.device_type);
    if (status == LT48_OK)
        status = lt48_rez_find_resource(rez, definition_name, &definition_index);
    if (status == LT48_OK)
        status = lt48_rez_get_logic_fuse_codes(
            rez, definition_index, &bindings);
    lt48_rez_close(rez);
    if (status != LT48_OK) return;
    {
        fuse_context_chooser *chooser = g_new0(fuse_context_chooser, 1);
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *cancel = gtk_button_new_with_label("Cancel");
        chooser->owner = widgets;
        chooser->action = action;
        chooser->file = file == NULL ? NULL : g_object_ref(file);
        chooser->window = GTK_WINDOW(gtk_window_new());
        gtk_window_set_title(chooser->window, "Select Fuse Context");
        gtk_window_set_transient_for(chooser->window, widgets->window);
        gtk_window_set_modal(chooser->window, TRUE);
        gtk_window_set_destroy_with_parent(chooser->window, TRUE);
        gtk_widget_set_margin_top(box, 12);
        gtk_widget_set_margin_bottom(box, 12);
        gtk_widget_set_margin_start(box, 12);
        gtk_widget_set_margin_end(box, 12);
        gtk_box_append(
            GTK_BOX(box),
            gtk_label_new(
                action == FUSE_CONTEXT_EDIT ?
                "This definition owns multiple decoded fuse maps. Select the exact resource context to edit." :
                action == FUSE_CONTEXT_LOAD_JEDEC ?
                "Select the exact fuse context that receives this JEDEC file." :
                "Select the exact fuse context to save as JEDEC."));
        for (size_t index = 0u; index < bindings.count; ++index) {
            char label[32];
            GtkWidget *choice;
            g_snprintf(label, sizeof(label), "Fuse context F%X",
                       bindings.identifiers[index]);
            choice = gtk_button_new_with_label(label);
            g_object_set_data(
                G_OBJECT(choice), "labtool48-fuse-context",
                GSIZE_TO_POINTER(index + 1u));
            g_signal_connect(
                choice, "clicked", G_CALLBACK(fuse_context_chosen), chooser);
            gtk_box_append(GTK_BOX(box), choice);
        }
        gtk_widget_set_halign(actions, GTK_ALIGN_END);
        gtk_box_append(GTK_BOX(actions), cancel);
        gtk_box_append(GTK_BOX(box), actions);
        gtk_window_set_child(chooser->window, box);
        g_signal_connect_swapped(
            cancel, "clicked", G_CALLBACK(gtk_window_destroy), chooser->window);
        g_signal_connect(
            chooser->window, "destroy",
            G_CALLBACK(fuse_context_chooser_destroyed), chooser);
        gtk_window_present(chooser->window);
    }
}

static void open_fuse_editor(app_widgets *widgets) {
    open_fuse_context_action(widgets, FUSE_CONTEXT_EDIT, NULL);
}

static void vector_editor_refresh(vector_editor *editor) {
    lt48_test_vector_info info;
    char row[49];
    char clipboard[49];
    char value[32];
    if (editor == NULL || editor->owner->test_vectors == NULL ||
        lt48_test_vectors_get_info(
            editor->owner->test_vectors, &info) != LT48_OK)
        return;
    if (lt48_test_vectors_get_row(
            editor->owner->test_vectors, info.current_row,
            row, sizeof(row)) != LT48_OK ||
        lt48_test_vectors_get_clipboard(
            editor->owner->test_vectors, clipboard,
            sizeof(clipboard)) != LT48_OK)
        return;
    editor->refreshing = 1;
    gtk_spin_button_set_range(
        GTK_SPIN_BUTTON(editor->number), 1.0, (double)info.row_count);
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(editor->number), (double)info.current_row + 1.0);
    gtk_editable_set_text(GTK_EDITABLE(editor->row), row);
    gtk_editable_set_text(GTK_EDITABLE(editor->clipboard), clipboard);
    g_snprintf(value, sizeof(value), "%zu", info.row_count);
    gtk_label_set_text(GTK_LABEL(editor->total), value);
    g_snprintf(value, sizeof(value), "%zu", info.pin_count);
    gtk_label_set_text(GTK_LABEL(editor->maximum_pin), value);
    gtk_widget_set_sensitive(editor->undo, info.has_undo);
    gtk_widget_set_sensitive(editor->cut, info.row_count > 1u);
    editor->refreshing = 0;
}

static void vector_editor_cursor_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    vector_editor *editor = user_data;
    lt48_test_vector_info info;
    int position;
    char value[32];
    (void)parameter;
    position = gtk_editable_get_position(GTK_EDITABLE(object));
    if (lt48_test_vectors_get_info(
            editor->owner->test_vectors, &info) == LT48_OK &&
        position >= (int)info.pin_count)
        position = (int)info.pin_count - 1;
    g_snprintf(value, sizeof(value), "%d", position + 1);
    gtk_label_set_text(GTK_LABEL(editor->current_pin), value);
}

static void vector_editor_row_changed(
    GtkEditable *editable, gpointer user_data
) {
    vector_editor *editor = user_data;
    lt48_test_vector_info info;
    lt48_status status;
    if (editor->refreshing || editor->owner->test_vectors == NULL) return;
    if (lt48_test_vectors_get_info(
            editor->owner->test_vectors, &info) != LT48_OK)
        return;
    status = lt48_test_vectors_set_row(
        editor->owner->test_vectors, info.current_row,
        gtk_editable_get_text(editable));
    if (status == LT48_OK) {
        gtk_label_set_text(
            GTK_LABEL(editor->status),
            "Vector accepted in the session document; no hardware was operated.");
        if (lt48_test_vectors_get_info(
                editor->owner->test_vectors, &info) == LT48_OK)
            gtk_widget_set_sensitive(editor->undo, info.has_undo);
    } else {
        gtk_label_set_text(
            GTK_LABEL(editor->status),
            "Enter exactly one symbol per pin: 0 1 B C D F H K L N P U X Z.");
    }
}

static void vector_editor_number_changed(
    GtkSpinButton *spin, gpointer user_data
) {
    vector_editor *editor = user_data;
    size_t row;
    if (editor->refreshing || editor->owner->test_vectors == NULL) return;
    row = (size_t)gtk_spin_button_get_value_as_int(spin);
    if (row == 0u || lt48_test_vectors_set_current(
            editor->owner->test_vectors, row - 1u) != LT48_OK)
        return;
    vector_editor_refresh(editor);
}

static void vector_editor_action(GtkButton *button, gpointer user_data) {
    vector_editor *editor = user_data;
    const char *action = g_object_get_data(
        G_OBJECT(button), "labtool48-vector-action");
    lt48_status status = LT48_INVALID_ARGUMENT;
    if (action == NULL || editor->owner->test_vectors == NULL) return;
    if (strcmp(action, "undo") == 0)
        status = lt48_test_vectors_undo(editor->owner->test_vectors);
    else if (strcmp(action, "insert") == 0)
        status = lt48_test_vectors_insert(editor->owner->test_vectors);
    else if (strcmp(action, "copy") == 0)
        status = lt48_test_vectors_copy(editor->owner->test_vectors);
    else if (strcmp(action, "cut") == 0)
        status = lt48_test_vectors_cut(editor->owner->test_vectors);
    if (status == LT48_OK) {
        char *message = g_strdup_printf(
            "%s completed in the session vector document; no hardware was operated.",
            gtk_button_get_label(button));
        vector_editor_refresh(editor);
        gtk_label_set_text(GTK_LABEL(editor->status), message);
        g_free(message);
    } else {
        char *message = g_strdup_printf(
            "%s is not available in the current vector state: %s",
            gtk_button_get_label(button), lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(editor->status), message);
        g_free(message);
    }
}

static void vector_editor_destroyed(GtkWidget *window, gpointer user_data) {
    vector_editor *editor = user_data;
    (void)window;
    g_free(editor);
}

static GtkWidget *vector_field_label(const char *text) {
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 1.0F);
    return label;
}

static void open_vector_editor(app_widgets *widgets) {
    lt48_device_spec spec;
    vector_editor *editor;
    GtkWidget *box;
    GtkWidget *grid;
    GtkWidget *actions;
    GtkWidget *help;
    GtkWidget *done;
    static const struct {
        const char *label;
        const char *action;
    } commands[] = {
        {"Undo", "undo"}, {"Insert", "insert"},
        {"Copy", "copy"}, {"Cut", "cut"}
    };
    if (widgets->installation == NULL || !widgets->has_selected_device ||
        lt48_installation_resolve_device(
            widgets->installation, widgets->selected_device_index,
            &spec) != LT48_OK || spec.catalogue.device_class < 2019u ||
        spec.catalogue.device_class > 2028u || spec.pin_count == 0u)
        return;
    if (widgets->test_vectors == NULL &&
        lt48_test_vectors_create(
            spec.pin_count, &widgets->test_vectors) != LT48_OK) {
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "The function-test vector document could not be created.");
        return;
    }
    editor = g_new0(vector_editor, 1);
    editor->owner = widgets;
    editor->window = GTK_WINDOW(gtk_window_new());
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    grid = gtk_grid_new();
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    editor->number = gtk_spin_button_new_with_range(1.0, 1.0, 1.0);
    editor->row = gtk_entry_new();
    editor->total = gtk_label_new("1");
    editor->clipboard = gtk_entry_new();
    editor->current_pin = gtk_label_new("1");
    editor->maximum_pin = gtk_label_new("");
    editor->status = gtk_label_new(
        "Recovered editor semantics only; live Function Test remains separately gated.");
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(editor->number), GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Vector number", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(editor->row), GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Vector", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(editor->clipboard), GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Clipboard", -1);
    gtk_window_set_title(editor->window, "Function Test Vectors Edit");
    gtk_window_set_transient_for(editor->window, widgets->window);
    gtk_window_set_destroy_with_parent(editor->window, TRUE);
    gtk_window_set_modal(editor->window, TRUE);
    gtk_window_set_default_size(editor->window, 760, 260);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_attach(GTK_GRID(grid), vector_field_label("Vector number:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), editor->number, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), vector_field_label("Vector:"), 0, 1, 1, 1);
    gtk_widget_set_hexpand(editor->row, TRUE);
    gtk_entry_set_max_length(GTK_ENTRY(editor->row), (int)spec.pin_count);
    gtk_widget_add_css_class(editor->row, "monospace");
    gtk_grid_attach(GTK_GRID(grid), editor->row, 1, 1, 5, 1);
    gtk_grid_attach(GTK_GRID(grid), vector_field_label("Total:"), 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), editor->total, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), vector_field_label("Current pin:"), 4, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), editor->current_pin, 5, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), vector_field_label("Clipboard:"), 0, 2, 1, 1);
    gtk_editable_set_editable(GTK_EDITABLE(editor->clipboard), FALSE);
    gtk_widget_add_css_class(editor->clipboard, "monospace");
    gtk_grid_attach(GTK_GRID(grid), editor->clipboard, 1, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), vector_field_label("Maximum pin:"), 4, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), editor->maximum_pin, 5, 2, 1, 1);
    gtk_box_append(GTK_BOX(box), grid);
    for (size_t index = 0u; index < G_N_ELEMENTS(commands); ++index) {
        GtkWidget *command = gtk_button_new_with_label(commands[index].label);
        g_object_set_data(
            G_OBJECT(command), "labtool48-vector-action",
            (gpointer)commands[index].action);
        g_signal_connect(command, "clicked",
                         G_CALLBACK(vector_editor_action), editor);
        gtk_box_append(GTK_BOX(actions), command);
        if (index == 0u) editor->undo = command;
        if (index == 3u) editor->cut = command;
    }
    help = gtk_button_new_with_label("Help");
    done = gtk_button_new_with_label("Done");
    gtk_widget_set_hexpand(help, TRUE);
    gtk_widget_set_halign(help, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(actions), help);
    gtk_box_append(GTK_BOX(actions), done);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_label_set_xalign(GTK_LABEL(editor->status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(editor->status), TRUE);
    gtk_widget_add_css_class(editor->status, "dim-label");
    gtk_box_append(GTK_BOX(box), editor->status);
    gtk_window_set_child(editor->window, box);
    g_signal_connect(editor->row, "changed",
                     G_CALLBACK(vector_editor_row_changed), editor);
    g_signal_connect(editor->row, "notify::cursor-position",
                     G_CALLBACK(vector_editor_cursor_changed), editor);
    g_signal_connect(editor->number, "value-changed",
                     G_CALLBACK(vector_editor_number_changed), editor);
    g_signal_connect_swapped(help, "clicked",
                             G_CALLBACK(gtk_window_present),
                             widgets->help_window);
    g_signal_connect_swapped(done, "clicked",
                             G_CALLBACK(gtk_window_close), editor->window);
    g_signal_connect(editor->window, "destroy",
                     G_CALLBACK(vector_editor_destroyed), editor);
    vector_editor_refresh(editor);
    gtk_window_present(editor->window);
}

static void quick_vector_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    open_vector_editor(user_data);
}

static void quick_operation_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    guint index = GPOINTER_TO_UINT(g_object_get_data(
        G_OBJECT(button), "labtool48-operation-index"));
    lt48_operation_kind operation;
    gboolean uses_image;
    gboolean use_buffer;
    if (index >= G_N_ELEMENTS(gtk_operation_kinds)) return;
    if (index == 8u && widgets->window != NULL) {
        GAction *memory_protect = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "memory-protect");
        GAction *secure = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "secure");
        if ((memory_protect == NULL ||
             !g_action_get_enabled(memory_protect)) &&
            secure != NULL && g_action_get_enabled(secure))
            index = 7u;
    }
    operation = gtk_operation_kinds[index];
    gtk_drop_down_set_selected(GTK_DROP_DOWN(widgets->operation_dropdown),
                               index);
    gtk_notebook_set_current_page(
        GTK_NOTEBOOK(widgets->device_workflow), 0);
    if (operation == LT48_OPERATION_CONFIGURATION) {
        open_configuration_editor(widgets);
        return;
    }
    uses_image = operation == LT48_OPERATION_PROGRAM ||
        operation == LT48_OPERATION_OVER_PROGRAM ||
        operation == LT48_OPERATION_VERIFY ||
        operation == LT48_OPERATION_COMPARE;
    use_buffer = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->verify_use_buffer_check));
    if (uses_image && !use_buffer &&
        gtk_editable_get_text(GTK_EDITABLE(
            widgets->verify_image_entry))[0] == '\0') {
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Choose the input image; then press the operation button to run");
        browse_programming_image(NULL, widgets);
        return;
    }
    prepare_read(button, widgets);
}

static void buffer_show_range(
    app_widgets *widgets, const char *action, uint32_t start, uint32_t length
);

static void digest_hex(
    const unsigned char *data, size_t size, char *out_text
) {
    static const char digits[] = "0123456789abcdef";
    for (size_t index = 0u; index < size; ++index) {
        out_text[index * 2u] = digits[data[index] >> 4];
        out_text[index * 2u + 1u] = digits[data[index] & 0x0fu];
    }
    out_text[size * 2u] = '\0';
}

static int parse_digest_hex(
    const char *text, unsigned char digest[LT48_SHA256_SIZE]
) {
    if (text == NULL || strlen(text) != LT48_SHA256_SIZE * 2u) return 0;
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        int high = g_ascii_xdigit_value(text[index * 2u]);
        int low = g_ascii_xdigit_value(text[index * 2u + 1u]);
        if (high < 0 || low < 0) return 0;
        digest[index] = (unsigned char)((high << 4) | low);
    }
    return 1;
}

static int entry_u32(GtkWidget *entry, uint32_t maximum, uint32_t *out_value) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    char *end = NULL;
    guint64 value;
    if (text == NULL || text[0] == '\0') return 0;
    value = g_ascii_strtoull(text, &end, 0);
    if (end == NULL || *end != '\0' || value > maximum) return 0;
    *out_value = (uint32_t)value;
    return 1;
}

static int entry_u64(GtkWidget *entry, uint64_t *out_value) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    char *end = NULL;
    guint64 value;
    if (text == NULL || text[0] == '\0' || text[0] == '-') return 0;
    errno = 0;
    value = g_ascii_strtoull(text, &end, 0);
    if (errno == ERANGE || end == NULL || *end != '\0') return 0;
    *out_value = value;
    return 1;
}

static unsigned char *hex_bytes(const char *text, size_t *out_size) {
    size_t length;
    unsigned char *bytes;
    if (text == NULL || out_size == NULL) return NULL;
    length = strlen(text);
    if (length == 0u || length % 2u != 0u) return NULL;
    bytes = g_malloc(length / 2u);
    for (size_t index = 0u; index < length / 2u; ++index) {
        int high = g_ascii_xdigit_value(text[index * 2u]);
        int low = g_ascii_xdigit_value(text[index * 2u + 1u]);
        if (high < 0 || low < 0) {
            g_free(bytes);
            return NULL;
        }
        bytes[index] = (unsigned char)((unsigned)high << 4u | (unsigned)low);
    }
    *out_size = length / 2u;
    return bytes;
}

static int contact_list(
    const char *text, uint8_t contacts[8], size_t *out_count
) {
    gchar **parts;
    size_t count = 0u;
    if (text == NULL || text[0] == '\0') return 0;
    parts = g_strsplit(text, ",", 9);
    while (parts[count] != NULL) {
        char *end = NULL;
        guint64 value;
        if (count == 8u || parts[count][0] == '\0' || parts[count][0] == '-') {
            g_strfreev(parts);
            return 0;
        }
        value = g_ascii_strtoull(parts[count], &end, 0);
        if (end == NULL || *end != '\0' || value == 0u || value > 48u) {
            g_strfreev(parts);
            return 0;
        }
        contacts[count] = (uint8_t)value;
        ++count;
    }
    g_strfreev(parts);
    if (count < 6u) return 0;
    *out_count = count;
    return 1;
}

static int optional_entry_u16(
    GtkWidget *entry, int *out_present, uint16_t *out_value
) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    char *end = NULL;
    guint64 value;
    if (out_present == NULL || out_value == NULL) return 0;
    *out_present = 0;
    *out_value = 0u;
    if (text == NULL || text[0] == '\0') return 1;
    value = g_ascii_strtoull(text, &end, 0);
    if (end == NULL || *end != '\0' || value > UINT16_MAX) return 0;
    *out_present = 1;
    *out_value = (uint16_t)value;
    return 1;
}

static void prepared_read_clear(prepared_read *request) {
    if (request == NULL) return;
    lt48_protocol_plan_close(request->plan);
    lt48_m37_resources_close(request->resources);
    g_free(request->selector);
    g_free(request->output);
    g_free(request->report);
    g_free(request->installation_path);
    memset(request, 0, sizeof(*request));
}

static lt48_status build_prepared_read(
    lt48_installation *installation, prepared_read *request
) {
    lt48_catalogue_entry entry;
    lt48_device_spec spec;
    char *device_name = NULL;
    lt48_status status;
    if (installation == NULL || request == NULL ||
        request->output == NULL || request->installation_path == NULL)
        return LT48_INVALID_ARGUMENT;
    status = lt48_installation_get_device(
        installation, request->device_index, &entry);
    if (status == LT48_OK)
        status = lt48_installation_resolve_device(
            installation, request->device_index, &spec);
    if (status == LT48_OK) {
        request->capacity = spec.capacity_bytes;
        device_name = entry.device_name == NULL ? g_strdup("") : g_strndup(
            (const char *)entry.device_name, entry.device_name_length);
        request->selector = g_strdup_printf("%s:%s", entry.vendor, device_name);
        status = lt48_m37_resources_open(
            installation, request->device_index, &request->resources);
    }
    if (status == LT48_OK)
        status = lt48_m37_resources_build_read_plan(
            request->resources, request->start, request->length,
            &request->plan);
    if (status == LT48_OK)
        status = lt48_m37_prepare_read(
            request->resources, request->plan, request->selector,
            request->output, request->bus, request->address, request->start,
            request->length, request->timeout, &request->preparation);
    g_free(device_name);
    return status;
}

static lt48_status prepare_selected_read(
    app_widgets *widgets, prepared_read *request
) {
    uint32_t bus;
    uint32_t address;
    const char *output;
    const char *report;
    lt48_status status;
    if (widgets == NULL || request == NULL) return LT48_INVALID_ARGUMENT;
    memset(request, 0, sizeof(*request));
    if (widgets->installation == NULL || !widgets->has_selected_device) {
        return LT48_NOT_FOUND;
    }
    output = gtk_editable_get_text(GTK_EDITABLE(widgets->read_output_entry));
    report = gtk_editable_get_text(
        GTK_EDITABLE(widgets->execution_report_entry)
    );
    if (!entry_u32(widgets->read_start_entry, UINT32_MAX, &request->start) ||
        !entry_u32(widgets->read_length_entry, UINT32_MAX, &request->length) ||
        !entry_u32(widgets->read_bus_entry, UINT8_MAX, &bus) ||
        !entry_u32(widgets->read_address_entry, UINT8_MAX, &address) ||
        !entry_u32(widgets->read_timeout_entry, UINT32_MAX,
                   &request->timeout) ||
        request->length == 0u || request->timeout == 0u || output == NULL ||
        output[0] == '\0') {
        return LT48_INVALID_ARGUMENT;
    }
    request->bus = (uint8_t)bus;
    request->address = (uint8_t)address;
    request->output = g_strdup(output);
    request->report = g_strdup(report == NULL ? "" : report);
    request->installation_path = g_strdup(
        lt48_installation_path(widgets->installation)
    );
    request->device_index = widgets->selected_device_index;
    status = build_prepared_read(widgets->installation, request);
    if (status != LT48_OK) prepared_read_clear(request);
    return status;
}

static int confirmation_matches(
    const char *text, const unsigned char digest[LT48_SHA256_SIZE]
) {
    char expected[LT48_SHA256_SIZE * 2u + 1u];
    unsigned difference = 0u;
    if (text == NULL || strlen(text) != LT48_SHA256_SIZE * 2u) return 0;
    digest_hex(digest, LT48_SHA256_SIZE, expected);
    for (size_t index = 0u; index < LT48_SHA256_SIZE * 2u; ++index) {
        difference |= (unsigned)(g_ascii_tolower(text[index]) - expected[index]);
    }
    return difference == 0u;
}

static void clear_list(GtkWidget *list) {
    GtkWidget *child = gtk_widget_get_first_child(list);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

static GtkWidget *left_label(const char *text) {
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD_CHAR);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_margin_top(label, 6);
    gtk_widget_set_margin_bottom(label, 6);
    gtk_widget_set_margin_start(label, 8);
    gtk_widget_set_margin_end(label, 8);
    return label;
}

static void append_vendor_once(
    GtkWidget *vendor_list, GHashTable *vendors, const char *vendor
) {
    GtkWidget *row;
    if (vendor == NULL || vendor[0] == '\0' ||
        g_hash_table_contains(vendors, vendor))
        return;
    g_hash_table_add(vendors, g_strdup(vendor));
    row = gtk_list_box_row_new();
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), left_label(vendor));
    g_object_set_data_full(
        G_OBJECT(row), "labtool48-vendor", g_strdup(vendor), g_free);
    gtk_list_box_append(GTK_LIST_BOX(vendor_list), row);
}

static void vendor_selected(
    GtkListBox *box, GtkListBoxRow *row, gpointer user_data
) {
    app_widgets *widgets = user_data;
    const char *selected_vendor;
    (void)box;
    if (row == NULL) return;
    selected_vendor = g_object_get_data(
        G_OBJECT(row), "labtool48-vendor");
    clear_list(widgets->device_list);
    if (widgets->installation == NULL ||
        widgets->device_result_indices == NULL)
        return;
    for (guint item = 0u; item < widgets->device_result_indices->len; ++item) {
        gsize device_index = g_array_index(
            widgets->device_result_indices, gsize, item);
        lt48_catalogue_entry entry;
        GtkWidget *device_row;
        char *name;
        if (lt48_installation_get_device(
                widgets->installation, device_index, &entry) != LT48_OK ||
            g_strcmp0(entry.vendor, selected_vendor) != 0)
            continue;
        name = entry.device_name == NULL ? g_strdup("") :
            g_strndup((const char *)entry.device_name,
                      entry.device_name_length);
        device_row = gtk_list_box_row_new();
        gtk_list_box_row_set_child(
            GTK_LIST_BOX_ROW(device_row), left_label(name));
        g_object_set_data(
            G_OBJECT(device_row), "labtool48-device-index",
            GSIZE_TO_POINTER(device_index + 1u));
        gtk_list_box_append(GTK_LIST_BOX(widgets->device_list), device_row);
        g_free(name);
    }
    {
        GtkListBoxRow *first = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(widgets->device_list), 0);
        if (first != NULL)
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->device_list), first);
    }
}

static const auto_id_candidate_evidence *auto_id_evidence_for_device(
    const app_widgets *widgets, size_t device_index
) {
    if (widgets->auto_id_candidate_evidence == NULL) return NULL;
    for (guint index = 0u;
         index < widgets->auto_id_candidate_evidence->len; ++index) {
        const auto_id_candidate_evidence *evidence = &g_array_index(
            widgets->auto_id_candidate_evidence,
            auto_id_candidate_evidence, index);
        if (evidence->device_index == device_index) return evidence;
    }
    return NULL;
}

static GtkWidget *auto_id_candidate_content(
    const char *name, const auto_id_candidate_evidence *evidence
) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *title = left_label(name);
    GString *description = g_string_new(NULL);
    GtkWidget *details;
    if (evidence == NULL) {
        g_string_append(description, "Ranking evidence unavailable");
    } else {
        g_string_append_printf(
            description, "Rank %zu · score %u · matched ",
            evidence->rank, evidence->score);
        if (evidence->manufacturer_id_match)
            g_string_append(description, "manufacturer ID, ");
        if (evidence->device_id_match)
            g_string_append(description, "device ID, ");
        if (evidence->pin_count_match)
            g_string_append_printf(
                description, "%u-pin package, ",
                evidence->resolved_pin_count);
        if (evidence->device_class_match)
            g_string_append(description, "device class, ");
        if (g_str_has_suffix(description->str, ", "))
            g_string_truncate(description, description->len - 2u);
        else
            g_string_append(description, "catalogue identity only");
    }
    details = gtk_label_new(description->str);
    gtk_label_set_xalign(GTK_LABEL(details), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(details), TRUE);
    gtk_widget_add_css_class(details, "dim-label");
    gtk_box_append(GTK_BOX(box), title);
    gtk_box_append(GTK_BOX(box), details);
    g_string_free(description, TRUE);
    return box;
}

static void auto_id_vendor_selected(
    GtkListBox *box, GtkListBoxRow *row, gpointer user_data
) {
    app_widgets *widgets = user_data;
    const char *selected_vendor;
    (void)box;
    gtk_widget_set_sensitive(widgets->auto_id_accept_button, FALSE);
    clear_list(widgets->auto_id_list);
    if (row == NULL || widgets->installation == NULL ||
        widgets->device_result_indices == NULL)
        return;
    selected_vendor = g_object_get_data(
        G_OBJECT(row), "labtool48-vendor");
    for (guint item = 0u; item < widgets->device_result_indices->len; ++item) {
        gsize device_index = g_array_index(
            widgets->device_result_indices, gsize, item);
        lt48_catalogue_entry entry;
        const auto_id_candidate_evidence *evidence;
        GtkWidget *device_row;
        char *name;
        if (lt48_installation_get_device(
                widgets->installation, device_index, &entry) != LT48_OK ||
            g_strcmp0(entry.vendor, selected_vendor) != 0)
            continue;
        name = entry.device_name == NULL ? g_strdup("") :
            g_strndup((const char *)entry.device_name,
                      entry.device_name_length);
        evidence = auto_id_evidence_for_device(widgets, device_index);
        device_row = gtk_list_box_row_new();
        gtk_list_box_row_set_child(
            GTK_LIST_BOX_ROW(device_row),
            auto_id_candidate_content(name, evidence));
        g_object_set_data(
            G_OBJECT(device_row), "labtool48-device-index",
            GSIZE_TO_POINTER(device_index + 1u));
        gtk_list_box_append(GTK_LIST_BOX(widgets->auto_id_list), device_row);
        g_free(name);
    }
    {
        GtkListBoxRow *first = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(widgets->auto_id_list), 0);
        if (first != NULL)
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->auto_id_list), first);
    }
}

static char *project_path(const char *relative_path) {
    char *executable = g_file_read_link("/proc/self/exe", NULL);
    char *build_directory;
    char *project_directory;
    char *candidate;
    if (executable == NULL) return NULL;
    build_directory = g_path_get_dirname(executable);
    project_directory = g_path_get_dirname(build_directory);
    candidate = g_build_filename(
        project_directory, relative_path, NULL);
    g_free(project_directory);
    g_free(build_directory);
    g_free(executable);
    if (!g_file_test(candidate, G_FILE_TEST_EXISTS)) {
        g_free(candidate);
        return NULL;
    }
    return candidate;
}

static const char *operation_availability_label(
    lt48_operation_availability availability
) {
    switch (availability) {
    case LT48_OPERATION_PLAN_ONLY: return "plan-only";
    case LT48_OPERATION_EXECUTABLE: return "executable";
    case LT48_OPERATION_HOST_ONLY: return "host-only";
    default: return "unavailable";
    }
}

static const char *device_type_category_label(uint16_t category) {
    static const char *const labels[] = {"EPROM", "PROM", "PLD", "MPU"};
    return category < G_N_ELEMENTS(labels) ? labels[category] : "Unknown";
}

static lt48_status exact_cold_programmer(
    lt48_context *context, uint8_t bus, uint8_t address
) {
    lt48_programmer_info *items = NULL;
    size_t count = 0u;
    lt48_status status;
    int found = 0;
    if (context == NULL) return LT48_INVALID_ARGUMENT;
    status = lt48_programmer_list(context, NULL, 0u, &count);
    if (status != LT48_OK && status != LT48_BUFFER_TOO_SMALL) return status;
    if (count != 0u) {
        items = g_new0(lt48_programmer_info, count);
        status = lt48_programmer_list(context, items, count, &count);
    }
    for (size_t index = 0u; status == LT48_OK && index < count; ++index) {
        if (items[index].bus_number == bus &&
            items[index].device_address == address &&
            items[index].vendor_id == LT48_USB_VENDOR_ID &&
            items[index].product_id == LT48_USB_PRODUCT_ID_COLD) {
            found = 1;
            break;
        }
    }
    g_free(items);
    return status != LT48_OK ? status : found ? LT48_OK : LT48_NOT_FOUND;
}

static int programmer_target(
    app_widgets *widgets, uint8_t *out_bus, uint8_t *out_address
) {
    if (!widgets->has_selected_programmer) return 0;
    *out_bus = widgets->selected_programmer_bus;
    *out_address = widgets->selected_programmer_address;
    return 1;
}

static void set_programmer_target(
    app_widgets *widgets, uint8_t bus, uint8_t address, uint16_t product_id
) {
    char bus_text[8];
    char address_text[8];
    char *status;
    int changed = !widgets->has_selected_programmer ||
        widgets->selected_programmer_bus != bus ||
        widgets->selected_programmer_address != address ||
        widgets->selected_programmer_product_id != product_id;
    widgets->selected_programmer_bus = bus;
    widgets->selected_programmer_address = address;
    widgets->selected_programmer_product_id = product_id;
    widgets->has_selected_programmer = 1;
    if (changed) {
        widgets->programmer_ready = 0;
        widgets->automatic_fx2_attempted = 0;
    }
    g_snprintf(bus_text, sizeof(bus_text), "%u", bus);
    g_snprintf(address_text, sizeof(address_text), "%u", address);
    if (widgets->programmer_bus_entry != NULL)
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->programmer_bus_entry), bus_text);
    if (widgets->programmer_address_entry != NULL)
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->programmer_address_entry), address_text);
    if (widgets->read_bus_entry != NULL)
        gtk_editable_set_text(GTK_EDITABLE(widgets->read_bus_entry), bus_text);
    if (widgets->read_address_entry != NULL)
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->read_address_entry), address_text);
    status = g_strdup("Programmer found; initializing…");
    if (widgets->selected_programmer_status != NULL)
        gtk_label_set_text(
            GTK_LABEL(widgets->selected_programmer_status), status);
    if (widgets->machine_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->machine_status), status);
    if (widgets->footer_programmer != NULL) {
        char *footer = g_strdup_printf(
            "USB %u:%u — initializing", bus, address);
        gtk_label_set_text(GTK_LABEL(widgets->footer_programmer), footer);
        g_free(footer);
    }
    if (changed && widgets->read_preparation != NULL &&
        widgets->read_cancellable == NULL)
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Detected programmer inherited here; select a device, then "
            "choose Read");
    g_free(status);
}

static void clear_programmer_target(app_widgets *widgets) {
    widgets->has_selected_programmer = 0;
    widgets->programmer_ready = 0;
    widgets->automatic_fx2_attempted = 0;
    widgets->selected_programmer_product_id = 0u;
    if (widgets->programmer_bus_entry != NULL)
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->programmer_bus_entry), "");
    if (widgets->programmer_address_entry != NULL)
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->programmer_address_entry), "");
    if (widgets->read_bus_entry != NULL)
        gtk_editable_set_text(GTK_EDITABLE(widgets->read_bus_entry), "");
    if (widgets->read_address_entry != NULL)
        gtk_editable_set_text(GTK_EDITABLE(widgets->read_address_entry), "");
    if (widgets->selected_programmer_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->selected_programmer_status),
                           "Programmer: not detected");
    if (widgets->machine_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->machine_status),
                           "No supported programmer found");
    if (widgets->footer_programmer != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_programmer), "Offline");
    if (widgets->read_preparation != NULL && widgets->read_cancellable == NULL)
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "No programmer detected; connect it and retry the operation. "
            "Discovery and initialization run automatically; manual "
            "selection is under Options → Search Machine.");
}

static void programmer_selected(
    GtkListBox *box, GtkListBoxRow *row, gpointer user_data
) {
    app_widgets *widgets = user_data;
    guint packed;
    uint16_t product_id;
    (void)box;
    if (row == NULL) return;
    packed = GPOINTER_TO_UINT(g_object_get_data(
        G_OBJECT(row), "labtool48-programmer-target"
    ));
    if (packed == 0u) return;
    --packed;
    product_id = (uint16_t)GPOINTER_TO_UINT(g_object_get_data(
        G_OBJECT(row), "labtool48-programmer-product-id"));
    set_programmer_target(
        widgets, (uint8_t)(packed >> 8), (uint8_t)(packed & 0xffu),
        product_id);
    gtk_widget_set_sensitive(widgets->programmer_fx2_execute_button, FALSE);
    if (widgets->machine_list != NULL &&
        GTK_WIDGET(box) == widgets->machine_list &&
        !widgets->programmer_initializing)
        programmer_initialize(NULL, widgets);
}

static void programmer_init_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    programmer_init_task *request = task_data;
    programmer_init_result *result = g_new0(programmer_init_result, 1);
    lt48_context *context = NULL;
    lt48_programmer *programmer = NULL;
    (void)source_object;
    (void)cancellable;
    result->status = lt48_context_create(&context);
    if (result->status == LT48_OK)
        result->status = lt48_programmer_open(
            context, request->bus, request->address, &programmer
        );
    if (result->status == LT48_OK)
        result->status = lt48_programmer_transport_initialize(
            programmer, request->timeout_ms, &result->transport
        );
    lt48_programmer_close(programmer);
    lt48_context_destroy(context);
    g_task_return_pointer(task, result, g_free);
}

static void programmer_init_finished(
    GObject *source, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    programmer_init_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL
    );
    char *message;
    (void)source;
    widgets->programmer_initializing = 0;
    gtk_widget_set_sensitive(widgets->programmer_initialize_button, TRUE);
    if (result == NULL) {
        widgets->pending_hardware_action = PENDING_HARDWARE_NONE;
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
                           "Transport readiness did not return a result");
        return;
    }
    if (result->status == LT48_OK) {
        int pending = widgets->pending_hardware_action;
        widgets->pending_hardware_action = PENDING_HARDWARE_NONE;
        widgets->programmer_ready = 1;
        message = g_strdup_printf(
            "Programmer ready (transport acknowledgements %02x/%02x).",
            result->transport.packet_size_ack,
            result->transport.mode_ack
        );
        if (widgets->selected_programmer_status != NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->selected_programmer_status),
                "Programmer ready");
        if (widgets->machine_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->machine_status),
                               "Programmer ready");
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status), message);
        g_free(message);
        g_free(result);
        if (pending == PENDING_HARDWARE_PREPARE_READ)
            prepare_read(NULL, widgets);
        else if (pending == PENDING_HARDWARE_EXECUTE)
            execute_read(NULL, widgets);
        else if (pending == PENDING_HARDWARE_AUTO_ID)
            plan_eprom_auto_id(NULL, widgets);
        return;
    } else if (widgets->selected_programmer_product_id ==
                   LT48_USB_PRODUCT_ID_COLD &&
               !widgets->automatic_fx2_attempted) {
        g_free(result);
        fx2_start_automatic(widgets);
        return;
    } else {
        int had_pending =
            widgets->pending_hardware_action != PENDING_HARDWARE_NONE;
        widgets->pending_hardware_action = PENDING_HARDWARE_NONE;
        widgets->programmer_ready = 0;
        message = g_strdup_printf(
            "Programmer initialization failed: %s.",
            lt48_status_string(result->status)
        );
        if (widgets->selected_programmer_status != NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->selected_programmer_status), message);
        if (widgets->machine_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->machine_status), message);
        if (had_pending && widgets->read_preparation != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
    }
    gtk_label_set_text(GTK_LABEL(widgets->programmer_status), message);
    g_free(message);
    g_free(result);
}

static void programmer_initialize(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    programmer_init_task *request;
    GTask *task;
    uint8_t bus;
    uint8_t address;
    uint32_t timeout;
    (void)button;
    if (widgets->programmer_initializing) return;
    if (!programmer_target(widgets, &bus, &address) ||
        !entry_u32(widgets->programmer_timeout_entry, UINT32_MAX, &timeout) ||
        timeout == 0u) {
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
            "Select an exact target and enter a nonzero timeout");
        return;
    }
    request = g_new0(programmer_init_task, 1);
    request->bus = bus;
    request->address = address;
    request->timeout_ms = timeout;
    task = tracked_task_new(widgets, NULL, programmer_init_finished);
    g_task_set_task_data(task, request, g_free);
    gtk_widget_set_sensitive(widgets->programmer_initialize_button, FALSE);
    widgets->programmer_ready = 0;
    widgets->programmer_initializing = 1;
    gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
        "Claiming the exact interface and checking transport readiness…");
    g_task_run_in_thread(task, programmer_init_worker);
    g_object_unref(task);
}

static void fx2_load_task_free(gpointer data) {
    fx2_load_task *task = data;
    if (task == NULL) return;
    g_free(task->firmware_path);
    g_free(task->confirmation);
    g_free(task);
}

static void fx2_prepare(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->programmer_fx2_path_entry)
    );
    lt48_fx2_firmware *firmware = NULL;
    lt48_fx2_firmware_info info;
    lt48_fx2_load_preparation preparation;
    lt48_context *context = NULL;
    uint8_t bus;
    uint8_t address;
    char file_digest[LT48_SHA256_SIZE * 2u + 1u];
    char image_digest[LT48_SHA256_SIZE * 2u + 1u];
    char confirmation[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    lt48_status status;
    (void)button;
    gtk_widget_set_sensitive(widgets->programmer_fx2_execute_button, FALSE);
    if (!programmer_target(widgets, &bus, &address) ||
        path == NULL || path[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
            "Select an exact programmer target and owner FX2 HEX file");
        return;
    }
    status = lt48_fx2_firmware_open_hex(path, &firmware);
    if (status == LT48_OK)
        status = lt48_fx2_firmware_info_get(firmware, &info);
    if (status == LT48_OK)
        status = lt48_fx2_prepare_load(firmware, bus, address, &preparation);
    if (status == LT48_OK) status = lt48_context_create(&context);
    if (status == LT48_OK)
        status = exact_cold_programmer(context, bus, address);
    if (status != LT48_OK) {
        message = g_strdup_printf(
            "FX2 preparation rejected: %s. The target must currently have "
            "the cold descriptor; no USB interface was opened.",
            lt48_status_string(status)
        );
    } else {
        digest_hex(info.file_sha256, LT48_SHA256_SIZE, file_digest);
        digest_hex(info.image_sha256, LT48_SHA256_SIZE, image_digest);
        digest_hex(preparation.confirmation_sha256,
                   LT48_SHA256_SIZE, confirmation);
        message = g_strdup_printf(
            "OFFLINE FX2 LOAD PLAN — %03u:%03u\n"
            "%zu records · %zu payload bytes · %zu-byte sparse image\n"
            "File SHA-256: %s\nImage SHA-256: %s\nConfirmation: %s\n"
            "No USB interface was opened. Paste the confirmation exactly, "
            "then press Execute volatile FX2 load.",
            bus, address, info.record_count, info.payload_size,
            info.image_size, file_digest, image_digest, confirmation
        );
        gtk_widget_set_sensitive(
            widgets->programmer_fx2_execute_button, TRUE
        );
    }
    gtk_label_set_text(GTK_LABEL(widgets->programmer_status), message);
    g_free(message);
    lt48_context_destroy(context);
    lt48_fx2_firmware_close(firmware);
}

static void fx2_load_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    fx2_load_task *request = task_data;
    fx2_load_task_result *result = g_new0(fx2_load_task_result, 1);
    lt48_fx2_firmware *firmware = NULL;
    lt48_context *context = NULL;
    (void)source_object;
    (void)cancellable;
    result->status = lt48_fx2_firmware_open_hex(
        request->firmware_path, &firmware
    );
    if (result->status == LT48_OK)
        result->status = lt48_fx2_prepare_load(
            firmware, request->bus, request->address, &result->preparation
        );
    if (result->status == LT48_OK && request->confirmation != NULL &&
        !confirmation_matches(
            request->confirmation,
            result->preparation.confirmation_sha256))
        result->status = LT48_SAFETY_REJECTED;
    if (result->status == LT48_OK)
        result->status = lt48_context_create(&context);
    if (result->status == LT48_OK)
        result->status = exact_cold_programmer(
            context, request->bus, request->address
        );
    if (result->status == LT48_OK)
        result->status = lt48_programmer_load_fx2(
            context, request->bus, request->address, firmware,
            LT48_FX2_LOAD_HARDWARE_AUTHORIZATION, &result->load
        );
    if (result->status == LT48_OK) g_usleep(750000u);
    lt48_context_destroy(context);
    lt48_fx2_firmware_close(firmware);
    g_task_return_pointer(task, result, g_free);
}

static void fx2_load_finished(
    GObject *source, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    fx2_load_task_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL
    );
    char *message;
    (void)source;
    widgets->programmer_initializing = 0;
    gtk_widget_set_sensitive(widgets->programmer_fx2_prepare_button, TRUE);
    gtk_widget_set_sensitive(widgets->programmer_fx2_execute_button, FALSE);
    if (result == NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
                           "FX2 load did not return a result");
        return;
    }
    if (result->status == LT48_OK) {
        message = g_strdup("Programmer firmware loaded; initializing…");
    } else if (result->load.cpu_halted && !result->load.cpu_released) {
        message = g_strdup_printf(
            "FX2 load FAILED: %s after %zu records/%zu bytes. The FX2 CPU "
            "remains halted; power-cycle the programmer before retrying.",
            lt48_status_string(result->status), result->load.records_written,
            result->load.bytes_written
        );
    } else {
        message = g_strdup_printf(
            "FX2 load rejected or failed: %s; records %zu, bytes %zu, CPU "
            "released %s.", lt48_status_string(result->status),
            result->load.records_written, result->load.bytes_written,
            result->load.cpu_released ? "yes" : "no"
        );
    }
    gtk_label_set_text(GTK_LABEL(widgets->programmer_status), message);
    g_free(message);
    if (result->status == LT48_OK) {
        g_free(result);
        refresh_programmers(NULL, widgets);
        return;
    }
    g_free(result);
}

static void fx2_start_automatic(app_widgets *widgets) {
    fx2_load_task *request;
    GTask *task;
    uint8_t bus;
    uint8_t address;
    const char *path;
    if (widgets->programmer_initializing ||
        !programmer_target(widgets, &bus, &address)) return;
    path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->programmer_fx2_path_entry));
    if (path == NULL || path[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
                           "Programmer firmware image was not found");
        return;
    }
    widgets->automatic_fx2_attempted = 1;
    widgets->programmer_initializing = 1;
    request = g_new0(fx2_load_task, 1);
    request->firmware_path = g_strdup(path);
    request->bus = bus;
    request->address = address;
    task = tracked_task_new(widgets, NULL, fx2_load_finished);
    g_task_set_task_data(task, request, fx2_load_task_free);
    gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
                       "Loading programmer firmware…");
    if (widgets->selected_programmer_status != NULL)
        gtk_label_set_text(
            GTK_LABEL(widgets->selected_programmer_status),
            "Programmer found; loading firmware…");
    g_task_run_in_thread(task, fx2_load_worker);
    g_object_unref(task);
}

static void fx2_execute(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->programmer_fx2_path_entry)
    );
    const char *confirmation = gtk_editable_get_text(
        GTK_EDITABLE(widgets->programmer_fx2_confirmation_entry)
    );
    fx2_load_task *request;
    GTask *task;
    uint8_t bus;
    uint8_t address;
    (void)button;
    if (!programmer_target(widgets, &bus, &address) || path == NULL ||
        path[0] == '\0' || confirmation == NULL ||
        strlen(confirmation) != LT48_SHA256_SIZE * 2u) {
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
            "Enter the exact target, owner firmware, and 64-character "
            "confirmation from the current offline plan");
        return;
    }
    request = g_new0(fx2_load_task, 1);
    request->firmware_path = g_strdup(path);
    request->confirmation = g_strdup(confirmation);
    request->bus = bus;
    request->address = address;
    task = tracked_task_new(widgets, NULL, fx2_load_finished);
    g_task_set_task_data(task, request, fx2_load_task_free);
    gtk_widget_set_sensitive(widgets->programmer_fx2_prepare_button, FALSE);
    gtk_widget_set_sensitive(widgets->programmer_fx2_execute_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
        "Loading volatile FX2 firmware… Do not disconnect or power-cycle "
        "unless a failure message explicitly requires it.");
    g_task_run_in_thread(task, fx2_load_worker);
    g_object_unref(task);
}

static GtkWidget *programmer_list_row(const lt48_programmer_info *item) {
    char text[160];
    GtkWidget *row = gtk_list_box_row_new();
    guint packed = ((guint)item->bus_number << 8) | item->device_address;
    g_snprintf(text, sizeof(text),
               "%03u:%03u   %04x:%04x   USB %x.%02x   %s",
               item->bus_number, item->device_address,
               item->vendor_id, item->product_id,
               item->usb_version_bcd >> 8,
               item->usb_version_bcd & 0xff,
               lt48_programmer_descriptor_role(item->product_id));
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), left_label(text));
    g_object_set_data(G_OBJECT(row), "labtool48-programmer-target",
                      GUINT_TO_POINTER(packed + 1u));
    g_object_set_data(G_OBJECT(row), "labtool48-programmer-product-id",
                      GUINT_TO_POINTER((guint)item->product_id));
    return row;
}

static void refresh_programmers(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    lt48_context *context = NULL;
    lt48_programmer_info *items = NULL;
    GtkListBoxRow *target_row = NULL;
    GtkListBoxRow *machine_target_row = NULL;
    lt48_status status;
    size_t count = 0;
    (void)button;
    clear_list(widgets->programmer_list);
    if (widgets->machine_list != NULL) clear_list(widgets->machine_list);
    status = lt48_context_create(&context);
    if (status == LT48_OK) {
        status = lt48_programmer_list(context, NULL, 0, &count);
    }
    if ((status == LT48_OK || status == LT48_BUFFER_TOO_SMALL) && count != 0) {
        items = g_new0(lt48_programmer_info, count);
        status = lt48_programmer_list(context, items, count, &count);
    }
    if (status != LT48_OK) {
        if (widgets->pending_hardware_action == PENDING_HARDWARE_AUTO_ID) {
            gtk_label_set_text(GTK_LABEL(widgets->device_status),
                               "Programmer probe failed");
            if (widgets->auto_id_status != NULL)
                gtk_label_set_text(GTK_LABEL(widgets->auto_id_status),
                                   "Programmer probe failed");
        }
        if (widgets->pending_hardware_action != PENDING_HARDWARE_NONE &&
            widgets->read_preparation != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                               "Programmer probe failed");
        widgets->pending_hardware_action = PENDING_HARDWARE_NONE;
        clear_programmer_target(widgets);
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
                           lt48_status_string(status));
        if (widgets->machine_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->machine_status),
                               lt48_status_string(status));
    } else if (count == 0) {
        if (widgets->pending_hardware_action == PENDING_HARDWARE_AUTO_ID) {
            gtk_label_set_text(GTK_LABEL(widgets->device_status),
                               "No supported programmer is online");
            if (widgets->auto_id_status != NULL)
                gtk_label_set_text(GTK_LABEL(widgets->auto_id_status),
                                   "No supported programmer is online");
        }
        if (widgets->pending_hardware_action != PENDING_HARDWARE_NONE &&
            widgets->read_preparation != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                               "No supported programmer is online");
        widgets->pending_hardware_action = PENDING_HARDWARE_NONE;
        clear_programmer_target(widgets);
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status),
                           "No supported programmer found");
        if (widgets->machine_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->machine_status),
                               "No supported programmer found");
    } else {
        char status_text[128];
        g_snprintf(status_text, sizeof(status_text),
                   "%zu programmer%s found (descriptor-only; USB not opened)",
                   count, count == 1 ? "" : "s");
        gtk_label_set_text(GTK_LABEL(widgets->programmer_status), status_text);
        if (widgets->machine_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->machine_status), status_text);
        for (size_t index = 0; index < count; ++index) {
            GtkWidget *row = programmer_list_row(&items[index]);
            GtkWidget *machine_row = widgets->machine_list == NULL ? NULL :
                programmer_list_row(&items[index]);
            gtk_list_box_append(GTK_LIST_BOX(widgets->programmer_list), row);
            if (machine_row != NULL)
                gtk_list_box_append(
                    GTK_LIST_BOX(widgets->machine_list), machine_row);
            if (count == 1u ||
                (widgets->has_selected_programmer &&
                 widgets->selected_programmer_bus == items[index].bus_number &&
                 widgets->selected_programmer_address ==
                    items[index].device_address))
                target_row = GTK_LIST_BOX_ROW(row);
            if (target_row == GTK_LIST_BOX_ROW(row) && machine_row != NULL)
                machine_target_row = GTK_LIST_BOX_ROW(machine_row);
        }
        if (target_row != NULL)
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->programmer_list), target_row);
        if (machine_target_row != NULL)
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->machine_list), machine_target_row);
        else
            clear_programmer_target(widgets);
        if (target_row != NULL && !widgets->programmer_initializing)
            programmer_initialize(NULL, widgets);
    }
    g_free(items);
    lt48_context_destroy(context);
}

static void catalogue_task_free(gpointer data) {
    catalogue_task *request = data;
    if (request == NULL) return;
    g_free(request->path);
    g_free(request->query);
    g_free(request);
}

static void catalogue_task_result_free(gpointer data) {
    catalogue_task_result *result = data;
    if (result == NULL) return;
    lt48_installation_close(result->installation);
    if (result->matching_indices != NULL)
        g_array_unref(result->matching_indices);
    g_free(result->candidates);
    g_free(result);
}

static void catalogue_set_busy(app_widgets *widgets, gboolean busy) {
    gtk_widget_set_sensitive(widgets->catalogue_verify_button, !busy);
    gtk_widget_set_sensitive(widgets->catalogue_search_button, !busy);
    gtk_widget_set_sensitive(widgets->catalogue_rank_button, !busy);
    gtk_widget_set_sensitive(widgets->cache_import_button, !busy);
    gtk_widget_set_sensitive(widgets->installation_entry, !busy);
    gtk_widget_set_sensitive(widgets->database_dropdown, !busy);
    gtk_widget_set_sensitive(widgets->database_manage_button, !busy);
    gtk_widget_set_sensitive(widgets->query_entry, !busy);
    for (size_t index = 0u;
         index < G_N_ELEMENTS(widgets->device_type_filters); ++index)
        gtk_widget_set_sensitive(
            widgets->device_type_filters[index], !busy);
    gtk_widget_set_sensitive(widgets->auto_manufacturer_entry, !busy);
    gtk_widget_set_sensitive(widgets->auto_device_entry, !busy);
    gtk_widget_set_sensitive(widgets->auto_pins_entry, !busy);
    gtk_widget_set_sensitive(widgets->auto_class_entry, !busy);
    gtk_widget_set_sensitive(widgets->catalogue_cancel_button, busy);
    gtk_widget_set_visible(widgets->catalogue_cancel_button, busy);
}

static void catalogue_task_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    catalogue_task *request = task_data;
    catalogue_task_result *result = g_new0(catalogue_task_result, 1);
    (void)source_object;
    result->kind = request->kind;
    if (g_cancellable_is_cancelled(cancellable)) {
        result->status = LT48_CANCELLED;
        g_task_return_pointer(task, result, catalogue_task_result_free);
        return;
    }
    result->status = lt48_installation_open(
        request->path, &result->installation);
    if (result->status == LT48_OK &&
        g_cancellable_is_cancelled(cancellable))
        result->status = LT48_CANCELLED;
    if (result->status == LT48_OK && request->kind == CATALOGUE_TASK_SEARCH) {
        char *folded_query = g_ascii_strdown(request->query, -1);
        result->matching_indices = g_array_new(FALSE, FALSE, sizeof(gsize));
        for (size_t index = 0u;
             index < lt48_installation_device_count(result->installation);
             ++index) {
            lt48_catalogue_entry entry;
            char *name;
            char *key;
            char *folded_key;
            if (g_cancellable_is_cancelled(cancellable)) {
                result->status = LT48_CANCELLED;
                break;
            }
            if (lt48_installation_get_device(
                    result->installation, index, &entry) != LT48_OK)
                continue;
            if (request->type_filter != 0u &&
                entry.unknown3 != request->type_filter - 1u)
                continue;
            name = entry.device_name == NULL ? g_strdup("") :
                g_strndup((const char *)entry.device_name,
                          entry.device_name_length);
            key = g_strdup_printf("%s:%s", entry.vendor, name);
            folded_key = g_ascii_strdown(key, -1);
            if (strstr(folded_key, folded_query) != NULL) {
                gsize stored = index;
                g_array_append_val(result->matching_indices, stored);
            }
            g_free(folded_key);
            g_free(key);
            g_free(name);
        }
        g_free(folded_query);
    } else if (result->status == LT48_OK &&
               request->kind == CATALOGUE_TASK_RANK) {
        result->status = lt48_installation_rank_auto_id(
            result->installation, &request->observation,
            NULL, 0u, &result->candidate_count);
        if (result->status == LT48_BUFFER_TOO_SMALL)
            result->status = LT48_OK;
        if (result->status == LT48_OK && result->candidate_count != 0u) {
            result->candidates = g_new0(
                lt48_auto_id_candidate, result->candidate_count);
            result->status = lt48_installation_rank_auto_id(
                result->installation, &request->observation,
                result->candidates, result->candidate_count,
                &result->candidate_count);
        }
        if (result->status == LT48_OK &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    } else if (result->status == LT48_OK &&
               request->kind == CATALOGUE_TASK_AUDIT) {
        result->status = lt48_installation_audit_all_cancellable(
            result->installation, gio_cancelled, cancellable,
            &result->audit);
    }
    g_task_return_pointer(task, result, catalogue_task_result_free);
}

static void catalogue_task_finished(
    GObject *source, GAsyncResult *async_result, gpointer user_data
) {
    enum { RESULT_LIMIT = 500 };
    app_widgets *widgets = user_data;
    catalogue_task_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    char *message;
    (void)source;
    catalogue_set_busy(widgets, FALSE);
    g_clear_object(&widgets->catalogue_cancellable);
    if (result == NULL) {
        widgets->auto_browse_after_audit = 0;
        clear_pending_project_device(widgets);
        clear_pending_project_operation_options(widgets);
        gtk_label_set_text(GTK_LABEL(widgets->device_status),
                           "Catalogue task did not return a result");
        return;
    }
    if (result->status == LT48_CANCELLED) {
        widgets->auto_browse_after_audit = 0;
        clear_pending_project_device(widgets);
        clear_pending_project_operation_options(widgets);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_status),
            "Catalogue task cancelled; previous selection retained");
        catalogue_task_result_free(result);
        return;
    }
    if (result->kind == CATALOGUE_TASK_AUDIT) {
        gboolean audit_ok = result->status == LT48_OK;
        gboolean started_search = FALSE;
        char digest[LT48_SHA256_SIZE * 2u + 1u];
        char *database;
        const char *path = gtk_editable_get_text(
            GTK_EDITABLE(widgets->installation_entry));
        char *base = path == NULL || path[0] == '\0' ?
            g_strdup("database") : g_path_get_basename(path);
        digest_hex(result->audit.manifest_sha256, LT48_SHA256_SIZE, digest);
        widgets->database_manifest_verified = result->status == LT48_OK;
        g_strlcpy(widgets->database_manifest_hex, digest,
                  sizeof(widgets->database_manifest_hex));
        message = g_strdup_printf(
            "Installation %s — %zu/%zu devices resolved, %zu definition "
            "files, %zu failures, %zu zero-pin compatibility entries; "
            "manifest SHA-256 %s. No USB opened.",
            result->status == LT48_OK ? "verified" : "FAILED",
            result->audit.resolved_device_count,
            result->audit.catalogue_device_count,
            result->audit.definition_file_count,
            result->audit.failed_device_count,
            result->audit.zero_pin_device_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->device_status), message);
        database = g_strdup_printf("%s — %.12s", base, digest);
        if (widgets->footer_database != NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->footer_database), database);
        g_free(database);
        g_free(base);
        g_free(message);
        catalogue_task_result_free(result);
        if (widgets->expected_project_manifest_hex[0] != '\0') {
            if (!widgets->database_manifest_verified ||
                g_ascii_strcasecmp(
                    widgets->expected_project_manifest_hex,
                    widgets->database_manifest_hex) != 0) {
                project_status(
                    widgets,
                    "Project rejected: the selected database manifest does "
                    "not match the database recorded by the project");
                g_clear_pointer(&widgets->pending_project_query, g_free);
                clear_pending_project_device(widgets);
                clear_pending_project_operation_options(widgets);
            } else if (widgets->pending_project_query != NULL) {
                char *query = g_steal_pointer(
                    &widgets->pending_project_query);
                gtk_editable_set_text(
                    GTK_EDITABLE(widgets->query_entry), query);
                g_free(query);
                project_status(
                    widgets,
                    "Database manifest verified; resolving the saved device");
                search_devices(NULL, widgets);
                started_search = TRUE;
            }
            widgets->expected_project_manifest_hex[0] = '\0';
        }
        if (widgets->auto_browse_after_audit) {
            widgets->auto_browse_after_audit = 0;
            if (audit_ok && !started_search)
                search_devices(NULL, widgets);
        }
        return;
    }
    if (result->status != LT48_OK) {
        clear_pending_project_device(widgets);
        clear_pending_project_operation_options(widgets);
        message = g_strdup_printf("Catalogue task failed: %s",
                                  lt48_status_string(result->status));
        gtk_label_set_text(GTK_LABEL(widgets->device_status), message);
        g_free(message);
        catalogue_task_result_free(result);
        return;
    }
    clear_list(widgets->vendor_list);
    clear_list(widgets->device_list);
    if (widgets->device_result_indices == NULL)
        widgets->device_result_indices = g_array_new(
            FALSE, FALSE, sizeof(gsize));
    else
        g_array_set_size(widgets->device_result_indices, 0u);
    if (widgets->auto_id_candidate_evidence == NULL)
        widgets->auto_id_candidate_evidence = g_array_new(
            FALSE, FALSE, sizeof(auto_id_candidate_evidence));
    else
        g_array_set_size(widgets->auto_id_candidate_evidence, 0u);
    if (widgets->installation == NULL) {
        widgets->installation = result->installation;
        result->installation = NULL;
    }
    if (widgets->footer_database != NULL) {
        const char *path = gtk_editable_get_text(
            GTK_EDITABLE(widgets->installation_entry));
        char *base = path == NULL || path[0] == '\0' ?
            g_strdup("database") : g_path_get_basename(path);
        char *database = widgets->database_manifest_verified &&
            widgets->database_manifest_hex[0] != '\0' ?
            g_strdup_printf("%s — %.12s", base,
                            widgets->database_manifest_hex) :
            g_strdup_printf("%s — loaded", base);
        gtk_label_set_text(GTK_LABEL(widgets->footer_database), database);
        g_free(database);
        g_free(base);
    }
    widgets->has_pending_device = 0;
    if (widgets->selection_accept_button != NULL)
        gtk_widget_set_sensitive(widgets->selection_accept_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->device_details),
                       "Click a result to select the device");
    gtk_label_set_text(GTK_LABEL(widgets->device_type_value), "—");
    gtk_label_set_text(GTK_LABEL(widgets->device_adapter_value), "—");
    gtk_label_set_text(GTK_LABEL(widgets->device_manufacturer_value), "—");
    gtk_label_set_text(GTK_LABEL(widgets->device_code_value), "—");
    gtk_label_set_text(GTK_LABEL(widgets->device_note_value), "—");
    GHashTable *vendors = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, NULL);
    if (result->kind == CATALOGUE_TASK_SEARCH) {
        size_t matches = result->matching_indices == NULL ? 0u :
                         result->matching_indices->len;
        size_t shown = matches;
        for (size_t item = 0u; item < shown; ++item) {
            gsize index = g_array_index(result->matching_indices, gsize, item);
            lt48_catalogue_entry entry;
            if (lt48_installation_get_device(
                    widgets->installation, index, &entry) != LT48_OK)
                continue;
            append_vendor_once(
                widgets->vendor_list, vendors, entry.vendor);
            g_array_append_val(widgets->device_result_indices, index);
        }
        message = shown == 1u ?
            g_strdup("1 match — selected automatically") :
            g_strdup_printf("%zu match%s in %zu installed devices", matches,
                matches == 1u ? "" : "es",
                lt48_installation_device_count(widgets->installation));
    } else {
        size_t shown = result->candidate_count < RESULT_LIMIT ?
                       result->candidate_count : RESULT_LIMIT;
        for (size_t index = 0u; index < shown; ++index) {
            const lt48_auto_id_candidate *candidate =
                &result->candidates[index];
            gsize device_index = candidate->device_index;
            append_vendor_once(
                widgets->vendor_list, vendors,
                candidate->catalogue.vendor);
            g_array_append_val(
                widgets->device_result_indices, device_index);
        }
        message = g_strdup_printf(
            "%zu ranked candidate%s%s; observations only, no USB opened",
            result->candidate_count,
            result->candidate_count == 1u ? "" : "s",
            shown < result->candidate_count ? " (first 500 shown)" : "");
    }
    {
        GtkListBoxRow *first_vendor = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(widgets->vendor_list), 0);
        if (first_vendor != NULL)
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->vendor_list), first_vendor);
    }
    if (result->kind == CATALOGUE_TASK_SEARCH &&
        widgets->pending_project_device_vendor != NULL &&
        widgets->pending_project_device_name != NULL) {
        size_t exact_count = 0u;
        gsize exact_index = 0u;
        size_t item;
        GtkWidget *vendor_row;
        GtkWidget *device_row;
        for (item = 0u;
             result->matching_indices != NULL &&
             item < result->matching_indices->len; ++item) {
            gsize candidate = g_array_index(
                result->matching_indices, gsize, item);
            lt48_catalogue_entry entry;
            char *name;
            if (lt48_installation_get_device(
                    widgets->installation, candidate, &entry) != LT48_OK)
                continue;
            name = entry.device_name == NULL ? g_strdup("") :
                g_strndup((const char *)entry.device_name,
                          entry.device_name_length);
            if (g_strcmp0(
                    entry.vendor,
                    widgets->pending_project_device_vendor) == 0 &&
                g_strcmp0(
                    name, widgets->pending_project_device_name) == 0) {
                exact_index = candidate;
                ++exact_count;
            }
            g_free(name);
        }
        vendor_row = gtk_widget_get_first_child(widgets->vendor_list);
        while (vendor_row != NULL &&
               g_strcmp0(
                   g_object_get_data(
                       G_OBJECT(vendor_row), "labtool48-vendor"),
                   widgets->pending_project_device_vendor) != 0)
            vendor_row = gtk_widget_get_next_sibling(vendor_row);
        if (exact_count == 1u && vendor_row != NULL) {
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->vendor_list),
                GTK_LIST_BOX_ROW(vendor_row));
            device_row = gtk_widget_get_first_child(widgets->device_list);
            while (device_row != NULL) {
                size_t encoded = GPOINTER_TO_SIZE(g_object_get_data(
                    G_OBJECT(device_row), "labtool48-device-index"));
                if (encoded != 0u && encoded - 1u == exact_index) break;
                device_row = gtk_widget_get_next_sibling(device_row);
            }
            if (device_row != NULL) {
                commit_device_row(
                    GTK_LIST_BOX(widgets->device_list),
                    GTK_LIST_BOX_ROW(device_row), widgets);
                gtk_widget_set_visible(
                    GTK_WIDGET(widgets->device_select_window), FALSE);
                project_status(
                    widgets,
                    "Project restored its exact database-bound device and "
                    "saved operation state");
            } else {
                project_status(
                    widgets,
                    "Project device resolution failed after exact catalogue matching");
                clear_pending_project_operation_options(widgets);
            }
        } else {
            project_status(
                widgets,
                exact_count == 0u ?
                "Project rejected: its exact saved device is absent from the verified database" :
                "Project rejected: its saved device identity is ambiguous in the verified database");
            clear_pending_project_operation_options(widgets);
        }
        clear_pending_project_device(widgets);
    }
    g_hash_table_unref(vendors);
    gtk_label_set_text(GTK_LABEL(widgets->device_status), message);
    g_free(message);
    catalogue_task_result_free(result);
}

static void catalogue_start(app_widgets *widgets, catalogue_task *request) {
    GTask *task;
    if (widgets->catalogue_cancellable != NULL) {
        catalogue_task_free(request);
        return;
    }
    widgets->catalogue_cancellable = g_cancellable_new();
    task = tracked_task_new(
        widgets, widgets->catalogue_cancellable, catalogue_task_finished);
    g_task_set_check_cancellable(task, FALSE);
    g_task_set_task_data(task, request, catalogue_task_free);
    catalogue_set_busy(widgets, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->device_status),
        request->kind == CATALOGUE_TASK_AUDIT ?
        "Auditing every installed definition… No USB access is used." :
        "Loading and searching the catalogue… No USB access is used.");
    g_task_run_in_thread(task, catalogue_task_worker);
    g_object_unref(task);
}

static void cancel_catalogue_task(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    if (widgets->catalogue_cancellable != NULL) {
        g_cancellable_cancel(widgets->catalogue_cancellable);
        gtk_widget_set_sensitive(widgets->catalogue_cancel_button, FALSE);
        gtk_label_set_text(GTK_LABEL(widgets->device_status),
            "Cancellation requested; waiting for the current catalogue step…");
    }
}

static void search_devices(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->installation_entry));
    const char *query = gtk_editable_get_text(GTK_EDITABLE(widgets->query_entry));
    catalogue_task *request;
    (void)button;
    request = g_new0(catalogue_task, 1);
    request->kind = CATALOGUE_TASK_SEARCH;
    request->path = g_strdup(path);
    request->query = g_strdup(query == NULL ? "" : query);
    for (guint index = 1u;
         index < G_N_ELEMENTS(widgets->device_type_filters); ++index) {
        if (gtk_check_button_get_active(
                GTK_CHECK_BUTTON(widgets->device_type_filters[index]))) {
            request->type_filter = index;
            break;
        }
    }
    catalogue_start(widgets, request);
}

static void device_type_filter_changed(
    GtkCheckButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    if (!gtk_check_button_get_active(button) ||
        widgets->catalogue_cancellable != NULL)
        return;
    search_devices(NULL, widgets);
}

static void rank_auto_id(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->installation_entry)
    );
    catalogue_task *request = g_new0(catalogue_task, 1);
    (void)button;
    if (!optional_entry_u16(
            widgets->auto_manufacturer_entry,
            &request->observation.has_manufacturer_id,
            &request->observation.manufacturer_id) ||
        !optional_entry_u16(
            widgets->auto_device_entry, &request->observation.has_device_id,
            &request->observation.device_id) ||
        !optional_entry_u16(
            widgets->auto_pins_entry,
            &request->observation.has_pin_count_hint,
            &request->observation.pin_count_hint) ||
        !optional_entry_u16(
            widgets->auto_class_entry,
            &request->observation.has_device_class_hint,
            &request->observation.device_class_hint) ||
        (!request->observation.has_manufacturer_id &&
         !request->observation.has_device_id &&
         !request->observation.has_pin_count_hint &&
         !request->observation.has_device_class_hint)) {
        catalogue_task_free(request);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_status),
            "Enter at least one numeric observed ID or optional hint"
        );
        return;
    }
    request->kind = CATALOGUE_TASK_RANK;
    request->path = g_strdup(path);
    catalogue_start(widgets, request);
}

static void eprom_auto_id_task_free(gpointer data) {
    eprom_auto_id_task *request = data;
    if (request == NULL) return;
    g_free(request->installation_path);
    g_free(request);
}

static void eprom_auto_id_result_free(gpointer data) {
    eprom_auto_id_result *result = data;
    if (result == NULL) return;
    lt48_installation_close(result->installation);
    g_free(result->candidates);
    g_free(result);
}

static void auto_id_candidate_changed(
    GtkListBox *box, GtkListBoxRow *row, gpointer user_data
) {
    app_widgets *widgets = user_data;
    (void)box;
    gtk_widget_set_sensitive(widgets->auto_id_accept_button, row != NULL);
}

static void auto_id_candidate_accept(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkListBoxRow *row = gtk_list_box_get_selected_row(
        GTK_LIST_BOX(widgets->auto_id_list));
    (void)button;
    if (row == NULL) return;
    commit_device_row(GTK_LIST_BOX(widgets->auto_id_list), row, widgets);
    gtk_widget_set_visible(GTK_WIDGET(widgets->auto_id_window), FALSE);
}

static void eprom_auto_id_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    eprom_auto_id_task *request = task_data;
    eprom_auto_id_result *result = g_new0(eprom_auto_id_result, 1);
    lt48_auto_id_eprom_preparation preparation;
    lt48_context *context = NULL;
    lt48_programmer *programmer = NULL;
    lt48_transport_info transport;
    (void)source_object;
    (void)cancellable;
    result->status = lt48_auto_id_eprom_prepare(
        request->bus, request->address, 1, 0, 0u, &preparation);
    if (result->status == LT48_OK)
        result->status = lt48_context_create(&context);
    if (result->status == LT48_OK)
        result->status = lt48_programmer_open(
            context, request->bus, request->address, &programmer);
    if (result->status == LT48_OK)
        result->status = lt48_programmer_transport_initialize(
            programmer, request->timeout_ms, &transport);
    if (result->status == LT48_OK)
        result->status = lt48_programmer_arm_auto_id_eprom(
            programmer, &preparation,
            LT48_AUTO_ID_EPROM_HARDWARE_AUTHORIZATION);
    if (result->status == LT48_OK)
        result->status = lt48_programmer_execute_auto_id_eprom(
            programmer, &preparation, request->timeout_ms,
            &result->execution);
    lt48_programmer_close(programmer);
    lt48_context_destroy(context);
    if (result->status == LT48_OK)
        result->status = lt48_installation_open(
            request->installation_path, &result->installation);
    if (result->status == LT48_OK)
        result->status = lt48_installation_rank_auto_id_eprom_execution(
            result->installation, &result->execution, NULL, 0u,
            &result->candidate_count);
    if (result->status == LT48_BUFFER_TOO_SMALL) result->status = LT48_OK;
    if (result->status == LT48_OK && result->candidate_count != 0u) {
        result->candidates = g_new0(
            lt48_auto_id_candidate, result->candidate_count);
        result->status = lt48_installation_rank_auto_id_eprom_execution(
            result->installation, &result->execution, result->candidates,
            result->candidate_count, &result->candidate_count);
    }
    g_task_return_pointer(task, result, eprom_auto_id_result_free);
}

static void eprom_auto_id_finished(
    GObject *source, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    eprom_auto_id_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    (void)source;
    gtk_widget_set_sensitive(widgets->auto_id_button, TRUE);
    if (result == NULL || result->status != LT48_OK) {
        char *message = g_strdup_printf(
            "Auto-ID failed: %s",
            result == NULL ? "no result" :
                             lt48_status_string(result->status));
        gtk_label_set_text(GTK_LABEL(widgets->device_status), message);
        gtk_label_set_text(GTK_LABEL(widgets->auto_id_status), message);
        if (widgets->read_preparation != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
        g_free(message);
        eprom_auto_id_result_free(result);
        return;
    }
    clear_list(widgets->vendor_list);
    clear_list(widgets->device_list);
    clear_list(widgets->auto_id_vendor_list);
    clear_list(widgets->auto_id_list);
    if (widgets->installation == NULL) {
        widgets->installation = result->installation;
        result->installation = NULL;
    }
    widgets->has_pending_device = 0;
    if (widgets->selection_accept_button != NULL)
        gtk_widget_set_sensitive(widgets->selection_accept_button, FALSE);
    if (widgets->device_result_indices == NULL)
        widgets->device_result_indices = g_array_new(
            FALSE, FALSE, sizeof(gsize));
    else
        g_array_set_size(widgets->device_result_indices, 0u);
    if (widgets->auto_id_candidate_evidence == NULL)
        widgets->auto_id_candidate_evidence = g_array_new(
            FALSE, FALSE, sizeof(auto_id_candidate_evidence));
    else
        g_array_set_size(widgets->auto_id_candidate_evidence, 0u);
    GHashTable *vendors = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, NULL);
    GHashTable *auto_vendors = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, NULL);
    for (size_t index = 0u; index < result->candidate_count; ++index) {
        const lt48_auto_id_candidate *candidate = &result->candidates[index];
        auto_id_candidate_evidence evidence = {
            .device_index = candidate->device_index,
            .score = candidate->score,
            .manufacturer_id_match = candidate->manufacturer_id_match,
            .device_id_match = candidate->device_id_match,
            .pin_count_match = candidate->pin_count_match,
            .device_class_match = candidate->device_class_match,
            .has_resolved_pin_count = candidate->has_resolved_pin_count,
            .resolved_pin_count = candidate->resolved_pin_count,
            .rank = index + 1u
        };
        gsize device_index = candidate->device_index;
        append_vendor_once(
            widgets->vendor_list, vendors, candidate->catalogue.vendor);
        append_vendor_once(
            widgets->auto_id_vendor_list, auto_vendors,
            candidate->catalogue.vendor);
        g_array_append_val(widgets->device_result_indices, device_index);
        g_array_append_val(widgets->auto_id_candidate_evidence, evidence);
    }
    {
        GtkListBoxRow *first_vendor = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(widgets->vendor_list), 0);
        if (first_vendor != NULL)
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->vendor_list), first_vendor);
    }
    {
        GtkListBoxRow *first_vendor = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(widgets->auto_id_vendor_list), 0);
        if (first_vendor != NULL)
            gtk_list_box_select_row(
                GTK_LIST_BOX(widgets->auto_id_vendor_list), first_vendor);
    }
    g_hash_table_unref(vendors);
    g_hash_table_unref(auto_vendors);
    {
        char *message = g_strdup_printf(
            "Auto-ID: %u-pin device, manufacturer ID 0x%02x, device ID "
            "0x%02x — %zu candidate%s; best match selected.",
            result->execution.socket.package_pin_count,
            result->execution.manufacturer_id,
            result->execution.device_id,
            result->candidate_count,
            result->candidate_count == 1u ? "" : "s");
        gtk_label_set_text(GTK_LABEL(widgets->device_status), message);
        gtk_label_set_text(GTK_LABEL(widgets->auto_id_status), message);
        if (widgets->read_preparation != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
        g_free(message);
    }
    if (result->candidate_count == 1u) {
        GtkListBoxRow *row = gtk_list_box_get_selected_row(
            GTK_LIST_BOX(widgets->auto_id_list));
        commit_device_row(GTK_LIST_BOX(widgets->auto_id_list), row, widgets);
        gtk_widget_set_visible(GTK_WIDGET(widgets->auto_id_window), FALSE);
    } else {
        gtk_window_present(widgets->auto_id_window);
    }
    eprom_auto_id_result_free(result);
}

static void plan_eprom_auto_id(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    eprom_auto_id_task *request;
    GTask *task;
    uint8_t bus;
    uint8_t address;
    const char *path;
    (void)button;
    clear_list(widgets->auto_id_vendor_list);
    clear_list(widgets->auto_id_list);
    if (widgets->auto_id_candidate_evidence != NULL)
        g_array_set_size(widgets->auto_id_candidate_evidence, 0u);
    gtk_widget_set_sensitive(widgets->auto_id_accept_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->auto_id_status),
                       "Identifying the inserted EPROM…");
    gtk_window_present(widgets->auto_id_window);
    if (!widgets->programmer_ready) {
        widgets->pending_hardware_action = PENDING_HARDWARE_AUTO_ID;
        gtk_label_set_text(GTK_LABEL(widgets->device_status),
                           "Programmer was not ready; probing and connecting now…");
        gtk_label_set_text(GTK_LABEL(widgets->auto_id_status),
                           "Programmer was not ready; probing and connecting now…");
        refresh_programmers(NULL, widgets);
        return;
    }
    if (!programmer_target(widgets, &bus, &address)) {
        gtk_label_set_text(GTK_LABEL(widgets->device_status),
                           "No exact programmer target is available");
        gtk_label_set_text(GTK_LABEL(widgets->auto_id_status),
                           "No exact programmer target is available");
        return;
    }
    path = gtk_editable_get_text(GTK_EDITABLE(widgets->installation_entry));
    request = g_new0(eprom_auto_id_task, 1);
    request->installation_path = g_strdup(path);
    request->bus = bus;
    request->address = address;
    request->timeout_ms = 2000u;
    gtk_widget_set_sensitive(widgets->auto_id_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->device_status),
                       "Identifying inserted EPROM…");
    task = tracked_task_new(widgets, NULL, eprom_auto_id_finished);
    g_task_set_task_data(task, request, eprom_auto_id_task_free);
    g_task_run_in_thread(task, eprom_auto_id_worker);
    g_object_unref(task);
}

static void search_entry_activated(GtkEntry *entry, gpointer user_data) {
    (void)entry;
    search_devices(NULL, user_data);
}

static void programming_image_selected(
    GObject *source, GAsyncResult *result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(
        GTK_FILE_DIALOG(source), result, &error);
    if (file != NULL) {
        char *path = g_file_get_path(file);
        if (path != NULL) {
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->verify_image_entry), path);
            g_free(path);
            inspect_programming_image(NULL, widgets);
        }
        g_object_unref(file);
    } else if (error != NULL &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_DISMISSED) &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_CANCELLED)) {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           error->message);
    }
    g_clear_error(&error);
}

static void browse_programming_image(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    (void)button;
    gtk_file_dialog_set_title(
        dialog, "Select BIN, ROM, Intel HEX, S-record, Straight, MOS, Tektronix, or HP64000 image");
    gtk_file_dialog_open(dialog, widgets->window, NULL,
                         programming_image_selected, widgets);
    g_object_unref(dialog);
}

static void image_inspect_task_free(gpointer data) {
    image_inspect_task *task = data;
    if (task == NULL) return;
    g_free(task->path);
    g_free(task);
}

static void image_inspect_result_free(gpointer data) {
    image_inspect_result *result = data;
    if (result == NULL) return;
    g_free(result->message);
    g_free(result);
}

static void image_inspect_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    image_inspect_task *work = task_data;
    image_inspect_result *result = g_new0(image_inspect_result, 1);
    lt48_image_import_result import = {0};
    lt48_buffer *buffer = NULL;
    lt48_status status;
    int range_known = 0;
    const char *format_name;
    GString *message;
    (void)source_object;

    result->input_revision = work->input_revision;
    status = lt48_buffer_load_image_cancellable(
        work->path, &work->options, gio_cancelled, cancellable,
        &import, &buffer);
    if (status == LT48_OK &&
        ((size_t)work->selected_start > lt48_buffer_size(buffer) ||
         (size_t)work->selected_length >
            lt48_buffer_size(buffer) - (size_t)work->selected_start))
        status = LT48_BUFFER_TOO_SMALL;
    if (status == LT48_OK)
        status = lt48_buffer_range_known(
            buffer, work->selected_start, work->selected_length,
            &range_known);
    result->status = status;
    result->selected_range_known = range_known;
    if (status != LT48_OK) {
        result->message = g_strdup_printf(
            "Image rejected: %s. No USB access was used.",
            lt48_status_string(status));
        lt48_buffer_close(buffer);
        g_task_return_pointer(task, result, image_inspect_result_free);
        return;
    }

    format_name = lt48_image_format_name(import.format);
    message = g_string_new(NULL);
    g_string_append_printf(
        message,
        "VALID %s IMAGE — %zu mapped data bytes from %zu record%s\n"
        "Device mapping: %zu bytes at source base 0x%llx; ",
        format_name == NULL ? "unknown" : format_name,
        import.mapped_data_byte_count, import.data_record_count,
        import.data_record_count == 1u ? "" : "s",
        work->options.buffer_size,
        (unsigned long long)work->options.address_base);
    if (import.has_data)
        g_string_append_printf(
            message, "source range 0x%llx..0x%llx\n",
            (unsigned long long)import.minimum_source_address,
            (unsigned long long)import.maximum_source_address_exclusive);
    else
        g_string_append(message, "source range is empty\n");
    g_string_append_printf(
        message, "Sparse mapping: %zu gap region%s / %zu byte%s, ",
        import.gap_count, import.gap_count == 1u ? "" : "s",
        import.gap_byte_count, import.gap_byte_count == 1u ? "" : "s");
    if (work->options.gap_policy == LT48_IMAGE_GAPS_FILL)
        g_string_append_printf(
            message, "filled with 0x%02x", work->options.gap_fill_value);
    else
        g_string_append(message, "preserved");
    g_string_append_printf(
        message, "\nSelected range 0x%x + %u bytes: %s",
        work->selected_start, work->selected_length,
        range_known ? "READY — every selected byte is known" :
                      "NOT READY — contains preserved sparse gaps");
    if (import.has_execution_start)
        g_string_append_printf(
            message,
            "\nExecution-start metadata: 0x%llx (descriptive only; not "
            "programmed)",
            (unsigned long long)import.execution_start_address);
    result->message = g_string_free(message, FALSE);
    lt48_buffer_close(buffer);
    g_task_return_pointer(task, result, image_inspect_result_free);
}

static void image_inspect_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    image_inspect_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    (void)source_object;
    g_clear_object(&widgets->image_inspect_cancellable);
    gtk_widget_remove_css_class(widgets->image_status, "error");
    gtk_widget_remove_css_class(widgets->image_status, "warning");
    if (result == NULL || result->status != LT48_OK)
        gtk_widget_add_css_class(widgets->image_status, "error");
    else if (!result->selected_range_known)
        gtk_widget_add_css_class(widgets->image_status, "warning");
    gtk_label_set_text(
        GTK_LABEL(widgets->image_status),
        result != NULL &&
        result->input_revision != widgets->operation_input_revision ?
            "Image inspection finished for superseded inputs; inspect the "
            "current mapping again" :
        result == NULL ? "Image inspection failed without a result" :
                         result->message);
    image_inspect_result_free(result);
    update_operation_controls(widgets);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
}

static void inspect_programming_image(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    image_inspect_task *work;
    lt48_device_spec spec;
    uint32_t fill_value;
    guint format_choice;
    guint gap_choice;
    const char *path;
    GTask *task;
    lt48_status status;
    (void)button;

    if (widgets->image_inspect_cancellable != NULL) return;
    if (widgets->installation == NULL || !widgets->has_selected_device) {
        gtk_label_set_text(GTK_LABEL(widgets->image_status),
                           "Select a device before inspecting its image map");
        return;
    }
    status = lt48_installation_resolve_device(
        widgets->installation, widgets->selected_device_index, &spec);
    path = gtk_editable_get_text(GTK_EDITABLE(widgets->verify_image_entry));
    format_choice = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->image_format_dropdown));
    gap_choice = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->image_gap_dropdown));
    work = g_new0(image_inspect_task, 1);
    if (status != LT48_OK || spec.capacity_bytes == 0u || path == NULL ||
        path[0] == '\0' || format_choice >
            LT48_IMAGE_FORMAT_HP64000_ABSOLUTE || gap_choice > 1u ||
        !entry_u32(widgets->read_start_entry, UINT32_MAX,
                   &work->selected_start) ||
        !entry_u32(widgets->read_length_entry, UINT32_MAX,
                   &work->selected_length) || work->selected_length == 0u ||
        !entry_u64(widgets->image_base_entry,
                   &work->options.address_base) ||
        !entry_u32(widgets->image_gap_fill_entry, UINT8_MAX, &fill_value)) {
        gtk_label_set_text(
            GTK_LABEL(widgets->image_status),
            "Enter a file, valid selected range, format, address base and "
            "gap value");
        image_inspect_task_free(work);
        return;
    }
    work->path = g_strdup(path);
    work->options.format = (lt48_image_format)format_choice;
    work->options.buffer_size = spec.capacity_bytes;
    work->options.gap_policy = gap_choice == 0u ?
        LT48_IMAGE_GAPS_PRESERVE : LT48_IMAGE_GAPS_FILL;
    work->options.gap_fill_value = (unsigned char)fill_value;
    work->input_revision = widgets->operation_input_revision;
    widgets->image_inspect_cancellable = g_cancellable_new();
    update_operation_controls(widgets);
    gtk_label_set_text(GTK_LABEL(widgets->image_status),
                       "Inspecting image mapping outside the UI thread…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->image_inspect_cancellable,
        image_inspect_finished);
    g_task_set_task_data(task, work, image_inspect_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, image_inspect_worker);
    g_object_unref(task);
}

static void verify_installation(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->installation_entry)
    );
    catalogue_task *request = g_new0(catalogue_task, 1);
    (void)button;
    request->kind = CATALOGUE_TASK_AUDIT;
    request->path = g_strdup(path);
    catalogue_start(widgets, request);
}

static void cache_import_task_free(gpointer data) {
    cache_import_task *task = data;
    if (task == NULL) return;
    g_free(task->source);
    g_free(task->runtime_source);
    g_free(task->destination);
    g_free(task);
}

static void cache_import_result_free(gpointer data) {
    cache_import_result *result = data;
    if (result == NULL) return;
    g_free(result->source);
    g_free(result->runtime_source);
    g_free(result->destination);
    g_free(result);
}

static void cache_import_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    cache_import_task *request = task_data;
    cache_import_result *result = g_new0(cache_import_result, 1);
    (void)source_object;
    (void)cancellable;
    result->source = g_strdup(request->source);
    result->runtime_source = g_strdup(request->runtime_source);
    result->destination = g_strdup(request->destination);
    result->status = lt48_private_cache_import_with_runtime(
        request->source, request->runtime_source, request->destination,
        &result->report
    );
    g_task_return_pointer(task, result, cache_import_result_free);
}

static void cache_import_finished(
    GObject *source, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    cache_import_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL
    );
    char installation_digest[LT48_SHA256_SIZE * 2u + 1u];
    char cache_digest[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    (void)source;
    gtk_widget_set_sensitive(widgets->cache_import_button, TRUE);
    if (result == NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->device_status),
                           "Private-cache import did not return a result");
        return;
    }
    if (result->status != LT48_OK) {
        message = g_strdup_printf(
            "Private-cache import failed: %s. Existing destinations are "
            "never overwritten.", lt48_status_string(result->status)
        );
        gtk_label_set_text(GTK_LABEL(widgets->device_status), message);
        g_free(message);
        cache_import_result_free(result);
        return;
    }
    digest_hex(result->report.installation_manifest_sha256,
               LT48_SHA256_SIZE, installation_digest);
    digest_hex(result->report.cache_manifest_sha256,
               LT48_SHA256_SIZE, cache_digest);
    lt48_installation_close(widgets->installation);
    widgets->installation = NULL;
    lt48_property_document_close(widgets->operation_option_document);
    widgets->operation_option_document = NULL;
    lt48_test_vectors_close(widgets->test_vectors);
    widgets->test_vectors = NULL;
    fuse_session_reset(widgets);
    lt48_buffer_close(widgets->secondary_buffer);
    widgets->secondary_buffer = NULL;
    widgets->secondary_buffer_kind = SECONDARY_BUFFER_NONE;
    widgets->has_selected_device = 0;
    refresh_operation_dropdown(widgets);
    clear_list(widgets->vendor_list);
    clear_list(widgets->device_list);
    if (widgets->device_result_indices != NULL)
        g_array_set_size(widgets->device_result_indices, 0u);
    gtk_editable_set_text(GTK_EDITABLE(widgets->installation_entry),
                          result->destination);
    message = g_strdup_printf(
        "Private cache created and selected: %zu source files, %zu "
        "content-addressed objects, %zu verified. Installation manifest %s; "
        "cache manifest %s. Definitions came from %s; the proved idle FPGA "
        "came from %s. WLT48UXP.exe was verified but not copied; no USB "
        "opened.", result->report.source_file_count,
        result->report.object_count, result->report.verified_object_count,
        installation_digest, cache_digest, result->source,
        result->runtime_source
    );
    gtk_label_set_text(GTK_LABEL(widgets->device_status), message);
    gtk_label_set_text(GTK_LABEL(widgets->device_details),
                       "Search the newly selected private cache");
    g_free(message);
    cache_import_result_free(result);
}

static void cache_destination_selected(
    GObject *source, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    GFile *destination = gtk_file_dialog_save_finish(
        GTK_FILE_DIALOG(source), async_result, &error
    );
    if (destination != NULL) {
        const char *source_path = gtk_editable_get_text(
            GTK_EDITABLE(widgets->installation_entry)
        );
        char *destination_path = g_file_get_path(destination);
        char *runtime_source = NULL;
        char *runtime_exe = NULL;
        char *runtime_idle = NULL;
        char *owned_fallback = NULL;
        if (source_path != NULL && source_path[0] != '\0') {
            runtime_exe = g_build_filename(source_path, "WLT48UXP.exe", NULL);
            runtime_idle = g_build_filename(source_path, "idle-fpga.bin", NULL);
            if (g_file_test(runtime_exe, G_FILE_TEST_IS_REGULAR) ||
                g_file_test(runtime_idle, G_FILE_TEST_IS_REGULAR)) {
                runtime_source = g_strdup(source_path);
            } else {
                owned_fallback = project_path("AEC/LabTool-48UXP");
                g_free(runtime_exe);
                g_free(runtime_idle);
                runtime_exe = owned_fallback == NULL ? NULL :
                    g_build_filename(owned_fallback, "WLT48UXP.exe", NULL);
                runtime_idle = owned_fallback == NULL ? NULL :
                    g_build_filename(owned_fallback, "idle-fpga.bin", NULL);
                if ((runtime_exe != NULL &&
                     g_file_test(runtime_exe, G_FILE_TEST_IS_REGULAR)) ||
                    (runtime_idle != NULL &&
                     g_file_test(runtime_idle, G_FILE_TEST_IS_REGULAR)))
                    runtime_source = g_strdup(owned_fallback);
            }
        }
        if (source_path == NULL || source_path[0] == '\0' ||
            destination_path == NULL || destination_path[0] == '\0' ||
            runtime_source == NULL) {
            gtk_label_set_text(GTK_LABEL(widgets->device_status),
                               "Select an owned definition installation and "
                               "a local private-cache destination. A verified "
                               "runtime source containing WLT48UXP.exe or "
                               "idle-fpga.bin is also required.");
        } else {
            cache_import_task *request = g_new0(cache_import_task, 1);
            GTask *task;
            request->source = g_strdup(source_path);
            request->runtime_source = g_strdup(runtime_source);
            request->destination = g_strdup(destination_path);
            task = tracked_task_new(widgets, NULL, cache_import_finished);
            g_task_set_task_data(task, request, cache_import_task_free);
            gtk_widget_set_sensitive(widgets->cache_import_button, FALSE);
            gtk_label_set_text(
                GTK_LABEL(widgets->device_status),
                "Creating and verifying a private operational cache from the "
                "selected definitions and proved runtime payload… No USB "
                "access is used."
            );
            g_task_run_in_thread(task, cache_import_worker);
            g_object_unref(task);
        }
        g_free(owned_fallback);
        g_free(runtime_idle);
        g_free(runtime_exe);
        g_free(runtime_source);
        g_free(destination_path);
        g_object_unref(destination);
    } else if (error != NULL &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_DISMISSED) &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_CANCELLED)) {
        gtk_label_set_text(GTK_LABEL(widgets->device_status), error->message);
    }
    g_clear_error(&error);
}

static void create_private_cache(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    (void)button;
    gtk_file_dialog_set_title(
        dialog, "Create a new private LabTool cache directory"
    );
    gtk_file_dialog_set_initial_name(dialog, "labtool48-private-cache");
    gtk_file_dialog_save(dialog, widgets->window, NULL,
                         cache_destination_selected, widgets);
    g_object_unref(dialog);
}

static GtkWidget *make_scrolled_list(GtkWidget **out_list) {
    GtkWidget *scroll = gtk_scrolled_window_new();
    *out_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(*out_list), GTK_SELECTION_NONE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), *out_list);
    gtk_widget_set_vexpand(scroll, TRUE);
    return scroll;
}

static void database_path_changed(GtkEditable *editable, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(editable);
    char *base = path == NULL || path[0] == '\0' ?
        g_strdup("none") : g_path_get_basename(path);
    char *database = g_strdup_printf("%s — not loaded", base);
    invalidate_operation_preparation(widgets, 0);
    widgets->database_manifest_verified = 0;
    widgets->database_manifest_hex[0] = '\0';
    lt48_installation_close(widgets->installation);
    widgets->installation = NULL;
    lt48_property_document_close(widgets->operation_option_document);
    widgets->operation_option_document = NULL;
    lt48_test_vectors_close(widgets->test_vectors);
    widgets->test_vectors = NULL;
    fuse_session_reset(widgets);
    lt48_buffer_close(widgets->secondary_buffer);
    widgets->secondary_buffer = NULL;
    widgets->secondary_buffer_kind = SECONDARY_BUFFER_NONE;
    widgets->has_selected_device = 0;
    widgets->selected_device_index = 0u;
    widgets->has_pending_device = 0;
    if (widgets->selection_accept_button != NULL)
        gtk_widget_set_sensitive(widgets->selection_accept_button, FALSE);
    widgets->has_socket_guidance = 0;
    if (widgets->vendor_list != NULL)
        clear_list(widgets->vendor_list);
    if (widgets->device_list != NULL)
        clear_list(widgets->device_list);
    if (widgets->auto_id_vendor_list != NULL)
        clear_list(widgets->auto_id_vendor_list);
    if (widgets->auto_id_list != NULL)
        clear_list(widgets->auto_id_list);
    if (widgets->device_result_indices != NULL)
        g_array_set_size(widgets->device_result_indices, 0u);
    if (widgets->socket_diagram != NULL)
        gtk_widget_queue_draw(widgets->socket_diagram);
    if (widgets->operation_device_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->operation_device_status),
                           "Selected device: none");
    if (widgets->footer_database != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_database), database);
    if (widgets->footer_device != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_device), "None");
    if (widgets->footer_package != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_package), "—");
    if (widgets->footer_pin != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_pin), "—");
    if (widgets->footer_capacity != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_capacity), "—");
    if (widgets->footer_voltage != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_voltage), "—");
    if (widgets->footer_vpp != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_vpp), "—");
    if (widgets->footer_manufacturer_code != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_manufacturer_code), "—");
    if (widgets->footer_device_code != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_device_code), "—");
    if (widgets->footer_note != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_note), "—");
    refresh_operation_dropdown(widgets);
    refresh_programming_parameters(widgets);
    g_free(database);
    g_free(base);
}

static void set_voltage_label(GtkWidget *label, uint16_t centivolts) {
    char text[32];
    g_snprintf(text, sizeof(text), "%.2f V", centivolts / 100.0);
    gtk_label_set_text(GTK_LABEL(label), text);
}

static void refresh_programming_parameters(app_widgets *widgets) {
    lt48_verify_voltage_profile profile;
    guint margin_type;
    lt48_status status;
    if (widgets->parameters_status == NULL) return;
    if (widgets->installation == NULL || !widgets->has_selected_device) {
        gtk_label_set_text(GTK_LABEL(widgets->parameters_nominal), "—");
        gtk_label_set_text(GTK_LABEL(widgets->parameters_low), "—");
        gtk_label_set_text(GTK_LABEL(widgets->parameters_high), "—");
        gtk_label_set_text(
            GTK_LABEL(widgets->parameters_status),
            "Select a device to load its programming parameters");
        return;
    }
    margin_type = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->parameters_margin_dropdown));
    status = lt48_installation_verify_voltage_profile(
        widgets->installation, widgets->selected_device_index,
        margin_type > 1u ? 0u : margin_type, &profile);
    if (status != LT48_OK) {
        char *message = g_strdup_printf(
            "Voltage profile unavailable: %s",
            lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->parameters_nominal), "—");
        gtk_label_set_text(GTK_LABEL(widgets->parameters_low), "—");
        gtk_label_set_text(GTK_LABEL(widgets->parameters_high), "—");
        gtk_label_set_text(GTK_LABEL(widgets->parameters_status), message);
        g_free(message);
        return;
    }
    set_voltage_label(widgets->parameters_nominal,
                      profile.nominal_centivolts);
    set_voltage_label(widgets->parameters_low, profile.low_centivolts);
    set_voltage_label(widgets->parameters_high, profile.high_centivolts);
    gtk_label_set_text(
        GTK_LABEL(widgets->parameters_status),
        profile.explicit_value_count == 0u ?
            "Verify margins derived from the selected margin policy" :
            "Verify margins supplied explicitly by the device definition");
}

static void parameters_margin_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    app_widgets *widgets = user_data;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    (void)parameter;
    if (widgets->verify_margin_type_dropdown != NULL)
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->verify_margin_type_dropdown), selected);
    invalidate_operation_preparation(widgets, 1);
    refresh_programming_parameters(widgets);
}

static void operation_margin_type_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    app_widgets *widgets = user_data;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    if (widgets->parameters_margin_dropdown != NULL)
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->parameters_margin_dropdown), selected);
    operation_input_notify(object, parameter, user_data);
    refresh_programming_parameters(widgets);
}

static void preview_device_selected(
    GtkListBox *box, GtkListBoxRow *row, gpointer user_data
) {
    app_widgets *widgets = user_data;
    size_t encoded_index;
    size_t index;
    lt48_device_spec spec;
    lt48_device_guidance guidance;
    lt48_m37_resource_audit audit;
    lt48_status status;
    char *device_name;
    char *text;
    GString *details;
    (void)box;
    widgets->has_pending_device = 0;
    if (widgets->selection_accept_button != NULL)
        gtk_widget_set_sensitive(widgets->selection_accept_button, FALSE);
    if (row == NULL || widgets->installation == NULL) return;
    encoded_index = GPOINTER_TO_SIZE(
        g_object_get_data(G_OBJECT(row), "labtool48-device-index"));
    if (encoded_index == 0u) return;
    index = encoded_index - 1u;
    status = lt48_installation_resolve_device(
        widgets->installation, index, &spec);
    if (status != LT48_OK) {
        text = g_strdup_printf(
            "Definition cannot be resolved: %s",
            lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->device_details), text);
        g_free(text);
        return;
    }
    status = lt48_installation_device_guidance(
        widgets->installation, index, &guidance);
    if (status != LT48_OK) {
        text = g_strdup_printf(
            "Guidance cannot be resolved: %s", lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->device_details), text);
        g_free(text);
        return;
    }
    device_name = spec.catalogue.device_name == NULL ? g_strdup("") :
        g_strndup((const char *)spec.catalogue.device_name,
                  spec.catalogue.device_name_length);
    {
        const char *adapter = guidance.adapter_requirement ==
            LT48_ADAPTER_REQUIRED ? "Required" :
            guidance.adapter_requirement == LT48_ADAPTER_POSSIBLY_DIRECT ?
            "Direct; verify package" : "Unknown";
        char *type = g_strdup_printf(
            "%s — class %u / type %u",
            device_type_category_label(spec.catalogue.unknown3),
            spec.catalogue.device_class, spec.catalogue.device_type);
        char *adapter_text = guidance.has_catalogue_package_hint ?
            g_strdup_printf("%s, %u pins — %s",
                guidance.catalogue_package_code,
                guidance.catalogue_package_pin_count, adapter) :
            g_strdup_printf("%u pins — %s", spec.pin_count, adapter);
        char *manufacturer = g_strdup_printf(
            "%04Xh", spec.catalogue.manufacturer_id);
        char *device_code = g_strdup_printf(
            "%04Xh", spec.catalogue.device_id);
        char *note = g_strdup_printf(
            "%s / D%X in the active database",
            spec.definition_stem == NULL ? "definition" :
                spec.definition_stem,
            spec.catalogue.device_type);
        gtk_label_set_text(GTK_LABEL(widgets->device_type_value), type);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_adapter_value), adapter_text);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_manufacturer_value), manufacturer);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_code_value), device_code);
        gtk_label_set_text(GTK_LABEL(widgets->device_note_value), note);
        g_free(note);
        g_free(device_code);
        g_free(manufacturer);
        g_free(adapter_text);
        g_free(type);
    }
    details = g_string_new(NULL);
    g_string_append_printf(
        details, "Device: %s %s\nPackage: %u pins",
        spec.catalogue.vendor, device_name, spec.pin_count);
    if (guidance.has_socket_span)
        g_string_append_printf(
            details, " — bottom of ZIF, contacts %u–%u",
            guidance.first_socket_position, guidance.last_socket_position);
    if (spec.element_count != 0U && spec.element_bits != 0U)
        g_string_append_printf(
            details, "\nCapacity: %u bytes — %u × %u-bit elements",
            spec.capacity_bytes, spec.element_count, spec.element_bits);
    else
        g_string_append_printf(
            details, "\nCapacity: %u bytes", spec.capacity_bytes);
    if (lt48_installation_audit_m37(
            widgets->installation, index, &audit) == LT48_OK)
        g_string_append_printf(
            details, "\nLive read: available — nominal VCC %.2f V",
            audit.nominal_vcc_centivolts / 100.0);
    else
        g_string_append(
            details, "\nLive read: not enabled for this definition");
    gtk_label_set_text(GTK_LABEL(widgets->device_details), details->str);
    widgets->pending_device_index = index;
    widgets->has_pending_device = 1;
    if (widgets->selection_accept_button != NULL)
        gtk_widget_set_sensitive(widgets->selection_accept_button, TRUE);
    g_string_free(details, TRUE);
    g_free(device_name);
}

static void commit_device_row(GtkListBox *box, GtkListBoxRow *row,
                              gpointer user_data) {
    app_widgets *widgets = user_data;
    size_t encoded_index;
    size_t index;
    lt48_device_spec spec;
    lt48_device_guidance guidance;
    lt48_m37_resource_audit audit;
    lt48_status status;
    char *device_name;
    char *text;
    GString *details;
    int selection_changed;
    (void)box;
    if (row == NULL || widgets->installation == NULL) {
        return;
    }
    encoded_index = GPOINTER_TO_SIZE(
        g_object_get_data(G_OBJECT(row), "labtool48-device-index")
    );
    if (encoded_index == 0) {
        return;
    }
    index = encoded_index - 1;
    selection_changed = !widgets->has_selected_device ||
        widgets->selected_device_index != index;
    invalidate_operation_preparation(widgets, 0);
    lt48_test_vectors_close(widgets->test_vectors);
    widgets->test_vectors = NULL;
    fuse_session_reset(widgets);
    status = lt48_installation_resolve_device(
        widgets->installation, index, &spec
    );
    if (status != LT48_OK) {
        widgets->has_socket_guidance = 0;
        if (widgets->socket_diagram != NULL)
            gtk_widget_queue_draw(widgets->socket_diagram);
        text = g_strdup_printf("Definition cannot be resolved: %s",
                               lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->device_details), text);
        g_free(text);
        return;
    }
    device_name = spec.catalogue.device_name == NULL ? g_strdup("") :
        g_strndup((const char *)spec.catalogue.device_name,
                  spec.catalogue.device_name_length);
    status = lt48_installation_device_guidance(
        widgets->installation, index, &guidance
    );
    if (status != LT48_OK) {
        widgets->has_socket_guidance = 0;
        if (widgets->socket_diagram != NULL)
            gtk_widget_queue_draw(widgets->socket_diagram);
        text = g_strdup_printf("Guidance cannot be resolved: %s",
                               lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->device_details), text);
        g_free(text);
        g_free(device_name);
        return;
    }
    widgets->socket_guidance = guidance;
    widgets->has_socket_guidance = 1;
    if (widgets->socket_diagram != NULL)
        gtk_widget_queue_draw(widgets->socket_diagram);
    {
        const char *adapter = guidance.adapter_requirement ==
            LT48_ADAPTER_REQUIRED ? "Required" :
            guidance.adapter_requirement == LT48_ADAPTER_POSSIBLY_DIRECT ?
            "Direct; verify package" : "Unknown";
        char *type = g_strdup_printf(
            "%s — class %u / type %u",
            device_type_category_label(spec.catalogue.unknown3),
            spec.catalogue.device_class, spec.catalogue.device_type);
        char *adapter_text = guidance.has_catalogue_package_hint ?
            g_strdup_printf("%s, %u pins — %s",
                guidance.catalogue_package_code,
                guidance.catalogue_package_pin_count, adapter) :
            g_strdup_printf("%u pins — %s", spec.pin_count, adapter);
        char *manufacturer = g_strdup_printf(
            "%04Xh", spec.catalogue.manufacturer_id);
        char *device_code = g_strdup_printf(
            "%04Xh", spec.catalogue.device_id);
        char *note = g_strdup_printf(
            "%s / D%X in the active database",
            spec.definition_stem == NULL ? "definition" :
                spec.definition_stem,
            spec.catalogue.device_type);
        gtk_label_set_text(GTK_LABEL(widgets->device_type_value), type);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_adapter_value), adapter_text);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_manufacturer_value), manufacturer);
        gtk_label_set_text(
            GTK_LABEL(widgets->device_code_value), device_code);
        gtk_label_set_text(GTK_LABEL(widgets->device_note_value), note);
        g_free(note);
        g_free(device_code);
        g_free(manufacturer);
        g_free(adapter_text);
        g_free(type);
    }
    details = g_string_new(NULL);
    g_string_append_printf(
        details,
        "Device: %s %s\nPackage: %u pins",
        spec.catalogue.vendor, device_name, spec.pin_count
    );
    if (guidance.has_socket_span)
        g_string_append_printf(
            details, " — bottom of ZIF, contacts %u–%u",
            guidance.first_socket_position, guidance.last_socket_position
        );
    if (spec.element_count != 0U && spec.element_bits != 0U)
        g_string_append_printf(
            details, "\nCapacity: %u bytes — %u × %u-bit elements",
            spec.capacity_bytes, spec.element_count, spec.element_bits);
    else
        g_string_append_printf(
            details, "\nCapacity: %u bytes", spec.capacity_bytes);
    if (lt48_installation_audit_m37(
            widgets->installation, index, &audit) == LT48_OK)
        g_string_append_printf(
            details, "\nLive read: available — nominal VCC %.2f V",
            audit.nominal_vcc_centivolts / 100.0);
    else
        g_string_append(details,
            "\nLive read: not enabled for this definition");
    gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->device_details), details->str);
    if (widgets->operation_device_status != NULL) {
        char *selected;
        if (spec.element_count != 0U && spec.element_bits != 0U)
            selected = g_strdup_printf(
                "Selected device: %s %s — %u bytes (%u × %u-bit elements)",
                spec.catalogue.vendor, device_name, spec.capacity_bytes,
                spec.element_count, spec.element_bits);
        else
            selected = g_strdup_printf(
                "Selected device: %s %s — %u bytes",
                spec.catalogue.vendor, device_name, spec.capacity_bytes);
        gtk_label_set_text(
            GTK_LABEL(widgets->operation_device_status), selected);
        g_free(selected);
    }
    if (widgets->footer_device != NULL) {
        char *selected = g_strdup_printf(
            "%s %s", spec.catalogue.vendor, device_name);
        gtk_label_set_text(GTK_LABEL(widgets->footer_device), selected);
        g_free(selected);
    }
    if (widgets->footer_package != NULL) {
        const char *adapter = guidance.adapter_requirement ==
            LT48_ADAPTER_REQUIRED ? "adapter required" :
            guidance.adapter_requirement == LT48_ADAPTER_POSSIBLY_DIRECT ?
            "direct/verify" : "adapter unknown";
        char *package = guidance.has_catalogue_package_hint ?
            g_strdup_printf("%s, %u pins — %s",
                guidance.catalogue_package_code,
                guidance.catalogue_package_pin_count, adapter) :
            g_strdup_printf("%u pins — %s", spec.pin_count, adapter);
        gtk_label_set_text(GTK_LABEL(widgets->footer_package), package);
        g_free(package);
    }
    if (widgets->footer_pin != NULL) {
        char *pin = g_strdup_printf("%u", spec.pin_count);
        gtk_label_set_text(GTK_LABEL(widgets->footer_pin), pin);
        g_free(pin);
    }
    if (widgets->footer_capacity != NULL) {
        char *capacity = g_strdup_printf("%u bytes", spec.capacity_bytes);
        gtk_label_set_text(GTK_LABEL(widgets->footer_capacity), capacity);
        g_free(capacity);
    }
    if (widgets->footer_voltage != NULL) {
        char *voltage;
        if (lt48_installation_audit_m37(
                widgets->installation, index, &audit) == LT48_OK)
            voltage = g_strdup_printf(
                "VCC %.2f V — VPP from definition",
                audit.nominal_vcc_centivolts / 100.0);
        else
            voltage = g_strdup("From selected definition");
        gtk_label_set_text(GTK_LABEL(widgets->footer_voltage), voltage);
        g_free(voltage);
    }
    if (widgets->footer_vpp != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_vpp),
                           "From selected definition");
    if (widgets->footer_manufacturer_code != NULL) {
        char *code = g_strdup_printf("%04Xh", spec.catalogue.manufacturer_id);
        gtk_label_set_text(GTK_LABEL(widgets->footer_manufacturer_code), code);
        g_free(code);
    }
    if (widgets->footer_device_code != NULL) {
        char *code = g_strdup_printf("%04Xh", spec.catalogue.device_id);
        gtk_label_set_text(GTK_LABEL(widgets->footer_device_code), code);
        g_free(code);
    }
    if (widgets->footer_note != NULL) {
        char *note = g_strdup_printf(
            "Database class %u, type %u",
            spec.catalogue.device_class, spec.catalogue.device_type);
        gtk_label_set_text(GTK_LABEL(widgets->footer_note), note);
        g_free(note);
    }
    if (!widgets->has_selected_device ||
        widgets->selected_device_index != index) {
        lt48_property_document_close(widgets->operation_option_document);
        widgets->operation_option_document = NULL;
    }
    widgets->selected_device_index = index;
    widgets->has_selected_device = 1;
    if (selection_changed) {
        /*
         * The Windows host owns one process-wide primary buffer before any
         * device object is constructed.  TMemDev selection resizes that
         * buffer to the definition's byte capacity; it does not wait for a
         * Load or Read command to create the editor model.  Keep an existing
         * resident image intact when changing definitions, but make the
         * first selection immediately useful and deterministic.  FF is the
         * non-programmed state used by the recovered memory workflows and
         * avoids exposing allocator contents as the original paged stream
         * implementation could do internally.
         */
        if (widgets->device_buffer == NULL && spec.capacity_bytes != 0u) {
            lt48_buffer *initial = NULL;
            if (lt48_buffer_create(
                    spec.capacity_bytes, 0xffu, &initial) == LT48_OK) {
                char length[32];
                widgets->device_buffer = initial;
                ++widgets->buffer_revision;
                gtk_editable_set_text(
                    GTK_EDITABLE(widgets->buffer_start_entry), "0");
                g_snprintf(length, sizeof(length), "%u", spec.capacity_bytes);
                gtk_editable_set_text(
                    GTK_EDITABLE(widgets->buffer_length_entry), length);
                buffer_show_range(
                    widgets,
                    "Initialized selected-device buffer to blank state",
                    0u, spec.capacity_bytes);
            }
        }
        uint32_t secondary_bytes_per_element =
            ((uint32_t)spec.secondary_element_bits + 7u) / 8u;
        uint32_t secondary_size = 0u;
        lt48_buffer_close(widgets->secondary_buffer);
        widgets->secondary_buffer = NULL;
        widgets->secondary_buffer_kind = SECONDARY_BUFFER_NONE;
        ++widgets->secondary_buffer_revision;
        if (secondary_bytes_per_element != 0u &&
            spec.secondary_element_count <=
                UINT32_MAX / secondary_bytes_per_element)
            secondary_size = spec.secondary_element_count *
                secondary_bytes_per_element;
        if (secondary_size != 0u &&
            lt48_buffer_create(
                secondary_size, 0xffu,
                &widgets->secondary_buffer) == LT48_OK)
            widgets->secondary_buffer_kind =
                selected_secondary_buffer_kind(widgets, &spec);
        if (widgets->secondary_buffer_kind == SECONDARY_BUFFER_NONE) {
            lt48_buffer_close(widgets->secondary_buffer);
            widgets->secondary_buffer = NULL;
        }
    }
    widgets->has_pending_device = 0;
    if (widgets->selection_accept_button != NULL)
        gtk_widget_set_sensitive(widgets->selection_accept_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->device_status),
                       "Device selected. Continue to Read / Operations.");
    {
        char capacity[32];
        g_snprintf(capacity, sizeof(capacity), "%u", spec.capacity_bytes);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->read_length_entry), capacity
        );
    }
    if (widgets->toolbar_vector_button != NULL)
        gtk_widget_set_sensitive(
            widgets->toolbar_vector_button,
            spec.catalogue.device_class >= 2019u &&
            spec.catalogue.device_class <= 2028u && spec.pin_count != 0u);
    if (widgets->window != NULL) {
        GAction *vector = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "vector");
        if (vector != NULL)
            g_simple_action_set_enabled(
                G_SIMPLE_ACTION(vector),
                spec.catalogue.device_class >= 2019u &&
                spec.catalogue.device_class <= 2028u &&
                spec.pin_count != 0u);
    }
    refresh_operation_dropdown(widgets);
    refresh_programming_parameters(widgets);
    restore_pending_project_operation_options(
        widgets, spec.catalogue.vendor, device_name);
    g_string_free(details, TRUE);
    g_free(device_name);
}

static void read_prepare_task_free(gpointer data) {
    read_prepare_task *task = data;
    if (task == NULL) return;
    prepared_read_clear(&task->request);
    g_free(task->plan_report);
    g_free(task);
}

static void read_prepare_result_free(gpointer data) {
    read_prepare_result *result = data;
    if (result == NULL) return;
    g_free(result->message);
    g_free(result);
}

static void read_prepare_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    read_prepare_task *work = task_data;
    prepared_read *request = &work->request;
    read_prepare_result *result = g_new0(read_prepare_result, 1);
    lt48_installation *installation = NULL;
    char plan_hex[LT48_SHA256_SIZE * 2u + 1u];
    char manifest_hex[LT48_SHA256_SIZE * 2u + 1u];
    char confirmation_hex[LT48_SHA256_SIZE * 2u + 1u];
    (void)source_object;
    result->input_revision = work->input_revision;
    result->status = g_cancellable_is_cancelled(cancellable) ?
                     LT48_CANCELLED :
                     lt48_installation_open(
                         request->installation_path, &installation);
    if (result->status == LT48_OK &&
        g_cancellable_is_cancelled(cancellable))
        result->status = LT48_CANCELLED;
    if (result->status == LT48_OK)
        result->status = build_prepared_read(installation, request);
    if (result->status == LT48_OK &&
        g_cancellable_is_cancelled(cancellable))
        result->status = LT48_CANCELLED;
    if (result->status == LT48_OK && work->plan_report != NULL &&
        work->plan_report[0] != '\0')
        result->status = lt48_m37_operation_report_write_planned(
            work->plan_report, installation, request->device_index,
            request->resources, request->plan, LT48_M37_READ,
            request->start, request->length, NULL);
    if (result->status == LT48_OK) {
        digest_hex(request->preparation.plan_sha256,
                   LT48_SHA256_SIZE, plan_hex);
        digest_hex(request->preparation.resource_manifest_sha256,
                   LT48_SHA256_SIZE, manifest_hex);
        digest_hex(request->preparation.confirmation_sha256,
                   LT48_SHA256_SIZE, confirmation_hex);
        g_strlcpy(result->confirmation, confirmation_hex,
                  sizeof(result->confirmation));
        result->message = g_strdup_printf(
            "OFFLINE DRY RUN — no USB opened\n"
            "%s · 0x%x + %u bytes · target %03u:%03u\n"
            "%zu transactions · %zu chunks · output %s\n"
            "Plan: %s\nManifest: %s\nConfirmation: %s\nReport: %s",
            request->selector, request->start, request->length,
            request->bus, request->address,
            request->preparation.transaction_count,
            request->preparation.read_chunk_count,
            request->output, plan_hex, manifest_hex, confirmation_hex,
            work->plan_report != NULL && work->plan_report[0] != '\0' ?
                work->plan_report : "not requested");
    } else if (result->status == LT48_CANCELLED) {
        result->message = g_strdup(
            "Read preparation cancelled before report commit; no USB opened");
    } else {
        result->message = result->status == LT48_FILE_ERROR ?
            g_strdup(
                "Cannot start Read: this catalogue directory lacks the "
                "live-operation support payload. Select the original AEC "
                "installation or a complete private cache.") :
            g_strdup_printf("Cannot start Read: %s",
                lt48_status_string(result->status));
    }
    lt48_installation_close(installation);
    g_task_return_pointer(task, result, read_prepare_result_free);
}

static void read_prepare_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    int execute_after_prepare = 0;
    read_prepare_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    (void)source_object;
    gtk_widget_set_sensitive(widgets->operation_prepare_button, TRUE);
    gtk_widget_set_sensitive(
        widgets->operation_prepare_cancel_button, FALSE);
    g_clear_object(&widgets->operation_prepare_cancellable);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_confirmation_entry), "");
    if (result != NULL &&
        result->input_revision != widgets->operation_input_revision) {
        gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Read preflight finished for superseded inputs; choose Read "
            "again");
    } else {
        gtk_widget_set_sensitive(
            widgets->read_execute_button,
            result != NULL && result->status == LT48_OK &&
            selected_operation_kind(widgets) == LT48_OPERATION_READ);
        if (result != NULL && result->status == LT48_OK)
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->read_confirmation_entry),
                result->confirmation);
        execute_after_prepare = widgets->read_after_prepare &&
            result != NULL && result->status == LT48_OK &&
            selected_operation_kind(widgets) == LT48_OPERATION_READ;
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            result == NULL ? "Read preparation failed without a result" :
                             result->message);
    }
    read_prepare_result_free(result);
    widgets->read_after_prepare = 0;
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    if (execute_after_prepare) execute_read(NULL, widgets);
}

static void cancel_operation_preparation(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    (void)button;
    if (widgets->operation_prepare_cancellable == NULL) return;
    g_cancellable_cancel(widgets->operation_prepare_cancellable);
    gtk_widget_set_sensitive(
        widgets->operation_prepare_cancel_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
        "Cancelling offline preparation at the next safe boundary…");
}

static void start_read_preparation(app_widgets *widgets) {
    read_prepare_task *work;
    GTask *task;
    uint32_t bus;
    uint32_t address;
    const char *output;
    const char *execution_report;
    const char *plan_report;
    if (widgets->operation_prepare_cancellable != NULL) return;
    if (widgets->installation == NULL || !widgets->has_selected_device) {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Select a device before preparing Read");
        return;
    }
    work = g_new0(read_prepare_task, 1);
    output = gtk_editable_get_text(GTK_EDITABLE(widgets->read_output_entry));
    execution_report = gtk_editable_get_text(
        GTK_EDITABLE(widgets->execution_report_entry));
    plan_report = gtk_editable_get_text(
        GTK_EDITABLE(widgets->operation_report_entry));
    if (!entry_u32(widgets->read_start_entry, UINT32_MAX,
                   &work->request.start) ||
        !entry_u32(widgets->read_length_entry, UINT32_MAX,
                   &work->request.length) ||
        !entry_u32(widgets->read_bus_entry, UINT8_MAX, &bus) ||
        !entry_u32(widgets->read_address_entry, UINT8_MAX, &address) ||
        !entry_u32(widgets->read_timeout_entry, UINT32_MAX,
                   &work->request.timeout) ||
        work->request.length == 0u || work->request.timeout == 0u ||
        output == NULL || output[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Enter a valid range, target, timeout, and output");
        read_prepare_task_free(work);
        return;
    }
    work->request.bus = (uint8_t)bus;
    work->request.address = (uint8_t)address;
    work->request.output = g_strdup(output);
    work->request.report = g_strdup(
        execution_report == NULL ? "" : execution_report);
    work->request.installation_path = g_strdup(
        lt48_installation_path(widgets->installation));
    work->request.device_index = widgets->selected_device_index;
    work->plan_report = g_strdup(plan_report == NULL ? "" : plan_report);
    work->input_revision = widgets->operation_input_revision;
    widgets->operation_prepare_cancellable = g_cancellable_new();
    gtk_widget_set_sensitive(widgets->operation_prepare_button, FALSE);
    gtk_widget_set_sensitive(
        widgets->operation_prepare_cancel_button, TRUE);
    gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_confirmation_entry), "");
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
        "Preparing Read outside the UI thread… No USB access is used.");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->operation_prepare_cancellable,
        read_prepare_finished);
    g_task_set_task_data(task, work, read_prepare_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, read_prepare_worker);
    g_object_unref(task);
}

static void operation_prepare_task_free(gpointer data) {
    operation_prepare_task *task = data;
    if (task == NULL) return;
    g_free(task->installation_path);
    g_free(task->image);
    g_free(task->image_path);
    g_free(task->image_source);
    g_free(task->report_path);
    g_free(task);
}

static operation_prepare_task *operation_prepare_task_clone(
    const operation_prepare_task *source
) {
    operation_prepare_task *copy;
    if (source == NULL) return NULL;
    copy = g_new0(operation_prepare_task, 1);
    *copy = *source;
    copy->installation_path = g_strdup(source->installation_path);
    copy->image = source->image_size == 0u ? NULL :
        g_memdup2(source->image, source->image_size);
    copy->image_path = g_strdup(source->image_path);
    copy->image_source = g_strdup(source->image_source);
    copy->report_path = g_strdup(source->report_path);
    if ((source->installation_path != NULL && copy->installation_path == NULL) ||
        (source->image_size != 0u && copy->image == NULL)) {
        operation_prepare_task_free(copy);
        return NULL;
    }
    return copy;
}

static void operation_execute_task_free(gpointer data) {
    operation_execute_task *task = data;
    if (task == NULL) return;
    operation_prepare_task_free(task->request);
    g_free(task);
}

static void operation_execute_result_free(gpointer data) {
    operation_execute_result *result = data;
    if (result == NULL) return;
    g_free(result->message);
    g_free(result);
}

static void operation_prepare_result_free(gpointer data) {
    operation_prepare_result *result = data;
    if (result == NULL) return;
    g_free(result->message);
    g_free(result);
}

static void operation_prepare_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    static const lt48_verify_margin_selection margin_values[] = {
        LT48_VERIFY_MARGIN_ALL, LT48_VERIFY_MARGIN_NOMINAL,
        LT48_VERIFY_MARGIN_LOW, LT48_VERIFY_MARGIN_HIGH
    };
    static const char *const program_phases[] = {
        "blank precheck", "mode-2 program", "mandatory nominal readback"
    };
    operation_prepare_task *work = task_data;
    operation_prepare_result *result = g_new0(operation_prepare_result, 1);
    lt48_installation *installation = NULL;
    lt48_m37_resources *resources = NULL;
    lt48_protocol_plan *plan = NULL;
    lt48_m37_verify_plan_set *verify_set = NULL;
    lt48_m37_program_plan_set *program_set = NULL;
    lt48_verify_voltage_profile voltage_profile;
    lt48_device_spec spec;
    lt48_buffer *image_buffer = NULL;
    lt48_image_import_result image_import;
    uint16_t verify_voltages[3];
    size_t verify_voltage_count = 0u;
    lt48_catalogue_entry entry;
    char *device_name = NULL;
    char *selector = NULL;
    char *loaded_image = NULL;
    gsize loaded_image_size = 0u;
    const unsigned char *expected = work->image;
    size_t expected_size = work->image_size;
    const char *image_source = work->image_source;
    char *image_description = NULL;
    unsigned char plan_digest[LT48_SHA256_SIZE];
    unsigned char image_digest[LT48_SHA256_SIZE];
    char plan_hex[LT48_SHA256_SIZE * 2u + 1u];
    char image_hex[LT48_SHA256_SIZE * 2u + 1u];
    lt48_status status;
    (void)source_object;
    result->operation = work->operation;
    result->input_revision = work->input_revision;
    result->buffer_revision = work->buffer_revision;
    result->uses_buffer = work->uses_buffer;
    status = g_cancellable_is_cancelled(cancellable) ? LT48_CANCELLED :
        lt48_installation_open(work->installation_path, &installation);
    if (status == LT48_OK && g_cancellable_is_cancelled(cancellable))
        status = LT48_CANCELLED;
    if (status == LT48_OK)
        status = lt48_installation_get_device(
            installation, work->device_index, &entry);
    if (status == LT48_OK)
        status = lt48_installation_resolve_device(
            installation, work->device_index, &spec);
    if (status == LT48_OK) {
        device_name = entry.device_name == NULL ? g_strdup("") :
            g_strndup((const char *)entry.device_name,
                      entry.device_name_length);
        selector = g_strdup_printf("%s:%s", entry.vendor, device_name);
        status = lt48_m37_resources_open(
            installation, work->device_index, &resources);
    }
    if (status == LT48_OK && g_cancellable_is_cancelled(cancellable))
        status = LT48_CANCELLED;
    if (status == LT48_OK && work->operation != LT48_OPERATION_READ &&
        work->operation != LT48_OPERATION_BLANK_CHECK &&
        expected == NULL) {
        lt48_image_import_options import_options = {
            work->image_format, spec.capacity_bytes,
            work->image_address_base, work->image_gap_policy,
            work->image_gap_fill
        };
        int range_known = 0;
        if (work->image_path == NULL || work->image_path[0] == '\0' ||
            spec.capacity_bytes == 0u) status = LT48_FILE_ERROR;
        else status = lt48_buffer_load_image_cancellable(
            work->image_path, &import_options, gio_cancelled, cancellable,
            &image_import, &image_buffer
        );
        if (status == LT48_OK &&
            ((size_t)work->start > lt48_buffer_size(image_buffer) ||
             (size_t)work->length >
                lt48_buffer_size(image_buffer) - (size_t)work->start))
            status = LT48_BUFFER_TOO_SMALL;
        if (status == LT48_OK)
            status = lt48_buffer_range_known(
                image_buffer, work->start, work->length, &range_known);
        if (status == LT48_OK && !range_known) status = LT48_SAFETY_REJECTED;
        if (status == LT48_OK) {
            loaded_image = g_memdup2(
                lt48_buffer_data(image_buffer) + work->start, work->length);
            if (loaded_image == NULL) status = LT48_NO_MEMORY;
            else {
                loaded_image_size = work->length;
                expected = (const unsigned char *)loaded_image;
                expected_size = loaded_image_size;
                g_free(work->image);
                work->image = g_memdup2(loaded_image, loaded_image_size);
                work->image_size = loaded_image_size;
                image_description = g_strdup_printf(
                    "%s (%s; source 0x%llx..0x%llx; %zu data bytes; "
                    "%zu gaps/%zu bytes %s)", work->image_path,
                    lt48_image_format_name(image_import.format),
                    (unsigned long long)image_import.minimum_source_address,
                    (unsigned long long)
                        image_import.maximum_source_address_exclusive,
                    image_import.mapped_data_byte_count,
                    image_import.gap_count, image_import.gap_byte_count,
                    work->image_gap_policy == LT48_IMAGE_GAPS_FILL ?
                        "filled" : "preserved");
                image_source = image_description;
            }
        }
    }
    if (status == LT48_OK && g_cancellable_is_cancelled(cancellable))
        status = LT48_CANCELLED;
    if (status == LT48_OK && work->operation == LT48_OPERATION_READ)
        status = lt48_m37_resources_build_offline_read_plan(
            resources, work->start, work->length, &plan);
    if (status == LT48_OK && work->operation == LT48_OPERATION_BLANK_CHECK)
        status = lt48_m37_resources_build_blank_check_plan(
            resources, work->start, work->length, &plan);
    if (status == LT48_OK && work->operation == LT48_OPERATION_VERIFY) {
        if (work->margin_choice >= G_N_ELEMENTS(margin_values) ||
            work->margin_type > 1u)
            status = LT48_INVALID_ARGUMENT;
        else
            status = lt48_installation_verify_voltage_profile(
                installation, work->device_index, work->margin_type,
                &voltage_profile);
        if (status == LT48_OK)
            status = lt48_verify_voltage_profile_select(
                &voltage_profile, margin_values[work->margin_choice],
                verify_voltages, G_N_ELEMENTS(verify_voltages),
                &verify_voltage_count);
        if (status == LT48_OK)
            status = lt48_m37_verify_plan_set_create(
                resources, work->start, expected, expected_size,
                verify_voltages, verify_voltage_count, &verify_set);
    }
    if (status == LT48_OK &&
        (work->operation == LT48_OPERATION_PROGRAM ||
         work->operation == LT48_OPERATION_OVER_PROGRAM))
        status = lt48_m37_program_plan_set_create(
            resources, work->start, expected, expected_size, &program_set);
    if (status == LT48_OK && work->operation == LT48_OPERATION_COMPARE)
        status = lt48_m37_resources_build_compare_plan(
            resources, work->start, expected, expected_size, &plan);
    if (status == LT48_OK && g_cancellable_is_cancelled(cancellable))
        status = LT48_CANCELLED;
    if (status == LT48_OK &&
        (work->operation == LT48_OPERATION_READ ||
         work->operation == LT48_OPERATION_BLANK_CHECK))
        status = lt48_protocol_plan_sha256(plan, plan_digest);
    if (status == LT48_OK && work->operation == LT48_OPERATION_VERIFY) {
        lt48_sha256(expected, expected_size, image_digest);
        status = lt48_m37_verify_plan_set_sha256(verify_set, plan_digest);
    }
    if (status == LT48_OK &&
        (work->operation == LT48_OPERATION_PROGRAM ||
         work->operation == LT48_OPERATION_OVER_PROGRAM)) {
        lt48_sha256(expected, expected_size, image_digest);
        status = lt48_m37_program_plan_set_sha256(program_set, plan_digest);
    }
    if (status == LT48_OK && work->operation == LT48_OPERATION_COMPARE) {
        lt48_sha256(expected, expected_size, image_digest);
        status = lt48_protocol_plan_sha256(plan, plan_digest);
    }
    if (status == LT48_OK && g_cancellable_is_cancelled(cancellable))
        status = LT48_CANCELLED;
    if (status == LT48_OK && work->report_path != NULL &&
        work->report_path[0] != '\0') {
        if (work->operation == LT48_OPERATION_PROGRAM ||
            work->operation == LT48_OPERATION_OVER_PROGRAM)
            status = lt48_m37_operation_report_write_program_plan_set(
                work->report_path, installation, work->device_index,
                resources, program_set, work->start, work->length,
                image_digest);
        else if (work->operation == LT48_OPERATION_VERIFY)
            status = lt48_m37_operation_report_write_verify_plan_set(
                work->report_path, installation, work->device_index,
                resources, verify_set, work->start, work->length,
                image_digest);
        else
            status = lt48_m37_operation_report_write_planned(
                work->report_path, installation, work->device_index,
                resources, plan,
                work->operation == LT48_OPERATION_COMPARE ?
                    LT48_M37_COMPARE :
                work->operation == LT48_OPERATION_READ ?
                    LT48_M37_READ : LT48_M37_BLANK_CHECK,
                work->start, work->length,
                work->operation == LT48_OPERATION_COMPARE ?
                    image_digest : NULL);
    }
    if (status == LT48_OK) {
        digest_hex(plan_digest, LT48_SHA256_SIZE, plan_hex);
        if (work->operation == LT48_OPERATION_READ) {
            result->message = g_strdup_printf(
                "OFFLINE READ PLAN — reference-matched; execution disabled\n"
                "%s · 0x%x + %u bytes · %zu transactions\nPlan: %s\n"
                "Report: %s\nNo programmer was opened. Controlled hardware "
                "validation is required before this engine may execute.",
                selector, work->start, work->length,
                lt48_protocol_plan_transaction_count(plan), plan_hex,
                work->report_path != NULL && work->report_path[0] != '\0' ?
                    work->report_path : "not requested");
        } else if (work->operation == LT48_OPERATION_VERIFY) {
            GString *passes = g_string_new(NULL);
            digest_hex(image_digest, LT48_SHA256_SIZE, image_hex);
            for (size_t index = 0u; index < verify_voltage_count; ++index) {
                const lt48_protocol_plan *pass = NULL;
                unsigned char pass_digest[LT48_SHA256_SIZE];
                char pass_hex[LT48_SHA256_SIZE * 2u + 1u];
                uint16_t voltage = 0u;
                (void)lt48_m37_verify_plan_set_get(
                    verify_set, index, &voltage, &pass);
                (void)lt48_protocol_plan_sha256(pass, pass_digest);
                digest_hex(pass_digest, LT48_SHA256_SIZE, pass_hex);
                g_string_append_printf(
                    passes, "  %.2f V · %zu transactions · %s\n",
                    voltage / 100.0,
                    lt48_protocol_plan_transaction_count(pass), pass_hex);
            }
            result->message = g_strdup_printf(
                "READY TO VERIFY — audited W16 workflow\n"
                "%s · 0x%x + %u bytes · %zu independent pass%s\n%s"
                "Image: %s\nImage SHA-256: %s\nCombined plan: %s\n"
                "Report: %s\nPress Execute to run the selected voltage passes.",
                selector, work->start, work->length, verify_voltage_count,
                verify_voltage_count == 1u ? "" : "es", passes->str,
                image_source, image_hex, plan_hex,
                work->report_path != NULL && work->report_path[0] != '\0' ?
                    work->report_path : "not requested");
            g_string_free(passes, TRUE);
        } else if (work->operation == LT48_OPERATION_PROGRAM ||
                   work->operation == LT48_OPERATION_OVER_PROGRAM) {
            GString *phases = g_string_new(NULL);
            for (size_t index = 0u; index < 3u; ++index) {
                const lt48_protocol_plan *phase = NULL;
                lt48_m37_operation phase_operation;
                unsigned char phase_digest[LT48_SHA256_SIZE];
                char phase_hex[LT48_SHA256_SIZE * 2u + 1u];
                (void)lt48_m37_program_plan_set_get(
                    program_set, index, &phase_operation, &phase);
                (void)phase_operation;
                (void)lt48_protocol_plan_sha256(phase, phase_digest);
                digest_hex(phase_digest, LT48_SHA256_SIZE, phase_hex);
                g_string_append_printf(
                    phases, "  %s · %zu transactions · %s\n",
                    program_phases[index],
                    lt48_protocol_plan_transaction_count(phase), phase_hex);
            }
            digest_hex(image_digest, LT48_SHA256_SIZE, image_hex);
            result->message = g_strdup_printf(
                "READY TO %s — audited M37 workflow\n"
                "%s · 0x%x + %u bytes\n%sImage: %s\nImage SHA-256: %s\n"
                "Combined plan: %s\nSelected-device Auto-ID is mandatory unless "
                "the per-operation override was captured during preparation. "
                "A blank precheck and readback verification are separate "
                "powered cycles.\nReport: %s",
                work->operation == LT48_OPERATION_OVER_PROGRAM ?
                    "OVER-PROGRAM" : "PROGRAM",
                selector, work->start, work->length, phases->str,
                image_source, image_hex, plan_hex,
                work->report_path != NULL && work->report_path[0] != '\0' ?
                    work->report_path : "not requested");
            g_string_free(phases, TRUE);
        } else if (work->operation == LT48_OPERATION_COMPARE) {
            digest_hex(image_digest, LT48_SHA256_SIZE, image_hex);
            result->message = g_strdup_printf(
                "READY TO COMPARE — audited mode-5 workflow\n"
                "%s · 0x%x + %u bytes · %zu transactions\n"
                "Image: %s\nImage SHA-256: %s\nPlan: %s\nReport: %s\n"
                "Mode 5 reads the selected range and compares it on the host.",
                selector, work->start, work->length,
                lt48_protocol_plan_transaction_count(plan), image_source,
                image_hex, plan_hex,
                work->report_path != NULL && work->report_path[0] != '\0' ?
                    work->report_path : "not requested");
        } else {
            result->message = g_strdup_printf(
                "READY TO BLANK CHECK — audited W14 workflow\n"
                "%s · 0x%x + %u bytes · blank value 0xff · "
                "%zu transactions\nPlan: %s\nReport: %s\nPress Execute to run.",
                selector, work->start, work->length,
                lt48_protocol_plan_transaction_count(plan), plan_hex,
                work->report_path != NULL && work->report_path[0] != '\0' ?
                    work->report_path : "not requested");
        }
    } else if (status == LT48_CANCELLED) {
        result->message = g_strdup(
            "Offline operation preparation cancelled before report commit; "
            "no USB opened");
    } else {
        result->message = g_strdup_printf(
            "Cannot start operation: %s%s", lt48_status_string(status),
            status == LT48_BUFFER_TOO_SMALL ?
                " (image size must equal the selected range)" : "");
    }
    result->status = status;
    g_free(loaded_image);
    g_free(image_description);
    g_free(device_name);
    g_free(selector);
    lt48_m37_program_plan_set_close(program_set);
    lt48_m37_verify_plan_set_close(verify_set);
    lt48_protocol_plan_close(plan);
    lt48_buffer_close(image_buffer);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    g_task_return_pointer(task, result, operation_prepare_result_free);
}

static void operation_prepare_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    operation_prepare_task *prepared = operation_prepare_task_clone(
        g_task_get_task_data(G_TASK(async_result)));
    operation_prepare_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    int current = result != NULL && result->status == LT48_OK &&
        result->input_revision == widgets->operation_input_revision &&
        (!result->uses_buffer ||
         result->buffer_revision == widgets->buffer_revision);
    int execute_after_prepare = current && widgets->read_after_prepare;
    (void)source_object;
    gtk_widget_set_sensitive(widgets->operation_prepare_button, TRUE);
    gtk_widget_set_sensitive(
        widgets->operation_prepare_cancel_button, FALSE);
    g_clear_object(&widgets->operation_prepare_cancellable);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_confirmation_entry), "");
    operation_prepare_task_free(widgets->prepared_operation);
    widgets->prepared_operation = current ? prepared : NULL;
    if (!current) operation_prepare_task_free(prepared);
    gtk_widget_set_sensitive(widgets->read_execute_button, current);
    gtk_label_set_text(
        GTK_LABEL(widgets->read_preparation),
        result != NULL &&
        (result->input_revision != widgets->operation_input_revision ||
         (result->uses_buffer &&
          result->buffer_revision != widgets->buffer_revision)) ?
            "Operation preflight finished for superseded inputs; choose "
            "the operation again" :
        result == NULL ?
            "Offline operation preparation failed without a result" :
            result->message);
    operation_prepare_result_free(result);
    widgets->read_after_prepare = 0;
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    if (execute_after_prepare) execute_prepared_operation(widgets);
}

static void start_operation_preparation(
    app_widgets *widgets, lt48_operation_kind operation
) {
    operation_prepare_task *work;
    GTask *task;
    const char *image_path;
    const char *report_path;
    int use_buffer;
    if (widgets->operation_prepare_cancellable != NULL) return;
    if (widgets->installation == NULL || !widgets->has_selected_device) {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Select a device and enter a valid range");
        return;
    }
    work = g_new0(operation_prepare_task, 1);
    work->operation = operation;
    if (!entry_u32(widgets->read_start_entry, UINT32_MAX, &work->start) ||
        !entry_u32(widgets->read_length_entry, UINT32_MAX, &work->length) ||
        work->length == 0u) {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Select a device and enter a valid range");
        operation_prepare_task_free(work);
        return;
    }
    work->installation_path = g_strdup(
        lt48_installation_path(widgets->installation));
    work->device_index = widgets->selected_device_index;
    work->margin_choice = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->verify_margin_dropdown));
    work->margin_type = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->verify_margin_type_dropdown));
    work->selected_device_override = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->program_identity_override_check));
    work->input_revision = widgets->operation_input_revision;
    report_path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->operation_report_entry));
    work->report_path = g_strdup(report_path == NULL ? "" : report_path);
    use_buffer = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->verify_use_buffer_check));
    image_path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->verify_image_entry));
    if (operation != LT48_OPERATION_READ &&
        operation != LT48_OPERATION_BLANK_CHECK) {
        if (use_buffer) {
            size_t buffer_size;
            int range_known = 0;
            if (widgets->device_buffer == NULL) {
                gtk_label_set_text(
                    GTK_LABEL(widgets->read_preparation),
                    "Cannot start operation: no resident buffer image");
                operation_prepare_task_free(work);
                return;
            }
            buffer_size = lt48_buffer_size(widgets->device_buffer);
            if ((size_t)work->start > buffer_size ||
                (size_t)work->length > buffer_size - (size_t)work->start) {
                gtk_label_set_text(
                    GTK_LABEL(widgets->read_preparation),
                    "Cannot start operation: buffer too small (image size "
                    "must equal the selected range)");
                operation_prepare_task_free(work);
                return;
            }
            if (lt48_buffer_range_known(
                    widgets->device_buffer, work->start, work->length,
                    &range_known) != LT48_OK || !range_known) {
                gtk_label_set_text(
                    GTK_LABEL(widgets->read_preparation),
                    "Cannot start operation: the selected resident-buffer range "
                    "contains bytes that have not been read or explicitly "
                    "written");
                operation_prepare_task_free(work);
                return;
            }
            work->image = g_memdup2(
                lt48_buffer_data(widgets->device_buffer) + work->start,
                work->length);
            work->image_size = work->length;
            work->image_source = g_strdup("resident buffer");
            work->uses_buffer = 1;
            work->buffer_revision = widgets->buffer_revision;
        } else {
            uint32_t fill_value;
            guint format_choice = gtk_drop_down_get_selected(
                GTK_DROP_DOWN(widgets->image_format_dropdown));
            guint gap_choice = gtk_drop_down_get_selected(
                GTK_DROP_DOWN(widgets->image_gap_dropdown));
            if (image_path == NULL || image_path[0] == '\0') {
                gtk_label_set_text(
                    GTK_LABEL(widgets->read_preparation),
                    "Enter an exact-size image or select Use resident buffer");
                operation_prepare_task_free(work);
                return;
            }
            if (format_choice > LT48_IMAGE_FORMAT_HP64000_ABSOLUTE ||
                gap_choice > 1u ||
                !entry_u64(widgets->image_base_entry,
                           &work->image_address_base) ||
                !entry_u32(widgets->image_gap_fill_entry, UINT8_MAX,
                           &fill_value)) {
                gtk_label_set_text(
                    GTK_LABEL(widgets->read_preparation),
                    "Enter valid image format, address base, and gap value");
                operation_prepare_task_free(work);
                return;
            }
            work->image_format = (lt48_image_format)format_choice;
            work->image_gap_policy = gap_choice == 0u ?
                LT48_IMAGE_GAPS_PRESERVE : LT48_IMAGE_GAPS_FILL;
            work->image_gap_fill = (unsigned char)fill_value;
            work->image_path = g_strdup(image_path);
        }
    }
    widgets->operation_prepare_cancellable = g_cancellable_new();
    gtk_widget_set_sensitive(widgets->operation_prepare_button, FALSE);
    gtk_widget_set_sensitive(
        widgets->operation_prepare_cancel_button, TRUE);
    gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_confirmation_entry), "");
    gtk_label_set_text(
        GTK_LABEL(widgets->read_preparation),
        "Preparing operation outside the UI thread… No USB access is used.");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->operation_prepare_cancellable,
        operation_prepare_finished);
    g_task_set_task_data(task, work, operation_prepare_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, operation_prepare_worker);
    g_object_unref(task);
}

static void prepare_read(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    char *message;
    char *report;
    const char *output;
    lt48_status status;
    lt48_operation_kind operation = selected_operation_kind(widgets);
    (void)button;
    widgets->read_after_prepare = 0;
    if (operation != LT48_OPERATION_READ &&
        operation != LT48_OPERATION_BLANK_CHECK &&
        operation != LT48_OPERATION_VERIFY &&
        operation != LT48_OPERATION_PROGRAM &&
        operation != LT48_OPERATION_OVER_PROGRAM &&
        operation != LT48_OPERATION_COMPARE) {
        lt48_operation_support support;
        if (widgets->installation == NULL || !widgets->has_selected_device) {
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                               "Select a device before inspecting operation support");
            return;
        }
        status = lt48_installation_operation_support_one(
            widgets->installation, widgets->selected_device_index,
            operation, &support
        );
        if (status == LT48_OK) {
            message = g_strdup_printf(
                "%s — %s (%s)\n%s\n%s",
                support.name, operation_availability_label(support.availability),
                support.support_level, support.reason,
                support.availability == LT48_OPERATION_HOST_ONLY ?
                    "This operation is host-only; use the resident-buffer "
                    "commands. No USB opened." :
                    "No safe native execution plan is available. No USB opened."
            );
        } else {
            message = g_strdup_printf("Cannot inspect operation support: %s",
                                      lt48_status_string(status));
        }
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->read_confirmation_entry), ""
        );
        gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
        g_free(message);
        return;
    }
    if (operation == LT48_OPERATION_READ) {
        lt48_operation_support support;
        if (widgets->installation == NULL || !widgets->has_selected_device) {
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                               "Select a device before preparing Read");
            return;
        }
        status = lt48_installation_operation_support_one(
            widgets->installation, widgets->selected_device_index,
            operation, &support
        );
        if (status != LT48_OK) {
            message = g_strdup_printf(
                "Cannot inspect Read support: %s",
                lt48_status_string(status));
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
            g_free(message);
            return;
        }
        if (support.availability == LT48_OPERATION_PLAN_ONLY) {
            start_operation_preparation(widgets, operation);
            return;
        }
        if (support.availability != LT48_OPERATION_EXECUTABLE) {
            message = g_strdup_printf(
                "Read — %s (%s)\n%s\nNo USB opened.",
                operation_availability_label(support.availability),
                support.support_level, support.reason);
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
            g_free(message);
            return;
        }
        if (!widgets->programmer_ready) {
            widgets->pending_hardware_action =
                PENDING_HARDWARE_PREPARE_READ;
            gtk_label_set_text(
                GTK_LABEL(widgets->read_preparation),
                "Programmer was not ready; probing and connecting now…");
            refresh_programmers(NULL, widgets);
            return;
        }
        output = gtk_editable_get_text(
            GTK_EDITABLE(widgets->read_output_entry));
        report = g_strdup_printf(
            "%s.report.json",
            output == NULL || output[0] == '\0' ? "read.bin" : output);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->execution_report_entry), report);
        g_free(report);
        widgets->read_after_prepare = 1;
        start_read_preparation(widgets);
    } else {
        widgets->read_after_prepare = 1;
        start_operation_preparation(widgets, operation);
    }
}
static int gio_cancelled(void *context) {
    return g_cancellable_is_cancelled(G_CANCELLABLE(context));
}

static void read_task_free(gpointer data) {
    read_task *task = data;
    if (task == NULL) return;
    prepared_read_clear(&task->request);
    g_free(task);
}

static void read_task_result_free(gpointer data) {
    read_task_result *result = data;
    if (result == NULL) return;
    g_free(result->message);
    lt48_buffer_close(result->buffer);
    g_free(result);
}

static void execute_read_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    read_task *work = task_data;
    app_widgets *widgets = work->widgets;
    prepared_read *request = &work->request;
    lt48_m37_read_collector *collector = NULL;
    lt48_context *context = NULL;
    lt48_programmer *programmer = NULL;
    lt48_transport_info ready = {0};
    lt48_cleanup_result finalization[12] = {{0}};
    lt48_m37_snapshot_read_execution_result execution = {0};
    const unsigned char *read_data = NULL;
    size_t read_size = 0u;
    uint32_t read_start = 0u;
    GFile *file = NULL;
    GFileOutputStream *stream = NULL;
    GError *error = NULL;
    char *message = NULL;
    lt48_buffer *retained_buffer = NULL;
    lt48_status status;
    int created_output = 0;
    int execution_attempted = 0;
    (void)source_object;
    status = g_cancellable_is_cancelled(cancellable) ?
             LT48_CANCELLED :
             lt48_m37_read_collector_create(request->plan, &collector);
    if (status == LT48_OK) status = lt48_context_create(&context);
    if (status == LT48_OK) {
        status = lt48_programmer_open(
            context, request->bus, request->address, &programmer
        );
        if (status == LT48_OK) {
            g_mutex_lock(&widgets->progress_mutex);
            widgets->active_programmer = programmer;
            g_mutex_unlock(&widgets->progress_mutex);
        }
    }
    if (status == LT48_OK) {
        status = lt48_programmer_transport_initialize(
            programmer, (unsigned)request->timeout, &ready
        );
    }
    if (status == LT48_OK && g_cancellable_is_cancelled(cancellable)) {
        status = LT48_CANCELLED;
    }
    if (status == LT48_OK) {
        status = lt48_programmer_set_cancel_check(
            programmer, gio_cancelled, cancellable
        );
    }
    if (status == LT48_OK) {
        status = lt48_programmer_arm_m37_read(
            programmer, request->resources,
            LT48_M37_READ_HARDWARE_AUTHORIZATION
        );
    }
    if (status == LT48_OK) {
        execution_attempted = 1;
        status = lt48_programmer_execute_m37_read(
            programmer, request->resources, request->plan, collector,
            (unsigned)request->timeout, finalization,
            sizeof(finalization) / sizeof(finalization[0]), &execution
        );
    }
    if (status == LT48_OK) {
        status = lt48_m37_read_collector_data(
            collector, &read_start, &read_data, &read_size
        );
        if (status == LT48_OK &&
            (read_start != request->start || read_size != request->length)) {
            status = LT48_PROTOCOL_ERROR;
        }
    }
    if (status == LT48_OK) {
        status = lt48_buffer_create(read_size, 0u, &retained_buffer);
        if (status == LT48_OK)
            status = lt48_buffer_write(
                retained_buffer, 0u, read_data, read_size
            );
    }
    if (status != LT48_OK) {
        message = g_strdup_printf(
            "Read failed: %s\nOperation: %s (%zu transactions) · "
            "finalization: %s · recovery: %s · failures: %zu",
            lt48_status_string(status),
            lt48_status_string(execution.operation_status),
            execution.operation_completed_transactions,
            lt48_status_string(execution.finalization_status),
            execution.used_recovery_plan ? "used" : "not used",
            execution.finalization_failure_count
        );
        goto done;
    }
    file = g_file_new_for_path(request->output);
    stream = request->overwrite_output ?
        g_file_replace(file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error) :
        g_file_create(file, G_FILE_CREATE_NONE, NULL, &error);
    if (stream == NULL) {
        status = LT48_FILE_ERROR;
        message = g_strdup_printf("Read succeeded but output was not created: %s",
                                  error->message);
        goto done;
    }
    created_output = !request->overwrite_output;
    if (!g_output_stream_write_all(
            G_OUTPUT_STREAM(stream), read_data, read_size, NULL, NULL,
            &error) ||
        !g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, &error)) {
        status = LT48_FILE_ERROR;
        message = g_strdup_printf("Output write failed: %s", error->message);
        goto done;
    }
    g_clear_object(&stream);
    message = g_strdup_printf(
        "READ COMPLETE — %zu bytes written exclusively to %s\n"
        "Read range 0x%x..0x%x is also retained in the resident buffer.\n"
        "Readiness acknowledgements %02x/%02x · operation and cleanup "
        "succeeded · no recovery used",
        read_size, request->output, read_start,
        (uint32_t)(read_start + read_size - 1u),
        ready.packet_size_ack, ready.mode_ack
    );

done:
    if (stream != NULL) {
        (void)g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, NULL);
        g_clear_object(&stream);
    }
    if (error != NULL && created_output && file != NULL)
        (void)g_file_delete(file, NULL, NULL);
    if (execution_attempted) {
        lt48_installation *report_installation = NULL;
        if (request->overwrite_output) {
            GFile *old_report = g_file_new_for_path(request->report);
            if (g_file_query_exists(old_report, NULL))
                (void)g_file_delete(old_report, NULL, NULL);
            g_object_unref(old_report);
        }
        lt48_status report_status = lt48_installation_open(
            request->installation_path, &report_installation
        );
        if (execution.finalization_phase_count == 0u &&
            execution.operation_status == LT48_OK && status != LT48_OK)
            execution.operation_status = status;
        if (report_status == LT48_OK)
            report_status = lt48_m37_operation_report_write_execution(
                request->report, report_installation, request->device_index,
                request->resources, request->plan, request->start,
                request->length, status, &execution, finalization,
                execution.finalization_phase_count,
                read_data, read_data == NULL ? 0u : read_size
            );
        lt48_installation_close(report_installation);
        if (report_status != LT48_OK) {
            char *with_report = g_strdup_printf(
                "%s\nExecution report was not written: %s",
                message == NULL ? "Read failed" : message,
                lt48_status_string(report_status)
            );
            g_free(message);
            message = with_report;
        } else {
            char *with_report = g_strdup_printf(
                "%s\nReport: %s",
                message == NULL ? "Read failed" : message,
                request->report
            );
            g_free(message);
            message = with_report;
        }
    }
    g_clear_error(&error);
    g_clear_object(&file);
    g_mutex_lock(&widgets->progress_mutex);
    if (widgets->active_programmer == programmer)
        widgets->active_programmer = NULL;
    lt48_programmer_close(programmer);
    g_mutex_unlock(&widgets->progress_mutex);
    lt48_context_destroy(context);
    lt48_m37_read_collector_close(collector);
    {
        read_task_result *outcome = g_new0(read_task_result, 1);
        if (retained_buffer != NULL && status != LT48_OK) {
            char *with_retention = g_strdup_printf(
                "%s\nCaptured range 0x%x + %zu bytes remains available in "
                "the resident buffer for another save attempt.",
                message == NULL ? "Initial save failed" : message,
                read_start, read_size
            );
            g_free(message);
            message = with_retention;
        }
        outcome->message = message == NULL ? g_strdup("Read failed") : message;
        outcome->status = status;
        outcome->buffer = retained_buffer;
        outcome->device_capacity = request->capacity;
        outcome->device_start = read_start;
        outcome->size = read_size;
        g_task_return_pointer(task, outcome, read_task_result_free);
    }
}

static gboolean poll_read_progress(gpointer user_data) {
    app_widgets *widgets = user_data;
    lt48_execution_progress progress = {0};
    lt48_status status = LT48_NOT_FOUND;
    char *message = NULL;
    g_mutex_lock(&widgets->progress_mutex);
    if (widgets->active_programmer != NULL)
        status = lt48_programmer_get_progress(
            widgets->active_programmer, &progress
        );
    g_mutex_unlock(&widgets->progress_mutex);
    if (status != LT48_OK || progress.stage == LT48_EXECUTION_IDLE)
        return widgets->read_cancellable == NULL ?
               G_SOURCE_REMOVE : G_SOURCE_CONTINUE;
    switch (progress.stage) {
    case LT48_EXECUTION_READ_OPERATION:
        message = g_strdup_printf(
            "%s running — transaction %zu/%zu. Cancel is cooperative; "
            "mandatory cleanup remains unconditional.",
            lt48_operation_kind_name(selected_operation_kind(widgets)),
            progress.completed_transactions, progress.total_transactions
        );
        break;
    case LT48_EXECUTION_READ_RECOVERY: {
        const char *phase = lt48_m37_cleanup_phase_name(
            (lt48_m37_cleanup_phase)progress.cleanup_phase
        );
        message = g_strdup_printf(
            "Mandatory recovery %zu/%zu: %s — transaction %zu/%zu. "
            "Cancellation is suppressed until cleanup completes.",
            progress.cleanup_phase_index, progress.cleanup_phase_count,
            phase == NULL ? "unknown phase" : phase,
            progress.completed_transactions, progress.total_transactions
        );
        break;
    }
    case LT48_EXECUTION_INTERFACE_RELEASE:
        message = g_strdup_printf(
            "Releasing programmer interface (%zu/%zu cleanup phases).",
            progress.cleanup_phase_index, progress.cleanup_phase_count
        );
        break;
    case LT48_EXECUTION_COMPLETE:
        message = g_strdup("Hardware owner completed; collecting final result…");
        break;
    case LT48_EXECUTION_AUTO_ID_OPERATION:
        message = g_strdup_printf(
            "EPROM Auto-ID running — command %zu/%zu.",
            progress.completed_transactions, progress.total_transactions
        );
        break;
    case LT48_EXECUTION_AUTO_ID_FINALIZATION:
        message = g_strdup(
            "EPROM Auto-ID controller finalizer running; cancellation is suppressed."
        );
        break;
    case LT48_EXECUTION_IDLE:
    default:
        break;
    }
    if (message != NULL) {
        if (widgets->operation_progress_status != NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->operation_progress_status), message);
        if (widgets->operation_progress_bar != NULL) {
            GtkProgressBar *bar = GTK_PROGRESS_BAR(
                widgets->operation_progress_bar);
            if (progress.stage == LT48_EXECUTION_COMPLETE) {
                gtk_progress_bar_set_fraction(bar, 1.0);
                gtk_progress_bar_set_text(bar, "Complete");
            } else if ((progress.stage == LT48_EXECUTION_READ_RECOVERY ||
                        progress.stage == LT48_EXECUTION_INTERFACE_RELEASE ||
                        progress.stage ==
                            LT48_EXECUTION_AUTO_ID_FINALIZATION) &&
                       progress.cleanup_phase_count != 0u) {
                double fraction = (double)progress.cleanup_phase_index /
                    (double)progress.cleanup_phase_count;
                gtk_progress_bar_set_fraction(bar, MIN(fraction, 1.0));
                gtk_progress_bar_set_text(bar, "Cleanup");
            } else if (progress.total_transactions != 0u) {
                double fraction = (double)progress.completed_transactions /
                    (double)progress.total_transactions;
                char *progress_text = g_strdup_printf(
                    "%zu / %zu", progress.completed_transactions,
                    progress.total_transactions);
                gtk_progress_bar_set_fraction(bar, MIN(fraction, 1.0));
                gtk_progress_bar_set_text(bar, progress_text);
                g_free(progress_text);
            } else {
                gtk_progress_bar_pulse(bar);
                gtk_progress_bar_set_text(bar, "Working…");
            }
        }
        g_free(message);
    }
    return widgets->read_cancellable == NULL ?
           G_SOURCE_REMOVE : G_SOURCE_CONTINUE;
}

static void statistics_update_labels(app_widgets *widgets) {
    char text[32];
    if (widgets->footer_current_count != NULL) {
        g_snprintf(text, sizeof(text), "%llu",
                   (unsigned long long)widgets->statistic_current_count);
        gtk_label_set_text(GTK_LABEL(widgets->footer_current_count), text);
    }
    if (widgets->footer_target_count != NULL) {
        g_snprintf(text, sizeof(text), "%u", widgets->statistic_target_count);
        gtk_label_set_text(GTK_LABEL(widgets->footer_target_count), text);
    }
    if (widgets->footer_current_failure != NULL) {
        g_snprintf(text, sizeof(text), "%llu",
                   (unsigned long long)widgets->statistic_current_failure);
        gtk_label_set_text(GTK_LABEL(widgets->footer_current_failure), text);
    }
    if (widgets->footer_max_failure != NULL) {
        g_snprintf(text, sizeof(text), "%u", widgets->statistic_max_failure);
        gtk_label_set_text(GTK_LABEL(widgets->footer_max_failure), text);
    }
    if (widgets->footer_statistics_toggle != NULL)
        gtk_button_set_label(
            GTK_BUTTON(widgets->footer_statistics_toggle),
            widgets->statistic_enabled ? "On" : "Off");
}

static void statistics_record(app_widgets *widgets, lt48_status status) {
    const char *alarm = NULL;
    if (!widgets->statistic_enabled || status == LT48_CANCELLED) return;
    if (status == LT48_OK)
        ++widgets->statistic_current_count;
    else
        ++widgets->statistic_current_failure;
    statistics_update_labels(widgets);
    if (!widgets->statistic_alarm_enabled) return;
    if (widgets->statistic_target_count != 0u &&
        widgets->statistic_current_count >= widgets->statistic_target_count)
        alarm = "Statistic alarm: target count reached.";
    if (widgets->statistic_max_failure != 0u &&
        widgets->statistic_current_failure >= widgets->statistic_max_failure)
        alarm = "Statistic alarm: maximum failure count reached.";
    if (alarm != NULL) {
        if (widgets->statistics_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->statistics_status), alarm);
        if (widgets->read_preparation != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->read_preparation), alarm);
    }
}

static void execute_read_finished(
    GObject *source_object, GAsyncResult *result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    read_task_result *outcome = g_task_propagate_pointer(
        G_TASK(result), &error
    );
    char *fallback = NULL;
    (void)source_object;
    if (outcome == NULL) {
        fallback = g_strdup_printf("Read task failed: %s",
                                   error == NULL ? "unknown error" :
                                                   error->message);
    } else if (outcome->buffer != NULL) {
        char size_text[32];
        lt48_buffer *target = widgets->device_buffer;
        lt48_status merge_status = LT48_OK;
        buffer_edit_discard_undo(widgets);
        if (target == NULL ||
            lt48_buffer_size(target) != outcome->device_capacity) {
            lt48_buffer *replacement = NULL;
            merge_status = lt48_buffer_create_sparse(
                outcome->device_capacity, &replacement);
            if (merge_status == LT48_OK) {
                lt48_buffer_close(target);
                widgets->device_buffer = replacement;
                target = replacement;
            }
        }
        if (merge_status == LT48_OK)
            merge_status = lt48_buffer_write_acquired(
                target, outcome->device_start,
                lt48_buffer_data(outcome->buffer), outcome->size);
        if (merge_status == LT48_OK) {
            char *action = g_strdup_printf(
                "Captured device range 0x%x + %zu bytes at its absolute "
                "offset; %zu/%u device bytes are now known",
                outcome->device_start, outcome->size,
                lt48_buffer_known_size(target), outcome->device_capacity);
            ++widgets->buffer_revision;
            invalidate_operation_preparation(widgets, 0);
            g_snprintf(size_text, sizeof(size_text), "%u",
                       outcome->device_start);
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->buffer_start_entry), size_text);
            g_snprintf(size_text, sizeof(size_text), "%zu", outcome->size);
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->buffer_length_entry), size_text);
            buffer_show_range(
                widgets, action, outcome->device_start,
                (uint32_t)outcome->size);
            if (widgets->verify_use_buffer_check != NULL &&
                lt48_buffer_known_size(target) == lt48_buffer_size(target))
                gtk_check_button_set_active(
                    GTK_CHECK_BUTTON(widgets->verify_use_buffer_check), TRUE);
            g_free(action);
        } else {
            char *with_merge = g_strdup_printf(
                "%s\nCaptured bytes could not be merged into the device "
                "buffer: %s",
                outcome->message, lt48_status_string(merge_status));
            g_free(outcome->message);
            outcome->message = with_merge;
        }
    }
    gtk_label_set_text(
        GTK_LABEL(widgets->read_preparation),
        outcome == NULL ? fallback : outcome->message
    );
    if (outcome != NULL) statistics_record(widgets, outcome->status);
    g_free(fallback);
    read_task_result_free(outcome);
    g_clear_error(&error);
    if (widgets->progress_source_id != 0u) {
        g_source_remove(widgets->progress_source_id);
        widgets->progress_source_id = 0u;
    }
    g_clear_object(&widgets->read_cancellable);
    operation_progress_finish(widgets);
    gtk_widget_set_sensitive(widgets->operation_prepare_button, TRUE);
    gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->read_confirmation_entry), "");
    gtk_widget_set_sensitive(widgets->read_cancel_button, FALSE);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)
    ));
}

static lt48_status execute_operation_phase(
    operation_execute_task *work, lt48_m37_resources *resources,
    const lt48_protocol_plan *plan, lt48_m37_operation operation,
    lt48_m37_read_collector *collector,
    lt48_m37_snapshot_read_execution_result *execution
) {
    lt48_context *context = NULL;
    lt48_programmer *programmer = NULL;
    lt48_transport_info ready = {0};
    lt48_cleanup_result finalization[12] = {{0}};
    lt48_status status = lt48_context_create(&context);
    if (status == LT48_OK)
        status = lt48_programmer_open(
            context, work->bus, work->address, &programmer);
    if (status == LT48_OK)
        status = lt48_programmer_transport_initialize(
            programmer, work->timeout, &ready);
    if (status == LT48_OK)
        status = lt48_programmer_set_cancel_check(
            programmer, gio_cancelled, work->widgets->read_cancellable);
    if (status == LT48_OK)
        status = lt48_programmer_arm_m37_operation(
            programmer, resources, plan, operation,
            LT48_M37_OPERATION_HARDWARE_AUTHORIZATION);
    if (status == LT48_OK) {
        g_mutex_lock(&work->widgets->progress_mutex);
        work->widgets->active_programmer = programmer;
        g_mutex_unlock(&work->widgets->progress_mutex);
        status = lt48_programmer_execute_m37_operation(
            programmer, resources, plan, operation, collector,
            work->timeout, finalization,
            G_N_ELEMENTS(finalization), execution);
    }
    g_mutex_lock(&work->widgets->progress_mutex);
    if (work->widgets->active_programmer == programmer)
        work->widgets->active_programmer = NULL;
    lt48_programmer_close(programmer);
    g_mutex_unlock(&work->widgets->progress_mutex);
    lt48_context_destroy(context);
    return status;
}

static lt48_status execute_selected_device_check(
    operation_execute_task *work, lt48_installation *installation,
    size_t selected_device
) {
    lt48_auto_id_eprom_preparation preparation;
    lt48_auto_id_eprom_execution_result execution = {0};
    lt48_auto_id_candidate *candidates = NULL;
    lt48_context *context = NULL;
    lt48_programmer *programmer = NULL;
    lt48_transport_info ready = {0};
    size_t candidate_count = 0u;
    lt48_status status = lt48_auto_id_eprom_prepare(
        work->bus, work->address, 1, 0, 0u, &preparation);
    if (status == LT48_OK) status = lt48_context_create(&context);
    if (status == LT48_OK)
        status = lt48_programmer_open(
            context, work->bus, work->address, &programmer);
    if (status == LT48_OK)
        status = lt48_programmer_transport_initialize(
            programmer, work->timeout, &ready);
    if (status == LT48_OK)
        status = lt48_programmer_set_cancel_check(
            programmer, gio_cancelled, work->widgets->read_cancellable);
    if (status == LT48_OK)
        status = lt48_programmer_arm_auto_id_eprom(
            programmer, &preparation,
            LT48_AUTO_ID_EPROM_HARDWARE_AUTHORIZATION);
    if (status == LT48_OK) {
        g_mutex_lock(&work->widgets->progress_mutex);
        work->widgets->active_programmer = programmer;
        g_mutex_unlock(&work->widgets->progress_mutex);
        status = lt48_programmer_execute_auto_id_eprom(
            programmer, &preparation, work->timeout, &execution);
    }
    g_mutex_lock(&work->widgets->progress_mutex);
    if (work->widgets->active_programmer == programmer)
        work->widgets->active_programmer = NULL;
    lt48_programmer_close(programmer);
    g_mutex_unlock(&work->widgets->progress_mutex);
    lt48_context_destroy(context);
    if (status == LT48_OK)
        status = lt48_installation_rank_auto_id_eprom_execution(
            installation, &execution, NULL, 0u, &candidate_count);
    if (status == LT48_BUFFER_TOO_SMALL && candidate_count != 0u) {
        status = LT48_OK;
        candidates = g_new0(lt48_auto_id_candidate, candidate_count);
        if (candidates == NULL) status = LT48_NO_MEMORY;
    }
    if (status == LT48_OK)
        status = lt48_installation_rank_auto_id_eprom_execution(
            installation, &execution, candidates, candidate_count,
            &candidate_count);
    if (status == LT48_OK) {
        int accepted = 0;
        uint32_t top_score = candidate_count == 0u ? 0u : candidates[0].score;
        for (size_t n = 0u; n < candidate_count &&
                            candidates[n].score == top_score; ++n)
            if (candidates[n].device_index == selected_device) accepted = 1;
        if (!accepted) status = LT48_DEVICE_MISMATCH;
    }
    g_free(candidates);
    return status;
}

static void operation_execute_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    static const lt48_verify_margin_selection margin_values[] = {
        LT48_VERIFY_MARGIN_ALL, LT48_VERIFY_MARGIN_NOMINAL,
        LT48_VERIFY_MARGIN_LOW, LT48_VERIFY_MARGIN_HIGH
    };
    operation_execute_task *work = task_data;
    operation_prepare_task *request = work->request;
    operation_execute_result *result = g_new0(operation_execute_result, 1);
    lt48_installation *installation = NULL;
    lt48_m37_resources *resources = NULL;
    lt48_protocol_plan *plan = NULL;
    lt48_m37_verify_plan_set *verify_set = NULL;
    lt48_m37_program_plan_set *program_set = NULL;
    lt48_m37_read_collector *collector = NULL;
    lt48_verify_voltage_profile profile = {0};
    uint16_t voltages[4] = {0};
    size_t voltage_count = 0u;
    lt48_status status;
    (void)source_object;
    (void)cancellable;
    status = lt48_installation_open(request->installation_path, &installation);
    if (status == LT48_OK)
        status = lt48_m37_resources_open(
            installation, request->device_index, &resources);
    if (status == LT48_OK && request->operation == LT48_OPERATION_BLANK_CHECK)
        status = lt48_m37_resources_build_blank_check_plan(
            resources, request->start, request->length, &plan);
    if (status == LT48_OK && request->operation == LT48_OPERATION_COMPARE)
        status = lt48_m37_resources_build_compare_plan(
            resources, request->start, request->image,
            request->image_size, &plan);
    if (status == LT48_OK && request->operation == LT48_OPERATION_VERIFY) {
        if (request->margin_choice >= G_N_ELEMENTS(margin_values) ||
            request->margin_type > 1u) status = LT48_INVALID_ARGUMENT;
        else status = lt48_installation_verify_voltage_profile(
            installation, request->device_index, request->margin_type,
            &profile);
        if (status == LT48_OK)
            status = lt48_verify_voltage_profile_select(
                &profile, margin_values[request->margin_choice], voltages,
                G_N_ELEMENTS(voltages), &voltage_count);
        if (status == LT48_OK)
            status = lt48_m37_verify_plan_set_create(
                resources, request->start, request->image,
                request->image_size, voltages, voltage_count, &verify_set);
    }
    if (status == LT48_OK &&
        (request->operation == LT48_OPERATION_PROGRAM ||
         request->operation == LT48_OPERATION_OVER_PROGRAM))
        status = lt48_m37_program_plan_set_create(
            resources, request->start, request->image,
            request->image_size, &program_set);

    if (status == LT48_OK && request->operation == LT48_OPERATION_BLANK_CHECK) {
        lt48_m37_snapshot_read_execution_result execution = {0};
        status = execute_operation_phase(
            work, resources, plan, LT48_M37_BLANK_CHECK, NULL, &execution);
        result->message = g_strdup(status == LT48_OK ?
            "BLANK CHECK COMPLETE — the selected range is blank (0xff)" :
            status == LT48_DEVICE_MISMATCH ?
            "BLANK CHECK COMPLETE — the selected range is not blank" : NULL);
    } else if (status == LT48_OK &&
               request->operation == LT48_OPERATION_VERIFY) {
        for (size_t n = 0u; n < voltage_count && status == LT48_OK; ++n) {
            const lt48_protocol_plan *pass = NULL;
            uint16_t voltage = 0u;
            lt48_m37_snapshot_read_execution_result execution = {0};
            status = lt48_m37_verify_plan_set_get(
                verify_set, n, &voltage, &pass);
            if (status == LT48_OK)
                status = execute_operation_phase(
                    work, resources, pass, LT48_M37_VERIFY, NULL, &execution);
        }
        result->message = g_strdup(status == LT48_OK ?
            "VERIFY COMPLETE — device matches the image at every selected voltage" :
            status == LT48_DEVICE_MISMATCH ?
            "VERIFY FAILED — device does not match the image" : NULL);
    } else if (status == LT48_OK && request->operation == LT48_OPERATION_COMPARE) {
        const unsigned char *actual = NULL;
        uint32_t actual_start = 0u;
        size_t actual_size = 0u;
        lt48_m37_snapshot_read_execution_result execution = {0};
        status = lt48_m37_read_collector_create(plan, &collector);
        if (status == LT48_OK)
            status = execute_operation_phase(
                work, resources, plan, LT48_M37_COMPARE, collector, &execution);
        if (status == LT48_OK)
            status = lt48_m37_read_collector_data(
                collector, &actual_start, &actual, &actual_size);
        if (status == LT48_OK &&
            (actual_start != request->start ||
             actual_size != request->image_size)) status = LT48_PROTOCOL_ERROR;
        if (status == LT48_OK &&
            memcmp(actual, request->image, actual_size) != 0)
            status = LT48_DEVICE_MISMATCH;
        result->message = g_strdup(status == LT48_OK ?
            "COMPARE COMPLETE — device and image are identical" :
            status == LT48_DEVICE_MISMATCH ?
            "COMPARE COMPLETE — device and image differ" : NULL);
    } else if (status == LT48_OK &&
               (request->operation == LT48_OPERATION_PROGRAM ||
                request->operation == LT48_OPERATION_OVER_PROGRAM)) {
        size_t first_phase = 0u;
        status = execute_selected_device_check(
            work, installation, request->device_index);
        if (status == LT48_DEVICE_MISMATCH &&
            request->selected_device_override)
            status = LT48_OK;
        if (request->operation == LT48_OPERATION_OVER_PROGRAM) {
            const unsigned char *actual = NULL;
            uint32_t actual_start = 0u;
            size_t actual_size = 0u;
            lt48_m37_snapshot_read_execution_result execution = {0};
            status = lt48_m37_resources_build_compare_plan(
                resources, request->start, request->image,
                request->image_size, &plan);
            if (status == LT48_OK)
                status = lt48_m37_read_collector_create(plan, &collector);
            if (status == LT48_OK)
                status = execute_operation_phase(
                    work, resources, plan, LT48_M37_COMPARE, collector,
                    &execution);
            if (status == LT48_OK)
                status = lt48_m37_read_collector_data(
                    collector, &actual_start, &actual, &actual_size);
            if (status == LT48_OK &&
                (actual_start != request->start ||
                 actual_size != request->image_size))
                status = LT48_PROTOCOL_ERROR;
            for (size_t n = 0u; n < actual_size && status == LT48_OK; ++n)
                if ((actual[n] & request->image[n]) != request->image[n])
                    status = LT48_SAFETY_REJECTED;
            first_phase = 1u;
        }
        for (size_t n = first_phase; n < 3u && status == LT48_OK; ++n) {
            const lt48_protocol_plan *phase = NULL;
            lt48_m37_operation operation;
            lt48_m37_snapshot_read_execution_result execution = {0};
            status = lt48_m37_program_plan_set_get(
                program_set, n, &operation, &phase);
            if (status == LT48_OK)
                status = execute_operation_phase(
                    work, resources, phase, operation, NULL, &execution);
        }
        result->message = g_strdup(status == LT48_OK ?
            (request->operation == LT48_OPERATION_PROGRAM ?
             (request->selected_device_override ?
              "PROGRAM COMPLETE — identity override was enabled; blank precheck, programming, and readback verify passed" :
              "PROGRAM COMPLETE — Auto-ID, blank precheck, programming, and readback verify passed") :
             (request->selected_device_override ?
              "OVER-PROGRAM COMPLETE — identity override was enabled; one-to-zero precheck, programming, and readback verify passed" :
              "OVER-PROGRAM COMPLETE — Auto-ID, one-to-zero precheck, programming, and readback verify passed")) :
            status == LT48_DEVICE_MISMATCH &&
            request->operation == LT48_OPERATION_PROGRAM ?
            "PROGRAM REJECTED — Auto-ID did not match the selected device, or the device is not blank" :
            status == LT48_SAFETY_REJECTED &&
            request->operation == LT48_OPERATION_OVER_PROGRAM ?
            "OVER-PROGRAM REJECTED — requested image requires a forbidden zero-to-one transition" : NULL);
    }
    result->status = status;
    if (result->message == NULL)
        result->message = g_strdup_printf(
            "%s failed: %s. Mandatory cleanup was attempted before return.",
            lt48_operation_kind_name(request->operation),
            lt48_status_string(status));
    lt48_m37_read_collector_close(collector);
    lt48_protocol_plan_close(plan);
    lt48_m37_verify_plan_set_close(verify_set);
    lt48_m37_program_plan_set_close(program_set);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    g_task_return_pointer(task, result, operation_execute_result_free);
}

static void operation_execute_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    operation_execute_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    (void)source_object;
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
        result == NULL ? "Operation failed without a result" : result->message);
    if (result != NULL) statistics_record(widgets, result->status);
    operation_execute_result_free(result);
    if (widgets->progress_source_id != 0u) {
        g_source_remove(widgets->progress_source_id);
        widgets->progress_source_id = 0u;
    }
    g_clear_object(&widgets->read_cancellable);
    operation_progress_finish(widgets);
    gtk_widget_set_sensitive(widgets->operation_prepare_button, TRUE);
    gtk_widget_set_sensitive(widgets->read_cancel_button, FALSE);
    operation_prepare_task_free(widgets->prepared_operation);
    widgets->prepared_operation = NULL;
    gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
}

static void execute_prepared_operation(app_widgets *widgets) {
    operation_execute_task *work;
    GTask *task;
    uint8_t bus, address;
    uint32_t timeout;
    if (widgets->prepared_operation == NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Operation preflight is not ready; choose the "
                           "operation again");
        return;
    }
    if (!programmer_target(widgets, &bus, &address) ||
        !entry_u32(widgets->read_timeout_entry, UINT32_MAX, &timeout) ||
        timeout == 0u) {
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Programmer target or timeout is invalid");
        return;
    }
    work = g_new0(operation_execute_task, 1);
    work->request = operation_prepare_task_clone(widgets->prepared_operation);
    work->widgets = widgets;
    work->bus = bus;
    work->address = address;
    work->timeout = timeout;
    if (work->request == NULL) {
        operation_execute_task_free(work);
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Cannot retain the operation preflight");
        return;
    }
    widgets->read_cancellable = g_cancellable_new();
    widgets->progress_source_id = g_timeout_add(100u, poll_read_progress, widgets);
    gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    gtk_widget_set_sensitive(widgets->operation_prepare_button, FALSE);
    gtk_widget_set_sensitive(widgets->read_cancel_button, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                       "Operation running; mandatory cleanup remains active…");
    operation_progress_present(
        widgets, lt48_operation_kind_name(work->request->operation),
        "Operation running; cancellation is cooperative and mandatory "
        "cleanup remains active.");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->read_cancellable, operation_execute_finished);
    g_task_set_task_data(task, work, operation_execute_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_run_in_thread(task, operation_execute_worker);
    g_object_unref(task);
}

static void read_overwrite_choice(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    gboolean replace = GPOINTER_TO_INT(g_object_get_data(
        G_OBJECT(button), "labtool48-replace"));
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(root)) gtk_window_destroy(GTK_WINDOW(root));
    if (!replace) {
        widgets->read_overwrite_authorized = 0;
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Read cancelled; the existing file was kept");
        return;
    }
    widgets->read_overwrite_authorized = 1;
    execute_read(NULL, widgets);
}

static void ask_read_overwrite(app_widgets *widgets, const char *path) {
    GtkWidget *dialog = gtk_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *replace = gtk_button_new_with_label("Replace");
    char *message = g_strdup_printf(
        "The output or its execution report already exists for ‘%s’. "
        "Replace the existing file(s) when this read runs?",
        path);
    GtkWidget *label = gtk_label_new(message);
    g_free(message);
    gtk_window_set_title(GTK_WINDOW(dialog), "Replace existing read?");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), widgets->window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_widget_set_margin_top(box, 18);
    gtk_widget_set_margin_bottom(box, 18);
    gtk_widget_set_margin_start(box, 18);
    gtk_widget_set_margin_end(box, 18);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 54);
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_widget_add_css_class(replace, "destructive-action");
    gtk_box_append(GTK_BOX(actions), cancel);
    gtk_box_append(GTK_BOX(actions), replace);
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(GTK_WINDOW(dialog), box);
    g_object_set_data(G_OBJECT(cancel), "labtool48-replace",
                      GINT_TO_POINTER(FALSE));
    g_object_set_data(G_OBJECT(replace), "labtool48-replace",
                      GINT_TO_POINTER(TRUE));
    g_signal_connect(cancel, "clicked",
                     G_CALLBACK(read_overwrite_choice), widgets);
    g_signal_connect(replace, "clicked",
                     G_CALLBACK(read_overwrite_choice), widgets);
    gtk_window_present(GTK_WINDOW(dialog));
}

static void execute_read(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    read_task *work;
    GFile *file;
    gboolean output_exists;
    gboolean report_exists;
    GTask *task;
    const char *confirmation;
    lt48_status status;
    (void)button;
    if (!widgets->programmer_ready) {
        widgets->pending_hardware_action = PENDING_HARDWARE_EXECUTE;
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                           "Programmer was not ready; probing and connecting "
                           "before execution…");
        refresh_programmers(NULL, widgets);
        return;
    }
    if (selected_operation_kind(widgets) != LT48_OPERATION_READ) {
        execute_prepared_operation(widgets);
        return;
    }
    work = g_new0(read_task, 1);
    work->widgets = widgets;
    status = prepare_selected_read(widgets, &work->request);
    if (status != LT48_OK) {
        char *message = g_strdup_printf("Cannot start Read: %s",
                                        lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
        g_free(message);
        read_task_free(work);
        return;
    }
    confirmation = gtk_editable_get_text(
        GTK_EDITABLE(widgets->read_confirmation_entry)
    );
    if (!confirmation_matches(
            confirmation, work->request.preparation.confirmation_sha256)) {
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Execution rejected: paste the exact confirmation from a fresh "
            "offline preparation"
        );
        read_task_free(work);
        return;
    }
    if (work->request.report == NULL || work->request.report[0] == '\0') {
        gtk_label_set_text(
            GTK_LABEL(widgets->read_preparation),
            "Execution rejected: enter a new execution-report path"
        );
        read_task_free(work);
        return;
    }
    file = g_file_new_for_path(work->request.output);
    output_exists = g_file_query_exists(file, NULL);
    g_object_unref(file);
    file = g_file_new_for_path(work->request.report);
    report_exists = g_file_query_exists(file, NULL);
    g_object_unref(file);
    if (output_exists || report_exists) {
        if (!widgets->read_overwrite_authorized) {
            ask_read_overwrite(widgets, work->request.output);
            read_task_free(work);
            return;
        }
        work->request.overwrite_output = 1;
        widgets->read_overwrite_authorized = 0;
    } else {
        widgets->read_overwrite_authorized = 0;
    }
    widgets->read_cancellable = g_cancellable_new();
    widgets->progress_source_id = g_timeout_add(
        100u, poll_read_progress, widgets
    );
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)
    ));
    task = tracked_task_new(
        widgets, widgets->read_cancellable, execute_read_finished);
    g_task_set_task_data(task, work, read_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    gtk_widget_set_sensitive(widgets->read_execute_button, FALSE);
    gtk_widget_set_sensitive(widgets->operation_prepare_button, FALSE);
    gtk_widget_set_sensitive(widgets->read_cancel_button, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                       "Read running… Cancel remains available; cleanup is "
                       "mandatory after cancellation.");
    operation_progress_present(
        widgets, "Read",
        "Read running; cancellation is cooperative and mandatory cleanup "
        "remains active.");
    g_task_run_in_thread(task, execute_read_worker);
    g_object_unref(task);
}

static void cancel_read(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    if (widgets->read_cancellable == NULL) return;
    g_cancellable_cancel(widgets->read_cancellable);
    gtk_widget_set_sensitive(widgets->read_cancel_button, FALSE);
    if (widgets->operation_progress_cancel != NULL)
        gtk_widget_set_sensitive(
            widgets->operation_progress_cancel, FALSE);
    if (widgets->operation_progress_status != NULL)
        gtk_label_set_text(
            GTK_LABEL(widgets->operation_progress_status),
            "Cancellation requested — waiting for mandatory hardware "
            "cleanup and interface release…");
    gtk_label_set_text(GTK_LABEL(widgets->read_preparation),
                       "Cancellation requested — waiting for mandatory "
                       "hardware cleanup and interface release…");
}

static gboolean operation_progress_close_requested(
    GtkWindow *window, gpointer user_data
) {
    app_widgets *widgets = user_data;
    (void)window;
    if (widgets->read_cancellable == NULL) return FALSE;
    cancel_read(NULL, widgets);
    return TRUE;
}

static void operation_progress_present(
    app_widgets *widgets, const char *operation, const char *status
) {
    char *heading;
    if (widgets->operation_progress_window == NULL) return;
    heading = g_strdup_printf(
        "%s in progress", operation == NULL ? "Operation" : operation);
    gtk_label_set_text(
        GTK_LABEL(widgets->operation_progress_heading), heading);
    gtk_label_set_text(
        GTK_LABEL(widgets->operation_progress_status), status);
    gtk_progress_bar_set_fraction(
        GTK_PROGRESS_BAR(widgets->operation_progress_bar), 0.0);
    gtk_progress_bar_set_text(
        GTK_PROGRESS_BAR(widgets->operation_progress_bar), "Starting…");
    gtk_widget_set_sensitive(widgets->operation_progress_cancel, TRUE);
    gtk_window_present(widgets->operation_progress_window);
    g_free(heading);
}

static void operation_progress_finish(app_widgets *widgets) {
    if (widgets->operation_progress_window == NULL) return;
    gtk_widget_set_sensitive(widgets->operation_progress_cancel, FALSE);
    gtk_widget_set_visible(
        GTK_WIDGET(widgets->operation_progress_window), FALSE);
}

static int buffer_range(
    app_widgets *widgets, uint32_t *out_start, uint32_t *out_length
) {
    return widgets->device_buffer != NULL &&
           entry_u32(widgets->buffer_start_entry, UINT32_MAX, out_start) &&
           entry_u32(widgets->buffer_length_entry, UINT32_MAX, out_length) &&
           (size_t)*out_start <= lt48_buffer_size(widgets->device_buffer) &&
           (size_t)*out_length <=
               lt48_buffer_size(widgets->device_buffer) - (size_t)*out_start;
}

static lt48_buffer *buffer_edit_buffer(app_widgets *widgets) {
    return widgets->buffer_edit_secondary ?
        widgets->secondary_buffer : widgets->device_buffer;
}

static void buffer_edit_note_mutation(app_widgets *widgets) {
    if (widgets->buffer_edit_secondary)
        ++widgets->secondary_buffer_revision;
    else
        ++widgets->buffer_revision;
    invalidate_operation_preparation(widgets, 1);
}

static void buffer_edit_discard_undo(app_widgets *widgets) {
    widgets->buffer_edit_has_undo = 0;
    widgets->buffer_edit_undo_size = 0u;
    g_clear_pointer(&widgets->buffer_edit_undo_data, g_free);
}

static int buffer_edit_capture_undo(
    app_widgets *widgets, uint32_t offset, size_t length
) {
    size_t size;
    lt48_buffer *buffer = buffer_edit_buffer(widgets);
    if (buffer == NULL) return 0;
    size = lt48_buffer_size(buffer);
    if ((size_t)offset > size || length > size - (size_t)offset ||
        length == 0u || length > UINT32_MAX)
        return 0;
    buffer_edit_discard_undo(widgets);
    widgets->buffer_edit_undo_data = g_memdup2(
        lt48_buffer_data(buffer) + offset, length);
    widgets->buffer_edit_undo_size = length;
    widgets->buffer_edit_undo_offset = offset;
    return widgets->buffer_edit_undo_data != NULL;
}

static void buffer_show_range(
    app_widgets *widgets, const char *action, uint32_t start, uint32_t length
) {
    enum { PREVIEW_LIMIT = 256, BYTES_PER_LINE = 16 };
    lt48_buffer_checksum checksum;
    lt48_buffer_binding binding;
    const unsigned char *data;
    GtkTextBuffer *text_buffer;
    GString *preview;
    char sha[LT48_SHA256_SIZE * 2u + 1u];
    char binding_sha[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    size_t shown;
    lt48_status binding_status;
    if (widgets->toolbar_save_button != NULL)
        gtk_widget_set_sensitive(widgets->toolbar_save_button, TRUE);
    if (widgets->toolbar_edit_button != NULL)
        gtk_widget_set_sensitive(widgets->toolbar_edit_button, TRUE);
    if (widgets->window != NULL) {
        GAction *save_action = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "save");
        if (save_action != NULL)
            g_simple_action_set_enabled(G_SIMPLE_ACTION(save_action), TRUE);
        save_action = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "edit");
        if (save_action != NULL)
            g_simple_action_set_enabled(G_SIMPLE_ACTION(save_action), TRUE);
    }
    if (widgets->device_buffer == NULL ||
        lt48_buffer_checksum_range(widgets->device_buffer, start, length,
                                   &checksum) != LT48_OK) {
        if (widgets->footer_checksum != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->footer_checksum), "—");
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "No valid buffer range is selected");
        return;
    }
    if (widgets->footer_checksum != NULL) {
        lt48_buffer_checksum full_checksum;
        char checksum_text[32];
        if (lt48_buffer_checksum_range(
                widgets->device_buffer, 0u,
                lt48_buffer_size(widgets->device_buffer),
                &full_checksum) == LT48_OK) {
            g_snprintf(checksum_text, sizeof(checksum_text), "%08Xh",
                       full_checksum.additive32);
            gtk_label_set_text(
                GTK_LABEL(widgets->footer_checksum), checksum_text);
        } else {
            gtk_label_set_text(
                GTK_LABEL(widgets->footer_checksum), "Incomplete buffer");
        }
    }
    digest_hex(checksum.sha256, LT48_SHA256_SIZE, sha);
    binding_status = lt48_buffer_bind_mutating_input(
        widgets->device_buffer, lt48_buffer_size(widgets->device_buffer),
        start, length, &binding);
    if (binding_status == LT48_OK) {
        digest_hex(binding.binding_sha256, LT48_SHA256_SIZE, binding_sha);
        message = g_strdup_printf(
            "%s · device image %zu bytes (%zu known) · range 0x%x + %u · "
            "additive32 %u · SHA-256 %s\nREQ-BUF-004 binding %s · %s · "
            "%zu post-origin mutation%s",
            action, lt48_buffer_size(widgets->device_buffer),
            lt48_buffer_known_size(widgets->device_buffer), start, length,
            checksum.additive32, sha, binding_sha,
            binding.provenance.origin == LT48_BUFFER_ORIGIN_FILE_IMPORT ?
                "file import" :
            binding.provenance.origin == LT48_BUFFER_ORIGIN_DEVICE_READ ?
                "device read" : "generated fill",
            binding.provenance.mutation_count,
            binding.provenance.mutation_count == 1u ? "" : "s");
    } else if (binding_status == LT48_SAFETY_REJECTED) {
        message = g_strdup_printf(
            "%s · device image %zu bytes (%zu known) · range 0x%x + %u · "
            "additive32 %u · SHA-256 %s\nMutating-input binding is "
            "safety-rejected until every device byte is known or explicitly "
            "written.",
            action, lt48_buffer_size(widgets->device_buffer),
            lt48_buffer_known_size(widgets->device_buffer), start, length,
            checksum.additive32, sha);
    } else {
        message = g_strdup_printf(
            "%s · cannot bind selected range: %s", action,
            lt48_status_string(binding_status));
    }
    gtk_label_set_text(GTK_LABEL(widgets->buffer_status), message);
    g_free(message);
    data = lt48_buffer_data(widgets->device_buffer);
    shown = length > PREVIEW_LIMIT ? PREVIEW_LIMIT : length;
    preview = g_string_new(NULL);
    for (size_t line = 0u; line < shown; line += BYTES_PER_LINE) {
        size_t line_size = shown - line;
        if (line_size > BYTES_PER_LINE) line_size = BYTES_PER_LINE;
        g_string_append_printf(preview, "%08zx  ", (size_t)start + line);
        for (size_t index = 0u; index < BYTES_PER_LINE; ++index) {
            if (index < line_size)
                g_string_append_printf(preview, "%02x ",
                    data[(size_t)start + line + index]);
            else
                g_string_append(preview, "   ");
        }
        g_string_append(preview, " ");
        for (size_t index = 0u; index < line_size; ++index) {
            unsigned char value = data[(size_t)start + line + index];
            g_string_append_c(preview,
                g_ascii_isprint(value) ? (char)value : '.');
        }
        g_string_append_c(preview, '\n');
    }
    if ((size_t)length > shown)
        g_string_append_printf(preview,
            "… %zu additional selected bytes retained but not rendered\n",
            (size_t)length - shown);
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->buffer_preview));
    gtk_text_buffer_set_text(text_buffer, preview->str, -1);
    g_string_free(preview, TRUE);
}

static void buffer_io_task_free(gpointer data) {
    buffer_io_task *request = data;
    if (request == NULL) return;
    g_free(request->path);
    lt48_buffer_close(request->snapshot);
    g_free(request);
}

static void buffer_io_result_free(gpointer data) {
    buffer_io_result *result = data;
    if (result == NULL) return;
    lt48_buffer_close(result->loaded);
    g_free(result->path);
    g_free(result);
}

static lt48_status merge_imported_image(
    lt48_buffer *destination, uint32_t target_offset,
    const lt48_buffer *imported, GCancellable *cancellable
) {
    const unsigned char *data = lt48_buffer_data(imported);
    size_t size = lt48_buffer_size(imported);
    size_t offset = 0u;
    if (destination == NULL || imported == NULL ||
        (size_t)target_offset > lt48_buffer_size(destination) ||
        size > lt48_buffer_size(destination) - (size_t)target_offset)
        return LT48_INVALID_ARGUMENT;
    while (offset < size) {
        unsigned char ignored;
        lt48_status status;
        size_t start;
        if ((offset & 0xfffu) == 0u &&
            g_cancellable_is_cancelled(cancellable))
            return LT48_CANCELLED;
        status = lt48_buffer_get_byte(imported, offset, &ignored);
        if (status == LT48_SAFETY_REJECTED) {
            ++offset;
            continue;
        }
        if (status != LT48_OK) return status;
        start = offset++;
        while (offset < size) {
            status = lt48_buffer_get_byte(imported, offset, &ignored);
            if (status != LT48_OK) break;
            ++offset;
        }
        status = lt48_buffer_write(
            destination, (size_t)target_offset + start,
            data + start, offset - start);
        if (status != LT48_OK) return status;
    }
    return LT48_OK;
}

static void buffer_io_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    buffer_io_task *request = task_data;
    buffer_io_result *result = g_new0(buffer_io_result, 1);
    (void)source_object;
    result->kind = request->kind;
    result->path = g_strdup(request->path);
    result->options = request->options;
    result->image_options = request->image_options;
    result->export_options = request->export_options;
    result->start = request->start;
    result->length = request->length;
    result->target_offset = request->target_offset;
    result->load_clear_mode = request->load_clear_mode;
    result->source_revision = request->source_revision;
    result->extract = request->extract;
    if (request->kind == BUFFER_IO_LOAD) {
        result->status = lt48_buffer_load_file_cancellable(
            request->path, &request->options, gio_cancelled, cancellable,
            &result->import_result, &result->loaded);
    } else if (request->kind == BUFFER_IO_LOAD_IMAGE) {
        lt48_buffer *imported = NULL;
        result->status = lt48_buffer_load_image_cancellable(
            request->path, &request->image_options, gio_cancelled,
            cancellable, &result->image_import_result, &imported);
        if (result->status == LT48_OK)
            result->status = merge_imported_image(
                request->snapshot, request->target_offset,
                imported, cancellable);
        lt48_buffer_close(imported);
        if (result->status == LT48_OK) {
            result->loaded = request->snapshot;
            request->snapshot = NULL;
        }
    } else if (request->kind == BUFFER_IO_SAVE) {
        result->status = lt48_buffer_save_image_cancellable(
            request->snapshot, request->path, &request->export_options,
            gio_cancelled, cancellable, &result->export_result);
    } else {
        lt48_buffer *expected = NULL;
        lt48_buffer_import_options options = {
            request->length, LT48_BUFFER_SHORT_REJECT,
            LT48_BUFFER_LONG_REJECT, 0u, 1u, 0
        };
        result->status = lt48_buffer_load_file_cancellable(
            request->path, &options, gio_cancelled, cancellable, NULL,
            &expected);
        if (result->status == LT48_OK)
            result->status = lt48_buffer_compare_range_cancellable(
                request->snapshot, 0u, lt48_buffer_data(expected),
                request->length, gio_cancelled, cancellable,
                &result->comparison);
        lt48_buffer_close(expected);
    }
    g_task_return_pointer(task, result, buffer_io_result_free);
}

static void buffer_io_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    buffer_io_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    char *message;
    (void)source_object;
    gtk_widget_set_sensitive(widgets->buffer_controls, TRUE);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, FALSE);
    if (widgets->load_image_controls != NULL)
        gtk_widget_set_sensitive(widgets->load_image_controls, TRUE);
    if (widgets->load_image_apply != NULL)
        gtk_widget_set_sensitive(widgets->load_image_apply, TRUE);
    if (widgets->load_image_cancel != NULL)
        gtk_widget_set_sensitive(widgets->load_image_cancel, TRUE);
    if (widgets->save_image_controls != NULL)
        gtk_widget_set_sensitive(widgets->save_image_controls, TRUE);
    if (widgets->save_image_apply != NULL)
        gtk_widget_set_sensitive(widgets->save_image_apply, TRUE);
    if (widgets->save_image_cancel != NULL)
        gtk_widget_set_sensitive(widgets->save_image_cancel, TRUE);
    widgets->active_buffer_dialog = NULL;
    g_clear_object(&widgets->buffer_cancellable);
    if (result == NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Buffer file task failed without a result");
    } else if (result->status == LT48_CANCELLED) {
        const char *cancel_message =
            (result->kind == BUFFER_IO_LOAD ||
             result->kind == BUFFER_IO_LOAD_IMAGE) ?
            "Buffer load cancelled; the previous resident image was retained." :
            result->kind == BUFFER_IO_SAVE ?
            "Buffer save cancelled; the incomplete exclusive output was removed." :
            "Buffer comparison cancelled; the resident image was unchanged.";
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), cancel_message);
        if (result->kind == BUFFER_IO_SAVE &&
            widgets->save_image_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->save_image_status),
                               cancel_message);
    } else if (result->status != LT48_OK) {
        message = g_strdup_printf(
            "%s rejected: %s. %s",
            (result->kind == BUFFER_IO_LOAD ||
             result->kind == BUFFER_IO_LOAD_IMAGE) ? "Load" :
            result->kind == BUFFER_IO_SAVE ? "Save" : "Compare",
            lt48_status_string(result->status),
            result->kind == BUFFER_IO_LOAD_IMAGE ?
                "Check the image format, buffer size/address, source base, "
                "clear mode and sparse-gap policy." :
            result->kind == BUFFER_IO_LOAD ?
                "Short and long raw files are adjusted only by explicit policy." :
            result->kind == BUFFER_IO_SAVE ?
                "Existing files are never overwritten." :
                "The expected file must exactly match the selected length.");
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), message);
        if (result->kind == BUFFER_IO_LOAD_IMAGE &&
            widgets->load_image_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->load_image_status), message);
        if (result->kind == BUFFER_IO_SAVE &&
            widgets->save_image_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->save_image_status), message);
        g_free(message);
    } else if (result->kind == BUFFER_IO_LOAD ||
               result->kind == BUFFER_IO_LOAD_IMAGE) {
        char text[32];
        char *action;
        buffer_edit_discard_undo(widgets);
        lt48_buffer_close(widgets->device_buffer);
        widgets->device_buffer = result->loaded;
        result->loaded = NULL;
        ++widgets->buffer_revision;
        invalidate_operation_preparation(widgets, 1);
        if (widgets->verify_use_buffer_check != NULL &&
            lt48_buffer_known_size(widgets->device_buffer) ==
                lt48_buffer_size(widgets->device_buffer))
            gtk_check_button_set_active(
                GTK_CHECK_BUTTON(widgets->verify_use_buffer_check), TRUE);
        gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_start_entry), "0");
        g_snprintf(text, sizeof(text), "%zu",
                   lt48_buffer_size(widgets->device_buffer));
        gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_length_entry), text);
        if (result->kind == BUFFER_IO_LOAD_IMAGE) {
            const lt48_image_import_result *image =
                &result->image_import_result;
            action = g_strdup_printf(
                "Loaded %s image at buffer 0x%x: %zu mapped byte%s from %zu "
                "data record%s; source 0x%llx..0x%llx; %zu gap region%s / "
                "%zu byte%s %s",
                lt48_image_format_name(image->format),
                result->target_offset,
                image->mapped_data_byte_count,
                image->mapped_data_byte_count == 1u ? "" : "s",
                image->data_record_count,
                image->data_record_count == 1u ? "" : "s",
                (unsigned long long)image->minimum_source_address,
                (unsigned long long)image->maximum_source_address_exclusive,
                image->gap_count, image->gap_count == 1u ? "" : "s",
                image->gap_byte_count,
                image->gap_byte_count == 1u ? "" : "s",
                result->image_options.gap_policy == LT48_IMAGE_GAPS_FILL ?
                    "filled" : "preserved");
        } else {
            action = g_strdup_printf(
                "Loaded raw image: input %zu, imported %zu, padded %zu, "
                "truncated %zu, %zu-byte words%s",
                result->import_result.input_size,
                result->import_result.imported_size,
                result->import_result.padded_size,
                result->import_result.truncated_size,
                result->options.word_size_bytes,
                result->options.reverse_word_bytes ?
                    ", bytes reversed per word" : "");
        }
        buffer_show_range(widgets, action, 0u,
                          (uint32_t)lt48_buffer_size(widgets->device_buffer));
        if (widgets->footer_file != NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->footer_file), result->path);
        if (result->kind == BUFFER_IO_LOAD_IMAGE) {
            if (widgets->load_image_status != NULL)
                gtk_label_set_text(GTK_LABEL(widgets->load_image_status),
                                   action);
            if (widgets->load_image_window != NULL)
                gtk_widget_set_visible(
                    GTK_WIDGET(widgets->load_image_window), FALSE);
        }
        g_free(action);
    } else if (result->kind == BUFFER_IO_COMPARE) {
        if (result->comparison.equal) {
            message = g_strdup_printf("Compare equal: %u bytes match",
                                      result->length);
        } else {
            message = g_strdup_printf(
                "Compare differs at device address 0x%zx: retained %02x, "
                "expected %02x",
                (size_t)result->start +
                    result->comparison.first_mismatch_offset,
                result->comparison.actual_value,
                result->comparison.expected_value);
        }
        if (widgets->buffer_revision == result->source_revision)
            buffer_show_range(widgets, message, result->start, result->length);
        else
            gtk_label_set_text(GTK_LABEL(widgets->buffer_status), message);
        g_free(message);
    } else if (widgets->buffer_revision == result->source_revision) {
        message = g_strdup_printf(
            "Saved %s image exclusively: %zu output byte%s from %u source "
            "byte%s at 0x%llx..0x%llx",
            lt48_image_format_name(result->export_result.format),
            result->export_result.output_data_byte_count,
            result->export_result.output_data_byte_count == 1u ? "" : "s",
            result->length, result->length == 1u ? "" : "s",
            (unsigned long long)
                result->export_result.minimum_output_address,
            (unsigned long long)
                result->export_result.maximum_output_address_exclusive);
        buffer_show_range(widgets, message, result->start, result->length);
        if (widgets->save_image_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->save_image_status), message);
        if (widgets->save_image_window != NULL)
            gtk_widget_set_visible(GTK_WIDGET(widgets->save_image_window),
                                   FALSE);
        if (widgets->footer_file != NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->footer_file), result->path);
        g_free(message);
    } else {
        const char *saved_message =
            "Saved the immutable buffer snapshot exclusively; the resident "
            "buffer changed while file I/O was running.";
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), saved_message);
        if (widgets->save_image_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->save_image_status),
                               saved_message);
        if (widgets->save_image_window != NULL)
            gtk_widget_set_visible(GTK_WIDGET(widgets->save_image_window),
                                   FALSE);
        if (widgets->footer_file != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->footer_file), result->path);
    }
    buffer_io_result_free(result);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
}

static void buffer_cancel_io(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    if (widgets->buffer_cancellable == NULL) return;
    g_cancellable_cancel(widgets->buffer_cancellable);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
        "Cancellation requested; waiting for the current bounded file chunk…");
}

static void buffer_dialog_request_cancel(
    app_widgets *widgets, GtkWindow *dialog
) {
    GtkWidget *status;
    GtkWidget *cancel;
    if (widgets->buffer_cancellable == NULL ||
        widgets->active_buffer_dialog != dialog)
        return;
    g_cancellable_cancel(widgets->buffer_cancellable);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, FALSE);
    status = dialog == widgets->load_image_window ?
        widgets->load_image_status : widgets->save_image_status;
    cancel = dialog == widgets->load_image_window ?
        widgets->load_image_cancel : widgets->save_image_cancel;
    if (cancel != NULL) gtk_widget_set_sensitive(cancel, FALSE);
    if (status != NULL)
        gtk_label_set_text(
            GTK_LABEL(status),
            "Cancellation requested; waiting for the current bounded file chunk…");
    gtk_label_set_text(
        GTK_LABEL(widgets->buffer_status),
        "Cancellation requested; waiting for the current bounded file chunk…");
}

static void buffer_dialog_cancel_clicked(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(root)) return;
    if (widgets->buffer_cancellable != NULL &&
        widgets->active_buffer_dialog == GTK_WINDOW(root)) {
        buffer_dialog_request_cancel(widgets, GTK_WINDOW(root));
        return;
    }
    gtk_window_close(GTK_WINDOW(root));
}

static gboolean buffer_dialog_close_requested(
    GtkWindow *window, gpointer user_data
) {
    app_widgets *widgets = user_data;
    if (widgets->buffer_cancellable == NULL ||
        widgets->active_buffer_dialog != window)
        return FALSE;
    buffer_dialog_request_cancel(widgets, window);
    return TRUE;
}

static void buffer_load(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->buffer_input_entry)
    );
    lt48_buffer_import_options options;
    buffer_io_task *request;
    GTask *task;
    uint32_t size;
    uint32_t pad_value;
    uint32_t word_size;
    (void)button;
    if (!entry_u32(widgets->buffer_size_entry, UINT32_MAX, &size) ||
        !entry_u32(widgets->buffer_value_entry, UINT8_MAX, &pad_value) ||
        !entry_u32(widgets->buffer_word_entry, 8u, &word_size) ||
        (word_size != 1u && word_size != 2u &&
         word_size != 4u && word_size != 8u) ||
        size == 0u || path == NULL || path[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Enter a path, nonzero size, pad byte and word size "
                           "of 1, 2, 4 or 8");
        return;
    }
    options.buffer_size = size;
    options.short_input = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->buffer_short_dropdown)) == 0u ?
        LT48_BUFFER_SHORT_REJECT : LT48_BUFFER_SHORT_PAD;
    options.long_input = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->buffer_long_dropdown)) == 0u ?
        LT48_BUFFER_LONG_REJECT : LT48_BUFFER_LONG_TRUNCATE;
    options.pad_value = (unsigned char)pad_value;
    options.word_size_bytes = word_size;
    options.reverse_word_bytes = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->buffer_reverse_check)
    );
    if (widgets->buffer_cancellable != NULL) return;
    request = g_new0(buffer_io_task, 1);
    request->kind = BUFFER_IO_LOAD;
    request->path = g_strdup(path);
    request->options = options;
    widgets->buffer_cancellable = g_cancellable_new();
    gtk_widget_set_sensitive(widgets->buffer_controls, FALSE);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                       "Loading buffer in bounded cancellable chunks…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->buffer_cancellable, buffer_io_finished);
    g_task_set_task_data(task, request, buffer_io_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_run_in_thread(task, buffer_io_worker);
    g_object_unref(task);
}

static lt48_status snapshot_resident_buffer(
    const lt48_buffer *source, size_t size, lt48_buffer **out_buffer
) {
    lt48_buffer *copy = NULL;
    size_t limit;
    size_t offset = 0u;
    lt48_status status;
    if (out_buffer == NULL) return LT48_INVALID_ARGUMENT;
    *out_buffer = NULL;
    status = lt48_buffer_create_sparse(size, &copy);
    if (status != LT48_OK || source == NULL) {
        if (status == LT48_OK) *out_buffer = copy;
        return status;
    }
    limit = lt48_buffer_size(source) < size ?
        lt48_buffer_size(source) : size;
    while (offset < limit) {
        unsigned char ignored;
        size_t start;
        status = lt48_buffer_get_byte(source, offset, &ignored);
        if (status == LT48_SAFETY_REJECTED) {
            ++offset;
            continue;
        }
        if (status != LT48_OK) break;
        start = offset++;
        while (offset < limit &&
               lt48_buffer_get_byte(source, offset, &ignored) == LT48_OK)
            ++offset;
        status = lt48_buffer_write(
            copy, start, lt48_buffer_data(source) + start, offset - start);
        if (status != LT48_OK) break;
    }
    if (status != LT48_OK && status != LT48_SAFETY_REJECTED) {
        lt48_buffer_close(copy);
        return status;
    }
    *out_buffer = copy;
    return LT48_OK;
}

static void load_image_execute(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->load_image_path));
    buffer_io_task *request;
    GTask *task;
    uint32_t size;
    uint32_t target;
    uint32_t gap_fill;
    guint format;
    guint gap_policy;
    guint clear_mode;
    lt48_status status;
    (void)button;

    format = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->load_image_format));
    gap_policy = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->load_image_gap_policy));
    clear_mode = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->load_image_clear));
    if (path == NULL || path[0] == '\0' ||
        !entry_u32(widgets->load_image_size, UINT32_MAX, &size) ||
        size == 0u ||
        !entry_u32(widgets->load_image_target, UINT32_MAX, &target) ||
        target >= size ||
        !entry_u32(widgets->load_image_gap_fill, UINT8_MAX, &gap_fill) ||
        format > LT48_IMAGE_FORMAT_HP64000_ABSOLUTE || gap_policy > 1u ||
        clear_mode > 3u) {
        gtk_label_set_text(GTK_LABEL(widgets->load_image_status),
            "Enter a source file, nonzero buffer size, in-range buffer "
            "address, valid format and gap-fill byte.");
        return;
    }
    request = g_new0(buffer_io_task, 1);
    if (!entry_u64(widgets->load_image_base,
                   &request->image_options.address_base)) {
        gtk_label_set_text(GTK_LABEL(widgets->load_image_status),
                           "Enter a valid source address base.");
        g_free(request);
        return;
    }
    if (widgets->buffer_cancellable != NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->load_image_status),
                           "Another buffer file operation is active.");
        g_free(request);
        return;
    }
    request->kind = BUFFER_IO_LOAD_IMAGE;
    request->path = g_strdup(path);
    request->target_offset = target;
    request->load_clear_mode = clear_mode;
    request->image_options.format = (lt48_image_format)format;
    request->image_options.buffer_size = (size_t)size - target;
    request->image_options.gap_policy = gap_policy == 0u ?
        LT48_IMAGE_GAPS_PRESERVE : LT48_IMAGE_GAPS_FILL;
    request->image_options.gap_fill_value = (unsigned char)gap_fill;
    if (clear_mode == 0u) {
        status = snapshot_resident_buffer(
            widgets->device_buffer, size, &request->snapshot);
    } else {
        unsigned char clear_value = clear_mode == 2u ? 0x00u : 0xffu;
        if (clear_mode == 1u) {
            lt48_catalogue_entry entry;
            if (widgets->installation == NULL ||
                !widgets->has_selected_device ||
                lt48_installation_get_device(
                    widgets->installation, widgets->selected_device_index,
                    &entry) != LT48_OK || entry.unknown3 > 1u) {
                gtk_label_set_text(
                    GTK_LABEL(widgets->load_image_status),
                    "Device blank-state clearing is available only when an "
                    "EPROM or PROM definition is selected.");
                buffer_io_task_free(request);
                return;
            }
        }
        status = lt48_buffer_create(size, clear_value, &request->snapshot);
    }
    if (status != LT48_OK) {
        char *message = g_strdup_printf(
            "Cannot prepare the target buffer: %s.",
            lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->load_image_status), message);
        g_free(message);
        buffer_io_task_free(request);
        return;
    }
    widgets->buffer_cancellable = g_cancellable_new();
    widgets->active_buffer_dialog = widgets->load_image_window;
    gtk_widget_set_sensitive(widgets->buffer_controls, FALSE);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, TRUE);
    gtk_widget_set_sensitive(widgets->load_image_controls, FALSE);
    gtk_widget_set_sensitive(widgets->load_image_apply, FALSE);
    gtk_widget_set_sensitive(widgets->load_image_cancel, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->load_image_status),
                       "Loading and validating image…");
    gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                       "Loading image in bounded cancellable chunks…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->buffer_cancellable, buffer_io_finished);
    g_task_set_task_data(task, request, buffer_io_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_run_in_thread(task, buffer_io_worker);
    g_object_unref(task);
}

static void save_image_execute(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->save_image_path));
    buffer_io_task *request;
    GTask *task;
    lt48_status status;
    uint32_t start;
    uint32_t length;
    uint64_t address_base;
    guint format = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->save_image_format));
    guint byte_mode = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->save_image_byte_mode));
    int range_known = 0;
    (void)button;
    if (widgets->device_buffer == NULL || path == NULL || path[0] == '\0' ||
        !entry_u32(widgets->save_image_start, UINT32_MAX, &start) ||
        !entry_u32(widgets->save_image_length, UINT32_MAX, &length) ||
        !entry_u64(widgets->save_image_base, &address_base) || length == 0u ||
        (size_t)start > lt48_buffer_size(widgets->device_buffer) ||
        (size_t)length >
            lt48_buffer_size(widgets->device_buffer) - (size_t)start ||
        format > LT48_IMAGE_FORMAT_HP64000_ABSOLUTE || byte_mode > 6u) {
        gtk_label_set_text(GTK_LABEL(widgets->save_image_status),
            "Enter a valid buffer range, output address, format and byte mode.");
        return;
    }
    if (widgets->buffer_cancellable != NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->save_image_status),
                           "Another buffer file operation is active.");
        return;
    }
    request = g_new0(buffer_io_task, 1);
    request->kind = BUFFER_IO_SAVE;
    request->path = g_strdup(path);
    request->start = start;
    request->length = length;
    request->source_revision = widgets->buffer_revision;
    request->extract = start != 0u ||
        (size_t)length != lt48_buffer_size(widgets->device_buffer);
    request->export_options.format = (lt48_image_format)format;
    request->export_options.buffer_offset = 0u;
    request->export_options.buffer_length = length;
    request->export_options.address_base = address_base;
    request->export_options.byte_mode = (lt48_image_byte_mode)byte_mode;
    request->export_options.overwrite = 0;
    status = lt48_buffer_range_known(
        widgets->device_buffer, start, length, &range_known);
    if (status == LT48_OK && !range_known) status = LT48_SAFETY_REJECTED;
    if (status == LT48_OK)
        status = lt48_buffer_create(length, 0u, &request->snapshot);
    if (status == LT48_OK)
        status = lt48_buffer_write(
            request->snapshot, 0u,
            lt48_buffer_data(widgets->device_buffer) + start, length);
    if (status != LT48_OK) {
        char *message = g_strdup_printf(
            "Cannot snapshot selected bytes for save: %s.",
            lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->save_image_status), message);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), message);
        g_free(message);
        buffer_io_task_free(request);
        return;
    }
    widgets->buffer_cancellable = g_cancellable_new();
    widgets->active_buffer_dialog = widgets->save_image_window;
    gtk_widget_set_sensitive(widgets->buffer_controls, FALSE);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, TRUE);
    gtk_widget_set_sensitive(widgets->save_image_controls, FALSE);
    gtk_widget_set_sensitive(widgets->save_image_apply, FALSE);
    gtk_widget_set_sensitive(widgets->save_image_cancel, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->save_image_status),
                       "Encoding immutable buffer snapshot…");
    gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                       "Saving image in bounded cancellable chunks…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->buffer_cancellable, buffer_io_finished);
    g_task_set_task_data(task, request, buffer_io_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_run_in_thread(task, buffer_io_worker);
    g_object_unref(task);
}

static void buffer_create(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    lt48_buffer *created = NULL;
    uint32_t size;
    uint32_t value;
    lt48_status status;
    char text[32];
    (void)button;
    if (!entry_u32(widgets->buffer_size_entry, UINT32_MAX, &size) ||
        !entry_u32(widgets->buffer_value_entry, UINT8_MAX, &value) ||
        size == 0u) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Enter a nonzero size and byte value 0..255");
        return;
    }
    status = lt48_buffer_create(size, (unsigned char)value, &created);
    if (status != LT48_OK) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           lt48_status_string(status));
        return;
    }
    buffer_edit_discard_undo(widgets);
    lt48_buffer_close(widgets->device_buffer);
    widgets->device_buffer = created;
    ++widgets->buffer_revision;
    invalidate_operation_preparation(widgets, 1);
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_start_entry), "0");
    g_snprintf(text, sizeof(text), "%u", size);
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_length_entry), text);
    buffer_show_range(widgets, "Created filled image", 0u, size);
}

static void buffer_checksum(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t start;
    uint32_t length;
    (void)button;
    if (!buffer_range(widgets, &start, &length)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Select a range wholly inside the current buffer");
        return;
    }
    buffer_show_range(widgets, "Range checksum", start, length);
}

static void buffer_compare(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->buffer_compare_entry)
    );
    buffer_io_task *request;
    GTask *task;
    uint32_t start;
    uint32_t length;
    lt48_status status;
    int range_known = 0;
    (void)button;
    if (!buffer_range(widgets, &start, &length) || length == 0u ||
        path == NULL || path[0] == '\0') {
        gtk_label_set_text(
            GTK_LABEL(widgets->buffer_status),
            "Select a nonempty retained range and an exact-size comparison file"
        );
        return;
    }
    if (widgets->buffer_cancellable != NULL) return;
    request = g_new0(buffer_io_task, 1);
    request->kind = BUFFER_IO_COMPARE;
    request->path = g_strdup(path);
    request->start = start;
    request->length = length;
    request->source_revision = widgets->buffer_revision;
    status = lt48_buffer_range_known(
        widgets->device_buffer, start, length, &range_known);
    if (status == LT48_OK && !range_known)
        status = LT48_SAFETY_REJECTED;
    if (status == LT48_OK)
        status = lt48_buffer_create(length, 0u, &request->snapshot);
    if (status == LT48_OK)
        status = lt48_buffer_write(
            request->snapshot, 0u,
            lt48_buffer_data(widgets->device_buffer) + start, length);
    if (status != LT48_OK) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Cannot snapshot the selected comparison range");
        buffer_io_task_free(request);
        return;
    }
    widgets->buffer_cancellable = g_cancellable_new();
    gtk_widget_set_sensitive(widgets->buffer_controls, FALSE);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
        "Loading and comparing the exact-size file outside the UI thread…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->buffer_cancellable, buffer_io_finished);
    g_task_set_task_data(task, request, buffer_io_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_run_in_thread(task, buffer_io_worker);
    g_object_unref(task);
}

static void buffer_fill(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t start;
    uint32_t length;
    uint32_t value;
    lt48_status status;
    (void)button;
    if (!buffer_range(widgets, &start, &length) ||
        !entry_u32(widgets->buffer_value_entry, UINT8_MAX, &value)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Select a valid range and byte value 0..255");
        return;
    }
    status = lt48_buffer_fill(widgets->device_buffer, start, length,
                              (unsigned char)value);
    if (status != LT48_OK) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           lt48_status_string(status));
        return;
    }
    buffer_edit_discard_undo(widgets);
    ++widgets->buffer_revision;
    invalidate_operation_preparation(widgets, 1);
    buffer_show_range(widgets, "Filled selected range", start, length);
}

static void buffer_edit(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t offset;
    uint32_t width;
    uint64_t value;
    uint64_t readback;
    lt48_buffer_byte_order order;
    lt48_status status;
    char *action;
    (void)button;
    if (widgets->device_buffer == NULL ||
        !entry_u32(widgets->buffer_start_entry, UINT32_MAX, &offset) ||
        !entry_u32(widgets->buffer_word_entry, 8u, &width) ||
        (width != 1u && width != 2u && width != 4u && width != 8u) ||
        !entry_u64(widgets->buffer_value_entry, &value)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Enter an offset, width 1/2/4/8, and fitting value");
        return;
    }
    order = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->buffer_byte_order_dropdown)) == 0u ?
        LT48_BUFFER_LITTLE_ENDIAN : LT48_BUFFER_BIG_ENDIAN;
    status = lt48_buffer_set_word(widgets->device_buffer, offset, width,
                                  order, value);
    if (status == LT48_OK)
        status = lt48_buffer_get_word(widgets->device_buffer, offset, width,
                                      order, &readback);
    if (status != LT48_OK) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Typed edit rejected: range, width, or value invalid");
        return;
    }
    buffer_edit_discard_undo(widgets);
    ++widgets->buffer_revision;
    invalidate_operation_preparation(widgets, 1);
    action = g_strdup_printf(
        "Edited %u-byte %s-endian word: 0x%0*" G_GINT64_MODIFIER "x",
        width, order == LT48_BUFFER_LITTLE_ENDIAN ? "little" : "big",
        (int)(width * 2u), (guint64)readback
    );
    buffer_show_range(widgets, action, offset, width);
    g_free(action);
}

static void buffer_edit_refresh(app_widgets *widgets) {
    enum { PAGE_BYTES = 512, BYTES_PER_LINE = 16 };
    uint32_t offset;
    size_t size;
    size_t shown;
    const unsigned char *data;
    GString *view;
    GtkTextBuffer *text_buffer;
    char *status;
    if (widgets->buffer_edit_view == NULL) return;
    text_buffer = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->buffer_edit_view));
    if (buffer_edit_buffer(widgets) == NULL ||
        !entry_u32(widgets->buffer_edit_address, UINT32_MAX, &offset)) {
        gtk_text_buffer_set_text(text_buffer, "", -1);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Load or read a buffer and enter a valid address");
        return;
    }
    size = lt48_buffer_size(buffer_edit_buffer(widgets));
    if ((size_t)offset >= size) {
        gtk_text_buffer_set_text(text_buffer, "", -1);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "View address is outside the resident buffer");
        return;
    }
    shown = size - (size_t)offset;
    if (shown > PAGE_BYTES) shown = PAGE_BYTES;
    data = lt48_buffer_data(buffer_edit_buffer(widgets));
    view = g_string_new(NULL);
    for (size_t line = 0u; line < shown; line += BYTES_PER_LINE) {
        size_t line_size = shown - line;
        if (line_size > BYTES_PER_LINE) line_size = BYTES_PER_LINE;
        g_string_append_printf(view, "%08zx  ", (size_t)offset + line);
        for (size_t index = 0u; index < BYTES_PER_LINE; ++index) {
            if (index < line_size)
                g_string_append_printf(
                    view, "%02x ", data[(size_t)offset + line + index]);
            else
                g_string_append(view, "   ");
            if (index == 7u) g_string_append_c(view, ' ');
        }
        g_string_append(view, " |");
        for (size_t index = 0u; index < line_size; ++index) {
            unsigned char value = data[(size_t)offset + line + index];
            g_string_append_c(view,
                g_ascii_isprint(value) ? (char)value : '.');
        }
        g_string_append(view, "|\n");
    }
    gtk_text_buffer_set_text(text_buffer, view->str, -1);
    g_string_free(view, TRUE);
    status = g_strdup_printf(
        "Showing 0x%x–0x%zx of %zu bytes; edit two-digit hex cells in place "
        "or enter replacement bytes below",
        offset, (size_t)offset + shown - 1u, size);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status), status);
    g_free(status);
}

static void buffer_edit_go(GtkButton *button, gpointer user_data) {
    (void)button;
    buffer_edit_refresh(user_data);
}

static void buffer_edit_move_page(GtkButton *button, gpointer user_data) {
    enum { PAGE_BYTES = 512 };
    app_widgets *widgets = user_data;
    uint32_t offset = 0u;
    size_t size;
    int direction = GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(button), "labtool48-page-direction"));
    char address[32];
    if (buffer_edit_buffer(widgets) == NULL) {
        buffer_edit_refresh(widgets);
        return;
    }
    (void)entry_u32(widgets->buffer_edit_address, UINT32_MAX, &offset);
    size = lt48_buffer_size(buffer_edit_buffer(widgets));
    if (direction < 0)
        offset = offset > PAGE_BYTES ? offset - PAGE_BYTES : 0u;
    else if ((size_t)offset + PAGE_BYTES < size)
        offset += PAGE_BYTES;
    g_snprintf(address, sizeof(address), "%u", offset);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->buffer_edit_address), address);
    buffer_edit_refresh(widgets);
}

static void buffer_edit_search_bytes(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t address = 0u;
    size_t pattern_size = 0u;
    unsigned char *pattern;
    const unsigned char *data;
    size_t size;
    size_t start;
    int find_next = GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(button), "labtool48-find-next"));
    if (buffer_edit_buffer(widgets) == NULL) {
        buffer_edit_refresh(widgets);
        return;
    }
    pattern = hex_bytes(gtk_editable_get_text(
        GTK_EDITABLE(widgets->buffer_edit_search)), &pattern_size);
    if (pattern == NULL || pattern_size == 0u) {
        g_free(pattern);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Enter hexadecimal bytes to search for");
        return;
    }
    (void)entry_u32(widgets->buffer_edit_address, UINT32_MAX, &address);
    size = lt48_buffer_size(buffer_edit_buffer(widgets));
    data = lt48_buffer_data(buffer_edit_buffer(widgets));
    start = (size_t)address + (find_next ? 1u : 0u);
    for (size_t offset = start;
         offset <= size && pattern_size <= size - offset; ++offset) {
        if (memcmp(data + offset, pattern, pattern_size) == 0) {
            char text[32];
            char *status;
            g_snprintf(text, sizeof(text), "%zu", offset);
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->buffer_edit_address), text);
            buffer_edit_refresh(widgets);
            status = g_strdup_printf(
                "Found %zu byte%s at device address 0x%zx",
                pattern_size, pattern_size == 1u ? "" : "s", offset);
            gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status), status);
            g_free(status);
            g_free(pattern);
            return;
        }
    }
    g_free(pattern);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                       "Pattern was not found at or after this address");
}

static void buffer_edit_checksum_page(GtkButton *button, gpointer user_data) {
    enum { PAGE_BYTES = 512 };
    app_widgets *widgets = user_data;
    lt48_buffer_checksum checksum;
    uint32_t offset;
    size_t length;
    char sha[LT48_SHA256_SIZE * 2u + 1u];
    char *status;
    (void)button;
    if (buffer_edit_buffer(widgets) == NULL ||
        !entry_u32(widgets->buffer_edit_address, UINT32_MAX, &offset) ||
        (size_t)offset >= lt48_buffer_size(buffer_edit_buffer(widgets))) {
        buffer_edit_refresh(widgets);
        return;
    }
    length = lt48_buffer_size(buffer_edit_buffer(widgets)) - (size_t)offset;
    if (length > PAGE_BYTES) length = PAGE_BYTES;
    if (lt48_buffer_checksum_range(
            buffer_edit_buffer(widgets), offset, length, &checksum) != LT48_OK)
        return;
    digest_hex(checksum.sha256, LT48_SHA256_SIZE, sha);
    status = g_strdup_printf(
        "Page checksum: 0x%x + %zu bytes · additive32 %u · SHA-256 %s",
        offset, length, checksum.additive32, sha);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status), status);
    g_free(status);
}

static void buffer_edit_undo(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    lt48_status status;
    char address[32];
    char *message;
    (void)button;
    if (buffer_edit_buffer(widgets) == NULL || !widgets->buffer_edit_has_undo) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "There is no Buffer Edit change to undo");
        return;
    }
    status = lt48_buffer_write(
        buffer_edit_buffer(widgets), widgets->buffer_edit_undo_offset,
        widgets->buffer_edit_undo_data, widgets->buffer_edit_undo_size);
    if (status != LT48_OK) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Undo failed because the buffer has changed shape");
        return;
    }
    buffer_edit_note_mutation(widgets);
    g_snprintf(address, sizeof(address), "%u",
               widgets->buffer_edit_undo_offset);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->buffer_edit_address), address);
    message = g_strdup_printf(
        "Restored %zu byte%s at device address 0x%x",
        widgets->buffer_edit_undo_size,
        widgets->buffer_edit_undo_size == 1u ? "" : "s",
        widgets->buffer_edit_undo_offset);
    if (!widgets->buffer_edit_secondary)
        buffer_show_range(
            widgets, message, widgets->buffer_edit_undo_offset,
            (uint32_t)widgets->buffer_edit_undo_size);
    widgets->buffer_edit_has_undo = 0;
    g_clear_pointer(&widgets->buffer_edit_undo_data, g_free);
    widgets->buffer_edit_undo_size = 0u;
    buffer_edit_refresh(widgets);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status), message);
    g_free(message);
}

static void buffer_edit_finish_mutation(
    app_widgets *widgets, const char *action, uint32_t offset, uint32_t length,
    GtkWindow *tool_window
) {
    char address[32];
    char range_length[32];
    buffer_edit_note_mutation(widgets);
    g_snprintf(address, sizeof(address), "%u", offset);
    g_snprintf(range_length, sizeof(range_length), "%u", length);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->buffer_edit_address), address);
    if (!widgets->buffer_edit_secondary) {
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->buffer_start_entry), address);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->buffer_length_entry), range_length);
        buffer_show_range(widgets, action, offset, length);
    }
    buffer_edit_refresh(widgets);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status), action);
    if (tool_window != NULL)
        gtk_widget_set_visible(GTK_WIDGET(tool_window), FALSE);
}

static void buffer_edit_apply_page(GtkButton *button, gpointer user_data) {
    enum { PAGE_BYTES = 512 };
    app_widgets *widgets = user_data;
    GtkTextBuffer *text_buffer;
    GtkTextIter start_iter;
    GtkTextIter end_iter;
    char *text;
    char **lines;
    unsigned char bytes[PAGE_BYTES];
    size_t byte_count = 0u;
    size_t expected;
    size_t buffer_size;
    uint32_t offset;
    lt48_status status;
    char *action;
    (void)button;
    if (buffer_edit_buffer(widgets) == NULL ||
        !entry_u32(widgets->buffer_edit_address, UINT32_MAX, &offset)) {
        buffer_edit_refresh(widgets);
        return;
    }
    buffer_size = lt48_buffer_size(buffer_edit_buffer(widgets));
    if ((size_t)offset >= buffer_size) {
        buffer_edit_refresh(widgets);
        return;
    }
    expected = buffer_size - (size_t)offset;
    if (expected > PAGE_BYTES) expected = PAGE_BYTES;
    text_buffer = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->buffer_edit_view));
    gtk_text_buffer_get_bounds(text_buffer, &start_iter, &end_iter);
    text = gtk_text_buffer_get_text(
        text_buffer, &start_iter, &end_iter, FALSE);
    lines = g_strsplit(text, "\n", -1);
    for (size_t line_index = 0u;
         lines[line_index] != NULL && byte_count < PAGE_BYTES;
         ++line_index) {
        char *line = lines[line_index];
        char *hex = strlen(line) >= 10u ? line + 10u : NULL;
        char *hex_end = hex == NULL ? NULL : strstr(hex, " |");
        if (hex == NULL || hex_end == NULL) continue;
        for (char *cursor = hex; cursor + 1 < hex_end;) {
            while (cursor < hex_end && !g_ascii_isxdigit(*cursor)) ++cursor;
            if (cursor + 1 >= hex_end ||
                !g_ascii_isxdigit(cursor[1])) break;
            bytes[byte_count++] = (unsigned char)(
                (g_ascii_xdigit_value(cursor[0]) << 4) |
                g_ascii_xdigit_value(cursor[1]));
            cursor += 2;
        }
    }
    g_strfreev(lines);
    g_free(text);
    if (byte_count != expected) {
        gtk_label_set_text(
            GTK_LABEL(widgets->buffer_edit_status),
            "Edited page is malformed; each displayed byte must remain a two-digit hex cell");
        return;
    }
    if (!buffer_edit_capture_undo(widgets, offset, byte_count)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Could not snapshot the page for Undo");
        return;
    }
    status = lt48_buffer_write(
        buffer_edit_buffer(widgets), offset, bytes, byte_count);
    if (status != LT48_OK) {
        buffer_edit_discard_undo(widgets);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           lt48_status_string(status));
        return;
    }
    widgets->buffer_edit_has_undo = 1;
    action = g_strdup_printf(
        "Applied %zu edited page byte%s at device address 0x%x",
        byte_count, byte_count == 1u ? "" : "s", offset);
    buffer_edit_finish_mutation(
        widgets, action, offset, (uint32_t)byte_count, NULL);
    g_free(action);
}

static void buffer_edit_fill_apply(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t start;
    uint32_t end;
    uint32_t value;
    uint64_t length64;
    uint32_t length;
    lt48_status status;
    char *action;
    (void)button;
    if (!entry_u32(widgets->buffer_fill_start, UINT32_MAX, &start) ||
        !entry_u32(widgets->buffer_fill_end, UINT32_MAX, &end) ||
        !entry_u32(widgets->buffer_fill_value, UINT8_MAX, &value) ||
        end < start) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Fill requires an inclusive From/To range and byte value");
        return;
    }
    length64 = (uint64_t)end - start + 1u;
    if (length64 > UINT32_MAX ||
        !buffer_edit_capture_undo(widgets, start, (size_t)length64)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Fill range is outside the resident buffer");
        return;
    }
    length = (uint32_t)length64;
    status = lt48_buffer_fill(
        buffer_edit_buffer(widgets), start, length, (unsigned char)value);
    if (status != LT48_OK) {
        buffer_edit_discard_undo(widgets);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           lt48_status_string(status));
        return;
    }
    widgets->buffer_edit_has_undo = 1;
    action = g_strdup_printf(
        "Filled 0x%x–0x%x with 0x%02x", start, end, value);
    buffer_edit_finish_mutation(
        widgets, action, start, length, widgets->buffer_fill_window);
    g_free(action);
}

static void buffer_edit_copy_apply(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t source;
    uint32_t destination;
    uint32_t length;
    size_t size;
    unsigned char *copy;
    lt48_status status;
    char *action;
    (void)button;
    if (buffer_edit_buffer(widgets) == NULL ||
        !entry_u32(widgets->buffer_copy_source, UINT32_MAX, &source) ||
        !entry_u32(widgets->buffer_copy_destination, UINT32_MAX, &destination) ||
        !entry_u32(widgets->buffer_copy_length, UINT32_MAX, &length) ||
        length == 0u) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Copy requires source, destination and nonzero length");
        return;
    }
    size = lt48_buffer_size(buffer_edit_buffer(widgets));
    if ((size_t)source > size || length > size - (size_t)source ||
        !buffer_edit_capture_undo(widgets, destination, length)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Copy source or destination is outside the buffer");
        return;
    }
    copy = g_memdup2(lt48_buffer_data(buffer_edit_buffer(widgets)) + source,
                     length);
    if (copy == NULL) {
        buffer_edit_discard_undo(widgets);
        return;
    }
    status = lt48_buffer_write(
        buffer_edit_buffer(widgets), destination, copy, length);
    g_free(copy);
    if (status != LT48_OK) {
        buffer_edit_discard_undo(widgets);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           lt48_status_string(status));
        return;
    }
    widgets->buffer_edit_has_undo = 1;
    action = g_strdup_printf(
        "Copied %u byte%s from 0x%x to 0x%x", length,
        length == 1u ? "" : "s", source, destination);
    buffer_edit_finish_mutation(
        widgets, action, destination, length, widgets->buffer_copy_window);
    g_free(action);
}

static void buffer_edit_swap_apply(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t start;
    uint32_t length;
    uint32_t word_size;
    unsigned char *swapped;
    lt48_status status;
    char *action;
    (void)button;
    if (buffer_edit_buffer(widgets) == NULL ||
        !entry_u32(widgets->buffer_swap_start, UINT32_MAX, &start) ||
        !entry_u32(widgets->buffer_swap_length, UINT32_MAX, &length) ||
        !entry_u32(widgets->buffer_swap_word_size, 8u, &word_size) ||
        (word_size != 2u && word_size != 4u && word_size != 8u) ||
        length == 0u || length % word_size != 0u ||
        !buffer_edit_capture_undo(widgets, start, length)) {
        gtk_label_set_text(
            GTK_LABEL(widgets->buffer_edit_status),
            "Swap requires a valid range divisible by word size 2, 4 or 8");
        return;
    }
    swapped = g_memdup2(
        lt48_buffer_data(buffer_edit_buffer(widgets)) + start, length);
    if (swapped == NULL) {
        buffer_edit_discard_undo(widgets);
        return;
    }
    for (size_t word = 0u; word < length; word += word_size) {
        for (size_t left = 0u; left < word_size / 2u; ++left) {
            unsigned char temporary = swapped[word + left];
            swapped[word + left] = swapped[word + word_size - left - 1u];
            swapped[word + word_size - left - 1u] = temporary;
        }
    }
    status = lt48_buffer_write(
        buffer_edit_buffer(widgets), start, swapped, length);
    g_free(swapped);
    if (status != LT48_OK) {
        buffer_edit_discard_undo(widgets);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           lt48_status_string(status));
        return;
    }
    widgets->buffer_edit_has_undo = 1;
    action = g_strdup_printf(
        "Reversed byte order in %u-byte words across 0x%x + %u bytes",
        word_size, start, length);
    buffer_edit_finish_mutation(
        widgets, action, start, length, widgets->buffer_swap_window);
    g_free(action);
}

static void buffer_edit_present_tool(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkWindow *window = g_object_get_data(
        G_OBJECT(button), "labtool48-buffer-tool-window");
    const char *address = gtk_editable_get_text(
        GTK_EDITABLE(widgets->buffer_edit_address));
    GtkWidget *start_entry = g_object_get_data(
        G_OBJECT(button), "labtool48-buffer-tool-start");
    if (start_entry != NULL)
        gtk_editable_set_text(GTK_EDITABLE(start_entry), address);
    gtk_window_present(window);
}

static void buffer_edit_hex_apply(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t offset;
    size_t byte_count = 0u;
    unsigned char *bytes;
    lt48_status status;
    char *action;
    (void)button;
    if (buffer_edit_buffer(widgets) == NULL ||
        !entry_u32(widgets->buffer_edit_address, UINT32_MAX, &offset)) {
        gtk_label_set_text(
            GTK_LABEL(widgets->buffer_edit_status),
            "Load or read a buffer and enter a valid address");
        return;
    }
    bytes = hex_bytes(gtk_editable_get_text(
        GTK_EDITABLE(widgets->buffer_edit_hex)), &byte_count);
    if (bytes == NULL || byte_count == 0u || byte_count > UINT32_MAX) {
        g_free(bytes);
        gtk_label_set_text(
            GTK_LABEL(widgets->buffer_edit_status),
            "Enter an even number of hexadecimal digits without separators");
        return;
    }
    if ((size_t)offset > lt48_buffer_size(buffer_edit_buffer(widgets)) ||
        byte_count > lt48_buffer_size(buffer_edit_buffer(widgets)) - (size_t)offset) {
        g_free(bytes);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Replacement bytes extend beyond the buffer");
        return;
    }
    if (!buffer_edit_capture_undo(widgets, offset, byte_count)) {
        g_free(bytes);
        gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status),
                           "Could not snapshot the edited range for Undo");
        return;
    }
    widgets->buffer_edit_has_undo = 0;
    status = lt48_buffer_write(
        buffer_edit_buffer(widgets), offset, bytes, byte_count);
    g_free(bytes);
    if (status != LT48_OK) {
        char *message = g_strdup_printf(
            "Edit rejected: %s", lt48_status_string(status));
        gtk_label_set_text(
            GTK_LABEL(widgets->buffer_edit_status), message);
        g_free(message);
        g_clear_pointer(&widgets->buffer_edit_undo_data, g_free);
        widgets->buffer_edit_undo_size = 0u;
        return;
    }
    widgets->buffer_edit_has_undo = 1;
    buffer_edit_note_mutation(widgets);
    {
        char address[32];
        char length[32];
        g_snprintf(address, sizeof(address), "%u", offset);
        g_snprintf(length, sizeof(length), "%zu", byte_count);
        if (!widgets->buffer_edit_secondary) {
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->buffer_start_entry), address);
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->buffer_length_entry), length);
        }
    }
    action = g_strdup_printf(
        "Edited %zu byte%s at device address 0x%x",
        byte_count, byte_count == 1u ? "" : "s", offset);
    if (!widgets->buffer_edit_secondary)
        buffer_show_range(widgets, action, offset, (uint32_t)byte_count);
    buffer_edit_refresh(widgets);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status), action);
    g_free(action);
}

static void buffer_edit_window_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *address;
    (void)button;
    if (widgets->buffer_edit_window == NULL) return;
    widgets->buffer_edit_secondary = 0;
    gtk_window_set_title(widgets->buffer_edit_window, "Buffer Edit");
    address = gtk_editable_get_text(
        GTK_EDITABLE(widgets->buffer_start_entry));
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->buffer_edit_address), address);
    buffer_edit_refresh(widgets);
    gtk_window_present(widgets->buffer_edit_window);
}

static void secondary_buffer_edit_window(
    app_widgets *widgets, int requested_kind
) {
    const char *title;
    char *status;
    if (widgets->buffer_edit_window == NULL ||
        widgets->secondary_buffer == NULL ||
        widgets->secondary_buffer_kind != requested_kind)
        return;
    widgets->buffer_edit_secondary = 1;
    buffer_edit_discard_undo(widgets);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->buffer_edit_address), "0");
    title = requested_kind == SECONDARY_BUFFER_ENCRYPTION ?
        "Encryption Array Edit" : "Data Buffer Edit";
    gtk_window_set_title(widgets->buffer_edit_window, title);
    buffer_edit_refresh(widgets);
    status = g_strdup_printf(
        "%s — %zu bytes retained separately from the main device buffer; "
        "initialized to FF for this selected definition",
        title, lt48_buffer_size(widgets->secondary_buffer));
    gtk_label_set_text(GTK_LABEL(widgets->buffer_edit_status), status);
    g_free(status);
    gtk_window_present(widgets->buffer_edit_window);
}

static void buffer_save(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->buffer_output_entry)
    );
    uint32_t start = 0u;
    uint32_t length;
    int extract = GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(button), "labtool48-extract")
    );
    buffer_io_task *request;
    GTask *task;
    lt48_status status;
    int range_known = 0;
    if (widgets->device_buffer == NULL || path == NULL || path[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                           "Create or load a buffer and enter an output path");
        return;
    }
    if (extract) {
        if (!buffer_range(widgets, &start, &length)) {
            gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                               "Select a range wholly inside the buffer");
            return;
        }
    } else {
        if (lt48_buffer_size(widgets->device_buffer) > UINT32_MAX) {
            gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
                               "Buffer is too large for this frontend");
            return;
        }
        length = (uint32_t)lt48_buffer_size(widgets->device_buffer);
    }
    if (widgets->buffer_cancellable != NULL) return;
    request = g_new0(buffer_io_task, 1);
    request->kind = BUFFER_IO_SAVE;
    request->path = g_strdup(path);
    request->start = start;
    request->length = length;
    request->source_revision = widgets->buffer_revision;
    request->extract = extract;
    request->export_options.format = LT48_IMAGE_FORMAT_RAW_BINARY;
    request->export_options.buffer_offset = 0u;
    request->export_options.buffer_length = length;
    request->export_options.address_base = 0u;
    request->export_options.byte_mode = LT48_IMAGE_BYTES_NORMAL;
    request->export_options.overwrite = 0;
    status = lt48_buffer_range_known(
        widgets->device_buffer, start, length, &range_known);
    if (status == LT48_OK && !range_known)
        status = LT48_SAFETY_REJECTED;
    if (status == LT48_OK)
        status = lt48_buffer_create(length == 0u ? 1u : length, 0u,
                                    &request->snapshot);
    if (status == LT48_OK && length != 0u)
        status = lt48_buffer_write(
            request->snapshot, 0u,
            lt48_buffer_data(widgets->device_buffer) + start, length);
    if (status != LT48_OK) {
        char *message = g_strdup_printf(
            "Cannot snapshot selected bytes for save: %s.",
            lt48_status_string(status)
        );
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), message);
        g_free(message);
        buffer_io_task_free(request);
        return;
    }
    widgets->buffer_cancellable = g_cancellable_new();
    gtk_widget_set_sensitive(widgets->buffer_controls, FALSE);
    gtk_widget_set_sensitive(widgets->buffer_cancel_button, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->buffer_status),
        "Saving an immutable buffer snapshot in bounded cancellable chunks…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->buffer_cancellable, buffer_io_finished);
    g_task_set_task_data(task, request, buffer_io_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_run_in_thread(task, buffer_io_worker);
    g_object_unref(task);
}

static GtkWidget *build_programmer_page(app_widgets *widgets) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *target = gtk_grid_new();
    GtkWidget *warning;
    GtkWidget *button;
    GtkWidget *scroll;
    char *firmware_path = NULL;
    const char *firmware_environment =
        g_getenv("LABTOOL48_FX2_FIRMWARE");
    widgets->programmer_status = gtk_label_new(
        "Descriptor-only discovery; no USB data is sent");
    gtk_label_set_xalign(GTK_LABEL(widgets->programmer_status), 0.0F);
    gtk_box_append(GTK_BOX(box), widgets->programmer_status);
    scroll = make_scrolled_list(&widgets->programmer_list);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(widgets->programmer_list), GTK_SELECTION_SINGLE
    );
    g_signal_connect(widgets->programmer_list, "row-selected",
                     G_CALLBACK(programmer_selected), widgets);
    button = gtk_button_new_with_label("Refresh programmers");
    g_signal_connect(button, "clicked", G_CALLBACK(refresh_programmers), widgets);
    gtk_box_append(GTK_BOX(box), button);
    widgets->programmer_bus_entry = gtk_entry_new();
    widgets->programmer_address_entry = gtk_entry_new();
    gtk_editable_set_editable(
        GTK_EDITABLE(widgets->programmer_bus_entry), FALSE);
    gtk_editable_set_editable(
        GTK_EDITABLE(widgets->programmer_address_entry), FALSE);
    widgets->programmer_timeout_entry = gtk_entry_new();
    widgets->programmer_initialize_button =
        gtk_button_new_with_label("Check transport readiness");
    widgets->programmer_fx2_path_entry = gtk_entry_new();
    widgets->programmer_fx2_confirmation_entry = gtk_entry_new();
    widgets->programmer_fx2_prepare_button =
        gtk_button_new_with_label("Prepare FX2 load (offline)");
    widgets->programmer_fx2_execute_button =
        gtk_button_new_with_label("Execute volatile FX2 load");
    gtk_widget_set_sensitive(widgets->programmer_fx2_execute_button, FALSE);
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->programmer_bus_entry), "bus"
    );
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->programmer_address_entry), "address"
    );
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->programmer_timeout_entry), "2000"
    );
    if (firmware_environment != NULL && firmware_environment[0] != '\0')
        firmware_path = g_strdup(firmware_environment);
    if (firmware_path == NULL)
        firmware_path = project_path("analysis/labtool-fx2.hex");
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->programmer_fx2_path_entry),
        firmware_path == NULL ? "analysis/labtool-fx2.hex" : firmware_path
    );
    g_free(firmware_path);
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->programmer_fx2_confirmation_entry),
        "paste the exact 64-character confirmation from Prepare"
    );
    gtk_widget_set_hexpand(widgets->programmer_fx2_path_entry, TRUE);
    gtk_widget_set_hexpand(
        widgets->programmer_fx2_confirmation_entry, TRUE
    );
    gtk_grid_set_row_spacing(GTK_GRID(target), 6);
    gtk_grid_set_column_spacing(GTK_GRID(target), 8);
    gtk_grid_attach(GTK_GRID(target), gtk_label_new("Detected bus:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(target), widgets->programmer_bus_entry,
                    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(target), gtk_label_new("Address:"),
                    2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(target), widgets->programmer_address_entry,
                    3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(target), gtk_label_new("Timeout ms:"),
                    4, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(target), widgets->programmer_timeout_entry,
                    5, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(target), widgets->programmer_initialize_button,
                    6, 0, 1, 1);
    g_signal_connect(widgets->programmer_fx2_prepare_button, "clicked",
                     G_CALLBACK(fx2_prepare), widgets);
    g_signal_connect(widgets->programmer_fx2_execute_button, "clicked",
                     G_CALLBACK(fx2_execute), widgets);
    g_signal_connect(widgets->programmer_initialize_button, "clicked",
                     G_CALLBACK(programmer_initialize), widgets);
    warning = gtk_label_new(
        "Debug information only. Normal startup discovers the programmer, "
        "loads its volatile FX2 firmware when required, and initializes the "
        "transport automatically."
    );
    gtk_label_set_xalign(GTK_LABEL(warning), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(warning), TRUE);
    gtk_widget_add_css_class(warning, "warning");
    gtk_box_append(GTK_BOX(box), target);
    gtk_box_append(GTK_BOX(box), warning);
    return box;
}

static void report_inspect(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->report_path_entry)
    );
    lt48_operation_report *report = NULL;
    lt48_operation_report_info info;
    const unsigned char *json = NULL;
    size_t json_size = 0u;
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->report_preview)
    );
    lt48_status status;
    GString *summary;
    (void)button;
    if (path == NULL || path[0] == '\0') {
        gtk_widget_remove_css_class(widgets->report_status, "error");
        gtk_widget_remove_css_class(widgets->report_status, "warning");
        gtk_label_set_text(GTK_LABEL(widgets->report_status),
                           "Enter an operation-report path");
        gtk_text_buffer_set_text(text_buffer, "", 0);
        return;
    }
    status = lt48_operation_report_open(path, &report);
    if (status == LT48_OK)
        status = lt48_operation_report_get_info(report, &info);
    if (status == LT48_OK)
        status = lt48_operation_report_json(report, &json, &json_size);
    if (status != LT48_OK) {
        char *error_summary = g_strdup_printf(
            "Rejected: %s. No unverified report data is displayed.",
            lt48_status_string(status)
        );
        gtk_widget_remove_css_class(widgets->report_status, "warning");
        gtk_widget_add_css_class(widgets->report_status, "error");
        gtk_label_set_text(GTK_LABEL(widgets->report_status), error_summary);
        gtk_text_buffer_set_text(text_buffer, "", 0);
        g_free(error_summary);
        lt48_operation_report_close(report);
        return;
    }
    summary = g_string_new(NULL);
    g_string_append_printf(
        summary,
        "INTEGRITY VERIFIED — canonical schema %u\n"
        "Outcome: %s · Operation: %s · Device: %s\n"
        "Run: %s · Support: %s\n",
        info.schema_version, info.status, info.operation, info.device_key,
        info.run_id, info.support_level
    );
    if (info.has_address_range)
        g_string_append_printf(
            summary, "Range: 0x%08x + %u bytes (end 0x%08" G_GINT64_MODIFIER "x)\n",
            info.address_start, info.address_length,
            (guint64)info.address_start +
                (info.address_length == 0u ? 0u :
                 (guint64)info.address_length - 1u)
        );
    else
        g_string_append(summary, "Range: not encoded by this schema\n");
    g_string_append_printf(
        summary,
        "Plan SHA-256: %s\nInput image: %s\n",
        info.plan_sha256,
        info.input_image_sha256 == NULL ? "not supplied" :
                                          info.input_image_sha256
    );
    if (info.has_output)
        g_string_append_printf(
            summary, "Output: %zu bytes · SHA-256 %s\n", info.output_size,
            info.output_sha256 == NULL ? "not encoded" : info.output_sha256
        );
    else
        g_string_append(summary, "Output: none\n");
    g_string_append_printf(
        summary,
        "Verification: %s · Observed result: %s\n"
        "Resources: %zu · Phases: %zu · Session events: %zu\n"
        "Cleanup: %zu phases, %zu failed · Transport close: %s\n"
        "Cancellation: %s · Device-ID mismatch override: %s · "
        "Progress observer errors: %zu\n",
        info.verification_result,
        info.observed_result == NULL ? "not recorded" : info.observed_result,
        info.resource_count, info.phase_count, info.session_event_count,
        info.cleanup_phase_count, info.cleanup_failed_phase_count,
        info.transport_close_status, info.cancelled ? "yes" : "no",
        info.mismatch_override ? "yes" : "no",
        info.progress_observer_error_count
    );
    if (info.has_failure)
        g_string_append_printf(
            summary, "Failure: %s — %s\n", info.failure_type,
            info.failure_message
        );
    else
        g_string_append(summary, "Failure: none\n");
    g_string_append_printf(summary, "Report SHA-256: %s", info.report_sha256);
    gtk_widget_remove_css_class(widgets->report_status, "error");
    gtk_widget_remove_css_class(widgets->report_status, "warning");
    if (info.has_failure || info.cancelled ||
        info.cleanup_failed_phase_count != 0u || info.mismatch_override ||
        info.progress_observer_error_count != 0u)
        gtk_widget_add_css_class(widgets->report_status, "warning");
    gtk_label_set_text(GTK_LABEL(widgets->report_status), summary->str);
    gtk_text_buffer_set_text(text_buffer, (const char *)json,
                             (gint)MIN(json_size, (size_t)G_MAXINT));
    g_string_free(summary, TRUE);
    lt48_operation_report_close(report);
}

static GtkWidget *build_report_page(app_widgets *widgets) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *inspect = gtk_button_new_with_label("Inspect report");
    GtkWidget *notice = gtk_label_new(
        "Offline inspection. Reports are displayed only after strict supported-"
        "schema validation and SHA-256 verification. Device-image payload fields and "
        "unknown fields are rejected."
    );
    GtkWidget *scroll = gtk_scrolled_window_new();
    widgets->report_path_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->report_path_entry),
                                   "operation-report.json");
    gtk_widget_set_hexpand(widgets->report_path_entry, TRUE);
    gtk_box_append(GTK_BOX(row), gtk_label_new("Report:"));
    gtk_box_append(GTK_BOX(row), widgets->report_path_entry);
    gtk_box_append(GTK_BOX(row), inspect);
    gtk_box_append(GTK_BOX(box), row);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_widget_add_css_class(notice, "dim-label");
    gtk_box_append(GTK_BOX(box), notice);
    widgets->report_status = gtk_label_new("No report inspected");
    gtk_label_set_xalign(GTK_LABEL(widgets->report_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->report_status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->report_status), TRUE);
    gtk_box_append(GTK_BOX(box), widgets->report_status);
    widgets->report_preview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->report_preview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(widgets->report_preview),
                                     FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(widgets->report_preview), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),
                                  widgets->report_preview);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    g_signal_connect(inspect, "clicked", G_CALLBACK(report_inspect), widgets);
    g_signal_connect_swapped(
        widgets->report_path_entry, "activate",
        G_CALLBACK(gtk_widget_activate), inspect
    );
    return box;
}

static void diagnostics_task_free(gpointer data) {
    diagnostics_task *task = data;
    if (task == NULL) return;
    g_free(task->installation_path);
    g_free(task);
}

static void diagnostics_task_result_free(gpointer data) {
    diagnostics_task_result *result = data;
    if (result == NULL) return;
    g_free(result->installation_path);
    g_free(result);
}

static void diagnostics_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    diagnostics_task *request = task_data;
    diagnostics_task_result *result = g_new0(diagnostics_task_result, 1);
    (void)source_object;
    (void)cancellable;
    result->installation_path = g_strdup(request->installation_path);
    result->status = lt48_diagnostics_self_test(
        request->installation_path, &result->report
    );
    g_task_return_pointer(task, result, diagnostics_task_result_free);
}

static void diagnostics_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    diagnostics_task_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL
    );
    GString *message = g_string_new(NULL);
    char manifest[LT48_SHA256_SIZE * 2u + 1u];
    (void)source_object;
    gtk_widget_set_sensitive(widgets->diagnostics_run_button, TRUE);
    gtk_widget_remove_css_class(widgets->diagnostics_status, "error");
    gtk_widget_remove_css_class(widgets->diagnostics_status, "warning");
    if (result == NULL || result->status != LT48_OK) {
        gtk_widget_add_css_class(widgets->diagnostics_status, "error");
        g_string_append_printf(
            message, "Diagnostics could not run: %s",
            result == NULL ? "unknown task failure" :
                lt48_status_string(result->status)
        );
    } else {
        const lt48_diagnostics_report *report = &result->report;
        g_string_append_printf(
            message,
            "OFFLINE NATIVE SELF-TEST — %s\n"
            "SHA-256 known answer: %s\n"
            "Operation surface: %s\n"
            "Buffer model: %s\n"
            "JTAG planner: %s\n"
            "Progress model: %s\n",
            report->passed ? "PASS" : "FAIL",
            report->sha256_check_passed ? "pass" : "FAIL",
            report->operation_surface_check_passed ? "pass" : "FAIL",
            report->buffer_model_check_passed ? "pass" : "FAIL",
            report->jtag_planner_check_passed ? "pass" : "FAIL",
            report->progress_model_check_passed ? "pass" : "FAIL"
        );
        if (report->installation_requested) {
            g_string_append_printf(
                message,
                "Installation/cache: %s\n"
                "Audit: %s · %zu catalogue entries · %zu definition files · "
                "%zu resolved · %zu failed\n",
                result->installation_path,
                lt48_status_string(report->installation_status),
                report->installation_audit.catalogue_device_count,
                report->installation_audit.definition_file_count,
                report->installation_audit.resolved_device_count,
                report->installation_audit.failed_device_count
            );
            if (report->installation_status == LT48_OK) {
                digest_hex(report->installation_audit.manifest_sha256,
                           LT48_SHA256_SIZE, manifest);
                g_string_append_printf(message, "Manifest SHA-256: %s\n",
                                       manifest);
            }
        } else {
            g_string_append(message, "Installation/cache audit: not requested\n");
        }
        g_string_append(message, "USB opened: no");
        if (!report->passed)
            gtk_widget_add_css_class(widgets->diagnostics_status, "warning");
    }
    gtk_label_set_text(GTK_LABEL(widgets->diagnostics_status), message->str);
    g_string_free(message, TRUE);
    diagnostics_task_result_free(result);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)
    ));
}

static void diagnostics_run(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->diagnostics_installation_entry)
    );
    diagnostics_task *request = g_new0(diagnostics_task, 1);
    GTask *task;
    (void)button;
    if (path != NULL && path[0] != '\0') request->installation_path = g_strdup(path);
    gtk_widget_set_sensitive(widgets->diagnostics_run_button, FALSE);
    gtk_label_set_text(
        GTK_LABEL(widgets->diagnostics_status),
        request->installation_path == NULL ?
            "Running deterministic core self-test…" :
            "Running core self-test and complete offline installation audit…"
    );
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)
    ));
    task = tracked_task_new(widgets, NULL, diagnostics_finished);
    g_task_set_task_data(task, request, diagnostics_task_free);
    g_task_run_in_thread(task, diagnostics_worker);
    g_object_unref(task);
}

static void self_test_range_changed(
    GtkCheckButton *one_pin, gpointer user_data
) {
    gtk_widget_set_sensitive(
        GTK_WIDGET(user_data), gtk_check_button_get_active(one_pin));
}

static GtkWidget *build_diagnostics_page(app_widgets *widgets) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *hardware_frame = gtk_frame_new("Hardware test");
    GtkWidget *hardware = gtk_grid_new();
    GtkWidget *all_pins = gtk_check_button_new_with_label("All pins");
    GtkWidget *one_pin = gtk_check_button_new_with_label("One pin");
    GtkWidget *pin = gtk_spin_button_new_with_range(1.0, 48.0, 1.0);
    GtkWidget *parallel_port =
        gtk_check_button_new_with_label("Parallel interface");
    GtkWidget *led_display =
        gtk_check_button_new_with_label("LED display");
    GtkWidget *pin_driver =
        gtk_check_button_new_with_label("Pin driver");
    GtkWidget *dac_drop =
        gtk_check_button_new_with_label("DAC voltage drop");
    GtkWidget *single = gtk_check_button_new_with_label("Single pass");
    GtkWidget *loop = gtk_check_button_new_with_label("Loop");
    static const char *drivers[] = {
        "TTL", "DAC0", "DAC1", "DAC2", "DAC3", "GND", "VGND", NULL
    };
    GtkWidget *driver = gtk_drop_down_new_from_strings(drivers);
    GtkWidget *execute_hardware =
        gtk_button_new_with_label("Execute hardware test");
    GtkWidget *offline_frame =
        gtk_frame_new("Application/core self-test (offline)");
    GtkWidget *offline = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *help = gtk_button_new_with_label("Help");
    GtkWidget *done = gtk_button_new_with_label("Done");
    GtkWidget *hardware_notice = gtk_label_new(
        "Recovered test controls are shown for workflow fidelity. Live driver, "
        "voltage, LED and socket execution remains disabled until its hardware "
        "sequence and cleanup are validated."
    );
    GtkWidget *notice = gtk_label_new(
        "This currently runs deterministic application/core checks without "
        "enumerating or opening USB. An optional owned installation or private "
        "cache adds the complete offline catalogue/definition audit. Live pin, "
        "driver, voltage, LED and socket hardware tests are not implemented yet."
    );
    const char *default_path = g_getenv("LABTOOL48_INSTALLATION");
    widgets->diagnostics_installation_entry = gtk_entry_new();
    widgets->diagnostics_run_button =
        gtk_button_new_with_label("Run offline self-test");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->diagnostics_installation_entry),
        "optional owned installation or private cache"
    );
    if (default_path != NULL)
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->diagnostics_installation_entry), default_path
        );
    gtk_widget_set_hexpand(widgets->diagnostics_installation_entry, TRUE);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_grid_set_row_spacing(GTK_GRID(hardware), 8);
    gtk_grid_set_column_spacing(GTK_GRID(hardware), 12);
    gtk_check_button_set_group(GTK_CHECK_BUTTON(one_pin),
                               GTK_CHECK_BUTTON(all_pins));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(all_pins), TRUE);
    gtk_widget_set_sensitive(pin, FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(pin_driver), TRUE);
    gtk_check_button_set_group(GTK_CHECK_BUTTON(loop),
                               GTK_CHECK_BUTTON(single));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(single), TRUE);
    gtk_widget_set_sensitive(execute_hardware, FALSE);
    gtk_widget_set_tooltip_text(
        execute_hardware,
        "Unavailable until the recovered live self-test sequences and mandatory "
        "electrical cleanup are validated.");
    gtk_label_set_xalign(GTK_LABEL(hardware_notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(hardware_notice), TRUE);
    gtk_widget_add_css_class(hardware_notice, "warning");
    gtk_grid_attach(GTK_GRID(hardware), gtk_label_new("Range:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), all_pins, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), one_pin, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), pin, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), gtk_label_new("Items:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), parallel_port, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), led_display, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), pin_driver, 3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), dac_drop, 4, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), gtk_label_new("Mode:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), single, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), loop, 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), gtk_label_new("Driver:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(hardware), driver, 1, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(hardware), execute_hardware, 3, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(hardware), hardware_notice, 0, 4, 5, 1);
    gtk_frame_set_child(GTK_FRAME(hardware_frame), hardware);
    gtk_box_append(GTK_BOX(box), hardware_frame);
    gtk_box_append(GTK_BOX(row), gtk_label_new("Installation/cache:"));
    gtk_box_append(GTK_BOX(row), widgets->diagnostics_installation_entry);
    gtk_box_append(GTK_BOX(row), widgets->diagnostics_run_button);
    gtk_box_append(GTK_BOX(offline), row);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_widget_add_css_class(notice, "dim-label");
    gtk_box_append(GTK_BOX(offline), notice);
    widgets->diagnostics_status = gtk_label_new("No diagnostic run yet");
    gtk_label_set_xalign(GTK_LABEL(widgets->diagnostics_status), 0.0F);
    gtk_label_set_yalign(GTK_LABEL(widgets->diagnostics_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->diagnostics_status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->diagnostics_status), TRUE);
    gtk_widget_set_vexpand(widgets->diagnostics_status, TRUE);
    gtk_box_append(GTK_BOX(offline), widgets->diagnostics_status);
    gtk_frame_set_child(GTK_FRAME(offline_frame), offline);
    gtk_widget_set_vexpand(offline_frame, TRUE);
    gtk_box_append(GTK_BOX(box), offline_frame);
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(actions), help);
    gtk_box_append(GTK_BOX(actions), done);
    gtk_box_append(GTK_BOX(box), actions);
    g_signal_connect(widgets->diagnostics_run_button, "clicked",
                     G_CALLBACK(diagnostics_run), widgets);
    g_signal_connect_swapped(
        widgets->diagnostics_installation_entry, "activate",
        G_CALLBACK(gtk_widget_activate), widgets->diagnostics_run_button
    );
    g_signal_connect_swapped(done, "clicked", G_CALLBACK(gtk_window_close),
                             widgets->self_test_window);
    g_signal_connect(one_pin, "toggled",
                     G_CALLBACK(self_test_range_changed), pin);
    g_signal_connect_swapped(help, "clicked", G_CALLBACK(gtk_window_present),
                             widgets->help_window);
    return box;
}

static void trace_task_free(gpointer data) {
    trace_task *task = data;
    if (task == NULL) return;
    g_free(task->left_path);
    g_free(task->right_path);
    g_free(task);
}

static void trace_task_result_free(gpointer data) {
    trace_task_result *result = data;
    if (result == NULL) return;
    g_free(result->summary);
    g_free(result->preview);
    g_free(result);
}

static void trace_append_event(
    GString *text, const char *side, const lt48_trace_event *event
) {
    const size_t preview_limit = 512u;
    size_t hex_length = strlen(event->data_hex);
    g_string_append_printf(
        text,
        "%s%zu  %s  ep 0x%02x  request %zu  payload %zu  timeout %u ms  %s\n"
        "  data: %.*s%s\n",
        side, event->sequence,
        event->direction == LT48_TRACE_OUT ? "OUT" : "IN ",
        event->endpoint, event->requested_length, event->data_size,
        event->timeout_ms, event->status,
        (int)MIN(hex_length, preview_limit), event->data_hex,
        hex_length > preview_limit ? "…" : ""
    );
    if (event->error != NULL)
        g_string_append_printf(text, "  error: %s\n", event->error);
}

static void trace_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    trace_task *request = task_data;
    trace_task_result *result = g_new0(trace_task_result, 1);
    lt48_transfer_trace *left = NULL;
    lt48_transfer_trace *right = NULL;
    lt48_trace_info info;
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    GString *summary = g_string_new(NULL);
    GString *preview = g_string_new(NULL);
    (void)source_object;
    (void)cancellable;
    result->status = lt48_trace_open(request->left_path, &left);
    if (result->status == LT48_OK && request->compare)
        result->status = lt48_trace_open(request->right_path, &right);
    if (result->status == LT48_OK && !request->compare) {
        result->status = lt48_trace_get_info(left, &info);
        if (result->status == LT48_OK) {
            size_t shown = MIN(info.event_count, 256u);
            digest_hex(info.trace_sha256, LT48_SHA256_SIZE, digest);
            g_string_append_printf(
                summary,
                "VALID SCHEMA-%u TRACE — %zu events — digest %s\n"
                "SHA-256: %s\nMetadata: ",
                info.schema_version, info.event_count,
                info.had_claimed_sha256 ? "verified" : "computed", digest
            );
            g_string_append_len(summary, (const char *)info.metadata_json,
                                (gssize)info.metadata_json_size);
            for (size_t index = 0u; index < shown; ++index) {
                lt48_trace_event event;
                result->status = lt48_trace_get_event(left, index, &event);
                if (result->status != LT48_OK) break;
                trace_append_event(preview, "#", &event);
            }
            if (result->status == LT48_OK && shown < info.event_count)
                g_string_append_printf(
                    preview, "… %zu further events omitted from the GUI preview\n",
                    info.event_count - shown
                );
        }
    } else if (result->status == LT48_OK) {
        lt48_trace_comparison comparison;
        result->status = lt48_trace_compare(left, right, &comparison);
        if (result->status == LT48_OK && comparison.equal) {
            g_string_append(summary,
                            "TRACE COMPARISON — event-for-event equal");
            g_string_append(preview,
                            "Metadata is intentionally not part of event comparison.");
        } else if (result->status == LT48_OK) {
            result->comparison_difference = 1;
            g_string_append_printf(
                summary, "TRACE COMPARISON — first difference at event %zu",
                comparison.first_difference
            );
            if (comparison.has_left)
                trace_append_event(preview, "LEFT #", &comparison.left);
            else
                g_string_append(preview, "LEFT: end of trace\n");
            if (comparison.has_right)
                trace_append_event(preview, "RIGHT #", &comparison.right);
            else
                g_string_append(preview, "RIGHT: end of trace\n");
        }
    }
    if (result->status != LT48_OK)
        g_string_printf(summary, "Trace rejected: %s",
                        lt48_status_string(result->status));
    result->summary = g_string_free(summary, FALSE);
    result->preview = g_string_free(preview, FALSE);
    lt48_trace_close(right);
    lt48_trace_close(left);
    g_task_return_pointer(task, result, trace_task_result_free);
}

static void trace_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    trace_task_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL
    );
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->trace_preview)
    );
    (void)source_object;
    gtk_widget_set_sensitive(widgets->trace_inspect_button, TRUE);
    gtk_widget_set_sensitive(widgets->trace_compare_button, TRUE);
    gtk_widget_remove_css_class(widgets->trace_status, "error");
    gtk_widget_remove_css_class(widgets->trace_status, "warning");
    if (result == NULL || result->status != LT48_OK)
        gtk_widget_add_css_class(widgets->trace_status, "error");
    else if (result->comparison_difference)
        gtk_widget_add_css_class(widgets->trace_status, "warning");
    gtk_label_set_text(
        GTK_LABEL(widgets->trace_status),
        result == NULL ? "Trace task failed without a result" : result->summary
    );
    gtk_text_buffer_set_text(
        buffer, result == NULL ? "" : result->preview, -1
    );
    trace_task_result_free(result);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)
    ));
}

static void trace_run(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *left = gtk_editable_get_text(
        GTK_EDITABLE(widgets->trace_left_entry)
    );
    const char *right = gtk_editable_get_text(
        GTK_EDITABLE(widgets->trace_right_entry)
    );
    int compare = GTK_WIDGET(button) == widgets->trace_compare_button;
    trace_task *request;
    GTask *task;
    if (left == NULL || left[0] == '\0' ||
        (compare && (right == NULL || right[0] == '\0'))) {
        gtk_label_set_text(
            GTK_LABEL(widgets->trace_status),
            compare ? "Select both trace paths" : "Select a trace path"
        );
        return;
    }
    request = g_new0(trace_task, 1);
    request->left_path = g_strdup(left);
    request->right_path = compare ? g_strdup(right) : NULL;
    request->compare = compare;
    gtk_widget_set_sensitive(widgets->trace_inspect_button, FALSE);
    gtk_widget_set_sensitive(widgets->trace_compare_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->trace_status),
                       compare ? "Comparing validated traces…" :
                                 "Validating and inspecting trace…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)
    ));
    task = tracked_task_new(widgets, NULL, trace_finished);
    g_task_set_task_data(task, request, trace_task_free);
    g_task_run_in_thread(task, trace_worker);
    g_object_unref(task);
}

static GtkWidget *build_trace_page(app_widgets *widgets) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *left_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *right_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *notice = gtk_label_new(
        "Strict, offline schema validation and digest verification. Event "
        "comparison ignores metadata and reports the first differing transfer."
    );
    GtkWidget *scroll = gtk_scrolled_window_new();
    widgets->trace_left_entry = gtk_entry_new();
    widgets->trace_right_entry = gtk_entry_new();
    widgets->trace_inspect_button = gtk_button_new_with_label("Inspect trace");
    widgets->trace_compare_button = gtk_button_new_with_label("Compare traces");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->trace_left_entry),
                                   "left or inspected trace.json");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->trace_right_entry),
                                   "right trace.json");
    gtk_widget_set_hexpand(widgets->trace_left_entry, TRUE);
    gtk_widget_set_hexpand(widgets->trace_right_entry, TRUE);
    gtk_box_append(GTK_BOX(left_row), gtk_label_new("Trace:"));
    gtk_box_append(GTK_BOX(left_row), widgets->trace_left_entry);
    gtk_box_append(GTK_BOX(left_row), widgets->trace_inspect_button);
    gtk_box_append(GTK_BOX(right_row), gtk_label_new("Compare with:"));
    gtk_box_append(GTK_BOX(right_row), widgets->trace_right_entry);
    gtk_box_append(GTK_BOX(right_row), widgets->trace_compare_button);
    gtk_box_append(GTK_BOX(box), left_row);
    gtk_box_append(GTK_BOX(box), right_row);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_widget_add_css_class(notice, "dim-label");
    gtk_box_append(GTK_BOX(box), notice);
    widgets->trace_status = gtk_label_new("No trace inspected");
    gtk_label_set_xalign(GTK_LABEL(widgets->trace_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->trace_status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->trace_status), TRUE);
    gtk_box_append(GTK_BOX(box), widgets->trace_status);
    widgets->trace_preview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->trace_preview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(widgets->trace_preview), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(widgets->trace_preview), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),
                                  widgets->trace_preview);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    g_signal_connect(widgets->trace_inspect_button, "clicked",
                     G_CALLBACK(trace_run), widgets);
    g_signal_connect(widgets->trace_compare_button, "clicked",
                     G_CALLBACK(trace_run), widgets);
    g_signal_connect_swapped(widgets->trace_left_entry, "activate",
        G_CALLBACK(gtk_widget_activate), widgets->trace_inspect_button);
    g_signal_connect_swapped(widgets->trace_right_entry, "activate",
        G_CALLBACK(gtk_widget_activate), widgets->trace_compare_button);
    return box;
}

static void component_start_long(
    app_widgets *widgets, component_long_task_kind kind
);

static void component_validate(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_VALIDATE_STRUCTURE);
}

static void component_check_semantics(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_CHECK_SEMANTICS);
}

static void component_long_task_free(gpointer data) {
    component_long_task *request = data;
    if (request == NULL) return;
    g_free(request->bundle_path);
    g_free(request->store_path);
    g_free(request->evidence_path);
    g_free(request->artifact_path);
    g_free(request->operation_id);
    g_free(request->object_id);
    g_free(request->base_object_id);
    g_free(request);
}

static void component_long_result_free(gpointer data) {
    component_long_task_result *result = data;
    if (result == NULL) return;
    if (result->kind == COMPONENT_LONG_COMPILE_MATRIX)
        lt48_component_plan_matrix_clear(&result->matrix);
    else if (result->kind == COMPONENT_LONG_AUDIT_EVIDENCE)
        lt48_component_evidence_audit_clear(&result->audit);
    else if (result->kind == COMPONENT_LONG_RELEASE_CHECK)
        lt48_component_release_check_clear(&result->release_check);
    else if (result->kind == COMPONENT_LONG_COMPILE_PLAN)
        lt48_component_plan_clear(&result->plan);
    else if (result->kind == COMPONENT_LONG_RESOLVE ||
             result->kind == COMPONENT_LONG_DIFF)
        lt48_component_resolution_clear(&result->resolution);
    else if (result->kind == COMPONENT_LONG_ADAPTER_TEST)
        lt48_component_adapter_test_plan_clear(&result->adapter_plan);
    else if (result->kind == COMPONENT_LONG_PAYLOAD_PLAN)
        lt48_component_payload_build_plan_clear(&result->payload_plan);
    g_free(result);
}

static void component_long_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    component_long_task *request = task_data;
    component_long_task_result *result = g_new0(
        component_long_task_result, 1);
    (void)source_object;
    result->kind = request->kind;
    if (request->kind == COMPONENT_LONG_VALIDATE_ALL) {
        result->status =
            lt48_component_bundle_validate_all_variants_cancellable(
                request->bundle_path, gio_cancelled, cancellable,
                &result->validation);
    } else if (request->kind == COMPONENT_LONG_COMPILE_MATRIX) {
        result->status =
            lt48_component_bundle_compile_plan_matrix_cancellable(
                request->bundle_path, gio_cancelled, cancellable,
                &result->matrix);
    } else if (request->kind == COMPONENT_LONG_AUDIT_EVIDENCE) {
        result->status = lt48_component_evidence_audit_store_cancellable(
            request->bundle_path, request->store_path, gio_cancelled,
            cancellable, &result->audit);
    } else if (request->kind == COMPONENT_LONG_RELEASE_CHECK) {
        result->status = lt48_component_release_check_create_cancellable(
            request->bundle_path, request->store_path, gio_cancelled,
            cancellable, &result->release_check);
    } else if (request->kind == COMPONENT_LONG_VERIFY_EVIDENCE) {
        result->status = lt48_component_evidence_verify_cancellable(
            request->bundle_path, request->evidence_path,
            request->artifact_path, gio_cancelled, cancellable,
            &result->verification);
    } else if (request->kind == COMPONENT_LONG_IMPORT_EVIDENCE) {
        result->status = lt48_component_evidence_import_cancellable(
            request->bundle_path, request->evidence_path,
            request->artifact_path, request->store_path, gio_cancelled,
            cancellable, &result->import_report);
    } else if (request->kind == COMPONENT_LONG_VALIDATE_STRUCTURE) {
        if (g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
        else
            result->status = lt48_component_bundle_validate(
                request->bundle_path, &result->structure);
        if (result->status != LT48_CANCELLED &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    } else if (request->kind == COMPONENT_LONG_CHECK_SEMANTICS) {
        if (g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
        else
            result->status = lt48_component_bundle_check_semantics(
                request->bundle_path, &result->semantics);
        if (result->status != LT48_CANCELLED &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    } else if (request->kind == COMPONENT_LONG_COMPILE_PLAN) {
        if (g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
        else
            result->status = lt48_component_bundle_compile_plan(
                request->bundle_path, request->combination_index,
                request->operation_id, &result->plan);
        if (result->status != LT48_CANCELLED &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    } else if (request->kind == COMPONENT_LONG_RESOLVE ||
               request->kind == COMPONENT_LONG_DIFF) {
        if (g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
        else if (request->kind == COMPONENT_LONG_RESOLVE)
            result->status = lt48_component_bundle_resolve(
                request->bundle_path, request->object_id,
                &result->resolution);
        else
            result->status = lt48_component_bundle_diff(
                request->bundle_path, request->base_object_id,
                request->object_id, &result->resolution);
        if (result->status != LT48_CANCELLED &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    } else if (request->kind == COMPONENT_LONG_ADAPTER_TEST) {
        if (g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
        else
            result->status = lt48_component_bundle_adapter_test_plan(
                request->bundle_path, request->object_id,
                &result->adapter_plan);
        if (result->status != LT48_CANCELLED &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    } else if (request->kind == COMPONENT_LONG_PAYLOAD_PLAN) {
        if (g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
        else
            result->status = lt48_component_bundle_plan_payload_builds(
                request->bundle_path, &result->payload_plan);
        if (result->status != LT48_CANCELLED &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    } else {
        if (g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
        else
            result->status = lt48_component_evidence_validate(
                request->evidence_path, &result->evidence);
        if (result->status != LT48_CANCELLED &&
            g_cancellable_is_cancelled(cancellable))
            result->status = LT48_CANCELLED;
    }
    g_task_return_pointer(task, result, component_long_result_free);
}

static void component_task_controls_sensitive(
    app_widgets *widgets, gboolean sensitive
) {
    gtk_widget_set_sensitive(widgets->component_validate_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_semantic_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_validate_all_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_resolve_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_diff_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_adapter_test_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_compile_plan_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_compile_matrix_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_payload_plan_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_payload_verify_button, sensitive);
    gtk_widget_set_sensitive(
        widgets->component_evidence_validate_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_evidence_verify_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_evidence_import_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_evidence_audit_button, sensitive);
    gtk_widget_set_sensitive(widgets->component_release_check_button, sensitive);
}

static void component_long_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    component_long_task_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    component_long_task *request = g_task_get_task_data(G_TASK(async_result));
    GtkTextBuffer *preview = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->component_preview));
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    (void)source_object;
    component_task_controls_sensitive(widgets, TRUE);
    gtk_widget_set_sensitive(widgets->component_validate_all_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_validate_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_semantic_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_compile_plan_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_validate_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_resolve_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_diff_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_adapter_test_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_payload_plan_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_compile_matrix_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_payload_verify_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_verify_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_import_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_audit_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_release_check_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_cancel_button, FALSE);
    g_clear_object(&widgets->component_cancellable);
    if (result == NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
                           "Component task failed without a result");
        gtk_text_buffer_set_text(preview, "", 0);
    } else if (result->status == LT48_CANCELLED) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
            "Component task cancelled at a bounded hash, record, or plan "
            "boundary; no partial result was accepted and no USB was opened.");
        gtk_text_buffer_set_text(preview, "", 0);
    } else if (result->kind == COMPONENT_LONG_VALIDATE_STRUCTURE) {
        lt48_component_validation_report *report = &result->structure;
        const char *issue = lt48_component_validation_issue_name(report->issue);
        if (issue == NULL) issue = "unknown";
        digest_hex(report->source_sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "%s — %s\n%s\nSchema %u · %zu documents · %zu objects · "
            "%zu combinations · %zu inheritance edges · %zu references\n"
            "Bundle source SHA-256: %s\nOffline structural validation only; "
            "this does not enable any operation or bypass the safety validator.",
            result->status == LT48_OK ? "Structurally valid" : "Rejected",
            issue, report->diagnostic, report->schema_version,
            report->document_count, report->object_count,
            report->combination_count, report->inherited_object_count,
            report->reference_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, "", 0);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_CHECK_SEMANTICS) {
        lt48_component_semantic_report *report = &result->semantics;
        digest_hex(report->source_sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "%s\nObjects %zu · validated semantic schemas %zu · explicit "
            "blockers %zu · invalid %zu\nCombinations %zu · validated %zu · "
            "invalid %zu\nFirst problem: %s\n%s\nSource SHA-256: %s\n"
            "Support registry unchanged; no USB opened.",
            result->status != LT48_OK ? "Semantic validation rejected" :
            report->complete ? "Implemented semantic schemas complete" :
                               "Semantic validation incomplete",
            report->object_count, report->validated_object_count,
            report->blocker_count, report->invalid_object_count,
            report->combination_count, report->validated_combination_count,
            report->invalid_combination_count,
            report->first_problem_object_id[0] == '\0' ? "none" :
                report->first_problem_object_id,
            report->diagnostic, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, "", 0);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_COMPILE_PLAN &&
               result->status == LT48_OK) {
        lt48_component_plan *plan = &result->plan;
        digest_hex(plan->sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Compiled combination %zu / %s offline · %zu phases · %zu "
            "mapped pin states · %zu rail states · typed IR entry %zu / "
            "cleanup %zu\nPlan SHA-256: %s\nTransport ready: no. Authored-plan "
            "evidence only; support registry unchanged and no USB opened.",
            plan->combination_index, plan->operation_id, plan->phase_count,
            plan->mapped_pin_state_count, plan->rail_state_count,
            plan->entry_phase_index, plan->cleanup_phase_index, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, (const char *)plan->json,
                                 (gint)plan->json_size);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_VALIDATE_EVIDENCE) {
        lt48_component_evidence_report *report = &result->evidence;
        digest_hex(report->source_sha256, LT48_SHA256_SIZE, digest);
        if (result->status != LT48_OK) {
            message = g_strdup_printf("Evidence record rejected: %s · %s",
                lt48_status_string(result->status), report->diagnostic);
            gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
            gtk_text_buffer_set_text(preview, "", 0);
        } else {
            message = g_strdup_printf(
                "Evidence %s validated offline · %s / %s · result %s\n"
                "Structural evidence only; support registry unchanged and "
                "no USB opened.", report->evidence_id,
                report->evidence_type, report->method, report->status);
            gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
            char *details = g_strdup_printf(
                "Evidence source SHA-256: %s\nArtifact SHA-256: %s\n"
                "Bundle SHA-256: %s\nMatrix SHA-256: %s\nPlan SHA-256: %s\n"
                "Combination: %zu\nOperation: %s\nHardware evidence: %s\n"
                "Performed: %s\n", digest, report->artifact_sha256,
                report->bundle_sha256, report->matrix_sha256,
                report->plan_sha256, report->combination_index,
                report->operation_id,
                report->hardware_evidence ? "yes" : "no",
                report->performed_date);
            gtk_text_buffer_set_text(preview, details, -1);
            g_free(details);
        }
        g_free(message);
    } else if ((result->kind == COMPONENT_LONG_RESOLVE ||
                result->kind == COMPONENT_LONG_DIFF) &&
               result->status == LT48_OK) {
        lt48_component_resolution *resolution = &result->resolution;
        char effective_sha[LT48_SHA256_SIZE * 2u + 1u];
        char delta_sha[LT48_SHA256_SIZE * 2u + 1u];
        digest_hex(resolution->effective_sha256, LT48_SHA256_SIZE,
                   effective_sha);
        digest_hex(resolution->delta_sha256, LT48_SHA256_SIZE, delta_sha);
        if (result->kind == COMPONENT_LONG_RESOLVE) {
            GString *text = g_string_new("Effective definition\n");
            message = g_strdup_printf(
                "Resolved %s offline · inheritance depth %zu · %zu effective "
                "properties · %zu changed properties\nEffective SHA-256: %s\n"
                "Delta SHA-256: %s\nResolution is authoring evidence only "
                "and does not enable execution.", request->object_id,
                resolution->inheritance_depth,
                resolution->effective_property_count,
                resolution->changed_property_count, effective_sha, delta_sha);
            g_string_append_len(text,
                (const char *)resolution->effective_json,
                resolution->effective_json_size);
            g_string_append(text, "\n\nBase-to-object delta\n");
            g_string_append_len(text, (const char *)resolution->delta_json,
                                resolution->delta_json_size);
            gtk_text_buffer_set_text(preview, text->str, -1);
            g_string_free(text, TRUE);
        } else {
            message = g_strdup_printf(
                "Strict base-to-variant delta · %s → %s\n"
                "%zu changed properties · inheritance depth %zu\n"
                "Delta SHA-256: %s\nOffline authoring evidence only; "
                "support registry unchanged and no USB opened.",
                request->base_object_id, request->object_id,
                resolution->changed_property_count,
                resolution->inheritance_depth, delta_sha);
            gtk_text_buffer_set_text(preview,
                (const char *)resolution->delta_json,
                (gint)resolution->delta_json_size);
        }
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_ADAPTER_TEST &&
               result->status == LT48_OK) {
        lt48_component_adapter_test_plan *plan = &result->adapter_plan;
        digest_hex(plan->sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Adapter %s · %zu continuity checks · %zu isolation checks\n"
            "Automatic passive continuity qualification: %s · active "
            "components: %s\nPlan SHA-256: %s\nProcedure requires an "
            "unpowered programmer and device; no USB opened.",
            request->object_id, plan->continuity_test_count,
            plan->isolation_test_count,
            plan->automatic_continuity_ready ? "ready" : "blocked",
            plan->active_components ? "yes" : "no", digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, (const char *)plan->json,
                                 (gint)plan->json_size);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_PAYLOAD_PLAN &&
               result->status == LT48_OK) {
        lt48_component_payload_build_plan *plan = &result->payload_plan;
        digest_hex(plan->sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Canonical payload build plan · %zu payloads · %zu authored · "
            "%zu imported\nPlan SHA-256: %s\nLocal source/toolchain/output "
            "files are not yet verified; no process executed and no USB opened.",
            plan->payload_count, plan->authored_payload_count,
            plan->imported_payload_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, (const char *)plan->json,
                                 (gint)plan->json_size);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_VALIDATE_ALL) {
        lt48_component_all_variants_report *report = &result->validation;
        if (report->matrix_complete)
            digest_hex(report->matrix_sha256, LT48_SHA256_SIZE, digest);
        else
            g_strlcpy(digest, "not generated", sizeof(digest));
        message = g_strdup_printf(
            "%s\nStructure: %s · semantics/composition: %s · matrix: %s\n"
            "Objects %zu · combinations %zu/%zu · plans %zu (%zu mutating)\n"
            "Matrix SHA-256: %s\nOffline validation only; support registry "
            "unchanged and no USB opened.",
            result->status == LT48_OK ? "All declared variants validated" :
                                        "All-variant validation rejected",
            report->structure_valid ? "valid" : "failed",
            report->semantics_complete ? "complete" : "failed/incomplete",
            report->matrix_complete ? "complete" : "not generated",
            report->structure.object_count,
            report->semantics.validated_combination_count,
            report->semantics.combination_count,
            report->plan_count, report->mutating_plan_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, "", 0);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_AUDIT_EVIDENCE &&
               result->audit.json != NULL) {
        lt48_component_evidence_audit *audit = &result->audit;
        digest_hex(audit->sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Evidence audit · %zu/%zu records verified · %zu invalid · %zu plans\n"
            "Safety %zu · behavior %zu · engine HW %zu · device HW %zu · "
            "release ready: no\nAudit SHA-256: %s%s",
            audit->verified_record_count, audit->stored_record_count,
            audit->invalid_record_count, audit->plan_count,
            audit->safety_validated_plan_count,
            audit->behavior_validated_plan_count,
            audit->engine_hardware_plan_count,
            audit->device_hardware_plan_count, digest,
            result->status == LT48_OK ? "" : " · store integrity rejected");
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, (const char *)audit->json,
                                 (gint)audit->json_size);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_RELEASE_CHECK &&
               result->release_check.json != NULL) {
        lt48_component_release_check *check = &result->release_check;
        digest_hex(check->sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Release ready: %s · evidence candidates %zu/%zu · transport "
            "ready %zu/%zu · %zu blockers\nCheck SHA-256: %s\nSupport "
            "registry unchanged and no USB opened.",
            check->release_ready ? "yes" : "no",
            check->evidence_candidate_count, check->plan_count,
            check->transport_ready_plan_count, check->plan_count,
            check->blocker_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, (const char *)check->json,
                                 (gint)check->json_size);
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_VERIFY_EVIDENCE) {
        lt48_component_evidence_verification *verification =
            &result->verification;
        digest_hex(verification->artifact_sha256, LT48_SHA256_SIZE, digest);
        message = result->status == LT48_OK ?
            g_strdup_printf(
                "Evidence bytes and subject bindings verified · %zu artifact "
                "bytes · recorded result %s\nSupport registry unchanged and "
                "no USB opened.", verification->artifact_size,
                verification->evidence.passed ? "pass" : "fail") :
            g_strdup_printf("Evidence verification rejected: %s · %s",
                lt48_status_string(result->status),
                verification->diagnostic);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        {
            char *details = g_strdup_printf(
                "Actual artifact SHA-256: %s\nArtifact: %s\nBundle: %s\n"
                "Matrix: %s\nPlan: %s\nComplete: %s\n%s\n",
                digest,
                verification->artifact_verified ? "verified" : "not verified",
                verification->bundle_verified ? "verified" : "not verified",
                verification->matrix_verified ? "verified" : "not verified",
                verification->plan_verified ? "verified" : "not verified",
                verification->complete ? "yes" : "no",
                verification->diagnostic);
            gtk_text_buffer_set_text(preview, details, -1);
            g_free(details);
        }
        g_free(message);
    } else if (result->kind == COMPONENT_LONG_IMPORT_EVIDENCE &&
               result->status == LT48_OK) {
        lt48_component_evidence_import_report *report =
            &result->import_report;
        char artifact_digest[LT48_SHA256_SIZE * 2u + 1u];
        digest_hex(report->record_sha256, LT48_SHA256_SIZE, digest);
        digest_hex(report->artifact_sha256, LT48_SHA256_SIZE,
                   artifact_digest);
        message = g_strdup_printf(
            "Evidence %s imported offline · record %s · artifact %s\n"
            "Recorded result %s; support registry unchanged and no USB opened.",
            report->evidence_id,
            report->record_created ? "created" : "already present",
            report->artifact_created ? "created" : "already present",
            report->evidence_passed ? "pass" : "fail");
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        {
            char *details = g_strdup_printf(
                "Store: %s\nRecord SHA-256: %s\nRecord bytes: %zu\n"
                "Artifact SHA-256: %s\nArtifact bytes: %zu\n",
                gtk_editable_get_text(GTK_EDITABLE(
                    widgets->component_evidence_store_entry)),
                digest, report->record_size, artifact_digest,
                report->artifact_size);
            gtk_text_buffer_set_text(preview, details, -1);
            g_free(details);
        }
        g_free(message);
    } else if (result->status != LT48_OK) {
        const char *action =
            result->kind == COMPONENT_LONG_AUDIT_EVIDENCE ?
                "Evidence audit" :
            result->kind == COMPONENT_LONG_RELEASE_CHECK ?
                "Release check" :
            result->kind == COMPONENT_LONG_IMPORT_EVIDENCE ?
                "Evidence import" :
            result->kind == COMPONENT_LONG_COMPILE_PLAN ?
                "Authored plan compilation" :
            result->kind == COMPONENT_LONG_RESOLVE ?
                "Object resolution" :
            result->kind == COMPONENT_LONG_DIFF ?
                "Strict variant delta" :
            result->kind == COMPONENT_LONG_ADAPTER_TEST ?
                "Adapter test plan" :
            result->kind == COMPONENT_LONG_PAYLOAD_PLAN ?
                "Payload build plan" : "Full plan matrix";
        message = g_strdup_printf("%s rejected: %s", action,
                                  lt48_status_string(result->status));
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, "", 0);
        g_free(message);
    } else {
        lt48_component_plan_matrix *matrix = &result->matrix;
        digest_hex(matrix->sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Compiled full offline matrix · %zu combinations · %zu plans · "
            "%zu mutating plans\nMatrix SHA-256: %s\nTransport ready: no. "
            "Every declared combination was compiled; support registry "
            "unchanged and no USB opened.",
            matrix->combination_count, matrix->plan_count,
            matrix->mutating_plan_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, (const char *)matrix->json,
                                 (gint)matrix->json_size);
        g_free(message);
    }
    component_long_result_free(result);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
}

static void component_start_long(
    app_widgets *widgets, component_long_task_kind kind
) {
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_path_entry));
    const char *store = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_evidence_store_entry));
    const char *evidence = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_evidence_entry));
    const char *artifact = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_evidence_artifact_entry));
    const char *operation = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_operation_entry));
    const char *object_id = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_object_entry));
    const char *base_object_id = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_base_object_entry));
    uint32_t combination_index = 0u;
    component_long_task *request;
    GTask *task;
    GtkTextBuffer *preview = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->component_preview));
    if (kind != COMPONENT_LONG_VALIDATE_EVIDENCE &&
        (path == NULL || path[0] == '\0')) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
                           "Enter a component bundle path");
        gtk_text_buffer_set_text(preview, "", 0);
        return;
    }
    if (kind == COMPONENT_LONG_COMPILE_PLAN &&
        (operation == NULL || operation[0] == '\0' ||
         !entry_u32(widgets->component_combination_entry, UINT32_MAX,
                    &combination_index))) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
            "Enter bundle path, combination index, and operation name");
        gtk_text_buffer_set_text(preview, "", 0);
        return;
    }
    if ((kind == COMPONENT_LONG_RESOLVE ||
         kind == COMPONENT_LONG_ADAPTER_TEST ||
         kind == COMPONENT_LONG_DIFF) &&
        (object_id == NULL || object_id[0] == '\0')) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
            kind == COMPONENT_LONG_ADAPTER_TEST ?
                "Enter a bundle path and adapter object ID" :
                "Enter a bundle path and component object ID");
        gtk_text_buffer_set_text(preview, "", 0);
        return;
    }
    if (kind == COMPONENT_LONG_DIFF &&
        (base_object_id == NULL || base_object_id[0] == '\0')) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
            "Enter a bundle path, base variant ID, and child variant ID");
        gtk_text_buffer_set_text(preview, "", 0);
        return;
    }
    if ((kind == COMPONENT_LONG_AUDIT_EVIDENCE ||
         kind == COMPONENT_LONG_RELEASE_CHECK ||
         kind == COMPONENT_LONG_IMPORT_EVIDENCE) &&
        (store == NULL || store[0] == '\0')) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
                           "Enter bundle and evidence-store paths");
        gtk_text_buffer_set_text(preview, "", 0);
        return;
    }
    if ((kind == COMPONENT_LONG_VALIDATE_EVIDENCE ||
         kind == COMPONENT_LONG_VERIFY_EVIDENCE ||
         kind == COMPONENT_LONG_IMPORT_EVIDENCE) &&
        (evidence == NULL || evidence[0] == '\0' ||
         (kind != COMPONENT_LONG_VALIDATE_EVIDENCE &&
          (artifact == NULL || artifact[0] == '\0')))) {
        gtk_label_set_text(
            GTK_LABEL(widgets->component_status),
            kind == COMPONENT_LONG_VALIDATE_EVIDENCE ?
                "Enter an immutable evidence-record path" :
                "Enter bundle, evidence record, and detailed artifact paths");
        gtk_text_buffer_set_text(preview, "", 0);
        return;
    }
    if (widgets->component_cancellable != NULL) return;
    request = g_new0(component_long_task, 1);
    request->bundle_path = g_strdup(path);
    if (store != NULL) request->store_path = g_strdup(store);
    if (evidence != NULL) request->evidence_path = g_strdup(evidence);
    if (artifact != NULL) request->artifact_path = g_strdup(artifact);
    if (operation != NULL) request->operation_id = g_strdup(operation);
    if (object_id != NULL) request->object_id = g_strdup(object_id);
    if (base_object_id != NULL)
        request->base_object_id = g_strdup(base_object_id);
    request->combination_index = combination_index;
    request->kind = kind;
    widgets->component_cancellable = g_cancellable_new();
    component_task_controls_sensitive(widgets, FALSE);
    gtk_widget_set_sensitive(widgets->component_validate_all_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_validate_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_semantic_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_compile_plan_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_validate_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_resolve_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_diff_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_adapter_test_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_payload_plan_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_validate_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_semantic_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_compile_plan_button, FALSE);
    gtk_widget_set_sensitive(
        widgets->component_evidence_validate_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_resolve_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_diff_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_adapter_test_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_payload_plan_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_compile_matrix_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_payload_verify_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_verify_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_import_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_audit_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_release_check_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_cancel_button, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->component_status),
        kind == COMPONENT_LONG_VALIDATE_ALL ?
            "Validating all variants outside the UI thread…" :
        kind == COMPONENT_LONG_VALIDATE_STRUCTURE ?
            "Validating bundle structure outside the UI thread…" :
        kind == COMPONENT_LONG_CHECK_SEMANTICS ?
            "Checking semantic schemas outside the UI thread…" :
        kind == COMPONENT_LONG_COMPILE_PLAN ?
            "Compiling the selected authored plan outside the UI thread…" :
        kind == COMPONENT_LONG_VALIDATE_EVIDENCE ?
            "Validating the evidence record outside the UI thread…" :
        kind == COMPONENT_LONG_RESOLVE ?
            "Resolving the selected object outside the UI thread…" :
        kind == COMPONENT_LONG_DIFF ?
            "Computing the strict variant delta outside the UI thread…" :
        kind == COMPONENT_LONG_ADAPTER_TEST ?
            "Generating the adapter test plan outside the UI thread…" :
        kind == COMPONENT_LONG_PAYLOAD_PLAN ?
            "Planning payload builds outside the UI thread…" :
        kind == COMPONENT_LONG_COMPILE_MATRIX ?
            "Compiling the full plan matrix outside the UI thread…" :
        kind == COMPONENT_LONG_AUDIT_EVIDENCE ?
            "Reverifying the evidence store outside the UI thread…" :
        kind == COMPONENT_LONG_VERIFY_EVIDENCE ?
            "Verifying evidence bindings outside the UI thread…" :
        kind == COMPONENT_LONG_IMPORT_EVIDENCE ?
            "Verifying and importing evidence outside the UI thread…" :
            "Building the release-check proposal outside the UI thread…");
    gtk_text_buffer_set_text(preview, "", 0);
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->component_cancellable, component_long_finished);
    g_task_set_task_data(task, request, component_long_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, component_long_worker);
    g_object_unref(task);
}

static void component_validate_all_variants(
    GtkButton *button, gpointer user_data
) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_VALIDATE_ALL);
}

static void component_cancel_long(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    if (widgets->component_cancellable == NULL) return;
    g_cancellable_cancel(widgets->component_cancellable);
    gtk_widget_set_sensitive(widgets->component_cancel_button, FALSE);
    gtk_label_set_text(GTK_LABEL(widgets->component_status),
        "Cancellation requested; waiting for the current bounded step or "
        "process-group termination before discarding partial output…");
}

static void component_resolve(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_RESOLVE);
}

static void component_diff(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_DIFF);
}

static void component_adapter_test_plan(
    GtkButton *button, gpointer user_data
) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_ADAPTER_TEST);
}

static void component_plan_payload_builds(
    GtkButton *button, gpointer user_data
) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_PAYLOAD_PLAN);
}

static void payload_verify_task_free(gpointer data) {
    payload_verify_task *request = data;
    if (request == NULL) return;
    g_free(request->bundle_path);
    g_free(request->workspace_root);
    g_free(request);
}

static void payload_verify_result_free(gpointer data) {
    payload_verify_task_result *result = data;
    if (result == NULL) return;
    lt48_component_payload_input_verification_clear(&result->verification);
    g_free(result);
}

static void payload_verify_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    payload_verify_task *request = task_data;
    payload_verify_task_result *result = g_new0(
        payload_verify_task_result, 1);
    (void)source_object;
    result->status = lt48_component_bundle_verify_payload_inputs_cancellable(
        request->bundle_path, request->workspace_root, gio_cancelled,
        cancellable, &result->verification);
    g_task_return_pointer(task, result, payload_verify_result_free);
}

static void payload_verify_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    payload_verify_task_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    GtkTextBuffer *preview = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->component_preview));
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    (void)source_object;
    component_task_controls_sensitive(widgets, TRUE);
    gtk_widget_set_sensitive(widgets->component_validate_all_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_compile_matrix_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_payload_verify_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_verify_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_import_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_audit_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_release_check_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_cancel_button, FALSE);
    g_clear_object(&widgets->component_cancellable);
    if (result == NULL || result->status == LT48_CANCELLED) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
            result == NULL ? "Payload verification failed without a result" :
            "Payload verification cancelled during bounded streaming hash; "
            "no confirmation was issued and no USB was opened.");
        gtk_text_buffer_set_text(preview, "", 0);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->component_payload_confirmation_entry), "");
        gtk_widget_set_sensitive(widgets->component_payload_execute_button,
                                 FALSE);
    } else if (result->verification.json == NULL) {
        message = g_strdup_printf("Payload input verification failed: %s",
                                  lt48_status_string(result->status));
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, "", 0);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->component_payload_confirmation_entry), "");
        gtk_widget_set_sensitive(widgets->component_payload_execute_button,
                                 FALSE);
        g_free(message);
    } else {
        lt48_component_payload_input_verification *verification =
            &result->verification;
        digest_hex(verification->sha256, LT48_SHA256_SIZE, digest);
        if (verification->complete) {
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->component_payload_confirmation_entry),
                digest);
            gtk_widget_set_sensitive(
                widgets->component_payload_execute_button, TRUE);
        } else {
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->component_payload_confirmation_entry),
                "");
            gtk_widget_set_sensitive(
                widgets->component_payload_execute_button, FALSE);
        }
        message = g_strdup_printf(
            "Payload inputs %s · %zu/%zu verified · %zu rejected\n"
            "Verification SHA-256: %s\nNo build process executed and no USB "
            "opened.",
            verification->complete ? "verified" : "rejected",
            verification->verified_input_count,
            verification->required_input_count,
            verification->rejected_input_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview, (const char *)verification->json,
                                 (gint)verification->json_size);
        g_free(message);
    }
    payload_verify_result_free(result);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
}

static void component_verify_payload_inputs(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_path_entry));
    const char *root = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_payload_root_entry));
    payload_verify_task *request;
    GTask *task;
    (void)button;
    if (path == NULL || path[0] == '\0' || root == NULL || root[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
            "Enter both component bundle and payload workspace paths");
        return;
    }
    if (widgets->component_cancellable != NULL) return;
    request = g_new0(payload_verify_task, 1);
    request->bundle_path = g_strdup(path);
    request->workspace_root = g_strdup(root);
    widgets->component_cancellable = g_cancellable_new();
    component_task_controls_sensitive(widgets, FALSE);
    gtk_widget_set_sensitive(widgets->component_validate_all_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_compile_matrix_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_payload_verify_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_verify_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_import_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_audit_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_release_check_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_cancel_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_payload_execute_button, FALSE);
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->component_payload_confirmation_entry), "");
    gtk_label_set_text(GTK_LABEL(widgets->component_status),
        "Verifying payload inputs with cancellable streaming hashes…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->component_cancellable, payload_verify_finished);
    g_task_set_task_data(task, request, payload_verify_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, payload_verify_worker);
    g_object_unref(task);
}

static void payload_build_task_free(gpointer data) {
    payload_build_task *request = data;
    if (request == NULL) return;
    g_free(request->bundle_path);
    g_free(request->workspace_root);
    g_free(request);
}

static void payload_build_task_result_free(gpointer data) {
    payload_build_task_result *result = data;
    if (result == NULL) return;
    lt48_component_payload_build_execution_clear(&result->execution);
    g_free(result);
}

static void payload_build_worker(
    GTask *task, gpointer source_object, gpointer task_data,
    GCancellable *cancellable
) {
    payload_build_task *request = task_data;
    payload_build_task_result *result = g_new0(payload_build_task_result, 1);
    (void)source_object;
    result->status = lt48_component_bundle_execute_payload_builds_cancellable(
        request->bundle_path, request->workspace_root,
        request->confirmation, gio_cancelled, cancellable,
        &result->execution);
    g_task_return_pointer(task, result, payload_build_task_result_free);
}

static void payload_build_finished(
    GObject *source_object, GAsyncResult *async_result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    payload_build_task_result *result = g_task_propagate_pointer(
        G_TASK(async_result), NULL);
    GtkTextBuffer *preview = gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(widgets->component_preview));
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    (void)source_object;
    component_task_controls_sensitive(widgets, TRUE);
    gtk_widget_set_sensitive(widgets->component_payload_execute_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_validate_all_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_validate_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_semantic_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_compile_plan_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_validate_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_resolve_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_diff_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_adapter_test_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_payload_plan_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_compile_matrix_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_payload_verify_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_verify_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_import_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_evidence_audit_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_release_check_button, TRUE);
    gtk_widget_set_sensitive(widgets->component_cancel_button, FALSE);
    g_clear_object(&widgets->component_cancellable);
    if (result == NULL || result->status != LT48_OK) {
        message = g_strdup_printf(
            "%s: %s\nNo USB was opened. Existing output is never overwritten "
            "and incomplete output is removed.",
            result != NULL && result->status == LT48_CANCELLED ?
                "Payload build cancelled" :
                "Payload build execution rejected",
            result == NULL ? "unknown task failure" :
                lt48_status_string(result->status));
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        if (result == NULL || result->execution.json == NULL)
            gtk_text_buffer_set_text(preview, "", 0);
    } else {
        digest_hex(result->execution.sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Payload builds complete · %zu/%zu completed · %zu outputs "
            "verified\nExecution SHA-256: %s\nThe hash-pinned toolchain ran "
            "without opening USB.",
            result->execution.completed_payload_count,
            result->execution.attempted_payload_count,
            result->execution.verified_output_count, digest);
        gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
        gtk_text_buffer_set_text(preview,
            (const char *)result->execution.json,
            (gint)result->execution.json_size);
    }
    g_free(message);
    payload_build_task_result_free(result);
    g_application_release(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
}

static void component_execute_payload_builds(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_path_entry));
    const char *root = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_payload_root_entry));
    const char *confirmation = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_payload_confirmation_entry));
    payload_build_task *request = g_new0(payload_build_task, 1);
    GTask *task;
    (void)button;
    if (path == NULL || path[0] == '\0' || root == NULL || root[0] == '\0' ||
        !parse_digest_hex(confirmation, request->confirmation)) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
            "Verify payload inputs first; execution requires the exact "
            "64-digit verification SHA-256 confirmation");
        g_free(request);
        return;
    }
    if (widgets->component_cancellable != NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->component_status),
                           "Another cancellable component task is running");
        g_free(request);
        return;
    }
    request->bundle_path = g_strdup(path);
    request->workspace_root = g_strdup(root);
    widgets->component_cancellable = g_cancellable_new();
    component_task_controls_sensitive(widgets, FALSE);
    gtk_widget_set_sensitive(widgets->component_payload_execute_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_validate_all_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_validate_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_semantic_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_compile_plan_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_validate_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_resolve_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_diff_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_adapter_test_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_payload_plan_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_compile_matrix_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_payload_verify_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_verify_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_import_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_evidence_audit_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_release_check_button, FALSE);
    gtk_widget_set_sensitive(widgets->component_cancel_button, TRUE);
    gtk_label_set_text(GTK_LABEL(widgets->component_status),
        "Running confirmed hash-pinned payload build in the background…");
    g_application_hold(G_APPLICATION(
        gtk_window_get_application(widgets->window)));
    task = tracked_task_new(
        widgets, widgets->component_cancellable, payload_build_finished);
    g_task_set_task_data(task, request, payload_build_task_free);
    g_task_set_return_on_cancel(task, FALSE);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, payload_build_worker);
    g_object_unref(task);
}

static void component_compile_plan(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_COMPILE_PLAN);
}

static void component_compile_plan_matrix(
    GtkButton *button, gpointer user_data
) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_COMPILE_MATRIX);
}

static void component_validate_evidence(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_VALIDATE_EVIDENCE);
}

static void component_verify_evidence(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_VERIFY_EVIDENCE);
}

static void component_import_evidence(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_IMPORT_EVIDENCE);
}

static void component_audit_evidence(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_AUDIT_EVIDENCE);
}

static void component_release_check(GtkButton *button, gpointer user_data) {
    (void)button;
    component_start_long(user_data, COMPONENT_LONG_RELEASE_CHECK);
}

static void component_init(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_path_entry));
    const char *bundle_id = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_id_entry));
    const char *version = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_version_entry));
    const char *author = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_author_entry));
    const char *license = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_license_entry));
    const char *date = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_date_entry));
    unsigned char digest[LT48_SHA256_SIZE];
    char digest_text[LT48_SHA256_SIZE * 2u + 1u];
    lt48_status status;
    char *message;
    (void)button;
    status = lt48_component_bundle_init(
        path, bundle_id, version, author, license, date, digest
    );
    if (status == LT48_OK) {
        digest_hex(digest, LT48_SHA256_SIZE, digest_text);
        message = g_strdup_printf(
            "Created %s exclusively and revalidated it\nBundle %s version %s\n"
            "Source SHA-256: %s\nNo USB opened and no operation enabled.",
            path, bundle_id, version, digest_text
        );
    } else {
        message = g_strdup_printf(
            "Bundle creation rejected: %s. All identity fields and a real "
            "YYYY-MM-DD date are required; existing files are never overwritten.",
            lt48_status_string(status)
        );
    }
    gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
    g_free(message);
}

static void component_add_object(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    const char *input = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_path_entry));
    const char *output = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_revision_output_entry));
    const char *version = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_version_entry));
    const char *date = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_date_entry));
    const char *object_record = gtk_editable_get_text(
        GTK_EDITABLE(widgets->component_object_record_entry));
    guint selected_kind = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->component_object_kind_dropdown));
    const char *expected_kind = selected_kind + 1u < G_N_ELEMENTS(component_object_kinds) ?
        component_object_kinds[selected_kind] : NULL;
    lt48_component_revision_report report;
    lt48_status status;
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    (void)button;
    status = lt48_component_bundle_add_object_of_kind(
        input, output, version, date, object_record, expected_kind, &report
    );
    if (status == LT48_OK) {
        digest_hex(report.output_source_sha256, LT48_SHA256_SIZE, digest);
        message = g_strdup_printf(
            "Created immutable bundle revision %s exclusively\n"
            "Object kind: %s · objects: %zu → %zu · output SHA-256: %s\n"
            "The source bundle was not modified. The new revision remains "
            "non-executable authoring data.",
            output, expected_kind, report.previous_object_count,
            report.output_object_count, digest
        );
    } else {
        message = g_strdup_printf(
            "Add-%s revision rejected: %s. Input, new output, changed "
            "bundle version, real date, and a strict object-record JSON are "
            "required; references must already exist.",
            expected_kind != NULL ? expected_kind : "object",
            lt48_status_string(status)
        );
    }
    gtk_label_set_text(GTK_LABEL(widgets->component_status), message);
    g_free(message);
}

static GtkWidget *build_component_page(app_widgets *widgets) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *resolve_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *plan_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *payload_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *evidence_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *evidence_store_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *validate = gtk_button_new_with_label("Validate bundle offline");
    GtkWidget *semantic = gtk_button_new_with_label("Check semantic schemas");
    GtkWidget *validate_all = gtk_button_new_with_label("Validate all variants");
    GtkWidget *resolve = gtk_button_new_with_label("Resolve + review delta");
    GtkWidget *diff = gtk_button_new_with_label("Strict variant diff");
    GtkWidget *adapter_test = gtk_button_new_with_label("Generate adapter test plan");
    GtkWidget *compile_plan = gtk_button_new_with_label("Compile authored plan");
    GtkWidget *compile_matrix = gtk_button_new_with_label("Compile full matrix");
    GtkWidget *cancel_component = gtk_button_new_with_label("Cancel component task");
    GtkWidget *build_payloads = gtk_button_new_with_label("Plan payload builds");
    GtkWidget *verify_payloads = gtk_button_new_with_label("Verify payload inputs");
    GtkWidget *execute_payloads =
        gtk_button_new_with_label("Execute confirmed payload builds");
    GtkWidget *validate_evidence = gtk_button_new_with_label("Validate evidence");
    GtkWidget *verify_evidence = gtk_button_new_with_label("Verify bindings");
    GtkWidget *import_evidence = gtk_button_new_with_label("Import verified evidence");
    GtkWidget *audit_evidence = gtk_button_new_with_label("Audit evidence sufficiency");
    GtkWidget *release_check = gtk_button_new_with_label("Run release check");
    GtkWidget *init = gtk_button_new_with_label("Create new bundle exclusively");
    GtkWidget *add_object = gtk_button_new_with_label("Add typed object as revision");
    GtkWidget *init_grid = gtk_grid_new();
    GtkWidget *revision_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *notice = gtk_label_new(
        "Authored component bundle schema 1 validates stable object identities, "
        "versions, SHA-256 declarations, references, same-kind inheritance, "
        "cycles, and variant/package/adapter combination types. Unknown fields "
        "and malformed graphs fail closed. Semantic composition and authored "
        "plan compilation produce a typed offline phase IR. The UI reports it "
        "as transport-ready only after a separate engine lowerer and evidence "
        "gate; executable support and hardware gates remain mandatory."
    );
    widgets->component_path_entry = gtk_entry_new();
    widgets->component_validate_button = validate;
    widgets->component_semantic_button = semantic;
    widgets->component_validate_all_button = validate_all;
    widgets->component_compile_plan_button = compile_plan;
    widgets->component_compile_matrix_button = compile_matrix;
    widgets->component_evidence_validate_button = validate_evidence;
    widgets->component_resolve_button = resolve;
    widgets->component_diff_button = diff;
    widgets->component_adapter_test_button = adapter_test;
    widgets->component_payload_plan_button = build_payloads;
    widgets->component_cancel_button = cancel_component;
    gtk_widget_set_sensitive(cancel_component, FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->component_path_entry),
                                   "component-bundle.json");
    gtk_widget_set_hexpand(widgets->component_path_entry, TRUE);
    gtk_box_append(GTK_BOX(row), gtk_label_new("Bundle:"));
    gtk_box_append(GTK_BOX(row), widgets->component_path_entry);
    gtk_box_append(GTK_BOX(row), validate);
    gtk_box_append(GTK_BOX(row), semantic);
    gtk_box_append(GTK_BOX(row), validate_all);
    gtk_box_append(GTK_BOX(box), row);
    gtk_grid_set_row_spacing(GTK_GRID(init_grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(init_grid), 8);
    widgets->component_id_entry = gtk_entry_new();
    widgets->component_version_entry = gtk_entry_new();
    widgets->component_author_entry = gtk_entry_new();
    widgets->component_license_entry = gtk_entry_new();
    widgets->component_date_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->component_id_entry),
                                   "stable.bundle-id");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->component_version_entry),
                                   "1.0.0");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->component_author_entry),
                                   "Author or organization");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->component_license_entry),
                                   "SPDX identifier or explicit license");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->component_date_entry),
                                   "YYYY-MM-DD");
    gtk_grid_attach(GTK_GRID(init_grid), gtk_label_new("Bundle ID:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), widgets->component_id_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), gtk_label_new("Version:"), 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), widgets->component_version_entry,
                    3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), gtk_label_new("Date:"), 4, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), widgets->component_date_entry,
                    5, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), gtk_label_new("Author:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), widgets->component_author_entry,
                    1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(init_grid), gtk_label_new("License:"), 3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), widgets->component_license_entry,
                    4, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(init_grid), init, 5, 1, 1, 1);
    gtk_box_append(GTK_BOX(box), init_grid);
    widgets->component_revision_output_entry = gtk_entry_new();
    widgets->component_object_record_entry = gtk_entry_new();
    widgets->component_object_kind_dropdown =
        gtk_drop_down_new_from_strings(component_object_kinds);
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_revision_output_entry),
        "new-bundle-revision.json");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_object_record_entry),
        "component-object-record.json");
    gtk_widget_set_hexpand(widgets->component_revision_output_entry, TRUE);
    gtk_widget_set_hexpand(widgets->component_object_record_entry, TRUE);
    gtk_box_append(GTK_BOX(revision_row), gtk_label_new("Revision output:"));
    gtk_box_append(GTK_BOX(revision_row),
                   widgets->component_revision_output_entry);
    gtk_box_append(GTK_BOX(revision_row), gtk_label_new("Object record:"));
    gtk_box_append(GTK_BOX(revision_row), widgets->component_object_record_entry);
    gtk_box_append(GTK_BOX(revision_row), gtk_label_new("Required kind:"));
    gtk_box_append(GTK_BOX(revision_row),
                   widgets->component_object_kind_dropdown);
    gtk_box_append(GTK_BOX(revision_row), add_object);
    gtk_box_append(GTK_BOX(box), revision_row);
    widgets->component_base_object_entry = gtk_entry_new();
    widgets->component_object_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_base_object_entry),
        "variant.base");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->component_object_entry),
                                   "variant.child or object ID");
    gtk_widget_set_hexpand(widgets->component_base_object_entry, TRUE);
    gtk_widget_set_hexpand(widgets->component_object_entry, TRUE);
    gtk_box_append(GTK_BOX(resolve_row), gtk_label_new("Base variant:"));
    gtk_box_append(GTK_BOX(resolve_row),
                   widgets->component_base_object_entry);
    gtk_box_append(GTK_BOX(resolve_row), gtk_label_new("Object ID:"));
    gtk_box_append(GTK_BOX(resolve_row), widgets->component_object_entry);
    gtk_box_append(GTK_BOX(resolve_row), resolve);
    gtk_box_append(GTK_BOX(resolve_row), diff);
    gtk_box_append(GTK_BOX(resolve_row), adapter_test);
    gtk_box_append(GTK_BOX(box), resolve_row);
    widgets->component_combination_entry = gtk_entry_new();
    widgets->component_operation_entry = gtk_entry_new();
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->component_combination_entry), "0");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_operation_entry), "read");
    gtk_widget_set_hexpand(widgets->component_operation_entry, TRUE);
    gtk_box_append(GTK_BOX(plan_row), gtk_label_new("Combination:"));
    gtk_box_append(GTK_BOX(plan_row), widgets->component_combination_entry);
    gtk_box_append(GTK_BOX(plan_row), gtk_label_new("Operation:"));
    gtk_box_append(GTK_BOX(plan_row), widgets->component_operation_entry);
    gtk_box_append(GTK_BOX(plan_row), compile_plan);
    gtk_box_append(GTK_BOX(plan_row), compile_matrix);
    gtk_box_append(GTK_BOX(plan_row), cancel_component);
    gtk_box_append(GTK_BOX(box), plan_row);
    widgets->component_payload_root_entry = gtk_entry_new();
    widgets->component_payload_confirmation_entry = gtk_entry_new();
    widgets->component_payload_execute_button = execute_payloads;
    widgets->component_payload_verify_button = verify_payloads;
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_payload_root_entry),
        "payload source/toolchain workspace root");
    gtk_widget_set_hexpand(widgets->component_payload_root_entry, TRUE);
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_payload_confirmation_entry),
        "verification SHA-256 confirmation");
    gtk_widget_set_hexpand(widgets->component_payload_confirmation_entry,
                           TRUE);
    gtk_widget_set_sensitive(execute_payloads, FALSE);
    gtk_box_append(GTK_BOX(payload_row), gtk_label_new("Payload workspace:"));
    gtk_box_append(GTK_BOX(payload_row), widgets->component_payload_root_entry);
    gtk_box_append(GTK_BOX(payload_row), build_payloads);
    gtk_box_append(GTK_BOX(payload_row), verify_payloads);
    gtk_box_append(GTK_BOX(payload_row),
                   widgets->component_payload_confirmation_entry);
    gtk_box_append(GTK_BOX(payload_row), execute_payloads);
    gtk_box_append(GTK_BOX(box), payload_row);
    widgets->component_evidence_entry = gtk_entry_new();
    widgets->component_evidence_artifact_entry = gtk_entry_new();
    widgets->component_evidence_verify_button = verify_evidence;
    widgets->component_evidence_import_button = import_evidence;
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_evidence_entry),
        "immutable-evidence-record.json");
    gtk_widget_set_hexpand(widgets->component_evidence_entry, TRUE);
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_evidence_artifact_entry),
        "detailed-report-or-transcript");
    gtk_widget_set_hexpand(widgets->component_evidence_artifact_entry, TRUE);
    gtk_box_append(GTK_BOX(evidence_row), gtk_label_new("Evidence record:"));
    gtk_box_append(GTK_BOX(evidence_row), widgets->component_evidence_entry);
    gtk_box_append(GTK_BOX(evidence_row), gtk_label_new("Artifact:"));
    gtk_box_append(GTK_BOX(evidence_row),
                   widgets->component_evidence_artifact_entry);
    gtk_box_append(GTK_BOX(evidence_row), validate_evidence);
    gtk_box_append(GTK_BOX(evidence_row), verify_evidence);
    gtk_box_append(GTK_BOX(box), evidence_row);
    widgets->component_evidence_store_entry = gtk_entry_new();
    widgets->component_evidence_audit_button = audit_evidence;
    widgets->component_release_check_button = release_check;
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->component_evidence_store_entry),
        "content-addressed-evidence-store");
    gtk_widget_set_hexpand(widgets->component_evidence_store_entry, TRUE);
    gtk_box_append(GTK_BOX(evidence_store_row),
                   gtk_label_new("Evidence store:"));
    gtk_box_append(GTK_BOX(evidence_store_row),
                   widgets->component_evidence_store_entry);
    gtk_box_append(GTK_BOX(evidence_store_row), import_evidence);
    gtk_box_append(GTK_BOX(evidence_store_row), audit_evidence);
    gtk_box_append(GTK_BOX(evidence_store_row), release_check);
    gtk_box_append(GTK_BOX(box), evidence_store_row);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_widget_add_css_class(notice, "dim-label");
    gtk_box_append(GTK_BOX(box), notice);
    widgets->component_status = gtk_label_new("No component bundle validated");
    gtk_label_set_xalign(GTK_LABEL(widgets->component_status), 0.0F);
    gtk_label_set_yalign(GTK_LABEL(widgets->component_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->component_status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->component_status), TRUE);
    gtk_widget_set_vexpand(widgets->component_status, TRUE);
    gtk_box_append(GTK_BOX(box), widgets->component_status);
    {
        GtkWidget *scroll = gtk_scrolled_window_new();
        widgets->component_preview = gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->component_preview),
                                   FALSE);
        gtk_text_view_set_cursor_visible(
            GTK_TEXT_VIEW(widgets->component_preview), FALSE);
        gtk_text_view_set_monospace(GTK_TEXT_VIEW(widgets->component_preview),
                                    TRUE);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),
                                      widgets->component_preview);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(GTK_BOX(box), scroll);
    }
    g_signal_connect(validate, "clicked", G_CALLBACK(component_validate), widgets);
    g_signal_connect(semantic, "clicked",
                     G_CALLBACK(component_check_semantics), widgets);
    g_signal_connect(validate_all, "clicked",
                     G_CALLBACK(component_validate_all_variants), widgets);
    g_signal_connect(resolve, "clicked", G_CALLBACK(component_resolve), widgets);
    g_signal_connect(diff, "clicked", G_CALLBACK(component_diff), widgets);
    g_signal_connect(adapter_test, "clicked",
                     G_CALLBACK(component_adapter_test_plan), widgets);
    g_signal_connect(compile_plan, "clicked",
                     G_CALLBACK(component_compile_plan), widgets);
    g_signal_connect(compile_matrix, "clicked",
                     G_CALLBACK(component_compile_plan_matrix), widgets);
    g_signal_connect(cancel_component, "clicked",
                     G_CALLBACK(component_cancel_long), widgets);
    g_signal_connect(build_payloads, "clicked",
                     G_CALLBACK(component_plan_payload_builds), widgets);
    g_signal_connect(verify_payloads, "clicked",
                     G_CALLBACK(component_verify_payload_inputs), widgets);
    g_signal_connect(execute_payloads, "clicked",
                     G_CALLBACK(component_execute_payload_builds), widgets);
    g_signal_connect(validate_evidence, "clicked",
                     G_CALLBACK(component_validate_evidence), widgets);
    g_signal_connect(verify_evidence, "clicked",
                     G_CALLBACK(component_verify_evidence), widgets);
    g_signal_connect(import_evidence, "clicked",
                     G_CALLBACK(component_import_evidence), widgets);
    g_signal_connect(audit_evidence, "clicked",
                     G_CALLBACK(component_audit_evidence), widgets);
    g_signal_connect(release_check, "clicked",
                     G_CALLBACK(component_release_check), widgets);
    g_signal_connect(init, "clicked", G_CALLBACK(component_init), widgets);
    g_signal_connect(add_object, "clicked",
                     G_CALLBACK(component_add_object), widgets);
    g_signal_connect_swapped(
        widgets->component_path_entry, "activate",
        G_CALLBACK(gtk_widget_activate), validate
    );
    g_signal_connect_swapped(
        widgets->component_object_entry, "activate",
        G_CALLBACK(gtk_widget_activate), resolve
    );
    g_signal_connect_swapped(
        widgets->component_base_object_entry, "activate",
        G_CALLBACK(gtk_widget_activate), diff
    );
    g_signal_connect_swapped(
        widgets->component_operation_entry, "activate",
        G_CALLBACK(gtk_widget_activate), compile_plan
    );
    g_signal_connect_swapped(
        widgets->component_evidence_entry, "activate",
        G_CALLBACK(gtk_widget_activate), validate_evidence
    );
    g_signal_connect_swapped(
        widgets->component_evidence_artifact_entry, "activate",
        G_CALLBACK(gtk_widget_activate), verify_evidence
    );
    g_signal_connect_swapped(
        widgets->component_evidence_store_entry, "activate",
        G_CALLBACK(gtk_widget_activate), import_evidence
    );
    return box;
}

static GtkWidget *build_buffer_page(app_widgets *widgets) {
    static const char *short_policies[] = {
        "Reject short input", "Pad short input", NULL
    };
    static const char *long_policies[] = {
        "Reject long input", "Truncate long input", NULL
    };
    static const char *byte_orders[] = {
        "Little-endian typed edit", "Big-endian typed edit", NULL
    };
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *load = gtk_button_new_with_label("Load with policies");
    GtkWidget *create = gtk_button_new_with_label("New filled buffer");
    GtkWidget *checksum = gtk_button_new_with_label("Checksum / preview");
    GtkWidget *fill = gtk_button_new_with_label("Fill range");
    GtkWidget *typed_edit = gtk_button_new_with_label("Set typed word");
    GtkWidget *edit = gtk_button_new_with_label("Edit bytes…");
    GtkWidget *save = gtk_button_new_with_label("Save full image");
    GtkWidget *extract = gtk_button_new_with_label("Extract range");
    GtkWidget *compare = gtk_button_new_with_label("Compare selected range");
    GtkWidget *cancel_io = gtk_button_new_with_label("Cancel file I/O");
    GtkWidget *notice = gtk_label_new(
        "Offline only. File bytes map directly to increasing device addresses. "
        "Padding, truncation and per-word byte reversal occur only when the "
        "corresponding controls explicitly request them. Output creation is "
        "exclusive. Compare requires an exact-size file and never adjusts it."
    );
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *edit_window = gtk_window_new();
    GtkWidget *edit_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *edit_grid = gtk_grid_new();
    GtkWidget *edit_navigation =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *edit_previous = gtk_button_new_with_label("Previous Page");
    GtkWidget *edit_go = gtk_button_new_with_label("Go");
    GtkWidget *edit_next = gtk_button_new_with_label("Next Page");
    GtkWidget *edit_scroll = gtk_scrolled_window_new();
    GtkWidget *edit_apply_page =
        gtk_button_new_with_label("Apply Edited Page");
    GtkWidget *edit_apply =
        gtk_button_new_with_label("Apply Replacement Bytes");
    GtkWidget *edit_search_row =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *edit_search = gtk_button_new_with_label("Search");
    GtkWidget *edit_search_next = gtk_button_new_with_label("Next");
    GtkWidget *edit_checksum = gtk_button_new_with_label("Checksum Page");
    GtkWidget *edit_undo = gtk_button_new_with_label("Undo");
    GtkWidget *edit_fill = gtk_button_new_with_label("Fill…");
    GtkWidget *edit_copy = gtk_button_new_with_label("Copy…");
    GtkWidget *edit_swap = gtk_button_new_with_label("Swap…");
    GtkWidget *edit_done = gtk_button_new_with_label("Done");
    GtkWidget *fill_window = gtk_window_new();
    GtkWidget *fill_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *fill_grid = gtk_grid_new();
    GtkWidget *fill_apply = gtk_button_new_with_label("Fill Range");
    GtkWidget *fill_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *copy_window = gtk_window_new();
    GtkWidget *copy_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *copy_grid = gtk_grid_new();
    GtkWidget *copy_apply = gtk_button_new_with_label("Copy Range");
    GtkWidget *copy_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *swap_window = gtk_window_new();
    GtkWidget *swap_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *swap_grid = gtk_grid_new();
    GtkWidget *swap_apply = gtk_button_new_with_label("Swap Words");
    GtkWidget *swap_cancel = gtk_button_new_with_label("Cancel");
    static const char *image_formats[] = {
        "Auto from suffix/content", "Raw BIN/ROM", "Intel HEX",
        "Motorola S-record", "Straight HEX", "MOS Technology",
        "Tektronix HEX", "HP64000 Absolute", NULL
    };
    static const char *gap_policies[] = {
        "Preserve sparse gaps", "Fill sparse gaps", NULL
    };
    static const char *load_clear_modes[] = {
        "Preserve resident buffer", "Device blank state",
        "Fill with 00", "Fill with FF", NULL
    };
    static const char *save_byte_modes[] = {
        "Normal", "Even bytes", "Odd bytes", "First byte of four",
        "Second byte of four", "Third byte of four",
        "Fourth byte of four", NULL
    };
    GtkWidget *load_image_window = gtk_window_new();
    GtkWidget *load_image_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *load_image_grid = gtk_grid_new();
    GtkWidget *load_image_actions =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *load_image_help = gtk_button_new_with_label("Help");
    GtkWidget *load_image_apply = gtk_button_new_with_label("OK");
    GtkWidget *load_image_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *load_image_notice = gtk_label_new(
        "The selected device capacity is used when available. Intel HEX and "
        "Motorola S-record addresses are mapped relative to File address; "
        "MOS Technology and Tektronix records use 16-bit addresses; HP64000 "
        "Absolute uses 32-bit addresses; Straight HEX is "
        "sequential address-free data. Imported bytes are "
        "then placed at Buffer address. Clear before load controls bytes "
        "outside the imported image; sparse gaps remain unchanged unless "
        "filling is explicitly selected.");
    GtkWidget *save_image_window = gtk_window_new();
    GtkWidget *save_image_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *save_image_grid = gtk_grid_new();
    GtkWidget *save_image_actions =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *save_image_help = gtk_button_new_with_label("Help");
    GtkWidget *save_image_apply = gtk_button_new_with_label("OK");
    GtkWidget *save_image_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *save_image_notice = gtk_label_new(
        "The source range is snapshotted before encoding. Address-bearing "
        "formats use Output address for the first exported byte. Byte-lane "
        "modes pack the selected lane contiguously. Existing files are not "
        "overwritten.");
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    widgets->buffer_input_entry = gtk_entry_new();
    widgets->buffer_size_entry = gtk_entry_new();
    widgets->buffer_start_entry = gtk_entry_new();
    widgets->buffer_length_entry = gtk_entry_new();
    widgets->buffer_value_entry = gtk_entry_new();
    widgets->buffer_output_entry = gtk_entry_new();
    widgets->buffer_compare_entry = gtk_entry_new();
    widgets->buffer_short_dropdown =
        gtk_drop_down_new_from_strings(short_policies);
    widgets->buffer_long_dropdown =
        gtk_drop_down_new_from_strings(long_policies);
    widgets->buffer_word_entry = gtk_entry_new();
    widgets->buffer_byte_order_dropdown =
        gtk_drop_down_new_from_strings(byte_orders);
    widgets->buffer_reverse_check =
        gtk_check_button_new_with_label("Reverse bytes in each word");
    widgets->buffer_controls = grid;
    widgets->buffer_cancel_button = cancel_io;
    widgets->buffer_edit_window = GTK_WINDOW(edit_window);
    widgets->buffer_fill_window = GTK_WINDOW(fill_window);
    widgets->buffer_copy_window = GTK_WINDOW(copy_window);
    widgets->buffer_swap_window = GTK_WINDOW(swap_window);
    widgets->load_image_window = GTK_WINDOW(load_image_window);
    widgets->save_image_window = GTK_WINDOW(save_image_window);
    widgets->load_image_controls = load_image_grid;
    widgets->load_image_apply = load_image_apply;
    widgets->load_image_cancel = load_image_cancel;
    widgets->save_image_controls = save_image_grid;
    widgets->save_image_apply = save_image_apply;
    widgets->save_image_cancel = save_image_cancel;
    widgets->buffer_edit_address = gtk_entry_new();
    widgets->buffer_edit_hex = gtk_entry_new();
    widgets->buffer_edit_view = gtk_text_view_new();
    widgets->buffer_edit_search = gtk_entry_new();
    widgets->buffer_edit_status = gtk_label_new(
        "Enter contiguous hexadecimal bytes, for example 55aa00ff");
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->buffer_edit_address),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Buffer edit address", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->buffer_edit_hex),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Replacement hex bytes", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->buffer_edit_view),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Buffer contents", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->buffer_edit_search),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Search bytes", -1);
    gtk_window_set_title(widgets->buffer_edit_window, "Buffer Edit");
    gtk_window_set_transient_for(widgets->buffer_edit_window, widgets->window);
    gtk_window_set_destroy_with_parent(widgets->buffer_edit_window, TRUE);
    gtk_window_set_hide_on_close(widgets->buffer_edit_window, TRUE);
    gtk_window_set_default_size(widgets->buffer_edit_window, 900, 650);
    widgets->buffer_fill_start = gtk_entry_new();
    widgets->buffer_fill_end = gtk_entry_new();
    widgets->buffer_fill_value = gtk_entry_new();
    widgets->buffer_copy_source = gtk_entry_new();
    widgets->buffer_copy_destination = gtk_entry_new();
    widgets->buffer_copy_length = gtk_entry_new();
    widgets->buffer_swap_start = gtk_entry_new();
    widgets->buffer_swap_length = gtk_entry_new();
    widgets->buffer_swap_word_size = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_fill_start), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_fill_end), "15");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_fill_value), "0xff");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_copy_source), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_copy_destination), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_copy_length), "16");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_swap_length), "16");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_swap_word_size), "2");
    gtk_window_set_title(widgets->buffer_fill_window, "Buffer Fill");
    gtk_window_set_title(widgets->buffer_copy_window, "Buffer Copy");
    gtk_window_set_title(widgets->buffer_swap_window, "Word Swap");
    gtk_window_set_title(
        widgets->load_image_window, "Load A File To Buffer");
    gtk_window_set_transient_for(widgets->load_image_window, widgets->window);
    gtk_window_set_destroy_with_parent(widgets->load_image_window, TRUE);
    gtk_window_set_modal(widgets->load_image_window, TRUE);
    gtk_window_set_hide_on_close(widgets->load_image_window, TRUE);
    gtk_window_set_default_size(widgets->load_image_window, 620, 330);
    gtk_widget_set_margin_top(load_image_box, 14);
    gtk_widget_set_margin_bottom(load_image_box, 14);
    gtk_widget_set_margin_start(load_image_box, 14);
    gtk_widget_set_margin_end(load_image_box, 14);
    gtk_grid_set_row_spacing(GTK_GRID(load_image_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(load_image_grid), 10);
    widgets->load_image_path = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(widgets->load_image_path), FALSE);
    gtk_widget_set_hexpand(widgets->load_image_path, TRUE);
    widgets->load_image_format =
        gtk_drop_down_new_from_strings(image_formats);
    widgets->load_image_size = gtk_entry_new();
    widgets->load_image_base = gtk_entry_new();
    widgets->load_image_target = gtk_entry_new();
    widgets->load_image_clear =
        gtk_drop_down_new_from_strings(load_clear_modes);
    widgets->load_image_gap_policy =
        gtk_drop_down_new_from_strings(gap_policies);
    widgets->load_image_gap_fill = gtk_entry_new();
    widgets->load_image_status = gtk_label_new("Choose how the image maps into the resident device buffer.");
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->load_image_size),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Buffer size", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->load_image_base),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "File address", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->load_image_target),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Buffer address", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->load_image_clear),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Clear before load", -1);
    gtk_editable_set_text(GTK_EDITABLE(widgets->load_image_base), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->load_image_target), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->load_image_gap_fill), "0xff");
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("Source file:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_path,
                    1, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("Format:"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_format,
                    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("Buffer size:"),
                    2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_size,
                    3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("File address:"),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_base,
                    1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("Buffer address:"),
                    2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_target,
                    3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("Clear before load:"),
                    0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_clear,
                    1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("Sparse gaps:"),
                    2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_gap_policy,
                    3, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), gtk_label_new("Gap fill:"),
                    2, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(load_image_grid), widgets->load_image_gap_fill,
                    3, 4, 1, 1);
    gtk_label_set_xalign(GTK_LABEL(load_image_notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(load_image_notice), TRUE);
    gtk_widget_add_css_class(load_image_notice, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(widgets->load_image_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->load_image_status), TRUE);
    gtk_widget_set_halign(load_image_actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(load_image_actions), load_image_help);
    gtk_box_append(GTK_BOX(load_image_actions), load_image_cancel);
    gtk_box_append(GTK_BOX(load_image_actions), load_image_apply);
    gtk_box_append(GTK_BOX(load_image_box), load_image_grid);
    gtk_box_append(GTK_BOX(load_image_box), load_image_notice);
    gtk_box_append(GTK_BOX(load_image_box), widgets->load_image_status);
    gtk_box_append(GTK_BOX(load_image_box), load_image_actions);
    gtk_window_set_child(widgets->load_image_window, load_image_box);
    g_signal_connect(load_image_apply, "clicked",
                     G_CALLBACK(load_image_execute), widgets);
    g_signal_connect(load_image_cancel, "clicked",
                     G_CALLBACK(buffer_dialog_cancel_clicked), widgets);
    g_signal_connect(
        widgets->load_image_window, "close-request",
        G_CALLBACK(buffer_dialog_close_requested), widgets);
    g_signal_connect_swapped(load_image_help, "clicked",
                             G_CALLBACK(gtk_window_present),
                             widgets->help_window);
    gtk_window_set_title(widgets->save_image_window, "Save Buffer To A File");
    gtk_window_set_transient_for(widgets->save_image_window, widgets->window);
    gtk_window_set_destroy_with_parent(widgets->save_image_window, TRUE);
    gtk_window_set_modal(widgets->save_image_window, TRUE);
    gtk_window_set_hide_on_close(widgets->save_image_window, TRUE);
    gtk_window_set_default_size(widgets->save_image_window, 650, 330);
    gtk_widget_set_margin_top(save_image_box, 14);
    gtk_widget_set_margin_bottom(save_image_box, 14);
    gtk_widget_set_margin_start(save_image_box, 14);
    gtk_widget_set_margin_end(save_image_box, 14);
    gtk_grid_set_row_spacing(GTK_GRID(save_image_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(save_image_grid), 10);
    widgets->save_image_path = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(widgets->save_image_path), FALSE);
    gtk_widget_set_hexpand(widgets->save_image_path, TRUE);
    widgets->save_image_format =
        gtk_drop_down_new_from_strings(image_formats);
    widgets->save_image_start = gtk_entry_new();
    widgets->save_image_length = gtk_entry_new();
    widgets->save_image_base = gtk_entry_new();
    widgets->save_image_byte_mode =
        gtk_drop_down_new_from_strings(save_byte_modes);
    widgets->save_image_status = gtk_label_new(
        "Choose the buffer range and output representation.");
    gtk_editable_set_text(GTK_EDITABLE(widgets->save_image_start), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->save_image_base), "0");
    gtk_grid_attach(GTK_GRID(save_image_grid), gtk_label_new("Output file:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), widgets->save_image_path,
                    1, 0, 5, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), gtk_label_new("Format:"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), widgets->save_image_format,
                    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), gtk_label_new("Byte mode:"),
                    2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), widgets->save_image_byte_mode,
                    3, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), gtk_label_new("Buffer start:"),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), widgets->save_image_start,
                    1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), gtk_label_new("Length:"),
                    2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), widgets->save_image_length,
                    3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), gtk_label_new("Output address:"),
                    4, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(save_image_grid), widgets->save_image_base,
                    5, 2, 1, 1);
    gtk_label_set_xalign(GTK_LABEL(save_image_notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(save_image_notice), TRUE);
    gtk_widget_add_css_class(save_image_notice, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(widgets->save_image_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->save_image_status), TRUE);
    gtk_widget_set_halign(save_image_actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(save_image_actions), save_image_help);
    gtk_box_append(GTK_BOX(save_image_actions), save_image_cancel);
    gtk_box_append(GTK_BOX(save_image_actions), save_image_apply);
    gtk_box_append(GTK_BOX(save_image_box), save_image_grid);
    gtk_box_append(GTK_BOX(save_image_box), save_image_notice);
    gtk_box_append(GTK_BOX(save_image_box), widgets->save_image_status);
    gtk_box_append(GTK_BOX(save_image_box), save_image_actions);
    gtk_window_set_child(widgets->save_image_window, save_image_box);
    g_signal_connect(save_image_apply, "clicked",
                     G_CALLBACK(save_image_execute), widgets);
    g_signal_connect(save_image_cancel, "clicked",
                     G_CALLBACK(buffer_dialog_cancel_clicked), widgets);
    g_signal_connect(
        widgets->save_image_window, "close-request",
        G_CALLBACK(buffer_dialog_close_requested), widgets);
    g_signal_connect_swapped(save_image_help, "clicked",
                             G_CALLBACK(gtk_window_present),
                             widgets->help_window);
    GtkWindow *tool_windows[] = {
        widgets->buffer_fill_window,
        widgets->buffer_copy_window,
        widgets->buffer_swap_window
    };
    for (size_t tool = 0u; tool < G_N_ELEMENTS(tool_windows); ++tool) {
        gtk_window_set_transient_for(
            tool_windows[tool], widgets->buffer_edit_window);
        gtk_window_set_destroy_with_parent(tool_windows[tool], TRUE);
        gtk_window_set_modal(tool_windows[tool], TRUE);
        gtk_window_set_hide_on_close(tool_windows[tool], TRUE);
        gtk_window_set_default_size(tool_windows[tool], 430, 210);
    }
    GtkWidget *tool_boxes[] = {fill_box, copy_box, swap_box};
    GtkWidget *tool_grids[] = {fill_grid, copy_grid, swap_grid};
    for (size_t tool = 0u; tool < G_N_ELEMENTS(tool_boxes); ++tool) {
        gtk_widget_set_margin_top(tool_boxes[tool], 12);
        gtk_widget_set_margin_bottom(tool_boxes[tool], 12);
        gtk_widget_set_margin_start(tool_boxes[tool], 12);
        gtk_widget_set_margin_end(tool_boxes[tool], 12);
        gtk_grid_set_row_spacing(GTK_GRID(tool_grids[tool]), 8);
        gtk_grid_set_column_spacing(GTK_GRID(tool_grids[tool]), 8);
    }
    gtk_grid_attach(GTK_GRID(fill_grid), gtk_label_new("From:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(fill_grid), widgets->buffer_fill_start,
                    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(fill_grid), gtk_label_new("To (inclusive):"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(fill_grid), widgets->buffer_fill_end,
                    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(fill_grid), gtk_label_new("Byte value:"),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(fill_grid), widgets->buffer_fill_value,
                    1, 2, 1, 1);
    gtk_box_append(GTK_BOX(fill_box), fill_grid);
    gtk_box_append(GTK_BOX(fill_box), fill_apply);
    gtk_box_append(GTK_BOX(fill_box), fill_cancel);
    gtk_window_set_child(widgets->buffer_fill_window, fill_box);
    gtk_grid_attach(GTK_GRID(copy_grid), gtk_label_new("Source:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(copy_grid), widgets->buffer_copy_source,
                    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(copy_grid), gtk_label_new("Destination:"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(copy_grid), widgets->buffer_copy_destination,
                    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(copy_grid), gtk_label_new("Length:"),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(copy_grid), widgets->buffer_copy_length,
                    1, 2, 1, 1);
    gtk_box_append(GTK_BOX(copy_box), copy_grid);
    gtk_box_append(GTK_BOX(copy_box), copy_apply);
    gtk_box_append(GTK_BOX(copy_box), copy_cancel);
    gtk_window_set_child(widgets->buffer_copy_window, copy_box);
    gtk_grid_attach(GTK_GRID(swap_grid), gtk_label_new("Start:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(swap_grid), widgets->buffer_swap_start,
                    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(swap_grid), gtk_label_new("Length:"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(swap_grid), widgets->buffer_swap_length,
                    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(swap_grid), gtk_label_new("Word bytes (2/4/8):"),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(swap_grid), widgets->buffer_swap_word_size,
                    1, 2, 1, 1);
    gtk_box_append(GTK_BOX(swap_box), swap_grid);
    gtk_box_append(GTK_BOX(swap_box), swap_apply);
    gtk_box_append(GTK_BOX(swap_box), swap_cancel);
    gtk_window_set_child(widgets->buffer_swap_window, swap_box);
    gtk_widget_set_margin_top(edit_box, 12);
    gtk_widget_set_margin_bottom(edit_box, 12);
    gtk_widget_set_margin_start(edit_box, 12);
    gtk_widget_set_margin_end(edit_box, 12);
    gtk_grid_set_row_spacing(GTK_GRID(edit_grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(edit_grid), 8);
    gtk_grid_attach(GTK_GRID(edit_grid), gtk_label_new("Address:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_grid), widgets->buffer_edit_address,
                    1, 0, 1, 1);
    gtk_widget_set_hexpand(widgets->buffer_edit_address, TRUE);
    gtk_box_append(GTK_BOX(edit_navigation), edit_previous);
    gtk_box_append(GTK_BOX(edit_navigation), edit_go);
    gtk_box_append(GTK_BOX(edit_navigation), edit_next);
    gtk_grid_attach(GTK_GRID(edit_grid), edit_navigation, 2, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(edit_grid), gtk_label_new("Hex bytes:"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_grid), widgets->buffer_edit_hex,
                    1, 1, 3, 1);
    gtk_widget_set_hexpand(widgets->buffer_edit_hex, TRUE);
    gtk_text_view_set_editable(
        GTK_TEXT_VIEW(widgets->buffer_edit_view), TRUE);
    gtk_text_view_set_cursor_visible(
        GTK_TEXT_VIEW(widgets->buffer_edit_view), TRUE);
    gtk_text_view_set_monospace(
        GTK_TEXT_VIEW(widgets->buffer_edit_view), TRUE);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(edit_scroll), widgets->buffer_edit_view);
    gtk_scrolled_window_set_min_content_height(
        GTK_SCROLLED_WINDOW(edit_scroll), 360);
    gtk_widget_set_vexpand(edit_scroll, TRUE);
    gtk_label_set_xalign(GTK_LABEL(widgets->buffer_edit_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->buffer_edit_status), TRUE);
    gtk_widget_set_halign(edit_done, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(edit_box), edit_grid);
    gtk_box_append(GTK_BOX(edit_box), edit_scroll);
    gtk_box_append(GTK_BOX(edit_box), edit_apply_page);
    gtk_box_append(GTK_BOX(edit_box), edit_apply);
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->buffer_edit_search),
        "hexadecimal byte pattern");
    gtk_widget_set_hexpand(widgets->buffer_edit_search, TRUE);
    gtk_box_append(GTK_BOX(edit_search_row), widgets->buffer_edit_search);
    gtk_box_append(GTK_BOX(edit_search_row), edit_search);
    gtk_box_append(GTK_BOX(edit_search_row), edit_search_next);
    gtk_box_append(GTK_BOX(edit_search_row), edit_checksum);
    gtk_box_append(GTK_BOX(edit_search_row), edit_undo);
    gtk_box_append(GTK_BOX(edit_search_row), edit_fill);
    gtk_box_append(GTK_BOX(edit_search_row), edit_copy);
    gtk_box_append(GTK_BOX(edit_search_row), edit_swap);
    gtk_box_append(GTK_BOX(edit_box), edit_search_row);
    gtk_box_append(GTK_BOX(edit_box), widgets->buffer_edit_status);
    gtk_box_append(GTK_BOX(edit_box), edit_done);
    gtk_window_set_child(widgets->buffer_edit_window, edit_box);
    g_signal_connect(edit_apply, "clicked",
                     G_CALLBACK(buffer_edit_hex_apply), widgets);
    g_signal_connect(edit_apply_page, "clicked",
                     G_CALLBACK(buffer_edit_apply_page), widgets);
    g_object_set_data(G_OBJECT(edit_previous), "labtool48-page-direction",
                      GINT_TO_POINTER(-1));
    g_object_set_data(G_OBJECT(edit_next), "labtool48-page-direction",
                      GINT_TO_POINTER(1));
    g_signal_connect(edit_previous, "clicked",
                     G_CALLBACK(buffer_edit_move_page), widgets);
    g_signal_connect(edit_go, "clicked",
                     G_CALLBACK(buffer_edit_go), widgets);
    g_signal_connect(edit_next, "clicked",
                     G_CALLBACK(buffer_edit_move_page), widgets);
    g_signal_connect_swapped(
        widgets->buffer_edit_address, "activate",
        G_CALLBACK(gtk_widget_activate), edit_go);
    g_object_set_data(G_OBJECT(edit_search), "labtool48-find-next",
                      GINT_TO_POINTER(0));
    g_object_set_data(G_OBJECT(edit_search_next), "labtool48-find-next",
                      GINT_TO_POINTER(1));
    g_signal_connect(edit_search, "clicked",
                     G_CALLBACK(buffer_edit_search_bytes), widgets);
    g_signal_connect(edit_search_next, "clicked",
                     G_CALLBACK(buffer_edit_search_bytes), widgets);
    g_signal_connect_swapped(
        widgets->buffer_edit_search, "activate",
        G_CALLBACK(gtk_widget_activate), edit_search);
    g_signal_connect(edit_checksum, "clicked",
                     G_CALLBACK(buffer_edit_checksum_page), widgets);
    g_signal_connect(edit_undo, "clicked",
                     G_CALLBACK(buffer_edit_undo), widgets);
    g_object_set_data(G_OBJECT(edit_fill), "labtool48-buffer-tool-window",
                      widgets->buffer_fill_window);
    g_object_set_data(G_OBJECT(edit_fill), "labtool48-buffer-tool-start",
                      widgets->buffer_fill_start);
    g_object_set_data(G_OBJECT(edit_copy), "labtool48-buffer-tool-window",
                      widgets->buffer_copy_window);
    g_object_set_data(G_OBJECT(edit_copy), "labtool48-buffer-tool-start",
                      widgets->buffer_copy_source);
    g_object_set_data(G_OBJECT(edit_swap), "labtool48-buffer-tool-window",
                      widgets->buffer_swap_window);
    g_object_set_data(G_OBJECT(edit_swap), "labtool48-buffer-tool-start",
                      widgets->buffer_swap_start);
    g_signal_connect(edit_fill, "clicked",
                     G_CALLBACK(buffer_edit_present_tool), widgets);
    g_signal_connect(edit_copy, "clicked",
                     G_CALLBACK(buffer_edit_present_tool), widgets);
    g_signal_connect(edit_swap, "clicked",
                     G_CALLBACK(buffer_edit_present_tool), widgets);
    g_signal_connect(fill_apply, "clicked",
                     G_CALLBACK(buffer_edit_fill_apply), widgets);
    g_signal_connect(copy_apply, "clicked",
                     G_CALLBACK(buffer_edit_copy_apply), widgets);
    g_signal_connect(swap_apply, "clicked",
                     G_CALLBACK(buffer_edit_swap_apply), widgets);
    g_signal_connect(fill_cancel, "clicked", G_CALLBACK(hide_auxiliary_window),
                     widgets->buffer_fill_window);
    g_signal_connect(copy_cancel, "clicked", G_CALLBACK(hide_auxiliary_window),
                     widgets->buffer_copy_window);
    g_signal_connect(swap_cancel, "clicked", G_CALLBACK(hide_auxiliary_window),
                     widgets->buffer_swap_window);
    g_signal_connect(edit_done, "clicked",
                     G_CALLBACK(hide_auxiliary_window),
                     widgets->buffer_edit_window);
    gtk_widget_set_sensitive(cancel_io, FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->buffer_input_entry),
                                   "binary input path");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->buffer_output_entry),
                                   "new output path");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->buffer_compare_entry),
                                   "exact-size expected binary path");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_start_entry), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_value_entry), "0xff");
    gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_word_entry), "1");
    gtk_widget_set_hexpand(widgets->buffer_input_entry, TRUE);
    gtk_widget_set_hexpand(widgets->buffer_output_entry, TRUE);
    gtk_widget_set_hexpand(widgets->buffer_compare_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Input:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_input_entry, 1, 0, 4, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Buffer size:"), 5, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_size_entry, 6, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), load, 7, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_short_dropdown,
                    0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_long_dropdown,
                    2, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Word bytes:"), 4, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_word_entry, 5, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_reverse_check,
                    6, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Start:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_start_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Length:"), 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_length_entry, 3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Value/pad:"),
                    4, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_value_entry, 5, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create, 6, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), checksum, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), fill, 2, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_byte_order_dropdown,
                    4, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), typed_edit, 6, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), edit, 7, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Output:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_output_entry, 1, 4, 4, 1);
    gtk_grid_attach(GTK_GRID(grid), save, 5, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), extract, 7, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Compare file:"),
                    0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->buffer_compare_entry,
                    1, 5, 5, 1);
    gtk_grid_attach(GTK_GRID(grid), compare, 6, 5, 2, 1);
    g_signal_connect(load, "clicked", G_CALLBACK(buffer_load), widgets);
    g_signal_connect(create, "clicked", G_CALLBACK(buffer_create), widgets);
    g_signal_connect(checksum, "clicked", G_CALLBACK(buffer_checksum), widgets);
    g_signal_connect(fill, "clicked", G_CALLBACK(buffer_fill), widgets);
    g_signal_connect(typed_edit, "clicked", G_CALLBACK(buffer_edit), widgets);
    g_signal_connect(edit, "clicked",
                     G_CALLBACK(buffer_edit_window_clicked), widgets);
    g_signal_connect(compare, "clicked", G_CALLBACK(buffer_compare), widgets);
    g_signal_connect(save, "clicked", G_CALLBACK(buffer_save), widgets);
    g_object_set_data(G_OBJECT(extract), "labtool48-extract",
                      GINT_TO_POINTER(1));
    g_signal_connect(extract, "clicked", G_CALLBACK(buffer_save), widgets);
    gtk_box_append(GTK_BOX(box), grid);
    g_signal_connect(cancel_io, "clicked", G_CALLBACK(buffer_cancel_io),
                     widgets);
    gtk_box_append(GTK_BOX(box), cancel_io);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_widget_add_css_class(notice, "dim-label");
    gtk_box_append(GTK_BOX(box), notice);
    widgets->buffer_status = gtk_label_new(
        "No buffer loaded; the complete image remains resident while only a "
        "bounded range is previewed"
    );
    gtk_label_set_xalign(GTK_LABEL(widgets->buffer_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->buffer_status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->buffer_status), TRUE);
    gtk_box_append(GTK_BOX(box), widgets->buffer_status);
    widgets->buffer_preview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->buffer_preview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(widgets->buffer_preview),
                                     FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(widgets->buffer_preview), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),
                                  widgets->buffer_preview);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll),
                                                260);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    return box;
}

static void toolbar_edit_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    if (selected_fuse_editor_available(widgets)) {
        open_fuse_editor(widgets);
        return;
    }
    gtk_notebook_set_current_page(
        GTK_NOTEBOOK(widgets->main_notebook), PAGE_BUFFER);
    buffer_edit_window_clicked(NULL, widgets);
}

static void present_load_image_file(app_widgets *widgets, GFile *file) {
    char *path;
    char *lower;
    guint format = LT48_IMAGE_FORMAT_AUTO;
    uint32_t target_size = 0u;
    char size_text[32];
    lt48_device_spec spec;
    if (widgets == NULL || file == NULL) return;
    path = g_file_get_path(file);
    if (path == NULL) {
        gtk_label_set_text(
            GTK_LABEL(widgets->buffer_status),
            "Only local image files can be loaded into the resident buffer");
        return;
    }
    lower = g_ascii_strdown(path, -1);
    if (g_str_has_suffix(lower, ".hex") ||
        g_str_has_suffix(lower, ".ihx"))
        format = LT48_IMAGE_FORMAT_INTEL_HEX;
    else if (g_str_has_suffix(lower, ".s19") ||
             g_str_has_suffix(lower, ".s28") ||
             g_str_has_suffix(lower, ".s37") ||
             g_str_has_suffix(lower, ".srec") ||
             g_str_has_suffix(lower, ".mot"))
        format = LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD;
    else if (g_str_has_suffix(lower, ".shx") ||
             g_str_has_suffix(lower, ".hexstr"))
        format = LT48_IMAGE_FORMAT_STRAIGHT_HEX;
    else if (g_str_has_suffix(lower, ".mos"))
        format = LT48_IMAGE_FORMAT_MOS_TECHNOLOGY;
    else if (g_str_has_suffix(lower, ".tek") ||
             g_str_has_suffix(lower, ".tekhex"))
        format = LT48_IMAGE_FORMAT_TEKTRONIX_HEX;
    else if (g_str_has_suffix(lower, ".abs") ||
             g_str_has_suffix(lower, ".hpabs"))
        format = LT48_IMAGE_FORMAT_HP64000_ABSOLUTE;
    else if (g_str_has_suffix(lower, ".bin") ||
             g_str_has_suffix(lower, ".rom"))
        format = LT48_IMAGE_FORMAT_RAW_BINARY;
    if (format == LT48_IMAGE_FORMAT_AUTO) {
        GFileInputStream *stream = g_file_read(file, NULL, NULL);
        unsigned char marker[20] = {0u};
        if (stream != NULL) {
            gssize count = g_input_stream_read(
                G_INPUT_STREAM(stream), marker, sizeof(marker), NULL, NULL);
            if (count >= 20 &&
                memcmp(marker, "Programmer 99/2000", 18u) == 0 &&
                marker[18] == 0u && marker[19] == 7u)
                format = LT48_IMAGE_FORMAT_HP64000_ABSOLUTE;
            else if (count >= 1 && marker[0] == ':')
                format = LT48_IMAGE_FORMAT_INTEL_HEX;
            else if (count >= 1 && marker[0] == 'S')
                format = LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD;
            else if (count >= 1 && marker[0] == ';')
                format = LT48_IMAGE_FORMAT_MOS_TECHNOLOGY;
            else if (count >= 1 && marker[0] == '/')
                format = LT48_IMAGE_FORMAT_TEKTRONIX_HEX;
            else if (count >= 1)
                format = LT48_IMAGE_FORMAT_RAW_BINARY;
            g_input_stream_close(G_INPUT_STREAM(stream), NULL, NULL);
            g_object_unref(stream);
        }
    }
    if (widgets->installation != NULL && widgets->has_selected_device &&
        lt48_installation_resolve_device(
            widgets->installation, widgets->selected_device_index,
            &spec) == LT48_OK)
        target_size = spec.capacity_bytes;
    else if (widgets->device_buffer != NULL &&
             lt48_buffer_size(widgets->device_buffer) <= UINT32_MAX)
        target_size = (uint32_t)lt48_buffer_size(widgets->device_buffer);
    else if (format == LT48_IMAGE_FORMAT_RAW_BINARY) {
        GFileInfo *info = g_file_query_info(
            file, G_FILE_ATTRIBUTE_STANDARD_SIZE,
            G_FILE_QUERY_INFO_NONE, NULL, NULL);
        if (info != NULL) {
            goffset file_size = g_file_info_get_size(info);
            if (file_size > 0 && file_size <= UINT32_MAX)
                target_size = (uint32_t)file_size;
            g_object_unref(info);
        }
    }
    gtk_editable_set_text(GTK_EDITABLE(widgets->load_image_path), path);
    gtk_editable_set_text(GTK_EDITABLE(widgets->load_image_target), "0");
    gtk_drop_down_set_selected(
        GTK_DROP_DOWN(widgets->load_image_format), format);
    if (target_size != 0u) {
        g_snprintf(size_text, sizeof(size_text), "%u", target_size);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->load_image_size), size_text);
        gtk_label_set_text(GTK_LABEL(widgets->load_image_status),
            widgets->has_selected_device ?
            "Target size inherited from the selected device definition." :
            "Review the inferred target size, format and address mapping.");
    } else {
        gtk_editable_set_text(GTK_EDITABLE(widgets->load_image_size), "");
        gtk_label_set_text(GTK_LABEL(widgets->load_image_status),
            "Enter the target device-buffer size before loading this image.");
    }
    gtk_window_present(widgets->load_image_window);
    g_free(lower);
    g_free(path);
}

static void toolbar_load_selected(
    GObject *source, GAsyncResult *result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(
        GTK_FILE_DIALOG(source), result, &error);
    if (file != NULL) {
        if (selected_fuse_editor_available(widgets))
            open_fuse_context_action(
                widgets, FUSE_CONTEXT_LOAD_JEDEC, file);
        else
            present_load_image_file(widgets, file);
        g_object_unref(file);
    } else if (error != NULL &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_DISMISSED) &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_CANCELLED)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), error->message);
    }
    g_clear_error(&error);
}

static void load_dialog_set_initial_file(
    GtkFileDialog *dialog, app_widgets *widgets
) {
    const char *candidates[3] = {NULL, NULL, NULL};
    size_t candidate_count = 0u;
    if (dialog == NULL || widgets == NULL) return;
    if (widgets->footer_file != NULL) {
        const char *path = gtk_label_get_text(
            GTK_LABEL(widgets->footer_file));
        if (path != NULL && path[0] != '\0' &&
            g_ascii_strcasecmp(path, "No file") != 0 &&
            g_ascii_strcasecmp(path, "No File") != 0)
            candidates[candidate_count++] = path;
    }
    if (widgets->load_image_path != NULL) {
        const char *path = gtk_editable_get_text(
            GTK_EDITABLE(widgets->load_image_path));
        if (path != NULL && path[0] != '\0')
            candidates[candidate_count++] = path;
    }
    if (widgets->read_output_entry != NULL) {
        const char *path = gtk_editable_get_text(
            GTK_EDITABLE(widgets->read_output_entry));
        if (path != NULL && path[0] != '\0')
            candidates[candidate_count++] = path;
    }
    for (size_t index = 0u; index < candidate_count; ++index) {
        char *canonical = g_canonicalize_filename(candidates[index], NULL);
        GFile *file = g_file_new_for_path(canonical);
        if (g_file_query_file_type(
                file, G_FILE_QUERY_INFO_NONE, NULL) ==
            G_FILE_TYPE_REGULAR) {
            gtk_file_dialog_set_initial_file(dialog, file);
            g_object_unref(file);
            g_free(canonical);
            return;
        }
        g_object_unref(file);
        g_free(canonical);
    }
}

static void toolbar_load_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    (void)button;
    gtk_file_dialog_set_title(
        dialog, selected_fuse_editor_available(widgets) ?
            "Load JEDEC fuse map" : "Load image into buffer");
    load_dialog_set_initial_file(dialog, widgets);
    gtk_file_dialog_open(
        dialog, widgets->window, NULL, toolbar_load_selected, widgets);
    g_object_unref(dialog);
}

static void toolbar_save_selected(
    GObject *source, GAsyncResult *result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_save_finish(
        GTK_FILE_DIALOG(source), result, &error);
    if (file != NULL) {
        if (selected_fuse_editor_available(widgets)) {
            open_fuse_context_action(
                widgets, FUSE_CONTEXT_SAVE_JEDEC, file);
            g_object_unref(file);
            g_clear_error(&error);
            return;
        }
        char *path = g_file_get_path(file);
        if (path != NULL) {
            char *lower = g_ascii_strdown(path, -1);
            guint format = LT48_IMAGE_FORMAT_RAW_BINARY;
            char length[32];
            if (g_str_has_suffix(lower, ".hex") ||
                g_str_has_suffix(lower, ".ihx"))
                format = LT48_IMAGE_FORMAT_INTEL_HEX;
            else if (g_str_has_suffix(lower, ".s19") ||
                     g_str_has_suffix(lower, ".s28") ||
                     g_str_has_suffix(lower, ".s37") ||
                     g_str_has_suffix(lower, ".srec") ||
                     g_str_has_suffix(lower, ".mot"))
                format = LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD;
            else if (g_str_has_suffix(lower, ".shx") ||
                     g_str_has_suffix(lower, ".hexstr"))
                format = LT48_IMAGE_FORMAT_STRAIGHT_HEX;
            else if (g_str_has_suffix(lower, ".mos"))
                format = LT48_IMAGE_FORMAT_MOS_TECHNOLOGY;
            else if (g_str_has_suffix(lower, ".tek") ||
                     g_str_has_suffix(lower, ".tekhex"))
                format = LT48_IMAGE_FORMAT_TEKTRONIX_HEX;
            else if (g_str_has_suffix(lower, ".abs") ||
                     g_str_has_suffix(lower, ".hpabs"))
                format = LT48_IMAGE_FORMAT_HP64000_ABSOLUTE;
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->save_image_path), path);
            gtk_drop_down_set_selected(
                GTK_DROP_DOWN(widgets->save_image_format), format);
            gtk_drop_down_set_selected(
                GTK_DROP_DOWN(widgets->save_image_byte_mode),
                LT48_IMAGE_BYTES_NORMAL);
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->save_image_start), "0");
            g_snprintf(length, sizeof(length), "%zu",
                widgets->device_buffer == NULL ? 0u :
                    lt48_buffer_size(widgets->device_buffer));
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->save_image_length), length);
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->save_image_base), "0");
            gtk_label_set_text(GTK_LABEL(widgets->save_image_status),
                "Review the output format, source range and byte mode.");
            gtk_window_present(widgets->save_image_window);
            g_free(lower);
            g_free(path);
        }
        g_object_unref(file);
    } else if (error != NULL &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_DISMISSED) &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_CANCELLED)) {
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), error->message);
    }
    g_clear_error(&error);
}

static void toolbar_save_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    (void)button;
    gtk_file_dialog_set_title(
        dialog, selected_fuse_editor_available(widgets) ?
            "Save JEDEC fuse map" : "Save buffer to an image");
    gtk_file_dialog_set_initial_name(
        dialog, selected_fuse_editor_available(widgets) ?
            "fuse-map.jed" : "buffer.bin");
    gtk_file_dialog_save(
        dialog, widgets->window, NULL, toolbar_save_selected, widgets);
    g_object_unref(dialog);
}

static void project_status(app_widgets *widgets, const char *message) {
    if (widgets->read_preparation != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->read_preparation), message);
}

static void project_store_entry(
    GKeyFile *project, const char *key, GtkWidget *entry
) {
    if (entry != NULL)
        g_key_file_set_string(
            project, "Session", key,
            gtk_editable_get_text(GTK_EDITABLE(entry)));
}

static void project_restore_entry(
    GKeyFile *project, const char *key, GtkWidget *entry
) {
    char *value;
    if (entry == NULL || !g_key_file_has_key(project, "Session", key, NULL))
        return;
    value = g_key_file_get_string(project, "Session", key, NULL);
    if (value != NULL) {
        gtk_editable_set_text(GTK_EDITABLE(entry), value);
        g_free(value);
    }
}

static void project_store_operation_options(
    app_widgets *widgets, GKeyFile *project
) {
    lt48_property_model *model;
    size_t page_index;
    size_t count = 0u;
    if (!widgets->has_selected_device ||
        widgets->operation_option_document == NULL ||
        widgets->operation_option_device_index !=
            widgets->selected_device_index)
        return;
    model = lt48_property_document_model(
        widgets->operation_option_document);
    if (lt48_property_model_validate(model, NULL) != LT48_OK) return;
    for (page_index = 0u; page_index < model->page_count; ++page_index)
        count += model->pages[page_index].control_count;
    g_key_file_set_integer(
        project, "Device Operation Option", "format-version", 1);
    g_key_file_set_uint64(
        project, "Device Operation Option", "control-count", count);
    for (page_index = 0u; page_index < model->page_count; ++page_index) {
        const lt48_property_page *page = &model->pages[page_index];
        size_t control_index;
        for (control_index = 0u; control_index < page->control_count;
             ++control_index) {
            const lt48_property_control *control =
                &page->controls[control_index];
            g_key_file_set_uint64(
                project, "Device Operation Option", control->key,
                control->value);
        }
    }
}

static gboolean project_load_operation_options(
    app_widgets *widgets, GKeyFile *project,
    const char *vendor, const char *device, GError **error
) {
    gchar **keys;
    gsize key_count = 0u;
    guint64 declared_count;
    gint version;
    size_t value_count = 0u;
    size_t index;
    GHashTable *values;
    clear_pending_project_operation_options(widgets);
    if (!g_key_file_has_group(project, "Device Operation Option"))
        return TRUE;
    if (vendor == NULL || device == NULL) {
        g_set_error(
            error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
            "Project has Device Operation Option values without a saved device");
        return FALSE;
    }
    version = g_key_file_get_integer(
        project, "Device Operation Option", "format-version", error);
    if (error != NULL && *error != NULL) return FALSE;
    if (version != 1) {
        g_set_error(
            error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
            "Unsupported Device Operation Option project format %d", version);
        return FALSE;
    }
    declared_count = g_key_file_get_uint64(
        project, "Device Operation Option", "control-count", error);
    if (error != NULL && *error != NULL) return FALSE;
    keys = g_key_file_get_keys(
        project, "Device Operation Option", &key_count, error);
    if (keys == NULL || (error != NULL && *error != NULL)) {
        g_strfreev(keys);
        return FALSE;
    }
    values = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free);
    for (index = 0u; index < key_count; ++index) {
        guint64 parsed;
        guint32 *stored;
        if (g_strcmp0(keys[index], "format-version") == 0 ||
            g_strcmp0(keys[index], "control-count") == 0)
            continue;
        parsed = g_key_file_get_uint64(
            project, "Device Operation Option", keys[index], error);
        if ((error != NULL && *error != NULL) || parsed > UINT32_MAX) {
            if (error != NULL && *error == NULL)
                g_set_error(
                    error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                    "Device Operation Option value exceeds 32 bits");
            g_hash_table_unref(values);
            g_strfreev(keys);
            return FALSE;
        }
        stored = g_new(guint32, 1);
        *stored = (guint32)parsed;
        g_hash_table_insert(values, g_strdup(keys[index]), stored);
        ++value_count;
    }
    g_strfreev(keys);
    if (declared_count != value_count) {
        g_set_error(
            error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
            "Device Operation Option control count is inconsistent");
        g_hash_table_unref(values);
        return FALSE;
    }
    widgets->pending_project_option_values = values;
    widgets->pending_project_option_vendor = g_strdup(vendor);
    widgets->pending_project_option_device = g_strdup(device);
    return TRUE;
}

static void project_clear_resident_buffer(
    app_widgets *widgets, const char *status
) {
    buffer_edit_discard_undo(widgets);
    lt48_buffer_close(widgets->device_buffer);
    widgets->device_buffer = NULL;
    ++widgets->buffer_revision;
    invalidate_operation_preparation(widgets, 1);
    if (widgets->toolbar_save_button != NULL)
        gtk_widget_set_sensitive(widgets->toolbar_save_button, FALSE);
    if (widgets->toolbar_edit_button != NULL)
        gtk_widget_set_sensitive(widgets->toolbar_edit_button, FALSE);
    if (widgets->window != NULL) {
        static const char *const actions[] = {"save", "edit"};
        for (size_t index = 0u; index < G_N_ELEMENTS(actions); ++index) {
            GAction *action = g_action_map_lookup_action(
                G_ACTION_MAP(widgets->window), actions[index]);
            if (action != NULL)
                g_simple_action_set_enabled(G_SIMPLE_ACTION(action), FALSE);
        }
    }
    if (widgets->buffer_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->buffer_status), status);
    if (widgets->footer_checksum != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->footer_checksum), "—");
}

static gboolean project_store_buffer(
    app_widgets *widgets, GKeyFile *project, GError **error
) {
    guint policy = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->project_buffer_policy));
    size_t size;
    const unsigned char *data;
    unsigned char digest[LT48_SHA256_SIZE];
    char digest_text[LT48_SHA256_SIZE * 2u + 1u];
    if (policy == 2u || widgets->device_buffer == NULL) {
        g_key_file_set_string(project, "Buffer", "policy", "settings-only");
        return TRUE;
    }
    size = lt48_buffer_size(widgets->device_buffer);
    data = lt48_buffer_data(widgets->device_buffer);
    if (policy == 1u) {
        const char *source = widgets->footer_file == NULL ? NULL :
            gtk_label_get_text(GTK_LABEL(widgets->footer_file));
        if (source == NULL || source[0] == '\0' ||
            g_strcmp0(source, "No File") == 0 ||
            !g_file_test(source, G_FILE_TEST_IS_REGULAR)) {
            g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                        "The resident buffer has no existing source file to reference");
            return FALSE;
        }
        g_key_file_set_string(project, "Buffer", "policy", "reference");
        g_key_file_set_string(project, "Buffer", "source-path", source);
        if (lt48_buffer_known_size(widgets->device_buffer) == size) {
            lt48_sha256(data, size, digest);
            digest_hex(digest, LT48_SHA256_SIZE, digest_text);
            g_key_file_set_string(project, "Buffer", "sha256", digest_text);
            g_key_file_set_uint64(project, "Buffer", "size", size);
        }
        return TRUE;
    }
    if (lt48_buffer_known_size(widgets->device_buffer) != size) {
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                    "A sparse resident buffer cannot be embedded; choose a "
                    "source reference or settings only");
        return FALSE;
    }
    {
        char *encoded = g_base64_encode(data, size);
        lt48_sha256(data, size, digest);
        digest_hex(digest, LT48_SHA256_SIZE, digest_text);
        g_key_file_set_string(project, "Buffer", "policy", "embedded");
        g_key_file_set_uint64(project, "Buffer", "size", size);
        g_key_file_set_string(project, "Buffer", "sha256", digest_text);
        g_key_file_set_string(project, "Buffer", "data-base64", encoded);
        g_free(encoded);
    }
    return TRUE;
}

static gboolean project_restore_buffer(
    app_widgets *widgets, GKeyFile *project, GError **error
) {
    enum { PROJECT_BUFFER_MAX_SIZE = 256 * 1024 * 1024 };
    char *policy = g_key_file_get_string(project, "Buffer", "policy", NULL);
    if (policy == NULL || g_strcmp0(policy, "settings-only") == 0) {
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->project_buffer_policy), 2u);
        gtk_label_set_text(
            GTK_LABEL(widgets->project_buffer_status),
            "Project settings only; no resident buffer was restored.");
        project_clear_resident_buffer(
            widgets, "Project contains settings only; no resident buffer loaded");
        if (widgets->footer_file != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->footer_file), "No File");
        g_free(policy);
        return TRUE;
    }
    if (g_strcmp0(policy, "reference") == 0) {
        char *source = g_key_file_get_string(
            project, "Buffer", "source-path", error);
        if (source == NULL || (error != NULL && *error != NULL)) {
            g_free(policy);
            g_free(source);
            return FALSE;
        }
        project_clear_resident_buffer(
            widgets,
            "Project references an external image; use Load to review and "
            "import it before programming");
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->project_buffer_policy), 1u);
        gtk_label_set_text(
            GTK_LABEL(widgets->project_buffer_status),
            "External image reference restored; load and review it explicitly.");
        if (widgets->footer_file != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->footer_file), source);
        if (widgets->buffer_input_entry != NULL)
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->buffer_input_entry), source);
        g_free(source);
        g_free(policy);
        return TRUE;
    }
    if (g_strcmp0(policy, "embedded") == 0) {
        guint64 declared = g_key_file_get_uint64(
            project, "Buffer", "size", error);
        char *expected = NULL;
        char *encoded = NULL;
        gsize decoded_size = 0u;
        unsigned char *decoded = NULL;
        unsigned char digest[LT48_SHA256_SIZE];
        char actual[LT48_SHA256_SIZE * 2u + 1u];
        lt48_buffer *restored = NULL;
        lt48_status status;
        if (error != NULL && *error != NULL) goto embedded_fail;
        if (declared == 0u || declared > PROJECT_BUFFER_MAX_SIZE) {
            g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                        "Embedded project buffer size is invalid or exceeds 256 MiB");
            goto embedded_fail;
        }
        expected = g_key_file_get_string(project, "Buffer", "sha256", error);
        if (expected == NULL || (error != NULL && *error != NULL) ||
            strlen(expected) != LT48_SHA256_SIZE * 2u) goto embedded_fail;
        encoded = g_key_file_get_string(
            project, "Buffer", "data-base64", error);
        if (encoded == NULL || (error != NULL && *error != NULL))
            goto embedded_fail;
        decoded = g_base64_decode(encoded, &decoded_size);
        if (decoded == NULL || decoded_size != (gsize)declared) {
            g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                        "Embedded project buffer length does not match its declaration");
            goto embedded_fail;
        }
        lt48_sha256(decoded, decoded_size, digest);
        digest_hex(digest, LT48_SHA256_SIZE, actual);
        if (g_ascii_strcasecmp(expected, actual) != 0) {
            g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                        "Embedded project buffer SHA-256 does not match");
            goto embedded_fail;
        }
        status = lt48_buffer_create(decoded_size, 0u, &restored);
        if (status == LT48_OK)
            status = lt48_buffer_write(restored, 0u, decoded, decoded_size);
        if (status != LT48_OK) {
            lt48_buffer_close(restored);
            g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                        "Could not restore embedded buffer: %s",
                        lt48_status_string(status));
            goto embedded_fail;
        }
        project_clear_resident_buffer(widgets, "Restoring embedded buffer…");
        widgets->device_buffer = restored;
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->project_buffer_policy), 0u);
        gtk_label_set_text(
            GTK_LABEL(widgets->project_buffer_status),
            "Verified embedded resident buffer restored.");
        gtk_editable_set_text(GTK_EDITABLE(widgets->buffer_start_entry), "0");
        {
            char length[32];
            g_snprintf(length, sizeof(length), "%zu", decoded_size);
            gtk_editable_set_text(
                GTK_EDITABLE(widgets->buffer_length_entry), length);
        }
        buffer_show_range(
            widgets, "Restored verified embedded project buffer", 0u,
            (uint32_t)decoded_size);
        if (widgets->footer_file != NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->footer_file), "Embedded project buffer");
        g_free(decoded);
        g_free(encoded);
        g_free(expected);
        g_free(policy);
        return TRUE;
embedded_fail:
        g_free(decoded);
        g_free(encoded);
        g_free(expected);
        g_free(policy);
        return FALSE;
    }
    g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                "Unknown project buffer policy: %s", policy);
    g_free(policy);
    return FALSE;
}

static gboolean project_write_file(
    app_widgets *widgets, const char *path, GError **error
) {
    GKeyFile *project = g_key_file_new();
    const char *database = gtk_editable_get_text(
        GTK_EDITABLE(widgets->installation_entry));
    gboolean saved;
    if (!widgets->database_manifest_verified ||
        widgets->database_manifest_hex[0] == '\0') {
        g_set_error(
            error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
            "Verify the active device database before saving a project");
        g_key_file_unref(project);
        return FALSE;
    }
    g_key_file_set_integer(project, "Project", "format-version", 1);
    g_key_file_set_string(project, "Project", "database-path", database);
    g_key_file_set_string(project, "Project", "database-manifest-sha256",
                          widgets->database_manifest_hex);
    if (widgets->project_description != NULL) {
        GtkTextBuffer *description = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(widgets->project_description));
        GtkTextIter start;
        GtkTextIter end;
        char *text;
        gtk_text_buffer_get_bounds(description, &start, &end);
        text = gtk_text_buffer_get_text(description, &start, &end, FALSE);
        g_key_file_set_string(project, "Project", "description", text);
        g_free(text);
    }
    if (widgets->installation != NULL && widgets->has_selected_device) {
        lt48_catalogue_entry entry;
        if (lt48_installation_get_device(
                widgets->installation, widgets->selected_device_index,
                &entry) == LT48_OK) {
            char *name = g_strndup(
                (const char *)entry.device_name, entry.device_name_length);
            g_key_file_set_string(
                project, "Device", "vendor", entry.vendor);
            g_key_file_set_string(project, "Device", "name", name);
            g_free(name);
        }
    }
    project_store_operation_options(widgets, project);
    project_store_entry(project, "buffer-input", widgets->buffer_input_entry);
    project_store_entry(project, "buffer-output", widgets->buffer_output_entry);
    project_store_entry(project, "operation-start", widgets->read_start_entry);
    project_store_entry(project, "operation-length", widgets->read_length_entry);
    project_store_entry(project, "timeout-ms", widgets->read_timeout_entry);
    project_store_entry(project, "input-image", widgets->verify_image_entry);
    project_store_entry(project, "plan-report", widgets->operation_report_entry);
    project_store_entry(
        project, "execution-report", widgets->execution_report_entry);
    g_key_file_set_integer(
        project, "Session", "operation",
        (gint)gtk_drop_down_get_selected(
            GTK_DROP_DOWN(widgets->operation_dropdown)));
    g_key_file_set_integer(
        project, "Session", "verify-margin",
        (gint)gtk_drop_down_get_selected(
            GTK_DROP_DOWN(widgets->verify_margin_dropdown)));
    g_key_file_set_integer(
        project, "Session", "verify-margin-type",
        (gint)gtk_drop_down_get_selected(
            GTK_DROP_DOWN(widgets->verify_margin_type_dropdown)));
    g_key_file_set_uint64(project, "Statistics", "current-count",
                          widgets->statistic_current_count);
    g_key_file_set_uint64(project, "Statistics", "current-failure",
                          widgets->statistic_current_failure);
    g_key_file_set_uint64(project, "Statistics", "target-count",
                          widgets->statistic_target_count);
    g_key_file_set_uint64(project, "Statistics", "max-failure",
                          widgets->statistic_max_failure);
    g_key_file_set_boolean(project, "Statistics", "enabled",
                           widgets->statistic_enabled);
    g_key_file_set_boolean(project, "Statistics", "alarm-enabled",
                           widgets->statistic_alarm_enabled);
    if (!project_store_buffer(widgets, project, error)) {
        g_key_file_unref(project);
        return FALSE;
    }
    saved = g_key_file_save_to_file(project, path, error);
    g_key_file_unref(project);
    return saved;
}

static void project_save_selected(
    GObject *source, GAsyncResult *result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_save_finish(
        GTK_FILE_DIALOG(source), result, &error);
    if (file != NULL) {
        char *path = g_file_get_path(file);
        if (path != NULL) {
            size_t buffer_size = widgets->device_buffer == NULL ? 0u :
                lt48_buffer_size(widgets->device_buffer);
            gtk_editable_set_text(GTK_EDITABLE(widgets->project_path), path);
            if (widgets->device_buffer != NULL &&
                lt48_buffer_known_size(widgets->device_buffer) == buffer_size) {
                gtk_drop_down_set_selected(
                    GTK_DROP_DOWN(widgets->project_buffer_policy), 0u);
                gtk_label_set_text(
                    GTK_LABEL(widgets->project_buffer_status),
                    "The complete known resident buffer can be embedded and "
                    "verified by SHA-256 when this project is loaded.");
            } else {
                gtk_drop_down_set_selected(
                    GTK_DROP_DOWN(widgets->project_buffer_policy), 2u);
                gtk_label_set_text(
                    GTK_LABEL(widgets->project_buffer_status),
                    widgets->device_buffer == NULL ?
                    "No resident buffer is present; settings only will be saved." :
                    "The resident buffer is sparse and cannot be embedded; "
                    "choose a valid source reference or settings only.");
            }
            gtk_label_set_text(
                GTK_LABEL(widgets->project_description_status),
                "Add an optional description, then save the project.");
            gtk_window_present(widgets->project_description_window);
            g_free(path);
        }
        g_object_unref(file);
    }
    if (error != NULL &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED))
        project_status(widgets, error->message);
    g_clear_error(&error);
}

static void project_description_save(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    const char *path = gtk_editable_get_text(
        GTK_EDITABLE(widgets->project_path));
    GError *error = NULL;
    (void)button;
    if (path == NULL || path[0] == '\0') {
        gtk_label_set_text(
            GTK_LABEL(widgets->project_description_status),
            "Choose a project output file first.");
        return;
    }
    if (project_write_file(widgets, path, &error)) {
        char *message = g_strdup_printf("Project saved: %s", path);
        gtk_label_set_text(
            GTK_LABEL(widgets->project_description_status), message);
        project_status(widgets, message);
        gtk_widget_set_visible(
            GTK_WIDGET(widgets->project_description_window), FALSE);
        g_free(message);
    } else {
        gtk_label_set_text(
            GTK_LABEL(widgets->project_description_status),
            error == NULL ? "Project save failed" : error->message);
        if (error != NULL) project_status(widgets, error->message);
    }
    g_clear_error(&error);
}

static void project_buffer_policy_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    app_widgets *widgets = user_data;
    guint policy = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->project_buffer_policy));
    size_t size = widgets->device_buffer == NULL ? 0u :
        lt48_buffer_size(widgets->device_buffer);
    (void)object;
    (void)parameter;
    if (policy == 0u)
        gtk_label_set_text(
            GTK_LABEL(widgets->project_buffer_status),
            widgets->device_buffer == NULL ?
            "No resident buffer is available to embed." :
            lt48_buffer_known_size(widgets->device_buffer) == size ?
            "The complete known resident buffer will be embedded with its "
            "size and SHA-256." :
            "The resident buffer is sparse and cannot be embedded; choose a "
            "source reference or settings only.");
    else if (policy == 1u)
        gtk_label_set_text(
            GTK_LABEL(widgets->project_buffer_status),
            "The current source-file path and buffer digest are retained; "
            "the image must be loaded and reviewed explicitly after restore.");
    else
        gtk_label_set_text(
            GTK_LABEL(widgets->project_buffer_status),
            "Device, database and operation settings are saved without buffer data.");
}

static GtkWindow *build_project_description_window(app_widgets *widgets) {
    static const char *buffer_policies[] = {
        "Embed complete resident buffer",
        "Reference source image (load explicitly on restore)",
        "Project settings only",
        NULL
    };
    GtkWidget *window = gtk_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *save = gtk_button_new_with_label("Save Project");
    GtkWidget *cancel = gtk_button_new_with_label("Cancel");
    widgets->project_path = gtk_entry_new();
    widgets->project_description = gtk_text_view_new();
    widgets->project_buffer_policy =
        gtk_drop_down_new_from_strings(buffer_policies);
    widgets->project_buffer_status = gtk_label_new(
        "No resident buffer is present; settings only will be saved.");
    widgets->project_description_status = gtk_label_new(
        "Add an optional project description.");
    gtk_window_set_title(GTK_WINDOW(window), "Project Description");
    gtk_window_set_transient_for(GTK_WINDOW(window), widgets->window);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_hide_on_close(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 660, 440);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_editable_set_editable(GTK_EDITABLE(widgets->project_path), FALSE);
    gtk_widget_set_hexpand(widgets->project_path, TRUE);
    gtk_text_view_set_wrap_mode(
        GTK_TEXT_VIEW(widgets->project_description), GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(scroll), widgets->project_description);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll),
                                                180);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_label_set_xalign(
        GTK_LABEL(widgets->project_description_status), 0.0F);
    gtk_label_set_wrap(
        GTK_LABEL(widgets->project_description_status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(widgets->project_buffer_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->project_buffer_status), TRUE);
    gtk_widget_add_css_class(widgets->project_buffer_status, "dim-label");
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(actions), cancel);
    gtk_box_append(GTK_BOX(actions), save);
    gtk_widget_add_css_class(save, "suggested-action");
    gtk_box_append(GTK_BOX(box), gtk_label_new("Project file:"));
    gtk_box_append(GTK_BOX(box), widgets->project_path);
    gtk_box_append(GTK_BOX(box), gtk_label_new("Description:"));
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_box_append(GTK_BOX(box), gtk_label_new("Resident buffer:"));
    gtk_box_append(GTK_BOX(box), widgets->project_buffer_policy);
    gtk_box_append(GTK_BOX(box), widgets->project_buffer_status);
    gtk_box_append(GTK_BOX(box), widgets->project_description_status);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect(save, "clicked",
                     G_CALLBACK(project_description_save), widgets);
    g_signal_connect(widgets->project_buffer_policy, "notify::selected",
                     G_CALLBACK(project_buffer_policy_changed), widgets);
    g_signal_connect_swapped(cancel, "clicked",
                             G_CALLBACK(gtk_window_close), window);
    return GTK_WINDOW(window);
}

static void project_save_dialog(app_widgets *widgets) {
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Save LabTool project");
    gtk_file_dialog_set_initial_name(dialog, "labtool-project.lt48project");
    gtk_file_dialog_save(
        dialog, widgets->window, NULL, project_save_selected, widgets);
    g_object_unref(dialog);
}

static gboolean project_load_file(
    app_widgets *widgets, const char *path, GError **error
) {
    GKeyFile *project = g_key_file_new();
    char *database = NULL;
    char *manifest = NULL;
    char *vendor = NULL;
    char *device = NULL;
    char *description = NULL;
    gint version;
    gboolean loaded = FALSE;
    if (!g_key_file_load_from_file(project, path, G_KEY_FILE_NONE, error))
        goto done;
    version = g_key_file_get_integer(
        project, "Project", "format-version", error);
    if (error != NULL && *error != NULL) goto done;
    if (version != 1) {
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                    "Unsupported LabTool project format version %d", version);
        goto done;
    }
    description = g_key_file_get_string(
        project, "Project", "description", NULL);
    if (widgets->project_description != NULL) {
        GtkTextBuffer *description_buffer = gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(widgets->project_description));
        gtk_text_buffer_set_text(
            description_buffer, description == NULL ? "" : description, -1);
    }
    if (description != NULL && description[0] != '\0') {
        char *description_message = g_strdup_printf(
            "Project description: %s", description);
        project_status(widgets, description_message);
        g_free(description_message);
    }
    database = g_key_file_get_string(
        project, "Project", "database-path", error);
    if (database == NULL || (error != NULL && *error != NULL)) goto done;
    if (!g_file_test(database, G_FILE_TEST_IS_DIR)) {
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_NOENT,
                    "Project database directory is unavailable: %s", database);
        goto done;
    }
    manifest = g_key_file_get_string(
        project, "Project", "database-manifest-sha256", error);
    if (manifest == NULL || (error != NULL && *error != NULL)) goto done;
    if (strlen(manifest) != LT48_SHA256_SIZE * 2u) {
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                    "Project database manifest hash is invalid");
        goto done;
    }
    for (size_t index = 0u; manifest[index] != '\0'; ++index) {
        if (!g_ascii_isxdigit(manifest[index])) {
            g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                        "Project database manifest hash is invalid");
            goto done;
        }
    }
    if (!project_restore_buffer(widgets, project, error)) goto done;
    g_clear_pointer(&widgets->pending_project_query, g_free);
    clear_pending_project_device(widgets);
    widgets->expected_project_manifest_hex[0] = '\0';
    gtk_editable_set_text(GTK_EDITABLE(widgets->installation_entry), database);
    if (!database_registry_contains(widgets, database)) {
        char *canonical = g_canonicalize_filename(database, NULL);
        if (!database_registry_contains(widgets, canonical))
            g_ptr_array_add(widgets->database_paths, canonical);
        else
            g_free(canonical);
    }
    database_registry_save(widgets);
    database_manager_rebuild(widgets);
    project_restore_entry(project, "buffer-input", widgets->buffer_input_entry);
    project_restore_entry(project, "buffer-output", widgets->buffer_output_entry);
    project_restore_entry(project, "operation-start", widgets->read_start_entry);
    project_restore_entry(project, "operation-length", widgets->read_length_entry);
    project_restore_entry(project, "timeout-ms", widgets->read_timeout_entry);
    project_restore_entry(project, "input-image", widgets->verify_image_entry);
    project_restore_entry(project, "plan-report", widgets->operation_report_entry);
    project_restore_entry(
        project, "execution-report", widgets->execution_report_entry);
    if (g_key_file_has_key(project, "Session", "operation", NULL))
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->operation_dropdown),
            (guint)g_key_file_get_integer(project, "Session", "operation", NULL));
    if (g_key_file_has_key(project, "Session", "verify-margin", NULL))
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->verify_margin_dropdown),
            (guint)g_key_file_get_integer(
                project, "Session", "verify-margin", NULL));
    if (g_key_file_has_key(project, "Session", "verify-margin-type", NULL))
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->verify_margin_type_dropdown),
            (guint)g_key_file_get_integer(
                project, "Session", "verify-margin-type", NULL));
    if (g_key_file_has_key(project, "Statistics", "current-count", NULL))
        widgets->statistic_current_count = g_key_file_get_uint64(
            project, "Statistics", "current-count", NULL);
    if (g_key_file_has_key(project, "Statistics", "current-failure", NULL))
        widgets->statistic_current_failure = g_key_file_get_uint64(
            project, "Statistics", "current-failure", NULL);
    if (g_key_file_has_key(project, "Statistics", "target-count", NULL))
        widgets->statistic_target_count = (uint32_t)MIN(
            g_key_file_get_uint64(
                project, "Statistics", "target-count", NULL), UINT32_MAX);
    if (g_key_file_has_key(project, "Statistics", "max-failure", NULL))
        widgets->statistic_max_failure = (uint32_t)MIN(
            g_key_file_get_uint64(
                project, "Statistics", "max-failure", NULL), UINT32_MAX);
    if (g_key_file_has_key(project, "Statistics", "enabled", NULL))
        widgets->statistic_enabled = g_key_file_get_boolean(
            project, "Statistics", "enabled", NULL);
    if (g_key_file_has_key(project, "Statistics", "alarm-enabled", NULL))
        widgets->statistic_alarm_enabled = g_key_file_get_boolean(
            project, "Statistics", "alarm-enabled", NULL);
    statistics_update_labels(widgets);
    vendor = g_key_file_get_string(project, "Device", "vendor", NULL);
    device = g_key_file_get_string(project, "Device", "name", NULL);
    if (!project_load_operation_options(
            widgets, project, vendor, device, error))
        goto done;
    if (vendor != NULL && device != NULL) {
        widgets->pending_project_device_vendor = g_strdup(vendor);
        widgets->pending_project_device_name = g_strdup(device);
        widgets->pending_project_query =
            g_strdup_printf("%s:%s", vendor, device);
        g_strlcpy(widgets->expected_project_manifest_hex, manifest,
                  sizeof(widgets->expected_project_manifest_hex));
        verify_installation(NULL, widgets);
        project_status(
            widgets,
            "Project loaded; verifying its database manifest before resolving the device");
    } else {
        g_strlcpy(widgets->expected_project_manifest_hex, manifest,
                  sizeof(widgets->expected_project_manifest_hex));
        verify_installation(NULL, widgets);
        project_status(
            widgets,
            "Project loaded; verifying its database manifest before device selection");
    }
    gtk_window_present(widgets->device_select_window);
    loaded = TRUE;
done:
    g_free(description);
    g_free(device);
    g_free(vendor);
    g_free(manifest);
    g_free(database);
    g_key_file_unref(project);
    return loaded;
}

static void project_load_selected(
    GObject *source, GAsyncResult *result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(
        GTK_FILE_DIALOG(source), result, &error);
    if (file != NULL) {
        char *path = g_file_get_path(file);
        if (path != NULL)
            (void)project_load_file(widgets, path, &error);
        g_free(path);
        g_object_unref(file);
    }
    if (error != NULL &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED))
        project_status(widgets, error->message);
    g_clear_error(&error);
}

static void project_load_dialog(app_widgets *widgets) {
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Load LabTool project");
    gtk_file_dialog_open(
        dialog, widgets->window, NULL, project_load_selected, widgets);
    g_object_unref(dialog);
}

static void hide_auxiliary_window(GtkButton *button, gpointer user_data) {
    GtkWindow *window = user_data;
    (void)button;
    gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
}

static char *database_registry_file(void) {
    char *directory = g_build_filename(
        g_get_user_config_dir(), "labtool48uxp", NULL);
    char *path;
    if (g_mkdir_with_parents(directory, 0700) != 0) {
        g_free(directory);
        return NULL;
    }
    path = g_build_filename(directory, "databases.ini", NULL);
    g_free(directory);
    return path;
}

static gboolean database_registry_contains(
    const app_widgets *widgets, const char *path
) {
    if (widgets->database_paths == NULL || path == NULL) return FALSE;
    for (guint index = 0u; index < widgets->database_paths->len; ++index) {
        const char *known = g_ptr_array_index(widgets->database_paths, index);
        if (g_strcmp0(known, path) == 0) return TRUE;
    }
    return FALSE;
}

static char *database_registry_load(app_widgets *widgets) {
    GKeyFile *key_file = g_key_file_new();
    char *registry = database_registry_file();
    char *active = NULL;
    gsize count = 0u;
    char **paths = NULL;
    widgets->database_paths = g_ptr_array_new_with_free_func(g_free);
    if (registry != NULL &&
        g_key_file_load_from_file(key_file, registry, G_KEY_FILE_NONE, NULL)) {
        paths = g_key_file_get_string_list(
            key_file, "Databases", "paths", &count, NULL);
        active = g_key_file_get_string(
            key_file, "Databases", "active", NULL);
    }
    for (gsize index = 0u; index < count; ++index) {
        char *canonical = g_canonicalize_filename(paths[index], NULL);
        if (!database_registry_contains(widgets, canonical))
            g_ptr_array_add(widgets->database_paths, canonical);
        else
            g_free(canonical);
    }
    g_strfreev(paths);
    g_key_file_unref(key_file);
    g_free(registry);
    return active;
}

static void database_registry_save(app_widgets *widgets) {
    GKeyFile *key_file;
    char *registry;
    const char *active;
    GError *error = NULL;
    if (widgets->database_paths == NULL) return;
    registry = database_registry_file();
    if (registry == NULL) {
        if (widgets->database_manager_status != NULL)
            gtk_label_set_text(GTK_LABEL(widgets->database_manager_status),
                               "Could not create the configuration directory");
        return;
    }
    key_file = g_key_file_new();
    if (widgets->database_paths->len != 0u)
        g_key_file_set_string_list(
            key_file, "Databases", "paths",
            (const char *const *)widgets->database_paths->pdata,
            widgets->database_paths->len);
    active = widgets->installation_entry == NULL ? "" :
        gtk_editable_get_text(GTK_EDITABLE(widgets->installation_entry));
    g_key_file_set_string(key_file, "Databases", "active", active);
    if (!g_key_file_save_to_file(key_file, registry, &error) &&
        widgets->database_manager_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->database_manager_status),
                           error->message);
    g_clear_error(&error);
    g_key_file_unref(key_file);
    g_free(registry);
}

static void database_selector_rebuild(app_widgets *widgets) {
    GtkStringList *model;
    const char *active;
    guint selected = GTK_INVALID_LIST_POSITION;
    if (widgets->database_dropdown == NULL ||
        widgets->database_paths == NULL) return;
    active = widgets->installation_entry == NULL ? NULL :
        gtk_editable_get_text(GTK_EDITABLE(widgets->installation_entry));
    model = gtk_string_list_new(NULL);
    for (guint index = 0u; index < widgets->database_paths->len; ++index) {
        const char *path = g_ptr_array_index(widgets->database_paths, index);
        char *base = g_path_get_basename(path);
        char *parent = g_path_get_dirname(path);
        char *parent_base = g_path_get_basename(parent);
        char *label = g_strdup_printf("%s — %s", base, parent_base);
        gtk_string_list_append(model, label);
        if (g_strcmp0(path, active) == 0) selected = index;
        g_free(label);
        g_free(parent_base);
        g_free(parent);
        g_free(base);
    }
    widgets->database_selector_refreshing = 1;
    gtk_drop_down_set_model(GTK_DROP_DOWN(widgets->database_dropdown),
                            G_LIST_MODEL(model));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(widgets->database_dropdown),
                               selected);
    widgets->database_selector_refreshing = 0;
    g_object_unref(model);
}

static void database_selector_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    app_widgets *widgets = user_data;
    guint selected;
    const char *path;
    (void)object;
    (void)parameter;
    if (widgets->database_selector_refreshing ||
        widgets->database_paths == NULL) return;
    selected = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->database_dropdown));
    if (selected == GTK_INVALID_LIST_POSITION ||
        selected >= widgets->database_paths->len) return;
    path = g_ptr_array_index(widgets->database_paths, selected);
    if (g_strcmp0(path, gtk_editable_get_text(
            GTK_EDITABLE(widgets->installation_entry))) != 0)
        gtk_editable_set_text(GTK_EDITABLE(widgets->installation_entry), path);
    database_registry_save(widgets);
    database_manager_rebuild(widgets);
}

static void database_manager_rebuild(app_widgets *widgets) {
    GtkListBoxRow *active_row = NULL;
    const char *active = widgets->installation_entry == NULL ? NULL :
        gtk_editable_get_text(GTK_EDITABLE(widgets->installation_entry));
    database_selector_rebuild(widgets);
    if (widgets->database_list == NULL) return;
    clear_list(widgets->database_list);
    for (guint index = 0u; index < widgets->database_paths->len; ++index) {
        const char *path = g_ptr_array_index(widgets->database_paths, index);
        char *text = g_strcmp0(path, active) == 0 ?
            g_strdup_printf("Active — %s", path) : g_strdup(path);
        GtkWidget *row = gtk_list_box_row_new();
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), left_label(text));
        g_object_set_data(G_OBJECT(row), "labtool48-database-index",
                          GUINT_TO_POINTER(index + 1u));
        gtk_list_box_append(GTK_LIST_BOX(widgets->database_list), row);
        if (g_strcmp0(path, active) == 0)
            active_row = GTK_LIST_BOX_ROW(row);
        g_free(text);
    }
    if (active_row != NULL)
        gtk_list_box_select_row(
            GTK_LIST_BOX(widgets->database_list), active_row);
}

static void database_manager_use_selected(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GtkListBoxRow *row = gtk_list_box_get_selected_row(
        GTK_LIST_BOX(widgets->database_list));
    guint packed;
    (void)button;
    if (row == NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->database_manager_status),
                           "Select a database first");
        return;
    }
    packed = GPOINTER_TO_UINT(g_object_get_data(
        G_OBJECT(row), "labtool48-database-index"));
    if (packed == 0u || packed > widgets->database_paths->len) return;
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->installation_entry),
        g_ptr_array_index(widgets->database_paths, packed - 1u));
    database_registry_save(widgets);
    database_manager_rebuild(widgets);
    gtk_widget_set_visible(GTK_WIDGET(widgets->database_manager_window), FALSE);
    if (widgets->database_manager_return_to_selection)
        gtk_window_present(widgets->device_select_window);
    else
        gtk_window_present(widgets->window);
}

static void database_manager_remove_selected(
    GtkButton *button, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GtkListBoxRow *row = gtk_list_box_get_selected_row(
        GTK_LIST_BOX(widgets->database_list));
    guint packed;
    const char *active;
    const char *path;
    (void)button;
    if (row == NULL) {
        gtk_label_set_text(GTK_LABEL(widgets->database_manager_status),
                           "Select a database to remove");
        return;
    }
    packed = GPOINTER_TO_UINT(g_object_get_data(
        G_OBJECT(row), "labtool48-database-index"));
    if (packed == 0u || packed > widgets->database_paths->len) return;
    path = g_ptr_array_index(widgets->database_paths, packed - 1u);
    active = gtk_editable_get_text(GTK_EDITABLE(widgets->installation_entry));
    if (g_strcmp0(path, active) == 0) {
        gtk_label_set_text(
            GTK_LABEL(widgets->database_manager_status),
            "The active database cannot be removed; activate another one first");
        return;
    }
    g_ptr_array_remove_index(widgets->database_paths, packed - 1u);
    database_registry_save(widgets);
    database_manager_rebuild(widgets);
    gtk_label_set_text(GTK_LABEL(widgets->database_manager_status),
                       "Database removed from the saved list; no files were deleted");
}

static void database_manager_folder_selected(
    GObject *source, GAsyncResult *result, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    GFile *folder = gtk_file_dialog_select_folder_finish(
        GTK_FILE_DIALOG(source), result, &error);
    if (folder != NULL) {
        char *path = g_file_get_path(folder);
        char *canonical = path == NULL ? NULL :
            g_canonicalize_filename(path, NULL);
        if (canonical != NULL &&
            !database_registry_contains(widgets, canonical)) {
            g_ptr_array_add(widgets->database_paths, canonical);
            database_registry_save(widgets);
            database_manager_rebuild(widgets);
            gtk_label_set_text(
                GTK_LABEL(widgets->database_manager_status),
                "Database directory added; select it and choose Use Selected");
        } else {
            g_free(canonical);
            gtk_label_set_text(GTK_LABEL(widgets->database_manager_status),
                               "That database directory is already listed");
        }
        g_free(path);
        g_object_unref(folder);
    } else if (error != NULL &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_DISMISSED) &&
               !g_error_matches(error, GTK_DIALOG_ERROR,
                                GTK_DIALOG_ERROR_CANCELLED)) {
        gtk_label_set_text(GTK_LABEL(widgets->database_manager_status),
                           error->message);
    }
    g_clear_error(&error);
}

static void database_manager_add(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    (void)button;
    gtk_file_dialog_set_title(dialog, "Add a device database directory");
    gtk_file_dialog_select_folder(
        dialog, widgets->database_manager_window, NULL,
        database_manager_folder_selected, widgets);
    g_object_unref(dialog);
}

static GtkWindow *build_database_manager(app_widgets *widgets) {
    GtkWidget *window = gtk_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *explanation = gtk_label_new(
        "Each device is bound to exactly one database. Search, Auto-ID and "
        "all operation resources remain inside the active database.");
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *action_spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *add = gtk_button_new_with_label("Add Directory…");
    GtkWidget *remove = gtk_button_new_with_label("Remove from List");
    GtkWidget *use = gtk_button_new_with_label("Use Selected");
    GtkWidget *done = gtk_button_new_with_label("Done");
    gtk_window_set_title(GTK_WINDOW(window), "Manage Device Databases");
    gtk_window_set_transient_for(GTK_WINDOW(window), widgets->window);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_hide_on_close(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 420);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_label_set_xalign(GTK_LABEL(explanation), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
    gtk_box_append(GTK_BOX(box), explanation);
    widgets->database_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(widgets->database_list), GTK_SELECTION_SINGLE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),
                                  widgets->database_list);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    widgets->database_manager_status = gtk_label_new(
        "Use Add Directory to register another owned vendor or authored database");
    gtk_label_set_xalign(GTK_LABEL(widgets->database_manager_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->database_manager_status), TRUE);
    gtk_box_append(GTK_BOX(box), widgets->database_manager_status);
    gtk_widget_set_hexpand(actions, TRUE);
    gtk_box_append(GTK_BOX(actions), add);
    gtk_box_append(GTK_BOX(actions), widgets->cache_import_button);
    gtk_box_append(GTK_BOX(actions), remove);
    gtk_box_append(GTK_BOX(actions), use);
    gtk_widget_set_hexpand(action_spacer, TRUE);
    gtk_box_append(GTK_BOX(actions), action_spacer);
    gtk_box_append(GTK_BOX(actions), done);
    gtk_widget_add_css_class(use, "suggested-action");
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect(add, "clicked", G_CALLBACK(database_manager_add), widgets);
    g_signal_connect(remove, "clicked",
                     G_CALLBACK(database_manager_remove_selected), widgets);
    g_signal_connect(use, "clicked",
                     G_CALLBACK(database_manager_use_selected), widgets);
    g_signal_connect(done, "clicked", G_CALLBACK(hide_auxiliary_window), window);
    return GTK_WINDOW(window);
}

static void present_database_manager(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    widgets->database_manager_return_to_selection = button != NULL;
    gtk_window_set_transient_for(
        widgets->database_manager_window,
        widgets->database_manager_return_to_selection ?
            widgets->device_select_window : widgets->window);
    database_manager_rebuild(widgets);
    gtk_window_present(widgets->database_manager_window);
}

static void selection_done_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    GtkListBoxRow *row = gtk_list_box_get_selected_row(
        GTK_LIST_BOX(widgets->device_list));
    size_t pending_index = widgets->pending_device_index;
    (void)button;
    if (!widgets->has_pending_device || row == NULL) {
        gtk_label_set_text(
            GTK_LABEL(widgets->device_status),
            "Select a search or Auto-ID result before accepting");
        return;
    }
    commit_device_row(GTK_LIST_BOX(widgets->device_list), row, widgets);
    if (!widgets->has_selected_device ||
        widgets->selected_device_index != pending_index) {
        gtk_label_set_text(
            GTK_LABEL(widgets->device_status),
            "The selected definition could not be activated");
        return;
    }
    widgets->has_pending_device = 0;
    gtk_list_box_unselect_all(GTK_LIST_BOX(widgets->device_list));
    gtk_widget_set_visible(
        GTK_WIDGET(widgets->device_select_window), FALSE);
}

static void selection_cancel_clicked(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    widgets->has_pending_device = 0;
    gtk_list_box_unselect_all(GTK_LIST_BOX(widgets->device_list));
    if (widgets->selection_accept_button != NULL)
        gtk_widget_set_sensitive(widgets->selection_accept_button, FALSE);
    gtk_widget_set_visible(
        GTK_WIDGET(widgets->device_select_window), FALSE);
}

static void operation_message_changed(
    GObject *object, GParamSpec *parameter, gpointer user_data
) {
    enum { LOG_CHARACTER_LIMIT = 32768 };
    app_widgets *widgets = user_data;
    GtkTextBuffer *buffer;
    GtkTextIter start;
    GtkTextIter end;
    const char *message;
    int characters;
    (void)parameter;
    if (widgets->central_log == NULL) return;
    message = gtk_label_get_text(GTK_LABEL(object));
    if (message == NULL || message[0] == '\0') return;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->central_log));
    characters = gtk_text_buffer_get_char_count(buffer);
    if (characters != 0) {
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_insert(buffer, &end, "\n", 1);
    }
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, message, -1);
    characters = gtk_text_buffer_get_char_count(buffer);
    if (characters > LOG_CHARACTER_LIMIT) {
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_iter_at_offset(
            buffer, &end, characters - LOG_CHARACTER_LIMIT);
        gtk_text_buffer_delete(buffer, &start, &end);
    }
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_view_scroll_to_iter(
        GTK_TEXT_VIEW(widgets->central_log), &end, 0.0, FALSE, 0.0, 1.0);
}

static GtkWidget *build_device_page(app_widgets *widgets) {
    static const struct {
        guint index;
        const char *label;
    } quick_operations[] = {
        {0u, "Read"}, {1u, "Blank"}, {2u, "Program"},
        {4u, "Verify"}, {6u, "Erase"}, {5u, "Compare"},
        {8u, "Protect"}, {9u, "Config"}
    };
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *quick_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *workflow = gtk_notebook_new();
    GtkWidget *select_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *operation_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *selection_content =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *selection_lists =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *vendor_frame = gtk_frame_new("Vendor");
    GtkWidget *device_frame = gtk_frame_new("Device");
    GtkWidget *vendor_scroll;
    GtkWidget *programmer_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *path_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *search_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *type_frame = gtk_frame_new("Type");
    GtkWidget *type_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *catalogue_internal_controls =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *guidance_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *selection_details = gtk_grid_new();
    GtkWidget *message_frame = gtk_frame_new("Messages");
    GtkWidget *message_scroll = gtk_scrolled_window_new();
    GtkWidget *selection_window = gtk_window_new();
    GtkWidget *auto_id_window = gtk_window_new();
    GtkWidget *auto_id_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *auto_id_lists =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *auto_id_vendor_frame = gtk_frame_new("Vendor");
    GtkWidget *auto_id_device_frame = gtk_frame_new("Device");
    GtkWidget *auto_id_vendor_scroll = gtk_scrolled_window_new();
    GtkWidget *auto_id_scroll = gtk_scrolled_window_new();
    GtkWidget *auto_id_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *auto_id_help = gtk_button_new_with_label("Help");
    GtkWidget *auto_id_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *auto_id_accept =
        gtk_button_new_with_label("Use selected candidate");
    GtkWidget *options_window = gtk_window_new();
    GtkWidget *parameters_window = gtk_window_new();
    GtkWidget *machine_window = gtk_window_new();
    GtkWidget *machine_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *machine_scroll = gtk_scrolled_window_new();
    GtkWidget *machine_actions =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *machine_refresh = gtk_button_new_with_label("Refresh");
    GtkWidget *machine_done = gtk_button_new_with_label("Done");
    GtkWidget *selection_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *selection_help = gtk_button_new_with_label("Help");
    GtkWidget *selection_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *selection_done = gtk_button_new_with_label("Use selected device");
    GtkWidget *options_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *options_actions =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *options_spacer =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *options_help = gtk_button_new_with_label("Help");
    GtkWidget *options_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *options_ok = gtk_button_new_with_label("OK");
    GtkWidget *parameters_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *parameters_grid = gtk_grid_new();
    GtkWidget *parameters_actions =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *parameters_spacer =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *parameters_help = gtk_button_new_with_label("Help");
    GtkWidget *parameters_cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *parameters_ok = gtk_button_new_with_label("OK");
    GtkWidget *manage_databases =
        gtk_button_new_with_label("Manage…");
    widgets->database_manage_button = manage_databases;
    widgets->selection_accept_button = selection_done;
    gtk_widget_set_sensitive(selection_done, FALSE);
    widgets->catalogue_verify_button =
        gtk_button_new_with_label("Verify installation");
    widgets->cache_import_button =
        gtk_button_new_with_label("Create private cache…");
    widgets->catalogue_search_button = gtk_button_new_with_label("Search");
    widgets->catalogue_rank_button =
        gtk_button_new_with_label("Rank observations");
    widgets->catalogue_cancel_button =
        gtk_button_new_with_label("Cancel catalogue task");
    GtkWidget *quick_save = gtk_button_new_with_label("Save");
    GtkWidget *quick_load = gtk_button_new_with_label("Load");
    GtkWidget *quick_select = gtk_button_new_with_label("Select");
    GtkWidget *quick_auto_id = gtk_button_new_with_label("Auto-ID");
    GtkWidget *quick_edit = gtk_button_new_with_label("Edit");
    GtkWidget *quick_vector = gtk_button_new_with_label("Vector");
    GtkWidget *quick_option = gtk_button_new_with_label("Option");
    widgets->toolbar_save_button = quick_save;
    widgets->toolbar_edit_button = quick_edit;
    widgets->toolbar_vector_button = quick_vector;
    widgets->toolbar_option_button = quick_option;
    gtk_widget_set_sensitive(quick_save, FALSE);
    gtk_widget_set_sensitive(quick_edit, FALSE);
    gtk_widget_set_sensitive(quick_option, FALSE);
    GtkWidget *fixed_quick_buttons[] = {
        quick_save, quick_load, quick_select, quick_auto_id,
        quick_edit, quick_vector, quick_option
    };
    for (size_t quick = 0u;
         quick < G_N_ELEMENTS(fixed_quick_buttons); ++quick)
        gtk_widget_add_css_class(
            fixed_quick_buttons[quick], "quick-command");
    widgets->auto_id_button = quick_auto_id;
    widgets->device_workflow = workflow;
    widgets->device_select_window = GTK_WINDOW(selection_window);
    widgets->auto_id_window = GTK_WINDOW(auto_id_window);
    widgets->operation_options_window = GTK_WINDOW(options_window);
    widgets->programming_parameters_window = GTK_WINDOW(parameters_window);
    widgets->machine_select_window = GTK_WINDOW(machine_window);
    gtk_window_set_title(
        widgets->device_select_window, "Change Device");
    gtk_window_set_transient_for(
        widgets->device_select_window, widgets->window);
    gtk_window_set_destroy_with_parent(widgets->device_select_window, TRUE);
    gtk_window_set_modal(widgets->device_select_window, TRUE);
    gtk_window_set_hide_on_close(widgets->device_select_window, TRUE);
    gtk_window_set_default_size(widgets->device_select_window, 850, 600);
    gtk_window_set_title(widgets->auto_id_window, "Auto Select EPROM");
    gtk_window_set_transient_for(widgets->auto_id_window, widgets->window);
    gtk_window_set_destroy_with_parent(widgets->auto_id_window, TRUE);
    gtk_window_set_modal(widgets->auto_id_window, TRUE);
    gtk_window_set_hide_on_close(widgets->auto_id_window, TRUE);
    gtk_window_set_default_size(widgets->auto_id_window, 620, 430);
    gtk_widget_set_margin_top(auto_id_box, 12);
    gtk_widget_set_margin_bottom(auto_id_box, 12);
    gtk_widget_set_margin_start(auto_id_box, 12);
    gtk_widget_set_margin_end(auto_id_box, 12);
    widgets->auto_id_status = gtk_label_new(
        "Start Auto-ID to probe the inserted EPROM");
    gtk_label_set_xalign(GTK_LABEL(widgets->auto_id_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->auto_id_status), TRUE);
    widgets->auto_id_vendor_list = gtk_list_box_new();
    widgets->auto_id_list = gtk_list_box_new();
    widgets->auto_id_accept_button = auto_id_accept;
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(widgets->auto_id_vendor_list), GTK_SELECTION_SINGLE);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(widgets->auto_id_list),
                                    GTK_SELECTION_SINGLE);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(auto_id_vendor_scroll),
        widgets->auto_id_vendor_list);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(auto_id_scroll),
                                  widgets->auto_id_list);
    gtk_widget_set_size_request(auto_id_vendor_scroll, 180, -1);
    gtk_widget_set_vexpand(auto_id_vendor_scroll, TRUE);
    gtk_widget_set_vexpand(auto_id_scroll, TRUE);
    gtk_widget_set_hexpand(auto_id_scroll, TRUE);
    gtk_widget_set_sensitive(auto_id_accept, FALSE);
    gtk_widget_set_halign(auto_id_actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(auto_id_actions), auto_id_help);
    gtk_box_append(GTK_BOX(auto_id_actions), auto_id_cancel);
    gtk_box_append(GTK_BOX(auto_id_actions), auto_id_accept);
    gtk_box_append(GTK_BOX(auto_id_box), widgets->auto_id_status);
    gtk_frame_set_child(
        GTK_FRAME(auto_id_vendor_frame), auto_id_vendor_scroll);
    gtk_frame_set_child(GTK_FRAME(auto_id_device_frame), auto_id_scroll);
    gtk_widget_set_hexpand(auto_id_device_frame, TRUE);
    gtk_box_append(GTK_BOX(auto_id_lists), auto_id_vendor_frame);
    gtk_box_append(GTK_BOX(auto_id_lists), auto_id_device_frame);
    gtk_box_append(GTK_BOX(auto_id_box), auto_id_lists);
    gtk_box_append(GTK_BOX(auto_id_box), auto_id_actions);
    gtk_window_set_child(widgets->auto_id_window, auto_id_box);
    g_signal_connect(widgets->auto_id_vendor_list, "row-selected",
                     G_CALLBACK(auto_id_vendor_selected), widgets);
    g_signal_connect(widgets->auto_id_list, "row-selected",
                     G_CALLBACK(auto_id_candidate_changed), widgets);
    g_signal_connect(auto_id_accept, "clicked",
                     G_CALLBACK(auto_id_candidate_accept), widgets);
    g_signal_connect_swapped(auto_id_cancel, "clicked",
                             G_CALLBACK(gtk_window_close),
                             widgets->auto_id_window);
    g_signal_connect_swapped(auto_id_help, "clicked",
                             G_CALLBACK(gtk_window_present),
                             widgets->help_window);
    gtk_window_set_title(
        widgets->operation_options_window, "Device Operation Option");
    gtk_window_set_transient_for(
        widgets->operation_options_window, widgets->window);
    gtk_window_set_destroy_with_parent(
        widgets->operation_options_window, TRUE);
    gtk_window_set_modal(widgets->operation_options_window, TRUE);
    gtk_window_set_hide_on_close(widgets->operation_options_window, TRUE);
    gtk_window_set_default_size(widgets->operation_options_window, 900, 430);
    gtk_window_set_title(
        widgets->programming_parameters_window, "Programming Parameters");
    gtk_window_set_transient_for(
        widgets->programming_parameters_window, widgets->window);
    gtk_window_set_destroy_with_parent(
        widgets->programming_parameters_window, TRUE);
    gtk_window_set_modal(widgets->programming_parameters_window, TRUE);
    gtk_window_set_hide_on_close(
        widgets->programming_parameters_window, TRUE);
    gtk_window_set_default_size(
        widgets->programming_parameters_window, 520, 300);
    gtk_window_set_title(widgets->machine_select_window, "Select Machine");
    gtk_window_set_transient_for(widgets->machine_select_window,
                                 widgets->window);
    gtk_window_set_destroy_with_parent(widgets->machine_select_window, TRUE);
    gtk_window_set_modal(widgets->machine_select_window, TRUE);
    gtk_window_set_hide_on_close(widgets->machine_select_window, TRUE);
    gtk_window_set_default_size(widgets->machine_select_window, 620, 360);
    gtk_widget_set_margin_top(machine_box, 12);
    gtk_widget_set_margin_bottom(machine_box, 12);
    gtk_widget_set_margin_start(machine_box, 12);
    gtk_widget_set_margin_end(machine_box, 12);
    widgets->machine_status = gtk_label_new(
        "Searching for supported programmers…");
    gtk_label_set_xalign(GTK_LABEL(widgets->machine_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->machine_status), TRUE);
    widgets->machine_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(widgets->machine_list),
                                    GTK_SELECTION_SINGLE);
    g_signal_connect(widgets->machine_list, "row-selected",
                     G_CALLBACK(programmer_selected), widgets);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(machine_scroll),
                                  widgets->machine_list);
    gtk_widget_set_vexpand(machine_scroll, TRUE);
    gtk_box_append(GTK_BOX(machine_actions), machine_refresh);
    gtk_box_append(GTK_BOX(machine_actions), machine_done);
    gtk_box_append(GTK_BOX(machine_box), widgets->machine_status);
    gtk_box_append(GTK_BOX(machine_box), machine_scroll);
    gtk_box_append(GTK_BOX(machine_box), machine_actions);
    gtk_window_set_child(widgets->machine_select_window, machine_box);
    g_signal_connect(machine_refresh, "clicked",
                     G_CALLBACK(refresh_programmers), widgets);
    g_signal_connect_swapped(machine_done, "clicked",
                             G_CALLBACK(gtk_window_close),
                             widgets->machine_select_window);
    gtk_widget_set_margin_top(select_box, 10);
    gtk_widget_set_margin_bottom(select_box, 10);
    gtk_widget_set_margin_start(select_box, 10);
    gtk_widget_set_margin_end(select_box, 10);
    gtk_widget_set_margin_top(options_box, 10);
    gtk_widget_set_margin_bottom(options_box, 10);
    gtk_widget_set_margin_start(options_box, 10);
    gtk_widget_set_margin_end(options_box, 10);
    gtk_widget_set_margin_top(parameters_box, 12);
    gtk_widget_set_margin_bottom(parameters_box, 12);
    gtk_widget_set_margin_start(parameters_box, 12);
    gtk_widget_set_margin_end(parameters_box, 12);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(workflow), FALSE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(workflow), FALSE);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(quick_bar), GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Primary commands", -1);
    g_signal_connect(quick_save, "clicked",
                     G_CALLBACK(toolbar_save_clicked), widgets);
    g_signal_connect(quick_load, "clicked",
                     G_CALLBACK(toolbar_load_clicked), widgets);
    g_signal_connect(quick_edit, "clicked",
                     G_CALLBACK(toolbar_edit_clicked), widgets);
    g_signal_connect(quick_vector, "clicked",
                     G_CALLBACK(quick_vector_clicked), widgets);
    gtk_widget_set_sensitive(quick_vector, FALSE);
    gtk_widget_set_tooltip_text(
        quick_vector, "Enabled only for definitions with editable test vectors");
    gtk_box_append(GTK_BOX(quick_bar), quick_save);
    gtk_box_append(GTK_BOX(quick_bar), quick_load);
    gtk_box_append(GTK_BOX(quick_bar),
                   gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    gtk_box_append(GTK_BOX(quick_bar), quick_select);
    gtk_box_append(GTK_BOX(quick_bar), quick_auto_id);
    gtk_box_append(GTK_BOX(quick_bar),
                   gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    gtk_box_append(GTK_BOX(quick_bar), quick_edit);
    gtk_box_append(GTK_BOX(quick_bar), quick_vector);
    gtk_box_append(GTK_BOX(quick_bar),
                   gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    g_signal_connect(quick_select, "clicked",
                     G_CALLBACK(quick_select_clicked), widgets);
    g_signal_connect(quick_auto_id, "clicked",
                     G_CALLBACK(quick_auto_id_clicked), widgets);
    for (size_t quick = 0u; quick < G_N_ELEMENTS(quick_operations); ++quick) {
        guint index = quick_operations[quick].index;
        GtkWidget *action = gtk_button_new_with_label(
            quick_operations[quick].label);
        if (index == 9u)
            gtk_box_append(GTK_BOX(quick_bar),
                           gtk_separator_new(GTK_ORIENTATION_VERTICAL));
        widgets->quick_operation_buttons[index] = action;
        gtk_widget_add_css_class(action, "quick-command");
        gtk_widget_set_sensitive(action, FALSE);
        g_object_set_data(G_OBJECT(action), "labtool48-operation-index",
                          GUINT_TO_POINTER(index));
        g_signal_connect(action, "clicked",
                         G_CALLBACK(quick_operation_clicked), widgets);
        gtk_box_append(GTK_BOX(quick_bar), action);
    }
    g_signal_connect(quick_option, "clicked",
                     G_CALLBACK(quick_option_clicked), widgets);
    gtk_box_append(GTK_BOX(quick_bar), quick_option);
    static const char *operations[] = {
        "Read",
        "Blank check",
        "Program",
        "Over-program",
        "Verify",
        "Compare",
        "Erase",
        "Secure",
        "Memory protect",
        "Configuration",
        "Functional test",
        "Auto-ID",
        "Mass production",
        "JTAG/ISP",
        NULL
    };
    static const char *verify_margins[] = {
        "All margins", "Nominal only", "Low only", "High only", NULL
    };
    static const char *verify_margin_types[] = {
        "Type 0 (95/105%)", "Type 1 (90/110%)", NULL
    };
    static const char *image_formats[] = {
        "Auto from suffix", "Raw BIN/ROM", "Intel HEX",
        "Motorola S-record", "Straight HEX", "MOS Technology",
        "Tektronix HEX", "HP64000 Absolute", NULL
    };
    static const char *image_gap_policies[] = {
        "Preserve sparse gaps", "Fill sparse gaps", NULL
    };
    static const char *auto_eprom_modes[] = {
        "Contact probe only (no ID voltage)",
        "Acquire signature (up to 12 V)",
        NULL
    };
    GtkWidget *prepare = gtk_button_new_with_label("Read device");
    GtkWidget *cancel_prepare =
        gtk_button_new_with_label("Cancel pending action");
    GtkWidget *prepare_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *execute = gtk_button_new_with_label("Execute prepared Read");
    GtkWidget *cancel = gtk_button_new_with_label("Cancel active operation");
    GtkWidget *read_frame = gtk_frame_new(
        "Device Operation Options"
    );
    GtkWidget *read_grid = gtk_grid_new();
    GtkWidget *internal_controls =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll;
    const char *default_path = g_getenv("LABTOOL48_INSTALLATION");
    char *owned_default_path = NULL;
    char *saved_active_path = database_registry_load(widgets);
    char *canonical_default_path;
    if (default_path == NULL || default_path[0] == '\0') {
        if (saved_active_path != NULL && saved_active_path[0] != '\0')
            default_path = saved_active_path;
        else {
            owned_default_path = project_path("AEC/LabTool-48UXP");
            if (owned_default_path == NULL)
                owned_default_path = project_path(
                    "analysis/software-v13.10/rez");
            default_path = owned_default_path;
        }
    }
    if (default_path == NULL || default_path[0] == '\0')
        default_path = "AEC/LabTool-48UXP";
    canonical_default_path = g_canonicalize_filename(default_path, NULL);
    widgets->installation_entry = gtk_entry_new();
    widgets->database_dropdown = gtk_drop_down_new(NULL, NULL);
    gtk_widget_set_hexpand(widgets->database_dropdown, TRUE);
    gtk_editable_set_text(GTK_EDITABLE(widgets->installation_entry),
                           canonical_default_path);
    if (!database_registry_contains(widgets, canonical_default_path))
        g_ptr_array_add(widgets->database_paths,
                        g_strdup(canonical_default_path));
    g_free(owned_default_path);
    g_free(saved_active_path);
    g_free(canonical_default_path);
    gtk_box_append(GTK_BOX(path_row), gtk_label_new("Device database:"));
    gtk_widget_set_visible(widgets->installation_entry, FALSE);
    gtk_box_append(GTK_BOX(path_row), widgets->installation_entry);
    gtk_box_append(GTK_BOX(path_row), widgets->database_dropdown);
    gtk_box_append(GTK_BOX(path_row), manage_databases);
    gtk_box_append(GTK_BOX(path_row), widgets->catalogue_verify_button);
    gtk_widget_set_visible(widgets->catalogue_verify_button, FALSE);
    g_signal_connect(manage_databases, "clicked",
                     G_CALLBACK(present_database_manager), widgets);
    g_signal_connect(widgets->catalogue_verify_button, "clicked",
                     G_CALLBACK(verify_installation), widgets);
    g_signal_connect(widgets->cache_import_button, "clicked",
                     G_CALLBACK(create_private_cache), widgets);
    widgets->database_manager_window = build_database_manager(widgets);
    database_selector_rebuild(widgets);
    g_signal_connect(widgets->database_dropdown, "notify::selected",
                     G_CALLBACK(database_selector_changed), widgets);
    database_registry_save(widgets);
    gtk_box_append(GTK_BOX(select_box), path_row);
    widgets->query_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->query_entry),
        "Device or vendor; leave empty to browse the complete database");
    gtk_widget_set_hexpand(widgets->query_entry, TRUE);
    gtk_box_append(GTK_BOX(search_row), widgets->query_entry);
    gtk_box_append(GTK_BOX(search_row), widgets->catalogue_search_button);
    gtk_box_append(GTK_BOX(search_row), widgets->catalogue_cancel_button);
    gtk_widget_set_sensitive(widgets->catalogue_cancel_button, FALSE);
    gtk_widget_set_visible(widgets->catalogue_cancel_button, FALSE);
    g_signal_connect(widgets->catalogue_search_button, "clicked",
                     G_CALLBACK(search_devices), widgets);
    g_signal_connect(widgets->catalogue_cancel_button, "clicked",
                     G_CALLBACK(cancel_catalogue_task), widgets);
    g_signal_connect(widgets->query_entry, "activate",
                     G_CALLBACK(search_entry_activated), widgets);
    gtk_box_append(GTK_BOX(select_box), search_row);
    {
        static const char *const type_labels[] = {
            "All", "EPROM", "PROM", "PLD", "MPU"
        };
        static const char *const type_actions[] = {
            "win.device-filter-all",
            "win.device-filter-eprom",
            "win.device-filter-prom",
            "win.device-filter-pld",
            "win.device-filter-mpu"
        };
        for (guint index = 0u; index < G_N_ELEMENTS(type_labels); ++index) {
            widgets->device_type_filters[index] =
                gtk_check_button_new_with_label(type_labels[index]);
            gtk_actionable_set_action_name(
                GTK_ACTIONABLE(widgets->device_type_filters[index]),
                type_actions[index]);
            if (index != 0u)
                gtk_check_button_set_group(
                    GTK_CHECK_BUTTON(widgets->device_type_filters[index]),
                    GTK_CHECK_BUTTON(widgets->device_type_filters[0]));
            gtk_box_append(
                GTK_BOX(type_box), widgets->device_type_filters[index]);
        }
        gtk_check_button_set_active(
            GTK_CHECK_BUTTON(widgets->device_type_filters[0]), TRUE);
        for (guint index = 0u; index < G_N_ELEMENTS(type_labels); ++index)
            g_signal_connect(
                widgets->device_type_filters[index], "toggled",
                G_CALLBACK(device_type_filter_changed), widgets);
    }
    gtk_widget_set_margin_top(type_box, 4);
    gtk_widget_set_margin_bottom(type_box, 4);
    gtk_widget_set_margin_start(type_box, 8);
    gtk_widget_set_margin_end(type_box, 8);
    gtk_frame_set_child(GTK_FRAME(type_frame), type_box);
    gtk_box_append(GTK_BOX(select_box), type_frame);
    widgets->auto_manufacturer_entry = gtk_entry_new();
    widgets->auto_device_entry = gtk_entry_new();
    widgets->auto_pins_entry = gtk_entry_new();
    widgets->auto_class_entry = gtk_entry_new();
    widgets->auto_eprom_mode_dropdown =
        gtk_drop_down_new_from_strings(auto_eprom_modes);
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->auto_manufacturer_entry), "observed ID");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->auto_device_entry), "observed ID");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->auto_pins_entry), "optional");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->auto_class_entry), "optional");
    gtk_editable_set_width_chars(
        GTK_EDITABLE(widgets->auto_manufacturer_entry), 10);
    gtk_editable_set_width_chars(
        GTK_EDITABLE(widgets->auto_device_entry), 10);
    gtk_editable_set_width_chars(GTK_EDITABLE(widgets->auto_pins_entry), 8);
    gtk_editable_set_width_chars(GTK_EDITABLE(widgets->auto_class_entry), 8);
    gtk_box_append(
        GTK_BOX(catalogue_internal_controls),
        widgets->auto_manufacturer_entry);
    gtk_box_append(
        GTK_BOX(catalogue_internal_controls), widgets->auto_device_entry);
    gtk_box_append(
        GTK_BOX(catalogue_internal_controls), widgets->auto_pins_entry);
    gtk_box_append(
        GTK_BOX(catalogue_internal_controls), widgets->auto_class_entry);
    gtk_box_append(
        GTK_BOX(catalogue_internal_controls), widgets->auto_eprom_mode_dropdown);
    gtk_box_append(
        GTK_BOX(catalogue_internal_controls), widgets->catalogue_rank_button);
    gtk_widget_set_visible(catalogue_internal_controls, FALSE);
    g_signal_connect(widgets->catalogue_rank_button, "clicked",
                     G_CALLBACK(rank_auto_id), widgets);
    gtk_box_append(GTK_BOX(select_box), catalogue_internal_controls);
    widgets->device_status = gtk_label_new(
        "Browse the active database, narrow it by search, or use Auto-ID");
    gtk_label_set_xalign(GTK_LABEL(widgets->device_status), 0.0F);
    gtk_label_set_lines(GTK_LABEL(widgets->device_status), 2);
    gtk_label_set_ellipsize(
        GTK_LABEL(widgets->device_status), PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(select_box), widgets->device_status);
    widgets->device_details = gtk_label_new(
        "Click a result to select the device");
    gtk_label_set_xalign(GTK_LABEL(widgets->device_details), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->device_details), TRUE);
    gtk_label_set_lines(GTK_LABEL(widgets->device_details), 4);
    gtk_label_set_ellipsize(
        GTK_LABEL(widgets->device_details), PANGO_ELLIPSIZE_END);
    gtk_widget_add_css_class(widgets->device_details, "dim-label");
    gtk_widget_set_hexpand(widgets->device_details, TRUE);
    gtk_widget_set_valign(widgets->device_details, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(guidance_row), widgets->device_details);
    widgets->device_type_value = gtk_label_new("—");
    widgets->device_adapter_value = gtk_label_new("—");
    widgets->device_manufacturer_value = gtk_label_new("—");
    widgets->device_code_value = gtk_label_new("—");
    widgets->device_note_value = gtk_label_new("—");
    gtk_grid_set_row_spacing(GTK_GRID(selection_details), 4);
    gtk_grid_set_column_spacing(GTK_GRID(selection_details), 8);
    gtk_grid_attach(GTK_GRID(selection_details), gtk_label_new("Type:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), widgets->device_type_value,
                    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), gtk_label_new("Adapter:"),
                    2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), widgets->device_adapter_value,
                    3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), gtk_label_new("Man. Code:"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details),
                    widgets->device_manufacturer_value, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), gtk_label_new("Dev. Code:"),
                    2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), widgets->device_code_value,
                    3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), gtk_label_new("Note:"),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(selection_details), widgets->device_note_value,
                    1, 2, 3, 1);
    for (guint column = 1u; column < 4u; column += 2u) {
        GtkWidget *value = column == 1u ? widgets->device_type_value :
            widgets->device_adapter_value;
        gtk_label_set_xalign(GTK_LABEL(value), 0.0F);
        gtk_widget_set_hexpand(value, TRUE);
    }
    gtk_label_set_xalign(
        GTK_LABEL(widgets->device_manufacturer_value), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(widgets->device_code_value), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(widgets->device_note_value), 0.0F);
    gtk_label_set_ellipsize(
        GTK_LABEL(widgets->device_note_value), PANGO_ELLIPSIZE_END);
    gtk_grid_set_row_spacing(GTK_GRID(read_grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(read_grid), 8);
    gtk_widget_set_margin_top(read_grid, 8);
    gtk_widget_set_margin_bottom(read_grid, 8);
    gtk_widget_set_margin_start(read_grid, 8);
    gtk_widget_set_margin_end(read_grid, 8);
    widgets->read_start_entry = gtk_entry_new();
    widgets->read_length_entry = gtk_entry_new();
    widgets->read_output_entry = gtk_entry_new();
    widgets->read_bus_entry = gtk_entry_new();
    widgets->read_address_entry = gtk_entry_new();
    widgets->read_timeout_entry = gtk_entry_new();
    widgets->operation_dropdown = gtk_drop_down_new_from_strings(operations);
    g_signal_connect(widgets->operation_dropdown, "notify::selected",
                     G_CALLBACK(operation_selection_changed), widgets);
    widgets->verify_image_entry = gtk_entry_new();
    widgets->verify_image_browse_button =
        gtk_button_new_with_label("Choose…");
    widgets->image_inspect_button =
        gtk_button_new_with_label("Inspect mapping");
    widgets->image_status = gtk_label_new(
        "Choose an image and inspect how its addresses map to the selected "
        "device before programming, verification or comparison");
    widgets->image_format_dropdown =
        gtk_drop_down_new_from_strings(image_formats);
    widgets->image_base_entry = gtk_entry_new();
    widgets->image_gap_dropdown =
        gtk_drop_down_new_from_strings(image_gap_policies);
    widgets->image_gap_fill_entry = gtk_entry_new();
    widgets->verify_margin_dropdown =
        gtk_drop_down_new_from_strings(verify_margins);
    widgets->verify_margin_type_dropdown =
        gtk_drop_down_new_from_strings(verify_margin_types);
    widgets->parameters_margin_dropdown =
        gtk_drop_down_new_from_strings(verify_margin_types);
    widgets->parameters_nominal = gtk_label_new("—");
    widgets->parameters_low = gtk_label_new("—");
    widgets->parameters_high = gtk_label_new("—");
    widgets->parameters_status = gtk_label_new(
        "Select a device to load its programming parameters");
    gtk_grid_set_row_spacing(GTK_GRID(parameters_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(parameters_grid), 12);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    gtk_label_new("Verify margin policy:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    widgets->parameters_margin_dropdown, 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    gtk_label_new("Verify passes:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    widgets->verify_margin_dropdown, 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    gtk_label_new("Nominal VCC:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    widgets->parameters_nominal, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    gtk_label_new("Low verify VCC:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    widgets->parameters_low, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    gtk_label_new("High verify VCC:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(parameters_grid),
                    widgets->parameters_high, 1, 4, 1, 1);
    gtk_label_set_xalign(GTK_LABEL(widgets->parameters_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->parameters_status), TRUE);
    gtk_widget_set_hexpand(parameters_spacer, TRUE);
    gtk_box_append(GTK_BOX(parameters_actions), parameters_help);
    gtk_box_append(GTK_BOX(parameters_actions), parameters_spacer);
    gtk_box_append(GTK_BOX(parameters_actions), parameters_cancel);
    gtk_box_append(GTK_BOX(parameters_actions), parameters_ok);
    gtk_widget_add_css_class(parameters_ok, "suggested-action");
    gtk_box_append(GTK_BOX(parameters_box), parameters_grid);
    gtk_box_append(GTK_BOX(parameters_box), widgets->parameters_status);
    gtk_box_append(GTK_BOX(parameters_box), parameters_actions);
    gtk_window_set_child(
        widgets->programming_parameters_window, parameters_box);
    g_signal_connect(parameters_help, "clicked",
                     G_CALLBACK(settings_dialog_help), widgets);
    g_signal_connect(parameters_cancel, "clicked",
                     G_CALLBACK(settings_dialog_cancel), widgets);
    g_signal_connect(parameters_ok, "clicked",
                     G_CALLBACK(settings_dialog_accept), widgets);
    g_signal_connect(
        widgets->programming_parameters_window, "close-request",
        G_CALLBACK(settings_dialog_close_requested), widgets);
    widgets->program_identity_override_check =
        gtk_check_button_new_with_label("Override Auto-ID mismatch");
    widgets->operation_report_entry = gtk_entry_new();
    widgets->execution_report_entry = gtk_entry_new();
    widgets->verify_use_buffer_check =
        gtk_check_button_new_with_label("Use resident buffer");
    widgets->read_confirmation_entry = gtk_entry_new();
    widgets->read_execute_button = execute;
    widgets->read_cancel_button = cancel;
    widgets->operation_prepare_button = prepare;
    widgets->operation_prepare_cancel_button = cancel_prepare;
    gtk_box_append(GTK_BOX(prepare_actions), prepare);
    gtk_box_append(GTK_BOX(prepare_actions), cancel_prepare);
    gtk_widget_set_sensitive(cancel_prepare, FALSE);
    gtk_widget_set_sensitive(cancel, FALSE);
    gtk_widget_set_sensitive(execute, FALSE);
    gtk_widget_add_css_class(prepare, "suggested-action");
    gtk_editable_set_text(GTK_EDITABLE(widgets->read_start_entry), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->read_output_entry), "read.bin");
    gtk_editable_set_text(GTK_EDITABLE(widgets->read_timeout_entry), "2000");
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->execution_report_entry), "read-report.json");
    gtk_editable_set_editable(GTK_EDITABLE(widgets->read_bus_entry), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(widgets->read_address_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->read_bus_entry), "bus");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->read_address_entry),
                                   "address");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->read_confirmation_entry),
        "filled automatically by internal read preflight"
    );
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->verify_image_entry),
        "BIN, ROM, Intel HEX, S-record, Straight HEX, MOS, Tektronix, or HP64000 image"
    );
    gtk_editable_set_text(GTK_EDITABLE(widgets->image_base_entry), "0");
    gtk_editable_set_text(GTK_EDITABLE(widgets->image_gap_fill_entry), "0xff");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->image_base_entry),
                                   "source address mapped to device offset 0");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->operation_report_entry),
        "new payload-free report path (optional for offline planning)"
    );
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->execution_report_entry),
        "new report path required for confirmed Read execution"
    );
    gtk_widget_set_hexpand(widgets->read_output_entry, TRUE);
    gtk_widget_set_hexpand(widgets->read_confirmation_entry, TRUE);
    gtk_widget_set_hexpand(widgets->verify_image_entry, TRUE);
    gtk_label_set_xalign(GTK_LABEL(widgets->image_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->image_status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->image_status), TRUE);
    gtk_widget_add_css_class(widgets->image_status, "dim-label");
    gtk_widget_set_hexpand(widgets->operation_report_entry, TRUE);
    gtk_widget_set_hexpand(widgets->execution_report_entry, TRUE);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->read_start_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Start address", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->read_length_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Operation length", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->read_output_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Read output file", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->read_timeout_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Operation timeout milliseconds", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->verify_image_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Input image", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->image_base_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Input image address base", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->image_gap_fill_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Sparse image gap fill", -1);
    gtk_grid_attach(GTK_GRID(read_grid), gtk_label_new("Start:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->read_start_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), gtk_label_new("Length:"), 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->read_length_entry, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), gtk_label_new("Timeout ms:"),
                    4, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->read_timeout_entry,
                    5, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(read_grid), gtk_label_new("Output:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->read_output_entry,
                    1, 1, 7, 1);
    gtk_grid_attach(GTK_GRID(read_grid), gtk_label_new("Input image:"),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->verify_image_entry,
                    1, 2, 5, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->verify_image_browse_button,
                    6, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->verify_use_buffer_check,
                    7, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), gtk_label_new("Image format:"),
                    0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->image_format_dropdown,
                    1, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(read_grid), gtk_label_new("Address base:"),
                    3, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->image_base_entry,
                    4, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->image_gap_dropdown,
                    5, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->image_gap_fill_entry,
                    7, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->image_status,
                    0, 4, 7, 1);
    gtk_grid_attach(GTK_GRID(read_grid), widgets->image_inspect_button,
                    7, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(read_grid),
                    widgets->program_identity_override_check, 0, 5, 8, 1);
    {
        GtkWidget *warning = gtk_label_new(
            "Operations use the selected device and detected programmer. "
            "Program verifies the selected device, requires a blank part, "
            "and performs mandatory readback. Over-program permits only "
            "one-to-zero transitions."
        );
        gtk_label_set_xalign(GTK_LABEL(warning), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(warning), TRUE);
        gtk_widget_add_css_class(warning, "warning");
        gtk_grid_attach(GTK_GRID(read_grid), warning, 0, 6, 7, 1);
    }
    gtk_box_append(GTK_BOX(internal_controls), widgets->operation_dropdown);
    gtk_box_append(GTK_BOX(internal_controls), prepare_actions);
    gtk_box_append(GTK_BOX(internal_controls), execute);
    gtk_box_append(GTK_BOX(internal_controls), cancel);
    gtk_box_append(GTK_BOX(internal_controls), widgets->read_confirmation_entry);
    gtk_box_append(GTK_BOX(internal_controls), widgets->operation_report_entry);
    gtk_box_append(GTK_BOX(internal_controls), widgets->execution_report_entry);
    gtk_box_append(GTK_BOX(internal_controls), widgets->read_bus_entry);
    gtk_box_append(GTK_BOX(internal_controls), widgets->read_address_entry);
    gtk_box_append(GTK_BOX(internal_controls),
                   widgets->verify_margin_type_dropdown);
    gtk_widget_set_visible(internal_controls, FALSE);
    widgets->read_preparation = gtk_label_new(
        "Select a supported device; the detected programmer is used automatically"
    );
    g_object_ref_sink(widgets->read_preparation);
    gtk_label_set_xalign(GTK_LABEL(widgets->read_preparation), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(widgets->read_preparation), TRUE);
    gtk_label_set_wrap(GTK_LABEL(widgets->read_preparation), TRUE);
    gtk_label_set_lines(GTK_LABEL(widgets->read_preparation), 6);
    gtk_label_set_ellipsize(
        GTK_LABEL(widgets->read_preparation), PANGO_ELLIPSIZE_END);
    widgets->central_log = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->central_log), FALSE);
    gtk_text_view_set_cursor_visible(
        GTK_TEXT_VIEW(widgets->central_log), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(widgets->central_log), TRUE);
    gtk_text_view_set_wrap_mode(
        GTK_TEXT_VIEW(widgets->central_log), GTK_WRAP_WORD_CHAR);
    g_signal_connect(widgets->read_preparation, "notify::label",
                     G_CALLBACK(operation_message_changed), widgets);
    operation_message_changed(
        G_OBJECT(widgets->read_preparation), NULL, widgets);
    g_signal_connect(prepare, "clicked", G_CALLBACK(prepare_read), widgets);
    g_signal_connect(cancel_prepare, "clicked",
                     G_CALLBACK(cancel_operation_preparation), widgets);
    g_signal_connect(execute, "clicked", G_CALLBACK(execute_read), widgets);
    g_signal_connect(cancel, "clicked", G_CALLBACK(cancel_read), widgets);
    g_signal_connect(widgets->read_start_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->read_length_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->read_output_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->read_bus_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->read_address_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->read_timeout_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->verify_image_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->verify_image_browse_button, "clicked",
                     G_CALLBACK(browse_programming_image), widgets);
    g_signal_connect(widgets->image_inspect_button, "clicked",
                     G_CALLBACK(inspect_programming_image), widgets);
    g_signal_connect(widgets->image_format_dropdown, "notify::selected",
                     G_CALLBACK(operation_input_notify), widgets);
    g_signal_connect(widgets->image_base_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->image_gap_dropdown, "notify::selected",
                     G_CALLBACK(operation_input_notify), widgets);
    g_signal_connect(widgets->image_gap_fill_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->operation_report_entry, "changed",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->verify_use_buffer_check, "toggled",
                     G_CALLBACK(operation_input_changed), widgets);
    g_signal_connect(widgets->verify_margin_dropdown, "notify::selected",
                     G_CALLBACK(operation_input_notify), widgets);
    g_signal_connect(widgets->verify_margin_type_dropdown, "notify::selected",
                     G_CALLBACK(operation_margin_type_changed), widgets);
    g_signal_connect(widgets->parameters_margin_dropdown, "notify::selected",
                     G_CALLBACK(parameters_margin_changed), widgets);
    g_signal_connect(widgets->program_identity_override_check, "toggled",
                     G_CALLBACK(operation_input_changed), widgets);
    refresh_operation_dropdown(widgets);
    gtk_frame_set_child(GTK_FRAME(read_frame), read_grid);
    gtk_widget_set_hexpand(options_spacer, TRUE);
    gtk_box_append(GTK_BOX(options_actions), options_help);
    gtk_box_append(GTK_BOX(options_actions), options_spacer);
    gtk_box_append(GTK_BOX(options_actions), options_cancel);
    gtk_box_append(GTK_BOX(options_actions), options_ok);
    gtk_widget_add_css_class(options_ok, "suggested-action");
    gtk_box_append(GTK_BOX(options_box), read_frame);
    gtk_box_append(GTK_BOX(options_box), internal_controls);
    gtk_box_append(GTK_BOX(options_box), options_actions);
    gtk_window_set_child(widgets->operation_options_window, options_box);
    g_signal_connect(options_help, "clicked",
                     G_CALLBACK(settings_dialog_help), widgets);
    g_signal_connect(options_cancel, "clicked",
                     G_CALLBACK(settings_dialog_cancel), widgets);
    g_signal_connect(options_ok, "clicked",
                     G_CALLBACK(settings_dialog_accept), widgets);
    g_signal_connect(
        widgets->operation_options_window, "close-request",
        G_CALLBACK(settings_dialog_close_requested), widgets);
    widgets->selected_programmer_status = gtk_label_new(
        "Programmer: not detected");
    gtk_label_set_xalign(
        GTK_LABEL(widgets->selected_programmer_status), 0.0F);
    gtk_widget_set_hexpand(widgets->selected_programmer_status, TRUE);
    gtk_box_append(
        GTK_BOX(programmer_row), widgets->selected_programmer_status);
    gtk_box_append(GTK_BOX(operation_box), programmer_row);
    widgets->operation_device_status = gtk_label_new(
        "Selected device: none");
    gtk_label_set_xalign(
        GTK_LABEL(widgets->operation_device_status), 0.0F);
    gtk_label_set_wrap(
        GTK_LABEL(widgets->operation_device_status), TRUE);
    gtk_label_set_wrap_mode(
        GTK_LABEL(widgets->operation_device_status), PANGO_WRAP_WORD_CHAR);
    gtk_widget_add_css_class(widgets->operation_device_status, "title-4");
    gtk_box_append(
        GTK_BOX(operation_box), widgets->operation_device_status);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(message_scroll), widgets->central_log);
    gtk_scrolled_window_set_min_content_height(
        GTK_SCROLLED_WINDOW(message_scroll), 260);
    gtk_widget_set_vexpand(message_scroll, TRUE);
    gtk_frame_set_child(GTK_FRAME(message_frame), message_scroll);
    gtk_widget_set_vexpand(message_frame, TRUE);
    gtk_box_append(GTK_BOX(operation_box), message_frame);
    vendor_scroll = make_scrolled_list(&widgets->vendor_list);
    gtk_scrolled_window_set_min_content_height(
        GTK_SCROLLED_WINDOW(vendor_scroll), 150);
    gtk_widget_set_size_request(vendor_scroll, 190, -1);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(widgets->vendor_list),
                                    GTK_SELECTION_SINGLE);
    g_signal_connect(widgets->vendor_list, "row-selected",
                     G_CALLBACK(vendor_selected), widgets);
    scroll = make_scrolled_list(&widgets->device_list);
    gtk_scrolled_window_set_min_content_height(
        GTK_SCROLLED_WINDOW(scroll), 150);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(widgets->device_list),
                                    GTK_SELECTION_SINGLE);
    g_signal_connect(widgets->device_list, "row-selected",
                     G_CALLBACK(preview_device_selected), widgets);
    g_signal_connect(widgets->installation_entry, "changed",
                     G_CALLBACK(database_path_changed), widgets);
    gtk_frame_set_child(GTK_FRAME(vendor_frame), vendor_scroll);
    gtk_frame_set_child(GTK_FRAME(device_frame), scroll);
    gtk_widget_set_hexpand(device_frame, TRUE);
    gtk_box_append(GTK_BOX(selection_lists), vendor_frame);
    gtk_box_append(GTK_BOX(selection_lists), device_frame);
    gtk_box_append(GTK_BOX(selection_content), selection_lists);
    gtk_box_append(GTK_BOX(selection_content), selection_details);
    gtk_box_append(GTK_BOX(selection_content), guidance_row);
    gtk_box_append(GTK_BOX(select_box), selection_content);
    gtk_widget_set_halign(selection_actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(selection_actions), selection_help);
    gtk_box_append(GTK_BOX(selection_actions), selection_cancel);
    gtk_box_append(GTK_BOX(selection_actions), selection_done);
    gtk_box_append(GTK_BOX(select_box), selection_actions);
    gtk_window_set_child(widgets->device_select_window, select_box);
    g_signal_connect(selection_done, "clicked",
                     G_CALLBACK(selection_done_clicked), widgets);
    g_signal_connect(selection_cancel, "clicked",
                     G_CALLBACK(selection_cancel_clicked), widgets);
    g_signal_connect_swapped(selection_help, "clicked",
                             G_CALLBACK(gtk_window_present),
                             widgets->help_window);
    gtk_notebook_append_page(GTK_NOTEBOOK(workflow), operation_box,
                             gtk_label_new("Programmer"));
    gtk_box_append(GTK_BOX(page), quick_bar);
    gtk_widget_set_vexpand(workflow, TRUE);
    gtk_box_append(GTK_BOX(page), workflow);
    return page;
}

static GtkWidget *build_start_page(void) {
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *title = gtk_label_new("Safe first-run sequence");
    GtkWidget *text = gtk_label_new(
        "1. Use only an installation you own. Choose Select, then Manage… → "
        "Add Directory… to register the directory containing dev48uxp.rez and "
        "its matching definition REZ files. Select that database and verify it. "
        "The files remain local and are never packaged.\n\n"
        "2. Create a private cache from Manage Device Databases → Create private "
        "cache… (recommended), "
        "or run:\n"
        "   build-c/labtool48 cache import AEC/LabTool-48UXP PRIVATE-CACHE --json\n"
        "The native importer hash-addresses every REZ object and the proved idle-FPGA "
        "slice, records its executable source hash and offset, verifies the complete "
        "cache, and never copies WLT48UXP.exe. The resulting directory works directly "
        "with this GUI and CLI. The older Python JSON cache remains supported by the "
        "legacy analysis frontend only.\n\n"
        "3. Linux: install packaging/70-labtool48.rules through the package or an "
        "administrator. Power and connect the programmer. The application probes it "
        "at startup and probes again when an operation is requested after an offline "
        "start; Search Machine is available when manual selection is needed.\n\n"
        "4. Choose Select and select the exact device from the active database, or "
        "use Auto-ID and review every candidate. Verify package, pin count, bottom "
        "centering, adapter wiring and pin-1 orientation against the datasheet and a "
        "continuity-checked adapter.\n\n"
        "5. Use Load to fill the resident buffer before Program, Verify or Compare. "
        "Use Option and Parameters for persistent range, voltage, margin and algorithm "
        "settings. The ordinary toolbar actions run immediately after their required "
        "readiness, identity and safety checks; there is no Prepare/Execute handshake.\n\n"
        "6. Useful diagnostics before execution:\n"
        "   build-c/labtool48 version --json\n"
        "   build-c/labtool48 cache verify PRIVATE-CACHE --json\n"
        "   build-c/labtool48 programmer list --json\n"
        "   build-c/labtool48 catalogue support AEC/LabTool-48UXP VENDOR:DEVICE --json\n"
        "   build-c/labtool48 diagnostics exit-codes --json\n\n"
        "7. A command is enabled only when the selected database definition and native "
        "engine classify that exact operation as available. Unsupported Erase, Protect, "
        "Config, Vector and other specialist commands remain disabled. Cancellation "
        "stops forward work but mandatory electrical cleanup still completes. Existing "
        "read files require an explicit replacement decision.\n\n"
        "Component Composer and ISP/JTAG are explicitly offline planning/validation "
        "workspaces. They do not advertise live hardware execution."
    );
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_add_css_class(title, "title-3");
    gtk_label_set_xalign(GTK_LABEL(text), 0.0F);
    gtk_label_set_yalign(GTK_LABEL(text), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(text), TRUE);
    gtk_label_set_selectable(GTK_LABEL(text), TRUE);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);
    gtk_box_append(GTK_BOX(box), title);
    gtk_box_append(GTK_BOX(box), text);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    return scroll;
}

static GtkWindow *build_help_window(app_widgets *widgets) {
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *content = build_start_page();
    GtkWidget *done = gtk_button_new_with_label("Close");
    gtk_window_set_title(window, "LabTool-48UXP Help Topics");
    gtk_window_set_transient_for(window, widgets->window);
    gtk_window_set_destroy_with_parent(window, TRUE);
    gtk_window_set_hide_on_close(window, TRUE);
    gtk_window_set_default_size(window, 850, 650);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_widget_set_halign(done, GTK_ALIGN_END);
    gtk_widget_set_margin_bottom(done, 8);
    gtk_widget_set_margin_end(done, 8);
    gtk_box_append(GTK_BOX(box), content);
    gtk_box_append(GTK_BOX(box), done);
    gtk_window_set_child(window, box);
    g_signal_connect(done, "clicked", G_CALLBACK(hide_auxiliary_window),
                     window);
    return window;
}

static void jtag_plan_scan(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    lt48_jtag_scan_request request = {0};
    lt48_jtag_plan *plan = NULL;
    lt48_jtag_plan_info info;
    const unsigned char *json;
    size_t json_size;
    uint32_t bits;
    unsigned char *tdi;
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    GtkTextBuffer *preview;
    lt48_status status;
    (void)button;
    tdi = hex_bytes(gtk_editable_get_text(
        GTK_EDITABLE(widgets->jtag_tdi_entry)), &request.tdi_size);
    if (tdi == NULL ||
        !entry_u32(widgets->jtag_bits_entry, UINT32_MAX, &bits) || bits == 0u ||
        !entry_u32(widgets->jtag_tck_entry, UINT32_MAX, &request.tck_hz) ||
        !entry_u32(widgets->jtag_max_tck_entry, UINT32_MAX,
                   &request.profile_max_tck_hz)) {
        g_free(tdi);
        gtk_label_set_text(GTK_LABEL(widgets->jtag_status),
            "Enter even-length TDI hex, a nonzero bit count, TCK and profile maximum");
        return;
    }
    request.kind = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->jtag_kind_dropdown)) == 0u ?
        LT48_JTAG_SCAN_IR : LT48_JTAG_SCAN_DR;
    request.safe_end_state = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->jtag_end_dropdown)) == 0u ?
        LT48_JTAG_TEST_LOGIC_RESET : LT48_JTAG_RUN_TEST_IDLE;
    request.tdi = tdi;
    request.bit_count = bits;
    status = lt48_jtag_plan_scan(&request, &plan);
    g_free(tdi);
    if (status != LT48_OK) {
        message = g_strdup_printf(
            "Plan rejected: %s. TDI must contain exactly ceil(bits/8) bytes; "
            "unused high bits must be zero and TCK must not exceed the profile limit.",
            lt48_status_string(status));
        gtk_label_set_text(GTK_LABEL(widgets->jtag_status), message);
        g_free(message);
        return;
    }
    (void)lt48_jtag_plan_get_info(plan, &info);
    (void)lt48_jtag_plan_json(plan, &json, &json_size);
    digest_hex(info.sha256, LT48_SHA256_SIZE, digest);
    message = g_strdup_printf(
        "Offline %s scan · %zu bits · %zu TCK edges at %u Hz · %zu samples · "
        "safe end %s · transport unavailable · SHA-256 %s",
        info.kind == LT48_JTAG_SCAN_IR ? "IR" : "DR", info.bit_count,
        info.step_count, info.tck_hz, info.sampled_step_count,
        lt48_jtag_tap_state_name(info.final_state), digest);
    gtk_label_set_text(GTK_LABEL(widgets->jtag_status), message);
    g_free(message);
    preview = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->jtag_preview));
    gtk_text_buffer_set_text(preview, (const char *)json, (gint)json_size);
    lt48_jtag_plan_close(plan);
}

static void jtag_evaluate(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    lt48_jtag_result_request request = {0};
    lt48_jtag_result result;
    unsigned char *tdo = NULL;
    unsigned char *expected = NULL;
    unsigned char *mask = NULL;
    const char *expected_text;
    const char *mask_text;
    uint32_t bits;
    char *message;
    GString *output;
    (void)button;
    if (!entry_u32(widgets->jtag_bits_entry, UINT32_MAX, &bits) || bits == 0u)
        goto invalid;
    request.bit_count = bits;
    tdo = hex_bytes(gtk_editable_get_text(
        GTK_EDITABLE(widgets->jtag_tdo_entry)), &request.tdo_size);
    if (tdo == NULL) goto invalid;
    request.tdo = tdo;
    expected_text = gtk_editable_get_text(
        GTK_EDITABLE(widgets->jtag_expected_entry));
    mask_text = gtk_editable_get_text(GTK_EDITABLE(widgets->jtag_mask_entry));
    if (expected_text != NULL && expected_text[0] != '\0') {
        expected = hex_bytes(expected_text, &request.expected_size);
        if (expected == NULL) goto invalid;
        request.expected = expected;
    }
    if (mask_text != NULL && mask_text[0] != '\0') {
        mask = hex_bytes(mask_text, &request.mask_size);
        if (mask == NULL) goto invalid;
        request.mask = mask;
    }
    if (lt48_jtag_evaluate_result(&request, &result) != LT48_OK) goto invalid;
    message = g_strdup_printf(
        "TDO %s · compared %zu bits · stuck-low observed %s · stuck-high observed %s%s",
        result.compared ? (result.matched ? "matches" : "DIFFERS") :
                          "captured without expected value",
        result.compared_bit_count,
        result.stuck_low_observed ? "yes" : "no",
        result.stuck_high_observed ? "yes" : "no",
        result.has_mismatch ? " · first mismatch is shown below" : "");
    gtk_label_set_text(GTK_LABEL(widgets->jtag_status), message);
    g_free(message);
    output = g_string_new(NULL);
    g_string_append_printf(output,
        "{\"schema\":1,\"operation\":\"jtag-evaluate\","
        "\"bit_count\":%u,\"compared\":%s,\"matched\":%s,"
        "\"compared_bit_count\":%zu,\"stuck_low_observed\":%s,"
        "\"stuck_high_observed\":%s,\"first_mismatch\":",
        bits, result.compared ? "true" : "false",
        result.matched ? "true" : "false", result.compared_bit_count,
        result.stuck_low_observed ? "true" : "false",
        result.stuck_high_observed ? "true" : "false");
    if (result.has_mismatch)
        g_string_append_printf(output,
            "{\"bit\":%zu,\"expected\":%u,\"actual\":%u}",
            result.first_mismatch_bit, result.expected_bit, result.actual_bit);
    else g_string_append(output, "null");
    g_string_append(output, ",\"usb_opened\":false}");
    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->jtag_preview)),
        output->str, -1);
    g_string_free(output, TRUE);
    g_free(tdo); g_free(expected); g_free(mask);
    return;

invalid:
    g_free(tdo); g_free(expected); g_free(mask);
    gtk_label_set_text(GTK_LABEL(widgets->jtag_status),
        "TDO evaluation rejected: exact byte lengths, zero unused high bits, "
        "and an expected value before a mask are required");
}

static void jtag_decode_chain(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    unsigned char *tdo = NULL;
    size_t tdo_size = 0u;
    uint32_t bits;
    uint32_t count;
    lt48_jtag_chain_element *elements = NULL;
    lt48_jtag_chain_report report = {0};
    GString *output;
    char *message;
    lt48_status status;
    (void)button;
    if (!entry_u32(widgets->jtag_bits_entry, UINT32_MAX, &bits) || bits == 0u ||
        !entry_u32(widgets->jtag_chain_count_entry, UINT32_MAX, &count) ||
        count == 0u) goto invalid;
    tdo = hex_bytes(gtk_editable_get_text(
        GTK_EDITABLE(widgets->jtag_tdo_entry)), &tdo_size);
    if (tdo == NULL) goto invalid;
    elements = g_try_new0(lt48_jtag_chain_element, count);
    if (elements == NULL) goto invalid;
    status = lt48_jtag_decode_chain(tdo, tdo_size, bits, count,
                                    elements, count, &report);
    if (status != LT48_OK) goto invalid;
    message = g_strdup_printf("Decoded %zu chain elements from %zu bits",
                              report.decoded_element_count,
                              report.consumed_bit_count);
    gtk_label_set_text(GTK_LABEL(widgets->jtag_status), message);
    g_free(message);
    output = g_string_new("Decoded external-target chain (offline):\n");
    for (size_t index = 0u; index < report.decoded_element_count; ++index) {
        const lt48_jtag_chain_element *element = &elements[index];
        if (element->kind == LT48_JTAG_CHAIN_IDCODE)
            g_string_append_printf(output,
                "%zu: IDCODE 0x%08x · manufacturer 0x%03x · part 0x%04x · version %u\n",
                index, element->idcode, element->manufacturer_id,
                element->part_number, element->version);
        else g_string_append_printf(output, "%zu: BYPASS (one bit)\n", index);
    }
    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->jtag_preview)),
        output->str, -1);
    g_string_free(output, TRUE);
    g_free(elements); g_free(tdo);
    return;

invalid:
    g_free(elements); g_free(tdo);
    gtk_label_set_text(GTK_LABEL(widgets->jtag_status),
        "Chain decode rejected: supply an exact bounded capture and element count; "
        "zero denotes one BYPASS bit and one begins a complete 32-bit IDCODE");
}

static void jtag_validate_profile(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    lt48_jtag_target_profile profile = {0};
    lt48_jtag_profile_report report;
    uint8_t contacts[8];
    size_t count;
    uint32_t minimum;
    uint32_t maximum;
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    char *message;
    GString *output;
    lt48_status status;
    (void)button;
    profile.adapter_id = gtk_editable_get_text(
        GTK_EDITABLE(widgets->jtag_adapter_entry));
    profile.adapter_revision = gtk_editable_get_text(
        GTK_EDITABLE(widgets->jtag_revision_entry));
    if (!contact_list(gtk_editable_get_text(
            GTK_EDITABLE(widgets->jtag_contacts_entry)), contacts, &count) ||
        !entry_u32(widgets->jtag_vref_min_entry, UINT32_MAX, &minimum) ||
        !entry_u32(widgets->jtag_vref_max_entry, UINT32_MAX, &maximum) ||
        !entry_u32(widgets->jtag_max_tck_entry, UINT32_MAX,
                   &profile.maximum_tck_hz)) {
        gtk_label_set_text(GTK_LABEL(widgets->jtag_status),
            "Profile syntax rejected: enter 6..8 contacts and numeric VREF/TCK bounds");
        return;
    }
    profile.tck_contact = contacts[0];
    profile.tms_contact = contacts[1];
    profile.tdi_contact = contacts[2];
    profile.tdo_contact = contacts[3];
    profile.ground_contact = contacts[4];
    profile.vref_contact = contacts[5];
    profile.reset_contact = count >= 7u ? contacts[6] : 0u;
    profile.target_power_contact = count >= 8u ? contacts[7] : 0u;
    profile.power_ownership = (lt48_jtag_power_ownership)(
        LT48_JTAG_POWER_EXTERNAL + gtk_drop_down_get_selected(
            GTK_DROP_DOWN(widgets->jtag_power_dropdown)));
    profile.vref_min_millivolts = minimum;
    profile.vref_max_millivolts = maximum;
    profile.level_translation = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->jtag_level_dropdown)) == 0u ?
        LT48_JTAG_ASSURANCE_PRESENT : LT48_JTAG_ASSURANCE_NOT_REQUIRED;
    profile.contention_control = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->jtag_contention_dropdown)) == 0u ?
        LT48_JTAG_ASSURANCE_PRESENT : LT48_JTAG_ASSURANCE_NOT_REQUIRED;
    profile.target_reset = (lt48_jtag_target_reset_policy)(
        LT48_JTAG_RESET_NOT_CONNECTED + gtk_drop_down_get_selected(
            GTK_DROP_DOWN(widgets->jtag_reset_dropdown)));
    profile.disconnect_high_impedance = 1;
    profile.disconnect_target_power_off = gtk_drop_down_get_selected(
        GTK_DROP_DOWN(widgets->jtag_disconnect_dropdown)) == 1u;
    status = lt48_jtag_validate_target_profile(&profile, &report);
    if (status == LT48_OK)
        digest_hex(report.sha256, LT48_SHA256_SIZE, digest);
    message = g_strdup_printf("Target profile %s: %s%s%s",
        status == LT48_OK ? "valid" : "REJECTED",
        report.diagnostic, status == LT48_OK ? " · SHA-256 " : "",
        status == LT48_OK ? digest : "");
    gtk_label_set_text(GTK_LABEL(widgets->jtag_status), message);
    g_free(message);
    output = g_string_new(NULL);
    g_string_append_printf(output,
        "{\"schema\":1,\"operation\":\"jtag-validate-profile\","
        "\"valid\":%s,\"issue\":\"%s\",\"diagnostic\":\"%s\","
        "\"mapped_contact_count\":%zu,\"profile_sha256\":",
        status == LT48_OK ? "true" : "false",
        lt48_jtag_profile_issue_name(report.issue), report.diagnostic,
        report.mapped_contact_count);
    if (status == LT48_OK) g_string_append_printf(output, "\"%s\"", digest);
    else g_string_append(output, "null");
    g_string_append(output,
        ",\"transport_ready\":false,\"usb_opened\":false}");
    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->jtag_preview)),
        output->str, -1);
    g_string_free(output, TRUE);
}

static GtkWidget *build_jtag_page(app_widgets *widgets) {
    static const char *kinds[] = {"IR scan", "DR scan", NULL};
    static const char *ends[] = {"Safe end: TAP reset", "Safe end: idle", NULL};
    static const char *powers[] = {
        "Target power: external", "Target power: adapter",
        "Target power: programmer", NULL
    };
    static const char *level_assurances[] = {
        "Level translation present", "Level translation proved unnecessary", NULL
    };
    static const char *contention_assurances[] = {
        "Contention control present", "Contention control proved unnecessary", NULL
    };
    static const char *resets[] = {
        "Reset not connected", "Hold reset during connect", "Reset released", NULL
    };
    static const char *disconnects[] = {
        "Disconnect: high-Z", "Disconnect: high-Z + target power off", NULL
    };
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *plan = gtk_button_new_with_label("Build offline TAP plan");
    GtkWidget *evaluate = gtk_button_new_with_label("Evaluate TDO");
    GtkWidget *decode = gtk_button_new_with_label("Decode chain");
    GtkWidget *validate_profile =
        gtk_button_new_with_label("Validate target profile");
    GtkWidget *notice = gtk_label_new(
        "Plan-only JTAG foundation. TDI is shifted least-significant bit first "
        "and TDO is sampled on every scan edge. No socket mapping, target "
        "voltage, level translation or LabTool transport is assumed; USB is never opened. "
        "TDO comparison and bounded IDCODE/BYPASS decoding operate on pasted captures."
    );
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    widgets->jtag_kind_dropdown = gtk_drop_down_new_from_strings(kinds);
    widgets->jtag_tdi_entry = gtk_entry_new();
    widgets->jtag_bits_entry = gtk_entry_new();
    widgets->jtag_tck_entry = gtk_entry_new();
    widgets->jtag_max_tck_entry = gtk_entry_new();
    widgets->jtag_end_dropdown = gtk_drop_down_new_from_strings(ends);
    widgets->jtag_tdo_entry = gtk_entry_new();
    widgets->jtag_expected_entry = gtk_entry_new();
    widgets->jtag_mask_entry = gtk_entry_new();
    widgets->jtag_chain_count_entry = gtk_entry_new();
    widgets->jtag_adapter_entry = gtk_entry_new();
    widgets->jtag_revision_entry = gtk_entry_new();
    widgets->jtag_contacts_entry = gtk_entry_new();
    widgets->jtag_power_dropdown = gtk_drop_down_new_from_strings(powers);
    widgets->jtag_vref_min_entry = gtk_entry_new();
    widgets->jtag_vref_max_entry = gtk_entry_new();
    widgets->jtag_level_dropdown =
        gtk_drop_down_new_from_strings(level_assurances);
    widgets->jtag_contention_dropdown =
        gtk_drop_down_new_from_strings(contention_assurances);
    widgets->jtag_reset_dropdown = gtk_drop_down_new_from_strings(resets);
    widgets->jtag_disconnect_dropdown =
        gtk_drop_down_new_from_strings(disconnects);
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->jtag_tdi_entry),
                                   "TDI bytes as hex, e.g. 0b");
    gtk_editable_set_text(GTK_EDITABLE(widgets->jtag_bits_entry), "4");
    gtk_editable_set_text(GTK_EDITABLE(widgets->jtag_tck_entry), "100000");
    gtk_editable_set_text(GTK_EDITABLE(widgets->jtag_max_tck_entry), "100000");
    gtk_editable_set_text(GTK_EDITABLE(widgets->jtag_chain_count_entry), "1");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->jtag_tdo_entry),
                                   "captured TDO hex");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->jtag_expected_entry),
                                   "optional expected hex");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->jtag_mask_entry),
                                   "optional comparison mask hex");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->jtag_adapter_entry),
                                   "adapter stable id");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->jtag_revision_entry),
                                   "revision");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->jtag_contacts_entry),
        "TCK,TMS,TDI,TDO,GND,VREF[,RESET[,POWER]] contacts");
    gtk_editable_set_text(GTK_EDITABLE(widgets->jtag_vref_min_entry), "1800");
    gtk_editable_set_text(GTK_EDITABLE(widgets->jtag_vref_max_entry), "5000");
    gtk_widget_set_hexpand(widgets->jtag_tdi_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_kind_dropdown, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("TDI hex:"), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_tdi_entry, 2, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Bits:"), 5, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_bits_entry, 6, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("TCK Hz:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_tck_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Profile max:"), 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_max_tck_entry, 3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_end_dropdown, 4, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), plan, 6, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("TDO:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_tdo_entry, 1, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Expected:"), 3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_expected_entry, 4, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), evaluate, 6, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Mask:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_mask_entry, 1, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Chain elements:"), 3, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_chain_count_entry, 4, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), decode, 5, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Adapter/revision:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_adapter_entry, 1, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_revision_entry, 3, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_contacts_entry, 4, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_power_dropdown, 0, 5, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("VREF min/max mV:"), 2, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_vref_min_entry, 3, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_vref_max_entry, 4, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_level_dropdown, 5, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_contention_dropdown, 6, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_reset_dropdown, 0, 6, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->jtag_disconnect_dropdown, 2, 6, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), validate_profile, 5, 6, 2, 1);
    gtk_box_append(GTK_BOX(box), grid);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_widget_add_css_class(notice, "dim-label");
    gtk_box_append(GTK_BOX(box), notice);
    widgets->jtag_status = gtk_label_new("No JTAG plan built");
    gtk_label_set_xalign(GTK_LABEL(widgets->jtag_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->jtag_status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->jtag_status), TRUE);
    gtk_box_append(GTK_BOX(box), widgets->jtag_status);
    widgets->jtag_preview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->jtag_preview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(widgets->jtag_preview), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(widgets->jtag_preview), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),
                                  widgets->jtag_preview);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    g_signal_connect(plan, "clicked", G_CALLBACK(jtag_plan_scan), widgets);
    g_signal_connect(evaluate, "clicked", G_CALLBACK(jtag_evaluate), widgets);
    g_signal_connect(decode, "clicked", G_CALLBACK(jtag_decode_chain), widgets);
    g_signal_connect(validate_profile, "clicked",
                     G_CALLBACK(jtag_validate_profile), widgets);
    return box;
}

static gboolean deferred_window_close(gpointer user_data);

static gboolean clear_window_focus_before_close(
    GtkWindow *window, gpointer user_data
) {
    app_widgets *widgets = user_data;
    gtk_window_set_focus(window, NULL);
    if (widgets->active_background_tasks == 0u) return FALSE;
    if (!widgets->close_pending) {
        widgets->close_pending = 1;
        if (widgets->read_cancellable != NULL) {
            cancel_read(NULL, widgets);
            if (widgets->operation_progress_window != NULL)
                gtk_window_present(widgets->operation_progress_window);
        }
        if (widgets->buffer_cancellable != NULL) {
            if (widgets->active_buffer_dialog != NULL)
                buffer_dialog_request_cancel(
                    widgets, widgets->active_buffer_dialog);
            else
                buffer_cancel_io(NULL, widgets);
        }
        if (widgets->catalogue_cancellable != NULL)
            cancel_catalogue_task(NULL, widgets);
        if (widgets->operation_prepare_cancellable != NULL)
            cancel_operation_preparation(NULL, widgets);
        if (widgets->image_inspect_cancellable != NULL) {
            g_cancellable_cancel(widgets->image_inspect_cancellable);
            gtk_label_set_text(
                GTK_LABEL(widgets->image_status),
                "Closing after image inspection reaches a safe boundary…");
        }
        if (widgets->component_cancellable != NULL)
            component_cancel_long(NULL, widgets);
        if (widgets->read_preparation != NULL &&
            widgets->read_cancellable == NULL)
            gtk_label_set_text(
                GTK_LABEL(widgets->read_preparation),
                "Closing after active background work completes…");
    }
    if (widgets->close_wait_source_id == 0u)
        widgets->close_wait_source_id = g_timeout_add(
            50u, deferred_window_close, widgets);
    return TRUE;
}

static gboolean deferred_window_close(gpointer user_data) {
    app_widgets *widgets = user_data;
    if (widgets->active_background_tasks != 0u) return G_SOURCE_CONTINUE;
    widgets->close_wait_source_id = 0u;
    widgets->close_pending = 0;
    gtk_window_close(widgets->window);
    return G_SOURCE_REMOVE;
}

static void shell_action(
    GSimpleAction *action, GVariant *parameter, gpointer user_data
) {
    app_widgets *widgets = user_data;
    const char *name = g_action_get_name(G_ACTION(action));
    gint page = PAGE_DEVICES;
    (void)parameter;
    if (g_str_has_prefix(name, "device-filter-")) {
        static const char *const filter_names[] = {
            "device-filter-all", "device-filter-eprom", "device-filter-prom",
            "device-filter-pld", "device-filter-mpu"
        };
        for (guint index = 0u; index < G_N_ELEMENTS(filter_names); ++index) {
            if (strcmp(name, filter_names[index]) == 0) {
                gtk_check_button_set_active(
                    GTK_CHECK_BUTTON(widgets->device_type_filters[index]), TRUE);
                break;
            }
        }
        return;
    }
    if (strcmp(name, "save") == 0) {
        toolbar_save_clicked(NULL, widgets);
        return;
    }
    if (strcmp(name, "load") == 0) {
        toolbar_load_clicked(NULL, widgets);
        return;
    }
    if (strcmp(name, "auto-id") == 0) {
        quick_auto_id_clicked(NULL, widgets);
        return;
    }
    if (strcmp(name, "edit") == 0) {
        toolbar_edit_clicked(NULL, widgets);
        return;
    } else if (strcmp(name, "vector") == 0) {
        open_vector_editor(widgets);
        return;
    } else if (strcmp(name, "edit-e-field") == 0) {
        open_auxiliary_fuse_editor(widgets, FUSE_BINDING_E_FIELD);
        return;
    } else if (strcmp(name, "edit-user-code") == 0) {
        open_auxiliary_fuse_editor(widgets, FUSE_BINDING_USER_CODE);
        return;
    } else if (strcmp(name, "edit-data-buffer") == 0) {
        secondary_buffer_edit_window(widgets, SECONDARY_BUFFER_DATA);
        return;
    } else if (strcmp(name, "edit-encryption-array") == 0) {
        secondary_buffer_edit_window(widgets, SECONDARY_BUFFER_ENCRYPTION);
        return;
    } else if (strcmp(name, "blank") == 0 &&
             widgets->quick_operation_buttons[1] != NULL) {
        gtk_widget_activate(widgets->quick_operation_buttons[1]);
        return;
    } else if (strcmp(name, "read") == 0 &&
               widgets->quick_operation_buttons[0] != NULL) {
        gtk_widget_activate(widgets->quick_operation_buttons[0]);
        return;
    } else if (strcmp(name, "program") == 0 &&
               widgets->quick_operation_buttons[2] != NULL) {
        gtk_widget_activate(widgets->quick_operation_buttons[2]);
        return;
    } else if (strcmp(name, "over-program") == 0) {
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->operation_dropdown), 3u);
        prepare_read(NULL, widgets);
        return;
    } else if (strcmp(name, "verify") == 0 &&
               widgets->quick_operation_buttons[4] != NULL) {
        gtk_widget_activate(widgets->quick_operation_buttons[4]);
        return;
    } else if (strcmp(name, "erase") == 0 &&
               widgets->quick_operation_buttons[6] != NULL) {
        gtk_widget_activate(widgets->quick_operation_buttons[6]);
        return;
    } else if (strcmp(name, "compare") == 0 &&
               widgets->quick_operation_buttons[5] != NULL) {
        gtk_widget_activate(widgets->quick_operation_buttons[5]);
        return;
    } else if (strcmp(name, "secure") == 0) {
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->operation_dropdown), 7u);
        prepare_read(NULL, widgets);
        return;
    } else if (strcmp(name, "memory-protect") == 0) {
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->operation_dropdown), 8u);
        prepare_read(NULL, widgets);
        return;
    } else if (strcmp(name, "protect") == 0) {
        GAction *memory_protect = g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "memory-protect");
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->operation_dropdown),
            memory_protect != NULL && g_action_get_enabled(memory_protect) ?
                8u : 7u);
        prepare_read(NULL, widgets);
        return;
    } else if (strcmp(name, "config") == 0 &&
               widgets->quick_operation_buttons[9] != NULL) {
        gtk_widget_activate(widgets->quick_operation_buttons[9]);
        return;
    } else if (strcmp(name, "function-test") == 0) {
        gtk_drop_down_set_selected(
            GTK_DROP_DOWN(widgets->operation_dropdown), 10u);
        prepare_read(NULL, widgets);
        return;
    }
    if (strcmp(name, "exit") == 0) {
        gtk_window_close(widgets->window);
        return;
    }
    if (strcmp(name, "database-manager") == 0) {
        present_database_manager(NULL, widgets);
        return;
    }
    if (strcmp(name, "mass-production") == 0) {
        gtk_label_set_text(
            GTK_LABEL(widgets->mass_setup_status),
            "Live mass-production execution is not implemented. OK previews "
            "the recovered monitor only; it cannot operate the programmer.");
        gtk_window_present(widgets->mass_setup_window);
        return;
    }
    if (strcmp(name, "project-save") == 0) {
        project_save_dialog(widgets);
        return;
    }
    if (strcmp(name, "project-load") == 0) {
        project_load_dialog(widgets);
        return;
    }
    if (strcmp(name, "search-machine") == 0) {
        refresh_programmers(NULL, widgets);
        if (widgets->machine_select_window != NULL)
            gtk_window_present(widgets->machine_select_window);
        return;
    }
    if (strcmp(name, "statistic") == 0) {
        char value[32];
        g_snprintf(value, sizeof(value), "%u",
                   widgets->statistic_target_count);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->statistics_target_entry), value);
        g_snprintf(value, sizeof(value), "%u",
                   widgets->statistic_max_failure);
        gtk_editable_set_text(
            GTK_EDITABLE(widgets->statistics_max_failure_entry), value);
        gtk_check_button_set_active(
            GTK_CHECK_BUTTON(widgets->statistics_enabled_check),
            widgets->statistic_enabled);
        gtk_check_button_set_active(
            GTK_CHECK_BUTTON(widgets->statistics_alarm_check),
            widgets->statistic_alarm_enabled);
        gtk_window_present(widgets->statistics_window);
        return;
    }
    if (strcmp(name, "self-test") == 0) {
        if (widgets->self_test_window != NULL)
            gtk_window_present(widgets->self_test_window);
        return;
    }
    if (strcmp(name, "about") == 0) {
        GtkWidget *about = gtk_about_dialog_new();
        gtk_about_dialog_set_program_name(
            GTK_ABOUT_DIALOG(about), "LabTool-48UXP");
        gtk_about_dialog_set_comments(
            GTK_ABOUT_DIALOG(about),
            "Independent Linux programmer software using the portable C core");
        gtk_window_set_transient_for(GTK_WINDOW(about), widgets->window);
        gtk_window_present(GTK_WINDOW(about));
        return;
    }
    if (strcmp(name, "help-topics") == 0) {
        if (widgets->help_window != NULL) {
            gtk_window_set_transient_for(
                widgets->help_window, widgets->window);
            gtk_window_present(widgets->help_window);
        }
        return;
    }
    if (strcmp(name, "reports") == 0)
        page = PAGE_REPORTS;
    else if (strcmp(name, "traces") == 0)
        page = PAGE_TRACES;
    else if (strcmp(name, "hardware-debug") == 0)
        page = PAGE_HARDWARE_DEBUG;
    else if (strcmp(name, "component-composer") == 0)
        page = PAGE_COMPONENTS;
    else if (strcmp(name, "jtag") == 0)
        page = PAGE_JTAG;
    if (widgets->main_notebook != NULL)
        gtk_notebook_set_current_page(
            GTK_NOTEBOOK(widgets->main_notebook), page);
    if (page == PAGE_DEVICES && widgets->device_workflow != NULL) {
        gtk_notebook_set_current_page(
            GTK_NOTEBOOK(widgets->device_workflow), 0);
        if (strcmp(name, "change-device") == 0 &&
            widgets->device_select_window != NULL) {
            gtk_window_present(widgets->device_select_window);
            start_change_device_browse(widgets);
        }
        if (strcmp(name, "operation") == 0)
            open_operation_option_editor(widgets);
        if (strcmp(name, "parameters") == 0 &&
            widgets->programming_parameters_window != NULL) {
            refresh_programming_parameters(widgets);
            present_settings_dialog(
                widgets, widgets->programming_parameters_window);
        }
    }
}

static GtkWidget *build_menu_bar(app_widgets *widgets) {
#define SHELL_ACTION(action_name) \
    { .name = action_name, .activate = shell_action }
    static const GActionEntry actions[] = {
        SHELL_ACTION("save"),
        SHELL_ACTION("load"),
        SHELL_ACTION("exit"),
        SHELL_ACTION("project-save"),
        SHELL_ACTION("project-load"),
        SHELL_ACTION("change-device"),
        SHELL_ACTION("device-filter-all"),
        SHELL_ACTION("device-filter-eprom"),
        SHELL_ACTION("device-filter-prom"),
        SHELL_ACTION("device-filter-pld"),
        SHELL_ACTION("device-filter-mpu"),
        SHELL_ACTION("database-manager"),
        SHELL_ACTION("mass-production"),
        SHELL_ACTION("component-composer"),
        SHELL_ACTION("parameters"),
        SHELL_ACTION("operation"),
        SHELL_ACTION("statistic"),
        SHELL_ACTION("search-machine"),
        SHELL_ACTION("self-test"),
        SHELL_ACTION("reports"),
        SHELL_ACTION("traces"),
        SHELL_ACTION("hardware-debug"),
        SHELL_ACTION("jtag"),
        SHELL_ACTION("help-topics"),
        SHELL_ACTION("about"),
        SHELL_ACTION("auto-id"),
        SHELL_ACTION("edit"),
        SHELL_ACTION("vector"),
        SHELL_ACTION("blank"),
        SHELL_ACTION("read"),
        SHELL_ACTION("program"),
        SHELL_ACTION("over-program"),
        SHELL_ACTION("verify"),
        SHELL_ACTION("erase"),
        SHELL_ACTION("compare"),
        SHELL_ACTION("secure"),
        SHELL_ACTION("memory-protect"),
        SHELL_ACTION("protect"),
        SHELL_ACTION("config"),
        SHELL_ACTION("function-test"),
        SHELL_ACTION("edit-e-field"),
        SHELL_ACTION("edit-user-code"),
        SHELL_ACTION("edit-data-buffer"),
        SHELL_ACTION("edit-encryption-array")
    };
#undef SHELL_ACTION
    GMenu *root = g_menu_new();
    GMenu *file = g_menu_new();
    GMenu *file_buffer = g_menu_new();
    GMenu *file_exit = g_menu_new();
    GMenu *project = g_menu_new();
    GMenu *device = g_menu_new();
    GMenu *device_selection = g_menu_new();
    GMenu *device_editors = g_menu_new();
    GMenu *device_operations = g_menu_new();
    GMenu *device_configuration = g_menu_new();
    GMenu *device_extensions = g_menu_new();
    GMenu *options = g_menu_new();
    GMenu *diagnostics = g_menu_new();
    GMenu *help = g_menu_new();
    GMenu *help_topics = g_menu_new();
    GMenu *help_about = g_menu_new();
    GtkWidget *bar;
    const char *developer_setting = g_getenv("LABTOOL48_DEVELOPER");
    gboolean developer_mode = developer_setting != NULL &&
        developer_setting[0] != '\0' &&
        g_strcmp0(developer_setting, "0") != 0;
    g_action_map_add_action_entries(
        G_ACTION_MAP(widgets->window), actions, G_N_ELEMENTS(actions), widgets);
    g_simple_action_set_enabled(
        G_SIMPLE_ACTION(g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "vector")), FALSE);
    g_simple_action_set_enabled(
        G_SIMPLE_ACTION(g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "save")), FALSE);
    g_simple_action_set_enabled(
        G_SIMPLE_ACTION(g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "edit")), FALSE);
    g_simple_action_set_enabled(
        G_SIMPLE_ACTION(g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "mass-production")), FALSE);
    {
        static const char *const unavailable_family_actions[] = {
            "edit-data-buffer", "edit-encryption-array"
        };
        for (size_t index = 0u;
             index < G_N_ELEMENTS(unavailable_family_actions); ++index)
            g_simple_action_set_enabled(
                G_SIMPLE_ACTION(g_action_map_lookup_action(
                    G_ACTION_MAP(widgets->window),
                    unavailable_family_actions[index])), FALSE);
    }
    g_simple_action_set_enabled(
        G_SIMPLE_ACTION(g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "traces")), developer_mode);
    g_simple_action_set_enabled(
        G_SIMPLE_ACTION(g_action_map_lookup_action(
            G_ACTION_MAP(widgets->window), "hardware-debug")),
        developer_mode);
    g_menu_append(file_buffer, "Save", "win.save");
    g_menu_append(file_buffer, "Load", "win.load");
    g_menu_append(file_exit, "Exit", "win.exit");
    g_menu_append_section(file, NULL, G_MENU_MODEL(file_buffer));
    g_menu_append_section(file, NULL, G_MENU_MODEL(file_exit));
    g_menu_append(project, "Save Project", "win.project-save");
    g_menu_append(project, "Load Project", "win.project-load");
    g_menu_append(device_selection, "Change", "win.change-device");
    g_menu_append(
        device_selection, "Auto Select EPROM", "win.auto-id");
    g_menu_append(
        device_selection, "Mass Production Mode", "win.mass-production");
    g_menu_append(device_editors, "Edit", "win.edit");
    g_menu_append(device_editors, "Modify Vectors", "win.vector");
    g_menu_append(device_editors, "Edit E-Field", "win.edit-e-field");
    g_menu_append(device_editors, "Edit User Code", "win.edit-user-code");
    g_menu_append(
        device_editors, "Edit Data Buffer", "win.edit-data-buffer");
    g_menu_append(
        device_editors, "Edit Encryption Array",
        "win.edit-encryption-array");
    g_menu_append(device_operations, "Read", "win.read");
    g_menu_append(device_operations, "Blank Check", "win.blank");
    g_menu_append(device_operations, "Program Auto", "win.program");
    g_menu_append(device_operations, "Over-program", "win.over-program");
    g_menu_append(device_operations, "Verify", "win.verify");
    g_menu_append(device_operations, "Secure", "win.secure");
    g_menu_append(
        device_operations, "Memory Protect/Program Config",
        "win.memory-protect");
    g_menu_append(
        device_operations, "Function Test", "win.function-test");
    g_menu_append(device_operations, "Erase", "win.erase");
    g_menu_append(device_operations, "Compare", "win.compare");
    g_menu_append(
        device_configuration, "Configuration", "win.config");
    g_menu_append(
        device_extensions, "Manage Device Databases…",
        "win.database-manager");
    g_menu_append(device_extensions, "Component Composer (offline)",
                  "win.component-composer");
    g_menu_append_section(device, NULL, G_MENU_MODEL(device_selection));
    g_menu_append_section(device, NULL, G_MENU_MODEL(device_editors));
    g_menu_append_section(device, NULL, G_MENU_MODEL(device_operations));
    g_menu_append_section(device, NULL, G_MENU_MODEL(device_configuration));
    g_menu_append_section(device, NULL, G_MENU_MODEL(device_extensions));
    g_menu_append(options, "Parameters", "win.parameters");
    g_menu_append(options, "Operation", "win.operation");
    g_menu_append(options, "Search Machine", "win.search-machine");
    g_menu_append(options, "Statistic", "win.statistic");
    g_menu_append(options, "ISP/JTAG Planning (offline)", "win.jtag");
    g_menu_append(diagnostics, "Self Test", "win.self-test");
    g_menu_append(diagnostics, "Reports", "win.reports");
    if (developer_mode) {
        g_menu_append(diagnostics, "Trace Inspector", "win.traces");
        g_menu_append(diagnostics, "Hardware Debug", "win.hardware-debug");
    }
    g_menu_append(help_topics, "Help Topics", "win.help-topics");
    g_menu_append(help_about, "About", "win.about");
    g_menu_append_section(help, NULL, G_MENU_MODEL(help_topics));
    g_menu_append_section(help, NULL, G_MENU_MODEL(help_about));
    g_menu_append_submenu(root, "File", G_MENU_MODEL(file));
    g_menu_append_submenu(root, "Project", G_MENU_MODEL(project));
    g_menu_append_submenu(root, "Device", G_MENU_MODEL(device));
    g_menu_append_submenu(root, "Options", G_MENU_MODEL(options));
    g_menu_append_submenu(root, "Diagnostics", G_MENU_MODEL(diagnostics));
    g_menu_append_submenu(root, "Help", G_MENU_MODEL(help));
    refresh_operation_dropdown(widgets);
    bar = gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(root));
    g_object_unref(help_about);
    g_object_unref(help_topics);
    g_object_unref(help);
    g_object_unref(diagnostics);
    g_object_unref(options);
    g_object_unref(device_extensions);
    g_object_unref(device_configuration);
    g_object_unref(device_operations);
    g_object_unref(device_editors);
    g_object_unref(device_selection);
    g_object_unref(device);
    g_object_unref(project);
    g_object_unref(file_exit);
    g_object_unref(file_buffer);
    g_object_unref(file);
    g_object_unref(root);
    return bar;
}

static void statistics_apply(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    uint32_t target;
    uint32_t maximum_failures;
    (void)button;
    if (!entry_u32(widgets->statistics_target_entry, UINT32_MAX, &target) ||
        !entry_u32(widgets->statistics_max_failure_entry, UINT32_MAX,
                   &maximum_failures)) {
        gtk_label_set_text(GTK_LABEL(widgets->statistics_status),
            "Target and maximum failure counts must be unsigned integers.");
        return;
    }
    widgets->statistic_target_count = target;
    widgets->statistic_max_failure = maximum_failures;
    widgets->statistic_enabled = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->statistics_enabled_check));
    widgets->statistic_alarm_enabled = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->statistics_alarm_check));
    statistics_update_labels(widgets);
    gtk_label_set_text(GTK_LABEL(widgets->statistics_status),
        widgets->statistic_enabled ?
            "Statistics enabled; completed live operations update the counters." :
            "Statistics disabled; existing counters are retained.");
    gtk_widget_set_visible(GTK_WIDGET(widgets->statistics_window), FALSE);
}

static void statistics_toggle(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    widgets->statistic_enabled = !widgets->statistic_enabled;
    if (widgets->statistics_enabled_check != NULL)
        gtk_check_button_set_active(
            GTK_CHECK_BUTTON(widgets->statistics_enabled_check),
            widgets->statistic_enabled);
    gtk_button_set_label(button,
        widgets->statistic_enabled ? "On" : "Off");
}

static void statistics_reset(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    widgets->statistic_current_count = 0u;
    widgets->statistic_current_failure = 0u;
    statistics_update_labels(widgets);
    if (widgets->statistics_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->statistics_status),
                           "Current and failure counters reset.");
}

static void statistics_acknowledge(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    (void)button;
    if (widgets->statistics_status != NULL)
        gtk_label_set_text(GTK_LABEL(widgets->statistics_status),
                           "Statistic alarm acknowledged.");
}

static gboolean mass_parse_count(
    GtkWidget *entry, const char *name, guint64 *value,
    GtkWidget *status_label
) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    char *end = NULL;
    guint64 parsed;
    errno = 0;
    parsed = g_ascii_strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed == 0u) {
        char *message = g_strdup_printf(
            "%s must be a positive whole number.", name);
        gtk_label_set_text(GTK_LABEL(status_label), message);
        g_free(message);
        return FALSE;
    }
    *value = parsed;
    return TRUE;
}

static void mass_setup_apply(GtkButton *button, gpointer user_data) {
    app_widgets *widgets = user_data;
    gboolean unlimited = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->mass_unlimited_check));
    gboolean stop_failure = gtk_check_button_get_active(
        GTK_CHECK_BUTTON(widgets->mass_failure_check));
    guint64 success_count = 0u;
    guint64 failure_count = 0u;
    char success_value[32];
    char failure_value[32];
    (void)button;
    if (!unlimited && !mass_parse_count(
            widgets->mass_success_entry, "Success Device No.",
            &success_count, widgets->mass_setup_status))
        return;
    if (stop_failure && !mass_parse_count(
            widgets->mass_failure_entry, "Max. Failed Device No.",
            &failure_count, widgets->mass_setup_status))
        return;
    g_snprintf(success_value, sizeof(success_value),
               "%" G_GUINT64_FORMAT, success_count);
    g_snprintf(failure_value, sizeof(failure_value),
               "%" G_GUINT64_FORMAT, failure_count);
    gtk_label_set_text(
        GTK_LABEL(widgets->mass_monitor_stop_total),
        unlimited ? "None" : success_value);
    gtk_label_set_text(
        GTK_LABEL(widgets->mass_monitor_stop_failure),
        stop_failure ? failure_value : "None");
    gtk_label_set_text(
        GTK_LABEL(widgets->mass_monitor_status),
        "PREVIEW ONLY — live repetition is unavailable and no programmer "
        "operation will be started.\nPlease remove device on socket !");
    gtk_window_close(widgets->mass_setup_window);
    gtk_window_present(widgets->mass_monitor_window);
}

static GtkWindow *build_mass_monitor_window(app_widgets *widgets) {
    GtkWidget *window = gtk_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *socket_frame = gtk_frame_new("48-pin socket");
    GtkWidget *socket = gtk_grid_new();
    GtkWidget *table = gtk_grid_new();
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *stop = gtk_button_new_with_label("Stop");
    GtkWidget *ignore = gtk_button_new_with_label("Ignore");
    GtkWidget *total_current = gtk_label_new("0");
    GtkWidget *failure_current = gtk_label_new("0");
    widgets->mass_monitor_status = gtk_label_new(
        "PREVIEW ONLY — live repetition is unavailable and no programmer "
        "operation will be started.\nPlease remove device on socket !");
    widgets->mass_monitor_stop_total = gtk_label_new("None");
    widgets->mass_monitor_stop_failure = gtk_label_new("None");
    gtk_window_set_title(GTK_WINDOW(window), "Mass Production Mode");
    gtk_window_set_transient_for(GTK_WINDOW(window), widgets->window);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_hide_on_close(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 520, 390);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_grid_set_row_spacing(GTK_GRID(socket), 6);
    gtk_grid_set_column_spacing(GTK_GRID(socket), 12);
    gtk_widget_set_margin_top(socket, 10);
    gtk_widget_set_margin_bottom(socket, 10);
    gtk_widget_set_margin_start(socket, 10);
    gtk_widget_set_margin_end(socket, 10);
    gtk_grid_attach(GTK_GRID(socket), gtk_label_new("48"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(socket), gtk_label_new("25"), 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(socket),
                    gtk_separator_new(GTK_ORIENTATION_VERTICAL),
                    1, 0, 1, 2);
    gtk_grid_attach(GTK_GRID(socket), gtk_label_new("01"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(socket), gtk_label_new("24"), 2, 1, 1, 1);
    gtk_frame_set_child(GTK_FRAME(socket_frame), socket);
    gtk_grid_set_row_spacing(GTK_GRID(table), 6);
    gtk_grid_set_column_spacing(GTK_GRID(table), 18);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new("Item"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new("Stop Point"),
                    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new("Current"), 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new("Total Count"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(table), widgets->mass_monitor_stop_total,
                    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(table), total_current, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(table), gtk_label_new("Failure"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(table), widgets->mass_monitor_stop_failure,
                    1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(table), failure_current, 2, 2, 1, 1);
    gtk_label_set_xalign(GTK_LABEL(widgets->mass_monitor_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->mass_monitor_status), TRUE);
    gtk_widget_set_sensitive(ignore, FALSE);
    gtk_widget_set_tooltip_text(
        ignore, "Available only while a live mass-production cycle is running.");
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(actions), ignore);
    gtk_box_append(GTK_BOX(actions), stop);
    gtk_box_append(GTK_BOX(box), widgets->mass_monitor_status);
    gtk_box_append(GTK_BOX(box), socket_frame);
    gtk_box_append(GTK_BOX(box), table);
    gtk_box_append(GTK_BOX(box), gtk_label_new("A device on socket !"));
    gtk_box_append(GTK_BOX(box),
                   gtk_label_new("Automatic increment disabled !"));
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect_swapped(stop, "clicked",
                             G_CALLBACK(gtk_window_close), window);
    return GTK_WINDOW(window);
}

static GtkWindow *build_mass_setup_window(app_widgets *widgets) {
    GtkWidget *window = gtk_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *ok = gtk_button_new_with_label("OK");
    GtkWidget *cancel = gtk_button_new_with_label("Cancel");
    widgets->mass_unlimited_check = gtk_check_button_new_with_label(
        "None Stop - Until user break !");
    widgets->mass_success_check = gtk_check_button_new_with_label(
        "Stop on Success Device No. Reached");
    widgets->mass_success_entry = gtk_entry_new();
    widgets->mass_failure_check = gtk_check_button_new_with_label(
        "Stop on Limited Failed Device Reached");
    widgets->mass_failure_entry = gtk_entry_new();
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->mass_success_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Success Device No.", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->mass_failure_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Max. Failed Device No.", -1);
    widgets->mass_setup_status = gtk_label_new(
        "Live mass-production execution is not implemented. OK previews the "
        "recovered monitor only; it cannot operate the programmer.");
    gtk_window_set_title(GTK_WINDOW(window), "Mass Production Control");
    gtk_window_set_transient_for(GTK_WINDOW(window), widgets->window);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_hide_on_close(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 560, 330);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_check_button_set_group(
        GTK_CHECK_BUTTON(widgets->mass_success_check),
        GTK_CHECK_BUTTON(widgets->mass_unlimited_check));
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(widgets->mass_unlimited_check), TRUE);
    g_object_bind_property(
        widgets->mass_success_check, "active",
        widgets->mass_success_entry, "sensitive",
        G_BINDING_SYNC_CREATE);
    g_object_bind_property(
        widgets->mass_failure_check, "active",
        widgets->mass_failure_entry, "sensitive",
        G_BINDING_SYNC_CREATE);
    gtk_editable_set_text(GTK_EDITABLE(widgets->mass_success_entry), "100");
    gtk_editable_set_text(GTK_EDITABLE(widgets->mass_failure_entry), "5");
    gtk_entry_set_input_purpose(GTK_ENTRY(widgets->mass_success_entry),
                                GTK_INPUT_PURPOSE_DIGITS);
    gtk_entry_set_input_purpose(GTK_ENTRY(widgets->mass_failure_entry),
                                GTK_INPUT_PURPOSE_DIGITS);
    gtk_grid_attach(GTK_GRID(grid), widgets->mass_unlimited_check,
                    0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->mass_success_check,
                    0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Success Device No."),
                    0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->mass_success_entry,
                    1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->mass_failure_check,
                    0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Max. Failed Device No."),
                    0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->mass_failure_entry,
                    1, 4, 1, 1);
    gtk_label_set_xalign(GTK_LABEL(widgets->mass_setup_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->mass_setup_status), TRUE);
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(actions), cancel);
    gtk_box_append(GTK_BOX(actions), ok);
    gtk_widget_add_css_class(ok, "suggested-action");
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), widgets->mass_setup_status);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect(ok, "clicked", G_CALLBACK(mass_setup_apply), widgets);
    g_signal_connect_swapped(cancel, "clicked",
                             G_CALLBACK(gtk_window_close), window);
    return GTK_WINDOW(window);
}

static GtkWindow *build_statistics_window(app_widgets *widgets) {
    GtkWidget *window = gtk_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *apply = gtk_button_new_with_label("Apply");
    GtkWidget *cancel = gtk_button_new_with_label("Cancel");
    widgets->statistics_target_entry = gtk_entry_new();
    widgets->statistics_max_failure_entry = gtk_entry_new();
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->statistics_target_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Target count", -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(widgets->statistics_max_failure_entry),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Maximum failures", -1);
    widgets->statistics_enabled_check =
        gtk_check_button_new_with_label("Count completed live operations");
    widgets->statistics_alarm_check =
        gtk_check_button_new_with_label("Enable target/failure alarm");
    widgets->statistics_status = gtk_label_new(
        "Zero target or maximum failure disables that threshold.");
    gtk_window_set_title(GTK_WINDOW(window), "Statistic Configuration");
    gtk_window_set_transient_for(GTK_WINDOW(window), widgets->window);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_hide_on_close(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 480, 280);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_editable_set_text(GTK_EDITABLE(widgets->statistics_target_entry), "0");
    gtk_editable_set_text(
        GTK_EDITABLE(widgets->statistics_max_failure_entry), "0");
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Target count:"),
                    0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->statistics_target_entry,
                    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Maximum failures:"),
                    0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->statistics_max_failure_entry,
                    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->statistics_enabled_check,
                    0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->statistics_alarm_check,
                    0, 3, 2, 1);
    gtk_label_set_xalign(GTK_LABEL(widgets->statistics_status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widgets->statistics_status), TRUE);
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(actions), cancel);
    gtk_box_append(GTK_BOX(actions), apply);
    gtk_widget_add_css_class(apply, "suggested-action");
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), widgets->statistics_status);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect(apply, "clicked", G_CALLBACK(statistics_apply), widgets);
    g_signal_connect_swapped(cancel, "clicked",
                             G_CALLBACK(gtk_window_close), window);
    return GTK_WINDOW(window);
}

static GtkWindow *build_operation_progress_window(app_widgets *widgets) {
    GtkWidget *window = gtk_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    widgets->operation_progress_heading = gtk_label_new("Operation in progress");
    widgets->operation_progress_status = gtk_label_new(
        "Waiting for the hardware owner…");
    widgets->operation_progress_bar = gtk_progress_bar_new();
    widgets->operation_progress_cancel =
        gtk_button_new_with_label("Cancel operation");
    gtk_window_set_title(GTK_WINDOW(window), "Operation Progress");
    gtk_window_set_transient_for(GTK_WINDOW(window), widgets->window);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_hide_on_close(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 520, 220);
    gtk_widget_set_margin_top(box, 16);
    gtk_widget_set_margin_bottom(box, 16);
    gtk_widget_set_margin_start(box, 16);
    gtk_widget_set_margin_end(box, 16);
    gtk_label_set_xalign(
        GTK_LABEL(widgets->operation_progress_heading), 0.0F);
    gtk_widget_add_css_class(widgets->operation_progress_heading, "title-3");
    gtk_label_set_xalign(
        GTK_LABEL(widgets->operation_progress_status), 0.0F);
    gtk_label_set_wrap(
        GTK_LABEL(widgets->operation_progress_status), TRUE);
    gtk_progress_bar_set_show_text(
        GTK_PROGRESS_BAR(widgets->operation_progress_bar), TRUE);
    gtk_widget_set_halign(actions, GTK_ALIGN_END);
    gtk_box_append(
        GTK_BOX(actions), widgets->operation_progress_cancel);
    gtk_box_append(
        GTK_BOX(box), widgets->operation_progress_heading);
    gtk_box_append(GTK_BOX(box), widgets->operation_progress_bar);
    gtk_box_append(GTK_BOX(box), widgets->operation_progress_status);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect(
        widgets->operation_progress_cancel, "clicked",
        G_CALLBACK(cancel_read), widgets);
    g_signal_connect(
        window, "close-request",
        G_CALLBACK(operation_progress_close_requested), widgets);
    return GTK_WINDOW(window);
}

static GtkWidget *footer_value(const char *text, GtkWidget **out) {
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(label, TRUE);
    *out = label;
    return label;
}

static GtkWidget *build_device_footer(app_widgets *widgets) {
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *on_off = gtk_button_new_with_label("Off");
    GtkWidget *reset = gtk_button_new_with_label("Reset");
    GtkWidget *alarm = gtk_button_new_with_label("Alarm");
    GtkWidget *config = gtk_button_new_with_label("Config");
    widgets->footer_statistics_toggle = on_off;
    const char *path = widgets->installation_entry == NULL ? NULL :
        gtk_editable_get_text(GTK_EDITABLE(widgets->installation_entry));
    char *base = path == NULL || path[0] == '\0' ?
        g_strdup("none") : g_path_get_basename(path);
    char *database = g_strdup_printf("%s — not loaded", base);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_widget_set_margin_top(grid, 6);
    gtk_widget_set_margin_bottom(grid, 6);
    gtk_widget_set_margin_start(grid, 8);
    gtk_widget_set_margin_end(grid, 8);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Database:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value(database, &widgets->footer_database), 1, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Device:"), 4, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("None", &widgets->footer_device), 5, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Adapter:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_package), 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Pin:"), 3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_pin), 4, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Size:"), 5, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_capacity), 6, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Check Sum:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_checksum), 1, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("VCC:"), 3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_voltage), 4, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("VPP:"), 5, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_vpp), 6, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Manu. Code:"),
                    0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_manufacturer_code), 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Device Code:"),
                    2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_device_code), 3, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("File:"), 4, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("No file", &widgets->footer_file), 5, 3, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Note:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("—", &widgets->footer_note), 1, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Programmer:"), 4, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("Offline", &widgets->footer_programmer), 5, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Current Count:"),
                    0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("0", &widgets->footer_current_count), 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Target Count:"),
                    2, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("0", &widgets->footer_target_count), 3, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Current Failure:"),
                    4, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("0", &widgets->footer_current_failure), 5, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Max Failure:"),
                    6, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
        footer_value("0", &widgets->footer_max_failure), 7, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), on_off, 4, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), reset, 5, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), alarm, 6, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), config, 7, 6, 1, 1);
    g_signal_connect(on_off, "clicked", G_CALLBACK(statistics_toggle), widgets);
    g_signal_connect(reset, "clicked", G_CALLBACK(statistics_reset), widgets);
    g_signal_connect(alarm, "clicked",
                     G_CALLBACK(statistics_acknowledge), widgets);
    g_signal_connect_swapped(config, "clicked",
                             G_CALLBACK(gtk_window_present),
                             widgets->statistics_window);
    gtk_frame_set_child(GTK_FRAME(frame), grid);
    g_free(database);
    g_free(base);
    return frame;
}

static void set_accelerator(
    GtkApplication *application, const char *action, const char *accelerator
) {
    const char *values[] = {accelerator, NULL};
    gtk_application_set_accels_for_action(application, action, values);
}

static void install_original_accelerators(GtkApplication *application) {
    const char *help_accelerators[] = {"F1", "<Shift>F1", NULL};
    set_accelerator(application, "win.save", "<Alt>s");
    set_accelerator(application, "win.load", "<Alt>l");
    set_accelerator(application, "win.exit", "<Alt>x");
    set_accelerator(application, "win.change-device", "<Alt>c");
    set_accelerator(application, "win.project-save", "<Alt>F1");
    set_accelerator(application, "win.project-load", "<Alt>F2");
    set_accelerator(application, "win.auto-id", "<Alt>a");
    set_accelerator(application, "win.edit", "<Alt>e");
    set_accelerator(application, "win.vector", "<Alt>m");
    set_accelerator(application, "win.blank", "<Alt>b");
    set_accelerator(application, "win.read", "<Alt>r");
    set_accelerator(application, "win.program", "<Alt>p");
    set_accelerator(application, "win.verify", "<Alt>v");
    set_accelerator(application, "win.erase", "<Primary>F1");
    set_accelerator(application, "win.compare", "<Primary>F3");
    set_accelerator(application, "win.config", "<Alt>g");
    set_accelerator(application, "win.function-test", "<Alt>t");
    set_accelerator(application, "win.parameters", "F3");
    set_accelerator(application, "win.operation", "F4");
    set_accelerator(application, "win.search-machine", "F5");
    set_accelerator(application, "win.statistic", "F6");
    set_accelerator(application, "win.self-test", "F7");
    gtk_application_set_accels_for_action(
        application, "win.help-topics", help_accelerators);
}

static void install_application_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(
        provider,
        "button.quick-command {"
        "  min-width: 0;"
        "  min-height: 28px;"
        "  padding: 4px 7px;"
        "}");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

static void activate(GtkApplication *application, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *outer;
    GtkWidget *menu_bar;
    GtkWidget *notebook;
    GtkWidget *hardware_debug;
    GtkWidget *self_test;
    GtkWidget *footer;
    app_widgets *widgets = user_data;
    if (widgets->window != NULL) {
        gtk_window_present(widgets->window);
        return;
    }
    window = gtk_application_window_new(application);
    widgets->window = GTK_WINDOW(window);
    widgets->project_description_window =
        build_project_description_window(widgets);
    widgets->operation_progress_window =
        build_operation_progress_window(widgets);
    widgets->statistics_window = build_statistics_window(widgets);
    widgets->mass_monitor_window = build_mass_monitor_window(widgets);
    widgets->mass_setup_window = build_mass_setup_window(widgets);
    widgets->self_test_window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(widgets->self_test_window, "Hardware Self-Test");
    gtk_window_set_transient_for(widgets->self_test_window, widgets->window);
    gtk_window_set_destroy_with_parent(widgets->self_test_window, TRUE);
    gtk_window_set_modal(widgets->self_test_window, TRUE);
    gtk_window_set_hide_on_close(widgets->self_test_window, TRUE);
    gtk_window_set_default_size(widgets->self_test_window, 760, 520);
    widgets->help_window = build_help_window(widgets);
    install_application_css();
    g_signal_connect(
        window, "close-request",
        G_CALLBACK(clear_window_focus_before_close), widgets);
    gtk_window_set_title(GTK_WINDOW(window), "LabTool-48UXP");
    gtk_window_set_default_size(GTK_WINDOW(window), 1100, 700);
    outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_bottom(outer, 6);
    gtk_widget_set_margin_start(outer, 6);
    gtk_widget_set_margin_end(outer, 6);
    gtk_window_set_child(GTK_WINDOW(window), outer);
    menu_bar = build_menu_bar(widgets);
    install_original_accelerators(application);
    gtk_box_append(GTK_BOX(outer), menu_bar);
    notebook = gtk_notebook_new();
    widgets->main_notebook = notebook;
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
    gtk_widget_set_vexpand(notebook, TRUE);
    hardware_debug = build_programmer_page(widgets);
    self_test = build_diagnostics_page(widgets);
    gtk_window_set_child(widgets->self_test_window, self_test);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_start_page(),
                             gtk_label_new("Start"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_device_page(widgets),
                             gtk_label_new("Devices"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_buffer_page(widgets),
                             gtk_label_new("Buffer"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_report_page(widgets),
                             gtk_label_new("Reports"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_trace_page(widgets),
                             gtk_label_new("Traces"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
                             gtk_box_new(GTK_ORIENTATION_VERTICAL, 0),
                             gtk_label_new("Diagnostics"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), hardware_debug,
                             gtk_label_new("Hardware debug"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_component_page(widgets),
                             gtk_label_new("Components"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_jtag_page(widgets),
                             gtk_label_new("JTAG/ISP"));
    gtk_box_append(GTK_BOX(outer), notebook);
    footer = build_device_footer(widgets);
    gtk_box_append(GTK_BOX(outer), footer);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), PAGE_DEVICES);
    gtk_window_present(GTK_WINDOW(window));
    refresh_programmers(NULL, widgets);
}

static void open_files(
    GtkApplication *application, GFile **files, gint file_count,
    const char *hint, gpointer user_data
) {
    app_widgets *widgets = user_data;
    GError *error = NULL;
    (void)hint;
    if (widgets->window == NULL) activate(application, widgets);
    if (file_count > 0 && files != NULL) {
        char *path = g_file_get_path(files[0]);
        char *lower = path == NULL ? NULL : g_ascii_strdown(path, -1);
        if (lower != NULL && g_str_has_suffix(lower, ".lt48project")) {
            if (!project_load_file(widgets, path, &error) && error != NULL)
                project_status(widgets, error->message);
        } else if (selected_fuse_editor_available(widgets)) {
            open_fuse_context_action(
                widgets, FUSE_CONTEXT_LOAD_JEDEC, files[0]);
        } else {
            present_load_image_file(widgets, files[0]);
        }
        g_clear_error(&error);
        g_free(lower);
        g_free(path);
    }
    if (widgets->window != NULL) gtk_window_present(widgets->window);
}

int main(int argc, char **argv) {
    GtkApplication *application;
    app_widgets widgets = {0};
    int result;
    g_mutex_init(&widgets.progress_mutex);
    application = gtk_application_new(
        "com.labtool48uxp.LabTool48", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(application, "activate", G_CALLBACK(activate), &widgets);
    g_signal_connect(application, "open", G_CALLBACK(open_files), &widgets);
    result = g_application_run(G_APPLICATION(application), argc, argv);
    if (widgets.catalogue_cancellable != NULL)
        g_cancellable_cancel(widgets.catalogue_cancellable);
    g_clear_object(&widgets.catalogue_cancellable);
    if (widgets.operation_prepare_cancellable != NULL)
        g_cancellable_cancel(widgets.operation_prepare_cancellable);
    g_clear_object(&widgets.operation_prepare_cancellable);
    if (widgets.image_inspect_cancellable != NULL)
        g_cancellable_cancel(widgets.image_inspect_cancellable);
    g_clear_object(&widgets.image_inspect_cancellable);
    operation_prepare_task_free(widgets.prepared_operation);
    g_clear_object(&widgets.read_preparation);
    lt48_buffer_close(widgets.device_buffer);
    lt48_buffer_close(widgets.secondary_buffer);
    lt48_test_vectors_close(widgets.test_vectors);
    fuse_session_reset(&widgets);
    lt48_property_document_close(widgets.operation_option_document);
    lt48_installation_close(widgets.installation);
    g_clear_pointer(&widgets.device_result_indices, g_array_unref);
    g_clear_pointer(&widgets.auto_id_candidate_evidence, g_array_unref);
    g_clear_pointer(&widgets.buffer_edit_undo_data, g_free);
    operation_options_snapshot_clear(&widgets);
    g_clear_pointer(&widgets.database_paths, g_ptr_array_unref);
    g_clear_pointer(&widgets.pending_project_query, g_free);
    clear_pending_project_device(&widgets);
    clear_pending_project_operation_options(&widgets);
    g_object_unref(application);
    g_mutex_clear(&widgets.progress_mutex);
    return result;
}
