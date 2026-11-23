#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/* Ordinary QT/QY source-bound operation owner. No device admission here.
 * Original payloads are loaded from the user's exact REZ, never embedded. */
#include "../../../c/src/hc908.h"
#include "../../../c/src/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "monitor_extensions.h"
#include "admission.h"
#include "members.h"
#include "ram_helpers.h"
#include "jk_jl_profiles.h"
#include "jk_jl_members.h"
#include "gxjb_profiles.h"
#include "gxjb_members.h"
#include "grkx_profiles.h"
#include "grkx_members.h"
#include "apsr_profiles.h"
#include "apsr_members.h"
#include "gpgz_profiles.h"
#include "abasaz_profiles.h"
#include "az60_clock_patch.h"
#include "fqtqy_profiles.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <errno.h>
#endif

struct lt48_hc908_resources {
 lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[20];
 lt48_compile_variable vars[6];lt48_wave_code workers[4];
 lt48_applet_address addresses[4];const unsigned char *fpga;
 unsigned char *idle,*derived_fpga;size_t idle_size;unsigned definition,first,variable_count;
 unsigned char security_key[8];const struct hc908_f_profile *f;int grkx,apsr,gpgz,abasaz;const struct hc908_jk_profile *jk;const struct hc908_gx_profile *gx;
};
int lt48_hc908_is_fqtqy(const lt48_hc908_resources *r){return r&&r->f;}
int lt48_hc908_is_abasaz(const lt48_hc908_resources *r){return r&&r->abasaz;}
int lt48_hc908_is_gpgz(const lt48_hc908_resources *r){return r&&r->gpgz;}
unsigned lt48_hc908_blank_value(const lt48_hc908_resources *r){return r&&((r->gpgz&&r->definition<=0x25)||(r->abasaz&&r->definition==0x37))?0:255;}
int lt48_hc908_is_gxjb(const lt48_hc908_resources *r){return r&&r->gx&&!r->grkx&&!r->apsr&&!r->gpgz&&!r->abasaz;}
int lt48_hc908_is_apsr(const lt48_hc908_resources *r){return r&&r->apsr;}
int lt48_hc908_is_grkx(const lt48_hc908_resources *r){return r&&r->grkx;}
int lt48_hc908_is_jkjl(const lt48_hc908_resources *r){return r&&r->jk;}
void lt48_hc908_close(lt48_hc908_resources *r) {
 if(!r)return;
 for(unsigned i=0;i<20;i++)lt48_compiled_wave_close(r->wave[i]);
 lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r->derived_fpga);free(r);
}
unsigned lt48_hc908_flash_start(const lt48_hc908_resources *r){return r?r->first:0;}
lt48_status lt48_hc908_set_security_key(lt48_hc908_resources *r,const unsigned char *key,size_t n){
 if(!r||(!key&&n)|| (n&&n!=8))return LT48_INVALID_ARGUMENT;
 if(n)memcpy(r->security_key,key,8);else memset(r->security_key,lt48_hc908_blank_value(r),8);
 return LT48_OK;
}

