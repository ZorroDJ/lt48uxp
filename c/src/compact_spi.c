#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct lt48_compact_spi_resources {

    unsigned chunk,configuration;
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[16];
    lt48_compile_variable vars[8];size_t var_count;
    unsigned buffer_count,mode_buffer,address_buffer,input_buffer,output_buffer;
    unsigned init_op,reset_op,load_op,program_op,width;uint32_t capacity;uint16_t nominal;
};
void lt48_compact_spi_close(lt48_compact_spi_resources *r) {
    if(!r)return;
    for(size_t i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    free(r->idle);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r);
}
uint32_t lt48_compact_spi_capacity(const lt48_compact_spi_resources *r){return r?r->capacity:0;}
int lt48_compact_spi_has_protection(const lt48_compact_spi_resources *r){return r && r->configuration;}
int lt48_compact_spi_has_idlock(const lt48_compact_spi_resources *r){return r && (r->configuration==8 || r->configuration==9);}
int lt48_compact_spi_has_raw_status(const lt48_compact_spi_resources *r){return r && (r->configuration==10 || r->configuration==11);}
int lt48_compact_spi_has_incremental_registers(const lt48_compact_spi_resources *r){return r && r->configuration==11;}
lt48_status lt48_compact_spi_open(const lt48_installation *x,size_t device,lt48_compact_spi_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,device,&spec);
    if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2000 || strcmp(spec.definition_stem,"GA_____"))return LT48_UNSUPPORTED;
    static const unsigned additions[][8]={
#include "serial_legacy_spi.inc"
        /* GA Dce/Dcf: 16-byte page, explicit three-bit IDLock status. */
        {0xce,0x57,0x25,512,8,16,8,2},
        {0xcf,0x58,0x25,1024,8,16,9,2},
        {0xdf,0x67,0x2d,65536,8,128,10,5},
        {0xe4,0x6c,0x2d,65536,8,128,10,5},
        {0xa2,0x3f,0x18,1024,8,2,11,2},
    };
    const unsigned *extra=NULL;
    for(size_t i=0;i<sizeof(additions)/sizeof(*additions);i++) {
        const unsigned *a=additions[i];
        if(spec.definition_resource==a[0] && spec.family_code==a[1] &&
           spec.pinmap==a[2] && spec.capacity_bytes==a[3] && spec.pin_count==a[4] && spec.element_bits==8)extra=a;
    }
    if(!extra && (spec.definition_resource!=0x96 || spec.family_code!=0x38 || spec.pinmap!=0x1b ||
       spec.capacity_bytes!=256 || spec.element_bits!=8 || spec.pin_count!=8 || spec.configuration_set!=0))return LT48_UNSUPPORTED;
    lt48_compact_spi_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    char path[4096],name[32],hex[65];unsigned char digest[32];size_t index,fi;
    int n=snprintf(path,sizeof(path),"%s/GA_____.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof(path)){free(r);return LT48_INVALID_ARGUMENT;}
#define TRY(call) do{status=(call);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(size_t i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,"a286d6799117d4e188067376cab75843fbb9299f4990091d91ad27d8abab99ad")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->chunk=extra?extra[5]:4;r->configuration=extra?extra[6]:0;
    r->capacity=spec.capacity_bytes;r->width=spec.element_bits/8;
    r->buffer_count=70;r->mode_buffer=7;r->address_buffer=1;r->input_buffer=2;r->output_buffer=3;
    r->init_op=2;r->reset_op=6;r->load_op=7;r->program_op=9;
    lt48_pin_code code;snprintf(name,sizeof(name),"P%x",spec.pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&code));TRY(lt48_pin_map_parse(&code,&r->pins));
    TRY(lt48_pin_map_complete_groups_internal(r->pins,13));
    lt48_family_info info;snprintf(name,sizeof(name),"M%x",spec.family_code);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&info));
    if(info.variable_count!=(extra?extra[7]:2u) || info.operation_count!=((lt48_compact_spi_has_idlock(r)||r->configuration==10)?9u:extra?8u:6u)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=info.variable_count;
    for(size_t i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};if(v.values[1]==0)r->nominal=v.values[3];}
    for(size_t i=0;i<info.operation_count;i++){
        lt48_family_operation op;lt48_wave_code wave;lt48_compact_wave *compact=NULL;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));if(op.operation_id>=16){status=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&wave));
        if(wave.wave_type!=0){status=LT48_FORMAT_ERROR;goto fail;}
        TRY(lt48_compact_decode(wave.data,wave.data_size,&compact));
        status=lt48_compact_compile_with_pin_policy(compact,r->pins,r->vars,r->var_count,1u,
            r->configuration==11?LT48_PIN_FALLBACK_RESIDENT_NOOP:LT48_PIN_FALLBACK_STRICT,&r->wave[op.operation_id]);lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
        size_t defaults=lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id]);
        if(defaults && !(r->configuration==11 && op.operation_id==2 && defaults==1)){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    if(r->configuration==10) {
        /* We5's original CS / WRSR / buffer10 / delay suffix, preceded by
         * its RESET caller's WREN. Keep BP bits instead of We4's lossy Boolean.
         * GA digest above binds both byte sequences. No pin/timing changes. */
        lt48_wave_code source;TRY(lt48_rez_find_resource(r->rez,"We5",&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&source));
        if(source.data_size!=0x49){status=LT48_FORMAT_ERROR;goto fail;}
        static const unsigned char wren[]={0x11,0x0b,1,0x11,0x0b,0,
            0x1d,0x1d,0x17,6,0,0,0,8,1,0x11,0x0b,1};
        unsigned char raw[128];size_t used=2;
        memcpy(raw+used,wren,sizeof(wren));used+=sizeof(wren);
        memcpy(raw+used,source.data+0x2f,source.data_size-0x2f);used+=source.data_size-0x2f;
        raw[0]=(unsigned char)(used-2);raw[1]=0;
        lt48_compact_wave *wave=NULL;TRY(lt48_compact_decode(raw,used,&wave));
        status=lt48_compact_compile(wave,r->pins,r->vars,r->var_count,1u,0,&r->wave[11]);
        lt48_compact_wave_close(wave);if(status!=LT48_OK)goto fail;
    }
    if(r->configuration==11) {
        lt48_wave_code source;TRY(lt48_rez_find_resource(r->rez,"W72",&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&source));
        if(source.data_size!=0x6b){status=LT48_FORMAT_ERROR;goto fail;}
        unsigned char raw[128];size_t used=5;raw[2]=0x11;raw[3]=0x0b;raw[4]=1;
        memcpy(raw+used,source.data+0x41,source.data_size-0x41);used+=source.data_size-0x41;
        raw[0]=(unsigned char)(used-2);raw[1]=0;
        lt48_compact_wave *wave=NULL;TRY(lt48_compact_decode(raw,used,&wave));
        status=lt48_compact_compile(wave,r->pins,r->vars,r->var_count,1u,0,&r->wave[11]);
        lt48_compact_wave_close(wave);if(status!=LT48_OK)goto fail;
    }
    /* This exact D96 has no configuration set or ProgramConf binding. Its
     * shared DEVRESET calls operation 13 nonetheless. Bind an explicit empty
     * configuration action; retain every electrical reset instruction. */
    if(!extra) { const unsigned char empty[]={0,0};lt48_compact_wave *compact=NULL;
      TRY(lt48_compact_decode(empty,sizeof(empty),&compact));
      status=lt48_compact_compile(compact,r->pins,r->vars,r->var_count,1u,0,&r->wave[13]);
      lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail; }
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_compact_spi_close(r);return status;
#undef TRY
}
lt48_status lt48_compact_spi_validate(const lt48_compact_spi_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=r->capacity || length>r->capacity-start || start%r->width || length%r->width)return LT48_SAFETY_REJECTED;
    if(r->configuration==11 && op==LT48_OPERATION_ERASE)return LT48_UNSUPPORTED;
    if((lt48_compact_spi_has_idlock(r)||lt48_compact_spi_has_raw_status(r)) && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)) {
        if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
        /* An explicit selector is essential: IDLock cannot lock the whole array. */
        if(!image || size!=1)return LT48_INVALID_ARGUMENT;
        if(lt48_compact_spi_has_raw_status(r)) {
            if((image[0]&~0x8c) || (r->configuration==11 && (image[0]&12)==12) || (op==LT48_OPERATION_SECURE && !(image[0]&12)))return LT48_INVALID_ARGUMENT;
        } else if(image[0]>7 || (op==LT48_OPERATION_SECURE && !image[0]))return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(op<LT48_OPERATION_READ || op>(r->configuration?LT48_OPERATION_SECURE:LT48_OPERATION_ERASE))return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE) && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_ERASE && op!=LT48_OPERATION_SECURE && (!image || size!=length))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_compact_spi_validate_verify_options(const lt48_compact_spi_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type) {
    if(!r || type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL &&
       margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
    (void)op;
    return LT48_OK;
}
typedef struct compact_spi_session {
    const lt48_compact_spi_resources *r;
    lt48_runtime_transport_session *wire;
    const lt48_protocol_transport *api;
    void *context;
    unsigned char buffers[70][32];
    uint32_t volts[4];
    lt48_compile_variable vars[8];
    uint32_t mode;
    unsigned fault;
    lt48_runtime_configuration_value config[16];size_t config_count;
    unsigned idlock_saved,idlock_dirty;
    unsigned raw_saved,raw_dirty;
    const unsigned char *original;
} compact_spi_session;
static lt48_status run(compact_spi_session *s,unsigned op) {
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[70];size_t count=0;
    lt48_runtime_nested_wave nested[16];size_t nested_count=0;
    for (uint16_t i=0;i<16;i++)if(s->r->wave[i]) nested[nested_count++]=(lt48_runtime_nested_wave){i,s->r->wave[i]};
    options.nested_waves=nested;options.nested_wave_count=nested_count;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    s->fault=0;
    for(uint8_t i=0;i<s->r->buffer_count;i++) if(i!=s->r->mode_buffer) seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    options.configuration=s->config;options.configuration_count=s->config_count;
    options.mode=s->mode;options.use_explicit_buffer_ids=1;
    options.mode_buffer=s->r->mode_buffer;options.address_buffer=s->r->address_buffer;options.parameter_buffer_id=s->r->input_buffer;
    options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));
    options.root_timeout_seconds=10;
    lt48_status status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,
        s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run(s->wire,machine,&result);
    if(status==LT48_OK) {
        const lt48_runtime_configuration_value *values;size_t count;
        status=lt48_runtime_get_configuration(result,&values,&count);
        if(status==LT48_OK && count>16)status=LT48_RUNTIME_ERROR;
        if(status==LT48_OK){memcpy(s->config,values,count*sizeof(*values));s->config_count=count;}
        for(uint8_t i=0;status==LT48_OK && i<s->r->buffer_count;i++) {
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
static lt48_compile_variable *var(compact_spi_session *s,unsigned id) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==id)return &s->vars[i];
    return NULL;
}
static lt48_status idlock_value(const compact_spi_session *s,unsigned *value) {
    unsigned field=s->r->configuration==8?4:5;
    if(s->config_count!=1 || s->config[0].object_id!=0 ||
       s->config[0].category!=1 || s->config[0].field!=field || s->config[0].value>7)
        return LT48_DEVICE_MISMATCH; /* FF also means a busy, absent or unreadable part. */
    *value=s->config[0].value;return LT48_OK;
}
static lt48_status idlock_read(compact_spi_session *s,unsigned *value) {
    memset(s->buffers[10],0,32);
    lt48_status status=run(s,12);
    return status==LT48_OK?idlock_value(s,value):status;
}
static lt48_status idlock_write(compact_spi_session *s,unsigned value) {
    /* Mode 8 INIT establishes CS high and controls without Wb0 unprotect.
     * Wae/Wb1 are the source WREN + IDLock + two tP delays. */
    if(value>7)return LT48_INVALID_ARGUMENT;
    lt48_compile_variable *v=var(s,0);if(v)v->value=s->r->nominal;
    s->mode=8;memset(s->buffers,0,sizeof(s->buffers));
    s->config[0]=(lt48_runtime_configuration_value){0,1,s->r->configuration==8?4:5,value};s->config_count=1;
    lt48_status status=run(s,0);
    if(status==LT48_OK)status=run(s,2);
    if(status==LT48_OK)status=run(s,11);
    unsigned readback=0;
    if(status==LT48_OK)status=idlock_read(s,&readback);
    if(status==LT48_OK && readback!=value)status=LT48_DEVICE_MISMATCH;
    return status;
}
static lt48_status idlock_restore(compact_spi_session *s) {
    if(!s->idlock_dirty)return LT48_OK;
    lt48_status status=idlock_write(s,s->idlock_saved);
    if(status==LT48_OK)s->idlock_dirty=0;
    return status;
}
/* Raw writable SRWD/BP bits are separate from the original UI fields. */
static lt48_status raw_read(compact_spi_session *s,unsigned *value) {
    lt48_status status=run(s,12);
    unsigned raw=s->buffers[10][0];
    if(status==LT48_OK && (raw==255 || (raw&1) || (s->r->configuration==11 && (raw&12)==12)))status=LT48_DEVICE_MISMATCH;
    if(status==LT48_OK)*value=raw&0x8c;
    return status;
}
static lt48_status raw_write(compact_spi_session *s,unsigned value) {
    lt48_compile_variable *v=var(s,0);if(v)v->value=s->r->nominal;
    s->mode=7;lt48_status status=run(s,0);
    if(status==LT48_OK)status=run(s,2); /* source mode7 establishes CS high */
    memset(s->buffers[10],0,32);s->buffers[10][0]=(unsigned char)value;
    if(status==LT48_OK)status=run(s,11);
    unsigned actual=0;if(status==LT48_OK)status=raw_read(s,&actual);
    if(status==LT48_OK && actual!=value)status=LT48_DEVICE_MISMATCH;
    return status;
}
static lt48_status raw_restore(compact_spi_session *s) {
    if(!s->raw_dirty)return LT48_OK;
    lt48_status status=raw_write(s,s->raw_saved);
    if(status==LT48_OK)s->raw_dirty=0;
    return status;
}
static lt48_status flash_erase(compact_spi_session *s) {
    s->mode=7;lt48_status status=run(s,0);
    if(status==LT48_OK)status=run(s,2);
    if(status==LT48_OK)status=run(s,14);
    unsigned actual=0;if(status==LT48_OK)status=raw_read(s,&actual);
    if(status==LT48_OK && actual)status=LT48_DEVICE_MISMATCH;
    return status;
}
static int persistent_config_equal(const compact_spi_session *s,
    const lt48_runtime_configuration_value *wanted,size_t count) {
    for(size_t i=0;i<count;i++) {
        /* WIP and WEL are volatile observations in the early ST maps. */
        if(s->r->configuration<=2 && wanted[i].category==0 && (wanted[i].field==8 || wanted[i].field==9))continue;
        int found=0;
        for(size_t j=0;j<s->config_count;j++)if(wanted[i].category==s->config[j].category &&
            wanted[i].field==s->config[j].field) {
            /* Compare the bits consumed by the original status-write wave.
             * Its scratch bit copies leave unrelated high bits untouched. */
            uint32_t mask=wanted[i].category==0?1u:(wanted[i].field==0?1u:3u);
            if((wanted[i].value&mask)==(s->config[j].value&mask))found=1;
        }
        if(!found)return 0;
    }
    return 1;
}
static lt48_status cycle(compact_spi_session *s,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_compact_spi_result *out) {
    lt48_runtime_configuration_value saved[16];size_t saved_count=s->config_count;
    memcpy(saved,s->config,saved_count*sizeof(*saved));
    memset(s->buffers,0,sizeof(s->buffers));
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    s->mode=writing?2:1;
    int idlock=lt48_compact_spi_has_idlock(s->r);
    if(writing && idlock) {
        lt48_status checked=idlock_value(s,&s->idlock_saved);
        if(checked!=LT48_OK)return checked;
        /* Mark before the source INIT can clear the nonvolatile status. */
        s->idlock_dirty=1;
    }
    if(writing && ((start&(s->r->chunk-1))||(length&(s->r->chunk-1))))return LT48_SAFETY_REJECTED;
    lt48_compile_variable *v=var(s,0);if(v)v->value=vcc;
    /* Apply the selected margin to both source POWERON and DEVINIT. */
    lt48_status status=run(s,0);
    for(unsigned settle=0;status==LT48_OK && settle<3;settle++)status=lt48_runtime_transport_session_status(s->wire,1);
    /* Source INIT starts sequential SPI reads at Address; programming INIT
     * uses zero to request temporary unprotection, retaining saved settings. */
    uint32_t base=(!writing && s->r->configuration==7)?0:start;
    if(!writing)for(unsigned j=0;j<4;j++)s->buffers[1][j]=(unsigned char)(base>>(8*j));
    if(status==LT48_OK)status=run(s,s->r->init_op);
    if(status==LT48_OK && !writing && lt48_compact_spi_has_raw_status(s->r) &&
       (s->buffers[10][0]==255 || (s->buffers[10][0]&1) || (s->r->configuration==11 && (s->buffers[10][0]&12)==12)))status=LT48_DEVICE_MISMATCH;
    if(status==LT48_OK && idlock) {
        unsigned value;
        status=writing?idlock_read(s,&value):idlock_value(s,&value);
        if(status==LT48_OK && writing && value!=0)status=LT48_DEVICE_MISMATCH;
    }
    for(uint32_t pos=base;status==LT48_OK && pos<start+length;pos+=s->r->width){
        size_t i=pos>=start?pos-start:0;
        if(writing && s->r->configuration==11 && pos<32 && !(pos&1) && s->original &&
           image[i]==s->original[pos] && image[i+1]==s->original[pos+1]) {
            pos++;out->completed_bytes=i+2;continue; /* do not re-burn unchanged counters */
        }
        uint32_t address=pos/s->r->width;
        for(unsigned j=0;j<4;j++)s->buffers[1][j]=(unsigned char)(address>>(8*j));
        memset(s->buffers[2],0,32);memset(s->buffers[3],0,32);
        for(unsigned j=0;j<s->r->width;j++)s->buffers[3][j]=op==LT48_OPERATION_BLANK_CHECK?(s->r->configuration==11 && pos<32?0:255):image?image[i+j]:0;
        status=run(s,(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)?9:7);
        if(status==LT48_OK && writing && s->r->configuration==11 &&
           ((s->buffers[10][0]&1) || (pos<32 && (pos&1) && (s->buffers[10][0]&16))))status=LT48_DEVICE_MISMATCH;
        if(status!=LT48_OK)break;
        if(pos<start)continue;
        for(unsigned j=0;j<s->r->width;j++){
            if(read)read[i+j]=s->buffers[2][j];
            if(!writing && op!=LT48_OPERATION_READ && s->buffers[2][j]!=s->buffers[3][j]){if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i+j;out->mismatch_count++;}
        }
        out->completed_bytes=i+s->r->width;
        if(s->api->progress)s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,out->completed_bytes,length,0,0,0);
        if(out->mismatch_count && op!=LT48_OPERATION_COMPARE){status=LT48_DEVICE_MISMATCH;break;}
    }
    /* LOAD closes CS per byte. PROGRAM closes CS after each source-defined page;
     * the next page's tP delay waits for the preceding nonvolatile write. */
    if(writing && status==LT48_OK) {
        const unsigned char delay[]={0x4f,0x64,0x00}; /* source tP = 10000 */
        status=lt48_runtime_transport_session_controller_write(s->wire,delay,sizeof(delay));
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s->wire);
    }
    if(writing && lt48_compact_spi_has_raw_status(s->r))s->mode=7; /* skip lossy We5 restoration */
    lt48_status reset=run(s,s->r->reset_op);
    if(status==LT48_OK)status=reset;
    if(writing && idlock) {
        reset=idlock_restore(s);if(status==LT48_OK)status=reset;
    } else if(status==LT48_OK && writing && s->r->configuration && !lt48_compact_spi_has_raw_status(s->r)) {
        status=run(s,12);
        if(status==LT48_OK && !persistent_config_equal(s,saved,saved_count))status=LT48_DEVICE_MISMATCH;
    }
    return status==LT48_OK && out->mismatch_count?LT48_DEVICE_MISMATCH:status;
}
lt48_status lt48_compact_spi_execute(const lt48_compact_spi_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned margin_type,lt48_compact_spi_result *out) {
    if(!out || !api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_compact_spi_validate(r,op,start,length,image,size);if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ && (!read || read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_compact_spi_validate_verify_options(r,op,margin,margin_type);if(status!=LT48_OK)return status;
    unsigned char *pending=malloc(r->capacity);if(!pending)return LT48_NO_MEMORY;
    unsigned char *expected=malloc(r->capacity);
    if(!expected){free(pending);return LT48_NO_MEMORY;}
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE;
    compact_spi_session s={0};s.r=r;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof(s.vars));s.mode=1;
    status=lt48_runtime_transport_session_create(api,context,&s.wire);if(status!=LT48_OK){free(expected);free(pending);return status;}
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
    if(status==LT48_OK && writing) {
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,out);
        if(status==LT48_OK){memcpy(expected,pending,r->capacity);if(op==LT48_OPERATION_ERASE)memset(expected+start,255,length);else memcpy(expected+start,image,length);}
    }
    if(status==LT48_OK && writing) {
        uint32_t mask=r->chunk-1,first=start&~mask;size_t span=((start+length+mask)&~(size_t)mask)-first;
        if(lt48_compact_spi_has_raw_status(r)) {
            s.raw_saved=s.buffers[10][0]&0x8c;s.raw_dirty=1;
            if(r->configuration==11) {
                status=raw_write(&s,0);s.original=pending;
                if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_PROGRAM,first,span,expected+first,NULL,nominal,out);
            } else {
            status=flash_erase(&s);
            /* Explicit erase and complete reconstruction preserve neighbors
             * across arbitrary NOR 0->1 transitions; source page size is128. */
            if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_BLANK_CHECK,0,r->capacity,NULL,NULL,nominal,out);
            if(status==LT48_OK && op!=LT48_OPERATION_ERASE)status=cycle(&s,LT48_OPERATION_PROGRAM,0,r->capacity,expected,NULL,nominal,out);
            }
            lt48_status restored=raw_restore(&s);if(status==LT48_OK)status=restored;
        } else status=cycle(&s,LT48_OPERATION_PROGRAM,first,span,expected+first,NULL,nominal,out);
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,expected,NULL,lo,out);
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,expected,NULL,hi,out);
    }else if(status==LT48_OK && lt48_compact_spi_has_raw_status(r) && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)){
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,expected,nominal,out);
        if(status==LT48_OK){s.raw_saved=s.buffers[10][0]&0x8c;s.raw_dirty=1;status=raw_write(&s,image[0]);}
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,expected,NULL,nominal,out);
        if(status==LT48_OK && (s.buffers[10][0]&0x8c)!=image[0])status=LT48_DEVICE_MISMATCH;
        if(status==LT48_OK){s.raw_dirty=0;out->completed_bytes=1;}
    }else if(status==LT48_OK && lt48_compact_spi_has_idlock(r) && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)){
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,expected,nominal,out);
        if(status==LT48_OK)status=idlock_value(&s,&s.idlock_saved);
        if(status==LT48_OK){s.idlock_dirty=1;status=idlock_write(&s,image[0]);}
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,expected,NULL,nominal,out);
        if(status==LT48_OK){s.idlock_dirty=0;out->completed_bytes=1;}
    }else if(status==LT48_OK && op==LT48_OPERATION_SECURE){
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,out);
        if(status==LT48_OK) {
            for(size_t i=0;i<s.config_count;i++) {
                lt48_runtime_configuration_value *v=&s.config[i];
                if(r->configuration<=2 && v->category==0 && (v->field==6 || v->field==7))v->value=1;
                if(r->configuration>=3 && v->category==1 && v->field==1)v->value=3;
            }
            lt48_runtime_configuration_value wanted[16];size_t wanted_count=s.config_count;
            memcpy(wanted,s.config,wanted_count*sizeof(*wanted));
            s.mode=2;memset(s.buffers[1],0,32);s.buffers[1][0]=1;
            /* The source RESET enables status writes, writes saved fields,
             * and restores pins. Nonzero Address selects the saved fields. */
            status=run(&s,6);
            if(status==LT48_OK)status=run(&s,12);
            if(status==LT48_OK && !persistent_config_equal(&s,wanted,wanted_count))status=LT48_DEVICE_MISMATCH;
        }
    }else if(status==LT48_OK && op==LT48_OPERATION_VERIFY){
        if(margin==LT48_VERIFY_MARGIN_NOMINAL)status=cycle(&s,op,start,length,image,NULL,nominal,out);
        else{if(margin!=LT48_VERIFY_MARGIN_HIGH)status=cycle(&s,op,start,length,image,NULL,lo,out);if(status==LT48_OK && margin!=LT48_VERIFY_MARGIN_LOW)status=cycle(&s,op,start,length,image,NULL,hi,out);}
    }else if(status==LT48_OK)status=cycle(&s,op,start,length,image,op==LT48_OPERATION_READ?pending:NULL,nominal,out);
    out->operation_status=status;
    /* A fresh uncancellable transport owns cleanup even after a failed queue.
     * Continue independent rail-off attempts and preserve the first error. */
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        /* A failed/cancelled write may have stopped while unlocked. Retry
         * restoration with a fresh queue and cancellation disabled. */
        if(s.idlock_dirty || s.raw_dirty) {
            /* Original host Stop/ready handshake before recovery. This is
             * not proof that an ambiguous transfer cleared pending input;
             * independent status readback must still establish restoration. */
            unsigned char stop[512]={0},reply[512]={0};size_t done=0;
            out->cleanup_status=cleanup.write(context,2,stop,sizeof(stop),&done);
            if(out->cleanup_status==LT48_OK && done!=sizeof(stop))out->cleanup_status=LT48_PROTOCOL_ERROR;
            if(out->cleanup_status==LT48_OK)out->cleanup_status=cleanup.read(context,0x88,reply,sizeof(reply),&done);
            if(out->cleanup_status==LT48_OK && (done!=sizeof(reply) || !(reply[0]|reply[1])))out->cleanup_status=LT48_PROTOCOL_ERROR;
            if(out->cleanup_status==LT48_OK)out->cleanup_status=s.raw_dirty?raw_restore(&s):idlock_restore(&s);
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            lt48_status restart=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(out->cleanup_status==LT48_OK)out->cleanup_status=restart;
            if(restart!=LT48_OK){free(expected);free(pending);return out->cleanup_status;}
        }
        lt48_status step=run(&s,1);
        if(step==LT48_OK)step=lt48_runtime_transport_session_flush(s.wire);
        if(step!=LT48_OK && out->cleanup_status==LT48_OK)out->cleanup_status=step;
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
            if(step!=LT48_OK){out->cleanup_status=step;out->cleanup_completed=0;}
        } else if(lt48_compact_spi_has_idlock(r) || lt48_compact_spi_has_raw_status(r)) {
            /* Unverified protection restoration must not leave a success
             * indication. Preserve the first cleanup error if LED I/O fails. */
            lt48_runtime_transport_session *led=NULL;
            step=lt48_runtime_transport_session_create(&cleanup,context,&led);
            const unsigned char error[]={0x40,2};
            if(step==LT48_OK)step=lt48_runtime_transport_session_controller_write(led,error,sizeof(error));
            if(step==LT48_OK)step=lt48_runtime_transport_session_flush(led);
            lt48_runtime_transport_session_close(led);
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);
    if(status==LT48_OK && writing) {
        /* Fresh power/read verifies EEPROM persistence and every untouched byte. */
        lt48_compact_spi_result fresh={0};
        status=lt48_compact_spi_execute(r,api,context,LT48_OPERATION_READ,0,r->capacity,NULL,0,
            pending,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,0,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(uint32_t i=0;i<r->capacity;i++)if(pending[i]!=expected[i]) {
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
    if(status==LT48_OK)out->completed_bytes=lt48_compact_spi_has_idlock(r) &&
        (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)?1:length;
    free(expected);free(pending);return status;
}
