#include "at89_ed2.h"
#include <string.h>
lt48_status lt48_ed2_layout_for(unsigned d,lt48_ed2_layout *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    if(d<0x148 || d>0x14c)return LT48_UNSUPPORTED;
    unsigned i=d-0x148;
    *out=(lt48_ed2_layout){d,234+i,i==0?109:i<3?110:i==3?111:112,
        i>=3?2:i==2?4:3,i>=3?4:3,i>=3?69632:67584,i>=3};
    return LT48_OK;
}
lt48_status lt48_ed2_address(const lt48_ed2_layout *l,uint32_t image,uint32_t *wire,uint32_t *remaining) {
    if(!l || !wire || !remaining)return LT48_INVALID_ARGUMENT;
    if(image>=l->capacity)return LT48_SAFETY_REJECTED;
    if(image<65536){*wire=image;*remaining=65536-image;}
    else if(image<67584){*wire=image-65536;*remaining=67584-image;}
    else {*wire=image-67584+0x8000;*remaining=l->capacity-image;}
    return LT48_OK;
}
static unsigned flag_bit(const lt48_ed2_layout *l,unsigned i) {
    static const unsigned ed[]={3,6,7},id[]={3,5,6,7},cc[]={6,7};
    return l->cc03?cc[i]:l->flags==4?id[i]:ed[i];
}
lt48_status lt48_ed2_decode(const lt48_ed2_layout *l,unsigned hsb,const unsigned char *extra,unsigned char *out) {
    if(!l || !extra || !out || hsb>255)return LT48_INVALID_ARGUMENT;
    unsigned low=hsb&7,lock=low==7?0:low==6?1:low==4?2:low==0?3:4;
    if(lock==4)return LT48_DEVICE_MISMATCH;
    for(unsigned i=0;i<l->flags;i++)out[i]=(unsigned char)(hsb>>flag_bit(l,i)&1);
    out[l->flags]=(unsigned char)lock;
    memcpy(out+l->flags+1,extra,l->numbers);
    return LT48_OK;
}
lt48_status lt48_ed2_encode(const lt48_ed2_layout *l,unsigned previous,const unsigned char *fields,unsigned *hsb) {
    if(!l || !fields || !hsb || previous>255)return LT48_INVALID_ARGUMENT;
    unsigned lock=fields[l->flags];
    if(lock>3)return LT48_INVALID_ARGUMENT;
    unsigned value=l->cc03?0x38:(previous&0x10)|(l->flags==3?0x20:0);
    static const unsigned locks[]={7,6,4,0};value|=locks[lock];
    for(unsigned i=0;i<l->flags;i++) {
        if(fields[i]>1)return LT48_INVALID_ARGUMENT;
        value|=(unsigned)fields[i]<<flag_bit(l,i);
    }
    *hsb=value;return LT48_OK;
}

#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
void lt48_ed2_close(lt48_ed2_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<15;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
lt48_status lt48_ed2_open(const lt48_installation *x,size_t device,lt48_ed2_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_ed2_layout layout;
    lt48_status st=lt48_installation_resolve_device(x,device,&spec);
    if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
    st=lt48_ed2_layout_for(spec.definition_resource,&layout);if(st!=LT48_OK)return st;
    if(spec.family_code!=layout.family || spec.pinmap!=layout.pinmap)return LT48_SAFETY_REJECTED;
    lt48_ed2_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    r->layout=layout;r->manufacturer=spec.catalogue.manufacturer_id;r->device=spec.catalogue.device_id;
    char path[4096],name[24];size_t ri,fi;
    int n=snprintf(path,sizeof path,"%s/GDCC___.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
    TRY(lt48_owner_rez_open_internal(path,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));
    snprintf(name,sizeof name,"P%x",layout.pinmap);lt48_pin_code pc;
    TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof name,"M%x",layout.family);lt48_family_info f;
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count!=(layout.cc03?5u:6u) || f.operation_count!=17){st=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=f.variable_count;
    for(unsigned i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    unsigned apps=0;
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
        if(w.wave_type==0 && op.operation_id<12 && !r->wave[op.operation_id]) {
            lt48_compact_wave *c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));
            st=lt48_compact_compile(c,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
            if(op.operation_id==2)TRY(lt48_ed2_compile_fragment(&w,LT48_ED2_PROGRAM_INIT,r->pins,r->vars,r->var_count,&r->wave[12]));
            if(op.operation_id==11)for(unsigned part=1;part<3;part++)TRY(lt48_ed2_compile_fragment(&w,part,r->pins,r->vars,r->var_count,&r->wave[12+part]));
        } else if(w.wave_type==6 && apps<4) {
            unsigned expected=op.operation_id==17?0x6000:op.operation_id==19?0x7800:op.operation_id==20?0x8000:op.operation_id==21?0x7000:0;
            if(!expected || w.load_address!=expected){st=LT48_FORMAT_ERROR;goto fail;}
            r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,w.load_address};
        } else if(w.wave_type==4 && op.operation_id==30 && w.data_size==2780 && w.data[0]==0xda && w.data[1]==0x0a)r->fpga=w.data+2;
        else {st=LT48_UNSUPPORTED;goto fail;}
    }
    if(apps!=4 || !r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<15;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_ed2_close(r);return st;
#undef TRY
}

