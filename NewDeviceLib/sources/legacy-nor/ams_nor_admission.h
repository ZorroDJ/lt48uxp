#ifndef LT48_AMS_NOR_ADMISSION_H
#define LT48_AMS_NOR_ADMISSION_H
#include <stddef.h>
#include <string.h>
/* Exact 13.10 profiles; the loader separately checks the complete REZ hash,
 * original operation tuples, pin map, supply variables and capacity. */
static int ams_nor(unsigned cls,unsigned def,unsigned man,unsigned id,
 const char *vendor,const unsigned char *name,size_t length) {
 static const struct {unsigned def,man,id;const char *vendor,*name;} rows[]={
  {0xb4,0x1f,0x22,"Atmel","AT49BV008A *40TS"},
  {0xb5,0x1f,0x21,"Atmel","AT49BV008AT *40TS"},
  {0x8b,0x20,0xf2,"ST","M28F410 *44P"},
  {0x59,0x20,0xf6,"ST","28F411 *40TS"},
  {0x8a,0x20,0xfa,"ST","M28F420 *44P"}};
 if(cls!=2007||!vendor||!name)return 0;
 for(size_t i=0;i<sizeof(rows)/sizeof(*rows);i++)if(def==rows[i].def&&man==rows[i].man&&id==rows[i].id&&!strcmp(vendor,rows[i].vendor)&&length==strlen(rows[i].name)&&!memcmp(name,rows[i].name,length))return 1;
 return 0;
}
#endif
