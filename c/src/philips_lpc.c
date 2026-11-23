#include "philips_lpc.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LPC_IMAGE 0xfd00u
void lt48_lpc_close(lt48_lpc_resources *r){if(!r)return;for(unsigned i=0;i<24;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);}
lt48_status lt48_lpc_open(const lt48_installation *x,size_t device,lt48_lpc_resources **out){
 static const unsigned tuples[][4]={{0x68,0x53,0x18,1024},{0x69,0x54,0x19,2048},{0x6a,0x55,0x1a,2048},{0x6b,0x55,0x1b,2048},{0x6c,0x56,0x1a,4096},{0x6d,0x56,0x1b,4096},{0x6e,0x57,0x1a,8192}};
 if(!x || !out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_device_spec s;lt48_status status=lt48_installation_resolve_device(x,device,&s);if(status!=LT48_OK)return status;
 if(s.catalogue.device_class!=2010 || strcmp(s.definition_stem,"GAB____") || s.definition_resource<0x68 || s.definition_resource>0x6e)return LT48_UNSUPPORTED;
 const unsigned *t=tuples[s.definition_resource-0x68];if(s.family_code!=t[1] || s.pinmap!=t[2])return LT48_SAFETY_REJECTED;
 lt48_lpc_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;r->definition=t[0];r->family=t[1];r->pinmap=t[2];r->code_size=t[3];
 char path[4096],name[24];size_t index,fi;
 int n=snprintf(path,sizeof path,"%s/GAB____.rez",lt48_installation_path(x));if(n<0 || (size_t)n>=sizeof path){status=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{status=(x);if(status!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9",&r->rez));
 lt48_device_definition d;snprintf(name,sizeof name,"D%x",r->definition);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_device_definition(r->rez,index,&d));
 if(d.kind!=LT48_DEVICE_DEFINITION_MEMORY || !d.has_element_geometry || d.element_count!=LPC_IMAGE || d.element_bits!=8 || d.auxiliary_value!=LPC_IMAGE){status=LT48_FORMAT_ERROR;goto fail;}
 lt48_pin_code p;snprintf(name,sizeof name,"P%x",r->pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&p));TRY(lt48_pin_map_parse(&p,&r->pins));
 lt48_family_info f;snprintf(name,sizeof name,"M%x",r->family);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count!=5 || f.operation_count!=8){status=LT48_FORMAT_ERROR;goto fail;}
 for(unsigned i=0;i<5;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 const unsigned ids[]={0,1,2,3,4,5,7,23},waves[]={151,2,152,153,154,155,156,157};unsigned seen=0;
 for(unsigned i=0;i<8;i++){
  lt48_family_operation op;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));unsigned j=0;for(;j<8 && ids[j]!=op.operation_id;j++);
  if(j==8 || (seen&(1u<<j)) || op.wave!=waves[j]){status=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<j;
  lt48_wave_code w;snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));if(w.wave_type){status=LT48_FORMAT_ERROR;goto fail;}
  lt48_compact_wave *compact=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&compact));status=lt48_compact_compile(compact,r->pins,r->vars,5,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
  if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){status=LT48_SAFETY_REJECTED;goto fail;}
 }
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_lpc_close(r);return status;
#undef TRY
}
static int physical(const lt48_lpc_resources *r,uint32_t address){return address<r->code_size || (address>=0xfce0 && address<LPC_IMAGE);}
lt48_status lt48_lpc_validate(const lt48_lpc_resources *r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *image,size_t size){
 if(!r)return LT48_INVALID_ARGUMENT;
 if((op<1 || op>8 || op==LT48_OPERATION_ERASE) && op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
 if(!n || start>=LPC_IMAGE || n>LPC_IMAGE-start)return LT48_SAFETY_REJECTED;
 if(op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE){
  if(start || n!=LPC_IMAGE)return LT48_SAFETY_REJECTED;
  if(!image && !size)return LT48_OK;
  if(!image)return LT48_INVALID_ARGUMENT;
  if(op==LT48_OPERATION_SECURE)return size==1 && image[0]>=1 && image[0]<=2?LT48_OK:LT48_INVALID_ARGUMENT;
  if(size!=8 || image[0]&~127u)return LT48_INVALID_ARGUMENT;
  for(unsigned i=0;i<7;i++)if((image[0]&(1u<<i)) && image[i+1]>(i<5?1u:i==5?4u:2u))return LT48_INVALID_ARGUMENT;
 }else if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE){
  if(!image || size!=n)return LT48_INVALID_ARGUMENT;
  for(size_t i=0;i<n;i++)if(!physical(r,start+(uint32_t)i) && image[i]!=255)return LT48_SAFETY_REJECTED;
 }
 return LT48_OK;
}
typedef struct lpc_state {
 const lt48_lpc_resources *r;lt48_runtime_transport_session *wire;lt48_compile_variable vars[5];
 unsigned char buffers[32][32];lt48_runtime_configuration_value cfg[8];size_t cfg_count;uint32_t volts[4];unsigned mode;
 lt48_m37_id_decision_fn decide;void *decision_context;int ignore;
} lpc_state;
static void voltage(lpc_state *s,unsigned cv){s->vars[0].value=cv;}
static void seed(lpc_state *s,const unsigned char *v){
 const unsigned fields[]={10,9,8,7,6,3,2};s->cfg_count=8;s->cfg[0]=(lt48_runtime_configuration_value){-1,0,1,s->ignore?0:1};
 for(unsigned i=0;i<7;i++)s->cfg[i+1]=(lt48_runtime_configuration_value){0,i<5?0:1,fields[i],v?v[i]:UINT32_MAX};
}
static lt48_status step(lpc_state *s,unsigned op){
 for(unsigned retry=0;retry<64;retry++){
  lt48_runtime_buffer_seed seeds[31];size_t count=0;for(unsigned i=0;i<32;i++)if(i!=4)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
  lt48_runtime_nested_wave nested={23,s->r->wave[23]};lt48_runtime_options o={0};o.mode=s->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=4;o.parameter_buffer_id=1;
  o.initial_buffers=seeds;o.initial_buffer_count=count;o.configuration=s->cfg;o.configuration_count=s->cfg_count;o.nested_waves=&nested;o.nested_wave_count=1;o.root_timeout_seconds=10;memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
  lt48_runtime_result *diagnostic=NULL;
  lt48_status status=lt48_owner_run_result_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,5,&o,NULL,0,s->buffers,s->cfg,8,&s->cfg_count,s->volts,&diagnostic);
  if(diagnostic && status==LT48_DEVICE_MISMATCH){const unsigned char *bytes;size_t length;if(lt48_runtime_get_buffer(diagnostic,22,&bytes,&length)==LT48_OK && length)s->buffers[22][0]=bytes[0];}
  lt48_runtime_result_close(diagnostic);
  if(op!=2 || status!=LT48_DEVICE_MISMATCH || !s->decide || s->ignore)return status;
  /* W98's only failure latch is its family signature check. The source
   * accepts the listed family IDs, not a per-catalogue exact model ID. */
  unsigned actual=s->buffers[22][0];lt48_m37_id_choice choice=s->decide(s->decision_context,0,0x15dd,0,(uint16_t)actual);
  if(choice==LT48_M37_ID_ABORT)return status;
  if(choice!=LT48_M37_ID_RETRY && choice!=LT48_M37_ID_IGNORE)return LT48_INVALID_ARGUMENT;
  status=step(s,3);if(status!=LT48_OK)return status;
  if(choice==LT48_M37_ID_IGNORE){s->ignore=1;s->cfg[0].value=0;}
 }
 return LT48_DEVICE_MISMATCH;
}
static void address(lpc_state *s,uint32_t a){for(unsigned i=0;i<4;i++)s->buffers[0][i]=(unsigned char)(a>>(8*i));}
static lt48_status memory(lpc_state *s,int program,uint32_t start,size_t n,const unsigned char *input,unsigned char *out){
 memset(s->buffers,0,sizeof s->buffers);s->mode=program?2:1;address(s,start);seed(s,NULL);
 lt48_status st=step(s,2);
 for(size_t i=0;st==LT48_OK && i<n;i++){
  uint32_t a=start+(uint32_t)i;address(s,a);
  if(!physical(s->r,a)){if(!program)out[i]=255;continue;}
  if(program)s->buffers[2][0]=input[i];
  st=step(s,program?5:4);if(st==LT48_OK && !program)out[i]=s->buffers[1][0];
 }
 lt48_status reset=step(s,3);return st==LT48_OK?reset:st;
}
/* Field order: CLKR,BOV,PRHI,RPD,WDTE; oscillator choice; security level. */
static void encode(const unsigned char *v,unsigned char raw[2]){
 static const unsigned osc[]={7,3,2,1,0},lock[]={3,2,0};raw[0]=(unsigned char)osc[v[5]];for(unsigned i=0;i<5;i++)raw[0]|=v[i]<<(i+3);raw[1]=(unsigned char)(63|(lock[v[6]]<<6));
}
static lt48_status settings(lpc_state *s,const unsigned char *desired,unsigned char actual[7]){
 lt48_status st=LT48_OK;
 if(desired){memset(s->buffers,0,sizeof s->buffers);s->mode=8;seed(s,desired);st=step(s,2);if(st==LT48_OK)st=step(s,7);lt48_status reset=step(s,3);if(st==LT48_OK)st=reset;}
 if(st!=LT48_OK)return st;
 memset(s->buffers,0,sizeof s->buffers);s->mode=1;seed(s,NULL);st=step(s,2);
 /* DEVINIT calls LoadConfig then reuses buffer29 for a second FD00 read.
  * Reject reserved oscillator/security encodings, instead of trusting the
  * original decoder's leftover scratch value for undefined combinations. */
 unsigned raw=s->buffers[29][0],sec=s->buffers[30][0]>>6;
 if(st==LT48_OK && ((raw&7)==4 || (raw&7)==5 || (raw&7)==6 || sec==1))st=LT48_DEVICE_MISMATCH;
 unsigned char observed[7];
 for(unsigned i=0;st==LT48_OK && i<7;i++){unsigned v=s->cfg[i+1].value;if(v>(i<5?1u:i==5?4u:2u))st=LT48_DEVICE_MISMATCH;else observed[i]=(unsigned char)v;}
 if(st==LT48_OK && desired && memcmp(desired,observed,7))st=LT48_DEVICE_MISMATCH;
 lt48_status reset=step(s,3);if(st==LT48_OK)st=reset;if(st==LT48_OK)memcpy(actual,observed,7);return st;
}
static lt48_status compare(lt48_serial_page_result *r,uint32_t start,const unsigned char *a,const unsigned char *b,size_t n){
 for(size_t i=0;i<n;i++)if(a[i]!=(b?b[i]:255)){if(!r->mismatch_count)r->first_mismatch=start+(uint32_t)i;r->mismatch_count++;}
 return r->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status cleanup(void *v,unsigned phase){lpc_state *s=v;if(phase==0)return step(s,3);if(phase==1)return step(s,1);return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);}
