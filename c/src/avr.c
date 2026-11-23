#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../NewDeviceLib/sources/avr-atmega/profiles.h"
#include "../../NewDeviceLib/sources/avr-atmega/profiles21.h"
#include "../../NewDeviceLib/sources/avr-atmega/profiles9.h"
#include "../../NewDeviceLib/sources/avr-atmega/profiles22.h"
#include "../../NewDeviceLib/sources/avr-atmega/profiles25.h"
#include "../../NewDeviceLib/sources/avr-atmega/profiles31.h"
#include "../../NewDeviceLib/sources/avr-atmega/profiles35.h"
#include "../../NewDeviceLib/sources/avr-atmega/normalize9.h"
#include "../../NewDeviceLib/sources/avr-atmega/normalize31.h"
#include "../../NewDeviceLib/sources/avr-atmega/normalize35.h"
#include "../../NewDeviceLib/sources/avr-atmega/bod31.h"

/* Dataman 13.10 audited classic AT90S: contiguous Flash bytes followed by EEPROM. */
struct lt48_avr_resources {
    lt48_rez *rez; lt48_pin_map *pins; lt48_compiled_wave *wave[32];
    lt48_compile_variable vars[16]; size_t var_count;
    unsigned char *idle; size_t idle_size;
    const struct avr25_schema *schema;const struct avr35_contract *old_avr;
    unsigned capacity,second_fuse_field; int atmega,newtiny; int boot_choice[2]; unsigned nominal; int spien, aux_fuse, fuse_byte, fuse_high, fuse_extended, serial, fpga_spi, rc_fuse;
};
/* EESAVE is not at the same physical fuse position on all AVR families. */
static int atmega_legacy(const lt48_avr_resources*r){return (r->old_avr&&r->old_avr->count&&r->old_avr->map[0].object==0)||(r->atmega==8&&r->schema&&(r->schema->family==132||r->schema->family==133));}
static unsigned atmega_spien_mask(const lt48_avr_resources*r){return r->old_avr?r->old_avr->spien:0x2000;}
static unsigned atmega_eesave_mask(const lt48_avr_resources*r){
 if(r->old_avr)return r->old_avr->eesave;
 if(r->atmega==8&&r->schema){unsigned f=r->schema->family;if(f==218||f==219||f==58)return 0x4000;if(f==59)return 0x0800;}
 return 0x0800;
}
void lt48_avr_close(lt48_avr_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<32;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
unsigned lt48_avr_nominal_centivolts(const lt48_avr_resources*r){return r?r->nominal:0;}
int lt48_avr_is_atmega(const lt48_avr_resources*r){return r&&r->atmega;}
int lt48_avr_has_boot_locks(const lt48_avr_resources*r){return r&&r->atmega&&r->capacity!=4352&&!(r->schema&&r->schema->no_boot);}
uint32_t lt48_avr_capacity(const lt48_avr_resources *r){return r?r->capacity:0;}
lt48_status lt48_avr_open(const lt48_installation *x,size_t device,lt48_avr_resources **out) {
    if(!x||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,device,&spec);
    if(status!=LT48_OK)return status;
    static const unsigned tuples[][4]={
        {0x51,0x3e,0xc,1088},{0x52,0x3f,0xd,1088},{0x53,0x40,0xd,2176},
        {0x54,0x41,0xe,4352},{0x55,0x41,0xf,4352},
        {0x56,0x42,0xe,8704},{0x57,0x42,0xf,8704},
        {0x58,0x43,0x10,4352},{0x59,0x44,0x10,8704},
        {0x5f,0x4a,0x14,2176},{0x60,0x4b,0x14,2176},
        {0x62,0x4d,0x17,2176},{0x63,0x4e,0x17,4352},{0x64,0x4f,0x17,8704}
    };
    unsigned definition=spec.definition_resource;
    if(definition>=0xfff0 && definition<=0xfff5)definition=0x62+(definition-0xfff0)/2;
    int mega=avr35_binding_matches(&spec);if(!mega)mega=avr31_binding_matches(&spec);if(!mega)mega=avr25_binding_matches(&spec);if(!mega)mega=avr22_binding_matches(&spec);if(!mega)mega=atmega9_binding_matches(&spec)?3:atmega21_binding_matches(&spec)?2:atmega_binding_matches(&spec);
    int tiny=avr22_tiny_binding_matches(&spec);
    if((mega||tiny)&&strcmp(lt48_installation_source_profile(x),"Dataman 13.10"))return LT48_UNSUPPORTED;
    int paged=(definition>=0x62 && definition<=0x64)||tiny;
    int fpga_spi=definition==0x5f || definition==0x60;
    int admitted=mega||tiny,is1200=spec.definition_resource==0x52,serial=mega?3:paged?2:spec.definition_resource==0x51;
    for(size_t i=0;i<sizeof(tuples)/sizeof(tuples[0]);i++)
        if(definition==tuples[i][0] && spec.family_code==tuples[i][1] &&
           spec.pinmap==tuples[i][2] && spec.capacity_bytes==tuples[i][3])admitted=1;
    if(spec.catalogue.device_class!=2010 || strcmp(spec.definition_stem,"GAB____") ||
       !admitted || spec.element_bits!=16)return LT48_UNSUPPORTED;
    lt48_avr_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    r->atmega=mega;r->old_avr=mega==9?avr35_contract_for(spec.family_code):NULL;r->schema=mega==9?avr35_schema_for(spec.family_code):mega==8?avr31_schema_for(spec.family_code):mega==7?avr25_schema_for(spec.family_code):NULL;r->newtiny=tiny;r->boot_choice[0]=r->boot_choice[1]=-1;r->nominal=500;r->capacity=spec.capacity_bytes;r->second_fuse_field=paged?15:serial?4:(is1200||fpga_spi)?11:13;r->spien=-1;r->aux_fuse=-1;r->fuse_byte=-1;r->fuse_high=-1;r->fuse_extended=-1;r->serial=serial;r->fpga_spi=fpga_spi;r->rc_fuse=is1200||definition==0x60;
    char path[4096],name[32];size_t index,fi;
    int n=snprintf(path,sizeof(path),"%s/GAB____.rez",lt48_installation_path(x));
    if(n<0||(size_t)n>=sizeof(path)){lt48_avr_close(r);return LT48_INVALID_ARGUMENT;}
#define TRY(a) do{status=(a);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_owner_rez_open_internal(path,"72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9",&r->rez));
    lt48_pin_code pc;snprintf(name,sizeof(name),"P%x",spec.pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&pc));
    TRY(lt48_pin_map_parse(&pc,&r->pins));
    lt48_family_info f;snprintf(name,sizeof(name),"M%x",spec.family_code);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&f));
    if(f.variable_count!=(mega==9?r->old_avr->variables:(mega||paged)?7u:(serial||fpga_spi)?5u:is1200?4u:7u) || f.operation_count!=((mega==9&&spec.family_code==69)?9u:(mega==9&&spec.family_code==162)?14u:mega?10u:fpga_spi?11u:serial?10u:is1200?8u:9u)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=f.variable_count;
    for(size_t i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};}
    TRY(lt48_installation_apply_new_device_variables(x,device,r->vars,r->var_count));
    if(mega)for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==0)r->nominal=r->vars[i].value;
    if(mega>=3&&r->nominal!=500){status=LT48_SAFETY_REJECTED;goto fail;}
    if(mega==9&&spec.family_code==61){int corrected=0;for(size_t v=0;v<r->var_count;v++)if(r->vars[v].variable_id==40&&r->vars[v].value==6)corrected=1;if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}}
    if(mega==6){int corrected=0;for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==40&&r->vars[i].value==0x82)corrected=1;if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}}
    for(size_t i=0;i<f.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code w;lt48_compact_wave *c=NULL;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(op.operation_id>=32 || r->wave[op.operation_id]){status=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&w));
        if(fpga_spi && op.operation_id==8 && w.wave_type==4) {
            if(r->idle || w.data_size!=2780 || w.data[0]!=0xda || w.data[1]!=0x0a){status=LT48_FORMAT_ERROR;goto fail;}
            r->idle_size=2778;r->idle=malloc(r->idle_size);if(!r->idle){status=LT48_NO_MEMORY;goto fail;}
            memcpy(r->idle,w.data+2,r->idle_size);continue;
        }
        if(w.wave_type){status=LT48_UNSUPPORTED;goto fail;}
        const unsigned char*compact=w.data;size_t compact_size=w.data_size;unsigned char normalized[1024];
        if(((mega==3&&spec.pinmap==0x53)||(mega==4&&(spec.pinmap==0x50||spec.pinmap==0x51)))&&op.operation_id==2){TRY(atmega9_normalize_init(w.data,w.data_size,normalized,&compact_size));compact=normalized;}
        if(mega==9&&(spec.family_code==129||spec.family_code==130||spec.family_code==126||spec.family_code==127)&&op.operation_id==2){unsigned chip=0;for(size_t v=0;v<r->var_count;v++)if(r->vars[v].variable_id==37)chip=r->vars[v].value;TRY(avr35_normalize_init(spec.family_code,chip,w.data,w.data_size,normalized,&compact_size));compact=normalized;}
        if(mega==8&&spec.family_code==132&&op.operation_id==2){unsigned package=0;for(size_t v=0;v<r->var_count;v++)if(r->vars[v].variable_id==37)package=r->vars[v].value;if(package!=40){status=LT48_SAFETY_REJECTED;goto fail;}TRY(avr31_normalize_323_init(w.data,w.data_size,normalized,&compact_size));compact=normalized;}
        if(mega==8&&(spec.family_code==160||spec.family_code==161)&&(op.operation_id==7||op.operation_id==23)){TRY(avr31_correct_bod(w.data,w.data_size,op.operation_id,normalized,&compact_size));compact=normalized;}
        TRY(lt48_compact_decode(compact,compact_size,&c));status=lt48_compact_compile(c,r->pins,r->vars,r->var_count,1,0,&r->wave[op.operation_id]);lt48_compact_wave_close(c);if(status!=LT48_OK)goto fail;
        if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    if(fpga_spi && !r->idle){status=LT48_FORMAT_ERROR;goto fail;}
    if(!fpga_spi)TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_avr_close(r);return status;