lt48_status lt48_hc908_open_candidate(const lt48_installation *x,size_t device,lt48_hc908_resources **out) {
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_device_spec s;lt48_status st=lt48_installation_resolve_device(x,device,&s);
 if(st!=LT48_OK)return st;
 static const unsigned tuples[][3]={{0x48,0x46,0x34},{0x49,0x47,0x34},{0x4a,0x48,0x35},{0x4b,0x49,0x36},{0x4c,0x4a,0x35},{0x4d,0x4b,0x36}};
 const struct hc908_f_profile *fp=NULL;
 int found=0,grkx=0,apsr=0,gpgz=0,abasaz=0;const struct hc908_jk_profile *jk=NULL;const struct hc908_gx_profile *gx=NULL;
 for(size_t i=0;i<sizeof hc908_jk_profiles/sizeof *hc908_jk_profiles;i++){const struct hc908_jk_profile *p=hc908_jk_profiles+i;if(s.definition_resource==p->definition&&s.family_code==p->family&&s.pinmap==p->pinmap){jk=p;found=1;}}
 for(size_t i=0;i<sizeof hc908_gx_profiles/sizeof *hc908_gx_profiles;i++){const struct hc908_gx_profile *p=hc908_gx_profiles+i;if(s.definition_resource==p->serial.definition&&s.family_code==p->serial.family&&s.pinmap==p->serial.pinmap){gx=p;jk=&p->serial;found=1;}}
 for(size_t i=0;i<sizeof hc908_grkx_profiles/sizeof *hc908_grkx_profiles;i++){const struct hc908_gx_profile *p=hc908_grkx_profiles+i;if(s.definition_resource==p->serial.definition&&s.family_code==p->serial.family&&s.pinmap==p->serial.pinmap){gx=p;jk=&p->serial;found=1;grkx=1;}}
 for(size_t i=0;i<sizeof hc908_apsr_profiles/sizeof *hc908_apsr_profiles;i++){const struct hc908_gx_profile *p=hc908_apsr_profiles+i;if(s.definition_resource==p->serial.definition&&s.family_code==p->serial.family&&s.pinmap==p->serial.pinmap){gx=p;jk=&p->serial;found=1;apsr=1;}}
 for(size_t i=0;i<sizeof hc908_gpgz_profiles/sizeof *hc908_gpgz_profiles;i++){const struct hc908_gx_profile *p=hc908_gpgz_profiles+i;if(s.definition_resource==p->serial.definition&&s.family_code==p->serial.family&&s.pinmap==p->serial.pinmap){gx=p;jk=&p->serial;found=1;gpgz=1;}}
 for(size_t i=0;i<sizeof hc908_abasaz_profiles/sizeof *hc908_abasaz_profiles;i++){const struct hc908_gx_profile *p=hc908_abasaz_profiles+i;if(s.definition_resource==p->serial.definition&&s.family_code==p->serial.family&&s.pinmap==p->serial.pinmap){gx=p;jk=&p->serial;found=1;abasaz=1;}}
 for(size_t i=0;i<sizeof hc908_f_profiles/sizeof *hc908_f_profiles;i++){const struct hc908_f_profile *p=hc908_f_profiles+i;if(s.definition_resource==p->definition&&s.family_code==p->family&&s.pinmap==p->pinmap){fp=p;found=1;}}
 for(size_t i=0;i<sizeof tuples/sizeof *tuples;i++)if(s.definition_resource==tuples[i][0]&&s.family_code==tuples[i][1]&&s.pinmap==tuples[i][2])found=1;
 if(!found||s.catalogue.device_class!=2014||strcmp(s.definition_stem,"GADB___")||s.capacity_bytes!=65536||s.element_bits!=8)return LT48_UNSUPPORTED;
 if(!s.catalogue.vendor||(strcmp(s.catalogue.vendor,"Motorola")&&strcmp(s.catalogue.vendor,"Freescale"))||s.catalogue.manufacturer_id||s.catalogue.device_id)return LT48_UNSUPPORTED;
 int named=0;
 for(size_t i=0;i<sizeof(hc908_members)/sizeof(*hc908_members);i++)if(s.definition_resource==hc908_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_members[i].name,s.catalogue.device_name_length))named=1;
 if(jk)for(size_t i=0;i<sizeof hc908_jk_members/sizeof *hc908_jk_members;i++)if(s.definition_resource==hc908_jk_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_jk_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_jk_members[i].name,s.catalogue.device_name_length))named=1;
 if(gx)for(size_t i=0;i<sizeof hc908_gx_members/sizeof *hc908_gx_members;i++)if(s.definition_resource==hc908_gx_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_gx_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_gx_members[i].name,s.catalogue.device_name_length))named=1;
 if(grkx)for(size_t i=0;i<sizeof hc908_grkx_members/sizeof *hc908_grkx_members;i++)if(s.definition_resource==hc908_grkx_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_grkx_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_grkx_members[i].name,s.catalogue.device_name_length))named=1;
 if(apsr)for(size_t i=0;i<sizeof hc908_apsr_members/sizeof *hc908_apsr_members;i++)if(s.definition_resource==hc908_apsr_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_apsr_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_apsr_members[i].name,s.catalogue.device_name_length))named=1;
 if(gpgz)for(size_t i=0;i<sizeof hc908_gpgz_members/sizeof *hc908_gpgz_members;i++)if(s.definition_resource==hc908_gpgz_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_gpgz_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_gpgz_members[i].name,s.catalogue.device_name_length))named=1;
 if(abasaz)for(size_t i=0;i<sizeof hc908_abasaz_members/sizeof *hc908_abasaz_members;i++)if(s.definition_resource==hc908_abasaz_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_abasaz_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_abasaz_members[i].name,s.catalogue.device_name_length))named=1;
 if(fp&&!strcmp(s.catalogue.vendor,"Freescale"))for(size_t i=0;i<sizeof hc908_f_members/sizeof *hc908_f_members;i++)if(s.definition_resource==hc908_f_members[i].definition&&s.catalogue.device_name_length==strlen(hc908_f_members[i].name)&&!memcmp(s.catalogue.device_name,hc908_f_members[i].name,s.catalogue.device_name_length))named=1;
 if(!named)return LT48_UNSUPPORTED;
 lt48_hc908_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
 r->f=fp;r->abasaz=abasaz;r->gpgz=gpgz;r->apsr=apsr;r->grkx=grkx;r->gx=gx;r->jk=jk;r->definition=s.definition_resource;r->first=fp?fp->first:jk?jk->first:(r->definition==0x49||r->definition>=0x4c)?0xee00:0xf800;
 memset(r->security_key,lt48_hc908_blank_value(r),8);
 char path[4096],name[32];size_t index,family;
 int n=snprintf(path,sizeof path,"%s/GADB___.rez",lt48_installation_path(x));
 if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(a) do{st=(a);if(st!=LT48_OK)goto fail;}while(0)
 TRY(lt48_owner_rez_open_internal(path,"a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b",&r->rez));
 lt48_pin_code pc;snprintf(name,sizeof name,"P%x",s.pinmap);
 TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
 snprintf(name,sizeof name,"M%x",s.family_code);lt48_family_info f;
 TRY(lt48_rez_find_resource(r->rez,name,&family));TRY(lt48_rez_get_family_info(r->rez,family,&f));
 if(f.variable_count!=(r->f?r->f->variables:r->gpgz&&r->definition>=0x56?6u:5u)||f.operation_count!=12){st=LT48_FORMAT_ERROR;goto fail;}
 r->variable_count=(unsigned)f.variable_count;
 for(unsigned i=0;i<r->variable_count;i++){
  lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,family,i,&v));
  r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
 }
 unsigned seen=0,worker=0;
 for(size_t i=0;i<f.operation_count;i++){
  lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,family,i,&op));
  if(op.operation_id>=20||(seen&(1u<<op.operation_id))){st=LT48_FORMAT_ERROR;goto fail;}
  seen|=1u<<op.operation_id;snprintf(name,sizeof name,"W%x",op.wave);
  TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));
  if(w.wave_type==6){
   if(worker>=4||w.length1!=w.data_size+4||w.length2!=w.data_size||w.load_address<0x5100||(size_t)w.load_address+w.data_size>(r->f||r->abasaz?0x6100u:r->gpgz&&r->definition==0x58?0x5e00u:r->apsr||(r->gpgz&&r->definition>=0x56)?0x5b00u:0x5a00u)){st=LT48_FORMAT_ERROR;goto fail;}
   r->workers[worker]=w;r->addresses[worker++]=(lt48_applet_address){op.operation_id,w.load_address};
  }else if(w.wave_type==4&&op.operation_id==11){
   if(w.data_size!=2780||w.data[0]!=0xda||w.data[1]!=0x0a){st=LT48_FORMAT_ERROR;goto fail;}
   r->fpga=w.data+2;
   if(r->abasaz&&r->definition==0x37){
    r->derived_fpga=malloc(2778);if(!r->derived_fpga){st=LT48_NO_MEMORY;goto fail;}memcpy(r->derived_fpga,r->fpga,2778);
    for(size_t k=0;k<sizeof hc908_az60_clock_patch/sizeof *hc908_az60_clock_patch;k++){
     unsigned a=hc908_az60_clock_patch[k].offset;if(a>=2778||r->derived_fpga[a]!=hc908_az60_clock_patch[k].before){st=LT48_SAFETY_REJECTED;goto fail;}r->derived_fpga[a]=hc908_az60_clock_patch[k].after;
    }r->fpga=r->derived_fpga;
   }
  }else if(w.wave_type==0){
   lt48_compact_wave *c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));
   st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,r->variable_count,1,r->gx&&op.operation_id==7?LT48_PIN_FALLBACK_RESIDENT_NOOP:LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);lt48_compact_wave_close(c);
   if(st!=LT48_OK)goto fail;
   for(size_t k=0;k<lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);k++){
    lt48_defaulted_logical_pin pin;TRY(lt48_compiled_wave_get_defaulted_pin(r->wave[op.operation_id],k,&pin));
    if(!r->gx||op.operation_id!=7||(pin.logical_pin<22||pin.logical_pin>24||pin.compact_offset!=(size_t)(2+3*(pin.logical_pin-22)))||pin.compact_opcode!=0x11||pin.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP){st=LT48_SAFETY_REJECTED;goto fail;}
   }
  }else {st=LT48_UNSUPPORTED;goto fail;}
 }
 if(worker!=4||!r->fpga||seen!=((1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<5)|(1u<<7)|(1u<<11)|(1u<<14)|(1u<<15)|(1u<<16)|(1u<<19))){st=LT48_FORMAT_ERROR;goto fail;}
 TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
 *out=r;return LT48_OK;
