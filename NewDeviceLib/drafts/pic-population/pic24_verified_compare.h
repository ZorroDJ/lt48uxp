#ifndef LT48_PIC24_VERIFIED_COMPARE_H
#define LT48_PIC24_VERIFIED_COMPARE_H
#include "pic24_configuration_transport.h"
#include "pic24_configuration_compare.h"
typedef struct {
 const lt48_pic_applet_plan_profile *profile;const lt48_pic_source_resources *source;
 lt48_pic_source_transport *compare_transport,*read_transport;
 const lt48_pic24_program_view *view;const lt48_pic_source_startup_plan *startup;
 const lt48_protocol_transport *indicator_transport;void *indicator_context;
 uint32_t checks;
} lt48_pic24_verified_compare_plan;
typedef struct {
 lt48_pic_source_operation_result code,configuration;
 lt48_pic24_configuration_mismatch mismatch;
 lt48_status operation_status,indication_status;unsigned readback_attempted;
} lt48_pic24_verified_compare_result;
/* Source code comparison followed by independent full Read/config extraction.
 * Compare raw low16 configuration bits: verifying an existing image does not
 * silently normalize it as programming does. Never change the caller image.
 * Rails are released by the shared owners before the final LED-only command. */
lt48_status lt48_pic24_verified_compare(const lt48_pic24_verified_compare_plan *,
 unsigned char *,size_t,lt48_pic24_verified_compare_result *);
#endif
