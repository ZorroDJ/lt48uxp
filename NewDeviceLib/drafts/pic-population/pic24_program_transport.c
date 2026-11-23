#include "pic24_program_transport.h"
#include <stdlib.h>
#include <string.h>
typedef struct {
 const unsigned char *image;size_t size;
 lt48_runtime_configuration_value fields[4];unsigned count;
 lt48_pic_source_program_erase_choice choose;void *context;
} program_context;
static lt48_status preserve(void *opaque,lt48_pic_source_session *session){
 program_context *p=opaque;lt48_pic_source_state state;
 lt48_status status=lt48_pic_source_session_state_internal(session,&state);
 if(status!=LT48_OK)return status;
 if(state.image_size!=p->size || memcmp(state.image,p->image,p->size))return LT48_SAFETY_REJECTED;
 state.configuration=p->fields;state.configuration_count=p->count;
 return lt48_pic_source_session_restore_internal(session,&state);
}
static int choose(void *opaque){program_context *p=opaque;return p->choose?p->choose(p->context):0;}
lt48_status lt48_pic24_program_transport(const lt48_pic_applet_plan_profile *profile,
 const lt48_pic_source_resources *source,lt48_pic_source_transport *transport,
 const lt48_pic24_program_view *view,uint32_t flags,const lt48_pic_source_startup_plan *startup,
 lt48_pic_source_program_erase_choice erase_choice,void *context,
 unsigned char *image,size_t size,lt48_pic_source_operation_result *result){
 if(!result)return LT48_INVALID_ARGUMENT;
 *result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
 if(!profile || !view || !view->geometry || !image || size!=profile->element_count || size!=view->image_size)return LT48_INVALID_ARGUMENT;
 if((flags&~UINT32_C(0xc103f)) || (flags&40)!=40){result->operation_status=LT48_UNSUPPORTED;return LT48_UNSUPPORTED;}
 lt48_pic24_configuration_rules rules;
 if(!lt48_pic24_configuration_rules_prepare(view->mandatory_rule_family,&rules) || rules.words!=view->configuration_words)return LT48_INVALID_ARGUMENT;
 uint32_t first=view->geometry->configuration_first_image_byte;
 if((uint64_t)first+4*rules.words!=size)return LT48_SAFETY_REJECTED;
 uint32_t input[4];uint16_t normalized[4];
 for(unsigned i=0;i<rules.words;i++)input[i]=image[first+4*i]|((uint32_t)image[first+4*i+1]<<8);
 if(!lt48_pic24_configuration_normalize(&rules,input,rules.words,normalized))return LT48_INVALID_ARGUMENT;
 unsigned char *staged=malloc(size);if(!staged){result->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 memcpy(staged,image,size);program_context policy={staged,size,{{0}},rules.words,erase_choice,context};
 for(unsigned i=0;i<rules.words;i++){
  staged[first+4*i]=(unsigned char)normalized[i];staged[first+4*i+1]=(unsigned char)(normalized[i]>>8);
  policy.fields[i]=(lt48_runtime_configuration_value){-1,2,(uint16_t)i,normalized[i]};
 }
 lt48_pic_source_applet_program_plan plan;
 lt48_status status=lt48_pic_applet_program_make(profile,flags&~UINT32_C(0x80003),0,(uint32_t)size,NULL,0,preserve,choose,&policy,&plan);
 if(status==LT48_OK)status=lt48_pic24_prepare_startup(profile,source,transport,view,flags&UINT32_C(0x80003),startup);
 if(status==LT48_OK){
  status=lt48_pic_source_transport_applet_program_internal(transport,&plan,staged,size,result);
  if(status==LT48_OK)memcpy(image,staged,size);
 }else result->operation_status=status;
 free(staged);return status;
}
