#ifndef LT48_PIC24_CONFIGURATION_EDITOR_H
#define LT48_PIC24_CONFIGURATION_EDITOR_H
#include "pic24_operator_profile.h"
/* Source PreConfig/PostConfig conversion for the normal configuration editor.
 * Values use source category/field identities. No I/O. Whole-image size and
 * immutable profile/source binding are required; failed edits are atomic.
 * Primary mandatory bit states are applied after source PostConfig. */
lt48_status lt48_pic24_configuration_values(const lt48_pic24_operator_profile *,
 const lt48_pic_source_resources *,const unsigned char *,size_t,
 lt48_runtime_configuration_value *,size_t,size_t *);
lt48_status lt48_pic24_configuration_edit(const lt48_pic24_operator_profile *,
 const lt48_pic_source_resources *,unsigned char *,size_t,
 const lt48_runtime_configuration_value *,size_t);
#endif
