#include "source_resources.h"
#include "otp14_word.h"
#include "pic17_pm2.h"
#include "pic18c_reset_alignment.h"
#include "pic75_entry_candidate.h"
#include "../../../c/src/internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct lt48_pic_source_resources {
    unsigned char digest[LT48_SHA256_SIZE];uint16_t family,pinmap;
    lt48_rez *rez;lt48_pin_map *pins;lt48_compiled_wave *waves[256];
    lt48_pic_source_session *session;lt48_runtime_frame_profile frame;
    lt48_pic_source_operation_contract operation;lt48_pic_source_host_binding host;
    lt48_compile_variable variables[256];size_t variable_count;
    lt48_pic_source_resource_entry entries[256];size_t entry_count;
    unsigned char *owned_payloads[256];
};
void lt48_pic_source_resources_close_internal(lt48_pic_source_resources *r) {
    if(!r)return;
    lt48_pic_source_session_close_internal(r->session);
    for(unsigned i=0;i<256;i++) {
        lt48_compiled_wave_close(r->waves[i]);free(r->owned_payloads[i]);
    }
    lt48_pin_map_close(r->pins);lt48_rez_close(r->rez);free(r);
}
lt48_pic_source_session *lt48_pic_source_resources_session_internal(lt48_pic_source_resources *r) {
    return r?r->session:NULL;
}
lt48_status lt48_pic_source_resources_frame_internal(const lt48_pic_source_resources *r,
    lt48_runtime_frame_profile *out) {
    if(!r||!out)return LT48_INVALID_ARGUMENT;
    *out=r->frame;return LT48_OK;
}
lt48_status lt48_pic_source_resources_contact_masks_internal(const lt48_pic_source_resources *r,
    lt48_pic_contact_masks *out) {
    if(!r||!out)return LT48_INVALID_ARGUMENT;
    char name[24];size_t index;lt48_pin_code code;
    snprintf(name,sizeof(name),"P%x",r->pinmap);
    lt48_status status=lt48_rez_find_resource(r->rez,name,&index);
    if(status==LT48_OK)status=lt48_rez_get_pin_code(r->rez,index,&code);
    if(status==LT48_OK)status=lt48_pic_source_contact_masks(code.pin_count,code.data,code.data_size,out);
    return status;
}
/* Host-independent immutable source table. Applet load order/overlap and FPGA
 * framing remain explicit strategy contracts; construction performs no upload. */
