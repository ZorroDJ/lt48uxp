#ifndef LT48_PIC18_FOURBIT_RUNTIME_H
#define LT48_PIC18_FOURBIT_RUNTIME_H
#include "source_resources.h"
#include "applet_recovery.h"
#include "configuration_recovery.h"
#include "source_erase_timing.h"
#include "pic18_fourbit_runtime_generated.h"

/* The normal owner selects an exact generated source profile first. Build
 * RAM-only corrections from the user's original library; never save them to
 * the source file or programmer nonvolatile memory. Public admission remains
 * the operator's separate complete-operation gate. */
static inline lt48_status lt48_pic18_fourbit_runtime_open(const char *path,
    const unsigned char digest[32],uint16_t family,uint16_t pinmap,
    lt48_pic_source_resources **out) {
    if(!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_pic_source_resources *original=NULL;
    lt48_status st=lt48_pic_source_resources_library_internal(path,digest,family,pinmap,&original);
    if(st!=LT48_OK)return st;
    unsigned char program[1731],configuration[1670];size_t configuration_size=0;
    lt48_pic_source_ram_overlay overlays[2]={0};unsigned found=0;
    for(size_t i=0;st==LT48_OK&&i<lt48_pic_source_resources_count_internal(original);i++) {
        lt48_pic_source_resource_entry e;
        st=lt48_pic_source_resources_entry_internal(original,i,&e);if(st!=LT48_OK)break;
        if(e.role!=20&&e.role!=24)continue;
        if(e.source.wave_type!=6){st=LT48_SAFETY_REJECTED;break;}
        unsigned index=e.role==20?0:1;lt48_pic_source_ram_overlay *o=overlays+index;
        found|=1u<<index;o->role=e.role;o->load_address=e.source.load_address;
        o->original_size=e.source.data_size;
        if(index==0) {
            st=lt48_pic18_patch_recovery(e.source.data,e.source.data_size,e.source.load_address,program,sizeof(program));
            o->data=program;o->size=sizeof(program);o->slot_end=0x9000;
            memcpy(o->original_sha256,lt48_pic18_fourbit_program_original_sha,32);
            memcpy(o->sha256,lt48_pic18_fourbit_program_patched_sha,32);
        } else {
            st=lt48_pic18_patch_configuration_recovery(e.source.data,e.source.data_size,e.source.load_address,configuration,sizeof(configuration),&configuration_size);
            o->data=configuration;o->size=configuration_size;o->slot_end=0xa000;
            memcpy(o->original_sha256,lt48_pic18_fourbit_configuration_original_sha,32);
            memcpy(o->sha256,lt48_pic18_fourbit_configuration_patched_sha,32);
        }
    }
    if(st==LT48_OK&&found!=3)st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK)st=lt48_pic_source_resources_overlay_library_internal(path,digest,family,pinmap,overlays,2,out);
    lt48_pic_source_resources_close_internal(original);return st;
}
#endif
