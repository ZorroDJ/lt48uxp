/* Dataman13.10 GAAF M1/M2 MCU owner. Shared transport, original resources;
 * independent range checks and byte readback. No programmer identity/security. */
#include "gaaf_serial.h"
#include <stdlib.h>
#include <string.h>
struct lt48_hitachi_resources { lt48_gaaf_serial_resources *core; };
lt48_status lt48_hitachi_open(const lt48_installation *x,size_t device,lt48_hitachi_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_gaaf_serial_resources *core=NULL;
 lt48_status s=lt48_gaaf_hitachi_open_internal(x,device,&core);if(s==LT48_UNSUPPORTED)s=lt48_fujitsu41_open_internal(x,device,&core);if(s==LT48_UNSUPPORTED)s=lt48_hitachi15_open_internal(x,device,&core);if(s!=LT48_OK)return s;
 lt48_hitachi_resources *r=calloc(1,sizeof(*r));
 if(!r){lt48_gaaf_serial_close(core);return LT48_NO_MEMORY;}
 r->core=core;*out=r;return LT48_OK;
}
void lt48_hitachi_close(lt48_hitachi_resources *r){if(r){lt48_gaaf_serial_close(r->core);free(r);}}
uint32_t lt48_hitachi_capacity(const lt48_hitachi_resources *r){return r?r->core->capacity:0;}
#include "../../NewDeviceLib/sources/fujitsu41/admission.h"
#include "../../NewDeviceLib/sources/hitachi15/admission.h"
int lt48_hitachi_is15_internal(const lt48_hitachi_resources*r){return r&&r->core->hitachi15;}
unsigned lt48_hitachi_is_fujitsu(const lt48_hitachi_resources*r){return r&&r->core->fujitsu41;}
unsigned lt48_hitachi_qualified(const lt48_hitachi_resources*r){return r&&(r->core->hitachi15?h15_admitted(r->core->definition):(!r->core->fujitsu41||(r->core->definition==0x70?LT48_FUJITSU41_STAGE>=1:LT48_FUJITSU41_STAGE>=2)));}
static lt48_status h15_validate(const lt48_hitachi_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
static lt48_status f41_validate(const lt48_hitachi_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t,lt48_verify_margin_selection,unsigned);
lt48_status lt48_hitachi_validate(const lt48_hitachi_resources *r,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t image_size,
 lt48_verify_margin_selection margin,unsigned type) {
 if(r&&r->core->hitachi15)return h15_validate(r,op,start,length,image,image_size,margin,type);
 if(r&&r->core->fujitsu41)return f41_validate(r,op,start,length,image,image_size,margin,type);
 if(!r||!length||start>=r->core->capacity||length>r->core->capacity-start)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_BLANK_CHECK)return LT48_UNSUPPORTED;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||image_size!=length))return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_PROGRAM&&((start&127)||(length&127)))return LT48_INVALID_ARGUMENT;
 if(op==LT48_OPERATION_ERASE&&(start||length!=r->core->capacity))return LT48_INVALID_ARGUMENT;
 if(type>2||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
static lt48_status compare(const unsigned char *actual,const unsigned char *expected,
 uint32_t start,size_t length,lt48_hitachi_result *out) {
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(size_t i=0;i<length;i++)if(actual[i]!=(expected?expected[i]:255)) {
  if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
  out->mismatch_count++;
 }
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
#include "../../NewDeviceLib/sources/fujitsu41/owner.inc"
#include "../../NewDeviceLib/sources/hitachi15/owner.inc"
lt48_status lt48_hitachi_execute(const lt48_hitachi_resources *owner,
 const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,uint32_t start,size_t length,
 const unsigned char *image,size_t image_size,unsigned char *read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned type,lt48_hitachi_result *out) {
 if(owner&&owner->core->hitachi15)return h15_execute(owner,api,ctx,op,start,length,image,image_size,read,read_size,margin,type,out);
 if(owner&&owner->core->fujitsu41)return f41_execute(owner,api,ctx,op,start,length,image,image_size,read,read_size,margin,type,out);
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 lt48_status s=lt48_hitachi_validate(owner,op,start,length,image,image_size,margin,type);
 if(s==LT48_OK&&(!api||!api->write||!api->read))s=LT48_INVALID_ARGUMENT;
 if(s==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))s=LT48_BUFFER_TOO_SMALL;
 if(s!=LT48_OK){out->operation_status=s;return s;}
 const lt48_gaaf_serial_resources *r=owner->core;
 unsigned char *pending=malloc(length),*expected=malloc(length);
 if(!pending||!expected){free(pending);free(expected);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)memcpy(expected,image,length);else memset(expected,255,length);
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);
 lt48_runtime_transport_session *wire=NULL;
 s=lt48_runtime_transport_session_create(api,ctx,&wire);
 if(s==LT48_OK)s=lt48_gaaf_serial_setup(r,&frame,wire);
#define STREAM(mode,voltage,data) lt48_gaaf_hitachi_stream_internal(r,&frame,wire,api,ctx,mode,voltage,data,length,start)
 if(s==LT48_OK&&op==LT48_OPERATION_ERASE)s=lt48_gaaf_serial_erase(r,&frame,wire);
 if(s==LT48_OK)s=STREAM(1,r->nominal,pending);
 if(s==LT48_OK&&op!=LT48_OPERATION_READ)s=compare(pending,op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_ERASE?NULL:expected,start,length,out);
 if(s==LT48_OK&&(op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE)) {
  /* Both original Blank applet and independent Read must agree. */
  unsigned char *blank=op==LT48_OPERATION_PROGRAM?pending:expected;
  s=STREAM(11,r->nominal,blank);
 }
 if(s==LT48_OK&&op==LT48_OPERATION_PROGRAM)s=STREAM(2,r->nominal,expected);
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE;
 if(s==LT48_OK&&(op==LT48_OPERATION_VERIFY||writing)) {
  /* Original 0044F0C0 x87 results for 330/500 cV, selectors 0/1/2. */
  unsigned delta=type==2?0:r->nominal/(type?10:20);
  unsigned volts[2]={r->nominal-delta,r->nominal+delta},n=type==2?1:2;
  if(!writing) {
   if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
   else if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
   else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
  }
  for(unsigned i=0;s==LT48_OK&&i<n;i++)s=STREAM(4,volts[i],expected);
  if(s==LT48_OK)s=STREAM(1,r->nominal,pending);
  if(s==LT48_OK)s=compare(pending,expected,start,length,out);
 }
 out->operation_status=s;lt48_runtime_transport_session_close(wire);wire=NULL;
 out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,s);
 if(s==LT48_OK&&out->cleanup_status==LT48_OK&&writing) {
  /* Fresh power/read catches failures hidden by the programming state. */
  lt48_gaaf_serial_frame_init(r,&frame);
  s=lt48_runtime_transport_session_create(api,ctx,&wire);
  if(s==LT48_OK)s=lt48_gaaf_serial_setup(r,&frame,wire);
  if(s==LT48_OK)s=STREAM(1,r->nominal,pending);
  if(s==LT48_OK)s=compare(pending,expected,start,length,out);
  out->operation_status=s;lt48_runtime_transport_session_close(wire);
  out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,s);
 }
 out->cleanup_completed=out->cleanup_status==LT48_OK;
 if(s==LT48_OK)s=out->cleanup_status;
 if(s==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,pending,length);}
 free(expected);free(pending);return s;
#undef STREAM
}
