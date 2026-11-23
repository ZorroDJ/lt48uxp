/* Exact authored ACTRANS admission; original resources remain BYO. */
#ifndef LT48_ACTRANS4_ADMISSION_H
#define LT48_ACTRANS4_ADMISSION_H
#include <stddef.h>
#include <string.h>
static int actrans4(unsigned cls,unsigned def,unsigned manufacturer,unsigned id,const char *vendor,const unsigned char *name,size_t length) {
 if(cls!=2004 || manufacturer!=0x7f || !vendor || strcmp(vendor,"ACTRANS") || !name)return 0;
 static const struct {unsigned def,id;const char *name;} rows[]={
{174,3,"AC29LV400B *44PS"},
{172,3,"AC29LV400B *48TS"},
{173,2,"AC29LV400T *44PS"},
{171,2,"AC29LV400T *48TS"},
 };
 for(size_t i=0;i<sizeof(rows)/sizeof(*rows);i++)if(def==rows[i].def && id==rows[i].id && length==strlen(rows[i].name) && !memcmp(name,rows[i].name,length))return 1;
 return 0;
}
#endif
