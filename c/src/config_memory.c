#include <labtool48/labtool48.h>
#include <labtool48/families_generated.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Dataman 13.10 class 2010 / 2026: exact compact source waves and host
 * stream ownership. No electrical procedure is inferred from device names. */
struct lt48_config_memory_resources {
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[64];
    lt48_compile_variable vars[64]; size_t var_count;
    unsigned char *idle; size_t idle_size;
    unsigned capacity,width,kind,family,definition,nominal,check_id,cfgmask;
    unsigned margins[3],margin_count;
};
enum { ATT=1, XILINX, EPC, EPC2 };
static uint32_t u32(const unsigned char *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put32(unsigned char *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
void lt48_config_memory_close(lt48_config_memory_resources *r){if(!r)return;for(unsigned i=0;i<64;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);}
uint32_t lt48_config_memory_capacity(const lt48_config_memory_resources*r){return r?r->capacity:0;}
unsigned lt48_config_memory_configuration_mask(const lt48_config_memory_resources*r){return r?r->cfgmask:0;}
unsigned lt48_config_memory_configuration_bits(const lt48_config_memory_resources*r){unsigned n=0,m=r?r->cfgmask:0;while(m){n+=m&1;m>>=1;}return n;}
const char *lt48_config_memory_profile_internal(const lt48_config_memory_resources*r){return r->kind==ATT?LT48_FAMILY_ATT_CONFIG_PROFILE:r->kind==XILINX?LT48_FAMILY_XILINX_CONFIG_PROFILE:r->kind==EPC?LT48_FAMILY_EPC_OTP_PROFILE:LT48_FAMILY_EPC2_PROFILE;}
unsigned lt48_config_memory_mask_internal(const lt48_config_memory_resources*r){return r->kind==ATT?LT48_FAMILY_ATT_CONFIG_OPERATION_MASK:r->kind==XILINX?LT48_FAMILY_XILINX_CONFIG_OPERATION_MASK:r->kind==EPC?LT48_FAMILY_EPC_OTP_OPERATION_MASK:LT48_FAMILY_EPC2_OPERATION_MASK;}
lt48_status lt48_config_memory_open(const lt48_installation*x,size_t device,lt48_config_memory_resources**out){
    if(!x||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    lt48_device_spec d;lt48_status st=lt48_installation_resolve_device(x,device,&d);if(st!=LT48_OK)return st;
    unsigned kind=0;
#define ADMITTED(n) lt48_family_##n##_admitted(d.definition_resource,d.family_code,d.pinmap,d.capacity_bytes,d.element_bits)
    if(d.catalogue.device_class==2010&&!strcmp(d.definition_stem,"GAB____")){if(ADMITTED(att_config))kind=ATT;else if(ADMITTED(xilinx_config))kind=XILINX;}
    if(d.catalogue.device_class==2026&&!strcmp(d.definition_stem,"GC_____")){if(ADMITTED(epc_otp))kind=EPC;else if(ADMITTED(epc2))kind=EPC2;}
#undef ADMITTED
    if(!kind)return LT48_UNSUPPORTED;
    lt48_config_memory_resources*r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    r->kind=kind;r->family=d.family_code;r->definition=d.definition_resource;r->capacity=d.capacity_bytes;r->width=d.element_bits/8;
    r->cfgmask=kind==ATT?1:kind==XILINX?(1u<<21):0;
    if(kind==XILINX&&r->family<0xb0){r->cfgmask|=1u<<22;if(r->family==0xa7||r->family==0xa8||r->family==0xa9||r->family==0xac||r->family==0xad)r->cfgmask|=1u<<20;}
    char path[4096],name[32];size_t at,fi;lt48_family_info f;lt48_pin_code pc;
#define TRY(expr) do{st=(expr);if(st!=LT48_OK)goto fail;}while(0)
    int n=snprintf(path,sizeof(path),"%s/%s.rez",lt48_installation_path(x),d.definition_stem);if(n<0||(size_t)n>=sizeof(path)){st=LT48_INVALID_ARGUMENT;goto fail;}
    TRY(lt48_owner_rez_open_internal(path,kind<=XILINX?LT48_FAMILY_ATT_CONFIG_SOURCE_SHA256:LT48_FAMILY_EPC_OTP_SOURCE_SHA256,&r->rez));
    snprintf(name,sizeof(name),"P%x",d.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&at));TRY(lt48_rez_get_pin_code(r->rez,at,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));TRY(lt48_pin_map_complete_groups_internal(r->pins,32));
    snprintf(name,sizeof(name),"M%x",d.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count>64){st=LT48_FORMAT_ERROR;goto fail;}r->var_count=f.variable_count;
    for(size_t i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};if(r->vars[i].variable_id==0)r->nominal=r->vars[i].value;}
    /* Use declared source margins, not invented part-family limits. */
    for(unsigned id=kind>=EPC?11:8;id<=(kind>=EPC?12:10);id++)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==id&&r->vars[i].value)r->margins[r->margin_count++]=r->vars[i].value;
    if(!r->margin_count)r->margins[r->margin_count++]=r->nominal;
    lt48_family_operation_options opts={0};st=lt48_rez_get_family_operation_options(r->rez,fi,&opts);if(st!=LT48_OK&&st!=LT48_NOT_FOUND)goto fail;r->check_id=!!(opts.checkbox_values&2);
    for(size_t i=0;i<f.operation_count;i++){
        lt48_family_operation op;lt48_wave_code w;lt48_compact_wave*c=NULL;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(op.operation_id>=64||r->wave[op.operation_id]){st=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&at));TRY(lt48_rez_get_wave_code(r->rez,at,&w));if(w.wave_type){st=LT48_UNSUPPORTED;goto fail;}
        /* New serial-read waves finish in Shift-DR. Their original reset
         * omits the final TMS-high clock; GetConf also omits that edge on
         * its 32-bit user-code scan. These bounded, digest-bound corrections
         * are verified by the independent TAP test (see the audit). */
        unsigned char corrected[1024];
        if(kind==XILINX&&d.family_code>=0xac&&op.operation_id==34){
            if((op.wave!=0x171&&op.wave!=0x17b)||w.data_size+3>sizeof(corrected)||w.data_size<0x32||memcmp(w.data+0x2d,"\x25\x19\x01\x00\x01",5)){st=LT48_FORMAT_ERROR;goto fail;}
            memcpy(corrected,w.data,0x2d);memcpy(corrected+0x2d,"\x11\x18\x01",3);memcpy(corrected+0x30,w.data+0x2d,w.data_size-0x2d);w.data_size+=3;corrected[0]=(unsigned char)(w.data_size-2);corrected[1]=(unsigned char)((w.data_size-2)>>8);w.data=corrected;
        }
        if(kind==XILINX&&d.family_code>=0xac&&op.operation_id==3){
            if(op.wave!=0x16c&&op.wave!=0x176){st=LT48_FORMAT_ERROR;goto fail;}
            static const unsigned char reset[]={0,0,
                0x5e,4,0,1,0,0,0, 0x11,24,1,0x2b,25,1,0x3d,30,0x3d,34,
                0x5f,0x5e,4,0,4,0,0,0, 0x11,24,1,0x2b,25,1,0x3d,30,
                0x5f,0x5e,4,0,11,0,0,0,0x11,24,1,0x2b,25,1,0x3d,30,
                0x60,0x60,0x60,0x3d,32};
            memcpy(corrected,reset,sizeof(reset));corrected[0]=sizeof(reset)-2;w.data=corrected;w.data_size=sizeof(reset);
        }
        TRY(lt48_compact_decode(w.data,w.data_size,&c));st=lt48_compact_compile(c,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
        if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
    }
    for(unsigned i=0;i<6;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_config_memory_close(r);return st;
#undef TRY
}
lt48_status lt48_config_memory_validate(const lt48_config_memory_resources*r,lt48_operation_kind op,uint32_t start,size_t n,const unsigned char*image,size_t size){
    if(!r)return LT48_INVALID_ARGUMENT;
    if(op<1||op>LT48_OPERATION_KIND_COUNT||!(lt48_config_memory_mask_internal(r)&(1u<<(op-1))))return LT48_UNSUPPORTED;
    if(start||n!=r->capacity)return LT48_SAFETY_REJECTED;
    if(op>=3&&op<=6&&(!image||size!=n))return LT48_INVALID_ARGUMENT;
    if(op==LT48_OPERATION_CONFIGURATION&&(!image||size!=8||(u32(image)&~r->cfgmask)||(r->kind==ATT&&u32(image+4))))return LT48_INVALID_ARGUMENT;
    /* Protection has its own explicit operation. Configuration may not enable it. */
    if(op==LT48_OPERATION_CONFIGURATION&&(u32(image)&(1u<<21)))return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
lt48_status lt48_config_memory_validate_verify_options(const lt48_config_memory_resources*r,lt48_operation_kind op,lt48_verify_margin_selection m,unsigned type){
    (void)op;if(!r||type>1||(m!=LT48_VERIFY_MARGIN_ALL&&m!=LT48_VERIFY_MARGIN_NOMINAL&&m!=LT48_VERIFY_MARGIN_LOW&&m!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;return LT48_OK;
}
typedef struct {
    const lt48_config_memory_resources*r;const lt48_protocol_transport*api;void*context;lt48_runtime_transport_session*wire;
    lt48_compile_variable vars[64];uint32_t volts[4];unsigned char buf[32][32];unsigned mode;
    lt48_runtime_configuration_value cfg[16];size_t cfg_count;
} session;
static void setcfg(session*s,int obj,unsigned cat,unsigned field,unsigned value){for(size_t i=0;i<s->cfg_count;i++)if(s->cfg[i].object_id==obj&&s->cfg[i].category==cat&&s->cfg[i].field==field){s->cfg[i].value=value;return;}if(s->cfg_count<16)s->cfg[s->cfg_count++]=(lt48_runtime_configuration_value){obj,cat,field,value};}
static unsigned getcfg(session*s,unsigned cat,unsigned field){for(size_t i=0;i<s->cfg_count;i++)if(s->cfg[i].object_id==0&&s->cfg[i].category==cat&&s->cfg[i].field==field)return s->cfg[i].value;return UINT32_MAX;}
static lt48_status run(session*s,unsigned op){
    if(op>=64||!s->r->wave[op])return LT48_UNSUPPORTED;
    lt48_runtime_options o={0};lt48_runtime_nested_wave nested[64];size_t nn=0;lt48_runtime_buffer_seed seed[31];size_t ns=0;unsigned mb=s->r->kind>=EPC?10:4;
    for(unsigned i=0;i<64;i++)if(s->r->wave[i])nested[nn++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};
    for(unsigned i=0;i<32;i++)if(i!=mb)seed[ns++]=(lt48_runtime_buffer_seed){i,s->buf[i],32};
    o.mode=s->mode;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=mb;o.parameter_buffer_id=s->r->kind>=EPC?5:1;o.initial_buffers=seed;o.initial_buffer_count=ns;o.nested_waves=nested;o.nested_wave_count=nn;o.configuration=s->cfg;o.configuration_count=s->cfg_count;o.root_timeout_seconds=45;memcpy(o.initial_voltage_centivolts,s->volts,sizeof(s->volts));
    return lt48_owner_run_mutable_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&o,NULL,0,s->buf,s->cfg,16,&s->cfg_count,s->volts);
}
static lt48_status start(session*s,unsigned mode,unsigned vcc){
    memset(s->buf,0,sizeof(s->buf));s->mode=mode;
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status st=run(s,0);for(unsigned i=0;st==LT48_OK&&i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);if(st==LT48_OK)st=run(s,2);return st;
}
static lt48_status end(session*s,lt48_status st){if(st!=LT48_OK){s->mode=1;return st;}st=run(s,3);return st==LT48_OK?run(s,1):st;}
static void config_set(session*s,unsigned flags,uint32_t usercode){
    if(s->r->kind==ATT)setcfg(s,0,1,0,flags);
    if(s->r->kind==XILINX){for(unsigned i=20;i<=22;i++)setcfg(s,0,0,i,(flags>>i)&1);setcfg(s,0,2,1,usercode&65535);setcfg(s,0,2,2,usercode>>16);}
}
static lt48_status read_config(session*s,lt48_config_memory_result*out){
    if(!s->r->cfgmask)return LT48_OK;
    lt48_status st=start(s,1,s->r->nominal);st=end(s,st);if(st!=LT48_OK)return st;
    unsigned flags=0;if(s->r->kind==ATT){flags=getcfg(s,1,0);if(flags>1)return LT48_RUNTIME_ERROR;}
    else{for(unsigned i=20;i<=22;i++)if(s->r->cfgmask&(1u<<i)){unsigned v=getcfg(s,0,i);if(v>1)return LT48_RUNTIME_ERROR;flags|=v<<i;}unsigned lo=getcfg(s,2,1),hi=getcfg(s,2,2);if(lo>65535||hi>65535)return LT48_RUNTIME_ERROR;out->usercode=lo|(hi<<16);}
    out->configuration=flags;out->configuration_valid=1;return LT48_OK;
}
static unsigned byte_mask(const lt48_config_memory_resources*r,unsigned a){
    if(r->kind==EPC2&&a==3)return 0xbf;
    if(r->kind==EPC&&(r->definition==6||r->definition==7||r->definition==14||r->definition==15)&&a==r->capacity-1)return 15;
    return 255;
}
static void address(session*s,unsigned a){
    put32(s->buf[0],a/s->r->width);
    /* TPofGroup's tag-7 outer coordinate is _BUF_Col, inner is _BUF_Row. */
    if(s->r->kind==EPC&&s->r->definition>=8&&s->r->definition<=13){put32(s->buf[2],a/512);put32(s->buf[12],a%512);put32(s->buf[9],255);}
}
static lt48_status scan(session*s,lt48_operation_kind op,const unsigned char*expect,unsigned char*dest,unsigned vcc,lt48_config_memory_result*out){
    lt48_status st=start(s,op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_READ?1:4,vcc);unsigned din=s->r->kind>=EPC?5:1,dout=s->r->kind>=EPC?4:2;
    for(unsigned a=0;st==LT48_OK&&a<s->r->capacity;a+=s->r->width){
        address(s,a);memset(s->buf[dout],255,32);if(expect)memcpy(s->buf[dout],expect+a,s->r->width);
        st=run(s,4);if(st!=LT48_OK)break;
        for(unsigned b=0;b<s->r->width;b++){unsigned mask=byte_mask(s->r,a+b),v=s->buf[din][b];if(dest)dest[a+b]=(unsigned char)(v|(~mask&255));if(op!=LT48_OPERATION_READ&&((v^(expect?expect[a+b]:255))&mask)){if(!out->mismatch_count)out->first_mismatch=a+b;out->mismatch_count++;}}
        out->completed_bytes=a+s->r->width;
        if(s->api->progress&&!(a&1023))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,out->completed_bytes,s->r->capacity,0,0,0);
        if(out->mismatch_count&&op!=LT48_OPERATION_COMPARE)st=LT48_DEVICE_MISMATCH;
    }
    st=end(s,st);return st==LT48_OK&&out->mismatch_count?LT48_DEVICE_MISMATCH:st;
}
static lt48_status program(session*s,const unsigned char*image,lt48_config_memory_result*out){
    lt48_status st=start(s,2,s->r->nominal);unsigned dout=s->r->kind>=EPC?4:2,din=s->r->kind>=EPC?5:1;
    for(unsigned a=0;st==LT48_OK&&a<s->r->capacity;a+=s->r->width){
        address(s,a);memcpy(s->buf[dout],image+a,s->r->width);memcpy(s->buf[din],image+a,s->r->width);st=run(s,5);
        /* EPC2 has a status register at each committed 64-bit word. */
        if(st==LT48_OK&&s->r->kind==EPC2&&(a%8)==7&&u32(s->buf[16])!=0)st=LT48_DEVICE_MISMATCH;
        out->completed_bytes=a+s->r->width;if(s->api->progress&&!(a&1023))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,out->completed_bytes,s->r->capacity,0,0,0);
    }
    /* Original _FUN_POSTPROG restores EPC2's first word with its PDB bit. */
    if(st==LT48_OK&&s->r->kind==EPC2){st=run(s,6);if(st==LT48_OK&&u32(s->buf[16])!=0)st=LT48_DEVICE_MISMATCH;}
    return end(s,st);
}
static lt48_status write_config(session*s,unsigned flags,uint32_t code,lt48_config_memory_result*out){
    config_set(s,flags,code);lt48_status st=start(s,s->r->kind==ATT?2:8,s->r->nominal);
    if(st==LT48_OK&&s->r->kind==XILINX)st=run(s,7);
    st=end(s,st);
    if(st==LT48_OK)st=read_config(s,out);
    if(st==LT48_OK&&(out->configuration!=flags||(s->r->kind==XILINX&&out->usercode!=code)))st=LT48_DEVICE_MISMATCH;
    return st;
}
static lt48_status cleanup_wave(void*p,unsigned op){return run(p,op);}
lt48_status lt48_config_memory_execute(const lt48_config_memory_resources*r,const lt48_protocol_transport*api,void*context,lt48_operation_kind op,uint32_t offset,size_t n,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_config_memory_result*out){
    if(!api||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_config_memory_validate(r,op,offset,n,image,size);if(st!=LT48_OK)return st;st=lt48_config_memory_validate_verify_options(r,op,margin,type);if(st!=LT48_OK)return st;
    if(op==LT48_OPERATION_READ&&(!read||read_size<n))return LT48_BUFFER_TOO_SMALL;
    unsigned char*pending=malloc(n);if(!pending)return LT48_NO_MEMORY;
    session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;memcpy(s.vars,r->vars,sizeof(s.vars));setcfg(&s,-1,0,1,r->check_id);setcfg(&s,-1,0,5,0);
    st=lt48_runtime_transport_session_create(api,context,&s.wire);if(st!=LT48_OK)goto done;
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};st=lt48_runtime_transport_session_wave_watchdog(s.wire,45000,1);
    if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
    if(st==LT48_OK&&op!=LT48_OPERATION_ERASE)st=read_config(&s,out);
    unsigned flags=out->configuration;uint32_t code=out->usercode;
    if(st==LT48_OK&&r->kind==XILINX&&(flags&(1u<<21))&&op!=LT48_OPERATION_SECURE)st=LT48_SAFETY_REJECTED;
    int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK&&op==LT48_OPERATION_ERASE){
        st=start(&s,7,r->nominal);if(st==LT48_OK)st=run(&s,r->kind==EPC2?7:6);
        if(st==LT48_OK&&r->kind==EPC2&&u32(s.buf[16])!=0)st=LT48_DEVICE_MISMATCH;
        st=end(&s,st);if(st==LT48_OK)st=scan(&s,LT48_OPERATION_BLANK_CHECK,NULL,NULL,r->nominal,out);if(st==LT48_OK)st=read_config(&s,out);
        if(st==LT48_OK&&r->kind==XILINX&&(out->configuration&(1u<<21)))st=LT48_DEVICE_MISMATCH;
    }else if(st==LT48_OK&&writing){
        if(op==LT48_OPERATION_PROGRAM)st=scan(&s,LT48_OPERATION_BLANK_CHECK,NULL,NULL,r->nominal,out);
        config_set(&s,flags,code);if(st==LT48_OK)st=program(&s,image,out);
        for(unsigned i=0;st==LT48_OK&&i<r->margin_count;i++)st=scan(&s,LT48_OPERATION_VERIFY,image,NULL,r->margins[i],out);
        if(st==LT48_OK&&r->kind==XILINX)st=write_config(&s,flags,code,out);
        if(st==LT48_OK)st=scan(&s,LT48_OPERATION_VERIFY,image,NULL,r->nominal,out);
    }else if(st==LT48_OK&&(op==LT48_OPERATION_CONFIGURATION||op==LT48_OPERATION_SECURE)){
        unsigned want=op==LT48_OPERATION_SECURE?flags|(1u<<21):u32(image);uint32_t wantcode=op==LT48_OPERATION_SECURE?code:u32(image+4);
        if(r->kind==XILINX&&((flags&~want)||(wantcode&~code)))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK&&!(flags&(1u<<21)))st=scan(&s,LT48_OPERATION_READ,NULL,pending,r->nominal,out);
        if(st==LT48_OK)st=write_config(&s,want,wantcode,out);
        if(st==LT48_OK&&!(want&(1u<<21)))st=scan(&s,LT48_OPERATION_VERIFY,pending,NULL,r->nominal,out);
    }else if(st==LT48_OK&&op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_NOMINAL){
        for(unsigned i=0;st==LT48_OK&&i<r->margin_count;i++)if(margin==LT48_VERIFY_MARGIN_ALL||(margin==LT48_VERIFY_MARGIN_LOW&&i==0)||(margin==LT48_VERIFY_MARGIN_HIGH&&i==r->margin_count-1))st=scan(&s,op,image,NULL,r->margins[i],out);
    }else if(st==LT48_OK)st=scan(&s,op,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,out);
    out->operation_status=st;s.mode=1;const unsigned off[]={1};st=lt48_owner_finish_internal(api,context,&s.wire,cleanup_wave,&s,off,1,st,&out->cleanup_status,&out->cleanup_completed);
    if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,n);
    if(st==LT48_OK)out->completed_bytes=n;
