/* Native baseline Flash adapter. Public membership remains disabled until
 * complete source/pin/power and installed GUI qualification is published. */
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include "../../NewDeviceLib/drafts/pic-population/baseline12_primary_power.h"
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define baseline_mkdir(p) _mkdir(p)
#define baseline_sync(f) _commit(_fileno(f))
#else
#include <unistd.h>
#define baseline_mkdir(p) mkdir(p,0700)
#define baseline_sync(f) fsync(fileno(f))
#endif
static const lt48_pic_profile pic_baseline12_source_profile={.strategy=LT48_PIC_COMPACT_ROWS};
static lt48_status pic_baseline12_open(const lt48_installation *x,const lt48_device_spec *spec,
 const lt48_pic_baseline12_profile *p,lt48_pic_resources **out){
 if(!x||!spec||!p||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 /* Explicit memory-programming tables govern VPP. Retain the DFP conflict
  * flag as provenance; an exact resolved binding/request is required. */
 if(!lt48_pic_baseline12_primary_vpp(p->definition,p->family,p->pinmap))return LT48_SAFETY_REJECTED;
 if(spec->catalogue.device_class!=2015||strcmp(spec->definition_stem,"GADC___")||spec->element_bits!=12)return LT48_UNSUPPORTED;
 if(spec->definition_resource!=p->definition||spec->family_code!=p->family||spec->pinmap!=p->pinmap||spec->capacity_bytes!=p->source_capacity||!lt48_pic_baseline12_valid(&p->policy))return LT48_SAFETY_REJECTED;
 lt48_pic_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
 r->baseline12=p;r->profile=&pic_baseline12_source_profile;r->capacity=8192;r->program_words=p->policy.code_words;r->core=12;
 char path[4096],name[32],hex[65];size_t idx,fi;unsigned char sha[32];lt48_status st=LT48_OK;
#define BTRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
 if(snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x))>=(int)sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
 BTRY(lt48_rez_open(path,&r->rez));BTRY(lt48_rez_sha256(r->rez,sha));for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",sha[i]);
 if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec")){st=LT48_SAFETY_REJECTED;goto fail;}
 snprintf(name,sizeof name,"P%x",p->pinmap);BTRY(lt48_rez_find_resource(r->rez,name,&idx));lt48_pin_code pc;
 BTRY(lt48_rez_get_pin_code(r->rez,idx,&pc));BTRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",p->family);BTRY(lt48_rez_find_resource(r->rez,name,&fi));lt48_family_info f;BTRY(lt48_rez_get_family_info(r->rez,fi,&f));
 if(f.variable_count!=p->variable_count||f.operation_count!=p->operation_count||f.variable_count>16){st=LT48_SAFETY_REJECTED;goto fail;}
 r->var_count=f.variable_count;
 for(unsigned i=0;i<f.variable_count;i++){lt48_family_variable v;BTRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
 BTRY(lt48_pic_baseline12_primary_power_validate(p->definition,p->family,p->pinmap,r->vars,r->var_count));
 for(unsigned i=0;i<f.operation_count;i++){
  lt48_family_operation op;BTRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));if(op.operation_id!=0&&op.operation_id!=1&&op.operation_id!=3&&op.operation_id!=4)continue;
  if(r->wave[op.operation_id]){st=LT48_SAFETY_REJECTED;goto fail;}
  snprintf(name,sizeof name,"W%x",op.wave);BTRY(lt48_rez_find_resource(r->rez,name,&idx));lt48_wave_code w;BTRY(lt48_rez_get_wave_code(r->rez,idx,&w));
  if(w.wave_type){st=LT48_UNSUPPORTED;goto fail;}
  lt48_compact_wave *raw=NULL;BTRY(lt48_compact_decode(w.data,w.data_size,&raw));
  st=lt48_compact_compile(raw,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(raw);if(st!=LT48_OK)goto fail;
  if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
 }
 if(!r->wave[0]||!r->wave[1]||!r->wave[3]||!r->wave[4]){st=LT48_SAFETY_REJECTED;goto fail;}
 BTRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_pic_close(r);return st;
#undef BTRY
}
typedef struct {pic_session s;classic_wire_context wire;lt48_pic_result *result;unsigned vcc;} baseline12_context;
static lt48_status baseline12_reenter(void *v){
 baseline12_context *c=v;pic_session *s=&c->s;s->mode=8;
 lt48_status st=reset(s);
 for(unsigned channel=4;st==LT48_OK&&channel;channel--){const unsigned char off[]={0x10,(unsigned char)(channel-1),0};st=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof off);}
 if(st==LT48_OK)st=classic_wire_delay(&c->wire,10000);
 if(st==LT48_OK)st=start(s,c->vcc);
 return st;
}
static lt48_status baseline12_transfer(void *v,int read,unsigned bits,uint32_t value,uint32_t *out){return classic_wire_transfer(&((baseline12_context*)v)->wire,read,bits,value,out);}
static lt48_status baseline12_delay(void *v,unsigned us){return classic_wire_delay(&((baseline12_context*)v)->wire,us);}
static lt48_status baseline12_select_voltage(void *v,unsigned field){
 baseline12_context *c=v;if(field!=0&&field!=5&&field!=6)return LT48_INVALID_ARGUMENT;
 unsigned value=p17_voltage(c->s.r,field);if(!value)return LT48_SAFETY_REJECTED;c->vcc=value;return LT48_OK;
}
static lt48_status baseline12_cleanup(void *v,lt48_status operation){baseline12_context *c=v;(void)finish(&c->s,operation,c->result);return c->result->cleanup_status;}
static lt48_status baseline12_directory(const char *path){
 if(!baseline_mkdir(path))return LT48_OK;
 struct stat info;if(errno==EEXIST&&!stat(path,&info)&&S_ISDIR(info.st_mode))return LT48_OK;return LT48_FILE_ERROR;
}
static lt48_status baseline12_save_factory(void *v,uint16_t primary,const uint16_t *backups,unsigned count){
 baseline12_context *c=v;const lt48_pic_baseline12_profile *p=c->s.r->baseline12;
 if(!backups||count!=p->policy.backup_words||count>4)return LT48_INVALID_ARGUMENT;
 char directory[4096],file[4096];const char *chosen=getenv("LT48_CALIBRATION_RECOVERY_DIR");
 if(chosen){if(snprintf(directory,sizeof directory,"%s",chosen)>=(int)sizeof directory)return LT48_FILE_ERROR;}
 else {
  const char *base=getenv("XDG_DATA_HOME"),*home=getenv("HOME");char root[4096];
  if(base){if(snprintf(root,sizeof root,"%s",base)>=(int)sizeof root)return LT48_FILE_ERROR;}
  else if(!home||snprintf(root,sizeof root,"%s/.local/share",home)>=(int)sizeof root)return LT48_FILE_ERROR;
  if(snprintf(directory,sizeof directory,"%s/labtool48",root)>=(int)sizeof directory)return LT48_FILE_ERROR;
  lt48_status st=baseline12_directory(directory);if(st!=LT48_OK)return st;
  size_t n=strlen(directory);if(n+sizeof "/calibration-recovery">sizeof directory)return LT48_FILE_ERROR;
  memcpy(directory+n,"/calibration-recovery",sizeof "/calibration-recovery");
 }
 lt48_status st=baseline12_directory(directory);if(st!=LT48_OK)return st;
 static unsigned sequence;FILE *stream=NULL;
 for(unsigned attempt=0;attempt<64&&!stream;attempt++){
  if(snprintf(file,sizeof file,"%s/baseline12-D%x-M%x-P%x-%lld-%u.json",directory,p->definition,p->family,p->pinmap,(long long)time(NULL),++sequence)>=(int)sizeof file)return LT48_FILE_ERROR;
  stream=fopen(file,"wx");if(!stream&&errno!=EEXIST)return LT48_FILE_ERROR;
 }
 if(!stream)return LT48_FILE_ERROR;
 int ok=fprintf(stream,"{\"schema\":2,\"definition\":%u,\"family\":%u,\"pinmap\":%u,\"code_words\":%u,\"primary\":%u,\"backup\":%u,\"backup_address\":%u,\"backups\":[",p->definition,p->family,p->pinmap,p->policy.code_words,primary,backups[0],p->policy.backup_address)>0;
 for(unsigned i=0;i<count;i++)if(fprintf(stream,"%s%u",i?",":"",backups[i])<0)ok=0;
 if(fprintf(stream,"],\"source_sha256\":\"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec\"}\n")<0)ok=0;
 if(fflush(stream)||baseline_sync(stream))ok=0;
 if(fclose(stream))ok=0;
#ifndef _WIN32
 if(ok){FILE *dir=fopen(directory,"r");if(!dir)ok=0;else {if(baseline_sync(dir))ok=0;fclose(dir);}}
#endif
 return ok?LT48_OK:LT48_FILE_ERROR;
}
static lt48_status pic_baseline12_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,lt48_pic_result *out){
 lt48_status power=lt48_pic_baseline12_primary_power_validate(r->baseline12->definition,
     r->baseline12->family,r->baseline12->pinmap,r->vars,r->var_count);
 if(power!=LT48_OK){out->operation_status=power;out->cleanup_status=LT48_OK;out->cleanup_completed=1;return power;}
 baseline12_context c={0};int upload_pending=0;c.s.r=r;c.s.api=api;c.s.context=context;c.s.mode=8;c.result=out;
 memcpy(c.s.vars,r->vars,sizeof c.s.vars);c.vcc=p17_voltage(r,0);
 c.s.image=malloc(8192);uint16_t *target=malloc(8192),*output=malloc(8192),*work=malloc(8192),*expected=malloc(8192);
 if(!c.s.image||!target||!output||!work||!expected){free(c.s.image);free(target);free(output);free(work);free(expected);return LT48_NO_MEMORY;}
 memset(c.s.image,255,8192);for(unsigned i=0;i<4096;i++)target[i]=image?word(image+2*i)&0xfff:0xfff;
 if(image&&r->baseline12->configuration_image_word!=0xfff){target[0xfff]=target[r->baseline12->configuration_image_word];target[r->baseline12->configuration_image_word]=0xfff;}
 lt48_logical_pin data,clock;lt48_status st=lt48_pin_map_find_logical(r->pins,r->baseline12->data_pin,&data);
 if(st==LT48_OK)st=lt48_pin_map_find_logical(r->pins,r->baseline12->clock_pin,&clock);
 if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,context,&c.s.wire);
 if(st==LT48_OK){c.wire=(classic_wire_context){&c.s,data.engine_pin,clock.engine_pin,c.vcc};
  for(unsigned ch=4;st==LT48_OK&&ch;ch--){const unsigned char off[]={0x10,(unsigned char)(ch-1),0};st=lt48_runtime_transport_session_controller_write(c.s.wire,off,sizeof off);}
  const unsigned char release[]={0x49,0xaa,0x55},fpga[]={0x38,0xaa,0x55},busy[]={0x56,0x0a,1,0x40,1};
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,release,sizeof release);
  if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c.s.wire);
  if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(c.s.wire,10000,1);
  if(st==LT48_OK){upload_pending=1;st=lt48_runtime_transport_session_controller_write(c.s.wire,fpga,sizeof fpga);}
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,r->idle,r->idle_size);
  if(st==LT48_OK)st=lt48_runtime_transport_session_flush(c.s.wire);
  if(st==LT48_OK)upload_pending=0;
  if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(c.s.wire,busy,sizeof busy);
 }
 lt48_pic_baseline12_result result={0};
 if(st==LT48_OK){lt48_pic_classic_wire wire={&c,baseline12_transfer,baseline12_delay,baseline12_reenter};
  unsigned levels[3],count=0;
  if(margin==LT48_VERIFY_MARGIN_ALL){levels[count++]=0;levels[count++]=5;levels[count++]=6;}
  else levels[count++]=margin==LT48_VERIFY_MARGIN_LOW?5:margin==LT48_VERIFY_MARGIN_HIGH?6:0;
  lt48_pic_baseline12_io io={.wire=&wire,.context=&c,.cleanup=baseline12_cleanup,.save_factory=baseline12_save_factory,
      .verify_levels=levels,.verify_count=count,.select_voltage=baseline12_select_voltage};
  st=lt48_pic_baseline12_execute(&r->baseline12->policy,&io,op,image?target:NULL,output,work,expected,&result);
  out->operation_status=result.operation_status;out->cleanup_status=result.cleanup_status;out->cleanup_completed=result.cleanup_status==LT48_OK;out->first_mismatch=result.first_mismatch==8190?2*r->baseline12->configuration_image_word:result.first_mismatch;out->mismatch_count=result.mismatches;
  if(st==LT48_OK&&op==LT48_OPERATION_READ){if(r->baseline12->configuration_image_word!=0xfff){output[r->baseline12->configuration_image_word]=output[0xfff];output[0xfff]=0xfff;}for(unsigned i=0;i<4096;i++)put_word(read+2*i,output[i]);}
 }else {out->operation_status=st;if(c.s.wire)st=finish(&c.s,st,out);
  /* An interrupted fixed-length FPGA stream can consume cleanup as payload.
   * Successful USB writes alone cannot confirm controller command alignment. */
  if(upload_pending){out->cleanup_completed=0;if(out->cleanup_status==LT48_OK)out->cleanup_status=LT48_PROTOCOL_ERROR;}
 }
 lt48_pic_source_session_close_internal(c.s.source);lt48_runtime_transport_session_close(c.s.wire);
 free(c.s.image);free(target);free(output);free(work);free(expected);return st;
}
#undef baseline_mkdir
#undef baseline_sync
