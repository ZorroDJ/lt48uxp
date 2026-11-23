/* Byte-addressed parallel EEPROM;64-byte ID memory selected by12V on A9.
 * SDP choice is a write policy, not a readable target status. */
lt48_status lt48_at28c64b1_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=16||!n||a>=r->capacity||n>r->capacity-a)return LT48_INVALID_ARGUMENT;
 if(op<1||op>7)return LT48_UNSUPPORTED;
 if((op==3||op==4||op==7)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 return LT48_OK;
}
/* Page host takes the48 completion guard only. AddTab additionally queries
 * controller status3 after the complete region, never AT89 status1. */
static lt48_status a28_block(s8253_session*s,int writing,unsigned char*p,size_t n){
 if(!writing)return block_s8253(s,0,p,n);
 unsigned char h[]={0x48,(unsigned char)(n/512),(unsigned char)(n/512>>8),(unsigned char)(n%512),(unsigned char)(n%512>>8),1},reply[512]={0};lt48_status st=command_s8253(s,h,sizeof h,NULL);
 if(!st)st=transfer_s8253(s,0,4,p,n);
 if(!st)st=transfer_s8253(s,1,0x88,reply,512);
 if(!st&&!(reply[0]|reply[1])){unsigned char actual;lt48_status drain=block_s8253(s,0,&actual,1);st=drain?drain:LT48_DEVICE_MISMATCH;}return st;
}
static lt48_status a28_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 s->mode=writing?2:1;n79_address(s,a);uint32_t policy=s->r->at28c64b1_sdp;numicro_config(s,&policy);lt48_status st=w78bank_voltage(s,cv);if(!st)st=run_s8253(s,2);size_t done=0;
 while(!st&&done<n){unsigned at=a+(unsigned)done,base=0,k=n79_region(s->r,at,&base);if(k>=s->r->regions){st=LT48_FORMAT_ERROR;break;}size_t take=n-done;if(take>base+s->r->region_length[k]-at)take=base+s->r->region_length[k]-at;n79_address(s,at);if(!s->r->n79_xp)st=run_s8253(s,writing?8:5);
  for(size_t q=0;!st&&q<take;){size_t chunk=take-q;size_t bound=4096;if(s->r->n79_xp){size_t page=s->r->white3_page?s->r->white3_page:64;size_t pages=take/page/100;if(!pages)pages=1;if(pages>8192/page)pages=8192/page;bound=pages*page;}if(chunk>bound)chunk=bound;st=a28_block(s,writing,writing?(unsigned char*)im+done+q:out+done+q,chunk);q+=chunk;}if(!st&&writing&&!s->r->n79_xp){unsigned char end[]={0x0c,3},reply[512]={0};st=command_s8253(s,end,2,reply);if(!st&&(!(reply[0]|reply[1])||!(reply[2]&1)))st=LT48_DEVICE_MISMATCH;}if(!st)st=stop_s8253(s);done+=take;}
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_at28c64b1(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*out,size_t outsize,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;/* No source GETID procedure. The extra bytes are user memory. */
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_at28c64b1_validate(r,op,a,n,im,size);if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL||r->at28c64b1_sdp>1))st=LT48_INVALID_ARGUMENT;if(!st&&op==1&&(!out||outsize<n))st=LT48_BUFFER_TOO_SMALL;if(st)return z->operation_status=st;if(api->cancelled&&api->cancelled(ctx)){z->cleanup_completed=1;return z->operation_status=LT48_CANCELLED;}
 unsigned char*actual=malloc(r->capacity),*erased=op==7?malloc(r->capacity):NULL;if(!actual||(op==7&&!erased)){free(actual);free(erased);return z->operation_status=LT48_NO_MEMORY;}if(erased)memset(erased,255,r->capacity);const unsigned char*want=op==7?erased:im;int writing=op==3||op==4||op==7;s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(!st)st=setup_s8253(&s);if(!st)s.ready=1;
 if(!st&&writing)st=a28_cycle(&s,1,0,r->capacity,want,NULL,r->nominal);
 uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
 for(unsigned pass=0;!st&&pass<passes;pass++){st=a28_cycle(&s,0,a,n,NULL,actual,volts[pass]);if(!st&&op!=1)st=w78bank_compare(z,a,actual,op==2?NULL:want,n);}
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 if(!st&&writing){lt48_serial_page_result fresh;st=execute_at28c64b1(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,NULL,NULL,&fresh);if(!st)st=w78bank_compare(z,0,actual,want,r->capacity);if(st){z->operation_status=st;lt48_runtime_transport_session*wire=NULL;lt48_status cs;int done;lt48_owner_finish_internal(&clean,ctx,&wire,NULL,NULL,NULL,0,st,&cs,&done);if(!done){z->cleanup_status=cs;z->cleanup_completed=0;}}}
 if(!st){if(op==1)memcpy(out,actual,n);z->completed_bytes=n;}free(actual);free(erased);return st;
}
