#include "at89_usb51.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void lt48_usb51_close(lt48_usb51_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<14;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
lt48_status lt48_usb51_open(const lt48_installation *x,size_t device,lt48_usb51_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;unsigned flash,family,pinmap;
    lt48_status st=lt48_installation_resolve_device(x,device,&spec);
    if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
    if(spec.definition_resource<0x14f || spec.definition_resource>0x151)return LT48_UNSUPPORTED;
    family=241+spec.definition_resource-0x14f;pinmap=family==241?115:116;flash=family==243?16384:32768;
    if(spec.family_code!=family || spec.pinmap!=pinmap)return LT48_SAFETY_REJECTED;
    lt48_usb51_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    r->definition=spec.definition_resource;r->family=family;r->pinmap=pinmap;r->flash=flash;
    char path[4096],name[24];size_t ri,fi;
    int n=snprintf(path,sizeof path,"%s/GDCC___.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
    TRY(lt48_owner_rez_open_internal(path,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));
    snprintf(name,sizeof name,"P%x",pinmap);lt48_pin_code pc;
    TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof name,"M%x",family);lt48_family_info f;
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count!=5 || f.operation_count!=20){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<5;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    unsigned apps=0;
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
        if(w.wave_type==0 && op.operation_id<12 && !r->wave[op.operation_id]) {
            lt48_compact_wave *c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));
            st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,5,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
            unsigned defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
            if(defaults!=(family==241 && (op.operation_id==2 || op.operation_id==4)?1u:0u)){st=LT48_SAFETY_REJECTED;goto fail;}
            if(op.operation_id==11)for(unsigned part=0;part<2;part++)TRY(lt48_usb51_compile_fragment(&w,part,r->pins,r->vars,&r->wave[12+part]));
        } else if(w.wave_type==6 && apps<7) {
            unsigned expected=op.operation_id==16?0x5000:op.operation_id==23?0x9000:op.operation_id==24?0x9800:op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0;
            if(!expected || w.load_address!=expected){st=LT48_FORMAT_ERROR;goto fail;}
            r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,w.load_address};
        } else if(w.wave_type==4 && op.operation_id==30 && w.data_size==2780 && w.data[0]==0xda && w.data[1]==0x0a)r->fpga=w.data+2;
        else {st=LT48_UNSUPPORTED;goto fail;}
    }
    if(apps!=7 || !r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<14;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_usb51_close(r);return st;
#undef TRY
}

lt48_status lt48_usb51_address(const lt48_usb51_resources *r,uint32_t image,uint32_t *wire,uint32_t *remaining,unsigned *region) {
    if(!r || !wire || !remaining || !region)return LT48_INVALID_ARGUMENT;
    if(image>=65536)return LT48_SAFETY_REJECTED;
    unsigned base,end;
    if(image<r->flash){base=0;end=r->flash;*region=0;}
    else if(image<0xf000){base=r->flash;end=0xf000;*region=1;}
    else if(image<0xf400){base=0xf000;end=0xf400;*region=2;}
    else {base=0xf400;end=65536;*region=3;}
    *wire=image-base;*remaining=end-image;return LT48_OK;
}

/* Split the original two commits so extra-row data can be read back before
 * applying irreversible lock changes. Every copied instruction is hash pinned. */
lt48_status lt48_usb51_compile_fragment(const lt48_wave_code *source,unsigned part,
    const lt48_pin_map *pins,const lt48_compile_variable *vars,lt48_compiled_wave **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!source || !pins || !vars || part>1)return LT48_INVALID_ARGUMENT;
    if(source->wave_type || !source->data || source->data_size!=269)return LT48_SAFETY_REJECTED;
    unsigned char digest[32];char hash[65];lt48_sha256(source->data,source->data_size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hash+2*i,3,"%02x",digest[i]);
    if(strcmp(hash,"e918d3b4918ddfb86b07d5186bb06e25c8a1986a892be21df0c41d609b18f320"))return LT48_SAFETY_REJECTED;
    unsigned char raw[269];size_t n=2;
    if(!part){memcpy(raw+2,source->data+2,0x65-2);n=0x65;}
    else {
        /* Both commits require the original tPW -> delay buffer calculation. */
        memcpy(raw+n,source->data+0x20,0x2b-0x20);n+=0x2b-0x20;
        memcpy(raw+n,source->data+0x65,269-0x65);n+=269-0x65;
    }
    raw[0]=(unsigned char)(n-2);raw[1]=(unsigned char)((n-2)>>8);
    lt48_compact_wave *c=NULL;lt48_status st=lt48_compact_decode(raw,n,&c);
    if(st==LT48_OK)st=lt48_compact_compile(c,pins,vars,5,1,0,out);
    lt48_compact_wave_close(c);return st;
}
