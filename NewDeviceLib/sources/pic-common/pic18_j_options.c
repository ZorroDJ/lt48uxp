#include "pic18_j_options.h"
#include "pic18_j_options_generated.h"
#include <string.h>
const lt48_pic18_j_options_profile *lt48_pic18_j_options_find(const char *key,uint16_t d,uint16_t f,uint16_t p) {
 if(!key)return NULL;
 for(size_t i=0;i<sizeof(j_option_profiles)/sizeof(*j_option_profiles);i++) {
  const lt48_pic18_j_options_profile *r=j_option_profiles+i;
  if(r->definition==d&&r->family==f&&r->pinmap==p&&!strcmp(r->key,key))return r;
 }
 return NULL;
}
static int selected(const lt48_pic18_j_option *f,uint32_t value) {
 for(size_t i=0;i<f->choice_count;i++)if(f->choices[i].value==value)return 1;
 return 0;
}
static int valid(const lt48_pic18_j_options_profile *p,const void *image,size_t size) {
 if(!p||!image)return 0;
 int canonical=0;
 for(size_t i=0;i<sizeof(j_option_profiles)/sizeof(*j_option_profiles);i++)if(p==j_option_profiles+i)canonical=1;
 return canonical&&size==p->program.code_bytes&&size>=8&&p->fields&&p->field_count&&p->field_count<=64;
}
lt48_status lt48_pic18_j_options_decode(const lt48_pic18_j_options_profile *p,
 const uint8_t *image,size_t size,lt48_pic18_j_option_value *out,size_t capacity,size_t *count) {
 if(!count)return LT48_INVALID_ARGUMENT;
 *count=0;if(!valid(p,image,size)||(capacity&&!out))return LT48_INVALID_ARGUMENT;
 *count=p->field_count;if(capacity<p->field_count)return LT48_BUFFER_TOO_SMALL;
 lt48_pic18_j_option_value staged[64];
 for(size_t i=0;i<p->field_count;i++) {
  const lt48_pic18_j_option *f=p->fields+i;unsigned v=(image[size-8+f->byte]&f->mask)>>f->shift;
  if(!selected(f,v))return LT48_UNSUPPORTED;
  staged[i]=(lt48_pic18_j_option_value){f->id,v};
 }
 memcpy(out,staged,p->field_count*sizeof(*out));return LT48_OK;
}
lt48_status lt48_pic18_j_options_edit(const lt48_pic18_j_options_profile *p,
 uint8_t *image,size_t size,const lt48_pic18_j_option_value *edits,size_t count) {
 if(!valid(p,image,size)||(count&&!edits)||count>p->field_count)return LT48_INVALID_ARGUMENT;
 uint8_t staged[8];memcpy(staged,image+size-8,8);
 for(size_t i=0;i<count;i++) {
  for(size_t j=0;j<i;j++)if(edits[j].field==edits[i].field)return LT48_INVALID_ARGUMENT;
  const lt48_pic18_j_option *f=NULL;
  for(size_t j=0;j<p->field_count;j++)if(p->fields[j].id==edits[i].field)f=p->fields+j;
  if(!f)return LT48_INVALID_ARGUMENT;
  if(!selected(f,edits[i].value))return LT48_SAFETY_REJECTED;
  staged[f->byte]=(uint8_t)((staged[f->byte]&~f->mask)|((edits[i].value<<f->shift)&f->mask));
 }
 for(unsigned i=0;i<8;i++)staged[i]=(uint8_t)((staged[i]|p->program.fixed_one[i])&~p->program.fixed_zero[i]);
 memcpy(image+size-8,staged,8);return LT48_OK;
}
lt48_status lt48_pic18_j_options_defaults(const lt48_pic18_j_options_profile *p,uint8_t *image,size_t size) {
 if(!valid(p,image,size))return LT48_INVALID_ARGUMENT;
 memcpy(image+size-8,p->defaults,8);return LT48_OK;
}
