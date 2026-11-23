#ifndef LT48_PIC_CALIBRATED14_ADMISSIONS_H
#define LT48_PIC_CALIBRATED14_ADMISSIONS_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Exact six-alias original-workflow cohort; geometry never grants admission. */
static inline int lt48_pic_cal14_public_allowed(const lt48_device_spec *s){
 if(!s||s->catalogue.device_class!=2015||!s->catalogue.vendor||strcmp(s->catalogue.vendor,"Microchip")||!s->catalogue.device_name)return 0;
 static const struct {uint32_t d;uint16_t f,p;const char *name;} rows[]={
 {338,229,77,"PIC12F629"},
 {338,229,77,"PIC12F629 @8DFN-S"},
 {338,229,77,"PIC12F675"},
 {338,229,77,"PIC12F675 @8DFN-S"},
 {339,230,78,"PIC16F630"},
 {339,230,78,"PIC16F676"},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(s->definition_resource==rows[i].d&&s->family_code==rows[i].f&&s->pinmap==rows[i].p&&s->catalogue.device_name_length==strlen(rows[i].name)&&!memcmp(s->catalogue.device_name,rows[i].name,s->catalogue.device_name_length))return 1;
 return 0;
}
#endif
