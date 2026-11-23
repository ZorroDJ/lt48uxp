#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../NewDeviceLib/sources/lv088three/unlock.h"

/* Original Dataman XP byte/word-flash resources and secondary protection geometry.
 * TSOP packages require the selected source adapter, not a straight adapter. */
#include "xp_flash_sectors.inc"
#include "xp_shared_word_shapes.inc"
#include "xp_primary_erase.inc"
#include "../../NewDeviceLib/sources/lv800three/members.h"
#include "../../NewDeviceLib/sources/lv800three/admission.h"
#include "../../NewDeviceLib/sources/lv800three/profiles.h"
#include "../../NewDeviceLib/sources/parallel20/profiles.h"
#include "../../NewDeviceLib/sources/parallel20/admission.h"
#include "../../NewDeviceLib/sources/sharp13/profiles.h"
#include "../../NewDeviceLib/sources/sharp13/admission.h"
#include "../../NewDeviceLib/sources/xp13/profiles.h"
#include "../../NewDeviceLib/sources/tpi15/profiles.h"
#include "../../NewDeviceLib/sources/nxp18/profiles.h"
#include "../../NewDeviceLib/sources/nxp18/admission.h"
#include "../../NewDeviceLib/sources/nxp18/image-policy.h"
#include "../../NewDeviceLib/sources/lpc16/profiles.h"
#include "../../NewDeviceLib/sources/lpc16/admission.h"
#include "../../NewDeviceLib/sources/lpc16/image-policy.h"
#include "../../NewDeviceLib/sources/nor2/profiles.h"
#define LT48_TMP1962ONE_XP_INTERNAL
#include "../../NewDeviceLib/sources/tmp1962one/profiles.h"
#undef LT48_TMP1962ONE_XP_INTERNAL
#include "../../NewDeviceLib/sources/xp-word/atmel_paired_program.h"
#include "../../NewDeviceLib/sources/xp-word/atmel64_four.h"
#include "../../NewDeviceLib/sources/xp-word/st102.h"
#include "generated/xp_atmel_sanyo15_batch.inc"
#include "../../NewDeviceLib/sources/xp-word/amic_unused_vid.h"
#include "intel_byte_closures.inc"
#include "../../NewDeviceLib/sources/xp-byte/ice28lf010.h"
#include "../../NewDeviceLib/sources/xp-byte/sharp1305.h"
#include "../../NewDeviceLib/sources/xp-byte/amd28fa.h"
#include "../../NewDeviceLib/sources/xp-byte/atmel_ll.h"
#include "../../NewDeviceLib/sources/xp-byte/mux_extensions.h"
#include "../../NewDeviceLib/sources/xp-byte/mux_variants.h"
#include "../../NewDeviceLib/sources/xp-byte/sanyo_sector256.h"
#include "xp_c3_closures.inc"
#include "xp_byte_closures.inc"
#include "../../NewDeviceLib/sources/xp-byte/lpc29.h"
#include "../../NewDeviceLib/sources/xp-byte/tail37.h"
#include "../../NewDeviceLib/sources/xp-byte/atmel_offpackage_vid.h"
#include "page_wide_closures.inc"
#include "page_byte21_closures.inc"
#include "intel_word_closures.inc"
#include "../../NewDeviceLib/sources/memreuse7/members.h"
#include "../../NewDeviceLib/sources/memreuse7/xp-profiles.h"
#include "../../NewDeviceLib/sources/memreuse7/defaults.h"
#include "../../NewDeviceLib/sources/intel-word/intel29.h"
#include "../../NewDeviceLib/sources/st-m36wwr3/profiles.h"
#include "../../NewDeviceLib/sources/intel-word/seiko12.h"
#include "../../NewDeviceLib/sources/intel-word/word-tail22.h"
#include "../../NewDeviceLib/sources/intel-word/atmel_secure_bga.h"
#include "../../NewDeviceLib/sources/intel-word/shared_defaults.h"
#include "../../NewDeviceLib/sources/page-wide/remaining_guards.h"
#include "../../NewDeviceLib/sources/intel-word/st_lock_write.h"
#include "../../NewDeviceLib/sources/xp-byte/efst_dead_protection.h"
static const uint32_t at17f_sector[]={0};
static const struct xp_closure at17f_closures[]={
 {176,76,68,20,524288,330,at17f_sector,1},
 {182,76,70,20,524288,330,at17f_sector,1},
 {183,78,71,32,524288,330,at17f_sector,1},
 {177,76,68,20,1048576,330,at17f_sector,1},
 {178,77,69,44,1048576,330,at17f_sector,1},
 {184,76,70,20,1048576,330,at17f_sector,1},
 {185,78,71,32,1048576,330,at17f_sector,1},
 {179,76,68,20,2097152,330,at17f_sector,1},
 {180,77,69,44,2097152,330,at17f_sector,1},
 {186,76,70,20,2097152,330,at17f_sector,1},
 {181,77,69,44,4194304,330,at17f_sector,1},
 {187,76,70,20,4194304,330,at17f_sector,1},
};
struct lt48_xp_flash_resources {
    const struct xp_shared_word_shape *shared;
    const struct xp_primary_erase *primary;
    unsigned tmp1962one,parallel20,sharp13,nxp18,emtc18,lpc16,nand15,tpi15,otp11;
    unsigned at29,page_bytes,boot,sst_page,atmel64,ice2,mux_bulk_status,at17f,c3,applet_count,element_bytes,protection_choice,legacy28f,as15,paired_program,sanyo256;
    const struct xp_byte_closure *byte;
    const struct intel_byte_closure *intel;
    const struct page_wide_closure *page;
    const struct intel_word_closure *word;
    const struct xp_extended_id *id_table;
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[16];
    lt48_wave_code applet[10];
    unsigned char *owned_applet[10];
    lt48_applet_address addresses[10];
    const unsigned char *fpga;
    lt48_compile_variable vars[8];
    size_t var_count;
    uint32_t capacity;
    uint16_t man,dev,nominal;
    const struct xp_closure *closure;
};
void lt48_xp_flash_close(lt48_xp_flash_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    for(unsigned i=0;i<10;i++)free(r->owned_applet[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
unsigned lt48_xp_flash_secure_kind(const lt48_xp_flash_resources *r){return !r?0:(r->emtc18||r->nand15)?0:r->shared?0:r->word?r->word->settings.secure_kind:r->intel?0:r->byte?r->byte->secure:r->page?r->page->secure_kind:(r->at17f||r->c3)?0:1;}
unsigned lt48_xp_flash_protection_choices(const lt48_xp_flash_resources *r){return r&&r->at29&&r->boot?3:r&&r->byte?r->byte->choice:0;}
const char *lt48_xp_flash_protection_choice_name(const lt48_xp_flash_resources *r,unsigned choice){
    unsigned kind=lt48_xp_flash_protection_choices(r);
    if(!kind || choice<1 || choice>2)return NULL;
    if(kind==3)return choice==1?"Permanently lock lower boot block":"Permanently lock upper boot block";
    return kind==1?(choice==1?"Bottom block":"Top block"):(choice==1?"Boot lock 16 KiB":"Boot lock 64 KiB");
}
lt48_status lt48_xp_flash_set_protection_choice(lt48_xp_flash_resources *r,unsigned choice){
    if(!r)return LT48_INVALID_ARGUMENT;
    if(!lt48_xp_flash_protection_choices(r))return LT48_UNSUPPORTED;
    if(choice<1 || choice>2)return LT48_INVALID_ARGUMENT;
    r->protection_choice=choice;return LT48_OK;
}
uint32_t lt48_xp_flash_capacity(const lt48_xp_flash_resources *r){return r?r->capacity:0;}
static lt48_status xp_flash_open_impl(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out,int candidate) {
    lt48_device_spec spec;lt48_family_info family;lt48_pin_code pc;
    lt48_xp_flash_resources *r=NULL;lt48_status status;char path[4096],name[24],hex[65];
    size_t index,fi;unsigned char digest[32];
    if(!out||!x)return LT48_INVALID_ARGUMENT;
    *out=NULL;status=lt48_installation_resolve_device(x,device,&spec);if(status!=LT48_OK)return status;
    int at17f=spec.catalogue.device_class==2033 && spec.definition_resource>=0xb0 && spec.definition_resource<=0xbb;
    int c3=spec.catalogue.device_class==2033 && !at17f;
    int pagebyte=spec.catalogue.device_class==2041 && spec.definition_resource>=0x14 && spec.definition_resource<=0x1a;
    int page=spec.catalogue.device_class==2042 || pagebyte;
    int byte=spec.catalogue.device_class==2030;
    int intel=spec.catalogue.device_class==2032;
    if(spec.catalogue.device_class!=2031 && !at17f && !c3 && !page && !byte && !intel && !(spec.catalogue.device_class==2038&&(candidate==19||candidate==20)))return LT48_UNSUPPORTED;
    const struct xp_closure *closure=NULL;
    const struct xp_closure *list=c3?c3_closures:at17f?at17f_closures:xp_closures;
    size_t total=c3?sizeof(c3_closures)/sizeof(*c3_closures):at17f?sizeof(at17f_closures)/sizeof(*at17f_closures):sizeof(xp_closures)/sizeof(*xp_closures);
    for(size_t i=0;i<total;i++) {
        const struct xp_closure *c=&list[i];
        if(spec.definition_resource==c->definition && spec.family_code==c->family &&
           spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==16)closure=c;
    }
    int lv800=lv800three_member(&spec.catalogue);
    if(spec.catalogue.device_class==2031 && spec.definition_resource==197){
        if(!lv800||(!LT48_LV800THREE_ADMITTED&&candidate!=22))return LT48_UNSUPPORTED;
        const struct xp_closure*c=&lv800three_closure;
        if(spec.family_code!=c->family||spec.pinmap!=c->pinmap||spec.pin_count!=c->pins||spec.capacity_bytes!=c->capacity||spec.element_bits!=16)return LT48_SAFETY_REJECTED;
        closure=c;
    }
    if(candidate==21){if(!tmp1962one_binding(&spec))return LT48_UNSUPPORTED;closure=&tmp1962one_closure;}
    const struct p20_profile*p20=p20_find(spec.catalogue.device_class,spec.definition_resource);
    if(candidate==17){if(!p20||!p20_binding(&spec))return LT48_UNSUPPORTED;const struct xp_closure*c=&p20->base;if(spec.family_code!=c->family||spec.pinmap!=c->pinmap||spec.pin_count!=c->pins||spec.capacity_bytes!=c->capacity||spec.element_bits!=16)return LT48_SAFETY_REJECTED;closure=c;}
    const struct sh13_profile*sh13=sh13_find(spec.catalogue.device_class,spec.definition_resource);
    if(candidate==18&&xp13_binding(&spec.catalogue)&&spec.definition_resource==22)sh13=&xp13_alias_bottom;
    if(candidate==18){if(!sh13||(!sh13_binding(&spec)&&!xp13_binding(&spec.catalogue)))return LT48_UNSUPPORTED;const struct xp_closure*c=&sh13->base;if(spec.family_code!=c->family||spec.pinmap!=c->pinmap||spec.pin_count!=c->pins||spec.capacity_bytes!=c->capacity||spec.element_bits!=16)return LT48_SAFETY_REJECTED;closure=c;}
    const struct nxp18_profile*nxp=nxp18_find(spec.catalogue.device_class,spec.definition_resource);
    if(candidate==19){if(!nxp||!nxp18_binding(&spec))return LT48_UNSUPPORTED;const struct xp_closure*c=&nxp->base;if(spec.family_code!=c->family||spec.pinmap!=c->pinmap||spec.pin_count!=c->pins||spec.capacity_bytes!=c->capacity||spec.element_bits!=8)return LT48_SAFETY_REJECTED;closure=c;}
    const struct lpc16_profile*lp=lpc16_find(spec.catalogue.device_class,spec.definition_resource);
    if(candidate==20){if(!lp||!lpc16_binding(&spec))return LT48_UNSUPPORTED;const struct xp_closure*c=&lp->base;if(spec.family_code!=c->family||spec.pinmap!=c->pinmap||spec.pin_count!=c->pins||spec.capacity_bytes!=c->capacity||spec.element_bits!=8)return LT48_SAFETY_REJECTED;closure=c;}
    int nor2=-1;
    if(spec.catalogue.device_class==2031 && !candidate)for(unsigned i=0;i<2;i++){
        const struct xp_closure*c=&nor2_closures[i];
        if(spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==16 && spec.catalogue.manufacturer_id==(i?1u:0x62u) && spec.catalogue.device_id==(i?0x2249u:0x2595u)){closure=c;nor2=(int)i;}
    }
    int paired=0;
    if(candidate==1 && spec.catalogue.device_class==2031) {
        for(size_t i=0;i<sizeof(xp_atmel7_closures)/sizeof(*xp_atmel7_closures);i++) {
            const struct xp_closure *c=&xp_atmel7_closures[i];
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity &&
               spec.element_bits==16 && spec.catalogue.manufacturer_id==0x1f){closure=c;paired=1;break;}
        }
        if(!paired)return LT48_UNSUPPORTED;
    }
    if(candidate==16) {
        closure=NULL;
        if(spec.catalogue.device_class!=2031 || spec.catalogue.manufacturer_id!=0x1f)return LT48_UNSUPPORTED;
        for(size_t i=0;i<sizeof(atmel64_closures)/sizeof(*atmel64_closures);i++) {
            const struct xp_closure *c=&atmel64_closures[i];
            if(spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==16){closure=c;paired=1;break;}
        }
        if(!closure)return LT48_UNSUPPORTED;
    }
    if(candidate==4) {
        if(spec.catalogue.device_class!=2031)return LT48_UNSUPPORTED;
        closure=NULL;
        for(size_t i=0;i<sizeof(xp_st102_closures)/sizeof(*xp_st102_closures);i++) {
            const struct xp_closure *c=&xp_st102_closures[i];
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity &&
               spec.element_bits==16 && spec.catalogue.manufacturer_id==0x20){closure=c;break;}
        }
        if(!closure)return LT48_UNSUPPORTED;
    }
    const struct page_wide_closure *pw=NULL;
    if(page) {
        const struct page_wide_closure *pages=pagebyte?page_byte21_closures:page_wide_closures;
        size_t count=pagebyte?sizeof(page_byte21_closures)/sizeof(*page_byte21_closures):sizeof(page_wide_closures)/sizeof(*page_wide_closures);
        closure=NULL;
        for(size_t i=0;i<count;i++){const struct xp_closure *c=&pages[i].base;if(spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==(pagebyte?8u:16u)){pw=&pages[i];closure=c;break;}}
    }
    const struct xp_byte_closure *bw=NULL;
    if(byte) {
        closure=NULL;
        for(size_t i=0;i<sizeof(xp_byte_closures)/sizeof(*xp_byte_closures);i++) {
            const struct xp_closure *c=&xp_byte_closures[i].base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8){bw=&xp_byte_closures[i];closure=c;break;}
        }
    }
    if(byte&&LT48_MR7_BYTE_ADMITTED&&LT48_MR7_ADMITTED&&mr7_member(&spec.catalogue))for(size_t i=0;i<sizeof mr7_bytes/sizeof*mr7_bytes;i++){const struct xp_closure*c=&mr7_bytes[i].base;if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==8){bw=mr7_bytes+i;closure=c;}}
    if(candidate==11) {
        if(!byte)return LT48_UNSUPPORTED;
        closure=NULL;bw=NULL;
        for(size_t i=0;i<sizeof(xp_lpc29_closures)/sizeof(*xp_lpc29_closures);i++) {
            const struct xp_closure *c=&xp_lpc29_closures[i].base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins &&
               spec.capacity_bytes==c->capacity && spec.element_bits==8) {bw=&xp_lpc29_closures[i];closure=c;break;}
        }
        if(!bw)return LT48_UNSUPPORTED;
    }
    if(candidate==12) {
        if(!byte)return LT48_UNSUPPORTED;
        closure=NULL;bw=NULL;
        for(size_t i=0;i<sizeof(xp_tail37_closures)/sizeof(*xp_tail37_closures);i++) {
            const struct xp_closure *c=&xp_tail37_closures[i].base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins &&
               spec.capacity_bytes==c->capacity && spec.element_bits==8) {bw=&xp_tail37_closures[i];closure=c;break;}
        }
        if(!bw)return LT48_UNSUPPORTED;
    }
    const struct intel_byte_closure *ib=NULL;
    if(intel) {
        closure=NULL;
        for(size_t i=0;i<sizeof(intel_byte_closures)/sizeof(*intel_byte_closures);i++) {
            const struct xp_closure *c=&intel_byte_closures[i].base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==8){ib=&intel_byte_closures[i];closure=c;break;}
        }
    }
    if(candidate==10) {
        if(!intel || spec.catalogue.manufacturer_id!=0x89 || spec.catalogue.device_id!=0xaa)return LT48_UNSUPPORTED;
        const struct xp_closure *c=&xp_sharp1305_closures[0].base;
        if(spec.definition_resource!=c->definition || spec.family_code!=c->family ||
           spec.pinmap!=c->pinmap || spec.pin_count!=c->pins || spec.capacity_bytes!=c->capacity || spec.element_bits!=8)return LT48_UNSUPPORTED;
        ib=&xp_sharp1305_closures[0];closure=c;
    }
    if(candidate==9) {
        if(!intel || spec.catalogue.manufacturer_id || spec.catalogue.device_id)return LT48_UNSUPPORTED;
        const struct xp_closure *c=&xp_ice2_closures[0].base;
        if(spec.definition_resource!=c->definition || spec.family_code!=c->family ||
           spec.pinmap!=c->pinmap || spec.pin_count!=c->pins || spec.capacity_bytes!=c->capacity || spec.element_bits!=8)return LT48_UNSUPPORTED;
        ib=&xp_ice2_closures[0];closure=c;
    }
    if(candidate==8) {
        if(!intel || spec.catalogue.manufacturer_id!=1)return LT48_UNSUPPORTED;
        closure=NULL;ib=NULL;
        for(size_t i=0;i<sizeof(xp_amd28fa_closures)/sizeof(*xp_amd28fa_closures);i++) {
            const struct xp_closure *c=&xp_amd28fa_closures[i].base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins &&
               spec.capacity_bytes==c->capacity && spec.element_bits==8) {
                ib=&xp_amd28fa_closures[i];closure=c;break;
            }
        }
        if(!ib)return LT48_UNSUPPORTED;
    }
    if(candidate==5 || candidate==6 || candidate==7) {
        if(!intel)return LT48_UNSUPPORTED;
        closure=NULL;ib=NULL;
        const struct intel_byte_closure *mux=candidate==7?xp_mux15_closures:candidate==6?xp_mux30_closures:xp_atmel_ll5_closures;
        size_t mux_count=candidate==7?sizeof(xp_mux15_closures)/sizeof(*xp_mux15_closures):candidate==6?sizeof(xp_mux30_closures)/sizeof(*xp_mux30_closures):sizeof(xp_atmel_ll5_closures)/sizeof(*xp_atmel_ll5_closures);
        for(size_t i=0;i<mux_count;i++) {
            const struct xp_closure *c=&mux[i].base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity &&
               spec.element_bits==8 && (spec.catalogue.manufacturer_id==0x1f || spec.catalogue.manufacturer_id==0x89 || spec.catalogue.manufacturer_id==0x20 || (candidate==7 && (spec.catalogue.manufacturer_id==0x37 || spec.catalogue.manufacturer_id==0xb0)))) {
                ib=&mux[i];closure=c;break;
            }
        }
        if(!ib)return LT48_UNSUPPORTED;
    }
    int sanyo256=0;
    if(candidate==2) {
        if(!intel)return LT48_UNSUPPORTED;
        closure=NULL;ib=NULL;
        for(size_t i=0;i<sizeof(xp_sanyo7_closures)/sizeof(*xp_sanyo7_closures);i++) {
            const struct xp_closure *c=&xp_sanyo7_closures[i].base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity &&
               spec.element_bits==8 && spec.catalogue.manufacturer_id==((c->definition==0x63 || c->definition==0x67 || c->definition==0x68)?0xbf:0x62)) {
                ib=&xp_sanyo7_closures[i];closure=c;sanyo256=1;break;
            }
        }
        if(!sanyo256)return LT48_UNSUPPORTED;
    }
    const struct intel_word_closure *iw=NULL;
    if(c3)for(size_t i=0;i<sizeof(intel_word_closures)/sizeof(*intel_word_closures);i++) {
        const struct xp_closure *c=&intel_word_closures[i].settings.base;
        if(spec.definition_resource==c->definition && spec.family_code==c->family && spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==16){iw=&intel_word_closures[i];closure=c;break;}
    }
    if(c3&&LT48_MR7_ADMITTED&&mr7_member(&spec.catalogue))for(size_t i=0;i<sizeof mr7_words/sizeof*mr7_words;i++){const struct xp_closure*c=&mr7_words[i].settings.base;if(spec.definition_resource==c->definition&&spec.family_code==c->family&&spec.pinmap==c->pinmap&&spec.pin_count==c->pins&&spec.capacity_bytes==c->capacity&&spec.element_bits==16){iw=mr7_words+i;closure=c;}}
    if(candidate==13) {
        if(!c3)return LT48_UNSUPPORTED;
        closure=NULL;iw=NULL;
        for(size_t i=0;i<sizeof(xp_intel29_closures)/sizeof(*xp_intel29_closures);i++) {
            const struct xp_closure *c=&xp_intel29_closures[i].settings.base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==16){iw=&xp_intel29_closures[i];closure=c;break;}
        }
        for(size_t i=0;!iw && i<sizeof(st_m36_closures)/sizeof(*st_m36_closures);i++) {
            const struct xp_closure *c=&st_m36_closures[i].settings.base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity &&
               spec.element_bits==16 && spec.catalogue.manufacturer_id==0x20 &&
               spec.catalogue.device_id==(c->definition==0x37?0x8810u:0x8811u)) {iw=&st_m36_closures[i];closure=c;}
        }
        if(!iw)return LT48_UNSUPPORTED;
    }
    if(candidate==14) {
        if(!c3)return LT48_UNSUPPORTED;
        closure=NULL;iw=NULL;
        for(size_t i=0;i<sizeof(xp_seiko12_closures)/sizeof(*xp_seiko12_closures);i++) {
            const struct xp_closure *c=&xp_seiko12_closures[i].settings.base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==16){iw=&xp_seiko12_closures[i];closure=c;break;}
        }
        if(!iw)return LT48_UNSUPPORTED;
    }
    if(candidate==15) {
        if(!c3)return LT48_UNSUPPORTED;
        closure=NULL;iw=NULL;
        for(size_t i=0;i<sizeof(xp_word_tail22_closures)/sizeof(*xp_word_tail22_closures);i++) {
            const struct xp_closure *c=&xp_word_tail22_closures[i].settings.base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==16){iw=&xp_word_tail22_closures[i];closure=c;break;}
        }
        if(!iw)return LT48_UNSUPPORTED;
    }
    if(candidate==3) {
        if(!c3)return LT48_UNSUPPORTED;
        closure=NULL;iw=NULL;
        for(size_t i=0;i<sizeof(xp_atmel_bga6_closures)/sizeof(*xp_atmel_bga6_closures);i++) {
            const struct xp_closure *c=&xp_atmel_bga6_closures[i].settings.base;
            if(spec.definition_resource==c->definition && spec.family_code==c->family &&
               spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity &&
               spec.element_bits==16 && spec.catalogue.manufacturer_id==0x1f) {
                iw=&xp_atmel_bga6_closures[i];closure=c;break;
            }
        }
        if(!iw)return LT48_UNSUPPORTED;
    }
    if(!closure || (page&&!pw) || (byte&&!bw))return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/%s",lt48_installation_path(x),(candidate==19||candidate==20)?"GDAE___.rez":intel?"GDAB___.rez":byte?"GDAA___.rez":pagebyte?"GDBA___.rez":page?"GDBAA__.rez":(at17f||c3)?"GDABA__.rez":"GDAAA__.rez");
    if(n<0||(size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
#define TRY(a) do{status=(a);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,(candidate==19||candidate==20)?"9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36":intel?"55d48e0a8888665c93b12551e9ddb3307abe819d088f8e164ac1c623b7ad0d44":byte?"6f780f677ae8e3f57c49d951f45509baead7bbfd4b678488e73383fc7d841471":pagebyte?"4129e4261b9eeb7a4b7f4272d99b3e8d4ec45e41f9150a86baa57f909e8fc7ff":page?"b889a62b78b8d63028e47b3c4c008b86f245fa96322c4f73336a5f882f19fe28":(at17f||c3)?"77e939e536a8af7d76fa8d661927e7f1e712eb7b1441fe6e7c86ddbeefdb51ec":"98e1cfe07d11ae94d93a71b49e90197370df36694b5ac880523b6012efcbcbde")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->shared=lv800?&lv800three_shape:candidate==21?&tmp1962one_shape:candidate==20?&lp->shape:candidate==19?&nxp->shape:candidate==18?&sh13->shape:candidate==17?&p20->shape:spec.catalogue.device_class==2031?xp_shared_word_find(spec.definition_resource):NULL;
    r->tmp1962one=candidate==21;r->parallel20=candidate==17;r->sharp13=candidate==18;r->nxp18=(candidate==19||candidate==20);r->lpc16=candidate==20;r->atmel64=candidate==16;r->paired_program=(unsigned)(paired||candidate==21);r->sanyo256=(unsigned)sanyo256;
    r->ice2=candidate==9;
    r->mux_bulk_status=candidate==7 && (spec.definition_resource==0x21 || spec.definition_resource==0x15 || spec.definition_resource==0x13 || spec.definition_resource==0x16 || spec.definition_resource==0x18);
    if(paired)r->shared=&xp_atmel7_shape;
    if(candidate==4)r->shared=&xp_st102_shape;
    r->primary=candidate==21?&tmp1962one_primary:candidate==18?&sh13->primary:candidate==17?&p20->primary:spec.catalogue.device_class==2031?xp_primary_find(spec.definition_resource):NULL;
    if(candidate==12)for(size_t i=0;i<sizeof(xp_tail37_erase_maps)/sizeof(*xp_tail37_erase_maps);i++)
        if(spec.definition_resource==xp_tail37_erase_maps[i].definition)r->primary=&xp_tail37_erase_maps[i];
    r->as15=spec.catalogue.device_class==2031 && xp_atmel_sanyo15_batch(spec.definition_resource);
    if(nor2>=0){r->shared=&nor2_shapes[nor2];r->primary=&nor2_primary[nor2];r->as15=nor2==0;}
    r->legacy28f=intel && spec.definition_resource<=3;
    r->word=iw;r->intel=ib;r->byte=bw;r->element_bytes=(candidate==19||candidate==20||byte||intel||pagebyte)?1:2;r->page=pw;r->at17f=at17f;r->c3=c3;r->applet_count=0;r->nominal=closure->vcc;r->closure=closure;
    r->capacity=spec.capacity_bytes;r->man=spec.catalogue.manufacturer_id;r->dev=spec.catalogue.device_id;
    if(candidate==13) {
        int bound=spec.definition_resource>=0x37 && spec.definition_resource<=0x39 && spec.catalogue.unknown6==0;
        for(size_t i=0;i<sizeof(xp_intel29_id_bindings)/sizeof(*xp_intel29_id_bindings);i++)
            if(xp_intel29_id_bindings[i][0]==spec.definition_resource && xp_intel29_id_bindings[i][1]==spec.catalogue.unknown6)bound=1;
        if(!bound){status=LT48_SAFETY_REJECTED;goto fail;}
        for(size_t i=0;i<sizeof(xp_intel29_ids)/sizeof(*xp_intel29_ids);i++)
            if(xp_intel29_ids[i].key==spec.catalogue.unknown6)r->id_table=&xp_intel29_ids[i];
        if(spec.catalogue.unknown6 && !r->id_table){status=LT48_SAFETY_REJECTED;goto fail;}
        /* M17 GETID returns one addressed word in DataIn, not the M16
         * manufacturer/device pair. Reuse the qualified addressed-ID owner. */
        if(spec.definition_resource==0x39)r->id_table=&st_m36_d_identity;
    }
    snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(family.variable_count>8 || (!page && (family.variable_count!=(iw?iw->variables:ib?ib->variables:bw?bw->variables:candidate==21?6u:candidate==20?lp->variables:candidate==19?nxp->variables:candidate==18?sh13->variables:c3?6u:at17f?2u:candidate==17?p20->variables:paired?3u:nor2>=0?(nor2==0?2u:7u):lv800?2u:xp_variable_count(closure->definition)) || family.operation_count!=(r->shared?r->shared->operations:iw?iw->operations:ib?ib->operations:bw?bw->operations:c3?15u:at17f?11u:18u)))){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    }
    if(r->atmel64 && !atmel64_correct_vpp(spec.definition_resource,r->vars,r->var_count)){status=LT48_FORMAT_ERROR;goto fail;}
    if(r->vars[0].variable_id!=0 || r->vars[0].value!=r->nominal){status=LT48_FORMAT_ERROR;goto fail;}
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(code.wave_type==0 && (op.operation_id<15 || ((sanyo256 || (candidate==15 && spec.definition_resource>=0xa6 && spec.definition_resource<=0xa9)) && op.operation_id==15))) {
            lt48_compact_wave *w=NULL;unsigned char *corrected=NULL;size_t code_size=code.data_size;
            if(page && spec.definition_resource==0x3d && op.operation_id==6){
                corrected=malloc(code.data_size);if(!corrected){status=LT48_NO_MEMORY;goto fail;}
                memcpy(corrected,code.data,code.data_size);
                if(op.wave!=0x6d || !page_llyd_mask_correction(corrected,code.data_size)){free(corrected);status=LT48_FORMAT_ERROR;goto fail;}
            }
            if(iw && iw->settings.secure_kind==4 && op.operation_id==11){
                if(op.wave!=0x11){status=LT48_FORMAT_ERROR;goto fail;}
                corrected=intel_word_st_lock_write(code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_FORMAT_ERROR;goto fail;}
            }
            if(bw==mr7_bytes && op.operation_id==2){corrected=lv088_init(&code,&code_size);if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}}
            status=lt48_compact_decode(corrected?corrected:code.data,code_size,&w);free(corrected);if(status!=LT48_OK)goto fail;
            int sharp_release=page && page_sharp_release_definition(spec.definition_resource) && op.operation_id==3;
            if(sharp_release && (op.wave!=0x97 || r->vars[2].variable_id!=1 || r->vars[2].channel!=3)){lt48_compact_wave_close(w);status=LT48_FORMAT_ERROR;goto fail;}
            int efst_dead=byte && xp_efst_dead_definition(spec.definition_resource) && op.operation_id==2;
            if(efst_dead && (op.wave!=0x19 || code.data_size!=196 || r->var_count!=4 || r->vars[1].variable_id!=2 || r->vars[1].channel!=3)){lt48_compact_wave_close(w);status=LT48_FORMAT_ERROR;goto fail;}
            int amic_vid=r->shared && xp_amic_unused_vid_definition(spec.definition_resource) && (op.operation_id==2 || op.operation_id==3);
            if(amic_vid && (spec.pinmap!=0x1e || spec.pin_count!=44 || op.wave!=(op.operation_id==2?0x27u:0x2cu) || code.data_size!=(op.operation_id==2?250u:66u) || r->var_count!=7 || r->vars[1].variable_id!=2 || r->vars[1].channel!=3 || r->vars[1].value!=1000)){lt48_compact_wave_close(w);status=LT48_SAFETY_REJECTED;goto fail;}
            int byte_vid=byte && xp_byte_atmel_vid_context(spec.definition_resource,op.operation_id);
            int mr7_default=iw && mr7_member(&spec.catalogue) && mr7_default_context(spec.definition_resource,op.operation_id);
            int iw_default=iw && intel_shared_default_context(spec.definition_resource,op.operation_id);
            status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,
                (mr7_default||sharp_release||efst_dead||amic_vid||iw_default||byte_vid)?LT48_PIN_FALLBACK_ANALYSIS_ALL:LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);
            lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            if(mr7_default && !mr7_defaults(r->wave[op.operation_id],op.operation_id)){status=LT48_SAFETY_REJECTED;goto fail;}
            if(byte_vid && !xp_byte_atmel_vid_defaults(r->wave[op.operation_id],op.operation_id)){status=LT48_SAFETY_REJECTED;goto fail;}
            if(iw_default && !intel_shared_defaults(r->wave[op.operation_id],spec.definition_resource,op.operation_id)){status=LT48_SAFETY_REJECTED;goto fail;}
            if(amic_vid && !xp_amic_unused_vid_defaults(r->wave[op.operation_id],op.operation_id)){status=LT48_SAFETY_REJECTED;goto fail;}
            if(efst_dead && !xp_efst_dead_defaults(r->wave[2])){status=LT48_SAFETY_REJECTED;goto fail;}
            if(sharp_release && !page_sharp_release_defaults(r->wave[3])){status=LT48_SAFETY_REJECTED;goto fail;}
        } else if(code.wave_type==6) {
            unsigned slot=op.operation_id==17?0:op.operation_id==19?1:(op.operation_id==20 || (r->legacy28f && op.operation_id==22))?2:op.operation_id==21?3:op.operation_id==16?4:op.operation_id==18?5:op.operation_id==23?6:op.operation_id==24?7:((candidate==19||candidate==20)&&op.operation_id==26)?8:9;
            static const uint16_t addr[]={0x6000,0x7800,0x8000,0x7000,0x5000,0x5800,0x9000,0x9800,0xa000};
            if(r->applet_count>=10 || slot==9 || code.load_address!=addr[slot] || code.data_size==0 ||
               code.data_size>((slot==8&&candidate==20)?0x1000u:0x800u) || code.length1!=code.data_size+4 || code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            if(bw==mr7_bytes && (op.operation_id==16 || op.operation_id==20 || op.operation_id==23)){size_t n=0;unsigned char*b=lv088_applet(op.operation_id,&code,&n);if(!b){status=LT48_SAFETY_REJECTED;goto fail;}r->owned_applet[r->applet_count]=b;code.data=b;code.data_size=n;code.length1=n+4;code.length2=n;}
            r->applet[r->applet_count]=code;r->addresses[r->applet_count++]=(lt48_applet_address){op.operation_id,code.load_address};
        } else if(code.wave_type==4 && op.operation_id==27 && code.data_size==2780 &&
                  code.data[0]==0xda && code.data[1]==0x0a)r->fpga=code.data+2;
        else {status=LT48_UNSUPPORTED;goto fail;}
    }
    if(!page && r->applet_count!=(r->shared?r->shared->applets:ib?(candidate==9?4u:candidate==8?6u:r->legacy28f?7u:5u):bw?bw->applets:(at17f||c3)?5u:7u)){status=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<r->applet_count;i++)if(!r->applet[i].data){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->wave[0]||(!r->wave[1]&&!r->atmel64&&!(candidate==17&&!r->wave[1]))||!r->wave[2]||!r->wave[3]||(!at17f&&!r->ice2&&!r->tmp1962one&&!(r->primary && spec.definition_resource>=0xdf && spec.definition_resource<=0xe8)&&!r->wave[4])||!r->wave[10]){status=LT48_FORMAT_ERROR;goto fail;}
    if(byte && xp_efst_dead_definition(spec.definition_resource) && (r->wave[6]||r->wave[11]||r->wave[12]||r->wave[13])){status=LT48_SAFETY_REJECTED;goto fail;}
    if(r->ice2 && !r->wave[14]){status=LT48_FORMAT_ERROR;goto fail;}
    if(sanyo256 && !r->wave[15]){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->fpga){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_xp_flash_close(r);return status;
#undef TRY
}
#include "../../NewDeviceLib/sources/sst-page/open.inc"
#include "../../NewDeviceLib/sources/at29-13/open.inc"
#include "../../NewDeviceLib/sources/xp13/open.inc"
#include "../../NewDeviceLib/sources/tpi15/open.inc"
#include "../../NewDeviceLib/sources/nand15/open.inc"
lt48_status lt48_nand15_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){return nand15_open(x,i,out);}
int lt48_nand15_member_internal(unsigned cls,unsigned d){return cls==2041&&d>=0x1b&&d<=0x1f;}
int lt48_nand15_admitted_internal(unsigned d){return nand15_admitted(d);}
#include "../../NewDeviceLib/sources/emtc18/open.inc"
lt48_status lt48_emtc18_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){return emtc18_open(x,i,out);}
int lt48_emtc18_member_internal(unsigned cls,unsigned d){return cls==2041&&d>=0x21&&d<=0x25;}
int lt48_emtc18_admitted_internal(unsigned d){return emtc18_admitted(d);}
lt48_status lt48_xp_flash_open(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    if(x){lt48_catalogue_entry e;if(!lt48_installation_get_device(x,device,&e)&&tpi15_binding(&e)){if(!LT48_TPI15_ADMITTED)return LT48_UNSUPPORTED;return tpi15_open(x,device,out);}}
    if(x){lt48_catalogue_entry e;if(lt48_installation_get_device(x,device,&e)==LT48_OK&&xp13_binding(&e)){
      if(!LT48_XP13_ADMITTED)return LT48_UNSUPPORTED;
      return e.device_class==2035?xp13_otp_open(x,device,out):xp_flash_open_impl(x,device,out,18);
    }}
    if(x && nand15_selected(x,device))return nand15_open(x,device,out);
    if(x && emtc18_selected(x,device))return emtc18_open(x,device,out);
    if(x && at29_selected(x,device))return at29_open(x,device,out);
    if(x && sst_page_selected(x,device))return sst_page_open(x,device,out);
    lt48_device_spec spec;int candidate=0;
    if(x && lt48_installation_resolve_device(x,device,&spec)==LT48_OK) {
        if(tmp1962one_member(&spec.catalogue)){if(!tmp1962one_binding(&spec))return LT48_SAFETY_REJECTED;candidate=21;}
        if(lpc16_find(spec.catalogue.device_class,spec.definition_resource)){if(!lpc16_admitted(spec.definition_resource))return LT48_UNSUPPORTED;candidate=20;}
        if(nxp18_find(spec.catalogue.device_class,spec.definition_resource)){if(!LT48_NXP18_ADMITTED)return LT48_UNSUPPORTED;candidate=19;}
        if(sh13_find(spec.catalogue.device_class,spec.definition_resource)){if(!sh13_admitted(spec.definition_resource))return LT48_UNSUPPORTED;candidate=18;}
        if(p20_find(spec.catalogue.device_class,spec.definition_resource)){if(!p20_admitted(spec.definition_resource))return LT48_UNSUPPORTED;candidate=17;}
        /* Dbd uses its explicit-address owner, not the tail37 closure. */
        if(spec.catalogue.device_class==2030 && spec.definition_resource!=0xbd)
            for(size_t i=0;i<sizeof(xp_tail37_closures)/sizeof(*xp_tail37_closures);i++)
                if(spec.definition_resource==xp_tail37_closures[i].base.definition)candidate=12;
        if(spec.catalogue.device_class==2030)for(size_t i=0;i<sizeof(xp_lpc29_closures)/sizeof(*xp_lpc29_closures);i++)
            if(spec.definition_resource==xp_lpc29_closures[i].base.definition)candidate=11;
        if(spec.catalogue.device_class==2033 && spec.definition_resource>=0xa6 && spec.definition_resource<=0xa9)candidate=15;
        if(spec.catalogue.device_class==2033 && spec.definition_resource>=0x24 && spec.definition_resource<=0x27)candidate=14;
        if(spec.catalogue.device_class==2033 && (spec.definition_resource==0x10 || spec.definition_resource==0x11 || spec.definition_resource==0x3a || spec.definition_resource==0x3b || (spec.definition_resource>=0x37 && spec.definition_resource<=0x39)))candidate=13;
        if(spec.catalogue.device_class==2032 && spec.definition_resource==0x5f)candidate=10;
        if(spec.catalogue.device_class==2032 && spec.definition_resource==8)candidate=9;
        if(spec.catalogue.device_class==2032 && spec.definition_resource>=4 && spec.definition_resource<=7)candidate=8;
        if(spec.catalogue.device_class==2031 && spec.definition_resource>=0x107 && spec.definition_resource<=0x10a)candidate=16;
        if(spec.catalogue.device_class==2031 && (spec.definition_resource==0xfd || spec.definition_resource==0xff || spec.definition_resource==0x100))candidate=1;
        if(spec.catalogue.device_class==2032 && (spec.definition_resource==0x11 || spec.definition_resource==0x13 || spec.definition_resource==0x15 || spec.definition_resource==0x16 || spec.definition_resource==0x18 || spec.definition_resource==0x1f || spec.definition_resource==0x20 || spec.definition_resource==0x21 || spec.definition_resource==0x22))candidate=7;
        if(spec.catalogue.device_class==2032 && spec.definition_resource>=0x62 && spec.definition_resource<=0x68)candidate=2;
        if(spec.catalogue.device_class==2033 && spec.definition_resource>=0xbc && spec.definition_resource<=0xc1)candidate=3;
        if(spec.catalogue.device_class==2031 && (spec.definition_resource==0x136 || (spec.definition_resource>=0x10c && spec.definition_resource<=0x10e)))candidate=4;
        if(spec.catalogue.device_class==2032 && (spec.definition_resource==0x9 || spec.definition_resource==0xa || spec.definition_resource==0xb || spec.definition_resource==0xc || spec.definition_resource==0xd || spec.definition_resource==0xf || spec.definition_resource==0x10 || spec.definition_resource==0x12 || spec.definition_resource==0x14 || spec.definition_resource==0x17 || spec.definition_resource==0x1c || spec.definition_resource==0x1d))candidate=6;
        if(spec.catalogue.device_class==2032 && (spec.definition_resource==0xe || spec.definition_resource==0x19 || spec.definition_resource==0x1a || spec.definition_resource==0x1b || spec.definition_resource==0x1e))candidate=5;
    }
    return xp_flash_open_impl(x,device,out,candidate);
}
/* Offline qualification entry only; not declared in the public header. */
lt48_status lt48_xp_atmel7_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,1);
}
lt48_status lt48_xp_sanyo7_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,2);
}
lt48_status lt48_xp_atmel_bga6_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,3);
}
lt48_status lt48_xp_st102_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,4);
}
lt48_status lt48_xp_atmel_ll5_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    lt48_status status=xp_flash_open_impl(x,device,out,5);
    if(status==LT48_UNSUPPORTED)status=xp_flash_open_impl(x,device,out,6);
    return status==LT48_UNSUPPORTED?xp_flash_open_impl(x,device,out,7):status;
}
lt48_status lt48_xp_amd28fa_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,8);
}
lt48_status lt48_xp_byte_tail37_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,12);
}
lt48_status lt48_xp_word_tail22_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {return xp_flash_open_impl(x,device,out,15);}
lt48_status lt48_xp_seiko12_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {return xp_flash_open_impl(x,device,out,14);}
lt48_status lt48_xp_intel29_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,13);
}
lt48_status lt48_xp_lpc29_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,11);
}
lt48_status lt48_xp_sharp1305_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,10);
}
lt48_status lt48_xp_ice2_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out) {
    return xp_flash_open_impl(x,device,out,9);
}
lt48_status lt48_xp_flash_validate(const lt48_xp_flash_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r||!length||start>=r->capacity||length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if((r->nxp18||r->nand15||r->tpi15)&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if((r->otp11&&op>LT48_OPERATION_COMPARE)||(r->tpi15&&op>LT48_OPERATION_ERASE))return LT48_UNSUPPORTED;
    if(op<LT48_OPERATION_READ||op>((!lt48_xp_flash_secure_kind(r))?LT48_OPERATION_ERASE:LT48_OPERATION_SECURE))return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_SECURE && lt48_xp_flash_protection_choices(r) && !r->protection_choice)return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE&&(!image||size!=length))return LT48_INVALID_ARGUMENT;
    if(r->nxp18&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)&&!(r->lpc16?lpc16_image_allowed(image,size):nxp18_image_allowed(image,size)))return LT48_SAFETY_REJECTED;
    /* Public ranges are bytes; source Address and counters use device elements. */
    if(start%r->element_bytes || length%r->element_bytes)return LT48_SAFETY_REJECTED;
    if(r->at29 && ((start%r->page_bytes)||(length%r->page_bytes)))return LT48_SAFETY_REJECTED;
    if((r->sst_page||r->emtc18) && ((start%128u)||(length%128u)))return LT48_SAFETY_REJECTED;
    if(r->page && ((start%(r->element_bytes*r->page->page)) || (length%(r->element_bytes*r->page->page))))return LT48_SAFETY_REJECTED;
    if((op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE) && (start!=0||length!=r->capacity))return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
