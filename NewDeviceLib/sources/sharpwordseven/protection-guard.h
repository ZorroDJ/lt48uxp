/* Observe every status capture in original LOADCONFIG, including sectors
 * beyond bit31. No protection/unprotect command is issued by this guard.
 * Ordinary mutation of locked parts is refused; standalone lock editing
 * remains unavailable. The original named bank summary is not a sector map. */
typedef struct{const lt48_protocol_transport*api;void*ctx;unsigned pending,count,locked;unsigned char sector[71];}sharpwordseven_probe;
static lt48_status sharpwordseven_write(void*ctx,uint8_t ep,const unsigned char*b,size_t n,size_t*done){sharpwordseven_probe*p=ctx;
 if(ep==2&&n&&b[0]==0x2c)for(size_t i=1;i<n&&b[i];){unsigned k=b[i++];if(k==3){if(p->pending)return LT48_PROTOCOL_ERROR;p->pending=1;continue;}if(i>=n)return LT48_PROTOCOL_ERROR;i++;}
 return p->api->write(p->ctx,ep,b,n,done);
}
static lt48_status sharpwordseven_read(void*ctx,uint8_t ep,unsigned char*b,size_t n,size_t*done){sharpwordseven_probe*p=ctx;lt48_status s=p->api->read(p->ctx,ep,b,n,done);
 if(s==LT48_OK&&p->pending){if(ep!=0x88||*done<3||p->count>=71)return LT48_PROTOCOL_ERROR;p->sector[p->count++]=b[2]&1;p->locked|=b[2]&1;p->pending=0;}return s;
}
static int sharpwordseven_cancelled(void*ctx){sharpwordseven_probe*p=ctx;return p->api->cancelled?p->api->cancelled(p->ctx):0;}
static lt48_status sharpwordseven_protection_guard(const lt48_gaaf_serial_resources*r,lt48_gaaf_serial_frame*f,const lt48_protocol_transport*api,void*ctx){
 if(!r->sharpwordseven)return LT48_OK;
 if(!r->sharpword_sector_count||r->sharpword_sector_count>71)return LT48_SAFETY_REJECTED;
 sharpwordseven_probe p={.api=api,.ctx=ctx};lt48_protocol_transport proxy=*api;proxy.write=sharpwordseven_write;proxy.read=sharpwordseven_read;proxy.cancelled=sharpwordseven_cancelled;
 lt48_runtime_transport_session*w=NULL;lt48_status s=lt48_runtime_transport_session_create(&proxy,&p,&w);if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,f,w,8);lt48_runtime_transport_session_close(w);
 if(s==LT48_OK&&(p.pending||p.count!=r->sharpword_sector_count))s=LT48_PROTOCOL_ERROR;
 if(s==LT48_OK&&p.locked)s=LT48_SAFETY_REJECTED;
 return s;
}
