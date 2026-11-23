/* Authored manufacturer/source-bound baseline adapter requirement. */
#ifndef LT48_PIC_BASELINE12_ADAPTER_GUIDANCE_H
#define LT48_PIC_BASELINE12_ADAPTER_GUIDANCE_H
#include <labtool48/labtool48.h>
#include <string.h>
static inline const char *lt48_pic_baseline12_adapter_note_for_spec(const lt48_device_spec *s){
 if(s->catalogue.device_class==2015&&s->definition_stem&&!strcmp(s->definition_stem,"GADC___")&&
 s->definition_resource==62&&s->family_code==53&&s->pinmap==20&&
 s->catalogue.device_name&&s->catalogue.device_name_length==14&&!memcmp(s->catalogue.device_name,"PIC16F59 *44TQ",14))
 return "Requires a TQFP44-to-DIP44 adapter with chip pin 1 wired to DIP pin 7 (six-position rotation). VDD 12/13/34/35 to DIP 18/19/40/41; VSS 1/2/23/24 to DIP 7/8/29/30; MCLR 11 to DIP 17; ICSPCLK 9 to DIP 15; ICSPDAT 10 to DIP 16.";
 return NULL;
}
#define LT48_PIC_BASELINE12_ADAPTER_SHA256 "24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec"
#endif
