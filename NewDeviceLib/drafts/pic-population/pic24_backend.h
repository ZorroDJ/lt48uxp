#ifndef LT48_PIC24_BACKEND_H
#define LT48_PIC24_BACKEND_H
#include "pic24_configuration_editor.h"
#include "pic24_verified_program.h"
#include "pic24_verified_compare.h"
#include "pic24_memory_transport.h"
#include "pic24_erase_transport.h"
#include "pic24_operator_binding.h"
typedef struct lt48_pic24_backend lt48_pic24_backend;
typedef struct {
 lt48_status status;lt48_pic_source_operation_result operation;
 lt48_pic_source_operation_result configuration_before,configuration_after;
 unsigned configuration_readback_confirmed,protection_write_completed;
 lt48_status configuration_indication_status;lt48_pic24_configuration_mismatch configuration_mismatch;
 lt48_pic24_verified_program_result program;lt48_pic24_verified_compare_result comparison;
} lt48_pic24_backend_result;
/* Candidate C backend. Exact source/profile loading grants no admission.
 * Each execution owns a fresh session/transport. The injected API is the sole
 * I/O boundary; construction/configuration APIs cannot touch hardware. */
lt48_status lt48_pic24_backend_open(const char *,const char *,lt48_pic24_backend **);
void lt48_pic24_backend_close(lt48_pic24_backend *);
lt48_status lt48_pic24_backend_expected_identity(const lt48_pic24_backend *,uint16_t *,uint16_t *);
size_t lt48_pic24_backend_capacity(const lt48_pic24_backend *);
lt48_status lt48_pic24_backend_configuration_values(const lt48_pic24_backend *,
 const unsigned char *,size_t,lt48_runtime_configuration_value *,size_t,size_t *);
lt48_status lt48_pic24_backend_configuration_edit(const lt48_pic24_backend *,
 unsigned char *,size_t,const lt48_runtime_configuration_value *,size_t);
/* Whole-image Read/Blank/Compare/Verify/Erase/Program. Program always includes
 * both source voltage margins, configuration programming and independent
 * configuration Read. Requested GCP protection is applied only after these
 * checks complete, through the separate status-checked Secure sequence.
 * Compare/Verify also independently read and compare all
 * low16 configuration words. flags selects source contact/ID checks only (0x80003).
 * Configuration-only writes preserve code, require an unprotected device and
 * reject changes needing erase. Secure snapshots unprotected configuration and clears GCP through the original
 * NVM completion-checked callback; it does not claim post-reset protected Read.
 * Inventory/GUI admission remains gated.
 * Caller image commits for successful Read/Program/configuration write only. */
lt48_status lt48_pic24_backend_execute(const lt48_pic24_backend *,
 const lt48_protocol_transport *,void *,lt48_operation_kind,unsigned char *,size_t,
 uint32_t,const lt48_pic_source_startup_plan *,lt48_pic24_backend_result *);
lt48_status lt48_pic24_backend_verify_levels(const lt48_pic24_backend *,
 lt48_verify_margin_selection,unsigned,uint32_t [2],unsigned *);
lt48_status lt48_pic24_backend_execute_with_margin(const lt48_pic24_backend *,
 const lt48_protocol_transport *,void *,lt48_operation_kind,unsigned char *,size_t,
 uint32_t,const lt48_pic_source_startup_plan *,lt48_verify_margin_selection,unsigned,
 lt48_pic24_backend_result *);
#endif
