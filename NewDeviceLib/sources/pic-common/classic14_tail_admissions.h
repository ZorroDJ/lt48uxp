#ifndef LT48_PIC_TAIL14_ADMISSIONS_H
#define LT48_PIC_TAIL14_ADMISSIONS_H
#include <labtool48/labtool48.h>
#include <string.h>
/* Exact eight-alias original-workflow cohort; geometry never grants admission. */
static inline int lt48_pic_tail14_public_allowed(const lt48_device_spec *s){
 if(!s||s->catalogue.device_class!=2015||!s->catalogue.vendor||strcmp(s->catalogue.vendor,"Microchip")||!s->catalogue.device_name)return 0;
 static const struct {uint32_t d;uint16_t f,p;const char *name;} rows[]={
 {369,256,89,"PIC12F617"},
 {369,256,89,"PIC12F617 *8SO"},
 {63,54,21,"PIC16F716"},
 {63,54,21,"PIC16F716 *18SO"},
 {64,54,22,"PIC16F716 *20SSOP"},
 {65,55,23,"PIC16F785"},
 {65,55,23,"PIC16F785 *20SO"},
 {65,55,23,"PIC16F785 *20SSOP"},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(s->definition_resource==rows[i].d&&s->family_code==rows[i].f&&s->pinmap==rows[i].p&&s->catalogue.device_name_length==strlen(rows[i].name)&&!memcmp(s->catalogue.device_name,rows[i].name,s->catalogue.device_name_length))return 1;
 return 0;
}
#endif
