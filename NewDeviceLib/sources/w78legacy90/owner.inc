/* Legacy high-voltage W78/W77 owner. Source compact routines execute intact;

 * fresh raw reads independently check all mutation and comparison outcomes. */
static unsigned legacy_fields(const lt48_xp_eprom_resources*r){return r->legacy->flags+r->legacy->seeds;
}
static lt48_status validate_w78legacy90(const lt48_xp_eprom_resources*r,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size){
 if(!r||!r->legacy||!n||a>=r->capacity||n>r->capacity-a)return LT48_SAFETY_REJECTED;

 if((op<1||op>8)&&op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;

 if((op==3||op==4||op==7||op==8||op==10)&&(a||n!=r->capacity))return LT48_SAFETY_REJECTED;

 if(op==10){if(!im&&!size)return LT48_OK;
unsigned fields=legacy_fields(r),bytes=r->legacy->seeds?9:8;
if(!im||size!=bytes||(im[0]&~((1u<<fields)-1)))return LT48_INVALID_ARGUMENT;
for(unsigned i=0;
i<fields;
i++)if((im[0]&(1u<<i))&&i<r->legacy->flags&&im[i+1]>1)return LT48_INVALID_ARGUMENT;
}
 else if(op==8){/* Shared XP ABI ignores image data for Secure. */}
 else if(op==3||op==4||op==5||op==6){if(!im||size!=n)return LT48_INVALID_ARGUMENT;
}
 return LT48_OK;

}
static lt48_status legacy_settings(xp_session*s,uint32_t*out){unsigned f=legacy_fields(s->r);
s->mode=14;
memset(s->buffers[0],0,32);
for(unsigned i=0;
i<8;
i++)s->configuration[i]=UINT32_MAX;

 lt48_status st=run(s,2);
if(!st)st=run(s,6);
for(unsigned i=0;
!st&&i<f;
i++){if(s->configuration[i]>(i<s->r->legacy->flags?1u:255u))st=LT48_DEVICE_MISMATCH;
else out[i]=s->configuration[i];
}lt48_status reset=run(s,3);
return st?st:reset;

}
static lt48_status legacy_apply(xp_session*s,const uint32_t*saved,const uint32_t*want){unsigned f=legacy_fields(s->r),flags=s->r->legacy->flags;

 for(unsigned i=0;
i<f;
i++)if(i<flags?want[i]<saved[i]:(want[i]&saved[i])!=want[i])return LT48_SAFETY_REJECTED;

 s->mode=13;
memset(s->buffers[0],0,32);
lt48_status st=run(s,2);
for(unsigned i=0;
i<f;
i++)s->configuration[i]=want[i];
if(!st)st=run(s,11);
if(!st&&s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
lt48_status reset=run(s,3);
if(!st)st=reset;
uint32_t got[8]={0};
if(!st)st=legacy_settings(s,got);
if(!st&&memcmp(got,want,f*sizeof*got))st=LT48_DEVICE_MISMATCH;
return st;

}
static lt48_status legacy_compare(lt48_xp_eprom_result*z,uint32_t a,const unsigned char*got,const unsigned char*want,size_t n){z->mismatch_count=0;
z->first_mismatch=UINT32_MAX;
for(size_t i=0;
i<n;
i++)if(got[i]!=(want?want[i]:255)){if(!z->mismatch_count)z->first_mismatch=a+(unsigned)i;
z->mismatch_count++;
}return z->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status legacy_cleanup(void*v,unsigned phase){xp_session*s=v;

 if(phase==0){const unsigned char stop[]={0,0};
unsigned char reply[512]={0};
lt48_status st=cmd(s,stop,2,reply);
return !st&&!(reply[0]|reply[1])?LT48_PROTOCOL_ERROR:st;
}
 if(phase==1)return run(s,3);

 if(phase==2)return run(s,1);

 if(phase>=3&&phase<=6){unsigned char off[]={0x10,6-phase,0};
lt48_status st=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof off);
if(!st)st=lt48_runtime_transport_session_flush(s->wire);
return st;
}
 return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);

}
static lt48_status execute_w78legacy90(const lt48_xp_eprom_resources*r,const lt48_protocol_transport*api,void*ctx,lt48_operation_kind op,uint32_t a,size_t n,const unsigned char*im,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void*dc,lt48_xp_eprom_result*z){
 if(!z||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
memset(z,0,sizeof*z);
z->first_mismatch=UINT32_MAX;

 unsigned fields=legacy_fields(r),flags=r->legacy->flags;
lt48_status st=validate_w78legacy90(r,op,a,n,im,size);
if(!st)st=lt48_xp_eprom_validate_verify_options(r,op,margin,type);
if(!st&&((op==1&&(!read||read_size<n))||(op==10&&(!read||read_size<fields))))st=LT48_BUFFER_TOO_SMALL;
if(st)return st;

 if(api->cancelled&&api->cancelled(ctx)){z->operation_status=LT48_CANCELLED;
z->cleanup_completed=1;
return LT48_CANCELLED;
}
 unsigned char*actual=malloc(r->capacity);
if(!actual)return LT48_NO_MEMORY;

 xp_session s={0};
s.r=r;
s.api=api;
s.context=ctx;
s.mode=1;
s.protection=1;
memcpy(s.vars,r->vars,sizeof s.vars);
for(unsigned i=0;
i<4;
i++)s.buffers[1][i]=(unsigned char)(r->capacity>>(i*8));

 uint32_t saved[8]={0},want[8]={0};
st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
if(st){free(actual);
return st;
}st=setup(&s);
if(!st)st=identify(&s,decide,dc);
if(!st)st=legacy_settings(&s,saved);

 int writing=op==3||op==4,protected=0;
for(unsigned i=0;
i<(flags<3?flags:3);
i++)if(saved[i])protected=1;

 if(!st&&!writing&&op!=7&&op!=8&&op!=10&&protected)st=LT48_SAFETY_REJECTED;

 if(!st&&(op==8||op==10)){memcpy(want,saved,sizeof want);
if(op==8)want[0]=1;
else if(im)for(unsigned i=0;
i<fields;
i++)if(im[0]&(1u<<i))want[i]=im[i+1];
if(memcmp(want,saved,fields*sizeof*want))st=legacy_apply(&s,saved,want);
if(!st&&op==10)for(unsigned i=0;
i<fields;
i++)actual[i]=(unsigned char)want[i];
}
 else if(!st&&op==7){s.mode=7;
memset(s.buffers[0],0,32);
st=run(&s,2);
if(!st)st=run(&s,10);
if(!st&&s.buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
lt48_status reset=run(&s,3);
if(!st)st=reset;
if(!st)st=legacy_settings(&s,want);
for(unsigned i=0;
!st&&i<fields;
i++)if(want[i]!=(i<flags?0u:255u))st=LT48_DEVICE_MISMATCH;
if(!st)st=cycle(&s,1,0,r->capacity,NULL,actual,r->nominal,z);
if(!st)st=legacy_compare(z,0,actual,NULL,r->capacity);
}
 else if(!st){
  if(writing&&r->legacy->seeds&&!saved[0]){if(protected)st=LT48_SAFETY_REJECTED;
else st=cycle(&s,1,0,r->capacity,NULL,actual,r->nominal,z);
if(!st&&op==3)st=legacy_compare(z,0,actual,NULL,r->capacity);
if(!st&&op==4)for(unsigned i=0;
i<r->capacity;
i++)if((actual[i]&im[i])!=im[i]){st=LT48_SAFETY_REJECTED;
break;
}}
  if(!st&&writing)st=cycle(&s,2,0,n,im,NULL,r->nominal,z);

  unsigned passes=writing||(op==5&&margin==LT48_VERIFY_MARGIN_ALL)?2:1;

  for(unsigned pass=0;
!st&&pass<passes;
pass++){unsigned cv=r->nominal;
if(writing||(op==5&&margin!=LT48_VERIFY_MARGIN_NOMINAL))cv=margin_voltage(r,type,passes==2?pass:margin==LT48_VERIFY_MARGIN_HIGH);
st=cycle(&s,1,a,n,NULL,actual,cv,z);
if(!st&&op!=1)st=legacy_compare(z,a,actual,op==2?NULL:im,n);
}
  if(!st&&writing){st=legacy_settings(&s,want);
for(unsigned i=0;
!st&&i<(flags<3?flags:3);
i++)if(want[i])st=LT48_DEVICE_MISMATCH;
uint32_t restored[8];
memcpy(restored,want,sizeof restored);
if(r->legacy->seeds)for(unsigned i=flags;
i<fields;
i++)restored[i]=saved[i];
else if(flags==4)restored[3]=saved[3];
if(!st&&memcmp(restored,want,fields*sizeof*want))st=legacy_apply(&s,want,restored);
}
 }
 z->operation_status=st;
lt48_protocol_transport clean=*api;
clean.cancelled=NULL;
s.api=&clean;
const unsigned phases[]={0,1,2,3,4,5,6,7};
st=lt48_owner_finish_internal(&clean,ctx,&s.wire,legacy_cleanup,&s,phases,8,st,&z->cleanup_status,&z->cleanup_completed);
if(!st){if(op==1)memcpy(read,actual,n);
if(op==10)memcpy(read,actual,fields);
z->completed_bytes=op==10?fields:n;
}free(actual);
return st;

}
