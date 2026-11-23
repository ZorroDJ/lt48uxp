#include <labtool48/labtool48.h>
#include "../../NewDeviceLib/drafts/pic-population/pic24_backend.h"
#include "../../NewDeviceLib/drafts/pic-population/pic24_ka_backend.h"
#include "../../NewDeviceLib/drafts/pic-population/pic24_klkm_backend.h"
#include "../../NewDeviceLib/drafts/pic-population/dspic30_backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
typedef struct {const char *key;uint32_t words;unsigned series,fv,pins;uint16_t family,pinmap;} ka_profile;
#include "../../NewDeviceLib/drafts/pic-population/pic24_ka_profiles_generated.inc"
typedef ka_profile klkm_profile;
#include "../../NewDeviceLib/drafts/pic-population/pic24_klkm_profiles_generated.inc"
typedef struct {const char *key;uint32_t words,dense;unsigned classic,motor,pins;uint16_t family,pinmap;} dspic30_profile;
#include "../../NewDeviceLib/drafts/pic-population/dspic30_profiles_generated.inc"
static uint32_t applet_capacity(const lt48_pic_applet_plan_profile *p){
 return p->family>=368&&p->family<=373?UINT32_C(0x1f0001a):p->family==450?UINT32_C(0x1f00020):UINT32_C(0x1f00024);
}
static const lt48_pic_applet_plan_profile *ka_plan(const char *key){
 for(size_t i=0;i<sizeof(ka_profiles)/sizeof(*ka_profiles);i++)if(!strcmp(key,ka_profiles[i].key)){
  for(size_t j=0;j<sizeof(pic_applet_plan_profiles)/sizeof(*pic_applet_plan_profiles);j++)
   if(!strcmp(key,pic_applet_plan_profiles[j].key))return pic_applet_plan_profiles+j;
 }
 for(size_t i=0;i<sizeof(klkm_profiles)/sizeof(*klkm_profiles);i++)if(!strcmp(key,klkm_profiles[i].key)){
  for(size_t j=0;j<sizeof(pic_applet_plan_profiles)/sizeof(*pic_applet_plan_profiles);j++)
   if(!strcmp(key,pic_applet_plan_profiles[j].key))return pic_applet_plan_profiles+j;
 }
 for(size_t i=0;i<sizeof(dspic30_profiles)/sizeof(*dspic30_profiles);i++)if(!strcmp(key,dspic30_profiles[i].key)){
  for(size_t j=0;j<sizeof(pic_applet_plan_profiles)/sizeof(*pic_applet_plan_profiles);j++)
   if(!strcmp(key,pic_applet_plan_profiles[j].key))return pic_applet_plan_profiles+j;
 }
 return NULL;
}
static int ka_key(const lt48_catalogue_entry *entry,char *key,size_t size){
 if(!entry->vendor||entry->device_name_length>INT_MAX)return 0;
 int n=snprintf(key,size,"%s:%.*s",entry->vendor,(int)entry->device_name_length,entry->device_name);
 return n>=0&&(size_t)n<size;
}
uint32_t lt48_pic24_image_capacity_internal(const lt48_installation *x,const lt48_device_spec *spec){
 if(!x||!spec||spec->catalogue.device_class!=2048||strcmp(spec->definition_stem,"GDCC___"))return 0;
 char key[512];if(!ka_key(&spec->catalogue,key,sizeof(key)))return 0;
 const lt48_pic_applet_plan_profile *p=ka_plan(key);
 if(!p||p->definition!=spec->definition_resource||p->family!=spec->family_code||p->pinmap!=spec->pinmap||
    p->element_count!=spec->capacity_bytes||spec->element_bits!=8)return 0;
 char path[4096],hex[65];unsigned char sha[32];lt48_rez *rez=NULL;
 int n=snprintf(path,sizeof(path),"%s/GDCC___.rez",lt48_installation_path(x));
 if(n<0||(size_t)n>=sizeof(path)||lt48_rez_open(path,&rez)!=LT48_OK)return 0;
 lt48_status status=lt48_rez_sha256(rez,sha);lt48_rez_close(rez);if(status!=LT48_OK)return 0;
 for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",sha[i]);
 return !strcmp(hex,p->source_sha256)?applet_capacity(p):0;
}
struct lt48_pic24_resources {lt48_pic24_backend *backend;lt48_pic24_ka_backend *ka;lt48_pic24_klkm_backend *klkm;lt48_dspic30_backend *dspic;unsigned configuration_words;uint16_t manufacturer,device;};
void lt48_pic24_close(lt48_pic24_resources *r){if(r){lt48_pic24_backend_close(r->backend);lt48_pic24_ka_backend_close(r->ka);lt48_pic24_klkm_backend_close(r->klkm);lt48_dspic30_backend_close(r->dspic);free(r);}}
uint32_t lt48_pic24_capacity(const lt48_pic24_resources *r){return r?(uint32_t)(r->dspic?lt48_dspic30_backend_capacity(r->dspic):r->klkm?lt48_pic24_klkm_backend_capacity(r->klkm):r->ka?lt48_pic24_ka_backend_capacity(r->ka):lt48_pic24_backend_capacity(r->backend)):0;}
lt48_status lt48_pic24_open(const lt48_installation *x,size_t device,lt48_pic24_resources **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!x)return LT48_INVALID_ARGUMENT;
 lt48_catalogue_entry entry;lt48_status status=lt48_installation_get_device(x,device,&entry);
 if(status!=LT48_OK)return status;
 if(entry.device_class!=2048)return LT48_UNSUPPORTED;
 if(!entry.vendor || entry.device_name_length>INT_MAX)return LT48_UNSUPPORTED;
 char key[512],path[4096];int n=snprintf(key,sizeof(key),"%s:%.*s",entry.vendor,(int)entry.device_name_length,entry.device_name);
 if(n<0 || (size_t)n>=sizeof(key))return LT48_INVALID_ARGUMENT;
 const lt48_pic24_operator_profile *p=lt48_pic24_operator_profile_find(key);
 const lt48_pic_applet_plan_profile *ka=p?NULL:ka_plan(key);
 if(!p&&!ka)return LT48_UNSUPPORTED;
 lt48_device_spec spec;status=lt48_installation_resolve_device(x,device,&spec);
 if(status!=LT48_OK)return status;
 if(strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
 if(p&&(p->definition!=spec.definition_resource || p->family!=spec.family_code || p->pinmap!=spec.pinmap || p->image_bytes!=spec.capacity_bytes))return LT48_SAFETY_REJECTED;
 if(ka&&(ka->definition!=spec.definition_resource||ka->family!=spec.family_code||ka->pinmap!=spec.pinmap||spec.capacity_bytes!=applet_capacity(ka)||ka->element_count!=spec.element_count||spec.element_bits!=8))return LT48_SAFETY_REJECTED;
 n=snprintf(path,sizeof(path),"%s/GDCC___.rez",lt48_installation_path(x));if(n<0 || (size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
 lt48_pic24_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
 r->configuration_words=ka?(applet_capacity(ka)==0x1f0001a?5:ka->family==450?7:8):p->configuration_words;
 if(ka){
  status=applet_capacity(ka)==0x1f0001a?lt48_dspic30_backend_open(path,key,&r->dspic):ka->family==450?lt48_pic24_klkm_backend_open(path,key,&r->klkm):lt48_pic24_ka_backend_open(path,key,&r->ka);
  const lt48_pic_applet_identity_profile *id=NULL;
  for(size_t i=0;i<sizeof(pic_applet_identity_profiles)/sizeof(*pic_applet_identity_profiles);i++)
   if(!strcmp(key,pic_applet_identity_profiles[i].key)){id=pic_applet_identity_profiles+i;break;}
  if(status==LT48_OK&&(!id||strcmp(ka->profile,id->profile)))status=LT48_SAFETY_REJECTED;
  if(status==LT48_OK){r->manufacturer=id->plan.manufacturer;r->device=id->plan.device;}
 }else{
  status=lt48_pic24_backend_open(path,key,&r->backend);
  if(status==LT48_OK)status=lt48_pic24_backend_expected_identity(r->backend,&r->manufacturer,&r->device);
 }
 if(status!=LT48_OK){lt48_pic24_close(r);return status;}*out=r;return LT48_OK;
}
lt48_status lt48_pic24_configuration_get(const lt48_pic24_resources *r,const unsigned char *image,size_t size,lt48_runtime_configuration_value *values,size_t capacity,size_t *count){
 return !r?LT48_INVALID_ARGUMENT:r->dspic?lt48_dspic30_backend_configuration_get(r->dspic,image,size,values,capacity,count):r->klkm?lt48_pic24_klkm_backend_configuration_get(r->klkm,image,size,values,capacity,count):r->ka?lt48_pic24_ka_backend_configuration_get(r->ka,image,size,values,capacity,count):lt48_pic24_backend_configuration_values(r->backend,image,size,values,capacity,count);
}
lt48_status lt48_pic24_configuration_set(const lt48_pic24_resources *r,unsigned char *image,size_t size,const lt48_runtime_configuration_value *values,size_t count){
 return !r?LT48_INVALID_ARGUMENT:r->dspic?lt48_dspic30_backend_configuration_set(r->dspic,image,size,values,count):r->klkm?lt48_pic24_klkm_backend_configuration_set(r->klkm,image,size,values,count):r->ka?lt48_pic24_ka_backend_configuration_set(r->ka,image,size,values,count):lt48_pic24_backend_configuration_edit(r->backend,image,size,values,count);
}
lt48_status lt48_pic24_validate(const lt48_pic24_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size){
 if(!r || start || length!=lt48_pic24_capacity(r))return LT48_INVALID_ARGUMENT;
 switch(op){
 case LT48_OPERATION_READ:case LT48_OPERATION_BLANK_CHECK:case LT48_OPERATION_ERASE:case LT48_OPERATION_SECURE:return LT48_OK;
 case LT48_OPERATION_PROGRAM:case LT48_OPERATION_OVER_PROGRAM:case LT48_OPERATION_VERIFY:case LT48_OPERATION_COMPARE:case LT48_OPERATION_CONFIGURATION:
  return image&&size==length?LT48_OK:LT48_INVALID_ARGUMENT;
 default:return LT48_UNSUPPORTED;
 }
}
lt48_status lt48_pic24_validate_verify_options(const lt48_pic24_resources *r,lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type){
 (void)op;uint32_t levels[2];unsigned count;
 if(!r)return LT48_INVALID_ARGUMENT;
 if(r->ka||r->klkm||r->dspic)return (unsigned)margin<=LT48_VERIFY_MARGIN_ALL&&type<2?LT48_OK:LT48_INVALID_ARGUMENT;
 return lt48_pic24_backend_verify_levels(r->backend,margin,type,levels,&count);
}
typedef struct {const lt48_pic24_resources *r;const lt48_pic24_execution_options *options;lt48_pic24_diagnostics *diagnostics;} decision_context;
static int decide(void *opaque,int identity,const lt48_pic_contact_result *contact,const lt48_pic_source_identity_result *actual){
 decision_context *c=opaque;(void)contact;
 if(!identity || !actual)return LT48_PIC_APP_START_ABORT;
 if(c->diagnostics){c->diagnostics->observed_manufacturer=actual->manufacturer;c->diagnostics->observed_device=actual->device;}
 if(!c->options || !c->options->id_decision)return LT48_PIC_APP_START_ABORT;
 lt48_m37_id_choice choice=c->options->id_decision(c->options->id_context,c->r->manufacturer,c->r->device,actual->manufacturer,actual->device);
 return choice==LT48_M37_ID_RETRY?LT48_PIC_APP_START_RETRY:choice==LT48_M37_ID_IGNORE?LT48_PIC_APP_START_IGNORE:LT48_PIC_APP_START_ABORT;
}
lt48_status lt48_pic24_execute(const lt48_pic24_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *input,size_t size,unsigned char *read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned type,const lt48_pic24_execution_options *options,lt48_pic24_diagnostics *diagnostics,lt48_pic24_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=(lt48_pic24_result){LT48_INVALID_ARGUMENT,LT48_OK,0,0,UINT32_MAX,0};if(diagnostics)memset(diagnostics,0,sizeof(*diagnostics));
 lt48_status status=lt48_pic24_validate(r,op,start,length,input,size);
 if(status==LT48_OK)status=lt48_pic24_validate_verify_options(r,op,margin,type);
 if(status==LT48_OK && (op==LT48_OPERATION_READ && (!read || read_size<length)))status=LT48_BUFFER_TOO_SMALL;
 unsigned checks=options?options->startup_checks:2;
 if(checks&~UINT32_C(0x80002))status=LT48_INVALID_ARGUMENT;
 if(status!=LT48_OK){out->operation_status=status;return status;}
 unsigned char *image=malloc(length);if(!image){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 if(input&&size==length)memcpy(image,input,length);else memset(image,255,length);
 decision_context decisions={r,options,diagnostics};lt48_pic_source_startup_plan request={0};request.decide=decide;request.context=&decisions;
 if(r->ka||r->klkm||r->dspic){
  lt48_pic24_ka_backend_result completed;
  if(r->dspic){
   lt48_dspic30_backend_result k;
   status=lt48_dspic30_backend_execute_with_margin(r->dspic,api,context,op,image,length,checks,&request,margin,type,&k);
   completed=(lt48_pic24_ka_backend_result){k.status,k.cleanup_status,k.indication_status,
    k.stages_completed,k.configuration_readback_confirmed,k.protection_write_completed,
    k.mismatch_offset,k.expected,k.observed,k.last};
  }else if(r->klkm){
   lt48_pic24_klkm_backend_result k;
   status=lt48_pic24_klkm_backend_execute(r->klkm,api,context,op,image,length,checks,&request,&k);
   completed=(lt48_pic24_ka_backend_result){k.status,k.cleanup_status,k.indication_status,
    k.stages_completed,k.configuration_readback_confirmed,k.protection_write_completed,
    k.mismatch_offset,k.expected,k.observed,k.last};
  }else status=lt48_pic24_ka_backend_execute(r->ka,api,context,op,image,length,checks,&request,&completed);
  out->operation_status=status;out->cleanup_status=completed.cleanup_status;
  out->cleanup_completed=completed.last.cleanup_attempted&&completed.cleanup_status==LT48_OK;
  out->completed_bytes=status==LT48_OK?length:0;
  if(status==LT48_DEVICE_MISMATCH&&completed.mismatch_offset!=UINT32_MAX){out->mismatch_count=1;out->first_mismatch=completed.mismatch_offset;}
  if(diagnostics){diagnostics->configuration_readback_confirmed=completed.configuration_readback_confirmed;diagnostics->protection_write_completed=completed.protection_write_completed;}
  if(status==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,image,length);
  free(image);return status;
 }
 lt48_pic24_backend_result completed;
 status=lt48_pic24_backend_execute_with_margin(r->backend,api,context,op,image,length,checks,&request,margin,type,&completed);
 out->operation_status=status;const lt48_pic_source_operation_result *phases[]={&completed.operation,&completed.program.program,&completed.program.readback,
  &completed.comparison.code,&completed.comparison.configuration,&completed.configuration_before,&completed.configuration_after};
 out->cleanup_status=LT48_OK;
 for(unsigned i=0;i<sizeof(phases)/sizeof(*phases);i++){
  if(phases[i]->cleanup_attempted)out->cleanup_completed=1;
  if(phases[i]->cleanup_status!=LT48_OK){out->cleanup_status=phases[i]->cleanup_status;out->cleanup_completed=0;break;}
 }
 out->completed_bytes=status==LT48_OK?length:completed.operation.traversal.completed_elements;
 if(status==LT48_DEVICE_MISMATCH){
  const lt48_pic24_configuration_mismatch *m=op==LT48_OPERATION_CONFIGURATION?&completed.configuration_mismatch:(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)?&completed.program.mismatch:&completed.comparison.mismatch;
  if(m->differing_bits){out->mismatch_count=1;out->first_mismatch=(uint32_t)length-4*r->configuration_words+4*m->word;}
 }
 if(diagnostics){diagnostics->configuration_readback_confirmed=completed.configuration_readback_confirmed;diagnostics->protection_write_completed=completed.protection_write_completed;}
 if(status==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,image,length);
 free(image);return status;
}
