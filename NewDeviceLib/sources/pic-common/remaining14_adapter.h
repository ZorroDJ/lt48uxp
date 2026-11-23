#ifndef LT48_PIC_REMAINING14_ADAPTER_H
#define LT48_PIC_REMAINING14_ADAPTER_H
/* Included by pic.c after its private resource/word helpers. Public admission
 * is independent of these complete candidate constructors and operators. */
static lt48_status pic_remaining14_open(const lt48_installation *x,const lt48_device_spec *spec,
 const rm14_profile *p,lt48_pic_resources **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!x||!spec||!p)return LT48_INVALID_ARGUMENT;
 if(p->has_erase){
  for(unsigned i=0;i<sizeof rm14_legacy_profiles/sizeof *rm14_legacy_profiles;i++){
   const lt48_pic_profile *candidate=rm14_legacy_profiles+i;
   if(candidate->definition==p->definition&&candidate->family==p->family&&candidate->pinmap==p->pinmap)
    return pic_open_profile(x,spec,candidate,out);
  }
  return LT48_SAFETY_REJECTED;
 }
 lt48_pic_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
 r->remaining14=p;r->capacity=p->backing_bytes;r->program_words=p->code_words;
 char path[4096];int n=snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x));
 if(n<0||(size_t)n>=sizeof path){lt48_pic_close(r);return LT48_INVALID_ARGUMENT;}
 r->otp_path=malloc((size_t)n+1);if(!r->otp_path){lt48_pic_close(r);return LT48_NO_MEMORY;}memcpy(r->otp_path,path,(size_t)n+1);
 unsigned char initial[16400];for(unsigned a=0;a<sizeof initial;a+=2){initial[a]=255;initial[a+1]=63;}
 lt48_device_spec original=*spec;original.capacity_bytes=p->source_bytes;
 rm14_resources *probe=NULL;lt48_status st=rm14_open(path,&original,p,initial,sizeof initial,&probe);
 rm14_close(probe);rm14_otp_policy policy;if(st==LT48_OK)st=rm14_public_policy(p,&policy);
 if(st==LT48_OK)st=lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size);
 if(st!=LT48_OK){lt48_pic_close(r);return st;}*out=r;return LT48_OK;
}
static lt48_status pic_remaining14_validate(const lt48_pic_resources *r,lt48_operation_kind op,
 const unsigned char *image,size_t size){
 if(op==LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
 if(op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_OVER_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE)return LT48_OK;
 if(!image||size!=r->capacity)return LT48_INVALID_ARGUMENT;
 rm14_otp_policy policy;lt48_status st=rm14_public_policy(r->remaining14,&policy);if(st!=LT48_OK)return st;
 for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++){
  unsigned value=word(image+2*a);if(value>0x3fff&&value!=0xffff)return LT48_SAFETY_REJECTED;
  if(!otp14_operator_mask(&policy.common,a)&&(value&0x3fff)!=0x3fff)return LT48_SAFETY_REJECTED;
 }
 unsigned configuration=word(image+0x400e)&0x3fff;
 if(!rm14_otp_target_valid(&policy,configuration))return LT48_SAFETY_REJECTED;
 if(r->remaining14->family==44&&!rm14_pic14000_configuration_valid(configuration))return LT48_SAFETY_REJECTED;
 return LT48_OK;
}
static lt48_status pic_remaining14_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,lt48_pic_result *out){
 size_t bytes=OTP14_IMAGE_WORDS*sizeof(uint16_t);
 int wants=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE;
 uint16_t *target=wants?malloc(bytes):NULL,*scratch=malloc(bytes),*pending=malloc(bytes);
 if((wants&&!target)||!scratch||!pending){free(target);free(scratch);free(pending);return LT48_NO_MEMORY;}
 if(wants)for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)target[a]=(uint16_t)(word(image+2*a)&0x3fff);
 lt48_device_spec spec={0};const rm14_profile *p=r->remaining14;
 spec.catalogue.device_class=2015;spec.catalogue.vendor="Microchip";spec.catalogue.device_name=(const unsigned char*)p->name;
 spec.catalogue.device_name_length=strlen(p->name);spec.definition_stem="GADC___";spec.definition_resource=p->definition;
 spec.family_code=p->family;spec.pinmap=p->pinmap;spec.element_bits=14;spec.capacity_bytes=p->source_bytes;
 otp14_operator_result result={0};result.first_mismatch=UINT32_MAX;lt48_status st;
 if(rm14_public_parity(p))st=rm14_parity_transport_execute_options(r->otp_path,&spec,p,r->idle,r->idle_size,
  api,context,op,target,pending,scratch,&result,(unsigned)margin+1);
 else{
  rm14_otp_source_factory factory=rm14_modern_source_open;rm14_otp_source_executor execute=rm14_modern_source_execute;
  rm14_otp_policy policy;
  if(p->family==44){factory=rm14_pic14000_source_open;execute=rm14_pic14000_source_execute;}
  else if(rm14_ordinary_policy(p,&policy)==LT48_OK)factory=rm14_ordinary_source_open;
  st=rm14_modern_transport_execute_owned_options(r->otp_path,&spec,p,factory,execute,r->idle,r->idle_size,
   api,context,op,target,pending,scratch,&result,(unsigned)margin+1);
 }
 out->operation_status=result.operation_status;out->cleanup_status=result.cleanup.status;out->cleanup_completed=result.cleanup.completed;
 out->mismatch_count=result.mismatches;out->first_mismatch=result.first_mismatch;
 if(st==LT48_OK){
  if(op==LT48_OPERATION_READ)for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)put_word(read+2*a,pending[a]);
  out->completed_bytes=r->capacity;
  if(api->progress)api->progress(context,LT48_EXECUTION_READ_OPERATION,r->capacity,r->capacity,0,0,0);
 }
 free(target);free(scratch);free(pending);return st;
}
#endif
