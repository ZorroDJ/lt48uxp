#include "pic24_configuration_editor.h"
#include "../../sources/pic-common/configuration.h"
#include "../../sources/pic-common/configuration_schemas_generated.h"
#include <stdlib.h>
#include <string.h>
typedef struct {
 const lt48_compiled_wave *pre,*post;const lt48_pin_map *pins;
 const lt48_compile_variable *variables;size_t variable_count;
 const lt48_pic_configuration_field *fields;size_t field_count;
 lt48_pic24_image_policy geometry;lt48_pic24_program_view view;
 const lt48_pic_applet_plan_profile *plan;
} editor_context;
static lt48_status context_make(const lt48_pic24_operator_profile *profile,
 const lt48_pic_source_resources *source,const unsigned char *image,size_t size,
 editor_context *c){
 memset(c,0,sizeof(*c));
 lt48_status status=lt48_pic24_operator_profile_view(profile,source,image,size,
  &c->geometry,&c->view,&c->plan);
 if(status!=LT48_OK)return status;
 status=lt48_pic_source_resources_bindings_internal(source,&c->pins,&c->variables,&c->variable_count);
 if(status!=LT48_OK)return status;
 for(size_t i=0;i<lt48_pic_source_resources_count_internal(source);i++){
  lt48_pic_source_resource_entry entry;status=lt48_pic_source_resources_entry_internal(source,i,&entry);
  if(status!=LT48_OK)return status;
  if(entry.role!=c->plan->preconfig && entry.role!=c->plan->postconfig)continue;
  if(entry.source.wave_type!=0 || !entry.compiled)return LT48_SAFETY_REJECTED;
  if(entry.role==c->plan->preconfig){if(c->pre)return LT48_SAFETY_REJECTED;c->pre=entry.compiled;}
  if(entry.role==c->plan->postconfig){if(c->post)return LT48_SAFETY_REJECTED;c->post=entry.compiled;}
 }
 c->fields=lt48_pic_configuration_fields(c->plan->configuration_id,&c->field_count);
 if(!c->pre || !c->post || !c->fields || c->field_count!=profile->configuration_words)return LT48_UNSUPPORTED;
 /* Audited PIC24 PreConfig exposes low16 words in source category2 only. */
 for(size_t i=0;i<c->field_count;i++)if(c->fields[i].category!=2 ||
  c->fields[i].field!=i || c->fields[i].minimum!=0 || c->fields[i].maximum!=65535)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
static int pure_result(const lt48_runtime_result *result){
 for(size_t i=0;i<lt48_runtime_event_count(result);i++){
  lt48_runtime_event e;if(lt48_runtime_get_event(result,i,&e)!=LT48_OK)return 0;
  switch(e.kind){
  case LT48_RUNTIME_EVALUATE_VARIABLE:case LT48_RUNTIME_BUFFER_ARITHMETIC:
  case LT48_RUNTIME_BUFFER_BIT_COPY:case LT48_RUNTIME_BUFFER_MEMMOVE:
  case LT48_RUNTIME_BUFFER_BYTE_OPERATION:case LT48_RUNTIME_BUFFER_FILL:break;
  case LT48_RUNTIME_RETURN_STATUS:if(!e.arguments[0])return 0;break;
  default:return 0;
  }
 }
 return 1;
}
lt48_status lt48_pic24_configuration_values(const lt48_pic24_operator_profile *profile,
 const lt48_pic_source_resources *source,const unsigned char *image,size_t size,
 lt48_runtime_configuration_value *output,size_t capacity,size_t *count){
 if(!count || (capacity && !output))return LT48_INVALID_ARGUMENT;
 *count=0;editor_context c;lt48_status status=context_make(profile,source,image,size,&c);
 if(status!=LT48_OK)return status;
 lt48_runtime_options options={0};options.device_image=image;options.device_image_size=size;
 options.device_element_bytes=1;options.max_steps=100000;
 lt48_runtime_result *result=NULL;
 status=lt48_runtime_interpret(c.pre,c.pins,c.variables,c.variable_count,&options,&result);
 if(status==LT48_OK && !pure_result(result))status=LT48_SAFETY_REJECTED;
 const lt48_runtime_configuration_value *values=NULL;size_t n=0;
 if(status==LT48_OK)status=lt48_runtime_get_configuration(result,&values,&n);
 lt48_runtime_configuration_value staged[4];
 for(size_t i=0;status==LT48_OK && i<c.field_count;i++){
  size_t found=SIZE_MAX;
  for(size_t j=0;j<n;j++)if(values[j].object_id==-1 && values[j].category==2 && values[j].field==i){
   if(found!=SIZE_MAX){status=LT48_FORMAT_ERROR;break;}found=j;
  }
  if(status==LT48_OK){if(found==SIZE_MAX || values[found].value>65535)status=LT48_FORMAT_ERROR;
   else staged[i]=values[found];}
 }
 if(status==LT48_OK){*count=c.field_count;if(capacity<c.field_count)status=LT48_BUFFER_TOO_SMALL;
  else memcpy(output,staged,c.field_count*sizeof(*output));}
 lt48_runtime_result_close(result);return status;
}
lt48_status lt48_pic24_configuration_edit(const lt48_pic24_operator_profile *profile,
 const lt48_pic_source_resources *source,unsigned char *image,size_t size,
 const lt48_runtime_configuration_value *edits,size_t count){
 if(count && !edits)return LT48_INVALID_ARGUMENT;
 editor_context c;lt48_status status=context_make(profile,source,image,size,&c);
 if(status!=LT48_OK)return status;
 unsigned char *stage=malloc(size);if(!stage)return LT48_NO_MEMORY;
 status=lt48_pic_configuration_edit_internal(c.pre,c.post,c.pins,c.variables,c.variable_count,
  1,image,size,stage,c.fields,c.field_count,edits,count);
 uint32_t first=c.geometry.configuration_first_image_byte;
 if(status==LT48_OK && memcmp(image,stage,first))status=LT48_SAFETY_REJECTED;
 uint32_t input[4];uint16_t normalized[4];lt48_pic24_configuration_rules rules;
 if(status==LT48_OK && !lt48_pic24_configuration_rules_prepare(profile->rule_family,&rules))status=LT48_SAFETY_REJECTED;
 if(status==LT48_OK){
  for(unsigned i=0;i<profile->configuration_words;i++){
   if(stage[first+4*i+2]!=image[first+4*i+2] || stage[first+4*i+3]!=image[first+4*i+3]){status=LT48_SAFETY_REJECTED;break;}
   input[i]=stage[first+4*i]|((uint32_t)stage[first+4*i+1]<<8);
  }
 }
 if(status==LT48_OK && !lt48_pic24_configuration_normalize(&rules,input,profile->configuration_words,normalized))status=LT48_SAFETY_REJECTED;
 if(status==LT48_OK){
  for(unsigned i=0;i<profile->configuration_words;i++){
   stage[first+4*i]=(unsigned char)normalized[i];stage[first+4*i+1]=(unsigned char)(normalized[i]>>8);
  }
  memcpy(image,stage,size);
 }
 free(stage);return status;
}
