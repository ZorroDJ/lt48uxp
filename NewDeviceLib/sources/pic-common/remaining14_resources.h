#ifndef LT48_PIC_REMAINING14_RESOURCES_H
#define LT48_PIC_REMAINING14_RESOURCES_H
#include "source_resources.h"
#include "../../drafts/pic-population/remaining14_profiles.h"
#include <stdlib.h>
#include <string.h>
/* Private active-cohort bindings. No transport, programmer access, operation
 * dispatch or public admission exists in this constructor. The verified source
 * backing extent must survive even when the exported traversal is code-only. */
typedef struct {
 const rm14_profile *profile;
 lt48_pic_source_resources *resources;
 lt48_pic_source_session *session;
} rm14_resources;
static inline void rm14_close(rm14_resources *r) {
 if(!r)return;
 lt48_pic_source_session_close_internal(r->session);
 lt48_pic_source_resources_close_internal(r->resources);free(r);
}
static inline int rm14_matches(const lt48_device_spec *s,const rm14_profile *p) {
 return s&&p&&s->catalogue.device_class==2015&&s->definition_stem&&!strcmp(s->definition_stem,"GADC___")&&
  s->catalogue.vendor&&!strcmp(s->catalogue.vendor,"Microchip")&&s->element_bits==14&&
  s->capacity_bytes==p->source_bytes&&s->definition_resource==p->definition&&s->family_code==p->family&&s->pinmap==p->pinmap&&
  s->catalogue.device_name&&s->catalogue.device_name_length==strlen(p->name)&&!memcmp(s->catalogue.device_name,p->name,strlen(p->name));
}
static inline lt48_status rm14_open(const char *path,const lt48_device_spec *spec,const rm14_profile *p,
 const unsigned char *image,size_t span,rm14_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!path||!rm14_matches(spec,p)||!image||span!=p->backing_bytes||p->source_bytes>span||
  !p->code_words||p->code_words>8192||span>17024||span<16400||p->variable_count>32||p->operation_count>24)return LT48_SAFETY_REJECTED;
 const unsigned char hash[32]={0x24,0xb8,0x32,0x0d,0xa4,0xcf,0xd7,0x00,0xee,0x43,0x6f,0xf0,0x6b,0xd5,0x5d,0x9d,
  0xfb,0xd4,0x72,0xc5,0x3a,0x02,0x1a,0xd5,0x27,0xc9,0xbb,0xcb,0x3c,0x80,0x4d,0xec};
 rm14_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->profile=p;
 lt48_status st=lt48_pic_source_resources_library_internal(path,hash,(uint16_t)p->family,(uint16_t)p->pinmap,&r->resources);
 const lt48_pin_map *pins=NULL;const lt48_compile_variable *vars=NULL;size_t count=0;
 if(st==LT48_OK)st=lt48_pic_source_resources_bindings_internal(r->resources,&pins,&vars,&count);
 if(st==LT48_OK&&(count!=p->variable_count||lt48_pic_source_resources_count_internal(r->resources)!=p->operation_count))st=LT48_SAFETY_REJECTED;
 for(size_t i=0;st==LT48_OK&&i<count;i++) {
  const lt48_compile_variable *a=vars+i,*b=p->variables+i;
  if(a->kind!=b->kind||a->variable_id!=b->variable_id||a->enabled!=b->enabled||a->value!=b->value||a->channel!=b->channel)st=LT48_SAFETY_REJECTED;
 }
 unsigned has_erase=0;
 for(unsigned i=0;st==LT48_OK&&i<p->operation_count;i++) {
  lt48_pic_source_resource_entry entry={0};st=lt48_pic_source_resources_entry_internal(r->resources,i,&entry);
  if(st==LT48_OK&&(entry.role!=p->roles[i]||entry.wave!=p->waves[i]||entry.source.wave_type||!entry.compiled))st=LT48_SAFETY_REJECTED;
  if(entry.role==9)has_erase=1;
 }
 if(st==LT48_OK&&has_erase!=p->has_erase)st=LT48_SAFETY_REJECTED;
 unsigned char seed[32]={0};for(unsigned i=0;i<4;i++)seed[i]=(unsigned char)(p->code_words>>(8*i));
 lt48_runtime_buffer_seed seeds={22,seed,sizeof seed};
 lt48_runtime_options options={0};options.device_image=image;options.device_image_size=span;options.device_element_bytes=2;
 options.initial_buffers=&seeds;options.initial_buffer_count=1;options.max_steps=100000;
 if(st==LT48_OK)st=lt48_pic_source_resources_create_session_internal(r->resources,&options,&r->session);
 if(st!=LT48_OK){rm14_close(r);return st;}
 *out=r;return LT48_OK;
}
static inline lt48_status rm14_prepare_configuration(rm14_resources *r) {
 if(!r)return LT48_INVALID_ARGUMENT;
 return lt48_pic_source_session_configuration_internal(r->session,11,12,8,0,NULL,0,NULL,0);
}
#endif