done:free(pending);return st;
}

static lt48_status pof_layout(const lt48_config_memory_resources*r,unsigned*tag,unsigned char h[12],const char**name){
    if(!r||r->kind<EPC)return LT48_UNSUPPORTED;
    memset(h,0,12);*tag=17;h[10]=1;
    switch(r->definition){
    case 6:case 7:*name="EPC1";put32(h+6,1046532);break;
    case 8:case 9:*name="EPC1064";*tag=7;h[4]=16;h[7]=2;h[8]=255;h[10]=0;break;
    case 10:case 11:case 12:case 13:*name="EPC1213";*tag=7;h[4]=52;h[7]=2;h[8]=255;h[10]=0;break;
    case 14:case 15:*name="EPC1441PC8";put32(h+6,440836);break;
    case 18:case 19:*name="EPC2";put32(h+6,1695744);break;
    default:return LT48_UNSUPPORTED;
    }
    return LT48_OK;
}
lt48_status lt48_config_memory_pof_import(const lt48_config_memory_resources*r,const unsigned char*d,size_t n,unsigned char*image,size_t count){
    if(!r||!d||!image||count!=r->capacity)return LT48_INVALID_ARGUMENT;
    unsigned tag;unsigned char h[12];const char*name;lt48_status st=pof_layout(r,&tag,h,&name);if(st!=LT48_OK)return st;
    if(n<12||memcmp(d,"POF\0",4)||u32(d+4)!=0x10000)return LT48_FORMAT_ERROR;
    size_t at=12;unsigned seen=0;const unsigned char*data=NULL;uint32_t records=u32(d+8);
    if(records>(n-12)/6)return LT48_FORMAT_ERROR;
    for(uint32_t i=0;i<records;i++){
        if(n-at<6)return LT48_FORMAT_ERROR;
        unsigned t=d[at]|(unsigned)d[at+1]<<8;size_t len=u32(d+at+2);at+=6;if(len>n-at)return LT48_FORMAT_ERROR;
        if(t==tag){if(data||len!=count+12||memcmp(d+at,h,12))return LT48_DEVICE_MISMATCH;data=d+at+12;}
        else if(t==2){if((seen&1)||len!=strlen(name)+1||memcmp(d+at,name,len))return LT48_DEVICE_MISMATCH;seen|=1;}
        else if(t==8){if((seen&2)||len!=2)return LT48_FORMAT_ERROR;seen|=2;}
        else if(t!=1&&t!=3&&t!=4&&t!=9&&t!=14&&t!=16)return LT48_UNSUPPORTED;
        at+=len;
    }
    if(at!=n||!data||!(seen&1))return LT48_FORMAT_ERROR;
    memcpy(image,data,count);return LT48_OK;
}
lt48_status lt48_config_memory_pof_export(const lt48_config_memory_resources*r,const unsigned char*image,size_t count,unsigned char*d,size_t capacity,size_t*size){
    if(!r||!image||!size||count!=r->capacity)return LT48_INVALID_ARGUMENT;
    unsigned tag;unsigned char h[12];const char*name;lt48_status st=pof_layout(r,&tag,h,&name);if(st!=LT48_OK)return st;
    size_t namesize=strlen(name)+1;*size=12+6+namesize+6+12+count+8;
    if(!d)return LT48_OK;
    if(capacity<*size)return LT48_BUFFER_TOO_SMALL;
    memset(d,0,*size);memcpy(d,"POF\0",4);put32(d+4,0x10000);put32(d+8,3);
    size_t at=12;d[at]=2;put32(d+at+2,(uint32_t)namesize);at+=6;memcpy(d+at,name,namesize);at+=namesize;
    d[at]=(unsigned char)tag;put32(d+at+2,(uint32_t)count+12);at+=6;memcpy(d+at,h,12);at+=12;memcpy(d+at,image,count);at+=count;
    d[at]=8;put32(d+at+2,2);return LT48_OK;
}
