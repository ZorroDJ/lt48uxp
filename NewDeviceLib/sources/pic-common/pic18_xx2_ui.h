#ifndef LT48_PIC18_XX2_UI_H
#define LT48_PIC18_XX2_UI_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Exact audited XX2/XX8 source bindings. Configuration/API and adapter guidance;
 * public operation admission is separate. DS39576B figure2-1 supplies pins. */
typedef struct {uint32_t definition;uint16_t family,pinmap;const char *name;} lt48_pic18_xx2_member;
static const lt48_pic18_xx2_member lt48_pic18_xx2_members[]={
 {0,0,0,"PIC18F242 *28SO"},
 {0,0,0,"PIC18F242 28DIP"},
 {1,1,0,"PIC18F248 *28SO"},
 {1,1,0,"PIC18F248 28DIP"},
 {8,8,0,"PIC18F252 *28SO"},
 {8,8,0,"PIC18F252 28DIP"},
 {9,9,0,"PIC18F258 *28SO"},
 {9,9,0,"PIC18F258 28DIP"},
 {4,4,2,"PIC18F442 *44 "},
 {6,6,3,"PIC18F442 *44TQ"},
 {2,2,1,"PIC18F442 40DIP"},
 {5,5,2,"PIC18F448 *44 "},
 {7,7,3,"PIC18F448 *44TQ"},
 {3,3,1,"PIC18F448 40DIP"},
 {12,12,2,"PIC18F452 *44 "},
 {14,14,3,"PIC18F452 *44TQ"},
 {10,10,1,"PIC18F452 40DIP"},
 {13,13,2,"PIC18F458 *44 "},
 {15,15,3,"PIC18F458 *44TQ"},
 {11,11,1,"PIC18F458 40DIP"},
};
static inline int lt48_pic18_xx2_configuration_binding(uint32_t d,uint16_t f,uint16_t p) {
 for(size_t i=0;i<sizeof(lt48_pic18_xx2_members)/sizeof(*lt48_pic18_xx2_members);i++) {
  const lt48_pic18_xx2_member *m=lt48_pic18_xx2_members+i;
  if(m->definition==d&&m->family==f&&m->pinmap==p)return 1;
 }return 0;
}
static inline int lt48_pic18_xx2_allowed(const lt48_device_spec *s) {
 if(!s||s->catalogue.device_class!=2048||!s->definition_stem||strcmp(s->definition_stem,"GDCC___")||!s->catalogue.device_name)return 0;
 for(size_t i=0;i<sizeof(lt48_pic18_xx2_members)/sizeof(*lt48_pic18_xx2_members);i++) {
  const lt48_pic18_xx2_member *m=lt48_pic18_xx2_members+i;
  if(m->definition==s->definition_resource&&m->family==s->family_code&&m->pinmap==s->pinmap&&strlen(m->name)==s->catalogue.device_name_length&&!memcmp(m->name,s->catalogue.device_name,s->catalogue.device_name_length))return 1;
 }return 0;
}
static inline const char *lt48_pic18_xx2_adapter_note(const lt48_device_spec *s) {
 if(!lt48_pic18_xx2_allowed(s)||s->pinmap!=3)return NULL;
 return "Requires QFP44-to-DIP44 wiring rotated six positions: chip 1 to DIP 7; VDD 7/28 to DIP 13/34; VSS 6/29 to DIP 12/35; MCLR 18 to DIP 24; PGC 16 to DIP 22; PGD 17 to DIP 23. Center DIP44 in ZIF48 (DIP1 at ZIF3). A straight adapter is incompatible. Wiring is source/specification audited; hardware testing pending.";
}
#endif
