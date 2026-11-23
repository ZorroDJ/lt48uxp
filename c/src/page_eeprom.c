#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../NewDeviceLib/sources/winbond-page2/profiles.h"
#include "../../NewDeviceLib/sources/page6/profiles.h"
#include "../../NewDeviceLib/sources/at29wordfour/policy.h"

/* Dataman 13.10 T29EPROMGroup. Exact X28C64 and AT28C256/F closures; audited CAT28C512P native composition.
 * The FPGA read path uses bridge 46/D0, not the XP engine 43 stream. */
struct lt48_page_eeprom_resources {
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[15];
    lt48_wave_code applet[4];
    lt48_applet_address addresses[4];
    const unsigned char *fpga;
    lt48_compile_variable vars[8];
    size_t var_count;
    uint32_t capacity;
    unsigned page_size;
    unsigned applet_count;
    int plain,winbond,tail6,word16;unsigned word_sdp;
    uint16_t nominal;

};
void lt48_page_eeprom_close(lt48_page_eeprom_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<15;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
unsigned lt48_page_eeprom_is_word(const lt48_page_eeprom_resources*r){return r?r->word16:0;}
lt48_status lt48_page_eeprom_set_sdp_choice(lt48_page_eeprom_resources*r,unsigned choice){if(!r||!r->word16||choice>1)return LT48_INVALID_ARGUMENT;r->word_sdp=choice;return LT48_OK;}
uint32_t lt48_page_eeprom_capacity(const lt48_page_eeprom_resources *r){return r?r->capacity:0;}
lt48_status lt48_page_eeprom_open(const lt48_installation *x,size_t device,lt48_page_eeprom_resources **out) {
    lt48_device_spec spec;lt48_family_info family;lt48_pin_code pc;
    lt48_page_eeprom_resources *r=NULL;lt48_status status;char path[4096],name[24],hex[65];
    size_t index,fi;unsigned char digest[32];
    if(!out||!x)return LT48_INVALID_ARGUMENT;
    *out=NULL;status=lt48_installation_resolve_device(x,device,&spec);if(status!=LT48_OK)return status;
    int word=at29word_binding(&spec);if(word&&(!LT48_AT29WORD_ADMITTED||strcmp(lt48_installation_source_profile(x),"Dataman 13.10")))return LT48_UNSUPPORTED;
    const struct page6_profile*p6=page6_find(&spec);
    if(p6&&!LT48_PAGE6_ADMITTED)return LT48_UNSUPPORTED;
    int winbond=winbond_page2_binding(&spec);
    if(winbond && strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;
    int cat512=spec.definition_resource==0xfffd && spec.family_code==0x14 &&
        spec.pinmap==3 && spec.capacity_bytes==65536 && spec.pin_count==32;
    /* Standard DIP32 source closures; Winbond admits its distinct
     * full-chip Erase13 below; standalone boot-block Protect14 is deferred. */
    int dip32=(spec.definition_resource==0x15 && spec.family_code==0x11 &&
        spec.pinmap==4 && spec.capacity_bytes==131072 && spec.pin_count==32) ||
        (spec.definition_resource==2 && spec.family_code==2 &&
        spec.pinmap==2 && spec.capacity_bytes==32768 && spec.pin_count==32);
    int plain256=spec.definition_resource==0x22 && spec.family_code==0x1a && spec.pinmap==9 && spec.capacity_bytes==32768;
    int plain=((spec.definition_resource==0x1c && spec.family_code==0x16 && spec.pinmap==7) ||
        (spec.definition_resource==0x1d && spec.family_code==0x16 && spec.pinmap==8) ||
        (spec.definition_resource==0x1e && spec.family_code==0x19 && spec.pinmap==7) ||
        (spec.definition_resource==0x1f && spec.family_code==0x17 && spec.pinmap==7) ||
        (spec.definition_resource==0x21 && spec.family_code==0x17 && spec.pinmap==8)) &&
        spec.capacity_bytes==8192 && spec.pin_count==28;
    plain=plain||plain256;
    int atmel=((spec.definition_resource==0x1b && spec.family_code==0x15 && spec.pinmap==9 && spec.capacity_bytes==32768) ||
        (spec.definition_resource==0x1a && spec.family_code==0x14 && spec.pinmap==9 && spec.capacity_bytes==32768) ||
        (spec.definition_resource==0x18 && spec.family_code==0x13 && spec.pinmap==7 && spec.capacity_bytes==8192) ||
        (spec.definition_resource==0x16 && spec.family_code==0x12 && spec.pinmap==7 && spec.capacity_bytes==8192) ||
        (spec.definition_resource==0x17 && spec.family_code==0x12 && spec.pinmap==8 && spec.capacity_bytes==8192));
    int xicor256=(spec.definition_resource==0x23 && spec.family_code==0x1b) ||
        (spec.definition_resource==0x24 && spec.family_code==0x1c);
    xicor256=xicor256 && spec.pinmap==9 && spec.capacity_bytes==32768;
    int xicor=(spec.definition_resource==0x20 && spec.family_code==0x18 &&
        spec.pinmap==7 && spec.capacity_bytes==8192) || xicor256;
    if(spec.catalogue.device_class!=2005 || (!winbond && !atmel && !xicor && !cat512 && !dip32 && !plain && !p6 && !word) ||
       spec.element_bits!=(word?16u:8u) || (!winbond && !cat512 && !dip32 && !p6 && !word && spec.pin_count!=28))return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/GAAC___.rez",lt48_installation_path(x));
    if(n<0||(size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
#define TRY(a) do{status=(a);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,"a5b774bb0fada8081d9a158f710586df4dafb71c298d4c049c625627dde173c4")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->word16=word;r->nominal=p6?p6->vcc:500;r->tail6=p6!=NULL;
    r->winbond=winbond;r->plain=plain;r->applet_count=p6?p6->applets:plain?3:4;
    r->capacity=spec.capacity_bytes;
    r->page_size=(cat512 || (dip32 && spec.capacity_bytes==131072) ||
        (xicor256 && spec.definition_resource==0x24))?128:64;
    if(plain)r->page_size=spec.family_code==0x16?32:spec.family_code==0x19?16:64;
    snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(family.variable_count!=(unsigned)(p6?p6->vars:plain&&!plain256?4:5) || family.operation_count!=(unsigned)(p6?p6->ops:winbond?13:plain?10:11)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
        if(cat512 && r->vars[i].variable_id==13)r->vars[i].value=128;
    }
    for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==13)r->page_size=r->vars[i].value;
    if(word)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==0)r->nominal=r->vars[i].value;
    if(!r->page_size || r->page_size>((winbond||p6||word)?256u:128u) || r->capacity%r->page_size){status=LT48_FORMAT_ERROR;goto fail;}
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(cat512 && op.operation_id==6)op.wave=0x25; /* Original DIP32, 16-address-bit FPGA. */
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(code.wave_type==0 && (op.operation_id<6 || (winbond&&(op.operation_id==13||op.operation_id==14)))) {
            lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(code.data,code.data_size,&w));
            status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,
                (winbond||atmel||cat512||dip32||plain||(p6&&p6->defaults))?LT48_PIN_FALLBACK_RESIDENT_NOOP:LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);
            lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            size_t defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
            if(defaults!=(size_t)((atmel||cat512||dip32||plain||(p6&&p6->defaults)) && op.operation_id==3)){status=LT48_SAFETY_REJECTED;goto fail;}
            if(defaults) {
                lt48_defaulted_logical_pin pin;
                TRY(lt48_compiled_wave_get_defaulted_pin(r->wave[op.operation_id],0,&pin));
                /* Original zero-filled lookup emits resident command 01 (RET).
                 * Exact extra DEVINIT instruction, absent in AEC 7.60. */
                if(pin.logical_pin!=18 || pin.compact_offset!=30 || pin.compact_opcode!=0x11 ||
                   pin.classification!=LT48_DEFAULTED_PIN_RESIDENT_NOOP){status=LT48_SAFETY_REJECTED;goto fail;}
            }
        } else if(code.wave_type==6) {
            unsigned slot=((plain||p6) && op.operation_id==9)?2:op.operation_id==7?0:op.operation_id==8?1:op.operation_id==11?2:op.operation_id==12?3:4;
            const uint16_t addr[]={0x5000,0x503a,p6?p6->third:plain?0x5252:(winbond||atmel||cat512||dip32)?0x5100:0x52df,(winbond||atmel||cat512||dip32||p6)?0x519f:0x5382};
            unsigned expected=word?(slot==0?0x5000:slot==1?0x5284:slot==2?0x5100:0x51b9):slot<4?addr[slot]:0;
            if(slot==4 || code.load_address!=expected || code.data_size==0 ||
               code.data_size>0x800 || code.length1!=code.data_size+4 || code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            r->applet[slot]=code;r->addresses[slot]=(lt48_applet_address){op.operation_id,code.load_address};
        } else if(code.wave_type==4 && op.operation_id==6 && code.data_size==2780 &&
                  code.data[0]==0xda && code.data[1]==0x0a)r->fpga=code.data+2;
        else {status=LT48_UNSUPPORTED;goto fail;}
    }
    for(unsigned i=0;i<6;i++)if(!r->wave[i]){status=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<r->applet_count;i++)if(!r->applet[i].data){status=LT48_FORMAT_ERROR;goto fail;}
    if(winbond&&(!r->wave[13]||!r->wave[14])){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->fpga){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_page_eeprom_close(r);return status;
#undef TRY
}
lt48_status lt48_page_eeprom_validate(const lt48_page_eeprom_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r||!length||start>=r->capacity||length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if(op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_ERASE && r->winbond && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op==LT48_OPERATION_SECURE && (r->plain||r->winbond||r->tail6))return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_SECURE && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE&&(!image||size!=length))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_page_eeprom_validate_verify_options(const lt48_page_eeprom_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned margin_type) {
    if(!r||margin_type>1)return LT48_INVALID_ARGUMENT;
    if(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&
       margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    if((r->tail6||r->word16)&&r->nominal<=360&&margin_type&&
      (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE||
       (op==LT48_OPERATION_VERIFY&&(margin==LT48_VERIFY_MARGIN_ALL||margin==LT48_VERIFY_MARGIN_HIGH))))return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
static uint16_t margin_voltage(const lt48_page_eeprom_resources *r,unsigned type,int high) {
    /* Original nominal-relative 5% / 10% margin choices at 5 V. */
    if(r->tail6){unsigned product=r->nominal*(high?(type?110u:105u):(type?90u:95u));unsigned q=product/100,rem=product%100;return (uint16_t)(q+(rem>50||(rem==50&&(q&1))));}
    return type?(high?550:450):(high?525:475);
}
typedef struct page_session {
    const lt48_page_eeprom_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[30][32];uint32_t volts[4];
    lt48_compile_variable vars[8];uint32_t mode;unsigned protect;
} page_session;
static lt48_status run(page_session *s,unsigned op) {
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[29];size_t count=0;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    for(uint8_t i=0;i<30;i++)if(i!=16)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_configuration_value configuration={0,1,0,s->protect?0:1};
    options.configuration=&configuration;options.configuration_count=1;
    options.host_staged_shutdown=s->r->tail6?4:s->r->winbond;
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=16;
    options.address_buffer=2;options.parameter_buffer_id=15;options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));options.root_timeout_seconds=10;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->applet_count,&result);
    if(status==LT48_OK) {
        for(uint8_t i=0;i<30;i++) {
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
static lt48_status transfer(page_session *s,int input,uint8_t endpoint,unsigned char *bytes,size_t length) {
    size_t done=0;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    lt48_status status=input?s->api->read(s->context,endpoint,bytes,length,&done):s->api->write(s->context,endpoint,bytes,length,&done);
    if(status==LT48_OK&&done!=length)status=LT48_PROTOCOL_ERROR;
    return status;
}
static lt48_status cmd(page_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    unsigned char packet[512]={0};if(n>512)return LT48_INVALID_ARGUMENT;
    memcpy(packet,p,n);lt48_status status=lt48_runtime_transport_session_flush(s->wire);
    if(status==LT48_OK)status=transfer(s,0,2,packet,512);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status block(page_session *s,unsigned opcode,unsigned char *data,size_t n,unsigned char *reply) {
    unsigned char p[6]={(unsigned char)opcode,(unsigned char)(n/512),(unsigned char)(n/512>>8),
        (unsigned char)(n%512),(unsigned char)(n%512>>8),opcode==0x46?0xd0:0};
    lt48_status status=cmd(s,p,6,NULL);
    if(status==LT48_OK)status=transfer(s,opcode==0x46,opcode==0x46?0x88:4,data,n);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status setup(page_session *s) {
    /* Native bridge initialization already establishes the transport. FX2
     * opcode 0x18 is reserved (RET at CODE:0a1f), not a reset with a reply. */
    lt48_status status=LT48_OK;
    /* Original 43d580 visits operation table slots, not source wave numbers. */
    for(unsigned i=0;status==LT48_OK&&i<s->r->applet_count;i++) {
        const lt48_wave_code *a=&s->r->applet[i];size_t last=a->data_size-1;
        unsigned char header[]={0x39,0xaa,0x55,(unsigned char)a->load_address,(unsigned char)(a->load_address>>8),(unsigned char)last,(unsigned char)(last>>8)};
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,header,sizeof(header));
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,a->data,a->data_size);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
    }
    const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,s->r->nominal>400?15:7},busy[]={0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,fpga,sizeof(fpga));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,s->r->fpga,2778);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,selector,sizeof(selector));
    if(status==LT48_OK)status=run(s,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->wire,1);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof(busy));
    return status;
}
/* Read, Blank, Compare and margin Verify use the original FPGA read path.
 * Comparing actual bytes also avoids trusting the controller's status bit. */
