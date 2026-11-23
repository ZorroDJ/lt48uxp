#ifndef LT48_PIC24_PROGRAM_TRANSPORT_DRAFT_H
#define LT48_PIC24_PROGRAM_TRANSPORT_DRAFT_H
#include "pic24_transport_prepare.h"
/* Whole-image code plus configuration Program. Flags must request code(8)
 * and configuration(32); optional erase/blank/verify/check decisions remain
 * source-bound. Caller image commits only after shared operation/cleanup pass.
 * Internal draft, not an admitted public device backend. */
lt48_status lt48_pic24_program_transport(const lt48_pic_applet_plan_profile *,
 const lt48_pic_source_resources *,lt48_pic_source_transport *,
 const lt48_pic24_program_view *,uint32_t,const lt48_pic_source_startup_plan *,
 lt48_pic_source_program_erase_choice,void *,unsigned char *,size_t,
 lt48_pic_source_operation_result *);
#endif
