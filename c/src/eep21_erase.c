/* Authored operation owner for source-bound, readback-driven W27 erase.
 * Six memory operations remain owned by the shared M37 stream engine. */
#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdlib.h>
#include <string.h>
typedef struct eep_session {const lt48_m37_resources*r;
lt48_runtime_transport_session*wire;
lt48_applet_address app[8];
size_t apps;
unsigned char buffers[70][32];
uint32_t volts[4];
} eep_session;

int lt48_m37_has_dynamic_erase(const lt48_m37_resources*r){const lt48_m37_resource_audit*a=lt48_m37_resources_audit(r);
return lt48_m37_resources_is_eep21(r)&&((a->device_type>=5&&a->device_type<=14&&a->device_type!=6)||a->device_type==0x3a||a->device_type==0x3b||a->device_type==0x26||a->device_type==0x27||a->device_type==0x28);
}
lt48_status lt48_m37_dynamic_erase_validate(const lt48_m37_resources*r,uint32_t start,uint32_t length){const lt48_m37_resource_audit*a=lt48_m37_resources_audit(r);
return !a||!lt48_m37_has_dynamic_erase(r)?LT48_UNSUPPORTED:start||length!=a->capacity_bytes?LT48_SAFETY_REJECTED:LT48_OK;
}
static lt48_status eep_run(eep_session*s,unsigned op){const lt48_compiled_wave*w;
uint16_t id;
lt48_status st=lt48_m37_resources_compiled_wave(s->r,op,&id,&w);
if(st)return st;

 s->buffers[2][0]=7;
s->buffers[2][1]=s->buffers[2][2]=s->buffers[2][3]=0;

 lt48_runtime_options o={0};
lt48_runtime_buffer_seed seeds[70];
for(unsigned i=0;i<70;i++)seeds[i]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
o.mode=7;
o.use_explicit_buffer_ids=1;
o.mode_buffer=2;
o.address_buffer=0;
o.parameter_buffer_id=1;
o.initial_buffers=seeds;
o.initial_buffer_count=70;
o.host_staged_shutdown=1;
o.host_m37_erase_notifications=1;
o.root_timeout_seconds=op==4||op==1?10:30;
memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);

 size_t nv;
const lt48_compile_variable*v=lt48_m37_resources_variables(s->r,&nv);
lt48_runtime_machine*m=NULL;
lt48_runtime_result*out=NULL;
st=lt48_runtime_machine_create(w,lt48_m37_resources_pin_map(s->r),v,nv,&o,&m);
if(!st)st=lt48_runtime_transport_session_run_with_applets(s->wire,m,s->app,s->apps,&out);

 if(!st){for(unsigned i=0;i<70;i++){const unsigned char*b;
size_t n;
st=lt48_runtime_get_buffer(out,i,&b,&n);
if(st||n!=32){st=LT48_RUNTIME_ERROR;
break;
}memcpy(s->buffers[i],b,32);
}for(unsigned i=0;!st&&i<4;i++)st=lt48_runtime_get_voltage(out,i,&s->volts[i]);

  for(size_t i=0;!st&&i<lt48_runtime_event_count(out);i++){lt48_runtime_event e;
lt48_runtime_get_event(out,i,&e);
if(e.kind==LT48_RUNTIME_RESULT_LATCH||(e.kind==LT48_RUNTIME_RETURN_STATUS&&e.argument_count==1&&!e.arguments[0]))st=LT48_DEVICE_MISMATCH;
}}
 if(!st&&op==6){for(size_t i=0;i<s->apps;i++)if(s->app[i].wave==12&&s->buffers[9][0]!=1)st=LT48_DEVICE_MISMATCH;}
 lt48_runtime_result_close(out);