lt48_status lt48_xp_flash_validate_verify_options(const lt48_xp_flash_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned margin_type) {
    if(!r||margin_type>1)return LT48_INVALID_ARGUMENT;
    if(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&
       margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    if(r->tmp1962one&&margin_type&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||(op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_NOMINAL)))return LT48_SAFETY_REJECTED;
    /* Only the 5% high margin is admitted for the 3.3 V flash definition.
     * The original 10% calculation asks for 3.63 V, outside our 3.6 V ceiling. */
    if(r->byte!=mr7_bytes && r->nominal<=360 && (unsigned)r->nominal*(margin_type?110u:105u)>36000u &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||((r->sst_page||r->at29)&&op==LT48_OPERATION_SECURE)||
        (op==LT48_OPERATION_VERIFY&&(margin==LT48_VERIFY_MARGIN_ALL||margin==LT48_VERIFY_MARGIN_HIGH))))
        return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
#include "../../NewDeviceLib/sources/pic-common/source_verify_voltage.h"
static uint16_t margin_voltage(const lt48_xp_flash_resources *r,unsigned type,int high) {
    if(r->tmp1962one){uint32_t volts[4];unsigned count;
        if(lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,volts,&count)!=LT48_OK || count!=2)return 0;
        return (uint16_t)volts[high?1:0];}
    /* Nearest-even integer centivolts, including the audited 330 -> 314/346. */
    unsigned product=r->nominal*(high?(type?110u:105u):(type?90u:95u));
    unsigned q=product/100,rem=product%100;
    unsigned result=q+(rem>50 || (rem==50 && (q&1)));
    if(r->byte==mr7_bytes){if(result<270)result=270;if(result>360)result=360;}
    return (uint16_t)result;
}
typedef struct xp_session {
    const lt48_xp_flash_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[32][32];uint32_t volts[4];
    lt48_compile_variable vars[8];uint32_t mode;
    lt48_runtime_configuration_value config[600];size_t config_count;
    unsigned char *config_image;size_t config_image_size;
} xp_session;
static lt48_status run(xp_session *s,unsigned op) {
    /* These exact source profiles omit POWEROFF. The owner still resets,
     * clears every DAC channel and restores idle FPGA on every exit. */
    if(op==1 && !s->r->wave[1] && (s->r->atmel64||s->r->parallel20))return LT48_OK;
    if(op==2 && s->r->byte && xp_efst_dead_definition(s->r->closure->definition) && !xp_efst_mode_allowed(s->mode))return LT48_SAFETY_REJECTED;
    lt48_runtime_options options={0};options.host_staged_shutdown=(s->r->byte==mr7_bytes||s->r->word==mr7_words||s->r->word==mr7_words+1)?4:(s->r->nand15||s->r->otp11||s->r->tpi15||s->r->tmp1962one)?4:(s->r->sharp13||s->r->nxp18)?3:s->r->parallel20?(s->r->closure->definition==0xe9?4:3):0;lt48_runtime_buffer_seed seeds[31];size_t count=0;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    for(uint8_t i=0;i<32;i++)if(i!=7)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_nested_wave nested[3];
    if(s->r->wave[11])nested[options.nested_wave_count++]=(lt48_runtime_nested_wave){11,s->r->wave[11]};
    if(s->r->ice2)nested[options.nested_wave_count++]=(lt48_runtime_nested_wave){14,s->r->wave[14]};
    if(s->r->wave[15])nested[options.nested_wave_count++]=(lt48_runtime_nested_wave){15,s->r->wave[15]};
    if(options.nested_wave_count)options.nested_waves=nested;
    if((s->r->page || s->r->word || s->r->as15 || s->r->at29 || s->r->tpi15) && (op==6 || op==11)) {
        /* Each original configuration worker binds Size in device elements.
         * Some callbacks convert it to bytes in place. */
        for(unsigned j=0;j<4;j++)s->buffers[1][j]=(unsigned char)((s->r->capacity/s->r->element_bytes)>>(8*j));
        options.configuration=s->config;options.configuration_count=s->config_count;
        if(s->config_image){options.device_image=s->config_image;options.device_image_size=s->config_image_size;options.device_element_bytes=s->r->element_bytes;}
    }
    if(s->r->byte && (s->r->byte->secure==2 || s->r->byte->secure==3) && (op==11 || op==12 || op==13)){
        s->config[0]=(lt48_runtime_configuration_value){-1,0,0,op==13?0:1};
        options.configuration=s->config;options.configuration_count=1;
    }
    if(!s->r->at29 && lt48_xp_flash_protection_choices(s->r) && (op==6 || op==11)) {
        s->config[0]=(lt48_runtime_configuration_value){-1,1,0,op==11?s->r->protection_choice:0};
        options.configuration=s->config;options.configuration_count=1;
    }
    if(s->r->sst_page||s->r->at29){options.configuration=s->config;options.configuration_count=s->config_count;}
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=7;
    options.address_buffer=0;options.parameter_buffer_id=2;options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));options.root_timeout_seconds=30;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,30000,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->applet_count,&result);
    if(status==LT48_OK) {
        if((s->r->page || s->r->word || s->r->as15 || s->r->at29 || s->r->tpi15) && (op==6 || op==11)) {
            const lt48_runtime_configuration_value *values=NULL;size_t n=0;
            status=lt48_runtime_get_configuration(result,&values,&n);
            if(status==LT48_OK && n<=600){memcpy(s->config,values,n*sizeof(*values));s->config_count=n;}else status=LT48_FORMAT_ERROR;
            if(status==LT48_OK && s->config_image){const unsigned char *bytes=NULL;size_t length=0;status=lt48_runtime_get_image(result,&bytes,&length);if(status==LT48_OK && length==s->config_image_size)memcpy(s->config_image,bytes,length);else status=LT48_FORMAT_ERROR;}
        }
        for(uint8_t i=0;status==LT48_OK && i<32;i++) {
            const unsigned char *data;size_t size;status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK||size!=32){status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        for(uint8_t i=0;status==LT48_OK&&i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(event.kind==LT48_RUNTIME_RESULT_LATCH ||
               (event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0))status=LT48_RUNTIME_ERROR;
        }
    }
    if(getenv("LABTOOL48_XP_TRACE"))fprintf(stderr,
        "xp wave=%u mode=%u status=%d address=%02x%02x%02x%02x data=%02x retry=%u volts=%u,%u,%u,%u\n",
        op,s->mode,status,s->buffers[0][3],s->buffers[0][2],s->buffers[0][1],s->buffers[0][0],
        s->buffers[2][0],s->buffers[9][0],s->volts[0],s->volts[1],s->volts[2],s->volts[3]);
    /* The LPC erase worker enters UART ISP; the RAM parallel reader must
     * be initialized again before the independent erased-array readback. */
    if(s->r->nxp18&&op==10)memset(s->buffers[10],0,32);
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_status transfer(xp_session *s,int input,uint8_t endpoint,unsigned char *bytes,size_t length) {
    size_t done=0;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    lt48_status status=input?s->api->read(s->context,endpoint,bytes,length,&done):s->api->write(s->context,endpoint,bytes,length,&done);
    if(status==LT48_OK&&done!=length)status=LT48_PROTOCOL_ERROR;
    return status;
}
static lt48_status cmd(xp_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    unsigned char packet[512]={0};if(n>512)return LT48_INVALID_ARGUMENT;
    memcpy(packet,p,n);lt48_status status=lt48_runtime_transport_session_flush(s->wire);
    if(status==LT48_OK)status=transfer(s,0,2,packet,512);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
/* Exact GDAB D0..D3 W9 loop. The generic resumable interpreter replays
 * prior captures; this full-array byte loop must run in linear time.
 * Original Retry is global across the scan, not renewed for each byte. */
static lt48_status legacy28f_erase(xp_session *s) {
    unsigned retry=0;
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==8)retry=s->vars[i].value;
    if(!s->r->legacy28f || retry!=1000)return LT48_SAFETY_REJECTED;
    unsigned char reply[512];
    static const unsigned char address[]={0x2c,2,0xc4,2,0,2,0xc8,2,0,2,0xcc,2,0,0,0};
    lt48_status st=cmd(s,address,sizeof(address),reply);
    if(st==LT48_OK && !(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;
    static const unsigned char start[]={0x3b,0xaa,0x55,0,0x90};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,start,sizeof(start));
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK)st=lt48_runtime_transport_session_wave_watchdog(s->wire,110000,1);
    static const unsigned char first[]={0x2c,1,5,2,0x20,2,0xa0,1,6,2,0xff,3,0,0,0};
    if(st==LT48_OK)st=cmd(s,first,sizeof(first),reply);
    uint32_t count=0;struct timespec begin,now;
    if(timespec_get(&begin,TIME_UTC)!=TIME_UTC)st=LT48_RUNTIME_ERROR;
    while(st==LT48_OK) {
        if(timespec_get(&now,TIME_UTC)!=TIME_UTC){st=LT48_RUNTIME_ERROR;break;}
        double elapsed=(double)(now.tv_sec-begin.tv_sec)+(double)(now.tv_nsec-begin.tv_nsec)/1000000000.0;
        if(elapsed<0 || elapsed>110.0){st=LT48_TIMEOUT;break;}
        if(!(reply[0]|reply[1])){st=LT48_PROTOCOL_ERROR;break;}
        unsigned step;
        if(reply[2]==255){count++;step=7;}
        else {if(!--retry){st=LT48_DEVICE_MISMATCH;break;}step=6;}
        unsigned char next[]={0x2c,1,(unsigned char)step,2,0xff,3,0,0,0};
        if(count==s->r->capacity) {
            /* W9 advances after the final FF, then stops the applet. */
            const unsigned char done[]={0x2c,1,7,1,0,0,0};
            st=cmd(s,done,sizeof(done),reply);
            if(st==LT48_OK && !(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;
            break;
        }
        st=cmd(s,next,sizeof(next),reply);
    }
    for(unsigned j=0;j<4;j++){s->buffers[9][j]=(unsigned char)(count>>(8*j));s->buffers[11][j]=(unsigned char)(retry>>(8*j));}
    s->buffers[2][0]=(st==LT48_OK);
    return st;
}
static lt48_status flag(xp_session *s,unsigned opcode,unsigned *value) {
    unsigned char p[2]={(unsigned char)opcode,1},reply[512]={0};
    lt48_status status=cmd(s,p,opcode==0x0b?1:2,reply);
    if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)*value=reply[2]&1;
    return status;
}
static lt48_status block(xp_session *s,unsigned opcode,unsigned char *data,size_t n,unsigned char *reply) {
    unsigned char p[6]={(unsigned char)opcode,(unsigned char)(n/512),(unsigned char)(n/512>>8),
        (unsigned char)(n%512),(unsigned char)(n%512>>8),opcode==0x48?(unsigned char)s->r->element_bytes:0};
    lt48_status status=cmd(s,p,6,NULL);
    if(status==LT48_OK)status=transfer(s,opcode==0x43,opcode==0x43?0x88:4,data,n);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status setup(xp_session *s) {
    /* Native bridge initialization already establishes the transport. FX2
     * opcode 0x18 is reserved (RET at CODE:0a1f), not a reset with a reply. */
    lt48_status status=LT48_OK;
    /* Original 43d580 visits operation table slots, not source wave numbers. */
    for(unsigned i=0;status==LT48_OK&&i<s->r->applet_count;i++) {
        const lt48_wave_code *a=&s->r->applet[i];size_t last=a->data_size-1;
        unsigned char header[]={0x39,0xaa,0x55,(unsigned char)a->load_address,(unsigned char)(a->load_address>>8),(unsigned char)last,(unsigned char)(last>>8)};
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,header,sizeof(header));
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,a->data,a->data_size);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
    }
    const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,s->r->nominal>400?15:7},busy[]={0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,fpga,sizeof(fpga));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,s->r->fpga,2778);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,selector,sizeof(selector));
    if(status==LT48_OK)status=run(s,0);
    if(status==LT48_OK && s->r->lpc16 && s->r->closure->definition<=0x147 && s->buffers[12][0]!=1)status=LT48_DEVICE_MISMATCH;
    if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->wire,1);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof(busy));
    return status;
}
static lt48_status cycle(xp_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_xp_flash_result *out) {
    for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)((start/s->r->element_bytes)>>(8*j));
    s->mode=mode;for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status status=run(s,2);size_t width=s->r->element_bytes;size_t chunk=length/100;chunk-=chunk%width;if(!chunk)chunk=width;if(chunk>8192)chunk=8192;
    if(s->r->paired_program && mode==2){chunk-=chunk%4;if(!chunk)chunk=4;}
    if(s->r->page && mode==2)chunk=width*s->r->page->page;
    out->completed_bytes=0;
    for(size_t offset=0;status==LT48_OK&&offset<length;) {
        size_t n=length-offset;if(n>chunk)n=chunk;
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,mode==1?5000:mode==11?10000:60000,0);
        if(status!=LT48_OK)break;
        if(s->r->page && mode==2) {
            unsigned passed=0;
            /* Original Page worker zero-route: command 02 enters data phase,
             * one whole page via 44, completion 0B, command 00 leaves phase. */
            const unsigned char enter[]={2,0},leave[]={0,0};unsigned char reply[512];
            status=cmd(s,enter,2,reply);
            if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
            if(status==LT48_OK)status=block(s,0x44,(unsigned char*)image+offset,n,NULL);
            if(status==LT48_OK)status=flag(s,0x0b,&passed);
            if(status==LT48_OK && !passed)status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK)status=cmd(s,leave,2,reply);
            if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
        } else if(mode==1)status=block(s,0x43,read+offset,n,NULL);
        else if(mode==11) {
            unsigned char count[2]={(unsigned char)(n/width),(unsigned char)(n/width>>8)};unsigned passed=0;
            status=block(s,0x44,count,2,NULL);
            if(status==LT48_OK)status=flag(s,0x0b,&passed);
            if(status==LT48_OK&&!passed) {
                unsigned char remaining[2]={0};status=block(s,0x43,remaining,2,NULL);
                size_t rem=remaining[0]|(size_t)remaining[1]<<8;
                if(status==LT48_OK) {
                    if(rem>=n/width)status=LT48_PROTOCOL_ERROR;
                    else {out->first_mismatch=start+(uint32_t)(offset+n-width*rem-width);out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}
                }
            }
        } else {
            unsigned char reply[512]={0};status=block(s,0x48,(unsigned char*)image+offset,n,reply);
            if(status==LT48_OK&&!(reply[0]|reply[1])) {
                /* The original derives an unchecked remaining count here.
                 * Never resume programming from untrusted failure counters. */
                /* Original error callback 004689e0 drains one actual device element
                 * through transport +24 (00496850) before stopping the applet. */
                unsigned char actual[2];
                status=block(s,0x43,actual,width,NULL);
                out->mismatch_count++;out->first_mismatch=UINT32_MAX;
                if(status==LT48_OK)status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK&&offset+n==length) {
                unsigned passed=0;status=flag(s,0x0c,&passed);
                if(status==LT48_OK&&!passed) {
                    unsigned char actual[2];
                    status=block(s,0x43,actual,width,NULL);
                    out->mismatch_count++;
                    out->first_mismatch=start+(uint32_t)(length-width);
                    if(status==LT48_OK) {
                        if(width==2 && actual[0]==image[length-2])out->first_mismatch++;
                        status=LT48_DEVICE_MISMATCH;
                    }
                }
            }
        }
        if(status==LT48_OK){offset+=n;out->completed_bytes=offset;}
    }
    /* DEVRESET must be sent even when the bulk worker failed. The final
     * uncancellable owner below also provides an independent reset path. */
    if(status==LT48_OK) {
        const unsigned char finish[]={0,0};unsigned char reply[512]={0};
        status=cmd(s,finish,sizeof(finish),reply);
        if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    }
    /* Original legacy pre-erase streaming keeps DEVINIT mode 7 active until
     * the Erase wave completes; resetting here would remove VPP early. */
    if(status==LT48_OK && !(s->r->legacy28f && mode==7))status=run(s,3);
    return status;
}
/* Flash validation must observe actual bytes, including the final word. A
 * controller success bit alone was insufficient on a disconnected adapter. */
