#include "legacy_dataflash.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct profile { unsigned definition,family,pins,capacity,page,ops[8]; };
static const struct profile profiles[]={
#include "legacy_dataflash_profiles.inc"
};
struct lt48_legacy_dataflash_resources {
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[5];
    lt48_wave_code applet[3];
    const unsigned char *fpga;
    lt48_compile_variable voltage;
    const struct profile *profile;
    unsigned char *idle;size_t idle_size;
    lt48_applet_address addresses[3];
};
void lt48_legacy_dataflash_close(lt48_legacy_dataflash_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<5;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
uint32_t lt48_legacy_dataflash_capacity(const lt48_legacy_dataflash_resources *r) {return r?r->profile->capacity:0;}
unsigned lt48_legacy_dataflash_page_size(const lt48_legacy_dataflash_resources *r) {return r?r->profile->page:0;}
lt48_status lt48_legacy_dataflash_address(const lt48_legacy_dataflash_resources *r,uint32_t offset,uint32_t *out) {
    if(!r||!out||offset>=r->profile->capacity)return LT48_INVALID_ARGUMENT;
    unsigned page=r->profile->page;
    *out=(offset/page)*(page==264?512u:1024u)+offset%page;
    return *out<=0xffffffu?LT48_OK:LT48_INVALID_ARGUMENT;
}
lt48_status lt48_legacy_dataflash_open(const lt48_installation *x,size_t device,lt48_legacy_dataflash_resources **out) {
    lt48_device_spec spec;lt48_status status;const struct profile *p=NULL;
    if(!x||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;status=lt48_installation_resolve_device(x,device,&spec);if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2009||spec.element_bits!=8)return LT48_UNSUPPORTED;
    for(size_t i=0;i<sizeof(profiles)/sizeof(profiles[0]);i++)
        if(spec.definition_resource==profiles[i].definition && spec.family_code==profiles[i].family &&
           spec.pinmap==profiles[i].pins && spec.capacity_bytes==profiles[i].capacity)p=&profiles[i];
    if(!p)return LT48_UNSUPPORTED;
    lt48_legacy_dataflash_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    r->profile=p;char path[4096],name[24],hex[65];unsigned char digest[32];size_t index,fi;
    int n=snprintf(path,sizeof(path),"%s/GAAF___.rez",lt48_installation_path(x));
    if(n<0||(size_t)n>=sizeof(path)){status=LT48_INVALID_ARGUMENT;goto fail;}
#define TRY(x) do{status=(x);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,"9fdc0c7a88bb96aaead0314fb4f85c0800ab987abf370908000493cc272d13f6")){status=LT48_SAFETY_REJECTED;goto fail;}
    lt48_pin_code pc;snprintf(name,sizeof(name),"P%x",p->pins);
    TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    lt48_family_info info;snprintf(name,sizeof(name),"M%x",p->family);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&info));
    if(info.variable_count!=1||info.operation_count!=8){status=LT48_FORMAT_ERROR;goto fail;}
    lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,0,&v));
    r->voltage=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    static const unsigned ids[]={0,1,3,4,10,12,13,15};unsigned seen=0;
    for(unsigned i=0;i<8;i++) {
        lt48_family_operation op;lt48_wave_code code;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        unsigned slot=0;while(slot<8&&ids[slot]!=op.operation_id)slot++;
        if(slot==8||(seen&(1u<<slot))||op.wave!=p->ops[slot]){status=LT48_FORMAT_ERROR;goto fail;}seen|=1u<<slot;
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(slot<4 && code.wave_type==0){
            lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(code.data,code.data_size,&w));
            status=lt48_compact_compile(w,r->pins,&r->voltage,1,1,0,&r->wave[op.operation_id]);
            lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){status=LT48_SAFETY_REJECTED;goto fail;}
        }else if(slot==4 && code.wave_type==4 && code.data_size==2780 && code.data[0]==0xda && code.data[1]==0x0a)r->fpga=code.data+2;
        else if(slot>=5 && code.wave_type==6 && code.data_size && code.length1==code.data_size+4 && code.length2==code.data_size && code.load_address>=0xa000 && (uint32_t)code.load_address+code.data_size<=0xb000)r->applet[slot-5]=code;
        else {status=LT48_FORMAT_ERROR;goto fail;}
    }
    if(seen!=255||!r->fpga||p->capacity%p->page){status=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<3;i++)r->addresses[i]=(lt48_applet_address){ids[i+5],r->applet[i].load_address};
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_legacy_dataflash_close(r);return status;
#undef TRY
}

