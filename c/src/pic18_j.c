#include "pic18_j.h"
#include "internal.h"
#include "../../NewDeviceLib/sources/pic-common/source_resources.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct {
 const char *key;uint16_t definition,family,pinmap,voltage_variable;
 uint8_t size_buffer,image_buffer,increment_buffer,data_in;
 uint16_t init,reset,erase,fpga,nominal_voltage,verify_low,verify_high;
} j_owner_context;
#include "../../NewDeviceLib/sources/pic-common/pic18_j_owner_contexts_generated.h"
struct lt48_pic18_j_owner {
 const j_owner_context *context;
 const lt48_pic18_j_options_profile *options;
 const lt48_pic18_j_adapter_profile *adapter;
 lt48_pic_source_resources *source;
 unsigned char *idle;size_t idle_size;
 lt48_m37_id_decision_fn id_decision;void *id_context;
};
void lt48_pic18_j_owner_close(lt48_pic18_j_owner *r){if(r){lt48_pic_source_resources_close_internal(r->source);free(r->idle);free(r);}}
const lt48_pic18_j_options_profile *lt48_pic18_j_owner_options(const lt48_pic18_j_owner *r){return r?r->options:NULL;}
const lt48_pic18_j_adapter_profile *lt48_pic18_j_owner_adapter(const lt48_pic18_j_owner *r){return r?r->adapter:NULL;}
lt48_status lt48_pic18_j_owner_open(const lt48_installation *installation,size_t device,lt48_pic18_j_owner **out) {
 if(!installation||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(installation,device,&spec);
 if(st!=LT48_OK)return st;
 if(spec.catalogue.device_class!=2048||!spec.definition_stem||strcmp(spec.definition_stem,"GDCC___")||
    !spec.catalogue.vendor||strcmp(spec.catalogue.vendor,"Microchip")||!spec.catalogue.device_name||spec.catalogue.device_name_length>200)return LT48_UNSUPPORTED;
 char key[256];int n=snprintf(key,sizeof(key),"Microchip:%.*s",(int)spec.catalogue.device_name_length,spec.catalogue.device_name);
 if(n<0||(size_t)n>=sizeof(key))return LT48_INVALID_ARGUMENT;
 const lt48_pic18_j_options_profile *options=lt48_pic18_j_options_find(key,spec.definition_resource,spec.family_code,spec.pinmap);
 const lt48_pic18_j_adapter_profile *adapter=lt48_pic18_j_adapter_find(key,spec.definition_resource,spec.family_code,spec.pinmap);
 if(!options||!adapter)return LT48_UNSUPPORTED;
 if(spec.capacity_bytes!=options->program.code_bytes||spec.element_bits!=8||spec.element_count!=options->program.code_bytes)return LT48_SAFETY_REJECTED;
 const j_owner_context *context=NULL;
 for(size_t i=0;i<sizeof(j_owner_contexts)/sizeof(*j_owner_contexts);i++)if(!strcmp(j_owner_contexts[i].key,key))context=j_owner_contexts+i;
 if(!context||context->definition!=spec.definition_resource||context->family!=spec.family_code||context->pinmap!=spec.pinmap)return LT48_SAFETY_REJECTED;
 lt48_pic18_j_owner *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
 r->context=context;r->options=options;r->adapter=adapter;char path[4096];
 n=snprintf(path,sizeof(path),"%s/GDCC___.rez",lt48_installation_path(installation));
 if(n<0||(size_t)n>=sizeof(path)){lt48_pic18_j_owner_close(r);return LT48_INVALID_ARGUMENT;}
 st=lt48_pic_source_resources_library_internal(path,j_owner_source_digest,context->family,context->pinmap,&r->source);
 if(st==LT48_OK)st=lt48_load_verified_idle_fpga_internal(lt48_installation_path(installation),&r->idle,&r->idle_size);
 if(st!=LT48_OK){lt48_pic18_j_owner_close(r);return st;}
 *out=r;return LT48_OK;
}
lt48_status lt48_pic18_j_owner_contact_masks(const lt48_pic18_j_owner *r,lt48_pic_contact_masks *out) {
 if(!r||!out)return LT48_INVALID_ARGUMENT;
 lt48_pic_contact_masks masks;lt48_status st=lt48_pic_source_resources_contact_masks_internal(r->source,&masks);
 if(st!=LT48_OK)return st;
 if(masks.alternate)return LT48_UNSUPPORTED;
 unsigned char pins[48]={0};const lt48_pic18_j_adapter_profile *a=r->adapter;
 for(size_t i=0;i<a->net_count;i++) {
  unsigned dip=a->nets[i].dip,zif=a->dip1_zif+dip-1;
  if(!dip||dip>a->dip_pins||zif>48)return LT48_SAFETY_REJECTED;
  pins[zif-1]=1;
 }
 uint32_t wired[2];lt48_pic_contact_pack(pins,1,wired);
 for(unsigned side=0;side<2;side++) {
  masks.required[side]&=wired[side];masks.auxiliary[side]&=wired[side];masks.adapter[side]&=wired[side];
  if(wired[side]&masks.forbidden[side])return LT48_SAFETY_REJECTED;
 }
 /* The Ethernet carrier deliberately isolates the stock external-core pin. */
 if(a->unused_core_dip) {
  memset(pins,0,sizeof(pins));unsigned zif=a->dip1_zif+a->unused_core_dip-1;
  if(zif>48)return LT48_SAFETY_REJECTED;
  pins[zif-1]=1;uint32_t isolated[2];lt48_pic_contact_pack(pins,1,isolated);
  for(unsigned side=0;side<2;side++)masks.forbidden[side]|=isolated[side];
 }
 if(!(masks.required[0]|masks.required[1]))return LT48_SAFETY_REJECTED;
 *out=masks;return LT48_OK;
}
static lt48_status j_prepare(const lt48_pic18_j_owner *r,lt48_pic_source_transport *owner,
 lt48_pic18_j_owner_result *result) {
 lt48_pic_contact_masks masks;lt48_status st=lt48_pic18_j_owner_contact_masks(r,&masks);
 if(st==LT48_OK)st=lt48_pic_source_resources_prepare_startup_internal(r->source,r->context->fpga,owner);
 if(st==LT48_OK)st=lt48_pic_source_transport_require_contact_internal(owner,&masks,&result->contact);
 return st;
}
static lt48_status j_memory_at_voltage(const lt48_pic18_j_owner *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind operation,int authorize_erase,
 const uint8_t *expected,size_t expected_size,uint8_t *output,size_t output_size,lt48_pic18_j_owner_result *result,unsigned voltage) {
 if(!result)return LT48_INVALID_ARGUMENT;
 memset(result,0,sizeof(*result));result->operation.first_mismatch=UINT32_MAX;
 result->operation.operation_status=LT48_INVALID_ARGUMENT;
 if(!r||!api||!api->write||!api->read||voltage<270||voltage>360)return LT48_INVALID_ARGUMENT;
 size_t size=r->options->program.code_bytes;
 if(operation!=LT48_OPERATION_READ&&operation!=LT48_OPERATION_BLANK_CHECK&&operation!=LT48_OPERATION_COMPARE&&operation!=LT48_OPERATION_VERIFY&&operation!=LT48_OPERATION_ERASE){result->operation.operation_status=LT48_UNSUPPORTED;return LT48_UNSUPPORTED;}
 if((operation==LT48_OPERATION_READ&&(!output||output_size<size))||
    ((operation==LT48_OPERATION_COMPARE||operation==LT48_OPERATION_VERIFY)&&(!expected||expected_size!=size)))return LT48_INVALID_ARGUMENT;
 if(authorize_erase&~3)return LT48_INVALID_ARGUMENT;
 if(operation==LT48_OPERATION_ERASE&&!(authorize_erase&LT48_PIC18_J_ERASE_AUTHORIZED)){result->operation.operation_status=LT48_SAFETY_REJECTED;return LT48_SAFETY_REJECTED;}
 uint8_t *image=malloc(size);if(!image){result->operation.operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}memset(image,255,size);
 if(operation==LT48_OPERATION_ERASE) {
  lt48_pic18_j_owner_result before;
  lt48_status pre=lt48_pic18_j_owner_memory(r,api,context,LT48_OPERATION_READ,
      authorize_erase&LT48_PIC18_J_IGNORE_ID_MISMATCH,NULL,0,image,size,&before);
  if(pre!=LT48_OK){*result=before;free(image);return pre;}
 }
 lt48_runtime_options opts={0};opts.device_image=image;opts.device_image_size=size;opts.device_element_bytes=1;opts.use_explicit_buffer_ids=1;opts.address_buffer=0;opts.mode_buffer=8;opts.parameter_buffer_id=r->context->data_in;
 lt48_pic_source_session *session=NULL;lt48_pic_source_transport *owner=NULL;
 lt48_status st=lt48_pic_source_resources_create_session_internal(r->source,&opts,&session);
 lt48_pic_source_host_binding binding={r->context->voltage_variable,0,1};
 if(st==LT48_OK)st=lt48_pic_source_transport_create_internal(api,context,session,&binding,voltage,270,360,&owner);
 if(st==LT48_OK)st=lt48_pic_source_transport_idle_internal(owner,r->idle,r->idle_size);
 if(st==LT48_OK)st=j_prepare(r,owner,result);
 lt48_pic18_j_memory_result memory={0};memory.transfer.first_mismatch=UINT32_MAX;lt48_pic_source_applet_program_plan plan={0};
 plan.capacity=(uint32_t)size;plan.stream.init_role=r->context->init;plan.stream.reset_role=r->context->reset;plan.stream.size_buffer=r->context->size_buffer;plan.stream.image_buffer=r->context->image_buffer;plan.stream.increment_buffer=r->context->increment_buffer;plan.stream.protocol=LT48_PIC_STREAM_BYTES_48;plan.erase_role=r->context->erase;plan.data_in=r->context->data_in;
 if(st==LT48_OK)st=lt48_pic_source_transport_j_memory_internal(owner,&plan,&r->options->program,operation==LT48_OPERATION_ERASE,image,size,&memory);
 result->operation.cleanup_status=owner?lt48_pic_source_transport_cleanup_status_internal(owner):LT48_OK;
 if(memory.transfer.cleanup_error)result->operation.cleanup_status=(lt48_status)memory.transfer.cleanup_error;
 result->operation.cleanup_completed=result->operation.cleanup_status==LT48_OK;
 result->observed_id=memory.device_id;result->id_mismatch=memory.id_observed&&(memory.device_id&r->options->device_id_mask)!=(r->options->device_id&r->options->device_id_mask);
 result->protected_read=memory.protected_read;result->configuration_from_mirrors=memory.configuration_from_mirrors;
 result->operation.first_mismatch=memory.transfer.first_mismatch;
 if(st==LT48_OK&&result->id_mismatch&&!(authorize_erase&LT48_PIC18_J_IGNORE_ID_MISMATCH))st=LT48_DEVICE_MISMATCH;
 if(st==LT48_OK&&(operation==LT48_OPERATION_COMPARE||operation==LT48_OPERATION_VERIFY)&&memory.protected_read)st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK&&operation!=LT48_OPERATION_READ&&operation!=LT48_OPERATION_ERASE)for(uint32_t a=0;a<size;a++) {
  unsigned mask=operation==LT48_OPERATION_BLANK_CHECK?255:a<size-8?255:r->options->program.mirror_masks[a-(size-8)];
  unsigned value=operation==LT48_OPERATION_BLANK_CHECK?255:expected[a];
  if((image[a]^value)&mask){if(result->operation.first_mismatch==UINT32_MAX)result->operation.first_mismatch=a;result->operation.mismatch_count++;}
 }
 if(st==LT48_OK&&result->operation.mismatch_count)st=LT48_DEVICE_MISMATCH;
 if(st==LT48_OK&&operation==LT48_OPERATION_READ)memcpy(output,image,size);
 if(st==LT48_OK)result->operation.completed_bytes=size;
 if(st!=LT48_OK&&result->operation.cleanup_completed) {
  lt48_status led=lt48_runtime_transport_error_indicator_internal(api,context);
  if(led!=LT48_OK){result->operation.cleanup_status=led;result->operation.cleanup_completed=0;}
 }
 result->operation.operation_status=st;lt48_pic_source_transport_close_internal(owner);lt48_pic_source_session_close_internal(session);free(image);return st;
}
void lt48_pic18_j_owner_set_id_decision(lt48_pic18_j_owner *r,lt48_m37_id_decision_fn decision,void *context) {
 if(r){r->id_decision=decision;r->id_context=context;}
}
static lt48_status j_memory_with_decision(const lt48_pic18_j_owner *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind operation,int flags,
 const uint8_t *expected,size_t expected_size,uint8_t *output,size_t output_size,
 lt48_pic18_j_owner_result *result,unsigned voltage) {
 for(;;) {
  lt48_status st=j_memory_at_voltage(r,api,context,operation,flags,expected,expected_size,output,output_size,result,voltage);
  /* Erase's preflight Read owns decisions before mutation. Never retry an
   * already attempted erase automatically due to a later identity failure. */
  if(operation==LT48_OPERATION_ERASE||!r||!result||st!=LT48_DEVICE_MISMATCH||!result->id_mismatch||
     !result->operation.cleanup_completed||!r->id_decision)return st;
  lt48_m37_id_choice choice=r->id_decision(r->id_context,0,r->options->device_id,0,result->observed_id);
  if(choice==LT48_M37_ID_IGNORE){flags|=LT48_PIC18_J_IGNORE_ID_MISMATCH;continue;}
  if(choice==LT48_M37_ID_RETRY)continue;
  if(choice==LT48_M37_ID_ABORT){result->operation.operation_status=LT48_CANCELLED;return LT48_CANCELLED;}
  result->operation.operation_status=LT48_INVALID_ARGUMENT;return LT48_INVALID_ARGUMENT;
 }
}
lt48_status lt48_pic18_j_owner_memory(const lt48_pic18_j_owner *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind operation,int flags,
 const uint8_t *expected,size_t expected_size,uint8_t *output,size_t output_size,lt48_pic18_j_owner_result *result) {
 return j_memory_with_decision(r,api,context,operation,flags,expected,expected_size,output,output_size,result,r?r->context->nominal_voltage:0);
}
lt48_status lt48_pic18_j_owner_verify(const lt48_pic18_j_owner *r,
 const lt48_protocol_transport *api,void *context,int flags,const uint8_t *expected,size_t size,
 lt48_verify_margin_selection margin,unsigned type,lt48_pic18_j_owner_result *result) {
 if(!result)return LT48_INVALID_ARGUMENT;
 memset(result,0,sizeof(*result));result->operation.operation_status=LT48_INVALID_ARGUMENT;result->operation.first_mismatch=UINT32_MAX;
 if(!r||type>1||margin>LT48_VERIFY_MARGIN_ALL||margin<LT48_VERIFY_MARGIN_NOMINAL)return LT48_INVALID_ARGUMENT;
 unsigned levels[2]={r->context->nominal_voltage,r->context->nominal_voltage},count=1;
 if(margin==LT48_VERIFY_MARGIN_LOW)levels[0]=r->context->verify_low;
 else if(margin==LT48_VERIFY_MARGIN_HIGH)levels[0]=r->context->verify_high;
 else if(margin==LT48_VERIFY_MARGIN_ALL){levels[0]=r->context->verify_low;levels[1]=r->context->verify_high;count=2;}
 for(unsigned i=0;i<count;i++) {
  lt48_status st=j_memory_with_decision(r,api,context,LT48_OPERATION_VERIFY,flags,expected,size,NULL,0,result,levels[i]);
  if(st!=LT48_OK)return st;
  if(result->id_mismatch)flags|=LT48_PIC18_J_IGNORE_ID_MISMATCH;
 }
 return LT48_OK;
}
lt48_status lt48_pic18_j_owner_mutate(const lt48_pic18_j_owner *r,
 const lt48_protocol_transport *api,void *context,lt48_operation_kind operation,int flags,
 const uint8_t *input,size_t input_size,uint8_t *backup,size_t backup_size,lt48_pic18_j_owner_result *result) {
 if(!result)return LT48_INVALID_ARGUMENT;
 memset(result,0,sizeof(*result));result->operation.first_mismatch=UINT32_MAX;result->operation.operation_status=LT48_INVALID_ARGUMENT;
 if(!r||!api||!api->write||!api->read||(flags&~3))return LT48_INVALID_ARGUMENT;
 size_t size=r->options->program.code_bytes;
 if(operation!=LT48_OPERATION_PROGRAM&&operation!=LT48_OPERATION_OVER_PROGRAM&&operation!=LT48_OPERATION_SECURE){result->operation.operation_status=LT48_UNSUPPORTED;return LT48_UNSUPPORTED;}
 if(operation!=LT48_OPERATION_SECURE&&(!input||input_size!=size))return LT48_INVALID_ARGUMENT;
 if(operation==LT48_OPERATION_SECURE&&(input||input_size))return LT48_INVALID_ARGUMENT;
 if(operation!=LT48_OPERATION_PROGRAM&&(!backup||backup_size<size))return LT48_INVALID_ARGUMENT;
 if(input&&backup) {
  uintptr_t a=(uintptr_t)input,b=(uintptr_t)backup;
  if(a<=b?b-a<size:a-b<backup_size)return LT48_INVALID_ARGUMENT;
 }
 uint8_t *staged=malloc(size),*before=malloc(size);
 if(!staged||!before){free(staged);free(before);result->operation.operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 lt48_status st=lt48_pic18_j_owner_memory(r,api,context,LT48_OPERATION_READ,flags&LT48_PIC18_J_IGNORE_ID_MISMATCH,NULL,0,before,size,result);
 if(st!=LT48_OK){free(staged);free(before);return st;}
 if(operation==LT48_OPERATION_SECURE&&result->protected_read){free(staged);free(before);return LT48_OK;}
 if(!(flags&LT48_PIC18_J_ERASE_AUTHORIZED)) {
  st=LT48_SAFETY_REJECTED;result->operation.operation_status=st;result->operation.completed_bytes=0;
  lt48_status led=lt48_runtime_transport_error_indicator_internal(api,context);
  if(led!=LT48_OK){result->operation.cleanup_status=led;result->operation.cleanup_completed=0;}
  free(staged);free(before);return st;
 }
 if(operation==LT48_OPERATION_PROGRAM)memcpy(staged,input,size);else memset(staged,255,size);
 if(operation==LT48_OPERATION_PROGRAM)st=lt48_pic18_j_options_edit(r->options,staged,size,NULL,0);
 lt48_pic_source_session *session=NULL;lt48_pic_source_transport *owner=NULL;
 lt48_runtime_options opts={0};opts.device_image=staged;opts.device_image_size=size;opts.device_element_bytes=1;opts.use_explicit_buffer_ids=1;opts.address_buffer=0;opts.mode_buffer=8;opts.parameter_buffer_id=r->context->data_in;
 if(st==LT48_OK)st=lt48_pic_source_resources_create_session_internal(r->source,&opts,&session);
 lt48_pic_source_host_binding binding={r->context->voltage_variable,0,1};
 if(st==LT48_OK)st=lt48_pic_source_transport_create_internal(api,context,session,&binding,r->context->nominal_voltage,270,360,&owner);
 if(st==LT48_OK)st=lt48_pic_source_transport_idle_internal(owner,r->idle,r->idle_size);
 if(st==LT48_OK)st=j_prepare(r,owner,result);
 lt48_pic_source_applet_program_plan plan={0};plan.capacity=(uint32_t)size;plan.stream.init_role=r->context->init;plan.stream.reset_role=r->context->reset;plan.stream.size_buffer=r->context->size_buffer;plan.stream.image_buffer=r->context->image_buffer;plan.stream.increment_buffer=r->context->increment_buffer;plan.stream.protocol=LT48_PIC_STREAM_BYTES_48;plan.erase_role=r->context->erase;plan.data_in=r->context->data_in;
 lt48_pic18_j_program_result programmed={0};programmed.first_mismatch=UINT32_MAX;
 if(st==LT48_OK&&operation==LT48_OPERATION_PROGRAM)st=lt48_pic_source_transport_j_program_internal(owner,&plan,&r->options->program,staged,size,&programmed);
 else if(st==LT48_OK) {
  lt48_pic18_j_rewrite_result rewrite={0};
  st=lt48_pic_source_transport_j_rewrite_internal(owner,&plan,&r->options->program,
      operation==LT48_OPERATION_SECURE?LT48_PIC18_J_REWRITE_SECURE:LT48_PIC18_J_REWRITE_PATCH,
      0,operation==LT48_OPERATION_SECURE?NULL:input,operation==LT48_OPERATION_SECURE?0:size,
      backup,staged,size,&rewrite);
  programmed=rewrite.program;result->backup_valid=rewrite.backup_valid;
 }
 result->operation.operation_status=st;result->operation.first_mismatch=programmed.first_mismatch;
 if(st==LT48_DEVICE_MISMATCH&&programmed.first_mismatch!=UINT32_MAX)result->operation.mismatch_count=1;
 result->operation.cleanup_status=owner?lt48_pic_source_transport_cleanup_status_internal(owner):LT48_OK;
 if(programmed.cleanup_error)result->operation.cleanup_status=(lt48_status)programmed.cleanup_error;
 result->operation.cleanup_completed=result->operation.cleanup_status==LT48_OK;
 result->operation.completed_bytes=st==LT48_OK?size:0;
 if(st!=LT48_OK&&result->operation.cleanup_completed) {
  lt48_status led=lt48_runtime_transport_error_indicator_internal(api,context);
  if(led!=LT48_OK){result->operation.cleanup_status=led;result->operation.cleanup_completed=0;}
 }
 lt48_pic_source_transport_close_internal(owner);lt48_pic_source_session_close_internal(session);free(staged);free(before);return st;
}
