#ifndef LT48_PIC_COMMON_CONFIGURATION_H
#define LT48_PIC_COMMON_CONFIGURATION_H
#include <labtool48/labtool48.h>
#include "configuration_composition.h"

/* Source-generated standalone PostConfig evaluator. Uses the same shared C
 * service as production callers. All used fields must be supplied explicitly;
 * this does not establish PreConfig state, silicon policy or device admission. */
int lt48_pic_configuration_compose_internal(
    const lt48_pic_configuration_composition *contract,
    const unsigned char *input,size_t span,
    const lt48_pic_composition_value *values,size_t count,unsigned char *output);

typedef struct {
    uint16_t category,field;
    uint32_t minimum,maximum;
} lt48_pic_configuration_field;

/* Decode image fields with PreConfig, validate/apply source-schema edits, and
 * encode with PostConfig. Edits refer to the selected direct configuration
 * object (-1 in the current PIC host contract). No chip I/O or admission. */
lt48_status lt48_pic_configuration_edit_internal(
    const lt48_compiled_wave *pre,const lt48_compiled_wave *post,
    const lt48_pin_map *pins,const lt48_compile_variable *variables,size_t variable_count,
    unsigned element_bytes,const unsigned char *input,size_t span,unsigned char *output,
    const lt48_pic_configuration_field *fields,size_t field_count,
    const lt48_runtime_configuration_value *edits,size_t edit_count);

/* Source-selected pure PreConfig/PostConfig lifecycle. Input and output may
 * alias; output is changed only after both callbacks complete successfully.
 * No transport exists in this service. Hardware/nested-wave events reject it.
 * This is a conversion capability, never an operation admission decision. */
lt48_status lt48_pic_configuration_normalize_internal(
    const lt48_compiled_wave *pre,const lt48_compiled_wave *post,
    const lt48_pin_map *pins,const lt48_compile_variable *variables,size_t variable_count,
    unsigned element_bytes,const unsigned char *input,size_t span,unsigned char *output);
#endif