static lt48_status checked_cycle(xp_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *pending,uint16_t vcc,lt48_xp_flash_result *out) {
    lt48_status status=cycle(s,mode,start,length,image,NULL,vcc,out);
    if(status==LT48_OK) {
        status=cycle(s,1,start,length,NULL,pending,vcc,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=image[i]) {
            if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    }
    return status;
}
static lt48_status identify(xp_session *s,lt48_m37_id_decision_fn decide,void *context) {
    if(!s->r->wave[4])return LT48_OK;
    for(unsigned attempt=0;attempt<64;attempt++) {
        if(s->r->id_table) {
            unsigned observed=0,expected=0,first_observed=0;int match=1;
            for(unsigned i=0;i<s->r->id_table->count;i++) {
                uint32_t address=s->r->id_table->addresses[i];expected=s->r->id_table->values[i];
                for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(8*j));
                memset(s->buffers[4],0,32);s->buffers[4][0]=(unsigned char)expected;s->buffers[4][1]=(unsigned char)(expected>>8);
                lt48_status status=run(s,4);
                memset(s->buffers[0],0,32);
                if(status!=LT48_OK)return status;
                observed=s->buffers[4][0]|(unsigned)s->buffers[4][1]<<8;
                if(i==0)first_observed=observed;
                if(observed!=expected){match=0;break;}
            }
            if(match)return LT48_OK;
            unsigned manufacturer=s->r->id_table->values[0];
            lt48_m37_id_choice choice=decide?decide(context,manufacturer,expected,first_observed,observed):LT48_M37_ID_ABORT;
            if(choice==LT48_M37_ID_IGNORE)return LT48_OK;
            if(choice!=LT48_M37_ID_RETRY)return LT48_DEVICE_MISMATCH;
            continue;
        }
        memset(s->buffers[4],0,32);memset(s->buffers[6],0,32);
        lt48_status status=run(s,4);if(status!=LT48_OK)return status;
        unsigned man=s->buffers[6][0]|(unsigned)s->buffers[6][1]<<8;
        unsigned dev=s->buffers[4][0]|(unsigned)s->buffers[4][1]<<8;
        int match=(s->r->man||s->r->dev)?man==s->r->man&&dev==s->r->dev:
            (man==dev && (man==0 || man==255));
        if(match)return LT48_OK;
        lt48_m37_id_choice choice=decide?decide(context,s->r->man,s->r->dev,man,dev):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)return LT48_OK;
        if(choice!=LT48_M37_ID_RETRY)return LT48_DEVICE_MISMATCH;
    }
    return LT48_SAFETY_REJECTED;
}
/* Sanyo M b5: UNPROTECT sets a constant DataIn=1. Verify both source lock
 * fields through READCFG after the original command and reset sequence. */
