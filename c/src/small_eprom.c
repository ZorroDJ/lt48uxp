#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../NewDeviceLib/sources/cypress55/profiles.h"
#include "../../NewDeviceLib/sources/hyundai25/profiles.h"
#include "../../NewDeviceLib/sources/hyundai18/profiles.h"
#include "../../NewDeviceLib/sources/hyundai13/profiles.h"
#include "../../NewDeviceLib/sources/scenix6/profiles.h"
#include "../../NewDeviceLib/sources/syntek4/profiles.h"
#include "../../NewDeviceLib/sources/elan5/profiles.h"
#include "../../NewDeviceLib/sources/sm2965four/profiles.h"
#include "../../NewDeviceLib/sources/scenix6/corrections.h"
#include "../../NewDeviceLib/sources/zilog70/profiles.h"
#include "../../NewDeviceLib/sources/z8tailfour/policy.h"
#include "../../NewDeviceLib/sources/st62-65/profiles.h"
#include "../../NewDeviceLib/sources/prom17/profiles.h"
#include "../../NewDeviceLib/sources/prom30/profiles.h"
#include "../../NewDeviceLib/sources/compact-prom3/profiles.h"
#include "../../NewDeviceLib/sources/atmel-hc2/profiles.h"
#include "../../NewDeviceLib/sources/cypress-prom11/profiles.h"
#include "../../NewDeviceLib/sources/compact40/profiles.h"
#include "../../NewDeviceLib/sources/cop48/profiles.h"
#include "../../NewDeviceLib/sources/holtek19/profiles.h"
#include "../../NewDeviceLib/sources/pic-common/source_verify_voltage.h"
#include "../../NewDeviceLib/sources/prom24/profiles.h"
#include "../../NewDeviceLib/sources/prom24/source_compat.h"

/* Digest-bound compact memory engines: GAD 2716/2732 and GA Philips
 * bipolar PROMs. Operation and buffer IDs retain their source meanings. */
struct lt48_small_eprom_resources {
    const struct sg4_profile*sg4;uint8_t sg4_mask,sg4_value;
    const struct el5_profile*el5;uint16_t el5_clear[2];
    const struct sy4_profile*sy4;unsigned sy4_clear;
    const struct sx6_profile*sx6;uint16_t sx6_clear[2];
    const struct hy25_profile*hy25;
    const struct hy18_profile*hy18;
    const struct hy13_profile*hy13;uint16_t hy13_set; unsigned char hy13_extended[16];
    unsigned hc2, nvram, eeprom, timekeeper, at89, protection, hc711_parallel;
    unsigned holtek19, cop48, compact40, seek_op, source_erase, source_secure, counter, mcm68766, prom17, prom30, cypress55, zilog70, zilog_parallel;
    uint32_t option_bits;
    unsigned init_op,protect_op,element_bytes,verify_count;uint16_t verify_levels[2];
    uint16_t zilog_config_mask,zilog_config_values;
    unsigned st62,st62_legacy,st62_nvm_base,st62_cfg_mask,st62_cfg_set;uint8_t st62_cfg_fields[16],st62_cfg_write_fields[16];uint32_t st62_holes[9];
    lt48_family_operation_options source_options;
    unsigned z8_ram_protection;
    unsigned phantom, multiplexed, st_power, second_enable, nominal, ram_guard;
    unsigned char *idle; size_t idle_size;
    const unsigned char *parallel_fpga;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[32];
    lt48_wave_code applets[3];lt48_applet_address applet_addresses[3];size_t applet_count;
    unsigned mode_buffer,address_buffer,input_buffer,output_buffer,buffer_count;
    unsigned reset_op,load_op,program_op,prom,mask,blank,cypress_prom11;
    lt48_compile_variable vars[16];
    size_t var_count;
    uint32_t capacity;
};
#include "../../NewDeviceLib/sources/hc711-parallel/profiles.inc"
#include "../../NewDeviceLib/sources/mcm68766/profiles.h"
int lt48_small_eprom_is_mcm68766(const lt48_small_eprom_resources *r){return r?r->mcm68766:0;}
int lt48_small_eprom_is_hc711_parallel(const lt48_small_eprom_resources *r){return r?r->hc711_parallel:0;}
void lt48_small_eprom_close(lt48_small_eprom_resources *r) {
    if (!r) return;
    for (size_t i=0;i<32;i++) lt48_compiled_wave_close(r->wave[i]);
    free(r->idle); lt48_pin_map_close(r->pins); lt48_rez_close(r->rez); free(r);
}
uint32_t lt48_small_eprom_capacity(const lt48_small_eprom_resources *r) {
    return r ? r->capacity : 0;
}
int lt48_small_eprom_is_nvram(const lt48_small_eprom_resources *r) { return r && r->nvram; }
int lt48_small_eprom_is_eeprom(const lt48_small_eprom_resources *r) { return r && (r->eeprom==1 || r->hc2); }
int lt48_small_eprom_is_sram(const lt48_small_eprom_resources *r) { return r && r->eeprom==2 && !r->timekeeper; }
int lt48_small_eprom_is_timekeeper(const lt48_small_eprom_resources *r) {return r?r->timekeeper:0;}
int lt48_small_eprom_is_at89(const lt48_small_eprom_resources *r){return r?r->at89:0;}
unsigned lt48_small_eprom_is_sm2965four(const lt48_small_eprom_resources*r){return r&&r->sg4;}
lt48_status lt48_small_eprom_set_sm2965four_configuration(lt48_small_eprom_resources*r,unsigned mask,unsigned value){if(!r||!r->sg4)return LT48_UNSUPPORTED;if((mask|value)&~127u||value&~mask)return LT48_INVALID_ARGUMENT;r->sg4_mask=mask;r->sg4_value=value;return LT48_OK;}
unsigned lt48_small_eprom_is_elan5(const lt48_small_eprom_resources*r){return r&&r->el5?r->el5->cfg_count:0;}
lt48_status lt48_small_eprom_set_elan5_configuration(lt48_small_eprom_resources*r,uint16_t a,uint16_t b){if(!r||!r->el5)return LT48_UNSUPPORTED;if((a|b)&~0x1fff||(b&&r->el5->cfg_count==1))return LT48_INVALID_ARGUMENT;r->el5_clear[0]=a;r->el5_clear[1]=b;return LT48_OK;}
unsigned lt48_small_eprom_is_syntek4(const lt48_small_eprom_resources*r){return r&&r->sy4?r->sy4->m:0;}
lt48_status lt48_small_eprom_set_syntek4_configuration(lt48_small_eprom_resources*r,unsigned clear){if(!r||!r->sy4)return LT48_UNSUPPORTED;if(clear&~63u)return LT48_INVALID_ARGUMENT;r->sy4_clear=clear;return LT48_OK;}
unsigned lt48_small_eprom_is_scenix6(const lt48_small_eprom_resources*r){return r&&r->sx6?r->sx6->m:0;}
lt48_status lt48_small_eprom_set_scenix6_configuration(lt48_small_eprom_resources*r,uint16_t fuse,uint16_t fusex){if(!r||!r->sx6)return LT48_UNSUPPORTED;if((fuse&~0xfff)||(fusex&~0x4f))return LT48_INVALID_ARGUMENT;r->sx6_clear[0]=fuse;r->sx6_clear[1]=fusex;return LT48_OK;}
unsigned lt48_small_eprom_is_hyundai13_internal(const lt48_small_eprom_resources*r){return r&&r->hy13?r->hy13->m:0;}
size_t lt48_small_eprom_hyundai13_configuration_size(const lt48_small_eprom_resources*r){return r&&r->hy13?(r->hy13->m==175?16:r->hy13->m==166||r->hy13->m==167?0:2):0;}
uint16_t lt48_small_eprom_hyundai13_option_mask(const lt48_small_eprom_resources*r){if(!r||!r->hy13)return 0;unsigned m=r->hy13->m;return m==168||m==169?0x3f01:m==170?5:m==171?6:m==174?255:m==175?65535:0;}
lt48_status lt48_small_eprom_set_hyundai13_configuration(lt48_small_eprom_resources*r,uint16_t bits){unsigned mask=lt48_small_eprom_hyundai13_option_mask(r);if(!mask||r->hy13->m==175)return LT48_UNSUPPORTED;if(bits&~mask)return LT48_INVALID_ARGUMENT;r->hy13_set=bits;return LT48_OK;}
lt48_status lt48_small_eprom_set_hyundai13_extended_configuration(lt48_small_eprom_resources*r,const unsigned char*bits,size_t n){if(!r||!r->hy13||r->hy13->m!=175)return LT48_UNSUPPORTED;if(!bits||n!=16)return LT48_INVALID_ARGUMENT;memcpy(r->hy13_extended,bits,16);return LT48_OK;}

unsigned lt48_small_eprom_is_hyundai18_internal(const lt48_small_eprom_resources*r){return r&&r->hy18;}
unsigned lt48_small_eprom_is_hyundai25_internal(const lt48_small_eprom_resources*r){return r&&r->hy25;}
int lt48_small_eprom_is_cypress55(const lt48_small_eprom_resources*r){return r&&r->cypress55;}
lt48_status lt48_small_eprom_set_protection(lt48_small_eprom_resources *r,unsigned level){
    if(!r || !r->at89 || level<1 || level>2)return LT48_INVALID_ARGUMENT;
    r->protection=level;return LT48_OK;
}
/* ST power sequencing retains its own source ramp/delay, while random byte
 * access uses the audited idle-FPGA bus with the same socket assignments. */
