#ifndef LT48_PIC24_MEMORY_TRANSPORT_DRAFT_H
#define LT48_PIC24_MEMORY_TRANSPORT_DRAFT_H
#include "pic24_transport_prepare.h"
/* Whole-image Read(1), Blank Check(3), Verify(4). Exact source map and
 * deferred startup; shared owner stages Read and commits after cleanup.
 * Internal draft; protocol fixtures do not confer device admission. */
lt48_status lt48_pic24_memory_transport(const lt48_pic_applet_plan_profile *,
 const lt48_pic_source_resources *,lt48_pic_source_transport *,
 const lt48_pic24_program_view *,uint32_t,const lt48_pic_source_startup_plan *,
 uint16_t,unsigned char *,size_t,lt48_pic_source_operation_result *);
#endif
