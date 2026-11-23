#include "pic24_erase_transport.h"
lt48_status lt48_pic24_erase_transport(const lt48_pic_applet_plan_profile *profile,
 const lt48_pic_source_resources *source,lt48_pic_source_transport *transport,
 const lt48_pic24_program_view *view,uint32_t checks,
 const lt48_pic_source_startup_plan *startup,lt48_pic_source_operation_result *result){
 if(!result)return LT48_INVALID_ARGUMENT;
 *result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
 lt48_status status=lt48_pic24_prepare_startup(profile,source,transport,view,checks,startup);
 if(status!=LT48_OK){result->operation_status=status;return status;}
 return lt48_pic_applet_erase_run(profile,source,transport,result);
}
