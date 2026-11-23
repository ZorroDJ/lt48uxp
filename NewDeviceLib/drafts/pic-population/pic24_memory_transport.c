#include "pic24_memory_transport.h"
lt48_status lt48_pic24_memory_transport(const lt48_pic_applet_plan_profile *profile,
 const lt48_pic_source_resources *source,lt48_pic_source_transport *transport,
 const lt48_pic24_program_view *view,uint32_t checks,const lt48_pic_source_startup_plan *startup,
 uint16_t mode,unsigned char *image,size_t size,lt48_pic_source_operation_result *result){
 if(!result)return LT48_INVALID_ARGUMENT;
 *result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
 if(!profile || !image || size!=profile->element_count)return LT48_INVALID_ARGUMENT;
 if(mode!=1 && mode!=3 && mode!=4){result->operation_status=LT48_UNSUPPORTED;return LT48_UNSUPPORTED;}
 lt48_pic_source_applet_stream_plan plan;
 lt48_status status=lt48_pic_applet_plan_make(profile,mode,0,profile->element_count,&plan);
 if(status==LT48_OK)status=lt48_pic24_prepare_startup(profile,source,transport,view,checks,startup);
 if(status!=LT48_OK){result->operation_status=status;return status;}
 return lt48_pic_source_transport_applet_stream_run_internal(transport,&plan,image,size,result);
}
