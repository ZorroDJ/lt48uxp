#include "at89_s2051.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void lt48_s2051_close(lt48_s2051_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
lt48_status lt48_s2051_open(const lt48_installation *x,size_t device,lt48_s2051_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;unsigned flash,family,pinmap;
    lt48_status st=lt48_installation_resolve_device(x,device,&spec);
    if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
    if(spec.definition_resource<0x152 || spec.definition_resource>0x153)return LT48_UNSUPPORTED;
    family=244;pinmap=117;flash=spec.definition_resource==0x152?2048:4096;
    if(spec.family_code!=family || spec.pinmap!=pinmap)return LT48_SAFETY_REJECTED;
    lt48_s2051_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
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
            unsigned alias=op.operation_id==2?12:op.operation_id==3?13:op.operation_id==11?14:0;
            if(alias) {
                lt48_compact_wave *copy=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&copy));
                st=lt48_compact_compile(copy,r->pins,r->vars,6,1,0,&r->wave[alias]);lt48_compact_wave_close(copy);if(st!=LT48_OK)goto fail;
            }
            if(op.operation_id==11) {
                lt48_compact_wave *copy=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&copy));
                st=lt48_compact_compile(copy,r->pins,r->vars,6,1,0,&r->wave[15]);lt48_compact_wave_close(copy);if(st!=LT48_OK)goto fail;
            }
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
fail:lt48_s2051_close(r);return st;
#undef TRY
}

lt48_status lt48_s2051_address(const lt48_s2051_resources *r,uint32_t image,uint32_t *wire,uint32_t *remaining) {
    if(!r || !wire || !remaining)return LT48_INVALID_ARGUMENT;
    if(image>=r->flash+32)return LT48_SAFETY_REJECTED;
    unsigned base=image<r->flash?0:r->flash,end=base?r->flash+32:r->flash;
    *wire=image-base;*remaining=end-image;return LT48_OK;
}
