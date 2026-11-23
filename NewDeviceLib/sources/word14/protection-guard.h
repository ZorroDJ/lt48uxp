/* Preserve protection: inspect every original sector capture before mutation.
 * Original object fields wrap modulo32; raw captures must not be collapsed. */
typedef struct {const lt48_protocol_transport*api;void*ctx;unsigned capture,count,protected;} word14_tap;
static lt48_status word14_tap_write(void*c,uint8_t ep,const unsigned char*b,size_t n,size_t*done){
 word14_tap*t=c;t->capture=0;
 if(ep==2&&n&&b[0]==0x2c)for(size_t k=1;k+1<n&&b[k];k+=2)if(b[k]==3)t->capture++;
 if(t->capture>1)return LT48_PROTOCOL_ERROR;
 return t->api->write(t->ctx,ep,b,n,done);
}
static lt48_status word14_tap_read(void*c,uint8_t ep,unsigned char*b,size_t n,size_t*done){
 word14_tap*t=c;lt48_status s=t->api->read(t->ctx,ep,b,n,done);
 if(s==LT48_OK&&t->capture){if(*done<3)return LT48_PROTOCOL_ERROR;t->count++;t->protected|=b[2]&1;t->capture=0;}
 return s;
}
static int word14_tap_cancelled(void*c){word14_tap*t=c;return t->api->cancelled&&t->api->cancelled(t->ctx);}
static lt48_status word14_protection_guard(const lt48_gaaf_serial_resources*r,lt48_gaaf_serial_frame*f,const lt48_protocol_transport*api,void*ctx){
 if(!r->word14||!r->waves[9])return LT48_OK;
 unsigned sectors=0;for(size_t v=0;v<r->variable_count;v++)if(r->variables[v].variable_id==26)sectors=r->variables[v].value;
 if(!sectors||sectors>256)return LT48_SAFETY_REJECTED;
 word14_tap tap={api,ctx,0,0,0};lt48_protocol_transport observed=*api;observed.write=word14_tap_write;observed.read=word14_tap_read;observed.cancelled=word14_tap_cancelled;
 lt48_runtime_transport_session*w=NULL;lt48_status s=lt48_runtime_transport_session_create(&observed,&tap,&w);
 if(s==LT48_OK)s=lt48_gaaf_serial_wave(r,f,w,9);
 lt48_runtime_transport_session_close(w);
 if(s==LT48_OK&&tap.count!=sectors)s=LT48_PROTOCOL_ERROR;
 if(s==LT48_OK&&tap.protected)s=LT48_SAFETY_REJECTED;
 return s;
}
