/* Original class-2002 byte-stream 28F owner. The shared bridge supplies
 * transport framing; the two-pass erase and operation order belong here. */
#include "gaaf_serial.h"
#include <stdlib.h>
#include <string.h>

static lt48_status compare(const unsigned char *actual,const unsigned char *expected,
 uint32_t start,size_t length,int blank,lt48_gaaf_serial_result *out) {
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(size_t i=0;i<length;i++)if(actual[i]!=(blank?255:expected[i])) {
  if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
  out->mismatch_count++;
 }
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}

lt48_status lt48_legacy28f_execute_internal(const lt48_gaaf_serial_resources *r,
 const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,uint32_t start,size_t length,
 const unsigned char *image,size_t image_size,unsigned char *read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,
 void *decision_ctx,lt48_gaaf_serial_result *out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 lt48_status status=lt48_gaaf_serial_validate(r,op,start,length,image,image_size,margin,margin_type);
 if(status==LT48_OK && (!r->legacy28f||op>LT48_OPERATION_ERASE))status=LT48_UNSUPPORTED;
 if(status==LT48_OK && (!api||!api->write||!api->read))status=LT48_INVALID_ARGUMENT;
 if(status==LT48_OK && op==LT48_OPERATION_ERASE && (start||length!=r->capacity))status=LT48_SAFETY_REJECTED;
 if(status==LT48_OK && op==LT48_OPERATION_READ && (!read||read_size<length))status=LT48_BUFFER_TOO_SMALL;
 if(status!=LT48_OK){out->operation_status=status;return status;}
 unsigned char *pending=malloc(length),*expected=malloc(length);
 if(!pending||!expected){free(pending);free(expected);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 int programming=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 int writing=programming||op==LT48_OPERATION_ERASE;
 if(image && op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_ERASE)memcpy(expected,image,length);
 else memset(expected,255,length);
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);
 lt48_runtime_transport_session *wire=NULL;
 status=lt48_runtime_transport_session_create(api,ctx,&wire);
 if(status==LT48_OK)status=lt48_gaaf_serial_setup(r,&frame,wire);
 if(status==LT48_OK)for(unsigned attempt=0;attempt<64;attempt++) {
  uint16_t man=0,dev=0;status=lt48_gaaf_serial_read_id(r,&frame,wire,&man,&dev);
  if(status!=LT48_OK|| (man==r->manufacturer && dev==r->device))break;
  /* Power down before an operator may reseat a mismatching device. */
  lt48_runtime_transport_session_close(wire);wire=NULL;
  status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,LT48_DEVICE_MISMATCH);
  if(status!=LT48_OK)break;
  lt48_m37_id_choice choice=decide?decide(decision_ctx,r->manufacturer,r->device,man,dev):LT48_M37_ID_ABORT;
  if(choice!=LT48_M37_ID_RETRY&&choice!=LT48_M37_ID_IGNORE){status=LT48_DEVICE_MISMATCH;break;}
  if(attempt==63&&choice==LT48_M37_ID_RETRY){status=LT48_SAFETY_REJECTED;break;}
  lt48_gaaf_serial_frame_init(r,&frame);
  status=lt48_runtime_transport_session_create(api,ctx,&wire);
  if(status==LT48_OK)status=lt48_gaaf_serial_setup(r,&frame,wire);
  if(status!=LT48_OK||choice==LT48_M37_ID_IGNORE)break;
 }
#define STREAM(mode,volts,bytes) lt48_legacy28f_stream_internal(r,&frame,wire,api,ctx,mode,volts,bytes,length,start)
 if(status==LT48_OK && op==LT48_OPERATION_ERASE) {
  /* Original 00451510: stop if mode-6 pre-erasing fails. The next pass
   * stages FF; it is not interchangeable with an ordinary Program pass. */
  memset(pending,0,length);status=STREAM(6,r->nominal,pending);
  if(status==LT48_OK){memset(pending,255,length);status=STREAM(7,r->nominal,pending);}
 }
 if(status==LT48_OK)status=STREAM(1,r->nominal,pending);
 if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_ERASE))
  status=compare(pending,NULL,start,length,1,out);
 if(status==LT48_OK && op==LT48_OPERATION_OVER_PROGRAM)for(size_t i=0;i<length;i++)
  if((pending[i]&expected[i])!=expected[i]){out->first_mismatch=start+(uint32_t)i;out->mismatch_count=1;status=LT48_DEVICE_MISMATCH;break;}
 if(status==LT48_OK && (op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))status=compare(pending,expected,start,length,0,out);
 if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK)) {
  memset(pending,255,length);status=STREAM(11,r->nominal,pending);
 }
 if(status==LT48_OK && programming)status=STREAM(2,r->nominal,expected);
 if(status==LT48_OK && (writing||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)) {
  unsigned delta=margin_type==2?0:r->nominal/(margin_type?10:20);
  unsigned volts[2]={r->nominal-delta,r->nominal+delta},n=margin_type==2?1:2;
  if(!writing){
   if(op==LT48_OPERATION_COMPARE||margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
   else if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
   else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
  }
  for(unsigned i=0;status==LT48_OK&&i<n;i++)status=STREAM(4,volts[i],expected);
  if(status==LT48_OK)status=STREAM(1,r->nominal,pending);
  if(status==LT48_OK) {
   status=compare(pending,expected,start,length,0,out);
   if(status!=LT48_OK) {
    out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,status);
    out->cleanup_completed=out->cleanup_status==LT48_OK;
   }
  }
 }
 out->operation_status=status;lt48_runtime_transport_session_close(wire);
 out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,status);
 out->cleanup_completed=out->cleanup_status==LT48_OK;
 if(status==LT48_OK)status=out->cleanup_status;
 if(status==LT48_OK && writing) {
  lt48_gaaf_serial_result fresh;
  status=lt48_legacy28f_execute_internal(r,api,ctx,LT48_OPERATION_READ,start,length,NULL,0,pending,length,
   LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_ctx,&fresh);
  out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
  if(status==LT48_OK) {
   status=compare(pending,expected,start,length,0,out);
   if(status!=LT48_OK) {
    out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,status);
    out->cleanup_completed=out->cleanup_status==LT48_OK;
   }
  }
 }
 out->operation_status=status;
 if(status==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,pending,length);}
 free(pending);free(expected);return status;
#undef STREAM
}