fail:lt48_hc908_close(r);return st;
#undef TRY
}
lt48_status lt48_hc908_open(const lt48_installation *x,size_t device,lt48_hc908_resources **out){
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 lt48_status st=LT48_HC908_QUALIFIED?lt48_hc908_open_candidate(x,device,out):LT48_UNSUPPORTED;
 if(st==LT48_OK&&(*out)->f&&LT48_HC908_FQTQY_STAGE<(*out)->f->stage){lt48_hc908_close(*out);*out=NULL;return LT48_UNSUPPORTED;}
 if(st==LT48_OK&&(*out)->abasaz&&LT48_HC908_ABASAZ_STAGE<(*out)->gx->stage){lt48_hc908_close(*out);*out=NULL;return LT48_UNSUPPORTED;}
 if(st==LT48_OK&&(*out)->gpgz&&LT48_HC908_GPGZ_STAGE<(*out)->gx->stage){lt48_hc908_close(*out);*out=NULL;return LT48_UNSUPPORTED;}
 if(st==LT48_OK&&(*out)->grkx&&LT48_HC908_GRKX_STAGE<(*out)->gx->stage){lt48_hc908_close(*out);*out=NULL;return LT48_UNSUPPORTED;}
 if(st==LT48_OK&&(*out)->apsr&&LT48_HC908_APSR_STAGE<(*out)->gx->stage){lt48_hc908_close(*out);*out=NULL;return LT48_UNSUPPORTED;}
 if(st==LT48_OK&&(*out)->gx&&!(*out)->grkx&&!(*out)->apsr&&!(*out)->gpgz&&!(*out)->abasaz&&LT48_HC908_GXJB_STAGE<(*out)->gx->stage){lt48_hc908_close(*out);*out=NULL;return LT48_UNSUPPORTED;}
 if(st==LT48_OK&&(*out)->jk&&!(*out)->gx&& (LT48_HC908_JKJL_STAGE<22 || ((*out)->jk->flbpr!=0xfe09&&LT48_HC908_JKJL_STAGE<38))){lt48_hc908_close(*out);*out=NULL;return LT48_UNSUPPORTED;}
 return st;
}

