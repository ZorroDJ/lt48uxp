#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Dataman 13.10 PIC closures. Addresses in this API are FILE BYTE offsets:
 * 14-bit cores use two little-endian bytes at each original word address. */
#include "../../NewDeviceLib/sources/pic-common/profiles_generated.h"
#include "../../NewDeviceLib/sources/pic-common/batch_profiles_generated.h"
#include "../../NewDeviceLib/sources/pic-common/transactions_generated.h"
#include "../../NewDeviceLib/sources/pic-common/traversal.h"
#include "../../NewDeviceLib/sources/pic-common/range_execution.h"
#include "../../NewDeviceLib/sources/pic-common/word_program.h"
#include "../../NewDeviceLib/sources/pic-common/internal_program.h"
#include "../../NewDeviceLib/sources/pic-common/power.h"
#include "../../NewDeviceLib/sources/pic-common/wait.h"
#include "../../NewDeviceLib/sources/pic-common/entry.h"
#include "../../NewDeviceLib/sources/pic-common/classic14.h"
#include "../../NewDeviceLib/sources/pic-common/flash14_87xa.h"
#include "../../NewDeviceLib/sources/pic-common/flash14_87x.h"
#include "../../NewDeviceLib/sources/pic-common/flash14_row4.h"
#include "../../NewDeviceLib/sources/pic-common/enhanced_qfn44.h"
#include "../../NewDeviceLib/sources/pic-common/baseline12_profiles_generated.h"
#include "../../NewDeviceLib/sources/pic-common/baseline12_admissions_generated.h"
#include "../../NewDeviceLib/sources/pic-common/calibrated14_operator.h"
#include "../../NewDeviceLib/sources/pic-common/baseline12_otp_source.h"
#include "../../NewDeviceLib/drafts/pic-population/baseline12_otp_profiles.h"
#include "../../NewDeviceLib/sources/pic-common/baseline12_otp_admissions.h"
#include "../../NewDeviceLib/sources/hyundai4/profiles.h"
#include "../../NewDeviceLib/sources/hms77two/profiles.h"
#include "../../NewDeviceLib/sources/pic-common/calibrated14_admissions.h"
#include "../../NewDeviceLib/sources/pic-common/classic14_tail_operator.h"
#include "../../NewDeviceLib/sources/pic-common/classic14_tail_admissions.h"
#include "../../NewDeviceLib/drafts/pic-population/classic14_tail_contracts.h"
#include "../../NewDeviceLib/sources/pic-common/source_session.h"
#include "pic_classic_entry.h"
#include "../../NewDeviceLib/sources/pic-common/midrange14_profiles_generated.h"
#include "../../NewDeviceLib/sources/pic-common/midrange14_operator.h"
#include "../../NewDeviceLib/sources/pic-common/midrange14_controller.h"
#include "../../NewDeviceLib/sources/pic-common/midrange14_entry.h"
#include "../../NewDeviceLib/sources/pic-common/flash14_72x.h"
#include "../../NewDeviceLib/sources/pic-common/flash14_62x.h"
#include "../../NewDeviceLib/sources/pic-common/flash14_7x.h"
#include "../../NewDeviceLib/sources/pic-common/legacy16_policy.h"
#include "../../NewDeviceLib/sources/pic-common/remaining14_public.h"
#include "../../NewDeviceLib/sources/pic-common/older17_public.h"
#include "../../NewDeviceLib/sources/pic-common/pic18c_public.h"
#include "../../NewDeviceLib/drafts/pic-population/otp14_transport.h"
#include "../../NewDeviceLib/sources/pic-common/otp14_profiles.h"
struct lt48_pic_resources {
    unsigned legacy_program_only;
    const o16_profile *older17;
    const o16_profile *pic18c;
    const rm14_profile *remaining14;
    lt48_rez *rez; lt48_pin_map *pins; lt48_compiled_wave *wave[32];
    lt48_compile_variable vars[16]; size_t var_count;
    unsigned char *idle; size_t idle_size;
    unsigned core, capacity, program_words, size_seed;
    const lt48_pic_profile *profile;
    const lt48_pic_midrange14_profile *midrange14;
    const lt48_pic_baseline12_profile *baseline12;
    unsigned calibrated14; /* Private DS41191D owner, exact binding only. */
    const lt48_pic_tail14_profile *tail14;
    const b12otp_profile *otp12;
    const lt48_pic_otp14_profile *otp;char *otp_path;unsigned char otp_hash[32];
    lt48_compiled_wave *word_program;
};
static lt48_status pic_older17_open(const lt48_installation *,const lt48_device_spec *,const o16_profile *,lt48_pic_resources **);
static lt48_status pic_older17_validate(const lt48_pic_resources *,lt48_operation_kind,const unsigned char *,size_t);
static lt48_status pic18c_open(const lt48_installation *,const lt48_device_spec *,const o16_profile *,lt48_pic_resources **);
static lt48_status pic_remaining14_open(const lt48_installation *,const lt48_device_spec *,const rm14_profile *,lt48_pic_resources **);
static lt48_status pic_remaining14_validate(const lt48_pic_resources *,lt48_operation_kind,const unsigned char *,size_t);
void lt48_pic_close(lt48_pic_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<32;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_compiled_wave_close(r->word_program);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r->otp_path);free(r);
}
uint32_t lt48_pic_capacity(const lt48_pic_resources *r){return r?r->capacity:0;}
int lt48_pic_is_hms77two_internal(const lt48_pic_resources*r){return r&&h77_profile(r->otp12);}
int lt48_pic_is_hyundai4_internal(const lt48_pic_resources*r){return r&&hy4_profile(r->otp12);}
int lt48_pic_is_baseline12_otp_internal(const lt48_pic_resources *r){return r&&r->otp12;}
int lt48_pic_is_older17_internal(const lt48_pic_resources *r){return r&&r->older17;}
int lt48_pic_is_pic18c_internal(const lt48_pic_resources *r){return r&&r->pic18c;}
uint16_t lt48_pic_configuration_encoding_mask(const lt48_pic_resources *r){return !r?0:(r->older17||r->pic18c)?65535:(r->baseline12||r->otp12)?4095:16383;}
int lt48_pic_is_remaining14_internal(const lt48_pic_resources *r){
 if(!r)return 0;
 if(r->remaining14)return 1;
 for(size_t i=0;i<sizeof rm14_legacy_profiles/sizeof *rm14_legacy_profiles;i++)if(r->profile==rm14_legacy_profiles+i)return 1;
 return 0;
}
int lt48_pic_is_tail14_internal(const lt48_pic_resources *r){return r&&r->tail14;}
int lt48_pic_is_calibrated14_internal(const lt48_pic_resources *r){return r&&r->calibrated14;}
size_t lt48_pic_configuration_word_count(const lt48_pic_resources *r){
 if(!r)return 0;
 if(r->pic18c)return r->pic18c->family==351?3:4;
 if(r->older17||r->remaining14||r->otp||r->otp12||r->midrange14||r->baseline12||r->calibrated14||r->tail14)return 1;
 size_t n=0;if(r->profile)for(size_t i=0;i<r->profile->region_count;i++)
  if(r->profile->regions[i].kind==LT48_PIC_REGION_CONFIGURATION)n+=r->profile->regions[i].count;
 return n;
}
lt48_status lt48_pic_configuration_word_info(const lt48_pic_resources *r,size_t index,lt48_pic_configuration_word *out){
 if(!r||!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));
 if(index>=lt48_pic_configuration_word_count(r))return LT48_INVALID_ARGUMENT;
 if(r->pic18c){unsigned word=(unsigned)index;if(r->pic18c->family==351&&word>=2)word++;unsigned offset=2*word;*out=(lt48_pic_configuration_word){0x300000+offset,(uint16_t)(o16_pic18c_configuration_mask(r->pic18c->family,offset)|(o16_pic18c_configuration_mask(r->pic18c->family,offset+1)<<8))};return LT48_OK;}
 if(r->older17){*out=(lt48_pic_configuration_word){0x1fc00,r->older17->family>=37?0xc05f:0x5f};return LT48_OK;}
 if(r->remaining14){rm14_otp_policy p;lt48_status st=rm14_public_policy(r->remaining14,&p);if(st!=LT48_OK)return st;*out=(lt48_pic_configuration_word){0x400e,p.common.configuration.mask};return LT48_OK;}
 if(r->otp12){*out=(lt48_pic_configuration_word){8190,r->otp12->policy.configuration_mask};return LT48_OK;}
 if(r->otp){*out=(lt48_pic_configuration_word){0x400e,(uint16_t)r->otp->configuration_mask};return LT48_OK;}
 if(r->calibrated14){*out=(lt48_pic_configuration_word){0x400e,0x1ff};return LT48_OK;}
 if(r->tail14){*out=(lt48_pic_configuration_word){0x400e,(uint16_t)lt48_pic_tail14_policies[r->tail14->policy].configuration_mask};return LT48_OK;}
 if(r->midrange14){*out=(lt48_pic_configuration_word){0x400e,r->midrange14->configuration_mask};return LT48_OK;}
 if(r->baseline12){*out=(lt48_pic_configuration_word){2*r->baseline12->configuration_image_word,(uint16_t)r->baseline12->policy.configuration_mask};return LT48_OK;}
 for(size_t i=0;i<r->profile->region_count;i++){
  const lt48_pic_region *q=r->profile->regions+i;if(q->kind!=LT48_PIC_REGION_CONFIGURATION)continue;
  if(index>=q->count){index-=q->count;continue;}
  uint64_t offset=(uint64_t)q->image_offset+index*q->image_stride;
  if(q->element_bytes!=2||offset+2>r->capacity||q->compare_mask>0x3fff)return LT48_FORMAT_ERROR;
  *out=(lt48_pic_configuration_word){(uint32_t)offset,(uint16_t)q->compare_mask};return LT48_OK;
 }
 return LT48_FORMAT_ERROR;
}
lt48_status lt48_pic_baseline12_layout(const lt48_pic_resources *r,lt48_pic_baseline12_layout_info *out){
 if(!r||!out)return LT48_INVALID_ARGUMENT;
 if(r->otp12){const b12otp_policy *p=&r->otp12->policy;*out=(lt48_pic_baseline12_layout_info){8192,p->code_words,0,p->code_words,4095,p->calibration?p->code_words-1:UINT32_MAX,0,0,12,4095};return LT48_OK;}
 if(!r->baseline12)return LT48_UNSUPPORTED;
 const lt48_pic_baseline12_profile *p=r->baseline12;
 *out=(lt48_pic_baseline12_layout_info){r->capacity,p->policy.code_words,p->policy.data_words,
  p->policy.id_base,p->configuration_image_word,p->policy.calibration?p->policy.code_words-1:UINT32_MAX,
  p->policy.backup_address,p->policy.backup_words,12,0xfff};return LT48_OK;
}

