#ifndef LT48_PIC24_SOURCE_STAGES_DRAFT_H
#define LT48_PIC24_SOURCE_STAGES_DRAFT_H
#include "pic24_operator.h"
#include "../../sources/pic-common/source_resources.h"
/* Build native runtime requests for four shared source stages. This does not
 * transfer them to USB or interpret an applet's hardware result as success.
 * Resource lifetime belongs to the caller; the returned result is owned. */
lt48_status lt48_pic24_source_stage_requests(const lt48_pic_source_resources *,
 enum lt48_pic_applet_program_phase,const lt48_pic24_program_view *,lt48_runtime_result **);
#endif
