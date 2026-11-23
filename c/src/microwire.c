#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct lt48_microwire_resources {
    unsigned nvram, extended;
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[16];
    lt48_compile_variable vars[8];size_t var_count;
    unsigned buffer_count,mode_buffer,address_buffer,input_buffer,output_buffer;
    unsigned init_op,reset_op,load_op,program_op,width;uint32_t capacity;uint16_t nominal;
};
void lt48_microwire_close(lt48_microwire_resources *r) {
    if(!r)return;
    for(size_t i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    free(r->idle);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r);
}
uint32_t lt48_microwire_capacity(const lt48_microwire_resources *r){return r?r->capacity:0;}
int lt48_microwire_is_nvram(const lt48_microwire_resources *r){return r && r->nvram;}
lt48_status lt48_microwire_open(const lt48_installation *x,size_t device,lt48_microwire_resources **out) {
    static const struct {unsigned d,m,p,size,bits;} approved[]={
        {2,0,4,128,16},
        /* GA source closures: low-voltage and ROHM 2LB variants. */
        {7,3,4,512,16},
        {3,24,4,128,16},
        {49,21,4,256,16},
        {50,21,4,512,16},
        {4,1,4,256,16},
        {23,6,4,128,8},
        {24,7,4,256,8},
        {6,1,4,512,16},
        {25,7,4,512,8},
        {56,0,5,128,16},
        {53,6,5,128,8},
        {5,2,4,256,16},
        {59,25,4,256,16},
        {57,1,5,256,16},
        {54,7,5,256,8},
        {58,1,5,512,16},
        {55,7,5,512,8},
        {0,0,4,32,16},
        {8,8,6,128,8},
        {9,9,6,128,16},
        {11,10,6,256,8},
        {12,11,6,256,16},
        {14,10,6,512,8},
        {15,11,6,512,16},
        {10,0,7,128,16},
        {13,1,7,256,16},
        {16,1,7,512,16},
        {34,16,4,32,16},
        {35,16,4,64,16},
        {36,16,4,128,16},
        {37,17,4,256,16},
        {38,17,4,512,16},
        {47,0,5,128,16},
    };
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,device,&spec);
    if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2000 || strcmp(spec.definition_stem,"GA_____"))return LT48_UNSUPPORTED;
    static const unsigned extended[][6]={
#include "serial_backlog_microwire.inc"





    };
    int extra=0;
    for(size_t i=0;i<sizeof(extended)/sizeof(extended[0]);i++)
        if(spec.definition_resource==extended[i][0] && spec.family_code==extended[i][1] &&
           spec.pinmap==extended[i][2] && spec.capacity_bytes==extended[i][3] && spec.element_bits==extended[i][4])extra=1;
    int admitted=extra;for(size_t i=0;i<sizeof(approved)/sizeof(*approved);i++)
        if(spec.definition_resource==approved[i].d && spec.family_code==approved[i].m && spec.pinmap==approved[i].p && spec.capacity_bytes==approved[i].size && spec.element_bits==approved[i].bits)admitted=1;
    int nvram=((spec.definition_resource==0x60 && spec.family_code==0x2b && spec.pinmap==0x15) ||
        (spec.definition_resource==0x5a && spec.family_code==0x23 && spec.pinmap==0x11)) && spec.capacity_bytes==32 && spec.element_bits==16 && spec.pin_count==8;
    if(!admitted && !nvram)return LT48_UNSUPPORTED;
    lt48_microwire_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    char path[4096],name[32],hex[65];unsigned char digest[32];size_t index,fi;
    int n=snprintf(path,sizeof(path),"%s/GA_____.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof(path)){free(r);return LT48_INVALID_ARGUMENT;}
#define TRY(call) do{status=(call);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(size_t i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,"a286d6799117d4e188067376cab75843fbb9299f4990091d91ad27d8abab99ad")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->nvram=nvram;r->extended=extra;r->capacity=spec.capacity_bytes;r->width=spec.element_bits/8;
    r->buffer_count=70;r->mode_buffer=7;r->address_buffer=1;r->input_buffer=2;r->output_buffer=3;
    r->init_op=2;r->reset_op=6;r->load_op=7;r->program_op=9;
    lt48_pin_code code;snprintf(name,sizeof(name),"P%x",spec.pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&code));TRY(lt48_pin_map_parse(&code,&r->pins));
    lt48_family_info info;snprintf(name,sizeof(name),"M%x",spec.family_code);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&info));
    if(info.variable_count!=(nvram?2u:4u) || info.operation_count!=(nvram?6u:7u)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=info.variable_count;
    for(size_t i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};if(v.values[1]==0)r->nominal=v.values[3];}
    for(size_t i=0;i<info.operation_count;i++){
        lt48_family_operation op;lt48_wave_code wave;lt48_compact_wave *compact=NULL;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));if(op.operation_id>=16){status=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&wave));
        if(wave.wave_type!=0){status=LT48_FORMAT_ERROR;goto fail;}
        TRY(lt48_compact_decode(wave.data,wave.data_size,&compact));
        status=lt48_compact_compile(compact,r->pins,r->vars,r->var_count,1u,spec.pinmap==6,&r->wave[op.operation_id]);lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
        size_t defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
        if(defaults!=(spec.pinmap==6 && op.operation_id==2?(extra?4u:2u):0u)){status=LT48_SAFETY_REJECTED;goto fail;}
        for(size_t j=0;j<defaults;j++){lt48_defaulted_logical_pin p;TRY(lt48_compiled_wave_get_defaulted_pin(r->wave[op.operation_id],j,&p));if(p.logical_pin!=38 && !(extra && p.logical_pin==47)){status=LT48_SAFETY_REJECTED;goto fail;}}
    }
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_microwire_close(r);return status;
#undef TRY
}
lt48_status lt48_microwire_validate(const lt48_microwire_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=r->capacity || length>r->capacity-start || start%r->width || length%r->width)return LT48_SAFETY_REJECTED;
    if(op<LT48_OPERATION_READ || op>(r->nvram?LT48_OPERATION_COMPARE:LT48_OPERATION_ERASE))return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_ERASE && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_ERASE && (!image || size!=length))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_microwire_validate_verify_options(const lt48_microwire_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type) {
    if(!r || type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL &&
       margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
    /* The X2444 commercial compatibility envelope is 5 V +/-5%. */
    if(r->nvram && type && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM ||
       (op==LT48_OPERATION_VERIFY && margin!=LT48_VERIFY_MARGIN_NOMINAL)))return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
typedef struct microwire_session {
    const lt48_microwire_resources *r;
    lt48_runtime_transport_session *wire;
    const lt48_protocol_transport *api;
    void *context;
    unsigned char buffers[70][32];
    uint32_t volts[4];
    lt48_compile_variable vars[8];
    uint32_t mode;
    unsigned fault;
} microwire_session;
static lt48_status run(microwire_session *s,unsigned op) {
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[70];size_t count=0;
    lt48_runtime_nested_wave nested[16];size_t nested_count=0;
    for (uint16_t i=0;i<16;i++)if(s->r->wave[i]) nested[nested_count++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};
    options.nested_waves=nested;options.nested_wave_count=nested_count;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    s->fault=0;
    for(uint8_t i=0;i<s->r->buffer_count;i++) if(i!=s->r->mode_buffer) seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    options.mode=s->mode;options.use_explicit_buffer_ids=1;
    options.mode_buffer=s->r->mode_buffer;options.address_buffer=s->r->address_buffer;options.parameter_buffer_id=s->r->input_buffer;
    options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));
    options.root_timeout_seconds=10;
    lt48_status status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,
        s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run(s->wire,machine,&result);
    if(status==LT48_OK) {
        for(uint8_t i=0;i<s->r->buffer_count;i++) {
            const unsigned char *data;size_t size;
            status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK || size!=32) {status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        for(uint8_t i=0;status==LT48_OK && i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(event.kind==LT48_RUNTIME_RESULT_LATCH) {s->fault=(unsigned)event.arguments[0];status=LT48_RUNTIME_ERROR;}
            if(event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0)status=LT48_RUNTIME_ERROR;
        }
    }
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_compile_variable *var(microwire_session *s,unsigned id) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==id)return &s->vars[i];
    return NULL;
}
static lt48_status cycle(microwire_session *s,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_microwire_result *out) {
    memset(s->buffers,0,sizeof(s->buffers));
    /* Microwire DEVINIT enables writing only in original mode 2. EEPROM
     * Program and Over-program both use that reversible per-word write path. */
    s->mode=(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)?2:
        op==LT48_OPERATION_VERIFY?4:op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_ERASE?(s->r->extended?7:5):1;
    lt48_compile_variable *v=var(s,0);if(v)v->value=vcc;
    lt48_status status=run(s,s->r->init_op);
    if(status==LT48_OK && op==LT48_OPERATION_ERASE)status=run(s,14);
    for(size_t i=0;status==LT48_OK && op!=LT48_OPERATION_ERASE && i<length;i+=s->r->width){
        uint32_t address=(start+(uint32_t)i)/s->r->width;
        for(unsigned j=0;j<4;j++)s->buffers[1][j]=(unsigned char)(address>>(8*j));
        memset(s->buffers[2],0,32);memset(s->buffers[3],0,32);
        for(unsigned j=0;j<s->r->width;j++)s->buffers[3][j]=op==LT48_OPERATION_BLANK_CHECK?255:image?image[i+j]:0;
        status=run(s,(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)?9:7);
        /* X24C44 PROGRAM writes SRAM only; unlike 93C its wave does not
         * populate DataIn. Read independently before accepting the word. */
        if(status==LT48_OK && s->r->nvram && s->mode==2)status=run(s,s->r->load_op);
        if(status!=LT48_OK)break;
        for(unsigned j=0;j<s->r->width;j++){
            if(read)read[i+j]=s->buffers[2][j];
            if(op!=LT48_OPERATION_READ && s->buffers[2][j]!=s->buffers[3][j]){if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i+j;out->mismatch_count++;}
        }
        out->completed_bytes=i+s->r->width;
        if(s->api->progress)s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,out->completed_bytes,length,0,0,0);
        if(out->mismatch_count && op!=LT48_OPERATION_COMPARE){status=LT48_DEVICE_MISMATCH;break;}
    }
    /* A failed SRAM update must not intentionally STORE a partial image. */
    lt48_status reset=(s->r->nvram && status!=LT48_OK)?LT48_OK:run(s,s->r->reset_op);
    if(status==LT48_OK)status=reset;
    if(status==LT48_OK && s->r->nvram && s->mode==2) {
        status=lt48_runtime_transport_session_status(s->wire,1);
        if(status==LT48_OK)status=lt48_nvram_store_hold_internal();
    }
    return status==LT48_OK && out->mismatch_count?LT48_DEVICE_MISMATCH:status;
}
lt48_status lt48_microwire_execute(const lt48_microwire_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned margin_type,lt48_microwire_result *out) {
    if(!out || !api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_microwire_validate(r,op,start,length,image,size);if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ && (!read || read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_microwire_validate_verify_options(r,op,margin,margin_type);if(status!=LT48_OK)return status;
    unsigned char *pending=malloc(r->nvram?r->capacity:length);if(!pending)return LT48_NO_MEMORY;
    unsigned char expected_nvram[32];
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    microwire_session s={0};s.r=r;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof(s.vars));s.mode=1;
    status=lt48_runtime_transport_session_create(api,context,&s.wire);if(status!=LT48_OK){free(pending);return status;}
    status=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    /* Class-2000 waves require the Windows idle FPGA. Explicitly establish it
     * so a preceding MCU/EPROM device-specific image cannot leak into this job. */
    const unsigned char header[]={0x38,0xaa,0x55};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof(header));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
    if(status==LT48_OK)status=run(&s,0);
    for(unsigned i=0;status==LT48_OK && i<3;i++)status=lt48_runtime_transport_session_status(s.wire,1);
    static const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
    uint16_t nominal=r->nominal,lo=(uint16_t)(nominal*(margin_type?90u:95u)/100u),hi=(uint16_t)(nominal*(margin_type?110u:105u)/100u);
    if(status==LT48_OK && r->nvram && writing) {
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,out);
        if(status==LT48_OK){memcpy(expected_nvram,pending,r->capacity);memcpy(expected_nvram+start,image,length);}
    }
    if(status==LT48_OK && op==LT48_OPERATION_VERIFY){
        if(margin==LT48_VERIFY_MARGIN_NOMINAL)status=cycle(&s,op,start,length,image,NULL,nominal,out);
        else{if(margin!=LT48_VERIFY_MARGIN_HIGH)status=cycle(&s,op,start,length,image,NULL,lo,out);if(status==LT48_OK && margin!=LT48_VERIFY_MARGIN_LOW)status=cycle(&s,op,start,length,image,NULL,hi,out);}
    }else if(status==LT48_OK)status=cycle(&s,op,start,length,image,op==LT48_OPERATION_READ?pending:NULL,nominal,out);
    if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)){
        status=cycle(&s,LT48_OPERATION_VERIFY,start,length,image,NULL,lo,out);
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,start,length,image,NULL,hi,out);
    }
    if(status==LT48_OK && op==LT48_OPERATION_ERASE)status=cycle(&s,LT48_OPERATION_BLANK_CHECK,0,r->capacity,NULL,NULL,nominal,out);
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
    lt48_runtime_transport_session_close(s.wire);
    if(status==LT48_OK && r->nvram && writing) {
        /* New power/setup plus source RECALL verifies EEPROM persistence and
         * the untouched portion of a partial-range STORE. */
        lt48_microwire_result fresh={0};
        status=lt48_microwire_execute(r,api,context,LT48_OPERATION_READ,0,r->capacity,NULL,0,
            pending,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,0,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(uint32_t i=0;i<r->capacity;i++)if(pending[i]!=expected_nvram[i]) {
            if(!out->mismatch_count)out->first_mismatch=i;
            out->mismatch_count++;status=LT48_DEVICE_MISMATCH;
        }
        out->operation_status=status;
        if(status!=LT48_OK) {
            lt48_runtime_transport_session *led=NULL;
            lt48_status step=lt48_runtime_transport_session_create(&cleanup,context,&led);
            static const unsigned char error[]={0x40,2};
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(led,error,sizeof(error));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(led);
            lt48_runtime_transport_session_close(led);
            if(out->cleanup_status==LT48_OK)out->cleanup_status=step;
        }
    }
    free(pending);return status;
}
