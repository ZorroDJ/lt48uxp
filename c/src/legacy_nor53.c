/* Private legacy class-2007 NOR owner. Source compact erase and per-mode
 * applet loading are separate from the older class-2002 two-pass erase. */
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

#include "../../NewDeviceLib/sources/mx26aone/completion.h"
#include "../../NewDeviceLib/sources/combined36/owner.inc"
#include "../../NewDeviceLib/sources/sst38one/erase-ack.h"
#include "../../NewDeviceLib/sources/legacy-nor/st_m28f201_preerase.h"
#include "../../NewDeviceLib/sources/legacy-nor/fujitsu_002_protection_guard.h"
#include "../../NewDeviceLib/sources/legacy-nor/atmel_008_protection_guard.h"
#include "../../NewDeviceLib/sources/legacy-nor/sharp_s3_protection_guard.h"
#include "../../NewDeviceLib/sources/nor3/protection-guard.h"
#include "../../NewDeviceLib/sources/efst1/protection-guard.h"

#include "../../NewDeviceLib/sources/word14/protection-guard.h"
#include "../../NewDeviceLib/sources/sharpwordseven/protection-guard.h"
static lt48_status word14_maskrom_execute(const lt48_gaaf_serial_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size,unsigned char*out,size_t os,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_gaaf_serial_result*z){
 (void)out;(void)os;
 lt48_status s=lt48_gaaf_serial_validate(r,op,start,n,image,size,margin,type);if(s!=LT48_OK){memset(z,0,sizeof*z);z->operation_status=s;return s;}
 unsigned char*actual=malloc(n);if(!actual)return LT48_NO_MEMORY;
 unsigned delta=type==2?0:r->nominal/(type?10:20),volts[2]={r->nominal-delta,r->nominal+delta},count=type==2?1:2;
 if(op!=LT48_OPERATION_VERIFY||margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;count=1;}
 else if(margin==LT48_VERIFY_MARGIN_LOW)count=1;
 else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];count=1;}
 for(unsigned k=0;s==LT48_OK&&k<count;k++){
  lt48_gaaf_serial_resources adjusted=*r;adjusted.nominal=volts[k];
  for(size_t v=0;v<adjusted.variable_count;v++)if(adjusted.variables[v].variable_id==0)adjusted.variables[v].value=(uint16_t)volts[k];
  s=lt48_legacy_nor53_execute_internal(&adjusted,api,ctx,LT48_OPERATION_READ,start,n,NULL,0,actual,n,LT48_VERIFY_MARGIN_NOMINAL,type,decide,dc,z);
  if(s==LT48_OK){s=compare(actual,image,start,n,op==LT48_OPERATION_BLANK_CHECK,z);z->operation_status=s;}
 }
 if(s==LT48_DEVICE_MISMATCH){lt48_gaaf_serial_frame f;lt48_gaaf_serial_frame_init(r,&f);z->cleanup_status=lt48_gaaf_serial_cleanup(r,&f,api,ctx,s);z->cleanup_completed=z->cleanup_status==LT48_OK;}
 free(actual);return s;
}
/* Private page-flash verification uses independently read bytes. This avoids
 * treating a controller completion response as an array comparison result. */
static lt48_status page_stream(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 const lt48_protocol_transport *api,void *ctx,unsigned mode,unsigned volts,
 unsigned char *bytes,size_t length,uint32_t start) {
 if(mode==1||mode==2)return lt48_legacy_macronix6_stream_internal(r,frame,wire,api,ctx,mode,volts,bytes,length,start);
 if(mode!=4&&mode!=11)return LT48_UNSUPPORTED;
 unsigned char *actual=malloc(length);if(!actual)return LT48_NO_MEMORY;
 lt48_status s=lt48_legacy_macronix6_stream_internal(r,frame,wire,api,ctx,1,volts,actual,length,start);
 if(s==LT48_OK)for(size_t i=0;i<length;i++)if(actual[i]!=(mode==11?255:bytes[i])){s=LT48_DEVICE_MISMATCH;break;}
 free(actual);return s;
}