lt48_status lt48_lpc_execute(const lt48_lpc_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char *input,size_t size,unsigned char *read,size_t capacity,lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,lt48_serial_page_result *result){
 if(!result || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
 memset(result,0,sizeof *result);result->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_lpc_validate(r,op,start,n,input,size);
 if(st==LT48_OK && (margin_type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK && ((op==LT48_OPERATION_READ && (!read || capacity<n)) || (op==LT48_OPERATION_CONFIGURATION && (!read || capacity<7))))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){result->operation_status=st;return st;}if(api->cancelled && api->cancelled(context)){result->operation_status=LT48_CANCELLED;result->cleanup_completed=1;return LT48_CANCELLED;}
 unsigned char *actual=malloc(n);if(!actual){result->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 lpc_state s={0};s.r=r;s.decide=decide;s.decision_context=decision_context;memcpy(s.vars,r->vars,sizeof s.vars);seed(&s,NULL);
 st=lt48_runtime_transport_session_create(api,context,&s.wire);int started=st==LT48_OK;
 if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
 if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
 if(st==LT48_OK)st=step(&s,0);
 for(unsigned i=0;st==LT48_OK && i<3;i++)st=lt48_runtime_transport_session_status(s.wire,1);
 const unsigned char busy[]={0x56,0x0a,1,0x40,1};if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof busy);if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
 int uncertain=started && st!=LT48_OK;unsigned char saved[7],desired[7];
 if(st==LT48_OK)st=settings(&s,NULL,saved);
 int program=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
 if(st==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)){
  memcpy(desired,saved,7);if(op==LT48_OPERATION_SECURE){unsigned lock=input?input[0]:2;if(lock>desired[6])desired[6]=(unsigned char)lock;}
  else if(input)for(unsigned i=0;i<7;i++)if(input[0]&(1u<<i))desired[i]=input[i+1];
  unsigned char oldraw[2],newraw[2];encode(saved,oldraw);encode(desired,newraw);
  if((oldraw[0]&newraw[0])!=newraw[0] || (oldraw[1]&newraw[1])!=newraw[1] || (saved[6] && memcmp(saved,desired,6)))st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK && memcmp(saved,desired,7))st=settings(&s,desired,saved);
 }else if(st==LT48_OK){
  if(saved[6]>=2 || (program && saved[6]))st=LT48_SAFETY_REJECTED;
  if(st==LT48_OK && program){
   st=memory(&s,0,start,n,NULL,actual);
   for(size_t i=0;st==LT48_OK && i<n;i++)if((actual[i]&input[i])!=input[i])st=LT48_SAFETY_REJECTED;
   if(st==LT48_OK)st=memory(&s,1,start,n,input,NULL);
  }
  unsigned cv[]={margin_type?450u:475u,margin_type?550u:525u};unsigned passes=program || (op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL)?2:1;
  if(!program){if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL)cv[0]=500;else if(margin==LT48_VERIFY_MARGIN_HIGH)cv[0]=cv[1];}
  for(unsigned i=0;st==LT48_OK && i<passes;i++){voltage(&s,cv[i]);st=memory(&s,0,start,n,NULL,actual);if(st==LT48_OK && op!=LT48_OPERATION_READ)st=compare(result,start,actual,op==LT48_OPERATION_BLANK_CHECK?NULL:input,n);}
 }
 if(st==LT48_OK){voltage(&s,500);st=settings(&s,NULL,desired);if(st==LT48_OK && memcmp(saved,desired,7))st=LT48_DEVICE_MISMATCH;}
 if(st==LT48_OK && api->cancelled && api->cancelled(context))st=LT48_CANCELLED;
 result->operation_status=st;
 if(started){s.decide=NULL;const unsigned phases[]={0,1,2};st=lt48_owner_finish_internal(api,context,&s.wire,cleanup,&s,phases,3,st,&result->cleanup_status,&result->cleanup_completed);}else {lt48_runtime_transport_session_close(s.wire);result->cleanup_completed=1;}
 if(uncertain){result->cleanup_status=LT48_CLEANUP_FAILED;result->cleanup_completed=0;}
 if(st==LT48_OK){if(op==LT48_OPERATION_READ)memcpy(read,actual,n);if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,desired,7);result->completed_bytes=op==LT48_OPERATION_CONFIGURATION?7:n;}
 free(actual);return st;
}
