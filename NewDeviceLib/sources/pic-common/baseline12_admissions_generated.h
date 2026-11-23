#ifndef LT48_BASELINE12_ADMISSIONS_GENERATED_H
#define LT48_BASELINE12_ADMISSIONS_GENERATED_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Exact completed audit cohort. Generated geometry alone grants no admission. */
#define LT48_PIC_BASELINE12_ADMISSIONS_ENABLED 1
static inline int lt48_pic_baseline12_public_allowed(const lt48_device_spec *s) {
 if(!s||s->catalogue.device_class!=2015||!s->catalogue.vendor||strcmp(s->catalogue.vendor,"Microchip")||!s->catalogue.device_name)return 0;
 static const struct {uint32_t d;uint16_t f,p;const char *name;} rows[]={
 {354,245,84,"PIC10F200"},
 {355,245,85,"PIC10F200 *6SOT"},
 {357,246,84,"PIC10F202"},
 {356,246,85,"PIC10F202 *6SOT"},
 {354,245,84,"PIC10F204"},
 {357,246,84,"PIC10F206"},
 {358,247,84,"PIC10F220"},
 {359,247,85,"PIC10F220 *6SOT"},
 {360,248,84,"PIC10F222"},
 {361,248,85,"PIC10F222 *6SOT"},
 {362,249,87,"PIC12F508"},
 {363,250,87,"PIC12F509"},
 {364,251,87,"PIC12F510"},
 {383,266,95,"PIC12F519"},
 {383,266,95,"PIC12F519 *8SO"},
 {365,252,88,"PIC16F505"},
 {365,252,88,"PIC16F505 *14SO"},
 {365,252,88,"PIC16F505 *14TSSOP"},
 {366,253,88,"PIC16F506"},
 {366,253,88,"PIC16F506 *14SO"},
 {366,253,88,"PIC16F506 *14TSSOP"},
 {367,254,88,"PIC16F526"},
 {367,254,88,"PIC16F526 *14SO"},
 {367,254,88,"PIC16F526 *14TSSOP"},
 {57,51,15,"PIC16F54"},
 {57,51,15,"PIC16F54 *18SO"},
 {58,51,16,"PIC16F54 *20SSOP"},
 {59,52,17,"PIC16F57"},
 {59,52,17,"PIC16F57 *28SO"},
 {60,52,18,"PIC16F57 *28SSOP"},
 {384,267,96,"PIC16F570"},
 {384,267,96,"PIC16F570 *28SO"},
 {384,267,96,"PIC16F570 *28SSOP"},
 {61,53,19,"PIC16F59"},
 {62,53,20,"PIC16F59 *44TQ"},
 };
 for(size_t i=0;i<sizeof(rows)/sizeof(*rows);i++)if(s->definition_resource==rows[i].d&&s->family_code==rows[i].f&&s->pinmap==rows[i].p&&s->catalogue.device_name_length==strlen(rows[i].name)&&!memcmp(s->catalogue.device_name,rows[i].name,s->catalogue.device_name_length))return 1;
 return 0;
}
#endif
