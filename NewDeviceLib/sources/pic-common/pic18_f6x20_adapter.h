#ifndef LT48_PIC18_F6X20_ADAPTER_H
#define LT48_PIC18_F6X20_ADAPTER_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Authored programming-only TQFP64-to-DIP32 wiring. DS39583B figure2-1
 * physical roles join original P14 endpoints; supplies share endpoints.
 * This does not reconstruct the original vendor adapter. */
typedef struct {unsigned char chip,dip;const char *role;} lt48_f6x20_adapter_net;
static const lt48_f6x20_adapter_net lt48_f6x20_adapter_nets[]={
 {10,32,"VDD"},{19,32,"VDD"},{26,32,"VDD"},{38,32,"VDD"},{57,32,"VDD"},
 {9,16,"VSS"},{20,16,"VSS"},{25,16,"VSS"},{41,16,"VSS"},{56,16,"VSS"},
 {7,1,"MCLR"},{42,23,"ICSPCLK"},{37,20,"ICSPDAT"},{43,24,"PGM"}
};
static inline const char *lt48_f6x20_adapter_note(const lt48_device_spec *s) {
 if(!s||s->catalogue.device_class!=2048||!s->definition_stem||strcmp(s->definition_stem,"GDCC___")||s->pinmap!=14u||!s->catalogue.device_name)return NULL;
 const char *name=NULL;
 if(s->definition_resource==36u&&s->family_code==26u)name="PIC18F6520 @64TQ";
 if(s->definition_resource==37u&&s->family_code==26u)name="PIC18F6620 @64TQ";
 if(s->definition_resource==38u&&s->family_code==27u)name="PIC18F6720 @64TQ";
 if(!name||strlen(name)!=s->catalogue.device_name_length||memcmp(name,s->catalogue.device_name,strlen(name)))return NULL;
 return "Requires the authored F6X20 TQFP64 programming-only adapter to DIP32, centered in ZIF48 (DIP1 at ZIF9): join chip VDD/AVDD 10/19/26/38/57 to DIP32; VSS/AVSS 9/20/25/41/56 to DIP16; MCLR7 to DIP1; PGC42 to DIP23; PGD37 to DIP20; PGM43 to DIP24. All other chip leads remain unconnected. A straight adapter is incompatible. Wiring is source/specification audited; hardware testing pending.";
}
#endif