static lt48_status source_resources_library(const char *path,
    const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
    int older16_context,lt48_pic_source_resources **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;if(!path||!expected)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    lt48_status status;unsigned char actual[LT48_SHA256_SIZE];char name[24];size_t index,fi;
#define TRY(x) do{status=(x);if(status!=LT48_OK)goto fail;}while(0)
    TRY(lt48_rez_open(path,&r->rez));TRY(lt48_rez_sha256(r->rez,actual));
    if(memcmp(actual,expected,sizeof(actual))){status=LT48_SAFETY_REJECTED;goto fail;}
    if(older16_context) {
        static const unsigned char source_hash[32]={0x24,0xb8,0x32,0x0d,0xa4,0xcf,0xd7,0x00,0xee,0x43,0x6f,0xf0,0x6b,0xd5,0x5d,0x9d,
            0xfb,0xd4,0x72,0xc5,0x3a,0x02,0x1a,0xd5,0x27,0xc9,0xbb,0xcb,0x3c,0x80,0x4d,0xec};
        unsigned selected=family==27||family==29||family==32||family==35?2:
            family==28||family==30||family==33||family==36?3:family==31||family==34?4:5;
        if(memcmp(actual,source_hash,32)||family<27||family>39||pinmap!=selected){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    memcpy(r->digest,actual,sizeof(actual));r->family=family;r->pinmap=pinmap;
    snprintf(name,sizeof(name),"P%x",pinmap);TRY(lt48_rez_find_resource(r->rez,name,&index));
    lt48_pin_code pc;TRY(lt48_rez_get_pin_code(r->rez,index,&pc));TRY(lt48_pin_map_parse(&pc,&r->pins));
    if(older16_context&&family<=36) {
        /* Original Kernel allocates zero-filled records for all36 groups.
         * P2/P3/P4 leave ADDRESS10 empty because PIC17 multiplexes that bus.
         * Reconstruct only this proved empty record; generic maps stay strict.
         * Proof: older16-20-import/empty-address-group-x86.json. */
        lt48_pin_group absent;
        if(lt48_pin_map_find_group(r->pins,10,&absent)!=LT48_NOT_FOUND||pc.data[0]==255||pc.data_size>SIZE_MAX-4) {
            status=LT48_SAFETY_REJECTED;goto fail;
        }
        size_t position=3;
        for(unsigned i=0;i<pc.data[0];i++) {
            if(position>pc.data_size||pc.data_size-position<4){status=LT48_FORMAT_ERROR;goto fail;}
            size_t count=(size_t)pc.data[position+2]|(size_t)pc.data[position+3]<<8;
            if(count>pc.data_size-position-4){status=LT48_FORMAT_ERROR;goto fail;}position+=4+count;
        }
        unsigned char *complete=malloc(pc.data_size+4);if(!complete){status=LT48_NO_MEMORY;goto fail;}
        memcpy(complete,pc.data,position);complete[0]++;
        complete[position]=10;memset(complete+position+1,0,3);
        memcpy(complete+position+4,pc.data+position,pc.data_size-position);
        lt48_pin_code full=pc;full.data=complete;full.data_size+=4;
        lt48_pin_map_close(r->pins);r->pins=NULL;status=lt48_pin_map_parse(&full,&r->pins);free(complete);
        if(status!=LT48_OK)goto fail;
    }
    snprintf(name,sizeof(name),"M%x",family);TRY(lt48_rez_find_resource(r->rez,name,&fi));
    lt48_family_info info;TRY(lt48_rez_get_family_info(r->rez,fi,&info));
    if(!info.operation_count||info.operation_count>256||info.variable_count>256){status=LT48_UNSUPPORTED;goto fail;}
    TRY(lt48_rez_get_family_frame_profile(r->rez,fi,&r->frame));
    r->variable_count=info.variable_count;
    for(size_t i=0;i<info.variable_count;i++) {
        lt48_family_variable v;TRY(lt48_rez_get_family_variable(r->rez,fi,i,&v));
        r->variables[i]=(lt48_compile_variable){v.values[0],v.values[1],v.values[2],v.values[3],v.values[4]};
        for(size_t j=0;j<i;j++)if(r->variables[j].variable_id==r->variables[i].variable_id){status=LT48_FORMAT_ERROR;goto fail;}
    }
    unsigned seen[256]={0};
    for(size_t i=0;i<info.operation_count;i++) {
        lt48_family_operation op;TRY(lt48_rez_get_family_operation(r->rez,fi,i,&op));
        if(op.operation_id>=256||seen[op.operation_id]++){status=LT48_FORMAT_ERROR;goto fail;}
        snprintf(name,sizeof(name),"W%x",op.wave);TRY(lt48_rez_find_resource(r->rez,name,&index));
        lt48_wave_code w;TRY(lt48_rez_get_wave_code(r->rez,index,&w));
        if(!w.data||!w.data_size){status=LT48_FORMAT_ERROR;goto fail;}
        if(w.wave_type==0) {
            lt48_compact_wave *compact=NULL;TRY(lt48_compact_decode(w.data,w.data_size,&compact));
            int exception=older16_context&&family>=37&&op.operation_id==3&&op.wave==44;
            if(exception) {
                static const unsigned char wave_hash[32]={0xa8,0xfb,0x77,0x1a,0x30,0xd8,0xab,0xdb,0x3b,0x8b,0x34,0xe7,0x2a,0xd5,0x62,0xa4,
                    0x53,0x47,0xbe,0xeb,0xfa,0x39,0xa8,0xc9,0x9f,0x00,0x25,0xfe,0xc7,0xcf,0xda,0x49};
                unsigned char digest[32];lt48_sha256(w.data,w.data_size,digest);
                if(w.data_size!=312||memcmp(digest,wave_hash,32)) {
                    lt48_compact_wave_close(compact);status=LT48_SAFETY_REJECTED;goto fail;
                }
            }
            status=lt48_compact_compile(compact,r->pins,r->variables,r->variable_count,1,exception,r->waves+op.operation_id);
            lt48_compact_wave_close(compact);if(status!=LT48_OK)goto fail;
            size_t defaults=lt48_compiled_wave_defaulted_pin_count(r->waves[op.operation_id]);
            if(exception) {
                lt48_defaulted_logical_pin pin;
                TRY(lt48_compiled_wave_get_defaulted_pin(r->waves[op.operation_id],0,&pin));
                if(defaults!=1||pin.logical_pin||pin.compact_offset!=28||pin.compact_opcode!=3) {
                    status=LT48_SAFETY_REJECTED;goto fail;
                }
                /* Exact missing logical0/channel0 expands to Q2(1),
                 * resident1C/01,Q2(2). The actual resident1C handler returns
                 * without relay, serial or EEPROM effects for selector0.
                 * Keep the substitution visible; do not relabel a socket pin.
                 * Proof: older16-20-import/pin-zero-resident.json. */
                unsigned matches=0;size_t word_count;const uint16_t *words=lt48_compiled_wave_words(r->waves[op.operation_id],&word_count);
                static const uint16_t noop[9]={0,1,1,28,1,1,0,2,79};
                for(size_t j=0;j<lt48_compiled_wave_expansion_count(r->waves[op.operation_id]);j++) {
                    lt48_compiled_expansion expansion;TRY(lt48_compiled_wave_get_expansion(r->waves[op.operation_id],j,&expansion));
                    if(expansion.compact_offset==28&&expansion.compact_opcode==3&&expansion.word_count==9&&
                       expansion.first_word<=word_count&&word_count-expansion.first_word>=9&&
                       !memcmp(words+expansion.first_word,noop,sizeof noop))matches++;
                }
                if(matches!=1){status=LT48_SAFETY_REJECTED;goto fail;}
            }else if(defaults){status=LT48_SAFETY_REJECTED;goto fail;}
        } else if(w.wave_type==6) {
            if(!w.has_applet_header||w.data_size>65536u-(size_t)w.load_address){status=LT48_FORMAT_ERROR;goto fail;}
        } else if(w.wave_type!=4){status=LT48_UNSUPPORTED;goto fail;}
        r->entries[r->entry_count++]=(lt48_pic_source_resource_entry){op.operation_id,op.wave,w,r->waves[op.operation_id]};
    }
    *out=r;return LT48_OK;
fail:lt48_pic_source_resources_close_internal(r);return status;
#undef TRY
}
lt48_status lt48_pic_source_resources_library_internal(const char *path,
    const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
    lt48_pic_source_resources **out) {
    return source_resources_library(path,expected,family,pinmap,0,out);
}
lt48_status lt48_pic_source_resources_older16_library_internal(const char *path,
    const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
    lt48_pic_source_resources **out) {
    return source_resources_library(path,expected,family,pinmap,1,out);
}
lt48_status lt48_pic_source_resources_older16_authored_library_internal(const char *path,
 const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
 lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_pic_source_resources *r=NULL;
 lt48_status st=lt48_pic_source_resources_older16_library_internal(path,expected,family,pinmap,&r);
 if(st!=LT48_OK)return st;
 if(family>=29&&family<=36) {
  size_t slot=r->entry_count;
  for(size_t i=0;i<r->entry_count;i++)if(r->entries[i].role==10)slot=i;
  if(slot==r->entry_count||r->entries[slot].wave!=37){st=LT48_SAFETY_REJECTED;goto fail;}
  lt48_compiled_wave *corrected=NULL;
  st=lt48_pic17_pm2_compile(&r->entries[slot].source,r->pins,r->variables,r->variable_count,&corrected);
  if(st!=LT48_OK)goto fail;
  lt48_compiled_wave_close(r->waves[10]);r->waves[10]=corrected;r->entries[slot].compiled=corrected;
 }
 if(family>=37&&family<=39){
  size_t slot=r->entry_count;for(size_t i=0;i<r->entry_count;i++)if(r->entries[i].role==3)slot=i;
  if(slot==r->entry_count||r->entries[slot].wave!=44){st=LT48_SAFETY_REJECTED;goto fail;}
  lt48_compiled_wave *candidate=NULL;st=lt48_pic75_entry_candidate_compile(&r->entries[slot].source,r->pins,r->variables,r->variable_count,&candidate);
  if(st!=LT48_OK)goto fail;
  lt48_compiled_wave_close(r->waves[3]);r->waves[3]=candidate;r->entries[slot].compiled=candidate;
 }
 *out=r;return LT48_OK;
fail:lt48_pic_source_resources_close_internal(r);return st;
}
lt48_status lt48_pic_source_resources_pic18c_authored_library_internal(const char *path,
 const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
 lt48_pic_source_resources **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 static const unsigned char required[32]={0x24,0xb8,0x32,0x0d,0xa4,0xcf,0xd7,0x00,0xee,0x43,0x6f,0xf0,0x6b,0xd5,0x5d,0x9d,0xfb,0xd4,0x72,0xc5,0x3a,0x02,0x1a,0xd5,0x27,0xc9,0xbb,0xcb,0x3c,0x80,0x4d,0xec};
 unsigned valid=(family==347||family==348)?pinmap==156:(family==349||family==350)?(pinmap==157||pinmap==158):family==351&&pinmap==159;
 if(!expected||memcmp(expected,required,32)||!valid)return LT48_SAFETY_REJECTED;
 lt48_pic_source_resources *r=NULL;lt48_status st=lt48_pic_source_resources_library_internal(path,expected,family,pinmap,&r);
 if(st!=LT48_OK)return st;
 size_t slot=r->entry_count;for(size_t i=0;i<r->entry_count;i++)if(r->entries[i].role==4)slot=i;
 if(slot==r->entry_count||r->entries[slot].wave!=513){st=LT48_SAFETY_REJECTED;goto fail;}
 lt48_compiled_wave *corrected=NULL;st=lt48_pic18c_reset_alignment_compile(&r->entries[slot].source,r->pins,r->variables,r->variable_count,&corrected);
 if(st!=LT48_OK)goto fail;
 lt48_compiled_wave_close(r->waves[4]);r->waves[4]=corrected;r->entries[slot].compiled=corrected;*out=r;return LT48_OK;
fail:lt48_pic_source_resources_close_internal(r);return st;
}
size_t lt48_pic_source_resources_count_internal(const lt48_pic_source_resources *r) {
    return r?r->entry_count:0;
}
lt48_status lt48_pic_source_resources_otp14_library_internal(const char *path,
 const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
 lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_pic_source_resources *r=NULL;
 lt48_status status=lt48_pic_source_resources_library_internal(path,expected,family,pinmap,&r);
 if(status!=LT48_OK)return status;
 size_t slot=r->entry_count;
 for(size_t i=0;i<r->entry_count;i++)if(r->entries[i].role==13)slot=i;
 if(slot==r->entry_count||r->entries[slot].source.wave_type!=0){status=LT48_SAFETY_REJECTED;goto fail;}
 lt48_compact_wave *source=NULL;lt48_compiled_wave *corrected=NULL;
 status=lt48_compact_decode(r->entries[slot].source.data,r->entries[slot].source.data_size,&source);
 if(status==LT48_OK)status=lt48_pic_otp14_word_compile(source,r->pins,r->variables,r->variable_count,&corrected);
 lt48_compact_wave_close(source);
 if(status!=LT48_OK)goto fail;
 lt48_compiled_wave_close(r->waves[13]);r->waves[13]=corrected;r->entries[slot].compiled=corrected;
 *out=r;return LT48_OK;
fail:lt48_pic_source_resources_close_internal(r);return status;
}
lt48_status lt48_pic_source_resources_overlay_library_internal(const char *path,
    const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
    const lt48_pic_source_ram_overlay *overlays,size_t count,lt48_pic_source_resources **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!overlays||!count||count>256)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_resources *r=NULL;
    lt48_status status=lt48_pic_source_resources_library_internal(path,expected,family,pinmap,&r);
    if(status!=LT48_OK)return status;
    for(size_t k=0;k<count;k++) {
        const lt48_pic_source_ram_overlay *o=overlays+k;
        size_t index=r->entry_count;
        for(size_t i=0;i<r->entry_count;i++)if(r->entries[i].role==o->role)index=i;
        if(index==r->entry_count){status=LT48_NOT_FOUND;goto fail;}
        lt48_wave_code *w=&r->entries[index].source;
        if(r->owned_payloads[index]||w->wave_type!=6||!w->has_applet_header||
           w->load_address!=o->load_address||w->data_size!=o->original_size||
           !o->data||!o->size||o->slot_end>65536u||o->slot_end<=o->load_address||
           o->size>o->slot_end-o->load_address){status=LT48_SAFETY_REJECTED;goto fail;}
        unsigned char actual[LT48_SHA256_SIZE];lt48_sha256(w->data,w->data_size,actual);
        if(memcmp(actual,o->original_sha256,sizeof(actual))){status=LT48_SAFETY_REJECTED;goto fail;}
        lt48_sha256(o->data,o->size,actual);
        if(memcmp(actual,o->sha256,sizeof(actual))){status=LT48_SAFETY_REJECTED;goto fail;}
        unsigned char *copy=malloc(o->size);
        if(!copy){status=LT48_NO_MEMORY;goto fail;}
        memcpy(copy,o->data,o->size);r->owned_payloads[index]=copy;
        w->data=copy;w->data_size=o->size;
    }
    /* Validate the final table, so changes earlier/later in the same request
     * cannot escape the overlap check. End addresses are exclusive. */
    for(size_t i=0;i<r->entry_count;i++)if(r->entries[i].source.wave_type==6)
        for(size_t j=0;j<i;j++)if(r->entries[j].source.wave_type==6) {
            const lt48_wave_code *a=&r->entries[i].source,*b=&r->entries[j].source;
            if(a->load_address<(size_t)b->load_address+b->data_size&&
               b->load_address<(size_t)a->load_address+a->data_size) {
                status=LT48_SAFETY_REJECTED;goto fail;
            }
        }
    *out=r;return LT48_OK;
fail:lt48_pic_source_resources_close_internal(r);return status;
}
lt48_status lt48_pic_source_resources_entry_internal(const lt48_pic_source_resources *r,size_t index,
    lt48_pic_source_resource_entry *entry) {
    if(!r||!entry||index>=r->entry_count)return LT48_INVALID_ARGUMENT;
    *entry=r->entries[index];return LT48_OK;
}
lt48_status lt48_pic_source_resources_overlay_time_library_internal(const char *path,
    const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
    const lt48_pic_source_ram_overlay *overlays,size_t overlay_count,
    const lt48_pic_source_time_override *overrides,size_t count,lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!overrides||!count||count>256)return LT48_INVALID_ARGUMENT;
 lt48_pic_source_resources *r=NULL;
 if((!overlays&&overlay_count)||(overlays&&!overlay_count))return LT48_INVALID_ARGUMENT;
 lt48_status status=overlay_count?
  lt48_pic_source_resources_overlay_library_internal(path,expected,family,pinmap,overlays,overlay_count,&r):
  lt48_pic_source_resources_library_internal(path,expected,family,pinmap,&r);
 if(status!=LT48_OK)return status;
 for(size_t k=0;k<count;k++) {
  const lt48_compile_variable *old=&overrides[k].original,*replacement=&overrides[k].replacement;
  if(old->kind!=1||replacement->kind!=1||old->variable_id!=replacement->variable_id||
     old->enabled!=replacement->enabled||old->value!=replacement->value||old->value>2||
     !replacement->channel){status=LT48_SAFETY_REJECTED;goto fail;}
  for(size_t j=0;j<k;j++)if(overrides[j].original.variable_id==old->variable_id){status=LT48_SAFETY_REJECTED;goto fail;}
  size_t index=r->variable_count;
  for(size_t j=0;j<r->variable_count;j++)if(r->variables[j].variable_id==old->variable_id)index=j;
  if(index==r->variable_count||memcmp(r->variables+index,old,sizeof(*old))){status=LT48_SAFETY_REJECTED;goto fail;}
  r->variables[index]=*replacement;
 }
 for(size_t i=0;i<r->entry_count;i++) {
  lt48_pic_source_resource_entry *e=r->entries+i;
  if(e->source.wave_type!=0)continue;
  lt48_compact_wave *compact=NULL;lt48_compiled_wave *compiled=NULL;
  status=lt48_compact_decode(e->source.data,e->source.data_size,&compact);
  if(status==LT48_OK)status=lt48_compact_compile(compact,r->pins,r->variables,r->variable_count,1,0,&compiled);
  lt48_compact_wave_close(compact);
  if(status!=LT48_OK){lt48_compiled_wave_close(compiled);goto fail;}
  if(lt48_compiled_wave_defaulted_pin_count(compiled)){lt48_compiled_wave_close(compiled);status=LT48_SAFETY_REJECTED;goto fail;}
  lt48_compiled_wave_close(r->waves[e->role]);r->waves[e->role]=compiled;e->compiled=compiled;
 }
 *out=r;return LT48_OK;
fail:lt48_pic_source_resources_close_internal(r);return status;
}
lt48_status lt48_pic_source_resources_time_library_internal(const char *path,
 const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
 const lt48_pic_source_time_override *overrides,size_t count,lt48_pic_source_resources **out) {
 return lt48_pic_source_resources_overlay_time_library_internal(path,expected,family,pinmap,NULL,0,overrides,count,out);
}
lt48_status lt48_pic_source_resources_bindings_internal(const lt48_pic_source_resources *r,
    const lt48_pin_map **pins,const lt48_compile_variable **variables,size_t *count) {
    if(!r||!pins||!variables||!count)return LT48_INVALID_ARGUMENT;
    *pins=r->pins;*variables=r->variables;*count=r->variable_count;return LT48_OK;
}
lt48_status lt48_pic_source_resources_create_session_internal(const lt48_pic_source_resources *r,
    const lt48_runtime_options *options,lt48_pic_source_session **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;if(!r||!options)return LT48_INVALID_ARGUMENT;
    lt48_runtime_nested_wave callbacks[256];lt48_applet_address applets[256];size_t compact_count=0,applet_count=0;
    for(size_t i=0;i<r->entry_count;i++) {
        const lt48_pic_source_resource_entry *entry=r->entries+i;
        if(entry->compiled)callbacks[compact_count++]=(lt48_runtime_nested_wave){entry->role,entry->compiled};
        else if(entry->source.wave_type==6)applets[applet_count++]=(lt48_applet_address){entry->role,entry->source.load_address};
    }
    /* Keep legacy Kernel storage separate from modern direct configuration.
     * Explicit caller values override only their corresponding source field. */
    if(options->configuration_count>1024 ||
       (options->configuration_count&&!options->configuration))return LT48_INVALID_ARGUMENT;
    lt48_runtime_options seeded=*options;
    seeded.use_explicit_legacy_configuration_object=1;
    seeded.legacy_configuration_object=options->use_explicit_legacy_configuration_object?
        options->legacy_configuration_object:-2;
    int direct_object=options->use_explicit_direct_configuration_object?options->direct_configuration_object:-1;
    if(seeded.legacy_configuration_object>=0 || seeded.legacy_configuration_object==direct_object)
        return LT48_INVALID_ARGUMENT;
    lt48_runtime_configuration_value initial[1024];size_t count=options->configuration_count,index;
    if(count)memcpy(initial,options->configuration,count*sizeof(*initial));
    lt48_status status=lt48_rez_find_resource(r->rez,"Kernel",&index);
    for(unsigned category=0;status==LT48_OK&&category<3;category++)for(unsigned field=0;field<32;field++) {
        uint32_t value;status=lt48_rez_get_kernel_configuration_value(r->rez,index,category,field,&value);
        if(status!=LT48_OK)break;
        int supplied=0;
        for(size_t i=0;i<count;i++)if(initial[i].object_id==seeded.legacy_configuration_object&&
            initial[i].category==category&&initial[i].field==field)supplied=1;
        if(supplied)continue;
        if(count==1024)return LT48_INVALID_ARGUMENT;
        initial[count++]=(lt48_runtime_configuration_value){seeded.legacy_configuration_object,
            (uint16_t)category,(uint16_t)field,value};
    }
    if(status!=LT48_OK)return status;
    seeded.configuration=initial;seeded.configuration_count=count;
    status=lt48_pic_source_session_create_internal(r->pins,r->variables,r->variable_count,
        callbacks,compact_count,&seeded,out);
    if(status==LT48_OK)status=lt48_pic_source_session_bind_frame_internal(*out,&r->frame);
    if(status==LT48_OK)status=lt48_pic_source_session_bind_applets_internal(*out,applets,applet_count);
    if(status==LT48_OK&&applet_count) {
        const char *wanted[]={"_FUN_READ","_FUN_PROG","_FUN_VERIFY","_FUN_BLKCHK"};
        uint16_t roles[4]={UINT16_MAX,UINT16_MAX,UINT16_MAX,UINT16_MAX};
        unsigned found[4]={0};size_t index;
        lt48_defined_name_collection_info info;
        status=lt48_rez_find_resource(r->rez,"DefineName",&index);
        if(status==LT48_OK)status=lt48_rez_get_defined_name_collection_info(r->rez,index,&info);
        for(size_t i=0;status==LT48_OK&&i<info.count;i++) {
            lt48_defined_name name;status=lt48_rez_get_defined_name(r->rez,index,i,&name);
            if(status!=LT48_OK)break;
            for(unsigned j=0;j<4;j++)if(name.name_length==strlen(wanted[j])&&!memcmp(name.name,wanted[j],name.name_length)) {
                if(found[j]++){status=LT48_FORMAT_ERROR;break;}
                roles[j]=name.identifier;
            }
        }
        for(unsigned i=0;i<4&&status==LT48_OK;i++)if(!found[i])status=LT48_UNSUPPORTED;
        lt48_pic_source_applet_host_roles binding={roles[0],roles[1],roles[2],roles[3]};
        if(status==LT48_OK)status=lt48_pic_source_session_bind_applet_host_internal(*out,&binding);
    }
    if(status==LT48_OK)status=lt48_pic_source_session_bind_origin_internal(*out,r);
    if(status!=LT48_OK){lt48_pic_source_session_close_internal(*out);*out=NULL;}
    return status;
}
lt48_status lt48_pic_source_resources_open_internal(const char *path,
    const unsigned char expected[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap,
    const lt48_pic_source_operation_contract *contract,const lt48_pic_source_host_binding *host,
    const lt48_runtime_options *options,lt48_pic_source_resources **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    if(!path||!expected||!contract||!host||!options)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_geometry geometry;
    if(!lt48_pic_source_geometry_prepare(contract->geometry.element_count,contract->element_bits,
        contract->geometry.backing_bytes,&geometry) ||
        geometry.element_bytes!=contract->geometry.element_bytes ||
        geometry.traversal_bytes!=contract->geometry.traversal_bytes ||
        geometry.first_element!=contract->geometry.first_element ||
        geometry.last_element!=contract->geometry.last_element ||
        geometry.last_file_byte!=contract->geometry.last_file_byte ||
        geometry.checksum_last_byte!=contract->geometry.checksum_last_byte ||
        options->device_image_size!=geometry.backing_bytes ||
        options->device_element_bytes!=geometry.element_bytes)return LT48_SAFETY_REJECTED;
    lt48_pic_source_resources *r=NULL;
    lt48_status status=lt48_pic_source_resources_library_internal(path,expected,family,pinmap,&r);
    if(status!=LT48_OK)return status;
#define TRY(x) do{status=(x);if(status!=LT48_OK)goto fail;}while(0)
    if(r->frame.buffer_count<=9||r->frame.buffer_count>LT48_RUNTIME_BUFFER_COUNT||r->frame.mode_buffer!=9){status=LT48_UNSUPPORTED;goto fail;}
    int voltage_found=0;
    for(size_t i=0;i<r->variable_count;i++)if(r->variables[i].variable_id==host->voltage_variable) {
        if(voltage_found++||r->variables[i].kind!=0||!r->variables[i].enabled||!r->variables[i].value){status=LT48_SAFETY_REJECTED;goto fail;}
    }
    if(!voltage_found){status=LT48_SAFETY_REJECTED;goto fail;}
    for(size_t i=0;i<r->entry_count;i++)
        if(!r->entries[i].compiled){status=LT48_UNSUPPORTED;goto fail;}
    const uint16_t roles[]={host->poweron_role,host->poweroff_role,contract->init_role,
        contract->reset_role,contract->load_role,contract->program_role,
        contract->erase_role,contract->configuration_role};
    if(host->poweron_role==UINT16_MAX||host->poweroff_role==UINT16_MAX){status=LT48_SAFETY_REJECTED;goto fail;}
    for(size_t i=0;i<sizeof(roles)/sizeof(*roles);i++)
        if(roles[i]!=UINT16_MAX&&(roles[i]>=256||!r->waves[roles[i]])){status=LT48_SAFETY_REJECTED;goto fail;}
    TRY(lt48_pic_source_resources_create_session_internal(r,options,&r->session));
    r->operation=*contract;r->host=*host;
    *out=r;return LT48_OK;
fail:lt48_pic_source_resources_close_internal(r);return status;
#undef TRY
}
lt48_status lt48_pic_source_resources_execute_internal(lt48_pic_source_resources *r,
    const lt48_protocol_transport *api,void *context,const unsigned char *fpga,size_t fpga_size,
    uint16_t mode,uint32_t first,uint32_t last,uint32_t voltage,uint32_t minimum,uint32_t maximum,
    uint32_t option_flags,lt48_pic_source_operation_result *result) {
    if(result)*result=(lt48_pic_source_operation_result){LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(!r||!r->session||!api||!fpga||!fpga_size||!result)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_transport *owner=NULL;
    lt48_status status=lt48_pic_source_transport_create_internal(api,context,r->session,&r->host,
        voltage,minimum,maximum,&owner);
    if(status!=LT48_OK){result->operation_status=status;return status;}
    status=lt48_pic_source_transport_prepare_internal(owner,fpga,fpga_size);
    if(status==LT48_OK) {
        lt48_pic_source_range_options options={0};options.logical_image_size=r->operation.geometry.traversal_bytes;
        options.reset_mask=r->frame.reset_mask;options.option_flags=option_flags;
        status=lt48_pic_source_transport_run_internal(owner,&r->operation,mode,first,last,&options,result);
    } else {
        result->operation_status=status;result->cleanup_attempted=1;
        result->cleanup_status=lt48_pic_source_transport_cleanup_status_internal(owner);
    }
    lt48_pic_source_transport_close_internal(owner);return status;
}

static int source_applet_role_compare(const void *left,const void *right) {
    const lt48_pic_source_applet_payload *a=left,*b=right;
    return (a->role>b->role)-(a->role<b->role);
}
lt48_status lt48_pic_source_resources_preparation_internal(const lt48_pic_source_resources *resources,
    uint16_t fpga_role,lt48_pic_source_preparation *out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));if(!resources)return LT48_INVALID_ARGUMENT;
    lt48_pic_source_preparation preparation={0};
    for(size_t i=0;i<resources->entry_count;i++) {
        const lt48_pic_source_resource_entry *entry=resources->entries+i;
        const lt48_wave_code *source=&entry->source;
        if(source->wave_type==6) {
            preparation.applets[preparation.applet_count++]=(lt48_pic_source_applet_payload){
                entry->role,source->load_address,source->data,source->data_size};
        }
        if(entry->role==fpga_role) {
            if(source->wave_type!=4)return LT48_UNSUPPORTED;
            if(source->data_size<3||((size_t)source->data[0]|((size_t)source->data[1]<<8))!=source->data_size-2)
                return LT48_FORMAT_ERROR;
            preparation.fpga=source->data+2;preparation.fpga_size=source->data_size-2;
        }
    }
    if(!preparation.fpga)return LT48_UNSUPPORTED;
    /* Original 0043d580 scans the callback table by numeric role, independently
     * of the order in which TFamCode lists its operation pairs. */
    qsort(preparation.applets,preparation.applet_count,sizeof(preparation.applets[0]),source_applet_role_compare);
    *out=preparation;return LT48_OK;
}
lt48_status lt48_pic_source_resources_prepare_internal(const lt48_pic_source_resources *resources,
    uint16_t fpga_role,lt48_pic_source_transport *transport) {
    if(!resources||!transport)return LT48_INVALID_ARGUMENT;
    if(!lt48_pic_source_transport_matches_origin_internal(transport,resources))return LT48_SAFETY_REJECTED;
    lt48_pic_source_preparation preparation;
    lt48_status status=lt48_pic_source_resources_preparation_internal(resources,fpga_role,&preparation);
    if(status!=LT48_OK)return status;
    return lt48_pic_source_transport_prepare_applets_internal(transport,preparation.fpga,preparation.fpga_size,
        preparation.applets,preparation.applet_count);
}