lt48_status lt48_pic_midrange14_layout(const lt48_pic_resources *r,lt48_pic_midrange14_layout_info *out){
 if(!r||!out)return LT48_INVALID_ARGUMENT;
 if(!r->midrange14)return LT48_UNSUPPORTED;
 const lt48_pic_midrange14_profile *p=r->midrange14;
 *out=(lt48_pic_midrange14_layout_info){r->capacity,p->protocol.code_words,0x2000,0x2007,0x2006,
  p->protocol.has_revision?0x2005:UINT32_MAX,0x2008,2,p->configuration_mask,p->identity_mask,p->identity_value};return LT48_OK;
}
static const lt48_pic_midrange14_profile *pic_midrange14_match(const lt48_device_spec *);
static lt48_status pic_midrange14_open(const lt48_installation *,const lt48_device_spec *,const lt48_pic_midrange14_profile *,lt48_pic_resources **);
static lt48_status pic_open_profile(const lt48_installation *x,const lt48_device_spec *source,
    const lt48_pic_profile *profile,lt48_pic_resources **out);
static lt48_status pic_baseline12_open(const lt48_installation *,const lt48_device_spec *,
    const lt48_pic_baseline12_profile *,lt48_pic_resources **);
static lt48_status pic_cal14_open(const lt48_installation *,const lt48_device_spec *,lt48_pic_resources **);
static lt48_status pic_tail14_open(const lt48_installation *,const lt48_device_spec *,const lt48_pic_tail14_profile *,lt48_pic_resources **);
static lt48_status pic_otp12_open(const lt48_installation *,const lt48_device_spec *,const b12otp_profile *,lt48_pic_resources **);
static const b12otp_profile *pic_otp12_match(const lt48_device_spec *s) {
    const b12otp_profile*hms=h77_match(s);if(hms)return hms;
    const b12otp_profile*hy=hy4_match(s);if(hy)return hy;
#if LT48_PIC_BASELINE12_OTP_ADMISSIONS_ENABLED
    if(!s||s->catalogue.device_class!=2015||!s->definition_stem||strcmp(s->definition_stem,"GADC___")||
       !s->catalogue.vendor||strcmp(s->catalogue.vendor,"Microchip")||s->element_bits!=12)return NULL;
    if(!lt48_pic_baseline12_otp_public_allowed(s))return NULL;
    for(size_t i=0;i<sizeof pic_baseline12_otp_profiles/sizeof *pic_baseline12_otp_profiles;i++) {
        const b12otp_profile *p=pic_baseline12_otp_profiles+i;
        if(s->definition_resource==p->definition&&s->family_code==p->family&&s->pinmap==p->pinmap&&
           s->catalogue.device_name_length==strlen(p->name)&&!memcmp(s->catalogue.device_name,p->name,strlen(p->name)))return p;
    }
#else
    (void)s;
#endif
    return NULL;
}
static lt48_status pic_otp_probe_command_transport(void *c,uint8_t ep,const unsigned char *p,size_t n,size_t *done) {
    (void)c;(void)ep;(void)p;(void)n;if(done)*done=0;return LT48_UNSUPPORTED;
}
static lt48_status pic_otp_probe_read_transport(void *c,uint8_t ep,unsigned char *p,size_t n,size_t *done) {
    (void)c;(void)ep;(void)p;(void)n;if(done)*done=0;return LT48_UNSUPPORTED;
}
static const lt48_pic_otp14_profile *pic_otp_match(const lt48_device_spec *spec) {
#if LT48_PIC_OTP14_ENABLED
    if(spec->catalogue.device_class!=2015||!spec->definition_stem||strcmp(spec->definition_stem,"GADC___")||spec->element_bits!=14)return NULL;
    for(size_t i=0;i<sizeof(pic_otp14_profiles)/sizeof(*pic_otp14_profiles);i++) {
        const lt48_pic_otp14_profile *p=pic_otp14_profiles+i;
        if(spec->definition_resource==p->definition&&spec->family_code==p->family&&spec->pinmap==p->pinmap&&
           !strcmp(spec->catalogue.vendor,p->manufacturer)&&spec->catalogue.device_name_length==strlen(p->device)&&
           !memcmp(spec->catalogue.device_name,p->device,strlen(p->device)))return p;
    }
#else
    (void)spec;
#endif
    return NULL;
}
static lt48_status pic_otp_probe_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **out) {
    (void)context;(void)machine;(void)out;return LT48_UNSUPPORTED;
}
static lt48_status pic_otp_probe_command(void *context,const unsigned char *bytes,size_t n) {
    (void)context;(void)bytes;(void)n;return LT48_UNSUPPORTED;
}
static lt48_status pic_otp_probe_ready(void *context) {(void)context;return LT48_UNSUPPORTED;}
static otp14_operator_policy pic_otp_policy(const lt48_pic_otp14_profile *p) {
    otp14_operator_policy policy={p->words,0x3fff,{p->configuration_mask,p->protection_mask,500,475,525}};return policy;
}
static lt48_status pic_otp_open(const lt48_installation *x,const lt48_device_spec *spec,
    const lt48_pic_otp14_profile *profile,lt48_pic_resources **out) {
    *out=NULL;if(spec->capacity_bytes!=OTP14_IMAGE_WORDS*2)return LT48_SAFETY_REJECTED;
    lt48_pic_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    r->otp=profile;r->capacity=OTP14_IMAGE_WORDS*2;r->program_words=profile->words;
    char path[4096];int n=snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x));
    if(n<0||(size_t)n>=sizeof path){lt48_pic_close(r);return LT48_INVALID_ARGUMENT;}
    r->otp_path=malloc((size_t)n+1);if(!r->otp_path){lt48_pic_close(r);return LT48_NO_MEMORY;}memcpy(r->otp_path,path,(size_t)n+1);
    lt48_status st=lt48_rez_open(path,&r->rez);char hex[65];
    if(st==LT48_OK)st=lt48_rez_sha256(r->rez,r->otp_hash);
    if(st==LT48_OK){for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",r->otp_hash[i]);
        if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec"))st=LT48_SAFETY_REJECTED;}
    otp14_source_adapter *probe=NULL;
    if(st==LT48_OK)st=otp14_source_open(path,r->otp_hash,profile->family,profile->pinmap,pic_otp_policy(profile),
        pic_otp_probe_runner,pic_otp_probe_runner,pic_otp_probe_command,pic_otp_probe_ready,NULL,&probe);
    otp14_source_close(probe);
    if(st==LT48_OK)st=lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size);
    if(st!=LT48_OK){lt48_pic_close(r);return st;}*out=r;return LT48_OK;
}
uint32_t lt48_pic_operation_mask_internal(const lt48_pic_resources *r) {return r?((r->pic18c||r->older17||r->remaining14||r->otp||r->otp12)?0xbfu:0xffu):0;}
const char *lt48_pic_otp_adapter_note_internal(const lt48_installation *x,size_t device) {
    lt48_device_spec spec;if(lt48_installation_resolve_device(x,device,&spec)!=LT48_OK)return NULL;
    const o16_profile *parallel17=pic_older17_match(&spec);
    if(parallel17&&parallel17->family>=37){
        lt48_pic_resources *r=NULL;if(pic_older17_open(x,&spec,parallel17,&r)!=LT48_OK)return NULL;lt48_pic_close(r);
        return "PIC17C75x @68 requires a dedicated PLCC68 programming adapter, not a straight adapter. Source P5 uses a 28-pin interface at ZIF11..38: MCLR/VPP=ZIF11 (device16), TEST=ZIF25 (device17), RA0=ZIF14 (device60), RA1=ZIF28 (device44), RA2=ZIF29 (device45), RA3=ZIF30 (device46), RA4=ZIF27 (device42), clock channels=ZIF23/24 (complementary); route through the dedicated programming adapter to OSC1 according to its schematic; VDD=ZIF38, VSS=ZIF22. Route all device supply pins according to DS30274B and the dedicated adapter wiring. Source bus group11 carries PORTC low byte and PORTB high byte. Physical adapter continuity and hardware operations remain unqualified.";
    }
    const o16_profile *ep=pic18c_match(&spec);
    if(ep&&(ep->pinmap==158||ep->pinmap==159)){
        lt48_pic_resources *r=NULL;if(pic18c_open(x,&spec,ep,&r)!=LT48_OK)return NULL;lt48_pic_close(r);
        return ep->pinmap==158?"PIC18C *44: straight PLCC44-to-DIP44 adapter, bottom-aligned at ZIF3..46; source MCLR/VPP=ZIF4, PGC=45, PGD=46; VDD=14/37, VSS=15/36. Adapter continuity has not been hardware-qualified.":"PIC18C658 @68 requires a dedicated PLCC68 programming adapter, not a straight adapter. Source interface: ZIF11=MCLR/VPP (device16), ZIF33=PGC (device54), ZIF32=PGD (device48), ZIF38=VDD, ZIF22=VSS. Connect every device VDD/AVDD and VSS/AVSS pin to its matching supply according to DS30475A; leave unrelated signals unconnected. Physical adapter continuity and hardware operation remain unqualified.";
    }
    const lt48_pic_otp14_profile *p=pic_otp_match(&spec);if(!p||!p->adapter_note)return NULL;
    lt48_pic_resources *r=NULL;if(pic_otp_open(x,&spec,p,&r)!=LT48_OK)return NULL;lt48_pic_close(r);return p->adapter_note;
}
uint32_t lt48_pic_image_capacity_internal(const lt48_installation *x,const lt48_device_spec *spec){
 if(!x||!spec||spec->catalogue.device_class!=2015||strcmp(spec->definition_stem,"GADC___"))return 0;
 const o16_profile *serial_eprom=pic18c_match(spec);
 if(serial_eprom){lt48_pic_resources *r=NULL;if(pic18c_open(x,spec,serial_eprom,&r)!=LT48_OK)return 0;uint32_t bytes=r->capacity;lt48_pic_close(r);return bytes;}
 const rm14_profile *remaining=rm14_public_match(spec);
 if(remaining&&remaining->source_bytes!=remaining->backing_bytes){
  char path[4096],hex[65];unsigned char sha[32];lt48_rez *rez=NULL;
  int n=snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x));
  if(n<0||(size_t)n>=sizeof path||lt48_rez_open(path,&rez)!=LT48_OK)return 0;
  lt48_status st=lt48_rez_sha256(rez,sha);lt48_rez_close(rez);if(st!=LT48_OK)return 0;
  for(unsigned j=0;j<32;j++)snprintf(hex+2*j,3,"%02x",sha[j]);
  if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec"))return 0;
  return remaining->backing_bytes;
 }

