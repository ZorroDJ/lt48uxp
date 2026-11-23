#include "pic24_backend.h"
#include "pic24_configuration_write_policy.h"
#include <stdlib.h>
#include <string.h>
struct lt48_pic24_backend {
 const lt48_pic24_operator_profile *profile;lt48_pic_source_resources *source;
 uint32_t voltage,low,high;
};
void lt48_pic24_backend_close(lt48_pic24_backend *r){
 if(r){lt48_pic_source_resources_close_internal(r->source);free(r);}
}
size_t lt48_pic24_backend_capacity(const lt48_pic24_backend *r){return r?r->profile->image_bytes:0;}
static int voltage(const lt48_compile_variable *v,size_t n,unsigned id,uint32_t *out){
 unsigned found=0;for(size_t i=0;i<n;i++)if(v[i].variable_id==id){
  if(found++ || v[i].kind!=0)return 0;
 *out=v[i].value;
 }return found==1;
}
lt48_status lt48_pic24_backend_open(const char *library,const char *key,lt48_pic24_backend **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!library || !key)return LT48_INVALID_ARGUMENT;
 const lt48_pic24_operator_profile *profile=lt48_pic24_operator_profile_find(key);
 if(!profile)return LT48_UNSUPPORTED;
 lt48_pic24_backend *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;r->profile=profile;
 lt48_status status=lt48_pic_source_resources_library_internal(library,profile->source_sha256,profile->family,profile->pinmap,&r->source);
 const lt48_pin_map *pins=NULL;const lt48_compile_variable *vars=NULL;size_t n=0;
 if(status==LT48_OK)status=lt48_pic_source_resources_bindings_internal(r->source,&pins,&vars,&n);
 if(status==LT48_OK && (!voltage(vars,n,0,&r->voltage) || !voltage(vars,n,1,&r->low) ||
  !voltage(vars,n,2,&r->high) || !r->low || r->low>r->voltage || r->voltage>r->high || r->high>360))status=LT48_SAFETY_REJECTED;
 if(status!=LT48_OK){lt48_pic24_backend_close(r);return status;}
 *out=r;return LT48_OK;
}
lt48_status lt48_pic24_backend_configuration_values(const lt48_pic24_backend *r,
 const unsigned char *image,size_t size,lt48_runtime_configuration_value *values,size_t capacity,size_t *count){
 if(!r)return LT48_INVALID_ARGUMENT;
 return lt48_pic24_configuration_values(r->profile,r->source,image,size,values,capacity,count);
}
lt48_status lt48_pic24_backend_configuration_edit(const lt48_pic24_backend *r,
 unsigned char *image,size_t size,const lt48_runtime_configuration_value *edits,size_t count){
 if(!r)return LT48_INVALID_ARGUMENT;
 return lt48_pic24_configuration_edit(r->profile,r->source,image,size,edits,count);
}
static lt48_status configuration_finish(const lt48_protocol_transport *api,void *context,lt48_status outcome){
 lt48_protocol_transport uncancelled=*api;uncancelled.cancelled=NULL;
 lt48_runtime_transport_session *wire=NULL;
 lt48_status status=lt48_runtime_transport_session_create(&uncancelled,context,&wire);
 unsigned char command[]={0x40,outcome==LT48_OK?4:2};
 if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,command,sizeof(command));
 if(status==LT48_OK)status=lt48_runtime_transport_session_flush(wire);
 lt48_runtime_transport_session_close(wire);return status;
}
static lt48_status configuration_write(const lt48_pic24_backend *r,
 const lt48_protocol_transport *api,void *context,lt48_pic_source_session *session,
 lt48_pic_source_transport *transport,lt48_pic24_operator_binding *binding,
 unsigned char *stage,size_t size,uint32_t flags,int secure,lt48_pic24_backend_result *out){
 unsigned count=r->profile->configuration_words;uint32_t first=binding->geometry.configuration_first_image_byte;
 if((uint64_t)first+4*count!=size)return LT48_INVALID_ARGUMENT;
 uint32_t requested[4];uint16_t observed[4],normalized[4];
 for(unsigned i=0;i<count;i++)requested[i]=stage[first+4*i]|((uint32_t)stage[first+4*i+1]<<8);
 /* Capture actual target configuration before issuing any NVM write. The
  * source configuration callback cannot modify the program-code array. */
 lt48_status status=lt48_pic24_configuration_read_transport(binding->plan,r->source,transport,&binding->view,
  flags,&binding->startup,observed,count,&out->configuration_before);
 if(status==LT48_OK){
  int accepted=secure?lt48_pic24_configuration_secure_prepare(r->profile->rule_family,observed,count,normalized):
   lt48_pic24_configuration_write_prepare(r->profile->rule_family,observed,requested,count,normalized);
  if(!accepted)status=LT48_SAFETY_REJECTED;
 }
 if(status==LT48_OK){
  lt48_pic_source_state state;lt48_runtime_configuration_value fields[4];
  status=lt48_pic_source_session_state_internal(session,&state);
  if(status==LT48_OK){
   for(unsigned i=0;i<count;i++){
    fields[i]=(lt48_runtime_configuration_value){-1,2,(uint16_t)i,normalized[i]};
    stage[first+4*i]=(unsigned char)normalized[i];stage[first+4*i+1]=(unsigned char)(normalized[i]>>8);
   }
   /* Source configuration setup can decode its image again. Bind both the
    * image and category2 fields to the same intended values. */
   state.image=stage;state.image_size=size;
   state.configuration=fields;state.configuration_count=count;
   status=lt48_pic_source_session_restore_internal(session,&state);
  }
  if(status==LT48_OK)status=lt48_pic24_prepare_startup(binding->plan,r->source,transport,&binding->view,flags,&binding->startup);
  if(status==LT48_OK)status=lt48_pic_applet_configuration_run(binding->plan,r->source,transport,13,&out->operation);
 }
 if(status==LT48_OK && secure)out->protection_write_completed=1;
 if(status==LT48_OK && !secure){
  status=lt48_pic24_configuration_read_transport(binding->plan,r->source,transport,&binding->view,
   flags,&binding->startup,observed,count,&out->configuration_after);
  out->configuration_mismatch.word=count;
  if(status==LT48_OK)for(unsigned i=0;i<count;i++)if(normalized[i]!=observed[i]){
   out->configuration_mismatch=(lt48_pic24_configuration_mismatch){i,normalized[i],observed[i],(uint16_t)(normalized[i]^observed[i])};
   status=LT48_DEVICE_MISMATCH;break;
  }
 }
 if(status==LT48_OK && !secure)out->configuration_readback_confirmed=1;
 out->configuration_indication_status=configuration_finish(api,context,status);
 if(status==LT48_OK)status=out->configuration_indication_status;
 if(status==LT48_OK)for(unsigned i=0;i<count;i++){
  stage[first+4*i]=(unsigned char)normalized[i];stage[first+4*i+1]=(unsigned char)(normalized[i]>>8);
 }
 return status;
}
static lt48_status execute_voltage(const lt48_pic24_backend *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind op,unsigned char *image,size_t size,
 uint32_t flags,const lt48_pic_source_startup_plan *request,uint32_t voltage,lt48_pic24_backend_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->status=LT48_INVALID_ARGUMENT;
 if(!r || !api || !api->write || !api->read || !image || size!=r->profile->image_bytes || !request)return LT48_INVALID_ARGUMENT;
 if(flags&~UINT32_C(0x80003)){out->status=LT48_UNSUPPORTED;return out->status;}
 uint16_t mode=0;
 switch(op){case LT48_OPERATION_READ:mode=1;break;case LT48_OPERATION_BLANK_CHECK:mode=3;break;
 case LT48_OPERATION_VERIFY:case LT48_OPERATION_COMPARE:mode=4;break;
 case LT48_OPERATION_OVER_PROGRAM:op=LT48_OPERATION_PROGRAM;break;
 case LT48_OPERATION_PROGRAM:case LT48_OPERATION_ERASE:case LT48_OPERATION_CONFIGURATION:case LT48_OPERATION_SECURE:break;
 default:out->status=LT48_UNSUPPORTED;return out->status;}
 unsigned char *stage=malloc(size);if(!stage){out->status=LT48_NO_MEMORY;return out->status;}
 memcpy(stage,image,size);
 lt48_runtime_options options={0};options.device_image=stage;options.device_image_size=size;
 options.device_element_bytes=1;options.use_explicit_buffer_ids=1;options.address_buffer=0;
 options.mode_buffer=8;options.parameter_buffer_id=3;options.root_timeout_seconds=10;
 lt48_pic_source_session *session=NULL;lt48_pic_source_transport *transport=NULL;
 lt48_status status=lt48_pic_source_resources_create_session_internal(r->source,&options,&session);
 if(status==LT48_OK)status=lt48_pic_source_session_voltage_value_internal(session,0,voltage);
 lt48_pic_source_host_binding host={0,0,1};
 if(status==LT48_OK)status=lt48_pic_source_transport_create_internal(api,context,session,&host,voltage,r->low,r->high,&transport);
 lt48_pic24_operator_binding binding;
 if(status==LT48_OK)status=lt48_pic24_operator_binding_make(r->profile,r->source,transport,stage,size,request,&binding);
 if(status==LT48_OK){
  if(op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE){
   status=configuration_write(r,api,context,session,transport,&binding,stage,size,flags,op==LT48_OPERATION_SECURE,out);
  }else if(op==LT48_OPERATION_PROGRAM){
   uint32_t cw1=binding.geometry.configuration_first_image_byte+4*(r->profile->configuration_words-1);
   int protect_after_verify=!(stage[cw1+1]&0x20);
   stage[cw1+1]|=0x20; /* Keep code/config readable until all Verify passes finish. */
   lt48_pic24_verified_program_plan plan={binding.plan,r->source,transport,transport,&binding.view,&binding.startup,
    api,context,NULL,NULL,flags|8|16|32};
   status=lt48_pic24_verified_program(&plan,stage,size,&out->program);out->operation=out->program.program;
   if(status==LT48_OK && protect_after_verify)
    status=configuration_write(r,api,context,session,transport,&binding,stage,size,flags,1,out);
  }else if(op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE){
   lt48_pic24_verified_compare_plan plan={binding.plan,r->source,transport,transport,&binding.view,&binding.startup,api,context,flags};
   status=lt48_pic24_verified_compare(&plan,stage,size,&out->comparison);out->operation=out->comparison.code;
  }else if(op==LT48_OPERATION_ERASE)status=lt48_pic24_erase_transport(binding.plan,r->source,transport,&binding.view,flags,&binding.startup,&out->operation);
  else status=lt48_pic24_memory_transport(binding.plan,r->source,transport,&binding.view,flags,&binding.startup,mode,stage,size,&out->operation);
 }
 if(status==LT48_OK && (op==LT48_OPERATION_READ || op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE))memcpy(image,stage,size);
 lt48_pic_source_transport_close_internal(transport);lt48_pic_source_session_close_internal(session);free(stage);
 out->status=status;return status;
}

