/* Exact source binding; original software resources remain BYO. */
#include "admission.h"
static int stk11one_binding(const lt48_device_spec*s){return STK11ONE_ADMITTED&&s->catalogue.device_class==2010&&s->definition_resource==0x91&&s->family_code==120&&s->pinmap==41&&s->pin_count==28&&s->capacity_bytes==8192&&s->element_bits==8&&!s->secondary_element_count&&!strcmp(s->definition_stem,"GAB____")&&!strcmp(s->catalogue.vendor,"Simtek")&&s->catalogue.device_name_length==strlen("STK11C68 *28SO")&&!memcmp(s->catalogue.device_name,"STK11C68 *28SO",strlen("STK11C68 *28SO"));}
