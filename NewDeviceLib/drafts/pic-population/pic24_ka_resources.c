#include "pic24_ka_resources.h"
#include "pic24_ka_erase_applet.h"
#include <stdlib.h>
typedef struct {const char *key;
uint32_t words;
unsigned series,fv,pins;
uint16_t family,pinmap;
} ka_profile;
#include "pic24_ka_profiles_generated.inc"
struct lt48_pic24_ka_resources {
 const ka_profile *profile;
 lt48_pic24_ka_image_policy geometry;
 lt48_pic24_ka_configuration_rules configuration;
 lt48_pic_source_resources *code,*eeprom;
};
static const unsigned char ka_source[32]={0xc0,0xe9,0x0a,0x55,0xe9,0x7a,0x3a,0xc1,0x2d,0x73,0x00,0x6e,0xb9,0xda,0x11,0x39,0xe3,0xd5,0x68,0x8a,0xef,0x85,0x24,0x2b,0x00,0x01,0xbc,0x40,0x6c,0xab,0x46,0x40};
void lt48_pic24_ka_resources_close(lt48_pic24_ka_resources *r){if(r){lt48_pic_source_resources_close_internal(r->code);
lt48_pic_source_resources_close_internal(r->eeprom);
free(r);
}}
lt48_status lt48_pic24_ka_resources_open(const char *path,const char *key,lt48_pic24_ka_resources **output){
 if(!output)return LT48_INVALID_ARGUMENT;
 *output=NULL;
 if(!path||!key)return LT48_INVALID_ARGUMENT;
 const ka_profile *p=NULL;
 for(size_t i=0;i<sizeof(ka_profiles)/sizeof(*ka_profiles);i++)if(!strcmp(key,ka_profiles[i].key)){p=ka_profiles+i;
 break;
 }
 if(!p)return LT48_UNSUPPORTED;
 lt48_pic24_ka_resources *r=calloc(1,sizeof(*r));
 if(!r)return LT48_NO_MEMORY;
 r->profile=p;
 if(!lt48_pic24_ka_image_policy_prepare(p->words,4*(p->words+256),&r->geometry)||
    !lt48_pic24_ka_configuration_rules_prepare(p->series,p->words,p->fv,p->pins,&r->configuration)){
  lt48_pic24_ka_resources_close(r);
  return LT48_SAFETY_REJECTED;
 }
 lt48_status status=lt48_pic_source_resources_library_internal(path,ka_source,p->family,p->pinmap,&r->code);
 lt48_pic_source_resource_entry erase={0};
 unsigned found=0;
 for(size_t i=0;status==LT48_OK&&i<lt48_pic_source_resources_count_internal(r->code);i++){
  lt48_pic_source_resource_entry e;
  status=lt48_pic_source_resources_entry_internal(r->code,i,&e);
  if(status==LT48_OK&&e.role==23){erase=e;
  found++;
  }
 }
 if(status==LT48_OK&&(found!=1||erase.source.wave_type!=6||erase.source.load_address!=0x5800))status=LT48_SAFETY_REJECTED;
 unsigned char data[528];
 size_t written=0;
 if(status==LT48_OK)status=lt48_pic24_ka_erase_applet(erase.source.data,erase.source.data_size,erase.source.load_address,0x4050,data,sizeof(data),&written);
 if(status==LT48_OK){
  lt48_pic_source_ram_overlay overlay={0};
  overlay.role=23;
  overlay.load_address=0x5800;
  overlay.slot_end=0x6000;
  overlay.original_size=erase.source.data_size;
  lt48_sha256(erase.source.data,erase.source.data_size,overlay.original_sha256);
  overlay.data=data;
  overlay.size=written;
  lt48_sha256(data,written,overlay.sha256);
  status=lt48_pic_source_resources_overlay_library_internal(path,ka_source,p->family,p->pinmap,&overlay,1,&r->eeprom);
 }
 if(status!=LT48_OK){lt48_pic24_ka_resources_close(r);
 return status;
 }*output=r;
 return LT48_OK;
}
const lt48_pic24_ka_image_policy *lt48_pic24_ka_resources_geometry(const lt48_pic24_ka_resources *r){return r?&r->geometry:NULL;
}
const lt48_pic_source_resources *lt48_pic24_ka_resources_source(const lt48_pic24_ka_resources *r,int eeprom){return r&&(eeprom==0||eeprom==1)?(eeprom?r->eeprom:r->code):NULL;
}
lt48_status lt48_pic24_ka_resources_configuration_edit(const lt48_pic24_ka_resources *r,unsigned char *image,size_t size,
 const lt48_runtime_configuration_value *edits,size_t count){
 if(!r||!image||size!=r->geometry.image_span||(count&&!edits))return LT48_INVALID_ARGUMENT;
 return lt48_pic24_ka_configuration_edit(&r->configuration,image,size,edits,count)?LT48_OK:LT48_SAFETY_REJECTED;
}
