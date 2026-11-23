#ifndef LT48_PIC18_X220_RUNTIME_H
#define LT48_PIC18_X220_RUNTIME_H
#include "source_resources.h"
#include "pic18_x220_pc_relocation.h"
/* Exact source applicability for owned RAM correction. Public admission remains separate. */
typedef struct {uint32_t definition;uint16_t family,pinmap;const char *name;} lt48_x220_candidate;
static const lt48_x220_candidate lt48_x220_candidates[]={
 {16,16,4,"PIC18F1220"},
 {16,16,4,"PIC18F1220 *18SO"},
 {17,17,5,"PIC18F1220 *20SSOP"},
 {18,16,4,"PIC18F1320"},
 {18,16,4,"PIC18F1320 *18SO"},
 {19,17,5,"PIC18F1320 *20SSOP"},
 {20,18,6,"PIC18F2220"},
 {20,18,6,"PIC18F2220 *28SO"},
 {21,18,6,"PIC18F2320"},
 {21,18,6,"PIC18F2320 *28SO"},
 {22,19,7,"PIC18F4220"},
 {23,20,8,"PIC18F4220 *44TQ"},
 {24,21,9,"PIC18F4220 @44QFN"},
 {25,19,7,"PIC18F4320"},
 {26,20,8,"PIC18F4320 *44TQ"},
 {27,21,9,"PIC18F4320 @44QFN"},
};
static inline int lt48_x220_candidate_allowed(uint32_t d,uint16_t f,uint16_t p,const unsigned char *name,size_t size) {
 for(size_t i=0;i<sizeof(lt48_x220_candidates)/sizeof(*lt48_x220_candidates);i++) {
  const lt48_x220_candidate *r=lt48_x220_candidates+i;
  if(r->definition==d&&r->family==f&&r->pinmap==p&&name&&strlen(r->name)==size&&!memcmp(r->name,name,size))return 1;
 }return 0;
}
static inline lt48_status lt48_x220_runtime_time_open(const char *path,const unsigned char digest[32],
 uint16_t family,uint16_t pinmap,const lt48_pic_source_time_override *times,size_t time_count,lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_pic_source_resources *before=NULL;
 lt48_status st=lt48_pic_source_resources_library_internal(path,digest,family,pinmap,&before);
 if(st!=LT48_OK)return st;
 unsigned char data[873];size_t size=0;lt48_pic_source_ram_overlay overlay={0};unsigned found=0;
 for(size_t i=0;st==LT48_OK&&i<lt48_pic_source_resources_count_internal(before);i++) {
  lt48_pic_source_resource_entry e;st=lt48_pic_source_resources_entry_internal(before,i,&e);if(st!=LT48_OK)break;
  if(e.role!=24)continue;
  if(found++||e.source.wave_type!=6){st=LT48_SAFETY_REJECTED;break;}
  st=lt48_pic18_x220_pc_relocation(e.source.data,e.source.data_size,e.source.load_address,data,sizeof(data),&size);
  overlay.role=24;overlay.load_address=0x9800;overlay.slot_end=0xa000;overlay.original_size=789;
  overlay.data=data;overlay.size=size;
 }
 if(st==LT48_OK&&found!=1)st=LT48_SAFETY_REJECTED;
 static const unsigned char old_sha[32]={0xc6,0xa2,0x26,0x1b,0x03,0x67,0xf2,0x6b,0x87,0xca,0xa9,0xe4,0x13,0x8b,0x76,0x8e,0x7a,0x93,0x95,0xaf,0x7b,0xd6,0x59,0xda,0x66,0x6f,0x81,0xdc,0x6d,0xed,0x9f,0x65};
 static const unsigned char new_sha[32]={0x28,0xaa,0x2c,0x19,0xdf,0x2b,0x9e,0xb6,0xda,0x93,0x82,0x7a,0x59,0x2e,0xdb,0xd5,0x6f,0x61,0x6c,0x08,0x4a,0x92,0x76,0xe4,0x96,0x6f,0x16,0xba,0x4f,0x93,0x53,0xf1};
 memcpy(overlay.original_sha256,old_sha,32);memcpy(overlay.sha256,new_sha,32);
 if(st==LT48_OK)st=time_count?
  lt48_pic_source_resources_overlay_time_library_internal(path,digest,family,pinmap,&overlay,1,times,time_count,out):
  lt48_pic_source_resources_overlay_library_internal(path,digest,family,pinmap,&overlay,1,out);
 lt48_pic_source_resources_close_internal(before);return st;
}
static inline lt48_status lt48_x220_runtime_open(const char *path,const unsigned char digest[32],
 uint16_t family,uint16_t pinmap,lt48_pic_source_resources **out) {
 return lt48_x220_runtime_time_open(path,digest,family,pinmap,NULL,0,out);
}
#endif
