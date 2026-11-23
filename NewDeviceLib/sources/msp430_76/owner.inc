/* Authored MSP430 owner over the audited AddTab/stream transport.
 * File order: Information 1000..10ff, then the selected main Flash range. */
static void msp430_config(s8253_session*s,unsigned protect,unsigned overwrite){s->config_count=s->r->fields;s->config[0]=(lt48_runtime_configuration_value){-1,0,0,protect};s->config[1]=(lt48_runtime_configuration_value){-1,1,0,overwrite};}
static lt48_status msp430_enter(s8253_session*s,unsigned mode,unsigned*fused){
 s->mode=mode;memset(s->buffers[12],0,32);memset(s->buffers[13],0,32);n79_address(s,0);lt48_status st=run_s8253(s,2);*fused=s->buffers[12][0]==0x55&&s->buffers[12][1]==0x55;
 /* Original INIT reports fuse notice9 and returns zero for 5555. The common
  * runner copies the captured frame before translating that return status. */
 if(st==LT48_DEVICE_MISMATCH&&*fused)return LT48_OK;
 return st;
}
static lt48_status msp430_read_all(s8253_session*s,unsigned char*out,unsigned cv){
 unsigned fused=0;lt48_status st=w78bank_voltage(s,cv);if(!st)st=msp430_enter(s,1,&fused);if(!st&&fused)st=LT48_SAFETY_REJECTED;
 size_t base=0;for(unsigned k=0;!st&&k<s->r->regions;k++){n79_address(s,(unsigned)base);st=run_s8253(s,5);for(size_t done=0;!st&&done<s->r->region_length[k];){size_t n=s->r->region_length[k]-done;if(n>4096)n=4096;st=block_s8253(s,0,out+base+done,n);done+=n;}if(!st)st=stop_s8253(s);base+=s->r->region_length[k];}
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status msp430_compare(lt48_serial_page_result*z,unsigned a,const unsigned char*actual,const unsigned char*want,size_t n,int preserve){
 for(size_t i=0;i<n;i++){unsigned at=a+(unsigned)i;if(preserve&&at>=192&&at<256)continue;unsigned char expected=want?want[i]:255;if(actual[i]!=expected){if(!z->mismatch_count){z->first_mismatch=at;}z->mismatch_count++;}}
 return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status msp430_write_all(s8253_session*s,const unsigned char*im){
 unsigned fused=0;lt48_status st=w78bank_voltage(s,s->r->nominal);if(!st)st=msp430_enter(s,2,&fused);if(!st&&fused)st=LT48_SAFETY_REJECTED;
 size_t base=0;for(unsigned k=0;!st&&k<s->r->regions;k++){n79_address(s,(unsigned)base);st=run_s8253(s,8);for(size_t done=0;!st&&done<s->r->region_length[k];){size_t n=s->r->region_length[k]-done;if(n>4096)n=4096;st=block_s8253(s,1,(unsigned char*)im+base+done,n);done+=n;}if(!st)st=stop_s8253(s);base+=s->r->region_length[k];}
 lt48_status reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status msp430_secure(s8253_session*s,unsigned already){
 if(already)return LT48_OK;
 unsigned fused=0;s->buffers[3][0]=0;msp430_config(s,1,s->config[1].value);lt48_status st=msp430_enter(s,13,&fused);if(!st&&!fused)st=run_s8253(s,11);if(!st&&!fused&&s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
 lt48_status reset=run_s8253(s,3);if(!st)st=reset;if(!st)st=msp430_enter(s,13,&fused);if(!st&&!fused)st=LT48_DEVICE_MISMATCH;reset=run_s8253(s,3);return st?st:reset;
}
static lt48_status execute_msp43076(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_serial_page_result*z){
 (void)decide;(void)dc;/* No independent GETID callback in these profiles. */
 if(!z||!api||!api->read||!api->write)return LT48_INVALID_ARGUMENT;
 memset(z,0,sizeof*z);z->first_mismatch=UINT32_MAX;lt48_status st=lt48_msp43076_validate(r,op,a,n,im,size);if(!st&&(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<8))))st=LT48_BUFFER_TOO_SMALL;if(st){z->operation_status=st;return st;}
 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;z->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned overwrite=r->fields==1?1:r->msp430_overwrite_calibration,requested=0,mask=0;if(op==10&&im){mask=im[0]|(unsigned)im[1]<<8;if(mask&2)overwrite=im[6];if(mask&1)requested=im[2];}
 unsigned char*actual=malloc(r->capacity),*expected=malloc(r->capacity);if(!actual||!expected){free(actual);free(expected);return LT48_NO_MEMORY;}s8253_session s={0};s.r=r;s.api=api;s.ctx=ctx;s.mode=1;memcpy(s.vars,r->vars,sizeof s.vars);msp430_config(&s,0,overwrite);st=lt48_runtime_transport_session_create(api,ctx,&s.wire);if(st){free(actual);free(expected);return st;}st=setup_s8253(&s);if(!st)s.ready=1;
 unsigned fused=0;if(!st)st=msp430_enter(&s,13,&fused);lt48_status reset=run_s8253(&s,3);if(!st)st=reset;
 if(!st&&(op==8||op==10)){if(op==8||((mask&1)&&requested)){st=msp430_secure(&s,fused);if(!st)fused=1;}else if((mask&1)&&!requested&&fused)st=LT48_SAFETY_REJECTED;if(!st&&op==10){memset(actual,0,8);actual[0]=(unsigned char)fused;actual[4]=r->fields==1?0:(unsigned char)overwrite;}
 }else if(!st){if(fused)st=LT48_SAFETY_REJECTED;unsigned char calibration[64]={0};int writing=op==3||op==4;
 if(!st&&(writing||op==7)){st=msp430_read_all(&s,actual,r->nominal);if(!st)memcpy(calibration,actual+192,64);}
 if(!st&&op==7){s.mode=7;n79_address(&s,0);st=msp430_enter(&s,7,&fused);if(!st&&fused)st=LT48_SAFETY_REJECTED;if(!st)st=run_s8253(&s,10);if(!st&&s.buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;reset=run_s8253(&s,3);if(!st)st=reset;if(!st)st=msp430_read_all(&s,actual,r->nominal);if(!st)st=msp430_compare(z,0,actual,NULL,r->capacity,!overwrite);if(!st&&!overwrite&&memcmp(actual+192,calibration,64))st=LT48_DEVICE_MISMATCH;
 }else if(!st){
 if(writing){memcpy(expected,im,r->capacity);if(!overwrite)memcpy(expected+192,calibration,64);for(size_t i=0;!st&&i<r->capacity;i++){if(!overwrite&&i>=192&&i<256)continue;if(op==3?actual[i]!=255:(actual[i]&expected[i])!=expected[i])st=LT48_SAFETY_REJECTED;}if(!st)st=msp430_write_all(&s,expected);}
 uint32_t volts[4]={0};unsigned count=0;if(!st)st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;if(!writing&&(op!=5||margin==LT48_VERIFY_MARGIN_NOMINAL))volts[0]=r->nominal;else if(!writing&&margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];for(unsigned pass=0;!st&&pass<passes;pass++){st=msp430_read_all(&s,actual,volts[pass]);if(!st&&op!=1){const unsigned char*want=writing?expected+a:op==2?NULL:im;st=msp430_compare(z,a,actual+a,want,n,writing?0:(op==2||op==5)&&!overwrite);}}
 }
 }
 z->operation_status=st;lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;const unsigned phases[]={0,1,2,3};st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);if(!st){if(op==1)memcpy(read,actual+a,n);if(op==10)memcpy(read,actual,8);z->completed_bytes=op==10?8:n;}free(actual);free(expected);return st;
}
