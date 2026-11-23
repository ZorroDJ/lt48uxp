#include "dspic30_resources.h"
#include <stdlib.h>
typedef struct {const char *key;uint32_t words,dense;unsigned classic,motor,pins;uint16_t family,pinmap;} dspic30_profile;
#include "dspic30_profiles_generated.inc"
struct lt48_dspic30_resources {const dspic30_profile *profile;lt48_dspic30_image_policy geometry;lt48_dspic30_configuration_rules configuration;lt48_pic_source_resources *code;};
static const unsigned char source_sha[32]={0xc0,0xe9,0x0a,0x55,0xe9,0x7a,0x3a,0xc1,0x2d,0x73,0x00,0x6e,0xb9,0xda,0x11,0x39,0xe3,0xd5,0x68,0x8a,0xef,0x85,0x24,0x2b,0x00,0x01,0xbc,0x40,0x6c,0xab,0x46,0x40};
void lt48_dspic30_resources_close(lt48_dspic30_resources *r){if(r){lt48_pic_source_resources_close_internal(r->code);free(r);}}
lt48_status lt48_dspic30_resources_open(const char *path,const char *key,lt48_dspic30_resources **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;if(!path||!key)return LT48_INVALID_ARGUMENT;
 const dspic30_profile *p=NULL;for(size_t i=0;i<sizeof(dspic30_profiles)/sizeof(*dspic30_profiles);i++)if(!strcmp(key,dspic30_profiles[i].key)){p=dspic30_profiles+i;break;}
 if(!p)return LT48_UNSUPPORTED;
 lt48_dspic30_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;r->profile=p;
 if(!lt48_dspic30_image_policy_prepare(p->words,p->dense,&r->geometry)||!lt48_dspic30_configuration_rules_prepare(p->classic,p->motor,&r->configuration)){free(r);return LT48_SAFETY_REJECTED;}
 lt48_status s=lt48_pic_source_resources_library_internal(path,source_sha,p->family,p->pinmap,&r->code);
 if(s!=LT48_OK){lt48_dspic30_resources_close(r);return s;}*out=r;return LT48_OK;
}
const lt48_dspic30_image_policy *lt48_dspic30_resources_geometry(const lt48_dspic30_resources *r){return r?&r->geometry:NULL;}
const lt48_dspic30_configuration_rules *lt48_dspic30_resources_rules(const lt48_dspic30_resources *r){return r?&r->configuration:NULL;}
const lt48_pic_source_resources *lt48_dspic30_resources_source(const lt48_dspic30_resources *r,int eeprom){return r&&eeprom==0?r->code:NULL;}
lt48_status lt48_dspic30_resources_configuration_edit(const lt48_dspic30_resources *r,unsigned char *image,size_t size,const lt48_runtime_configuration_value *v,size_t n){
 if(!r)return LT48_INVALID_ARGUMENT;
 return lt48_dspic30_configuration_edit(&r->configuration,image,size,v,n)?LT48_OK:LT48_SAFETY_REJECTED;
}
