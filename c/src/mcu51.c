#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exact Dataman 13.10 GADA/GAF MCU closures. No family-name admission. */
struct lt48_mcu51_resources {
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[21];
    lt48_wave_code aduc812_applets[2];
    lt48_applet_address aduc812_addresses[2];
    size_t aduc812_count;
    unsigned mode_buffer,address_buffer,input_buffer,output_buffer,buffer_count;
    const struct legacy14_profile *l14;
    const struct lm20_profile *lm20;
    unsigned mcs48,blank,at89,upi,expected_manufacturer,legacy9,myson2,mysonM2;
    const struct mcu70_profile *cohort;
    const struct hy24_profile*hy24_profile;
    unsigned hy24,cl38,mcu70,temic,philips,otp20,flash51_41,flash51_57,nominal,program_end;
    unsigned init_op,reset_op,load_op,program_op,protect_op,encrypt_op,legacy,protect_max;
    const unsigned char *fpga;
    lt48_compile_variable vars[16];
    size_t var_count;
    uint32_t capacity, secondary;
};
#include "../../NewDeviceLib/sources/upi41ah/resources.inc"
#include "mcu51_near28.inc"
#include "../../NewDeviceLib/sources/mcu70/profiles.h"
#include "../../NewDeviceLib/sources/class2018_38/profiles.h"
#include "../../NewDeviceLib/sources/class2018_38/configuration.h"
#include "../../NewDeviceLib/sources/legacy51-9/profiles.h"
#include "../../NewDeviceLib/sources/myson2/profiles.h"
#include "../../NewDeviceLib/sources/myson44/profiles.h"
#include "../../NewDeviceLib/sources/mysonM2/profiles.h"
#include "../../NewDeviceLib/sources/legacy14/profiles.h"
#include "../../NewDeviceLib/sources/legacy14/admission.h"
#include "../../NewDeviceLib/sources/aduc812one/profile.h"
#include "../../NewDeviceLib/sources/legacy_mcu20/profiles.h"
#include "../../NewDeviceLib/sources/legacy_mcu20/admission.h"
static int cl38_mapped(const lt48_mcu51_resources*r,uint32_t a){return !(r&&r->cl38&&r->cohort->m==120&&a>=0x8800&&a<0xf800);}
static lt48_status cl38_validate(const lt48_mcu51_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
static lt48_status lm20_validate(const lt48_mcu51_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
static lt48_status legacy14_validate(const lt48_mcu51_resources*,lt48_operation_kind,uint32_t,size_t,const unsigned char*,size_t);
unsigned lt48_mcu51_legacy14_kind(const lt48_mcu51_resources*r){return r&&r->l14?r->l14->kind:0;}
unsigned lt48_mcu51_legacy14_admitted(const lt48_mcu51_resources*r){return r&&r->l14&&r->l14->stage<=LT48_LEGACY14_STAGE;}
#include "../../NewDeviceLib/sources/mcu70/initial-address.h"
#include "../../NewDeviceLib/sources/mcu70/t87c5101-protection.h"
#include "../../NewDeviceLib/sources/flash51_41/profiles.h"
#include "../../NewDeviceLib/sources/flash51_57/profiles.h"
#include "../../NewDeviceLib/sources/flash51_41/image_regions.h"
#include "../../NewDeviceLib/sources/pic-common/source_verify_voltage.h"
#include "../../NewDeviceLib/sources/temic26/profiles.h"
#include "../../NewDeviceLib/sources/hyundai24/profiles.h"
#include "../../NewDeviceLib/sources/hyundai24/counter_origin.h"
#include "../../NewDeviceLib/sources/hyundai24/second_lock.h"
#include "../../NewDeviceLib/sources/philips39/profiles.h"
#include "../../NewDeviceLib/sources/otp20/profiles.h"
void lt48_mcu51_close(lt48_mcu51_resources *r) {
    if (!r) return;
    for (size_t i=0;i<21;i++) lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins); lt48_rez_close(r->rez); free(r->idle); free(r);
}
uint32_t lt48_mcu51_capacity(const lt48_mcu51_resources *r) {
    return r ? r->capacity : 0;
}
unsigned lt48_mcu51_is_hy24_internal(const lt48_mcu51_resources*r){return r?r->hy24:0;}
unsigned lt48_mcu51_is_upi41ah(const lt48_mcu51_resources *r) { return r ? r->upi : 0; }
unsigned lt48_mcu51_philips_options(const lt48_mcu51_resources*r){return r?r->philips:0;}
unsigned lt48_mcu51_is_flash51_57(const lt48_mcu51_resources*r){return r?r->flash51_57:0;}
unsigned lt48_mcu51_is_flash51_41(const lt48_mcu51_resources*r){return r?r->flash51_41:0;}
unsigned lt48_mcu51_cl38(const lt48_mcu51_resources*r){return r?r->cl38:0;}
unsigned lt48_mcu51_cl38_family(const lt48_mcu51_resources*r){return r&&r->cl38?r->cohort->m:0;}
unsigned lt48_mcu51_cl38_admitted(const lt48_mcu51_resources*r){return r&&r->cl38&&LT48_CL38_STAGE>=(r->cohort->m>=128&&r->cohort->m<=135?1:2);}
unsigned lt48_mcu51_is_mcu70(const lt48_mcu51_resources*r){return r?r->mcu70:0;}
unsigned lt48_mcu51_is_otp20(const lt48_mcu51_resources*r){return r?r->otp20:0;}
unsigned lt48_mcu51_is_at89(const lt48_mcu51_resources *r) { return r ? r->at89 : 0; }
const char*lt48_mcu51_protection_choice_name(const lt48_mcu51_resources*r,unsigned choice){
 if(r&&r->lm20&&r->lm20->group==8&&choice>=1&&choice<=6){static const char*names[]={"Secure: original choice 1 (06h)","Secure: original choice 2 (0Ah)","Secure: original choice 3 (12h)","Secure: original choice 4 (22h)","Secure: original choice 5 (42h)","Secure: original choice 6 (82h)"};return names[choice-1];}
 if(r&&(r->hy24==116||r->hy24==117)&&choice>=1&&choice<=2)return choice==1?"Secure: program LB1":"Secure: program LB2";
 return NULL;
}
unsigned lt48_mcu51_security_levels(const lt48_mcu51_resources *r) { return r ? r->protect_max : 0; }
unsigned lt48_mcu51_configuration_bytes(const lt48_mcu51_resources*r){
 if(!r||!r->cohort||(r->cohort->kind!=6&&r->cohort->kind!=8&&r->cohort->kind!=11))return 0;
 return (r->cohort->m==8||r->cohort->m==66||r->cohort->m==67)?2:r->cohort->m==7?8:4;
}
uint32_t lt48_mcu51_configuration_address(const lt48_mcu51_resources*r){
 if(!lt48_mcu51_configuration_bytes(r))return 0;
 if(r->philips==8||r->philips==11)return 0;
 return (r->cohort->m==6||r->cohort->m==18||r->cohort->m==66)?0x80:0xfff8;
}
static int mcu70_issi(const lt48_mcu51_resources*r){return r && r->cohort && (r->cohort->m==99 || r->cohort->m==101 || r->cohort->m==102);}
static int mcu70_electrical(const lt48_mcu51_resources*r){
 return (r&&r->hy24==117)||(r&&r->cl38&&r->philips==7)||mcu70_issi(r)||(r&&r->cohort&&(r->cohort->m==73||r->cohort->m==126||r->cohort->m==127||r->cohort->m==103));
}
unsigned lt48_mcu51_id_can_ignore(const lt48_mcu51_resources*r){
 return r&&!(r->cohort&&(r->cohort->m==85||r->cohort->m==86||r->cohort->m==100));
}
lt48_status lt48_mcu51_open(const lt48_installation *installation,
    size_t device, lt48_mcu51_resources **out) {
    static const struct {uint16_t cls,d,m,p;uint32_t size,secondary;} approved[] = {
        {2013,0x91,0x4f,0x2b,1024,0},{2013,0x92,0x4f,0x2b,2048,0},{2013,0x93,0x4f,0x2c,2048,0},
        {2013,0x8d,0x4d,0x2b,1024,0},{2013,0x8e,0x4e,0x2b,2048,0},
        {2013,0x8f,0x4e,0x2b,1024,0},{2013,0x90,0x4e,0x2b,2048,0}, /* Intel 8749H: zero-erased MCS-48. */
        {2013,0x63,0x2e,24,10240,0},{2013,0x64,0x2f,24,10240,0},
        {2013,0x65,0x30,25,10240,0},{2013,0x66,0x31,25,10240,0},
        {2013,0x67,0x32,24,12288,0},{2013,0x68,0x33,24,12288,0},
        {2013,0x69,0x34,25,12288,0},{2013,0x6a,0x35,25,12288,0},
        {2013,0x0f,9,0,4096,0},{2013,0x11,9,2,4096,0},
        {2013,0x13,10,1,8192,0},{2013,0x15,10,3,8192,0},
        {2013,0x48,32,17,4096,0},
        {2013,0x49,32,17,8192,0},
        {2013,0x4a,32,17,8192,0},
        {2018,0x0,0,0,4096,32},
        {2018,0x1,0,0,8192,32},
        {2018,0x10,3,2,8192,64},
        {2018,0x11,3,2,16384,64},
        {2018,0x12,3,2,32768,64},
        {2018,0x2,0,0,4096,32},
        {2018,0x3,1,0,8192,0},
        {2018,0x4,0,0,16384,64},
        {2018,0x5,0,0,32768,64},
        {2018,0x6,0,1,4096,32},
        {2018,0x7,0,1,8192,32},
        {2018,0x8,0,1,4096,32},
        {2018,0x9,1,1,8192,0},
        {2018,0xa,0,1,16384,64},
        {2018,0xb,0,1,32768,64},
        {2018,0x27,9,0,4096,32},
        {2018,0x28,9,0,8192,32},
        {2018,0x29,9,0,8192,32},
        {2018,0x2a,9,0,16384,32},
        {2018,0x38,9,1,4096,32},
        {2018,0x39,9,1,8192,32},
        {2018,0x3a,9,1,8192,32},
        {2018,0x3b,9,1,16384,32},
        {2018,0x13,4,0,8192,64},
        {2018,0x14,4,0,16384,64},
        {2018,0x15,4,0,32768,64},
        {2018,0x16,4,1,8192,64},
        {2018,0x17,4,1,16384,64},
        {2018,0x18,4,1,32768,64},
        {2018,0x19,5,2,8192,64},
        {2018,0x1a,5,2,16384,64},
        {2018,0x1b,5,2,32768,64},
    };
    lt48_device_spec spec; lt48_family_info family; lt48_pin_code pins;
    lt48_mcu51_resources *r=NULL; lt48_status status;
    char path[4096],name[24],hex[65]; unsigned char digest[32];
    size_t index,fi; int admitted=0;unsigned profile_levels=0,expected_manufacturer=0x89;
    if (!out || !installation) return LT48_INVALID_ARGUMENT;
    *out=NULL;
    status=lt48_installation_resolve_device(installation,device,&spec);
    if (status!=LT48_OK) return status;
    int legacy=spec.catalogue.device_class==2013u;
    if (!legacy && spec.catalogue.device_class!=2018u) return LT48_UNSUPPORTED;
    for (size_t i=0;i<sizeof(approved)/sizeof(approved[0]);i++)
        if (spec.catalogue.device_class==approved[i].cls && spec.definition_resource==approved[i].d &&
            spec.family_code==approved[i].m && spec.pinmap==approved[i].p &&
            spec.capacity_bytes==approved[i].size && spec.secondary_element_count==approved[i].secondary)
            admitted=1;
    if (!legacy) for(size_t i=0;i<sizeof(mcu51_near28_profiles)/sizeof(mcu51_near28_profiles[0]);i++) {
        if(spec.definition_resource==mcu51_near28_profiles[i].d &&
           spec.family_code==mcu51_near28_profiles[i].m && spec.pinmap==mcu51_near28_profiles[i].p &&
           spec.capacity_bytes==mcu51_near28_profiles[i].size &&
           spec.secondary_element_count==mcu51_near28_profiles[i].secondary) {
            admitted=1;profile_levels=mcu51_near28_profiles[i].levels;
            expected_manufacturer=mcu51_near28_profiles[i].id;
        }
    }
    int temic=temic26_binding(&spec);
    if(temic){admitted=1;profile_levels=3;}
    unsigned philips=philips39_binding(&spec);if(philips){admitted=1;profile_levels=3;expected_manufacturer=0x15;}
    unsigned otp20=otp20_binding(&spec);if(otp20){admitted=1;philips=otp20-1;profile_levels=philips?3:2;expected_manufacturer=spec.family_code==72?0x89:0x15;}
    const struct flash51_41_profile*f57=flash51_57_binding(&spec);
    const struct flash51_41_profile*f41=flash51_41_binding(&spec);if(!f41)f41=f57;if(f41){if(strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;admitted=1;}
    const struct mcu70_profile*cl38=cl38_binding(&spec);
    const struct mcu70_profile*n70=mcu70_binding(&spec);if(cl38)n70=cl38;
    if(n70){if(strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;admitted=1;philips=cl38&&n70->kind==12?0:n70->kind;profile_levels=n70->levels;expected_manufacturer=n70->manufacturer;}
    const struct legacy51_9_profile*my2=myson2_binding(&spec);if(!my2)my2=myson44_binding(&spec);
    const struct legacy51_9_profile*myM=mysonM2_binding(&spec);
    const struct legacy51_9_profile*l9=legacy51_9_binding(&spec);if(!l9)l9=my2;if(!l9)l9=myM;
    if(l9){if(strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;admitted=1;profile_levels=my2?1:3;expected_manufacturer=l9->manufacturer;}
    const struct legacy14_profile*l14=aduc812one_binding(&spec);if(!l14)l14=legacy14_binding(&spec);if(l14){if(strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;admitted=1;}
    const struct lm20_profile*lm20=lm20_binding(&spec);if(lm20){if(strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;admitted=1;}
    const struct hy24_profile*hy24=hy24_binding(&spec);
    if(hy24){if(strcmp(lt48_installation_source_profile(installation),"Dataman 13.10"))return LT48_UNSUPPORTED;admitted=1;profile_levels=hy24->levels;expected_manufacturer=hy24->m==69?0x89:0xe0;}
    if (!admitted) return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/%s",lt48_installation_path(installation),legacy?"GADA___.rez":"GAF____.rez");
    if (n<0 || (size_t)n>=sizeof(path)) return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r)); if (!r) return LT48_NO_MEMORY;
#define TRY(x) do {status=(x);if(status!=LT48_OK) goto fail;} while(0)
    TRY(lt48_rez_open(path,&r->rez)); TRY(lt48_rez_sha256(r->rez,digest));
    for(size_t i=0;i<32;i++) snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,legacy?"aca05831121f0e30654601df133b298cec1d33e7ffb94626c5d62634e690c452":"126dcd7dd7252b5af78e151d34106f0c17b5e52575b9606a80b9f615f912c123")) {
        status=LT48_SAFETY_REJECTED;goto fail;
    }
    r->flash51_41=f41?f41->m:0;r->flash51_57=f57?f57->stage:0;r->nominal=500;
    r->cohort=n70;r->legacy9=l9!=NULL;r->myson2=my2!=NULL||myM!=NULL;r->mysonM2=myM?(spec.definition_resource==0xa7?2u:1u):0;r->l14=l14;r->lm20=lm20;
    r->cl38=cl38!=NULL;r->mcu70=n70&&!cl38?1:0;
    r->hy24=hy24?hy24->m:0;r->hy24_profile=hy24;r->temic=temic;r->philips=philips;r->otp20=otp20;
    r->expected_manufacturer=expected_manufacturer;
    r->at89=legacy ? ((spec.family_code>=0x2e && spec.family_code<=0x35)?2:(spec.family_code==9 || spec.family_code==10)?1:0):0;
    r->upi=legacy && spec.definition_resource>=0x91 && spec.definition_resource<=0x93;
    r->mcs48=legacy && spec.definition_resource>=0x8d && spec.definition_resource<=0x93; r->blank=r->mcs48?0:255;
    r->capacity=spec.capacity_bytes;r->secondary=spec.secondary_element_count;r->legacy=legacy;
    r->mode_buffer=legacy?12:4;r->address_buffer=legacy?4:0;
    r->input_buffer=legacy?5:1;r->output_buffer=legacy?7:2;
    uint16_t buffer_count=0;
    TRY(lt48_rez_find_resource(r->rez,"Kernel",&index));
    TRY(lt48_rez_get_kernel_buffer_count(r->rez,index,&buffer_count));
    if(buffer_count>70){status=LT48_FORMAT_ERROR;goto fail;}r->buffer_count=buffer_count;
    r->init_op=legacy?4:2;r->reset_op=legacy?5:3;r->load_op=legacy?6:4;
    r->program_op=legacy?7:6;r->protect_op=legacy?9:8;r->encrypt_op=7;
    r->protect_max=r->upi?1:r->mcs48?0:r->at89?3:legacy?1:((spec.family_code==0 || spec.family_code==1 || spec.family_code==3)?2:3);
    if(profile_levels||cl38)r->protect_max=profile_levels;
    if(myM)r->protect_max=0;
    if(f41)r->protect_max=f41->m==11||f41->m==12?2:f41->m>=14&&f41->m<=19?3:1;
    if(lm20){r->protect_max=lm20->secure;}
    if(l14){r->protect_max=l14->secure;r->nominal=l14->nominal;}
    if((!l14 && spec.element_bits!=8) || (r->secondary && spec.secondary_element_bits!=8)){status=LT48_UNSUPPORTED;goto fail;}
    snprintf(name,sizeof(name),"P%x",spec.pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pins));
    TRY(r->upi?upi_pins(&pins,&r->pins):lt48_pin_map_parse(&pins,&r->pins));
    if(f41)TRY(lt48_pin_map_complete_groups_internal(r->pins,15));
    snprintf(name,sizeof(name),"M%x",spec.family_code);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(family.operation_count!=(hy24?hy24->operations:lm20?lm20->operations:l14?l14->operations:n70?n70->operations:r->flash51_41?((r->flash51_41>=16&&r->flash51_41<=19)?10u:9u):r->philips?(r->philips>=3 && r->philips!=5?11u:10u):r->mcs48?7u:r->at89==2?11u:r->at89?9u:legacy?8u:9u) || family.variable_count>16) {status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    }
    if(mcu70_issi(r)||myM)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==0)r->nominal=r->vars[i].value;
    if(f41)for(size_t i=0;i<r->var_count;i++){if(r->vars[i].variable_id==0)r->nominal=r->vars[i].value;if(r->vars[i].variable_id==44)r->program_end=(uint32_t)r->vars[i].value|((uint32_t)r->vars[i].channel<<16);}
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;lt48_compact_wave *w=NULL;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof(name),"W%x",op.wave);
        TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(code.wave_type==4 && op.operation_id==(legacy?10u:21u) && code.data_size==2780 &&
           code.data[0]==0xda && code.data[1]==0x0a) {r->fpga=code.data+2;continue;}
        if(l14&&l14->kind==10&&code.wave_type==6){
            unsigned slot=op.operation_id==12?0:op.operation_id==13?1:2;
            if(slot>=2||!code.has_applet_header||code.load_address!=(slot?0x5100:0x5135)||code.data_size!=(slot?53u:47u)||r->aduc812_applets[slot].data){status=LT48_SAFETY_REJECTED;goto fail;}
            r->aduc812_applets[slot]=code;r->aduc812_addresses[slot]=(lt48_applet_address){op.operation_id,code.load_address};r->aduc812_count++;continue;
        }
        if(op.operation_id>=21 || r->wave[op.operation_id] || code.wave_type!=0) {status=LT48_UNSUPPORTED;goto fail;}
        if(n70&&n70->m==74&&op.operation_id==8){
            code.data=mcu70_5101_protection;code.data_size=sizeof mcu70_5101_protection;
        }
        TRY(lt48_compact_decode(code.data,code.data_size,&w));
        int bank_noop=n70 && n70->address_cache && spec.pinmap==40 && op.operation_id==3;
        status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1u,
            bank_noop?LT48_PIN_FALLBACK_RESIDENT_NOOP:LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);
        lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
        size_t defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
        if(defaults!=(bank_noop?1u:0u)){status=LT48_SAFETY_REJECTED;goto fail;}
        if(bank_noop){lt48_defaulted_logical_pin missing;
            TRY(lt48_compiled_wave_get_defaulted_pin(r->wave[op.operation_id],0,&missing));
            if(missing.logical_pin!=12 || missing.compact_offset!=5 || missing.compact_opcode!=0x11 ||
               missing.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP){status=LT48_SAFETY_REJECTED;goto fail;}}
    }
    if(n70&&!cl38&&(r->philips==7||r->philips==8||r->philips==11)&&mcu70_electrical(r)){
        lt48_compact_wave*w=NULL;TRY(lt48_compact_decode(mcu70_initial_address,sizeof mcu70_initial_address,&w));
        status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[18]);
        lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
    }
    if(f41&&(!r->wave[8]||((r->flash51_41>=16&&r->flash51_41<=19)&&!r->wave[18]))){status=LT48_FORMAT_ERROR;goto fail;}
    if(r->at89==2 && (!r->wave[17] || !r->wave[18])){status=LT48_FORMAT_ERROR;goto fail;}
    if((!r->fpga && !(lm20&&lm20->group==7) && !(l14&&(l14->kind>=5)) && !(n70&&(n70->m==138||n70->m==139||n70->m==103))) || !r->wave[0] || !r->wave[1] || !r->wave[r->init_op] ||
       !r->wave[r->reset_op] || !r->wave[r->load_op] || !r->wave[r->program_op] ||
       (!r->mcs48 && !myM && !(lm20&&!lm20->secure) && !(l14&&!l14->secure) && !(cl38&&!r->protect_max) && !r->wave[r->protect_op]) || (!legacy && (!hy24 || r->secondary) && (!n70 || (r->secondary&&r->philips!=8&&r->philips!=11)) && !r->wave[r->encrypt_op])) {status=LT48_FORMAT_ERROR;goto fail;}
    if(r->hy24==114||r->hy24==115){
        lt48_compact_wave*w=NULL;TRY(lt48_compact_decode(hy24_counter_origin,sizeof hy24_counter_origin,&w));
        status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[18]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
    }
    if(r->hy24==118){lt48_compact_wave*w=NULL;TRY(lt48_compact_decode(hy24_second_lock,sizeof hy24_second_lock,&w));status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[19]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;}
    if(r->philips && ((r->philips!=5 && r->philips!=6 && r->philips!=7 && r->philips!=8 && r->philips!=9 && r->philips!=10 && r->philips!=11 && !(n70&&n70->m==74) && !r->wave[15]) || (r->philips>=3 && !(n70 && n70->m==140) && !r->wave[12]))){status=LT48_FORMAT_ERROR;goto fail;}
    if(l14&&l14->kind==10&&r->aduc812_count!=2){status=LT48_FORMAT_ERROR;goto fail;}
    if(r->upi)TRY(upi_resources(r));
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(installation),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_mcu51_close(r);return status;
#undef TRY
}
/* Source holes are non-addressable. Normalize file placeholders to FF;
 * never treat a source self-copy of DataOut as physical readback. F54/F58's
 * FFFF security byte is excluded from writable code images. */
