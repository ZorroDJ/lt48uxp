#include "pic24_operator_profile.h"
#include <string.h>
#include "pic24_operator_profiles_generated.inc"
const lt48_pic24_operator_profile *lt48_pic24_operator_profile_find(const char *key){
 if(key)for(size_t i=0;i<sizeof(profiles)/sizeof(*profiles);i++)if(!strcmp(key,profiles[i].key))return profiles+i;
 return NULL;
}
lt48_status lt48_pic24_operator_profile_view(const lt48_pic24_operator_profile *profile,
 const lt48_pic_source_resources *source,const unsigned char *image,size_t size,
 lt48_pic24_image_policy *geometry,lt48_pic24_program_view *view,
 const lt48_pic_applet_plan_profile **plan){
 if(!profile || !profile->key || !source || !image || !geometry || !view || !plan)return LT48_INVALID_ARGUMENT;
 if(lt48_pic24_operator_profile_find(profile->key)!=profile)return LT48_SAFETY_REJECTED;
 if(size!=profile->image_bytes)return LT48_INVALID_ARGUMENT;
 if(!lt48_pic_source_resources_matches_binding_internal(source,profile->source_sha256,profile->family,profile->pinmap))return LT48_SAFETY_REJECTED;
 const lt48_pic_applet_plan_profile *selected=NULL;
 for(size_t i=0;i<sizeof(pic_applet_plan_profiles)/sizeof(*pic_applet_plan_profiles);i++)
  if(!strcmp(profile->key,pic_applet_plan_profiles[i].key)){selected=pic_applet_plan_profiles+i;break;}
 if(!selected || selected->definition!=profile->definition || selected->family!=profile->family ||
    selected->pinmap!=profile->pinmap || selected->element_count!=size)return LT48_SAFETY_REJECTED;
 const lt48_pin_map *pins;const lt48_compile_variable *variables;size_t count;
 lt48_status status=lt48_pic_source_resources_bindings_internal(source,&pins,&variables,&count);
 if(status!=LT48_OK)return status;
 unsigned found=0;uint32_t bank=0;
 for(size_t i=0;i<count;i++)if(variables[i].variable_id==23){
  if(variables[i].kind!=2 || found++)return LT48_SAFETY_REJECTED;
  bank=variables[i].value|((uint32_t)variables[i].channel<<16);
 }
 lt48_logical_pin selector;
 if(!found || lt48_pin_map_find_logical(pins,2,&selector)!=LT48_OK)return LT48_SAFETY_REJECTED;
 lt48_pic24_image_policy staged_geometry;
 if(!lt48_pic24_image_policy_prepare(size,profile->last_target_address,profile->configuration_words,&staged_geometry))return LT48_SAFETY_REJECTED;
 lt48_pic24_program_view staged={0};staged.geometry=&staged_geometry;staged.image=image;staged.image_size=size;
 staged.configuration_words=profile->configuration_words;staged.mandatory_rule_family=profile->rule_family;
 staged.code_image_bytes=staged_geometry.configuration_first_image_byte;
 if(!lt48_pic24_program_plan_prepare(&staged_geometry,0,(uint32_t)size,selector.engine_pin,bank,&staged.program))return LT48_SAFETY_REJECTED;
 lt48_runtime_result *requests=NULL;
 status=lt48_pic24_source_stage_requests(source,LT48_PIC_APP_PROGRAM_PROGRESS,&staged,&requests);
 lt48_runtime_result_close(requests);if(status!=LT48_OK)return status;
 *geometry=staged_geometry;staged.geometry=geometry;*view=staged;*plan=selected;
 return LT48_OK;
}
