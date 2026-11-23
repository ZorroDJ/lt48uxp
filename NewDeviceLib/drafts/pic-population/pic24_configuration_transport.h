#ifndef LT48_PIC24_CONFIGURATION_TRANSPORT_DRAFT_H
#define LT48_PIC24_CONFIGURATION_TRANSPORT_DRAFT_H
#include "pic24_memory_transport.h"
/* Independent full Read, then raw low16 configuration extraction in ascending
 * canonical-image order. Caller image remains untouched. Output words commit
 * only after successful Read and cleanup. This authored route does not execute
 * the dedicated READCFG applet or infer unimplemented-bit comparison masks. */
lt48_status lt48_pic24_configuration_read_transport(const lt48_pic_applet_plan_profile *,
 const lt48_pic_source_resources *,lt48_pic_source_transport *,
 const lt48_pic24_program_view *,uint32_t,const lt48_pic_source_startup_plan *,
 uint16_t *,unsigned,lt48_pic_source_operation_result *);
#endif
