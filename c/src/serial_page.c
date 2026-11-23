#include <labtool48/labtool48.h>
#include "internal.h"
#include "at89_lp21.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Original Dataman 13.10 I2C Page engine: 43 reads, per-page 44
 * writes after source PROG, then 0b completion. No programmer security applets. */
struct serial_flash_profile {
    unsigned closure[8], has_id, field_count, status_mask, protected_status;
    unsigned fields[5][3]; /* configuration category, index, Protect-All value */
};
static const struct serial_flash_profile flash_profiles[]={
#include "serial_flash_profiles.inc"
};
struct serial_flash_extra {
    unsigned closure[8],var_count;
    lt48_compile_variable vars[8];
    uint32_t opmask;
    unsigned secure_kind,field_count;
    unsigned fields[96][4]; /* category, field, policy: preserve/BP/OTP/clear, secure value */
    size_t image_size;
    const uint32_t *erase_sectors;size_t erase_count;
    const uint32_t *protect_sectors;size_t protect_count;
};
#include "serial_flash_extra.inc"
#include "../../NewDeviceLib/sources/sphe3/profiles.h"
#include "../../NewDeviceLib/sources/aec1440-spi/page.h"
#include "../../NewDeviceLib/sources/aec1440-spi/members.h"
#include "../../NewDeviceLib/sources/aec1440-spi/admission.h"
struct lt48_serial_page_resources {
    const struct serial_flash_extra *extra;
    const struct serial_flash_profile *flash;
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[16];
    lt48_wave_code applet[10];
    lt48_applet_address addresses[10];
    const unsigned char *fpga;
    lt48_compile_variable vars[8];
    size_t var_count;
    uint32_t capacity;
    unsigned page_size;
    unsigned applet_count;
    unsigned i2c_config, i2c_giantec;
    int lp21;
    int plain, spi, nor, backlog, read_a17_workaround, boolean_spi, rram;
    uint16_t man, dev;
    uint16_t nominal;

};
void lt48_serial_page_close(lt48_serial_page_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
uint32_t lt48_serial_page_capacity(const lt48_serial_page_resources *r){return r?r->capacity:0;}
unsigned lt48_serial_page_i2c_config_kind(const lt48_serial_page_resources *r){return r?r->i2c_config:0;}
int lt48_serial_page_is_lp21(const lt48_serial_page_resources *r){return r?r->lp21:0;}
int lt48_serial_page_is_nor(const lt48_serial_page_resources *r){return r?r->nor:0;}
int lt48_serial_page_secure_is_otp(const lt48_serial_page_resources *r){return r&&r->extra&&r->extra->secure_kind==2;}
lt48_status lt48_serial_page_open(const lt48_installation *x,size_t device,lt48_serial_page_resources **out) {
    lt48_device_spec spec;lt48_family_info family;lt48_pin_code pc;
    lt48_serial_page_resources *r=NULL;lt48_status status;char path[4096],name[24],hex[65];
    size_t index,fi;unsigned char digest[32];
    if(!out||!x)return LT48_INVALID_ARGUMENT;
    *out=NULL;status=lt48_installation_resolve_device(x,device,&spec);if(status!=LT48_OK)return status;
    /* Definition, family, capacity, page, VCC, tWC quantity, pin map, pins.
     * Source-bound 16 small and 17 larger I2C definitions. */
    static const unsigned closures[][8]={
#include "serial_backlog_page.inc"
        {0xfc,147,2048,32,310,100,32,14},
        {0xfd,148,2048,32,310,100,33,16},
        {239,136,262144,256,320,0,25,8},
        {240,137,262144,256,320,0,26,20},
        {241,138,262144,256,320,0,27,44},
        {237,134,131072,128,320,0,25,8},
        {238,135,131072,128,320,0,26,20},
        {242,138,524288,256,320,0,27,44},
        {233,132,32768,64,320,0,25,8},
        {234,133,32768,64,320,0,26,20},
        {235,134,65536,128,320,0,25,8},
        {236,135,65536,128,320,0,26,20},
        {0x2f,25,256,16,500,50,8,8},
        {0x30,26,256,16,500,50,8,8},
        {0x31,27,256,16,330,50,8,8},
        {0x32,28,1024,16,500,50,9,8},
        {0x0,0,128,8,500,100,0,8},
        {0x1,0,256,8,500,100,0,8},
        {0x2,1,512,16,500,100,0,8},
        {0x3,1,1024,16,500,100,0,8},
        {0x4,1,2048,16,500,100,0,8},
        {0x5,4,256,16,320,50,0,8},
        {0x6,4,512,16,320,50,0,8},
        {0x7,4,1024,16,320,50,0,8},
        {0x8,4,2048,16,320,50,0,8},
        {0x9,0,256,8,500,100,0,8},
        {0xb,2,128,8,320,50,0,8},
        {0xc,2,256,8,320,50,0,8},
        {0xd,4,512,16,320,50,0,8},
        {0xe,4,1024,16,320,50,0,8},
        {0xf,4,2048,16,320,50,0,8},
        {0x10,3,256,8,320,100,0,8},
        {0x18,8,4096,32,320,50,3,8},
        {0x19,8,8192,32,320,50,3,8},
        {0x1a,9,16384,64,330,50,3,8},
        {0x1b,10,4096,32,500,50,3,8},
        {0x1c,11,4096,32,500,50,2,6},
        {0x1d,10,8192,32,500,50,3,8},
        {0x1e,11,8192,32,500,50,2,6},
        {0x1f,12,4096,32,500,100,3,8},
        {0x20,12,8192,32,500,100,3,8},
        {0x21,13,16384,64,500,50,3,8},
        {0x22,13,32768,64,500,50,3,8},
        {0x23,14,32768,64,500,50,5,14},
        {0x24,15,65536,128,500,50,3,8},
        {0x25,16,65536,128,500,50,5,14},
        {0x27,18,16384,64,500,100,3,8},
        {0x28,18,32768,64,500,100,3,8},
        {0x29,19,65536,128,500,100,3,8},
        {0xc7,112,65536,256,310,700,21,8},
        {0xc8,112,131072,256,310,700,21,8},
        {0xc9,112,262144,256,310,700,21,8},
        {0xca,112,524288,256,310,700,21,8},
        {0xcb,112,1048576,256,310,700,21,8},
        {0xcc,112,2097152,256,310,700,21,8},
        {0xfe,149,1024,16,500,0,34,8},
        {0xff,150,1024,32,500,0,34,8},
        {0x100,150,2048,32,500,0,34,8},
        {0x101,150,4096,32,500,0,34,8},
        {0x102,150,8192,32,500,0,34,8},
        {0x103,151,16384,64,500,0,34,8},
        {0x104,151,32768,64,500,0,34,8},
        {0x105,152,65536,128,500,0,34,8},
    };
    const unsigned *closure=NULL;
    for(size_t i=0;i<sizeof(closures)/sizeof(closures[0]);i++)
        if(spec.definition_resource==closures[i][0] && spec.family_code==closures[i][1] &&
           spec.capacity_bytes==closures[i][2])closure=closures[i];
    const struct serial_flash_profile *flash=NULL;
    for(size_t i=0;i<sizeof(flash_profiles)/sizeof(flash_profiles[0]);i++) {
        const unsigned *c=flash_profiles[i].closure;
        if(spec.definition_resource==c[0] && spec.family_code==c[1] && spec.capacity_bytes==c[2]) {
            flash=&flash_profiles[i];closure=c;break;
        }
    }
    const struct serial_flash_extra *extra=NULL;
    for(size_t i=0;i<sizeof(serial_flash_extras)/sizeof(serial_flash_extras[0]);i++) {
        const unsigned *c=serial_flash_extras[i].closure;
        if(spec.definition_resource==c[0] && spec.family_code==c[1] && spec.capacity_bytes==c[2]){extra=&serial_flash_extras[i];closure=c;break;}
    }
    const struct serial_flash_extra*sphe=sphe3_binding(&spec);if(sphe){extra=sphe;closure=sphe->closure;}
    int aec1440=!strcmp(lt48_installation_source_profile(x),"AEC 14.40");
    if(aec1440){
        if(!LT48_AEC1440_ADMITTED||!a1440_binding(&spec.catalogue))return LT48_UNSUPPORTED;
        extra=&a1440_page;closure=extra->closure;flash=NULL;
        if(spec.definition_resource!=closure[0]||spec.family_code!=closure[1]||spec.capacity_bytes!=closure[2])return LT48_UNSUPPORTED;
    }
    if(spec.catalogue.device_class!=2043 || !closure || spec.pinmap!=closure[6] ||
       spec.element_bits!=8 || spec.pin_count!=closure[7])return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/GDBB___.rez",lt48_installation_path(x));
    if(n<0||(size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
#define TRY(a) do{status=(a);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,aec1440?"a1eaf80cdd90e46b7bb197320577e6a9d4a6b78f3b114764d838388d68973b70":"d2c4b6b43cff23dcfca67c92e4341a9e630322bb41850236ce83937228f5fab0")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->lp21=spec.definition_resource==0xfc || spec.definition_resource==0xfd;
    r->flash=flash;r->extra=extra;r->i2c_giantec=spec.definition_resource==0x31;
    r->i2c_config=(spec.definition_resource>=0xe9 && spec.definition_resource<=0xf2)?4:spec.definition_resource==0x2f?1:(spec.definition_resource==0x30 || spec.definition_resource==0x31)?2:spec.definition_resource==0x32?3:0;
    r->rram=spec.definition_resource>=0x5c && spec.definition_resource<=0x5e;
    int legacy_spi=spec.definition_resource==0x60 || (spec.definition_resource>=0x62 && spec.definition_resource<=0x65);
    r->backlog=extra || flash || r->rram || legacy_spi || (spec.definition_resource>=0x106 && spec.definition_resource<=0x115) || (spec.definition_resource>=0x11 && spec.definition_resource<=0x17) || spec.definition_resource==0x26 || (spec.definition_resource>=0x2a && spec.definition_resource<=0x2e);
    r->nominal=(uint16_t)closure[4];r->nor=extra || flash || (spec.definition_resource>=0xc7 && spec.definition_resource<=0xcc);
    r->spi=r->lp21 || r->rram || legacy_spi || spec.definition_resource>=0xfe || r->nor;r->man=spec.catalogue.manufacturer_id;r->dev=spec.catalogue.device_id;
    r->boolean_spi=spec.definition_resource>=0x109 && spec.definition_resource<=0x10c;
    r->read_a17_workaround=spec.definition_resource==0x2c;
    r->plain=(!r->spi && !r->i2c_config) || r->rram || (extra && !extra->secure_kind);r->applet_count=r->nor?(flash&&!flash->has_id?6:7):r->spi?5:4;
    if(extra || r->i2c_config || r->lp21)r->applet_count=0;
    r->capacity=spec.capacity_bytes;r->page_size=closure[3];
    snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    /* GDBB Kernel declares seven zero-initialized control groups. */
    TRY(lt48_pin_map_complete_groups_internal(r->pins,7));
    snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(!r->lp21 && !extra && !r->i2c_config && (family.variable_count!=(unsigned)(r->nor?3:r->spi?((legacy_spi||r->rram)?3:2):3) || family.operation_count!=(unsigned)(r->rram?12:r->nor?(flash&&!flash->has_id?15:17):r->spi?14:10))){status=LT48_FORMAT_ERROR;goto fail;}
    if(family.variable_count>8 || (extra && family.variable_count!=extra->var_count)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};

    }
    if(extra && memcmp(r->vars,extra->vars,r->var_count*sizeof(r->vars[0]))){status=LT48_SAFETY_REJECTED;goto fail;}
    if(!r->lp21 && !extra && !r->i2c_config && (r->vars[0].variable_id!=0 || r->vars[0].value!=closure[4] ||
       (r->nor?(r->vars[1].variable_id!=6 || r->vars[1].value!=2 || r->vars[1].channel!=closure[5] || r->vars[2].variable_id!=2 || r->vars[2].value!=closure[3]):r->spi?(r->vars[r->var_count-1].variable_id!=2 || r->vars[r->var_count-1].value!=closure[3] || ((legacy_spi||r->rram) && (r->vars[1].variable_id!=6 || r->vars[1].value!=2 || r->vars[1].channel!=closure[5]))):
       (r->vars[1].variable_id!=5 || r->vars[1].value!=1 || r->vars[1].channel!=closure[5] ||
        r->vars[2].variable_id!=2 || r->vars[2].value!=closure[3])))){status=LT48_SAFETY_REJECTED;goto fail;}
    if(r->lp21) {
        lt48_compile_variable expected[]={{0,0,1,310,spec.pinmap==32?2:1},{1,6,0,2,100},{2,2,0,32,0}};
        if(r->var_count!=3 || family.operation_count!=16 || memcmp(r->vars,expected,sizeof(expected))){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    if(r->i2c_config==4) {
        if(r->var_count!=3 || family.operation_count!=13 || r->vars[0].variable_id!=0 || r->vars[0].value!=320 || r->vars[1].variable_id!=2 || r->vars[1].value!=r->page_size || r->vars[2].variable_id!=4){status=LT48_SAFETY_REJECTED;goto fail;}
    } else if(r->i2c_config) {
        unsigned expected=r->i2c_config==2?4:3;
        if(r->var_count!=expected || family.operation_count!=(r->i2c_config==3?14u:12u)){status=LT48_SAFETY_REJECTED;goto fail;}
        for(unsigned i=0;i<expected;i++) {
            unsigned id=r->vars[i].variable_id;
            if((id==0 && (r->vars[i].value!=closure[4] || r->vars[i].channel!=(r->i2c_config==3?2u:1u))) ||
               (id==1 && (r->i2c_config!=2 || r->vars[i].value!=1000 || r->vars[i].channel!=3)) ||
               (id==5 && (r->vars[i].value!=1 || r->vars[i].channel!=50)) ||
               (id==2 && r->vars[i].value!=16) || (id!=0 && id!=1 && id!=2 && id!=5)){status=LT48_SAFETY_REJECTED;goto fail;}
        }
        /* AT34C02B 3417E specifies 1.7--3.6 V. D30's original 5 V is
         * outside that range: retain the protocol, use 3.3 V (3.63 V
         * ten-percent margin is clamped below). This is an explicit fix. */
        if(spec.definition_resource==0x30){r->nominal=330;for(unsigned i=0;i<expected;i++)if(r->vars[i].variable_id==0)r->vars[i].value=330;}
    }
    uint32_t opmask=0;
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(op.operation_id>=28){status=LT48_FORMAT_ERROR;goto fail;}
        opmask|=1u<<op.operation_id;
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(code.wave_type==0 && (r->lp21 || ((r->i2c_config && (op.operation_id==6 || op.operation_id==11)) || (extra && op.operation_id==14) || op.operation_id<4 || op.operation_id==8 || ((r->nor || r->rram) && op.operation_id==10) || (r->nor && op.operation_id==4) || (r->spi && (op.operation_id==6 || op.operation_id==11 || op.operation_id==15))))) {
            unsigned slot=op.operation_id==14?10:op.operation_id==10?8:op.operation_id==4?9:op.operation_id==8?4:op.operation_id==15?5:op.operation_id==6?6:op.operation_id==11?7:op.operation_id;
            lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(code.data,code.data_size,&w));
            status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,
                r->backlog?LT48_PIN_FALLBACK_RESIDENT_NOOP:LT48_PIN_FALLBACK_STRICT,&r->wave[slot]);
            lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            if(r->lp21 && op.operation_id==11) {
                for(unsigned row=0;row<3;row++)TRY(lt48_lp21_compile_row(&code,row,r->pins,r->vars,r->var_count,&r->wave[11+row]));
            }
            if(extra && extra->secure_kind==3 && op.operation_id==10) {
                /* KH25L8036D: original Erase begins with a standalone F3
                 * global-unlock transaction. Reuse only that complete prefix
                 * before programming, without executing the following C7. */
                unsigned char prefix[0x21];
                if(code.data_size<sizeof(prefix) || code.data[2]!=0x40 || code.data[5]!=0xf3 || code.data[0x21]!=0x40){status=LT48_FORMAT_ERROR;goto fail;}
                memcpy(prefix,code.data,sizeof(prefix));prefix[0]=sizeof(prefix)-2;prefix[1]=0;
                w=NULL;TRY(lt48_compact_decode(prefix,sizeof(prefix),&w));
                status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,LT48_PIN_FALLBACK_STRICT,&r->wave[11]);
                lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            }
            for(size_t j=0;j<lt48_compiled_wave_defaulted_pin_count(r->wave[slot]);j++){
                lt48_defaulted_logical_pin p;TRY(lt48_compiled_wave_get_defaulted_pin(r->wave[slot],j,&p));
                if(!r->backlog || (op.operation_id!=2 && !(extra && op.operation_id==4)) || (!extra && p.logical_pin!=2 && p.logical_pin!=6 && p.logical_pin!=15)){status=LT48_SAFETY_REJECTED;goto fail;}
            }
        } else if(code.wave_type==6 && (extra || r->i2c_config || r->lp21)) {
            if(r->applet_count>=10 || code.data_size==0 || code.data_size>0x800 || code.length1!=code.data_size+4 || code.length2!=code.data_size || code.load_address<0x5000 || code.load_address>=0xb000){status=LT48_FORMAT_ERROR;goto fail;}
            r->applet[r->applet_count]=code;r->addresses[r->applet_count++]=(lt48_applet_address){op.operation_id,code.load_address};
        } else if(code.wave_type==6) {
            unsigned slot=op.operation_id==17?0:op.operation_id==19?1:op.operation_id==20?2:op.operation_id==21?3:op.operation_id==24?4:op.operation_id==23?5:op.operation_id==16?6:7;
            if(r->rram && op.operation_id==23)slot=4;
            uint16_t addr[]={0x6000,0x7800,0x8000,0x7000,0x9800,0x9000,0x5000};
            if(r->rram)addr[4]=0x9000;
            if(slot>=r->applet_count || code.load_address!=addr[slot] || code.data_size==0 ||
               code.data_size>0x800 || code.length1!=code.data_size+4 || code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            r->applet[slot]=code;r->addresses[slot]=(lt48_applet_address){op.operation_id,code.load_address};
        } else if(code.wave_type==4 && op.operation_id==27 && code.data_size==2780 &&
                  code.data[0]==0xda && code.data[1]==0x0a)r->fpga=code.data+2;
        else {status=LT48_UNSUPPORTED;goto fail;}
    }
    if(r->lp21) {
        const uint32_t expected=(1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<6)|(1u<<8)|(1u<<10)|(1u<<11)|(1u<<16)|(1u<<17)|(1u<<19)|(1u<<20)|(1u<<21)|(1u<<23)|(1u<<27);
        if(opmask!=expected || r->applet_count!=6){status=LT48_FORMAT_ERROR;goto fail;}
        for(unsigned i=0;i<14;i++)if(i!=5 && i!=10 && !r->wave[i]){status=LT48_FORMAT_ERROR;goto fail;}
    }
    if(extra && (opmask!=extra->opmask || !r->wave[0] || !r->wave[1] || !r->wave[2] || !r->wave[3] || !r->wave[4] || !r->wave[8])){status=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;!r->lp21 && !extra && !r->i2c_config && i<(r->nor?10u:r->spi&&!r->rram?8u:5u);i++)if(!(r->nor && (i==5 || (i==9 && flash && !flash->has_id))) && !r->wave[i]){status=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<r->applet_count;i++)if(!r->applet[i].data){status=LT48_FORMAT_ERROR;goto fail;}
    if(r->rram && !r->wave[8]){status=LT48_FORMAT_ERROR;goto fail;}
    if(r->i2c_config) {
        const uint32_t expected=(1u<<0)|(1u<<1)|(1u<<2)|(1u<<3)|(1u<<8)|(1u<<11)|(1u<<17)|(1u<<19)|(1u<<20)|(1u<<21)|(1u<<24)|(1u<<27)| (r->i2c_config==3?((1u<<6)|(1u<<18)):r->i2c_config==4?(1u<<6):0);
        if(opmask!=expected || !r->wave[7] || (r->i2c_config>=3 && !r->wave[6])){status=LT48_FORMAT_ERROR;goto fail;}
        if(r->i2c_config!=4) {
            /* Address-only ACK query; same START/byte/ACK/STOP instructions
             * as the audited compact half-protection owner, mapped to P8 SDA1,
             * SCL0. No word address/data follows, hence no protect write. */
            {
                static const unsigned char raw[]={6,0,0x11,1,1,0x11,0,1};
                lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(raw,sizeof(raw),&w));
                status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[12]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            }
            if(r->i2c_config==2) {
                static const unsigned char raw[]={5,0,3,13,1,0,1};
                lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(raw,sizeof(raw),&w));
                status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[14]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
                static const unsigned char off[]={10,0,4,13,1,0,0x11,13,0,0x11,14,0};
                w=NULL;TRY(lt48_compact_decode(off,sizeof(off),&w));status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[13]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            }
            {
                static const unsigned char body[]={
                    0x11,1,0,0x11,0,1,0x11,1,1,0x11,0,0,0x31,5,
                    0x11,0,0,0x11,1,1,0x11,0,1,0x11,1,0,0x11,0,0,
                    0x1c,1,0,28,8,1,0x1e,1,0,8,1,1,0x2b,0,1,
                    0x11,1,0,0x11,0,1,0x11,1,1,0x11,0,0};
                unsigned char raw[sizeof(body)+2];raw[0]=sizeof(body);raw[1]=0;memcpy(raw+2,body,sizeof(body));
                lt48_compact_wave *w=NULL;TRY(lt48_compact_decode(raw,sizeof(raw),&w));
                status=lt48_compact_compile(w,r->pins,r->vars,r->var_count,1,0,&r->wave[15]);lt48_compact_wave_close(w);if(status!=LT48_OK)goto fail;
            }
        }
    }
    if(!r->fpga){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_serial_page_close(r);return status;
#undef TRY
}
lt48_status lt48_serial_page_validate(const lt48_serial_page_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r||!length||start>=r->capacity||length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if(r->lp21) {
        if(op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION || op==LT48_OPERATION_ERASE || op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)
            if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
        if(op==LT48_OPERATION_CONFIGURATION){
            if(!image && !size)return LT48_OK;
            if(!image || size!=79 || (image[1]&0xc0))return LT48_INVALID_ARGUMENT;
            for(unsigned i=0;i<11;i++)if(((image[0]|image[1]*256u)&(1u<<i)) && image[i+2]>1)return LT48_INVALID_ARGUMENT;
            if((image[1]&8) && image[13]>2)return LT48_INVALID_ARGUMENT;
            return LT48_OK;
        }
        if(op==LT48_OPERATION_SECURE)return (!image && !size) || (image && size==1 && image[0]>=1 && image[0]<=2)?LT48_OK:LT48_INVALID_ARGUMENT;
    }
    if(r->i2c_config && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)) {
        if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
        if(r->i2c_config==4 && op==LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
        if(r->i2c_config==1)return op==LT48_OPERATION_SECURE?LT48_OK:LT48_UNSUPPORTED;
        if(!image || size!=1 || image[0]>(r->i2c_config==4?1:r->i2c_config==2?3:15))return LT48_INVALID_ARGUMENT;
        if(r->i2c_config==2 && op==LT48_OPERATION_SECURE && !image[0])return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_SECURE && r->plain)return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_SECURE || ((r->nor||r->rram) && op==LT48_OPERATION_ERASE)) && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE&&(!image||size!=length))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_serial_page_validate_verify_options(const lt48_serial_page_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned margin_type) {
    if(!r||margin_type>1)return LT48_INVALID_ARGUMENT;
    if(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&
       margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    (void)op;
    return LT48_OK;
}
static uint16_t margin_voltage(const lt48_serial_page_resources *r,unsigned type,int high) {
    unsigned delta=r->nominal/(type?10:20);
    unsigned value=high?r->nominal+delta:r->nominal-delta;
    if(r->i2c_config==2 && value>360)value=360;
    return (uint16_t)value;
}
typedef struct page_session {
    const lt48_serial_page_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[32][32];
    lt48_runtime_configuration_value config[96];size_t config_count;
    unsigned char *config_image;uint32_t volts[4];
    lt48_compile_variable vars[8];uint32_t mode;unsigned protect;
} page_session;
static lt48_status run(page_session *s,unsigned op) {
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[31];size_t count=0;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    for(uint8_t i=0;i<32;i++)if(i!=7)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_nested_wave engineer={15,s->r->wave[5]};
    lt48_runtime_nested_wave erase={10,s->r->wave[8]};
    if(s->r->lp21){options.nested_waves=&erase;options.nested_wave_count=1;}
    if(s->r->spi && !s->r->nor && !s->r->rram && !s->r->lp21){options.nested_waves=&engineer;options.nested_wave_count=1;}
    lt48_runtime_configuration_value configuration[5]={{-1,1,0,s->protect?((s->r->nor||s->r->boolean_spi)?1:3):0},{-1,1,1,(s->r->nor||s->r->boolean_spi)&&s->protect?1:0},{-1,1,2,s->r->nor&&s->protect?1:0},{-1,1,3,0}};
    if(s->r->nor||s->r->boolean_spi)for(unsigned i=0;i<4;i++)configuration[i].category=0;
    if(s->r->spi && !s->r->rram){options.configuration=configuration;options.configuration_count=s->r->nor?4:s->r->boolean_spi?3:2;}
    if(s->r->flash) {
        const struct serial_flash_profile *p=s->r->flash;
        for(unsigned i=0;i<p->field_count;i++)configuration[i]=(lt48_runtime_configuration_value){-1,p->fields[i][0],p->fields[i][1],s->protect?p->fields[i][2]:0};
        options.configuration_count=p->field_count;
    }
    if(s->r->i2c_config || s->r->lp21){options.configuration=s->config;options.configuration_count=s->config_count;}
    if(s->r->extra) {
        options.configuration=s->config;options.configuration_count=s->config_count;
        if((op==6 || op==7) && s->config_image) {
            options.device_image=s->config_image;options.device_image_size=s->r->extra->image_size;options.device_element_bytes=1;
        }
    }
    if(s->r->lp21 && s->config_image) {
        options.device_image=s->config_image;options.device_image_size=2112;options.device_element_bytes=1;
    }
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=7;
    options.address_buffer=0;options.parameter_buffer_id=2;options.initial_buffers=seeds;options.initial_buffer_count=count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));options.root_timeout_seconds=10;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,10000,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->applet_count,&result);
    if(status==LT48_OK) {
        for(uint8_t i=0;i<32;i++) {
            const unsigned char *data;size_t size;status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK||size!=32){status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        if(status==LT48_OK && (s->r->extra || s->r->i2c_config || s->r->lp21)) {
            const lt48_runtime_configuration_value *values;size_t n;
            status=lt48_runtime_get_configuration(result,&values,&n);
            if(status==LT48_OK && n>96)status=LT48_RUNTIME_ERROR;
            if(status==LT48_OK){memcpy(s->config,values,n*sizeof(*values));s->config_count=n;}
            if(status==LT48_OK && options.device_image) {
                const unsigned char *bytes;size_t size;
                status=lt48_runtime_get_image(result,&bytes,&size);
                if(status==LT48_OK && size!=(s->r->lp21?2112:s->r->extra->image_size))status=LT48_RUNTIME_ERROR;
                if(status==LT48_OK)memcpy(s->config_image,bytes,size);
            }
        }
        for(uint8_t i=0;status==LT48_OK&&i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(event.kind==LT48_RUNTIME_RESULT_LATCH ||
               (event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0))status=LT48_RUNTIME_ERROR;
        }
    }
    if(status!=LT48_OK && getenv("LT48_SERIAL_TRACE"))fprintf(stderr,"serial wave%u mode%u status%d\n",op,s->mode,status);
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
static lt48_status flag(page_session *s,unsigned opcode,unsigned *value) {
    unsigned char p[2]={(unsigned char)opcode,1},reply[512]={0};
    lt48_status status=cmd(s,p,opcode==0x0b?1:2,reply);
    if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)*value=reply[2]&1;
    return status;
}
static lt48_status block(page_session *s,unsigned opcode,unsigned char *data,size_t n,unsigned char *reply) {
    unsigned char p[6]={(unsigned char)opcode,(unsigned char)(n/512),(unsigned char)(n/512>>8),
        (unsigned char)(n%512),(unsigned char)(n%512>>8),opcode==0x48?1:0};
    lt48_status status=cmd(s,p,6,NULL);
    if(status==LT48_OK)status=transfer(s,opcode==0x43,opcode==0x43?0x88:4,data,n);
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
    const unsigned char fpga[]={0x38,0xaa,0x55},selector[]={0x0f,15},busy[]={0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_wave_watchdog(s->wire,5000,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,fpga,sizeof(fpga));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,s->r->fpga,2778);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,selector,sizeof(selector));
    if(status==LT48_OK)status=run(s,0);
    if(status==LT48_OK)status=lt48_runtime_transport_session_status(s->wire,1);
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s->wire,busy,sizeof(busy));
    return status;
}
/* Read, Blank, Compare and margin Verify acquire the original serial read stream.
 * Comparing actual bytes also avoids trusting the controller's status bit. */
