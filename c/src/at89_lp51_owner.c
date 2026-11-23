#include "at89_lp51.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lt48_at89_lp51_resources {
    lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[15];
    lt48_compile_variable vars[7];lt48_wave_code applet[6];lt48_applet_address addresses[6];
    const unsigned char *fpga;unsigned char *idle;size_t idle_size;
    unsigned flash,signature,calibration,capacity,nominal,man,dev;
    unsigned flags,choices,settings,lock;
};
void lt48_at89_lp51_close(lt48_at89_lp51_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<15;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
unsigned lt48_at89_lp51_configuration_count(const lt48_at89_lp51_resources *r){return r?r->settings:0;}
uint32_t lt48_at89_lp51_capacity(const lt48_at89_lp51_resources *r){return r?r->capacity:0;}
lt48_status lt48_at89_lp51_open(const lt48_installation *x,size_t device,lt48_at89_lp51_resources **out) {
    if(!out || !x)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,device,&spec);
    if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___") ||
       spec.definition_resource<0x278 || spec.definition_resource>0x27c)return LT48_UNSUPPORTED;
    unsigned index=spec.definition_resource-0x278;
    unsigned pinmap=(index==0 || index==3)?233:234;
    if(spec.family_code!=477+index || spec.pinmap!=pinmap)return LT48_SAFETY_REJECTED;
    lt48_at89_lp51_resources *r=calloc(1,sizeof *r);if(!r)return LT48_NO_MEMORY;
    r->flash=65536;r->signature=r->flash+(index<3?4096:0);
    r->calibration=384;r->capacity=r->signature+512;r->nominal=500;
    r->flags=index==2?11:10;r->choices=index==2?5:4;
    r->settings=r->flags+r->choices;r->lock=r->flags+3;
    r->man=spec.catalogue.manufacturer_id;r->dev=spec.catalogue.device_id;
    char path[4096],name[24];size_t ri,fi;
    int n=snprintf(path,sizeof path,"%s/GDCC___.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
    TRY(lt48_owner_rez_open_internal(path,"c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640",&r->rez));
    snprintf(name,sizeof name,"P%x",pinmap);lt48_pin_code pc;
    TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_pin_code(r->rez,ri,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof name,"M%x",spec.family_code);lt48_family_info f;
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count!=7 || f.operation_count!=19){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<7;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    unsigned apps=0;
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&ri));TRY(lt48_rez_get_wave_code(r->rez,ri,&w));
        if(w.wave_type==0 && op.operation_id<12) {
            lt48_compact_wave *c=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&c));
            st=lt48_compact_compile_with_pin_policy(c,r->pins,r->vars,7,1,LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(st!=LT48_OK)goto fail;
            if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])!=(op.operation_id==4 || op.operation_id==10?1u:0u)){st=LT48_SAFETY_REJECTED;goto fail;}
            if(op.operation_id==11)for(unsigned part=0;part<2;part++)TRY(lt48_lp51_compile_fragment(&w,part,r->pins,r->vars,7,&r->wave[12+part]));
            if(op.operation_id==10)TRY(lt48_lp51_compile_fragment(&w,LT48_LP51_CHIP_ERASE,r->pins,r->vars,7,&r->wave[14]));
        } else if(w.wave_type==6 && apps<6){r->applet[apps]=w;r->addresses[apps++]=(lt48_applet_address){op.operation_id,w.load_address};}
        else if(w.wave_type==4 && op.operation_id==30 && w.data_size==2780)r->fpga=w.data+2;
        else {st=LT48_UNSUPPORTED;goto fail;}
    }
    if(apps!=6 || !r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<15;i++)if(!r->wave[i]){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_at89_lp51_close(r);return st;
#undef TRY
}
lt48_status lt48_at89_lp51_validate(const lt48_at89_lp51_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=r->capacity || length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if((op<1 || op>8) && op!=LT48_OPERATION_CONFIGURATION)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)
        if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_CONFIGURATION) {
        if(!image && !size)return LT48_OK;
        if(!image || size!=r->settings+2)return LT48_INVALID_ARGUMENT;
        unsigned mask=image[0]|image[1]*256u;
        if(mask>>r->settings)return LT48_INVALID_ARGUMENT;
        for(unsigned i=0;i<r->settings;i++)if((mask&(1u<<i)) && image[2+i]>(i<r->flags?1:3))return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(op==LT48_OPERATION_SECURE)return (!image && !size) || (image && size==1 && image[0]>=1 && image[0]<=3)?LT48_OK:LT48_INVALID_ARGUMENT;
    if(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE)
        if(!image || size!=length)return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}