typedef struct {
    const lt48_legacy_dataflash_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[8][32];uint32_t volts[4];unsigned mode;
    lt48_compile_variable voltage;
} dataflash_session;
static lt48_status wave(dataflash_session *s,unsigned op) {
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[7];size_t count=0;
    for(unsigned i=0;i<8;i++)if(i!=2)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=2;
    options.address_buffer=0;options.parameter_buffer_id=7;
    options.initial_buffers=seeds;options.initial_buffer_count=count;options.root_timeout_seconds=30;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,30000,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,&s->voltage,1,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,3,&result);
    if(status==LT48_OK) {
        for(unsigned i=0;i<8;i++) {
            const unsigned char *b;size_t n;status=lt48_runtime_get_buffer(result,i,&b,&n);
            if(status!=LT48_OK||n!=32){status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],b,32);
        }
        for(unsigned i=0;status==LT48_OK&&i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event e;lt48_runtime_get_event(result,i,&e);
            if(e.kind==LT48_RUNTIME_RESULT_LATCH || (e.kind==LT48_RUNTIME_RETURN_STATUS&&e.argument_count==1&&!e.arguments[0]))status=LT48_RUNTIME_ERROR;
        }
    }
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_status io(dataflash_session *s,int input,unsigned char *b,size_t n) {
    size_t done=0;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    lt48_status status=input?s->api->read(s->context,0x88,b,n,&done):s->api->write(s->context,4,b,n,&done);
    return status==LT48_OK&&done!=n?LT48_PROTOCOL_ERROR:status;
}
static lt48_status command(dataflash_session *s,const unsigned char *b,size_t n,unsigned char *reply) {
    unsigned char packet[512]={0};size_t done=0;
    if(n>sizeof(packet))return LT48_INVALID_ARGUMENT;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    memcpy(packet,b,n);lt48_status status=lt48_runtime_transport_session_flush(s->wire);
    if(status==LT48_OK)status=s->api->write(s->context,2,packet,sizeof(packet),&done);
    if(status==LT48_OK&&done!=sizeof(packet))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK&&reply)status=io(s,1,reply,512);
    return status;
}
static lt48_status setup(dataflash_session *s) {
    lt48_status status=LT48_OK;
    for(unsigned i=0;i<3&&status==LT48_OK;i++) {
        const lt48_wave_code *a=&s->r->applet[i];size_t last=a->data_size-1;
        unsigned char h[]={0x39,0xaa,0x55,a->load_address&255,a->load_address>>8,last&255,last>>8};
        status=lt48_runtime_transport_session_controller_write(s->wire,h,sizeof(h));
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,a->data,a->data_size);
    }
    const unsigned char fpga[]={0x38,0xaa,0x55};
    /* Original selector uses 7 for <=4V and 15 above 4V. */
    const unsigned char selector[]={0x0f,s->r->voltage.value>400?15:7};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,fpga,sizeof(fpga));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,s->r->fpga,2778);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,selector,sizeof(selector));
    if(status==LT48_OK)status=wave(s,0);
    return status;
}
/* Original GAAF worker: first write byte via04, remaining bytes via47,
 * termination via40. Read callback4543B0 uses transport+24 (43), not46. */
