#ifndef LT48_PIC_SOURCE_SESSION_H
#define LT48_PIC_SOURCE_SESSION_H
#include <labtool48/labtool48.h>
#include "range_execution.h"
#include "configuration_composition.h"
#include "addtab_map.h"

/* Shared source callback owner. Compiled callbacks and pin map are borrowed;
 * mutable variables, image, configuration and scratch buffers are owned.
 * The caller supplies the runner (transport or offline model), source-bound
 * entry/cleanup and admission policy. No programmer is opened here. */
typedef struct lt48_pic_source_session lt48_pic_source_session;
typedef struct {
    const lt48_compile_variable *variables;size_t variable_count;
    const lt48_runtime_configuration_value *configuration;size_t configuration_count;
    const unsigned char *image;size_t image_size;
    const unsigned char (*buffers)[32];
    uint32_t voltages[4];
    uint16_t applet_stream_role;int applet_stream_selected;
} lt48_pic_source_state;
typedef lt48_status (*lt48_pic_source_runner)(void *,lt48_runtime_machine *,lt48_runtime_result **);
typedef lt48_status (*lt48_pic_source_start)(void *,lt48_pic_source_session *,uint16_t,uint32_t);
typedef struct {
    /* Public host image extent can be smaller than the source callback frame.
     * These values must come from the selected source profile. */
    size_t logical_image_size;
    uint32_t reset_mask, option_flags;
    int started;
    lt48_pic_source_start start;
    lt48_pic_source_runner runner;
    lt48_pic_range_cancelled cancelled;
    void *context;
} lt48_pic_source_range_options;
typedef lt48_status (*lt48_pic_source_selector_write)(void *,uint8_t);
typedef lt48_status (*lt48_pic_source_ready)(void *);
typedef struct {
    uint16_t voltage_variable, poweron_role, init_role, reset_role, poweroff_role;
    uint16_t timeout_seconds;
    uint32_t voltage_centivolts, minimum_centivolts, maximum_centivolts;
    lt48_pic_source_selector_write selector_write;
    lt48_pic_source_ready ready;
    lt48_pic_source_runner runner;
    void *context;
} lt48_pic_source_lifecycle;

lt48_status lt48_pic_source_session_create_internal(
    const lt48_pin_map *,const lt48_compile_variable *,size_t,
    const lt48_runtime_nested_wave *,size_t,const lt48_runtime_options *,
    lt48_pic_source_session **);
void lt48_pic_source_session_close_internal(lt48_pic_source_session *);
/* Compact PIC host buffer roles: address=2, observed=5, expected=6, mode=9.
 * Expected/observed elements are in/out and may be NULL for non-element calls.
 * Successful and explicitly failed finished callbacks retain their final state
 * for cleanup. Unfinished runner failures do not commit speculative buffers.
 * Element outputs change only on success. No hardware rollback is implied. */
lt48_status lt48_pic_source_session_invoke_internal(
    lt48_pic_source_session *,uint16_t,uint32_t,uint32_t,
    unsigned char *,unsigned char *,size_t,lt48_pic_source_runner,void *);
lt48_status lt48_pic_source_session_image_internal(
    lt48_pic_source_session *,const unsigned char **,size_t *);
lt48_status lt48_pic_source_session_buffer_internal(
    lt48_pic_source_session *,unsigned,const unsigned char **);
/* Change a source voltage declaration without changing its identity/channel. */
lt48_status lt48_pic_source_session_voltage_value_internal(lt48_pic_source_session *,uint16_t,uint32_t);
lt48_status lt48_pic_source_session_variable_internal(
    lt48_pic_source_session *,uint16_t,lt48_compile_variable *);
/* View pointers remain valid only until the next mutation/close. Restore
 * changes mutable state atomically; callback/pin bindings and geometry remain
 * fixed. This allows an operation owner to stage configuration/preservation
 * changes without replacing the common callback executor. */
lt48_status lt48_pic_source_session_state_internal(lt48_pic_source_session *,lt48_pic_source_state *);
lt48_status lt48_pic_source_session_restore_internal(lt48_pic_source_session *,const lt48_pic_source_state *);
/* Clone complete mutable state while borrowing the same immutable source
 * callback/pin bindings. Both sessions must close before their resource owner.
 * This stages configuration and preservation without speculative mutations. */
/* Opt in only after proving this callback and its nested callbacks use no
 * device image. A hidden image read/write fails closed; replacement waves
 * do not inherit the optimization. Owned image and transactional commit stay. */
lt48_status lt48_pic_source_session_omit_callback_image_internal(lt48_pic_source_session *,uint16_t,int);
lt48_status lt48_pic_source_session_clone_internal(const lt48_pic_source_session *,lt48_pic_source_session **);
/* Bind the original family's declared buffer count and compiler-derived reset
 * flags. Bound sessions use these for every host mode change; caller-supplied
 * reset masks cannot override the source preservation flags. */
lt48_status lt48_pic_source_session_bind_frame_internal(lt48_pic_source_session *,const lt48_runtime_frame_profile *);
/* Immutable applet role/address bindings, copied into this owner and clones.
 * Compact and applet roles cannot overlap. Payload upload belongs to transport
 * preparation; arbitrary unresolved EXECUTE_WAVE events remain rejected. */
lt48_status lt48_pic_source_session_bind_applets_internal(lt48_pic_source_session *,
    const lt48_applet_address *,size_t);
lt48_status lt48_pic_source_session_applets_internal(const lt48_pic_source_session *,
    const lt48_applet_address **,size_t *);