typedef struct {
 lt48_runtime_configuration_value saved[32];size_t count;int armed;
} page_protection_guard;
static lt48_status page_protection_begin(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,page_protection_guard *g) {
 lt48_status s=lt48_gaaf_serial_wave(r,frame,wire,16);if(s!=LT48_OK)return s;
 unsigned seen=0;int protected=0;
 for(size_t i=0;i<frame->configuration_count;i++) {
  const lt48_runtime_configuration_value *v=&frame->configuration[i];
  if(v->object_id||v->category||v->field>=32||v->value>1||(seen&(1u<<v->field)))return LT48_RUNTIME_ERROR;
  seen|=1u<<v->field;protected|=v->value;
 }
 if(seen!=r->legacy_config_mask)return LT48_RUNTIME_ERROR;
 g->count=frame->configuration_count;memcpy(g->saved,frame->configuration,g->count*sizeof(*g->saved));
 /* Arm before any INIT that might partially unprotect before failing. */
 g->armed=protected;return LT48_OK;
}
static lt48_status page_protection_restore(const lt48_gaaf_serial_resources *r,
 const lt48_protocol_transport *api,void *ctx,page_protection_guard *g) {
 if(!g->armed)return LT48_OK;
 lt48_protocol_transport recovery=*api;recovery.cancelled=NULL;
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);
 lt48_runtime_transport_session *wire=NULL;lt48_status s=lt48_runtime_transport_session_create(&recovery,ctx,&wire);
 if(s==LT48_OK)s=lt48_gaaf_serial_setup(r,&frame,wire);
 frame.mode=8;
 if(s==LT48_OK)s=lt48_runtime_frame_set_mode(frame.buffers,r->frame_profile.buffer_count,r->frame_profile.reset_mask,r->frame_profile.mode_buffer,8,0,0);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,&frame,wire,3);
 if(s==LT48_OK){frame.configuration_count=g->count;memcpy(frame.configuration,g->saved,g->count*sizeof(*g->saved));s=lt48_gaaf_serial_wave(r,&frame,wire,14);}
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,&frame,wire,LT48_GAAF_OPERATIONS);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,&frame,wire,r->reset_op);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,&frame,wire,16);
 if(s==LT48_OK) {
  if(frame.configuration_count!=g->count)s=LT48_DEVICE_MISMATCH;
  for(size_t i=0;s==LT48_OK&&i<g->count;i++) {
   size_t j;for(j=0;j<frame.configuration_count;j++)if(frame.configuration[j].field==g->saved[i].field)break;
   if(j==frame.configuration_count||frame.configuration[j].value!=g->saved[i].value)s=LT48_DEVICE_MISMATCH;
  }
 }
 lt48_runtime_transport_session_close(wire);
 lt48_status cleanup=lt48_gaaf_serial_cleanup(r,&frame,&recovery,ctx,s);
 if(s==LT48_OK)s=cleanup;
 if(s==LT48_OK)g->armed=0;
 return s;
}

#include "../../NewDeviceLib/sources/pss2005one/protection-guard.h"
#include "../../NewDeviceLib/sources/x88one/owner.inc"

