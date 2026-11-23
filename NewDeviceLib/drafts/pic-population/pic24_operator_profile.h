#ifndef LT48_PIC24_OPERATOR_PROFILE_DRAFT_H
#define LT48_PIC24_OPERATOR_PROFILE_DRAFT_H
#include "pic24_transport_prepare.h"
typedef struct {
 const char *key;uint16_t definition,family,pinmap;
 uint8_t rule_family,configuration_words;uint32_t image_bytes,last_target_address;
 unsigned char source_sha256[32];
} lt48_pic24_operator_profile;
const lt48_pic24_operator_profile *lt48_pic24_operator_profile_find(const char *);
/* No hardware I/O. Own profile table, exact source binding and source request
 * semantics validate the whole-image geometry/selector/bank before exposing a
 * C operation view. Outputs commit together; geometry storage is caller-owned.
 * Profile lookup is preparation metadata, not device support admission. */
lt48_status lt48_pic24_operator_profile_view(const lt48_pic24_operator_profile *,
 const lt48_pic_source_resources *,const unsigned char *,size_t,
 lt48_pic24_image_policy *,lt48_pic24_program_view *,
 const lt48_pic_applet_plan_profile **);
#endif
