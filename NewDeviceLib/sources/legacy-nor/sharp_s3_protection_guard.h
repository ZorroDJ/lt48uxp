/* Observe every source GETCONFIG capture rather than its summarized table.
 * Source D82 queries all 64 block-status addresses, one resident byte at a
 * time. Only DQ0 means locked. Standalone protection changes are deferred. */
typedef struct {
 const lt48_protocol_transport *api;void *ctx;
 unsigned pending,captures,locked;
} sharp_s3_lock_probe;
static lt48_status sharp_s3_probe_write(void *ctx,uint8_t ep,const unsigned char *b,size_t n,size_t *done) {
 sharp_s3_lock_probe *p=ctx;
 if(ep==2&&n&&b[0]==0x2c)for(size_t i=1;i+1<n&&b[i];i+=2)if(b[i]==3){
  if(b[i+1]||p->pending)return LT48_SAFETY_REJECTED;
  p->pending=1;
 }
 return p->api->write(p->ctx,ep,b,n,done);
}
static lt48_status sharp_s3_probe_read(void *ctx,uint8_t ep,unsigned char *b,size_t n,size_t *done) {
 sharp_s3_lock_probe *p=ctx;lt48_status s=p->api->read(p->ctx,ep,b,n,done);
 if(s==LT48_OK&&p->pending){
  if(ep!=0x88||*done<3)return LT48_PROTOCOL_ERROR;
  p->locked|=b[2]&1u;p->captures++;p->pending=0;
 }
 return s;
}
static int sharp_s3_probe_cancelled(void *ctx) {
 sharp_s3_lock_probe *p=ctx;return p->api->cancelled?p->api->cancelled(p->ctx):0;
}
static lt48_status sharp_s3_protection_guard(const lt48_gaaf_serial_resources *r,
 lt48_gaaf_serial_frame *frame,const lt48_protocol_transport *api,void *ctx) {
 if(!r->legacy_nor||r->manufacturer!=0xb0||r->definition!=0x82)return LT48_OK;
 if(r->capacity!=0x400000||r->legacy_config_mask!=1)return LT48_SAFETY_REJECTED;
 sharp_s3_lock_probe probe={api,ctx,0,0,0};lt48_protocol_transport proxy=*api;
 proxy.write=sharp_s3_probe_write;proxy.read=sharp_s3_probe_read;proxy.cancelled=sharp_s3_probe_cancelled;
 lt48_runtime_transport_session *wire=NULL;
 lt48_status s=lt48_runtime_transport_session_create(&proxy,&probe,&wire);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,frame,wire,9);
 lt48_runtime_transport_session_close(wire);
 if(s==LT48_OK&&(probe.pending||probe.captures!=64))s=LT48_PROTOCOL_ERROR;
 if(s==LT48_OK&&probe.locked)s=LT48_SAFETY_REJECTED;
 return s;
}