static lt48_status timekeeper_st_power(lt48_small_eprom_resources *r,const char *root,unsigned source_pinmap) {
    char path[4096],hex[65],pinname[16];unsigned char sha[32];size_t index;
    lt48_rez *z=NULL;lt48_pin_map *pins=NULL;lt48_pin_code pc;
    snprintf(path,sizeof path,"%s/GDAC___.rez",root);
    lt48_status st=lt48_rez_open(path,&z);
    if(st==LT48_OK)st=lt48_rez_sha256(z,sha);
    if(st==LT48_OK){for(size_t i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",sha[i]);
        if(strcmp(hex,"4abfb9024179d1cf38f17bc43ebfdaabaca4694b95e76e73edfdbbe7a0f46710"))st=LT48_SAFETY_REJECTED;}
    snprintf(pinname,sizeof pinname,"P%x",source_pinmap);
    if(st==LT48_OK)st=lt48_rez_find_resource(z,pinname,&index);
    if(st==LT48_OK)st=lt48_rez_get_pin_code(z,index,&pc);
    if(st==LT48_OK)st=lt48_pin_map_parse(&pc,&pins);
    r->vars[r->var_count++]=(lt48_compile_variable){0,4,1,r->nominal==330?270:420,0};
    r->vars[r->var_count++]=(lt48_compile_variable){1,10,0,0,500};
    for(unsigned i=0;st==LT48_OK&&i<2;i++){
        lt48_wave_code code;lt48_compact_wave *w=NULL;
        st=lt48_rez_find_resource(z,i?"W80":"W7f",&index);
        if(st==LT48_OK)st=lt48_rez_get_wave_code(z,index,&code);
        if(st==LT48_OK)st=lt48_compact_decode(code.data,code.data_size,&w);
        lt48_compiled_wave_close(r->wave[i]);r->wave[i]=NULL;
        if(st==LT48_OK)st=lt48_compact_compile_with_pin_policy(w,pins,r->vars,r->var_count,1,source_pinmap==0x16?LT48_PIN_FALLBACK_STRICT:LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[i]);
        lt48_compact_wave_close(w);
        if(st==LT48_OK&&lt48_compiled_wave_defaulted_pin_count(r->wave[i])!=(i==0&&source_pinmap!=0x16?1u:0u))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK&&i==1){
            /* Native shutdown keeps VCC up until the source 5.0 -> 4.2 V
             * ramp. W80's preceding all-rails-off would otherwise cut VCC
             * and then reassert it when the ramp seeds its starting DAC. */
            static const uint16_t expected[]={23,0,117,0,145,80,26,148,0,4,10,79,7,0,2,1,29,1,46,1,30,1,46,79};
            size_t n=0;const uint16_t *words=lt48_compiled_wave_words(r->wave[i],&n);
            uint16_t wanted[24];memcpy(wanted,expected,sizeof wanted);
            if(source_pinmap==0x18)wanted[7]=156;
            if(n!=24||memcmp(words,wanted,sizeof wanted))st=LT48_SAFETY_REJECTED;
            else {
                uint16_t adjusted[23];memcpy(adjusted,words,5*sizeof *words);
                memcpy(adjusted+5,words+6,18*sizeof *words);adjusted[0]=22;
                lt48_compiled_wave_close(r->wave[i]);r->wave[i]=NULL;
                st=lt48_compiled_host_words_internal(adjusted,23,&r->wave[i]);
            }
        }
    }
    lt48_pin_map_close(pins);lt48_rez_close(z);return st;
}
lt48_status lt48_small_eprom_open(const lt48_installation *installation,
    size_t device, lt48_small_eprom_resources **out) {
    static const struct {uint16_t d,m,p;uint32_t size;} approved[] = {
        {0,0,0,2048},{1,1,1,4096},{9,8,9,4096},{10,9,9,4096},
        {11,10,8,2048},{12,11,9,4096}};
    lt48_device_spec spec; lt48_family_info family; lt48_pin_code pins;
    lt48_small_eprom_resources *r=NULL; lt48_status status;
    char path[4096],name[24],hex[65]; unsigned char digest[32];
    size_t index,fi; int admitted=0;
    if (!out || !installation) return LT48_INVALID_ARGUMENT;
    *out=NULL;
    status=lt48_installation_resolve_device(installation,device,&spec);
    if (status!=LT48_OK) return status;
    const struct sg4_profile*sg4=sg4_binding(&spec);if(sg4&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct el5_profile*el5=el5_binding(&spec);if(el5&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct sy4_profile*sy4=sy4_binding(&spec);if(sy4&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct sx6_profile*sx6=sx6_binding(&spec);if(sx6&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct hy13_profile*hy13=hy13_binding(&spec);if(hy13&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct hy18_profile*hy18=hy18_binding(&spec);if(hy18&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct hy25_profile*hy25=hy25_binding(&spec);if(hy25&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct st62_profile*st=st62_binding(&spec);
    if(st&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct zilog70_profile*zi=zilog70_binding(&spec);if(!zi)zi=z8tail_binding(&spec);
    if(zi&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct cypress55_profile*cy=cypress55_binding(&spec);
    if(cy&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct holtek19_profile*ht=holtek19_binding(&spec);
    if(ht&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct cop48_profile*cop=cop48_binding(&spec);
    if(cop&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct compact40_profile*c40=compact40_binding(&spec);
    if(c40&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    const struct prom30_profile*prom30=prom30_binding(&spec);
    const struct prom30_profile*cp3=compact_prom3_binding(&spec);if(cp3)prom30=cp3;
    const struct prom30_profile*hc2=atmel_hc2_binding(&spec);if(hc2)prom30=hc2;
    const struct prom30_profile*cyp=cypress_prom11_binding(&spec);if(cyp)prom30=cyp;
    if(prom30&&strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    unsigned hc711_parallel=hc711_parallel_binding(&spec);
    if(hc711_parallel && LT48_HC711_PARALLEL_STAGE<(hc711_parallel==2?6:4))return LT48_UNSUPPORTED;
    int prom=spec.catalogue.device_class==2000u;
    int mcm68766=mcm68766_binding(&spec);
    if(mcm68766 && strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;
    int st_timekeeper=0;unsigned source_pinmap=spec.pinmap,nominal=hc711_parallel==1?525:500;
    if(spec.catalogue.device_class==2034u && spec.element_bits==8) {
        static const unsigned rows[][4]={{0x2a,0x1c,0x17,8192},{0x2b,0x1b,0x16,8192},{0x2c,0x1d,0x19,32768},{0x2d,0x1e,0x19,32768},{0x2e,0x1f,0x18,131072},{0x2f,0x20,0x18,131072}};
        for(unsigned i=0;i<6;i++)if(spec.definition_resource==rows[i][0]&&spec.family_code==rows[i][1]&&spec.pinmap==rows[i][2]&&spec.capacity_bytes==rows[i][3])st_timekeeper=1;
        if(spec.definition_resource==0x2c||spec.definition_resource==0x2e)nominal=330;
    }
    if (spec.catalogue.device_class!=2012u && spec.catalogue.device_class!=2011u && !prom && !st_timekeeper && hc711_parallel!=2 && !zi && !st && !hy18 && !el5 && !sg4) return LT48_UNSUPPORTED;
    for (size_t i=0;i<sizeof(approved)/sizeof(approved[0]);i++)
        if (spec.definition_resource==approved[i].d && spec.family_code==approved[i].m &&
            spec.pinmap==approved[i].p && spec.capacity_bytes==approved[i].size) admitted=1;
    if(prom) {
        static const struct {uint16_t d,m,p;uint32_t size;} groups[]={
            {0x106,0x7e,0x3a,32},{0x107,0x7e,0x3d,256},{0x104,0x7e,0x1,256},{0x103,0x7e,0x41,512},{0x108,0x7e,0x40,512},{0x109,0x7e,0x45,512},{0x105,0x7e,0x3e,512},{0x10a,0x7f,0x3f,1024},{0x10b,0x7e,0x46,1024},{0x10c,0x7f,0x43,2048},{0x10d,0x7f,0x47,4096},
            {0xf7,0x7a,0x3a,32},{0xf8,0x7b,0x3b,256},{0xf9,0x7b,0x3c,512},
            {0xfa,0x7a,0x3d,256},{0xfb,0x7a,0x3e,512},{0xfc,0x7a,1,256},
            {0xfd,0x7c,0x3f,1024},{0xfe,0x7a,0x40,512},{0xff,0x7a,0x41,512},
            {0x100,0x7d,0x42,1024},{0x101,0x7c,0x43,2048},{0x102,0x7d,0x44,2048}};
        admitted=0;
        for(size_t i=0;i<sizeof(groups)/sizeof(groups[0]);i++)
            if(spec.definition_resource==groups[i].d && spec.family_code==groups[i].m &&
               spec.pinmap==groups[i].p && spec.capacity_bytes==groups[i].size)admitted=1;
        if(mcm68766)admitted=1;
        if(prom17_binding(&spec)||prom24_binding(&spec)){if(strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;admitted=1;}
    }
    int nvram=spec.catalogue.device_class==2012u && spec.definition_resource==0x70 &&
        spec.family_code==0x38 && spec.pinmap==0x3b && spec.capacity_bytes==512 && spec.element_bits==8;
    if(!prom && spec.family_code==0x39 && spec.element_bits==4 &&
       ((spec.definition_resource==0x71 && spec.pinmap==0x3c && spec.capacity_bytes==64) ||
        (spec.definition_resource==0x72 && spec.pinmap==0x3d && spec.capacity_bytes==256)))nvram=2;
    static const struct {unsigned d,m,p,capacity;} rewritable[]={
        {0x6b,0x34,0x36,512},{0x6c,0x34,0x37,2048},{0x6d,0x34,0x38,2048},
        {0x6e,0x34,0x39,2048},{0x73,0x34,0x3e,2048},{0x76,0x35,0x3f,8192},
        {0x77,0x36,0x37,2048},{0x78,0x36,0x3e,2048},{0x79,0x36,0x3f,8192}};
    int eeprom=0,timekeeper=0;
    if(!prom)for(size_t i=0;i<sizeof(rewritable)/sizeof(*rewritable);i++)
        if(spec.definition_resource==rewritable[i].d && spec.family_code==rewritable[i].m &&
           spec.pinmap==rewritable[i].p && spec.capacity_bytes==rewritable[i].capacity && spec.element_bits==8)eeprom=1;
    if(spec.catalogue.device_class==2011u) {
        static const struct {unsigned d,m,p,capacity;} sram[]={
            {0xf,9,12,8192},{0x10,9,12,32768},{0x17,13,19,131072},
            {7,7,4,128},{8,8,5,32768},{9,8,6,8192},{10,8,7,131072},
            {0x11,11,14,2048},{0x12,10,16,8192},{0x13,11,17,32768},{0x14,11,15,131072},{0x15,11,13,524288},
            {0xb,9,8,2048},{0xc,9,9,8192},{0xd,9,10,32768},
            {0xe,9,11,131072},{0x16,12,18,524288}};
        admitted=0;nvram=0;eeprom=0;
        for(size_t i=0;i<sizeof(sram)/sizeof(*sram);i++)
            if(spec.definition_resource==sram[i].d && spec.family_code==sram[i].m &&
               spec.pinmap==sram[i].p && spec.capacity_bytes==sram[i].capacity && spec.element_bits==8)eeprom=2;
    }
    if(eeprom==2)timekeeper=spec.definition_resource==0x17?2:(spec.definition_resource==0xf||spec.definition_resource==0x10)?1:0;
    unsigned phantom=0,multiplexed=0,second_enable=0;
    if(eeprom==2 && spec.definition_resource>=7 && spec.definition_resource<=10){
        phantom=spec.definition_resource!=7;multiplexed=spec.definition_resource==7;
        if(multiplexed)timekeeper=5;
    }
    if(eeprom==2 && spec.definition_resource>=0x11 && spec.definition_resource<=0x15){
        int zram=spec.catalogue.device_name_length>=4&&!memcmp(spec.catalogue.device_name,"M48Z",4);
        timekeeper=zram?0:!strcmp(spec.catalogue.vendor,"Dallas")?4:3;
        second_enable=spec.definition_resource==0x12;
        /* Same physical bus, explicit full addresses and OE strobes; do not
         * execute the source INIT's unresolved logical pin 0. */
        spec.family_code=9;
    }
    if(st_timekeeper) {
        /* Dataman M48T08 bus audited against GAC M9/P9: identical VCC,
         * GND, A0..A12, DQ0..DQ7, CE/OE/WE. Use the idle FPGA byte bus
         * rather than whole-array XP applets that include clock registers. */
        lt48_rez *source=NULL;
        snprintf(path,sizeof(path),"%s/GDAC___.rez",lt48_installation_path(installation));
        status=lt48_rez_open(path,&source);if(status!=LT48_OK)return status;
        status=lt48_rez_sha256(source,digest);lt48_rez_close(source);
        if(status!=LT48_OK)return status;
        for(size_t i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
        if(strcmp(hex,"4abfb9024179d1cf38f17bc43ebfdaabaca4694b95e76e73edfdbbe7a0f46710"))return LT48_SAFETY_REJECTED;
        eeprom=2;timekeeper=spec.definition_resource==0x2a?0:spec.definition_resource>=0x2e?6:3;
        second_enable=spec.definition_resource==0x2b;
        spec.family_code=9;spec.pinmap=spec.capacity_bytes==8192?9:spec.capacity_bytes==32768?10:11;
    }
    unsigned at89=0;
    if(spec.catalogue.device_class==2012u && spec.pin_count==20 && spec.element_bits==8 && !spec.secondary_element_count) {
        if(spec.family_code==13 && spec.pinmap==11 &&
           ((spec.definition_resource==14 && spec.capacity_bytes==1024) ||
            (spec.definition_resource==15 && spec.capacity_bytes==2048) ||
            (spec.definition_resource==16 && spec.capacity_bytes==4096)))at89=1;
        if(spec.definition_resource==62 && spec.family_code==31 && spec.pinmap==33 && spec.capacity_bytes==2048)at89=2;
    }
    if(c40){
        if(c40->m==0x37)nvram=3;
        if(c40->m==0x42||c40->m==0x43||c40->m==3||c40->m==0x3a||c40->m==0x3b)eeprom=1;
    }
    if(sg4||el5||sy4||sx6||hy13||hy18||hy25||ht||cop||c40||prom30||cy||zi||st)admitted=1;
    if (!admitted && !nvram && !eeprom && !at89 && !hc711_parallel) return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/%s",lt48_installation_path(installation),sg4?"GADB___.rez":hy18?"GAB____.rez":(el5||hc711_parallel==2||(zi&&zi->cls==2013)||(st&&st->cls==2013))?"GADA___.rez":prom?"GA_____.rez":(eeprom==2||cyp)?"GAC____.rez":"GAD____.rez");
    if (n<0 || (size_t)n>=sizeof(path)) return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r)); if (!r) return LT48_NO_MEMORY;
#define TRY(x) do {status=(x);if(status!=LT48_OK){if(getenv("LT48_COMPACT_TRACE"))fprintf(stderr,"small EPROM open line%u status%d: %s\n",__LINE__,status,#x);goto fail;}} while(0)
    TRY(lt48_rez_open(path,&r->rez)); TRY(lt48_rez_sha256(r->rez,digest));
    for(size_t i=0;i<32;i++) snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,sg4?"a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b":hy18?"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9":(el5||hc711_parallel==2||(zi&&zi->cls==2013)||(st&&st->cls==2013))?"aca05831121f0e30654601df133b298cec1d33e7ffb94626c5d62634e690c452":prom?"a286d6799117d4e188067376cab75843fbb9299f4990091d91ad27d8abab99ad":(eeprom==2||cyp)?"bbc71e51baba29f24c9485678eb31b5bd5634789cd1acc503334c70f950a842f":"aa366e3c14e1e088730223f8be3e1343a16c551039f95fcdf7d101bc45c46706")) {
        status=LT48_SAFETY_REJECTED;goto fail;
    }
    if(mcm68766){lt48_device_definition d;
      TRY(lt48_rez_find_resource(r->rez,"D123",&index));TRY(lt48_rez_get_device_definition(r->rez,index,&d));
      if(d.value3!=255 || d.family_code!=0x8c || d.pinmap!=0x55 || d.element_count!=8192 || d.element_bits!=8){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    r->sg4=sg4;r->el5=el5;r->sy4=sy4;r->sx6=sx6;
    r->hy25=hy25;r->hy18=hy18;r->hy13=hy13;
    r->st62=st?st->m:0;r->st62_legacy=st&&st->cls==2013;if(st){memcpy(r->st62_holes,st->holes,sizeof r->st62_holes);r->st62_nvm_base=st->nvm_base;r->st62_cfg_mask=st->cfg_mask;memcpy(r->st62_cfg_fields,st->cfg_fields,16);memcpy(r->st62_cfg_write_fields,st->cfg_write_fields,16);}
    r->holtek19=ht?ht->m:0;
    r->cop48=cop?cop->m:0;
    r->hc2=hc2!=NULL;r->compact40=c40?c40->m:0;r->counter=c40&&(c40->m==14||(c40->m>=27&&c40->m<=30));r->source_secure=c40&&(c40->m==16||(c40->m>=27&&c40->m<=30));r->source_erase=c40&&c40->m>=27&&c40->m<=30;
    r->zilog_parallel=zi&&(zi->m==37||zi->m==38||(zi->m>=40&&zi->m<=44));r->zilog70=zi?zi->m:0;r->cypress55=cy!=NULL;r->cypress_prom11=cyp?cyp->m+1:0;r->prom30=prom30!=NULL;r->mcm68766=mcm68766;r->prom17=sg4||el5||sy4||prom17_binding(&spec)||prom24_binding(&spec)||(c40!=NULL)||(prom30!=NULL)||(cy!=NULL)||(zi!=NULL)||(st!=NULL)||(cop!=NULL)||(ht!=NULL)||(hy25!=NULL)||(hy18!=NULL)||(hy13!=NULL)||(sx6!=NULL);r->hc711_parallel=hc711_parallel;
    r->ram_guard=eeprom==2 && spec.catalogue.device_class==2011 && spec.definition_resource==0x11;
    r->phantom=phantom;r->multiplexed=multiplexed;r->second_enable=second_enable;r->st_power=st_timekeeper;r->nominal=nominal;
    r->at89=at89;r->protection=1;r->timekeeper=timekeeper;r->capacity=spec.capacity_bytes;r->prom=prom;r->nvram=nvram;r->eeprom=eeprom;
    r->mode_buffer=sg4?4:hy18?4:(el5||hc711_parallel==2||(zi&&zi->cls==2013)||(st&&st->cls==2013))?12:prom?7:3;r->address_buffer=(el5||hc711_parallel==2||(zi&&zi->cls==2013)||(st&&st->cls==2013))?4:prom?1:0;
    r->input_buffer=(el5||hc711_parallel==2||(zi&&zi->cls==2013)||(st&&st->cls==2013))?5:prom?2:1;r->output_buffer=(el5||hc711_parallel==2||(zi&&zi->cls==2013)||(st&&st->cls==2013))?7:prom?3:2;r->buffer_count=(sg4||el5||sy4)?70:hc711_parallel==2?14:prom?70:at89?34:(hy18||zi||st||c40||cop||ht||cyp)?70:9;
    r->init_op=(el5||(zi&&zi->cls==2013)||(st&&st->cls==2013))?4:2;r->protect_op=(el5||(zi&&zi->cls==2013)||(st&&st->cls==2013))?9:7;
    r->reset_op=(el5||(zi&&zi->cls==2013)||(st&&st->cls==2013))?5:prom?6:3;r->load_op=(el5||(zi&&zi->cls==2013)||(st&&st->cls==2013))?6:prom?7:4;r->program_op=(el5||(zi&&zi->cls==2013)||(st&&st->cls==2013))?7:prom?9:5;
    r->element_bytes=el5||sy4||sx6||ht||(zi&&zi->m==129)||(c40&&c40->bits==16)?2:1;
    if(!el5 && !sy4 && !sx6 && !ht && spec.element_bits!=8 && !((prom || nvram==2) && spec.element_bits==4) && !(((zi&&zi->m==129)||c40)&&spec.element_bits==16)){status=LT48_UNSUPPORTED;goto fail;}
    r->mask=(1u<<spec.element_bits)-1u;
    r->blank=prom&&!mcm68766?0:r->mask;
    if(sx6)r->blank=0xfff;
    if(sy4)r->blank=0x3fff;
    if(el5)r->blank=0x1fff;
    if(cy)r->blank=cy->blank;
    if(hy25)r->blank=0;
    if(hy13)r->blank=hy13->blank;
    if(zi)r->blank=zi->blank;
    if(st)r->blank=0;
    if(c40)r->blank=c40->blank;
    if(cop)r->blank=cop->blank;
    if(ht)r->blank=ht->blank;
    if(prom30){
        lt48_device_definition original;snprintf(name,sizeof name,"D%x",spec.definition_resource);
        TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_device_definition(r->rez,index,&original));
        if(original.value3!=prom30->blank){status=LT48_SAFETY_REJECTED;goto fail;}r->blank=prom30->blank;
    }
    if(prom24_binding(&spec)){
        lt48_device_definition original;
        snprintf(name,sizeof name,"D%x",spec.definition_resource);
        TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_device_definition(r->rez,index,&original));
        unsigned expected=spec.family_code==0x84?255u:0u;
        if(original.value3!=expected){status=LT48_SAFETY_REJECTED;goto fail;}
        r->blank=expected;
    }
    snprintf(name,sizeof(name),"P%x",spec.pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pins));
    TRY(lt48_pin_map_parse(&pins,&r->pins));
    /* Source P13 appends INTA/SQW and INTB to the data group. They are
     * device outputs, not DQ8/DQ9. Keep both high-Z for every write/read. */
    if(timekeeper==2)TRY(lt48_pin_map_limit_group_internal(r->pins,6,10,8));
    snprintf(name,sizeof(name),"M%x",spec.family_code);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(ht&&family.operation_count!=8){status=LT48_FORMAT_ERROR;goto fail;}
    if(cop&&family.operation_count!=(cop->m<23?8u:13u)){status=LT48_FORMAT_ERROR;goto fail;}
    if((!sg4&&!el5&&!sy4&&!sx6&&!hy13&&!hy18&&!hy25&&!ht&&!cop&&!st&&family.operation_count!=(zi?((zi->m==129||zi->m==106||zi->m==107)?9u:zi->m==47?8u:7u):(cy||cyp)?7u:c40?(r->source_erase?8u:r->source_secure?7u:6u):hc2?7u:hc711_parallel==2?7u:at89?8u:6u)) || (st&&(family.operation_count<11||family.operation_count>13)) || family.variable_count>((hy13||zi||st||c40||cop||ht)?16u:8u)) {status=LT48_FORMAT_ERROR;goto fail;}
    if(sg4||el5||sy4||sx6||hy13||hy18||zi||st||c40||cop||ht){lt48_family_operation_options o={0};status=lt48_rez_get_family_operation_options(r->rez,fi,&o);if(status!=LT48_OK&&!((sg4||el5||sy4||hy13||hy18||c40||cop||ht||(zi&&(zi->m==106||zi->m==107)))&&status==LT48_NOT_FOUND))goto fail;r->source_options=o;r->option_bits=o.checkbox_values;}
    if(sg4&&(family.operation_count!=10||family.variable_count!=3)){status=LT48_FORMAT_ERROR;goto fail;}
    if(el5&&(family.operation_count!=8||family.variable_count!=6)){status=LT48_FORMAT_ERROR;goto fail;}
    if(sy4&&(family.operation_count!=7||family.variable_count!=7)){status=LT48_FORMAT_ERROR;goto fail;}
    if(sx6&&(family.operation_count!=9||family.variable_count!=7)){status=LT48_FORMAT_ERROR;goto fail;}
    if(hy13&&hy13->stage==1&&(family.operation_count!=11||family.variable_count!=8)){status=LT48_FORMAT_ERROR;goto fail;}
    if(hy18&&(family.operation_count!=(hy18->stage==1?7u:8u)||family.variable_count!=(hy18->stage==1?4u:6u))){status=LT48_FORMAT_ERROR;goto fail;}
    if(hy25&&(family.operation_count!=(hy25->stage==3?10u:8u)||family.variable_count!=(hy25->stage==1?5u:7u))){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    }
    if(sg4||el5||sy4||sx6||hy13||hy18||hy25||ht||cop||c40||r->prom30||cy||zi||st)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==0)nominal=r->nominal=r->vars[i].value;
    for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==0)r->vars[i].value=nominal;
    if(hy13&&hy13->m==175)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==1||r->vars[i].variable_id==2)r->verify_levels[r->verify_count++]=r->vars[i].value;
    if(hy18)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==8||r->vars[i].variable_id==9)r->verify_levels[r->verify_count++]=r->vars[i].value;
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;lt48_compact_wave *w=NULL;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        unsigned operation=op.operation_id;
        if(hc711_parallel==2 && operation>=4 && operation<=7)operation-=2;
        if(hc711_parallel==2 && op.operation_id==10){
            snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&code));
            if(code.wave_type!=4 || code.data_size!=2780 || code.data[0]!=0xda || code.data[1]!=0x0a){status=LT48_FORMAT_ERROR;goto fail;}
            r->parallel_fpga=code.data+2;continue;
        }
        if(operation>=((sg4||el5||sx6||hy13||hy18||hy25||zi||st||cop||ht||hc2)?32u:cy?12u:10u) || r->wave[operation]) {status=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof(name),"W%x",op.wave);
        TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(sg4&&operation==11&&code.wave_type==4){if(code.data_size!=2780||code.data[0]!=0xda||code.data[1]!=0x0a){status=LT48_SAFETY_REJECTED;goto fail;}r->parallel_fpga=code.data+2;continue;}
        if(zi&&(zi->m==106||zi->m==107)&&code.wave_type==4){
            if(operation!=8||code.data_size!=2780||code.data[0]!=0xda||code.data[1]!=0x0a){if(getenv("LT48_COMPACT_TRACE"))fprintf(stderr,"Z8 FPGA op%u size%zu first%x/%x\n",operation,code.data_size,code.data[0],code.data[1]);status=LT48_SAFETY_REJECTED;goto fail;}
            r->parallel_fpga=code.data+2;continue;
        }
        if(zi&&(zi->m==106||zi->m==107)&&code.wave_type==6){
            if(operation!=11||code.load_address!=0x4100||code.data_size!=25||r->applet_count){if(getenv("LT48_COMPACT_TRACE"))fprintf(stderr,"Z8 applet op%u addr%x size%zu count%zu\n",operation,code.load_address,code.data_size,r->applet_count);status=LT48_SAFETY_REJECTED;goto fail;}
            r->applets[0]=code;r->applet_addresses[0]=(lt48_applet_address){11,code.load_address};r->applet_count=1;continue;
        }
        if((cop||ht)&&code.wave_type==6){
            if(r->applet_count>=3||!code.data_size||code.length1!=code.data_size+4||code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            unsigned expected_op=cop&&cop->m<23?9u:10u+(unsigned)r->applet_count;
            static const unsigned addresses[]={0xa000,0xa20a,0xa0d0},sizes[]={208,581,314};
            unsigned ht_program=ht&&ht->m==80?502:489,ht_read=ht&&ht->m==80?392:380;
            unsigned expected_address=ht?(r->applet_count?0xa000+ht_program:0xa000):cop->m<23?0xa000:addresses[r->applet_count];
            unsigned expected_size=ht?(r->applet_count?ht_read:ht_program):cop->m<23?98:sizes[r->applet_count];
            if(operation!=expected_op||code.load_address!=expected_address||code.data_size!=expected_size){status=LT48_SAFETY_REJECTED;goto fail;}
            size_t slot=r->applet_count++;
            r->applets[slot]=code;r->applet_addresses[slot]=(lt48_applet_address){operation,code.load_address};continue;
        }
        if(((hy25&&hy25->stage==3)||hy13)&&code.wave_type==1&&(operation==19||operation==20)){
            TRY(lt48_compiled_word_table_internal(code.data,code.data_size,&r->wave[operation]));continue;
        }
        if(code.wave_type!=0) {status=LT48_UNSUPPORTED;goto fail;}
        TRY(lt48_compact_decode(code.data,code.data_size,&w));
        status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1u,(c40&&r->source_erase&&operation==3)?LT48_PIN_FALLBACK_COP_RESET:(zi||st||c40||cop||ht)?LT48_PIN_FALLBACK_RESIDENT_NOOP:prom24_zero_strobe(&spec)?LT48_PIN_FALLBACK_SOURCE_ZERO_STROBE:LT48_PIN_FALLBACK_STRICT,&r->wave[operation]);
        lt48_compact_wave_close(w);if(status!=LT48_OK){if(getenv("LT48_COMPACT_TRACE"))fprintf(stderr,"compile D%x M%x op%u status%d\n",spec.definition_resource,spec.family_code,operation,status);goto fail;}
        if(!zi&&!st&&!c40&&!cop&&!ht)TRY(prom24_check_defaults(&spec,operation,r->wave[operation]));
    }
    if(sx6){TRY(sx6_corrected(r->wave[14],1,sx6->m>=137,&r->wave[30]));if(sx6->m<137)TRY(sx6_corrected(r->wave[11],0,0,&r->wave[31]));}
    if(ht&&r->applet_count!=2){status=LT48_FORMAT_ERROR;goto fail;}
    if(cop&&r->applet_count!=(cop->m<23?1u:3u)){status=LT48_FORMAT_ERROR;goto fail;}
    if(zi&&(zi->m==35||zi->m==36||(zi->m>=48&&zi->m<=51))){
        /* Authored partial-range seek: use the original resident CLOCK helper,
         * retaining the entry callback's OE state during programming. */
        const unsigned char code[]={4,0,0x26,17,0,1};lt48_compact_wave*w=NULL;
        TRY(lt48_compact_decode(code,sizeof code,&w));status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[31]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
    }
    /* Establish every address bit after ID initialization, which leaves
     * address pins in ID state. Later LOADs use the original carry update. */
    if((!sg4 && !el5 && !sy4 && !hy18 && !ht && !cop && !c40 && !prom && !at89 && !zi && !st && !cyp && hc711_parallel!=2)||r->zilog_parallel) { const unsigned char address_code[]={3,0,0x16,eeprom==2?7:9,0};
      lt48_compact_wave *w=NULL;
      TRY(lt48_compact_decode(address_code,sizeof(address_code),&w));
      r->seek_op=6;status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1u,0,&r->wave[6]);
      lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail; }
    if(sg4){const unsigned char seek[]={3,0,0x16,6,0};lt48_compact_wave*w=NULL;TRY(lt48_compact_decode(seek,sizeof seek,&w));r->seek_op=30;status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[30]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;}
    if(hy13&&(hy13->m==166||hy13->m==167)){
        /* LOAD uses a binary carry update, so establish all address bits for
         * the first byte of an arbitrary range before reusing that callback. */
        const unsigned char seek[]={3,0,0x16,7,8};lt48_compact_wave*w=NULL;
        TRY(lt48_compact_decode(seek,sizeof seek,&w));r->seek_op=30;
        status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[30]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
    }
    if(cyp){
        /* Preserve source OVERPROGRAM6: it is nested by PROGRAM5. */
        const unsigned char seek[]={3,0,0x16,7,0};lt48_compact_wave*w=NULL;
        TRY(lt48_compact_decode(seek,sizeof seek,&w));r->seek_op=30;
        status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[30]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
    }
    if(c40&&!r->counter&&c40->m!=15&&c40->m!=16){
        unsigned group=(c40->m==68||c40->m==69)?13:9;
        const unsigned char seek[]={3,0,0x16,(unsigned char)group,0};lt48_compact_wave*w=NULL;
        TRY(lt48_compact_decode(seek,sizeof seek,&w));r->seek_op=30;
        status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[30]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
    }
    if(!r->wave[0] || !r->wave[1] || !r->wave[r->init_op] || !r->wave[r->reset_op] ||
       !r->wave[r->load_op] || !r->wave[r->program_op]) {status=LT48_FORMAT_ERROR;goto fail;}
    if(timekeeper || st_timekeeper || phantom || r->ram_guard) {
        /* Explicit source logical OE high, WE high, CE low. Multiplexed bus
         * read/write waves already release their strobes. */
        const unsigned char code[]={9,0,0x11,8,1,0x11,9,1,0x11,6,0};
        lt48_compact_wave *w=NULL;
        if(multiplexed){const unsigned char idle[]={6,0,0x11,4,1,0x11,11,1};TRY(lt48_compact_decode(idle,sizeof idle,&w));status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[10]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;}
        else {TRY(lt48_compact_decode(code,sizeof code,&w));status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[10]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;}
    }
    if(st_timekeeper)TRY(timekeeper_st_power(r,lt48_installation_path(installation),source_pinmap));
    if(second_enable) {
        /* ST E2 is DIP26 / ZIF36 (engine 140); never drive INT at DIP1. */
        const uint16_t high[]={2,0,141},low[]={2,0,140};
        TRY(lt48_compiled_host_words_internal(high,3,&r->wave[8]));
        TRY(lt48_compiled_host_words_internal(low,3,&r->wave[9]));
    }
    if(nvram==1) {
        /* X20C04/X2004 datasheet RECALL: WE high, CE/OE low, NE low,
         * source tPowerUp delay, NE high. Explicit NE avoids relying on
         * a floating input; no EEPROM write occurs in this added wave. */
        static const unsigned char recall[]={17,0,0x11,0x20,1,0x11,0x0e,0,
            0x11,0x0f,0,0x11,0x10,0,0x31,20,0x11,0x10,1};
        lt48_compact_wave *w=NULL;
        TRY(lt48_compact_decode(recall,sizeof(recall),&w));
        status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1u,0,&r->wave[7]);
        lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
        TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(installation),&r->idle,&r->idle_size));
    }
    if(c40||cop){uint32_t levels[4],explicit_values[4]={0};unsigned present=0,count=0;
        if(r->source_erase||cop){present=1;for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==2)explicit_values[0]=r->vars[i].value;}
        TRY(lt48_pic_source_verify_voltages(r->nominal,explicit_values,present,0,0,levels,&count));
        if(!count||count>2){status=LT48_SAFETY_REJECTED;goto fail;}r->verify_count=count;for(unsigned i=0;i<count;i++)r->verify_levels[i]=(uint16_t)levels[i];
    }
    if(r->prom17 || r->prom30 || mcm68766 || eeprom || nvram==2 || at89 || hc711_parallel)TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(installation),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_small_eprom_close(r);return status;
#undef TRY
}
lt48_status lt48_timekeeper_region(const lt48_small_eprom_resources *r,uint32_t *base,size_t *count){
    if(!r||!r->timekeeper||!base||!count)return LT48_INVALID_ARGUMENT;
    *count=r->timekeeper==6?16:r->timekeeper==3||r->timekeeper==4?8:14;
    *base=(r->timekeeper==3||r->timekeeper==4||r->timekeeper==6)?r->capacity-(uint32_t)*count:0;return LT48_OK;
}
static unsigned small_blank_at(const lt48_small_eprom_resources*r,uint32_t a){
 return r->cypress_prom11&&(((r->cypress_prom11==1||r->cypress_prom11==2)&&a>=2048)||(r->cypress_prom11==5&&a==1024))?0:r->blank;
}
static int st62_mapped(const lt48_small_eprom_resources *r,uint32_t a){
    if(r->st62_nvm_base&&a>=r->st62_nvm_base)return 1;
    if(a<=r->st62_holes[0])return 0;
    for(unsigned i=1;i<9;i+=2)if(a>=r->st62_holes[i]&&a<=r->st62_holes[i+1])return 0;
    return 1;
}
int lt48_small_eprom_is_st62(const lt48_small_eprom_resources *r){return r&&r->st62;}
uint16_t lt48_small_eprom_st62_option_mask(const lt48_small_eprom_resources*r){return r?(uint16_t)r->st62_cfg_mask:0;}
lt48_status lt48_small_eprom_set_st62_configuration(lt48_small_eprom_resources*r,uint16_t bits){if(!r||!r->st62_cfg_mask)return LT48_UNSUPPORTED;if(bits&~r->st62_cfg_mask)return LT48_INVALID_ARGUMENT;r->st62_cfg_set=bits;return LT48_OK;}

