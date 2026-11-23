#ifndef LT48_PIC18_F6_RUNTIME_H
#define LT48_PIC18_F6_RUNTIME_H
#include "source_resources.h"
#include "pic18_eeprom_timeout.h"
#include "pic18_f6_configuration_recovery.h"
#include "pic18_f6_erase_selector.h"
#include "pic18_f6_runtime_profiles.h"
/* Exact eight-profile BYO13.10 owner. All overlays are temporary MCU RAM
 * payloads built by the qualified C composers; no original firmware included.
 * This constructor does not grant operator admission. */
static inline const lt48_pic18_f6_runtime_policy *lt48_pic18_f6_runtime_find(uint32_t d,uint16_t f,uint16_t p){
 for(size_t i=0;i<sizeof(lt48_pic18_f6_runtime_profiles)/sizeof(*lt48_pic18_f6_runtime_profiles);i++){
  const lt48_pic18_f6_runtime_policy *r=lt48_pic18_f6_runtime_profiles+i;
  if(r->definition==d&&r->family==f&&r->pinmap==p)return r;
 }return NULL;
}
static inline const unsigned char *lt48_pic18_f6_erase_mask(uint32_t d,uint16_t f,uint16_t p){
 const lt48_pic18_f6_runtime_policy *r=lt48_pic18_f6_runtime_find(d,f,p);
 return r&&!r->modern?r->erase_mask:NULL;
}
static inline lt48_status lt48_pic18_f6_runtime_open(const char *path,const unsigned char digest[32],
 const lt48_device_spec *spec,lt48_pic_source_resources **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!path||!digest||!spec||!spec->definition_stem||!spec->catalogue.device_name)return LT48_INVALID_ARGUMENT;
 const lt48_pic18_f6_runtime_policy *policy=lt48_pic18_f6_runtime_find(spec->definition_resource,spec->family_code,spec->pinmap);
 static const unsigned char expected[32]={0xc0,0xe9,0x0a,0x55,0xe9,0x7a,0x3a,0xc1,0x2d,0x73,0x00,0x6e,0xb9,0xda,0x11,0x39,0xe3,0xd5,0x68,0x8a,0xef,0x85,0x24,0x2b,0x00,0x01,0xbc,0x40,0x6c,0xab,0x46,0x40};
 if(!policy||spec->catalogue.device_class!=2048||strcmp(spec->definition_stem,"GDCC___")||
    spec->catalogue.device_name_length!=strlen(policy->name)||memcmp(spec->catalogue.device_name,policy->name,strlen(policy->name))||
    memcmp(digest,expected,32))return LT48_SAFETY_REJECTED;
 lt48_pic_source_resources *original=NULL;
 lt48_status st=lt48_pic_source_resources_library_internal(path,digest,policy->family,policy->pinmap,&original);
 if(st!=LT48_OK)return st;
 unsigned char program[2200],configuration[2048],erase[800];size_t program_size=0,configuration_size=0;
 lt48_pic_source_ram_overlay overlays[3]={0};unsigned found=0;size_t count=0;
 for(size_t i=0;st==LT48_OK&&i<lt48_pic_source_resources_count_internal(original);i++){
  lt48_pic_source_resource_entry e;st=lt48_pic_source_resources_entry_internal(original,i,&e);if(st!=LT48_OK)break;
  if(e.role!=20&&e.role!=24&&e.role!=23)continue;
  unsigned bit=e.role==20?1u:e.role==24?2u:4u;
  if(e.source.wave_type!=6||(found&bit)){st=LT48_SAFETY_REJECTED;break;}found|=bit;
  const unsigned char *bytes=NULL;size_t size=0;uint16_t end=0;
  if(e.role==20){
   st=lt48_pic18_patch_f6_program(e.source.data,e.source.data_size,e.source.load_address,program,sizeof(program),&program_size);
   bytes=program;size=program_size;end=0x9000;
  }else if(policy->modern&&e.role==24){
   st=lt48_pic18_patch_f6_configuration_recovery(e.source.data,e.source.data_size,e.source.load_address,configuration,sizeof(configuration),&configuration_size);
   bytes=configuration;size=configuration_size;end=0xa000;
  }else if(policy->modern&&e.role==23){
   st=lt48_pic18_patch_f6_erase_selector(e.source.data,e.source.data_size,e.source.load_address,erase,sizeof(erase));
   bytes=erase;size=sizeof(erase);end=0x9800;
  }
  if(st==LT48_OK&&bytes){
   lt48_pic_source_ram_overlay *o=overlays+count++;o->role=e.role;o->load_address=e.source.load_address;o->slot_end=end;
   o->original_size=e.source.data_size;o->data=bytes;o->size=size;
   lt48_sha256(e.source.data,e.source.data_size,o->original_sha256);lt48_sha256(bytes,size,o->sha256);
  }
 }
 if(st==LT48_OK&&(found!=7||count!=(policy->modern?3u:1u)))st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK)st=lt48_pic_source_resources_overlay_library_internal(path,digest,policy->family,policy->pinmap,overlays,count,out);
 lt48_pic_source_resources_close_internal(original);return st;
}
#endif
