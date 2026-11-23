#ifndef LT48_PIC18_J_OPTIONS_H
#define LT48_PIC18_J_OPTIONS_H
#include <labtool48/labtool48.h>
#include "pic18_j_program.h"
typedef struct {uint8_t value;const char *name;} lt48_pic18_j_choice;
typedef struct {
 uint16_t id;const char *name;
 uint8_t byte,shift,mask;
 const lt48_pic18_j_choice *choices;size_t choice_count;
} lt48_pic18_j_option;
typedef struct {
 const char *key;uint16_t definition,family,pinmap;
 uint16_t device_id,device_id_mask;
 lt48_pic18_j_program_profile program;
 const lt48_pic18_j_option *fields;size_t field_count;
 uint8_t defaults[8];
} lt48_pic18_j_options_profile;
typedef struct {uint16_t field;uint32_t value;} lt48_pic18_j_option_value;
/* Exact alias AND source binding. Shared Windows definitions alone cannot
 * choose a J profile. Descriptors do not grant operation/GUI admission. */
const lt48_pic18_j_options_profile *lt48_pic18_j_options_find(const char *,uint16_t,uint16_t,uint16_t);
lt48_status lt48_pic18_j_options_decode(const lt48_pic18_j_options_profile *,
 const uint8_t *,size_t,lt48_pic18_j_option_value *,size_t,size_t *);
/* Atomic edits to the final eight Flash bytes only; values are physical field
 * encodings from named choices, including inverted enable/protect bits. */
lt48_status lt48_pic18_j_options_edit(const lt48_pic18_j_options_profile *,
 uint8_t *,size_t,const lt48_pic18_j_option_value *,size_t);
lt48_status lt48_pic18_j_options_defaults(const lt48_pic18_j_options_profile *,uint8_t *,size_t);
#endif
