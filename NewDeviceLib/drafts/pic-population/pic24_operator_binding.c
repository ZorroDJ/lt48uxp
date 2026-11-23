#include "pic24_operator_binding.h"
lt48_status lt48_pic24_operator_binding_make(const lt48_pic24_operator_profile *profile,
 const lt48_pic_source_resources *source,lt48_pic_source_transport *transport,
 const unsigned char *image,size_t size,const lt48_pic_source_startup_plan *request,
 lt48_pic24_operator_binding *out){
 if(!request || !out)return LT48_INVALID_ARGUMENT;
 lt48_pic24_operator_binding staged={0};
 lt48_status status=lt48_pic24_operator_profile_view(profile,source,image,size,
  &staged.geometry,&staged.view,&staged.plan);
 if(status==LT48_OK)status=lt48_pic_applet_startup_make(staged.plan,source,transport,request,&staged.startup);
 if(status!=LT48_OK)return status;
 *out=staged;out->view.geometry=&out->geometry;
 return LT48_OK;
}
