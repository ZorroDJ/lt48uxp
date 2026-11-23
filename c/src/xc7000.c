#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Dataman 13.10 TXilinxMemGroup (456b90/456e00/457520).
 * Library data stay in the user's selected, digest-checked REZ. */
struct lt48_xc7000_resources {
    lt48_rez *rez; lt48_pin_map *pins; lt48_compiled_wave *wave[8];
    lt48_compile_variable vars[26]; size_t var_count;
    lt48_runtime_frame_profile profile;
    lt48_xc7000_row *rows;size_t row_count,capacity;
    unsigned width,retries;unsigned char *idle;size_t idle_size;
};
static unsigned u16(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t u32(const unsigned char *p){return u16(p)|(uint32_t)u16(p+2)<<16;}
static void put(unsigned char *p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(n>>(8*i));}
void lt48_xc7000_close(lt48_xc7000_resources *r){if(!r)return;for(unsigned i=0;i<8;i++)lt48_compiled_wave_close(r->wave[i]);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->rows);free(r->idle);free(r);}
size_t lt48_xc7000_capacity(const lt48_xc7000_resources *r){return r?r->capacity:0;}
size_t lt48_xc7000_row_count(const lt48_xc7000_resources *r){return r?r->row_count:0;}
lt48_status lt48_xc7000_get_row(const lt48_xc7000_resources *r,size_t i,lt48_xc7000_row *out){if(!r||!out)return LT48_INVALID_ARGUMENT;if(i>=r->row_count)return LT48_NOT_FOUND;*out=r->rows[i];return LT48_OK;}
static lt48_status table(lt48_xc7000_resources *r,const unsigned char *p,size_t n) {
    static const unsigned char head[]={2,'[',16,'T','X','A','d','d','r','C','o','l','l','e','c','t','i','o','n'};
    static const unsigned char unit[]={2,'[',10,'T','X','A','d','d','r','U','n','i','t'};
    if(n<sizeof head+12||memcmp(p,head,sizeof head))return LT48_FORMAT_ERROR;
    unsigned groups=u32(p+sizeof head);size_t at=sizeof head+12;
    if(!groups||groups>64)return LT48_FORMAT_ERROR;
    r->rows=calloc(65536,sizeof(*r->rows));if(!r->rows)return LT48_NO_MEMORY;
    for(unsigned g=0;g<groups;g++) {
        if(at>n||sizeof unit+8>n-at||memcmp(p+at,unit,sizeof unit))return LT48_FORMAT_ERROR;
        at+=sizeof unit;unsigned bits=u16(p+at),mask=u32(p+at+2),count=u16(p+at+6);at+=8;
        if(!bits||bits>r->width*8||mask>=(1u<<bits)||count>65536-r->row_count||at>n||2u*count+1>n-at)return LT48_FORMAT_ERROR;
        for(unsigned i=0;i<count;i++) {
            unsigned address=u16(p+at);at+=2;
            if(address+(r->width-1)*65536u>=r->capacity)return LT48_FORMAT_ERROR;
            r->rows[r->row_count++]=(lt48_xc7000_row){(uint16_t)address,(uint16_t)mask};
        }
        if(p[at++]!=']')return LT48_FORMAT_ERROR;
    }
    /* End of collection, followed by serialized address count. */
    if(at>=n||p[at++]!=']')return LT48_FORMAT_ERROR;
    /* Separate protection unit is outside the normal range collection. */
    if(at>n||sizeof unit+8>n-at||memcmp(p+at,unit,sizeof unit))return LT48_FORMAT_ERROR;
    at+=sizeof unit;unsigned bits=u16(p+at),mask=u32(p+at+2),count=u16(p+at+6);at+=8;
    if(!bits||bits>r->width*8||mask>=(1u<<bits)||!count||at>n||2u*count+3!=n-at)return LT48_FORMAT_ERROR;
    for(unsigned i=0;i<count;i++)if(u16(p+at+2*i)+(r->width-1)*65536u>=r->capacity)return LT48_FORMAT_ERROR;
    at+=2u*count;
    if(p[at++]!=']'||u16(p+at)!=r->row_count)return LT48_FORMAT_ERROR;
    return r->row_count?LT48_OK:LT48_FORMAT_ERROR;
}
lt48_status lt48_xc7000_open(const lt48_installation *x,size_t device,lt48_xc7000_resources **out) {
    static const unsigned tuples[][3]={{0,0,0},{1,1,0},{2,4,1},{4,5,1},{5,5,2},{6,6,3},{7,6,4},{8,7,4},{9,2,5},{10,3,5},{11,2,6},{12,3,6}};
    if(!x||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,device,&spec);if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2017)return LT48_UNSUPPORTED;
    int admitted=0;for(size_t i=0;i<sizeof tuples/sizeof *tuples;i++)if(spec.definition_resource==tuples[i][0]&&spec.family_code==tuples[i][1]&&spec.pinmap==tuples[i][2])admitted=1;
    if(!admitted||!spec.definition_stem||strcmp(spec.definition_stem,"GAE____"))return LT48_UNSUPPORTED;
    lt48_xc7000_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    char path[4096],name[24],hex[65];unsigned char digest[32];size_t i,fi;lt48_device_definition d;lt48_pin_code pins;lt48_family_info family;