static lt48_status sanyo_unprotect(xp_session *s) {
    memset(s->buffers[0],0,32);s->mode=14;
    lt48_status status=run(s,2);
    if(status==LT48_OK)status=run(s,13);
    if(status==LT48_OK)status=run(s,3);
    s->config_count=2;
    s->config[0]=(lt48_runtime_configuration_value){-1,0,0,0};
    s->config[1]=(lt48_runtime_configuration_value){-1,0,1,0};
    memset(s->buffers[0],0,32);s->mode=12;
    if(status==LT48_OK)status=run(s,2);
    if(status==LT48_OK)status=run(s,6);
    if(status==LT48_OK) {
        unsigned found=0;
        for(size_t i=0;i<s->config_count;i++)if(s->config[i].category==0 && s->config[i].field<2){
            if(s->config[i].value)status=LT48_DEVICE_MISMATCH;
            found|=1u<<s->config[i].field;
        }
        if(found!=3)status=LT48_FORMAT_ERROR;
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}
#include "../../NewDeviceLib/sources/tmp1962one/security.inc"
/* Original 00467c90: mode 14, all PREUNPROTECT sectors, then all
 * UNPROTECT sectors; mode 13 plus PROTECT for Secure. Address is a device
 * element index on entry; every source wave may mutate it, so restore each boundary. */
static lt48_status protection(xp_session *s,int secure) {
    if(s->r->at17f)return secure?LT48_UNSUPPORTED:LT48_OK;
    /* Original 00467c90: missing UNPROTECT skips the entire prerequisite;
     * PREUNPROTECT is optional even when UNPROTECT exists. */
    if(!secure && !s->r->wave[13])return LT48_OK;
    if(!secure && s->r->as15)return sanyo_unprotect(s);
    if(secure && !s->r->wave[11])return LT48_UNSUPPORTED;
    memset(s->buffers[0],0,32);s->mode=secure?13:14;
    lt48_status status=run(s,2);
    const uint32_t *protection_sectors=s->r->word?s->r->word->settings.protection:s->r->closure->sectors;
    size_t protection_count=s->r->word?s->r->word->settings.protection_count:s->r->closure->count;
    for(unsigned phase=0;status==LT48_OK && phase<(secure?1u:2u);phase++) {
        unsigned op=secure?11:phase==0?12:13;
        if(!s->r->wave[op])continue;
        for(size_t i=0;status==LT48_OK && i<protection_count;i++) {
            uint32_t word=protection_sectors[i]/s->r->element_bytes;
            for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(word>>(j*8));
            s->buffers[3][0]=1;
            status=run(s,op);
            if(status==LT48_OK && !s->buffers[2][0])status=LT48_DEVICE_MISMATCH;
        }
    }
    if(status==LT48_OK)status=run(s,3);
    /* Original 00467c90 completes at reset. Word secondary tables can include
     * permanent-lock fields which global unprotect cannot clear. */
    if(s->r->word || s->r->intel)return status;
    /* Power the array without starting the mode-1 streaming read applet.
     * READCFG starts its own 0x5800 applet. A live read stream otherwise
     * contaminates the status capture with queued array bytes. */
    memset(s->buffers[0],0,32);s->mode=0;
    if(status==LT48_OK)status=run(s,2);
    for(size_t i=0;status==LT48_OK && i<s->r->closure->count;i++) {
        uint32_t word=s->r->closure->sectors[i]/s->r->element_bytes;
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(word>>(j*8));
        status=run(s,6);
        if(status==LT48_OK && s->buffers[2][0]!=(secure?(lt48_xp_flash_protection_choices(s->r)?s->r->protection_choice:1):0))status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)status=run(s,3);
    /* Re-read status has established every block unlocked. A cached 0x98
     * also represents an ordinary eight-bit mask, so do not let that stale
     * value skip the explicit erase workers. This is an authored policy. */
    if(status==LT48_OK && s->r->tmp1962one && !secure)memset(s->buffers[15],0,32);
    return status;
}
/* Original XP Unlock worker 00467e00: mode 15, primary sector traversal.
 * The special protection-register wave (11) is deliberately not called. */
static lt48_status c3_sectors(xp_session *s,unsigned mode,unsigned operation) {
    /* Original 00467e00 returns success when UNLOCK is absent. */
    if(operation==14 && !s->r->wave[14])return LT48_OK;
    s->mode=mode;memset(s->buffers[0],0,32);
    lt48_status status=run(s,2);
    const uint32_t *sectors=s->r->primary?s->r->primary->sectors:s->r->intel?s->r->intel->erase:s->r->closure->sectors;
    size_t count=s->r->primary?s->r->primary->count:s->r->intel?s->r->intel->erase_count:s->r->closure->count;
    for(size_t i=0;status==LT48_OK && i<count;i++) {
        uint32_t word=s->r->sanyo256?(uint32_t)i*256:sectors[i]/s->r->element_bytes;
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(word>>(8*j));
        /* ICE fixed-delay erase preserves the nested unlock success result. */
        if(!(s->r->ice2 && operation==10))memset(s->buffers[2],0,32);
        status=run(s,operation);
        if(status==LT48_OK && (s->r->page?!s->buffers[2][0]:s->buffers[2][0]!=((operation==10 && s->r->mux_bulk_status)?0x80:1)))status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}
static lt48_status page_unlock(xp_session *s) {
    if(s->r->wave[14])return c3_sectors(s,15,14);
    if(!s->r->wave[13])return LT48_OK;
    s->mode=14;memset(s->buffers[0],0,32);lt48_status st=run(s,2);
    for(size_t i=0;st==LT48_OK && i<s->r->page->protection_count;i++) {
        uint32_t a=s->r->page->protection[i]/s->r->element_bytes;
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(a>>(8*j));
        st=run(s,13);if(st==LT48_OK && !s->buffers[2][0])st=LT48_DEVICE_MISMATCH;
    }
    if(st==LT48_OK)st=run(s,3);
    return st;
}
/* Read before writing settings; retain all OTP data and unrelated lock bits.
 * For OTP-only definitions Secure is explicitly permanent and is identified
 * through the public secure-kind query and operator warning. */
static lt48_status page_secure(xp_session *s) {
    const struct page_wide_closure *p=s->r->word?&s->r->word->settings:s->r->page;
    if(!p->secure_kind)return LT48_UNSUPPORTED;
    if(p->field_count>600)return LT48_FORMAT_ERROR;
    s->config_count=p->field_count;
    for(size_t i=0;i<p->field_count;i++)s->config[i]=(lt48_runtime_configuration_value){-1,p->fields[i][0],p->fields[i][1],0};
    if(p->uses_image){s->config_image_size=s->r->capacity+2048;s->config_image=malloc(s->config_image_size);if(!s->config_image)return LT48_NO_MEMORY;memset(s->config_image,255,s->config_image_size);}
    s->mode=12;memset(s->buffers[0],0,32);lt48_status st=run(s,2);
    for(size_t k=0;st==LT48_OK && k<p->protection_count;k++) {
        uint32_t address=p->protection[k]/s->r->element_bytes;
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(8*j));
        st=run(s,6);if(st!=LT48_OK)break;
        lt48_runtime_configuration_value wanted[600];size_t count=s->config_count;
        memcpy(wanted,s->config,count*sizeof(*wanted));
        for(size_t i=0;i<count;i++)for(size_t j=0;j<p->field_count;j++)if(wanted[i].category==p->fields[j][0] && wanted[i].field==p->fields[j][1] && p->fields[j][2])wanted[i].value=1;
        memcpy(s->config,wanted,count*sizeof(*wanted));
        unsigned char *original=NULL;
        if(s->config_image){original=malloc(2048);if(!original){st=LT48_NO_MEMORY;break;}memcpy(original,s->config_image+s->r->capacity,2048);}
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(8*j));
        st=run(s,3);s->mode=13;
        if(st==LT48_OK)st=run(s,2);
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(8*j));
        s->buffers[3][0]=1;if(st==LT48_OK)st=run(s,11);
        if(st==LT48_OK && !p->field_count && !s->buffers[2][0])st=LT48_DEVICE_MISMATCH;
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(8*j));
        if(st==LT48_OK)st=run(s,3);
        s->mode=12;
        if(st==LT48_OK)st=run(s,2);
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(8*j));
        if(st==LT48_OK)st=run(s,6);
        if(st==LT48_OK && !p->field_count && s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
        for(size_t i=0;st==LT48_OK && i<count;i++){size_t j=0;for(;j<s->config_count;j++)if(wanted[i].category==s->config[j].category && wanted[i].field==s->config[j].field)break;if(j==s->config_count || wanted[i].value!=s->config[j].value){if(getenv("LABTOOL48_XP_TRACE"))fprintf(stderr,"page config mismatch category%u field%u expected%u actual%u\n",wanted[i].category,wanted[i].field,wanted[i].value,j<s->config_count?s->config[j].value:999);st=LT48_DEVICE_MISMATCH;}}
        if(st==LT48_OK && original && memcmp(original,s->config_image+s->r->capacity,2048)){if(getenv("LABTOOL48_XP_TRACE")){size_t j=0;while(j<2048 && original[j]==s->config_image[s->r->capacity+j])j++;fprintf(stderr,"page OTP data changed offset%zu before%02x after%02x\n",j,original[j],s->config_image[s->r->capacity+j]);}st=LT48_DEVICE_MISMATCH;}
        free(original);
    }
    lt48_status end=run(s,3);if(st==LT48_OK)st=end;
    free(s->config_image);s->config_image=NULL;s->config_image_size=0;return st;
}
#include "../../NewDeviceLib/sources/sst-page/owner.inc"
#include "../../NewDeviceLib/sources/at29-13/owner.inc"
#include "../../NewDeviceLib/sources/emtc18/owner.inc"
#include "../../NewDeviceLib/sources/nand15/owner.inc"
#include "../../NewDeviceLib/sources/xp13/owner.inc"
#include "../../NewDeviceLib/sources/tpi15/owner.inc"
lt48_status lt48_xp_flash_execute(const lt48_xp_flash_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_xp_flash_result *out) {
    if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
    if(r && r->tpi15)return tpi15_execute(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(r && r->otp11)return xp13_otp_execute(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(r && r->nand15)return nand15_execute(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(r && r->emtc18)return emtc18_execute(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(r && r->at29)return at29_execute(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(r && r->sst_page)return sst_page_execute(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_xp_flash_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_xp_flash_validate_verify_options(r,op,margin,margin_type);
    if(status!=LT48_OK)return status;
    if(r->paired_program && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM) &&
       ((start&3u) || (length&3u))) {
        /* Preserve an adjacent word when the requested range cuts a pair.
         * Read and later verify the entire physical write span. */
        uint32_t aligned=start&~3u;
        size_t prefix=start-aligned,span=(prefix+length+3u)&~(size_t)3u;
        if(span>r->capacity-aligned)return LT48_SAFETY_REJECTED;
        unsigned char *merged=malloc(span);if(!merged)return LT48_NO_MEMORY;
        status=lt48_xp_flash_execute(r,api,context,LT48_OPERATION_READ,aligned,span,
            NULL,0,merged,span,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++) {
            unsigned char old=merged[prefix+i];
            if((op==LT48_OPERATION_PROGRAM && old!=255) || (old&image[i])!=image[i]) {
                out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;
                status=LT48_DEVICE_MISMATCH;break;
            }
        }
        if(status==LT48_OK) {
            memcpy(merged+prefix,image,length);
            status=lt48_xp_flash_execute(r,api,context,LT48_OPERATION_OVER_PROGRAM,aligned,span,
                merged,span,NULL,0,margin,margin_type,decide,decision_context,out);
        }
        out->operation_status=status;
        out->completed_bytes=status==LT48_OK?length:0;
        free(merged);return status;
    }
    unsigned char *pending=malloc(length),*preserved=NULL;if(!pending)return LT48_NO_MEMORY;
    if(op==LT48_OPERATION_SECURE) {
        preserved=malloc(length);if(!preserved){free(pending);return LT48_NO_MEMORY;}
        lt48_xp_flash_result before;
        status=lt48_xp_flash_execute(r,api,context,LT48_OPERATION_READ,start,length,NULL,0,preserved,length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&before);
        if(status!=LT48_OK){*out=before;free(preserved);free(pending);return status;}
    }
    xp_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;memcpy(s.vars,r->vars,sizeof(s.vars));
    for(unsigned j=0;j<4;j++)s.buffers[1][j]=(unsigned char)((r->capacity/r->element_bytes)>>(j*8));
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(preserved);free(pending);return status;}
    status=setup(&s);
    if(status==LT48_OK)status=identify(&s,decide,decision_context);
    if(status==LT48_OK && r->tmp1962one && op!=LT48_OPERATION_ERASE)status=tmp1962one_readable(&s);
    if(status==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) {
        status=cycle(&s,op==LT48_OPERATION_PROGRAM?11:1,start,length,NULL,pending,r->nominal,out);
        if(status==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM)
            for(size_t i=0;i<length;i++)if((pending[i]&image[i])!=image[i]) {
                out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;status=LT48_DEVICE_MISMATCH;break;
            }
    }
    /* 00466c10 invokes virtual +a8 (Unlock) before the program worker. */
    if(status==LT48_OK && r->page && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))status=page_unlock(&s);
    if(status==LT48_OK && (r->c3||r->atmel64) && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))
        status=c3_sectors(&s,15,14);
    if(status==LT48_OK && op==LT48_OPERATION_ERASE) {
        status=r->page?page_unlock(&s):protection(&s,0);
        if(status==LT48_OK && (r->c3||r->intel||r->atmel64))status=c3_sectors(&s,15,14);
        s.mode=7;memset(s.buffers[0],0,32);
        if(status==LT48_OK && r->legacy28f) {
            /* Original 00467a80: full-array inverse-blank fill via +CC,
             * then Erase via +D0 without intervening DEVRESET. */
            memset(pending,0,length);
            status=cycle(&s,7,0,length,pending,NULL,r->nominal,out);
            memset(s.buffers[0],0,32);
            if(status==LT48_OK)status=legacy28f_erase(&s);
            if(status==LT48_OK && s.buffers[2][0]!=1)status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK)status=run(&s,3);
        }
        if(status==LT48_OK && !r->legacy28f && (r->c3||r->page||r->intel||r->primary))status=c3_sectors(&s,7,10);
        if(status==LT48_OK && !r->c3 && !r->page && !r->intel && !r->primary)status=run(&s,2);
        if(status==LT48_OK && !r->c3 && !r->page && !r->intel && !r->primary)status=run(&s,10);
        if(status==LT48_OK && (r->page?!s.buffers[2][0]:s.buffers[2][0]!=(r->mux_bulk_status?0x80:1)))status=LT48_DEVICE_MISMATCH;
        if(status==LT48_OK && !r->c3 && !r->page && !r->intel && !r->primary)status=run(&s,3);
        if(status==LT48_OK)status=cycle(&s,1,0,length,NULL,pending,r->nominal,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=255) {
            if(!out->mismatch_count)out->first_mismatch=(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    } else if(status==LT48_OK && op==LT48_OPERATION_SECURE) {
        status=(r->page||r->word)?page_secure(&s):protection(&s,1);
    } else if(status==LT48_OK) {
        if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};size_t n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
            for(size_t i=0;status==LT48_OK&&i<n;i++)status=checked_cycle(&s,4,start,length,image,pending,volts[i],out);
        } else {
            unsigned mode=op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_COMPARE?5:2;
            if(op==LT48_OPERATION_COMPARE)status=checked_cycle(&s,mode,start,length,image,pending,r->nominal,out);
            else status=cycle(&s,mode,start,length,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,out);
        }
    }
    if(status==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))
        for(unsigned i=0;status==LT48_OK&&i<2;i++)status=checked_cycle(&s,4,start,length,image,pending,margin_voltage(r,margin_type,i),out);
    out->operation_status=status;
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;s.api=&cleanup;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        const unsigned char stop[]={0,0};unsigned char reply[512]={0};
        lt48_status step=cmd(&s,stop,sizeof(stop),reply);
        if(step==LT48_OK&&!(reply[0]|reply[1]))step=LT48_PROTOCOL_ERROR;
        if(step==LT48_OK)step=run(&s,3);
        if(step==LT48_OK)step=run(&s,1);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(step!=LT48_OK)out->cleanup_status=step;
        for(unsigned channel=0;channel<4;channel++) {
            unsigned char off[]={0x10,(unsigned char)(3-channel),0};
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,off,sizeof(off));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        }
        /* Original generic tail restores the Windows idle FPGA after power-off. */
        static const unsigned char idle_header[]={0x38,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s.wire,idle_header,sizeof(idle_header));
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK && step!=LT48_OK)out->cleanup_status=step;
        static const unsigned char finish[]={0x49,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s.wire,finish,sizeof(finish));
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        out->cleanup_completed=out->cleanup_status==LT48_OK;
        if(out->cleanup_completed) {
            unsigned char led[]={0x40,status==LT48_OK?4:2};
            step=lt48_runtime_transport_session_controller_write(s.wire,led,sizeof(led));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(step!=LT48_OK){out->cleanup_status=step;out->cleanup_completed=0;}
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    if(status==LT48_OK &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||
        op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_ERASE||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_SECURE)) {
        /* Fresh power/setup/read after cleanup also rejects a floating bus
         * echoing the last written byte during same-session verification. */
        lt48_xp_flash_result fresh;
        status=lt48_xp_flash_execute(r,api,context,LT48_OPERATION_READ,start,length,
            NULL,0,pending,length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=((op==LT48_OPERATION_ERASE||op==LT48_OPERATION_BLANK_CHECK)?255:op==LT48_OPERATION_SECURE?preserved[i]:image[i])) {
            if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
        out->operation_status=status;
        if(status!=LT48_OK && out->cleanup_completed) {
            const unsigned char error_led[]={0x40,2};
            lt48_status led=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(led==LT48_OK)led=lt48_runtime_transport_session_controller_write(s.wire,error_led,sizeof(error_led));
            if(led==LT48_OK)led=lt48_runtime_transport_session_flush(s.wire);
            if(led!=LT48_OK){out->cleanup_status=led;out->cleanup_completed=0;}
            lt48_runtime_transport_session_close(s.wire);
        }
    }
    if(status==LT48_OK)out->completed_bytes=length;
    free(preserved);free(pending);return status;
}