static lt48_status cycle(page_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_serial_page_result *out) {
    /* Original D2c/W14 random-address handler masks A17. Its sequential
     * stream spans the full array; start at zero and discard the prefix for
     * upper-half ranges. Preserve the original applet and FPGA unchanged. */
    size_t prefix=mode!=2 && s->r->read_a17_workaround && start>=131072?start:0;
    uint32_t wire_start=prefix?0:start;
    if(s->r->lp21 && mode!=2){prefix=start%32;wire_start=start-(uint32_t)prefix;}
    /* SPHE6400A-SF2 source INIT divides the logical byte address by two.
     * Start at the containing pair and discard its first stream byte for an
     * odd range; keep the original workers and paired-lane order unchanged. */
    if(mode!=2 && s->r->extra==&sphe3_profiles[1]){prefix=start&1u;wire_start=start-(uint32_t)prefix;}
    for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(wire_start>>(8*j));
    s->mode=mode==2?2:1;
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    lt48_status status=s->r->i2c_config?run(s,0):LT48_OK;
    if(status==LT48_OK)status=run(s,2);
    out->completed_bytes=0;
    while(status==LT48_OK && prefix) {
        unsigned char discard[256];size_t n=prefix>sizeof(discard)?sizeof(discard):prefix;
        status=block(s,0x43,discard,n,NULL);prefix-=n;
    }
    for(size_t offset=0;status==LT48_OK&&offset<length;) {
        size_t n=length-offset;if(n>s->r->page_size)n=s->r->page_size;unsigned char bytes[512];
        if(mode==2) {
            if(n!=s->r->page_size || ((start+offset)&(s->r->page_size-1)))return LT48_SAFETY_REJECTED;
            for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)((start+offset)>>(8*j));
            status=run(s,4);
            if(status==LT48_OK)status=block(s,0x44,(unsigned char*)image+offset,n,NULL);
            unsigned passed=0;
            if(status==LT48_OK)status=flag(s,0x0b,&passed);
            if(status==LT48_OK && !passed)status=LT48_DEVICE_MISMATCH;
        } else {
            status=block(s,0x43,bytes,n,NULL);
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
    if(status==LT48_OK)status=run(s,3);
    if(status==LT48_OK&&out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    return status;
}
/* Original XP/Page host traversal uses primary geometry for Erase/Unlock,
 * secondary geometry for protection. Each wave owns its original pin and SPI
 * sequence; host loops restore the selected byte address before each call. */
static void extra_address(page_session *s,uint32_t address) {
    for(unsigned j=0;j<4;j++)s->buffers[0][j]=(unsigned char)(address>>(j*8));
}
static int extra_image_lock_byte(const page_session *s,size_t index) {
    size_t base=s->r->extra->image_size-0x300;
    return s->r->extra->secure_kind==2 && s->r->extra->image_size>=0x200300 &&
        (index==base+0x100 || index==base+0x112 || index==base+0x113 || index==base+0x214 || index==base+0x215);
}
static lt48_status extra_protection(page_session *s,int secure) {
    const struct serial_flash_extra *p=s->r->extra;
    if(!p->secure_kind || (!secure && p->secure_kind==2))return LT48_OK;
    if(p->secure_kind==3) {
        s->mode=13;extra_address(s,0);lt48_status st=run(s,2);
        if(!secure && st==LT48_OK)st=run(s,11);
        if(!secure && st==LT48_OK && s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
        for(size_t i=0;secure && st==LT48_OK && i<p->protect_count;i++) {
            extra_address(s,p->protect_sectors[i]);s->buffers[3][0]=1;
            st=run(s,7);
            if(st==LT48_OK && !s->buffers[2][0])st=LT48_DEVICE_MISMATCH;
        }
        if(st==LT48_OK)st=run(s,3);
        s->mode=0;if(st==LT48_OK)st=run(s,2);
        for(size_t i=0;st==LT48_OK && i<p->protect_count;i++) {
            extra_address(s,p->protect_sectors[i]);st=run(s,6);
            if(st==LT48_OK && s->buffers[2][0]!=(secure?1:0))st=LT48_DEVICE_MISMATCH;
        }
        if(st==LT48_OK)st=run(s,3);
        return st;
    }
    s->mode=0;extra_address(s,0);lt48_status st=run(s,2);
    if(st==LT48_OK)st=run(s,6);
    lt48_runtime_configuration_value wanted[96];size_t count=s->config_count;
    if(st!=LT48_OK)return st;
    for(unsigned i=0;i<p->field_count;i++) {
        const unsigned *f=p->fields[i];int found=0;
        for(size_t j=0;j<count;j++)if(s->config[j].category==f[0] && s->config[j].field==f[1]) {
            found=1;
            if(f[2]==1 || f[2]==2)s->config[j].value=secure?f[3]:0;
            else if(f[2]==3)s->config[j].value=0;
        }
        if(!found)return LT48_RUNTIME_ERROR;
    }
    memcpy(wanted,s->config,count*sizeof(*wanted));
    unsigned char *before=NULL;
    if(p->image_size){before=malloc(p->image_size);if(!before)return LT48_NO_MEMORY;memcpy(before,s->config_image,p->image_size);}
    s->mode=13;st=run(s,7);
    if(st==LT48_OK)st=run(s,6);
    for(size_t i=0;st==LT48_OK && i<count;i++) {
        int match=0;
        for(size_t j=0;j<s->config_count;j++)if(wanted[i].object_id==s->config[j].object_id && wanted[i].category==s->config[j].category && wanted[i].field==s->config[j].field && wanted[i].value==s->config[j].value)match=1;
        if(!match)st=LT48_DEVICE_MISMATCH;
    }
    if(st==LT48_OK && before)for(size_t i=0;i<p->image_size;i++)if(!extra_image_lock_byte(s,i) && before[i]!=s->config_image[i]){st=LT48_DEVICE_MISMATCH;break;}
    free(before);
    if(st==LT48_OK)st=run(s,3);
    return st;
}
static lt48_status extra_before_write(page_session *s) {
    const struct serial_flash_extra *p=s->r->extra;
    lt48_status st=extra_protection(s,0);
    if(st==LT48_OK && s->r->wave[10]) {
        s->mode=15;extra_address(s,0);st=run(s,2);
        for(size_t i=0;st==LT48_OK && i<p->erase_count;i++) {
            extra_address(s,p->erase_sectors[i]);st=run(s,10);
            if(st==LT48_OK && !s->buffers[2][0])st=LT48_DEVICE_MISMATCH;
        }
        if(st==LT48_OK)st=run(s,3);
    }
    return st;
}
static lt48_status extra_erase(page_session *s) {
    const struct serial_flash_extra *p=s->r->extra;
    s->mode=7;extra_address(s,0);lt48_status st=run(s,2);
    for(size_t i=0;st==LT48_OK && i<p->erase_count;i++) {
        extra_address(s,p->erase_sectors[i]);st=run(s,8);
        if(st==LT48_OK && s->buffers[2][0]!=1)st=LT48_DEVICE_MISMATCH;
    }
    if(st==LT48_OK)st=run(s,3);
    return st;
}

/* Observe configuration without modifying memory. Presence probes bracket
 * SPD status queries so an absent device cannot look permanently protected. */
static lt48_status i2c_settings_begin(page_session *s) {
    s->mode=8;lt48_status st=run(s,0);
    if(st==LT48_OK)st=run(s,2);
    if(st==LT48_OK){const unsigned char stop[]={0,0};unsigned char reply[512];st=cmd(s,stop,2,reply);if(st==LT48_OK && !(reply[0]|reply[1]))st=LT48_PROTOCOL_ERROR;}
    s->buffers[13][0]=5; /* source tWC=50 quantities / 10 -> 5 ms */
    return st;
}
static lt48_status i2c_settings_end(page_session *s,lt48_status st) {
    if(st==LT48_OK)st=run(s,3);
    if(st==LT48_OK)st=run(s,1);
    return st; /* outer unconditional cleanup handles any failure */
}
static lt48_status i2c_settings_read(page_session *s,unsigned *value) {
    *value=0;lt48_status ready=i2c_settings_begin(s);if(ready!=LT48_OK)return ready;
    if(s->r->i2c_config==4) {
        lt48_status st=run(s,6);
        if(st==LT48_OK && (s->config_count!=1 || s->config[0].object_id!=-1 || s->config[0].category!=1 || s->config[0].field!=0 || s->config[0].value>1))st=LT48_DEVICE_MISMATCH;
        if(st==LT48_OK)*value=s->config[0].value;
        return i2c_settings_end(s,st);
    }
    if(s->r->i2c_config==3) {
        s->buffers[28][0]=0xa0;s->buffers[8][0]=0;lt48_status st=run(s,15);
        if(st==LT48_OK && (s->buffers[8][0]&1))st=LT48_DEVICE_MISMATCH;
        if(st==LT48_OK)st=run(s,12); /* source READCFG starts with SDA falling, SCL must be high */
        if(st==LT48_OK)st=run(s,6);
        unsigned seen=0;
        for(size_t i=0;st==LT48_OK && i<s->config_count;i++) {
            const lt48_runtime_configuration_value *v=&s->config[i];
            if(v->category!=0 || v->field>3 || v->value>1 || (seen&(1u<<v->field)))return LT48_DEVICE_MISMATCH;
            seen|=1u<<v->field;*value|=v->value<<v->field;
        }
        if(st==LT48_OK){s->buffers[28][0]=0xa0;s->buffers[8][0]=0;st=run(s,15);if(st==LT48_OK && (s->buffers[8][0]&1))st=LT48_DEVICE_MISMATCH;}
        return i2c_settings_end(s,st!=LT48_OK?st:seen==15?LT48_OK:LT48_DEVICE_MISMATCH);
    }
    unsigned addresses[]={0xa0,s->r->i2c_config==1?0x60:0x61,0x63,0xa0};
    if(s->r->i2c_config==1)addresses[2]=0xa0;
    for(unsigned i=0;i<4;i++) {
        lt48_status st=LT48_OK;
        if(i==2 && s->r->i2c_config==2)st=run(s,14);
        s->buffers[28][0]=addresses[i];s->buffers[8][0]=0;
        if(st==LT48_OK)st=run(s,15);
        if(i==2 && s->r->i2c_config==2 && st==LT48_OK)st=run(s,13);
        if(st!=LT48_OK)return st;
        unsigned nack=s->buffers[8][0]&1;
        if(addresses[i]==0xa0){if(nack)return LT48_DEVICE_MISMATCH;}
        else *value|=nack<<(i-1);
    }
    return i2c_settings_end(s,LT48_OK);
}
static lt48_status i2c_settings_write(page_session *s,unsigned wanted) {
    lt48_status ready=i2c_settings_begin(s);if(ready!=LT48_OK)return ready;
    s->config_count=s->r->i2c_config==3?4:1;
    for(unsigned i=0;i<s->config_count;i++)s->config[i]=(lt48_runtime_configuration_value){-1,(s->r->i2c_config==2 || s->r->i2c_config==4)?1u:0u,i,s->r->i2c_config==3?((wanted>>i)&1):wanted};
    lt48_status st=run(s,7); /* exact original PROTECT wave and applet */
    return i2c_settings_end(s,st);
}
#include "serial_at89_lp21.inc"

static lt48_status serial_page_execute_impl(const lt48_serial_page_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_serial_page_result *out,unsigned *observed_settings) {
    if(r && r->lp21)return lp21_execute(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out);
    if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_serial_page_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_serial_page_validate_verify_options(r,op,margin,margin_type);
    if(status!=LT48_OK)return status;

    int writing=op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_ERASE||op==LT48_OPERATION_SECURE||op==LT48_OPERATION_CONFIGURATION;
    unsigned char *pending=malloc(r->capacity),*desired=malloc(r->capacity);
    if(!pending||!desired){free(pending);free(desired);return LT48_NO_MEMORY;}
    /* Preserve the complete array, including both partial page edges. */
    if(writing) {
        lt48_serial_page_result before;
        status=serial_page_execute_impl(r,api,context,LT48_OPERATION_READ,0,r->capacity,NULL,0,desired,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&before,NULL);
        if(status!=LT48_OK){*out=before;free(pending);free(desired);return status;}
        if(op==LT48_OPERATION_ERASE)memset(desired+start,255,length);
        else if(op!=LT48_OPERATION_SECURE && op!=LT48_OPERATION_CONFIGURATION) {
            if(r->nor)for(size_t i=0;i<length;i++)if((op==LT48_OPERATION_PROGRAM && desired[start+i]!=255) || (desired[start+i]&image[i])!=image[i]) {
                out->operation_status=LT48_DEVICE_MISMATCH;out->first_mismatch=start+(uint32_t)i;out->mismatch_count=1;
                free(pending);free(desired);return LT48_DEVICE_MISMATCH;
            }
            memcpy(desired+start,image,length);
        }
    }
    page_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;s.protect=op==LT48_OPERATION_SECURE;memcpy(s.vars,r->vars,sizeof(s.vars));
    if(r->i2c_config){s.config_count=r->i2c_config==3?4:1;for(unsigned i=0;i<s.config_count;i++)s.config[i]=(lt48_runtime_configuration_value){-1,r->i2c_config==4?1u:0u,i,0};}
    if(r->extra) {
        for(unsigned i=0;i<r->extra->field_count;i++)s.config[s.config_count++]=(lt48_runtime_configuration_value){-1,r->extra->fields[i][0],r->extra->fields[i][1],0};
        if(r->extra->image_size){s.config_image=malloc(r->extra->image_size);if(!s.config_image){free(pending);free(desired);return LT48_NO_MEMORY;}memset(s.config_image,255,r->extra->image_size);}
    }
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(s.config_image);free(pending);free(desired);return status;}
    status=setup(&s);
    if(status==LT48_OK && r->nor && r->wave[9]) {
        for(unsigned attempt=0;attempt<64;attempt++) {
            status=run(&s,9);if(status!=LT48_OK)break;
            unsigned man=s.buffers[6][0],dev=s.buffers[4][0];
            if(man==r->man && dev==r->dev)break;
            lt48_m37_id_choice choice=decide?decide(decision_context,r->man,r->dev,man,dev):LT48_M37_ID_ABORT;
            if(choice==LT48_M37_ID_IGNORE)break;
            if(choice!=LT48_M37_ID_RETRY || attempt==63){status=LT48_DEVICE_MISMATCH;break;}
        }
    }
    unsigned saved_i2c=0;
    if(status==LT48_OK && r->i2c_config) {
        status=i2c_settings_read(&s,&saved_i2c);
        if(status==LT48_OK && writing && op!=LT48_OPERATION_SECURE && op!=LT48_OPERATION_CONFIGURATION && r->i2c_config<3 && saved_i2c) {
            unsigned char before[256];lt48_serial_page_result check={0};
            status=cycle(&s,1,0,256,NULL,before,r->nominal,&check);
            if(status==LT48_OK && memcmp(before,desired,128))status=LT48_SAFETY_REJECTED;
        }
    }
    if(status==LT48_OK) {
        if(r->i2c_config && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)) {
            unsigned wanted=r->i2c_config==1?1:image[0],actual=saved_i2c;
            if(r->i2c_config<3 && (saved_i2c&1) && !(wanted&1))status=LT48_SAFETY_REJECTED;
            if(status==LT48_OK && wanted!=saved_i2c) {
                if(r->i2c_config>=3)status=i2c_settings_write(&s,wanted);
                else {
                    /* Reversible state first; permanent state only when
                     * explicitly requested, after cancellable reversible work. */
                    if(r->i2c_config==2 && ((wanted^saved_i2c)&2))status=i2c_settings_write(&s,(wanted&2)?2:3);
                    if(status==LT48_OK && (wanted&1) && !(saved_i2c&1))status=i2c_settings_write(&s,1);
                }
                if(status==LT48_OK)status=i2c_settings_read(&s,&actual);
                if(status==LT48_OK && actual!=(r->i2c_giantec && (wanted&1)?3u:wanted))status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK)saved_i2c=r->i2c_giantec && (wanted&1)?3u:wanted;
        } else if(op==LT48_OPERATION_SECURE) {
            if(r->extra)status=extra_protection(&s,1);
            else {s.mode=1;status=run(&s,2);
            if(status==LT48_OK)status=run(&s,7); /* Source ProgramConf: BP=3, WPEN=0. */
            if(status==LT48_OK)status=run(&s,6); /* Independent ReadConf. */
            if(status==LT48_OK && (s.buffers[2][0]&(r->flash?r->flash->status_mask:r->nor?0x9c:0x8c))!=(r->flash?r->flash->protected_status:r->nor?0x1c:0x0c))status=LT48_DEVICE_MISMATCH;}
        } else if(writing) {
            if(r->extra)status=extra_before_write(&s);
            else if(r->nor) {
                s.mode=1;status=run(&s,2);
                if(status==LT48_OK)status=run(&s,7);
                if(status==LT48_OK)status=run(&s,6);
                if(status==LT48_OK && (s.buffers[2][0]&(r->flash?r->flash->status_mask:0x9c)))status=LT48_DEVICE_MISMATCH;
                if(status==LT48_OK)status=run(&s,3);
            }
            uint32_t mask=r->page_size-1,first=start&~mask;size_t span=((start+length+mask)&~(size_t)mask)-first;
            if(status==LT48_OK && (r->nor||r->rram) && op==LT48_OPERATION_ERASE) {
                if(r->extra)status=extra_erase(&s);
                else {s.mode=7;status=run(&s,2);
                if(status==LT48_OK)status=run(&s,8);
                if(status==LT48_OK && s.buffers[2][0]!=1)status=LT48_DEVICE_MISMATCH;
                if(status==LT48_OK)status=run(&s,3);}
            } else if(status==LT48_OK)status=cycle(&s,2,first,span,desired+first,NULL,r->nominal,out);
            for(unsigned i=0;status==LT48_OK&&i<2;i++)status=cycle(&s,4,0,r->capacity,desired,NULL,margin_voltage(r,margin_type,i),out);
        } else if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};size_t n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
            for(size_t i=0;status==LT48_OK&&i<n;i++)status=cycle(&s,4,start,length,image,NULL,volts[i],out);
        } else status=cycle(&s,op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:5,start,length,image,pending,r->nominal,out);
    }
    if(status==LT48_OK && r->i2c_config && writing){unsigned actual=0;status=i2c_settings_read(&s,&actual);if(status==LT48_OK && actual!=saved_i2c)status=LT48_DEVICE_MISMATCH;}
    out->operation_status=status;
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;s.api=&cleanup;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        const unsigned char stop[]={0,0};unsigned char reply[512]={0};
        lt48_status step=cmd(&s,stop,sizeof(stop),reply);
        if(step==LT48_OK&&!(reply[0]|reply[1]))step=LT48_PROTOCOL_ERROR;
        if(step==LT48_OK)step=run(&s,3);
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
        lt48_serial_page_result fresh;
        unsigned fresh_settings=0;
        status=serial_page_execute_impl(r,api,context,LT48_OPERATION_READ,0,r->capacity,
            NULL,0,pending,r->capacity,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&fresh,&fresh_settings);
        if(status==LT48_OK && r->i2c_config && fresh_settings!=saved_i2c)status=LT48_DEVICE_MISMATCH;
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
    if(status==LT48_OK && observed_settings)*observed_settings=saved_i2c;
    free(s.config_image);free(pending);free(desired);return status;
}

lt48_status lt48_serial_page_execute(const lt48_serial_page_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_serial_page_result *out) {
    return serial_page_execute_impl(r,api,context,op,start,length,image,size,read,read_size,margin,margin_type,decide,decision_context,out,NULL);
}
