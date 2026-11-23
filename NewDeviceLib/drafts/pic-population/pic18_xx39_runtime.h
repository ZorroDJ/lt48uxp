#ifndef LT48_DRAFT_PIC18_XX39_RUNTIME_H
#define LT48_DRAFT_PIC18_XX39_RUNTIME_H
#include "../../sources/pic-common/pic18_xx39_timing.h"
#include "pic18_xx39_erase_applet.h"
#include "pic18_xx39_eeprom_read.h"
/* Complete candidate resource composition; public admission remains closed.
 * Source83/88/89/8A+81 replacement preserves the motor-control kernel.
 * Independent UID/configuration/protection qualification is still required. */
static inline int lt48_pic18_xx39_configuration_binding(uint32_t d,uint16_t f,uint16_t p) {
 return ((d==28||d==29)&&f==22&&p==10)||((d==30||d==33)&&f==23&&p==11)||
        ((d==31||d==34)&&f==24&&p==12)||((d==32||d==35)&&f==25&&p==13);
}
static inline lt48_status lt48_pic18_xx39_complete_runtime_open(const char *path,
 const unsigned char digest[32],const lt48_device_spec *spec,uint32_t code_bytes,
 lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!lt48_pic18_xx39_timing_allowed(spec))return LT48_SAFETY_REJECTED;
 unsigned expected=(spec->definition_resource==28||spec->definition_resource==30||spec->definition_resource==31||spec->definition_resource==32)?12288:24576;
 if(code_bytes!=expected)return LT48_SAFETY_REJECTED;
 lt48_pic_source_resources *before=NULL;
 /* This helper checks the exact source digest and original timing domain. */
 lt48_status st=lt48_pic18_xx39_timing_open(path,digest,spec,&before);
 if(st!=LT48_OK)return st;
 unsigned char bytes[5][2048];lt48_pic_source_ram_overlay overlays[5]={{0}};
 unsigned found=0,roles=0;
 for(size_t i=0;st==LT48_OK&&i<lt48_pic_source_resources_count_internal(before);i++) {
  lt48_pic_source_resource_entry e;st=lt48_pic_source_resources_entry_internal(before,i,&e);
  if(st!=LT48_OK)break;
  if(e.role!=17&&e.role!=19&&e.role!=21&&e.role!=23&&e.role!=24)continue;
  unsigned bit=e.role==17?1:e.role==19?2:e.role==21?4:e.role==23?8:16;
  if(found==5||(roles&bit)||e.source.wave_type!=6){st=LT48_SAFETY_REJECTED;break;}
  lt48_pic_source_ram_overlay *o=overlays+found;
  if(e.role==23)st=lt48_pic18_xx39_erase_applet(e.source.data,e.source.data_size,e.source.load_address,code_bytes,bytes[found],sizeof(bytes[found]),&o->size);
  else if(e.role==24)st=lt48_pic18_x220_pc_relocation(e.source.data,e.source.data_size,e.source.load_address,bytes[found],sizeof(bytes[found]),&o->size);
  else st=lt48_pic18_xx39_eeprom_read_applet(e.role,e.source.data,e.source.data_size,e.source.load_address,bytes[found],sizeof(bytes[found]),&o->size);
  if(st!=LT48_OK)break;
  o->role=e.role;o->load_address=e.source.load_address;
  o->slot_end=e.role==17?0x7000:e.role==19?0x8000:e.role==21?0x7800:e.role==23?0x9800:0xa000;
  o->original_size=e.source.data_size;o->data=bytes[found];
  lt48_sha256(e.source.data,e.source.data_size,o->original_sha256);lt48_sha256(o->data,o->size,o->sha256);
  roles|=bit;found++;
 }
 if(st==LT48_OK&&(found!=5||roles!=31))st=LT48_SAFETY_REJECTED;
 const lt48_pic_source_time_override timing={{1,10,1,1,70},{1,10,1,1,110}};
 if(st==LT48_OK)st=lt48_pic_source_resources_overlay_time_library_internal(path,digest,spec->family_code,spec->pinmap,overlays,found,&timing,1,out);
 lt48_pic_source_resources_close_internal(before);return st;
}
#endif
