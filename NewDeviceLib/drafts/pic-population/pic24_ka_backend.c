#include "pic24_ka_backend.h"
#include "applet_plans_generated.h"
#include <stdlib.h>
#include <string.h>
struct lt48_pic24_ka_backend {
 lt48_pic24_ka_resources *resources;
 const lt48_pic_applet_plan_profile *plan;
 lt48_pic24_ka_configuration_rules configuration;
};
void lt48_pic24_ka_backend_close(lt48_pic24_ka_backend *r){if(r){lt48_pic24_ka_resources_close(r->resources);
free(r);
}}
size_t lt48_pic24_ka_backend_capacity(const lt48_pic24_ka_backend *r){
 const lt48_pic24_ka_image_policy *g=r?lt48_pic24_ka_resources_geometry(r->resources):NULL;
 return g?g->image_span:0;
}
lt48_status lt48_pic24_ka_backend_open(const char *library,const char *key,lt48_pic24_ka_backend **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!library||!key)return LT48_INVALID_ARGUMENT;
 lt48_pic24_ka_backend *r=calloc(1,sizeof(*r));
 if(!r)return LT48_NO_MEMORY;
 lt48_status s=lt48_pic24_ka_resources_open(library,key,&r->resources);
 for(size_t i=0;s==LT48_OK&&i<sizeof(pic_applet_plan_profiles)/sizeof(*pic_applet_plan_profiles);i++)
  if(!strcmp(key,pic_applet_plan_profiles[i].key)){r->plan=pic_applet_plan_profiles+i;
  break;
  }
 const lt48_pic24_ka_image_policy *g=s==LT48_OK?lt48_pic24_ka_resources_geometry(r->resources):NULL;
 if(s==LT48_OK&&(!r->plan||r->plan->element_count!=g->dense_bytes||r->plan->element_width!=1||
    !lt48_pic24_ka_configuration_rules_prepare(g->code_words==11264?3:1,g->code_words,
       strstr(key,"PIC24FV")!=NULL,r->plan->pinmap==203?20:28,&r->configuration)))s=LT48_SAFETY_REJECTED;
 if(s!=LT48_OK){lt48_pic24_ka_backend_close(r);
 return s;
 }*out=r;
 return LT48_OK;
}
lt48_status lt48_pic24_ka_backend_configuration_get(const lt48_pic24_ka_backend *r,
 const unsigned char *image,size_t size,lt48_runtime_configuration_value *values,size_t capacity,size_t *count){
 if(!r||!count||(capacity&&!values))return LT48_INVALID_ARGUMENT;
 *count=0;
 uint32_t raw[8];
 if(size!=lt48_pic24_ka_backend_capacity(r)||!lt48_pic24_ka_configuration_read_image(image,size,raw))return LT48_INVALID_ARGUMENT;
 *count=8;
 if(capacity<8)return LT48_BUFFER_TOO_SMALL;
 for(unsigned i=0;i<8;i++)values[i]=(lt48_runtime_configuration_value){-1,2,(uint16_t)i,raw[i]};
 return LT48_OK;
}
lt48_status lt48_pic24_ka_backend_configuration_set(const lt48_pic24_ka_backend *r,
 unsigned char *image,size_t size,const lt48_runtime_configuration_value *values,size_t count){
 return r?lt48_pic24_ka_resources_configuration_edit(r->resources,image,size,values,count):LT48_INVALID_ARGUMENT;
}
static lt48_status finish(const lt48_protocol_transport *api,void *context,lt48_status outcome){
 lt48_protocol_transport uncancelled=*api;
 uncancelled.cancelled=NULL;
 lt48_runtime_transport_session *wire=NULL;
 lt48_status s=lt48_runtime_transport_session_create(&uncancelled,context,&wire);
 const unsigned char bytes[]={0x40,outcome==LT48_OK?4:2};
 if(s==LT48_OK)s=lt48_runtime_transport_session_controller_write(wire,bytes,sizeof(bytes));
 if(s==LT48_OK)s=lt48_runtime_transport_session_flush(wire);
 lt48_runtime_transport_session_close(wire);
 return s;
}
/* Each generic transaction retains the source host mode/DEVINIT/action/reset
 * order and mapped code-to-EEPROM cursor behavior. No custom pin I/O here. */
