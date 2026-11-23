/* STM8 SWIM, original byte/page workers and option pairs. Arrays erase to00. */
static uint32_t s8_u32(const unsigned char*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
lt48_status lt48_stm8s4_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=15||!n||a>=r->capacity||n>r->capacity-a)return LT48_INVALID_ARGUMENT;
 if((op<1||op>8)&&op!=10)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7||op==8||op==10)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op==10){if(!im&&!size)return LT48_OK;if(!im||size!=4+4*r->fields)return LT48_INVALID_ARGUMENT;uint32_t mask=s8_u32(im),legal=r->fields==32?UINT32_MAX:(1u<<r->fields)-1;if(mask&~legal)return LT48_INVALID_ARGUMENT;for(unsigned i=0;i<r->fields;i++)if((mask>>i&1)&&s8_u32(im+4+4*i)>r->configuration_maximum[i])return LT48_INVALID_ARGUMENT;}
 else if(op==8){if(im&&(size!=1||*im!=1))return LT48_INVALID_ARGUMENT;if(!im&&size)return LT48_INVALID_ARGUMENT;}
 else if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
static lt48_status s8_compare(lt48_serial_page_result*z,unsigned a,const unsigned char*got,const unsigned char*want,size_t n){for(size_t k=0;k<n;k++)if(got[k]!=(want?want[k]:0)){if(!z->mismatch_count)z->first_mismatch=a+(unsigned)k;z->mismatch_count++;}return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;}
static lt48_status s8_settings(s8253_session*s,uint32_t*v){s->mode=13;n79_address(s,0);numicro_config(s,NULL);lt48_status st=run_s8253(s,2);if(!st)st=run_s8253(s,6);for(unsigned i=0;!st&&i<s->r->fields;i++){if(s->config[i].value>s->r->configuration_maximum[i])st=LT48_DEVICE_MISMATCH;else v[i]=s->config[i].value;}lt48_status reset=run_s8253(s,3);return st?st:reset;}
static lt48_status s8_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;n79_address(s,a);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);size_t done=0;
 while(!st&&done<n){unsigned at=a+(unsigned)done,base=0,k=n79_region(s->r,at,&base);if(k>=s->r->regions){st=LT48_FORMAT_ERROR;break;}size_t take=n-done;if(take>base+s->r->region_length[k]-at)take=base+s->r->region_length[k]-at;n79_address(s,at);st=run_s8253(s,writing?8:5);
  for(size_t q=0;!st&&q<take;){size_t chunk=take-q;if(chunk>4096)chunk=4096;st=block_s8253(s,writing,writing?(unsigned char*)im+done+q:out+done+q,chunk);q+=chunk;}if(!st)st=stop_s8253(s);done+=take;
 }
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status s8_apply(s8253_session*s,const uint32_t*old,const uint32_t*want){
 if(old[0]&&memcmp(old,want,s->r->fields*sizeof*old))return LT48_SAFETY_REJECTED;
 s->mode=13;n79_address(s,0);lt48_status st=run_s8253(s,2);numicro_config(s,want);if(!st)st=run_s8253(s,11);lt48_status reset=run_s8253(s,3);if(!st)st=reset;uint32_t got[32]={0};if(!st)st=s8_settings(s,got);if(!st&&memcmp(got,want,s->r->fields*sizeof*got))st=LT48_DEVICE_MISMATCH;return st;
}
static lt48_status execute_stm8s4(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;/* No original GETID callback or expected chip signature. */
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_stm8s4_validate(r,op,a,n,im,size);if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL))st=LT48_INVALID_ARGUMENT;if(!st&&((op==1&&(!out||outsize<n))||(op==10&&(!out||outsize<4*r->fields))))st=LT48_BUFFER_TOO_SMALL;if(st){z->operation_status=st;return st;}if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return z->operation_status=LT48_NO_MEMORY;s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=13;memcpy(s.vars,r->vars,sizeof s.vars);numicro_config(&s,NULL);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;uint32_t saved[32]={0},want[32]={0};if(!st)st=s8_settings(&s,saved);int writing=op==3||op==4;
 if(!st&&(op==8||op==10)){memcpy(want,saved,sizeof want);if(op==8)want[0]=1;else if(im){uint32_t mask=s8_u32(im);for(unsigned i=0;i<r->fields;i++)if(mask>>i&1)want[i]=s8_u32(im+4+4*i);}if(memcmp(want,saved,r->fields*sizeof*want))st=s8_apply(&s,saved,want);if(!st&&op==10)for(unsigned i=0;i<r->fields;i++)for(unsigned k=0;k<4;k++)actual[4*i+k]=(unsigned char)(want[i]>>(8*k));
 }else if(!st&&op==7){s.mode=7;n79_address(&s,0);st=run_s8253(&s,2);/* Original nested Engineer resets option pairs and clears ROP. */
  for(unsigned off=0;!st&&off<r->capacity;off+=r->stm8s4_page){for(unsigned k=0;k<4;k++)s.buffers[0][k]=(unsigned char)(off>>(8*k));st=run_s8253(&s,10);if(!st&&s.buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;}
  lt48_status reset=run_s8253(&s,3);if(!st)st=reset;if(!st)st=s8_settings(&s,want);for(unsigned i=0;!st&&i<r->fields;i++)if(want[i])st=LT48_DEVICE_MISMATCH;if(!st)st=s8_cycle(&s,0,0,r->capacity,NULL,actual,r->nominal);if(!st)st=s8_compare(z,0,actual,NULL,r->capacity);
 }else if(!st){if(saved[0]||(writing&&saved[r->fields-1]))st=LT48_SAFETY_REJECTED;if(!st&&writing){st=s8_cycle(&s,0,0,n,NULL,actual,r->nominal);for(size_t k=0;!st&&k<n;k++)if(op==3?actual[k]!=0:(actual[k]|im[k])!=im[k]){z->first_mismatch=k;z->mismatch_count=1;st=LT48_DEVICE_MISMATCH;}if(!st)st=s8_cycle(&s,1,0,n,im,NULL,r->nominal);}
  uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];for(unsigned pass=0;!st&&pass<passes;pass++){st=s8_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=s8_compare(z,a,actual,op==2?NULL:im,n);}if(!st&&writing){st=s8_settings(&s,want);if(!st&&memcmp(saved,want,r->fields*sizeof*want))st=LT48_DEVICE_MISMATCH;}
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&(writing||op==7)){lt48_serial_page_result fresh;st=execute_stm8s4(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,NULL,NULL,&fresh);if(!st)st=s8_compare(z,0,actual,writing?im:NULL,r->capacity);if(st){z->operation_status=st;lt48_runtime_transport_session*wire=NULL;lt48_status cs;int done;lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cs,&done);if(!done){z->cleanup_status=cs;z->cleanup_completed=0;}}}
 if(!st){if(op==1)memcpy(out,actual,n);if(op==10)memcpy(out,actual,4*r->fields);z->completed_bytes=op==10?4*r->fields:n;}free(actual);return st;
}
