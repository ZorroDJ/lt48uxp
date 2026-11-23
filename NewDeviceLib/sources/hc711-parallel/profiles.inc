/* Authored exact bindings. Original payloads are loaded from a SHA-guarded
 * user-supplied library; no vendor code or bitstream is distributed here. */
#include "admission.h"
static unsigned hc711_parallel_binding(const lt48_device_spec *s) {
    const unsigned char *name=s->catalogue.device_name;
    if(!s->catalogue.vendor || (strcmp(s->catalogue.vendor,"Motorola") && strcmp(s->catalogue.vendor,"Freescale")) || s->catalogue.manufacturer_id || s->catalogue.device_id || s->element_bits!=8 || s->secondary_element_count)return 0;
    if(s->catalogue.device_class==2013 && !strcmp(s->definition_stem,"GADA___") && s->definition_resource==0xc5 && s->family_code==0x76 && s->pinmap==0x4d && s->pin_count==40 && s->capacity_bytes==16384 && s->catalogue.device_name_length==sizeof("MC68HC711L6 @68")-1 && !memcmp(name,"MC68HC711L6 @68",sizeof("MC68HC711L6 @68")-1))return 2;
    if(strcmp(s->definition_stem,"GAD____"))return 0;
    if(s->catalogue.device_class!=2012 || s->capacity_bytes!=4096 || s->catalogue.manufacturer_id || s->catalogue.device_id)return 0;
    if(s->definition_resource==5 && s->family_code==4 && s->pinmap==4 && s->pin_count==40 && s->catalogue.device_name_length==11 && !memcmp(name,"MC68HC711D3",11))return 1;
    if(s->definition_resource==6 && s->family_code==5 && s->pinmap==5 && s->pin_count==44 && s->catalogue.device_name_length==15 && !memcmp(name,"MC68HC711D3 *44",15))return 1;
    return 0;
}