lt48_status lt48_pic_source_resources_prepare_startup_internal(const lt48_pic_source_resources *resources,
    uint16_t fpga_role,lt48_pic_source_transport *transport) {
    if(!resources||!transport)return LT48_INVALID_ARGUMENT;
    if(!lt48_pic_source_transport_matches_origin_internal(transport,resources))return LT48_SAFETY_REJECTED;
    lt48_pic_source_preparation preparation;
    lt48_status status=lt48_pic_source_resources_preparation_internal(resources,fpga_role,&preparation);
    if(status!=LT48_OK)return status;
    return lt48_pic_source_transport_prepare_startup_internal(transport,preparation.fpga,preparation.fpga_size,
        preparation.applets,preparation.applet_count);
}

int lt48_pic_source_resources_matches_binding_internal(const lt48_pic_source_resources *resources,
    const unsigned char digest[LT48_SHA256_SIZE],uint16_t family,uint16_t pinmap) {
    return resources&&digest&&resources->family==family&&resources->pinmap==pinmap&&
        !memcmp(resources->digest,digest,LT48_SHA256_SIZE);
}

lt48_status lt48_pic_source_resources_prepare_wire_internal(const lt48_pic_source_resources *resources,
    lt48_pic_source_session *session,uint16_t fpga_role,lt48_runtime_transport_session *wire,uint8_t selector) {
    if(!resources||!session||!wire)return LT48_INVALID_ARGUMENT;
    if(!lt48_pic_source_session_matches_origin_internal(session,resources))return LT48_SAFETY_REJECTED;
    lt48_pic_source_preparation preparation;
    lt48_status status=lt48_pic_source_resources_preparation_internal(resources,fpga_role,&preparation);
    if(status!=LT48_OK)return status;
    return lt48_pic_source_transport_load_internal(wire,preparation.fpga,preparation.fpga_size,
        preparation.applets,preparation.applet_count,selector);
}
