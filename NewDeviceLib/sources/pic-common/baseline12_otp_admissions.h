#ifndef LT48_PIC_BASELINE12_OTP_ADMISSIONS_H
#define LT48_PIC_BASELINE12_OTP_ADMISSIONS_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Exact audited cohort. Candidate regeneration never changes admission. */
#define LT48_PIC_BASELINE12_OTP_ADMISSIONS_ENABLED 1
static inline int lt48_pic_baseline12_otp_public_allowed(const lt48_device_spec *s) {
 if(!s||s->catalogue.device_class!=2015||!s->catalogue.vendor||strcmp(s->catalogue.vendor,"Microchip")||!s->catalogue.device_name)return 0;
 static const struct {uint32_t definition;uint16_t family,pinmap;const char *name;} rows[]={
 {334,225,75,"PIC12C508"},
 {334,225,75,"PIC12C508A"},
 {335,226,75,"PIC12C509"},
 {335,226,75,"PIC12C509A"},
 {334,225,75,"PIC12CE518"},
 {335,226,75,"PIC12CE519"},
 {40,40,6,"PIC16C505"},
 {1,1,1,"PIC16C52"},
 {2,2,1,"PIC16C52-LP"},
 {0,0,1,"PIC16C52-XT/-RC"},
 {1,1,1,"PIC16C52/-HS"},
 {4,4,1,"PIC16C54"},
 {6,6,1,"PIC16C54A"},
 {19,7,1,"PIC16C54B"},
 {19,7,1,"PIC16C54C"},
 {8,9,0,"PIC16C55"},
 {9,10,0,"PIC16C55-LP"},
 {7,8,0,"PIC16C55-XT/-RC"},
 {8,9,0,"PIC16C55/-HS"},
 {10,11,0,"PIC16C55A"},
 {12,13,1,"PIC16C56"},
 {13,14,1,"PIC16C56-LP"},
 {11,12,1,"PIC16C56-XT/-RC"},
 {12,13,1,"PIC16C56/-HS"},
 {20,15,1,"PIC16C56A"},
 {15,17,0,"PIC16C57"},
 {16,18,0,"PIC16C57-LP"},
 {14,16,0,"PIC16C57-XT/-RC"},
 {15,17,0,"PIC16C57/-HS"},
 {17,19,0,"PIC16C57C"},
 {18,21,1,"PIC16C58A"},
 {21,20,1,"PIC16C58B"},
 {26,26,1,"PIC16HV540"},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(s->definition_resource==rows[i].definition&&s->family_code==rows[i].family&&s->pinmap==rows[i].pinmap&&s->catalogue.device_name_length==strlen(rows[i].name)&&!memcmp(s->catalogue.device_name,rows[i].name,s->catalogue.device_name_length))return 1;
 return 0;
}
#endif
