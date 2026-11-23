/* Authored ST M28F201 erase prerequisite, ST August 1998 datasheet page 7.
 * Included after the legacy owner's comparison helper. Original resources
 * remain BYO and pass the owner's exact source/profile checks first.
 * This does not admit the still-private profile to operator selection. */
#ifndef LT48_ST_M28F201_PREERASE_H
#define LT48_ST_M28F201_PREERASE_H
static int st_m28f201_preerase_required(const lt48_gaaf_serial_resources *r) {
 return r->legacy_nor && !r->legacy_word && !r->legacy_route &&
        r->definition==0x89u && r->capacity==0x40000u &&
        r->manufacturer==0x20u && r->device==0xf4u;
}
static lt48_status st_m28f201_preerase(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,lt48_runtime_transport_session *wire,
 const lt48_protocol_transport *api,void *ctx,unsigned char *pending,
 unsigned char *expected,size_t length,uint32_t start,lt48_gaaf_serial_result *out) {
 memset(expected,0,length);
 lt48_status status=lt48_legacy_nor53_stream_internal(r,frame,wire,api,ctx,2,r->nominal,expected,length,start);
 if(status==LT48_OK)status=lt48_legacy_nor53_stream_internal(r,frame,wire,api,ctx,4,r->nominal,expected,length,start);
 if(status==LT48_OK)status=lt48_legacy_nor53_stream_internal(r,frame,wire,api,ctx,1,r->nominal,pending,length,start);
 if(status==LT48_OK)status=compare(pending,expected,start,length,0,out);
 memset(expected,255,length);
 return status;
}
#endif
