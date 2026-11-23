#include "pic24_transport_prepare.h"
#include "applet_identity_plans_generated.h"
lt48_status lt48_pic24_prepare_startup(const lt48_pic_applet_plan_profile *p,
 const lt48_pic_source_resources *source,lt48_pic_source_transport *transport,
 const lt48_pic24_program_view *view,uint32_t checks,const lt48_pic_source_startup_plan *request){
 if(!p || !view || !view->geometry || !request)return LT48_INVALID_ARGUMENT;
 if(!p->key || !p->profile || !p->source_sha256)return LT48_INVALID_ARGUMENT;
 const lt48_pic_applet_plan_profile *canonical=NULL;
 for(size_t i=0;i<sizeof(pic_applet_plan_profiles)/sizeof(*pic_applet_plan_profiles);i++)
  if(!strcmp(p->key,pic_applet_plan_profiles[i].key)){canonical=pic_applet_plan_profiles+i;break;}
 if(!canonical || strcmp(p->profile,canonical->profile) || strcmp(p->source_sha256,canonical->source_sha256) ||
    p->definition!=canonical->definition || p->family!=canonical->family || p->pinmap!=canonical->pinmap ||
    p->size_buffer!=canonical->size_buffer || p->image_buffer!=canonical->image_buffer ||
    p->increment_buffer!=canonical->increment_buffer || p->voltage_variable!=canonical->voltage_variable ||
    p->configuration_id!=canonical->configuration_id)return LT48_SAFETY_REJECTED;
 if(p->element_count!=view->image_size || p->element_width!=1 || p->init!=2 || p->reset!=3 ||
    p->program!=8 || p->verify!=9 || p->blank!=7 || p->erase!=10 || p->protect!=11 ||
    p->preconfig!=27 || p->postconfig!=28 || p->fpga!=30)return LT48_SAFETY_REJECTED;
 const lt48_pic_applet_identity_profile *identity=NULL;
 for(size_t i=0;i<sizeof(pic_applet_identity_profiles)/sizeof(*pic_applet_identity_profiles);i++)
  if(!strcmp(p->key,pic_applet_identity_profiles[i].key)){identity=pic_applet_identity_profiles+i;break;}
 if(!identity || strcmp(p->profile,identity->profile) || strcmp(p->source_sha256,identity->source_sha256) ||
    p->family!=identity->family || p->pinmap!=identity->pinmap)return LT48_SAFETY_REJECTED;
 const lt48_pic_source_identity_session_plan *id=&request->identity,*expected=&identity->plan;
 if(id->getid_role!=expected->getid_role || id->manufacturer!=expected->manufacturer ||
    id->device!=expected->device || id->table_index!=expected->table_index ||
    id->manufacturer_buffer!=expected->manufacturer_buffer || id->device_buffer!=expected->device_buffer)
  return LT48_SAFETY_REJECTED;
 lt48_status st=lt48_pic_applet_plan_validate(p,source,transport);
 if(st!=LT48_OK)return st;
 lt48_runtime_result *requests=NULL;
 st=lt48_pic24_source_stage_requests(source,LT48_PIC_APP_PROGRAM_PROGRESS,view,&requests);
 lt48_runtime_result_close(requests);
 if(st!=LT48_OK)return st;
 return lt48_pic_applet_plan_prepare_startup(p,source,transport,checks,request);
}
