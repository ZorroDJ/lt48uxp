/* R8C parallel flash. Each address is written at most once after erase.
 * Original applets/FPGA and source bank erase recipes remain external BYO.
 * OFS is excluded from the main stream and committed once, after code verify.
 * Parallel ROM protection cannot be undone by this programmer. */
lt48_status lt48_r8c8_validate(const lt48_s8253_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||r->kind!=51||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;
 
 
 if((op<1||op>8)&&op!=10)return LT48_UNSUPPORTED;
 
 
 if((op==3||op==4||op==7||op==8||op==10)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;
 
 
 if(op>=3&&op<=6&&(!im||size!=n))return LT48_INVALID_ARGUMENT;
 
 
 if(op==8&&im&&(size!=1||im[0]!=1))return LT48_INVALID_ARGUMENT;
 
 
 if(op==10&&im&&(size!=8||im[0]>1||im[1]||im[2]||im[3]||im[4]>1||im[5]||im[6]||im[7]))return LT48_INVALID_ARGUMENT;
 
 
 /* Reserved OFS bits1/4/6 must stay1;
 
  other option bits come from the image. */
 if((op==3||op==4)&&(im[n-1]&0x52)!=0x52)return LT48_SAFETY_REJECTED;
 
 if((op==3||op==4)&&!r->n79_xp)for(unsigned at=0;
 at<r->capacity;
 at++){unsigned k;
 for(k=0;
 k<r->regions;
 k++)if(at>=r->region_bank[k]&&at-r->region_bank[k]<r->region_length[k])break;
 if(k==r->regions&&im[at]!=255)return LT48_SAFETY_REJECTED;
 }

 
 return LT48_OK;
 
 
}
static int r8c8_locked(unsigned v){return (v&12)==4;
 
 }
/* XP images are compact code;
  AddTab active-file indices are physical MCU
 * addresses. The latter use a64KiB sparse image, with FF padding in holes. */
static unsigned r8c8_image_base(const lt48_s8253_resources*r,unsigned k){return r->n79_xp?0:r->region_bank[k];
 }
static void r8c8_address(s8253_session*s,unsigned mode,unsigned image){
 memset(s->buffers,0,sizeof s->buffers);
 s->mode=mode;
 
 unsigned b=s->r->n79_xp?s->r->capacity:image;
 
 for(unsigned i=0;
 i<4;
 i++){s->buffers[0][i]=(unsigned char)(image>>(8*i));
 s->buffers[1][i]=(unsigned char)(b>>(8*i));
 }
}
static lt48_status r8c8_cycle(s8253_session*s,int writing,unsigned a,size_t n,const unsigned char*im,unsigned char*out,unsigned cv){
 lt48_status st=LT48_OK;
 size_t done=0;
 
 while(!st&&done<n){unsigned offset=a+done,k,next=s->r->capacity;
 size_t span=0;
 
  for(k=0;
 k<s->r->regions;
 k++){unsigned base=r8c8_image_base(s->r,k);
 if(offset>=base&&offset-base<s->r->region_length[k]){span=s->r->region_length[k]-(offset-base);
 break;
 }if(base>offset&&base<next)next=base;
 }
  if(!span){span=next-offset;
 if(span>n-done)span=n-done;
 if(!span)return LT48_SAFETY_REJECTED;
 if(!writing)memset(out+done,255,span);
 done+=span;
 continue;
 }
  if(span>n-done)span=n-done;
 
  r8c8_address(s,writing?2:1,offset);
 st=w78bank_voltage(s,cv);
 if(!st)st=run_s8253(s,2);
 if(!st&&!s->r->n79_xp)st=run_s8253(s,writing?8:5);
 
  for(size_t q=0;
 !st&&q<span;
 ){size_t take=span-q;
 if(take>4096)take=4096;
 st=a28_block(s,writing,writing?(unsigned char*)im+done+q:out+done+q,take);
 q+=take;
 }
  if(!st&&writing){unsigned char query[]={0x0c,(unsigned char)(s->r->n79_xp?1:3)},reply[512]={0};
 st=command_s8253(s,query,2,reply);
 if(!st&&!(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;
 else if(!st&&!(reply[2]&1)){unsigned char actual;
 lt48_status drain=block_s8253(s,0,&actual,1);
 st=drain?drain:LT48_DEVICE_MISMATCH;
 }}
  if(!st)st=stop_s8253(s);
 
  lt48_status reset=run_s8253(s,3);
 if(!st)st=reset;
 done+=span;
 
 }
 return st;
 
}
static lt48_status r8c8_erase(s8253_session*s){r8c8_address(s,7,0);
 
 lt48_status st=run_s8253(s,2);
 
 if(!st)st=run_s8253(s,10);
 
 if(!st&&s->buffers[s->r->n79_xp?2:3][0]!=1)st=LT48_DEVICE_MISMATCH;
 
 lt48_status reset=run_s8253(s,3);
 
 return st?st:reset;
 
 }
static lt48_status r8c8_rewrite(s8253_session*s,unsigned char*work,unsigned char*actual,int lock,lt48_serial_page_result*z){
 unsigned cap=s->r->capacity;
 
 unsigned char ofs=work[cap-1];
 
 if(lock)ofs=(ofs|4)&~8u;
 
 else ofs|=8;
 
 work[cap-1]=ofs;
 
 
 lt48_status st=r8c8_erase(s);
 
 if(!st)st=r8c8_cycle(s,0,0,cap,NULL,actual,s->r->nominal);
 
 if(!st)st=w78bank_compare(z,0,actual,NULL,cap);
 
 
 if(!st)st=r8c8_cycle(s,1,0,cap-1,work,NULL,s->r->nominal);
 
 
 if(!st)st=r8c8_cycle(s,0,0,cap-1,NULL,actual,s->r->nominal);
 
 if(!st)st=w78bank_compare(z,0,actual,work,cap-1);
 
 
 /* Commit the option byte exactly once. Never attempt a retry without erase. */
 if(!st)st=r8c8_cycle(s,1,cap-1,1,&ofs,NULL,s->r->nominal);
 
 
 if(!st&&!lock){st=r8c8_cycle(s,0,0,cap,NULL,actual,s->r->nominal);
 
 if(!st)st=w78bank_compare(z,0,actual,work,cap);
 
 }
 return st;
 
 
}
static lt48_status execute_r8c8(const lt48_s8253_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_serial_page_result*z){
 if(!z||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
 
 memset(z,0,sizeof*z);
 
 z->first_mismatch=UINT32_MAX;
 
 lt48_status st=lt48_r8c8_validate(r,op,a,n,im,size);
 
 
 if(!st&&(type>1||margin>LT48_VERIFY_MARGIN_ALL))st=LT48_INVALID_ARGUMENT;
 
 
 if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<4))))st=LT48_BUFFER_TOO_SMALL;
 
 if(st)return z->operation_status=st;
 
 
 if(api->cancelled&&api->cancelled(ctx)){z->cleanup_completed=1;
 
 return z->operation_status=LT48_CANCELLED;
 
 }
 unsigned char*actual=malloc(r->capacity),*work=malloc(r->capacity);
 
 if(!actual||!work){free(actual);
 
 free(work);
 
 return z->operation_status=LT48_NO_MEMORY;
 
 }
 s8253_session s={0};
 
 s.r=r;
 
 s.api=api;
 
 s.ctx=ctx;
 
 memcpy(s.vars,r->vars,sizeof s.vars);
 
 r8c8_address(&s,1,0);
 
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
 
 if(st){free(actual);
 
 free(work);
 
 return z->operation_status=st;
 
 }st=setup_s8253(&s);
 
 if(!st)s.ready=1;
 
 
 int config_lock=0,locked=0,mutated=0;
 
 unsigned cfg=0;
 
 
 if(!st&&(op==3||op==4)){memcpy(work,im,n);
 
 st=r8c8_rewrite(&s,work,actual,0,z);
 
 mutated=1;
 
 }
 else if(!st&&(op==8||op==10)){
  st=r8c8_cycle(&s,0,0,r->capacity,NULL,work,r->nominal);
 
 if(!st){locked=r8c8_locked(work[r->capacity-1]);
 
 cfg=locked;
 
 config_lock=op==8||(op==10&&im&&im[0]&&im[4]);
 
 if(locked&&!config_lock&&im&&im[0])st=LT48_SAFETY_REJECTED;
 
 else if(config_lock&&!locked){st=r8c8_rewrite(&s,work,actual,1,z);
 
 mutated=1;
 
 cfg=1;
 
 }}
 }else if(!st){
  if(op==7){st=r8c8_erase(&s);
 
 mutated=1;
 
 }
  uint32_t volts[4]={r->nominal,0};
 
 unsigned count=0,passes=1;
 
 if(!st&&op==5){st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count);
 
 if(margin==LT48_VERIFY_MARGIN_ALL)passes=2;
 
 else if(margin==LT48_VERIFY_MARGIN_NOMINAL)volts[0]=r->nominal;
 
 else if(margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
 
 }
  for(unsigned pass=0;
 
 !st&&pass<passes;
 
 pass++){st=r8c8_cycle(&s,0,a,n,NULL,actual,volts[pass]);
 
 if(!st&&op!=1)st=w78bank_compare(z,a,actual,(op==2||op==7)?NULL:im,n);
 
 }
 }
 z->operation_status=st;
 
 lt48_protocol_transport clean=*api;
 
 clean.cancelled=NULL;
 
 s.api=&clean;
 
 const unsigned phases[]={0,1,2,3};
 
 st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_sst,&s,phases,4,st,&z->cleanup_status,&z->cleanup_completed);
 
 
 /* Ordinary mutations also receive a fresh powered raw Read after cleanup.
  * A deliberate ROM lock disables parallel Read and is committed last. */
 if(!st&&mutated&&!config_lock){lt48_serial_page_result fresh;
 
 st=execute_r8c8(r,api,ctx,1,0,r->capacity,NULL,0,actual,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,type,&fresh);
 
 if(!st)st=w78bank_compare(z,0,actual,op==7?NULL:work,r->capacity);
 
 if(st){z->operation_status=st;
 if(!fresh.cleanup_completed){z->cleanup_completed=0;
 z->cleanup_status=fresh.cleanup_status;
 }}
 }
 if(!st){if(op==1)memcpy(read,actual,n);
 
 if(op==10){memset(read,0,4);
 
 read[0]=cfg;
 
 }z->completed_bytes=op==10?4:n;
 
 }free(work);
 
 free(actual);
 
 return st;
 
 
}