typedef struct lp51_session {
    const lt48_at89_lp51_resources *r;const lt48_protocol_transport *api;void *ctx;
    lt48_runtime_transport_session *wire;lt48_compile_variable vars[7];
    unsigned char buffers[32][32];lt48_runtime_configuration_value config[16];size_t config_count;
    uint32_t volts[4];
    unsigned restore_fuses;unsigned char restore_settings[16];
} lp51_session;
static void address(lp51_session *s,unsigned a) {
    unsigned physical=a>=s->r->signature?a-s->r->signature:a>=s->r->flash?a-s->r->flash:a;
    for(unsigned i=0;i<4;i++) {
        s->buffers[0][i]=(unsigned char)(physical>>(8*i));
        s->buffers[1][i]=(unsigned char)(a>>(8*i));
    }
}
static lt48_status run_lp51(lp51_session *s,unsigned op) {
    lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[31];size_t n=0;
    for(unsigned i=0;i<32;i++)if(i!=8)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    o.mode=13;o.use_explicit_buffer_ids=1;o.address_buffer=0;o.mode_buffer=8;o.parameter_buffer_id=3;
    o.initial_buffers=seeds;o.initial_buffer_count=n;o.configuration=s->config;o.configuration_count=s->config_count;
    memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);o.root_timeout_seconds=10;
    lt48_status st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
    if(st==LT48_OK)st=lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,7,&o,s->r->addresses,6,s->buffers,s->config,16,&s->config_count,s->volts);
    return st;
}
static lt48_status transfer_lp51(lp51_session *s,int input,unsigned ep,unsigned char *p,size_t n) {
    if(s->api->cancelled && s->api->cancelled(s->ctx))return LT48_CANCELLED;
    size_t done=0;lt48_status st=input?s->api->read(s->ctx,(uint8_t)ep,p,n,&done):s->api->write(s->ctx,(uint8_t)ep,p,n,&done);
    return st==LT48_OK && done!=n?LT48_PROTOCOL_ERROR:st;
}
static lt48_status command_lp51(lp51_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    lt48_status st=lt48_runtime_transport_session_flush(s->wire);
    if(st==LT48_OK)st=transfer_lp51(s,0,2,(unsigned char *)p,n);
    if(st==LT48_OK && reply)st=transfer_lp51(s,1,0x88,reply,512);
    return st;
}
static lt48_status stop_lp51(lp51_session *s) {
    const unsigned char stop[]={0,0};unsigned char reply[512];
    lt48_status st=command_lp51(s,stop,sizeof stop,reply);
    return st==LT48_OK && !(reply[0]|reply[1])?LT48_PROTOCOL_ERROR:st;
}
static lt48_status block_lp51(lp51_session *s,int writing,unsigned char *data,size_t n) {
    unsigned char h[]={writing?0x48:0x43,(unsigned char)(n/512),(unsigned char)(n/512>>8),(unsigned char)(n%512),(unsigned char)(n%512>>8),writing?1:0},reply[512];
    lt48_status st=command_lp51(s,h,sizeof h,NULL);
    if(st==LT48_OK)st=transfer_lp51(s,!writing,writing?4:0x88,data,n);
    if(st==LT48_OK && writing) {
        st=transfer_lp51(s,1,0x88,reply,512);if(st==LT48_OK && !reply[0])st=LT48_PROTOCOL_ERROR;
        const unsigned char end[]={0x0c,1};if(st==LT48_OK)st=command_lp51(s,end,sizeof end,reply);
        if(st==LT48_OK && (!(reply[0]|reply[1]) || !(reply[2]&1)))st=LT48_DEVICE_MISMATCH;
    }
    return st;
}
static lt48_status read_lp51(lp51_session *s,unsigned start,size_t n,unsigned char *out,unsigned vcc) {
    /* Align arbitrary reads, then stream across the applet's 64-byte pages.
     * Bound every stream to one command space and the host's 4K transfer. */
    s->vars[0].value=vcc;lt48_status st=run_lp51(s,2);unsigned char block[4096];
    size_t done=0;
    while(st==LT48_OK && done<n) {
        unsigned a=start+(unsigned)done,base=a&~63u;
        unsigned end=a<s->r->flash?s->r->flash:a<s->r->signature?s->r->signature:s->r->capacity;
        size_t offset=a-base,take=sizeof block-offset;
        if(take>n-done)take=n-done;
        if(take>end-a)take=end-a;
        size_t wire=((offset+take+63)/64)*64;
        address(s,base);s->buffers[16][0]=64;st=run_lp51(s,5);
        if(st==LT48_OK)st=block_lp51(s,0,block,wire);
        if(st==LT48_OK)memcpy(out+done,block+offset,take);
        done+=take;
    }
    if(st==LT48_OK)st=stop_lp51(s);
    if(st==LT48_OK)st=run_lp51(s,3);
    s->vars[0].value=s->r->nominal;return st;
}
static void configure(lp51_session *s,const unsigned char *values) {
    s->config_count=s->r->settings;
    for(unsigned i=0;i<s->r->settings;i++)
        s->config[i]=(lt48_runtime_configuration_value){-1,i<s->r->flags?0:1,i<s->r->flags?i:i-s->r->flags,values?values[i]:UINT32_MAX};
}
static lt48_status settings_read(lp51_session *s,unsigned char out[16]) {
    lt48_status st=run_lp51(s,2);configure(s,NULL);
    if(st==LT48_OK)st=run_lp51(s,6);
    for(unsigned i=0;st==LT48_OK && i<s->r->settings;i++) {
        if(s->config[i].value>(i<s->r->flags?1u:3u))st=LT48_DEVICE_MISMATCH;
        else out[i]=(unsigned char)s->config[i].value;
    }
    if(st==LT48_OK)st=run_lp51(s,3);
    return st;
}
static lt48_status settings_write(lp51_session *s,unsigned row,const unsigned char expected[16]) {
    lt48_status st=run_lp51(s,2);configure(s,expected);
    if(st==LT48_OK)st=run_lp51(s,row==0?12:13);
    if(st==LT48_OK)st=run_lp51(s,3);
    unsigned char actual[16];if(st==LT48_OK)st=settings_read(s,actual);
    if(st==LT48_OK && memcmp(actual,expected,s->r->settings))st=LT48_DEVICE_MISMATCH;
    return st;
}
static lt48_status cleanup_lp51(void *v,unsigned phase) {
    lp51_session *s=v;
    if(phase==0){unsigned char reply[512];const unsigned char stop[]={0,0};lt48_status st=command_lp51(s,stop,2,reply);return st==LT48_OK && !(reply[0]|reply[1])?LT48_PROTOCOL_ERROR:st;}
    if(phase==1)return run_lp51(s,3);
    if(phase==2)return run_lp51(s,1);
    if(phase==4)return s->restore_fuses?settings_write(s,0,s->restore_settings):LT48_OK;
    return lt48_owner_load_fpga_internal(s->wire,s->r->idle,s->r->idle_size);
}
static lt48_status mismatch_lp51(lt48_serial_page_result *out,unsigned base,const unsigned char *a,const unsigned char *b,size_t n,int blank) {
    for(size_t i=0;i<n;i++)if(a[i]!=(blank?255:b[i])) {
        if(!out->mismatch_count)out->first_mismatch=base+(unsigned)i;
        out->mismatch_count++;
    }
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
static lt48_status erase_lp51(lp51_session *s,unsigned char saved[16],const unsigned char *analog,lt48_serial_page_result *out) {
    const lt48_at89_lp51_resources *r=s->r;
    unsigned char state[16];lt48_status st=run_lp51(s,2);
    if(st==LT48_OK)st=run_lp51(s,14);
    if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK)st=run_lp51(s,3);
    if(st==LT48_OK)st=settings_read(s,state);
    saved[r->lock]=0;
    if(st==LT48_OK && memcmp(state,saved,r->settings))st=LT48_DEVICE_MISMATCH;
    /* Unlock first if necessary, then explicitly erase the user signature.
     * Chip erase never touches signature page 3; exclude W651's 72h tail.
     * Temporarily allow EEPROM erase when its disable fuse is set. */
    if(st==LT48_OK && (!saved[6] || (r->signature>r->flash && saved[8]))) {
        state[6]=1;state[8]=0;s->restore_fuses=1;memcpy(s->restore_settings,saved,r->settings);st=settings_write(s,0,state);
        if(st==LT48_OK)st=run_lp51(s,2);
        if(st==LT48_OK)st=run_lp51(s,14);
        if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
        if(st==LT48_OK)st=run_lp51(s,3);
        if(st==LT48_OK)st=settings_write(s,0,saved);
        if(st==LT48_OK)s->restore_fuses=0;
    }
    unsigned char *actual=malloc(s->r->capacity);
    if(!actual && st==LT48_OK)st=LT48_NO_MEMORY;
    if(st==LT48_OK)st=read_lp51(s,0,s->r->capacity,actual,s->r->nominal);
    unsigned boundary=s->r->signature+s->r->calibration;
    if(st==LT48_OK)st=mismatch_lp51(out,0,actual,NULL,boundary,1);
    if(st==LT48_OK)st=mismatch_lp51(out,boundary,actual+boundary,analog,128,0);
    free(actual);return st;
}
static lt48_status write_lp51(lp51_session *s,const unsigned char *image) {
    lt48_status st=run_lp51(s,2);
    /* Code/EEPROM/signature select different commands. Full 64-byte chunks;
     * stop before the analog page, which is verified unchanged separately. */
    unsigned end=s->r->signature+s->r->calibration;
    for(unsigned a=0;st==LT48_OK && a<end;) {
        unsigned boundary=a<s->r->flash?s->r->flash:a<s->r->signature?s->r->signature:end;
        size_t n=boundary-a;if(n>4096)n=4096;
        address(s,a);s->buffers[16][0]=64;st=run_lp51(s,8);
        if(st==LT48_OK)st=block_lp51(s,1,(unsigned char *)image+a,n);
        a+=(unsigned)n;
    }
    if(st==LT48_OK)st=stop_lp51(s);
    if(st==LT48_OK)st=run_lp51(s,3);
    return st;
}
static lt48_status setup_lp51(lp51_session *s) {
    lt48_status st=LT48_OK;
    for(unsigned i=0;st==LT48_OK && i<6;i++) {
        const lt48_wave_code *a=&s->r->applet[i];
        st=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
        if(st==LT48_OK)st=lt48_owner_load_applet_internal(s->wire,a->load_address,a->data,a->data_size);
    }
    if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s->wire,s->r->fpga,2778);
    if(st==LT48_OK)st=run_lp51(s,0);
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof busy);
    return st;
}
lt48_status lt48_at89_lp51_execute(const lt48_at89_lp51_resources *r,const lt48_protocol_transport *api,void *ctx,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned type,lt48_m37_id_decision_fn decide,void *decision_ctx,lt48_serial_page_result *out) {
    if(!out || !api || !api->write || !api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_at89_lp51_validate(r,op,start,length,image,size);
    if(st==LT48_OK && (type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH)))st=LT48_INVALID_ARGUMENT;
    if(st==LT48_OK && ((op==LT48_OPERATION_READ && (!read || read_size<length)) || (op==LT48_OPERATION_CONFIGURATION && (!read || read_size<r->settings))))st=LT48_BUFFER_TOO_SMALL;
    if(st!=LT48_OK){out->operation_status=st;return st;}
    unsigned char *actual=malloc(r->capacity),*wanted=malloc(r->capacity),analog[128],saved[16],observed[16];
    if(!actual || !wanted){free(actual);free(wanted);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    lp51_session s={0};s.r=r;s.api=api;s.ctx=ctx;memcpy(s.vars,r->vars,sizeof s.vars);
    lt48_protocol_transport clean=*api;clean.cancelled=NULL;int setup_uncertain=0;
    if(api->cancelled && api->cancelled(ctx))st=LT48_CANCELLED;
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(&clean,ctx,&s.wire);
    if(st==LT48_OK) {
        s.api=&clean;st=setup_lp51(&s);
        if(st==LT48_OK)st=lt48_runtime_transport_session_flush(s.wire);
        setup_uncertain=st!=LT48_OK;
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;s.api=api;
    }
    if(st==LT48_OK)st=lt48_runtime_transport_session_create(api,ctx,&s.wire);
    for(unsigned attempt=0;st==LT48_OK && attempt<64;attempt++) {
        st=run_lp51(&s,4);if(st!=LT48_OK)break;
        if(s.buffers[7][0]==r->man && s.buffers[5][0]==r->dev)break;
        lt48_m37_id_choice choice=decide?decide(decision_ctx,r->man,r->dev,s.buffers[7][0],s.buffers[5][0]):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)break;
        if(choice!=LT48_M37_ID_RETRY || attempt==63)st=LT48_DEVICE_MISMATCH;
    }
    if(st==LT48_OK)st=settings_read(&s,saved);
    unsigned boundary=r->signature+r->calibration;
    if(st==LT48_OK)st=read_lp51(&s,boundary,128,analog,r->nominal);
    int programming=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK && (op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_SECURE)) {
        memcpy(observed,saved,r->settings);
        if(op==LT48_OPERATION_SECURE){unsigned level=image?image[0]:2;if(level>observed[r->lock])observed[r->lock]=(unsigned char)level;}
        else if(image){unsigned mask=image[0]|image[1]*256u;for(unsigned i=0;i<r->settings;i++)if(mask&(1u<<i))observed[i]=image[i+2];}
        int fuses=0;for(unsigned i=0;i<r->settings;i++)if(i!=r->lock && observed[i]!=saved[i])fuses=1;
        if(observed[r->lock]<saved[r->lock] || (fuses && saved[r->lock]))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && fuses){unsigned char step[16];memcpy(step,observed,r->settings);step[r->lock]=saved[r->lock];st=settings_write(&s,0,step);}
        if(st==LT48_OK && observed[r->lock]!=saved[r->lock])st=settings_write(&s,1,observed);
        if(st==LT48_OK)memcpy(saved,observed,r->settings);
    } else if(st==LT48_OK) {
        if(programming) {
            memcpy(wanted,image,r->capacity);
            for(unsigned i=0;i<128;i++) {
                if(wanted[boundary+i]!=255 && wanted[boundary+i]!=analog[i])st=LT48_SAFETY_REJECTED;
                wanted[boundary+i]=analog[i];
            }
        }
        if(st==LT48_OK && (op==LT48_OPERATION_ERASE || op==LT48_OPERATION_PROGRAM))st=erase_lp51(&s,saved,analog,out);
        else if(st==LT48_OK && ((saved[r->lock]>=2 && start<r->signature) || (programming && saved[r->lock])))st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK && programming) {
            if(op==LT48_OPERATION_OVER_PROGRAM) {
                st=read_lp51(&s,0,r->capacity,actual,r->nominal);
                for(unsigned i=0;st==LT48_OK && i<boundary;i++)if((actual[i]&wanted[i])!=wanted[i]) {
                    out->first_mismatch=i;out->mismatch_count=1;st=LT48_DEVICE_MISMATCH;
                }
            }
            unsigned char temporary[16];memcpy(temporary,saved,r->settings);temporary[6]=1;
            if(st==LT48_OK && !saved[6]){s.restore_fuses=1;memcpy(s.restore_settings,saved,r->settings);st=settings_write(&s,0,temporary);}
            if(st==LT48_OK)st=write_lp51(&s,wanted);
            unsigned delta=r->nominal/(type?10:20),volts[2]={r->nominal-delta,r->nominal+delta};
            if(r->nominal==330 && volts[1]>360)volts[1]=360;
            for(unsigned pass=0;st==LT48_OK && pass<2;pass++) {
                st=read_lp51(&s,0,r->capacity,actual,volts[pass]);
                if(st==LT48_OK)st=mismatch_lp51(out,0,actual,wanted,r->capacity,0);
            }
            if(st==LT48_OK && !saved[6])st=settings_write(&s,0,saved);
            if(st==LT48_OK)s.restore_fuses=0;
        } else if(st==LT48_OK && op!=LT48_OPERATION_ERASE) {
            unsigned delta=r->nominal/(type?10:20),high=r->nominal+delta,volts[2]={r->nominal-delta,high};
            if(r->nominal==330 && volts[1]>360)volts[1]=360;
            unsigned passes=op==LT48_OPERATION_VERIFY && margin==LT48_VERIFY_MARGIN_ALL?2:1;
            if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL)volts[0]=r->nominal;
            else if(margin==LT48_VERIFY_MARGIN_HIGH)volts[0]=volts[1];
            for(unsigned pass=0;st==LT48_OK && pass<passes;pass++) {
                st=read_lp51(&s,start,length,actual,volts[pass]);
                size_t compare=length;
                if(op==LT48_OPERATION_BLANK_CHECK)compare=start>=boundary?0:length>boundary-start?boundary-start:length;
                if(st==LT48_OK && op!=LT48_OPERATION_READ)st=mismatch_lp51(out,start,actual,image,compare,op==LT48_OPERATION_BLANK_CHECK);
            }
        }
    }
    if(st==LT48_OK)st=settings_read(&s,observed);
    if(st==LT48_OK && memcmp(saved,observed,r->settings))st=LT48_DEVICE_MISMATCH;
    unsigned char end_analog[128];
    if(st==LT48_OK)st=read_lp51(&s,boundary,128,end_analog,r->nominal);
    if(st==LT48_OK)st=mismatch_lp51(out,boundary,end_analog,analog,128,0);
    out->operation_status=st;s.api=&clean;
    const unsigned phases[]={0,1,4,2,3};
    st=lt48_owner_finish_internal(&clean,ctx,&s.wire,cleanup_lp51,&s,phases,5,st,&out->cleanup_status,&out->cleanup_completed);
    if(setup_uncertain){out->cleanup_status=LT48_CLEANUP_FAILED;out->cleanup_completed=0;}
    if(st==LT48_OK) {
        if(op==LT48_OPERATION_READ)memcpy(read,actual,length);
        if(op==LT48_OPERATION_CONFIGURATION)memcpy(read,observed,r->settings);
        out->completed_bytes=op==LT48_OPERATION_CONFIGURATION?r->settings:length;
    }
    free(actual);free(wanted);return st;
}
