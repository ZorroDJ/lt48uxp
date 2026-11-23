#include "../../NewDeviceLib/sources/addtab-remaining73/mx26_sop.h"
#include "../../NewDeviceLib/sources/addtab-remaining73/read_start.h"
#include "../../NewDeviceLib/sources/addtab-remaining73/admission.h"
#include "../../NewDeviceLib/sources/addtab-remaining73/erase_setup.h"
#include "../../NewDeviceLib/sources/addtab-remaining73/program_banks.h"
#include "../../NewDeviceLib/sources/addtab-remaining73/init_sequence.h"
#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Digest-bound AMD and Sharp/ST AddTab word flash; source-mapped regions. */
#include "addtab_regions.h"
#include "addtab_dl46_definitions.h"
#include "../../NewDeviceLib/sources/addtab-gl45/special_protect_guard.h"
#include "../../NewDeviceLib/sources/addtab-dl/macronix_offpackage_vid.h"
#include "../../NewDeviceLib/sources/addtab-ew/region_transitions.h"
#include "../../NewDeviceLib/sources/addtab-gln/region_transitions.h"
#include "../../NewDeviceLib/sources/addtab-micron27/region_transitions.h"
#include "../../NewDeviceLib/sources/addtab-tail-word/init_sequence.h"
#include "../../NewDeviceLib/sources/addtab-tail-word/region_transitions.h"
#include "../../NewDeviceLib/sources/addtab-tail-word/program_address.h"
#include "../../NewDeviceLib/sources/addtab-tail-word/sst_security_extent.h"
#include "../../NewDeviceLib/sources/addtab-tail-word/erase_status.h"
#include "../../NewDeviceLib/sources/addtab-tail-word/vid_readonly.h"
#include "addtab_flash_closures.inc"
#include "addtab_otp_closures.inc"
#include "addtab_byte8_closures.inc"
#include "../../NewDeviceLib/sources/addtab-amd-mx6/profiles.h"
#include "addtab_tail_word_profiles.inc"
#include "../../NewDeviceLib/sources/addtab-remaining73/profiles.h"
struct lt48_addtab_flash_resources {
    unsigned applet_count,otp,gl,page_bytes,dl46,tail,byte8,element_bytes,single_byte;
    unsigned char *idle;size_t idle_size;
    lt48_rez *rez;
    lt48_pin_map *pins;
    lt48_compiled_wave *wave[15];
    lt48_wave_code applet[9];
    lt48_applet_address addresses[9];
    const unsigned char *fpga;
    lt48_compile_variable vars[32];
    size_t var_count;
    uint32_t capacity;
    uint16_t man,dev,nominal;
    const struct af_closure *closure;
    const struct af_id_table *id_table;
};
void lt48_addtab_flash_close(lt48_addtab_flash_resources *r) {
    if(!r)return;
    for(unsigned i=0;i<15;i++)lt48_compiled_wave_close(r->wave[i]);
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r->idle);free(r);
}
unsigned lt48_addtab_flash_secure_kind(const lt48_addtab_flash_resources *r){return !r||!r->wave[11]||r->dl46||r->tail||r->byte8?0:r->otp?2:1;}
uint32_t lt48_addtab_flash_capacity(const lt48_addtab_flash_resources *r){return r?r->capacity:0;}
static lt48_status addtab_open(const lt48_installation *x,size_t device,lt48_addtab_flash_resources **out,int private_tail) {
    lt48_device_spec spec;lt48_family_info family;lt48_pin_code pc;
    lt48_addtab_flash_resources *r=NULL;lt48_status status;char path[4096],name[24],hex[65];
    size_t index,fi;unsigned char digest[32];
    if(!out||!x)return LT48_INVALID_ARGUMENT;
    *out=NULL;status=lt48_installation_resolve_device(x,device,&spec);if(status!=LT48_OK)return status;
    unsigned single_byte=private_tail==3;
    unsigned byte8=private_tail==2 || single_byte;
    if(byte8)private_tail=0;
    unsigned otp=spec.catalogue.device_class==2047;
    if(byte8 && spec.catalogue.device_class!=2044)return LT48_UNSUPPORTED;
    if(spec.catalogue.device_class!=2045 && !otp && !byte8)return LT48_UNSUPPORTED;
    /* Source-audited tail definitions; D31 remains private. */
    if(!private_tail && !otp && !byte8 && (spec.definition_resource==0xbd || spec.definition_resource==0xbe || spec.definition_resource==0xc6 || spec.definition_resource==0xc7 || spec.definition_resource==0x121 || spec.definition_resource==0x122 || spec.definition_resource==0xf0 || spec.definition_resource==0x57 || spec.definition_resource==0xac || spec.definition_resource==0xc2 || spec.definition_resource==0xe3 ||
       spec.definition_resource==0xcf || spec.definition_resource==0xd0 || spec.definition_resource==0xd1 || spec.definition_resource==0x117 || spec.definition_resource==0x128 ||
       spec.definition_resource==0x12a || spec.definition_resource==0x126 || spec.definition_resource==0x11b || spec.definition_resource==0x11c || spec.definition_resource==0xaf || spec.definition_resource==0x120 ||
       spec.definition_resource==0x13b || spec.definition_resource==0x13c || (spec.definition_resource>=0xeb && spec.definition_resource<=0xee) || spec.definition_resource==0xf1 || spec.definition_resource==0xf2 || spec.definition_resource==0xe9 || spec.definition_resource==0x11a))private_tail=1;
    if(!private_tail && !otp && !byte8 && addtab_remaining_admitted(spec.definition_resource))private_tail=4;
    const struct af_closure *closure=NULL;
    const struct af_closure *table=af_closures;
    size_t table_count=sizeof(af_closures)/sizeof(*af_closures);
    if(private_tail){table=tail_closures;table_count=sizeof(tail_closures)/sizeof(*tail_closures);}
    if(private_tail==4){table=r73_closures;table_count=sizeof(r73_closures)/sizeof(*r73_closures);}
    if(otp){table=otp_closures;table_count=sizeof(otp_closures)/sizeof(*otp_closures);}
    if(byte8){table=byte8_closures;table_count=sizeof(byte8_closures)/sizeof(*byte8_closures);}
    if(single_byte){table=amx6_closures;table_count=sizeof(amx6_closures)/sizeof(*amx6_closures);}
    for(size_t i=0;i<table_count;i++) {
        const struct af_closure *c=&table[i];
        if(spec.definition_resource==c->definition && spec.family_code==c->family &&
           spec.pinmap==c->pinmap && spec.pin_count==c->pins && spec.capacity_bytes==c->capacity && spec.element_bits==(byte8?8u:16u))closure=c;
    }
    if(!closure || (private_tail && otp))return LT48_UNSUPPORTED;
    const struct tail_shape *shape=NULL;
    for(size_t i=0;private_tail && i<sizeof(tail_shapes)/sizeof(*tail_shapes);i++)if(tail_shapes[i].definition==closure->definition)shape=&tail_shapes[i];
    if(private_tail==4){shape=NULL;for(size_t i=0;i<sizeof(r73_shapes)/sizeof(*r73_shapes);i++)if(r73_shapes[i].definition==closure->definition)shape=&r73_shapes[i];}
    if(private_tail && !shape)return LT48_UNSUPPORTED;
    int n=snprintf(path,sizeof(path),"%s/%s",lt48_installation_path(x),byte8?"GDCA___.rez":otp?"GDCBA__.rez":"GDCAA__.rez");
    if(n<0||(size_t)n>=sizeof(path))return LT48_INVALID_ARGUMENT;
    r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
#define TRY(a) do{status=(a);if(status!=LT48_OK){if(getenv("LABTOOL48_ADDTAB_TRACE"))fprintf(stderr,"addtab open line %d status %d\n",__LINE__,status);goto fail;}}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,digest));
    for(unsigned i=0;i<32;i++)snprintf(hex+2*i,3,"%02x",digest[i]);
    if(strcmp(hex,byte8?"5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39":otp?"84d3f2c830be58a084b5c763687d8a7c3a6217d440d4a96fb63fb2cf842a38fa":"2439a3725457dfc077c67ea7420701829f4d91d62155ffd070921c767d322ac8")){status=LT48_SAFETY_REJECTED;goto fail;}
    r->gl=!otp&&(closure->definition==0x114 || closure->definition==0x115 || closure->definition==0x127 || closure->definition==0x129);
    static const unsigned gl67_defs[]={0x10b,0x10c,0x55,0x5c,0x5d,0x5f,0x60,0x61,0x69,0x6d,0xaa,0xd4,0xd6,0xd8,0xda,0xdc,0x48,0x4d,0x56,0x5e,0x68,0x6a,0x6b,0x6c,0x9b,0xa2,0xab,0xd5,0xd7,0xd9,0xdb,0x136,0x139,0x13a};
    for(size_t i=0;!otp && i<sizeof(gl67_defs)/sizeof(*gl67_defs);i++)if(closure->definition==gl67_defs[i])r->gl=2;
    if(!otp && (closure->definition==0xb0 || closure->definition==0xb1 || closure->definition==0xb2 || closure->definition==0xb3 || closure->definition==0xb5 || closure->definition==0xb6 || closure->definition==0xb4 || closure->definition==0xe2 || closure->definition==0xe4 || closure->definition==0xe5 || closure->definition==0xe6))r->gl=3;
    static const unsigned gl57_defs[]={0x10f,0x110,0x111,0x112,0x113,0x118,0x119,0x11d,0x11e,0x11f,0x123,0x124,0x125};
    for(size_t i=0;!otp && i<sizeof(gl57_defs)/sizeof(*gl57_defs);i++)if(closure->definition==gl57_defs[i])r->gl=4;
    static const unsigned gl47_defs[]={0x49,0x4a,0x4b,0x4c,0x9c,0x9d,0x9e,0x9f,0x135,0x137,0x138,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x66,0x67,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xd2,0xd3};
    for(size_t i=0;!otp && i<sizeof(gl47_defs)/sizeof(*gl47_defs);i++)if(closure->definition==gl47_defs[i])r->gl=2;
    static const unsigned gln86_defs[]={0xb8,0xb9,0xbf,0xc0,0xdd,0xde,0xb7,0xcc,0xcd,0xce,0xba,0xbb,0xbc,0xc1,0xc3,0xc4,0xc5,0xc8,0xc9,0xca,0xdf,0xe0,0xe1};
    for(size_t i=0;!otp && i<sizeof(gln86_defs)/sizeof(*gln86_defs);i++)if(closure->definition==gln86_defs[i])r->gl=5;
    r->page_bytes=r->gl==5&&(closure->definition==0xb7||closure->definition==0xcc||closure->definition==0xcd||closure->definition==0xce)?64:(r->gl==4||r->gl==5)?32:r->gl==3?(closure->definition==0xb4?1024:512):r->gl==1?64:r->gl==2?32:0;
    r->dl46=!otp&&addtab_dl46_batch(closure->definition);r->otp=otp;r->applet_count=0;r->nominal=closure->vcc;r->closure=closure;
    r->tail=private_tail;r->byte8=byte8;r->element_bytes=byte8?1:2;
    if(byte8){r->gl=single_byte?0:2;r->page_bytes=single_byte?0:32;r->dl46=0;r->single_byte=single_byte;}
    if(shape){r->gl=shape->page_bytes?6:0;r->page_bytes=shape->page_bytes;}
    r->capacity=spec.capacity_bytes;r->man=spec.catalogue.manufacturer_id;r->dev=spec.catalogue.device_id;
    /* Host 00465f30 uses catalogue word +0x37 as an extended-ID table key.
     * Bind it to the audited definition as well as the table, never to zero IDs. */
    const struct af_id_binding *id_bindings=private_tail?tail_id_bindings:af_id_bindings;
    size_t binding_count=private_tail?sizeof(tail_id_bindings)/sizeof(*tail_id_bindings):sizeof(af_id_bindings)/sizeof(*af_id_bindings);
    const struct af_id_table *id_tables=private_tail?tail_ids:af_ids;
    size_t id_count=private_tail?sizeof(tail_ids)/sizeof(*tail_ids):sizeof(af_ids)/sizeof(*af_ids);
    if(private_tail==4){id_bindings=r73_id_bindings;binding_count=sizeof(r73_id_bindings)/sizeof(*r73_id_bindings);id_tables=r73_ids;id_count=sizeof(r73_ids)/sizeof(*r73_ids);}
    if(byte8){id_bindings=byte8_id_bindings;binding_count=sizeof(byte8_id_bindings)/sizeof(*byte8_id_bindings);id_tables=byte8_ids;id_count=sizeof(byte8_ids)/sizeof(*byte8_ids);}
    if(single_byte){binding_count=0;id_count=0;}
    unsigned needs_extended=0;
    for(size_t i=0;!otp && i<binding_count;i++)
        if(id_bindings[i].definition==closure->definition)needs_extended=1;
    if(needs_extended) {
        int bound=0;
        for(size_t i=0;i<binding_count;i++)
            if(id_bindings[i].definition==closure->definition && id_bindings[i].key==spec.catalogue.unknown6)bound=1;
        for(size_t i=0;bound && i<id_count;i++)
            if(id_tables[i].key==spec.catalogue.unknown6)r->id_table=&id_tables[i];
        if(!r->id_table){if(getenv("LABTOOL48_ADDTAB_TRACE"))fprintf(stderr,"unbound ID key %u definition %x private %d\n",spec.catalogue.unknown6,closure->definition,private_tail);status=private_tail?LT48_UNSUPPORTED:LT48_SAFETY_REJECTED;goto fail;}
    } else if(spec.catalogue.unknown6){status=LT48_SAFETY_REJECTED;goto fail;}
    snprintf(name,sizeof(name),"P%x",spec.pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    snprintf(name,sizeof(name),"M%x",spec.family_code);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    TRY(lt48_rez_get_family_info(r->rez,fi,&family));
    if(shape && (family.variable_count!=shape->variables || family.operation_count!=shape->operations)){status=LT48_FORMAT_ERROR;goto fail;}
    if(!shape) {
    if(family.variable_count>32 || family.variable_count<((otp||(r->dl46&&closure->definition==0x3d))?6u:(r->gl==3||r->gl==5)?8u:10u) || (!otp && (r->gl==5?family.operation_count!=((closure->definition==0xb7||closure->definition==0xdd||closure->definition==0xde||closure->definition==0xdf||closure->definition==0xe0||closure->definition==0xe1)?17u:20u):r->gl==4?family.operation_count!=((closure->definition==0x118||closure->definition==0x119)?20u:17u):r->gl==3?family.operation_count!=((closure->definition==0xe4||closure->definition==0xe6)?18u:17u):(r->gl==2||r->dl46)?(family.operation_count!=19u && family.operation_count!=23u && !(closure->definition==0xd6 && family.operation_count==21u)):family.operation_count!=(r->gl==1?17u:23u))) || (otp && (family.operation_count<16 || family.operation_count>19))){status=LT48_FORMAT_ERROR;goto fail;}
    }
    if(byte8 && (family.variable_count!=(single_byte?(closure->definition<=1?10u:11u):12u) || family.operation_count!=23)){status=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=family.variable_count;
    for(size_t i=0;i<r->var_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->vars[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
    }
    if((r->byte8 && !r->single_byte) || r->gl==3 || r->gl==4 || r->gl==5 || r->gl==6) {
        unsigned page_words=0;
        for(size_t i=0;i<r->var_count;i++)if(r->vars[i].variable_id==(r->byte8?12:19))page_words=r->vars[i].value;
        if(page_words*r->element_bytes!=r->page_bytes){status=LT48_FORMAT_ERROR;goto fail;}
    }
    if((r->nominal!=300 && r->nominal!=310 && r->nominal!=320 && r->nominal!=330) || r->vars[0].variable_id!=0 || r->vars[0].value!=r->nominal){status=LT48_FORMAT_ERROR;goto fail;}
    for(size_t i=0;i<family.operation_count;i++) {
        lt48_family_operation op;lt48_wave_code code;
        TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        if(private_tail && (closure->definition==0x120 || closure->definition==0x57 || closure->definition==0xac) && op.operation_id==20) {
            TRY(lt48_rez_find_resource(r->rez,"W9d",&index));
            TRY(lt48_rez_get_wave_code(r->rez,index,&code));
        }
        if(code.wave_type==0 && op.operation_id<15) {
            lt48_compact_wave *w=NULL;unsigned char *corrected=NULL;size_t code_size=code.data_size;
            if(r->gl==3 && ((closure->definition==0xb4 && (op.operation_id==5 || op.operation_id==7 || op.operation_id==8 || op.operation_id==9)) ||
                ((closure->definition==0xe4 || closure->definition==0xe6) && op.operation_id==3))) {
                corrected=ew_region_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(r->gl==3 && (closure->definition==0xb0 || closure->definition==0xb1 || closure->definition==0xb2 || closure->definition==0xb3 || closure->definition==0xb5 || closure->definition==0xb6) &&
                (op.operation_id==5 || op.operation_id==7 || op.operation_id==8 || op.operation_id==9)) {
                corrected=micron27_region_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(r->gl==5 && closure->definition!=0xdd && closure->definition!=0xde && closure->definition!=0xdf && closure->definition!=0xe0 && closure->definition!=0xe1 &&
                (op.operation_id==5 || op.operation_id==7 || op.operation_id==9)) {
                corrected=gln_region_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(private_tail && (closure->definition==0xc2 || closure->definition==0xcf ||
                closure->definition==0xd0 || closure->definition==0xd1 || closure->definition==0xac ||
                closure->definition==0xbd || closure->definition==0xbe || closure->definition==0xc6 || closure->definition==0xc7) &&
                (op.operation_id==5 || op.operation_id==7 || op.operation_id==9)) {
                corrected=tail_region_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(private_tail && (closure->definition==0xc2 || closure->definition==0xe3 ||
                closure->definition==0xcf || closure->definition==0xd0 || closure->definition==0xd1 ||
                closure->definition==0x13b || closure->definition==0x13c || closure->definition==0xac ||
                closure->definition==0xbd || closure->definition==0xbe || closure->definition==0xc6 || closure->definition==0xc7) && op.operation_id==2) {
                corrected=tail_word_init_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(private_tail && closure->definition==0x120 && op.operation_id==8) {
                corrected=tail_program_address_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(private_tail && (closure->definition==0x57 || closure->definition==0xac) && op.operation_id==4) {
                corrected=tail_vid_readonly_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(private_tail==4 && (closure->definition==0x74 || closure->definition==0x75 || closure->definition==0x3e || closure->definition==0x3f) && op.operation_id==2) {
                corrected=remaining73_init_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(private_tail==4 && closure->definition==0x80 && op.operation_id==10) {
                corrected=remaining73_erase_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            if(private_tail==4 && closure->definition==0xea && (op.operation_id==2 || op.operation_id==10 || op.operation_id==11)) {
                corrected=mx26_sop_wave(closure->definition,op.operation_id,code.data,code.data_size,&code_size);
                if(!corrected){status=LT48_SAFETY_REJECTED;goto fail;}
            }
            status=lt48_compact_decode(corrected?corrected:code.data,code_size,&w);free(corrected);
            if(status!=LT48_OK)goto fail;
            status=lt48_compact_compile_with_pin_policy(w,r->pins,r->vars,r->var_count,1,
                ((addtab_dl_vid_context(closure->definition,op.operation_id)&&r->dl46)||addtab_gl45_protect_context(closure->definition,op.operation_id))?LT48_PIN_FALLBACK_ANALYSIS_ALL:LT48_PIN_FALLBACK_RESIDENT_NOOP,&r->wave[op.operation_id]);
            if(status==LT48_OK && r->dl46 && addtab_dl_vid_context(closure->definition,op.operation_id) &&
               !addtab_dl_vid_defaults(r->wave[op.operation_id],closure->definition,op.operation_id))status=LT48_SAFETY_REJECTED;
            if(status==LT48_OK && addtab_gl45_protect_context(closure->definition,op.operation_id) &&
               !addtab_gl45_protect_defaults(r->wave[op.operation_id],closure->definition,op.operation_id))status=LT48_SAFETY_REJECTED;
            lt48_compact_wave_close(w);if(status!=LT48_OK){if(getenv("LABTOOL48_ADDTAB_TRACE"))fprintf(stderr,"compile op %u status %d\n",op.operation_id,status);goto fail;}
        } else if(code.wave_type==6) {
            unsigned slot=op.operation_id==17?0:op.operation_id==19?1:op.operation_id==20?2:op.operation_id==21?3:op.operation_id==16?4:op.operation_id==18?5:op.operation_id==23?6:op.operation_id==26?7:(private_tail && op.operation_id==24)?8:9;
            static const uint16_t addr[]={0x6000,0x7800,0x8000,0x7000,0x5000,0x5800,0x9000,0xa000,0x9800};
            if(r->applet_count>=(private_tail?9u:8u) || slot==9 || code.load_address!=addr[slot] || code.data_size==0 ||
               code.data_size>0x800 || code.length1!=code.data_size+4 || code.length2!=code.data_size){status=LT48_FORMAT_ERROR;goto fail;}
            r->applet[r->applet_count]=code;r->addresses[r->applet_count++]=(lt48_applet_address){op.operation_id,code.load_address};
        } else if(code.wave_type==4 && op.operation_id==30 && code.data_size==2780 &&
                  code.data[0]==0xda && code.data[1]==0x0a)r->fpga=code.data+2;
        else {status=LT48_UNSUPPORTED;goto fail;}
    }
    if(r->applet_count!=(shape?shape->applets:((r->gl==1||r->gl==3||r->gl==4||r->gl==5)?6u:otp?5u:8u))){status=LT48_FORMAT_ERROR;goto fail;}
    for(unsigned i=0;i<r->applet_count;i++)if(!r->applet[i].data){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->wave[0]||!r->wave[1]||!r->wave[2]||!r->wave[3]||!r->wave[4]||!r->wave[10]){status=LT48_FORMAT_ERROR;goto fail;}
    if(!r->fpga){status=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));
    *out=r;return LT48_OK;
fail:if(getenv("LABTOOL48_ADDTAB_TRACE"))fprintf(stderr,"open final status %d\n",status);lt48_addtab_flash_close(r);return status;
#undef TRY
}
lt48_status lt48_addtab_flash_open(const lt48_installation *x,size_t device,lt48_addtab_flash_resources **out) {
    lt48_device_spec spec;
    if(x && lt48_installation_resolve_device(x,device,&spec)==LT48_OK && spec.catalogue.device_class==2044)
        return addtab_open(x,device,out,(spec.definition_resource==0 || spec.definition_resource==1 || spec.definition_resource==4 || spec.definition_resource==5 || spec.definition_resource==9 || spec.definition_resource==10)?3:2);
    return addtab_open(x,device,out,0);
}
lt48_status lt48_addtab_remaining73_open_private(const lt48_installation *x,size_t device,lt48_addtab_flash_resources **out) {return addtab_open(x,device,out,4);}

lt48_status lt48_addtab_tail_word_open_internal(const lt48_installation *x,size_t device,lt48_addtab_flash_resources **out) {
    return addtab_open(x,device,out,1);
}

lt48_status lt48_addtab_byte8_open_internal(const lt48_installation *x,size_t device,lt48_addtab_flash_resources **out) {
    return addtab_open(x,device,out,2);
}

lt48_status lt48_addtab_amd_mx6_open_internal(const lt48_installation *x,size_t device,lt48_addtab_flash_resources **out) {
    return addtab_open(x,device,out,3);
}

lt48_status lt48_addtab_flash_validate(const lt48_addtab_flash_resources *r,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r||!length||start>=r->capacity||length>r->capacity-start)return LT48_SAFETY_REJECTED;
    if(op<LT48_OPERATION_READ||op>LT48_OPERATION_SECURE)return LT48_UNSUPPORTED;
    if(op==LT48_OPERATION_SECURE && !lt48_addtab_flash_secure_kind(r))return LT48_UNSUPPORTED;
    if(op!=LT48_OPERATION_READ&&op!=LT48_OPERATION_BLANK_CHECK&&op!=LT48_OPERATION_ERASE&&op!=LT48_OPERATION_SECURE&&(!image||size!=length))return LT48_INVALID_ARGUMENT;
    /* API addresses are bytes; source Address and stream counters use the bound element width. */
    if(start%r->element_bytes || length%r->element_bytes)return LT48_SAFETY_REJECTED;
    if((op==LT48_OPERATION_ERASE || op==LT48_OPERATION_SECURE) && (start!=0||length!=r->capacity))return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
lt48_status lt48_addtab_flash_validate_verify_options(const lt48_addtab_flash_resources *r,
    lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned margin_type) {
    if(!r||margin_type>1)return LT48_INVALID_ARGUMENT;
    if(margin!=LT48_VERIFY_MARGIN_ALL&&margin!=LT48_VERIFY_MARGIN_NOMINAL&&
       margin!=LT48_VERIFY_MARGIN_LOW&&margin!=LT48_VERIFY_MARGIN_HIGH)return LT48_INVALID_ARGUMENT;
    /* Only the 5% high margin is admitted for the 3.3 V flash definition.
     * The original 10% calculation asks for 3.63 V, outside our 3.6 V ceiling. */
    if(r->nominal==330 && margin_type &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||
        (op==LT48_OPERATION_VERIFY&&(margin==LT48_VERIFY_MARGIN_ALL||margin==LT48_VERIFY_MARGIN_HIGH))))
        return LT48_SAFETY_REJECTED;
    return LT48_OK;
}
static uint16_t margin_voltage(const lt48_addtab_flash_resources *r,unsigned type,int high) {
    /* Original x87 default rounding: nearest integer, ties to even. Both
     * 3.00 V, 3.20 V and 3.30 V definitions occur in these exact resource sets. */
    unsigned scaled=r->nominal*(high?(type?110:105):(type?90:95));
    unsigned q=scaled/100,rem=scaled%100;
    return (uint16_t)(q+(rem>50 || (rem==50 && (q&1))));
}

typedef struct af_session {
    const lt48_addtab_flash_resources *r;
    const lt48_protocol_transport *api;void *context;
    lt48_runtime_transport_session *wire;
    unsigned char buffers[32][32];uint32_t volts[4];
    lt48_compile_variable vars[32];uint32_t mode;
    lt48_runtime_configuration_value configuration;
    size_t configuration_count;
} af_session;
static lt48_status run(af_session *s,unsigned op) {
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0 && (s->vars[i].value<270 || s->vars[i].value>360))return LT48_SAFETY_REJECTED;
    lt48_runtime_options options={0};lt48_runtime_buffer_seed seeds[31];size_t count=0;
    lt48_runtime_machine *machine=NULL;lt48_runtime_result *result=NULL;
    for(uint8_t i=0;i<32;i++)if(i!=8)seeds[count++]=(lt48_runtime_buffer_seed){i,s->buffers[i],32};
    lt48_runtime_nested_wave protect={11,s->r->wave[11]};
    if(s->r->wave[11]){options.nested_waves=&protect;options.nested_wave_count=1;}
    options.mode=s->mode;options.use_explicit_buffer_ids=1;options.mode_buffer=8;
    options.address_buffer=0;options.parameter_buffer_id=3;options.initial_buffers=seeds;options.initial_buffer_count=count;
    options.configuration=&s->configuration;options.configuration_count=s->configuration_count;
    memcpy(options.initial_voltage_centivolts,s->volts,sizeof(s->volts));options.root_timeout_seconds=30;
    lt48_status status=lt48_runtime_transport_session_wave_watchdog(s->wire,30000,1);
    if(status==LT48_OK)status=lt48_runtime_machine_create(s->r->wave[op],s->r->pins,s->vars,s->r->var_count,&options,&machine);
    if(status==LT48_OK)status=lt48_runtime_transport_session_run_with_applets(s->wire,machine,s->r->addresses,s->r->applet_count,&result);
    if(status==LT48_OK) {
        for(uint8_t i=0;i<32;i++) {
            const unsigned char *data;size_t size;status=lt48_runtime_get_buffer(result,i,&data,&size);
            if(status!=LT48_OK||size!=32){status=LT48_RUNTIME_ERROR;break;}
            memcpy(s->buffers[i],data,32);
        }
        for(uint8_t i=0;status==LT48_OK&&i<4;i++)status=lt48_runtime_get_voltage(result,i,&s->volts[i]);
        if(status==LT48_OK && s->configuration_count) {
            const lt48_runtime_configuration_value *values;size_t n;
            status=lt48_runtime_get_configuration(result,&values,&n);
            if(status==LT48_OK && (n!=1 || values[0].category!=0 || values[0].field!=0))status=LT48_RUNTIME_ERROR;
            if(status==LT48_OK)s->configuration=values[0];
        }
        for(size_t i=0;i<lt48_runtime_event_count(result);i++) {
            lt48_runtime_event event;lt48_runtime_get_event(result,i,&event);
            if(event.kind==LT48_RUNTIME_RESULT_LATCH ||
               (event.kind==LT48_RUNTIME_RETURN_STATUS && event.argument_count==1 && event.arguments[0]==0))status=LT48_RUNTIME_ERROR;
        }
    }
    if(getenv("LABTOOL48_ADDTAB_TRACE"))fprintf(stderr,
        "addtab wave=%u mode=%u status=%d address=%02x%02x%02x%02x data=%02x retry=%u volts=%u,%u,%u,%u\n",
        op,s->mode,status,s->buffers[0][3],s->buffers[0][2],s->buffers[0][1],s->buffers[0][0],
        s->buffers[3][0],s->buffers[15][0],s->volts[0],s->volts[1],s->volts[2],s->volts[3]);
    lt48_runtime_result_close(result);lt48_runtime_machine_close(machine);return status;
}
static lt48_status transfer(af_session *s,int input,uint8_t endpoint,unsigned char *bytes,size_t length) {
    size_t done=0;
    if(s->api->cancelled&&s->api->cancelled(s->context))return LT48_CANCELLED;
    lt48_status status=input?s->api->read(s->context,endpoint,bytes,length,&done):s->api->write(s->context,endpoint,bytes,length,&done);
    if(status==LT48_OK&&done!=length)status=LT48_PROTOCOL_ERROR;
    return status;
}
static lt48_status cmd(af_session *s,const unsigned char *p,size_t n,unsigned char *reply) {
    unsigned char packet[512]={0};if(n>512)return LT48_INVALID_ARGUMENT;
    memcpy(packet,p,n);lt48_status status=lt48_runtime_transport_session_flush(s->wire);
    if(status==LT48_OK)status=transfer(s,0,2,packet,512);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status flag(af_session *s,unsigned opcode,unsigned *value) {
    unsigned char p[2]={(unsigned char)opcode,1},reply[512]={0};
    lt48_status status=cmd(s,p,opcode==0x0b?1:2,reply);
    if(status==LT48_OK && !(reply[0]|reply[1]))status=LT48_PROTOCOL_ERROR;
    if(status==LT48_OK)*value=reply[2]&1;
    return status;
}
static lt48_status block(af_session *s,unsigned opcode,unsigned char *data,size_t n,unsigned char *reply) {
    unsigned char p[6]={(unsigned char)opcode,(unsigned char)(n/512),(unsigned char)(n/512>>8),
        (unsigned char)(n%512),(unsigned char)(n%512>>8),opcode==0x48?s->r->element_bytes:0};
    lt48_status status=cmd(s,p,6,NULL);
    if(status==LT48_OK)status=transfer(s,opcode==0x43,opcode==0x43?0x88:4,data,n);
    if(status==LT48_OK&&reply)status=transfer(s,1,0x88,reply,512);
    return status;
}
static lt48_status setup(af_session *s) {
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
static lt48_status stream(af_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_addtab_flash_result *out) {
    (void)vcc;
    lt48_status status=LT48_OK;size_t chunk=length/100;chunk-=chunk%s->r->element_bytes;if(!chunk)chunk=s->r->element_bytes;if(chunk>8192)chunk=8192;
    if(s->r->gl && mode==2 && start<s->r->closure->main_size) {
        /* Original 8000 confirms only after a full source-sized group. USB blocks may be
         * smaller, but each completed program range must contain full pages. */
        if((start|length)&(s->r->page_bytes-1))return LT48_SAFETY_REJECTED;
        chunk-=chunk%s->r->page_bytes;if(!chunk)chunk=s->r->page_bytes;
    }
    out->completed_bytes=0;
    for(size_t offset=0;status==LT48_OK&&offset<length;) {
        size_t n=length-offset;if(n>chunk)n=chunk;
        status=lt48_runtime_transport_session_wave_watchdog(s->wire,mode==1?5000:mode==11?10000:60000,0);
        if(status!=LT48_OK)break;
        if(mode==1)status=block(s,0x43,read+offset,n,NULL);
        else if(mode==11) {
            unsigned char count[2]={(unsigned char)(n/s->r->element_bytes),(unsigned char)(n/s->r->element_bytes>>8)};unsigned passed=0;
            status=block(s,0x44,count,2,NULL);
            if(status==LT48_OK)status=flag(s,0x0b,&passed);
            if(status==LT48_OK&&!passed) {
                unsigned char remaining[2]={0};status=block(s,0x43,remaining,2,NULL);
                size_t rem=remaining[0]|(size_t)remaining[1]<<8;
                if(status==LT48_OK) {
                    if(rem>=n/s->r->element_bytes)status=LT48_PROTOCOL_ERROR;
                    else {out->first_mismatch=start+(uint32_t)(offset+n-s->r->element_bytes*(rem+1));out->mismatch_count++;status=LT48_DEVICE_MISMATCH;}
                }
            }
        } else {
            unsigned char reply[512]={0};status=block(s,0x48,(unsigned char*)image+offset,n,reply);
            if(status==LT48_OK&&!(reply[0]|reply[1])) {
                /* The original derives an unchecked remaining count here.
                 * Never resume programming from untrusted failure counters. */
                /* Original error callback 004689e0 drains one actual word
                 * through transport +24 (00496850) before stopping the applet. */
                unsigned char actual[2];
                status=block(s,0x43,actual,s->r->element_bytes,NULL);
                out->mismatch_count++;out->first_mismatch=UINT32_MAX;
                if(status==LT48_OK)status=LT48_DEVICE_MISMATCH;
            }
            if(status==LT48_OK&&offset+n==length) {
                unsigned passed=0;status=flag(s,0x0c,&passed);
                if(status==LT48_OK&&!passed) {
                    unsigned char actual[2];
                    status=block(s,0x43,actual,s->r->element_bytes,NULL);
                    out->mismatch_count++;
                    out->first_mismatch=start+(uint32_t)(length-s->r->element_bytes);
                    if(status==LT48_OK) {
                        if(s->r->element_bytes==2 && actual[0]==image[length-2])out->first_mismatch++;
                        status=LT48_DEVICE_MISMATCH;
                    }
                }
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
    return status;
}
static void set32(unsigned char *b,uint32_t value) {
    for(unsigned j=0;j<4;j++)b[j]=(unsigned char)(value>>(8*j));
}
static int region_chunk(const lt48_addtab_flash_resources *r,uint32_t start,
    uint32_t length,uint32_t *physical,uint32_t *chunk) {
    const struct af_closure *c=r->closure;
    /* Older OTP closures retain the same two-region representation. */
    struct lt48_addtab_region legacy[2]={{0,c->main_size},
        {c->second_address,r->capacity>c->main_size?r->capacity-c->main_size:0}};
    return lt48_addtab_region_chunk(c->regions?c->regions:legacy,
        c->regions?c->region_count:(r->capacity>c->main_size?2u:1u),
        r->capacity,start,length,physical,chunk) &&
        tail_sst_region_address(r->tail,c->definition,c->main_size,start,physical);
}
static lt48_status cycle(af_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *read,uint16_t vcc,lt48_addtab_flash_result *out) {
    s->mode=mode;
    for(size_t i=0;i<s->r->var_count;i++)if(s->vars[i].variable_id==0)s->vars[i].value=vcc;
    set32(s->buffers[2],s->r->capacity/s->r->element_bytes);
    uint32_t first,first_chunk;
    if(length>UINT32_MAX || !region_chunk(s->r,start,(uint32_t)length,&first,&first_chunk))return LT48_SAFETY_REJECTED;
    first=remaining73_initial_address(s->r->tail,s->r->closure->definition,mode,start,first);
    set32(s->buffers[0],first/s->r->element_bytes);
    lt48_status status=run(s,2);
    size_t done=0;
    while(status==LT48_OK && done<length) {
        uint32_t logical=start+(uint32_t)done,physical,n;
        if(!region_chunk(s->r,logical,(uint32_t)(length-done),&physical,&n))return LT48_SAFETY_REJECTED;
        n=remaining73_program_chunk(s->r->tail,s->r->closure->definition,mode,logical,n);
        set32(s->buffers[0],physical/s->r->element_bytes);
        /* Byte-source READ/VERIFY/BLANK enter SecSi when the buffer pointer
         * equals its region boundary. A partial native range can begin inside
         * it: retain the physical offset, but supply that region selector. */
        set32(s->buffers[1],s->r->byte8 && logical>=s->r->closure->main_size?s->r->closure->main_size:logical);
        set32(s->buffers[9],1);
        status=run(s,mode==1?5:mode==11?7:mode==2?8:9);
        if(status==LT48_OK)status=stream(s,mode,logical,n,image?image+done:NULL,read?read+done:NULL,vcc,out);
        /* D80's applet can advance the selected die before its final poll.
         * Verify the completed first-bank range before programming bank two;
         * a success flag from that transition alone is not sufficient. */
        if(status==LT48_OK && s->r->tail && s->r->closure->definition==0x80 &&
           mode==2 && logical+n==0x800000u && done+n<length) {
            unsigned char *observed=malloc(n);
            if(!observed)status=LT48_NO_MEMORY;
            else {
                status=cycle(s,1,logical,n,NULL,observed,vcc,out);
                if(status==LT48_OK)for(uint32_t j=0;j<n;j++)if(observed[j]!=image[done+j]) {
                    if(!out->mismatch_count)out->first_mismatch=logical+j;
                    out->mismatch_count++;status=LT48_DEVICE_MISMATCH;
                }
                free(observed);
            }
            s->mode=mode;
            if(status==LT48_OK) {
                set32(s->buffers[0],(physical+n)/s->r->element_bytes);
                status=run(s,2);
            }
        }
        if(status==LT48_OK)done+=n;
    }
    if(status==LT48_OK)status=run(s,3);
    out->completed_bytes=done;
    return status;
}
/* Flash validation must observe actual bytes, including the final word. A
 * controller success bit alone was insufficient on a disconnected adapter. */
static lt48_status checked_cycle(af_session *s,unsigned mode,uint32_t start,size_t length,
    const unsigned char *image,unsigned char *pending,uint16_t vcc,lt48_addtab_flash_result *out) {
    lt48_status status=cycle(s,mode,start,length,image,NULL,vcc,out);
    if(status==LT48_OK) {
        status=cycle(s,1,start,length,NULL,pending,vcc,out);
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=image[i]) {
            if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    }
    return status;
}
static lt48_status identify(af_session *s,lt48_m37_id_decision_fn decide,void *context) {
    if(!s->r->wave[4])return LT48_OK;
    for(unsigned attempt=0;attempt<64;attempt++) {
        unsigned man=0,dev=0,expected_man=s->r->man,expected_dev=s->r->dev;
        int match=1;
        if(s->r->id_table) {
            for(unsigned i=0;i<s->r->id_table->count;i++) {
                set32(s->buffers[0],s->r->id_table->addresses[i]);memset(s->buffers[5],0,32);
                lt48_status status=run(s,4);if(status!=LT48_OK)return status;
                unsigned value=s->buffers[5][0]|(unsigned)s->buffers[5][1]<<8;
                if(i==0)man=value;
                dev=value;expected_man=s->r->id_table->values[0];expected_dev=s->r->id_table->values[i];
                if(value!=expected_dev){match=0;break;}
            }
            /* Each following operation supplies its own Address, but also
             * restore the normal initial value on Retry/Ignore/Abort. */
            set32(s->buffers[0],0);
        } else {
            memset(s->buffers[5],0,32);memset(s->buffers[7],0,32);
            lt48_status status=run(s,4);if(status!=LT48_OK)return status;
            man=s->buffers[7][0]|(unsigned)s->buffers[7][1]<<8;
            dev=s->buffers[5][0]|(unsigned)s->buffers[5][1]<<8;
            match=(s->r->man||s->r->dev)?man==s->r->man&&dev==s->r->dev:
                (man==dev && (man==0 || man==255));
        }
        if(match)return LT48_OK;
        lt48_m37_id_choice choice=decide?decide(context,expected_man,expected_dev,man,dev):LT48_M37_ID_ABORT;
        if(choice==LT48_M37_ID_IGNORE)return LT48_OK;
        if(choice!=LT48_M37_ID_RETRY)return LT48_DEVICE_MISMATCH;
    }
    return LT48_SAFETY_REJECTED;
}
/* Source secondary tables include special-region status/lock fields.
 * Class 2045 Secure is main-array only; class 2047 explicitly exposes the
 * original permanent locks and is identified as such to the operator. */
static lt48_status protection(af_session *s,int secure) {
    /* M256 tail admission deliberately excludes legacy high-voltage sector
     * protection. Erase must verify success without attempting unprotect. */
    if(!secure && tail_vid_defer_unprotect(s->r->tail,s->r->closure->definition))return LT48_OK;
    /* Original shared XP host dispatch checks optional routine presence. */
    if(!secure && !s->r->wave[13])return LT48_OK;
    if(secure && !s->r->wave[11])return LT48_UNSUPPORTED;
    if(s->r->otp) {
        /* 2047 has no operation 12. Its optional operation 13 is a
         * whole-array unlock guarded by the source's buffer 12 latch.
         * Never substitute the OTP/array protect wave for that operation. */
        if(secure) {
            if(!s->r->wave[11] || !s->r->wave[6])return LT48_UNSUPPORTED;
            /* This explicit Secure action includes the original permanent
             * lock fields. GUI admission must advertise that permanence. */
            if(s->r->closure->definition==8){
                s->configuration=(lt48_runtime_configuration_value){-1,0,0,1};s->configuration_count=1;
            }
            s->mode=13;set32(s->buffers[0],0);set32(s->buffers[2],s->r->capacity/s->r->element_bytes);
            lt48_status st=run(s,2);
            for(size_t i=0;st==LT48_OK && i<s->r->closure->protect_count;i++) {
                set32(s->buffers[0],s->r->closure->protect[i]/s->r->element_bytes);set32(s->buffers[4],1);
                st=run(s,11);if(st==LT48_OK && !s->buffers[3][0])st=LT48_DEVICE_MISMATCH;
            }
            if(st==LT48_OK)st=run(s,3);
            s->mode=12;set32(s->buffers[0],0);
            if(st==LT48_OK)st=run(s,2);
            for(size_t i=0;st==LT48_OK && i<s->r->closure->protect_count;i++) {
                set32(s->buffers[0],s->r->closure->protect[i]/s->r->element_bytes);st=run(s,6);
                /* 004663d0 treats any nonzero DataIn byte as checked. */
                if(st==LT48_OK && !s->buffers[3][0])st=LT48_DEVICE_MISMATCH;
            }
            if(st==LT48_OK && s->configuration_count && s->configuration.value!=1)st=LT48_DEVICE_MISMATCH;
            if(st==LT48_OK)st=run(s,3);
            return st;
        }
        if(!s->r->wave[13])return LT48_OK;
        s->mode=14;set32(s->buffers[0],0);set32(s->buffers[2],s->r->capacity/s->r->element_bytes);
        lt48_status st=run(s,2);
        if(st==LT48_OK)st=run(s,13);
        if(st==LT48_OK && s->buffers[3][0]!=1)st=LT48_DEVICE_MISMATCH;
        if(st==LT48_OK)st=run(s,3);
        return st;
    }
    memset(s->buffers[0],0,32);s->mode=secure?13:14;
    set32(s->buffers[2],s->r->capacity/s->r->element_bytes);
    lt48_status status=run(s,2);
    for(unsigned phase=0;status==LT48_OK && phase<(secure?1u:2u);phase++) {
        unsigned op=secure?11:phase==0?12:13;
        if(!s->r->wave[op])continue;
        for(size_t i=0;status==LT48_OK && i<s->r->closure->protect_count;i++) {
            uint32_t byte=s->r->closure->protect[i];
            /* Original host traverses both per-die SecSi fields for D31 too.
             * Evidence: addtab-exact23-next/bank-host.json (98 fields). */
            if(byte>=s->r->closure->main_size &&
               !((s->r->dl46 && (s->r->closure->definition==0x25 || s->r->closure->definition==0x36)) ||
                 (s->r->tail && s->r->closure->definition==0x31)))break;
            set32(s->buffers[0],byte/s->r->element_bytes);s->buffers[4][0]=1;
            status=run(s,op);
            if(status==LT48_OK && !s->buffers[3][0])status=LT48_DEVICE_MISMATCH;
        }
    }
    if(status==LT48_OK)status=run(s,3);
    memset(s->buffers[0],0,32);s->mode=0;set32(s->buffers[2],s->r->capacity/s->r->element_bytes);
    if(status==LT48_OK)status=run(s,2);
    for(size_t i=0;status==LT48_OK && i<s->r->closure->protect_count;i++) {
        uint32_t byte=s->r->closure->protect[i];if(byte>=s->r->closure->main_size)break;
        set32(s->buffers[0],byte/s->r->element_bytes);status=run(s,6);
        if(status==LT48_OK && s->buffers[3][0]!=(secure?1:0)){if(getenv("LABTOOL48_ADDTAB_TRACE"))fprintf(stderr,"protection at %x got %u expected %u\n",byte,s->buffers[3][0],secure?1:0);status=LT48_DEVICE_MISMATCH;}
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}
/* Original AddTab vtable +A8 (00467e00): mode 15, then UNLOCK at
 * primary sector boundaries. Program/Over-program/Erase call it before writes.
 * Keep the source tail boundary: the host walks to capacity, not EndErase. */
static lt48_status unlock(af_session *s) {
    if(!s->r->wave[14])return LT48_OK;
    s->mode=15;set32(s->buffers[0],0);set32(s->buffers[2],s->r->capacity/s->r->element_bytes);
    lt48_status status=run(s,2);
    for(size_t i=0;status==LT48_OK && i<s->r->closure->erase_count;i++) {
        uint32_t address=s->r->closure->erase[i];if(address>=s->r->capacity)break;
        set32(s->buffers[0],address/s->r->element_bytes);status=run(s,14);
        if(status==LT48_OK && s->buffers[3][0]!=1)status=LT48_DEVICE_MISMATCH;
    }
    if(status==LT48_OK)status=run(s,3);
    return status;
}
lt48_status lt48_addtab_flash_execute(const lt48_addtab_flash_resources *r,
    const lt48_protocol_transport *api,void *context,lt48_operation_kind op,uint32_t start,
    size_t length,const unsigned char *image,size_t size,unsigned char *read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned margin_type,lt48_m37_id_decision_fn decide,void *decision_context,
    lt48_addtab_flash_result *out) {
    if(!out||!api||!api->write||!api->read)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status status=lt48_addtab_flash_validate(r,op,start,length,image,size);
    if(status!=LT48_OK)return status;
    if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    status=lt48_addtab_flash_validate_verify_options(r,op,margin,margin_type);
    if(status!=LT48_OK)return status;
    /* The GL applet has no partial-page flush. Preserve neighbouring words
     * by reading the boundary pages and programming complete source-sized groups.
     * The separate engineer region uses its original word-program route.
     * This is native range handling, not an inferred Windows padding policy. */
    if(r->gl && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM) &&
       start<r->closure->main_size &&
       (start%r->page_bytes || (start+length<r->closure->main_size && (start+length)%r->page_bytes))) {
        uint32_t first=start&~(uint32_t)(r->page_bytes-1);
        size_t end=start+length;
        if(end<r->closure->main_size)end=(end+r->page_bytes-1)&~(size_t)(r->page_bytes-1);
        size_t expanded=end-first,offset=start-first;
        unsigned char *merged=malloc(expanded);if(!merged)return LT48_NO_MEMORY;
        if(op==LT48_OPERATION_PROGRAM)
            status=lt48_addtab_flash_execute(r,api,context,LT48_OPERATION_BLANK_CHECK,start,length,
                NULL,0,NULL,0,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,out);
        if(status==LT48_OK)
            status=lt48_addtab_flash_execute(r,api,context,LT48_OPERATION_READ,first,expanded,
                NULL,0,merged,expanded,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,out);
        if(status==LT48_OK) {
            memcpy(merged+offset,image,length);
            status=lt48_addtab_flash_execute(r,api,context,LT48_OPERATION_OVER_PROGRAM,first,expanded,
                merged,expanded,NULL,0,margin,margin_type,decide,decision_context,out);
        }
        free(merged);
        out->operation_status=status;out->completed_bytes=status==LT48_OK?length:0;
        return status;
    }
    uint32_t erase_end=tail_sst_erase_end(r->tail,r->closure->definition,r->closure->main_size,r->closure->erase_end);
    unsigned char *pending=malloc(length),*preserved=NULL;if(!pending)return LT48_NO_MEMORY;
    if(op==LT48_OPERATION_SECURE || (op==LT48_OPERATION_ERASE && erase_end<r->capacity)) {
        preserved=malloc(length);if(!preserved){free(pending);return LT48_NO_MEMORY;}
        lt48_addtab_flash_result before;
        /* Erase preserves only bytes outside its audited extent. Secure
         * still preserves the complete image. The final fresh read below
         * independently verifies the full requested range in both cases. */
        size_t preserve_offset=op==LT48_OPERATION_ERASE?erase_end:0;
        size_t preserve_length=length-preserve_offset;
        status=lt48_addtab_flash_execute(r,api,context,LT48_OPERATION_READ,
            start+(uint32_t)preserve_offset,preserve_length,NULL,0,
            preserved+preserve_offset,preserve_length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&before);
        if(status!=LT48_OK){*out=before;free(preserved);free(pending);return status;}
    }
    af_session s={0};s.r=r;s.api=api;s.context=context;s.mode=1;memcpy(s.vars,r->vars,sizeof(s.vars));
    set32(s.buffers[2],r->capacity/r->element_bytes);
    status=lt48_runtime_transport_session_create(api,context,&s.wire);
    if(status!=LT48_OK){free(preserved);free(pending);return status;}
    status=setup(&s);
    if(status==LT48_OK)status=identify(&s,decide,decision_context);
    if(status==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM)) {
        status=cycle(&s,op==LT48_OPERATION_PROGRAM?11:1,start,length,NULL,pending,r->nominal,out);
        if(status==LT48_OK&&op==LT48_OPERATION_OVER_PROGRAM)
            for(size_t i=0;i<length;i++)if((pending[i]&image[i])!=image[i]) {
                out->first_mismatch=start+(uint32_t)i;out->mismatch_count++;status=LT48_DEVICE_MISMATCH;break;
            }
    }
    if(status==LT48_OK && (op==LT48_OPERATION_PROGRAM || op==LT48_OPERATION_OVER_PROGRAM || op==LT48_OPERATION_ERASE))status=unlock(&s);
    if(status==LT48_OK && op==LT48_OPERATION_ERASE) {
        status=protection(&s,0);
        s.mode=7;memset(s.buffers[0],0,32);set32(s.buffers[2],r->capacity/r->element_bytes);
        if(status==LT48_OK)status=run(&s,2);
        for(size_t i=0;status==LT48_OK && i<r->closure->erase_count;i++) {
            uint32_t address=r->closure->erase[i];if(address>=erase_end)break;
            set32(s.buffers[0],address/r->element_bytes);status=run(&s,10);
            /* Fujitsu direct-sector erase retains DQ7 (80), unlike the
             * shared erase applet which normalizes success to 01. */
            unsigned success=r->dl46&&(r->closure->definition==0x1b||r->closure->definition==0x3d)?0x80u:1u;
            success=tail_erase_success(r->tail,r->closure->definition,success);
            success=mx26_sop_erase_success(r->tail,r->closure->definition,success);
            if(status==LT48_OK && s.buffers[3][0]!=success)status=LT48_DEVICE_MISMATCH;
        }
        if(status==LT48_OK)status=run(&s,3);
        if(status==LT48_OK)status=cycle(&s,1,0,erase_end,NULL,pending,r->nominal,out);
        if(status==LT48_OK)for(size_t i=0;i<erase_end;i++)if(pending[i]!=255) {
            if(!out->mismatch_count)out->first_mismatch=(uint32_t)i;
            out->mismatch_count++;
        }
        if(status==LT48_OK && out->mismatch_count)status=LT48_DEVICE_MISMATCH;
    } else if(status==LT48_OK && op==LT48_OPERATION_SECURE) {
        status=protection(&s,1);
    } else if(status==LT48_OK) {
        if(op==LT48_OPERATION_VERIFY) {
            uint16_t volts[2]={margin_voltage(r,margin_type,0),margin_voltage(r,margin_type,1)};size_t n=2;
            if(margin==LT48_VERIFY_MARGIN_NOMINAL){volts[0]=r->nominal;n=1;}
            if(margin==LT48_VERIFY_MARGIN_LOW)n=1;
            if(margin==LT48_VERIFY_MARGIN_HIGH){volts[0]=volts[1];n=1;}
            for(size_t i=0;status==LT48_OK&&i<n;i++)status=checked_cycle(&s,4,start,length,image,pending,volts[i],out);
        } else {
            unsigned mode=op==LT48_OPERATION_READ?1:op==LT48_OPERATION_BLANK_CHECK?11:op==LT48_OPERATION_COMPARE?5:2;
            if(op==LT48_OPERATION_COMPARE)status=checked_cycle(&s,mode,start,length,image,pending,r->nominal,out);
            else status=cycle(&s,mode,start,length,image,op==LT48_OPERATION_READ?pending:NULL,r->nominal,out);
        }
    }
    if(status==LT48_OK&&(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM))
        for(unsigned i=0;status==LT48_OK&&i<2;i++)status=checked_cycle(&s,4,start,length,image,pending,margin_voltage(r,margin_type,i),out);
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
            if(step!=LT48_OK){out->cleanup_status=step;out->cleanup_completed=0;}
        }
    }
    if(status==LT48_OK)status=out->cleanup_status;
    if(status==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,pending,length);
    lt48_runtime_transport_session_close(s.wire);s.wire=NULL;
    if(status==LT48_OK &&
       (op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||
        op==LT48_OPERATION_VERIFY||op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_ERASE||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_SECURE)) {
        /* Fresh power/setup/read after cleanup also rejects a floating bus
         * echoing the last written byte during same-session verification. */
        lt48_addtab_flash_result fresh;
        status=lt48_addtab_flash_execute(r,api,context,LT48_OPERATION_READ,start,length,
            NULL,0,pending,length,LT48_VERIFY_MARGIN_NOMINAL,0,decide,decision_context,&fresh);
        out->cleanup_status=fresh.cleanup_status;out->cleanup_completed=fresh.cleanup_completed;
        if(status==LT48_OK)for(size_t i=0;i<length;i++)if(pending[i]!=(op==LT48_OPERATION_ERASE?(i<erase_end?255:preserved[i]):op==LT48_OPERATION_BLANK_CHECK?255:op==LT48_OPERATION_SECURE?preserved[i]:image[i])) {
            if(!out->mismatch_count)out->first_mismatch=start+(uint32_t)i;
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
    free(preserved);free(pending);return status;
}
