#ifndef LT48_PIC24_TRANSPORT_PREPARE_DRAFT_H
#define LT48_PIC24_TRANSPORT_PREPARE_DRAFT_H
#include "pic24_source_stages.h"
#include "applet_plans_generated.h"
/* Connect a validated whole-image view to existing deferred source startup.
 * Caller supplies transport and decisions. Applets upload now; selected FPGA
 * and target power remain deferred to the shared contact/startup service.
 * This is not device admission and must not be called without an audited
 * profile/adapter context. Borrowed resources remain alive throughout. */
lt48_status lt48_pic24_prepare_startup(const lt48_pic_applet_plan_profile *,
 const lt48_pic_source_resources *,lt48_pic_source_transport *,
 const lt48_pic24_program_view *,uint32_t,const lt48_pic_source_startup_plan *);
#endif
