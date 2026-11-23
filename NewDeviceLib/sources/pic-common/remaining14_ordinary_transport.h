#ifndef LT48_PIC_REMAINING14_ORDINARY_TRANSPORT_H
#define LT48_PIC_REMAINING14_ORDINARY_TRANSPORT_H
#include "remaining14_ordinary_source.h"
#include "remaining14_modern_transport.h"
/* Same production transport/operation engine, a different exact policy/resource
 * constructor. Both validate before hardware preparation; no device admission. */
static inline lt48_status rm14_ordinary_transport_execute(const char *path,const lt48_device_spec *spec,
 const rm14_profile *p,const unsigned char *idle,size_t idle_size,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const uint16_t *image,uint16_t *output,uint16_t *scratch,otp14_operator_result *result) {
 return rm14_modern_transport_execute_bound(path,spec,p,rm14_ordinary_source_open,idle,idle_size,api,context,op,image,output,scratch,result);
}
#endif