static int flash(const lt48_hc908_resources *r,unsigned a){if(r->gx){for(unsigned i=0;i<r->gx->range_count;i++)if(a>=r->gx->ranges[i][0]&&a<r->gx->ranges[i][1])return 1;return 0;}if(r->jk)return (a>=r->first&&a<0xfc00)||(r->jk->flbpr==0xfe09?a>=0xffd0:(a==0xffcf||a==0xffd0||a>=0xffdc));return (a>=r->first&&a<0xfe00)||(a>=0xffb0&&a!=0xffbf);}
lt48_status lt48_hc908_validate(const lt48_hc908_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,lt48_verify_margin_selection margin,unsigned type){
 if(!r)return LT48_INVALID_ARGUMENT;
 if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
 if(start||length!=65536)return LT48_SAFETY_REJECTED;
 if(type>1||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
 if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||size!=65536))return LT48_INVALID_ARGUMENT;
 if(image&&size!=65536)return LT48_INVALID_ARGUMENT;
 if(!image&&size)return LT48_INVALID_ARGUMENT;
 if(image&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))for(unsigned a=0;a<65536;a++)if(!flash(r,a)&&image[a]!=255)return LT48_SAFETY_REJECTED;
 return LT48_OK;
}

typedef struct hc908_session {
 const lt48_hc908_resources *r;lt48_runtime_transport_session *wire;
 const lt48_protocol_transport *api;void *context;
 lt48_compile_variable vars[6];unsigned char buffers[32][32];
 uint32_t volts[4];unsigned mode,address;unsigned char *image;
} hc908_session;
static lt48_status run(void *context,unsigned op){
 hc908_session *s=context;if(op>=20||!s->r->wave[op])return LT48_UNSUPPORTED;
 lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[30];size_t n=0;
 for(unsigned i=0;i<32;i++)if(i!=0&&i!=4)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
 o.mode=s->mode;o.address=s->address;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=4;o.parameter_buffer_id=1;
 o.initial_buffers=seeds;o.initial_buffer_count=n;o.root_timeout_seconds=15;
 o.device_image=s->image;o.device_image_size=65536;o.device_element_bytes=1;
 memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);
 lt48_runtime_configuration_value cfg[1];size_t count=0;
 return lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->variable_count,&o,s->r->addresses,4,s->buffers,cfg,1,&count,s->volts);
}

