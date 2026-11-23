/* Shared source-bound PIC applet plans. Profiles do not grant device support.
 * Preservation and startup policy remain explicit owner obligations. */
#ifndef LT48_PIC_APPLET_PROFILE_H
#define LT48_PIC_APPLET_PROFILE_H
#include "source_resources.h"
#include "addtab_maps_generated.h"
#include <string.h>
typedef struct {const char *key,*profile,*source_sha256;uint32_t definition,family,pinmap;
 uint8_t size_buffer,image_buffer,increment_buffer;uint16_t voltage_variable;
 uint16_t init,reset,read,program,verify,blank,erase,protect,readcfg,preconfig,postconfig,fpga;
 uint32_t element_count,erase_step;uint16_t element_width;uint8_t data_in_buffer;
 uint16_t configuration_id;uint16_t verify_variables[4];
} lt48_pic_applet_plan_profile;
static inline lt48_status lt48_pic_applet_plan_make(const lt48_pic_applet_plan_profile *p,
 uint16_t mode,uint32_t first,uint32_t count,lt48_pic_source_applet_stream_plan *out) {
 if(!p||!out||!count)return LT48_INVALID_ARGUMENT;
 uint16_t role=mode==1?p->read:mode==2?p->program:mode==3?p->blank:mode==4?p->verify:65535;
 if(role==65535||p->init==65535||p->reset==65535)return LT48_UNSUPPORTED;
 const lt48_pic_addtab_map *map=lt48_pic_addtab_find(p->definition);
 if(!map||first>map->dense_capacity||count>map->dense_capacity-first)return LT48_INVALID_ARGUMENT;
 *out=(lt48_pic_source_applet_stream_plan){mode,p->init,p->reset,p->size_buffer,
 p->image_buffer,p->increment_buffer,LT48_PIC_STREAM_BYTES_48,mode==2||mode==4,map,first,count};
 return LT48_OK;
}
static inline lt48_status lt48_pic_applet_erase_make(const lt48_pic_applet_plan_profile *p,
 lt48_pic_source_applet_transaction *transaction,lt48_pic_source_applet_action_range *range,uint8_t *data_in) {
 if(!p||!transaction||!range||!data_in||!p->element_count)return LT48_INVALID_ARGUMENT;
 if(p->erase==65535||p->init==65535||p->reset==65535)return LT48_UNSUPPORTED;
 lt48_pic_source_applet_action_range r={0,p->element_count-1,p->erase_step,p->element_width};
 if(!lt48_pic_source_applet_action_valid(&r))return LT48_INVALID_ARGUMENT;
 *transaction=(lt48_pic_source_applet_transaction){7,p->init,p->erase,p->reset,p->size_buffer,p->element_count,0,10};
 *range=r;*data_in=p->data_in_buffer;return LT48_OK;
}
static inline lt48_status lt48_pic_applet_plan_validate(const lt48_pic_applet_plan_profile *,
 const lt48_pic_source_resources *,lt48_pic_source_transport *);