lt48_status lt48_pic24_backend_verify_levels(const lt48_pic24_backend *r,
 lt48_verify_margin_selection margin,unsigned type,uint32_t levels[2],unsigned *count){
 if(!r || !levels || !count || type>1 || margin>LT48_VERIFY_MARGIN_ALL || margin<LT48_VERIFY_MARGIN_NOMINAL)return LT48_INVALID_ARGUMENT;
 uint32_t selected[2]={r->voltage,0};unsigned n=1;
 if(margin==LT48_VERIFY_MARGIN_LOW)selected[0]=r->low;
 else if(margin==LT48_VERIFY_MARGIN_HIGH)selected[0]=r->high;
 else if(margin==LT48_VERIFY_MARGIN_ALL){selected[0]=r->low;selected[1]=r->high;n=2;}
 memcpy(levels,selected,sizeof(selected));*count=n;return LT48_OK;
}
lt48_status lt48_pic24_backend_execute_with_margin(const lt48_pic24_backend *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind op,unsigned char *image,size_t size,
 uint32_t flags,const lt48_pic_source_startup_plan *request,lt48_verify_margin_selection margin,unsigned type,
 lt48_pic24_backend_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 uint32_t levels[2];unsigned count=0;lt48_status status=lt48_pic24_backend_verify_levels(r,margin,type,levels,&count);
 if(status!=LT48_OK){memset(out,0,sizeof(*out));out->status=status;return status;}
 if(op!=LT48_OPERATION_VERIFY)return execute_voltage(r,api,context,op,image,size,flags,request,r->voltage,out);
 for(unsigned i=0;i<count;i++){
  status=execute_voltage(r,api,context,op,image,size,flags,request,levels[i],out);
  if(status!=LT48_OK)break;
 }
 return status;
}
lt48_status lt48_pic24_backend_execute(const lt48_pic24_backend *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind op,unsigned char *image,size_t size,
 uint32_t flags,const lt48_pic_source_startup_plan *request,lt48_pic24_backend_result *out){
 return lt48_pic24_backend_execute_with_margin(r,api,context,op,image,size,flags,request,LT48_VERIFY_MARGIN_NOMINAL,0,out);
}

lt48_status lt48_pic24_backend_expected_identity(const lt48_pic24_backend *r,uint16_t *manufacturer,uint16_t *device){
 if(!r || !manufacturer || !device)return LT48_INVALID_ARGUMENT;
 for(size_t i=0;i<sizeof(pic_applet_identity_profiles)/sizeof(*pic_applet_identity_profiles);i++)
  if(!strcmp(r->profile->key,pic_applet_identity_profiles[i].key)){
   *manufacturer=pic_applet_identity_profiles[i].plan.manufacturer;*device=pic_applet_identity_profiles[i].plan.device;return LT48_OK;
  }
 return LT48_UNSUPPORTED;
}