#if LT48_PIC_MIDRANGE14_PUBLIC_ENABLED
 const lt48_pic_midrange14_profile *midrange=pic_midrange14_match(spec);
 if(midrange){lt48_pic_resources *r=NULL;if(pic_midrange14_open(x,spec,midrange,&r)!=LT48_OK)return 0;uint32_t bytes=r->capacity;lt48_pic_close(r);return bytes;}
#endif
 for(size_t i=0;i<sizeof(pic_batch_profiles)/sizeof(*pic_batch_profiles);i++){
  const lt48_pic_profile *p=pic_batch_profiles+i;
  if(p->definition==spec->definition_resource&&p->family==spec->family_code&&p->pinmap==spec->pinmap&&
     lt48_pic_7x_source_capacity(p,spec->capacity_bytes)&&
     lt48_pic_member_allowed(pic_batch_members,sizeof(pic_batch_members)/sizeof(*pic_batch_members),
        p->definition,p->family,p->pinmap,spec->catalogue.vendor,spec->catalogue.device_name,spec->catalogue.device_name_length)){
       char path[4096],hex[65];unsigned char sha[32];lt48_rez *rez=NULL;
       int n=snprintf(path,sizeof path,"%s/GADC___.rez",lt48_installation_path(x));
       if(n<0||(size_t)n>=sizeof path||lt48_rez_open(path,&rez)!=LT48_OK)return 0;
       lt48_status st=lt48_rez_sha256(rez,sha);lt48_rez_close(rez);if(st!=LT48_OK)return 0;
       for(unsigned j=0;j<32;j++)snprintf(hex+2*j,3,"%02x",sha[j]);
       if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec"))return 0;
       return p->image_span;
     }
 }
 return 0;
}
lt48_status lt48_pic_open(const lt48_installation *x,size_t device,lt48_pic_resources **out) {

    (void)pic_baseline12_open;

    (void)pic_midrange14_open;
    if(!x||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,device,&spec);
    if(status!=LT48_OK)return status;
    const o16_profile *serial_eprom=pic18c_match(&spec);if(serial_eprom)return pic18c_open(x,&spec,serial_eprom,out);
    const o16_profile *older17=pic_older17_match(&spec);if(older17)return pic_older17_open(x,&spec,older17,out);
    const rm14_profile *remaining=rm14_public_match(&spec);
    if(remaining)return pic_remaining14_open(x,&spec,remaining,out);
    const b12otp_profile *otp12=pic_otp12_match(&spec);if(otp12)return pic_otp12_open(x,&spec,otp12,out);
    if(lt48_pic_cal14_public_allowed(&spec))return pic_cal14_open(x,&spec,out);
    if(lt48_pic_tail14_public_allowed(&spec))for(size_t i=0;i<sizeof pic_tail14_profiles/sizeof *pic_tail14_profiles;i++){const lt48_pic_tail14_profile *p=pic_tail14_profiles+i;if(p->definition==spec.definition_resource&&p->family==spec.family_code&&p->pinmap==spec.pinmap)return pic_tail14_open(x,&spec,p,out);}
#if LT48_PIC_BASELINE12_ADMISSIONS_ENABLED
    for(size_t i=0;i<sizeof pic_baseline12_profiles/sizeof *pic_baseline12_profiles;i++){
        const lt48_pic_baseline12_profile *p=pic_baseline12_profiles+i;
        if(p->definition==spec.definition_resource&&p->family==spec.family_code&&p->pinmap==spec.pinmap&&lt48_pic_baseline12_public_allowed(&spec))
            return pic_baseline12_open(x,&spec,p,out);
    }
#endif
#if LT48_PIC_MIDRANGE14_PUBLIC_ENABLED
    const lt48_pic_midrange14_profile *midrange=pic_midrange14_match(&spec);
    if(midrange)return pic_midrange14_open(x,&spec,midrange,out);
#endif
    const lt48_pic_otp14_profile *otp=pic_otp_match(&spec);
    if(otp)return pic_otp_open(x,&spec,otp,out);
    const lt48_pic_profile *profile=NULL;
    for(size_t i=0;i<sizeof(pic_profiles)/sizeof(*pic_profiles);i++) {
        const lt48_pic_profile *p=&pic_profiles[i];
        if(spec.definition_resource==p->definition && spec.family_code==p->family && spec.pinmap==p->pinmap)profile=p;
    }
    for(size_t i=0;i<sizeof(pic_batch_profiles)/sizeof(*pic_batch_profiles);i++) {
        const lt48_pic_profile *p=&pic_batch_profiles[i];
        if(spec.definition_resource==p->definition && spec.family_code==p->family && spec.pinmap==p->pinmap) {
            if(profile)return LT48_SAFETY_REJECTED;
            if(!lt48_pic_member_allowed(pic_batch_members,
                sizeof(pic_batch_members)/sizeof(*pic_batch_members),
                spec.definition_resource,spec.family_code,spec.pinmap,
                spec.catalogue.vendor,spec.catalogue.device_name,spec.catalogue.device_name_length))
                return LT48_UNSUPPORTED;
            profile=p;
        }
    }
    if(!profile)return LT48_UNSUPPORTED;
    return pic_open_profile(x,&spec,profile,out);
}
/* Shared loader also supports offline qualification of generated candidates.
 * Public availability remains exclusively controlled by the admitted registry. */