static inline lt48_status lt48_pic_applet_program_make(const lt48_pic_applet_plan_profile *p,uint32_t flags,uint32_t first,uint32_t count,
 const uint32_t *levels,size_t level_count,lt48_pic_source_program_preserve preserve,
 lt48_pic_source_program_erase_choice choice,void *context,lt48_pic_source_applet_program_plan *out) {
 if(!p||!out||!preserve)return LT48_INVALID_ARGUMENT;
 if(flags&~UINT32_C(0x4103c))return LT48_UNSUPPORTED;
 if(((flags&4)&&p->blank==65535)||((flags&16)&&p->verify==65535)||((flags&32)&&p->protect==65535))return LT48_UNSUPPORTED;
 if((flags&16)&&((!levels&&level_count)||(levels&&!level_count)||level_count>4))return LT48_INVALID_ARGUMENT;
 lt48_pic_source_applet_program_plan plan={0};
 lt48_status st=lt48_pic_applet_plan_make(p,2,first,count,&plan.stream);
 if(st!=LT48_OK)return st;
 if(!p->element_count||p->erase==65535||!lt48_pic_source_applet_configuration_whole(p->configuration_id))return LT48_UNSUPPORTED;
 plan.flags=flags;plan.capacity=p->element_count;plan.erase_role=p->erase;plan.protect_role=p->protect;
 plan.configuration_id=p->configuration_id;plan.data_in=p->data_in_buffer;
 plan.erase_range=(lt48_pic_source_applet_action_range){0,p->element_count-1,p->erase_step,p->element_width};
 if(!lt48_pic_source_applet_action_valid(&plan.erase_range))return LT48_INVALID_ARGUMENT;
 plan.verify_voltages=levels;plan.verify_voltage_count=level_count;plan.preserve=preserve;
 if((flags&16)&&!levels){plan.auto_verify_voltages=1;memcpy(plan.verify_variables,p->verify_variables,sizeof(plan.verify_variables));}
 plan.erase_choice=choice;plan.policy_context=context;*out=plan;return LT48_OK;
}
static inline lt48_status lt48_pic_applet_program_run(const lt48_pic_applet_plan_profile *p,
 const lt48_pic_source_resources *resources,lt48_pic_source_transport *transport,uint32_t flags,uint32_t first,uint32_t count,
 const uint32_t *levels,size_t level_count,lt48_pic_source_program_preserve preserve,
 lt48_pic_source_program_erase_choice choice,void *context,unsigned char *image,size_t image_size,lt48_pic_source_operation_result *result) {
 lt48_pic_source_applet_program_plan plan;lt48_status st=lt48_pic_applet_plan_validate(p,resources,transport);
 if(st==LT48_OK)st=lt48_pic_applet_program_make(p,flags,first,count,levels,level_count,preserve,choice,context,&plan);
 if(st!=LT48_OK){lt48_pic_source_transport_applet_program_internal(transport,NULL,image,image_size,result);
  if(result){result->operation_status=st;}return st;}
 return lt48_pic_source_transport_applet_program_internal(transport,&plan,image,image_size,result);
}
static inline lt48_status lt48_pic_applet_configuration_make(const lt48_pic_applet_plan_profile *p,uint16_t mode,
 lt48_pic_source_applet_transaction *transaction,uint16_t *configuration_id,uint8_t *data_in) {
 if(!p||!transaction||!configuration_id||!data_in||!p->element_count||(mode!=12&&mode!=13))return LT48_INVALID_ARGUMENT;
 uint16_t role=mode==12?p->readcfg:p->protect;
 if(!lt48_pic_source_applet_configuration_whole(p->configuration_id)||role==65535||p->init==65535||p->reset==65535)return LT48_UNSUPPORTED;
 *transaction=(lt48_pic_source_applet_transaction){mode,p->init,role,p->reset,p->size_buffer,p->element_count,0,10};
 *configuration_id=p->configuration_id;*data_in=p->data_in_buffer;return LT48_OK;
}
static inline lt48_status lt48_pic_applet_configuration_run(const lt48_pic_applet_plan_profile *p,
 const lt48_pic_source_resources *resources,lt48_pic_source_transport *transport,uint16_t mode,lt48_pic_source_operation_result *result) {
 lt48_pic_source_applet_transaction transaction;uint16_t configuration_id;uint8_t data_in;
 lt48_status st=lt48_pic_applet_plan_validate(p,resources,transport);
 if(st==LT48_OK)st=lt48_pic_applet_configuration_make(p,mode,&transaction,&configuration_id,&data_in);
 if(st!=LT48_OK){lt48_pic_source_transport_applet_configuration_internal(transport,NULL,0,0,result);
  if(result){result->operation_status=st;}return st;}
 return lt48_pic_source_transport_applet_configuration_internal(transport,&transaction,configuration_id,data_in,result);
}
static inline lt48_status lt48_pic_applet_erase_run(const lt48_pic_applet_plan_profile *p,
 const lt48_pic_source_resources *resources,lt48_pic_source_transport *transport,lt48_pic_source_operation_result *result) {
 lt48_pic_source_applet_transaction transaction;lt48_pic_source_applet_action_range range;uint8_t data_in;
 lt48_status st=lt48_pic_applet_plan_validate(p,resources,transport);
 if(st==LT48_OK)st=lt48_pic_applet_erase_make(p,&transaction,&range,&data_in);
 if(st!=LT48_OK){lt48_pic_source_transport_applet_action_internal(transport,NULL,NULL,0,result);
  if(result){result->operation_status=st;}return st;}
 return lt48_pic_source_transport_applet_action_internal(transport,&transaction,&range,data_in,result);
}
static inline lt48_status lt48_pic_applet_plan_validate(const lt48_pic_applet_plan_profile *p,
 const lt48_pic_source_resources *resources,lt48_pic_source_transport *transport) {
 if(!p||!resources||!transport||p->family>65535||p->pinmap>65535||!p->source_sha256||strlen(p->source_sha256)!=64)return LT48_INVALID_ARGUMENT;
 if(!lt48_pic_source_transport_matches_origin_internal(transport,resources))return LT48_SAFETY_REJECTED;
 unsigned char digest[32];
 for(unsigned i=0;i<32;i++){unsigned value=0;
  for(unsigned j=0;j<2;j++){unsigned c=(unsigned char)p->source_sha256[2*i+j];
   if(c>='0'&&c<='9')c-='0';else if(c>='a'&&c<='f')c=c-'a'+10;else return LT48_FORMAT_ERROR;
   value=(value<<4)|c;}digest[i]=(unsigned char)value;}
 if(!lt48_pic_source_resources_matches_binding_internal(resources,digest,(uint16_t)p->family,(uint16_t)p->pinmap))
  return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
static inline lt48_status lt48_pic_applet_plan_prepare(const lt48_pic_applet_plan_profile *p,
 const lt48_pic_source_resources *resources,lt48_pic_source_transport *transport) {
 lt48_status st=lt48_pic_applet_plan_validate(p,resources,transport);
 return st==LT48_OK?lt48_pic_source_resources_prepare_internal(resources,p->fpga,transport):st;
}
#endif
