#include "gdac_serial.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gdac_serial_geometry.inc"

void lt48_gdac_serial_close(lt48_gdac_serial_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<28;i++)lt48_compiled_wave_close(r->waves[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
lt48_status lt48_gdac_serial_open(const lt48_installation *x,size_t device,
                                lt48_gdac_serial_resources **out) {
    static const struct {uint16_t d,m,p,bits,pins;uint32_t size;} approved[]={
#include "gdac_serial_profiles.inc"
    };
    lt48_device_spec spec;lt48_family_info family;lt48_pin_code pc;
    lt48_status status;lt48_gdac_serial_resources *r=NULL;
    char path[4096],name[24],hex[65];unsigned char digest[32];
    size_t index,fi;int admitted=0;
    if(!x||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    status=lt48_installation_resolve_device(x,device,&spec);
    if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2034)return LT48_UNSUPPORTED;
    for(size_t i=0;i<sizeof(approved)/sizeof(*approved);i++)
        if(spec.definition_resource==approved[i].d && spec.family_code==approved[i].m &&
           spec.pinmap==approved[i].p && spec.element_bits==approved[i].bits &&
           spec.pin_count==approved[i].pins && spec.capacity_bytes==approved[i].size)admitted=1;
    if(!admitted)return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/GDAC___.rez",lt48_installation_path(x));
    if(n<0||(size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
#define TRY(a) do {status=(a);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,"4abfb9024179d1cf38f17bc43ebfdaabaca4694b95e76e73edfdbbe7a0f46710")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->capacity=spec.capacity_bytes;r->definition=spec.definition_resource;
    r->man=spec.catalogue.manufacturer_id;r->dev=spec.catalogue.device_id;
    for(size_t i=0;i<sizeof(gdac_geometry)/sizeof(*gdac_geometry);i++)
        if(gdac_geometry[i].definition==r->definition){r->sectors=gdac_geometry[i].starts;r->sector_count=gdac_geometry[i].count;}
    if(!r->sector_count){status=LT48_FORMAT_ERROR;goto fail;}
    snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(!family.variable_count||family.variable_count>8||family.operation_count>28){status=LT48_FORMAT_ERROR;goto fail;}
    r->variable_count=family.variable_count;
    for(size_t i=0;i<r->variable_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->variables[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
        if(v.values[0]==0&&v.values[1]==0)r->nominal=v.values[3];
    }
    if(r->nominal!=310&&r->nominal!=320){status=LT48_FORMAT_ERROR;goto fail;}
    uint32_t seen=0;
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(op.operation_id>=28||(seen&(1u<<op.operation_id))){status=LT48_FORMAT_ERROR;goto fail;}
        seen|=1u<<op.operation_id;
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(code.wave_type==0) {
            lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(code.data,code.data_size,&w));
            /* Holtek compact27 uses the original zero lookup, emitting resident
             * Q2(2) no-op while preserving the complete DAC ramp. */
            status=lt48_compact_compile_with_pin_policy(w,r->pins,r->variables,r->variable_count,
                1,r->definition==0xa?LT48_PIN_FALLBACK_RESIDENT_NOOP_WITH_RAMP:LT48_PIN_FALLBACK_STRICT,&r->waves[op.operation_id]);
            lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            size_t defaults=lt48_compiled_wave_defaulted_pin_count(r->waves[op.operation_id]);
            if(defaults) {
                lt48_defaulted_logical_pin pin;
                if(r->definition!=0xa||op.operation_id!=2||defaults!=1){status=LT48_SAFETY_REJECTED;goto fail;}
                TRY(lt48_compiled_wave_get_defaulted_pin(r->waves[op.operation_id],0,&pin));
                if(pin.logical_pin!=4||pin.compact_offset!=39||pin.compact_opcode!=0x27||
                   pin.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP){status=LT48_SAFETY_REJECTED;goto fail;}
            }

        } else if(code.wave_type==6) {
            uint16_t addr=op.operation_id==16?0x5000:op.operation_id==17?0x6000:
                op.operation_id==19?0x7800:op.operation_id==20?0x8000:
                op.operation_id==21?0x7000:op.operation_id==23?0x9000:
                op.operation_id==25?0x9800:0;
            if(!addr||code.load_address!=addr||!code.data_size||code.data_size>0x800||
               code.length1!=code.data_size+4||code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            r->applets[op.operation_id]=code;
            r->addresses[r->address_count++]=(lt48_applet_address){op.operation_id,addr};
        } else if(code.wave_type==4&&op.operation_id==27&&code.data_size==2780&&
                  code.data[0]==0xda&&code.data[1]==0x0a)r->fpga=code.data+2;
        else {status=LT48_UNSUPPORTED;goto fail;}
    }
    for(unsigned i=0;i<4;i++)if(!r->waves[i]){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->fpga||!r->applets[17].data||!r->applets[19].data||
       !r->applets[20].data||!r->applets[21].data){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_gdac_serial_close(r);return status;
#undef TRY
}

typedef struct gdac_session {
    const lt48_gdac_serial_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[13][32];uint32_t volts[4];
    lt48_compile_variable vars[8];uint32_t mode;
} gdac_session;
static lt48_status run(gdac_session *s,unsigned op) {
    if(op>=28||!s->r->waves[op])return LT48_UNSUPPORTED;
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[12];size_t count=0;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    for(uint8_t i=0;i<13;i++)if(i!=7)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=7;
    options.address_buffer=0;options.parameter_buffer_id=2;options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));options.root_timeout_seconds=300;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,300000,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->waves[op],s->r->pins,s->vars,s->r->variable_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->address_count,&result);
    if(status==LT48_OK) {
        for(uint8_t i=0;i<13;i++) {
            const unsigned char *data;size_t size;status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK||size!=32){status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        for(uint8_t i=0;status==LT48_OK&&i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(event.kind==LT48_RUNTIME_RESULT_LATCH ||
               (event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0))status=LT48_RUNTIME_ERROR;
        }
    }
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_status transfer(gdac_session *s,int input,uint8_t endpoint,unsigned char *bytes,size_t length) {
    size_t done=0;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    lt48_status status=input?s->api->read(s->context,endpoint,bytes,length,&done):s->api->write(s->context,endpoint,bytes,length,&done);
    if(status==LT48_OK&&done!=length)status=LT48_PROTOCOL_ERROR;
    return status;
}
static lt48_status cmd(gdac_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    unsigned char packet[512]={0};if(n>512)return LT48_INVALID_ARGUMENT;
    memcpy(packet,p,n);lt48_status status=lt48_runtime_transport_session_flush(s->wire);
    if(status==LT48_OK)status=transfer(s,0,2,packet,512);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status flag(gdac_session *s,unsigned opcode,unsigned *value) {
    unsigned char p[2]={(unsigned char)opcode,1},reply[512]={0};
    lt48_status status=cmd(s,p,opcode==0x0b?1:2,reply);
    if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)*value=reply[2]&1;
    return status;
}
static lt48_status block(gdac_session *s,unsigned opcode,unsigned char *data,size_t n,unsigned char *reply) {
    unsigned char p[6]={(unsigned char)opcode,(unsigned char)(n/512),(unsigned char)(n/512>>8),
        (unsigned char)(n%512),(unsigned char)(n%512>>8),opcode==0x48?1:0};
    lt48_status status=cmd(s,p,6,NULL);
    if(status==LT48_OK)status=transfer(s,opcode==0x43,opcode==0x43?0x88:4,data,n);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status setup(gdac_session *s) {
    /* Native bridge initialization already establishes the transport. FX2
     * opcode 0x18 is reserved (RET at CODE:0a1f), not a reset with a reply. */
    lt48_status status=LT48_OK;
    /* Original 43d580 visits operation table slots, not source wave numbers. */
    for(unsigned i=0;status==LT48_OK&&i<28;i++) {
        const lt48_wave_code *a=&s->r->applets[i];if(!a->data)continue;size_t last=a->data_size-1;
        unsigned char header[]={0x39,0xaa,0x55,(unsigned char)a->load_address,(unsigned char)(a->load_address>>8),(unsigned char)last,(unsigned char)(last>>8)};
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,header,sizeof(header));
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,a->data,a->data_size);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
    }
    const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,7},busy[]={0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,fpga,sizeof(fpga));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,s->r->fpga,2778);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,selector,sizeof(selector));
    if(status==LT48_OK)status=run(s,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->wire,1);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof(busy));
    return status;
}
static lt48_status prepare(gdac_session *s);
static lt48_status cycle(gdac_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_gdac_serial_result *out) {
    for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(start>>(8*j));
    s->mode=mode;for(size_t i=0;i<s->r->variable_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status status=LT48_OK;
    if(mode==2)status=prepare(s);
    s->mode=mode;
    for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(start>>(8*j));
    if(status==LT48_OK)status=run(s,2);
    size_t chunk=length/100;if(!chunk)chunk=1;if(chunk>8192)chunk=8192;
    out->completed_bytes=0;
    for(size_t offset=0;status==LT48_OK&&offset<length;) {
        size_t n=length-offset;if(n>chunk)n=chunk;
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,mode==1?5000:mode==11?10000:60000,0);
        if(status!=LT48_OK)break;
        if(mode==1)status=block(s,0x43,read+offset,n,NULL);
        else if(mode==11) {
            unsigned char count[2]={(unsigned char)n,(unsigned char)(n>>8)};unsigned passed=0;
            status=block(s,0x44,count,2,NULL);
            if(status==LT48_OK)status=flag(s,0x0b,&passed);
            if(status==LT48_OK&&!passed) {
                unsigned char remaining[2]={0};status=block(s,0x43,remaining,2,NULL);
                size_t rem=remaining[0]|(size_t)remaining[1]<<8;
                if(status==LT48_OK) {
                    if(rem>=n)status=LT48_PROTOCOL_ERROR;
                    else {out->first_mismatch=start+(uint32_t)(offset+n-rem-1);out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}
                }
            }
        } else {
            unsigned char reply[512]={0};status=block(s,0x48,(unsigned char*)image+offset,n,reply);
            if(status==LT48_OK&&!(reply[0]|reply[1])) {
                /* The original derives an unchecked remaining count here.
                 * Never resume programming from untrusted failure counters. */
                out->mismatch_count++;out->first_mismatch=UINT32_MAX;status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK&&offset+n==length) {
                unsigned passed=0;status=flag(s,0x0c,&passed);
                if(status==LT48_OK&&!passed){out->mismatch_count++;out->first_mismatch=start+(uint32_t)(length-1);status=LT48_DEVICE_MISMATCH;}
            }
        }
        if(status==LT48_OK){offset+=n;out->completed_bytes=offset;}
    }
    /* DEVRESET must be sent even when the bulk worker failed. The final
     * uncancellable owner below also provides an independent reset path. */
    if(status==LT48_OK) {
        const unsigned char finish[]={0,0};unsigned char reply[512]={0};
        status=cmd(s,finish,sizeof(finish),reply);
        if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}

static void address(gdac_session *s,uint32_t value) {
    for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(value>>(8*j));
}
/* Original 467E00 invokes INIT(15), each primary-geometry UNLOCK wave,
 * then DEVRESET. AT26F004 deliberately retains power through that reset. */
static lt48_status prepare(gdac_session *s) {
    if(!s->r->waves[14])return LT48_OK;
    s->mode=15;address(s,0);lt48_status status=run(s,2);
    for(size_t i=0;status==LT48_OK&&i<s->r->sector_count;i++) {
        address(s,s->r->sectors[i]);s->buffers[2][0]=0;
        status=run(s,14);
        if(status==LT48_OK&&!s->buffers[2][0])status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}
static lt48_status erase(gdac_session *s) {
    if(!s->r->waves[10])return LT48_UNSUPPORTED;
    lt48_status status=prepare(s);s->mode=7;address(s,0);
    if(status==LT48_OK)status=run(s,2);
    for(size_t i=0;status==LT48_OK&&i<s->r->sector_count;i++) {
        address(s,s->r->sectors[i]);s->buffers[2][0]=0;
        status=run(s,10);
        if(status==LT48_OK&&!s->buffers[2][0])status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}
/* Exact original 467420 outputs, replayed in audit_gdac_serial_voltages.py. */
static uint16_t margin_voltage(const lt48_gdac_serial_resources *r,unsigned kind,int high) {
    if(kind==2)return r->nominal;
    if(r->nominal==310)return kind?(high?341:279):(high?326:295);
    return kind?(high?352:288):(high?336:304);
}
static lt48_status compare_readback(gdac_session *s,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *pending,lt48_gdac_serial_result *out) {
    lt48_status status=cycle(s,1,start,length,NULL,pending,s->r->nominal,out);
    if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=image[i]) {
        if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
        out->mismatch_count++;
    }
    return status==LT48_OK&&out->mismatch_count?LT48_DEVICE_MISMATCH:status;
}
static lt48_status identify(gdac_session *s,lt48_m37_id_decision_fn decide,void *context) {
    if(!s->r->waves[4])return LT48_OK;
    for(unsigned attempt=0;attempt<64;attempt++) {
        memset(s->buffers[4],0,32);memset(s->buffers[6],0,32);
        lt48_status status=run(s,4);if(status!=LT48_OK)return status;
        unsigned man=s->buffers[6][0]|(unsigned)s->buffers[6][1]<<8;
        unsigned dev=s->buffers[4][0]|(unsigned)s->buffers[4][1]<<8;
        int match=(s->r->man||s->r->dev)?man==s->r->man&&dev==s->r->dev:
            (man==dev && (man==0 || man==255));
        if(match)return LT48_OK;
        lt48_m37_id_choice choice=decide?decide(context,s->r->man,s->r->dev,man,dev):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)return LT48_OK;
        if(choice!=LT48_M37_ID_RETRY)return LT48_DEVICE_MISMATCH;
    }
    return LT48_SAFETY_REJECTED;
}
int lt48_gdac_serial_is_otp(const lt48_gdac_serial_resources *r){return r&&r->definition==0xa;}
uint32_t lt48_gdac_serial_capacity(const lt48_gdac_serial_resources *r){return r?r->capacity:0;}
lt48_status lt48_gdac_serial_validate(const lt48_gdac_serial_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size,
    lt48_verify_margin_selection margin,unsigned margin_type) {
    if(!r)return LT48_INVALID_ARGUMENT;
    if(r->definition==0xa&&op==LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
    if(!length||start>=r->capacity||length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_PROGRAM&&
       op!=LT48_OPERATION_VERIFY&&op!=LT48_OPERATION_COMPARE&&op!=LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE)&&
       (!image||size!=length))return LT48_INVALID_ARGUMENT;
    if(margin_type>2||(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&
       margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_gdac_serial_execute(const lt48_gdac_serial_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size,
    unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,
    unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,lt48_gdac_serial_result *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_gdac_serial_validate(r,op,start,length,image,size,margin,margin_type);
    if(!api||!api->write||!api->read)status=LT48_INVALID_ARGUMENT;
    if(status==LT48_OK&&op==LT48_OPERATION_READ&&(!read||read_size<length))status=LT48_BUFFER_TOO_SMALL;
    if(status!=LT48_OK){out->operation_status=status;return status;}
    int mutate=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_ERASE;
    size_t work_length=mutate?r->capacity:length;
    unsigned char *pending=malloc(work_length),*desired=mutate?malloc(work_length):NULL;
    if(!pending||(mutate&&!desired)){free(pending);free(desired);out->operation_status=LT48_NO_MEMORY;return LT48_NO_MEMORY;}
    gdac_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;
    memcpy(s.vars,r->variables,sizeof(s.vars));
    for(unsigned j=0;j<4;j++)s.buffers[1][j]=(unsigned char)(r->capacity>>(8*j));
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(pending);free(desired);out->operation_status=status;return status;}
    status=setup(&s);
    if(status==LT48_OK)status=identify(&s,decide,decision_context);
    if(status==LT48_OK&&r->definition==0xa&&op==LT48_OPERATION_PROGRAM) {
        /* OTP: blank-check the selected range, then program only that range.
         * Never emulate erase by writing FF or rewrite preserved neighbors. */
        status=cycle(&s,11,start,length,NULL,NULL,r->nominal,out);
        if(status==LT48_OK)status=cycle(&s,2,start,length,image,NULL,r->nominal,out);
        if(status==LT48_OK)status=compare_readback(&s,start,length,image,pending,out);
        for(unsigned hi=0;status==LT48_OK&&hi<2;hi++)
            status=cycle(&s,4,start,length,image,NULL,margin_voltage(r,margin_type,hi),out);
    } else if(status==LT48_OK&&mutate) {
        /* Chip erase requires retaining bytes outside the requested range. */
        if(start||length!=r->capacity)status=cycle(&s,1,0,r->capacity,NULL,desired,r->nominal,out);
        else memset(desired,255,r->capacity);
        if(status==LT48_OK) {
            if(op==LT48_OPERATION_ERASE)memset(desired+start,255,length);
            else memcpy(desired+start,image,length);
            if(r->waves[10])status=erase(&s);
            int program=op==LT48_OPERATION_PROGRAM||!r->waves[10]||start||length!=r->capacity;
            if(status==LT48_OK&&program)status=cycle(&s,2,0,r->capacity,desired,NULL,r->nominal,out);
            if(status==LT48_OK)status=compare_readback(&s,0,r->capacity,desired,pending,out);
            for(unsigned hi=0;status==LT48_OK&&hi<2;hi++)
                status=cycle(&s,4,0,r->capacity,desired,NULL,margin_voltage(r,margin_type,hi),out);
        }
    } else if(status==LT48_OK&&op==LT48_OPERATION_VERIFY) {
        uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};unsigned n=2;
        if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
        else if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
        else if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
        for(unsigned i=0;status==LT48_OK&&i<n;i++)status=cycle(&s,4,start,length,image,NULL,volts[i],out);
        if(status==LT48_OK)status=compare_readback(&s,start,length,image,pending,out);
    } else if(status==LT48_OK&&op==LT48_OPERATION_COMPARE) {
        status=cycle(&s,5,start,length,image,NULL,r->nominal,out);
        if(status==LT48_OK)status=compare_readback(&s,start,length,image,pending,out);
    } else if(status==LT48_OK)status=cycle(&s,op==LT48_OPERATION_READ?1:11,start,length,NULL,pending,r->nominal,out);
    out->operation_status=status;
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;s.api=&cleanup;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        unsigned char reply[512]={0};const unsigned char stop[]={0,0};
        lt48_status step=cmd(&s,stop,sizeof(stop),reply);
        if(step==LT48_OK&&!(reply[0]|reply[1]))step=LT48_PROTOCOL_ERROR;
        if(step!=LT48_OK)out->cleanup_status=step;
        for(unsigned op_index=0;op_index<2;op_index++) {
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=run(&s,op_index?1:3);
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        }
        for(unsigned channel=0;channel<4;channel++) {
            unsigned char off[]={0x10,(unsigned char)(3-channel),0};
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,off,sizeof(off));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        }
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
        step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
        const unsigned char idle_header[]={0x38,0xaa,0x55},finish[]={0x49,0xaa,0x55};
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,idle_header,sizeof(idle_header));
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
        if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,finish,sizeof(finish));
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        if(out->cleanup_status==LT48_OK) {
            unsigned char led[]={0x40,status==LT48_OK?4:2};
            step=lt48_runtime_transport_session_controller_write(s.wire,led,sizeof(led));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            out->cleanup_status=step;
        }
    }
    out->cleanup_completed=out->cleanup_status==LT48_OK;
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
    if(status==LT48_OK)out->completed_bytes=length;
    lt48_runtime_transport_session_close(s.wire);free(pending);free(desired);return status;
}