lt48_runtime_machine_close(m);
return st;
}
static lt48_status eep_send(eep_session*s,const unsigned char*p,size_t n){return lt48_runtime_transport_session_controller_write(s->wire,p,n);
}
static lt48_status noop_release(void*p){(void)p;
return LT48_OK;
}
lt48_status lt48_m37_dynamic_erase_execute(const lt48_m37_resources*r,const lt48_protocol_transport*api,void*ctx,uint32_t start,uint32_t length,lt48_small_eprom_result*out){
 if(!api||!api->write||!api->read||!out)return LT48_INVALID_ARGUMENT;
memset(out,0,sizeof *out);
lt48_status st=lt48_m37_dynamic_erase_validate(r,start,length);
if(st){out->operation_status=st;return st;}

 eep_session s={0};
s.r=r;
lt48_m37_applet_resource a[8];
s.apps=lt48_m37_resources_applet_count(r);
if(s.apps>8)return LT48_SAFETY_REJECTED;

 for(size_t i=0;i<s.apps;i++){st=lt48_m37_resources_get_applet(r,i,&a[i]);
if(st)return st;
for(size_t j=i;j>0&&a[j-1].wave>a[j].wave;j--){lt48_m37_applet_resource t=a[j];
a[j]=a[j-1];
a[j-1]=t;
}}
 st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
if(st)return st;

 static const unsigned char init0[]={0x11,0xaa,0x55,0,0,2},init1[]={0x11,0xaa,0x55,1,0,2},fh[]={0x38,0xaa,0x55},busy[]={0x56,0x0a,1,0x40,1};

 st=eep_send(&s,init0,sizeof init0);
if(!st)st=lt48_runtime_transport_session_flush(s.wire);
if(!st)st=eep_send(&s,init1,sizeof init1);
if(!st)st=lt48_runtime_transport_session_flush(s.wire);

 for(size_t i=0;!st&&i<s.apps;i++){if(!a[i].data_size||a[i].data_size>65536){st=LT48_SAFETY_REJECTED;
break;
}unsigned last=a[i].data_size-1;
unsigned char h[]={0x39,0xaa,0x55,a[i].load_address,a[i].load_address>>8,last,last>>8};
st=eep_send(&s,h,sizeof h);
if(!st)st=eep_send(&s,a[i].data,a[i].data_size);
s.app[i]=(lt48_applet_address){a[i].operation_id,a[i].load_address};
}
 const unsigned char*fpga;
size_t size;
if(!st)st=lt48_m37_resources_active_fpga(r,&fpga,&size);
if(!st)st=eep_send(&s,fh,sizeof fh);
if(!st)st=eep_send(&s,fpga,size);
const lt48_m37_resource_audit*au=lt48_m37_resources_audit(r);
unsigned char sel[]={0x0f,au->expected_vcc_selector};
if(!st)st=eep_send(&s,sel,sizeof sel);
if(!st)st=eep_run(&s,0);
for(unsigned i=0;!st&&i<2;i++)st=lt48_runtime_transport_session_status(s.wire,1);
if(!st)st=eep_send(&s,busy,sizeof busy);

 if(!st)st=eep_run(&s,3);
if(!st)st=eep_run(&s,6);
if(!st)st=eep_run(&s,4);

 out->operation_status=st;
lt48_runtime_transport_session_close(s.wire);
s.wire=NULL;

 /* Cleanup uses a new uncancelled queue even after a failed capture. Each
  * emergency DAC command is attempted independently. Never retry erase. */
 lt48_protocol_transport clean=*api;
clean.cancelled=NULL;
lt48_status cs=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);

 if(!cs&&st==LT48_OK)cs=eep_run(&s,1);

 for(unsigned ch=0;ch<4;ch++){lt48_runtime_transport_session_close(s.wire);
s.wire=NULL;
lt48_status t=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
unsigned char off[]={0x10,3-ch,0};
if(!t)t=eep_send(&s,off,sizeof off);
if(!t)t=lt48_runtime_transport_session_flush(s.wire);
if(!cs&&t)cs=t;
}
 if(st!=LT48_OK){memset(s.volts,0,sizeof s.volts);
lt48_status t=eep_run(&s,1);
if(!cs&&t)cs=t;
}
 lt48_status t=lt48_m37_resources_idle_fpga(r,&fpga,&size);
if(!t)t=eep_send(&s,fh,sizeof fh);
if(!t)t=eep_send(&s,fpga,size);
static const unsigned char reset[]={0x49,0xaa,0x55};
if(!t)t=eep_send(&s,reset,sizeof reset);
if(!t)t=lt48_runtime_transport_session_flush(s.wire);
if(!cs&&t)cs=t;
lt48_runtime_transport_session_close(s.wire);
s.wire=NULL;
out->cleanup_status=cs;
out->cleanup_completed=!cs;

 if(!st&&cs)st=LT48_CLEANUP_FAILED;

 if(!st){lt48_protocol_plan*q=NULL;
st=lt48_m37_resources_build_blank_check_plan(r,0,length,&q);
lt48_cleanup_action release={"release-interface",noop_release,NULL};
lt48_cleanup_result cr[12];
lt48_m37_snapshot_read_execution_result v={0};

  if(!st)st=lt48_m37_snapshot_read_execute_injected(r,q,api,ctx,NULL,&release,cr,12,&v);
lt48_protocol_plan_close(q);
if(v.finalization_status){out->cleanup_status=v.finalization_status;
out->cleanup_completed=0;
}if(!st)out->completed_bytes=length;
}
 /* The shared verifier restores the idle pin configuration. Explicitly zero
  * disconnected internal DACs as well; retain the operation's first failure. */
 for(unsigned ch=0;ch<4;ch++){
  lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
  lt48_status z=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
  unsigned char off[]={0x10,3-ch,0};
  if(!z)z=eep_send(&s,off,sizeof off);
  if(!z)z=lt48_runtime_transport_session_flush(s.wire);
  if(z){if(out->cleanup_status==LT48_OK)out->cleanup_status=z;out->cleanup_completed=0;if(st==LT48_OK)st=LT48_CLEANUP_FAILED;}
 }
 lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
 if(st!=LT48_OK){lt48_status t2=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
unsigned char error[]={0x40,2};
if(!t2)t2=eep_send(&s,error,sizeof error);
if(!t2)t2=lt48_runtime_transport_session_flush(s.wire);
lt48_runtime_transport_session_close(s.wire);
if(t2){out->cleanup_status=t2;
out->cleanup_completed=0;
}}
 out->operation_status=st;
return st;

}
