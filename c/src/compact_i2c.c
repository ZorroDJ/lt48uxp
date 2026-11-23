#include <labtool48/labtool48.h>
#include "../../NewDeviceLib/sources/xicor-compact2/admission.h"
#include "../../NewDeviceLib/sources/xicor-compact2/reset.inc"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../NewDeviceLib/sources/x4c105one/setup.h"
struct lt48_compact_i2c_resources {

    unsigned read_from_zero,write_whole,protection,at17,at17_osc,x4c105;
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *wave[16];
    lt48_compile_variable vars[8];size_t var_count;
    lt48_wave_code applet[2];unsigned applet_count;
    unsigned buffer_count,mode_buffer,address_buffer,input_buffer,output_buffer;
    unsigned init_op,reset_op,load_op,program_op,width,write_chunk;uint32_t capacity;uint16_t nominal;
};
void lt48_compact_i2c_close(lt48_compact_i2c_resources *r) {
    if(!r)return;
    for(size_t i=0;i<16;i++)lt48_compiled_wave_close(r->wave[i]);
    free(r->idle);lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r);
}
uint32_t lt48_compact_i2c_capacity(const lt48_compact_i2c_resources *r){return r?r->capacity:0;}
int lt48_compact_i2c_is_at17(const lt48_compact_i2c_resources *r){return r ? r->at17 : 0;}
unsigned lt48_compact_i2c_at17_configuration_bits(const lt48_compact_i2c_resources *r){return r && r->at17 ? ((r->at17==3?2u:1u)+(r->at17_osc?1u:0u)):0u;}
int lt48_compact_i2c_has_protection(const lt48_compact_i2c_resources *r){return r && r->protection;}
int lt48_compact_i2c_has_half_protection(const lt48_compact_i2c_resources *r){return r && (r->protection==4 || r->protection==5);}
int lt48_compact_i2c_has_endurance_config(const lt48_compact_i2c_resources *r){return r && r->protection==6;}
unsigned lt48_compact_i2c_spd_protection_bits(const lt48_compact_i2c_resources *r){return r?(r->protection==7?4u:r->protection==8?2u:0u):0u;}
lt48_status lt48_compact_i2c_open(const lt48_installation *x,size_t device,lt48_compact_i2c_resources **out) {
    if(!x || !out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;lt48_status status=lt48_installation_resolve_device(x,device,&spec);
    if(status!=LT48_OK)return status;
    if(spec.catalogue.device_class!=2000 || strcmp(spec.definition_stem,"GA_____"))return LT48_UNSUPPORTED;
    static const unsigned legacy[][4]={
        {99,39,128,8},
        {100,39,256,8},
        {101,40,512,16},
        {102,39,512,8},
        {103,40,1024,16},
        {104,40,2048,16},
        {114,38,128,4},
        {115,38,256,4},
        {116,40,2048,16},
        {117,40,128,16},
        {118,40,256,16},
        {119,40,512,16},
        {120,40,1024,16},
        {121,40,2048,16},
    };
    const unsigned *old=NULL;
    for(size_t i=0;i<sizeof(legacy)/sizeof(legacy[0]);i++)
        if(spec.definition_resource==legacy[i][0] && spec.family_code==legacy[i][1] &&
           spec.capacity_bytes==legacy[i][2] && spec.pinmap==0x13)old=legacy[i];
    static const unsigned additions[][11]={
#include "serial_legacy_i2c.inc"
        {234,111,49,128,8,2,3,16,6,0,0},
        {232,112,48,256,8,2,3,16,6,0,0},
        {235,112,49,256,8,2,3,16,6,0,0},
        {233,113,48,512,8,2,3,16,6,0,0},
        {236,113,49,512,8,2,3,16,6,0,0},

        {0x19f,0xbd,0x80,262144,8,256,4,8,10,0,0},
        {0x1a0,0xc1,0x81,262144,20,256,4,8,10,0,0},
        {0x1a1,0xbe,0x85,262144,44,256,4,8,10,0,0},
        {0x1a5,0xc1,0x82,262144,20,256,4,8,10,0,0},
        {0x1a6,0xc2,0x83,262144,32,256,4,8,10,0,0},
        {0x1aa,0xc2,0x84,524288,32,256,4,8,10,0,0},
        {0x1a2,0xbf,0x80,262144,8,256,4,8,10,0,0},
        {0x1a3,0xc3,0x81,262144,20,256,4,8,10,0,0},
        {0x1a4,0xc0,0x85,262144,44,256,4,8,10,0,0},
        {0x1a7,0xc3,0x82,262144,20,256,4,8,10,0,0},
        {0x1a8,0xc4,0x83,262144,32,256,4,8,10,0,0},
        {0x1a9,0xc5,0x85,524288,44,256,4,8,10,0,0},
        {0x1ab,0xc4,0x84,524288,32,256,4,8,10,0,0},
        {0x19b,0xb9,0x7c,262144,20,128,2,8,9,1,0},
        {0x19c,0xba,0x7e,262144,20,128,2,8,9,1,0},
        {0x19d,0xbb,0x7c,262144,20,128,2,8,9,1,0},
        {0x19e,0xbc,0x7e,262144,20,128,2,8,9,1,0},
        {0x18b,0xb3,0x7b,131072,8,128,2,8,9,0,0},
        {0x18c,0xb3,0x7c,131072,20,128,2,8,9,0,0},
        {0x18f,0xb4,0x7b,131072,8,128,2,8,9,0,0},
        {0x190,0xb4,0x7e,131072,20,128,2,8,9,0,0},
        {0x191,0xb5,0x7f,131072,32,128,2,8,9,0,0},
        {0x189,0xb3,0x7b,65536,8,128,2,8,9,0,0},
        {0x18a,0xb3,0x7c,65536,20,128,2,8,9,0,0},
        {0x18d,0xb4,0x7b,65536,8,128,2,8,9,0,0},
        {0x18e,0xb4,0x7e,65536,20,128,2,8,9,0,0},
        {0x194,0xb6,0x7b,131072,8,128,2,8,9,0,0},
        {0x195,0xb6,0x7c,131072,20,128,2,8,9,0,0},
        {0x198,0xb7,0x7b,131072,8,128,2,8,9,0,0},
        {0x199,0xb7,0x7e,131072,20,128,2,8,9,0,0},
        {0x19a,0xb8,0x7f,131072,32,128,2,8,9,0,0},
        {0x192,0xb6,0x7b,65536,8,128,2,8,9,0,0},
        {0x193,0xb6,0x7c,65536,20,128,2,8,9,0,0},
        {0x196,0xb7,0x7b,65536,8,128,2,8,9,0,0},
        {0x197,0xb7,0x7e,65536,20,128,2,8,9,0,0},
        {0x17c,0xb0,0x7b,16384,8,64,2,8,6,0,0},
        {0x17d,0xb0,0x7d,16384,20,64,2,8,6,0,0},
        {0x17e,0xb0,0x7b,32768,8,64,2,8,6,0,0},
        {0x17f,0xb0,0x7d,32768,20,64,2,8,6,0,0},
        {0x17a,0xb0,0x7b,8192,8,64,2,8,6,0,0},
        {0x17b,0xb0,0x7d,8192,20,64,2,8,6,0,0},
        {0x182,0xb1,0x7b,16384,8,64,2,8,6,0,0},
        {0x183,0xb1,0x7d,16384,20,64,2,8,6,0,0},
        {0x186,0xb1,0x7e,16384,20,64,2,8,6,0,0},
        {0x184,0xb1,0x7b,32768,8,64,2,8,6,0,0},
        {0x185,0xb1,0x7d,32768,20,64,2,8,6,0,0},
        {0x187,0xb1,0x7e,32768,20,64,2,8,6,0,0},
        {0x180,0xb1,0x7b,8192,8,64,2,8,6,0,0},
        {0x181,0xb1,0x7d,8192,20,64,2,8,6,0,0},
        {0x61,0x25,0x12,256,8,16,4,8,8,0,0},
        {0x131,0x9a,0,256,8,16,2,8,8,1,0},
        {0xab,0x45,0,8192,8,16,2,8,10,0,0},
        {0xac,0x46,0,8192,8,16,2,8,10,0,0},
        {0xd7,0x5f,0x2a,512,8,16,5,8,9,1,1},
        {0xd8,0x60,0x2a,512,8,16,5,8,9,1,1},
        {0xd9,0x61,0x2a,512,8,16,5,8,9,1,1},
        {0xda,0x62,0x2a,512,8,16,5,8,9,1,1},
        {0xdb,0x63,0x2b,256,8,16,5,8,9,1,1},
    };
    static const unsigned xicor_closures[][11]={
#include "../../NewDeviceLib/sources/xicor-compact2/profiles.inc"
    };
    const unsigned *extra=NULL;
    if(LT48_XICOR_COMPACT2_ADMITTED && spec.catalogue.vendor && !strcmp(spec.catalogue.vendor,"Xicor"))for(size_t i=0;i<2;i++){if(i==1&&!LT48_XICOR_X4C105_ADMITTED)continue;const unsigned*a=xicor_closures[i];if(spec.definition_resource==a[0]&&spec.family_code==a[1]&&spec.pinmap==a[2]&&spec.capacity_bytes==a[3]&&spec.pin_count==a[4]&&spec.element_bits==a[7])extra=a;}
    for(size_t i=0;i<sizeof(additions)/sizeof(*additions);i++) {
        const unsigned *a=additions[i];
        if(spec.definition_resource==a[0] && spec.family_code==a[1] &&
           spec.pinmap==a[2] && spec.capacity_bytes==a[3] && spec.pin_count==a[4] && spec.element_bits==a[7])extra=a;
    }
    if(!extra && !old && ((spec.definition_resource!=0xa6 && spec.definition_resource!=0xfffc) ||
       spec.family_code!=0x42 || spec.pinmap!=0 || spec.capacity_bytes!=8192))return LT48_UNSUPPORTED;
    if(!extra && (spec.element_bits!=8 || spec.pin_count!=8))return LT48_UNSUPPORTED;
    lt48_compact_i2c_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    char path[4096],name[32],hex[65];unsigned char digest[32];size_t index,fi;
    int n=snprintf(path,sizeof(path),"%s/GA_____.rez",lt48_installation_path(x));
    if(n<0 || (size_t)n>=sizeof(path)){free(r);return LT48_INVALID_ARGUMENT;}
#define TRY(call) do{status=(call);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(size_t i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    /* AEC 14.30 GA differs only in Version. Admit the three audited new
     * catalogue aliases; other 14.30 owners retain their own source gates. */
    int aec1430 = !strcmp(hex,"dc8181694cd156c82014857990cb02b4671a1c8907095547960656e1c986433d") &&
        !strcmp(lt48_installation_source_profile(x),"AEC 14.30") &&
        extra && spec.family_code==0xb1 && spec.pinmap==0x7d &&
        (spec.definition_resource==0x181 || spec.definition_resource==0x183 || spec.definition_resource==0x185);
    if(strcmp(hex,"a286d6799117d4e188067376cab75843fbb9299f4990091d91ad27d8abab99ad") && !aec1430){status=LT48_SAFETY_REJECTED;goto fail;}
    r->write_chunk=extra?extra[5]:old?old[3]:spec.definition_resource==0xfffc?32:16;
    r->read_from_zero=extra?extra[9]:0;r->write_whole=extra?extra[10]:0;
    r->protection=extra && spec.family_code>=0x91 && spec.family_code<=0x99 ? (spec.family_code>=0x97?2u:1u) : 0u;
    if(extra && ((spec.family_code>=0x2e && spec.family_code<=0x30) ||
       (spec.family_code>=0x8d && spec.family_code<=0x90)))r->protection=3;
    if(extra && spec.definition_resource==0x61)r->protection=4;
    if(extra && spec.definition_resource==0x131)r->protection=5;
    if(extra && (spec.definition_resource==0xab || spec.definition_resource==0xac))r->protection=6;
    if(extra && spec.family_code>=0x5f && spec.family_code<=0x63)r->protection=spec.family_code==0x63?8:7;
    r->at17=(spec.family_code==0xb0 || spec.family_code==0xb1)?1u:(spec.family_code>=0xb3 && spec.family_code<=0xb8)?2u:(spec.family_code>=0xb9 && spec.family_code<=0xbc)?3u:(spec.family_code>=0xbd && spec.family_code<=0xc5)?4u:0u;
    r->at17_osc=spec.family_code==0xb4 || spec.family_code==0xb5 || spec.family_code==0xb7 || spec.family_code==0xb8 || spec.family_code==0xba || spec.family_code==0xbc || (r->at17==4 && spec.pinmap>=0x82 && spec.pinmap<=0x84);
    r->x4c105=spec.definition_resource==0xe5 && spec.family_code==0x6d;
    r->capacity=spec.capacity_bytes;r->width=spec.element_bits/8;
    r->buffer_count=70;r->mode_buffer=7;r->address_buffer=1;r->input_buffer=2;r->output_buffer=3;
    r->init_op=2;r->reset_op=6;r->load_op=7;r->program_op=9;
    lt48_pin_code code;snprintf(name,sizeof(name),"P%x",spec.pinmap);
    TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_pin_code(r->rez,index,&code));if(r->x4c105)TRY(x4c105_pins(&code,&r->pins));else TRY(lt48_pin_map_parse(&code,&r->pins));
    /* GA Kernel declares thirteen zero-initialized control groups. */
    TRY(lt48_pin_map_complete_groups_internal(r->pins,13));
    lt48_family_info info;snprintf(name,sizeof(name),"M%x",spec.family_code);
    TRY(lt48_rez_find_resource(r->rez,name,&fi));TRY(lt48_rez_get_family_info(r->rez,fi,&info));
    if(info.variable_count!=(extra?extra[6]:old?4u:2u) || info.operation_count!=(extra?extra[8]:7u)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=info.variable_count;
    for(size_t i=0;i<r->var_count;i++){lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};if(v.values[1]==0)r->nominal=v.values[3];}
    if(r->x4c105){for(size_t i=0;i<r->var_count;i++){if(r->vars[i].variable_id==0)r->vars[i].value=330;if(r->vars[i].variable_id==21)r->vars[i].channel=5000;}r->nominal=330;}
    for(size_t i=0;i<info.operation_count;i++){
        lt48_family_operation op;lt48_wave_code wave;lt48_compact_wave *compact=NULL;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));TRY(lt48_rez_get_wave_code(r->rez,index,&wave));
        if(r->at17==4 && (op.operation_id==16 || op.operation_id==17)){
            if(wave.wave_type!=6 || wave.load_address!=(op.operation_id==16?0x4100u:0x4200u) || !wave.data_size || wave.data_size>256 || wave.length1!=wave.data_size+4 || wave.length2!=wave.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            r->applet[op.operation_id-16]=wave;r->applet_count++;continue;
        }
        if(wave.wave_type!=0 || op.operation_id>=16){status=LT48_FORMAT_ERROR;goto fail;}
        unsigned char page32[110],safe_x4[512];size_t safe_x4_size=0;
        if(spec.definition_resource==0xfffc && op.operation_id==9) {
            /* X24641 datasheet: begin writes at a 32-byte page boundary.
             * Source W7a masks address low bits and stops at the final byte.
             * Change only mask 0x0f and terminal value 0x0f to 0x1f. */
            if(op.wave!=0x7a || wave.data_size!=sizeof page32 ||
               wave.data[8]!=0x4b || wave.data[11]!=15 ||
               wave.data[53]!=0x5e || wave.data[56]!=15){status=LT48_SAFETY_REJECTED;goto fail;}
            memcpy(page32,wave.data,sizeof page32);page32[11]=31;page32[56]=31;
            TRY(lt48_compact_decode(page32,sizeof page32,&compact));
        } else if(LT48_XICOR_COMPACT2_ADMITTED && spec.definition_resource==0xbe && op.operation_id==6)TRY(lt48_compact_decode(x84041_reset,sizeof x84041_reset,&compact));
        else if(r->x4c105){TRY(x4c105_wave(op.operation_id,&wave,safe_x4,&safe_x4_size));TRY(lt48_compact_decode(safe_x4,safe_x4_size,&compact));}
        else TRY(lt48_compact_decode(wave.data,wave.data_size,&compact));
        status=lt48_compact_compile(compact,r->pins,r->vars,r->var_count,1u,0,&r->wave[op.operation_id]);lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
        if(lt48_compiled_wave_defaulted_pin_count(r->wave[op.operation_id])){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    if(r->at17==4 && (r->applet_count!=2 || !r->applet[0].data || !r->applet[1].data)){status=LT48_FORMAT_ERROR;goto fail;}
    if(r->protection==6) {
        /* Retain W7d's two configuration command prefixes, but capture whole
         * response bytes. Resident 0x61 samples before clocking and omits its
         * final clock: the two subsequent clocks finish the byte and send
         * ACK/NACK, as in the source ordinary-memory W79/W7c sequences.
         * This avoids relying on the split-nibble response alignment. */
        lt48_wave_code source;TRY(lt48_rez_find_resource(r->rez,"W7d",&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&source));
        if(source.data_size!=0x13e){status=LT48_FORMAT_ERROR;goto fail;}
        unsigned char raw[512];size_t used=2;
        static const unsigned char stop[]={0x11,0x19,0,0x11,0x17,1,0x11,0x19,1,0x11,0x17,0};
        for(unsigned field=1;field<=3;field++) {
            if(field!=2){size_t offset=field==1?2:0xad;memcpy(raw+used,source.data+offset,96);used+=96;}
            unsigned char capture[]={0x1e,0x19,0x17,2,8,1,
                0x1a,0x19,0x17,0,0,0,0,2,1,
                0x4b,1,2,15,0,0,0,4,0x6f,0,2,0,2};
            capture[9]=field==1?0:2;capture[26]=field%3;
            memcpy(raw+used,capture,sizeof(capture));used+=sizeof(capture);
            if(field!=1){memcpy(raw+used,stop,sizeof(stop));used+=sizeof(stop);}
        }
        raw[used++]=0x31;raw[used++]=0x11;raw[0]=(unsigned char)(used-2);raw[1]=(unsigned char)((used-2)>>8);
        lt48_compact_wave *readconf=NULL;TRY(lt48_compact_decode(raw,used,&readconf));
        lt48_compiled_wave_close(r->wave[12]);r->wave[12]=NULL;
        status=lt48_compact_compile(readconf,r->pins,r->vars,r->var_count,1,0,&r->wave[12]);
        lt48_compact_wave_close(readconf);if(status!=LT48_OK)goto fail;
    }
    if(r->protection>=4 || r->x4c105) {
        /* Compose an address-only ACK query from digest-bound source Wd4:
         * START / buffer-28 byte / sample ACK / clock completion, then STOP.
         * The initial STOP completes W158, which leaves its write open.
         * Never transmit a word address or data in this query: 0x60 alone
         * cannot set the protection register. 0xa0 proves device presence. */
        static const unsigned char body[]={
            0x11,0x19,0,0x11,0x17,1,0x11,0x19,1,0x11,0x17,0,0x31,0x11,
            0x11,0x17,0,0x11,0x19,1,0x11,0x17,1,0x11,0x19,0,0x11,0x17,0,
            0x1c,0x19,0x17,0x1c,8,1,0x1e,0x19,0x17,8,1,1,0x2b,0x17,1,
            0x11,0x19,0,0x11,0x17,1,0x11,0x19,1,0x11,0x17,0};
        unsigned char raw[sizeof(body)+7];size_t prefix=r->x4c105?5:0,n=sizeof(body)+2+prefix;raw[0]=(unsigned char)(n-2);raw[1]=0;
        if(prefix){static const unsigned char settle[]={0x32,0x20,0xa1,7,0};memcpy(raw+2,settle,sizeof settle);}
        memcpy(raw+2+prefix,body,sizeof(body));lt48_compact_wave *probe=NULL;
        TRY(lt48_compact_decode(raw,n,&probe));
        status=lt48_compact_compile(probe,r->pins,r->vars,r->var_count,1u,0,&r->wave[15]);
        lt48_compact_wave_close(probe);if(status!=LT48_OK)goto fail;
        if(lt48_compiled_wave_defaulted_pin_count(r->wave[15])){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:lt48_compact_i2c_close(r);return status;
#undef TRY
}
lt48_status lt48_compact_i2c_validate(const lt48_compact_i2c_resources *r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r || !length || start>=r->capacity || length>r->capacity-start || start%r->width || length%r->width)return LT48_SAFETY_REJECTED;
    if(r->at17 && op==LT48_OPERATION_CONFIGURATION){
        if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
        return image && size==1 && image[0]<=((1u<<lt48_compact_i2c_at17_configuration_bits(r))-1u)?LT48_OK:LT48_INVALID_ARGUMENT;
    }
    if(lt48_compact_i2c_spd_protection_bits(r) && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)) {
        unsigned mask=(1u<<lt48_compact_i2c_spd_protection_bits(r))-1;
        if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
        if(!image || size!=1 || image[0]>mask || (op==LT48_OPERATION_SECURE && !image[0]))return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(r->protection==6 && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)) {
        if(start || length!=r->capacity)return LT48_SAFETY_REJECTED;
        if(!image || size!=(op==LT48_OPERATION_SECURE?2u:1u))return LT48_INVALID_ARGUMENT;
        if(image[0]>15)return LT48_INVALID_ARGUMENT;
        if(op==LT48_OPERATION_SECURE && (!image[1] || image[1]>15 || image[0]+image[1]>16))return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(op<LT48_OPERATION_READ || op>(r->protection?LT48_OPERATION_SECURE:LT48_OPERATION_ERASE))return LT48_UNSUPPORTED;
    if((op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE) && (start || length!=r->capacity))return LT48_SAFETY_REJECTED;
    if(op!=LT48_OPERATION_READ && op!=LT48_OPERATION_BLANK_CHECK && op!=LT48_OPERATION_ERASE && op!=LT48_OPERATION_SECURE && (!image || size!=length))return LT48_INVALID_ARGUMENT;
    return LT48_OK;
}
lt48_status lt48_compact_i2c_validate_verify_options(const lt48_compact_i2c_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type) {
    if(!r || type>1 || (margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL &&
       margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;
    (void)op;
    return LT48_OK;
}
typedef struct compact_i2c_session {
    const lt48_compact_i2c_resources *r;
    lt48_runtime_transport_session *wire;
    const lt48_protocol_transport *api;
    void *context;
    unsigned char buffers[70][32];
    uint32_t volts[4];
    lt48_compile_variable vars[8];
    uint32_t mode;
    unsigned fault;
    /* 24C65 has three fields; SPD EEPROMs have up to four block flags.
     * Retain the complete source configuration across nested operations. */
    lt48_runtime_configuration_value config[16];size_t config_count;
    unsigned protection_start,protection_clear,protection_valid;
    unsigned spd_saved_mask,spd_dirty;
} compact_i2c_session;
static lt48_status run(compact_i2c_session *s,unsigned op) {
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
    if(status!=LT48_OK && getenv("LT48_RUNTIME_DEBUG"))fprintf(stderr,"I2C machine op %u status %d\n",op,status);
    if(status==LT48_OK) {
        const lt48_runtime_configuration_value *values;size_t n;
        status=lt48_runtime_get_configuration(result,&values,&n);
        if(status==LT48_OK && n>sizeof(s->config)/sizeof(*s->config))status=LT48_RUNTIME_ERROR;
        if(status==LT48_OK){memcpy(s->config,values,n*sizeof(*values));s->config_count=n;}
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
    if(status!=LT48_OK && getenv("LT48_RUNTIME_DEBUG"))fprintf(stderr,"compact I2C op %u status %d fault %u\n",op,status,s->fault);
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_compile_variable *var(compact_i2c_session *s,unsigned id) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==id)return &s->vars[i];
    return NULL;
}
/* A missing or busy device must not be mistaken for a locked device.
 * Bracket the 0x60 query with ordinary 0xa0 presence acknowledgements. */
static lt48_status memory_presence(compact_i2c_session *s) {
    memset(s->buffers[8],0,32);memset(s->buffers[28],0,32);s->buffers[28][0]=0xa0;
    lt48_status status=run(s,15);
    return status==LT48_OK && (s->buffers[8][0]&1)?LT48_DEVICE_MISMATCH:status;
}
static lt48_status endurance_fields(compact_i2c_session *s,unsigned values[3]) {
    if(s->config_count!=3)return LT48_DEVICE_MISMATCH;
    unsigned seen=0;
    for(size_t i=0;i<3;i++) {
        const lt48_runtime_configuration_value *v=&s->config[i];
        if(v->object_id!=0 || v->category!=2 || v->field>2 || v->value>15 || (seen&(1u<<v->field)))return LT48_DEVICE_MISMATCH;
        values[v->field]=v->value;seen|=1u<<v->field;
    }
    return seen==7?LT48_OK:LT48_DEVICE_MISMATCH;
}
static lt48_status endurance_check(compact_i2c_session *s,const unsigned wanted[3]) {
    lt48_status status=memory_presence(s);
    if(status==LT48_OK)status=run(s,12);
    unsigned actual[3];
    if(status==LT48_OK)status=endurance_fields(s,actual);
    if(status==LT48_OK && memcmp(actual,wanted,sizeof(actual)))status=LT48_DEVICE_MISMATCH;
    return status;
}
static lt48_status half_protection_read(compact_i2c_session *s,unsigned *locked) {
    static const unsigned char addresses[]={0xa0,0x60,0xa0};
    unsigned found=0;
    for(unsigned i=0;i<3;i++) {
        memset(s->buffers[8],0,32);memset(s->buffers[28],0,32);
        s->buffers[28][0]=addresses[i];lt48_status status=run(s,15);
        if(status!=LT48_OK)return status;
        unsigned nack=s->buffers[8][0]&1;
        if(i==1)found=nack;else if(nack)return LT48_DEVICE_MISMATCH;
    }
    *locked=found;return LT48_OK;
}
static lt48_status spd_fields(compact_i2c_session *s,unsigned *mask) {
    unsigned bits=lt48_compact_i2c_spd_protection_bits(s->r),seen=0,value=0;
    if(!bits || s->config_count!=bits)return LT48_DEVICE_MISMATCH;
    for(size_t i=0;i<s->config_count;i++) {
        const lt48_runtime_configuration_value *v=&s->config[i];
        if(v->object_id!=-1 || v->category!=0 || v->field>=bits || v->value>1 || (seen&(1u<<v->field)))return LT48_DEVICE_MISMATCH;
        seen|=1u<<v->field;value|=v->value<<v->field;
    }
    if(seen!=(1u<<bits)-1)return LT48_DEVICE_MISMATCH;
    *mask=value;return LT48_OK;
}
static lt48_status spd_read(compact_i2c_session *s,unsigned *mask) {
    lt48_status status=memory_presence(s);
    if(status==LT48_OK)status=run(s,12);
    if(status==LT48_OK)status=spd_fields(s,mask);
    if(status==LT48_OK)status=memory_presence(s);
    return status;
}
static unsigned spd_observed_mask(const lt48_compact_i2c_resources *r,unsigned mask) {
    /* STTS2002 PWP disables every protection-address acknowledgement, so
     * both query flags read one. The reversible flag is then unobservable. */
    return r->protection==8 && (mask&1u)?3u:mask;
}
static lt48_status spd_write(compact_i2c_session *s,unsigned mask) {
    lt48_compile_variable *v=var(s,0);if(v)v->value=s->r->nominal;
    unsigned bits=lt48_compact_i2c_spd_protection_bits(s->r);
    s->mode=8;lt48_status status=run(s,0);
    if(status==LT48_OK)status=run(s,2);
    s->config_count=bits;
    for(unsigned i=0;i<bits;i++)s->config[i]=(lt48_runtime_configuration_value){-1,0,(uint16_t)i,(mask>>i)&1u};
    if(status==LT48_OK)status=run(s,11);
    unsigned actual=0;
    if(status==LT48_OK)status=spd_read(s,&actual);
    if(status==LT48_OK && actual!=spd_observed_mask(s->r,mask))status=LT48_DEVICE_MISMATCH;
    return status;
}
static lt48_status spd_restore(compact_i2c_session *s) {
    if(!s->spd_dirty)return LT48_OK;
    unsigned wanted=s->spd_saved_mask;lt48_status status=LT48_OK;
    if(s->r->protection==8) {
        /* Discover an already-applied permanent bit; never burn a new one
         * merely to recover from an interrupted explicit setting operation. */
        s->mode=8;status=run(s,0);
        if(status==LT48_OK)status=run(s,2);
        unsigned actual=0;if(status==LT48_OK)status=spd_read(s,&actual);
        if(status==LT48_OK)wanted|=actual&1u;
    }
    if(status==LT48_OK)status=spd_write(s,wanted);
    if(status==LT48_OK)s->spd_dirty=0;
    return status;
}
/* W14e normally captures only the boundary. ST PRREAD also returns the
 * protection flag immediately after it. Request that extra bit so PRCLEAR
 * and protection of the last word remain distinguishable. No REZ is patched. */
static lt48_status protection_read(compact_i2c_session *s) {
    if(s->r->protection==3) {
        /* READCONF copies one/two bits into scratch byte 11. Clear the
         * prior ProgramConf selector before the one-bit WPEN extraction. */
        memset(s->buffers[11],0,32);return run(s,12);
    }
    unsigned count=s->buffers[8][0];
    if(count!=6 && count!=8)return LT48_FORMAT_ERROR;
    s->buffers[8][0]=(unsigned char)(count+(s->r->protection==2));s->mode=1;
    lt48_status status=run(s,6);s->buffers[8][0]=(unsigned char)count;
    if(status==LT48_OK) {
        unsigned raw=s->buffers[1][0]|(unsigned)s->buffers[1][1]<<8;
        unsigned address_mask=s->r->capacity/s->r->width-1u;
        if(s->r->protection==2) {
            s->protection_start=(raw>>1)&address_mask;s->protection_clear=raw&1;
        } else {
            /* National/Microchip PRREAD exposes no clear flag. Their source
             * Program clears protection; do not infer or restore hidden state. */
            s->protection_start=raw&address_mask;s->protection_clear=s->protection_start==address_mask;
        }
        s->protection_valid=1;
    }
    return status;
}
static lt48_status protection_write(compact_i2c_session *s,unsigned start) {
    if(s->r->protection==3) {
        /* W4b/W149 select saved configuration when Address is nonzero. */
        memset(s->buffers[1],0,32);s->buffers[1][0]=1;
        return run(s,13);
    }
    /* Explicit zero for PRDS: ordinary protection never burns the OTP lock. */
    s->config[0]=(lt48_runtime_configuration_value){0,2,3,start};
    s->config[1]=(lt48_runtime_configuration_value){0,0,0,0};s->config_count=2;
    return run(s,11);
}
/* Small AT17 source DEVINIT observes polarity in read mode and writes the
 * explicitly supplied setting in program mode. It is outside the array. */
static int at17_field_bit(const lt48_compact_i2c_resources *r,const lt48_runtime_configuration_value *v){
    if(v->object_id)return -1;
    if(r->at17==3){
        if(v->category==1 && v->field==6)return 0;
        if(v->category==1 && v->field==7)return 1;
        if(r->at17_osc && v->category==0 && v->field==4)return 2;
    }else{
        if(v->category==1 && v->field==2)return 0;
        if((r->at17==2 || r->at17==4) && v->category==0 && v->field==4)return 1;
    }
    return -1;
}
static lt48_status at17_polarity(compact_i2c_session *s,unsigned *value){
    s->mode=1;lt48_status st=run(s,1);
    if(st==LT48_OK)st=run(s,0);
    if(st==LT48_OK)st=run(s,2);
    if(st==LT48_OK && s->r->at17==4)st=run(s,6);
    if(st!=LT48_OK)return st;
    unsigned found=0;*value=0;
    for(size_t i=0;i<s->config_count;i++) {
        const lt48_runtime_configuration_value *v=&s->config[i];int bit=at17_field_bit(s->r,v);
        if(bit<0 || v->value>1)continue;
        *value|=v->value<<bit;found|=1u<<bit;
    }
    unsigned bits=(s->r->at17==2 || s->r->at17==4)?2u:lt48_compact_i2c_at17_configuration_bits(s->r);
    return found==((1u<<bits)-1u)?LT48_OK:LT48_RUNTIME_ERROR;
}
static lt48_status cycle(compact_i2c_session *s,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_compact_i2c_result *out) {
    unsigned saved_start=s->protection_start,saved_clear=s->protection_clear;
    lt48_runtime_configuration_value saved_config[2];
    memcpy(saved_config,s->config,sizeof(saved_config));
    memset(s->buffers,0,sizeof(s->buffers));
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM;
    unsigned saved_endurance[3]={0};
    if(writing && s->r->protection==6) {
        lt48_status config_status=endurance_fields(s,saved_endurance);
        if(config_status!=LT48_OK)return config_status;
    }
    s->mode=writing?2:1;
    if(writing && ((start&(s->r->write_chunk-1))||(length&(s->r->write_chunk-1))))return LT48_SAFETY_REJECTED;
    lt48_compile_variable *v=var(s,0);if(v)v->value=vcc;
    /* This DEVINIT does not set VCC. Reapply the source POWERON after
     * selecting each margin, otherwise changing the variable alone is inert. */
    if(writing && s->r->protection && s->r->protection<3 && !s->protection_valid)return LT48_SAFETY_REJECTED;
    if(writing && s->r->protection==3 && s->config_count!=2)return LT48_SAFETY_REJECTED;
    lt48_status status=run(s,0);
    for(unsigned settle=0;status==LT48_OK && settle<3;settle++)status=lt48_runtime_transport_session_status(s->wire,1);
    /* DEVINIT emits the initial serial address before LOAD. Preserve the
     * requested nonzero read start; Program also uses this live address. */
    uint32_t base=(!writing && s->r->read_from_zero)?0:start;
    for(unsigned j=0;j<4;j++)s->buffers[1][j]=(unsigned char)(((writing && s->r->protection==3?0:base)/s->r->width)>>(8*j));
    if(status==LT48_OK && writing && lt48_compact_i2c_spd_protection_bits(s->r)) {
        status=spd_fields(s,&s->spd_saved_mask);
        if(status==LT48_OK)s->spd_dirty=1;
    }
    if(status==LT48_OK && s->r->x4c105)status=memory_presence(s);
    if(status==LT48_OK)status=run(s,s->r->init_op);
    if(status==LT48_OK && writing && s->spd_dirty) {
        unsigned cleared=0;status=spd_read(s,&cleared);
        if(status==LT48_OK && cleared!=(s->r->protection==8 && (s->spd_saved_mask&1u)?3u:0u))status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK && writing && s->r->protection==6)status=endurance_check(s,saved_endurance);
    for(uint32_t pos=base;status==LT48_OK && pos<start+length;pos+=s->r->width){
        size_t i=pos>=start?pos-start:0;
        uint32_t address=pos/s->r->width;
        for(unsigned j=0;j<4;j++)s->buffers[1][j]=(unsigned char)(address>>(8*j));
        memset(s->buffers[2],0,32);memset(s->buffers[3],0,32);
        for(unsigned j=0;j<s->r->width;j++)s->buffers[3][j]=op==LT48_OPERATION_BLANK_CHECK?255:image?image[i+j]:0;
        status=run(s,(op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM)?9:7);
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
    /* Stop a read stream (NACK + STOP). Every complete write chunk already
     * issued STOP and waited tP in the original PROGRAM waveform. */
    lt48_status reset;
    if(s->r->protection==3) {
        /* The source RESET writes the saved WPEN/BP bits. A partial request
         * ending at zero must still select restore rather than clear. */
        uint32_t config_address=(uint32_t)s->buffers[14][0] |
            (uint32_t)s->buffers[14][1]<<8 | (uint32_t)s->buffers[14][2]<<16 |
            (uint32_t)s->buffers[14][3]<<24;
        if(writing && config_address!=s->r->capacity-1)reset=LT48_SAFETY_REJECTED;
        else if(writing) {
            memcpy(s->config,saved_config,sizeof(saved_config));s->config_count=2;
            reset=protection_write(s,0);
            if(reset==LT48_OK)reset=protection_read(s);
            if(reset==LT48_OK && (s->config_count!=2 ||
               memcmp(s->config,saved_config,sizeof(saved_config))))reset=LT48_DEVICE_MISMATCH;
        } else reset=LT48_OK;
        s->mode=1;lt48_status stop=run(s,s->r->reset_op);
        if(reset==LT48_OK)reset=stop;
    } else if(s->r->protection && s->r->protection<3) {
        reset=LT48_OK;
        if(writing && s->r->protection==2 && !saved_clear)reset=protection_write(s,saved_start);
        if(reset==LT48_OK)reset=protection_read(s);
        if(reset==LT48_OK && writing) {
            if(s->r->protection==2) {
                if(s->protection_clear!=saved_clear || (!saved_clear && s->protection_start!=saved_start))reset=LT48_DEVICE_MISMATCH;
            } else if(!s->protection_clear)reset=LT48_DEVICE_MISMATCH;
        }
    } else reset=run(s,s->r->reset_op);
    if(status==LT48_OK)status=reset;
    if(writing && s->spd_dirty) {lt48_status restored=spd_restore(s);if(status==LT48_OK)status=restored;}
    if(status==LT48_OK && writing && s->r->protection==6)status=endurance_check(s,saved_endurance);
    return status==LT48_OK && out->mismatch_count?LT48_DEVICE_MISMATCH:status;
}
lt48_status lt48_compact_i2c_execute(const lt48_compact_i2c_resources *r,const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned margin_type,lt48_compact_i2c_result *out) {
    if(!out || !api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_compact_i2c_validate(r,op,start,length,image,size);if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ && (!read || read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_compact_i2c_validate_verify_options(r,op,margin,margin_type);if(status!=LT48_OK)return status;
    unsigned char *pending=malloc(r->capacity);if(!pending)return LT48_NO_MEMORY;
    unsigned char *expected=malloc(r->capacity);
    if(!expected){free(pending);return LT48_NO_MEMORY;}
    int writing=op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE;
    compact_i2c_session s={0};s.r=r;s.api=api;s.context=context;memcpy(s.vars,r->vars,sizeof(s.vars));s.mode=1;
    status=lt48_runtime_transport_session_create(api,context,&s.wire);if(status!=LT48_OK){free(expected);free(pending);return status;}
    status=lt48_runtime_transport_session_wave_watchdog(s.wire,10000,1);
    /* Class-2000 waves require the Windows idle FPGA. Explicitly establish it
     * so a preceding MCU/EPROM device-specific image cannot leak into this job. */
    for(unsigned i=0;status==LT48_OK && i<r->applet_count;i++){
        const lt48_wave_code *a=&r->applet[i];size_t last=a->data_size-1;
        unsigned char h[]={0x39,0xaa,0x55,(unsigned char)a->load_address,(unsigned char)(a->load_address>>8),(unsigned char)last,(unsigned char)(last>>8)};
        status=lt48_runtime_transport_session_controller_write(s.wire,h,sizeof(h));
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.wire);
        if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,a->data,a->data_size);
        if(status==LT48_OK)status=lt48_runtime_transport_session_flush(s.wire);
    }
    const unsigned char header[]={0x38,0xaa,0x55};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,header,sizeof(header));
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,r->idle,r->idle_size);
    if(status==LT48_OK)status=run(&s,0);
    for(unsigned i=0;status==LT48_OK && i<3;i++)status=lt48_runtime_transport_session_status(s.wire,1);
    static const unsigned char busy[]={0x56,0x0a,1,0x40,1};
    if(status==LT48_OK)status=lt48_runtime_transport_session_controller_write(s.wire,busy,sizeof(busy));
    unsigned at17_saved=0,at17_wanted=0;
    if(status==LT48_OK && r->at17)status=at17_polarity(&s,&at17_saved);
    if(r->at17 && op==LT48_OPERATION_CONFIGURATION)at17_wanted=image[0]|((r->at17==2 || r->at17==4) && !r->at17_osc?(at17_saved&2u):0u);
    unsigned half_locked=0;
    if(status==LT48_OK && lt48_compact_i2c_has_half_protection(r))status=half_protection_read(&s,&half_locked);
    if(status==LT48_OK && (r->protection==6 || lt48_compact_i2c_spd_protection_bits(r)))status=memory_presence(&s);
    uint16_t nominal=r->nominal,lo=(uint16_t)(nominal*(margin_type?90u:95u)/100u),hi=(uint16_t)(nominal*(margin_type?110u:105u)/100u);
    if(r->x4c105){if(lo<300)lo=300;if(hi>360)hi=360;}
    if(status==LT48_OK && writing) {
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,out);
        if(status==LT48_OK){memcpy(expected,pending,r->capacity);if(op==LT48_OPERATION_ERASE)memset(expected+start,255,length);else memcpy(expected+start,image,length);}
        if(status==LT48_OK && lt48_compact_i2c_has_half_protection(r) && half_locked && memcmp(expected,pending,128))status=LT48_SAFETY_REJECTED;
    }
    if(status==LT48_OK && writing) {
        uint32_t mask=r->write_chunk-1,first=start&~mask;size_t span=((start+length+mask)&~(size_t)mask)-first;
        /* 59C INIT performs a bulk erase. Reconstruct the complete saved
         * array so a partial request cannot discard untouched words. */
        if(r->write_whole){first=0;span=r->capacity;}
        status=cycle(&s,LT48_OPERATION_PROGRAM,first,span,expected+first,NULL,nominal,out);
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,expected,NULL,lo,out);
        if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,expected,NULL,hi,out);
    }else if(status==LT48_OK && (op==LT48_OPERATION_SECURE || ((r->at17 || r->protection==6 || lt48_compact_i2c_spd_protection_bits(r)) && op==LT48_OPERATION_CONFIGURATION))){
        status=cycle(&s,LT48_OPERATION_READ,0,r->capacity,NULL,pending,nominal,out);
        if(status==LT48_OK) {
            if(r->at17){
                for(size_t i=0;i<s.config_count;i++) {int bit=at17_field_bit(r,&s.config[i]);if(bit>=0)s.config[i].value=(at17_wanted>>bit)&1u;}
                s.mode=2;status=run(&s,0);if(status==LT48_OK)status=run(&s,2);if(status==LT48_OK && r->at17==2)status=run(&s,13);if(status==LT48_OK)status=run(&s,6);
                unsigned actual=0;if(status==LT48_OK)status=at17_polarity(&s,&actual);
                if(status==LT48_OK && actual!=at17_wanted)status=LT48_DEVICE_MISMATCH;
                if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,pending,NULL,nominal,out);
            } else if(lt48_compact_i2c_spd_protection_bits(r)) {
                unsigned prior=0;status=spd_fields(&s,&prior);
                if(status==LT48_OK && r->protection==8 && (prior&1u) && !(image[0]&1u))status=LT48_SAFETY_REJECTED;
                if(status==LT48_OK && prior!=spd_observed_mask(r,image[0])) {
                    s.spd_saved_mask=prior;s.spd_dirty=1;
                    status=spd_write(&s,image[0]);
                }
                if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,pending,NULL,nominal,out);
                unsigned actual=0;if(status==LT48_OK)status=spd_fields(&s,&actual);
                if(status==LT48_OK && actual!=spd_observed_mask(r,image[0]))status=LT48_DEVICE_MISMATCH;
                if(status==LT48_OK)s.spd_dirty=0;
            } else if(r->protection==6) {
                unsigned previous[3],wanted[3];status=endurance_fields(&s,previous);
                if(status==LT48_OK) {
                    memcpy(wanted,previous,sizeof(wanted));
                    if(op==LT48_OPERATION_CONFIGURATION)wanted[0]=image[0];
                    else {wanted[1]=image[0];wanted[2]=image[1];}
                    if(previous[2] && memcmp(previous,wanted,sizeof(wanted)))status=LT48_SAFETY_REJECTED;
                }
                if(status==LT48_OK && memcmp(previous,wanted,sizeof(wanted))) {
                    for(size_t i=0;i<3;i++)s.config[i].value=wanted[s.config[i].field];
                    s.mode=8;status=run(&s,0);
                    if(status==LT48_OK)status=run(&s,2);
                    if(status==LT48_OK)status=run(&s,op==LT48_OPERATION_CONFIGURATION?13:11);
                }
                if(status==LT48_OK)status=endurance_check(&s,wanted);
                if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,pending,NULL,nominal,out);
            } else if(lt48_compact_i2c_has_half_protection(r)) {
                s.mode=8;status=run(&s,0);
                if(status==LT48_OK)status=run(&s,2);
                if(status==LT48_OK && !half_locked)status=run(&s,11);
                unsigned locked=0;
                if(status==LT48_OK)status=half_protection_read(&s,&locked);
                if(status==LT48_OK && !locked)status=LT48_DEVICE_MISMATCH;
                if(status==LT48_OK)status=cycle(&s,LT48_OPERATION_VERIFY,0,r->capacity,pending,NULL,nominal,out);
            } else if(r->protection==3) {
                /* Original configuration wave; preserve WPEN and set both
                 * reversible block-lock bits, without touching array bytes. */
                lt48_runtime_configuration_value wanted[2];
                if(s.config_count!=2)status=LT48_RUNTIME_ERROR;
                for(size_t i=0;i<s.config_count;i++)if(s.config[i].category==1 && s.config[i].field==1)s.config[i].value=3;
                memcpy(wanted,s.config,sizeof(wanted));
                s.mode=2;memset(s.buffers,0,sizeof(s.buffers));s.buffers[1][0]=1;
                if(status==LT48_OK)status=run(&s,0);
                if(status==LT48_OK)status=run(&s,2);
                if(status==LT48_OK)status=protection_read(&s);
                if(status==LT48_OK && (s.config_count!=2 || memcmp(wanted,s.config,sizeof(wanted))))status=LT48_DEVICE_MISMATCH;
                s.mode=1;lt48_status stop=run(&s,6);if(status==LT48_OK)status=stop;
            } else {
                /* Reestablish controls without the Microwire mode-2 erase. */
                s.mode=8;status=run(&s,0);
                if(status==LT48_OK)status=run(&s,2);
                if(status==LT48_OK)status=protection_write(&s,0);
                if(status==LT48_OK)status=protection_read(&s);
                if(status==LT48_OK && (s.protection_clear || s.protection_start))status=LT48_DEVICE_MISMATCH;
            }
        }
    }else if(status==LT48_OK && op==LT48_OPERATION_VERIFY){
        if(margin==LT48_VERIFY_MARGIN_NOMINAL)status=cycle(&s,op,start,length,image,NULL,nominal,out);
        else{if(margin!=LT48_VERIFY_MARGIN_HIGH)status=cycle(&s,op,start,length,image,NULL,lo,out);if(status==LT48_OK && margin!=LT48_VERIFY_MARGIN_LOW)status=cycle(&s,op,start,length,image,NULL,hi,out);}
    }else if(status==LT48_OK)status=cycle(&s,op,start,length,image,op==LT48_OPERATION_READ?pending:NULL,nominal,out);
    if(status==LT48_OK && r->at17){unsigned observed=0;status=at17_polarity(&s,&observed);
        if(status==LT48_OK && observed!=(op==LT48_OPERATION_CONFIGURATION?at17_wanted:at17_saved))status=LT48_DEVICE_MISMATCH;}
    out->operation_status=status;
    /* A fresh uncancellable transport owns cleanup even after a failed queue.
     * Continue independent rail-off attempts and preserve the first error. */
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    lt48_protocol_transport cleanup=*api;cleanup.cancelled=NULL;
    out->cleanup_status=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
    if(out->cleanup_status==LT48_OK) {
        if(s.spd_dirty) {
            unsigned char stop[512]={0},reply[512]={0};size_t done=0;
            out->cleanup_status=cleanup.write(context,2,stop,sizeof(stop),&done);
            if(out->cleanup_status==LT48_OK && done!=sizeof(stop))out->cleanup_status=LT48_PROTOCOL_ERROR;
            if(out->cleanup_status==LT48_OK)out->cleanup_status=cleanup.read(context,0x88,reply,sizeof(reply),&done);
            if(out->cleanup_status==LT48_OK && (done!=sizeof(reply) || !(reply[0]|reply[1])))out->cleanup_status=LT48_PROTOCOL_ERROR;
            if(out->cleanup_status==LT48_OK)out->cleanup_status=spd_restore(&s);
            lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
            lt48_status restart=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
            if(out->cleanup_status==LT48_OK)out->cleanup_status=restart;
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
        }
    }
    if(lt48_compact_i2c_spd_protection_bits(r) && out->cleanup_status!=LT48_OK) {
        lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
        lt48_status led=lt48_runtime_transport_session_create(&cleanup,context,&s.wire);
        static const unsigned char error[]={0x40,2};
        if(led==LT48_OK)led=lt48_runtime_transport_session_controller_write(s.wire,error,sizeof(error));
        if(led==LT48_OK)led=lt48_runtime_transport_session_flush(s.wire);
        (void)led; /* Preserve the original restoration/rail-off failure. */
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK && op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);
    if(status==LT48_OK && writing) {
        /* Fresh power/read verifies EEPROM persistence and every untouched byte. */
        lt48_compact_i2c_result fresh={0};
        status=lt48_compact_i2c_execute(r,api,context,LT48_OPERATION_READ,0,r->capacity,NULL,0,
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
    if(status==LT48_OK)out->completed_bytes=(r->protection==6 || lt48_compact_i2c_spd_protection_bits(r)) && (op==LT48_OPERATION_SECURE || op==LT48_OPERATION_CONFIGURATION)?size:length;
    free(expected);free(pending);return status;
}
