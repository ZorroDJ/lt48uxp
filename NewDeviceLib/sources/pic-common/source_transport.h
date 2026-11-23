#ifndef LT48_PIC_SOURCE_TRANSPORT_H
#define LT48_PIC_SOURCE_TRANSPORT_H
#include "source_operation.h"
#include "pic18_j_program.h"
#include "pic18_j_rewrite.h"
#include "addtab_map.h"
#include "source_applet_action.h"
#include "source_applet_configuration.h"
#include "source_applet_program_sequence.h"
#include "source_contact_masks.h"
#include "source_applet_startup_sequence.h"
#include "source_applet_identity_session.h"

typedef struct lt48_pic_source_transport lt48_pic_source_transport;
/* Injected production protocol transport. Borrows source session and callback
 * context; owns protocol queues. Creation performs no I/O; closing an unused
 * prepared owner releases its hardware settings before freeing queues. */
lt48_status lt48_pic_source_transport_create_internal(const lt48_protocol_transport *,void *,
    lt48_pic_source_session *,const lt48_pic_source_host_binding *,uint32_t,uint32_t,uint32_t,
    lt48_pic_source_transport **);
void lt48_pic_source_transport_close_internal(lt48_pic_source_transport *);
lt48_status lt48_pic_source_transport_cleanup_status_internal(const lt48_pic_source_transport *);
/* Bind caller-verified idle bytes before preparation. Copied into the owner;
 * restored on every finish after rail-off and before release/result LED. */
lt48_status lt48_pic_source_transport_idle_internal(lt48_pic_source_transport *,const unsigned char *,size_t);
/* Explicit source-selected FPGA, including the source idle load where required.
 * Every public operation requires fresh preparation; no failed queue is reused. */
lt48_status lt48_pic_source_transport_prepare_internal(lt48_pic_source_transport *,const unsigned char *,size_t);
typedef struct {
    uint16_t role,address;
    const unsigned char *data;size_t size;
} lt48_pic_source_applet_payload;
/* Shared upload sequence with an explicit post-FPGA selector. The caller
 * validates source identity/bindings; this helper validates all ranges before
 * I/O. It owns no power or cleanup policy. */
lt48_status lt48_pic_source_transport_load_internal(lt48_runtime_transport_session *,
    const unsigned char *,size_t,const lt48_pic_source_applet_payload *,size_t,uint8_t);
/* Upload exact session-bound applets in the caller's audited role order,
 * then load the explicit FPGA. Reject missing/changed bindings, invalid ranges
 * and conflicting overlapping payloads before any I/O. Each preparation is
 * fresh; a download failure performs independent global cleanup. */
lt48_status lt48_pic_source_transport_prepare_applets_internal(lt48_pic_source_transport *,
    const unsigned char *,size_t,const lt48_pic_source_applet_payload *,size_t);
/* Upload bound applets and retain FPGA without loading it or selecting voltage.
 * The composite startup owner loads FPGA only after contact acceptance. */
lt48_status lt48_pic_source_transport_prepare_startup_internal(lt48_pic_source_transport *,
    const unsigned char *,size_t,const lt48_pic_source_applet_payload *,size_t);
/* Prepared, unpowered startup substage. Captures with source-selected masks,
 * then reloads the exact retained FPGA before reporting contact match state.
 * Alternate payload comes from the user's pinned original executable. */
/* Deferred standalone contact gate. Accept reloads the retained selected FPGA;
 * rejection/failure releases all settings without powering the target. */
lt48_status lt48_pic_source_transport_require_contact_internal(lt48_pic_source_transport *,
    const lt48_pic_contact_masks *,lt48_pic_contact_result *);
lt48_status lt48_pic_source_transport_contact_internal(lt48_pic_source_transport *,
    const lt48_pic_contact_masks *,const unsigned char *,size_t,lt48_pic_contact_result *,int *);
/* Byte-stream route used by the admitted source PIC18 host: command 43 for
 * read, 48/mode 1 for write, completion 0c/1. Other stream grammars require a
 * separately audited protocol capability; byte counts are already packed by
 * the host's address/region policy. This does not infer a device strategy. */
typedef enum {LT48_PIC_STREAM_BYTES_48=1} lt48_pic_source_stream_protocol;
/* Original 00465e80 worker chunk policy, restricted to positive nonoverflowing
 * element counts/widths; returns zero for invalid input. */
uint32_t lt48_pic_source_stream_chunk_internal(uint32_t,uint16_t);
lt48_status lt48_pic_source_stream_internal(lt48_runtime_transport_session *,
    const lt48_protocol_transport *,void *,lt48_pic_source_stream_protocol,int,
    unsigned char *,size_t);
lt48_status lt48_pic_source_transport_stream_internal(lt48_pic_source_transport *,
    lt48_pic_source_stream_protocol,int,unsigned char *,size_t);
typedef lt48_status (*lt48_pic_source_stream_begin)(void *,int,const lt48_pic_map_span *);
typedef struct {uint32_t completed_bytes,failed_dense_offset;} lt48_pic_source_stream_result;
/* Traverse an explicit dense range across source AddTab regions, preserving
 * canonical-image and physical-device addresses as separate callback inputs.
 * Validate the entire range/backing/count framing before any callback or I/O.
 * Reads commit complete blocks; the operation owner stages its public image.
 * Host address units, applet role choice and preservation stay source-bound. */