/* A legacy source shares its ERASE callback with EEPROM-bearing parts. For
 * the explicit no-EEPROM source shape, omit only the data-bank erase command.
 * Retain the complete power/configuration sequence and all original waits.
 * Do not model an undocumented target command as harmless to make a test pass. */
static lt48_status pic_compile_empty_data_erase(const lt48_compact_wave *source,
    const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t var_count,
    const lt48_pic_internal_program_policy *policy,unsigned eeprom_bytes,
    lt48_compiled_wave **output) {
    if(!output)return LT48_INVALID_ARGUMENT;
    *output=NULL;
    if(!source||!pins||!vars||!policy)return LT48_INVALID_ARGUMENT;
    if(eeprom_bytes || !(policy->data_wait&LT48_PIC_INTERNAL_DORMANT_DATA) ||
       lt48_compact_instruction_count(source)!=10)return LT48_SAFETY_REJECTED;
    const unsigned bits[10]={6,1,14,1,6,0,0,6,0,0};
    const unsigned values[10]={0,0,0x3fff,0,9,0,0,11,0,0};
    unsigned char raw[128];size_t used=2;unsigned wait_id=256;
    for(unsigned i=0;i<10;i++) {
        lt48_compact_instruction in;lt48_status st=lt48_compact_get_instruction(source,i,&in);
        if(st!=LT48_OK)return st;
        if(bits[i]) {
            if(in.opcode!=0x1a||in.operand_size!=8||in.operands[0]!=policy->data_pin||
               in.operands[1]!=policy->clock_pin||in.operands[6]!=bits[i]||in.operands[7]!=1||
               (in.operands[2]|(unsigned)in.operands[3]<<8|(unsigned)in.operands[4]<<16|
                (unsigned)in.operands[5]<<24)!=values[i])return LT48_SAFETY_REJECTED;
        } else {
            if(in.opcode!=0x31||in.operand_size!=1)return LT48_SAFETY_REJECTED;
            if(wait_id==256)wait_id=in.operands[0];
            if(in.operands[0]!=wait_id)return LT48_SAFETY_REJECTED;
        }
        if(i==7)continue;
        raw[used++]=in.opcode;memcpy(raw+used,in.operands,in.operand_size);used+=in.operand_size;
    }
    unsigned found=0;
    for(size_t i=0;i<var_count;i++)if(vars[i].variable_id==wait_id) {
        if(vars[i].kind!=1||vars[i].enabled!=1||vars[i].value!=1||vars[i].channel<50)return LT48_SAFETY_REJECTED;
        found++;
    }
    if(found!=1)return LT48_SAFETY_REJECTED;
    raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
    lt48_compact_wave *changed=NULL;lt48_status status=lt48_compact_decode(raw,used,&changed);
    if(status==LT48_OK)status=lt48_compact_compile(changed,pins,vars,var_count,1,0,output);
    if(status==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(*output)) {
        lt48_compiled_wave_close(*output);*output=NULL;status=LT48_SAFETY_REJECTED;
    }
    lt48_compact_wave_close(changed);return status;
}