#define TRY(x) do{st=(x);if(st!=LT48_OK)goto fail;}while(0)
    int n=snprintf(path,sizeof path,"%s/GAE____.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof path){st=LT48_INVALID_ARGUMENT;goto fail;}
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));for(i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,"7c4cdca4c1dd5bfd3c405001054e555d12e1925815f52ae8fb8db1cd034d9c90")){st=LT48_SAFETY_REJECTED;goto fail;}
    snprintf(name,sizeof name,"D%x",spec.definition_resource);TRY(lt48_rez_find_resource(r->rez,name,&i));TRY(lt48_rez_get_device_definition(r->rez,i,&d));
    if(d.kind!=LT48_DEVICE_DEFINITION_XILINX_MEMORY||d.trailing_data_size!=12||(d.element_bits!=8&&d.element_bits!=16)){st=LT48_FORMAT_ERROR;goto fail;}
    r->width=d.element_bits/8;r->capacity=d.element_count+(r->width-1)*65536u;
    snprintf(name,sizeof name,"X%x",u16(d.trailing_data+10));TRY(lt48_rez_find_resource(r->rez,name,&i));
    const unsigned char *p;size_t bytes;TRY(lt48_rez_xilinx_table_internal(r->rez,i,&p,&bytes));TRY(table(r,p,bytes));
    snprintf(name,sizeof name,"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&i));TRY(lt48_rez_get_pin_code(r->rez,i,&pins));TRY(lt48_pin_map_parse(&pins,&r->pins));
    snprintf(name,sizeof name,"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&family));TRY(lt48_rez_get_family_frame_profile(r->rez,fi,&r->profile));
    if(family.operation_count!=8||family.variable_count>26||r->profile.buffer_count!=11||r->profile.mode_buffer!=3){st=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;for(i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};if(v.values[1]==25)r->retries=v.values[3];}
    if(r->retries!=10&&r->retries!=20){st=LT48_FORMAT_ERROR;goto fail;}
    for(i=0;i<8;i++) {
        lt48_family_operation op;lt48_wave_code code;lt48_compact_wave *wave=NULL;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(op.operation_id>=8||r->wave[op.operation_id]){st=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof name,"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&bytes));TRY(lt48_rez_get_wave_code(r->rez,bytes,&code));
        if(code.wave_type){st=LT48_UNSUPPORTED;goto fail;}TRY(lt48_compact_decode(code.data,code.data_size,&wave));st=lt48_compact_compile(wave,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(wave);if(st!=LT48_OK)goto fail;
        if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){st=LT48_SAFETY_REJECTED;goto fail;}
    }
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_xc7000_close(r);return st;
#undef TRY
}
lt48_status lt48_xc7000_validate(const lt48_xc7000_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r)return LT48_INVALID_ARGUMENT;
    if(start||length!=r->capacity)return LT48_SAFETY_REJECTED;
    switch(op){
    case LT48_OPERATION_READ:case LT48_OPERATION_BLANK_CHECK:case LT48_OPERATION_SECURE:return LT48_OK;
    case LT48_OPERATION_PROGRAM:case LT48_OPERATION_OVER_PROGRAM:case LT48_OPERATION_VERIFY:case LT48_OPERATION_COMPARE:
        return image&&size==r->capacity?LT48_OK:LT48_INVALID_ARGUMENT;
    default:return LT48_UNSUPPORTED;
    }
}
typedef struct session {
    const lt48_xc7000_resources *r;lt48_runtime_transport_session *wire;
    unsigned char buffers[11][32];uint32_t volts[4];lt48_compile_variable vars[26];
    unsigned mode;const unsigned char *image;
    lt48_runtime_configuration_value config[16];size_t config_count;
} session;
static lt48_status run(session *s,unsigned op) {
    lt48_runtime_options o={0};lt48_runtime_buffer_seed seeds[10];size_t n=0;
    lt48_runtime_nested_wave nested[8];for(unsigned i=0;i<8;i++)nested[i]=(lt48_runtime_nested_wave){i,s->r->wave[i]};
    for(unsigned i=0;i<11;i++)if(i!=3)seeds[n++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    o.host_xc7000_ramp=1;o.mode=s->mode;o.use_explicit_buffer_ids=1;o.mode_buffer=3;o.address_buffer=0;o.parameter_buffer_id=1;
    o.initial_buffers=seeds;o.initial_buffer_count=n;o.nested_waves=nested;o.nested_wave_count=8;
    o.device_image=s->image;o.device_image_size=s->image?s->r->capacity:0;o.device_element_bytes=1;
    o.configuration=s->config;o.configuration_count=s->config_count;
    memcpy(o.initial_voltage_centivolts,s->volts,sizeof s->volts);o.root_timeout_seconds=10;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    lt48_status st=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&o,&machine);
    if(st==LT48_OK)st=lt48_runtime_transport_session_run(s->wire,machine,&result);
    if(st==LT48_OK)for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
        lt48_runtime_event e;st=lt48_runtime_get_event(result,i,&e);if(st!=LT48_OK)break;
        if(e.kind==LT48_RUNTIME_RESULT_LATCH||(e.kind==LT48_RUNTIME_RETURN_STATUS&&e.argument_count==1&&!e.arguments[0])){st=LT48_RUNTIME_ERROR;break;}
    }
    for(unsigned i=0;st==LT48_OK&&i<11;i++) {
        const unsigned char *data;size_t size;st=lt48_runtime_get_buffer(result,i,&data,&size);
        if(st==LT48_OK&&size!=32)st=LT48_RUNTIME_ERROR;
        if(st==LT48_OK)memcpy(s->buffers[i],data,32);
    }
    for(unsigned i=0;st==LT48_OK&&i<4;i++)st=lt48_runtime_get_voltage(result,i,&s->volts[i]);
    for(size_t i=0;st==LT48_OK&&i<s->r->var_count;i++)st=lt48_runtime_get_variable(result,s->vars[i].variable_id,&s->vars[i]);
    if(st==LT48_OK){const lt48_runtime_configuration_value *cfg;size_t size;st=lt48_runtime_get_configuration(result,&cfg,&size);if(st==LT48_OK&&size>16)st=LT48_RUNTIME_ERROR;if(st==LT48_OK){memcpy(s->config,cfg,size*sizeof(*cfg));s->config_count=size;}}
    if(st!=LT48_OK&&getenv("LT48_SERIAL_TRACE"))fprintf(stderr,"XC7000 wave %u mode %u: %s\n",op,s->mode,lt48_status_string(st));
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return st;
}
static lt48_status enter(session *s,unsigned mode) {
    s->mode=mode==5?1:mode;
    return lt48_runtime_frame_set_mode(s->buffers,11,s->r->profile.reset_mask,3,(uint16_t)s->mode,0,0);
}
static lt48_status range(session *s,unsigned mode,unsigned char *read,lt48_xc7000_result *out,const lt48_protocol_transport *api,void *ctx) {
    lt48_status st=enter(s,mode);if(st==LT48_OK)st=run(s,3);
    out->mismatch_count=0;out->first_mismatch=SIZE_MAX;out->completed_rows=0;
    for(size_t i=0;st==LT48_OK&&i<s->r->row_count;i++) {
        if(api->cancelled&&api->cancelled(ctx)){st=LT48_CANCELLED;break;}
        lt48_xc7000_row row=s->r->rows[i];unsigned expected=0;
        for(unsigned j=0;j<s->r->width;j++)expected|=(unsigned)(mode==11?255:s->image?s->image[row.address+j*65536u]:0)<<(8*j);
        expected&=row.mask;put(s->buffers[0],row.address);put(s->buffers[10],row.mask);put(s->buffers[2],mode==11?(s->r->width==2?65535u:255u):expected);
        if(mode==2||mode==3||mode==4)put(s->buffers[1],expected);
        st=run(s,mode==2?6:5);if(st!=LT48_OK)break;
        unsigned actual=u32(s->buffers[1])&row.mask;out->completed_rows=i+1;
        if(mode==1)for(unsigned j=0;j<s->r->width;j++)read[row.address+j*65536u]=(unsigned char)(actual>>(8*j));
        else if(actual!=expected){out->mismatch_count++;out->first_mismatch=row.address;st=LT48_DEVICE_MISMATCH;}
        if(api->progress&&((i&63)==63||i+1==s->r->row_count))api->progress(ctx,LT48_EXECUTION_READ_OPERATION,i+1,s->r->row_count,0,0,0);
    }
    lt48_status reset=run(s,4);return st==LT48_OK?reset:st==LT48_DEVICE_MISMATCH&&reset!=LT48_OK?LT48_CLEANUP_FAILED:st;
}
lt48_status lt48_xc7000_execute(const lt48_xc7000_resources *r,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,
    unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned margin_type,lt48_xc7000_result *out) {
    if(!api||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=SIZE_MAX;
    lt48_status status=lt48_xc7000_validate(r,op,start,length,image,size);
    if(status==LT48_OK&&(margin<LT48_VERIFY_MARGIN_NOMINAL||margin>LT48_VERIFY_MARGIN_ALL||margin_type>1))status=LT48_INVALID_ARGUMENT;
    if(status==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))status=LT48_INVALID_ARGUMENT;
    if(status!=LT48_OK){out->operation_status=status;return status;}
    unsigned char *pending=malloc(length);if(!pending)return LT48_NO_MEMORY;memset(pending,255,length);
    session s={0};s.r=r;s.mode=1;s.image=image;memcpy(s.vars,r->vars,sizeof s.vars);
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(pending);return status;}
    /* Source FileToCfg runs on the loaded byte image, before powering pins.
     * Protection remains a separate explicit operator action. */
    if(image)status=run(&s,2);
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    static const unsigned char header[]={0x38,0xaa,0x55};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof header);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
    static const unsigned char selector[]={0x0f,15};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,selector,sizeof selector);
    if(status==LT48_OK)status=enter(&s,op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_SECURE?8:1);
    if(status==LT48_OK)status=run(&s,0);
    for(unsigned i=0;status==LT48_OK&&i<3;i++)status=lt48_runtime_transport_session_status(s.wire,1);
    static const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof busy);
    if(status==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) {
        if(op==LT48_OPERATION_PROGRAM)status=range(&s,11,NULL,out,api,context);
        for(unsigned attempt=0;status==LT48_OK&&attempt<r->retries;attempt++) {
            out->attempts=attempt+1;
            status=range(&s,2,NULL,out,api,context);
            if(status!=LT48_OK)break;
            status=range(&s,3,NULL,out,api,context);
            if(status!=LT48_DEVICE_MISMATCH||attempt+1==r->retries)break;
            status=LT48_OK;
        }
    } else if(status==LT48_OK&&op==LT48_OPERATION_VERIFY) {
        uint16_t volts[2]={margin_type?450:475,margin_type?550:525};unsigned count=2;
        if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=500;count=1;}
        if(margin==LT48_VERIFY_MARGIN_LOW)count=1;
        if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];count=1;}
        for(unsigned i=0;status==LT48_OK&&i<count;i++) {
            for(size_t v=0;v<r->var_count;v++)if(s.vars[v].variable_id==0)s.vars[v].value=volts[i];
            status=range(&s,4,NULL,out,api,context);
        }
    } else if(status==LT48_OK&&op==LT48_OPERATION_SECURE) {
        status=enter(&s,8);if(status==LT48_OK)status=run(&s,3);
        if(status==LT48_OK)status=run(&s,7);
        lt48_status reset=run(&s,4);if(status==LT48_OK)status=reset;
    } else if(status==LT48_OK)status=range(&s,op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_COMPARE?5:4,pending,out,api,context);
    out->operation_status=status;
    /* A fresh uncancellable transport owns cleanup even after a failed queue.
     * Continue independent rail-off attempts and preserve the first error. */
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        lt48_status step=run(&s,1);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(step!=LT48_OK)out->cleanup_status=step;
        for(unsigned channel=0;channel<4;channel++) {
            unsigned char off[]={0x10,(unsigned char)(3-channel),0};
            /* Restart each phase after transport failure. */
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,off,sizeof(off));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK && step!=LT48_OK)out->cleanup_status=step;
        }
        /* Original generic tail restores the Windows idle FPGA after power-off. */
        static const unsigned char idle_header[]={0x38,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s.wire,idle_header,sizeof(idle_header));
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK && step!=LT48_OK)out->cleanup_status=step;
        static const unsigned char finish[]={0x49,0xaa,0x55};
        step=lt48_runtime_transport_session_controller_write(s.wire,finish,sizeof(finish));
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK)out->cleanup_status=step;
        out->cleanup_completed=out->cleanup_status==LT48_OK;
        if(out->cleanup_completed) {
            unsigned char led[]={0x40,status==LT48_OK?4:2};
            step=lt48_runtime_transport_session_controller_write(s.wire,led,sizeof(led));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(step!=LT48_OK)out->cleanup_status=step;
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);free(pending);return status;
}
