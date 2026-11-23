#include "at89_s8253.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void lt48_s8253_close(lt48_s8253_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<17;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->gc8_protect_copy);free(r->idle);free(r);
}
lt48_status lt48_s8253_open(const lt48_installation *x,size_t device,lt48_s8253_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;unsigned flash,family,pinmap;
    lt48_status st=lt48_installation_resolve_device(x,device,&spec);
    if(st!=LT48_OK)return st;
    if(lt48_n76one_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_n76one_open(x,device,out);
    if(lt48_sy80_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sy80_open(x,device,out);
    if(lt48_sy76_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sy76_open(x,device,out);
    if(lt48_mw36_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_mw36_open(x,device,out);
    if(lt48_sm30_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sm30_open(x,device,out);
    if(lt48_sx30_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sx30_open(x,device,out);
    if(lt48_mg15_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_mg15_open(x,device,out);
    if(lt48_sn9_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sn9_open(x,device,out);
    if(lt48_ob16_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_ob16_open(x,device,out);
    if(lt48_wt10_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_wt10_open(x,device,out);
    if(lt48_wt8_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_wt8_open(x,device,out);
    if(lt48_hy14_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_hy14_open(x,device,out);
    if(lt48_mw6_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_mw6_open(x,device,out);
    if(lt48_my7_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_my7_open(x,device,out);
    if(lt48_tp4_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_tp4_open(x,device,out);
    if(lt48_tp6_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_tp6_open(x,device,out);
    if(lt48_my4_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_my4_open(x,device,out);
    if(lt48_sc2_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sc2_open(x,device,out);
    if(lt48_sc4g_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sc4g_open(x,device,out);
    if(lt48_sc4a_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sc4a_open(x,device,out);
    if(lt48_mw8_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_mw8_open(x,device,out);
    if(lt48_gc8_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_gc8_open(x,device,out);
    if(lt48_ab11_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_ab11_open(x,device,out);
    if(lt48_su16_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_su16_open(x,device,out);
    if(lt48_r8c8_admitted_internal(spec.catalogue.device_class,spec.definition_resource))return lt48_r8c8_open(x,device,out);
    if(lt48_ite2_admitted_internal(spec.catalogue.device_class,spec.definition_resource))return lt48_ite2_open(x,device,out);
    if(lt48_so30_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_so30_open(x,device,out);
    if(lt48_sm3_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sm3_open(x,device,out);
    if(lt48_sm6_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sm6_open(x,device,out);
    if(lt48_mw27_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_mw27_open(x,device,out);
    if(lt48_mf74_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_mf74_open(x,device,out);
    if(lt48_t89six_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_t89six_open(x,device,out);
    if(lt48_at91nine_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_at91nine_open(x,device,out);
    if(lt48_ramtron6_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_ramtron6_open(x,device,out);
    if(lt48_white3_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_white3_open(x,device,out);
    if(lt48_eep2041_3_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_eep2041_3_open(x,device,out);
    if(lt48_at28c64b1_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_at28c64b1_open(x,device,out);
    if(lt48_novatek25_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_novatek25_open(x,device,out);
    if(lt48_nuvoton8_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_nuvoton8_open(x,device,out);
    if(lt48_xmega7_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_xmega7_open(x,device,out);
    if(lt48_psoc29_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_psoc29_open(x,device,out);
    if(lt48_stm8s4_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_stm8s4_open(x,device,out);
    if(lt48_cc254x5_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_cc254x5_open(x,device,out);
    if(lt48_sst58rc2_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sst58rc2_open(x,device,out);
    if(lt48_sstmx16_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_sstmx16_open(x,device,out);
    if(spec.catalogue.device_class==2038 || (spec.catalogue.device_class==2048&&((spec.definition_resource>=0x156&&spec.definition_resource<=0x162)||(spec.definition_resource>=0x185&&spec.definition_resource<=0x18a))))return lt48_n79w79_open(x,device,out);
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
    if(lt48_n79w79_21_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_n79w79_21_open(x,device,out);
    if(lt48_nuvoton30_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_nuvoton30_open(x,device,out);
    if(lt48_msp43076_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_msp43076_open(x,device,out);
    if(lt48_numicro81_capacity_internal(spec.catalogue.device_class,spec.definition_resource,spec.family_code,spec.pinmap))return lt48_numicro81_open(x,device,out);
    if(spec.definition_resource>=0x9a && spec.definition_resource<=0xb1)return lt48_w78bank147_open(x,device,out);
    if(spec.definition_resource>=0xce&&spec.definition_resource<=0xd3)return lt48_w78d24_open(x,device,out);
    if(spec.definition_resource>=0xb2 && spec.definition_resource<=0xc1)return lt48_sst_open(x,device,out);
    if(spec.definition_resource>=0xc2 && spec.definition_resource<=0xcc)return lt48_p89v_open(x,device,out);
    if(spec.definition_resource<0x14d || spec.definition_resource>0x14e)return LT48_UNSUPPORTED;
    family=spec.definition_resource==0x14d?239:240;pinmap=spec.definition_resource==0x14d?113:114;flash=12288;
    if(spec.family_code!=family || spec.pinmap!=pinmap)return LT48_SAFETY_REJECTED;
    lt48_s8253_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    r->definition=spec.definition_resource;r->family=family;r->pinmap=pinmap;r->flash=flash;r->manufacturer=spec.catalogue.manufacturer_id;r->device=spec.catalogue.device_id;
    char path[4096],name[24];size_t ri,fi;
    int n=snprintf(path,sizeof path,"%s/GDCC___.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
    TRY(lt48_owner_rez_open_internal(path,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));
    snprintf(name,sizeof name,"P%x",pinmap);lt48_pin_code pc;
    TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof name,"M%x",family);lt48_family_info f;
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count!=4 || f.operation_count!=17){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<4;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    unsigned apps=0;
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
        if(w.wave_type==0 && op.operation_id<12 && !r->wave[op.operation_id]) {
            lt48_compact_wave *c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));
            st=lt48_compact_compile(c,r->pins,r->vars,4,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
            unsigned defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
            if(defaults!=0){st=LT48_SAFETY_REJECTED;goto fail;}
            if(op.operation_id==2)TRY(lt48_s8253_compile_fragment(&w,0,r->pins,r->vars,&r->wave[12]));
            if(op.operation_id==3)TRY(lt48_s8253_compile_fragment(&w,1,r->pins,r->vars,&r->wave[13]));
            if(op.operation_id==11)for(unsigned part=2;part<4;part++)TRY(lt48_s8253_compile_fragment(&w,part,r->pins,r->vars,&r->wave[12+part]));
            if(op.operation_id==10)TRY(lt48_s8253_compile_fragment(&w,4,r->pins,r->vars,&r->wave[16]));
        } else if(w.wave_type==6 && apps<4) {
            unsigned expected=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0;
            if(!expected || w.load_address!=expected){st=LT48_FORMAT_ERROR;goto fail;}
            r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,w.load_address};
        } else if(w.wave_type==4 && op.operation_id==30 && w.data_size==2780 && w.data[0]==0xda && w.data[1]==0x0a)r->fpga=w.data+2;
        else {st=LT48_UNSUPPORTED;goto fail;}
    }
    if(apps!=4 || !r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<17;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_s8253_close(r);return st;
#undef TRY
}

lt48_status lt48_s8253_address(const lt48_s8253_resources *r,uint32_t image,uint32_t *wire,uint32_t *remaining) {
    if(!r || !wire || !remaining)return LT48_INVALID_ARGUMENT;
    if(r->kind==17||r->kind==18){if(image>=r->capacity)return LT48_SAFETY_REJECTED;*wire=image;*remaining=r->capacity-image;return LT48_OK;}
    if(r->kind==16){if(image>=r->capacity)return LT48_SAFETY_REJECTED;if(r->n79_xp){*wire=image;*remaining=r->capacity-image;}else{*wire=image<8192?image:8128+image-8192;*remaining=(image<8192?8192:r->capacity)-image;}return LT48_OK;}
    if(image>=14400)return LT48_SAFETY_REJECTED;
    unsigned base=image<12288?0:image<14336?12288:14336,end=base==0?12288:base==12288?14336:14400;
    *wire=image-base;*remaining=end-image;return LT48_OK;
}
lt48_status lt48_s8253_compile_fragment(const lt48_wave_code *w,unsigned part,const lt48_pin_map *pins,const lt48_compile_variable *vars,lt48_compiled_wave **out) {
    static const char *hashes[]={
        "89185d4708cadce4b5e79eda60c3afe92b209de9dfbcc4d7b3418c655c743746",
        "96a296d224f285c67bee93c30f8a309157f0daa35dc5b87e410b78630a09cfc7",
        "e8cdfb4d33cda774cf6c4d33b5b869a7728b5e418521dced020b8db398d71a05",
        "e8cdfb4d33cda774cf6c4d33b5b869a7728b5e418521dced020b8db398d71a05",
        "62c7fcc7b87ae029b7a520d9d254905ecddd615fc370f57f23192d2d9417a866"};
    static const unsigned sizes[]={101,2,234,234,79},starts[]={2,2,2,0x87,0x3c},ends[]={0x39,2,0x87,234,79};
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;if(!w || !pins || !vars || part>4)return LT48_INVALID_ARGUMENT;
    if(w->wave_type || !w->data || w->data_size!=sizes[part])return LT48_SAFETY_REJECTED;
    unsigned char digest[32];char hash[65];lt48_sha256(w->data,w->data_size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hash+2*i,3,"%02x",digest[i]);
    if(strcmp(hash,hashes[part]))return LT48_SAFETY_REJECTED;
    unsigned char raw[234];size_t n=ends[part]-starts[part];memcpy(raw+2,w->data+starts[part],n);
    raw[0]=(unsigned char)n;raw[1]=(unsigned char)(n>>8);
    lt48_compact_wave *c=NULL;lt48_status st=lt48_compact_decode(raw,n+2,&c);
    if(st==LT48_OK)st=lt48_compact_compile(c,pins,vars,4,1,0,out);
    lt48_compact_wave_close(c);return st;
}

#include "../../NewDeviceLib/sources/w78d24/loader.inc"

#include "../../NewDeviceLib/sources/w78bank147/loader.inc"

#include "../../NewDeviceLib/sources/n79w79_88/loader.inc"

#include "../../NewDeviceLib/sources/numicro81/loader.inc"

#include "../../NewDeviceLib/sources/msp430_76/loader.inc"

#include "../../NewDeviceLib/sources/nuvoton30/loader.inc"

#include "../../NewDeviceLib/sources/n79w79_21/loader.inc"
#include "../../NewDeviceLib/sources/n76one/loader.inc"

#include "../../NewDeviceLib/sources/sstmx16/loader.inc"

#include "../../NewDeviceLib/sources/psoc29/loader.inc"

#include "../../NewDeviceLib/sources/xmega7/loader.inc"

#include "../../NewDeviceLib/sources/nuvoton8/loader.inc"

#include "../../NewDeviceLib/sources/novatek25/loader.inc"

#include "../../NewDeviceLib/sources/sst58rc2/loader.inc"

#include "../../NewDeviceLib/sources/cc254x5/loader.inc"

#include "../../NewDeviceLib/sources/stm8s4/loader.inc"

#include "../../NewDeviceLib/sources/at28c64b1/loader.inc"

#include "../../NewDeviceLib/sources/eep2041_3/loader.inc"

#include "../../NewDeviceLib/sources/ramtron6/loader.inc"

#include "../../NewDeviceLib/sources/ramtron21/membership.inc"

#include "../../NewDeviceLib/sources/at91nine/loader.inc"

#include "../../NewDeviceLib/sources/atmel-t89six/loader.inc"

#include "../../NewDeviceLib/sources/megawin-fudan74/loader.inc"

#include "../../NewDeviceLib/sources/megawin27/loader.inc"

#include "../../NewDeviceLib/sources/syncmos80/loader.inc"
#include "../../NewDeviceLib/sources/syncmos76/loader.inc"
#include "../../NewDeviceLib/sources/megawin36/loader.inc"
#include "../../NewDeviceLib/sources/syncmos30/loader.inc"

#include "../../NewDeviceLib/sources/syncmos595x30/loader.inc"

#include "../../NewDeviceLib/sources/magna15/loader.inc"

#include "../../NewDeviceLib/sources/syntek9/loader.inc"

#include "../../NewDeviceLib/sources/syncmos6/loader.inc"
#include "../../NewDeviceLib/sources/syncmos3/loader.inc"
#include "../../NewDeviceLib/sources/onbright16/loader.inc"
#include "../../NewDeviceLib/sources/weltrend10/loader.inc"
#include "../../NewDeviceLib/sources/sonix30/loader.inc"

#include "../../NewDeviceLib/sources/weltrend8/loader.inc"

#include "../../NewDeviceLib/sources/abov11/loader.inc"

#include "../../NewDeviceLib/sources/sunplus16/loader.inc"

#include "../../NewDeviceLib/sources/hyundai14/loader.inc"
#include "../../NewDeviceLib/sources/megawin6/loader.inc"
#include "../../NewDeviceLib/sources/myson7/loader.inc"
#include "../../NewDeviceLib/sources/myson4/loader.inc"
#include "../../NewDeviceLib/sources/megawin8/loader.inc"
#include "../../NewDeviceLib/sources/gencore8/loader.inc"

#include "../../NewDeviceLib/sources/topro4/loader.inc"

#include "../../NewDeviceLib/sources/topro6/loader.inc"

#include "../../NewDeviceLib/sources/syncmos2/loader.inc"

#include "../../NewDeviceLib/sources/syncmos4a/loader.inc"

#include "../../NewDeviceLib/sources/syncmos4g/loader.inc"

#include "../../NewDeviceLib/sources/white3/loader.inc"

#include "../../NewDeviceLib/sources/ite2/loader.inc"

#include "../../NewDeviceLib/sources/r8c8/loader.inc"