static lt48_status pic_open_profile(const lt48_installation *x,const lt48_device_spec *source,
    const lt48_pic_profile *profile,lt48_pic_resources **out) {
    if(!x||!source||!profile||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec=*source;lt48_status status;
    if(spec.catalogue.device_class!=2015 || strcmp(spec.definition_stem,"GADC___") || spec.element_bits!=14)return LT48_UNSUPPORTED;
    if(spec.definition_resource!=profile->definition || spec.family_code!=profile->family ||
        spec.pinmap!=profile->pinmap)return LT48_SAFETY_REJECTED;
    if(!lt48_pic_profile_valid(profile))return LT48_SAFETY_REJECTED;
    lt48_pic_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    r->profile=profile;
    r->core=profile->strategy==LT48_PIC_COMPACT_ROWS?1704:profile->strategy==LT48_PIC_COMPACT_LEGACY16?16:12;
    r->program_words=profile->program_words;r->size_seed=profile->size_seed;r->capacity=profile->image_span;
    if(spec.capacity_bytes!=r->capacity&&!lt48_pic_7x_source_capacity(profile,spec.capacity_bytes)){lt48_pic_close(r);return LT48_SAFETY_REJECTED;}
    char path[4096],name[32],hex[65];unsigned char digest[32];size_t index,fi;
    int n=snprintf(path,sizeof(path),"%s/GADC___.rez",lt48_installation_path(x));
    if(n<0||(size_t)n>=sizeof(path)){lt48_pic_close(r);return LT48_INVALID_ARGUMENT;}
#define TRY(a) do{status=(a);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,"24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec")){status=LT48_SAFETY_REJECTED;goto fail;}
    lt48_pin_code pc;snprintf(name,sizeof(name),"P%x",spec.pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    TRY(lt48_pic_72x_select_pinmap(profile,&r->pins));
    TRY(lt48_pic_7x_select_pinmap(profile,&r->pins));
    TRY(lt48_pic_enhanced_qfn44_select(profile,&r->pins));
    lt48_family_info f;snprintf(name,sizeof(name),"M%x",spec.family_code);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count!=profile->variable_count || f.operation_count!=profile->operation_count ||
        f.variable_count>sizeof(r->vars)/sizeof(*r->vars)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=f.variable_count;
    for(size_t i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    if(profile->classic14&&(lt48_pic_classic_is62x(profile->classic14->protocol)||lt48_pic_classic_is62xa(profile->classic14->protocol))) {
        TRY(lt48_pic_62x_power_apply(profile->power,r->vars,r->var_count));
    } else {TRY(lt48_pic_power_apply(profile->power,r->vars,r->var_count));}
    if(r->core==16){
        unsigned found=0;
        for(size_t i=0;i<f.operation_count;i++){
            lt48_family_operation op;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
            if(op.operation_id!=8)continue;
            lt48_wave_code source;snprintf(name,sizeof(name),"W%x",op.wave);
            TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&source));
            TRY(lt48_pic_legacy16_program_only(&source,&r->legacy_program_only));found++;
        }
        if(found!=1){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;lt48_compact_wave *c=NULL;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(op.operation_id>=32 || r->wave[op.operation_id]){status=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));
        if(w.wave_type){status=LT48_UNSUPPORTED;goto fail;}
        TRY(lt48_compact_decode(w.data,w.data_size,&c));
        if(profile->strategy==LT48_PIC_COMPACT_LEGACY16 && !r->legacy_program_only && op.operation_id==9){
            status=lt48_pic_legacy16_erase_compile(&w,r->pins,r->vars,r->var_count,&r->wave[op.operation_id]);
        } else if(profile->strategy==LT48_PIC_COMPACT_LEGACY16 && r->legacy_program_only && profile->configuration_count==1 &&
           profile->configuration[0].secure_clear==0x3ff0 && op.operation_id==3) {
            status=lt48_pic_legacy84a_entry_compile(c,r->pins,r->vars,r->var_count,&r->wave[op.operation_id]);
        } else if(profile->classic14 && lt48_pic_classic_isrow4(profile->classic14->protocol)&&(op.operation_id==3||op.operation_id==4)) {
            status=lt48_pic_row4_entry_compile(c,r->pins,r->vars,r->var_count,op.operation_id,profile->classic14->protocol,&r->wave[op.operation_id]);
        } else if(profile->classic14 && lt48_pic_classic_is87x(profile->classic14->protocol)&&(op.operation_id==3||op.operation_id==4)) {
            status=lt48_pic_87x_entry_compile(c,r->pins,r->vars,r->var_count,op.operation_id,profile->classic14->protocol,&r->wave[op.operation_id]);
        } else if(profile->classic14 && lt48_pic_classic_is7x(profile->classic14->protocol)&&(op.operation_id==3||op.operation_id==4)) {
            status=lt48_pic_7x_entry_compile(c,r->pins,r->vars,r->var_count,op.operation_id,profile->classic14->protocol,&r->wave[op.operation_id]);
        } else if(profile->classic14 && profile->classic14->protocol&&
           (lt48_pic_classic_is62x(profile->classic14->protocol)||lt48_pic_classic_is62xa(profile->classic14->protocol))&&(op.operation_id==3||op.operation_id==4)) {
            status=lt48_pic_62x_entry_compile(c,r->pins,r->vars,r->var_count,op.operation_id,profile->classic14->protocol,&r->wave[op.operation_id]);
        } else if(profile->classic14 && profile->classic14->protocol&&
           lt48_pic_classic_is72x(profile->classic14->protocol)&&(op.operation_id==3||op.operation_id==4)) {
            status=lt48_pic_72x_entry_compile(c,r->pins,r->vars,r->var_count,op.operation_id,profile->classic14->protocol,&r->wave[op.operation_id]);
        } else if(profile->classic14 && profile->classic14->protocol&&
           profile->classic14->protocol->configuration_words==2&&(op.operation_id==3||op.operation_id==4)) {
            status=pic_classic_entry_compile(c,r->pins,r->vars,r->var_count,op.operation_id,&r->wave[op.operation_id]);
        } else if(profile->classic14 && !profile->classic14->protocol && op.operation_id==3) {
            status=lt48_pic_classic14_identity_compile(c,r->pins,r->vars,r->var_count,
                profile->classic14->entry,&r->wave[op.operation_id]);
        } else if(profile->classic14 && !profile->classic14->protocol && (op.operation_id==8 || op.operation_id==10)) {
            status=lt48_pic_classic14_wait_compile(c,r->pins,r->vars,r->var_count,
                profile->classic14,op.operation_id,&r->wave[op.operation_id]);
        } else if(profile->entry && op.operation_id==profile->entry->operation_id) {
            status=lt48_pic_entry_compile(c,r->pins,r->vars,r->var_count,profile->entry,&r->wave[op.operation_id]);
        } else if(op.operation_id==8 && profile->internal_program) {
            const lt48_pic_internal_program_policy *policy=profile->internal_program;
            unsigned eeprom_bytes=0;
            for(size_t region=0;region<profile->region_count;region++)
                if(profile->regions[region].kind==LT48_PIC_REGION_EEPROM)
                    eeprom_bytes+=profile->regions[region].count;
            status=lt48_pic_internal_program_compile_policy(c,r->pins,r->vars,r->var_count,
                policy,eeprom_bytes,
                &r->wave[op.operation_id]);
        } else if(op.operation_id==9 && profile->internal_program &&
                  (profile->internal_program->data_wait&LT48_PIC_INTERNAL_DORMANT_DATA)) {
            unsigned eeprom_bytes=0;
            for(size_t region=0;region<profile->region_count;region++)
                if(profile->regions[region].kind==LT48_PIC_REGION_EEPROM)eeprom_bytes+=profile->regions[region].count;
            status=pic_compile_empty_data_erase(c,r->pins,r->vars,r->var_count,
                profile->internal_program,eeprom_bytes,&r->wave[op.operation_id]);
        } else if(profile->wait && profile->wait->operation_id==op.operation_id) {
            status=lt48_pic_wait_compile(c,r->pins,r->vars,r->var_count,
                profile->wait,&r->wave[op.operation_id]);
        } else status=lt48_compact_compile(c,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);
        lt48_compact_wave_close(c);if(status!=LT48_OK)goto fail;
        if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    if(profile->wait && !r->wave[profile->wait->operation_id]){status=LT48_SAFETY_REJECTED;goto fail;}
    if(profile->classic14 && (!r->wave[3] || !r->wave[8] || !r->wave[10])){status=LT48_SAFETY_REJECTED;goto fail;}
    if(profile->entry && !r->wave[profile->entry->operation_id]){status=LT48_SAFETY_REJECTED;goto fail;}
    if(profile->internal_program && !r->wave[8]){status=LT48_SAFETY_REJECTED;goto fail;}
    if(profile->word_program) {
        const lt48_pic_word_program_policy *w=profile->word_program;
        TRY(lt48_pic_word_program_compile(r->pins,r->vars,r->var_count,
            w->data_pin,w->clock_pin,lt48_pic_word_timing_variable(w),lt48_pic_word_protocol(w),&r->word_program));
    }
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_pic_close(r);return status;
#undef TRY
}
static unsigned word_mask(const lt48_pic_resources *r,unsigned a) {
    if(r->remaining14){rm14_otp_policy p;if(rm14_public_policy(r->remaining14,&p)!=LT48_OK)return 0;return otp14_operator_mask(&p.common,a);}
    if(r->otp12)return b12otp_mask(&r->otp12->policy,a);
    if(r->tail14)return lt48_pic_tail14_mask(lt48_pic_tail14_policies+r->tail14->policy,a);
    if(r->calibrated14)return lt48_pic_cal14_mask(a);
    if(r->midrange14){const lt48_pic_midrange14_profile *p=r->midrange14;lt48_pic_midrange_operation_policy policy={&p->protocol,p->configuration_mask,p->code_protect,p->identity_mask,p->identity_value};return lt48_pic_midrange_mask(&policy,a);}
    if(r->otp){otp14_operator_policy policy=pic_otp_policy(r->otp);return otp14_operator_mask(&policy,a);}
    return lt48_pic_profile_mask(r->profile,a);
}
static unsigned pic_legacy_protection_mask(const lt48_pic_resources *r) {
    if(r->core==16 && r->profile && r->profile->configuration_count==1 &&
       r->profile->configuration[0].image_offset==0x400e &&
       r->profile->configuration[0].secure_clear)
        return r->profile->configuration[0].secure_clear;
    return r->core==16?16u:64u;
}
lt48_status lt48_pic_otp14_layout(const lt48_pic_resources *r,lt48_pic_otp14_layout_info *out){
 if(!r||!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof *out);if(!r->remaining14)return LT48_UNSUPPORTED;
 rm14_otp_policy policy;lt48_status st=rm14_public_policy(r->remaining14,&policy);if(st!=LT48_OK)return st;
 out->image_bytes=r->capacity;out->code_words=r->program_words;out->calibration_word=policy.factory_word;
 out->calibration_words=policy.factory_word!=UINT32_MAX;
 if(r->remaining14->family==44){out->calibration_word=RM14_CAL_FIRST;out->calibration_words=RM14_CAL_END-RM14_CAL_FIRST;}
 return LT48_OK;
}
lt48_status lt48_pic_legacy16_policy(const lt48_pic_resources *r,lt48_pic_legacy16_policy_info *out) {
    if(!r||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    if(r->core!=16 || !r->profile)return LT48_UNSUPPORTED;
    out->code_words=r->program_words;out->protection_mask=(uint16_t)pic_legacy_protection_mask(r);
    for(size_t i=0;i<r->profile->region_count;i++)if(r->profile->regions[i].kind==LT48_PIC_REGION_EEPROM)out->eeprom_bytes=r->profile->regions[i].count;
    out->maximum_verify_centivolts=600;
    if(r->profile->power)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==6)out->maximum_verify_centivolts=(uint16_t)r->vars[i].value;
    out->program_erases_first=out->over_program_clears_only=r->legacy_program_only;
    return LT48_OK;
}
lt48_status lt48_pic_validate(const lt48_pic_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r)return LT48_INVALID_ARGUMENT;
    if(op<LT48_OPERATION_READ || op>LT48_OPERATION_SECURE || !(lt48_pic_operation_mask_internal(r)&(1u<<(op-1))))return LT48_UNSUPPORTED;
    /* Full-image operation: serial device counters and delayed configuration
     * commits require traversal from address zero, never arbitrary starts. */
    if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
    if(r->pic18c)return op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE?pic18c_image_validate(r->pic18c,image,size):LT48_OK;
    if(r->older17)return pic_older17_validate(r,op,image,size);
    if(r->remaining14)return pic_remaining14_validate(r,op,image,size);
    if(r->otp12){
        if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE) {
            if(!image||size!=length)return LT48_INVALID_ARGUMENT;
            for(unsigned a=0;a<4096;a++)if(!b12otp_mask(&r->otp12->policy,a)&&((image[2*a]|(unsigned)image[2*a+1]<<8)&4095)!=4095)return LT48_SAFETY_REJECTED;
            unsigned cp=(image[8190]|(unsigned)image[8191]<<8)&r->otp12->policy.protection_mask;
            if(cp&&cp!=r->otp12->policy.protection_mask)return LT48_SAFETY_REJECTED;
        }
        return LT48_OK;
    }
    if(r->tail14){
        if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE){
            if(!image||size!=length)return LT48_INVALID_ARGUMENT;
            const lt48_pic_tail14_policy *p=lt48_pic_tail14_policies+r->tail14->policy;
            for(unsigned a=0;a<p->file_words;a++){
                unsigned v=image[2*a]|(unsigned)image[2*a+1]<<8;
                if(v!=0xffff&&v!=0x3fff&&(v>0x3fff||(a!=0x2007&&(v&~lt48_pic_tail14_mask(p,a)))))return LT48_SAFETY_REJECTED;
            }
        }
        return LT48_OK;
    }
    if(r->calibrated14){
        if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE){
            if(!image||size!=length)return LT48_INVALID_ARGUMENT;
            for(unsigned a=0;a<LT48_CAL14_WORDS;a++){
                unsigned v=image[2*a]|(unsigned)image[2*a+1]<<8;
                if(v!=0xffff&&v!=0x3fff&&(v>0x3fff||(a!=LT48_CAL14_CFG&&(v&~lt48_pic_cal14_mask(a)))))return LT48_SAFETY_REJECTED;
            }
        }
        return LT48_OK;
    }
    if(r->baseline12){
        if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE){
            if(!image||size!=length)return LT48_INVALID_ARGUMENT;
            for(unsigned a=0;a<0x1000;a++)if(!lt48_pic_baseline12_source_mask(r->baseline12,a)&&
                ((image[2*a]|(unsigned)image[2*a+1]<<8)&0xfff)!=0xfff)return LT48_SAFETY_REJECTED;
        }
        return LT48_OK;
    }
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE) {
        if(!image || size!=length)return LT48_INVALID_ARGUMENT;
        if(r->otp){
            otp14_operator_policy policy=pic_otp_policy(r->otp);
            uint16_t cfg=(uint16_t)((image[0x400e]|(unsigned)image[0x400f]<<8)&0x3fff);
            if(!otp14_configuration_target_valid(&policy.configuration,cfg))return LT48_SAFETY_REJECTED;
        }
        if(r->profile && r->core==16 && pic_legacy_protection_mask(r)==0x3ff0 &&
           (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) {
            unsigned cp=(image[0x400e]|(unsigned)image[0x400f]<<8)&0x3ff0;
            if(cp && cp!=0x3ff0)return LT48_SAFETY_REJECTED;
        }
        if(r->midrange14)for(unsigned a=0;a<r->capacity/2;a++){unsigned v=image[2*a]|(unsigned)image[2*a+1]<<8;if(v>0x3fff&&v!=0xffff)return LT48_SAFETY_REJECTED;}
        if(r->profile&&r->profile->classic14&&lt48_pic_classic_is62x(r->profile->classic14->protocol)){
            unsigned cfg=image[0x400e]|(unsigned)image[0x400f]<<8;
            if(((cfg>>10)&3)!=((cfg>>12)&3))return LT48_SAFETY_REJECTED;
        }
        if(r->profile&&r->profile->classic14&&lt48_pic_classic_is87x(r->profile->classic14->protocol)){
            unsigned cfg=image[0x400e]|(unsigned)image[0x400f]<<8;
            if(!lt48_pic_87x_configuration_valid(r->profile->classic14->protocol,cfg&0x3fff))return LT48_SAFETY_REJECTED;
        }
        for(unsigned a=0;a<r->capacity/2;a++)if(!word_mask(r,a) && (image[a*2]!=255 || (image[a*2+1]&63)!=63))return LT48_SAFETY_REJECTED;
    }
    return LT48_OK;
}
lt48_status lt48_pic_validate_verify_options(const lt48_pic_resources *r,lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type) {
    (void)op;
    if(!r||type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
    return LT48_OK; /* Source VDDmin/max, never generic percentage margins. */
}
typedef struct pic_session {
    const lt48_pic_resources *r;const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;uint32_t volts[4];lt48_compile_variable vars[16];
    unsigned char *image;size_t image_capacity;unsigned mode;
    unsigned char buffers[LT48_RUNTIME_BUFFER_COUNT][32];
    lt48_runtime_configuration_value config[1024];size_t config_count;
    lt48_pic_source_session *source;
} pic_session;
static void dword(unsigned char *p,unsigned v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
static unsigned word(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static void put_word(unsigned char *p,unsigned v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
static lt48_status compact_source_runner(void *context,lt48_runtime_machine *machine,lt48_runtime_result **result) {
    pic_session *s=context;
    return lt48_runtime_transport_session_run(s->wire,machine,result);
}
static lt48_status compact_source_run(pic_session *s,unsigned op,unsigned address,unsigned value,
    unsigned *read_value,unsigned timeout) {
    /* Legacy compact dispatch starts with fresh scratch buffers for each
     * callback; rows retain protocol/cursor state. Both share the executor. */
    if(s->r->profile->strategy!=LT48_PIC_COMPACT_ROWS) {
        memset(s->buffers,0,sizeof(s->buffers));dword(s->buffers[22],s->r->size_seed);
    }
    size_t span=s->image_capacity?s->image_capacity:s->r->capacity;
    dword(s->buffers[2],address);dword(s->buffers[9],s->mode);dword(s->buffers[6],value);
    lt48_pic_source_state state={s->vars,s->r->var_count,s->config,s->config_count,
        s->image,span,(const unsigned char (*)[32])s->buffers,{0},0,0};
    memcpy(state.voltages,s->volts,sizeof(s->volts));
    lt48_status status;
    if(!s->source) {
        lt48_runtime_nested_wave callbacks[32];size_t count=0;
        for(unsigned i=0;i<32;i++)if(s->r->wave[i])callbacks[count++]=(lt48_runtime_nested_wave){(uint16_t)i,s->r->wave[i]};
        lt48_runtime_buffer_seed seeds[LT48_RUNTIME_BUFFER_COUNT];
        for(unsigned i=0;i<LT48_RUNTIME_BUFFER_COUNT;i++)seeds[i]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
        lt48_runtime_options options={0};options.device_image=s->image;options.device_image_size=span;
        options.device_element_bytes=2;options.configuration=s->config;options.configuration_count=s->config_count;
        options.initial_buffers=seeds;options.initial_buffer_count=LT48_RUNTIME_BUFFER_COUNT;
        memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));
        status=lt48_pic_source_session_create_internal(s->r->pins,s->vars,s->r->var_count,
            callbacks,count,&options,&s->source);
    } else status=lt48_pic_source_session_restore_internal(s->source,&state);
    if(status!=LT48_OK)return status;
    const lt48_compiled_wave *selected=NULL;
    const lt48_pic_word_program_policy *policy=s->r->profile->word_program;
    if(op==8 && policy && address>=policy->address && (uint64_t)address-policy->address<policy->count) {
        selected=s->r->word_program;if(!selected)return LT48_SAFETY_REJECTED;
    }
    status=lt48_pic_source_session_invoke_wave_internal(s->source,(uint16_t)op,selected,
        s->mode,address,timeout,compact_source_runner,s);
    if(status==LT48_OK || status==LT48_DEVICE_MISMATCH) {
        lt48_status exported=lt48_pic_source_session_state_internal(s->source,&state);
        if(exported!=LT48_OK)return exported;
        if(state.image_size!=span || state.variable_count!=s->r->var_count || state.configuration_count>1024)
            return LT48_RUNTIME_ERROR;
        memcpy(s->image,state.image,state.image_size);memcpy(s->buffers,state.buffers,sizeof(s->buffers));
        memcpy(s->vars,state.variables,state.variable_count*sizeof(*s->vars));
        memcpy(s->volts,state.voltages,sizeof(s->volts));
        if(state.configuration_count)memcpy(s->config,state.configuration,state.configuration_count*sizeof(*s->config));
        s->config_count=state.configuration_count;
        if(read_value)*read_value=word(s->buffers[5]);
    }
    return status;
}
static lt48_status run_with_timeout(pic_session *s,unsigned op,unsigned address,unsigned value,unsigned *read_value,unsigned timeout) {
    if(op>=32 || !s->r->wave[op])return LT48_UNSUPPORTED;
    return compact_source_run(s,op,address,value,read_value,timeout);
}

static lt48_status run(pic_session *s,unsigned op,unsigned address,unsigned value,unsigned *read_value) {
    return run_with_timeout(s,op,address,value,read_value,10);
}
static lt48_status reset(pic_session *s) {return run(s,4,0,0,NULL);}
static lt48_status start(pic_session *s,unsigned vcc) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status st=run(s,0,0,0,NULL);
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);
    if(st==LT48_OK)st=run(s,3,0,0,NULL);
    return st;
}
static unsigned voltage(pic_session *s,unsigned id) {
    if(s->r->core==16 && (id==5 || id==6)) {
        unsigned selected=(word(s->image+0x400e)&3)==2 ? (id==5?450:550):(id==5?400:600);
        if(s->r->profile->power)for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==id){
            if(id==6 && selected>s->vars[i].value)selected=s->vars[i].value;
            if(id==5 && selected<s->vars[i].value)selected=s->vars[i].value;
        }
        return selected;
    }
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==id)return s->vars[i].value;
    return 0;
}
static lt48_status scan(pic_session *s,lt48_operation_kind op,const unsigned char *expected,unsigned char *dest,unsigned vcc,lt48_pic_result *out) {
    /* Read is the source's actual acquisition path. Some legacy callbacks
     * skip mode 11 entirely; comparison and blank decisions belong here. */
    s->mode=1;
    lt48_status st=start(s,vcc);
    if(dest)for(unsigned a=0;a<s->r->capacity/2;a++)put_word(dest+2*a,0x3fff);
    for(unsigned a=0;st==LT48_OK && a<s->r->capacity/2;a++) {
        unsigned actual=0,mask=word_mask(s->r,a),want=expected?word(expected+2*a):0x3fff;
        st=run(s,s->r->core==16 && a==0x2007?16:6,a,want,&actual);
        if(st!=LT48_OK)break;
        if(mask && dest)put_word(dest+2*a,actual);
        if(mask && op!=LT48_OPERATION_READ && ((want^actual)&mask)) {
            if(!out->mismatch_count)out->first_mismatch=2*a;
            out->mismatch_count++;
        }
        out->completed_bytes=2*a+2;
        if(s->api->progress && !(a&127))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,2*a+2,s->r->capacity,0,0,0);
        if(out->mismatch_count && op!=LT48_OPERATION_COMPARE){st=LT48_DEVICE_MISMATCH;break;}
    }
    lt48_status end=reset(s);if(st==LT48_OK)st=end;
    if(st==LT48_OK && out->mismatch_count)st=LT48_DEVICE_MISMATCH;
    return st;
}
static lt48_status finish(pic_session *s,lt48_status operation,lt48_pic_result *out) {
    /* Retain a successful callback's pending STOP before changing queues. */
    if(operation==LT48_OK&&s->wire){lt48_status drained=lt48_runtime_transport_session_flush(s->wire);if(drained!=LT48_OK)operation=drained;}
    out->operation_status=operation;
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
    lt48_protocol_transport clean=*s->api;clean.cancelled=NULL;lt48_status first=LT48_OK;
    for(unsigned phase=0;phase<7;phase++) {
        lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
        lt48_status st=lt48_runtime_transport_session_create(&clean,s->context,&s->wire);
        if(st==LT48_OK) {
            if(phase==0)st=run(s,1,0,0,NULL);
            else if(phase<=4){unsigned char off[]={0x10,(unsigned char)(4-phase),0};st=lt48_runtime_transport_session_controller_write(s->wire,off,3);}
            else if(phase==5){const unsigned char done[]={0x49,0xaa,0x55};st=lt48_runtime_transport_session_controller_write(s->wire,done,3);}
            else {unsigned char led[]={0x40,operation==LT48_OK && first==LT48_OK?4:2};st=lt48_runtime_transport_session_controller_write(s->wire,led,2);}
            if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
        }
        if(first==LT48_OK)first=st;
    }
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
    out->cleanup_status=first;out->cleanup_completed=first==LT48_OK;
    return operation==LT48_OK?first:operation;
}
#include "pic1704_operations.inc"
#include "pic_baseline12.inc"
#include "pic_calibrated14.inc"
#include "pic_tail14.inc"
#include "pic_baseline12_otp.inc"
#include "../../NewDeviceLib/sources/pic-common/midrange14_adapter.h"
static lt48_status pic_otp_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,const unsigned char *image,unsigned char *read,lt48_verify_margin_selection margin,lt48_pic_result *out) {
    size_t n=OTP14_IMAGE_WORDS*sizeof(uint16_t);uint16_t *target=image?malloc(n):NULL,*scratch=malloc(n),*pending=malloc(n);
    if((image&&!target)||!scratch||!pending){free(target);free(scratch);free(pending);return LT48_NO_MEMORY;}
    if(image)for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)target[a]=word(image+2*a)&0x3fff;
    otp14_operator_result result={0};lt48_status st=otp14_transport_execute_options(r->otp_path,r->otp_hash,
        r->otp->family,r->otp->pinmap,pic_otp_policy(r->otp),r->idle,r->idle_size,api,context,
        op,target,pending,scratch,&result,margin);
    out->operation_status=result.operation_status;out->cleanup_status=result.cleanup.status;
    out->cleanup_completed=result.cleanup.completed;out->mismatch_count=result.mismatches;out->first_mismatch=result.first_mismatch;
    if(st==LT48_OK){out->completed_bytes=r->capacity;if(op==LT48_OPERATION_READ)for(unsigned a=0;a<OTP14_IMAGE_WORDS;a++)put_word(read+2*a,pending[a]);}
    free(target);free(scratch);free(pending);return st;
}