lt48_status lt48_pic_source_mapped_stream_internal(lt48_runtime_transport_session *,
    const lt48_protocol_transport *,void *,lt48_pic_source_stream_protocol,int,
    const lt48_pic_addtab_map *,uint32_t,uint32_t,unsigned char *,size_t,
    lt48_pic_source_stream_begin,void *,lt48_pic_source_stream_result *);
/* Source chunk policy, one callback per span and write completion per span.
 * Exposed separately for differential worker audits and prepared owners. */
lt48_status lt48_pic_source_mapped_worker_internal(lt48_runtime_transport_session *,
    const lt48_protocol_transport *,void *,lt48_pic_source_stream_protocol,int,
    const lt48_pic_addtab_map *,uint32_t,uint32_t,unsigned char *,size_t,
    lt48_pic_source_stream_begin,void *,lt48_pic_source_stream_result *);
/* Original applet Blank Check: one callback per mapped span, 16-bit element
 * count through command 44, result command 0B, and a remaining-element word
 * through command 43 on mismatch. No expected image or host-side FF guess.
 * Reject malformed mismatch positions and preserve transport failures. */
lt48_status lt48_pic_source_mapped_blank_internal(lt48_runtime_transport_session *,
    const lt48_protocol_transport *,void *,const lt48_pic_addtab_map *,uint32_t,uint32_t,
    lt48_pic_source_stream_begin,void *,lt48_pic_source_stream_result *);
lt48_status lt48_pic_source_transport_run_internal(lt48_pic_source_transport *,
    const lt48_pic_source_operation_contract *,uint16_t,uint32_t,uint32_t,
    const lt48_pic_source_range_options *,lt48_pic_source_operation_result *);
/* Bind a prepared compact transport to a device-specific complete owner.
 * The callback receives this transport's production I/O and global release.
 * It must execute the exact supplied session/origin; no I/O services are
 * accepted from the caller. Deferred/startup-bound routes require the separate
 * composite startup service and reject here. Early rejection still releases
 * an already prepared transport. This service grants no device admission. */
typedef lt48_status (*lt48_pic_source_transport_compact_owner)(void *,lt48_pic_source_session *,
    const lt48_pic_source_host *,lt48_pic_source_global_finish,void *,
    lt48_pic_range_cancelled,lt48_pic_source_operation_result *);
lt48_status lt48_pic_source_transport_compact_owner_internal(lt48_pic_source_transport *,
    lt48_pic_source_transport_compact_owner,void *,lt48_pic_source_operation_result *);
/* Applet transaction owner, separate from compact element traversal. All
 * roles, capacity and timeout come from the selected host contract. The
 * prepared transport supplies resources; this service owns reset-mode,
 * POWERON/status, DEVINIT/action, uncancelled DEVRESET/POWEROFF and global
 * release on every outcome. It does not infer erase/protection semantics. */
typedef struct {
    uint16_t mode,init_role,action_role,reset_role;
    uint8_t size_buffer;
    uint32_t capacity,address,action_timeout_seconds;
} lt48_pic_source_applet_transaction;
lt48_status lt48_pic_source_transport_applet_transaction_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_transaction *,
    lt48_pic_source_operation_result *);
/* Source non-table erase/action traversal, including the callback's DataIn
 * Boolean. The caller binds step and width from the original definition and
 * DataIn from DefineName. Optional unlock/unprotect/preerase remain explicit
 * higher-level stages; they are absent in the 534-entry PIC applet population. */
lt48_status lt48_pic_source_transport_applet_action_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_transaction *,
    const lt48_pic_source_applet_action_range *,uint8_t,
    lt48_pic_source_operation_result *);
/* Whole configuration callback branch; explicit source ID rejects the
 * sector/bit route before power entry. Modes 12/13 select read/write and the
 * distinct DataIn result rule, with unchanged cursor for DEVRESET. */
lt48_status lt48_pic_source_transport_applet_configuration_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_transaction *,uint16_t,uint8_t,
    lt48_pic_source_operation_result *);
/* Shared mapped applet transfer lifecycle. The selected source mode chooses
 * the stream callback. This owns transport sequencing and image staging only;
 * callers must supply audited public-operation and preservation policy. */
typedef struct {
    uint16_t mode,init_role,reset_role;
    uint8_t size_buffer,image_buffer,increment_buffer;
    lt48_pic_source_stream_protocol protocol;
    int writing;
    const lt48_pic_addtab_map *map;
    uint32_t first,count;
} lt48_pic_source_applet_stream_plan;
lt48_status lt48_pic_source_transport_applet_stream_run_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_stream_plan *,
    unsigned char *,size_t,lt48_pic_source_operation_result *);
/* Source-bound read tail (e.g. READCFG) runs before RESET/POWER-OFF in the
 * retained read mode. The combined image commits only after tail and cleanup. */
