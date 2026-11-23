#include "at89_re2.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Authored transformation is built only in the pre-admission test target. */
#include "../../NewDeviceLib/sources/at89/re2_timeout_guard.c"

void lt48_re2_close(lt48_re2_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    for(unsigned i=0;i<2;i++)free(r->guarded[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
lt48_status lt48_re2_address(uint32_t image,uint32_t *bank,uint32_t *wire,uint32_t *remaining) {
    if(!bank || !wire || !remaining)return LT48_INVALID_ARGUMENT;
    if(image>=131072)return LT48_SAFETY_REJECTED;
    *bank=image>>16;*wire=image&65535;*remaining=65536-*wire;return LT48_OK;
}
lt48_status lt48_re2_compile_fragment(const lt48_wave_code *w,unsigned part,const lt48_pin_map *pins,const lt48_compile_variable *vars,lt48_compiled_wave **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!w || !pins || !vars || part>3)return LT48_INVALID_ARGUMENT;
    if(w->wave_type || !w->data || w->data_size!=(part==3?351u:257u))return LT48_SAFETY_REJECTED;
    unsigned char digest[32];char hash[65];lt48_sha256(w->data,w->data_size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hash+2*i,3,"%02x",digest[i]);
    if(strcmp(hash,part==3?"057289a1e70cc7ea4a90aee42235770c596dab18b22dfb4f8ea66cce4f029a7a":"ccb21e435f64894cc37e164e05b36b80f4a7061c77f17738a3bfc75981f1a725"))return LT48_SAFETY_REJECTED;
    const size_t starts[]={0x12,0x8a,2,0x1b},ends[]={0x8a,257,0x12,0x22};
    unsigned char raw[257];size_t n=ends[part]-starts[part];
    /* Owner executes shared preamble once and carries buffer 25 into fields. */
    memcpy(raw+2,w->data+starts[part],n);
    raw[0]=(unsigned char)n;raw[1]=(unsigned char)(n>>8);
    lt48_compact_wave *c=NULL;lt48_status st=lt48_compact_decode(raw,n+2,&c);
    if(st==LT48_OK)st=lt48_compact_compile(c,pins,vars,6,1,0,out);
    lt48_compact_wave_close(c);return st;
}
lt48_status lt48_re2_open(const lt48_installation *x,size_t device,lt48_re2_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,device,&spec);
    if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___") || spec.definition_resource!=0x163)return LT48_UNSUPPORTED;
    if(spec.family_code!=258 || spec.pinmap!=121 || spec.catalogue.manufacturer_id || spec.catalogue.device_id)return LT48_SAFETY_REJECTED;
    lt48_re2_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    char path[4096],name[24];size_t ri,fi;
    int n=snprintf(path,sizeof path,"%s/GDCC___.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
    TRY(lt48_owner_rez_open_internal(path,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));
    lt48_pin_code pc;
    TRY(lt48_rez_find_resource(r->rez,"P79",&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    lt48_family_info f;TRY(lt48_rez_find_resource(r->rez,"M102",&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count!=6 || f.operation_count!=19){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<6;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    unsigned apps=0;
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
        if(w.wave_type==0 && op.operation_id<12 && !r->wave[op.operation_id]) {
            lt48_compact_wave *c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));
            st=lt48_compact_compile(c,r->pins,r->vars,6,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
            if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
            if(op.operation_id==0)TRY(lt48_re2_compile_fragment(&w,3,r->pins,r->vars,&r->wave[15]));
            if(op.operation_id==11)for(unsigned part=0;part<3;part++)TRY(lt48_re2_compile_fragment(&w,part,r->pins,r->vars,&r->wave[12+part]));
        } else if(w.wave_type==6 && apps<6) {
            unsigned expected=op.operation_id==16?0x5000:op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:op.operation_id==23?0x9000:0;
            if(!expected || w.load_address!=expected){st=LT48_FORMAT_ERROR;goto fail;}
            if(op.operation_id==16 || op.operation_id==17) {
                unsigned g=op.operation_id-16;size_t length=0;
                r->guarded[g]=malloc(w.data_size+43);if(!r->guarded[g]){st=LT48_NO_MEMORY;goto fail;}
                TRY(lt48_re2_guard_applet(expected,w.data,w.data_size,r->guarded[g],w.data_size+43,&length));
                w.data=r->guarded[g];w.data_size=length;
            }
            r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,w.load_address};
        } else if(w.wave_type==4 && op.operation_id==30 && w.data_size==2780 && w.data[0]==0xda && w.data[1]==0x0a)r->fpga=w.data+2;
        else {st=LT48_UNSUPPORTED;goto fail;}
    }
    if(apps!=6 || !r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<16;i++)if(!r->wave[i] || lt48_compiled_wave_defaulted_pin_count(r->wave[i])){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_re2_close(r);return st;
#undef TRY
}
