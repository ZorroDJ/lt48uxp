/* Authored owner for the remaining legacy class2018 configuration shapes.
 * Reuses the same compact interpreter, per-byte range engine and safe finish. */
static uint32_t cl38_u32(const unsigned char*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void cl38_put(unsigned char*p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
static int cl38_rich(const lt48_mcu51_resources*r){return r&&r->cl38&&r->cohort->kind==12;}
static uint32_t cl38_operation_mask(const lt48_mcu51_resources*r){unsigned n;cl38_schema(r->cohort->m,&n);return 0x3fu|(r->wave[10]?0x40u:0)|(r->wave[8]?0x80u:0)|(n?0x200u:0);}
static lt48_status cl38_validate(const lt48_mcu51_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*im,size_t size){
 if(!r||!n||start>=r->capacity||n>r->capacity-start)return LT48_SAFETY_REJECTED;
 if(op<1||op>10||!(cl38_operation_mask(r)&(1u<<(op-1))))return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7||op==8||op==10)&&(start||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==10){unsigned nf=0;const struct cl38_field*f=cl38_schema(r->cohort->m,&nf);if(!im&&!size)return LT48_OK;if(!im||size!=2+4*nf)return LT48_INVALID_ARGUMENT;unsigned mask=im[0]|(unsigned)im[1]<<8;if(mask&~((1u<<nf)-1))return LT48_INVALID_ARGUMENT;for(unsigned i=0;i<nf;i++)if((mask>>i&1)&&cl38_u32(im+2+4*i)>f[i].maximum)return LT48_INVALID_ARGUMENT;}
 else if(op>=3&&op<=6){if(!im||size!=n)return LT48_INVALID_ARGUMENT;for(size_t i=0;i<n;i++)if(!cl38_mapped(r,start+(uint32_t)i)&&im[i]!=255)return LT48_SAFETY_REJECTED;}
 return LT48_OK;
}
static lt48_status cl38_config_read(mcu_session*s,lt48_m37_id_decision_fn decide,void*dc){
 lt48_status st=config_cycle(s,1,0,decide,dc);unsigned n;const struct cl38_field*f=cl38_schema(s->r->cohort->m,&n);
 if(!st&&s->r->cohort->m==120){unsigned raw=s->buffers[9][0]&7u;if(raw!=7&&raw!=6&&raw!=5&&raw!=3)st=LT48_FORMAT_ERROR;}
 if(!st&&s->clcount!=n+1)st=LT48_FORMAT_ERROR;
 for(unsigned i=0;!st&&i<n;i++)if(s->clcfg[i+1].value>f[i].maximum)st=LT48_FORMAT_ERROR;
 return st;
}
static int cl38_locked(mcu_session*s){unsigned n;const struct cl38_field*f=cl38_schema(s->r->cohort->m,&n);for(unsigned i=0;i<n;i++)if(f[i].lock&&s->clcfg[i+1].value)return 1;return 0;}
static lt48_status cl38_read(mcu_session*s,unsigned char*out,unsigned cv,lt48_m37_id_decision_fn decide,void*dc,lt48_mcu51_result*z){
 lt48_status st=cl38_config_read(s,decide,dc);if(!st&&cl38_locked(s))st=LT48_SAFETY_REJECTED;if(!st)st=cycle(s,LT48_OPERATION_READ,0,s->r->capacity,NULL,out,cv,decide,dc,z);return st;
}
static lt48_status cl38_match(lt48_mcu51_result*z,unsigned a,const unsigned char*got,const unsigned char*want,size_t n){z->mismatch_count=0;z->first_mismatch=UINT32_MAX;for(size_t i=0;i<n;i++)if(got[i]!=(want?want[i]:255)){if(!z->mismatch_count)z->first_mismatch=a+(unsigned)i;z->mismatch_count++;}return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;}
static lt48_status cl38_config_write(mcu_session*s,const uint32_t*want,lt48_m37_id_decision_fn decide,void*dc){
 unsigned n;cl38_schema(s->r->cohort->m,&n);for(unsigned i=0;i<n;i++)s->clcfg[i+1].value=want[i];lt48_status st=config_cycle(s,8,1,decide,dc);if(!st)st=cl38_config_read(s,decide,dc);for(unsigned i=0;!st&&i<n;i++)if(s->clcfg[i+1].value!=want[i])st=LT48_DEVICE_MISMATCH;return st;
}
static lt48_status cl38_execute(const lt48_mcu51_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,const lt48_mcu51_options*opts,lt48_m37_id_decision_fn decide,void*dc,lt48_mcu51_result*z){
 if(!api||!z||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;unsigned nf;const struct cl38_field*f=cl38_schema(r->cohort->m,&nf);lt48_status st=cl38_validate(r,op,start,n,im,size);lt48_mcu51_options def={LT48_VERIFY_MARGIN_ALL,0,1,NULL,0};if(!opts)opts=&def;if(!st)st=lt48_mcu51_validate_options(r,op,opts);if(!st&&((op==1&&(!out||outsize<n))||(op==10&&(!out||outsize<4*nf))))st=LT48_BUFFER_TOO_SMALL;if(st)return st;
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;mcu_session s={0};s.r=r;s.api=api;s.context=ctx;s.mode=1;s.protection=opts->protection_level;memcpy(s.vars,r->vars,sizeof s.vars);
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);const unsigned char header[]={0x38,0xaa,0x55};if(!st)st=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof header);if(!st)st=lt48_runtime_transport_session_controller_write(s.wire,r->fpga,2778);const unsigned char selector[]={0x0f,15};if(!st)st=lt48_runtime_transport_session_controller_write(s.wire,selector,sizeof selector);if(!st)st=run(&s,0);for(unsigned i=0;!st&&i<3;i++)st=lt48_runtime_transport_session_status(s.wire,1);static const unsigned char busy[]={0x56,0x0a,1,0x40,1};if(!st)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof busy);
 if(!st)st=cl38_config_read(&s,decide,dc);
 if(!st&&(op==8||op==10)){
  uint32_t before[8]={0},want[8]={0};for(unsigned i=0;i<nf;i++)before[i]=want[i]=s.clcfg[i+1].value;
  unsigned mask=op==10&&im?(im[0]|(unsigned)im[1]<<8):0;
  if(op==8){for(unsigned i=0;i<nf;i++)if(f[i].lock){mask|=1u<<i;want[i]=f[i].category==0?(i<opts->protection_level?1:before[i]):opts->protection_level;}}
  else if(im)for(unsigned i=0;i<nf;i++)if(mask>>i&1)want[i]=cl38_u32(im+2+4*i);
  for(unsigned i=0;!st&&i<nf;i++)if(want[i]!=before[i]){if(f[i].category==2?(before[i]&want[i])!=want[i]:f[i].category==1&&r->cohort->m==120?before[i]!=0:(before[i]&want[i])!=before[i])st=LT48_SAFETY_REJECTED;}
  if(!st&&mask)st=cl38_config_write(&s,want,decide,dc);
  if(!st&&op==10)for(unsigned i=0;i<nf;i++)cl38_put(actual+4*i,s.clcfg[i+1].value);
 }else if(!st&&op==7){s.mode=7;memset(s.buffers,0,sizeof s.buffers);st=init(&s,decide,dc);if(!st)st=run(&s,10);lt48_status reset=run(&s,3);if(!st)st=reset;if(!st)st=cl38_read(&s,actual,r->nominal,decide,dc,z);if(!st)st=cl38_match(z,0,actual,NULL,r->capacity);
 }else if(!st){
  if(op==3||op==4){st=cl38_read(&s,actual,r->nominal,decide,dc,z);if(!st)for(size_t i=0;i<r->capacity;i++)if(op==3?actual[i]!=255:(actual[i]&im[i])!=im[i]){st=LT48_SAFETY_REJECTED;break;}if(!st)st=cycle(&s,op,0,r->capacity,im,NULL,r->nominal,decide,dc,z);}
  uint16_t cv[2];mcu51_verify_levels(r,opts->margin_type,cv);unsigned passes=(op==3||op==4||(op==5&&opts->margin==LT48_VERIFY_MARGIN_ALL))?2:1;if(op!=3&&op!=4){if(op!=5||opts->margin==LT48_VERIFY_MARGIN_NOMINAL)cv[0]=r->nominal;else if(opts->margin==LT48_VERIFY_MARGIN_HIGH)cv[0]=cv[1];}
  for(unsigned p=0;!st&&p<passes;p++){st=cl38_read(&s,actual,cv[p],decide,dc,z);if(!st&&op!=1)st=cl38_match(z,start,actual+start,op==2?NULL:im,n);}
 }
 z->operation_status=st;st=mcu51_finish(&s,st,z);if(!st&&op==1)memcpy(out,actual+start,n);if(!st&&op==10)memcpy(out,actual,4*nf);if(!st)z->completed_bytes=op==10?4*nf:n;lt48_runtime_transport_session_close(s.wire);free(actual);return st;
}

unsigned lt48_mcu51_cl38_mask(const lt48_mcu51_resources*r){return cl38_rich(r)?cl38_operation_mask(r):r&&r->cl38?0x2ffu:0;}