static int flash51_mapped(const lt48_mcu51_resources*r,uint32_t a){return lt48_flash51_address_mapped(r->flash51_41,r->program_end,a);}
static void mcu51_verify_levels(const lt48_mcu51_resources*r,unsigned margin,uint16_t levels[2]){
 levels[0]=margin?450:475;levels[1]=margin?550:525;
 if(r->flash51_41||r->mysonM2||mcu70_issi(r)){uint32_t v[4];unsigned n=0;if(lt48_pic_source_verify_voltages(r->nominal,NULL,0,margin,0,v,&n)==LT48_OK&&n==2){levels[0]=(uint16_t)v[0];levels[1]=(uint16_t)v[1];}}
}
lt48_status lt48_mcu51_validate(const lt48_mcu51_resources *r,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(r&&r->cl38&&r->cohort->kind==12)return cl38_validate(r,op,start,length,image,size);
    if(r&&r->lm20)return lm20_validate(r,op,start,length,image,size);
    if(r&&r->l14)return legacy14_validate(r,op,start,length,image,size);
    if(!r || !length || start>=r->capacity || length>r->capacity-start) return LT48_SAFETY_REJECTED;
    if(r->mysonM2&&op==LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if(r->mysonM2&&((r->mysonM2==2&&op>=1&&op<=7)||op==LT48_OPERATION_ERASE)&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(mcu70_electrical(r) && op==LT48_OPERATION_ERASE && (start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(((r->at89==1 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) || (r->at89 && op==LT48_OPERATION_ERASE)) && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->flash51_41>=16&&r->flash51_41<=19&&op==LT48_OPERATION_CONFIGURATION)return !image&&!size?LT48_OK:LT48_UNSUPPORTED;
    unsigned config_bytes=lt48_mcu51_configuration_bytes(r);
    if(config_bytes && r->philips!=8 && op==LT48_OPERATION_CONFIGURATION){
        if(!size)return image?LT48_INVALID_ARGUMENT:LT48_OK;
        if(!image || size!=3+config_bytes || image[0]>1 ||
          (image[1]!=0&&image[1]!=1&&image[1]!=3&&image[1]!=7) ||
          (image[2]&~((1u<<config_bytes)-1)) || (!image[0]&&!image[2]))return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(r->cohort&&r->cohort->m==74&&op==LT48_OPERATION_CONFIGURATION&&size){
        if(image&&size==3&&(image[0]&2))return LT48_UNSUPPORTED;
        if(image&&size==3&&image[1]>3)return LT48_INVALID_ARGUMENT;
    }
    if((r->philips==9||r->philips==10)&&op==LT48_OPERATION_CONFIGURATION)return !image&&!size?LT48_OK:LT48_UNSUPPORTED;
    if(r->philips==9&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(r->philips==8&&op==LT48_OPERATION_CONFIGURATION){
        if(!size)return image?LT48_INVALID_ARGUMENT:LT48_OK;
        return image&&size==7&&!image[0]&&!image[1]&&(image[2]&~15u)==0&&image[2]?LT48_OK:LT48_INVALID_ARGUMENT;
    }
    if(r->philips==7&&op==LT48_OPERATION_CONFIGURATION){
        if(!size)return image?LT48_INVALID_ARGUMENT:LT48_OK;
        return image&&size==3&&image[0]==2&&image[2]==1?LT48_OK:LT48_UNSUPPORTED;
    }
    if(r->philips>=2 && op==LT48_OPERATION_CONFIGURATION) {
        if(!size)return image?LT48_INVALID_ARGUMENT:r->philips>=3?LT48_OK:LT48_UNSUPPORTED;
        if(!image || size!=3 || !image[0] || (image[0]&~3u) ||
           ((image[0]&1) && (r->philips==4?image[1]>7:(image[1]!=0 && image[1]!=1 && image[1]!=3 && image[1]!=7))) ||
           ((image[0]&2) && image[2]>1))return LT48_INVALID_ARGUMENT;
        if(r->philips==2 && (image[0]!=2 || image[2]!=1))return LT48_UNSUPPORTED;
        if(r->philips==5 && image[0]!=1)return LT48_UNSUPPORTED;
        return LT48_OK;
    }
    if(r->upi && op==LT48_OPERATION_CONFIGURATION) {
        if(!size)return image?LT48_INVALID_ARGUMENT:LT48_OK;
        return image && size==3 && image[0] && !(image[0]&~3u)?LT48_OK:LT48_INVALID_ARGUMENT;
    }
    if(r->at89==2 && op==LT48_OPERATION_CONFIGURATION) {
        if(!size)return image?LT48_INVALID_ARGUMENT:LT48_OK;
        if(!image || size!=3 || !image[0] || (image[0]&~3u) ||
           ((image[0]&1) && image[1]!=0 && image[1]!=1 && image[1]!=3 && image[1]!=7) ||
           ((image[0]&2) && image[2]>1))return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(r->flash51_41){
        if((op==LT48_OPERATION_ERASE||r->flash51_41==18||r->flash51_41==19)&&(start||length!=r->capacity))return LT48_SAFETY_REJECTED;
        if(image&&size!=length)return LT48_INVALID_ARGUMENT;
        if(image&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE))
            for(size_t i=0;i<size;i++)if(!flash51_mapped(r,start+(uint32_t)i)&&image[i]!=255)return LT48_SAFETY_REJECTED;
    }
    if(r->mcs48 && !r->upi && op==LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if((op<LT48_OPERATION_READ || op>LT48_OPERATION_COMPARE) && op!=LT48_OPERATION_SECURE && !((r->at89||r->flash51_41||r->mysonM2||mcu70_electrical(r)) && op==LT48_OPERATION_ERASE)) return LT48_UNSUPPORTED;
    if(op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_SECURE && op!=LT48_OPERATION_ERASE && (!image || size!=length)) return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
typedef struct mcu_session {
    const lt48_mcu51_resources *r;
    lt48_runtime_transport_session *wire;
    const lt48_protocol_transport *api;
    void *context;
    unsigned char buffers[70][32];
    uint32_t volts[4];
    lt48_compile_variable vars[16];
    uint32_t mode;
    lt48_runtime_configuration_value clcfg[16];size_t clcount;
    unsigned fault, ignore_id, protection, configuration[4],source_flags,id_expected;
    unsigned char configuration_bytes[8];
    lt48_runtime_configuration_value l14cfg[40];size_t l14count;
    unsigned char*l14image;size_t l14image_size;
    unsigned lm20_captures;
} mcu_session;
static lt48_status run(mcu_session *s,unsigned op) {
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[70];size_t count=0;
    lt48_runtime_nested_wave nested[21];size_t nested_count=0;
    for (uint16_t i=0;i<21;i++)if(s->r->wave[i]) nested[nested_count++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};
    options.nested_waves=nested;options.nested_wave_count=nested_count;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    s->fault=0;s->lm20_captures=0;
    for(uint8_t i=0;i<s->r->buffer_count;i++) if(i!=s->r->mode_buffer) seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_configuration_value configuration[]={
        {-1,0,1,!s->ignore_id}, {0,1,0,s->protection}, {0,1,1,s->protection},
        {0,0,22,s->configuration[0]},{0,0,23,s->configuration[1]},
        {0,0,24,s->configuration[2]},{0,0,28,s->configuration[3]},
        {0,0,0,s->r->philips==4?s->configuration[0]:s->protection>=1},{0,0,1,s->r->philips==4?s->configuration[1]:s->protection>=2},{0,0,2,s->r->philips==4?s->configuration[2]:s->protection>=3},
        {0,0,16,s->configuration[3]}};
    lt48_runtime_configuration_value fcfg[97];
    if(s->r->flash51_41){
        size_t n=0;fcfg[n++]=(lt48_runtime_configuration_value){-1,0,1,!s->ignore_id};
        for(unsigned c=0;c<3;c++)for(unsigned i=0;i<32;i++){unsigned value=0;
            if(c==1&&i==4)value=s->protection;
            if(c==0)value=(s->source_flags>>i)&1;
            if(c==0&&i>=22&&i<=24&&op==s->r->protect_op)value|=s->protection>=i-21;
            fcfg[n++]=(lt48_runtime_configuration_value){0,c,i,value};}
        options.configuration=fcfg;options.configuration_count=n;
    } else { options.configuration=configuration;options.configuration_count=s->r->philips?11:s->r->temic?10:s->r->at89==2?7:3;}
    lt48_runtime_configuration_value hycfg[]={{-1,0,1,!s->ignore_id},{0,1,s->r->hy24_profile?s->r->hy24_profile->lock_field:0,s->protection}};
    if(s->r->hy24){options.configuration=hycfg;options.configuration_count=2;}
    if(s->r->legacy9 || (s->r->cohort && s->r->cohort->over_mode==2))options.host_staged_shutdown=3;
    lt48_runtime_configuration_value n70cfg[]={ {-1,0,1,!s->ignore_id},{0,1,s->r->cohort?s->r->cohort->lock_field:0,s->protection} };
    if(s->r->cohort && !s->r->cohort->kind){options.configuration=n70cfg;options.configuration_count=2;}
    lt48_runtime_configuration_value bytecfg[14];
    if(s->r->philips==6||s->r->philips==8||s->r->philips==11){
        int reading=op==12||(op==s->r->init_op&&s->mode==1);
        bytecfg[0]=(lt48_runtime_configuration_value){-1,0,1,!s->ignore_id};
        bytecfg[1]=(lt48_runtime_configuration_value){0,1,s->r->cohort->lock_field,reading?0:s->protection};
        unsigned n=lt48_mcu51_configuration_bytes(s->r);
        for(unsigned i=0;i<n;i++)bytecfg[i+2]=(lt48_runtime_configuration_value){0,2,i+(s->r->philips==8?8u:0u),s->configuration_bytes[i]};
        options.configuration=bytecfg;options.configuration_count=n+2;
        if(s->r->philips==8)for(unsigned i=0;i<3;i++)bytecfg[options.configuration_count++]=(lt48_runtime_configuration_value){0,0,i,(reading||op!=8)?s->configuration[i]:s->protection>=i+1};
    }
    lt48_runtime_configuration_value dallas_cfg[]={
        {-1,0,1,!s->ignore_id},{0,1,0,s->protection},{0,1,2,s->configuration[3]}};
    if(s->r->philips==7){options.configuration=dallas_cfg;options.configuration_count=3;}
    lt48_runtime_configuration_value serial_cfg[]={{-1,0,1,!s->ignore_id},{0,0,12,s->protection>0}};
    if(s->r->philips==9){options.configuration=serial_cfg;options.configuration_count=2;}
    lt48_runtime_configuration_value dual_cfg[6]={{-1,0,1,!s->ignore_id},{0,1,0,(op==12||(op==2&&s->mode==1))?0:s->protection}};
    if(s->r->philips==10){for(unsigned i=0;i<4;i++)dual_cfg[2+i]=(lt48_runtime_configuration_value){0,0,6+i,!(s->configuration_bytes[0]&(1u<<(4-i)))};options.configuration=dual_cfg;options.configuration_count=6;}
    if(s->r->l14||s->r->lm20){options.configuration=s->l14cfg;options.configuration_count=s->l14count;options.host_staged_shutdown=3;
        if(s->r->l14&&s->r->l14->kind==5){options.use_explicit_direct_configuration_object=1;options.direct_configuration_object=0;}
        if(op==3&&s->r->l14&&s->r->l14->kind==1){options.device_image=s->l14image;options.device_image_size=s->l14image_size;options.device_element_bytes=2;}
        if(s->r->lm20&&s->r->lm20->group==8&&s->mode==2){options.device_image=s->l14image;options.device_image_size=s->l14image_size;options.device_element_bytes=1;}}
    if(s->r->cl38&&s->r->cohort->kind==12){
        if(!s->clcount){unsigned nf=0;const struct cl38_field*f=cl38_schema(s->r->cohort->m,&nf);s->clcfg[s->clcount++]=(lt48_runtime_configuration_value){-1,0,1,!s->ignore_id};for(unsigned i=0;i<nf;i++)s->clcfg[s->clcount++]=(lt48_runtime_configuration_value){0,f[i].category,f[i].field,0};}
        s->clcfg[0].value=!s->ignore_id;options.configuration=s->clcfg;options.configuration_count=s->clcount;options.host_staged_shutdown=3;
    }
    options.mode=s->mode;options.use_explicit_buffer_ids=1;
    options.mode_buffer=s->r->mode_buffer;options.address_buffer=s->r->address_buffer;options.parameter_buffer_id=s->r->input_buffer;
    options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));
    options.root_timeout_seconds=s->r->lm20&&s->r->lm20->group==7?60:s->r->lm20&&s->r->lm20->group==8?150:10;
    if(s->r->lm20&&s->r->lm20->group==8)options.max_steps=(size_t)s->r->capacity*1024u+65536u;
    if(s->r->cl38&&s->r->cohort->kind==12)options.max_steps=(size_t)s->r->capacity*8192u+65536u;
    lt48_status status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,
        s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK && (mcu70_issi(s->r)||(s->r->cohort&&s->r->cohort->m==103)) && op==10)
        status=lt48_runtime_machine_pin_resume_internal(machine);
    if(status==LT48_OK&&((s->r->cl38&&s->r->cohort->kind==12)||(s->r->lm20&&s->r->lm20->group==8)))status=lt48_runtime_machine_stream_resume_internal(machine);
    if(status==LT48_OK)status=s->r->aduc812_count?lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->aduc812_addresses,s->r->aduc812_count,&result):lt48_runtime_transport_session_run(s->wire,machine,&result);
    if(status==LT48_OK&&s->r->flash51_41>=16&&s->r->flash51_41<=19&&(op==18||(op==4&&s->mode==1))){
        const lt48_runtime_configuration_value*values;size_t n;status=lt48_runtime_get_configuration(result,&values,&n);
        for(size_t i=0;status==LT48_OK&&i<n;i++)if(values[i].object_id==0&&values[i].category==0){unsigned field=values[i].field;if(field==22||field==23||field==24||field==29||field==30||field==31){if(values[i].value>1)status=LT48_RUNTIME_ERROR;else s->source_flags=(s->source_flags&~(1u<<field))|(values[i].value<<field);}}
    }
    /* Only W31 (directly or through mode-1 INIT) reads settings. Other
     * routines merely receive requested configuration inputs; those values
     * must not overwrite the last independently read device state. */
    if(status==LT48_OK && s->r->philips>=3 &&
       (op==12 || (op==s->r->init_op && s->mode==1))) {
        const lt48_runtime_configuration_value *values;size_t n;
        status=lt48_runtime_get_configuration(result,&values,&n);
        for(size_t i=0;status==LT48_OK && i<n;i++) {
            if(s->r->philips==9&&values[i].object_id==0&&values[i].category==0&&values[i].field==12){if(values[i].value>1)status=LT48_RUNTIME_ERROR;else s->configuration[0]=values[i].value;}
            if(s->r->philips!=4 && s->r->philips!=8 && s->r->philips!=9 && (s->r->philips!=7||mcu70_electrical(s->r)) && values[i].object_id==0 && values[i].category==1 && values[i].field==((s->r->philips==6||s->r->philips==11)?s->r->cohort->lock_field:0)){
                if(values[i].value>3)status=LT48_RUNTIME_ERROR;else s->configuration[0]=values[i].value;}
            if((s->r->philips==4||s->r->philips==8) && values[i].object_id==0 && values[i].category==0 && values[i].field<3){
                if(values[i].value>1)status=LT48_RUNTIME_ERROR;else s->configuration[values[i].field]=values[i].value;}
            if((s->r->philips==6||s->r->philips==8||s->r->philips==11) && values[i].object_id==0 && values[i].category==2 && values[i].field>=(s->r->philips==8?8u:0u) && values[i].field<(s->r->philips==8?8u:0u)+lt48_mcu51_configuration_bytes(s->r)){
                if(values[i].value>255)status=LT48_RUNTIME_ERROR;else s->configuration_bytes[values[i].field-(s->r->philips==8?8u:0u)]=(unsigned char)values[i].value;
            }
            if(s->r->philips==7&&values[i].object_id==0&&values[i].category==1&&values[i].field==2){
                if(values[i].value>1)status=LT48_RUNTIME_ERROR;else s->configuration[3]=values[i].value;
            }
            if(values[i].object_id==0 && values[i].category==0 && values[i].field==16){
                if(values[i].value>1)status=LT48_RUNTIME_ERROR;else s->configuration[3]=values[i].value;}
        }
    }
    if(status==LT48_OK && s->r->at89==2) {
        const lt48_runtime_configuration_value *values;size_t n;
        status=lt48_runtime_get_configuration(result,&values,&n);
        if(status==LT48_OK && n!=7)status=LT48_RUNTIME_ERROR;
        for(unsigned i=0;status==LT48_OK && i<4;i++) {
            if(values[i+3].object_id!=0 || values[i+3].category!=0 || values[i+3].field!=(i==3?28:22+i) || values[i+3].value>1)status=LT48_RUNTIME_ERROR;
            else s->configuration[i]=values[i+3].value;
        }
    }
    if(status==LT48_OK && (s->r->l14||s->r->lm20)){
        const lt48_runtime_configuration_value*v;size_t n;status=lt48_runtime_get_configuration(result,&v,&n);
        if(!status && n>40)status=LT48_RUNTIME_ERROR;
        if(!status){memcpy(s->l14cfg,v,n*sizeof *v);s->l14count=n;}
        if(!status&&op==3&&s->r->l14&&s->r->l14->kind==1){const unsigned char*b;size_t n;status=lt48_runtime_device_image_internal(result,&b,&n);if(!status&&n!=s->l14image_size)status=LT48_RUNTIME_ERROR;if(!status)memcpy(s->l14image,b,n);}
    }
    if(status==LT48_OK&&s->r->cl38&&s->r->cohort->kind==12){const lt48_runtime_configuration_value*v;size_t n;status=lt48_runtime_get_configuration(result,&v,&n);if(!status&&n>16)status=LT48_FORMAT_ERROR;if(!status){memcpy(s->clcfg,v,n*sizeof*v);s->clcount=n;}}
    if(status==LT48_OK) {
        for(uint8_t i=0;i<s->r->buffer_count;i++) {
            const unsigned char *data;size_t size;
            status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK || size!=32) {status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        if(status==LT48_OK && (s->r->philips==3 || s->r->philips==5 || s->r->philips==6) && (op==12 || (op==s->r->init_op && s->mode==1))) {
            unsigned raw=s->buffers[1][0];
            unsigned v=s->r->philips==6?(s->r->cohort->lock_field==3?s->buffers[1][0]&7:(s->buffers[1][0]&14)>>1):s->r->philips==5?((raw>>7)&1)|(((raw>>6)&1)<<1)|(((raw>>3)&1)<<2):s->buffers[12][0];
            if(s->r->philips==6?(s->r->cohort->lock_field!=3&&v!=0&&v!=1&&v!=3&&v!=7):(v!=7 && v!=6 && v!=4 && v!=0))status=LT48_FORMAT_ERROR;
        }
        if(status==LT48_OK&&s->r->philips==11&&(op==12||(op==2&&s->mode==1))){
            unsigned v=(s->buffers[1][0]>>4)&7u,level=v==7?0:v==6?1:v==4?2:v==0?3:4;
            if(level>3||s->configuration[0]!=level)status=LT48_FORMAT_ERROR;
        }
        if(status==LT48_OK&&s->r->philips==10&&(op==12||(op==2&&s->mode==1))){
            unsigned raw=s->buffers[8][0],top=raw&224u;unsigned level=top==224?0:top==96?1:top==160?2:top==192?3:4;
            if(level>3||s->configuration[0]!=level)status=LT48_FORMAT_ERROR;
            else s->configuration_bytes[0]=(unsigned char)raw;
        }
        if(s->r->l14 || s->r->lm20 || (s->r->cohort && s->r->cohort->persist_variables))
            for(size_t i=0;status==LT48_OK && i<s->r->var_count;i++)
                status=lt48_runtime_get_variable(result,s->vars[i].variable_id,&s->vars[i]);
        for(uint8_t i=0;status==LT48_OK && i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        unsigned serial_id=0;
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(s->r->philips==9&&event.kind==LT48_RUNTIME_CONTROLLER_PACKED_WRITE&&event.argument_count==6&&event.arguments[0]==0x58&&event.arguments[3]==8){
                unsigned v=(unsigned)event.arguments[4]|((unsigned)event.arguments[5]<<8);
                if(serial_id){unsigned a=v&255;s->id_expected=a==0x30?0x58:a==0x31?0x57:a==0x60?0xbb:a==0x61?0xcf:0;serial_id=0;}
                else if(v==0x10b)serial_id=1;
            }
            if(s->r->lm20&&(event.kind==11||event.kind==19))s->lm20_captures++;
            if(event.kind==LT48_RUNTIME_RESULT_LATCH) {s->fault=(unsigned)event.arguments[0];status=LT48_RUNTIME_ERROR;}
            if(event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0)status=LT48_RUNTIME_ERROR;
        }
    }
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_compile_variable *var(mcu_session *s,unsigned id) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==id)return &s->vars[i];
    return NULL;
}
#include "../../NewDeviceLib/sources/upi41ah/operations.inc"
static lt48_status init(mcu_session *s,lt48_m37_id_decision_fn decide,void *context) {
    for(unsigned attempt=0;attempt<64;attempt++) {
        lt48_status status=run(s,s->r->init_op);
        /* The source-locked C515 CHECKID Wc5 reports 24, not 6, on
         * signature mismatch. Do not reinterpret 24 for any other family. */
        int c515_id=s->r->cohort && s->r->cohort->m==111 && s->fault==24;
        if(s->r->philips==9&&s->fault==9){s->configuration[0]=1;return LT48_SAFETY_REJECTED;}
        if(status==LT48_OK || (s->fault!=6 && !c515_id && !((s->r->hy24||s->r->cl38||s->r->philips||s->r->flash51_41||s->r->legacy9) && s->fault==5)))return status;
        if(!decide)return LT48_DEVICE_MISMATCH;
        /* LV init compares one captured byte at a time. Buffer13 holds the
         * corresponding expected manufacturer/device byte, buffer5 the actual. */
        lt48_m37_id_choice choice=s->r->hy24?
            decide(context,s->r->expected_manufacturer,var(s,20)?var(s,20)->value:0,s->buffers[(s->r->hy24>=114&&s->r->hy24<=118)?9:1][0],0):s->r->legacy9?
            (s->buffers[13][0]==s->r->expected_manufacturer?decide(context,s->r->expected_manufacturer,0,s->buffers[5][0],0):decide(context,s->r->expected_manufacturer,s->buffers[13][0],s->r->expected_manufacturer,s->buffers[5][0])):s->r->philips==10?(s->fault==5?decide(context,0x58,0,s->buffers[6][0],0):decide(context,0x58,0x57,0x58,s->buffers[7][0])):s->r->philips==9?(s->fault==5?decide(context,0x58,0,s->buffers[10][0],0):decide(context,0x58,(uint16_t)s->id_expected,0x58,s->buffers[10][0])):s->r->philips==7?
            ((mcu70_electrical(s->r)?s->fault==5:s->buffers[5][0]==0xda)?decide(context,0xda,0,s->buffers[1][0],0):decide(context,0xda,var(s,20)?var(s,20)->value:0x42,0xda,s->buffers[1][0])):s->r->flash51_41?decide(context,var(s,37)?var(s,37)->value:0,var(s,36)?var(s,36)->value:0,s->fault==5?s->buffers[5][0]:var(s,37)?var(s,37)->value:0,s->fault==6?s->buffers[5][0]:0):s->r->philips && var(s,20) ?
            (s->fault==5?decide(context,0x15,0,s->buffers[1][0],0):decide(context,0x15,(uint16_t)var(s,20)->value,0x15,s->buffers[1][0])) : s->r->at89==2 ? decide(context,0,(uint16_t)var(s,36)->value,0,s->buffers[5][0]) : s->r->at89 ?
            (s->buffers[13][0]==30 ? decide(context,30,0,s->buffers[5][0],0) :
             decide(context,30,(uint16_t)var(s,36)->value,30,s->buffers[5][0])) :
            mcu70_issi(s->r) && var(s,20) ?
            (s->buffers[5][0]==var(s,20)->value?decide(context,0xd5,(uint16_t)var(s,20)->value,0xd5,s->buffers[1][0]):decide(context,0xd5,0,s->buffers[1][0],0)):
            s->r->cohort && s->r->cohort->address_cache ?
            decide(context,0xc5,s->r->cohort->m==104||s->r->cohort->m==105?0x84:s->r->cohort->m==106?0x05:s->r->cohort->m==107?0x83:s->r->cohort->m==108?0xc1:s->r->cohort->m==111?0x95:0x08,s->buffers[8][0],s->buffers[8][1]):
            decide(context,(uint16_t)s->r->expected_manufacturer,0,s->buffers[s->r->cohort?s->r->cohort->id_buffer:1][0],0);
        if(choice==LT48_M37_ID_ABORT)return LT48_DEVICE_MISMATCH;
        if(choice==LT48_M37_ID_IGNORE){
            if(!lt48_mcu51_id_can_ignore(s->r))return LT48_DEVICE_MISMATCH;
            s->ignore_id=1;
            /* Continue the selected procedure through its complete INIT.
             * Some source SST routines check IDs unconditionally. Override
             * only that session's expected ID, never select another profile. */
            if(s->r->flash51_41){lt48_compile_variable*mid=var(s,37),*did=var(s,36);
                if(mid&&(s->fault==5||((s->r->flash51_41<=15)&&s->buffers[13][0]==mid->value)))mid->value=s->buffers[5][0];
                else if(did)did->value=s->buffers[5][0];}
        }
        else if(choice!=LT48_M37_ID_RETRY)return LT48_INVALID_ARGUMENT;
    }
    return LT48_DEVICE_MISMATCH;
}
static lt48_status range(mcu_session *s,lt48_operation_kind op,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,lt48_mcu51_result *out) {
    for(size_t i=0;i<length;i++) {
        uint32_t addr=start+(uint32_t)i;
        if((s->r->flash51_41&&!flash51_mapped(s->r,addr))||!cl38_mapped(s->r,addr)){if(read)read[i]=255;out->completed_bytes=i+1;continue;}
        for(unsigned j=0;j<4;j++)s->buffers[s->r->address_buffer][j]=(unsigned char)(addr>>(8*j));
        s->buffers[s->r->output_buffer][0]=op==LT48_OPERATION_BLANK_CHECK?s->r->blank:(image?image[i]:0);
        s->buffers[s->r->input_buffer][0]=s->buffers[s->r->output_buffer][0];
        lt48_status status=run(s,op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM?s->r->program_op:s->r->load_op);
        if(status!=LT48_OK)return status;
        unsigned char actual=s->buffers[s->r->input_buffer][0];
        if(read)read[i]=actual;
        out->completed_bytes=i+1;
        if(s->api->progress && ((i&63u)==63u || i+1==length))
            s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,i+1,length,0,0,0);
        if(op!=LT48_OPERATION_READ && !(s->r->flash51_41&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) && actual!=s->buffers[s->r->output_buffer][0]) {
            if(!out->mismatch_count)out->first_mismatch=addr;
            out->mismatch_count++;
            if(op!=LT48_OPERATION_COMPARE)return LT48_DEVICE_MISMATCH;
        }
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status cycle(mcu_session *s,lt48_operation_kind op,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_m37_id_decision_fn decide,
    void *context,lt48_mcu51_result *out) {
    if(s->r->upi)return upi_cycle(s,op,start,length,image,read,vcc,0,out);
    memset(s->buffers,0,sizeof(s->buffers));
    for(unsigned j=0;j<4;j++)s->buffers[s->r->address_buffer][j]=(unsigned char)(start>>(8*j));
    if(s->r->philips==9)memset(s->buffers[0],0,4);
    s->mode=op==LT48_OPERATION_READ?1:
        (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)?
            (op==LT48_OPERATION_OVER_PROGRAM && !s->r->at89?((s->r->hy24==114||s->r->hy24==115||s->r->myson2)?2:s->r->cohort?s->r->cohort->over_mode:3):2):
        op==LT48_OPERATION_VERIFY?4:op==LT48_OPERATION_COMPARE?1:11;
    lt48_compile_variable *v=var(s,0);if(v)v->value=vcc;
    lt48_status status=init(s,decide,context);
    /* A fresh range cannot inherit the address of the final ID capture.
     * These callbacks emit only the carry-chain delta after initialization. */
    if(status==LT48_OK&&s->r->wave[18]&&(s->r->philips==7||s->r->philips==8||s->r->philips==11)&&mcu70_electrical(s->r))status=run(s,18);
    /* M74's multiplexed source LOAD latches the high byte only at low==0.
     * Prime its exact existing branch at the aligned address for a partial
     * range. The discarded read is within this device; no programming pulse. */
    if(status==LT48_OK&&s->r->cohort&&s->r->cohort->m==74&&(start&255u)){
        s->buffers[s->r->address_buffer][0]=0;
        status=run(s,s->r->load_op);
        s->buffers[s->r->address_buffer][0]=(unsigned char)start;
    }
    /* A new range has no valid previous low-byte address. Force the exact
     * source's full bank+low-address path for its first element, rather than
     * applying its carry-only update to an unrelated initialization value. */
    if(status==LT48_OK && s->r->cohort && s->r->cohort->address_cache)
        s->buffers[s->r->cohort->address_cache][0]=(unsigned char)((start>>8)^255u);
    if(status==LT48_OK&&(s->r->hy24==114||s->r->hy24==115))status=run(s,18);
    /* M72/M73 LOAD selects the internal counter parity, not an external
     * address bus. Advance through original read cycles from the reset origin
     * before a partial range; never prime with programming pulses. */
    if(status==LT48_OK&&(s->r->hy24==114||s->r->hy24==115)&&start)status=range(s,LT48_OPERATION_READ,0,start,NULL,NULL,out);
    if(status==LT48_OK&&s->r->philips==9&&start)status=range(s,LT48_OPERATION_READ,0,start,NULL,NULL,out);
    if(status==LT48_OK)status=range(s,op,start,length,image,read,out);
    if(status==LT48_OK&&s->r->philips==8&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)&&((start+length)&127u)){
        uint32_t tail=(start+(uint32_t)length-1)|127u;
        for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(tail>>(8*j));
        s->buffers[2][0]=255;status=run(s,18);if(status==LT48_OK)status=run(s,6);
    }
    lt48_status reset=run(s,s->r->reset_op);
    return status==LT48_OK?reset:status;
}
static unsigned lock_mask(const mcu_session *s) {return s->configuration[0]|(s->configuration[1]<<1)|(s->configuration[2]<<2);}
static void set_locks(mcu_session *s,unsigned mask) {for(unsigned i=0;i<3;i++)s->configuration[i]=(mask>>i)&1;}
static lt48_status config_cycle(mcu_session *s,unsigned mode,int protect,lt48_m37_id_decision_fn decide,void *context) {
    memset(s->buffers,0,sizeof(s->buffers));s->mode=mode;
    lt48_compile_variable *v=var(s,0);if(v)v->value=s->r->nominal;
    lt48_status status=init(s,decide,context);
    if(status==LT48_SAFETY_REJECTED&&s->r->philips==9&&mode==1&&s->fault==9)status=LT48_OK;
    if(status==LT48_OK && protect)status=run(s,protect==2?14:s->r->protect_op);
    lt48_status reset=run(s,s->r->reset_op);return status==LT48_OK?reset:status;
}
lt48_status lt48_mcu51_validate_options(const lt48_mcu51_resources *r,
    lt48_operation_kind op,const lt48_mcu51_options *options) {
    if(!r || !options)return LT48_INVALID_ARGUMENT;
    lt48_verify_margin_selection margin=options->margin;
    if(options->margin_type>1 || (op==LT48_OPERATION_SECURE &&
       (!options->protection_level || options->protection_level>r->protect_max)))return LT48_INVALID_ARGUMENT;
    if(options->encryption_size && (r->legacy || r->philips==8 || r->philips==11 || !options->encryption ||
       options->encryption_size!=r->secondary || !r->secondary ||
       (op!=LT48_OPERATION_PROGRAM && op!=LT48_OPERATION_OVER_PROGRAM)))return LT48_INVALID_ARGUMENT;
    if(margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL &&
       margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
static lt48_status mcu51_finish(mcu_session*s,lt48_status status,lt48_mcu51_result*out){
 const lt48_protocol_transport*api=s->api;void*context=s->context;const lt48_mcu51_resources*r=s->r;
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s->wire);
    if(out->cleanup_status==LT48_OK) {
        lt48_status step=run(s,1);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
        if(step!=LT48_OK)out->cleanup_status=step;
        for(unsigned channel=0;channel<4;channel++) {
            unsigned char off[]={0x10,(unsigned char)(3-channel),0};
            /* Restart each phase after transport failure. */
            lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s->wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof(off));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
            if(out->cleanup_status==LT48_OK && step!=LT48_OK)out->cleanup_status=step;
        }
        /* Original generic tail restores the Windows idle FPGA after power-off. */
        static const unsigned char idle_header[]={0x38,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s->wire,idle_header,sizeof(idle_header));
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s->wire,r->idle,r->idle_size);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
        if(out->cleanup_status==LT48_OK && step!=LT48_OK)out->cleanup_status=step;
        static const unsigned char finish[]={0x49,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s->wire,finish,sizeof(finish));
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
        if(out->cleanup_status==LT48_OK)out->cleanup_status=step;
        out->cleanup_completed=out->cleanup_status==LT48_OK;
        if(out->cleanup_completed) {
            unsigned char led[]={0x40,status==LT48_OK?4:2};
            step=lt48_runtime_transport_session_controller_write(s->wire,led,sizeof(led));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
            if(step!=LT48_OK)out->cleanup_status=step;
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
 return status;
}
#include "../../NewDeviceLib/sources/class2018_38/owner.inc"
#include "../../NewDeviceLib/sources/legacy14/owner.inc"
#include "../../NewDeviceLib/sources/legacy_mcu20/owner.inc"
lt48_status lt48_mcu51_execute(const lt48_mcu51_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    const lt48_mcu51_options *options,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_mcu51_result *out) {
    if(cl38_rich(r))return cl38_execute(r,api,context,op,start,length,image,size,read,read_size,options,decide,decision_context,out);
    if(r&&r->lm20)return lm20_execute(r,api,context,op,start,length,image,size,read,read_size,options,decide,decision_context,out);
    if(r&&r->l14)return legacy14_execute(r,api,context,op,start,length,image,size,read,read_size,options,decide,decision_context,out);
    if(!out || !api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_mcu51_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if((op==LT48_OPERATION_READ || op==LT48_OPERATION_CONFIGURATION) && (!read || read_size<(op==LT48_OPERATION_CONFIGURATION?(r->upi?32:lt48_mcu51_configuration_bytes(r)?1+lt48_mcu51_configuration_bytes(r):2):length)))return LT48_BUFFER_TOO_SMALL;
    lt48_mcu51_options defaults={LT48_VERIFY_MARGIN_ALL,0,1,NULL,0};
    if(!options)options=&defaults;
    lt48_verify_margin_selection margin=options->margin;
    status=lt48_mcu51_validate_options(r,op,options);
    if(status!=LT48_OK)return status;
    unsigned char *pending=malloc(length);if(!pending)return LT48_NO_MEMORY;
    mcu_session s={0};s.r=r;if(r->philips==10)s.configuration_bytes[0]=255;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof(s.vars));s.mode=1;s.protection=options->protection_level;
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(pending);return status;}
    status=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    /* USB bridge initialization is owned by the qualified native transport.
     * FX2 opcode 0x18 is reserved (RET at CODE:0a1f) in both owned
     * images. Original 43d580 visits type-6 applets only; these groups have none. */
    const unsigned char fpga_header[]={0x38,0xaa,0x55};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,fpga_header,sizeof(fpga_header));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,r->fpga?r->fpga:r->idle,r->fpga?2778:r->idle_size);
    const unsigned char selector[]={0x0f,15};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,selector,sizeof(selector));
    if(status==LT48_OK)status=run(&s,0);
    for(unsigned i=0;status==LT48_OK && i<3;i++)status=lt48_runtime_transport_session_status(s.wire,1);
    static const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
    if(status==LT48_OK && r->at89==2 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION))
        status=config_cycle(&s,1,0,decide,decision_context);
    unsigned char signature[32]={0};
    if(status==LT48_OK && r->upi)status=upi_identify(&s,signature,decide,decision_context,out);
    if(status==LT48_OK && r->upi && signature[31] && op!=LT48_OPERATION_CONFIGURATION)status=LT48_SAFETY_REJECTED;
    /* Mode2 INIT nests PROGRAMconfig on these exact source procedures.
     * Preserve physical byte settings before any code-writing initialization. */
    if(status==LT48_OK && (r->philips==6||r->philips==8||r->philips==9||r->philips==11) &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))
        status=config_cycle(&s,1,0,decide,decision_context);
    if(status==LT48_OK&&(r->philips==8||r->philips==9||r->philips==11)&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)&&lock_mask(&s))status=LT48_SAFETY_REJECTED;
    if(status==LT48_OK&&r->philips==10&&op>=1&&op<=6){
        status=config_cycle(&s,1,0,decide,decision_context);
        if(status==LT48_OK&&(s.configuration[0]>=2||((op==3||op==4)&&s.configuration[0])))status=LT48_SAFETY_REJECTED;
    }
    if(status==LT48_OK&&r->cl38&&r->philips==7&&op>=1&&op<=6){status=config_cycle(&s,1,0,decide,decision_context);if(status==LT48_OK&&s.configuration[0])status=LT48_SAFETY_REJECTED;}
    unsigned original_spi=s.configuration[3];
    if(status==LT48_OK && r->at89==2 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) {
        if(lock_mask(&s))status=LT48_SAFETY_REJECTED;
        else status=cycle(&s,LT48_OPERATION_BLANK_CHECK,start,length,NULL,NULL,r->nominal,decide,decision_context,out);
    }
    if(status==LT48_OK && !r->at89 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) {
        status=cycle(&s,op==LT48_OPERATION_PROGRAM?LT48_OPERATION_BLANK_CHECK:LT48_OPERATION_READ,
            start,length,NULL,pending,r->nominal,decide,decision_context,out);
        if(status==LT48_OK && op==LT48_OPERATION_OVER_PROGRAM)
            for(size_t i=0;i<length;i++)if(r->mcs48 ? ((pending[i]|image[i])!=image[i]) : ((pending[i]&image[i])!=image[i])) {status=LT48_DEVICE_MISMATCH;out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;break;}
    }
    if(status==LT48_OK&&r->flash51_41>=16&&r->flash51_41<=19&&op==LT48_OPERATION_SECURE)status=config_cycle(&s,1,0,decide,decision_context);
    unsigned source_before=s.source_flags;
    if(status==LT48_OK && r->philips>=3 && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION || ((r->philips==7||r->philips==8||r->philips==11)&&op==LT48_OPERATION_ERASE)))
        status=config_cycle(&s,1,0,decide,decision_context);
    if(status==LT48_OK) {
        if(r->flash51_41&&op==LT48_OPERATION_CONFIGURATION){status=config_cycle(&s,1,0,decide,decision_context);}
        else if(r->philips==7&&op==LT48_OPERATION_CONFIGURATION){
            if(size&&!s.configuration[3]){
                s.configuration[3]=1;unsigned level=s.protection;s.protection=0;
                status=config_cycle(&s,8,mcu70_electrical(r)?1:2,decide,decision_context);s.protection=level;
                if(status==LT48_OK)status=config_cycle(&s,1,0,decide,decision_context);
            }
            if(status==LT48_OK&&size&&s.configuration[3]!=1)status=LT48_DEVICE_MISMATCH;
        }
        else if((r->philips==9||r->philips==10)&&op==LT48_OPERATION_CONFIGURATION){/* Read completed by source INIT/status capture. */}
        else if(r->philips==8&&op==LT48_OPERATION_CONFIGURATION){
            unsigned char wanted[4];memcpy(wanted,s.configuration_bytes,4);unsigned mask=lock_mask(&s);
            if(size){
                if(mask!=0&&mask!=1&&mask!=3&&mask!=7)status=LT48_SAFETY_REJECTED;
                for(unsigned i=0;i<4;i++)if(image[2]&(1u<<i))wanted[i]=image[3+i];
                wanted[2]=(wanted[2]&248u)|(s.configuration_bytes[2]&7u);
                if(status==LT48_OK){memcpy(s.configuration_bytes,wanted,4);s.protection=mask==7?3:mask==3?2:mask==1?1:0;status=config_cycle(&s,8,1,decide,decision_context);}
                if(status==LT48_OK)status=config_cycle(&s,1,0,decide,decision_context);
            }
            if(status==LT48_OK&&(lock_mask(&s)!=mask||memcmp(s.configuration_bytes,wanted,4)))status=LT48_DEVICE_MISMATCH;
        }
        else if((r->philips==6||r->philips==11) && op==LT48_OPERATION_CONFIGURATION){
            unsigned n=lt48_mcu51_configuration_bytes(r),level=s.configuration[0];
            unsigned char wanted[8];memcpy(wanted,s.configuration_bytes,n);
            if(size){
                if(image[0])level=image[1]==7?3:image[1]==3?2:image[1];
                if(level<s.configuration[0])status=LT48_SAFETY_REJECTED;
                for(unsigned i=0;i<n;i++)if(image[2]&(1u<<i)){
                    if((wanted[i]&image[3+i])!=image[3+i])status=LT48_SAFETY_REJECTED;
                    wanted[i]=image[3+i];
                }
                if(status==LT48_OK&&image[2]){memcpy(s.configuration_bytes,wanted,n);if(r->philips==11)s.protection=level;status=config_cycle(&s,r->philips==11?8:2,r->philips==11?1:2,decide,decision_context);}
                if(status==LT48_OK&&image[0]&&level>s.configuration[0]){s.protection=level;status=config_cycle(&s,8,1,decide,decision_context);}
                if(status==LT48_OK)status=config_cycle(&s,1,0,decide,decision_context);
            }
            if(status==LT48_OK&&(s.configuration[0]!=level||memcmp(s.configuration_bytes,wanted,n)))status=LT48_DEVICE_MISMATCH;
        }
        else if(r->philips==4 && op==LT48_OPERATION_CONFIGURATION) {
            unsigned mask=lock_mask(&s),clock=s.configuration[3];
            if(size && (image[0]&1))mask=image[1];
            if(size && (image[0]&2))clock=image[2];
            if((mask&lock_mask(&s))!=lock_mask(&s) || clock<s.configuration[3])status=LT48_SAFETY_REJECTED;
            if(status==LT48_OK && size){set_locks(&s,mask);s.configuration[3]=clock;status=config_cycle(&s,8,1,decide,decision_context);}
            if(status==LT48_OK && size)status=config_cycle(&s,1,0,decide,decision_context);
            if(status==LT48_OK && (lock_mask(&s)!=mask || s.configuration[3]!=clock))status=LT48_DEVICE_MISMATCH;
        } else if(r->philips>=2 && op==LT48_OPERATION_CONFIGURATION) {
            unsigned level=s.configuration[0],clock=s.configuration[3];
            if(size && (image[0]&1))level=image[1]==7?3:image[1]==3?2:image[1];
            if(size && (image[0]&2))clock=image[2];
            if(r->philips>=3 && (level<s.configuration[0] || clock<s.configuration[3]))status=LT48_SAFETY_REJECTED;
            if(status==LT48_OK && size){s.protection=level;s.configuration[3]=clock;status=config_cycle(&s,8,1,decide,decision_context);}
            if(status==LT48_OK && size && r->philips>=3)status=config_cycle(&s,1,0,decide,decision_context);
            if(status==LT48_OK && r->philips>=3 && (s.configuration[0]!=level || s.configuration[3]!=clock))status=LT48_DEVICE_MISMATCH;
        } else if(r->upi && op==LT48_OPERATION_CONFIGURATION) {
            status=upi_configuration(&s,image,size,signature,out);
        } else if(r->upi && op==LT48_OPERATION_SECURE) {
            status=upi_secure(&s,out);
        } else if(op==LT48_OPERATION_CONFIGURATION) {
            unsigned desired_locks=lock_mask(&s),desired_spi=s.configuration[3];
            if(size && (image[0]&1))desired_locks=image[1];
            if(size && (image[0]&2))desired_spi=image[2];
            if((desired_locks&lock_mask(&s))!=lock_mask(&s))status=LT48_SAFETY_REJECTED;
            if(status==LT48_OK && size && (image[0]&2)) {
                s.configuration[3]=desired_spi;status=config_cycle(&s,2,0,decide,decision_context);
            }
            if(status==LT48_OK && size && (image[0]&1)) {
                set_locks(&s,desired_locks);status=config_cycle(&s,8,1,decide,decision_context);
            }
            if(status==LT48_OK && size)status=config_cycle(&s,1,0,decide,decision_context);
            if(status==LT48_OK && (lock_mask(&s)!=desired_locks || s.configuration[3]!=desired_spi))status=LT48_DEVICE_MISMATCH;
        } else if(op==LT48_OPERATION_ERASE) {
            unsigned spi=s.configuration[3];
            s.mode=7;status=init(&s,decide,decision_context);
            if(status==LT48_OK)status=run(&s,mcu70_electrical(r)?10:8);
            lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
            if(status==LT48_OK&&r->philips==8){
                unsigned char wanted[4];memcpy(wanted,s.configuration_bytes,4);wanted[2]|=7;memcpy(s.configuration_bytes,wanted,4);s.protection=0;set_locks(&s,0);
                status=config_cycle(&s,2,0,decide,decision_context);
                if(status==LT48_OK)status=config_cycle(&s,8,1,decide,decision_context);
                if(status==LT48_OK)status=config_cycle(&s,1,0,decide,decision_context);
                if(status==LT48_OK&&(lock_mask(&s)||memcmp(wanted,s.configuration_bytes,4)))status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK&&r->philips==11){
                unsigned char wanted[4];memcpy(wanted,s.configuration_bytes,4);s.protection=0;
                status=config_cycle(&s,8,1,decide,decision_context);
                if(status==LT48_OK)status=config_cycle(&s,1,0,decide,decision_context);
                if(status==LT48_OK&&(s.configuration[0]||memcmp(wanted,s.configuration_bytes,4)))status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK&&r->philips==7){
                status=config_cycle(&s,1,0,decide,decision_context);
                if(status==LT48_OK&&(s.configuration[0]||s.configuration[3]))status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK && r->at89==2){s.configuration[3]=spi;status=config_cycle(&s,2,0,decide,decision_context);}
            if(status==LT48_OK && r->at89==2)status=config_cycle(&s,1,0,decide,decision_context);
            if(status==LT48_OK && r->at89==2 && (lock_mask(&s) || s.configuration[3]!=spi))status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_BLANK_CHECK,0,r->capacity,NULL,NULL,r->nominal,decide,decision_context,out);
        } else if(op==LT48_OPERATION_SECURE) {
            unsigned locks=lock_mask(&s)|((1u<<s.protection)-1),spi=s.configuration[3];
            if(r->at89==2 || r->philips==4)set_locks(&s,locks);
            unsigned philips_level=s.protection,philips_clock=s.configuration[3];
            unsigned char boot_before[4];memcpy(boot_before,s.configuration_bytes,4);
            if((r->philips==3 || r->philips==5 || r->philips==6 || r->philips==10 || r->philips==11) && s.configuration[0]>philips_level)philips_level=s.configuration[0];
            if(r->philips==8){unsigned old=lock_mask(&s);if(old!=0&&old!=1&&old!=3&&old!=7)status=LT48_SAFETY_REJECTED;unsigned level=old==7?3:old==3?2:old==1?1:0;if(level>philips_level)philips_level=level;}
            if(r->philips)s.protection=philips_level;
            unsigned original_level=s.configuration[0];
            if(r->philips==10&&original_level&&options->protection_level!=original_level)status=LT48_SAFETY_REJECTED;
            int already_protected=(r->philips==9&&s.configuration[0])||(r->philips==10&&original_level);
            s.mode=8;if(status==LT48_OK&&!already_protected)status=init(&s,decide,decision_context);
            if(status==LT48_OK&&!already_protected)status=run(&s,r->protect_op);
            if(status==LT48_OK&&r->hy24==118&&s.protection==2)status=run(&s,19);
            lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
            if(status==LT48_OK&&r->flash51_41>=16&&r->flash51_41<=19){
                unsigned desired=source_before;for(unsigned i=0;i<s.protection;i++)desired|=1u<<(22+i);
                status=config_cycle(&s,1,0,decide,decision_context);
                if(status==LT48_OK&&s.source_flags!=desired)status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK && r->at89==2)status=config_cycle(&s,1,0,decide,decision_context);
            if(status==LT48_OK && r->at89==2 && (lock_mask(&s)!=locks || s.configuration[3]!=spi))status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK && r->philips>=3)status=config_cycle(&s,1,0,decide,decision_context);
            if(status==LT48_OK && r->philips>=3 && r->philips!=7 && r->philips!=8 && r->philips!=9 && ((r->philips==4?lock_mask(&s)!=locks:s.configuration[0]!=philips_level) || s.configuration[3]!=philips_clock))status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK&&r->philips==11&&memcmp(boot_before,s.configuration_bytes,4))status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK&&r->philips==10&&((s.configuration_bytes[0]&31)!=(boot_before[0]&31)))status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK&&r->philips==9&&!s.configuration[0])status=LT48_DEVICE_MISMATCH;
            if(status==LT48_OK&&r->philips==8){boot_before[2]=(boot_before[2]&248u)|(7u^locks);if(lock_mask(&s)!=locks||memcmp(boot_before,s.configuration_bytes,4))status=LT48_DEVICE_MISMATCH;}
            if(status==LT48_OK&&r->philips==7&&(s.configuration[3]!=philips_clock||(mcu70_electrical(r)&&philips_level==1&&!s.configuration[0])))status=LT48_DEVICE_MISMATCH;
        } else if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2];mcu51_verify_levels(r,options->margin_type,volts);size_t n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
            for(size_t i=0;status==LT48_OK && i<n;i++)status=cycle(&s,op,start,length,image,NULL,volts[i],decide,decision_context,out);
        } else status=cycle(&s,op,start,length,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,decide,decision_context,out);
    }
    if(status==LT48_OK && r->upi && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM))
        status=cycle(&s,LT48_OPERATION_VERIFY,start,length,image,NULL,500,decide,decision_context,out);
    if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)){uint16_t levels[2];mcu51_verify_levels(r,options->margin_type,levels);
        for(unsigned i=0;status==LT48_OK && i<2;i++)status=cycle(&s,LT48_OPERATION_VERIFY,start,length,image,NULL,levels[i],decide,decision_context,out);}
    if(status==LT48_OK && r->at89==2 && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)) {
        status=config_cycle(&s,1,0,decide,decision_context);
        if(status==LT48_OK && (lock_mask(&s) || s.configuration[3]!=original_spi))status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK && options->encryption_size) {
        /* Original class2018 mode9: DEVINIT, each secondary byte, DEVRESET.
         * Code verification precedes encryption. Array has no direct read callback. */
        s.mode=9;memset(s.buffers,0,sizeof(s.buffers));
        lt48_compile_variable *vcc=var(&s,0);if(vcc)vcc->value=500;
        status=init(&s,decide,decision_context);
        if(status==LT48_OK&&(r->hy24==114||r->hy24==115))status=run(&s,18);
        for(size_t i=0;status==LT48_OK && i<options->encryption_size;i++) {
            for(unsigned j=0;j<4;j++)s.buffers[0][j]=(unsigned char)(i>>(8*j));
            s.buffers[1][0]=s.buffers[2][0]=options->encryption[i];
            status=run(&s,r->encrypt_op);
        }
        lt48_status reset=run(&s,r->reset_op);if(status==LT48_OK)status=reset;
    }
    /* These source resets have no trailing status/capture. Commit the final
     * operation queue before replacing its transport with cleanup's session. */
    if(status==LT48_OK && (r->hy24 || r->legacy9 || (r->cohort && r->cohort->flush_tail)))
        status=lt48_runtime_transport_session_flush(s.wire);
    out->operation_status=status;
    /* A fresh uncancellable transport owns cleanup even after a failed queue.
     * Continue independent rail-off attempts and preserve the first error. */
    status=mcu51_finish(&s,status,out);
    if(status==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,pending,length);
    if(status==LT48_OK && r->upi && op==LT48_OPERATION_CONFIGURATION)memcpy(read,signature,32);
    else if(status==LT48_OK&&op==LT48_OPERATION_CONFIGURATION&&r->flash51_41){read[0]=(s.source_flags>>22)&7;read[1]=(s.source_flags>>29)&7;}
    else if(status==LT48_OK&&op==LT48_OPERATION_CONFIGURATION&&r->philips==10){read[0]=(unsigned char)s.configuration[0];read[1]=s.configuration_bytes[0];}
    else if(status==LT48_OK&&op==LT48_OPERATION_CONFIGURATION&&r->philips==9){read[0]=(unsigned char)s.configuration[0];read[1]=255;}
    else if(status==LT48_OK&&op==LT48_OPERATION_CONFIGURATION&&r->philips==7){read[0]=mcu70_electrical(r)?(unsigned char)s.configuration[0]:255;read[1]=(unsigned char)s.configuration[3];}
    else if(status==LT48_OK && op==LT48_OPERATION_CONFIGURATION && r->philips==8){read[0]=(unsigned char)lock_mask(&s);memcpy(read+1,s.configuration_bytes,4);}
    else if(status==LT48_OK && op==LT48_OPERATION_CONFIGURATION && (r->philips==6||r->philips==11)){read[0]=(unsigned char)((1u<<s.configuration[0])-1);memcpy(read+1,s.configuration_bytes,lt48_mcu51_configuration_bytes(r));}
    else if(status==LT48_OK && op==LT48_OPERATION_CONFIGURATION && r->philips){read[0]=r->philips==4?(unsigned char)lock_mask(&s):r->philips>=3?(unsigned char)((1u<<s.configuration[0])-1):255;read[1]=(r->philips==3 || r->philips==4)?(unsigned char)s.configuration[3]:255;}
    else if(status==LT48_OK && op==LT48_OPERATION_CONFIGURATION){read[0]=(unsigned char)lock_mask(&s);read[1]=(unsigned char)s.configuration[3];}
    lt48_runtime_transport_session_close(s.wire);free(pending);return status;
}

unsigned lt48_mcu51_lm20_kind(const lt48_mcu51_resources*r){return r&&r->lm20?r->lm20->group:0;}
unsigned lt48_mcu51_lm20_admitted(const lt48_mcu51_resources*r){return r&&r->lm20&&(r->lm20->group==8?LT48_HMS99ONE_ADMITTED:r->lm20->group==1?LT48_LM20_STAGE>=1:LT48_LM20_STAGE>=2);}
unsigned lt48_mcu51_lm20_mask(const lt48_mcu51_resources*r){return r&&r->lm20?63u|(r->lm20->erase?64:0)|(r->lm20->secure?128:0)|(r->lm20->configuration?512:0):0;}
