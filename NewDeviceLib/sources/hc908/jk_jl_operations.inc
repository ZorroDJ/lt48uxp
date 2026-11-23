/* JK/JL profile composition over the qualified C transport/monitor/RAM owner.
 * Original GPIO target programs are replaced by documented MON08 transfers;
 * source monitor entry, clock/pins and serial helpers remain source-selected. */
static size_t jk_preface(unsigned char *p,const struct hc908_jk_profile *j){
 if(j->auth==0x10000){
  /* KX has no documented RAM authentication flag. This header reports bridge
   * readiness; jk_enter verifies all eight Flash key bytes before any RAM code. */
  const unsigned char ready[]={0x20,0xb2,0xfd,0x74,0x40,0xf0,0x20,0xb2,0xfd,0x74,0xff,0xf0,0x20,0xb2,0xfd,0xf0,2,0x5c,0};
  memcpy(p,ready,sizeof ready);return sizeof ready;
 }
 size_t n=0;const unsigned values[]={0x4a,j->auth>>8,j->auth&255};
 for(unsigned i=0;i<3;i++){const unsigned char b[]={0x79,(unsigned char)values[i],0x12,(unsigned char)(j->tx>>8),(unsigned char)j->tx,0x12,(unsigned char)(j->rx>>8),(unsigned char)j->rx};memcpy(p+n,b,sizeof b);n+=sizeof b;}
 const unsigned char end[]={0x12,(unsigned char)(j->rx>>8),(unsigned char)j->rx,0x20,0xb2,0xfd,0xea,0xf0,0x20,0xb2,0xfd,0x74,0xff,0xf0,0x20,0xb2,0xfd,0xf0,2,0x5c,0};
 memcpy(p+n,end,sizeof end);return n+sizeof end;
}
static lt48_status jk_select(hc908_session *s){
 const struct hc908_jk_profile *j=s->r->jk;unsigned char p[128],w[640];size_t n=jk_preface(p,j);
 p[n-2]=(unsigned char)(hc908_bridge_base(s->r)>>8);p[n-1]=(unsigned char)hc908_bridge_base(s->r);
 lt48_status st=lt48_owner_load_applet_internal(s->wire,hc908_preface_base(s->r),p,n);
 for(unsigned i=0;st==LT48_OK&&i<4;i++)if(s->r->addresses[i].wave==15){
  const lt48_wave_code *v=s->r->workers+i;
  unsigned char before[]={0x7b,6,0x7f,255,0xdf,0xfe,0xdb,0xfa};
  unsigned reg=s->r->gx?s->r->gx->delay_reg:3;before[0]=(unsigned char)(0x78+reg);before[6]=(unsigned char)(0xd8+reg);
  const unsigned char patch[]={2,(unsigned char)(hc908_preface_base(s->r)>>8),0,0,0,0,0,0};
  if(s->r->gpgz&&s->r->definition<=0x25)memcpy(before,(unsigned char[]){0x7c,0xb0,0x7d,0,0x7e,0,0x79,0x4a},8);
  if(v->load_address!=(s->r->gpgz&&s->r->definition<=0x25?0x54d3u:0x5100u)||v->data_size!=j->read_size||memcmp(v->data+j->patch,before,8))return LT48_SAFETY_REJECTED;
  memcpy(w,v->data,v->data_size);memcpy(w+j->patch,patch,8);
  if(s->r->abasaz&&s->r->definition==0x37){
   if(memcmp(w+0x50,(unsigned char[]){0x75,0x8d,0xf2},3))return LT48_SAFETY_REJECTED;
   w[0x52]=0xef; /* target-specific monitor baud */
  }
  st=lt48_owner_load_applet_internal(s->wire,v->load_address,w,v->data_size);
 }
 n=hc908_monitor_bridge(p);
 for(size_t i=0;i+2<n;i++)if(p[i]==0x12){unsigned a=(unsigned)p[i+1]*256+p[i+2];if(a==0x58c8)a=j->tx;else if(a==0x5904)a=j->rx;else continue;p[i+1]=(unsigned char)(a>>8);p[i+2]=(unsigned char)a;i+=2;}
 p[n-2]=(unsigned char)(j->cleanup>>8);p[n-1]=(unsigned char)j->cleanup;
 if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,hc908_bridge_base(s->r),p,n);
 return st;
}
static lt48_status jk_enter(hc908_session *s,unsigned voltage,int allow_locked){
 s->mode=1;s->address=0;memset(s->buffers,0,sizeof s->buffers);
 for(unsigned i=0;i<s->r->variable_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=voltage;
 lt48_status st=jk_select(s);unsigned char pre[3]={0};
 if(st==LT48_OK)st=run(s,2);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,pre,3);
 if(st==LT48_OK&&!(pre[0]&0x40)&&!allow_locked)st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK&&s->r->jk->auth==0x10000&&!allow_locked){
  unsigned char key[8];st=monitor_read(s,0xfff6,key,sizeof key);
  if(st==LT48_OK&&memcmp(key,s->image+0xfff6,sizeof key))st=LT48_SAFETY_REJECTED;
 }
 return st;
}
static lt48_status jk_exit(hc908_session *s){
 const unsigned char stop[]={0};unsigned char status=0;
 lt48_status st=lt48_runtime_transport_session_controller_write(s->wire,stop,1);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,&status,1);
 if(st==LT48_OK&&status!=1)st=LT48_PROTOCOL_ERROR;
 lt48_status reset=run(s,3);return st==LT48_OK?reset:st;
}
static lt48_status jk_read_range(hc908_session *s,unsigned a,unsigned char *p,unsigned n){
 if(!n)return LT48_OK;
 lt48_status st=monitor_read(s,a,p,1);unsigned i=1;
 while(st==LT48_OK&&i+1<n){const unsigned char next[]={0x1a};st=monitor(s,next,1,p+i,2);i+=2;}
 if(st==LT48_OK&&i<n)st=monitor_read(s,a+i,p+i,1);
 return st;
}
static lt48_status jk_read(hc908_session *s,unsigned voltage,unsigned char *p){
 lt48_status st=jk_enter(s,voltage,0);
 if(s->r->gx){
  if(st==LT48_OK)memset(p,255,65536);
  for(unsigned i=0;st==LT48_OK&&i<s->r->gx->range_count;i++){unsigned a=s->r->gx->ranges[i][0],b=s->r->gx->ranges[i][1];st=jk_read_range(s,a,p+a,b-a);}
 }else{
 if(st==LT48_OK){memset(p,255,65536);st=jk_read_range(s,s->r->first,p+s->r->first,0xfc00-s->r->first);}
 if(st==LT48_OK&&s->r->jk->flbpr==0xfe09)st=jk_read_range(s,0xffd0,p+0xffd0,48);
 else if(st==LT48_OK){st=jk_read_range(s,0xffcf,p+0xffcf,2);if(st==LT48_OK)st=jk_read_range(s,0xffdc,p+0xffdc,36);}
 }
 lt48_status end=jk_exit(s);return st==LT48_OK?end:st;
}
/* Rebase the known helper extended stores to FLCR; address/data literals remain
 * unchanged. The BD48 control register precedes its volatile protection. */
