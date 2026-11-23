#ifndef LT48_PIC24_VERIFIED_PROGRAM_DRAFT_H
#define LT48_PIC24_VERIFIED_PROGRAM_DRAFT_H
#include "pic24_program_transport.h"
#include "pic24_configuration_transport.h"
#include "pic24_configuration_compare.h"
typedef struct {
 const lt48_pic_applet_plan_profile *profile;
 const lt48_pic_source_resources *source;
 lt48_pic_source_transport *program_transport,*read_transport;
 const lt48_pic24_program_view *view;
 const lt48_pic_source_startup_plan *startup;
 const lt48_protocol_transport *indicator_transport;void *indicator_context;
 lt48_pic_source_program_erase_choice erase_choice;void *policy_context;
 uint32_t flags;
} lt48_pic24_verified_program_plan;
typedef struct {
 lt48_pic_source_operation_result program,readback;
 lt48_pic24_configuration_mismatch mismatch;
 lt48_status operation_status,indication_status;
 unsigned readback_attempted;
} lt48_pic24_verified_program_result;
/* Whole Program+margin Verify, fresh full Read, configuration comparison and
 * final indication. Caller supplies consistent injected transport bindings.
 * Caller image commits only after every stage including final indication.
 * Shared owners release rails before the separate final LED-only command.
 * This internal draft grants no public inventory admission. */
lt48_status lt48_pic24_verified_program(const lt48_pic24_verified_program_plan *,
 unsigned char *,size_t,lt48_pic24_verified_program_result *);
#endif