static lt48_status f_select(hc908_session *s);
static lt48_status f_read_cycle(hc908_session *s,unsigned voltage,unsigned char *data,unsigned char trim[2]);
static unsigned hc908_preface_base(const lt48_hc908_resources*r){return r->f||r->abasaz?0x6100:r->gpgz&&r->definition==0x58?0x5e00:r->apsr||(r->gpgz&&r->definition>=0x56)?0x5b00:0x5a00;}
static unsigned hc908_bridge_base(const lt48_hc908_resources*r){return r->f||r->abasaz?0x6200:r->gpgz&&r->definition==0x58?0x5f00:0x5c00;}
static lt48_status setup(hc908_session *s) {
 lt48_status st=LT48_OK;unsigned char extension[128],worker[852];
 size_t n=hc908_read_preface(extension,s->r->first);
 st=lt48_owner_load_applet_internal(s->wire,hc908_preface_base(s->r),extension,n);
 for(unsigned i=0;st==LT48_OK&&i<4;i++) {
  const lt48_wave_code *w=&s->r->workers[i];const unsigned char *data=w->data;
  if(s->r->addresses[i].wave==15&&!s->r->jk&&!s->r->f){
   static const unsigned char before[]={0x7c,0xf8,0x7d,0,0x7e,0};
   memcpy(worker,data,w->data_size);
   unsigned char expected[6];memcpy(expected,before,6);expected[1]=(unsigned char)(s->r->first>>8);
   if(w->load_address!=0x5100||w->data_size!=376||memcmp(worker+0xa6,expected,6))return LT48_SAFETY_REJECTED;
   const unsigned char patch[]={0x12,0x5a,0,0x02,0x51,0xac};memcpy(worker+0xa6,patch,6);data=worker;
  }
  st=lt48_owner_load_applet_internal(s->wire,w->load_address,data,w->data_size);
 }
 if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);
 if(st==LT48_OK)st=run(s,0);
 return st;
}
static lt48_status select_read_worker(hc908_session *s,int bridge) {
 if(s->r->f)return f_select(s);
 unsigned char extension[128],worker[376];size_t n=hc908_read_preface(extension,s->r->first);
 if(bridge){n-=7;extension[n++]=2;extension[n++]=0x5c;extension[n++]=0;}
 lt48_status st=lt48_owner_load_applet_internal(s->wire,0x5a00,extension,n);
 for(unsigned i=0;st==LT48_OK&&i<4;i++)if(s->r->addresses[i].wave==15){
  memcpy(worker,s->r->workers[i].data,sizeof worker);
  const unsigned char ordinary[]={0x12,0x5a,0,0x02,0x51,0xac},proxy[]={2,0x5a,0,0,0,0};
  memcpy(worker+0xa6,bridge?proxy:ordinary,6);
  st=lt48_owner_load_applet_internal(s->wire,0x5100,worker,sizeof worker);
 }
 if(st==LT48_OK&&bridge){n=hc908_monitor_bridge(extension);st=lt48_owner_load_applet_internal(s->wire,0x5c00,extension,n);}
 return st;
}
static lt48_status read_cycle(hc908_session *s,unsigned voltage,unsigned char *data,unsigned char trim[2]) {
 if(s->r->f)return f_read_cycle(s,voltage,data,trim);
 s->mode=1;s->address=0;memset(s->buffers,0,sizeof s->buffers);
 /* LOAD explicitly copies DataOut for reserved address FFBF. Canonical
  * address-image padding is FF, independent of the previous operation. */
 memset(s->buffers[2],255,sizeof s->buffers[2]);
 for(unsigned i=0;i<s->r->variable_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=voltage;
 lt48_status st=select_read_worker(s,0);unsigned char preface[3];
 if(st==LT48_OK)st=run(s,2);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,preface,3);
 if(st==LT48_OK&&!(preface[0]&0x40))st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK){trim[0]=preface[1];trim[1]=preface[2];memset(data,255,65536);}
 for(unsigned a=s->r->first;st==LT48_OK&&a<65536;a++) {
  if(a>=0xfe00&&a<0xffb0)continue;
  s->address=a;st=run(s,4);
  if(st==LT48_OK)data[a]=s->buffers[1][0];
 }
 if(st==LT48_OK){data[0xffc0]=trim[0];data[0xffc1]=trim[1];}
 lt48_status reset=run(s,3);return st==LT48_OK?reset:st;
}
static lt48_status io(hc908_session *s,int input,unsigned char *p,size_t n) {
 if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
 size_t done=0;lt48_status st=input?s->api->read(s->context,0x88,p,n,&done):s->api->write(s->context,2,p,n,&done);
 return st==LT48_OK&&done!=n?LT48_PROTOCOL_ERROR:st;
}
static lt48_status monitor(hc908_session *s,const unsigned char *tx,unsigned n,unsigned char *rx,unsigned nr) {
 if(!n||n>4||nr>2)return LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_runtime_transport_session_flush(s->wire);
 unsigned char packet[512]={0x2c},ack[512]={0},reply[6];size_t at=1;
 packet[at++]=2;packet[at++]=(unsigned char)n;packet[at++]=2;packet[at++]=(unsigned char)nr;
 for(unsigned i=0;i<n;i++){packet[at++]=2;packet[at++]=tx[i];}
 if(st==LT48_OK)st=io(s,0,packet,sizeof packet);
 if(st==LT48_OK)st=io(s,1,ack,sizeof ack);
 if(st==LT48_OK&&!ack[0])st=LT48_PROTOCOL_ERROR;
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,reply,n+nr);
 if(st==LT48_OK&&memcmp(reply,tx,n))st=LT48_PROTOCOL_ERROR;
 if(st==LT48_OK&&nr)memcpy(rx,reply+n,nr);
 return st;
}
static lt48_status monitor_write(hc908_session *s,unsigned address,const unsigned char *p,size_t n) {
 if(!n||address>65535||n>65536-address)return LT48_INVALID_ARGUMENT;
 unsigned char tx[]={0x49,(unsigned char)(address>>8),(unsigned char)address,p[0]};
 lt48_status st=monitor(s,tx,4,NULL,0);
 for(size_t i=1;st==LT48_OK&&i<n;i++){unsigned char next[]={0x19,p[i]};st=monitor(s,next,2,NULL,0);}
 return st;
}
static lt48_status monitor_read(hc908_session *s,unsigned address,unsigned char *p,size_t n) {
 lt48_status st=LT48_OK;
 for(size_t i=0;st==LT48_OK&&i<n;i++){
  unsigned a=address+(unsigned)i;unsigned char tx[]={0x4a,(unsigned char)(a>>8),(unsigned char)a};
  st=monitor(s,tx,3,p+i,1);
 }
 return st;
}
#include "fqtqy_operations.inc"