lt48_status lt48_xp_atmel64_open_private(const lt48_installation *x,size_t device,lt48_xp_flash_resources **out){return xp_flash_open_impl(x,device,out,16);}

lt48_status lt48_parallel20_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){return xp_flash_open_impl(x,i,out,17);}
int lt48_parallel20_member_internal(unsigned cls,unsigned d){return p20_find(cls,d)!=NULL;}
int lt48_parallel20_admitted_internal(unsigned d){return p20_find(2031,d)&&p20_admitted(d);}

lt48_status lt48_sharp13_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){return xp_flash_open_impl(x,i,out,18);}
int lt48_sharp13_member_internal(unsigned cls,unsigned d){return sh13_find(cls,d)!=NULL;}
int lt48_sharp13_admitted_internal(unsigned d){return sh13_find(2033,d)&&sh13_admitted(d);}

lt48_status lt48_nxp18_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){return xp_flash_open_impl(x,i,out,19);}
int lt48_nxp18_member_internal(unsigned cls,unsigned d){return nxp18_find(cls,d)!=NULL;}
int lt48_nxp18_admitted_internal(unsigned d){return nxp18_find(2038,d)&&LT48_NXP18_ADMITTED;}

lt48_status lt48_lpc16_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){return xp_flash_open_impl(x,i,out,20);}
int lt48_lpc16_member_internal(unsigned cls,unsigned d){return lpc16_find(cls,d)!=NULL;}
int lt48_lpc16_admitted_internal(unsigned d){return lpc16_admitted(d);}

lt48_status lt48_xp13_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){lt48_catalogue_entry e;lt48_status st=lt48_installation_get_device(x,i,&e);if(st)return st;if(!xp13_binding(&e))return LT48_UNSUPPORTED;return e.device_class==2035?xp13_otp_open(x,i,out):xp_flash_open_impl(x,i,out,18);}
int lt48_xp13_binding_internal(const lt48_catalogue_entry*e){return xp13_binding(e);}
int lt48_xp13_admitted_internal(void){return LT48_XP13_ADMITTED;}

lt48_status lt48_tpi15_open_private(const lt48_installation*x,size_t i,lt48_xp_flash_resources**out){return tpi15_open(x,i,out);}
int lt48_tpi15_binding_internal(const lt48_catalogue_entry*e){return tpi15_binding(e);}
int lt48_tpi15_admitted_internal(void){return LT48_TPI15_ADMITTED;}
