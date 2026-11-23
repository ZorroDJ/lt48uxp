#ifndef LT48_PIC18C_ADAPTER_H
#define LT48_PIC18C_ADAPTER_H
static lt48_status pic18c_open(const lt48_installation *x,const lt48_device_spec *spec,const o16_profile *p,lt48_pic_resources **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!x||!spec||pic18c_match(spec)!=p)return LT48_SAFETY_REJECTED;
 lt48_pic_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->pic18c=p;r->capacity=p->backing_bytes;r->program_words=p->code_words;
 char path[4096];int n=snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x));
 if(n<0||(size_t)n>=sizeof path){lt48_pic_close(r);return LT48_INVALID_ARGUMENT;}
 r->otp_path=malloc((size_t)n+1);unsigned char *blank=malloc(r->capacity);
 if(!r->otp_path||!blank){free(blank);lt48_pic_close(r);return LT48_NO_MEMORY;}
 memcpy(r->otp_path,path,(size_t)n+1);memset(blank,255,r->capacity);o16_owner *probe=NULL;
 lt48_status st=o16_open(path,p,blank,r->capacity,&probe);
 /* These thirteen callbacks are compact-only. The original host leaves
  * the verified common idle image active; no device-specific FPGA/applet. */
 if(st==LT48_OK)for(unsigned i=0;i<p->operation_count;i++)if(p->types[i])st=LT48_SAFETY_REJECTED;
 o16_close(probe);free(blank);
 if(st==LT48_OK)st=lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size);
 if(st!=LT48_OK){lt48_pic_close(r);return st;}*out=r;return LT48_OK;
}
static lt48_status pic18c_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,unsigned type,lt48_pic_result *out){
 unsigned char *initial=malloc(r->capacity);if(!initial)return LT48_NO_MEMORY;
 if(image&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))memcpy(initial,image,r->capacity);else memset(initial,255,r->capacity);
 o16_owner *owner=NULL;lt48_pic_source_transport *wire=NULL;
 lt48_status st=o16_open(r->otp_path,r->pic18c,initial,r->capacity,&owner);free(initial);
 const lt48_pic_source_host_binding binding={0,0,1};
 if(st==LT48_OK)st=lt48_pic_source_transport_create_internal(api,context,owner->session,&binding,500,420,550,&wire);
 if(st==LT48_OK)st=lt48_pic_source_transport_idle_internal(wire,r->idle,r->idle_size);
 if(st==LT48_OK)st=lt48_pic_source_transport_prepare_startup_internal(wire,r->idle,r->idle_size,NULL,0);
 lt48_pic_contact_masks masks;lt48_pic_contact_result contact;
 if(st==LT48_OK)st=lt48_pic_source_resources_contact_masks_internal(owner->resources,&masks);
 if(st==LT48_OK)st=lt48_pic_source_transport_require_contact_internal(wire,&masks,&contact);
 unsigned levels[4],count=0;uint32_t values[4]={420,550,0,0},source[4];
 if(st==LT48_OK)st=lt48_pic_source_verify_voltages(500,values,3,type,margin==LT48_VERIFY_MARGIN_NOMINAL,source,&count);
 if(st==LT48_OK){for(unsigned i=0;i<count;i++)levels[i]=source[i];if(margin==LT48_VERIFY_MARGIN_LOW)count=1;if(margin==LT48_VERIFY_MARGIN_HIGH){levels[0]=levels[count-1];count=1;}}
 lt48_pic_source_operation_result result={LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
 if(st==LT48_OK)st=o16_execute_transport(wire,owner,op,levels,count,op==LT48_OPERATION_PROGRAM,0,&result);
 out->operation_status=st;out->cleanup_status=result.cleanup_status;out->cleanup_completed=result.cleanup_attempted&&result.cleanup_status==LT48_OK;
 out->first_mismatch=result.traversal.first_mismatch==UINT32_MAX?UINT32_MAX:2*result.traversal.first_mismatch;out->mismatch_count=result.traversal.first_mismatch!=UINT32_MAX;
 if(st==LT48_OK&&op==LT48_OPERATION_READ){const unsigned char *data;size_t size;st=lt48_pic_source_session_image_internal(owner->session,&data,&size);if(st==LT48_OK&&size==r->capacity)memcpy(read,data,size);else if(st==LT48_OK)st=LT48_SAFETY_REJECTED;}
 if(wire&&!result.cleanup_attempted){out->cleanup_status=lt48_pic_source_transport_cleanup_status_internal(wire);out->cleanup_completed=out->cleanup_status==LT48_OK;}
 out->operation_status=st;if(st==LT48_OK)out->completed_bytes=r->capacity;
 lt48_pic_source_transport_close_internal(wire);o16_close(owner);return st;
}
#endif