lt48_status lt48_small_eprom_validate(const lt48_small_eprom_resources *r,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=r->capacity || length>r->capacity-start) return LT48_SAFETY_REJECTED;
    if(r->cypress_prom11&&(r->cypress_prom11==1||r->cypress_prom11==2)&&op>=LT48_OPERATION_PROGRAM&&op<=LT48_OPERATION_COMPARE&&image&&size==length&&start<=2049&&start+length>2049&&image[2049-start]>1)return LT48_INVALID_ARGUMENT;
    if(r->timekeeper && op==LT48_OPERATION_CONFIGURATION)
        {uint32_t base;size_t count;lt48_timekeeper_region(r,&base,&count);return start==base&&length==count?lt48_timekeeper_validate_registers(r->timekeeper,image,size):LT48_INVALID_ARGUMENT;}
    if(r->timekeeper && op!=LT48_OPERATION_READ && ((r->timekeeper==3||r->timekeeper==4||r->timekeeper==6)?start>=r->capacity-(r->timekeeper==6?16:8):start+length<=14))return LT48_SAFETY_REJECTED;
    if(r->zilog70>=100&&r->zilog70<=102&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->st62_cfg_mask&&op==LT48_OPERATION_CONFIGURATION)return !start&&length==2&&!image&&!size?LT48_OK:LT48_INVALID_ARGUMENT;
    if(r->st62){
        if(start||length!=r->capacity)return LT48_SAFETY_REJECTED;
        if(image&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))
            for(size_t i=0;i<size;i++)if(!st62_mapped(r,(uint32_t)i)&&image[i]!=255)return LT48_SAFETY_REJECTED;
    }
    if(r->sg4&&op==LT48_OPERATION_CONFIGURATION)return !start&&length==2&&!image&&!size?LT48_OK:LT48_INVALID_ARGUMENT;
    if(r->sg4&&(op==LT48_OPERATION_SECURE||op==LT48_OPERATION_ERASE))return !start&&length==r->capacity&&!image&&!size?LT48_OK:LT48_SAFETY_REJECTED;
    if(r->el5&&op==LT48_OPERATION_CONFIGURATION)return !start&&length==r->el5->cfg_count*2&&!image&&!size?LT48_OK:LT48_INVALID_ARGUMENT;
    if(r->el5&&(op==LT48_OPERATION_ERASE||op==LT48_OPERATION_SECURE))return LT48_UNSUPPORTED;
    if(r->sy4&&op==LT48_OPERATION_CONFIGURATION)return !start&&length==2&&!image&&!size?LT48_OK:LT48_INVALID_ARGUMENT;
    if(r->sy4&&op==LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
    if(r->sy4&&op==LT48_OPERATION_SECURE&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->sx6&&op==LT48_OPERATION_CONFIGURATION)return !start&&length==4&&!image&&!size?LT48_OK:LT48_INVALID_ARGUMENT;
    if(r->sx6&&(op==LT48_OPERATION_SECURE||op==LT48_OPERATION_ERASE)&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(lt48_small_eprom_hyundai13_option_mask(r)&&op==LT48_OPERATION_CONFIGURATION)return !start&&length==lt48_small_eprom_hyundai13_configuration_size(r)&&!image&&!size?LT48_OK:LT48_INVALID_ARGUMENT;
    if(r->hy13&&(r->hy13->m==166||r->hy13->m==167)&&image)for(size_t i=0;i<size;i++)if((start+i<(r->hy13->m==166?0x6000u:0x7000u)||start+i>=0x8000u)&&image[i]!=255)return LT48_SAFETY_REJECTED;
    if(r->hy13&&r->hy13->m>=168&&r->hy13->m<=171&&image)for(size_t i=0;i<size&&start+i<r->hy13->start;i++)if(image[i]&&!(start+i>=(r->hy13->m<=169?0x7000:0x0e00)&&start+i<=(r->hy13->m<=169?0x7022:0x0f40)))return LT48_SAFETY_REJECTED;
    if(r->hy25&&image&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))
        for(size_t i=0;i<size&&start+i<r->hy25->start;i++)if(image[i]&&!(r->hy25->stage==3&&start+i>=0x7000&&start+i<=0x7009))return LT48_SAFETY_REJECTED;
    /* Retain the original complete traversal: applet/counter state can span
     * adjacent bytes. Arbitrary subrange entry is not qualified for COP/Holtek. */
    if((r->holtek19||r->cop48)&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->holtek19&&(op==LT48_OPERATION_ERASE||op==LT48_OPERATION_SECURE))return LT48_UNSUPPORTED;
    if(r->cop48&&op==LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
    if(r->cop48&&op==LT48_OPERATION_SECURE&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->hy13&&(r->hy13->m==166||r->hy13->m==167||r->hy13->m==174||r->hy13->m==175)&&op==LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if(r->source_secure&&op==LT48_OPERATION_SECURE&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->source_secure&&!r->source_erase&&op==LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
    if((r->source_erase||r->hc2)&&op==LT48_OPERATION_ERASE&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->hc711_parallel && op==LT48_OPERATION_OVER_PROGRAM)return LT48_UNSUPPORTED;
    if(r->element_bytes==2&&((start|length|(image?size:0))&1))return LT48_SAFETY_REJECTED;
    if(r->zilog70==129&&op==LT48_OPERATION_CONFIGURATION)return !start&&length==2&&!image&&!size?LT48_OK:LT48_INVALID_ARGUMENT;
    if(op<LT48_OPERATION_READ || op>((r->sg4||r->el5||r->sy4||r->sx6||r->hy13||r->hy18||r->hy25||r->cop48||r->source_secure||r->at89||r->cypress55||r->zilog70||r->st62)?LT48_OPERATION_SECURE:(r->eeprom||r->hc2)?LT48_OPERATION_ERASE:LT48_OPERATION_COMPARE)) return LT48_UNSUPPORTED;
    if((r->hy13||r->hy18||r->hy25||r->cypress55||r->zilog70||r->st62)&&op==LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
    if((r->hy13||r->hy18||r->hy25||r->cypress55||r->zilog70||r->st62)&&op==LT48_OPERATION_SECURE&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_ERASE && op!=LT48_OPERATION_SECURE && (!image || size!=length)) return LT48_INVALID_ARGUMENT;
    if(r->at89 && (op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE ||
       (r->at89==2 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM))) && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(image && op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK)
        for(size_t i=0;i<size;i+=r->element_bytes){unsigned value=image[i];if(r->element_bytes==2)value|=(unsigned)image[i+1]<<8;if(value&~r->mask)return LT48_INVALID_ARGUMENT;}
    return LT48_OK;
}
typedef struct small_session {
    const lt48_small_eprom_resources *r;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[70][32];
    uint32_t volts[4];
    lt48_compile_variable vars[16];
    uint32_t mode;
    unsigned char legacy_image[1];
    unsigned fault;
    lt48_runtime_configuration_value configuration[128];size_t configuration_count;
} small_session;
static lt48_status run(small_session *s,unsigned op) {
    lt48_runtime_options options={0};options.host_staged_shutdown=s->r->prom17;
    /* Legacy option name: also reproduces the original explicit-channel
     * opcode3 float conversion (COP87L88 BV duplicates DAC2 onto DAC1). */
    options.host_avr_parallel=s->r->cop48!=0;
    lt48_runtime_buffer_seed seeds[70];size_t count=0;
    lt48_runtime_nested_wave nested[32];size_t nested_count=0;
    for (uint16_t i=0;i<32;i++)if(s->r->wave[i]) nested[nested_count++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};
    options.nested_waves=nested;options.nested_wave_count=nested_count;
    /* Named configuration 0 supplies the two lock levels. Direct checkbox 1
     * enables the source X2 ID check; Ignore still uses the selected procedure. */
    lt48_runtime_configuration_value configuration[]={{0,1,0,s->r->protection},{-1,0,1,1}};
    if(s->r->sg4||s->r->el5||s->r->sy4||s->r->sx6||s->r->hy13||s->r->hy18||s->r->zilog70||s->r->st62||s->r->compact40||s->r->cop48||s->r->holtek19){
        options.use_explicit_direct_configuration_object=1;options.direct_configuration_object=-2;
        if(s->r->sg4||s->r->el5||s->r->sy4||s->r->sx6||s->r->hy13||s->r->hy18||s->r->cop48||s->r->holtek19){options.use_explicit_legacy_configuration_object=1;options.legacy_configuration_object=0;}
        if(!s->configuration_count&&s->r->zilog70==129)for(unsigned i=0;i<9;i++)s->configuration[s->configuration_count++]=(lt48_runtime_configuration_value){-2,0,i<8?i:0x100,0};
        if(!s->configuration_count)for(unsigned category=0;category<3;category++)for(unsigned i=0;i<32;i++){
            uint32_t value=0;
            if(s->r->sg4||s->r->el5||s->r->sy4||s->r->sx6||s->r->hy13||s->r->hy18||s->r->cop48||s->r->holtek19)value=category==0?!!(s->r->source_options.checkbox_values&(1u<<i)):category==1?s->r->source_options.choice_values[i]:s->r->source_options.number_values[i];
            if((s->r->zilog70==106||s->r->zilog70==107)&&category==0&&i==3)value=s->r->z8_ram_protection;
            s->configuration[s->configuration_count++]=(lt48_runtime_configuration_value){0,category,i,value};
        }
        if(s->r->hy13&&s->r->hy13->m==175&&s->configuration_count==96)for(unsigned i=0;i<16;i++)s->configuration[s->configuration_count++]=(lt48_runtime_configuration_value){-2,2,i,0};
        options.configuration=s->configuration;options.configuration_count=s->configuration_count;
    }else if(s->r->at89){options.configuration=configuration;options.configuration_count=2;}
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    s->fault=0;
    for(uint8_t i=0;i<s->r->buffer_count;i++) if(i!=s->r->mode_buffer) seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    if(s->r->zilog70==101||s->r->zilog70==102){options.device_image=s->legacy_image;options.device_image_size=1;options.device_element_bytes=1;}
    options.mode=s->mode;options.use_explicit_buffer_ids=1;
    options.mode_buffer=s->r->mode_buffer;options.address_buffer=s->r->address_buffer;options.parameter_buffer_id=s->r->input_buffer;
    options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));
    options.root_timeout_seconds=10;
    lt48_status status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,
        s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->applet_addresses,s->r->applet_count,&result);
    if(status==LT48_OK) {
        for(uint8_t i=0;i<s->r->buffer_count;i++) {
            const unsigned char *data;size_t size;
            status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK || size!=32) {status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        if(status==LT48_OK&&(s->r->zilog70==101||s->r->zilog70==102)){const unsigned char *image;size_t n;status=lt48_runtime_get_image(result,&image,&n);if(status==LT48_OK&&n==1)s->legacy_image[0]=image[0];else status=LT48_RUNTIME_ERROR;}
        for(uint8_t i=0;status==LT48_OK && i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        if(status==LT48_OK&&(s->r->sg4||s->r->el5||s->r->sy4||s->r->sx6||s->r->hy13||s->r->hy18||s->r->zilog70||s->r->st62||s->r->compact40||s->r->cop48||s->r->holtek19||s->r->cypress_prom11)){
            const lt48_runtime_configuration_value*v;size_t n;status=lt48_runtime_get_configuration(result,&v,&n);
            if(status==LT48_OK){if(n>128)status=LT48_RUNTIME_ERROR;else{memcpy(s->configuration,v,n*sizeof *v);s->configuration_count=n;}}
            for(size_t i=0;status==LT48_OK&&i<s->r->var_count;i++)status=lt48_runtime_get_variable(result,s->vars[i].variable_id,&s->vars[i]);
        }
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(event.kind==LT48_RUNTIME_RESULT_LATCH) {s->fault=(unsigned)event.arguments[0];status=LT48_RUNTIME_ERROR;}
            if(event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0)status=LT48_RUNTIME_ERROR;
        }
    }
    if(status!=LT48_OK&&s->r->st62&&getenv("LT48_ST62_TRACE"))fprintf(stderr,"ST62 M%u wave%u mode%u status%d fault%u cfg%u in%u expected%u\n",s->r->st62,op,s->mode,status,s->fault,s->configuration_count?s->configuration[64].value:0,s->buffers[s->r->input_buffer][0],s->buffers[s->r->output_buffer][0]);
    if(status!=LT48_OK&&s->r->cop48&&getenv("LT48_COMPACT_TRACE"))fprintf(stderr,"COP M%u wave%u mode%u status%d fault%u\n",s->r->cop48,op,s->mode,status,s->fault);
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_compile_variable *var(small_session *s,unsigned id) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==id)return &s->vars[i];
    return NULL;
}
static lt48_status init(small_session *s,lt48_m37_id_decision_fn decide,void *context) {
    /* W1c reports manufacturer/device mismatch as status 5/6. Only the
     * selected session's comparison variables can change on explicit Ignore;
     * device, voltages, pins, pulses and programming procedure stay selected. */
    if(s->r->st62 || s->r->prom || s->r->nvram || s->r->eeprom || s->r->hc711_parallel)return run(s,s->r->init_op);
    for(unsigned attempt=0;attempt<64;attempt++) {
        lt48_status status=run(s,s->r->init_op);
        if(status==LT48_OK || (s->fault!=5 && s->fault!=6))return status;
        lt48_compile_variable *id=var(s,s->fault==5 ? 29 : 30);
        if(s->r->cop48)return LT48_DEVICE_MISMATCH;
        if(!id || !decide)return LT48_DEVICE_MISMATCH;
        uint16_t actual=s->buffers[s->r->at89?1:5][0];
        lt48_compile_variable *man=var(s,29),*dev=var(s,30);
        lt48_m37_id_choice choice=decide(context,man?man->value:0,dev?dev->value:0,
            s->fault==5?actual:(man?man->value:0),s->fault==6?actual:(dev?dev->value:0));
        if(choice==LT48_M37_ID_ABORT)return LT48_DEVICE_MISMATCH;
        if(choice==LT48_M37_ID_IGNORE)id->value=actual;
        else if(choice!=LT48_M37_ID_RETRY)return LT48_INVALID_ARGUMENT;
    }
    return LT48_DEVICE_MISMATCH;
}
static lt48_status range(small_session *s,lt48_operation_kind op,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,lt48_small_eprom_result *out) {
    unsigned stride=s->r->element_bytes;
    for(size_t i=0;i<length;i+=stride) {
        uint32_t addr=(start+(uint32_t)i)/stride;
        if(s->r->hy13){unsigned m=s->r->hy13->m;
            if(m>=168&&m<=171&&addr<s->r->hy13->start&&!(s->mode==1&&addr>=(m<=169?0x7000:0xe00)&&addr<=(m<=169?0x7022:0xf40))){if(read)read[i]=0;out->completed_bytes=i+1;continue;}
            if((m==166||m==167)&&(addr<(m==166?0x6000:0x7000)||addr>=0x8000)){if(read)read[i]=255;out->completed_bytes=i+1;continue;}
        }
        if(s->r->hy25&&addr<s->r->hy25->start&&!(s->r->hy25->stage==3&&s->mode==1&&addr>=0x7000&&addr<=0x7009)){if(read)read[i]=0;out->completed_bytes=i+1;continue;}
        if(s->r->cypress55&&(i==0||!(addr&255u))){memset(s->buffers[12],0,32);s->buffers[12][1]=1;}
        for(unsigned j=0;j<4;j++)s->buffers[s->r->address_buffer][j]=(unsigned char)(addr>>(8*j));
        for(unsigned j=0;j<stride;j++){
            s->buffers[s->r->output_buffer][j]=op==LT48_OPERATION_BLANK_CHECK?((s->r->el5||s->r->sx6||s->r->sy4)?small_blank_at(s->r,addr)>>(8*j):small_blank_at(s->r,addr)):(image?image[i+j]:0);
            if(s->r->source_erase&&addr==s->r->capacity-1&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY))s->buffers[s->r->output_buffer][j]&=0xdf;
            s->buffers[s->r->input_buffer][j]=s->buffers[s->r->output_buffer][j];
        }
        unsigned char requested[2]={s->buffers[s->r->output_buffer][0],s->buffers[s->r->output_buffer][1]};
        lt48_status status=run(s,op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM?s->r->program_op:s->r->load_op);
        if(status==LT48_OK && s->r->eeprom==2 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM))status=run(s,s->r->load_op);
        if(status!=LT48_OK)return status;
        for(unsigned j=0;j<stride;j++){
            unsigned char actual=s->buffers[s->r->input_buffer][j]&s->r->mask;
            if(read)read[i+j]=actual;
            out->completed_bytes=i+j+1;
            /* COP applets do not all return an inline data byte. AJC's
             * configuration readback also masks reserved bits, unlike its
             * caller's file byte. Do not compare a seeded/stale input here:
             * the mandatory complete source Verify below reads every byte
             * independently and applies the source configuration masks. */
            int deferred_cop_verify=(s->r->sg4||s->r->el5||s->r->sy4||s->r->sx6||s->r->cop48||s->r->holtek19)&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM);
            if(op!=LT48_OPERATION_READ && !deferred_cop_verify && actual!=((s->r->compact40&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))?requested[j]:s->buffers[s->r->output_buffer][j])) {
                if((s->r->cop48||s->r->el5)&&getenv("LT48_COMPACT_TRACE"))fprintf(stderr,"Compact compare op%u addr%u got%x expected%x requested%x\n",op,addr,actual,s->buffers[s->r->output_buffer][j],requested[j]);
                if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i+j;
                out->mismatch_count++;
                if(op!=LT48_OPERATION_COMPARE)return LT48_DEVICE_MISMATCH;
            }
        }
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status cycle(small_session *s,lt48_operation_kind op,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_m37_id_decision_fn decide,
    void *context,lt48_small_eprom_result *out) {
    /* M104's original no-reset buffer30 carries delayed option restoration
     * from Program to Verify. Other scratch buffers start a fresh host cycle. */
    unsigned delayed_st62=s->buffers[30][0];
    memset(s->buffers,0,sizeof(s->buffers));
    if(s->r->st62_legacy&&s->r->st62==104)s->buffers[30][0]=(unsigned char)delayed_st62;
    if(s->r->zilog70==129)s->buffers[3][0]=2;
    if(s->r->zilog70==101||s->r->zilog70==102)s->legacy_image[0]=image?image[0]:s->r->blank;
    for(unsigned j=0;j<4;j++)s->buffers[s->r->address_buffer][j]=(unsigned char)((start/s->r->element_bytes)>>(8*j));
    /* Cypress DEVINIT gates its programming rails on mode2. Its Overprogram
     * uses the original programming callback with that entry mode; compatibility
     * pre-read remains distinct. Other GA PROMs retain source mode3.
     * The GA PROM host keeps Over-program mode 3 in its source frame.
     * Preserve the previously qualified GAD programming-mode behavior. */
    s->mode=op==LT48_OPERATION_READ?1:
        (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)?
            ((s->r->el5||s->r->st62||(s->r->prom&&!s->r->cypress55&&!s->r->sx6))&&op==LT48_OPERATION_OVER_PROGRAM?3:2):
        op==LT48_OPERATION_VERIFY?4:op==LT48_OPERATION_COMPARE?1:11;
    lt48_compile_variable *v=var(s,0);if(v)v->value=vcc;
    if(s->r->zilog70==44){memcpy(s->buffers[6],s->buffers[s->r->address_buffer],4);if(image)s->buffers[5][0]=image[0];}
    lt48_status status=init(s,decide,context);
    if(status==LT48_OK&&s->r->zilog70==129&&s->mode==1)
        for(size_t i=0;i<s->configuration_count;i++)if(s->configuration[i].object_id==-2&&s->configuration[i].category==0&&s->configuration[i].field==0&&s->configuration[i].value)status=LT48_SAFETY_REJECTED;

    if(status==LT48_OK&&s->r->hy13&&(s->r->hy13->m==166||s->r->hy13->m==167)){
        uint32_t a=start<(s->r->hy13->m==166?0x6000u:0x7000u)?(s->r->hy13->m==166?0x6000u:0x7000u):start;
        for(unsigned j=0;j<4;j++)s->buffers[8][j]=(unsigned char)(a>>(8*j));
    }
    if(status==LT48_OK && s->r->nvram==1)status=run(s,7);
    if(status==LT48_OK && s->r->seek_op&&!s->r->nvram)status=run(s,s->r->seek_op);
    /* These parts latch an internal counter on reset and increment XTAL1
     * after each byte. Read discarded prefix bytes to reach a nonzero start. */
    for(uint32_t i=0;status==LT48_OK && (s->r->sx6||s->r->at89||s->r->counter) && i<start/s->r->element_bytes;i++)status=run(s,s->r->load_op);
    if(s->r->el5)for(uint32_t a=0;status==LT48_OK&&a<start/2;a++){for(unsigned j=0;j<4;j++)s->buffers[4][j]=(unsigned char)(a>>(8*j));status=run(s,6);}
    if(s->r->sy4)for(uint32_t a=0;status==LT48_OK&&a<start/2;a++){for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(a>>(8*j));status=run(s,4);}
    if(status==LT48_OK&&start&&!s->r->sx6&&s->r->wave[31])status=run(s,31);
    /* Cypress source LOAD caches the upper address through buffer12. Seed
     * a valid first transfer for partial ranges as well as address zero. */
    if(status==LT48_OK&&s->r->cypress55){memset(s->buffers[12],0,32);s->buffers[12][1]=1;}
    if(status==LT48_OK)status=range(s,op,start,length,image,read,out);
    /* Do not STORE a partially failed/cancelled write. Cleanup only removes power. */
    lt48_status reset=((s->r->nvram || ((s->r->zilog70==101||s->r->zilog70==102)&&s->mode==2)) && status!=LT48_OK)?LT48_OK:run(s,s->r->reset_op);
    if(s->r->nvram && status==LT48_OK && reset==LT48_OK && s->mode==2) {
        /* Confirm the queued STORE reached the controller, then hold power
         * for another 20 ms. X2004 specifies 10 ms maximum (X20C04: 5 ms).
         * This guard does not depend on the resident delay calibration. */
        reset=lt48_runtime_transport_session_status(s->wire,1);
        if(reset==LT48_OK) {
            reset=lt48_nvram_store_hold_internal();
        }
    }
    if(status==LT48_OK&&reset==LT48_OK&&(s->r->zilog70==101||s->r->zilog70==102)&&s->mode==1){
        if(read)read[0]=s->legacy_image[0];
        if(op==LT48_OPERATION_COMPARE&&image&&s->legacy_image[0]!=image[0]){if(!out->mismatch_count)out->first_mismatch=0;out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}
    }
    return status==LT48_OK?reset:status;
}
#include "timekeeper.inc"

