#ifndef LT48_PIC_FLASH14_87XA_H
#define LT48_PIC_FLASH14_87XA_H
#include "classic14_protocol.h"
/* DS39589C: eight latches, externally timed command 18h/17h, 1-ms
 * minimum; configuration and chip erase require the 10-ms electrical bound.
 * Entry, rails, configuration conversion and publication belong to the owner.
 * This protocol component does not grant any catalogue admission. */
typedef struct {unsigned program_words,eeprom_bytes;} lt48_pic_87xa_profile;
static inline int lt48_pic_87xa_valid(const lt48_pic_87xa_profile *p){
 return p&&((p->program_words==4096&&p->eeprom_bytes==128)||
            (p->program_words==8192&&p->eeprom_bytes==256));
}
static inline int lt48_pic_87xa_range(const lt48_pic_87xa_profile *p,unsigned address,size_t n,int write){
 if(!lt48_pic_87xa_valid(p)||!n||n>8192)return 0;
 if(address<p->program_words)return n<=p->program_words-address;
 if(address>=0x2100&&address<0x2100+p->eeprom_bytes)return n<=0x2100+p->eeprom_bytes-address;
 if(address>=0x2000&&address<0x2004)return n<=0x2004-address;
 return !write&&address>=0x2004&&address<=0x2007&&n<=0x2008-address;
}
static inline lt48_status lt48_pic_87xa_seek(const lt48_pic_classic_wire *w,unsigned address){
 lt48_status s=w->reenter(w->context);
 if(s==LT48_OK&&address>=0x2000&&address<0x2100){
  s=lt48_pic_classic_load(w,0,0x3fff);address-=0x2000;
 }else if(address>=0x2100)address-=0x2100;
 for(unsigned i=0;s==LT48_OK&&i<address;i++)s=lt48_pic_classic_command(w,6);
 return s;
}
static inline lt48_status lt48_pic_87xa_read(const lt48_pic_87xa_profile *p,
 const lt48_pic_classic_wire *w,unsigned address,uint16_t *values,size_t n){
 if(!lt48_pic_87xa_range(p,address,n,0)||!lt48_pic_classic_wire_valid(w)||!values)return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_87xa_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_command(w,address>=0x2100?5:4);
  if(s==LT48_OK)s=lt48_pic_classic_frame(w,1,values+i);
  if(s==LT48_OK&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_87xa_program(const lt48_pic_87xa_profile *p,
 const lt48_pic_classic_wire *w,unsigned address,const uint16_t *values,size_t n){
 if(!lt48_pic_87xa_range(p,address,n,1)||!lt48_pic_classic_wire_valid(w)||!values)return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(values[i]>(address>=0x2100?255:0x3fff))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_87xa_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_load(w,address>=0x2100?3:2,values[i]);
  unsigned commit=address>=0x2100||i+1==n||
      (address<0x2000?((address+(unsigned)i)&7)==7:address+i==0x2003);
  if(s==LT48_OK&&commit)s=lt48_pic_classic_command(w,0x18);
  if(s==LT48_OK&&commit)s=w->delay(w->context,3000);
  if(s==LT48_OK&&commit)s=lt48_pic_classic_command(w,0x17);
  if(s==LT48_OK&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_87xa_configuration(const lt48_pic_87xa_profile *p,
 const lt48_pic_classic_wire *w,uint16_t value){
 if(!lt48_pic_87xa_valid(p)||!lt48_pic_classic_wire_valid(w)||value>0x3fff)return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_87xa_seek(w,0x2007);
 if(s==LT48_OK)s=lt48_pic_classic_load(w,2,value|0x1030);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,8);
  if(s==LT48_OK)s=w->delay(w->context,10000);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,0x17);
 return s;
}
static inline lt48_status lt48_pic_87xa_erase(const lt48_pic_87xa_profile *p,const lt48_pic_classic_wire *w){
 if(!lt48_pic_87xa_valid(p)||!lt48_pic_classic_wire_valid(w))return LT48_INVALID_ARGUMENT;
 /* PC must select the test row to erase IDs as well as code/data/config. */
 lt48_status s=lt48_pic_87xa_seek(w,0x2000);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,0x1f);
 if(s==LT48_OK)s=w->delay(w->context,10000);
 return s;
}
#endif