lt48_status lt48_pic_source_transport_applet_read_tail_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_stream_plan *,uint16_t,
    unsigned char *,size_t,lt48_pic_source_operation_result *);
/* Composite public programming: one prepared/powered session. The caller
 * supplies source-bound flags/ranges/margin levels and a preservation policy;
 * no calibration masks or optional unlock/unprotect stages are inferred.
 * Nonblank prompts return 1 to erase, 0 to abort. Missing prompts abort.
 * Source flags 16 mean voltage-margin Verify; 32 mean protection write. */
typedef lt48_status (*lt48_pic_source_program_preserve)(void *,lt48_pic_source_session *);
typedef int (*lt48_pic_source_program_erase_choice)(void *);
/* Check decisions use the original 3/4/5 Abort/Retry/Ignore values. Missing
 * decisions abort. Contact diagnostics are provided before FPGA/power entry;
 * ID diagnostics are provided after source cleanup. */
typedef int (*lt48_pic_source_startup_decision)(void *,int,
    const lt48_pic_contact_result *,const lt48_pic_source_identity_result *);
typedef struct {
    lt48_pic_contact_masks contact;
    const unsigned char *alternate_payload;size_t alternate_payload_size;
    lt48_pic_source_identity_session_plan identity;
    lt48_pic_source_startup_decision decide;
    void *context;
} lt48_pic_source_startup_plan;
/* Copy source-bound startup metadata before deferred preparation. Payload and
 * UI context are borrowed until the prepared operation finishes. Only contact,
 * ID and forced-ID flags belong to this shared binding. */
lt48_status lt48_pic_source_transport_bind_startup_internal(lt48_pic_source_transport *,
    const lt48_pic_source_startup_plan *,uint32_t);
typedef struct {
    lt48_pic_source_applet_stream_plan stream;
    uint32_t flags,capacity;
    uint16_t erase_role,protect_role,configuration_id;
    uint8_t data_in;
    lt48_pic_source_applet_action_range erase_range;
    const uint32_t *verify_voltages;size_t verify_voltage_count;
    uint16_t verify_variables[4];
    uint8_t auto_verify_voltages,verify_margin_type,verify_nominal_only;
    lt48_pic_source_program_preserve preserve;
    lt48_pic_source_program_erase_choice erase_choice;
    void *policy_context;
    const lt48_pic_source_startup_plan *startup;
} lt48_pic_source_applet_program_plan;
lt48_status lt48_pic_source_transport_applet_program_internal(lt48_pic_source_transport *,
    const lt48_pic_source_applet_program_plan *,unsigned char *,size_t,lt48_pic_source_operation_result *);
/* J-only whole replacement after explicit chip-erase authorization. Uses
 * the selected source erase callback and Read/Program stream callbacks;
 * retains powered entry between writes and full independent Read verification.
 * The caller must bind an audited J profile/entry/package and option policy.
 * This internal service alone grants no device admission. */
lt48_status lt48_pic_source_transport_j_program_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_program_plan *,
    const lt48_pic18_j_program_profile *,const unsigned char *,size_t,
    lt48_pic18_j_program_result *);
/* Preserving Secure/patch rewrite; caller retains complete backup and supplies
 * explicit whole-chip erase authorization before invoking this operation. */
lt48_status lt48_pic_source_transport_j_rewrite_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_program_plan *,
    const lt48_pic18_j_program_profile *,unsigned,uint32_t,const unsigned char *,size_t,
    unsigned char *,unsigned char *,size_t,lt48_pic18_j_rewrite_result *);
typedef struct {
    lt48_pic18_j_program_result transfer;
    uint8_t configuration[8];uint16_t device_id;
    uint32_t received_bytes;int id_observed,protected_read,configuration_from_mirrors;
} lt48_pic18_j_memory_result;
/* Read or erase+independent full blank verification in one powered owner.
 * Reads stage the whole image and commit only after successful cleanup.
 * Protected code reads remain zero; final configuration bytes are reconstructed
 * from readable mirrors and this distinction is explicit in the result. */
lt48_status lt48_pic_source_transport_j_memory_internal(
    lt48_pic_source_transport *,const lt48_pic_source_applet_program_plan *,
    const lt48_pic18_j_program_profile *,int,unsigned char *,size_t,
    lt48_pic18_j_memory_result *);
int lt48_pic_source_transport_matches_origin_internal(const lt48_pic_source_transport *,const void *);
/* Source compact public Program over one prepared/powered C session. Requires
 * source-bound startup and deferred FPGA preparation; source flags, initial
 * stop state, geometry and Verify slots remain explicit. The transport owns
 * runners, startup and complete source/global finish, including prompt re-entry.
 * No device preservation or protocol admission is inferred by this adapter. */
lt48_status lt48_pic_source_transport_compact_program_internal(lt48_pic_source_transport *,
    const lt48_pic_source_operation_contract *,const lt48_pic_source_program_request *,
    lt48_pic_source_operation_result *);
typedef lt48_status (*lt48_pic_source_transport_program_engine)(lt48_pic_source_transport *,
    const lt48_pic_source_operation_contract *,const lt48_pic_source_program_request *,
    lt48_pic_source_operation_result *);
#endif