lt48_status lt48_ed2_compile_fragment(const lt48_wave_code *source,enum lt48_ed2_fragment part,
    const lt48_pin_map *pins,const lt48_compile_variable *vars,size_t count,lt48_compiled_wave **out) {
    static const struct {const char *hash;unsigned size,split,unlock,end;} accepted[]={
        {"b672a8d5c6da1a8d573a0da4ee1da9bd43778d2464888577db4687e50c556123",247,0xae,0xdc,0xf6},
        {"6181b378f9ece1193fcf713f4360327e422a90a3fcc380ed7c07a3df993f3e35",152,0x5e,0x7d,0x97},
        {"39cfd51a1e3f39f0c13e82e9aee78718f3d73b3bb5318c0a21653840ade288d2",293,0x6a,0x1c,293},
        {"46adff60dc2ac9a01d46b502f6abfa2bd430883dfa320f2e53050a9c9d4fde1b",312,0x6a,0x1c,312},
        {"87cb4d198e0a676975cc9b395c91cee3479f5e42f782d3af46ca174b6a701118",269,0x80,0x1c,269}
    };
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!source || !pins || !vars || part<0 || part>LT48_ED2_HSB_WRITE)return LT48_INVALID_ARGUMENT;
    if(source->wave_type || !source->data)return LT48_SAFETY_REJECTED;
    unsigned char digest[32];char hash[65];lt48_sha256(source->data,source->data_size,digest);
    for(unsigned i=0;i<32;i++)snprintf(hash+2*i,3,"%02x",digest[i]);
    unsigned match=5;
    for(unsigned i=0;i<5;i++)if(source->data_size==accepted[i].size && !strcmp(hash,accepted[i].hash))match=i;
    if(match==5 || (part==LT48_ED2_PROGRAM_INIT)!=(match<2))return LT48_SAFETY_REJECTED;
    unsigned char raw[312];size_t n=2;
#define COPY(a,b) do{size_t length=(b)-(a);memcpy(raw+n,source->data+(a),length);n+=length;}while(0)
    if(part==LT48_ED2_PROGRAM_INIT) {
        /* Original entry + unlock tail, excluding conditional chip erase. */
        COPY(2,accepted[match].split);COPY(accepted[match].unlock,accepted[match].end);
    } else if(part==LT48_ED2_EXTRA_WRITE) {
        /* Extra row commit only. HSB/locks require an independent later step. */
        COPY(2,accepted[match].split);
    } else {
        /* Original unlock prefix, then HSB computation and commit. */
        COPY(2,accepted[match].unlock);COPY(accepted[match].split,accepted[match].end);
    }
#undef COPY
    raw[0]=(unsigned char)(n-2);raw[1]=(unsigned char)((n-2)>>8);
    lt48_compact_wave *c=NULL;lt48_status st=lt48_compact_decode(raw,n,&c);
    if(st==LT48_OK)st=lt48_compact_compile(c,pins,vars,count,1,0,out);
    lt48_compact_wave_close(c);return st;
}

uint32_t lt48_ed2_capacity(const lt48_ed2_resources *r){return r?r->layout.capacity:0;}
unsigned lt48_ed2_configuration_count(const lt48_ed2_resources *r){return r?r->layout.flags+1+r->layout.numbers:0;}
unsigned lt48_ed2_configuration_flags(const lt48_ed2_resources *r){return r?r->layout.flags:0;}
