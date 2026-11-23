#ifndef LT48_PIC_REMAINING14_PIC14000_TRANSPORT_H
#define LT48_PIC_REMAINING14_PIC14000_TRANSPORT_H
#include "remaining14_pic14000_source.h"
#include "remaining14_modern_transport.h"
static inline lt48_status rm14_pic14000_transport_execute(const char *path,const lt48_device_spec *spec,
 const rm14_profile *p,const unsigned char *idle,size_t idle_size,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result){
 return rm14_modern_transport_execute_owned(path,spec,p,rm14_pic14000_source_open,rm14_pic14000_source_execute,
  idle,idle_size,api,context,op,image,output,scratch,result);
}
#endif
