#include "at89_tail.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void lt48_at89_tail_close(lt48_at89_tail_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<17;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
lt48_status lt48_at89_tail_open(const lt48_installation *x,size_t device,lt48_at89_tail_resources **out) {
    static const struct {
        unsigned d,m,p,variables,operations;
        uint32_t count,aux;
        unsigned wave[22]; /* zero is also a real source wave: use operation mask */
        uint32_t mask;
    } closures[]={
        {0x116,161,71,1,12,4096,4096,
         {[0]=1,[1]=2,[2]=355,[3]=356,[4]=357,[6]=359,[8]=360,[10]=358,[12]=361,[13]=362,[15]=364,[16]=363},
         (1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<6)|(1u<<8)|(1u<<10)|(1u<<12)|(1u<<13)|(1u<<15)|(1u<<16)},
        {0xf1,125,55,3,12,69632,69760,
         {[0]=1,[1]=2,[2]=254,[3]=255,[4]=257,[5]=256,[6]=258,[8]=260,[10]=259,[12]=261,[17]=262,[21]=263},
         (1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<5)|(1u<<6)|(1u<<8)|(1u<<10)|(1u<<12)|(1u<<17)|(1u<<21)},
        {0xee,123,52,6,10,32768,32768,
         {[0]=0,[1]=2,[2]=245,[3]=246,[4]=247,[6]=248,[8]=250,[10]=249,[12]=251,[21]=8},
         (1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<6)|(1u<<8)|(1u<<10)|(1u<<12)|(1u<<21)},
        MCU70_IB2_CLOSURE
    };
    if(!out || !x)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,device,&spec);
    if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2018 || strcmp(spec.definition_stem,"GAF____"))return LT48_UNSUPPORTED;
    size_t which=0;for(;which<4;which++)if(spec.definition_resource==closures[which].d)break;
    if(which==4 || (which==3 && !mcu70_ib2_spec(&spec)))return LT48_UNSUPPORTED;
    if(spec.family_code!=closures[which].m || spec.pinmap!=closures[which].p)return LT48_SAFETY_REJECTED;
    lt48_at89_tail_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    r->definition=spec.definition_resource;r->family=spec.family_code;r->pinmap=spec.pinmap;
    char path[4096],name[24];size_t index,fi;lt48_device_definition definition;
    int n=snprintf(path,sizeof path,"%s/GAF____.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof path){status=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{status=(x);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_owner_rez_open_internal(path,"126dcd7dd7252b5af78e151d34106f0c17b5e52575b9606a80b9f615f912c123",&r->rez));
    snprintf(name,sizeof name,"D%x",r->definition);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_device_definition(r->rez,index,&definition));
    if(definition.kind!=LT48_DEVICE_DEFINITION_DOUBLE_BUFFER_MEMORY ||
       !definition.has_element_geometry || !definition.has_secondary_element_geometry ||
       definition.element_count!=closures[which].count || definition.element_bits!=8 ||
       definition.auxiliary_value!=closures[which].aux || definition.secondary_element_count ||
       definition.secondary_element_bits!=8){status=LT48_SAFETY_REJECTED;goto fail;}
    r->primary_count=definition.element_count;r->auxiliary_value=definition.auxiliary_value;
    lt48_pin_code pin;snprintf(name,sizeof name,"P%x",r->pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pin));TRY(lt48_pin_map_parse(&pin,&r->pins));
    lt48_family_info family;snprintf(name,sizeof name,"M%x",r->family);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(family.variable_count!=closures[which].variables || family.operation_count!=closures[which].operations){status=LT48_FORMAT_ERROR;goto fail;}
    r->variable_count=family.variable_count;
    for(size_t i=0;i<r->variable_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    }
    uint32_t seen=0;
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        unsigned id=op.operation_id;
        if(id>=22 || !(closures[which].mask&(1u<<id)) || (seen&(1u<<id)) || op.wave!=closures[which].wave[id]){status=LT48_FORMAT_ERROR;goto fail;}
        seen|=1u<<id;snprintf(name,sizeof name,"W%x",op.wave);
        TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(id==21) {
            if(code.wave_type!=4 || code.data_size!=2780 || code.data[0]!=0xda || code.data[1]!=0x0a){status=LT48_FORMAT_ERROR;goto fail;}
            r->fpga=code.data+2;
        } else if(id==17) {
            if(code.wave_type!=6 || code.load_address!=0x4100 || code.data_size!=29){status=LT48_FORMAT_ERROR;goto fail;}
            r->read_applet=code;
        } else {
            if(id>=17 || code.wave_type){status=LT48_FORMAT_ERROR;goto fail;}
            lt48_compact_wave *compact=NULL;TRY(lt48_compact_decode(code.data,code.data_size,&compact));
            status=lt48_compact_compile(compact,r->pins,r->vars,r->variable_count,1,0,&r->wave[id]);
            lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
            if(lt48_compiled_wave_defaulted_pin_count(r->wave[id])){status=LT48_SAFETY_REJECTED;goto fail;}
        }
    }
    if(seen!=closures[which].mask){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_at89_tail_close(r);return status;
#undef TRY
}
