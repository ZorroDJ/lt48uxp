#ifndef LT48_PIC24_OPERATOR_DRAFT_H
#define LT48_PIC24_OPERATOR_DRAFT_H
#include <labtool48/labtool48.h>
#include "pic24_program_plan.h"
#include "pic24_configuration_rules.h"
#include "../../sources/pic-common/source_applet_program_sequence.h"
/* Shared FJ40 program owner, below the source-bound transport adapter.
 * This draft performs no hardware I/O and grants no device admission. The
 * adapter must implement the declared phases with audited source resources. */
typedef struct {
 const lt48_pic24_image_policy *geometry;
 lt48_pic24_program_plan program;
 const unsigned char *image;size_t image_size;
 uint16_t configuration[4];unsigned configuration_words;
 unsigned mandatory_rule_family;
 uint32_t code_image_bytes;
} lt48_pic24_program_view;
typedef lt48_status (*lt48_pic24_program_execute)(void *,enum lt48_pic_applet_program_phase,
 const lt48_pic24_program_view *,lt48_pic_applet_program_state *);
typedef struct {
 lt48_status operation_status,cleanup_status;
 unsigned aborted,cancelled;
} lt48_pic24_program_result;
lt48_status lt48_pic24_program_owned(const lt48_pic24_image_policy *,unsigned family,
 unsigned selector,unsigned bank,uint32_t flags,const unsigned char *,size_t,
 lt48_pic24_program_execute,void *,lt48_pic24_program_result *);
#endif