static lt48_status cycle(page_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_page_eeprom_result *out) {
    for(unsigned j=0;j<4;j++)s->buffers[2][j]=(unsigned char)((s->r->word16?start/2:start)>>(8*j));
    s->mode=mode==2?2:1;
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status status=run(s,3);out->completed_bytes=0;
    if(status==LT48_OK&&s->r->word16&&mode!=2&&(start&1)){unsigned char skipped;status=block(s,0x46,&skipped,1,NULL);}
    for(size_t offset=0;status==LT48_OK&&offset<length;) {
        size_t n=length-offset;if(n>s->r->page_size)n=s->r->page_size;unsigned char bytes[256];
        if(mode==2) {
            if(n!=s->r->page_size || ((start+offset)&(s->r->page_size-1)))return LT48_SAFETY_REJECTED;
            for(unsigned j=0;j<4;j++)s->buffers[2][j]=(unsigned char)((s->r->word16?(start+offset)/2:start+offset)>>(8*j));
            status=run(s,5);
            if(status==LT48_OK)status=block(s,0x44,(unsigned char*)image+offset,n,NULL);
            /* The downloaded applet waits tWC=10 ms. Status handshake below
             * must also complete before another page or power-off is issued. */
            if(status==LT48_OK)status=lt48_nvram_store_hold_internal();
            if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->wire,1);
        } else {
            status=block(s,0x46,bytes,n,NULL);
            if(status==LT48_OK && mode==1)memcpy(read+offset,bytes,n);
            if(status==LT48_OK && mode!=1)for(size_t i=0;i<n;i++)
                if(bytes[i]!=(mode==11?255:image[offset+i])) {
                    if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)(offset+i);
                    out->mismatch_count++;
                }
        }
        if(status==LT48_OK){offset+=n;out->completed_bytes=offset;}
    }
    if(status==LT48_OK) {
        const unsigned char finish[]={0,0};unsigned char reply[512]={0};
        status=cmd(s,finish,sizeof(finish),reply);
        if(status==LT48_OK&&!(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    }
    if(status==LT48_OK)status=run(s,4);
    if(status==LT48_OK&&out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    return status;
}
lt48_status lt48_page_eeprom_execute(const lt48_page_eeprom_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_page_eeprom_result *out) {
    if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_page_eeprom_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_page_eeprom_validate_verify_options(r,op,margin,margin_type);
    if(status!=LT48_OK)return status;
    (void)decide;(void)decision_context;
    int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE||op==LT48_OPERATION_SECURE;
    unsigned char *pending=malloc(r->capacity),*desired=malloc(r->capacity);
    if(!pending||!desired){free(pending);free(desired);return LT48_NO_MEMORY;}
    /* Preserve the complete array, including both partial page edges. */
    if(writing) {
        lt48_page_eeprom_result before;
        status=lt48_page_eeprom_execute(r,api,context,LT48_OPERATION_READ,0,r->capacity,NULL,0,desired,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,0,NULL,NULL,&before);
        if(status!=LT48_OK){*out=before;free(pending);free(desired);return status;}
        if(op==LT48_OPERATION_ERASE)memset(desired+start,255,length);
        else if(op!=LT48_OPERATION_SECURE)memcpy(desired+start,image,length);
    }
    page_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;s.protect=op==LT48_OPERATION_SECURE||(r->word16&&!r->word_sdp);memcpy(s.vars,r->vars,sizeof(s.vars));
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(pending);free(desired);return status;}
    status=setup(&s);
    if(status==LT48_OK) {
        if(r->winbond && op==LT48_OPERATION_ERASE) {
            s.mode=7;memset(s.buffers[2],0,32);status=run(&s,3);
            if(status==LT48_OK)status=run(&s,13);
            if(status==LT48_OK)status=run(&s,4);
            if(status==LT48_OK)status=cycle(&s,11,0,r->capacity,NULL,NULL,r->nominal,out);
        } else if(writing) {
            uint32_t mask=r->page_size-1,first=start&~mask;size_t span=((start+length+mask)&~(size_t)mask)-first;
            status=cycle(&s,2,first,span,desired+first,NULL,r->nominal,out);
            for(unsigned i=0;status==LT48_OK&&i<2;i++)status=cycle(&s,4,0,r->capacity,desired,NULL,margin_voltage(r,margin_type,i),out);
        } else if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};size_t n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
            for(size_t i=0;status==LT48_OK&&i<n;i++)status=cycle(&s,4,start,length,image,NULL,volts[i],out);
        } else status=cycle(&s,op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:5,start,length,image,pending,r->nominal,out);
    }
    out->operation_status=status;
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;s.api=&cleanup;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        const unsigned char stop[]={0,0};unsigned char reply[512]={0};
        lt48_status step=cmd(&s,stop,sizeof(stop),reply);
        if(step==LT48_OK&&!(reply[0]|reply[1]))step=LT48_PROTOCOL_ERROR;
        if(step==LT48_OK)step=run(&s,4);
        if(step==LT48_OK)step=run(&s,1);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(step!=LT48_OK)out->cleanup_status=step;
        for(unsigned channel=0;channel<4;channel++) {
            unsigned char off[]={0x10,(unsigned char)(3-channel),0};
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(s.wire,off,sizeof(off));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
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
        if(out->cleanup_status==LT48_OK&&step!=LT48_OK)out->cleanup_status=step;
        out->cleanup_completed=out->cleanup_status==LT48_OK;
        if(out->cleanup_completed) {
            unsigned char led[]={0x40,status==LT48_OK?4:2};
            step=lt48_runtime_transport_session_controller_write(s.wire,led,sizeof(led));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
            if(step!=LT48_OK)out->cleanup_status=step;
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    if(status==LT48_OK && writing) {
        /* Fresh power/setup/read after cleanup also rejects a floating bus
         * echoing the last written byte during same-session verification. */
        lt48_page_eeprom_result fresh;
        status=lt48_page_eeprom_execute(r,api,context,LT48_OPERATION_READ,0,r->capacity,
            NULL,0,pending,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,0,NULL,NULL,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(size_t i=0;i<r->capacity;i++)if(pending[i]!=desired[i]) {
            if(!out->mismatch_count)out->first_mismatch=(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
        out->operation_status=status;
        if(status!=LT48_OK && out->cleanup_completed) {
            const unsigned char error_led[]={0x40,2};
            lt48_status led=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(led==LT48_OK)led=lt48_runtime_transport_session_controller_write(s.wire,error_led,sizeof(error_led));
            if(led==LT48_OK)led=lt48_runtime_transport_session_flush(s.wire);
            if(led!=LT48_OK){out->cleanup_status=led;out->cleanup_completed=0;}
            lt48_runtime_transport_session_close(s.wire);
        }
    }
    if(status==LT48_OK)out->completed_bytes=length;
    free(pending);free(desired);return status;
}

int lt48_page6_member_internal(unsigned cls,unsigned d){return page6_member(cls,d);}
