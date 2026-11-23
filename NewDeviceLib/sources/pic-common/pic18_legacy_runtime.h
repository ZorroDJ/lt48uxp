#ifndef LT48_PIC18_LEGACY_RUNTIME_H
#define LT48_PIC18_LEGACY_RUNTIME_H
#include "source_resources.h"
#include "legacy_applet_recovery.h"
#include "configuration_recovery.h"
#include "erase_key_recovery.h"
#include "pic18_legacy_runtime_generated.h"

/* Construction-only owned RAM corrections. Exact policy lookup and original
 * library digest are mandatory; public admission belongs to the operator. */
static inline lt48_status lt48_pic18_legacy_runtime_open(const char *path,
    const unsigned char digest[32],uint16_t family,uint16_t pinmap,
    const lt48_pic18_legacy_runtime_policy *policy,lt48_pic_source_resources **out) {
    if(!out||!policy||policy->family!=family||policy->pinmap!=pinmap)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_pic_source_resources *original=NULL;
    lt48_status st=lt48_pic_source_resources_library_internal(path,digest,family,pinmap,&original);
    if(st!=LT48_OK)return st;
    unsigned char program[1614],configuration[1670],erase[800];size_t cfg_size=0;
    lt48_pic_source_ram_overlay overlays[3]={0};unsigned count=0,found=0;
    for(size_t i=0;st==LT48_OK&&i<lt48_pic_source_resources_count_internal(original);i++) {
        lt48_pic_source_resource_entry e;
        st=lt48_pic_source_resources_entry_internal(original,i,&e);if(st!=LT48_OK)break;
        if(e.role!=20&&e.role!=24&&(e.role!=23||!policy->erase_patch))continue;
        if(e.source.wave_type!=6){st=LT48_SAFETY_REJECTED;break;}
        unsigned bit=e.role==20?1u:e.role==24?2u:4u;
        if(found&bit||count>=3){st=LT48_SAFETY_REJECTED;break;}
        found|=bit;lt48_pic_source_ram_overlay *o=overlays+count++;
        o->role=e.role;o->load_address=e.source.load_address;o->original_size=e.source.data_size;
        const unsigned char *before,*after;
        if(e.role==20) {
            st=lt48_pic18_patch_legacy_recovery(e.source.data,e.source.data_size,e.source.load_address,program,sizeof(program));
            o->data=program;o->size=sizeof(program);o->slot_end=0x9000;
            before=lt48_pic18_legacy_program_original_sha;after=lt48_pic18_legacy_program_patched_sha;
        } else if(e.role==24) {
            st=lt48_pic18_patch_configuration_recovery(e.source.data,e.source.data_size,e.source.load_address,configuration,sizeof(configuration),&cfg_size);
            o->data=configuration;o->size=cfg_size;o->slot_end=0xa000;
            before=lt48_pic18_legacy_configuration_original_sha;after=lt48_pic18_legacy_configuration_patched_sha;
        } else {
            st=lt48_pic18_patch_erase_key(e.source.data,e.source.data_size,e.source.load_address,erase,sizeof(erase));
            o->data=erase;o->size=sizeof(erase);o->slot_end=0x9800;
            before=lt48_pic18_legacy_erase_original_sha;after=lt48_pic18_legacy_erase_patched_sha;
        }
        memcpy(o->original_sha256,before,32);memcpy(o->sha256,after,32);
    }
    if(st==LT48_OK&&found!=(policy->erase_patch?7u:3u))st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK)st=lt48_pic_source_resources_overlay_library_internal(path,digest,family,pinmap,overlays,count,out);
    lt48_pic_source_resources_close_internal(original);return st;
}
#endif
