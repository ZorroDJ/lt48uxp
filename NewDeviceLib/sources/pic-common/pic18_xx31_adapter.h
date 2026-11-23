#ifndef LT48_PIC18_XX31_ADAPTER_H
#define LT48_PIC18_XX31_ADAPTER_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Authored programming-only adapter: DS30500B figure2-3 physical pins to
 * the audited P42 DIP endpoints. This does not claim original adapter wiring.
 * Includes all analog/digital supplies and grounds. Other leads unconnected.
 */
typedef struct {unsigned char chip,dip;const char *role;} lt48_xx31_adapter_net;
static const lt48_xx31_adapter_net lt48_xx31_qfn_nets[]={
 {7,10,"VDD"},{8,22,"VDD"},{28,23,"VDD"},{29,27,"VDD"},
 {6,13,"VSS"},{30,31,"VSS"},{31,32,"VSS"},
 {18,28,"MCLR"},{16,21,"ICSPCLK"},{17,24,"ICSPDAT"},{15,20,"PGM"}
};
static inline const char *lt48_xx31_adapter_note(const lt48_device_spec *s) {
 if(!s||s->catalogue.device_class!=2048||!s->definition_stem||strcmp(s->definition_stem,"GDCC___")||!s->catalogue.device_name)return NULL;
 const char *name=NULL;
 if(s->pinmap==41u) {
  if(s->definition_resource==106u&&s->family_code==93u)name="PIC18F4331 *44TQ";
  if(s->definition_resource==110u&&s->family_code==97u)name="PIC18F4431 *44TQ";
 }else if(s->pinmap==42u) {
  if(s->definition_resource==107u&&s->family_code==94u)name="PIC18F4331 @44QFN";
  if(s->definition_resource==111u&&s->family_code==98u)name="PIC18F4431 @44QFN";
 }
 if(!name||strlen(name)!=s->catalogue.device_name_length||memcmp(name,s->catalogue.device_name,strlen(name)))return NULL;
 if(s->pinmap==41u)return "Requires TQFP44-to-DIP44 wiring rotated six positions: chip 1 to DIP 7; VDD/AVDD 7/28 to DIP 13/34; VSS/AVSS 6/29 to DIP 12/35; MCLR 18 to DIP 24; PGC 16 to DIP 22; PGD 17 to DIP 23; PGM 15 to DIP 21. Center DIP44 in ZIF48 (DIP1 at ZIF3). A straight adapter is incompatible.";
 return "Requires the authored XX31 QFN44 programming-only adapter, centered as DIP44 (DIP1 at ZIF3): chip->DIP VDD/AVDD 7->10, 8->22, 28->23, 29->27; VSS/AVSS 6->13, 30->31, 31->32; MCLR 18->28; PGC 16->21; PGD 17->24; PGM 15->20. All other chip leads remain unconnected. A straight or uniformly rotated adapter is incompatible. Wiring is source/specification audited; hardware testing pending.";
}
#endif