static lt48_status target_hold(unsigned milliseconds) {
#ifdef _WIN32
 Sleep(milliseconds);
#else
 struct timespec delay={(time_t)(milliseconds/1000),(long)(milliseconds%1000)*1000000L};while(nanosleep(&delay,&delay)!=0)if(errno!=EINTR)return LT48_RUNTIME_ERROR;
#endif
 return LT48_OK;
}
static lt48_status execute_ram(hc908_session *s,const unsigned char *code,size_t count,const unsigned char *data,size_t n) {
 if(count>64||n>32)return LT48_SAFETY_REJECTED;
 lt48_status st=monitor_write(s,0xb0,code,count);unsigned char verify[64];
 if(st==LT48_OK)st=monitor_read(s,0xb0,verify,count);
 if(st==LT48_OK&&memcmp(verify,code,count))st=LT48_DEVICE_MISMATCH;
 if(st==LT48_OK&&n)st=monitor_write(s,0x8c,data,n);
 if(st==LT48_OK&&n)st=monitor_read(s,0x8c,verify,n);
 if(st==LT48_OK&&n&&memcmp(verify,data,n))st=LT48_DEVICE_MISMATCH;
 unsigned char sp[2]={0},readsp[]={0x0c};
 if(st==LT48_OK)st=monitor(s,readsp,1,sp,2);
 unsigned stack=(unsigned)sp[0]*256+sp[1];
 if(st==LT48_OK&&(stack<0xf0||stack>0xfa))st=LT48_SAFETY_REJECTED;
 unsigned char pc[]={0,0xb0};
 if(st==LT48_OK)st=monitor_write(s,stack+4,pc,2);
 if(st==LT48_OK)st=monitor_read(s,stack+4,verify,2);
 if(st==LT48_OK&&memcmp(verify,pc,2))st=LT48_DEVICE_MISMATCH;
 unsigned char go[]={0x28};
 if(st==LT48_OK){
  st=monitor(s,go,1,NULL,0);
  /* A missing RUN acknowledgement does not prove the target stayed idle. */
  if(st==LT48_OK||s->r->f||s->r->gpgz||s->r->abasaz){lt48_status held=target_hold(s->r->abasaz?(s->r->definition==0x37?(count==15?4:n?2:120):n?2:10):s->r->gpgz&&s->r->definition>=0x56?(n?2:10):s->r->gpgz&&s->r->definition<=0x25?(n?2:140):s->r->gx&&s->r->gx->erase_loops==0xffff&&!n?300:20);if(st==LT48_OK)st=held;}
 }
 unsigned control=s->r->gx?s->r->gx->control:0xfe08;
 if((s->r->gpgz&&s->r->definition>=0x56)||(s->r->abasaz&&count!=15)){if(count<6||code[3]!=0xc7)return LT48_SAFETY_REJECTED;control=(unsigned)code[4]*256+code[5];}
 if(st==LT48_OK)st=monitor_read(s,control,verify,1);
 if(st==LT48_OK&&verify[0]!=(s->r->abasaz&&s->r->definition==0x37?(count==15?0:n?1:2):s->r->gpgz&&s->r->definition<=0x25?(n?0x41:0x42):0))st=LT48_DEVICE_MISMATCH;
 return st;
}
static lt48_status write_row(hc908_session *s,unsigned a,const unsigned char *p,unsigned n) {
 unsigned char code[64],got[32];size_t count=hc908_program_ram(code,a,n);
 if(!count)return LT48_SAFETY_REJECTED;
 lt48_status st=execute_ram(s,code,count,p,n);
 if(st==LT48_OK)st=monitor_read(s,a,got,n);
 return st==LT48_OK&&memcmp(got,p,n)?LT48_DEVICE_MISMATCH:st;
}
static lt48_status mutate(hc908_session *s,int erase,const unsigned char *input,const unsigned char trim[2]) {
 s->mode=1;s->address=0;memset(s->buffers,0,sizeof s->buffers);
 for(unsigned i=0;i<s->r->variable_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=500;
 lt48_status st=select_read_worker(s,1);unsigned char preface[3];
 if(st==LT48_OK)st=run(s,2);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,preface,3);
 if(st==LT48_OK&&(!(preface[0]&0x40)||memcmp(preface+1,trim,2)))st=LT48_SAFETY_REJECTED;
 if(st==LT48_OK&&erase){
  unsigned char code[64];size_t n=hc908_erase_ram(code,s->r->first);st=execute_ram(s,code,n,NULL,0);
  if(st==LT48_OK)st=write_row(s,0xffc0,trim,2);
 }else if(st==LT48_OK){
  for(unsigned a=s->r->first;st==LT48_OK&&a<0xfe00;a+=32)st=write_row(s,a,input+a,32);
  if(st==LT48_OK)st=write_row(s,0xffb0,input+0xffb0,15);
  if(st==LT48_OK)st=write_row(s,0xffc2,input+0xffc2,30);
  if(st==LT48_OK)st=write_row(s,0xffe0,input+0xffe0,32);
 }
 const unsigned char stop[]={0};unsigned char terminal=0;
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,stop,1);
 if(st==LT48_OK)st=lt48_runtime_transport_session_controller_capture_internal(s->wire,&terminal,1);
 if(st==LT48_OK&&terminal!=1)st=LT48_PROTOCOL_ERROR;
 lt48_status reset=run(s,3);return st==LT48_OK?reset:st;
}
static lt48_status compare(const unsigned char *data,const unsigned char *expected,const unsigned char trim[2],lt48_small_eprom_result *out) {
 out->mismatch_count=0;out->first_mismatch=UINT32_MAX;
 for(unsigned a=0;a<65536;a++) {
  unsigned want=a==0xffc0?trim[0]:a==0xffc1?trim[1]:expected?expected[a]:255;
  if(data[a]!=want){if(!out->mismatch_count)out->first_mismatch=a;out->mismatch_count++;}
 }
 return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}