#include "../../NewDeviceLib/sources/pic-common/remaining14_adapter.h"
#include "../../NewDeviceLib/sources/pic-common/older17_adapter.h"
#include "../../NewDeviceLib/sources/pic-common/pic18c_adapter.h"
lt48_status lt48_pic_execute(const lt48_pic_resources *r,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,uint32_t offset,size_t length,const unsigned char *image,size_t size,
    unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_pic_result *out) {
    if(!out || !api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_pic_validate(r,op,offset,length,image,size);if(st!=LT48_OK)return st;
    st=lt48_pic_validate_verify_options(r,op,margin,type);if(st!=LT48_OK)return st;
    if(op==LT48_OPERATION_READ && (!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    if(r->pic18c)return pic18c_execute(r,api,context,op,image,read,margin,type,out);
    if(r->older17)return pic_older17_execute(r,api,context,op,image,read,margin,type,out);
    if(r->remaining14)return pic_remaining14_execute(r,api,context,op,image,read,margin,out);
    if(r->otp12)return pic_otp12_execute(r,api,context,op,image,read,out);
    if(r->tail14)return pic_tail14_execute(r,api,context,op,image,read,margin,out);
    if(r->calibrated14)return pic_cal14_execute(r,api,context,op,image,read,margin,out);
    if(r->midrange14)return pic_midrange14_execute(r,api,context,op,image,read,margin,out);
    if(r->baseline12)return pic_baseline12_execute(r,api,context,op,image,read,margin,out);
    if(r->otp)return pic_otp_execute(r,api,context,op,image,read,margin,out);
    if(r->profile->strategy==LT48_PIC_COMPACT_ROWS)return pic_compact_rows_execute(r,api,context,op,image,read,margin,out);
    pic_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;
    memcpy(s.vars,r->vars,sizeof(s.vars));s.image=malloc(length);
    unsigned char *pending=malloc(length),*expected=malloc(length);
    if(!s.image||!pending||!expected){free(s.image);free(pending);free(expected);return LT48_NO_MEMORY;}
    for(unsigned a=0;a<length/2;a++)put_word(s.image+2*a,image?(word(image+2*a)&0x3fff):0x3fff);
    memcpy(expected,s.image,length);
    /* PIC16 PROGRAMconfig reads/restores this explicit operation option. */
    s.config[0]=(lt48_runtime_configuration_value){-1,0,5,0};s.config_count=1;
    st=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(st!=LT48_OK)goto done;
    /* Preparation owns safe rails and a complete FPGA download. Sending
     * cleanup commands into a partial FPGA stream cannot establish safety. */
    for(unsigned phase=0;st==LT48_OK&&phase<5;phase++){
        const unsigned char off[]={0x10,(unsigned char)(3-phase),0},clear[]={0x49,0xaa,0x55};
        st=lt48_runtime_transport_session_controller_write(s.wire,phase<4?off:clear,3);
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
    if(st!=LT48_OK){out->operation_status=st;out->cleanup_status=LT48_CLEANUP_FAILED;goto done;}
    const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,0x0f},busy[]={0x56,0x0a,1,0x40,1};
    st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,fpga,3);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,selector,2);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
    if(st!=LT48_OK){out->operation_status=st;out->cleanup_status=LT48_CLEANUP_FAILED;goto done;}
    if(st==LT48_OK && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) {
        /* Delay CP until after code verification. Keep all other config bits. */
        unsigned cp=pic_legacy_protection_mask(r);unsigned target=word(expected+0x400e);
        put_word(s.image+0x400e,target|cp);put_word(expected+0x400e,target|cp);
        if(r->core==16 && r->legacy_program_only) {
            /* 84A uses programming-only EEPROM cycles. A normal full-image
             * Program therefore erases first; Over Program clears bits only. */
            if(op==LT48_OPERATION_PROGRAM) {
                s.mode=7;st=start(&s,500);if(st==LT48_OK)st=run(&s,9,0,0,NULL);
                lt48_status en=reset(&s);if(st==LT48_OK)st=en;
            } else {
                st=scan(&s,LT48_OPERATION_READ,NULL,pending,500,out);
                for(unsigned a=0;st==LT48_OK && a<length/2;a++) {
                    unsigned mask=word_mask(r,a);
                    if((word(expected+2*a)&~word(pending+2*a))&mask)st=LT48_SAFETY_REJECTED;
                }
            }
        }
        if(st==LT48_OK && r->core==16 && r->legacy_program_only)memcpy(s.image,expected,length);
        s.mode=2;if(st==LT48_OK)st=start(&s,500);
        for(unsigned a=0;st==LT48_OK && a<length/2;a++) {
            st=run(&s,8,a,word(expected+2*a),NULL);
            if(api->progress && !(a&127))api->progress(context,LT48_EXECUTION_READ_OPERATION,2*a+2,length,0,0,0);
        }
        /* PIC12 PROGRAM intentionally skips CONFIG; PROTECT is its complete
         * configuration writer and is also used with CP left unprotected. */
        if(st==LT48_OK && r->core==12)st=run(&s,10,0,0,NULL);
        lt48_status end=reset(&s);if(st==LT48_OK)st=end;
        if(st==LT48_OK)st=scan(&s,LT48_OPERATION_VERIFY,expected,NULL,voltage(&s,5),out);
        if(st==LT48_OK)st=scan(&s,LT48_OPERATION_VERIFY,expected,NULL,voltage(&s,6),out);
        if(st==LT48_OK && !(target&cp)) {
            put_word(s.image+0x400e,target);s.mode=8;st=start(&s,500);
            if(st==LT48_OK)st=run(&s,10,0,0,NULL);
            lt48_status end=reset(&s);if(st==LT48_OK)st=end;
            if(st==LT48_OK)st=scan(&s,LT48_OPERATION_READ,NULL,pending,500,out);
            if(st==LT48_OK && (word(pending+0x400e)&cp))st=LT48_DEVICE_MISMATCH;
        }
    } else if(st==LT48_OK && op==LT48_OPERATION_ERASE) {
        s.mode=7;st=start(&s,500);if(st==LT48_OK)st=run(&s,9,0,0,NULL);
        lt48_status end=reset(&s);if(st==LT48_OK)st=end;
        if(st==LT48_OK)st=scan(&s,LT48_OPERATION_BLANK_CHECK,NULL,NULL,500,out);
    } else if(st==LT48_OK && op==LT48_OPERATION_SECURE) {
        st=scan(&s,LT48_OPERATION_READ,NULL,pending,500,out);
        if(st==LT48_OK) {
            memcpy(s.image,pending,length);put_word(s.image+0x400e,word(pending+0x400e)&~pic_legacy_protection_mask(r));
            s.mode=8;st=start(&s,500);if(st==LT48_OK)st=run(&s,10,0,0,NULL);
            lt48_status end=reset(&s);if(st==LT48_OK)st=end;
            if(st==LT48_OK)st=scan(&s,LT48_OPERATION_READ,NULL,pending,500,out);
            if(st==LT48_OK && (word(pending+0x400e)&pic_legacy_protection_mask(r)))st=LT48_DEVICE_MISMATCH;
        }
    } else if(st==LT48_OK && op==LT48_OPERATION_VERIFY && margin!=LT48_VERIFY_MARGIN_NOMINAL) {
        /* DEVINIT derives PIC16 oscillator-dependent limits from config. */
        if(margin!=LT48_VERIFY_MARGIN_HIGH)st=scan(&s,op,expected,NULL,voltage(&s,5),out);
        if(st==LT48_OK && margin!=LT48_VERIFY_MARGIN_LOW)st=scan(&s,op,expected,NULL,voltage(&s,6),out);
    } else if(st==LT48_OK)st=scan(&s,op,
        (op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE)?expected:NULL,
        op==LT48_OPERATION_READ?pending:NULL,500,out);
    out->operation_status=st;st=finish(&s,st,out);
    if(st==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,pending,length);
    if(st==LT48_OK)out->completed_bytes=length;
done:lt48_runtime_transport_session_close(s.wire);lt48_pic_source_session_close_internal(s.source);free(s.image);free(pending);free(expected);return st;
}