/* MUZE option cells are OTP. Direct chip fields and host options occupy
 * different namespaces. Read actual cells before selecting any changes. */
static lt48_status zilog_word_configuration(small_session *s,unsigned secure,
        unsigned char raw[2],lt48_small_eprom_result *out) {
    memset(s->buffers,0,sizeof s->buffers);s->buffers[3][0]=2;s->mode=8;
    lt48_status st=init(s,NULL,NULL);
    if(st==LT48_OK)st=run(s,17);
    uint16_t before=s->buffers[5][0]|((uint16_t)s->buffers[5][1]<<8);
    uint16_t mask=secure?1:s->r->zilog_config_mask,values=secure?1:s->r->zilog_config_values;
    uint16_t wanted=before&~(mask&values);
    /* A cleared physical OTP cell cannot be restored to one. */
    if(st==LT48_OK&&((uint16_t)~before&mask&~values))st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK&&wanted!=before){
        if(!s->configuration_count)st=LT48_RUNTIME_ERROR;
        for(size_t i=0;i<s->configuration_count;i++){
            lt48_runtime_configuration_value *v=&s->configuration[i];
            if(v->object_id==-2&&v->category==0){unsigned bit=v->field==0x100?8:v->field;
                if(bit<9)v->value=!!(mask&values&(1u<<bit));}
        }
        if(st==LT48_OK)st=run(s,9);
        if(st==LT48_OK)st=run(s,17);
        uint16_t after=s->buffers[5][0]|((uint16_t)s->buffers[5][1]<<8);
        if(st==LT48_OK&&after!=wanted){out->mismatch_count++;out->first_mismatch=0;st=LT48_DEVICE_MISMATCH;}
    }
    raw[0]=(unsigned char)wanted;raw[1]=(unsigned char)(wanted>>8);
    if(st==LT48_OK)out->completed_bytes=2;
    lt48_status reset=run(s,s->r->reset_op);return st==LT48_OK?reset:st;
}
int lt48_small_eprom_is_zilog_word(const lt48_small_eprom_resources *r){return r&&r->zilog70==129;}
lt48_status lt48_small_eprom_set_zilog_configuration(lt48_small_eprom_resources *r,uint16_t mask,uint16_t values){
    if(!lt48_small_eprom_is_zilog_word(r)||(mask&~0x1ffu)||(values&~mask))return LT48_INVALID_ARGUMENT;
    r->zilog_config_mask=mask;r->zilog_config_values=values;return LT48_OK;
}

