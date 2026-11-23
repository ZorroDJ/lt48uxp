#ifndef LT48_PIC18_J_PROPERTY_H
#define LT48_PIC18_J_PROPERTY_H
#include "pic18_j_options.h"
typedef struct lt48_pic18_j_property lt48_pic18_j_property;
/* Owns a transactional model for the normal generic GTK property-sheet
 * renderer. No hardware or operation admission. The profile is borrowed. */
lt48_status lt48_pic18_j_property_create(const lt48_pic18_j_options_profile *,
 const uint8_t *,size_t,lt48_pic18_j_property **);
lt48_property_model *lt48_pic18_j_property_model(lt48_pic18_j_property *);
lt48_status lt48_pic18_j_property_commit(lt48_pic18_j_property *,uint8_t *,size_t);
void lt48_pic18_j_property_close(lt48_pic18_j_property *);
#endif
