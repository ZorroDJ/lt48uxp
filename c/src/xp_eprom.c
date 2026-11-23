#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../NewDeviceLib/sources/ptc-xp4/profiles.h"
#include "../../NewDeviceLib/sources/amic4/profiles.h"
#include "../../NewDeviceLib/sources/gdacotp6/profiles.h"
#include "../../NewDeviceLib/sources/ptc010three/profiles.h"
#include "../../NewDeviceLib/sources/sierra2/profiles.h"
#include "at89_classic.inc"
#include "../../NewDeviceLib/sources/w78eight/profiles.h"
#include "../../NewDeviceLib/sources/w78eight/admission.h"
#include "at89_parallel_extension.inc"
#include "at89_rb2.inc"
#include "../../NewDeviceLib/sources/w78legacy90/profiles.h"
#include "../../NewDeviceLib/sources/w78legacy90/admission.h"
#include "../../NewDeviceLib/sources/w78legacy90/erase.inc"
#include "../../NewDeviceLib/sources/w78e354one/member.h"
#include "../../NewDeviceLib/sources/w78e354one/admission.h"
#include "../../NewDeviceLib/sources/w78e354one/preservation.h"
#include "xp_mcu_w78.inc"
#include "xp_mcu_w78d.inc"
#include "xp_philips_basic.inc"
#include "xp_philips_x2.inc"
#include "xp_philips_ra2.inc"
#include "../../NewDeviceLib/sources/xp-philips82/profiles.inc"

/* Exact-source TXPMemGroup profiles: EPROM/FRAM, AT89 and W78C.
 * Shared transport does not admit other members of the Windows class. */
