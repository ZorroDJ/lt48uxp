#ifndef LT48_PIC24_OPERATOR_BINDING_DRAFT_H
#define LT48_PIC24_OPERATOR_BINDING_DRAFT_H
#include "pic24_operator_profile.h"
typedef struct {
 lt48_pic24_image_policy geometry;lt48_pic24_program_view view;
 const lt48_pic_applet_plan_profile *plan;
 lt48_pic_source_startup_plan startup;
} lt48_pic24_operator_binding;
/* No I/O. Join C profile/geometry to original contact and identity metadata.
 * Request supplies operator decisions and borrowed alternate payload only.
 * Keep the output object at its constructed address: view.geometry points
 * into it. Source, image and UI context must outlive the operation. */
lt48_status lt48_pic24_operator_binding_make(const lt48_pic24_operator_profile *,
 const lt48_pic_source_resources *,lt48_pic_source_transport *,
 const unsigned char *,size_t,const lt48_pic_source_startup_plan *,
 lt48_pic24_operator_binding *);
#endif
