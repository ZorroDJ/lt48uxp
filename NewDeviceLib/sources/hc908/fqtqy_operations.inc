/* Source F_ monitor entry is retained. Stop before its first write to target
 * RAM0080 (which would destroy the documented authentication flag), and use
 * the shared instruction-timed Flash owner instead of the vendor GPIO loop. */
static lt48_status f_select(hc908_session *s){
 const struct hc908_f_profile *f=s->r->f;unsigned char p[128],w[640];
 size_t n=hc908_read_preface(p,f->first);n-=7;p[n++]=2;p[n++]=0x62;p[n++]=0;
 for(size_t i=0;i+2<n;i++)if(p[i]==0x12){unsigned a=(unsigned)p[i+1]*256+p[i+2];if(a==0x58c8)a=f->tx;else if(a==0x5904)a=f->rx;else continue;p[i+1]=(unsigned char)(a>>8);p[i+2]=(unsigned char)a;i+=2;}
 lt48_status st=lt48_owner_load_applet_internal(s->wire,0x6100,p,n);
 for(unsigned i=0;st==LT48_OK&&i<4;i++)if(s->r->addresses[i].wave==15){
  const lt48_wave_code *v=s->r->workers+i;const unsigned char before[]={0x79,0x49,0x12,(unsigned char)(f->tx>>8),(unsigned char)f->tx,0x12,(unsigned char)(f->rx>>8),(unsigned char)f->rx};
  if(v->load_address!=0x5100||v->data_size!=f->read_size||memcmp(v->data+f->patch,before,8))return LT48_SAFETY_REJECTED;
  memcpy(w,v->data,v->data_size);memcpy(w+f->patch,(unsigned char[]){2,0x61,0,0,0,0,0,0},8);
  st=lt48_owner_load_applet_internal(s->wire,v->load_address,w,v->data_size);
 }
 n=hc908_monitor_bridge(p);
 for(size_t i=0;i+2<n;i++)if(p[i]==0x12){unsigned a=(unsigned)p[i+1]*256+p[i+2];if(a==0x58c8)a=f->tx;else if(a==0x5904)a=f->rx;else continue;p[i+1]=(unsigned char)(a>>8);p[i+2]=(unsigned char)a;i+=2;}
 p[n-2]=(unsigned char)(f->cleanup>>8);p[n-1]=(unsigned char)f->cleanup;
 if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,0x6200,p,n);
 return st;
}
static lt48_status f_read_cycle(hc908_session *s,unsigned voltage,unsigned char *data,unsigned char trim[2]){
 s->mode=1;s->address=0;memset(s->buffers,0,sizeof s->buffers);
 for(unsigned i=0;i<s->r->variable_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=voltage;
 lt48_status st=f_select(s);unsigned char pre[3];
 if(st==LT48_OK)st=run(s,2);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,pre,3);
 if(st==LT48_OK&&!(pre[0]&0x40))st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK){memset(data,255,65536);trim[0]=pre[1];trim[1]=pre[2];}
 const unsigned ranges[][2]={{s->r->first,0xfe00},{0xffb0,0xffbf},{0xffc0,0x10000}};
 for(unsigned k=0;st==LT48_OK&&k<3;k++){
  unsigned a=ranges[k][0],end=ranges[k][1];st=monitor_read(s,a,data+a,1);a++;
  while(st==LT48_OK&&a+1<end){const unsigned char next[]={0x1a};st=monitor(s,next,1,data+a,2);a+=2;}
  if(st==LT48_OK&&a<end)st=monitor_read(s,a,data+a,1);
 }
 const unsigned char stop[]={0};unsigned char terminal=0;
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,stop,1);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1);
 if(st==LT48_OK&&terminal!=1)st=LT48_PROTOCOL_ERROR;
 lt48_status reset=run(s,3);return st==LT48_OK?reset:st;
}