struct lt48_xp_eprom_resources {
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[12];
    lt48_wave_code applet[5];
    lt48_applet_address addresses[5];
    unsigned extension, applet_count, w78_fields;
    const struct philips82_tuple *extended;
    const struct w78legacy90_profile *legacy;
    const unsigned char *fpga;
    lt48_compile_variable vars[32];
    size_t var_count;
    uint32_t capacity;
    uint16_t man,dev,nominal;
    int w78e354one, fram, at89, electrical, w78eight, amic4, gdacotp6, ptc010three, sierra2;
    unsigned protection;
};
void lt48_xp_eprom_close(lt48_xp_eprom_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<12;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
int lt48_xp_eprom_is_fram(const lt48_xp_eprom_resources *r){return r?r->fram:0;}
int lt48_xp_eprom_is_at89(const lt48_xp_eprom_resources *r){return r?r->at89:0;}
unsigned lt48_xp_eprom_w78_fields(const lt48_xp_eprom_resources *r){return r && r->extension<5?r->w78_fields:0;}
int lt48_xp_eprom_is_philips(const lt48_xp_eprom_resources *r){return r && r->extension>=5;}
unsigned lt48_xp_eprom_flag_fields(const lt48_xp_eprom_resources *r){return r?r->w78_fields:0;}
unsigned lt48_xp_eprom_extended_fields(const lt48_xp_eprom_resources *r){return r && r->legacy?(r->legacy->seeds?r->legacy->flags+r->legacy->seeds:0):r && r->extended?r->extended->flags+r->extended->numbers:0;}
unsigned lt48_xp_eprom_extended_flags(const lt48_xp_eprom_resources *r){return r && r->legacy?r->legacy->flags:r && r->extended?r->extended->flags:0;}
const char *lt48_xp_eprom_extended_label(const lt48_xp_eprom_resources *r,unsigned i){
    if(r && r->legacy){static const char*n[]={"Lock Bit","MOVC Inhibit","Encryption Feature","Seed 0","Seed 1"};return i<5?n[i]:"Invalid field";}
    static const char *normal[]={"Status Byte","Boot Vector"};
    static const char *wide[]={"Status Byte","I2C slave address","Boot Vector 0","Boot Vector 1"};
    static const char *flags[]={"Lock Bit 1","Lock Bit 2","Lock Bit 3","Clock / CONFIG","Status Bit =1"};
    if(!r || !r->extended || i>=lt48_xp_eprom_extended_fields(r))return "";
    if(i<r->extended->flags){if(i==3)return r->extended->m==16?"CONFIG":r->extended->m==18?"6 Clock Enable.":"x12 Clk Cfg. En.";return flags[i];}
    return r->extended->numbers==4?wide[i-r->extended->flags]:r->extended->numbers==1?"Boot Vector":normal[i-r->extended->flags];
}
int lt48_xp_eprom_is_ra2(const lt48_xp_eprom_resources *r){return r && r->extension==7;}
int lt48_xp_eprom_is_rb2(const lt48_xp_eprom_resources *r){return r && r->extension==3;}
int lt48_xp_eprom_has_configuration(const lt48_xp_eprom_resources *r){return r && !r->w78e354one&&r->extension;}
lt48_status lt48_xp_eprom_set_protection(lt48_xp_eprom_resources *r,unsigned level) {
    if(r&&r->w78e354one)return level==1?LT48_OK:LT48_UNSUPPORTED;
    if(!r || !r->at89 || level<1 || level>(r->w78_fields && r->extension<5?1u:3u))return LT48_INVALID_ARGUMENT;
    r->protection=level;return LT48_OK;
}
uint32_t lt48_xp_eprom_capacity(const lt48_xp_eprom_resources *r){return r?r->capacity:0;}
lt48_status lt48_xp_eprom_open(const lt48_installation *x,size_t device,lt48_xp_eprom_resources **out) {
    static const struct {uint16_t d,m,p,bits,pins;uint32_t size;} approved[]={
        {0,0,0,8,28,65536},{1,1,1,8,32,524288},
        {0x1e,18,16,8,28,8192},{0x1f,19,17,8,28,16384},
        {0x20,21,18,8,28,16384},{0x21,21,18,5,28,16384},
        {0x22,20,18,8,28,32768},{0x23,21,18,8,28,32768},
        {0x24,21,18,5,28,32768},{0x25,22,19,8,32,131072},
        {0x26,23,19,8,32,131072},{0x27,24,20,8,32,131072},
        {0x30,33,25,8,28,32768},{0x31,34,25,8,28,32768},{0x32,35,25,8,28,32768}};
    lt48_device_spec spec;lt48_family_info family;lt48_pin_code pc;
    lt48_xp_eprom_resources *r=NULL;lt48_status status;char path[4096],name[24],hex[65];
    size_t index,fi;unsigned char digest[32];int admitted=0;
    if(!out||!x)return LT48_INVALID_ARGUMENT;
    *out=NULL;status=lt48_installation_resolve_device(x,device,&spec);if(status!=LT48_OK)return status;
    int at89=spec.catalogue.device_class==2038;unsigned extension=0,w78_fields=0;int w78e354=w78e354one_member(&spec.catalogue);
    if(w78e354){if(spec.definition_resource!=0x37||spec.family_code!=0x1b||spec.pinmap!=10||spec.pin_count!=48||spec.capacity_bytes!=16384||spec.element_bits!=8||spec.secondary_element_count)return LT48_UNSUPPORTED;admitted=1;extension=4;w78_fields=4;}
    const struct philips82_tuple *extended=NULL;
    const struct w78legacy90_profile *legacy=NULL;
    if(spec.catalogue.device_class!=2034 && !at89)return LT48_UNSUPPORTED;
    if(at89)for(size_t i=0;i<sizeof(at89_tuples)/sizeof(*at89_tuples);i++) {
        const struct at89_tuple *t=&at89_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0)admitted=1;
    }
    if(at89)for(size_t i=0;i<sizeof(at89_extension_tuples)/sizeof(*at89_extension_tuples);i++) {
        const struct at89_extension_tuple *t=&at89_extension_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=t->variant;}
    }
    if(at89)for(size_t i=0;i<sizeof(at89_rb2_tuples)/sizeof(*at89_rb2_tuples);i++) {
        if(spec.definition_resource==at89_rb2_tuples[i].d && spec.family_code==at89_rb2_tuples[i].m &&
           spec.pinmap==at89_rb2_tuples[i].p && spec.capacity_bytes==at89_rb2_tuples[i].capacity &&
           spec.pin_count==(at89_rb2_tuples[i].m==58?40u:44u) &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=3;}
    }
    if(at89)for(size_t i=0;i<sizeof(w78_tuples)/sizeof(*w78_tuples);i++) {
        const struct w78_tuple *t=&w78_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=4;w78_fields=t->fields;}
    }
    if(at89)for(size_t i=0;i<sizeof(w78d_tuples)/sizeof(*w78d_tuples);i++) {
        const struct w78d_tuple *t=&w78d_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=4;w78_fields=t->fields;}
    }
    if(at89)for(size_t i=0;i<sizeof(philips_basic_tuples)/sizeof(*philips_basic_tuples);i++) {
        const struct philips_basic_tuple *t=&philips_basic_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=5;w78_fields=3;}
    }
    if(at89)for(size_t i=0;i<sizeof(philips_x2_tuples)/sizeof(*philips_x2_tuples);i++) {
        const struct philips_x2_tuple *t=&philips_x2_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=6;w78_fields=4;}
    }    if(at89)for(size_t i=0;i<sizeof(philips_ra2_tuples)/sizeof(*philips_ra2_tuples);i++) {
        const struct philips_ra2_tuple *t=&philips_ra2_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=7;}
    }
    if(at89)for(size_t i=0;i<sizeof(philips82_tuples)/sizeof(*philips82_tuples);i++) {
        const struct philips82_tuple *t=&philips82_tuples[i];
        if(spec.definition_resource==t->d && spec.family_code==t->m && spec.pinmap==t->p &&
           spec.capacity_bytes==t->capacity && spec.pin_count==t->pins &&
           spec.element_bits==8 && spec.secondary_element_count==0){admitted=1;extension=8;extended=t;}
    }
    for(size_t i=0;i<sizeof(approved)/sizeof(approved[0]);i++)
        if(!at89 && spec.definition_resource==approved[i].d && spec.family_code==approved[i].m &&
           spec.pinmap==approved[i].p && spec.capacity_bytes==approved[i].size &&
           spec.element_bits==approved[i].bits && spec.pin_count==approved[i].pins)admitted=1;
    if(at89)for(size_t i=0;i<sizeof w78legacy90_profiles/sizeof*w78legacy90_profiles;i++){
        const struct w78legacy90_profile*p=w78legacy90_profiles+i;
        if(spec.definition_resource!=p->definition)continue;
        unsigned found=0;for(size_t k=0;k<sizeof w78legacy90_members/sizeof*w78legacy90_members;k++){const char*n=w78legacy90_members[k].name;if(w78legacy90_members[k].definition==p->definition && w78legacy90_members[k].manufacturer==spec.catalogue.manufacturer_id && w78legacy90_members[k].device==spec.catalogue.device_id && !strcmp(w78legacy90_members[k].vendor,spec.catalogue.vendor) && strlen(n)==spec.catalogue.device_name_length && !memcmp(n,spec.catalogue.device_name,spec.catalogue.device_name_length))found=1;}
        if(!found||spec.family_code!=p->family||spec.pinmap!=p->pinmap||spec.capacity_bytes!=p->capacity||spec.pin_count!=p->pins||spec.element_bits!=8||spec.secondary_element_count)return LT48_SAFETY_REJECTED;
        admitted=1;extension=4;w78_fields=p->flags;legacy=p;
    }
    int w78eight=w78eight_binding(&spec);
    if(w78eight){if(spec.definition_resource!=0x72&&LT48_W78EIGHT_STAGE<2)return LT48_UNSUPPORTED;admitted=1;}
    const struct gdacotp6_profile*otp6=gdacotp6_binding(&spec);if(otp6)admitted=1;
    int amic=amic4_binding(&spec);if(amic)admitted=1;
    int sierra=sierra2_binding(&spec);if(sierra)admitted=1;
    int ptc010=ptc010three_binding(&spec);
    int electrical=ptc010||ptc_xp4_binding(&spec);
    if(electrical)admitted=1;
    if(!admitted)return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/%s",lt48_installation_path(x),at89?"GDAE___.rez":"GDAC___.rez");
    if(n<0||(size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
#define TRY(a) do{status=(a);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,at89?"9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36":"4abfb9024179d1cf38f17bc43ebfdaabaca4694b95e76e73edfdbbe7a0f46710")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->w78e354one=w78e354;r->sierra2=sierra;r->ptc010three=ptc010;r->gdacotp6=otp6!=NULL;r->amic4=amic;r->w78eight=w78eight;r->electrical=electrical;r->extended=extended;r->legacy=legacy;r->w78_fields=w78_fields;r->at89=at89;r->protection=1;r->extension=extension;r->applet_count=extension==2?5:4;
    r->fram=!at89 && spec.definition_resource>=0x30 && spec.definition_resource<=0x32;
    r->nominal=otp6?otp6->nominal:r->fram && spec.definition_resource!=0x31?330:500;
    r->capacity=spec.capacity_bytes;r->man=spec.catalogue.manufacturer_id;r->dev=spec.catalogue.device_id;
    snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(family.variable_count!=(sierra?3u:otp6?otp6->variables:amic?7u:at89?(w78e354?9u:w78eight?9u:legacy?11u:extended?extended->variables:extension>=6?11u:extension==5?8u:w78_fields?3u:extension==3?4u:extension==1?7u:6u):r->fram?1u:r->electrical?(ptc010?10u:9u):spec.definition_resource==0?10u:spec.definition_resource==1?9u:8u) || (at89?family.operation_count!=(w78e354?13u:w78eight?11u:legacy?13u:extension==2?14u:extension>=5?13u:w78_fields?12u:extension?13u:12u):(family.operation_count<9 || family.operation_count>10))){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    }
    if(r->fram && (r->vars[0].variable_id!=0 || r->vars[0].value!=r->nominal)){status=LT48_FORMAT_ERROR;goto fail;}
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(code.wave_type==0 && (op.operation_id<5 || (electrical && op.operation_id==10) || (at89 && (op.operation_id==10 || op.operation_id==11 || (extension && op.operation_id==6))))) {
            lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(code.data,code.data_size,&w));
            status=w78e354&&op.operation_id==11?w78e354one_preserve_compile(&code,r->pins,r->vars,r->var_count,&r->wave[11]):legacy && op.operation_id==10?legacy_erase_compile(&code,r->pins,r->vars,r->var_count,&r->wave[op.operation_id]):lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,
                LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);
            lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
        } else if(code.wave_type==6) {
            unsigned slot=op.operation_id==17?0:op.operation_id==19?1:op.operation_id==20?2:op.operation_id==21?3:extension==2 && op.operation_id==23?4:5;
            static const uint16_t addr[]={0x6000,0x7800,0x8000,0x7000,0x9000};
            if(slot>=r->applet_count || code.load_address!=addr[slot] || code.data_size==0 ||
               code.data_size>0x800 || code.length1!=code.data_size+4 || code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            r->applet[slot]=code;r->addresses[slot]=(lt48_applet_address){op.operation_id,code.load_address};
        } else if(code.wave_type==4 && op.operation_id==27 && code.data_size==2780 &&
                  code.data[0]==0xda && code.data[1]==0x0a)r->fpga=code.data+2;
        else {status=LT48_UNSUPPORTED;goto fail;}
    }
    for(unsigned i=0;i<4;i++)if(!r->wave[i]||!r->applet[i].data){status=LT48_FORMAT_ERROR;goto fail;}
    if(extension && (!r->wave[6] || (extension==2 && !r->applet[4].data))){status=LT48_FORMAT_ERROR;goto fail;}
    if(at89 && (!r->wave[10] || (!w78eight&&!r->wave[11]))){status=LT48_FORMAT_ERROR;goto fail;}
    if(electrical && !r->wave[10]){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->fpga){status=LT48_FORMAT_ERROR;goto fail;}
    if(w78e354){lt48_compact_wave*w=NULL;TRY(lt48_compact_decode(w78e354one_rommap,sizeof w78e354one_rommap,&w));status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[5]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_xp_eprom_close(r);return status;
#undef TRY
}
static lt48_status validate_w78legacy90(const lt48_xp_eprom_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
lt48_status lt48_xp_eprom_validate(const lt48_xp_eprom_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(r&&r->w78e354one&&(op<1||op>7))return LT48_UNSUPPORTED;
    if(r&&r->w78eight&&(op==LT48_OPERATION_SECURE||op==LT48_OPERATION_CONFIGURATION))return LT48_UNSUPPORTED;
    if(r && r->legacy)return validate_w78legacy90(r,op,start,length,image,size);
    if(!r||!length||start>=r->capacity||length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if((op<LT48_OPERATION_READ||op>(r->at89?LT48_OPERATION_SECURE:LT48_OPERATION_COMPARE)) && !((r->electrical||r->sierra2) && op==LT48_OPERATION_ERASE) && !(r->extension && op==LT48_OPERATION_CONFIGURATION))return LT48_UNSUPPORTED;
    /* Original AT89 DEVINIT mode 2 calls chip erase. Never accept a partial
     * image under a promise of preserving bytes outside the selected range. */
    if((r->at89 || (r->electrical && op==LT48_OPERATION_ERASE)) && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM ||
       op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION) &&
       (start!=0 || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE&&op!=LT48_OPERATION_CONFIGURATION&&(!image||size!=length))return LT48_INVALID_ARGUMENT;
    if(r->w78_fields && op==LT48_OPERATION_CONFIGURATION && size) {
        if(!image || size!=8 || image[0]>=(1u<<r->w78_fields))return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<r->w78_fields;i++)if((image[0]&(1u<<i)) && image[i+1]>1)return LT48_INVALID_ARGUMENT;
    }
    if(r->extended && op==LT48_OPERATION_CONFIGURATION && size) {
        unsigned fields=lt48_xp_eprom_extended_fields(r);
        if(!image || size!=9 || image[0]>=(1u<<fields))return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<r->extended->flags;i++)if((image[0]&(1u<<i)) && image[i+1]>1)return LT48_INVALID_ARGUMENT;
    }
    if(r->extension==7 && op==LT48_OPERATION_CONFIGURATION && size) {
        if(!image || size!=8 || image[0]>=16)return LT48_INVALID_ARGUMENT;
        if((image[0]&1) && image[1]>1)return LT48_INVALID_ARGUMENT;
        if((image[0]&2) && image[2]>3)return LT48_INVALID_ARGUMENT;
    }
    if(r->extension==3 && op==LT48_OPERATION_CONFIGURATION && size) {
        if(!image || size!=8 || image[0]>127)return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<4;i++)if((image[0]&(1u<<i)) && image[i+1]>(i==3?3:1))return LT48_INVALID_ARGUMENT;
    }
    /* Original 465e80/467570 use ceil(element_bits/8). The two five-bit
     * catalogue variants still transfer full bytes through the same applets. */
    return LT48_OK;
}
lt48_status lt48_xp_eprom_validate_verify_options(const lt48_xp_eprom_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned margin_type) {
    if(!r||margin_type>1)return LT48_INVALID_ARGUMENT;
    if(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&
       margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    if(r->gdacotp6&&r->nominal==330&&margin_type&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||(op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_NOMINAL)))return LT48_SAFETY_REJECTED;
    /* Only the 5% high margin is admitted for the 3.3 V FRAM definitions.
     * The original 10% calculation asks for 3.63 V, outside our 3.6 V ceiling. */
    if((r->fram||r->gdacotp6) && r->nominal==330 && margin_type &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||
        (op==LT48_OPERATION_VERIFY&&(margin==LT48_VERIFY_MARGIN_ALL||margin==LT48_VERIFY_MARGIN_HIGH))))
        return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