#include "../../NewDeviceLib/sources/st62-65/options.inc"
#include "../../NewDeviceLib/sources/hyundai13/options.inc"
#include "../../NewDeviceLib/sources/syntek4/owner.inc"
#include "../../NewDeviceLib/sources/elan5/owner.inc"
#include "../../NewDeviceLib/sources/sm2965four/owner.inc"
#include "../../NewDeviceLib/sources/scenix6/owner.inc"

lt48_status lt48_small_eprom_execute(const lt48_small_eprom_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_small_eprom_result *out) {
    if(!out || !api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_small_eprom_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if((op==LT48_OPERATION_READ || ((r->sg4||r->el5||r->sy4||r->sx6||r->zilog70==129||r->st62_cfg_mask||lt48_small_eprom_hyundai13_option_mask(r))&&op==LT48_OPERATION_CONFIGURATION&&read)) && (!read || read_size<length))return LT48_BUFFER_TOO_SMALL;
    if(margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL &&
       margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    if(r->timekeeper||r->st_power||r->phantom||r->ram_guard)return timekeeper_execute(r,api,context,op,start,length,image,read,margin,out);
    if(op==LT48_OPERATION_ERASE && !r->sg4 && !r->sx6 && !r->at89 && !r->source_erase && !r->hc2) {
        unsigned char *ff=malloc(length);if(!ff)return LT48_NO_MEMORY;
        memset(ff,255,length);
        status=lt48_small_eprom_execute(r,api,context,LT48_OPERATION_PROGRAM,start,length,
            ff,length,NULL,0,margin,decide,decision_context,out);
        free(ff);return status;
    }
    unsigned char *pending=malloc((r->nvram||r->eeprom)?r->capacity:length);
    unsigned char *expected_nvram=malloc(r->capacity);
    if(!pending||!expected_nvram){free(pending);free(expected_nvram);return LT48_NO_MEMORY;}
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    unsigned sy4_saved=0,sy4_have=0;uint16_t el5_saved[2]={0};unsigned sg4_before=0,sg4_locked=0;
    uint16_t nominal=r->hc711_parallel==1?525:500,low=r->hc711_parallel==1?499:475,high=r->hc711_parallel==1?551:525;
    if(r->sx6||r->hy13||r->hy18||r->hy25||r->holtek19||r->cop48||r->compact40||r->prom30||r->cypress55||r->zilog70||r->st62){uint32_t levels[4];unsigned count=0;nominal=r->nominal;
        status=lt48_pic_source_verify_voltages(nominal,NULL,0,0,0,levels,&count);
        if(status!=LT48_OK||count!=2){free(pending);free(expected_nvram);return LT48_SAFETY_REJECTED;}
        low=(uint16_t)levels[0];high=(uint16_t)levels[1];}
    if(((r->hy18||r->hy13)&&r->verify_count)||r->compact40||r->cop48){low=r->verify_levels[0];high=r->verify_levels[r->verify_count-1];if((r->hy18||r->hy13)&&low>high){uint16_t swap=low;low=high;high=swap;}}
    small_session s={0};s.r=r;memcpy(s.vars,r->vars,sizeof(s.vars));s.mode=1;
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(pending);free(expected_nvram);return status;}
    status=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    if(status==LT48_OK && (r->prom17||r->prom30||r->mcm68766||r->nvram||r->eeprom||r->at89||r->hc711_parallel)) {
        static const unsigned char header[]={0x38,0xaa,0x55};
        status=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof(header));
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,r->parallel_fpga?r->parallel_fpga:r->idle,r->idle_size);
    }
    if(status==LT48_OK&&(r->cop48||r->holtek19||r->zilog70==106||r->zilog70==107)){
        for(size_t i=0;status==LT48_OK&&i<r->applet_count;i++){
            const lt48_wave_code*a=&r->applets[i];size_t last=a->data_size-1;
            unsigned char header[]={0x39,0xaa,0x55,(unsigned char)a->load_address,(unsigned char)(a->load_address>>8),(unsigned char)last,(unsigned char)(last>>8)};
            status=lt48_runtime_transport_session_wave_watchdog(s.wire,5000,0);
            if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof header);
            if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.wire);
            if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,a->data,a->data_size);
            if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.wire);
        }
    }
    if(status==LT48_OK)status=run(&s,0);
    for(unsigned i=0;status==LT48_OK && i<3;i++)status=lt48_runtime_transport_session_status(s.wire,1);
    static const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
    if(status==LT48_OK&&r->sg4&&op!=LT48_OPERATION_CONFIGURATION&&op!=LT48_OPERATION_SECURE&&op!=LT48_OPERATION_ERASE){status=sg4_read_options(&s,&sg4_before,&sg4_locked);if(!status&&sg4_locked)status=LT48_SAFETY_REJECTED;}
    if(status==LT48_OK&&r->st62&&!r->st62_legacy&&op!=LT48_OPERATION_CONFIGURATION&&op!=LT48_OPERATION_SECURE){
        status=st62_read_options(&s);
        if(status==LT48_OK&&st62_protected(&s))status=LT48_SAFETY_REJECTED;
    }
    if(status==LT48_OK&&r->st62_legacy&&(r->st62==104||r->st62==106)&&op==LT48_OPERATION_VERIFY){
        /* Standalone Verify has no preceding GUI Read to populate chip options.
         * Use the source mode1 init/reset pair before mode4 compares them. */
        s.mode=1;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,r->reset_op);
    }
    /* Holtek RESET reads/writes a separate configuration word. Snapshot it
     * before any write, and before standalone Verify; never replace it with
     * an assumed blank/default value. Code images contain code words only. */
    if(status==LT48_OK&&(r->holtek19||r->sy4)&&(writing||op==LT48_OPERATION_VERIFY)){
        s.mode=1;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,r->reset_op);
    }
    if(status==LT48_OK&&r->el5&&writing){status=el5_read_options(&s,el5_saved);}
    if(status==LT48_OK&&r->sy4&&writing){sy4_saved=sy4_options(&s);sy4_have=1;}
    if(status==LT48_OK&&r->compact40==16&&writing){s.mode=1;status=init(&s,decide,decision_context);if(status==LT48_OK)status=run(&s,r->reset_op);}
    if(status==LT48_OK && (r->nvram || r->eeprom) && writing) {
        /* STORE writes the whole backing array. Preserve and later verify
         * bytes outside the requested range as well. */
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,NULL,NULL,out);
        if(status==LT48_OK) {
            memcpy(expected_nvram,pending,r->capacity);
            memcpy(expected_nvram+start,image,length);
        }
    }
    if(status==LT48_OK && !(r->nvram || r->eeprom || (r->at89==2 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM))) && writing) {
        int preserve_zilog=r->zilog70>=48&&r->zilog70<=51;
        int preserve_st62=r->st62_legacy&&(r->st62==104||r->st62==106);
        status=cycle(&s,(op==LT48_OPERATION_PROGRAM || r->at89==1)&&!(preserve_zilog||preserve_st62)?LT48_OPERATION_BLANK_CHECK:LT48_OPERATION_READ,
            start,length,NULL,pending,nominal,decide,decision_context,out);
        if(status==LT48_OK&&(preserve_zilog||preserve_st62)&&op==LT48_OPERATION_PROGRAM)for(size_t i=0;i<length;i++)if((!preserve_st62||st62_mapped(r,start+(uint32_t)i))&&pending[i]!=r->blank){status=LT48_DEVICE_MISMATCH;out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;break;}
        if(status==LT48_OK && op==LT48_OPERATION_OVER_PROGRAM)
            for(size_t i=0;i<length;i++)if((!r->hy13||r->hy13->m<168||r->hy13->m>171||start+i>=r->hy13->start)&&(!r->hy25||start+i>=r->hy25->start)&&(!r->st62_nvm_base||start+i<r->st62_nvm_base)&&(small_blank_at(r,start+(uint32_t)i)==0 ? ((pending[i]|image[i])!=image[i]) : ((pending[i]&image[i])!=image[i]))) {status=LT48_DEVICE_MISMATCH;out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;break;}
    }
    if(status==LT48_OK&&r->sg4&&(op==LT48_OPERATION_ERASE||op==LT48_OPERATION_SECURE||op==LT48_OPERATION_CONFIGURATION)){status=sg4_special(&s,op,pending,out);
    } else if(status==LT48_OK&&r->el5&&op==LT48_OPERATION_CONFIGURATION){status=el5_configuration(&s,pending,out);
    } else if(status==LT48_OK&&r->sy4&&op==LT48_OPERATION_CONFIGURATION){status=sy4_configuration(&s,pending,out);
    } else if(status==LT48_OK&&r->sy4&&op==LT48_OPERATION_SECURE){s.mode=8;status=init(&s,decide,decision_context);if(!status)status=run(&s,7);lt48_status reset=run(&s,3);if(!status)status=reset;
    } else if(status==LT48_OK&&r->sx6&&(op==LT48_OPERATION_CONFIGURATION||op==LT48_OPERATION_SECURE||op==LT48_OPERATION_ERASE)){
        status=sx6_configuration(&s,op,pending,out);
    } else if(status==LT48_OK&&lt48_small_eprom_hyundai13_option_mask(r)&&(op==LT48_OPERATION_CONFIGURATION||op==LT48_OPERATION_SECURE)){status=hy13_configuration(&s,op==LT48_OPERATION_SECURE,pending,out);
    } else if(status==LT48_OK&&r->st62&&(op==LT48_OPERATION_CONFIGURATION||(!r->st62_legacy&&op==LT48_OPERATION_SECURE))){
        status=st62_configuration(&s,op==LT48_OPERATION_SECURE,pending);
    } else if(status==LT48_OK&&r->st62&&op==LT48_OPERATION_SECURE){
        s.mode=8;status=init(&s,decide,decision_context);if(status==LT48_OK)status=run(&s,r->protect_op);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
        if(status==LT48_OK)status=run(&s,1);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.wire);
        if(status==LT48_OK)status=run(&s,0);
        if(status==LT48_OK){s.mode=1;lt48_status check=init(&s,NULL,NULL);status=(check==LT48_RUNTIME_ERROR&&s.fault==9)?LT48_OK:check==LT48_OK?LT48_DEVICE_MISMATCH:check;}
    } else if(status==LT48_OK && r->zilog70==129 && (op==LT48_OPERATION_CONFIGURATION||op==LT48_OPERATION_SECURE)){
        status=zilog_word_configuration(&s,op==LT48_OPERATION_SECURE,pending,out);
    } else if(status==LT48_OK && r->zilog70 && op==LT48_OPERATION_SECURE){
        /* Named chip configuration is not the general operation-options mask.
         * Request EPROM protection alone; retain other decoded chip fields. */
        unsigned field=r->zilog70>=47&&r->zilog70<=51?11:5;
        for(size_t i=0;i<s.configuration_count;i++)if(s.configuration[i].object_id==0&&s.configuration[i].category==0&&s.configuration[i].field==field)s.configuration[i].value=1;
        s.mode=8;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,r->protect_op);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
    } else if(status==LT48_OK && r->hy18 && op==LT48_OPERATION_SECURE){
        /* Only the original protection choice is requested, not arbitrary options. */
        s.mode=8;status=init(&s,decide,decision_context);
        for(size_t i=0;i<s.configuration_count;i++)if(s.configuration[i].object_id==0&&s.configuration[i].category==1&&s.configuration[i].field==1)s.configuration[i].value=1;
        if(status==LT48_OK)status=run(&s,7);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
    } else if(status==LT48_OK && r->hy25 && op==LT48_OPERATION_SECURE){
        s.mode=8;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,11);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
    } else if(status==LT48_OK && r->cypress55 && op==LT48_OPERATION_SECURE){
        s.mode=8;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,11);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
        /* Re-enter after power removal and require the original protected
         * status9 path. Never accept a no-effect Secure on a bare OK return. */
        if(status==LT48_OK)status=run(&s,1);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.wire);
        if(status==LT48_OK)status=run(&s,0);
        if(status==LT48_OK){s.mode=1;lt48_status check=init(&s,decide,decision_context);
            status=(check==LT48_RUNTIME_ERROR&&s.fault==9&&!(s.buffers[10][0]&0x20))?LT48_OK:check==LT48_OK?LT48_DEVICE_MISMATCH:check;}
    } else if(status==LT48_OK && r->cop48 && op==LT48_OPERATION_SECURE){
        s.mode=8;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,7);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
    } else if(status==LT48_OK && r->hc2 && op==LT48_OPERATION_ERASE) {
        s.mode=7;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,14);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,decide,decision_context,out);
        if(status==LT48_OK)for(size_t i=0;i<r->capacity;i++)if(pending[i]!=r->blank){if(!out->mismatch_count)out->first_mismatch=(uint32_t)i;out->mismatch_count++;}
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    } else if(status==LT48_OK && r->source_secure && (op==LT48_OPERATION_ERASE||op==LT48_OPERATION_SECURE)){
        s.mode=op==LT48_OPERATION_ERASE?7:8;status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,op==LT48_OPERATION_ERASE?6:7);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
        if(status==LT48_OK&&op==LT48_OPERATION_ERASE)status=cycle(&s,LT48_OPERATION_BLANK_CHECK,0,r->capacity,NULL,NULL,nominal,decide,decision_context,out);
    } else if(status==LT48_OK && r->at89 && (op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE)) {
        s.mode=op==LT48_OPERATION_ERASE?7:13;
        status=init(&s,decide,decision_context);
        if(status==LT48_OK)status=run(&s,op==LT48_OPERATION_ERASE?6:7);
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
        if(status==LT48_OK && op==LT48_OPERATION_ERASE)status=cycle(&s,LT48_OPERATION_BLANK_CHECK,0,r->capacity,NULL,NULL,nominal,decide,decision_context,out);
    } else if(status==LT48_OK) {
        if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={low,high};if((r->hy18||r->hy13)&&r->verify_count&&margin==LT48_VERIFY_MARGIN_ALL){volts[0]=r->verify_levels[0];volts[1]=r->verify_levels[1];}size_t n=(r->compact40||r->cop48)?r->verify_count:2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=high;n=1;}
            for(size_t i=0;status==LT48_OK && i<n;i++)status=cycle(&s,op,start,length,image,NULL,volts[i],decide,decision_context,out);
        } else status=cycle(&s,op,start,length,image,op==LT48_OPERATION_READ?pending:NULL,nominal,decide,decision_context,out);
    }
    if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM))
        for(unsigned i=0;status==LT48_OK && i<((r->compact40||r->cop48)?r->verify_count:2);i++)status=cycle(&s,LT48_OPERATION_VERIFY,start,length,image,NULL,(r->hy18||r->hy13)&&r->verify_count?r->verify_levels[i]:i?high:low,decide,decision_context,out);
    if(status==LT48_OK&&(r->zilog70==101||r->zilog70==102)&&(writing||op==LT48_OPERATION_VERIFY)){
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,decide,decision_context,out);
        if(status==LT48_OK&&pending[0]!=image[0]){out->mismatch_count++;out->first_mismatch=0;status=LT48_DEVICE_MISMATCH;}
    }
    if(status==LT48_OK&&r->sg4&&writing){unsigned v=0,l=0;status=sg4_read_options(&s,&v,&l);if(!status&&(v!=sg4_before||l)){out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}}
    if(status==LT48_OK&&r->el5&&writing){uint16_t after[2]={0};status=el5_read_options(&s,after);if(!status&&memcmp(after,el5_saved,sizeof after)){if(getenv("LT48_COMPACT_TRACE"))fprintf(stderr,"ELAN configuration changed %x/%x expected%x/%x\n",after[0],after[1],el5_saved[0],el5_saved[1]);out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}}
    if(status==LT48_OK&&r->sy4&&sy4_have){status=sy4_read_options(&s);if(!status&&sy4_options(&s)!=sy4_saved){out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}}
    /* Complete successful queued tail commands before destroying this session.
     * Some original Secure callbacks end after the pulse without a capture or
     * trailing status; destroying the plan would silently discard that pulse. */
    if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.wire);
    out->operation_status=status;
    /* A fresh uncancellable transport owns cleanup even after a failed queue.
     * Continue independent rail-off attempts and preserve the first error. */
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        lt48_status step=run(&s,1);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(step!=LT48_OK)out->cleanup_status=step;
        for(unsigned channel=0;channel<4;channel++) {
            unsigned char off[]={0x10,(unsigned char)(3-channel),0};
            /* Restart each phase after transport failure. */
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,off,sizeof(off));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK && step!=LT48_OK)out->cleanup_status=step;
        }
        if(r->parallel_fpga){
            static const unsigned char header[]={0x38,0xaa,0x55};
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof header);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        }
        static const unsigned char finish[]={0x49,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s.wire,finish,sizeof(finish));
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK)out->cleanup_status=step;
        out->cleanup_completed=out->cleanup_status==LT48_OK;
        if(out->cleanup_completed) {
            unsigned char led[]={0x40,status==LT48_OK?4:2};
            step=lt48_runtime_transport_session_controller_write(s.wire,led,sizeof(led));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(step!=LT48_OK)out->cleanup_status=step;
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK && (op==LT48_OPERATION_READ||((r->sg4||r->el5||r->sy4||r->sx6||r->zilog70==129||r->st62_cfg_mask||lt48_small_eprom_hyundai13_option_mask(r))&&op==LT48_OPERATION_CONFIGURATION&&read))){
        if(read_size<length)status=LT48_BUFFER_TOO_SMALL;else memcpy(read,pending,length);
    }
    lt48_runtime_transport_session_close(s.wire);
    if(status==LT48_OK && (r->nvram || r->eeprom) && writing) {
        /* Independent complete read after power removal proves EEPROM STORE
         * and power-on/explicit RECALL, not merely the SRAM write echo. */
        lt48_small_eprom_result fresh={0};
        status=lt48_small_eprom_execute(r,api,context,LT48_OPERATION_READ,0,r->capacity,
            NULL,0,pending,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,NULL,NULL,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(uint32_t i=0;i<r->capacity;i++)if(pending[i]!=expected_nvram[i]) {
            if(!out->mismatch_count)out->first_mismatch=i;
            out->mismatch_count++;status=LT48_DEVICE_MISMATCH;
        }
        out->operation_status=status;
        if(status!=LT48_OK) {
            lt48_runtime_transport_session *led=NULL;
            lt48_status step=lt48_runtime_transport_session_create(&cleanup,context,&led);
            static const unsigned char error[]={0x40,2};
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(led,error,sizeof(error));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(led);
            lt48_runtime_transport_session_close(led);
            if(out->cleanup_status==LT48_OK)out->cleanup_status=step;
        }
    }
    if(status==LT48_OK&&r->zilog70==129&&(op==LT48_OPERATION_SECURE||(op==LT48_OPERATION_CONFIGURATION&&r->zilog_config_mask))){
        lt48_small_eprom_resources check=*r;check.zilog_config_mask=check.zilog_config_values=0;
        lt48_small_eprom_result fresh={0};unsigned char actual[2];
        status=lt48_small_eprom_execute(&check,api,context,LT48_OPERATION_CONFIGURATION,0,2,NULL,0,actual,2,LT48_VERIFY_MARGIN_NOMINAL,NULL,NULL,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK&&memcmp(actual,pending,2)){out->mismatch_count++;out->first_mismatch=0;status=LT48_DEVICE_MISMATCH;}
        out->operation_status=status;
        if(status!=LT48_OK){lt48_runtime_transport_session *led=NULL;lt48_status step=lt48_runtime_transport_session_create(&cleanup,context,&led);static const unsigned char error[]={0x40,2};if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(led,error,sizeof error);if(step==LT48_OK)step=lt48_runtime_transport_session_flush(led);lt48_runtime_transport_session_close(led);if(out->cleanup_status==LT48_OK)out->cleanup_status=step;}
    }
    free(pending);free(expected_nvram);return status;
}

int lt48_small_eprom_is_zilog70(const lt48_small_eprom_resources*r){return r?r->zilog70:0;}

int lt48_small_eprom_is_compact40(const lt48_small_eprom_resources*r){return r?r->compact40:0;}

int lt48_small_eprom_is_cop48(const lt48_small_eprom_resources*r){return r?r->cop48:0;}

int lt48_small_eprom_is_holtek19(const lt48_small_eprom_resources*r){return r?r->holtek19:0;}

int lt48_small_eprom_is_z8tail(const lt48_small_eprom_resources*r){return r&&(r->zilog70==106||r->zilog70==107);}
lt48_status lt48_small_eprom_set_z8_ram_protection(lt48_small_eprom_resources*r,unsigned enable){
 if(!lt48_small_eprom_is_z8tail(r)||enable>1)return LT48_INVALID_ARGUMENT;
 r->z8_ram_protection=enable;return LT48_OK;
}