/* Source applet host 00465870: optional declared-frame reset, then source
 * ChipSize, Address and signed 16-bit Mode writes. The host supplies reset
 * and dense capacity explicitly; this does not use compact mode-5 remapping. */
lt48_status lt48_pic_source_session_applet_mode_internal(lt48_pic_source_session *,
    uint16_t,int,uint32_t,uint32_t,uint8_t);
typedef struct {uint16_t read,program,verify,blank;} lt48_pic_source_applet_host_roles;
/* Immutable DefineName roles for real applet host override 0046aec0.
 * Selection does not imply that the selected family's callback exists. */
lt48_status lt48_pic_source_session_bind_applet_host_internal(lt48_pic_source_session *,
    const lt48_pic_source_applet_host_roles *);
lt48_status lt48_pic_source_session_applet_stream_role_internal(const lt48_pic_source_session *,uint16_t *);
lt48_status lt48_pic_source_session_applet_invoke_selected_internal(lt48_pic_source_session *,
    uint32_t,lt48_pic_source_runner,void *);
/* The three outputs of original AddTab helpers 0046a640/0046a720. Buffer
 * roles come from DefineName; device addresses and image offsets remain
 * distinct. Only their DWORDs change; Mode and other scratch state persist. */
lt48_status lt48_pic_source_session_applet_span_internal(lt48_pic_source_session *,
    const lt48_pic_map_span *,uint8_t,uint8_t);

/* Source callback dispatch without a new host mode/address boundary. These
 * values are read from the persistent frame, including changes by prior
 * callbacks. Unknown roles reject before frame mutation or runner activity. */
int lt48_pic_source_session_has_callback_internal(const lt48_pic_source_session *,uint16_t);
lt48_status lt48_pic_source_session_invoke_current_internal(lt48_pic_source_session *,
    uint16_t,uint32_t,lt48_pic_source_runner,void *);
/* Original applet wrapper 004688e0: retain the frame, update host mode/address/
 * capacity, then execute the explicit source callback with a ten-second
 * watchdog. UI logging is outside the operation engine. */
lt48_status lt48_pic_source_session_applet_callback_internal(lt48_pic_source_session *,
    uint16_t,uint16_t,uint32_t,uint32_t,uint8_t,lt48_pic_source_runner,void *);
/* Decode with the source PreConfig, apply only existing source-schema fields,
 * then encode with PostConfig. Runs without any transport, rejecting hardware
 * events/captures. Commits image/configuration/variables/scratch together only
 * on complete success. Explicit roles and field domains are source-derived;
 * no silicon preservation or operation admission is inferred. */
lt48_status lt48_pic_source_session_configuration_internal(lt48_pic_source_session *,
    uint16_t,uint16_t,uint16_t,uint32_t,const lt48_pic_composition_field *,size_t,
    const lt48_runtime_configuration_value *,size_t);
typedef lt48_status (*lt48_pic_configuration_completion)(void *,lt48_pic_source_session *);
/* Source Pre, qualified missing-field completion, source Post. All work is
 * cloned and committed atomically; completion never runs a transport. */
lt48_status lt48_pic_source_session_configuration_complete_internal(
    lt48_pic_source_session *,uint16_t,uint16_t,uint16_t,uint32_t,
    lt48_pic_configuration_completion,void *);
/* Qualified owners may select an authored root replacement while retaining
 * the complete source nested table. NULL selects the source-bound root.
 * Timeout is explicit; owned expected/observed buffers are used unchanged. */
lt48_status lt48_pic_source_session_invoke_wave_internal(
    lt48_pic_source_session *,uint16_t,const lt48_compiled_wave *,uint32_t,uint32_t,
    uint32_t,lt48_pic_source_runner,void *);
/* Executes the shared host traversal using source callbacks, including
 * Pre/PostConfig and persistent nested state. Does not infer region masks,
 * power entry, cleanup or physical verification from a part name. The caller
 * must unconditionally clean up its hardware session on every outcome. */
lt48_status lt48_pic_source_session_range_internal(
    lt48_pic_source_session *,const lt48_pic_range_contract *,uint32_t,uint32_t,
    const lt48_pic_source_range_options *,lt48_pic_range_execution_result *);
/* Shared erase/configuration transaction order and per-step timeout. Uses
 * the same source frame and startup contract as range operations. */
lt48_status lt48_pic_source_session_transaction_internal(
    lt48_pic_source_session *,const lt48_pic_transaction_plan *,
    const lt48_pic_source_range_options *);
/* Source host voltage selector: <= 400 cV selects 7; above selects 15. */
lt48_status lt48_pic_source_voltage_selector_internal(uint32_t,uint8_t *);
/* The owner initializes transport/FPGA and frame mode first. This begins
 * source POWERON, two readiness checks and DEVINIT. Pin/ID policy lives in
 * source callbacks and their qualified owner; no admission is implied. */
lt48_status lt48_pic_source_session_begin_internal(
    lt48_pic_source_session *,const lt48_pic_source_lifecycle *,uint16_t,uint32_t);
/* Both source cleanup callbacks are attempted even after the first fails.
 * Global DAC-off/release/LED cleanup still belongs to the transport owner. */
lt48_status lt48_pic_source_session_end_internal(
    lt48_pic_source_session *,const lt48_pic_source_lifecycle *,uint16_t,
    lt48_status,lt48_status *);
/* Borrowed immutable resource-owner identity; retained by clones. */
lt48_status lt48_pic_source_session_bind_origin_internal(lt48_pic_source_session *,const void *);
int lt48_pic_source_session_matches_origin_internal(const lt48_pic_source_session *,const void *);
#endif