static uint16_t margin_voltage(const lt48_xp_eprom_resources *r,unsigned type,int high) {
    /* Exact outputs of original 00467420, including its x87 rounding at 3.3 V. */
    if(r->electrical)for(size_t i=0;i<r->var_count;i++)
        if(r->vars[i].kind==0 && r->vars[i].variable_id==(high?2u:1u))
            return (uint16_t)r->vars[i].value; /* Source slot order, even editor-disabled. */
    if(r->nominal==330)return type?(high?363:297):(high?346:314);
    return type?(high?550:450):(high?525:475);
}
typedef struct xp_session {
    const lt48_xp_eprom_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[22][32];uint32_t volts[4];
    lt48_compile_variable vars[32];uint32_t mode,protection,configuration[8];
} xp_session;
static lt48_status run(xp_session *s,unsigned op) {
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[21];size_t count=0;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    for(uint8_t i=0;i<(s->r->at89?22:13);i++)if(i!=7)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_nested_wave nested[12];size_t nested_count=0;
    for(unsigned i=0;i<12;i++)if(s->r->wave[i])nested[nested_count++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};
    static const unsigned categories[7]={0,0,0,1,2,2,2},fields[7]={0,1,2,0,0,1,2};
    lt48_runtime_configuration_value configuration[8]={{-1,1,0,s->protection}};
    static const unsigned ra_categories[4]={0,1,2,2},ra_fields[4]={0,0,0,1};
    size_t config_count=s->r->extended?lt48_xp_eprom_extended_fields(s->r):s->r->extension==7?4:s->r->w78_fields?s->r->w78_fields:s->r->extension==3?7:1;
    if(s->r->w78_fields)for(unsigned i=0;i<config_count;i++)configuration[i]=(lt48_runtime_configuration_value){-1,0,i,s->configuration[i]};
    if(s->r->legacy){config_count=s->r->legacy->flags+s->r->legacy->seeds;for(unsigned i=0;i<config_count;i++)configuration[i]=(lt48_runtime_configuration_value){-1,i<s->r->legacy->flags?0:2,i<s->r->legacy->flags?i:i-s->r->legacy->flags,s->configuration[i]};}
    if(s->r->extension==7)for(unsigned i=0;i<4;i++)configuration[i]=(lt48_runtime_configuration_value){-1,ra_categories[i],ra_fields[i],s->configuration[i]};
    if(s->r->extended)for(unsigned i=0;i<config_count;i++)configuration[i]=(lt48_runtime_configuration_value){-1,i<s->r->extended->flags?0:2,i<s->r->extended->flags?i:i-s->r->extended->flags,s->configuration[i]};
    if(!s->r->extended && config_count==7)for(unsigned i=0;i<7;i++)configuration[i]=(lt48_runtime_configuration_value){-1,categories[i],fields[i],i==3?s->protection:s->configuration[i]};
    if(s->r->at89){
        options.nested_waves=nested;options.nested_wave_count=nested_count;
        options.configuration=configuration;options.configuration_count=config_count;
    }
    options.host_staged_shutdown=s->r->w78e354one||s->r->legacy||s->r->w78eight||s->r->amic4||s->r->gdacotp6||s->r->sierra2?3:s->r->electrical;
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=7;
    options.address_buffer=0;options.parameter_buffer_id=2;options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));options.root_timeout_seconds=10;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->applet_count,&result);
    if(status==LT48_OK && s->r->at89) {
        const lt48_runtime_configuration_value *values;size_t n;
        status=lt48_runtime_get_configuration(result,&values,&n);
        if(status==LT48_OK && n==config_count) {
            if(n==1)s->protection=values[0].value;
            else {
                for(size_t i=0;i<n;i++) {
                    if(values[i].category!=(s->r->legacy?(i<s->r->legacy->flags?0u:2u):s->r->extended?(i<s->r->extended->flags?0u:2u):s->r->extension==7?ra_categories[i]:s->r->w78_fields?0:categories[i]) || values[i].field!=(s->r->legacy?(i<s->r->legacy->flags?i:i-s->r->legacy->flags):s->r->extended?(i<s->r->extended->flags?i:i-s->r->extended->flags):s->r->extension==7?ra_fields[i]:s->r->w78_fields?i:fields[i])){status=LT48_RUNTIME_ERROR;break;}
                    s->configuration[i]=values[i].value;
                }
                s->protection=s->configuration[s->r->extension==7?1:s->r->w78_fields?0:3];
                if(s->r->extended){s->protection=0;for(unsigned i=0;i<3;i++)if(s->configuration[i])s->protection=i+1;}
                if(s->r->w78_fields && s->r->extension>=5)for(unsigned i=0;i<3;i++)if(s->configuration[i])s->protection=i+1;
            }
        }
        else if(status==LT48_OK)status=LT48_RUNTIME_ERROR;
    }
    if(status==LT48_OK) {
        for(uint8_t i=0;i<(s->r->at89?22:13);i++) {
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
static lt48_status flag(xp_session *s,unsigned opcode,unsigned *value) {
    unsigned char p[2]={(unsigned char)opcode,1},reply[512]={0};
    lt48_status status=cmd(s,p,opcode==0x0b?1:2,reply);
    if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)*value=reply[2]&1;
    return status;
}
static lt48_status block(xp_session *s,unsigned opcode,unsigned char *data,size_t n,unsigned char *reply) {
    unsigned char p[6]={(unsigned char)opcode,(unsigned char)(n/512),(unsigned char)(n/512>>8),
        (unsigned char)(n%512),(unsigned char)(n%512>>8),opcode==0x48?1:0};
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
    const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,15},busy[]={0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,fpga,sizeof(fpga));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,s->r->fpga,2778);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,selector,sizeof(selector));
    if(status==LT48_OK)status=run(s,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->wire,1);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof(busy));
    return status;
}
static lt48_status cycle(xp_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_xp_eprom_result *out) {
    for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(start>>(8*j));
    s->mode=mode;for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status status=run(s,2);size_t chunk=length/100;if(!chunk)chunk=1;if(chunk>8192)chunk=8192;
    out->completed_bytes=0;
    for(size_t offset=0;status==LT48_OK&&offset<length;) {
        size_t n=length-offset;if(n>chunk)n=chunk;
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,mode==1?5000:mode==11?10000:60000,0);
        if(status!=LT48_OK)break;
        if(mode==1)status=block(s,0x43,read+offset,n,NULL);
        else if(mode==11) {
            unsigned char count[2]={(unsigned char)n,(unsigned char)(n>>8)};unsigned passed=0;
            status=block(s,0x44,count,2,NULL);
            if(status==LT48_OK)status=flag(s,0x0b,&passed);
            if(status==LT48_OK&&!passed) {
                unsigned char remaining[2]={0};status=block(s,0x43,remaining,2,NULL);
                size_t rem=remaining[0]|(size_t)remaining[1]<<8;
                if(status==LT48_OK) {
                    if(rem>=n)status=LT48_PROTOCOL_ERROR;
                    else {out->first_mismatch=start+(uint32_t)(offset+n-rem-1);out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}
                }
            }
        } else {
            unsigned char reply[512]={0};status=block(s,0x48,(unsigned char*)image+offset,n,reply);
            if(status==LT48_OK&&!(reply[0]|reply[1])) {
                /* The original derives an unchecked remaining count here.
                 * Never resume programming from untrusted failure counters. */
                out->mismatch_count++;out->first_mismatch=UINT32_MAX;if(s->r->amic4||s->r->gdacotp6||s->r->ptc010three||s->r->sierra2){unsigned char actual;lt48_status drain=block(s,0x43,&actual,1,NULL);status=drain?drain:LT48_DEVICE_MISMATCH;}else status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK&&offset+n==length) {
                unsigned passed=0;status=flag(s,0x0c,&passed);
                if(status==LT48_OK&&!passed){out->mismatch_count++;out->first_mismatch=start+(uint32_t)(length-1);if(s->r->amic4||s->r->gdacotp6||s->r->ptc010three||s->r->sierra2){unsigned char actual;lt48_status drain=block(s,0x43,&actual,1,NULL);status=drain?drain:LT48_DEVICE_MISMATCH;}else status=LT48_DEVICE_MISMATCH;}
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
    if(status==LT48_OK)status=run(s,3);
    return status;
}
/* FRAM validation must observe actual bytes, including the final byte. A
 * controller success bit alone was insufficient on a disconnected adapter. */
static lt48_status checked_cycle(xp_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *pending,uint16_t vcc,lt48_xp_eprom_result *out) {
    lt48_status status=s->r->sierra2?LT48_OK:cycle(s,mode,start,length,image,NULL,vcc,out);
    if(status==LT48_OK && (s->r->fram || s->r->at89 || s->r->electrical || s->r->gdacotp6 || s->r->sierra2)) {
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
    if(!s->r->wave[4] || (s->r->at89 && !s->r->man && !s->r->dev))return LT48_OK;
    for(unsigned attempt=0;attempt<64;attempt++) {
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
static lt48_status read_configuration(xp_session *s,unsigned *level) {
    s->mode=14;s->protection=UINT32_MAX;
    for(unsigned i=0;i<8;i++)s->configuration[i]=UINT32_MAX;
    lt48_status status=run(s,2);
    if(status==LT48_OK)status=run(s,6);
    if(status==LT48_OK && s->protection>3)status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK && s->r->w78_fields)for(unsigned i=0;i<s->r->w78_fields;i++)
        if(s->configuration[i]>1)status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK && s->r->extension==7)for(unsigned i=0;i<4;i++)
        if(s->configuration[i]>(i==0?1:i==1?3:255))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK && s->r->extension==3)for(unsigned i=0;i<7;i++)
        if(s->configuration[i]>(i<3?1:i==3?3:255))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK && s->r->extended)for(unsigned i=0;i<lt48_xp_eprom_extended_fields(s->r);i++)
        if(s->configuration[i]>(i<s->r->extended->flags?1u:255u))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)*level=s->protection;
    if(status==LT48_OK)status=run(s,3);
    return status;
}
static lt48_status write_rb2_configuration(xp_session *s,const uint32_t desired[8]) {
    s->mode=13;
    lt48_status status=run(s,2);
    memcpy(s->configuration,desired,sizeof(s->configuration));s->protection=desired[s->r->extension==7?1:s->r->w78_fields?0:3];
    if(s->r->extended){s->protection=0;for(unsigned i=0;i<3;i++)if(desired[i])s->protection=i+1;}
    if(status==LT48_OK)status=run(s,11);
    if(status==LT48_OK && s->buffers[2][0]!=1)status=LT48_DEVICE_MISMATCH;
    if(status==LT48_OK)status=run(s,3);
    unsigned level;
    if(status==LT48_OK)status=read_configuration(s,&level);
    if(status==LT48_OK && memcmp(s->configuration,desired,sizeof(s->configuration)))status=LT48_DEVICE_MISMATCH;
    return status;
}
#include "../../NewDeviceLib/sources/w78legacy90/owner.inc"
#include "../../NewDeviceLib/sources/w78e354one/owner.inc"
lt48_status lt48_xp_eprom_execute(const lt48_xp_eprom_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_xp_eprom_result *out) {
    if(r&&r->w78e354one)return execute_w78e354one(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(r && r->legacy)return execute_w78legacy90(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_xp_eprom_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if((op==LT48_OPERATION_READ&&(!read||read_size<length)) || (op==LT48_OPERATION_CONFIGURATION&&(!read||read_size<(r->extended?lt48_xp_eprom_extended_fields(r):r->extension==7?4:r->w78_fields?r->w78_fields:r->extension==3?7u:1u))))return LT48_BUFFER_TOO_SMALL;
    status=lt48_xp_eprom_validate_verify_options(r,op,margin,margin_type);
    if(status!=LT48_OK)return status;
    unsigned char *pending=malloc(length);if(!pending)return LT48_NO_MEMORY;
    xp_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;s.protection=r->protection;memcpy(s.vars,r->vars,sizeof(s.vars));
    for(unsigned j=0;j<4;j++)s.buffers[1][j]=(unsigned char)(r->capacity>>(j*8));
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(pending);return status;}
    status=setup(&s);
    if(status==LT48_OK)status=identify(&s,decide,decision_context);
    unsigned observed_protection=0;uint32_t saved_configuration[8]={0};
    if(status==LT48_OK && r->extension && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) {
        status=read_configuration(&s,&observed_protection);
        memcpy(saved_configuration,s.configuration,sizeof(saved_configuration));
        if(status==LT48_OK && r->extension>=6 && observed_protection && !(r->extended && r->extended->erase_before_program)) {
            /* X2/RA2 DEVINIT has no conditional erase. Use its explicit source
             * erase, then verify unlock/blank before entering programming. */
            s.mode=7;status=run(&s,2);
            if(status==LT48_OK)status=run(&s,10);
            if(status==LT48_OK && r->extension==7 && s.buffers[2][0]!=1)status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK)status=run(&s,3);
            if(status==LT48_OK)status=read_configuration(&s,&observed_protection);
            if(status==LT48_OK && observed_protection)status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK)status=cycle(&s,11,0,length,NULL,NULL,r->nominal,out);
        }
        if(status==LT48_OK && !observed_protection && (!r->w78_fields || r->extension>=5) && !(r->extended && r->extended->erase_before_program)) {
            status=cycle(&s,op==LT48_OPERATION_PROGRAM?11:1,start,length,NULL,pending,r->nominal,out);
            if(status==LT48_OK && op==LT48_OPERATION_OVER_PROGRAM)for(size_t i=0;i<length;i++)
                if((pending[i]&image[i])!=image[i]){out->first_mismatch=(uint32_t)i;out->mismatch_count++;status=LT48_DEVICE_MISMATCH;break;}
        }
    }
    if(status==LT48_OK&&!r->fram&&!r->sierra2&&(!r->at89||r->w78eight)&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) {
        status=cycle(&s,op==LT48_OPERATION_PROGRAM?11:1,start,length,NULL,pending,r->gdacotp6?r->nominal:500,out);
        if(status==LT48_OK&&r->w78eight&&op==LT48_OPERATION_PROGRAM){
            status=cycle(&s,1,start,length,NULL,pending,500,out);
            if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=255){out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;}
            if(status==LT48_OK&&out->mismatch_count)status=LT48_DEVICE_MISMATCH;
        }
        if(status==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM)
            for(size_t i=0;i<length;i++)if((pending[i]&image[i])!=image[i]) {
                out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;status=LT48_DEVICE_MISMATCH;break;
            }
    }
    if(status==LT48_OK && (r->extended || r->extension==7 || r->extension==3 || r->w78_fields) && op==LT48_OPERATION_SECURE) {
        status=read_configuration(&s,&observed_protection);
        uint32_t desired[8];memcpy(desired,s.configuration,sizeof(desired));
        if(r->extended){for(unsigned i=0;i<r->protection;i++)desired[i]=1;}
        else if(r->extension==7){if(desired[1]<r->protection)desired[1]=r->protection;}
        else if(r->extension>=5){for(unsigned i=0;i<r->protection;i++)desired[i]=1;}
        else if(r->w78_fields)desired[0]=1;
        else if(desired[3]<r->protection)desired[3]=r->protection;
        if(status==LT48_OK)status=write_rb2_configuration(&s,desired);
    } else if(status==LT48_OK && r->sierra2 && op==LT48_OPERATION_ERASE) {
        /* The source EEPROM self-erases each written byte. Clear through its
         * authenticated Program worker; there is no chip-erase opcode. */
        unsigned char*blank=malloc(length);if(!blank)status=LT48_NO_MEMORY;
        else {memset(blank,255,length);status=cycle(&s,2,start,length,blank,NULL,r->nominal,out);
            if(status==LT48_OK)status=checked_cycle(&s,4,start,length,blank,pending,r->nominal,out);
            free(blank);
        }
    } else if(status==LT48_OK && r->sierra2 && op==LT48_OPERATION_BLANK_CHECK) {
        status=cycle(&s,1,start,length,NULL,pending,r->nominal,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=255){if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;}
        if(status==LT48_OK&&out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    } else if(status==LT48_OK && r->electrical && op==LT48_OPERATION_ERASE) {
        s.mode=7;memset(s.buffers[0],0,32);memset(s.buffers[2],0,32);
        status=run(&s,2);
        if(status==LT48_OK)status=run(&s,10);
        if(status==LT48_OK && s.buffers[2][0]!=1)status=LT48_DEVICE_MISMATCH;
        if(status==LT48_OK)status=run(&s,3);
        /* The source pulse declares success without reading the array. Require
         * independent full-array readback, including the last byte. */
        if(status==LT48_OK)status=cycle(&s,1,0,length,NULL,pending,r->nominal,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=255){
            if(!out->mismatch_count)out->first_mismatch=(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    } else if(status==LT48_OK && r->at89 && (op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE)) {
        s.mode=op==LT48_OPERATION_ERASE?7:13;s.protection=r->protection;
        memset(s.buffers[0],0,32);
        status=run(&s,2);
        if(status==LT48_OK)status=run(&s,op==LT48_OPERATION_ERASE?10:11);
        if(status==LT48_OK && s.buffers[2][0]!=(op==LT48_OPERATION_ERASE?(r->extended?r->extended->erase_result:r->extension==6?255u:1u):1u))status=LT48_DEVICE_MISMATCH;
        if(status==LT48_OK)status=run(&s,3);
        if(status==LT48_OK && r->extension) {
            status=read_configuration(&s,&observed_protection);
            if(status==LT48_OK && (op==LT48_OPERATION_ERASE?observed_protection!=0:observed_protection<r->protection))status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK && op==LT48_OPERATION_ERASE && r->extension==6 && s.configuration[3])status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK && op==LT48_OPERATION_ERASE && r->extension==7)for(unsigned i=0;i<4;i++)if(s.configuration[i])status=LT48_DEVICE_MISMATCH;
        }
        if(status==LT48_OK && op==LT48_OPERATION_ERASE) {
            status=cycle(&s,1,0,length,NULL,pending,r->nominal,out);
            if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=255) {
                if(!out->mismatch_count)out->first_mismatch=(uint32_t)i;
                out->mismatch_count++;
            }
            if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
        }
    } else if(status==LT48_OK && op==LT48_OPERATION_CONFIGURATION) {
        status=read_configuration(&s,&observed_protection);
        if(status==LT48_OK && (r->extended || r->extension==7 || r->extension==3 || r->w78_fields) && size) {
            uint32_t desired[8];memcpy(desired,s.configuration,sizeof(desired));
            for(unsigned i=0;i<(r->extended?lt48_xp_eprom_extended_fields(r):7u);i++)if(image[0]&(1u<<i))desired[i]=image[i+1];
            if(r->extended){for(unsigned i=0;i<r->extended->flags;i++)if(desired[i]<s.configuration[i])status=LT48_SAFETY_REJECTED;}
            else if(r->w78_fields) {
                for(unsigned i=0;i<r->w78_fields;i++)if(desired[i]<s.configuration[i])status=LT48_SAFETY_REJECTED;
            } else if(r->extension==7){if(desired[0]<s.configuration[0] || desired[1]<observed_protection)status=LT48_SAFETY_REJECTED;}
            else if(desired[3]<observed_protection)status=LT48_SAFETY_REJECTED;
            if(status==LT48_OK && image[0])status=write_rb2_configuration(&s,desired);
        }
    } else if(status==LT48_OK) {
        if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};size_t n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW){volts[0]=volts[0]<volts[1]?volts[0]:volts[1];n=1;}
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[0]>volts[1]?volts[0]:volts[1];n=1;}
            for(size_t i=0;status==LT48_OK&&i<n;i++)status=checked_cycle(&s,4,start,length,image,pending,volts[i],out);
        } else {
            unsigned mode=op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_COMPARE?5:2;
            if(op==LT48_OPERATION_COMPARE)status=checked_cycle(&s,mode,start,length,image,pending,r->nominal,out);
            else status=cycle(&s,mode,start,length,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,out);
        }
    }
    if(status==LT48_OK&&r->w78eight&&op==LT48_OPERATION_BLANK_CHECK){
        status=cycle(&s,1,start,length,NULL,pending,r->nominal,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=255){out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;}
        if(status==LT48_OK&&out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))
        for(unsigned i=0;status==LT48_OK&&i<2;i++)status=checked_cycle(&s,4,start,length,image,pending,margin_voltage(r,margin_type,i),out);
    if(status==LT48_OK && r->w78_fields && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) {
        unsigned level;
        status=read_configuration(&s,&level);
        if(status==LT48_OK) {
            uint32_t desired[8];memcpy(desired,s.configuration,sizeof(desired));
            for(unsigned i=0;i<(r->w78_fields==4 || r->extension==5?3u:2u);i++)if(desired[i])status=LT48_DEVICE_MISMATCH;
            /* W78C oscillator flag; W78D CBS/noise/clock/crystal flags.
             * Source programming erases these with the array. Restore only
             * non-protection settings after code verification. */
            unsigned first=r->w78_fields==6?2u:3u;int changed=0;
            for(unsigned i=first;i<r->w78_fields;i++)if(desired[i]!=saved_configuration[i]) {
                desired[i]=saved_configuration[i];changed=1;
            }
            if(status==LT48_OK && changed)status=write_rb2_configuration(&s,desired);
        }
    }
    if(status==LT48_OK && (r->extended || r->extension==3 || r->extension==7) && (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) {
        /* Preserve boot settings across source auto-erase; leave protection off
         * after code programming. Secure remains a separate explicit action. */
        if(r->extended)for(unsigned i=0;i<3;i++)saved_configuration[i]=0;
        else saved_configuration[r->extension==7?1:3]=0;
        status=write_rb2_configuration(&s,saved_configuration);
    }
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
            if(step!=LT48_OK)out->cleanup_status=step;
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    if(status==LT48_OK && (r->fram||r->w78eight) &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||
        op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||(r->w78eight&&op==LT48_OPERATION_ERASE))) {
        /* Fresh power/setup/read after cleanup also rejects a floating bus
         * echoing the last written byte during same-session verification. */
        lt48_xp_eprom_result fresh;
        status=lt48_xp_eprom_execute(r,api,context,LT48_OPERATION_READ,start,length,
            NULL,0,pending,length,LT48_VERIFY_MARGIN_NOMINAL,0,r->w78eight?decide:NULL,r->w78eight?decision_context:NULL,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=(op==LT48_OPERATION_ERASE?255:image[i])) {
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
    if(status==LT48_OK && op==LT48_OPERATION_CONFIGURATION){
        out->completed_bytes=r->extended?lt48_xp_eprom_extended_fields(r):r->extension==7?4:r->w78_fields?r->w78_fields:r->extension==3?7:1;
        if(r->extended || r->w78_fields || r->extension==3 || r->extension==7)for(unsigned i=0;i<out->completed_bytes;i++)read[i]=(unsigned char)s.configuration[i];
        else read[0]=(unsigned char)observed_protection;
    }
    free(pending);return status;
}
