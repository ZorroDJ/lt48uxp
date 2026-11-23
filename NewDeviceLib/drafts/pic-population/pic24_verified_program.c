#include "pic24_verified_program.h"
#include <stdlib.h>
#include <string.h>
static lt48_status indicate(const lt48_pic24_verified_program_plan *p,lt48_status outcome){
 lt48_protocol_transport api=*p->indicator_transport;api.cancelled=NULL;
 lt48_runtime_transport_session *wire=NULL;
 lt48_status status=lt48_runtime_transport_session_create(&api,p->indicator_context,&wire);
 unsigned char command[]={0x40,outcome==LT48_OK?4:2};
 if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(wire,command,sizeof(command));
 if(status==LT48_OK)status=lt48_runtime_transport_session_flush(wire);
 lt48_runtime_transport_session_close(wire);return status;
}
lt48_status lt48_pic24_verified_program(const lt48_pic24_verified_program_plan *p,
 unsigned char *image,size_t size,lt48_pic24_verified_program_result *result){
 if(!result)return LT48_INVALID_ARGUMENT;
 memset(result,0,sizeof(*result));result->operation_status=LT48_INVALID_ARGUMENT;
 if(!p || !p->profile || !p->source || !p->program_transport || !p->read_transport ||
    !p->view || !p->view->geometry || !p->startup || !image || size!=p->profile->element_count ||
    !p->indicator_transport || !p->indicator_transport->write || !p->indicator_transport->read)return LT48_INVALID_ARGUMENT;
 if((p->flags&56)!=56 || (p->flags&~UINT32_C(0xc103f))){result->operation_status=LT48_UNSUPPORTED;return LT48_UNSUPPORTED;}
 unsigned count=p->view->configuration_words;
 uint32_t first=p->view->geometry->configuration_first_image_byte;
 if((count!=2 && count!=4) || (uint64_t)first+4*count!=size)return LT48_INVALID_ARGUMENT;
 unsigned char *staged=malloc(size);if(!staged){result->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 memcpy(staged,image,size);
 lt48_status status=lt48_pic24_program_transport(p->profile,p->source,p->program_transport,p->view,p->flags,
  p->startup,p->erase_choice,p->policy_context,staged,size,&result->program);
 if(status==LT48_OK){
  uint16_t observed[4];result->readback_attempted=1;
  status=lt48_pic24_configuration_read_transport(p->profile,p->source,p->read_transport,p->view,
   p->flags&UINT32_C(0x80003),p->startup,observed,count,&result->readback);
  if(status==LT48_OK){
   uint32_t expected[4];for(unsigned i=0;i<count;i++)expected[i]=staged[first+4*i]|((uint32_t)staged[first+4*i+1]<<8);
   status=lt48_pic24_configuration_compare(p->view->mandatory_rule_family,expected,observed,count,&result->mismatch);
  }
 }
 result->operation_status=status;result->indication_status=indicate(p,status);
 if(status==LT48_OK)status=result->indication_status;
 if(status==LT48_OK)memcpy(image,staged,size);
 free(staged);return status;
}