lt48_status lt48_legacy_nor53_execute_internal(const lt48_gaaf_serial_resources *r,
 const lt48_protocol_transport *api,void *ctx,lt48_operation_kind op,uint32_t start,size_t length,
 const unsigned char *image,size_t image_size,unsigned char *read,size_t read_size,
 lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,
 void *decision_ctx,lt48_gaaf_serial_result *out) {
 if(r&&r->x88one)return x88one_execute(r,api,ctx,op,start,length,image,image_size,read,read_size,margin,margin_type,out);
 if(r&&r->word14&&(r->utron4||r->definition==0x39)&&op!=LT48_OPERATION_READ){if(!out)return LT48_INVALID_ARGUMENT;return word14_maskrom_execute(r,api,ctx,op,start,length,image,image_size,read,read_size,margin,margin_type,decide,decision_ctx,out);}
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
 lt48_status status=lt48_gaaf_serial_validate(r,op,start,length,image,image_size,margin,margin_type);
 if(status==LT48_OK && (!r->legacy_nor||(op>LT48_OPERATION_ERASE&&!(r->sm2964two&&op==LT48_OPERATION_SECURE))))status=LT48_UNSUPPORTED;
 if(status==LT48_OK && (!api||!api->write||!api->read))status=LT48_INVALID_ARGUMENT;
 if(status==LT48_OK && op==LT48_OPERATION_ERASE && (start||length!=r->capacity))status=LT48_SAFETY_REJECTED;
 if(status==LT48_OK && op==LT48_OPERATION_READ && (!read||read_size<length))status=LT48_BUFFER_TOO_SMALL;
 if(status!=LT48_OK){out->operation_status=status;return status;}
 /* Page flashes consume complete source-sized pages. Expand the transport range,
  * retaining FF outside the requested range. The recursive owner checks the
  * WHOLE expanded range is blank before any write, so existing neighboring
  * data is never silently overwritten or reprogrammed. */
 if(r->legacy_page&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)&&((start&(r->page_bytes-1))||(length&(r->page_bytes-1)))) {
  uint32_t base=start&~(r->page_bytes-1);size_t extent=((size_t)start+length+r->page_bytes-1)&~(size_t)(r->page_bytes-1);
  if(extent>r->capacity){out->operation_status=LT48_SAFETY_REJECTED;return LT48_SAFETY_REJECTED;}
  size_t span=extent-base;unsigned char *pages=malloc(span);
  if(!pages){out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
  memset(pages,255,span);memcpy(pages+start-base,image,length);
  status=lt48_legacy_nor53_execute_internal(r,api,ctx,op,base,span,pages,span,NULL,0,margin,margin_type,decide,decision_ctx,out);
  if(status==LT48_OK)out->completed_bytes=length;
  free(pages);return status;
 }
 mx26aone_monitor mx26a={.api=api,.ctx=ctx};
 lt48_protocol_transport mx26a_api={.write=mx26aone_write,.read=mx26aone_read,.cancelled=mx26aone_cancel};
 if(r->mx26aone){api=&mx26a_api;ctx=&mx26a;}
 sst38_monitor sst38={.api=api,.ctx=ctx};lt48_protocol_transport sst38_api={.write=sst38_write,.read=sst38_read,.cancelled=sst38_cancel};
 if(r->sst38one&&op==LT48_OPERATION_ERASE){api=&sst38_api;ctx=&sst38;}
 unsigned char *pending=malloc(length),*expected=malloc(length);
 if(!pending||!expected){free(pending);free(expected);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 int programming=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
 int writing=programming||op==LT48_OPERATION_ERASE;
 if(image && op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_ERASE)memcpy(expected,image,length);
 else memset(expected,255,length);
 page_protection_guard protection={0};
 lt48_gaaf_serial_frame frame;lt48_gaaf_serial_frame_init(r,&frame);
 lt48_runtime_transport_session *wire=NULL;
 status=lt48_runtime_transport_session_create(api,ctx,&wire);
 if(status==LT48_OK)status=lt48_gaaf_serial_setup(r,&frame,wire);
 if(status==LT48_OK&&!r->pss2005one&&!r->sm2964two&&!r->utron4&&(!r->word14||r->waves[2]))for(unsigned attempt=0;attempt<64;attempt++) {
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
 if(status==LT48_OK&&r->sm2964two&&op==LT48_OPERATION_SECURE){
  frame.mode=8;
  status=lt48_runtime_frame_set_mode(frame.buffers,r->frame_profile.buffer_count,r->frame_profile.reset_mask,r->frame_profile.mode_buffer,8,0,0);
  if(!status)status=lt48_gaaf_serial_wave(r,&frame,wire,3);
  if(!status)status=lt48_gaaf_serial_wave(r,&frame,wire,6);
  lt48_runtime_transport_session_close(wire);
  out->operation_status=status;out->cleanup_status=lt48_gaaf_serial_cleanup(r,&frame,api,ctx,status);out->cleanup_completed=!out->cleanup_status;
  if(!status)status=out->cleanup_status;
  if(!status)out->completed_bytes=length;
  free(pending);free(expected);return status;
 }
 if(status==LT48_OK&&writing)status=pss2005one_protection_guard(r,&frame,wire);
 if(status==LT48_OK&&writing)status=efst1_protection_guard(r,&frame,wire);
 if(status==LT48_OK&&writing)status=nor3_protection_guard(r,&frame,wire);
 if(status==LT48_OK&&writing)status=word14_protection_guard(r,&frame,api,ctx);
 if(status==LT48_OK&&writing)status=atmel_008_protection_guard(r,&frame,wire);
 if(status==LT48_OK&&writing)status=fujitsu_002_protection_guard(r,&frame,wire);
 if(status==LT48_OK&&writing)status=sharp_s3_protection_guard(r,&frame,api,ctx);
 if(status==LT48_OK&&writing)status=sharpwordseven_protection_guard(r,&frame,api,ctx);
 if(status==LT48_OK&&r->legacy_page&&!r->legacy_page_leave_protection&&r->legacy_config_mask&&writing&&(!r->legacy_page_erase_protection_only||op==LT48_OPERATION_ERASE))status=page_protection_begin(r,&frame,wire,&protection);
#define STREAM(mode,volts,bytes) (r->legacy_page?page_stream(r,&frame,wire,api,ctx,mode,volts,bytes,length,start):lt48_legacy_nor53_stream_internal(r,&frame,wire,api,ctx,mode,volts,bytes,length,start))
 if(status==LT48_OK && op==LT48_OPERATION_ERASE && st_m28f201_preerase_required(r))
  status=st_m28f201_preerase(r,&frame,wire,api,ctx,pending,expected,length,start,out);
 if(status==LT48_OK && op==LT48_OPERATION_ERASE) {
  /* Original 0044F750 compact erase: DEVINIT(7), ERASE, DEVRESET. */
  if(r->sst38one)sst38.observe=1;
  if(r->mx26aone)mx26a.observe=1;
  status=r->legacy_page?lt48_legacy_macronix6_erase_internal(r,&frame,wire,api,ctx):lt48_gaaf_serial_erase(r,&frame,wire);
  if(r->mx26aone){mx26a.observe=0;if(!status&&(mx26a.pending||!mx26a.samples))status=LT48_PROTOCOL_ERROR;}
  if(r->sst38one){sst38.observe=0;if(!status&&(sst38.pending||sst38.count!=3))status=LT48_PROTOCOL_ERROR;}
 }
 if(status==LT48_OK)status=STREAM(1,r->nominal,pending);
 if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_ERASE))
  status=compare(pending,NULL,start,length,1,out);
 if(status==LT48_OK && op==LT48_OPERATION_OVER_PROGRAM)for(size_t i=0;i<length;i++)
  if(r->legacy_page?pending[i]!=255:(pending[i]&expected[i])!=expected[i]){out->first_mismatch=start+(uint32_t)i;out->mismatch_count=1;status=LT48_DEVICE_MISMATCH;break;}
 if(status==LT48_OK && (op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))status=compare(pending,expected,start,length,0,out);
 if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK)) {
  memset(pending,255,length);status=STREAM(11,r->nominal,pending);
 }
 if(status==LT48_OK && programming)status=STREAM(2,r->nominal,expected);
 if(status==LT48_OK && (writing||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)) {
  unsigned delta=margin_type==2?0:r->nominal/(margin_type?10:20);
  unsigned volts[2]={r->nominal-delta,r->nominal+delta},n=margin_type==2?1:2;
  /* Exact byte-mode008AT: primary1049I limits operatingVCC to3.6V. */
  if(!r->legacy_word&&!r->legacy_route&&!r->legacy_f3&&!r->legacy_page&&r->manufacturer==0x1f&&r->definition==0xb5&&volts[1]>360)volts[1]=360;
  if(r->mxwordtwo&&r->definition==0x3a){if(volts[0]<475)volts[0]=475;if(volts[1]>525)volts[1]=525;}
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
 if(r->legacy_page&&protection.armed) {
  lt48_status restored=page_protection_restore(r,api,ctx,&protection);
  if(restored!=LT48_OK)out->cleanup_status=LT48_CLEANUP_FAILED;
 }
 out->cleanup_completed=out->cleanup_status==LT48_OK;
 if(status==LT48_OK)status=out->cleanup_status;
 if(status==LT48_OK && writing) {
  lt48_gaaf_serial_result fresh;
  status=lt48_legacy_nor53_execute_internal(r,api,ctx,LT48_OPERATION_READ,start,length,NULL,0,pending,length,
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