#include "jk_jl_operations.inc"

lt48_status lt48_hc908_execute(const lt48_hc908_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *input,size_t input_size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_small_eprom_result *out){
 if(!out)return LT48_INVALID_ARGUMENT;
 if(r&&r->jk)return hc908_jk_execute(r,api,context,op,start,length,input,input_size,read,read_size,margin,type,out);
 memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
 lt48_status st=lt48_hc908_validate(r,op,start,length,input,input_size,margin,type);
 if(st==LT48_OK&&(!api||!api->write||!api->read))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<65536))st=LT48_BUFFER_TOO_SMALL;
 if(st!=LT48_OK){out->operation_status=st;return st;}
 hc908_session s={0};s.r=r;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof s.vars);
 s.image=malloc(65536);unsigned char *pending=malloc(65536),trim[2]={255,255};
 if(!s.image||!pending){free(s.image);free(pending);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
 if(input)memcpy(s.image,input,65536);else memset(s.image,255,65536);
 memcpy(s.image+0xfff6,r->security_key,8);
 st=lt48_runtime_transport_session_create(api,context,&s.wire);
 if(st==LT48_OK)st=setup(&s);
 if(st==LT48_OK)st=read_cycle(&s,500,pending,trim);
 int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE;
 if(st==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_BLANK_CHECK))st=compare(pending,NULL,trim,out);
 if(st==LT48_OK&&(op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_VERIFY))st=compare(pending,input,trim,out);
 if(st==LT48_OK&&op==LT48_OPERATION_ERASE){st=mutate(&s,1,NULL,trim);memset(s.image,255,65536);}
 if(st==LT48_OK&&op==LT48_OPERATION_PROGRAM){st=mutate(&s,0,input,trim);if(st==LT48_OK)memcpy(s.image+0xfff6,input+0xfff6,8);}
 if(st==LT48_OK&&(writing||op==LT48_OPERATION_VERIFY)) {
  unsigned delta=type?50:25;
  unsigned volts[3]={500,500-delta,500+delta},count=3;
  if(!writing&&margin!=LT48_VERIFY_MARGIN_ALL){volts[0]=margin==LT48_VERIFY_MARGIN_LOW?500-delta:margin==LT48_VERIFY_MARGIN_HIGH?500+delta:500;count=1;}
  for(unsigned i=0;st==LT48_OK&&i<count;i++) {
   unsigned char current_trim[2];st=read_cycle(&s,volts[i],pending,current_trim);
   if(st==LT48_OK)st=compare(pending,op==LT48_OPERATION_ERASE?NULL:input,trim,out);
  }
 }
 out->operation_status=st;static const unsigned cleanup[]={3,1};
 lt48_owner_finish_fpga_internal(api,context,&s.wire,run,&s,cleanup,2,r->idle,r->idle_size,st,&out->cleanup_status,&out->cleanup_completed);
 lt48_runtime_transport_session_close(s.wire);
 if(st==LT48_OK)st=out->cleanup_status;
 if(st==LT48_OK){out->completed_bytes=65536;if(op==LT48_OPERATION_READ)memcpy(read,pending,65536);}
 free(s.image);free(pending);return st;
}
