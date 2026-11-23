#ifndef LT48_PIC_CLASSIC14_PROTOCOL_H
#define LT48_PIC_CLASSIC14_PROTOCOL_H
#include <stdint.h>
#include <stddef.h>
#include <labtool48/labtool48.h>

/* Authored protocol-family engine, independent of compact source instruction
 * shapes. DS41244C sections 3.1/3.1.6 and DS41287D sections 3.2/3.2.6.
 * The owner supplies audited entry/re-entry, electrical transport and cleanup.
 * This module alone never grants catalogue availability. */
typedef struct {
 unsigned program_words, eeprom_bytes, latches, configuration_words;
 uint16_t configuration_mask[2];
 unsigned code_program_us, data_program_us, erase_us, discharge_us;
 unsigned calibration_words;
} lt48_pic_classic_protocol;
typedef struct {
 void *context;
 lt48_status (*transfer)(void *,int,unsigned,uint32_t,uint32_t *);
 lt48_status (*delay)(void *,unsigned);
 lt48_status (*reenter)(void *);
} lt48_pic_classic_wire;
/* Preserve the stored profile schema; this exact trait tuple selects the
 * separate DS39589C engine. No existing classic tuple has this mask/calibration. */
static inline int lt48_pic_classic_is87xa(const lt48_pic_classic_protocol *p){
 return p&&((p->program_words==4096&&p->eeprom_bytes==128)||
            (p->program_words==8192&&p->eeprom_bytes==256))&&p->latches==8&&
  p->configuration_words==1&&p->configuration_mask[0]==0x2fcf&&!p->configuration_mask[1]&&
  p->code_program_us==3000&&p->data_program_us==3000&&p->erase_us==10000&&
  p->discharge_us==1&&!p->calibration_words;
}
/* DS41332D: no data EEPROM, eight code latches and two immutable
 * factory words. This bounded tuple never changes an earlier family. */
static inline int lt48_pic_classic_is72x(const lt48_pic_classic_protocol *p){
 return p&&(p->program_words==2048||p->program_words==4096||p->program_words==8192)&&
  !p->eeprom_bytes&&p->latches==8&&p->configuration_words==2&&
  p->configuration_mask[0]==0x377f&&p->configuration_mask[1]==0x30&&
  p->code_program_us==2500&&!p->data_program_us&&p->erase_us==6000&&
  p->discharge_us==100&&p->calibration_words==2;
}
/* DS41196G and DS30034D have one physical program latch and no
 * factory words. Their programming/erase commands differ despite shared pins. */
static inline int lt48_pic_classic_is62xa(const lt48_pic_classic_protocol *p){
 return p&&((p->program_words==1024&&p->eeprom_bytes==128)||
  (p->program_words==2048&&p->eeprom_bytes==128)||(p->program_words==4096&&p->eeprom_bytes==256))&&
  p->latches==1&&p->configuration_words==1&&p->configuration_mask[0]==0x21ff&&!p->configuration_mask[1]&&
  p->code_program_us==4000&&p->data_program_us==6000&&p->erase_us==6000&&p->discharge_us==1&&!p->calibration_words;
}
static inline int lt48_pic_classic_is62x(const lt48_pic_classic_protocol *p){
 return p&&(p->program_words==1024||p->program_words==2048)&&p->eeprom_bytes==128&&
  p->latches==1&&p->configuration_words==1&&p->configuration_mask[0]==0x3dff&&!p->configuration_mask[1]&&
  p->code_program_us==8000&&p->data_program_us==8000&&p->erase_us==13000&&p->discharge_us==1&&!p->calibration_words;
}
/* DS39588A/DS30324B/DS30492B: two-word latches, externally ended
 * 08h/0Eh cycles, 30-ms chip erase and no data EEPROM or factory words. */
static inline int lt48_pic_classic_is7x(const lt48_pic_classic_protocol *p){
 return p&&(p->program_words==2048||p->program_words==4096||p->program_words==8192)&&
  !p->eeprom_bytes&&p->latches==2&&!p->calibration_words&&
  ((p->configuration_words==1&&p->configuration_mask[0]==0x5f&&!p->configuration_mask[1])||
   (p->configuration_words==2&&p->configuration_mask[0]==0x39ff&&p->configuration_mask[1]==0x43&&p->program_words>=4096))&&
  p->code_program_us==1000&&!p->data_program_us&&p->erase_us==30000&&p->discharge_us==1;
}
/* DS39025F: one erase/program latch, replicated CP fields and no factory
 * calibration. Geometry is bounded separately from the later A-generation. */
static inline int lt48_pic_classic_is87x(const lt48_pic_classic_protocol *p){
 return p&&((p->program_words==2048&&p->eeprom_bytes==64)||
  (p->program_words==4096&&p->eeprom_bytes==128)||
  (p->program_words==8192&&p->eeprom_bytes==256))&&p->latches==1&&
  p->configuration_words==1&&p->configuration_mask[0]==0x3bff&&!p->configuration_mask[1]&&
  p->code_program_us==4000&&p->data_program_us==4000&&p->erase_us==8000&&
  p->discharge_us==1&&!p->calibration_words;
}
/* DS39603C/DS39607C: four latches, externally ended 18h/17h,
 * no exposed factory words and bounded one/two-configuration geometries. */