static lt48_status stage(const lt48_pic24_ka_backend *r,const lt48_protocol_transport *api,void *context,
 unsigned char *image,size_t size,uint32_t flags,const lt48_pic_source_startup_plan *request,
 unsigned mode,unsigned eeprom,lt48_pic24_ka_backend_result *result){
 if(api->cancelled&&api->cancelled(context))return LT48_CANCELLED;
 const lt48_pic_source_resources *source=lt48_pic24_ka_resources_source(r->resources,(int)eeprom);
 lt48_runtime_options options={0};
 options.device_image=image;
 options.device_image_size=size;
 options.device_element_bytes=1;
 options.use_explicit_buffer_ids=1;
 options.address_buffer=0;
 options.mode_buffer=8;
 options.parameter_buffer_id=3;
 options.root_timeout_seconds=10;
 lt48_pic_source_session *session=NULL;
 lt48_pic_source_transport *transport=NULL;
 lt48_status s=lt48_pic_source_resources_create_session_internal(source,&options,&session);
 lt48_pic_source_host_binding host={r->plan->voltage_variable,0,1};
 if(s==LT48_OK)s=lt48_pic_source_transport_create_internal(api,context,session,&host,320,320,320,&transport);
 if(s==LT48_OK)s=lt48_pic_applet_plan_prepare_startup(r->plan,source,transport,flags,request);
 memset(&result->last,0,sizeof(result->last));
 if(s==LT48_OK){
  if(mode==7)s=lt48_pic_applet_erase_run(r->plan,source,transport,&result->last);
  else if(mode==12||mode==13){
   s=lt48_pic_applet_configuration_run(r->plan,source,transport,(uint16_t)mode,&result->last);
   if(s==LT48_OK&&mode==12){const unsigned char *observed=NULL;
   size_t observed_size=0;
    s=lt48_pic_source_session_image_internal(session,&observed,&observed_size);
    if(s==LT48_OK&&observed_size!=size)s=LT48_RUNTIME_ERROR;
    if(s==LT48_OK)memcpy(image+0x1f00000u,observed+0x1f00000u,36);
   }
  }else{
   lt48_pic_source_applet_stream_plan p;
   s=lt48_pic_applet_plan_make(r->plan,(uint16_t)mode,0,r->plan->element_count,&p);
   if(s==LT48_OK)s=mode==1?lt48_pic_source_transport_applet_read_tail_internal(transport,&p,r->plan->readcfg,image,size,&result->last):
    lt48_pic_source_transport_applet_stream_run_internal(transport,&p,image,size,&result->last);
  }
 }
 if(result->last.cleanup_status!=LT48_OK&&result->cleanup_status==LT48_OK)result->cleanup_status=result->last.cleanup_status;
 /* Preparation can fail before an operation result exists. Its cleanup
  * still runs in the shared transport and must not disappear from diagnostics. */
 if(transport&&result->cleanup_status==LT48_OK)
  result->cleanup_status=lt48_pic_source_transport_cleanup_status_internal(transport);
 lt48_pic_source_transport_close_internal(transport);
 lt48_pic_source_session_close_internal(session);
 if(s==LT48_OK)result->stages_completed++;
 return s;
}
static lt48_status compare(const lt48_pic24_ka_backend *r,const unsigned char *want,const unsigned char *got,
 int blank,int include_configuration,lt48_pic24_ka_backend_result *result){
 const lt48_pic24_ka_image_policy *g=lt48_pic24_ka_resources_geometry(r->resources);
 for(uint32_t dense=0;dense<g->dense_bytes;dense+=4){
  lt48_pic24_ka_location loc;
  uint32_t a,b;
  if(!lt48_pic24_ka_dense_to_location(g,dense,&loc)||
     !lt48_pic24_ka_word_decode(&loc,want+loc.image_offset,&a)||
     !lt48_pic24_ka_word_decode(&loc,got+loc.image_offset,&b))return LT48_RUNTIME_ERROR;
  if(blank)a=loc.bits==24?0xffffff:65535;
  if(a!=b){result->mismatch_offset=loc.image_offset;
  result->expected=a;
  result->observed=b;
  return LT48_DEVICE_MISMATCH;
  }
 }
 if(!include_configuration)return LT48_OK;
 uint32_t a[8],b[8];
 if(!lt48_pic24_ka_configuration_read_image(want,g->image_span,a)||
    !lt48_pic24_ka_configuration_read_image(got,g->image_span,b))return LT48_RUNTIME_ERROR;
 for(unsigned i=0;i<8;i++){
  unsigned mask=r->configuration.read_mask[i];
  uint32_t expected=blank?mask:a[i]&mask;
  if(expected!=(b[i]&mask)){result->mismatch_offset=0x1f00000u+4*(i?i+1:0);
  result->expected=expected;
  result->observed=b[i]&mask;
  return LT48_DEVICE_MISMATCH;
  }
 }
 result->configuration_readback_confirmed=1;
 return LT48_OK;
}
static lt48_status independent_read(const lt48_pic24_ka_backend *r,const lt48_protocol_transport *api,void *context,
 unsigned char *expected,size_t size,uint32_t flags,const lt48_pic_source_startup_plan *request,
 int blank,int include_configuration,lt48_pic24_ka_backend_result *out){
 unsigned char *got=malloc(size);
 if(!got)return LT48_NO_MEMORY;
 memset(got,0xa5,size);
 lt48_status s=stage(r,api,context,got,size,flags,request,1,0,out);
 if(s==LT48_OK)s=compare(r,expected,got,blank,include_configuration,out);
 free(got);
 return s;
}
static lt48_status configuration_readback(const lt48_pic24_ka_backend *r,const lt48_protocol_transport *api,void *context,
 unsigned char *image,size_t size,uint32_t flags,const lt48_pic_source_startup_plan *request,
 const uint32_t expected[8],lt48_pic24_ka_backend_result *out){
 unsigned char saved[36];
 memcpy(saved,image+0x1f00000u,36);
 lt48_status status=stage(r,api,context,image,size,flags,request,12,0,out);
 uint32_t actual[8];
 if(status==LT48_OK&&!lt48_pic24_ka_configuration_read_image(image,size,actual))status=LT48_RUNTIME_ERROR;
 for(unsigned i=0;status==LT48_OK&&i<8;i++){
  unsigned mask=r->configuration.read_mask[i];
  if((expected[i]&mask)!=(actual[i]&mask)){out->mismatch_offset=0x1f00000u+4*(i?i+1:0);
   out->expected=expected[i]&mask;
   out->observed=actual[i]&mask;
   status=LT48_DEVICE_MISMATCH;
   }
 }
 memcpy(image+0x1f00000u,saved,36);
 if(status==LT48_OK)out->configuration_readback_confirmed=1;
 return status;
}
static lt48_status erase_all(const lt48_pic24_ka_backend *r,const lt48_protocol_transport *api,void *context,
 unsigned char *image,size_t size,uint32_t flags,const lt48_pic_source_startup_plan *request,lt48_pic24_ka_backend_result *out){
 lt48_status s=stage(r,api,context,image,size,flags,request,7,0,out);
 if(s==LT48_OK)s=stage(r,api,context,image,size,flags,request,7,1,out);
 if(s==LT48_OK)s=independent_read(r,api,context,image,size,flags,request,1,1,out);
 return s;
}
lt48_status lt48_pic24_ka_backend_execute(const lt48_pic24_ka_backend *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind operation,unsigned char *image,size_t size,
 uint32_t flags,const lt48_pic_source_startup_plan *request,lt48_pic24_ka_backend_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));
 out->status=LT48_INVALID_ARGUMENT;
 out->mismatch_offset=UINT32_MAX;
 if(!r||!api||!api->read||!api->write||!request||!image||size!=lt48_pic24_ka_backend_capacity(r))return out->status;
 if(flags&~UINT32_C(0x80003)){out->status=LT48_UNSUPPORTED;
 return out->status;
 }
 switch(operation){case LT48_OPERATION_READ:case LT48_OPERATION_BLANK_CHECK:case LT48_OPERATION_VERIFY:
 case LT48_OPERATION_COMPARE:case LT48_OPERATION_ERASE:case LT48_OPERATION_PROGRAM:case LT48_OPERATION_OVER_PROGRAM:
 case LT48_OPERATION_CONFIGURATION:case LT48_OPERATION_SECURE:break;
 default:out->status=LT48_UNSUPPORTED;
 return out->status;
 }
 unsigned char *staged=malloc(size);
 if(!staged){out->status=LT48_NO_MEMORY;
 return out->status;
 }memcpy(staged,image,size);
 lt48_status s=LT48_OK;
 uint32_t desired[8]={0};
 uint16_t normalized[8]={0};
 int protect=0;
 if(operation==LT48_OPERATION_PROGRAM||operation==LT48_OPERATION_OVER_PROGRAM||operation==LT48_OPERATION_VERIFY||operation==LT48_OPERATION_COMPARE||operation==LT48_OPERATION_CONFIGURATION){
  if(!lt48_pic24_ka_configuration_read_image(staged,size,desired)||
     !lt48_pic24_ka_configuration_normalize(&r->configuration,desired,normalized))s=LT48_SAFETY_REJECTED;
  for(unsigned i=0;i<8;i++)desired[i]=normalized[i];
  if(s==LT48_OK&&!lt48_pic24_ka_configuration_write_image(&r->configuration,staged,size,desired))s=LT48_SAFETY_REJECTED;
 }
 if(s==LT48_OK)switch(operation){
 case LT48_OPERATION_READ:s=stage(r,api,context,staged,size,flags,request,1,0,out);
 break;
 case LT48_OPERATION_BLANK_CHECK:
  s=stage(r,api,context,staged,size,flags,request,3,0,out);
  if(s==LT48_OK)s=independent_read(r,api,context,staged,size,flags,request,1,1,out);
  break;
 case LT48_OPERATION_ERASE:s=erase_all(r,api,context,staged,size,flags,request,out);
 break;
 case LT48_OPERATION_VERIFY:
  s=stage(r,api,context,staged,size,flags,request,4,0,out);
  if(s==LT48_OK)s=independent_read(r,api,context,staged,size,flags,request,0,1,out);
  break;
 case LT48_OPERATION_COMPARE:s=independent_read(r,api,context,staged,size,flags,request,0,1,out);
 break;
 case LT48_OPERATION_PROGRAM:case LT48_OPERATION_OVER_PROGRAM: {
  uint16_t unprotected[8];
  if(!lt48_pic24_ka_configuration_before_verify(&r->configuration,desired,unprotected,&protect)){s=LT48_SAFETY_REJECTED;
  break;
  }
  uint32_t initial[8];
  for(unsigned i=0;i<8;i++)initial[i]=unprotected[i];
  if(!lt48_pic24_ka_configuration_write_image(&r->configuration,staged,size,initial)){s=LT48_SAFETY_REJECTED;
  break;
  }
  if(operation==LT48_OPERATION_PROGRAM)s=erase_all(r,api,context,staged,size,flags,request,out);
  if(s==LT48_OK)s=stage(r,api,context,staged,size,flags,request,2,0,out);
  /* Both original Verify voltage variables are320cV for this exact cohort.
   * Retain the two source passes; independently read before protection. */
  for(unsigned pass=0;s==LT48_OK&&pass<2;pass++)s=stage(r,api,context,staged,size,flags,request,4,0,out);
  /* Original00466c10 calls Verify before Protect/configuration. Keep that
   * order: verify user arrays first,then write/read ordinary configuration,
   * then apply protection. Do not compare unwritten requested configuration
   * during the array readback. */
  if(s==LT48_OK)s=independent_read(r,api,context,staged,size,flags,request,0,0,out);
  if(s==LT48_OK)s=stage(r,api,context,staged,size,flags,request,13,0,out);
  if(s==LT48_OK)s=configuration_readback(r,api,context,staged,size,flags,request,initial,out);
  if(s==LT48_OK&&protect){
   if(!lt48_pic24_ka_configuration_write_image(&r->configuration,staged,size,desired))s=LT48_SAFETY_REJECTED;
   if(s==LT48_OK)s=stage(r,api,context,staged,size,flags,request,13,0,out);
   if(s==LT48_OK)out->protection_write_completed=1;
  }
  break;
 }
 case LT48_OPERATION_CONFIGURATION:case LT48_OPERATION_SECURE: {
  unsigned char saved[36];
  memcpy(saved,staged+0x1f00000u,36);
  s=stage(r,api,context,staged,size,flags,request,12,0,out);
  uint32_t observed[8];
  uint16_t current[8];
  if(s==LT48_OK&&!lt48_pic24_ka_configuration_read_image(staged,size,observed))s=LT48_RUNTIME_ERROR;
  if(s==LT48_OK){
   for(unsigned i=0;i<8;i++)current[i]=(uint16_t)observed[i];
   if(operation==LT48_OPERATION_SECURE){for(unsigned i=0;i<8;i++)desired[i]=observed[i];
   desired[1]&=~2u;
   }
   if(current[0]!=15||(current[1]&3)!=3||!lt48_pic24_ka_configuration_can_program(&r->configuration,current,desired,normalized))s=LT48_SAFETY_REJECTED;
  }
  if(s==LT48_OK){memcpy(staged+0x1f00000u,saved,36);
   if(!lt48_pic24_ka_configuration_write_image(&r->configuration,staged,size,desired))s=LT48_SAFETY_REJECTED;
  }
  protect=operation==LT48_OPERATION_SECURE||desired[0]!=15||desired[1]!=3;
  uint16_t unprotected[8];
  uint32_t initial[8];
  if(s==LT48_OK&&!lt48_pic24_ka_configuration_before_verify(&r->configuration,desired,unprotected,&protect))s=LT48_SAFETY_REJECTED;
  if(s==LT48_OK){for(unsigned i=0;i<8;i++)initial[i]=unprotected[i];
   if(!lt48_pic24_ka_configuration_write_image(&r->configuration,staged,size,initial))s=LT48_SAFETY_REJECTED;
   }
  if(s==LT48_OK)s=stage(r,api,context,staged,size,flags,request,13,0,out);
  if(s==LT48_OK)s=configuration_readback(r,api,context,staged,size,flags,request,initial,out);
  if(s==LT48_OK&&protect){
   if(!lt48_pic24_ka_configuration_write_image(&r->configuration,staged,size,desired))s=LT48_SAFETY_REJECTED;
   if(s==LT48_OK)s=stage(r,api,context,staged,size,flags,request,13,0,out);
   if(s==LT48_OK)out->protection_write_completed=1;
  }
  break;
 }
 default:s=LT48_UNSUPPORTED;
 break;
 }
 /* Preflight failures must not initiate hardware merely to set an LED. */
 if(out->last.cleanup_attempted||out->stages_completed){out->indication_status=finish(api,context,s);
 if(s==LT48_OK)s=out->indication_status;
 }
 if(s==LT48_OK&&(operation==LT48_OPERATION_READ||operation==LT48_OPERATION_PROGRAM||operation==LT48_OPERATION_OVER_PROGRAM||operation==LT48_OPERATION_CONFIGURATION||operation==LT48_OPERATION_SECURE))memcpy(image,staged,size);
 free(staged);
 out->status=s;
 return s;
}
