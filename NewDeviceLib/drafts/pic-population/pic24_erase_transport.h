#ifndef LT48_PIC24_ERASE_TRANSPORT_DRAFT_H
#define LT48_PIC24_ERASE_TRANSPORT_DRAFT_H
#include "pic24_transport_prepare.h"
/* Whole-device erase, selected original resources, deferred startup and
 * unconditional source/global finish through the shared C transport.
 * Internal draft: no inventory admission or hardware qualification inferred.
 * The caller owns transport lifetime and supplies the source-bound identity
 * and operator decisions. Each invocation requires a fresh prepared owner. */
lt48_status lt48_pic24_erase_transport(const lt48_pic_applet_plan_profile *,
 const lt48_pic_source_resources *,lt48_pic_source_transport *,
 const lt48_pic24_program_view *,uint32_t,const lt48_pic_source_startup_plan *,
 lt48_pic_source_operation_result *);
#endif
