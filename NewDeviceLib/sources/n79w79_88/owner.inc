/* Common N79/W79 target owner over the existing transport/interpreter.
 * Physical/logical region bases are audited against original AddTab mapping. */
static unsigned n79_region(const lt48_s8253_resources*r,unsigned a,unsigned*base){unsigned off=0;for(unsigned k=0;k<r->regions;k++){if(a<off+r->region_length[k]){*base=off;return k;}off+=r->region_length[k];}*base=off;return r->regions;}
static void n79_address(s8253_session*s,unsigned a){unsigned base=0,k=n79_region(s->r,a,&base),physical=s->r->n79_physical[k]+a-base,logical=s->r->n79_xp?s->r->capacity:s->r->region_bank[k]+a-base;for(unsigned i=0;i<4;i++){s->buffers[0][i]=(unsigned char)(physical>>(8*i));s->buffers[1][i]=(unsigned char)(logical>>(8*i));}}
static void n79_config(s8253_session*s,const unsigned char*v){s->config_count=s->r->fields;for(unsigned i=0;i<s->r->fields;i++)s->config[i]=(lt48_runtime_configuration_value){-1,0,i,v?v[i]:UINT32_MAX};}
static lt48_status n79_settings(s8253_session*s,unsigned char*v){s->mode=13;n79_address(s,0);n79_config(s,NULL);lt48_status st=run_s8253(s,2);if(!st)st=run_s8253(s,6);for(unsigned i=0;!st&&i<s->r->fields;i++){if(s->config[i].value>1)st=LT48_DEVICE_MISMATCH;else v[i]=(unsigned char)s->config[i].value;}lt48_status reset=run_s8253(s,3);return st?st:reset;}
static int n79_protected(const lt48_s8253_resources*r,const unsigned char*v){for(unsigned i=0;i<r->fields;i++)if((r->n79_locks>>i&1)&&v[i])return 1;return 0;}
static lt48_status n79_apply(s8253_session*s,const unsigned char*old,const unsigned char*want){
 if(s->r->configuration_id!=25189&&s->r->configuration_id!=25149){unsigned first=s->r->n79_direct==31?5:6;if(want[first]&&want[first+1])return LT48_SAFETY_REJECTED;}
 for(unsigned i=0;i<s->r->fields;i++)if((s->r->n79_direct>>i&1)?want[i]>old[i]:want[i]<old[i])return LT48_SAFETY_REJECTED;
 s->mode=13;n79_address(s,0);lt48_status st=run_s8253(s,2);n79_config(s,want);if(!st)st=run_s8253(s,11);lt48_status reset=run_s8253(s,3);if(!st)st=reset;unsigned char got[16];if(!st)st=n79_settings(s,got);if(!st&&memcmp(got,want,s->r->fields))st=LT48_DEVICE_MISMATCH;return st;
}
static lt48_status n79_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;n79_address(s,a);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);
 /* Original INIT mode2 erases once. Do not reinitialize between regions. */
 size_t done=0;while(!st&&done<n){unsigned at=a+(unsigned)done,base=0,k=n79_region(s->r,at,&base);size_t take=n-done;if(take>base+s->r->region_length[k]-at)take=base+s->r->region_length[k]-at;n79_address(s,at);if(!s->r->n79_xp)st=run_s8253(s,writing?8:5);for(size_t q=0;!st&&q<take;){size_t chunk=take-q;if(chunk>4096)chunk=4096;st=block_s8253(s,writing,writing?(unsigned char*)im+done+q:out+done+q,chunk);q+=chunk;}if(!st)st=stop_s8253(s);done+=take;}
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_n79w79(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;/* These source profiles have no GETID callback. */
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_n79w79_validate(r,op,a,n,im,size);if(!st&&(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<r->fields))))st=LT48_BUFFER_TOO_SMALL;if(st)return st;
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity);if(!actual)return LT48_NO_MEMORY;s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);n79_config(&s,NULL);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);return st;}st=setup_s8253(&s);if(!st)s.ready=1;
 unsigned char saved[16]={0},want[16]={0},blank[16]={0};for(unsigned i=0;i<r->fields;i++)blank[i]=r->n79_direct>>i&1;if(!st)st=n79_settings(&s,saved);int writing=op==3||op==4;if(!st&&op!=7&&op!=8&&op!=10&&!writing&&n79_protected(r,saved))st=LT48_SAFETY_REJECTED;
 if(!st&&(op==8||op==10)){memcpy(want,saved,r->fields);if(op==8){for(unsigned i=0;i<r->fields;i++)if((r->n79_secure>>i&1)&&(r->configuration_id==25189||r->configuration_id==25149||!saved[i+1]))want[i]=1;}else if(im){unsigned mask=im[0]|(unsigned)im[1]<<8;for(unsigned i=0;i<r->fields;i++)if(mask>>i&1)want[i]=im[i+2];}if(memcmp(want,saved,r->fields))st=n79_apply(&s,saved,want);if(!st&&op==10)memcpy(actual,want,r->fields);
 }else if(!st&&op==7){s.mode=7;n79_address(&s,0);st=run_s8253(&s,2);if(!st)st=run_s8253(&s,10);lt48_status reset=run_s8253(&s,3);if(!st)st=reset;if(!st)st=n79_settings(&s,want);if(!st&&memcmp(want,blank,r->fields))st=LT48_DEVICE_MISMATCH;if(!st)st=n79_cycle(&s,0,0,r->capacity,NULL,actual,r->nominal);if(!st)st=w78bank_compare(z,0,actual,NULL,r->capacity);
 }else if(!st){if(writing){st=n79_cycle(&s,1,0,n,im,NULL,r->nominal);if(!st)st=n79_settings(&s,want);if(!st&&n79_protected(r,want))st=LT48_DEVICE_MISMATCH;}
 uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];for(unsigned pass=0;!st&&pass<passes;pass++){st=n79_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2?NULL:im,n);}
 if(!st&&writing){memcpy(want,saved,r->fields);for(unsigned i=0;i<r->fields;i++)if(r->n79_locks>>i&1)want[i]=0;if(memcmp(want,blank,r->fields))st=n79_apply(&s,blank,want);}
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);if(!st){if(op==1)memcpy(read,actual,n);if(op==10)memcpy(read,actual,r->fields);z->completed_bytes=op==10?r->fields:n;}free(actual);return st;
}
