/* Exact MX26C4000 PM0662 completion correction; vendor files stay unchanged.
 * Original USB04/02 returns the flash byte in the fitted route, while the
 * common Windows host poll tests bit0. Present Q7 there only during Erase.
 * Full independent array blank/read verification remains mandatory. */
#ifndef LT48_MX26AONE_COMPLETION_H
#define LT48_MX26AONE_COMPLETION_H
typedef struct {
 const lt48_protocol_transport *api;void *ctx;
 unsigned observe,pending,samples;
} mx26aone_monitor;
static lt48_status mx26aone_write(void *context,uint8_t ep,const unsigned char*b,size_t n,size_t*done){
 mx26aone_monitor*m=context;
 if(m->observe&&ep==2&&n&&b[0]==4){
  if(n!=512||b[1]!=2||m->pending)return LT48_PROTOCOL_ERROR;
 }
 lt48_status s=m->api->write(m->ctx,ep,b,n,done);
 if(s==LT48_OK&&*done==n&&m->observe&&ep==2&&n&&b[0]==4)m->pending=1;
 return s;
}
static lt48_status mx26aone_read(void *context,uint8_t ep,unsigned char*b,size_t n,size_t*done){
 mx26aone_monitor*m=context;lt48_status s=m->api->read(m->ctx,ep,b,n,done);
 if(m->pending){
  m->pending=0;
  if(s==LT48_OK&&(*done!=n||n!=512||ep!=0x88||!b[0]))return LT48_PROTOCOL_ERROR;
  if(s==LT48_OK){b[2]=(unsigned char)((b[2]<<1)|(b[2]>>7));m->samples++;}
 }
 return s;
}
static int mx26aone_cancel(void*context){mx26aone_monitor*m=context;return m->api->cancelled&&m->api->cancelled(m->ctx);}
#endif
