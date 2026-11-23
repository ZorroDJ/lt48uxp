#include "pic24_configuration_transport.h"
#include <stdlib.h>
#include <string.h>
lt48_status lt48_pic24_configuration_read_transport(const lt48_pic_applet_plan_profile *profile,
 const lt48_pic_source_resources *source,lt48_pic_source_transport *transport,
 const lt48_pic24_program_view *view,uint32_t checks,const lt48_pic_source_startup_plan *startup,
 uint16_t *words,unsigned count,lt48_pic_source_operation_result *result){
 if(!result)return LT48_INVALID_ARGUMENT;
 *result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
 if(!profile || !view || !view->geometry || !words || count!=view->configuration_words ||
    (count!=2 && count!=4) || view->image_size!=profile->element_count)return LT48_INVALID_ARGUMENT;
 uint32_t first=view->geometry->configuration_first_image_byte;
 if((uint64_t)first+4*count!=view->image_size){result->operation_status=LT48_SAFETY_REJECTED;return LT48_SAFETY_REJECTED;}
 unsigned char *readback=calloc(view->image_size,1);
 if(!readback){result->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 lt48_status status=lt48_pic24_memory_transport(profile,source,transport,view,checks,startup,
  1,readback,view->image_size,result);
 if(status==LT48_OK){
  uint16_t staged[4];
  for(unsigned i=0;i<count;i++)staged[i]=(uint16_t)(readback[first+4*i]|((uint16_t)readback[first+4*i+1]<<8));
  memcpy(words,staged,count*sizeof(*words));
 }
 free(readback);return status;
}
