#include "at89_lp2052.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void lt48_lp2052_close(lt48_lp2052_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
lt48_status lt48_lp2052_open(const lt48_installation *x,size_t device,lt48_lp2052_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;unsigned flash,family,pinmap;
    lt48_status st=lt48_installation_resolve_device(x,device,&spec);
    if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
    if(spec.definition_resource<0x154 || spec.definition_resource>0x155)return LT48_UNSUPPORTED;
    family=246+spec.definition_resource-0x154;pinmap=118;flash=family==246?2048:4096;
    if(spec.family_code!=family || spec.pinmap!=pinmap)return LT48_SAFETY_REJECTED;
    lt48_lp2052_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
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
    if(f.variable_count!=6 || f.operation_count!=17){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<6;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    unsigned apps=0;
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
        if(w.wave_type==0 && op.operation_id<12 && !r->wave[op.operation_id]) {
            lt48_compact_wave *c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));
            st=lt48_compact_compile(c,r->pins,r->vars,6,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
            unsigned defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
            if(defaults!=0){st=LT48_SAFETY_REJECTED;goto fail;}
            if(op.operation_id==2)TRY(lt48_lp2052_compile_fragment(&w,0,r->pins,r->vars,&r->wave[12]));
            if(op.operation_id==3)TRY(lt48_lp2052_compile_fragment(&w,1,r->pins,r->vars,&r->wave[13]));
            if(op.operation_id==11)for(unsigned part=2;part<4;part++)TRY(lt48_lp2052_compile_fragment(&w,part,r->pins,r->vars,&r->wave[12+part]));
        } else if(w.wave_type==6 && apps<4) {
            unsigned expected=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0;
            if(!expected || w.load_address!=expected){st=LT48_FORMAT_ERROR;goto fail;}
            r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,w.load_address};
        } else if(w.wave_type==4 && op.operation_id==30 && w.data_size==2780 && w.data[0]==0xda && w.data[1]==0x0a)r->fpga=w.data+2;
        else {st=LT48_UNSUPPORTED;goto fail;}
    }
    if(apps!=4 || !r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<16;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_lp2052_close(r);return st;
#undef TRY
}

lt48_status lt48_lp2052_address(const lt48_lp2052_resources *r,uint32_t image,uint32_t *wire,uint32_t *remaining) {
    if(!r || !wire || !remaining)return LT48_INVALID_ARGUMENT;
    if(image>=r->flash+32)return LT48_SAFETY_REJECTED;
    unsigned base=image<r->flash?0:r->flash,end=base?r->flash+32:r->flash;
    *wire=image-base;*remaining=end-image;return LT48_OK;
}
lt48_status lt48_lp2052_compile_fragment(const lt48_wave_code *w,unsigned part,const lt48_pin_map *pins,const lt48_compile_variable *vars,lt48_compiled_wave **out) {
    static const char *hashes[]={
        "13f9d8ba4336c39aab6985215302e043d7b6d0a7d77f90574199f3184eb3bad0",
        "6ae762004dbe43a49498c1131b0dd6296b7e95d413c329ca5c4062a0d41aec1e",
        "7ef6d91ad9ec4d2776d2c87750c2dc92ef7a527fe7027f972881b54d2be7236f",
        "7ef6d91ad9ec4d2776d2c87750c2dc92ef7a527fe7027f972881b54d2be7236f"};
    static const unsigned sizes[]={136,76,279,279},starts[]={2,0x34,2,0xa4},ends[]={0x54,76,0xa4,279};
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;if(!w || !pins || !vars || part>3)return LT48_INVALID_ARGUMENT;
    if(w->wave_type || !w->data || w->data_size!=sizes[part])return LT48_SAFETY_REJECTED;
    unsigned char digest[32];char hash[65];lt48_sha256(w->data,w->data_size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hash+2*i,3,"%02x",digest[i]);
    if(strcmp(hash,hashes[part]))return LT48_SAFETY_REJECTED;
    unsigned char raw[279];size_t n=ends[part]-starts[part];raw[0]=(unsigned char)n;raw[1]=(unsigned char)(n>>8);
    memcpy(raw+2,w->data+starts[part],n);lt48_compact_wave *c=NULL;lt48_status st=lt48_compact_decode(raw,n+2,&c);
    if(st==LT48_OK)st=lt48_compact_compile(c,pins,vars,6,1,0,out);
    lt48_compact_wave_close(c);return st;
}