static inline int lt48_pic_classic_isrow4(const lt48_pic_classic_protocol *p){
 return p&&p->latches==4&&!p->calibration_words&&p->configuration_mask[0]==0x3fff&&
  ((p->program_words==1024&&p->eeprom_bytes==128&&p->configuration_words==1&&!p->configuration_mask[1])||
   (p->program_words==2048&&p->eeprom_bytes==256&&p->configuration_words==1&&!p->configuration_mask[1])||
   (p->program_words==4096&&p->eeprom_bytes==256&&p->configuration_words==2&&p->configuration_mask[1]==3))&&
  p->code_program_us==3000&&p->data_program_us==3000&&p->erase_us==10000&&p->discharge_us==1;
}
static inline int lt48_pic_classic_protocol_valid(const lt48_pic_classic_protocol *p){
 if(lt48_pic_classic_is87xa(p)||lt48_pic_classic_is72x(p)||lt48_pic_classic_is62xa(p)||lt48_pic_classic_is62x(p)||lt48_pic_classic_is7x(p)||lt48_pic_classic_is87x(p)||lt48_pic_classic_isrow4(p))return 1;
 return p&&(p->program_words==2048||p->program_words==4096||p->program_words==8192)&&
  (p->eeprom_bytes==128||p->eeprom_bytes==256)&&
  p->latches==(p->program_words>4096?8u:4u)&&
  (p->configuration_words==1||p->configuration_words==2)&&
  p->configuration_mask[0]&&!(p->configuration_mask[0]&~0x3fff)&&
  (p->configuration_words==2?p->configuration_mask[1]==0x700:!p->configuration_mask[1])&&
  p->code_program_us==(p->configuration_words==2?3000u:2500u)&&
  p->data_program_us==6000&&p->erase_us==6000&&p->discharge_us==100&&
  p->calibration_words==3-p->configuration_words;
}
static inline lt48_status lt48_pic_classic_command(const lt48_pic_classic_wire *w,unsigned cmd){
 uint32_t out=0;lt48_status s=w->transfer(w->context,0,6,cmd,&out);
 return s==LT48_OK?w->delay(w->context,1):s;
}
static inline lt48_status lt48_pic_classic_frame(const lt48_pic_classic_wire *w,int read,uint16_t *value){
 uint32_t out=0;lt48_status s=w->transfer(w->context,0,1,0,&out);
 if(s==LT48_OK)s=w->transfer(w->context,read,14,read?0:*value,&out);
 if(s==LT48_OK&&read)*value=(uint16_t)out;
 if(s==LT48_OK)s=w->transfer(w->context,0,1,0,&out);
 if(s==LT48_OK)s=w->delay(w->context,1);
 return s;
}
static inline lt48_status lt48_pic_classic_load(const lt48_pic_classic_wire *w,unsigned cmd,uint16_t value){
 lt48_status s=lt48_pic_classic_command(w,cmd);
 return s==LT48_OK?lt48_pic_classic_frame(w,0,&value):s;
}
static inline lt48_status lt48_pic_classic_seek(const lt48_pic_classic_wire *w,unsigned address){
 lt48_status s=w->reenter(w->context);
 if(s==LT48_OK&&address>=0x2000&&address<0x2100){
  s=lt48_pic_classic_load(w,0,0x3fff);address-=0x2000;
 }else if(address>=0x2100)address-=0x2100;
 for(unsigned i=0;s==LT48_OK&&i<address;i++)s=lt48_pic_classic_command(w,6);
 return s;
}
static inline int lt48_pic_classic_range(const lt48_pic_classic_protocol *p,unsigned address,size_t n,int write){
 if(!n||n>0x4000)return 0;
 if(address<p->program_words)return n<=p->program_words-address;
 if(address>=0x2100)return address-0x2100<p->eeprom_bytes&&n<=p->eeprom_bytes-(address-0x2100);
 if(address>=0x2000&&address<0x2004)return n<=0x2004-address;
 return !write&&address>=0x2004&&address<0x2007+p->configuration_words+p->calibration_words&&
  n<=0x2007+p->configuration_words+p->calibration_words-address;
}
static inline int lt48_pic_classic_wire_valid(const lt48_pic_classic_wire *w){
 return w&&w->transfer&&w->delay&&w->reenter;
}
static inline lt48_status lt48_pic_classic_read_range(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,unsigned address,uint16_t *values,size_t n){
 if(!lt48_pic_classic_protocol_valid(p)||!lt48_pic_classic_wire_valid(w)||!values||
    !lt48_pic_classic_range(p,address,n,0))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_command(w,address>=0x2100?5:4);
  if(s==LT48_OK)s=lt48_pic_classic_frame(w,1,&values[i]);
  if(s==LT48_OK&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
/* Streaming verification has bounded storage and one traversal. NULL expected
 * selects blank-check; every read still goes through the physical endpoint. */
static inline lt48_status lt48_pic_classic_check_range(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,unsigned address,const uint16_t *expected,size_t n,
 unsigned *failed_address){
 if(!lt48_pic_classic_protocol_valid(p)||!lt48_pic_classic_wire_valid(w)||
    !lt48_pic_classic_range(p,address,n,0))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  uint16_t value=0;
  s=lt48_pic_classic_command(w,address>=0x2100?5:4);
  if(s==LT48_OK)s=lt48_pic_classic_frame(w,1,&value);
  if(s==LT48_OK&&value!=(expected?expected[i]:address>=0x2100?255:0x3fff)){
   if(failed_address)*failed_address=address+(unsigned)i;
   return LT48_DEVICE_MISMATCH;
  }
  if(s==LT48_OK&&i+1<n)s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_classic_program_range(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,unsigned address,const uint16_t *values,size_t n){
 if(!lt48_pic_classic_protocol_valid(p)||!lt48_pic_classic_wire_valid(w)||!values||
    !lt48_pic_classic_range(p,address,n,1))return LT48_INVALID_ARGUMENT;
 unsigned code=address<p->program_words,data=address>=0x2100;
 if(code&&(address%p->latches||n%p->latches))return LT48_INVALID_ARGUMENT;
 /* Validate before any bus transaction, including the first entry. */
 for(size_t i=0;i<n;i++)if(values[i]>(data?255:0x3fff))return LT48_INVALID_ARGUMENT;
 lt48_status s=lt48_pic_classic_seek(w,address);
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  /* Configuration-space latches persist. Reset them between individual UID
   * writes, then seek the actual register; do not batch UIDs as code rows. */
  if(!code&&!data&&i)s=lt48_pic_classic_seek(w,address+(unsigned)i);
  if(s==LT48_OK)s=lt48_pic_classic_load(w,data?3:2,values[i]);
  if(s==LT48_OK&&(!code||(i+1)%p->latches==0)){
   s=lt48_pic_classic_command(w,lt48_pic_classic_is62x(p)?24:8);
   if(s==LT48_OK)s=w->delay(w->context,data?p->data_program_us:p->code_program_us);
  }
  if(s==LT48_OK&&i+1<n&&(code||data))s=lt48_pic_classic_command(w,6);
 }
 return s;
}
static inline lt48_status lt48_pic_classic_configuration(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w,const uint16_t *values,size_t n){
 if(!lt48_pic_classic_protocol_valid(p)||!lt48_pic_classic_wire_valid(w)||!values||n!=p->configuration_words)
  return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<n;i++)if(values[i]>0x3fff)return LT48_INVALID_ARGUMENT;
 if(lt48_pic_classic_is62x(p)&&((values[0]>>10)&3)!=((values[0]>>12)&3))return LT48_SAFETY_REJECTED;
 lt48_status s=LT48_OK;
 for(size_t i=0;s==LT48_OK&&i<n;i++){
  s=lt48_pic_classic_seek(w,0x2007+(unsigned)i);
  if(s==LT48_OK)s=lt48_pic_classic_load(w,2,values[i]|(0x3fff^p->configuration_mask[i]));
  if(s==LT48_OK)s=lt48_pic_classic_command(w,lt48_pic_classic_is62x(p)?24:8);
  if(s==LT48_OK)s=w->delay(w->context,lt48_pic_classic_is72x(p)?5000:p->code_program_us);
 }
 return s;
}
static inline lt48_status lt48_pic_classic_erase(const lt48_pic_classic_protocol *p,
 const lt48_pic_classic_wire *w){
 if(!lt48_pic_classic_protocol_valid(p)||!lt48_pic_classic_wire_valid(w))return LT48_INVALID_ARGUMENT;
 if(lt48_pic_classic_is62x(p)){
  /* DS30034D 3.3: reset protection (and all memories), not a plain
   * bulk-erase command that would do nothing on protected devices. */
  lt48_status s=w->reenter(w->context);
  if(s==LT48_OK)s=lt48_pic_classic_load(w,0,0);
  for(unsigned i=0;s==LT48_OK&&i<7;i++)s=lt48_pic_classic_command(w,6);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,1);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,7);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,8);
  if(s==LT48_OK)s=w->delay(w->context,p->erase_us);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,1);
  if(s==LT48_OK)s=lt48_pic_classic_command(w,7);
  return s;
 }
 /* PC 2000 includes user IDs and configurations while preserving calibration.
  * Erasing code clears CPD, so subsequent explicit data erase is permitted. */
 lt48_status s=lt48_pic_classic_seek(w,0x2000);
 if(s==LT48_OK&&lt48_pic_classic_is62xa(p))s=lt48_pic_classic_load(w,2,0x3fff);
 if(s==LT48_OK)s=lt48_pic_classic_command(w,9);
 if(s==LT48_OK)s=w->delay(w->context,p->erase_us);
 if(s==LT48_OK&&p->eeprom_bytes)s=lt48_pic_classic_command(w,11);
 if(s==LT48_OK&&p->eeprom_bytes)s=w->delay(w->context,p->erase_us);
 return s;
}
#endif
