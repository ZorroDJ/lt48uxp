/* Reuse the combined-memory capture observer pattern. Each of the three
 * source erase workers must return1; a later success cannot hide an earlier
 * failed capture. Cell completion still requires independent whole readback. */
typedef struct{const lt48_protocol_transport*api;void*ctx;unsigned observe,pending,count;}sst38_monitor;
static lt48_status sst38_write(void*ctx,uint8_t ep,const unsigned char*b,size_t n,size_t*done){sst38_monitor*m=ctx;if(m->observe&&ep==2&&n&&b[0]==0x2c){unsigned captures=0;for(size_t j=1;j+1<n&&b[j];j+=2)captures+=b[j]==3;if(captures>1)return LT48_PROTOCOL_ERROR;m->pending=captures;}return m->api->write(m->ctx,ep,b,n,done);}
static lt48_status sst38_read(void*ctx,uint8_t ep,unsigned char*b,size_t n,size_t*done){sst38_monitor*m=ctx;lt48_status s=m->api->read(m->ctx,ep,b,n,done);if(!s&&m->observe&&m->pending&&ep==0x88){m->pending=0;if(*done!=n||n<3)return LT48_PROTOCOL_ERROR;m->count++;if(b[2]!=1)return LT48_DEVICE_MISMATCH;}return s;}
static int sst38_cancel(void*ctx){sst38_monitor*m=ctx;return m->api->cancelled&&m->api->cancelled(m->ctx);}