#undef TRY
}

const char *lt48_avr_aux_fuse_name(const lt48_avr_resources *r) {
    return r ? (r->atmega?"EESAVE":r->serial?"RSTDISBL":r->rc_fuse?"RCEN":"FSTRT") : NULL;
}
int lt48_avr_has_spien(const lt48_avr_resources *r){return r && !r->fpga_spi && (!r->old_avr||r->old_avr->spien);}
unsigned lt48_avr_fuse_byte_count(const lt48_avr_resources *r){return r?(r->old_avr?r->old_avr->bytes:((r->atmega==4||(r->schema&&r->schema->fields==16))?2:r->serial>=2?3:r->serial?1:0)):0;}
lt48_status lt48_avr_set_boot_locks(lt48_avr_resources*r,int a,int b){
 if(!r||a < -1||a>3||b < -1||b>3)return LT48_INVALID_ARGUMENT;
 if(!r->atmega)return LT48_UNSUPPORTED;
 if((r->capacity==4352||(r->schema&&r->schema->no_boot))&&(a>0||b>0))return LT48_UNSUPPORTED;
 r->boot_choice[0]=a;r->boot_choice[1]=b;return LT48_OK;
}
lt48_status lt48_avr_set_fuse_bytes(lt48_avr_resources *r,int low,int high,int extended) {
    if(!r || low < -1 || low > 255 || high < -1 || high > 255 || extended < -1 || extended > 255)return LT48_INVALID_ARGUMENT;
    if(r->serial<2)return LT48_UNSUPPORTED;
    if(r->old_avr&&((r->old_avr->bytes==1&&high!=-1)||(r->old_avr->bytes<3&&extended!=-1)))return LT48_INVALID_ARGUMENT;
    if((r->atmega==8||r->atmega==9)&&((low>=0&&(((unsigned)low&(r->schema->reserved&255))!=(r->schema->reserved&255)))||(high>=0&&(((unsigned)high&((r->schema->reserved>>8)&255))!=((r->schema->reserved>>8)&255)))))return LT48_INVALID_ARGUMENT;

    if((r->atmega==4||(r->schema&&r->schema->fields==16)) && extended!=-1)return LT48_INVALID_ARGUMENT;
    if(r->schema&&extended>=0&&(((unsigned)extended&(r->schema->reserved>>16))!=(r->schema->reserved>>16)))return LT48_INVALID_ARGUMENT;
    if(r->atmega==6 && extended>=0 && (extended&0xf0)!=0xf0)return LT48_INVALID_ARGUMENT;
    if(r->atmega==5 && extended>=0 && (extended&0xf1)!=0xf1)return LT48_INVALID_ARGUMENT;
    if(r->atmega==2 && extended>=0 && (extended&0xf8)!=0xf8)return LT48_INVALID_ARGUMENT;
    if(!r->atmega && extended>=0 && (extended&0xfe)!=0xfe)return LT48_INVALID_ARGUMENT;
    r->fuse_byte=low;r->fuse_high=high;r->fuse_extended=extended;return LT48_OK;
}
int lt48_avr_has_fuse_byte(const lt48_avr_resources *r){return r && r->serial;}
lt48_status lt48_avr_set_fuse_byte(lt48_avr_resources *r,int value) {
    if(!r || value < -1 || value > 255)return LT48_INVALID_ARGUMENT;
    if(r->serial!=1 && !(r->old_avr&&r->old_avr->bytes==1) && value!=-1)return LT48_UNSUPPORTED;
    if(r->old_avr)return lt48_avr_set_fuse_bytes(r,value,-1,-1);
    r->fuse_byte=value;return LT48_OK;
}
lt48_status lt48_avr_set_fuses(lt48_avr_resources *r,int spien,int aux_fuse) {
    if(!r || spien < -1 || spien > 1 || aux_fuse < -1 || aux_fuse > 1)return LT48_INVALID_ARGUMENT;
    if(r->old_avr&&!r->old_avr->eesave&&aux_fuse!=-1)return LT48_UNSUPPORTED;
    if(r->fpga_spi && spien!=-1)return LT48_UNSUPPORTED;
    r->spien=spien;r->aux_fuse=aux_fuse;return LT48_OK;
}
lt48_status lt48_avr_validate(const lt48_avr_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r)return LT48_INVALID_ARGUMENT;
    if(op<LT48_OPERATION_READ || op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
    if((op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_VERIFY || op==LT48_OPERATION_COMPARE) &&
       (!image || size!=length))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_avr_validate_verify_options(const lt48_avr_resources *r,lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type) {
    (void)op;
    if(!r || type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
typedef struct avr_session {
    const lt48_avr_resources *r;const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;uint32_t volts[4];lt48_compile_variable vars[16];
    unsigned char buffers[32][32];unsigned mode;
    lt48_runtime_configuration_value config[64];size_t config_count;
} avr_session;
static void dword(unsigned char *p,unsigned v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
static unsigned cfg(avr_session *s,int object,unsigned category,unsigned field) {
    for(size_t i=0;i<s->config_count;i++)if(s->config[i].object_id==object && s->config[i].category==category && s->config[i].field==field)return s->config[i].value;
    return UINT32_MAX;
}
static void setcfg(avr_session *s,int object,unsigned category,unsigned field,unsigned value) {
    for(size_t i=0;i<s->config_count;i++)if(s->config[i].object_id==object && s->config[i].category==category && s->config[i].field==field){s->config[i].value=value;return;}
    if(s->config_count<64)s->config[s->config_count++]=(lt48_runtime_configuration_value){object,(uint16_t)category,(uint16_t)field,value};
}
/* ATtiny12 source object 0 bit bank -> actual active-low fuse byte. */
static const unsigned serial_fuse_fields[8]={0,1,2,3,4,12,15,14};
static int config_object(const lt48_avr_resources *r){return r->serial>=2?-2:0;}
static unsigned lock_field(const lt48_avr_resources *r){return r->serial==2?0:2;}
static unsigned spi_field(const lt48_avr_resources *r){return r->serial==2?13:12;}
static unsigned get_fuse_byte(avr_session *s) {
    if(s->r->old_avr&&s->r->old_avr->count){const struct avr35_contract*c=s->r->old_avr;unsigned v=s->r->schema->reserved;for(size_t i=0;i<c->count;i++){const struct avr31_fuse_field*m=&c->map[i];unsigned b=cfg(s,m->object,0,m->field);if(b>1)return UINT32_MAX;v|=(b^m->invert)<<m->bit;}return v;}
    if(s->r->atmega==8){size_t n;const struct avr31_fuse_field*m=avr31_fuse_map(s->r->schema->family,&n);if(m){unsigned v=s->r->schema->reserved;for(size_t i=0;i<n;i++){unsigned b=cfg(s,m[i].object,0,m[i].field);if(b>1)return UINT32_MAX;v|=(b^m[i].invert)<<m[i].bit;}return v;}}

    if(s->r->schema&&!s->r->schema->fields){unsigned v=0;for(unsigned i=0;i<3;i++){unsigned b=cfg(s,-2,2,s->r->schema->numeric_base+i);if(b>255)return UINT32_MAX;v|=b<<(8*i);}return v;}
    if(s->r->schema){const struct avr25_schema*p=s->r->schema;unsigned v=p->reserved;for(unsigned i=0;i<p->fields;i++){unsigned b=cfg(s,-2,0,i);if(b>1)return UINT32_MAX;v|=b<<(i<16?i:p->extended_start+i-16);}return v^p->invert;}
    if(s->r->atmega==6){unsigned v=0xf00000;for(unsigned i=0;i<20;i++){unsigned b=cfg(s,-2,0,i);if(b>1)return UINT32_MAX;v|=b<<i;}return v^0x8f900;}
    if(s->r->atmega==5){unsigned v=0xf10000;for(unsigned i=0;i<19;i++){unsigned b=cfg(s,-2,0,i);if(b>1)return UINT32_MAX;v|=b<<(i<16?i:i+1);}return v^0xf900;}
    if(s->r->atmega==2){unsigned v=0xf80000;for(unsigned i=0;i<19;i++){unsigned b=cfg(s,-2,0,i);if(b>1)return UINT32_MAX;v|=b<<i;}return v^0xf900;}
    if(s->r->atmega){unsigned v=s->r->atmega==4?0xff0000:0;for(unsigned i=0;i<(s->r->atmega==4?2u:3u);i++){unsigned b=cfg(s,-2,2,2+i);if(b>255)return UINT32_MAX;v|=b<<(8*i);}return v;}
    unsigned value=s->r->serial==2?0xfe0000:0;
    unsigned count=s->r->serial==2?17:8;
    for(unsigned i=0;i<count;i++) {
        unsigned field=s->r->serial==2?i:serial_fuse_fields[i];
        unsigned bit=cfg(s,config_object(s->r),0,field);if(bit>1)return UINT32_MAX;
        if(s->r->serial!=2 || ((0xb800u>>i)&1))bit^=1;
        value|=bit<<i;
    }
    return value;
}
static void set_fuse_byte(avr_session *s,unsigned value) {
    if(s->r->old_avr&&s->r->old_avr->count){const struct avr35_contract*c=s->r->old_avr;for(size_t i=0;i<c->count;i++){const struct avr31_fuse_field*m=&c->map[i];setcfg(s,m->object,0,m->field,((value>>m->bit)&1)^m->invert);}return;}
    if(s->r->atmega==8){size_t n;const struct avr31_fuse_field*m=avr31_fuse_map(s->r->schema->family,&n);if(m){for(size_t i=0;i<n;i++)setcfg(s,m[i].object,0,m[i].field,((value>>m[i].bit)&1)^m[i].invert);return;}}

    if(s->r->schema&&!s->r->schema->fields){for(unsigned i=0;i<3;i++)setcfg(s,-2,2,s->r->schema->numeric_base+i,(value>>(8*i))&255);return;}
    if(s->r->schema){const struct avr25_schema*p=s->r->schema;value^=p->invert;for(unsigned i=0;i<p->fields;i++)setcfg(s,-2,0,i,(value>>(i<16?i:p->extended_start+i-16))&1);return;}
    if(s->r->atmega==6){value^=0x8f900;for(unsigned i=0;i<20;i++)setcfg(s,-2,0,i,(value>>i)&1);return;}
    if(s->r->atmega==5){value^=0xf900;for(unsigned i=0;i<19;i++)setcfg(s,-2,0,i,(value>>(i<16?i:i+1))&1);return;}
    if(s->r->atmega==2){value^=0xf900;for(unsigned i=0;i<19;i++)setcfg(s,-2,0,i,(value>>i)&1);return;}
    if(s->r->atmega){for(unsigned i=0;i<3;i++)setcfg(s,-2,2,2+i,(value>>(8*i))&255);return;}
    unsigned count=s->r->serial==2?17:8;
    for(unsigned i=0;i<count;i++) {
        unsigned field=s->r->serial==2?i:serial_fuse_fields[i],bit=(value>>i)&1;
        if(s->r->serial!=2 || ((0xb800u>>i)&1))bit^=1;
        setcfg(s,config_object(s->r),0,field,bit);
    }
}
static unsigned result_fuses(const lt48_avr_result *r){return r->fuse_byte|(r->fuse_high<<8)|(r->fuse_extended<<16);}
static lt48_status run(avr_session *s,unsigned op) {
    if(op>=32 || !s->r->wave[op])return LT48_UNSUPPORTED;
    lt48_runtime_options opts={0};lt48_runtime_nested_wave nested[32];size_t count=0;
    for(unsigned i=0;i<32;i++)if(s->r->wave[i])nested[count++]=(lt48_runtime_nested_wave){(uint16_t)i,s->r->wave[i]};
    lt48_runtime_buffer_seed seeds[32];size_t ns=0;
    for(unsigned i=0;i<32;i++)if(i!=4)seeds[ns++]=(lt48_runtime_buffer_seed){(uint8_t)i,s->buffers[i],32};
    opts.host_avr_parallel=s->r->atmega||s->r->newtiny;
    opts.mode=s->mode;opts.use_explicit_buffer_ids=1;opts.address_buffer=0;opts.mode_buffer=4;opts.parameter_buffer_id=1;
    opts.initial_buffers=seeds;opts.initial_buffer_count=ns;opts.nested_waves=nested;opts.nested_wave_count=count;
    if(s->r->serial>=2){opts.use_explicit_direct_configuration_object=1;opts.direct_configuration_object=-2;}
    opts.configuration=s->config;opts.configuration_count=s->config_count;opts.root_timeout_seconds=10;
    memcpy(opts.initial_voltage_centivolts,s->volts,sizeof(s->volts));
    return lt48_owner_run_internal(s->wire,s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&opts,NULL,0,s->buffers,s->config,64,&s->config_count,s->volts);
}

static lt48_status start(avr_session *s,unsigned mode,unsigned vcc) {
    memset(s->buffers,0,sizeof(s->buffers));s->mode=mode;
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status st=run(s,0);
    for(unsigned i=0;st==LT48_OK && i<2;i++)st=lt48_runtime_transport_session_status(s->wire,1);
    if(st==LT48_OK)st=run(s,2);
    return st;
}
static lt48_status end(avr_session *s,lt48_status st) {
    lt48_status reset=run(s,3);if(st==LT48_OK)st=reset;
    reset=run(s,1);if(st==LT48_OK)st=reset;
    return st;
}
static lt48_status atmega_get_config(avr_session*,lt48_avr_result*);
static lt48_status get_config(avr_session *s,lt48_avr_result *out) {
    if(s->r->atmega)return atmega_get_config(s,out);
    /* SPI lock mode 3 hides signature bytes. Read locks first with CheckID
     * disabled, then perform the ordinary source signature check when readable.
     * Never enter source mode 11: it can erase while checking locks. */
    if(s->r->fpga_spi)setcfg(s,-1,0,1,0);
    lt48_status st=start(s,1,500);
    if(st==LT48_OK && s->r->fpga_spi && cfg(s,0,1,2)!=2) {
        st=end(s,st);setcfg(s,-1,0,1,1);
        if(st==LT48_OK)st=start(s,1,500);
    }
    if(st==LT48_OK) {
        int object=config_object(s->r);
        unsigned lock=cfg(s,object,1,lock_field(s->r)),spi=s->r->fpga_spi?UINT32_MAX:cfg(s,object,0,spi_field(s->r)),fast=cfg(s,object,0,s->r->second_fuse_field);
        if(lock>2 || (!s->r->fpga_spi && spi>1) || fast>1)st=LT48_RUNTIME_ERROR;
        else {
            out->configuration_valid=1;out->lock_mode=lock;out->spien=spi;out->aux_fuse=fast;
            if(s->r->serial) {
                unsigned fuse=get_fuse_byte(s),cal=cfg(s,object,2,0);
                if(fuse>0xffffff || cal>255)st=LT48_RUNTIME_ERROR;
                else {out->fuse_byte_valid=1;out->fuse_byte=fuse&255;out->fuse_high=(fuse>>8)&255;out->fuse_extended=(fuse>>16)&255;out->fuse_byte_count=lt48_avr_fuse_byte_count(s->r);out->calibration=cal;}
            }
        }
    }
    return end(s,st);
}
static lt48_status scan(avr_session *s,lt48_operation_kind op,const unsigned char *expected,unsigned char *dest,unsigned vcc,lt48_avr_result *out) {
    /* Reject before POWERON/DEVINIT; protected readout cannot prove contents. */
    if(cfg(s,config_object(s->r),1,lock_field(s->r))==2 && (s->r->fpga_spi || op!=LT48_OPERATION_READ))return LT48_DEVICE_MISMATCH;
    unsigned mode=s->r->fpga_spi?1:op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_VERIFY?4:1;
    lt48_status st=start(s,mode,vcc);
    /* A protected array can read as FF: never report it as blank or verified. */
    if(st==LT48_OK && cfg(s,config_object(s->r),1,lock_field(s->r))==2 && op!=LT48_OPERATION_READ)st=LT48_DEVICE_MISMATCH;
    for(unsigned a=0;st==LT48_OK && a<s->r->capacity/2;a++) {
        dword(s->buffers[0],a);
        for(unsigned b=0;b<2;b++)s->buffers[2][b]=op==LT48_OPERATION_BLANK_CHECK?255:expected?expected[2*a+b]:0;
        st=run(s,4);if(st!=LT48_OK)break;
        for(unsigned b=0;b<2;b++) {
            unsigned off=2*a+b;unsigned char actual=s->buffers[1][b];
            if(dest)dest[off]=actual;
            if(op!=LT48_OPERATION_READ && actual!=s->buffers[2][b]) {
                if(!out->mismatch_count)out->first_mismatch=off;
                out->mismatch_count++;
            }
        }
        out->completed_bytes=2*a+2;
        if(s->api->progress && !(a&63))s->api->progress(s->context,LT48_EXECUTION_READ_OPERATION,2*a+2,s->r->capacity,0,0,0);
        if(out->mismatch_count && op!=LT48_OPERATION_COMPARE){st=LT48_DEVICE_MISMATCH;break;}
    }
    st=end(s,st);return st==LT48_OK && out->mismatch_count?LT48_DEVICE_MISMATCH:st;
}
static lt48_status cleanup_wave(void *owner,unsigned op){return run(owner,op);}
static lt48_status finish(avr_session *s,lt48_status operation,lt48_avr_result *out) {
    const unsigned prefix[]={1};
    return lt48_owner_finish_internal(s->api,s->context,&s->wire,cleanup_wave,s,prefix,1,operation,&out->cleanup_status,&out->cleanup_completed);
}

#include "../../NewDeviceLib/sources/avr-atmega/owner.inc"

lt48_status lt48_avr_execute(const lt48_avr_resources *r,const lt48_protocol_transport *api,void *context,
    lt48_operation_kind op,uint32_t offset,size_t length,const unsigned char *image,size_t size,
    unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_avr_result *out) {
    if(!out || !api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_avr_validate(r,op,offset,length,image,size);if(st!=LT48_OK)return st;
    st=lt48_avr_validate_verify_options(r,op,margin,type);if(st!=LT48_OK)return st;
    if(op==LT48_OPERATION_READ && (!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    if(r->atmega)return atmega_execute(r,api,context,op,image,read,margin,type,out);
    avr_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;memcpy(s.vars,r->vars,sizeof(s.vars));
    unsigned char *pending=malloc(length);if(!pending)return LT48_NO_MEMORY;
    setcfg(&s,-1,0,1,1); /* Source CheckID enabled; no signature bypass. */
    setcfg(&s,-1,0,5,0); /* Protection is an explicit post-verify operation. */
    setcfg(&s,0,1,8,0); /* Do not silently insert calibration into user Flash. */
    setcfg(&s,0,2,4,0); /* Inactive source calibration destination. */
    if(r->serial==2){setcfg(&s,-2,1,1,1);setcfg(&s,-2,2,1,0);}
    st=lt48_runtime_transport_session_create(api,context,&s.wire);if(st!=LT48_OK)goto done;
    const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    st=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    if(st==LT48_OK)st=lt48_owner_load_fpga_internal(s.wire,r->idle,r->idle_size);
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
    if(st==LT48_OK)st=get_config(&s,out);
    unsigned verify_low=475,verify_high=525;
    if(st==LT48_OK&&r->newtiny){uint32_t levels[4]={0};unsigned count=0;st=lt48_pic_source_verify_voltages(r->nominal,NULL,0,type,0,levels,&count);verify_low=levels[0];verify_high=levels[1];}
    unsigned old_spi=out->spien,old_fast=out->aux_fuse,old_byte=result_fuses(out),old_cal=out->calibration;
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    if(st==LT48_OK && (op==LT48_OPERATION_ERASE || (op==LT48_OPERATION_PROGRAM && r->serial!=2))) {
        /* Normal Program follows source pre-erase. The 2313 erase uses
         * VCCL=4 V; the 1200 retains nominal VCC, with its own pulse/delay. */
        st=start(&s,7,500);if(st==LT48_OK)st=run(&s,6);st=end(&s,st);
        /* Original paged erase clears EESAVE to erase EEPROM. Restore it. */
        if(st==LT48_OK && r->serial==2) {
            set_fuse_byte(&s,old_byte);setcfg(&s,-2,1,0,0);
            st=start(&s,8,500);if(st==LT48_OK)st=run(&s,7);st=end(&s,st);
        }
        if(st==LT48_OK)st=get_config(&s,out);
        if(st==LT48_OK && (out->lock_mode || out->spien!=old_spi || out->aux_fuse!=old_fast || (r->serial && (result_fuses(out)!=old_byte || out->calibration!=old_cal))))st=LT48_DEVICE_MISMATCH;
        if(st==LT48_OK)st=scan(&s,LT48_OPERATION_BLANK_CHECK,NULL,NULL,500,out);
    }
    if(st==LT48_OK && r->fpga_spi && op==LT48_OPERATION_OVER_PROGRAM && out->lock_mode)st=LT48_DEVICE_MISMATCH;
    if(st==LT48_OK && writing) {
        setcfg(&s,config_object(r),1,lock_field(r),0);
        if(r->serial) {
            unsigned want=old_byte;
            if(r->fuse_byte>=0)want=(want&0xffff00)|(unsigned)r->fuse_byte;
            if(r->fuse_high>=0)want=(want&0xff00ff)|((unsigned)r->fuse_high<<8);
            if(r->fuse_extended>=0)want=(want&0xffff)|((unsigned)r->fuse_extended<<16);
            set_fuse_byte(&s,want);
        }
        if(r->spien>=0)setcfg(&s,config_object(r),0,spi_field(r),(unsigned)r->spien);
        if(r->aux_fuse>=0)setcfg(&s,config_object(r),0,r->second_fuse_field,(unsigned)r->aux_fuse);
        unsigned want_spi=r->fpga_spi?UINT32_MAX:cfg(&s,config_object(r),0,spi_field(r)),want_fast=cfg(&s,config_object(r),0,r->second_fuse_field),want_byte=r->serial?get_fuse_byte(&s):0;
        /* Both public writes use source mode 2 so its fuse writer runs;
         * Over-program omits only the normal pre-erase. */
        st=start(&s,2,500);
        for(unsigned a=0;st==LT48_OK && a<length/2;a++) {
            dword(s.buffers[0],a);memcpy(s.buffers[2],image+2*a,2);memcpy(s.buffers[1],image+2*a,2);
            st=run(&s,5);
            if(api->progress && !(a&63))api->progress(context,LT48_EXECUTION_READ_OPERATION,2*a+2,length,0,0,0);
        }
        st=end(&s,st);
        if(st==LT48_OK)st=scan(&s,LT48_OPERATION_VERIFY,image,NULL,verify_low,out);
        if(st==LT48_OK)st=scan(&s,LT48_OPERATION_VERIFY,image,NULL,verify_high,out);
        /* ATtiny12 source PROTECT writes the complete fuse byte, then locks.
         * Keep locks clear here; Secure remains an explicit later operation. */
        if(st==LT48_OK && r->serial) {
            set_fuse_byte(&s,want_byte);setcfg(&s,config_object(r),1,lock_field(r),0);
            st=start(&s,8,500);if(st==LT48_OK)st=run(&s,7);st=end(&s,st);
        }
        if(st==LT48_OK)st=get_config(&s,out);
        if(st==LT48_OK && (out->spien!=want_spi || out->aux_fuse!=want_fast || out->lock_mode || (r->serial && (result_fuses(out)!=want_byte || out->calibration!=old_cal))))st=LT48_DEVICE_MISMATCH;
    } else if(st==LT48_OK && op==LT48_OPERATION_SECURE) {
        setcfg(&s,config_object(r),1,lock_field(r),2);st=start(&s,8,500);if(st==LT48_OK)st=run(&s,7);st=end(&s,st);
        if(st==LT48_OK)st=get_config(&s,out);
        if(st==LT48_OK && (out->lock_mode!=2 || out->spien!=old_spi || out->aux_fuse!=old_fast || (r->serial && (result_fuses(out)!=old_byte || out->calibration!=old_cal))))st=LT48_DEVICE_MISMATCH;
    } else if(st==LT48_OK && op==LT48_OPERATION_VERIFY && margin!=LT48_VERIFY_MARGIN_NOMINAL) {
        if(margin!=LT48_VERIFY_MARGIN_HIGH)st=scan(&s,op,image,NULL,verify_low,out);
        if(st==LT48_OK && margin!=LT48_VERIFY_MARGIN_LOW)st=scan(&s,op,image,NULL,verify_high,out);
    } else if(st==LT48_OK && op!=LT48_OPERATION_ERASE)st=scan(&s,op,image,op==LT48_OPERATION_READ?pending:NULL,500,out);
    out->operation_status=st;st=finish(&s,st,out);
    if(st==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,pending,length);
    if(st==LT48_OK)out->completed_bytes=length;
done:free(pending);return st;
}