static lt48_status cycle(dataflash_session *s,unsigned mode,unsigned char *bytes,size_t n,unsigned vcc) {
    /* Source INIT: 1 reads, 2 programs, 4 selects the verification applet.
     * Program and Verify both stream the expected bytes toward the controller. */
    int transmit=mode!=1;
    s->mode=mode;s->voltage.value=vcc;memset(s->buffers[0],0,32);
    lt48_status status=wave(s,3);unsigned char reply[512]={0};size_t pos=0;
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s->wire,30000,0);
    if(transmit&&status==LT48_OK) {
        unsigned char first[]={4,bytes[0]};status=command(s,first,2,reply);
        if(status==LT48_OK&&!reply[0])status=LT48_PROTOCOL_ERROR;
        pos=1;
    }
    while(status==LT48_OK&&pos<n) {
        size_t len=n-pos;if(len>8192)len=8192;
        unsigned char h[]={transmit?0x47:0x43,len/512&255,len/512>>8,len%512&255,len%512>>8,transmit?1:0};
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,30000,0);
        if(status==LT48_OK)status=command(s,h,sizeof(h),NULL);
        if(status==LT48_OK)status=io(s,!transmit,bytes+pos,len);
        if(status==LT48_OK&&transmit) {
            status=io(s,1,reply,512);
            if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
        }
        if(status==LT48_OK)pos+=len;
    }
    if(status==LT48_OK) {
        const unsigned char terminal[]={0x40,0};status=command(s,terminal,2,reply);
        if(status==LT48_OK&&!reply[0])status=LT48_PROTOCOL_ERROR;
        if(status==LT48_OK&&transmit&&!reply[2])status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)status=wave(s,4);
    return status;
}
static lt48_status cleanup(dataflash_session *s,lt48_status operation) {
    lt48_status status=LT48_OK,step;
    const unsigned char stop[]={0,0};unsigned char reply[512]={0};
    step=command(s,stop,2,reply);if(step==LT48_OK&&!reply[0])step=LT48_PROTOCOL_ERROR;
    if(step==LT48_OK)step=wave(s,4);
    if(step==LT48_OK)step=wave(s,1);
    if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
    if(step!=LT48_OK)status=step;
    /* A failed waveform must not prevent the independent rail-off attempts. */
    for(unsigned i=0;i<4;i++) {
        lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
        step=lt48_runtime_transport_session_create(s->api,s->context,&s->wire);
        unsigned char off[]={0x10,3-i,0};
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s->wire,off,sizeof(off));
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
        if(status==LT48_OK&&step!=LT48_OK)status=step;
    }
    lt48_runtime_transport_session_close(s->wire);s->wire=NULL;
    step=lt48_runtime_transport_session_create(s->api,s->context,&s->wire);
    const unsigned char h[]={0x38,0xaa,0x55},finish[]={0x49,0xaa,0x55};
    if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s->wire,h,sizeof(h));
    if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s->wire,s->r->idle,s->r->idle_size);
    if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s->wire,finish,sizeof(finish));
    if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s->wire);
    if(status==LT48_OK&&step!=LT48_OK)status=step;
    if(status==LT48_OK) {
        const unsigned char led[]={0x40,operation==LT48_OK?4:2};
        status=lt48_runtime_transport_session_controller_write(s->wire,led,sizeof(led));
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
    }
    return status;
}
lt48_status lt48_legacy_dataflash_validate(const lt48_legacy_dataflash_resources *r,
    lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t image_size,
    lt48_verify_margin_selection margin,unsigned margin_type) {
    if(!r||!length||start>=r->profile->capacity||length>r->profile->capacity-start)return LT48_INVALID_ARGUMENT;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_PROGRAM&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_BLANK_CHECK)return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&(!image||image_size<length))return LT48_BUFFER_TOO_SMALL;
    if(margin_type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL &&
        margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_legacy_dataflash_execute(const lt48_legacy_dataflash_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,
    const unsigned char *image,size_t image_size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_legacy_dataflash_result *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status valid=lt48_legacy_dataflash_validate(r,op,start,length,image,image_size,margin,margin_type);
    if(valid==LT48_OK&&(!api||!api->read||!api->write))valid=LT48_INVALID_ARGUMENT;
    if(valid==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))valid=LT48_BUFFER_TOO_SMALL;
    if(valid!=LT48_OK){out->operation_status=valid;return valid;}
    unsigned nominal=r->voltage.value,delta=nominal/(margin_type?10u:20u);
    unsigned verify_volts[2]={nominal-delta,nominal+delta};size_t verify_count=2;
    /* Original Program post-verify runs both margins. The explicit selection
     * controls standalone Verify; Compare/Blank retain nominal verification. */
    if(op!=LT48_OPERATION_PROGRAM && op!=LT48_OPERATION_ERASE) {
        if(op!=LT48_OPERATION_VERIFY || margin==LT48_VERIFY_MARGIN_NOMINAL){verify_volts[0]=nominal;verify_count=1;}
        else if(margin==LT48_VERIFY_MARGIN_LOW)verify_count=1;
        else if(margin==LT48_VERIFY_MARGIN_HIGH){verify_volts[0]=verify_volts[1];verify_count=1;}
    }
    size_t capacity=r->profile->capacity;unsigned char *pending=malloc(capacity),*expected=NULL;
    if(!pending)return LT48_NO_MEMORY;
    int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE;
    if(writing){expected=malloc(capacity);if(!expected){free(pending);return LT48_NO_MEMORY;}}
    dataflash_session s={0};s.r=r;s.api=api;s.context=context;s.voltage=r->voltage;
    lt48_status status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status==LT48_OK)status=setup(&s);
    /* Original applets always start at zero. Preserve untouched bytes and never
     * disguise a partial address as a token or a power-of-two page index. */
    if(status==LT48_OK)status=cycle(&s,1,pending,capacity,nominal);
    if(writing&&status==LT48_OK) {
        memcpy(expected,pending,capacity);
        if(op==LT48_OPERATION_ERASE)memset(expected+start,255,length);else memcpy(expected+start,image,length);
        status=cycle(&s,2,expected,capacity,nominal);
        for(size_t i=0;status==LT48_OK&&i<verify_count;i++)status=cycle(&s,4,expected,capacity,verify_volts[i]);
        if(status==LT48_OK)status=cycle(&s,1,pending,capacity,nominal);
    }
    if(status==LT48_OK&&op!=LT48_OPERATION_READ) {
        size_t count=writing?capacity:length;uint32_t base=writing?0:start;
        for(size_t i=0;i<count;i++) {
            unsigned char want=writing?expected[i]:op==LT48_OPERATION_BLANK_CHECK?255:image[i];
            if(pending[base+i]!=want){if(!out->mismatch_count)out->first_mismatch=base+(uint32_t)i;out->mismatch_count++;}
        }
        if(out->mismatch_count)status=LT48_DEVICE_MISMATCH;
        /* After matching the requested range in the host, run the original
         * verifier over the full source geometry. Outside the requested range,
         * the expected bytes are the snapshot just read from the device. */
        if(!writing)for(size_t i=0;status==LT48_OK&&i<verify_count;i++)status=cycle(&s,4,pending,capacity,verify_volts[i]);
    }
    out->operation_status=status;
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport clean=*api;clean.cancelled=NULL;s.api=&clean;
    out->cleanup_status=lt48_runtime_transport_session_create(&clean,context,&s.wire);
    if(out->cleanup_status==LT48_OK)out->cleanup_status=cleanup(&s,status);
    out->cleanup_completed=out->cleanup_status==LT48_OK;
    lt48_runtime_transport_session_close(s.wire);
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK){out->completed_bytes=length;if(op==LT48_OPERATION_READ)memcpy(read,pending+start,length);}
    free(expected);free(pending);return status;
}