static void jk_ram_control(const lt48_hc908_resources *r,unsigned char *code,size_t n){
 if(!r->gx)return;
 for(size_t i=0;i+2<n;i++)if(code[i]==0xc7&&code[i+1]==0xfe&&code[i+2]==8){code[i+2]=(unsigned char)r->gx->control;i+=2;}
}
#include "gz_operations.inc"
static const struct hc908_gz_bank *abasaz_bank(const lt48_hc908_resources*,unsigned);
static lt48_status jk_write_row(hc908_session *s,unsigned a,const unsigned char *data,unsigned n){
 unsigned char code[64],got[32];size_t bytes=hc908_program_ram(code,a,n);if(!bytes)return LT48_SAFETY_REJECTED;
 code[7]=(unsigned char)(s->r->jk->flbpr>>8);code[8]=(unsigned char)s->r->jk->flbpr;code[53]=(unsigned char)s->r->jk->delay;
 if(s->r->gpgz&&s->r->definition>=0x56){const struct hc908_gz_bank*b=gz_bank(s->r,a);if(!b)return LT48_SAFETY_REJECTED;gz_ram(code,bytes,b);}
 else if(s->r->abasaz){const struct hc908_gz_bank*b=abasaz_bank(s->r,a);if(!b)return LT48_SAFETY_REJECTED;gz_ram(code,bytes,b);}
 else jk_ram_control(s->r,code,bytes);
 lt48_status st=execute_ram(s,code,bytes,data,n);
 if(st==LT48_OK)st=monitor_read(s,a,got,n);
 return st==LT48_OK&&memcmp(got,data,n)?LT48_DEVICE_MISMATCH:st;
}
#include "gp20_operations.inc"
#include "abasaz_operations.inc"
static lt48_status jk_mutate(hc908_session *s,int erase,const unsigned char *data){
 if(s->r->abasaz)return abasaz_mutate(s,erase,data);
 if(s->r->gpgz&&s->r->definition<=0x25)return gp20_mutate(s,erase,data);
 if(s->r->gpgz&&s->r->definition>=0x56&&erase)return gz_erase(s);
 lt48_status st=jk_enter(s,500,erase);
 if(st==LT48_OK&&s->r->jk->flbpr==0xfe09){unsigned char value=255,got=0;st=monitor_write(s,0xfe09,&value,1);if(st==LT48_OK)st=monitor_read(s,0xfe09,&got,1);if(st==LT48_OK&&got!=255)st=LT48_DEVICE_MISMATCH;}
 if(st==LT48_OK&&s->r->gx&&s->r->gx->control==0xfe07){unsigned char value=0,got=255;st=monitor_write(s,0xfe08,&value,1);if(st==LT48_OK)st=monitor_read(s,0xfe08,&got,1);if(st==LT48_OK&&got)st=LT48_DEVICE_MISMATCH;}
 if(st==LT48_OK&&erase){unsigned char code[64];int long_erase=s->r->gx&&s->r->gx->erase_loops==0xffff;size_t n=long_erase?hc908_erase_ram_long(code,s->r->first):hc908_erase_ram(code,s->r->grkx?0xff7e:s->r->first);code[7]=(unsigned char)(s->r->jk->flbpr>>8);code[8]=(unsigned char)s->r->jk->flbpr;code[n-4]=s->r->gx?s->r->gx->erase_delay:s->r->jk->flbpr==0xfe09?38:21;if(s->r->gx&&!long_erase){code[20]=(unsigned char)(s->r->gx->erase_loops>>8);code[21]=(unsigned char)s->r->gx->erase_loops;}if(long_erase&&s->r->apsr)code[20]=1;jk_ram_control(s->r,code,n);st=execute_ram(s,code,n,NULL,0);}
 else if(st==LT48_OK&&s->r->gx){
  /* Profile region order puts nonvolatile protection last. Every chunk fits
   * the shared32-byte staging buffer and its physical programming row. */
  for(unsigned i=0;st==LT48_OK&&i<s->r->gx->range_count;i++){
   unsigned a=s->r->gx->ranges[i][0],end=s->r->gx->ranges[i][1];
   while(st==LT48_OK&&a<end){unsigned n=32-(a&31);if(n>end-a)n=end-a;st=jk_write_row(s,a,data+a,n);a+=n;}
  }
 }else if(st==LT48_OK){
  for(unsigned a=s->r->first;st==LT48_OK&&a<0xfc00;a+=32)st=jk_write_row(s,a,data+a,32);
  if(st==LT48_OK&&s->r->jk->flbpr==0xfe09)st=jk_write_row(s,0xffd0,data+0xffd0,16);
  else if(st==LT48_OK){st=jk_write_row(s,0xffd0,data+0xffd0,1);if(st==LT48_OK)st=jk_write_row(s,0xffdc,data+0xffdc,4);}
  if(st==LT48_OK)st=jk_write_row(s,0xffe0,data+0xffe0,32);
  /* Set nonvolatile block protection only after the remaining Flash bytes. */
  if(st==LT48_OK&&s->r->jk->flbpr==0xffcf)st=jk_write_row(s,0xffcf,data+0xffcf,1);
 }
 lt48_status end=jk_exit(s);return st==LT48_OK?end:st;
}
static lt48_status jk_compare(const unsigned char *data,const unsigned char *expected,lt48_hc908_result *out){
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<65536;a++)if(data[a]!=(expected?expected[a]:255)){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status gp20_compare(const lt48_hc908_resources*r,const unsigned char*p,const unsigned char*expected,lt48_hc908_result*out){
 if(expected||lt48_hc908_blank_value(r))return jk_compare(p,expected,out);
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<65536;a++)if(p[a]!=(flash(r,a)&&!(r->abasaz&&abasaz_ee(r,a))?0:255)){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status hc908_jk_execute(const lt48_hc908_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *input,size_t input_size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_hc908_result *out){
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_hc908_validate(r,op,start,length,input,input_size,margin,type);
 if(st==LT48_OK&&(!api||!api->write||!api->read))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<65536))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){out->operation_status=st;return st;}
 hc908_session s={0};s.r=r;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof s.vars);
 s.image=malloc(65536);unsigned char *pending=malloc(65536);
 if(!s.image||!pending){free(s.image);free(pending);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 memset(s.image,255,65536);memcpy(s.image+0xfff6,r->security_key,8);
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 if(st==LT48_OK)st=setup(&s);
 if(st==LT48_OK&&op!=LT48_OPERATION_ERASE)st=jk_read(&s,500,pending);
 if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK))st=gp20_compare(s.r,pending,NULL,out);
 if(st==LT48_OK&&(op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_VERIFY))st=jk_compare(pending,input,out);
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE;
 if(st==LT48_OK&&writing){st=jk_mutate(&s,op==LT48_OPERATION_ERASE,input);if(st==LT48_OK){if(op==LT48_OPERATION_ERASE)memset(s.image+0xfff6,lt48_hc908_blank_value(r),8);else memcpy(s.image+0xfff6,input+0xfff6,8);}}
 if(st==LT48_OK&&(writing||op==LT48_OPERATION_VERIFY)){
  unsigned delta=type?50:25,volts[]={500,500-delta,500+delta},count=3;
  if(!writing&&margin!=LT48_VERIFY_MARGIN_ALL){volts[0]=margin==LT48_VERIFY_MARGIN_LOW?500-delta:margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;count=1;}
  for(unsigned i=0;st==LT48_OK&&i<count;i++){st=jk_read(&s,volts[i],pending);if(st==LT48_OK)st=gp20_compare(s.r,pending,op==LT48_OPERATION_ERASE?NULL:input,out);}
 }
 out->operation_status=st;static const unsigned cleanup[]={3,1};
 lt48_owner_finish_fpga_internal(api,context,&s.wire,run,&s,cleanup,2,r->idle,r->idle_size,st,&out->cleanup_status,&out->cleanup_completed);lt48_runtime_transport_session_close(s.wire);
 if(st==LT48_OK)st=out->cleanup_status;
 if(st==LT48_OK){out->completed_bytes=65536;if(op==LT48_OPERATION_READ)memcpy(read,pending,65536);}
 free(s.image);free(pending);return st;
}
