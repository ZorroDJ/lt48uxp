#include <labtool48/labtool48.h>
#include "internal.h"
#include "pic18_j.h"
#include "../../NewDeviceLib/sources/pic-common/preservation.h"
#include "../../NewDeviceLib/sources/pic-common/source_resources.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../NewDeviceLib/sources/pic-common/addtab_maps_generated.h"
/* Sparse byte-addressed PIC18 image: code, ID 200000..07,
 * CONFIG 300000..0d, EEPROM f00000..ff. HEX addresses need no translation. */
#include "../../NewDeviceLib/sources/pic-common/applet_image.h"
#include "../../NewDeviceLib/sources/pic-common/applet_images_generated.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_profiles_generated.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_image_corrections.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_admissions_generated.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_fourbit_runtime.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_legacy_runtime.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_x220_runtime.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_xx39_timing.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_xx2_ui.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_xx31_ui.h"
#include "../../NewDeviceLib/drafts/pic-population/pic18_k80_ui.h"
#include "../../NewDeviceLib/drafts/pic-population/pic18_k80_runtime.h"
#include "../../NewDeviceLib/drafts/pic-population/pic18_xx39_runtime.h"
#include "../../NewDeviceLib/drafts/pic-population/pic18_xx39_policy.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_f6x20_policy.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_f6_runtime.h"
#include "../../NewDeviceLib/sources/pic-common/configuration_completion.h"
#include "../../NewDeviceLib/sources/pic-common/pic18_legacy_compositions_generated.h"
#include "../../NewDeviceLib/sources/pic-common/configuration.h"
#include "../../NewDeviceLib/sources/pic-common/configuration_schemas_generated.h"
struct lt48_pic18_resources {
    lt48_pic18_j_owner *j;
    lt48_pic_source_resources *source;const lt48_pin_map *pins;const lt48_compiled_wave *wave[32];
    lt48_compile_variable vars[16];size_t var_count;
    lt48_applet_address *addresses;size_t applet_count;
    const unsigned char *fpga;unsigned char *idle;size_t idle_size;
    unsigned flash,page;
    const lt48_pic_addtab_map *mapping;
    const lt48_pic_applet_image_policy *policy;
    const lt48_pic18_operator_profile *profile;
    lt48_pic_applet_image_policy corrected_image_policy;
    unsigned char xx39_secure_mask[14];
    const lt48_pic18_fourbit_runtime_policy *runtime_policy;
    const lt48_pic18_legacy_runtime_policy *legacy_runtime_policy;
    const lt48_pic18_f6_runtime_policy *f6_runtime_policy;
    const lt48_pic_configuration_composition *legacy_composition;
};
void lt48_pic18_close(lt48_pic18_resources *r){if(!r)return;lt48_pic18_j_owner_close(r->j);lt48_pic_source_resources_close_internal(r->source);free(r->addresses);free(r->idle);free(r);}
uint32_t lt48_pic18_capacity(const lt48_pic18_resources *r){return r?(r->j?lt48_pic18_j_owner_options(r->j)->program.code_bytes:r->policy->image_span):0;}
int lt48_pic18_is_j_series(const lt48_pic18_resources *r){return r&&r->j;}
/* Construction-only fixture factory. Public open still owns admission. */
lt48_status lt48_pic18_j_open_candidate_internal(const lt48_installation *x,size_t device,lt48_pic18_resources **out) {
 if(!x||!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;lt48_pic18_j_owner *j=NULL;lt48_status st=lt48_pic18_j_owner_open(x,device,&j);
 if(st!=LT48_OK)return st;
 lt48_pic18_resources *r=calloc(1,sizeof(*r));if(!r){lt48_pic18_j_owner_close(j);return LT48_NO_MEMORY;}
 r->j=j;*out=r;return LT48_OK;
}
static int pic18_configuration_binding(const lt48_pic18_resources *r) {
    if(!r||!r->profile||!r->policy)return 0;
    for(size_t i=0;i<sizeof(lt48_x220_candidates)/sizeof(*lt48_x220_candidates);i++) {
        const lt48_x220_candidate *p=lt48_x220_candidates+i;
        if(p->definition==r->profile->plan.definition&&p->family==r->profile->plan.family&&p->pinmap==r->profile->plan.pinmap)return 1;
    }return lt48_pic18_xx39_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)||
            lt48_pic18_k80_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)||
            lt48_pic18_xx2_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)||
            lt48_pic18_xx31_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)||
            lt48_pic18_f6x20_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)||
            lt48_pic18_f6_runtime_find(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)!=NULL;
}
lt48_status lt48_pic18_configuration_values(const lt48_pic18_resources *r,
    const unsigned char *image,size_t size,lt48_runtime_configuration_value *output,size_t capacity,size_t *count) {
    if(!r||!image||!count||(capacity&&!output))return LT48_INVALID_ARGUMENT;
    *count=0;
    if(r->j) {
        const lt48_pic18_j_options_profile *p=lt48_pic18_j_owner_options(r->j);lt48_pic18_j_option_value values[64];size_t n=0;
        lt48_status st=lt48_pic18_j_options_decode(p,image,size,values,64,&n);if(st!=LT48_OK)return st;
        *count=n;if(capacity<n)return LT48_BUFFER_TOO_SMALL;
        for(size_t i=0;i<n;i++)output[i]=(lt48_runtime_configuration_value){-1,1,values[i].field,values[i].value};
        return LT48_OK;
    }
    if(!pic18_configuration_binding(r))return LT48_UNSUPPORTED;
    if(size!=r->policy->image_span)return LT48_INVALID_ARGUMENT;
    size_t n=0;const lt48_pic_configuration_field *fields=lt48_pic_configuration_fields(r->profile->plan.configuration_id,&n);
    if(!fields||!n)return LT48_UNSUPPORTED;
    *count=n;if(capacity<n)return LT48_BUFFER_TOO_SMALL;
    lt48_runtime_options options={0};options.device_image=image;options.device_image_size=size;
    options.device_element_bytes=r->profile->plan.element_width;options.max_steps=100000;
    lt48_runtime_result *result=NULL;
    lt48_status st=lt48_runtime_interpret(r->wave[r->profile->plan.preconfig],r->pins,r->vars,r->var_count,&options,&result);
    for(size_t i=0;st==LT48_OK&&i<lt48_runtime_event_count(result);i++) {
        lt48_runtime_event e;st=lt48_runtime_get_event(result,i,&e);if(st!=LT48_OK)break;
        switch(e.kind) {
        case LT48_RUNTIME_EVALUATE_VARIABLE:case LT48_RUNTIME_BUFFER_ARITHMETIC:
        case LT48_RUNTIME_BUFFER_BIT_COPY:case LT48_RUNTIME_BUFFER_MEMMOVE:
        case LT48_RUNTIME_BUFFER_BYTE_OPERATION:case LT48_RUNTIME_BUFFER_FILL:break;
        case LT48_RUNTIME_RETURN_STATUS:if(!e.arguments[0])st=LT48_DEVICE_MISMATCH;break;
        default:st=LT48_UNSUPPORTED;break;
        }
    }
    const lt48_runtime_configuration_value *values=NULL;size_t available=0;
    if(st==LT48_OK)st=lt48_runtime_get_configuration(result,&values,&available);
    lt48_runtime_configuration_value staged[1024];
    if(n>1024)st=LT48_RUNTIME_ERROR;
    for(size_t i=0;st==LT48_OK&&i<n;i++) {
        size_t found=SIZE_MAX;
        for(size_t j=0;j<available;j++)if(values[j].object_id==-1&&values[j].category==fields[i].category&&values[j].field==fields[i].field) {
            if(found!=SIZE_MAX){st=LT48_FORMAT_ERROR;break;}found=j;
        }
        if(st!=LT48_OK)break;
        if(found==SIZE_MAX){st=LT48_UNSUPPORTED;break;}staged[i]=values[found];
    }
    if(st==LT48_OK)memcpy(output,staged,n*sizeof(*output));
    lt48_runtime_result_close(result);return st;
}
lt48_status lt48_pic18_configuration_edit(const lt48_pic18_resources *r,
    unsigned char *image,size_t size,const lt48_runtime_configuration_value *edits,size_t count) {
    if(!r||!image||(count&&!edits))return LT48_INVALID_ARGUMENT;
    if(r->j) {
        if(count>64)return LT48_INVALID_ARGUMENT;
        lt48_pic18_j_option_value values[64];
        for(size_t i=0;i<count;i++){if(edits[i].object_id!=-1||edits[i].category!=1||edits[i].value>255)return LT48_INVALID_ARGUMENT;values[i]=(lt48_pic18_j_option_value){edits[i].field,(uint8_t)edits[i].value};}
        return lt48_pic18_j_options_edit(lt48_pic18_j_owner_options(r->j),image,size,values,count);
    }
    if(!pic18_configuration_binding(r))return LT48_UNSUPPORTED;
    if(size!=r->policy->image_span)return LT48_INVALID_ARGUMENT;
    if(lt48_pic18_xx39_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)&&r->flash==12288)
        for(size_t i=0;i<count;i++)if(edits[i].category==0&&edits[i].value!=1&&
            (edits[i].field==258||edits[i].field==514||edits[i].field==770))return LT48_SAFETY_REJECTED;
    size_t fields_count=0;const lt48_pic_configuration_field *fields=lt48_pic_configuration_fields(r->profile->plan.configuration_id,&fields_count);
    if(!fields||!fields_count)return LT48_UNSUPPORTED;
    unsigned char *stage=malloc(size);if(!stage)return LT48_NO_MEMORY;
    lt48_status st=lt48_pic_configuration_edit_internal(r->wave[r->profile->plan.preconfig],r->wave[r->profile->plan.postconfig],
        r->pins,r->vars,r->var_count,r->profile->plan.element_width,image,size,stage,fields,fields_count,edits,count);
    uint32_t id=r->policy->id_address,id_end=id+r->policy->id_count,cfg=r->policy->configuration_address,cfg_end=cfg+r->policy->configuration_count;
    if(st==LT48_OK&&(id_end>cfg||cfg_end>size||memcmp(image,stage,id)||
        memcmp(image+id_end,stage+id_end,cfg-id_end)||memcmp(image+cfg_end,stage+cfg_end,size-cfg_end)))st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK&&lt48_pic18_xx39_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap))
        st=lt48_pic18_xx39_configuration_safe(r->flash,stage+cfg,r->policy->configuration_count);
    if(st==LT48_OK&&lt48_pic18_xx39_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap))
        lt48_pic18_xx39_prepare_configuration(r->flash,stage+cfg);
    if(st==LT48_OK)memcpy(image,stage,size);
    free(stage);return st;
}
/* Source-bound loader shared by admitted profiles and offline cohort fixtures.
 * The public entry point below owns admission; this loader is private. */
static lt48_status pic18_open_profile(const lt48_installation *x,const lt48_device_spec *selected,lt48_pic18_resources **out) {
    if(!x||!selected||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    lt48_device_spec spec=*selected;lt48_status st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
    lt48_pic18_resources *r=calloc(1,sizeof(*r));if(!r)return LT48_NO_MEMORY;
    r->profile=lt48_pic18_operator_find(spec.definition_resource,spec.family_code,spec.pinmap);
    r->policy=lt48_pic_applet_image_find(spec.definition_resource,spec.family_code,spec.pinmap);
    r->mapping=lt48_pic_addtab_find(spec.definition_resource);
    if(r->profile&&r->policy) {
        int correction=lt48_pic18_correct_image_policy(spec.definition_resource,spec.family_code,spec.pinmap,
            r->profile->plan.source_sha256,r->policy,&r->corrected_image_policy);
        if(correction<0){lt48_pic18_close(r);return LT48_FORMAT_ERROR;}
        if(correction)r->policy=&r->corrected_image_policy;
    }
    if(!r->profile||!lt48_pic_applet_image_policy_valid(r->policy,r->mapping)||
       r->profile->plan.element_count!=r->mapping->dense_capacity||r->profile->plan.element_width!=r->mapping->element_bytes){lt48_pic18_close(r);return LT48_FORMAT_ERROR;}
    r->flash=r->mapping->segments[0].length;r->page=r->flash/1024;
    if(lt48_pic18_xx39_configuration_binding(spec.definition_resource,spec.family_code,spec.pinmap)) {
        if(r->policy->configuration_count!=14){lt48_pic18_close(r);return LT48_FORMAT_ERROR;}
        memcpy(r->xx39_secure_mask,r->policy->secure_mask,14);
        r->xx39_secure_mask[8]&=(unsigned char)lt48_pic18_xx39_secure_code_mask(r->flash);
        r->corrected_image_policy=*r->policy;r->corrected_image_policy.secure_mask=r->xx39_secure_mask;
        r->policy=&r->corrected_image_policy;
    }
    char path[4096];unsigned char digest[32];
    int n=snprintf(path,sizeof(path),"%s/GDCC___.rez",lt48_installation_path(x));if(n<0||(size_t)n>=sizeof(path)){lt48_pic18_close(r);return LT48_INVALID_ARGUMENT;}
#define TRY(a) do{st=(a);if(st!=LT48_OK)goto fail;}while(0)
    const char *source_hash=r->profile->plan.source_sha256;
    for(unsigned i=0;i<32;i++){unsigned value;if(sscanf(source_hash+2*i,"%2x",&value)!=1){st=LT48_FORMAT_ERROR;goto fail;}digest[i]=(unsigned char)value;}
    r->runtime_policy=lt48_pic18_fourbit_runtime_find(spec.definition_resource,spec.family_code,spec.pinmap);
    int x220=lt48_x220_candidate_allowed(spec.definition_resource,spec.family_code,spec.pinmap,
        spec.catalogue.device_name,spec.catalogue.device_name_length);
    r->legacy_runtime_policy=lt48_pic18_legacy_runtime_find(spec.definition_resource,spec.family_code,spec.pinmap);
    if(spec.catalogue.device_name&&!lt48_pic18_legacy_runtime_member_allowed(spec.definition_resource,
        spec.family_code,spec.pinmap,spec.catalogue.device_name,spec.catalogue.device_name_length))
        r->legacy_runtime_policy=NULL;
    if(r->legacy_runtime_policy) {
        for(size_t i=0;i<sizeof(pic_configuration_composition_members)/sizeof(*pic_configuration_composition_members);i++) {
            const lt48_pic_composition_member *m=pic_configuration_composition_members+i;
            if(m->definition!=spec.definition_resource||m->family!=spec.family_code||m->pinmap!=spec.pinmap)continue;
            if(r->legacy_composition&&r->legacy_composition!=m->composition){st=LT48_SAFETY_REJECTED;goto fail;}
            r->legacy_composition=m->composition;
        }
        if(!r->legacy_composition){st=LT48_SAFETY_REJECTED;goto fail;}
    }
    if(r->runtime_policy&&r->legacy_runtime_policy){st=LT48_SAFETY_REJECTED;goto fail;}
    if(x220&&(r->runtime_policy||r->legacy_runtime_policy)){st=LT48_SAFETY_REJECTED;goto fail;}
    int xx39=lt48_pic18_xx39_timing_allowed(&spec);
    if(xx39&&(x220||r->runtime_policy||r->legacy_runtime_policy)){st=LT48_SAFETY_REJECTED;goto fail;}
    r->f6_runtime_policy=lt48_pic18_f6_runtime_find(spec.definition_resource,spec.family_code,spec.pinmap);
    if(r->f6_runtime_policy&&(xx39||x220||r->runtime_policy||r->legacy_runtime_policy)){st=LT48_SAFETY_REJECTED;goto fail;}
    if(r->f6_runtime_policy) {
        TRY(lt48_pic18_f6_runtime_open(path,digest,&spec,&r->source));
    } else if(xx39) {
        TRY(lt48_pic18_xx39_complete_runtime_open(path,digest,&spec,r->flash,&r->source));
    } else if(x220) {
        /* Bind the corrected owner before borrowing waves, pins or FPGA data.
         * Public admission is enforced independently by lt48_pic18_open. */
        TRY(lt48_x220_runtime_open(path,digest,spec.family_code,spec.pinmap,&r->source));
    } else if(r->runtime_policy) {
        TRY(lt48_pic18_fourbit_runtime_open(path,digest,spec.family_code,spec.pinmap,&r->source));
    } else if(r->legacy_runtime_policy) {
        TRY(lt48_pic18_legacy_runtime_open(path,digest,spec.family_code,spec.pinmap,r->legacy_runtime_policy,&r->source));
    } else if(lt48_pic18_k80_configuration_binding(spec.definition_resource,spec.family_code,spec.pinmap)) {
        TRY(lt48_pic18_k80_runtime_open(path,digest,spec.family_code,spec.pinmap,&r->source));
    } else {
        TRY(lt48_pic_source_resources_library_internal(path,digest,spec.family_code,spec.pinmap,&r->source));
    }
    const lt48_compile_variable *variables;size_t count;
    TRY(lt48_pic_source_resources_bindings_internal(r->source,&r->pins,&variables,&count));
    if(!count || count>sizeof(r->vars)/sizeof(*r->vars)){st=LT48_FORMAT_ERROR;goto fail;}
    r->var_count=count;memcpy(r->vars,variables,count*sizeof(*variables));
    size_t resource_count=lt48_pic_source_resources_count_internal(r->source);
    /* Roles and load addresses belong to the selected resource family, not a
     * fixed six-applet layout. Public profile admission remains above. */
    if(!resource_count){st=LT48_FORMAT_ERROR;goto fail;}
    r->addresses=calloc(resource_count,sizeof(*r->addresses));
    if(!r->addresses){st=LT48_NO_MEMORY;goto fail;}
    for(size_t i=0;i<resource_count;i++) {
        lt48_pic_source_resource_entry entry;TRY(lt48_pic_source_resources_entry_internal(r->source,i,&entry));
        const lt48_wave_code *w=&entry.source;
        if(w->wave_type==0 && entry.role<32)r->wave[entry.role]=entry.compiled;
        else if(w->wave_type==6){r->addresses[r->applet_count++]=(lt48_applet_address){entry.role,w->load_address};}
        else if(w->wave_type==4 && entry.role==30 && w->data_size==2780 && w->data[0]==0xda && w->data[1]==10)r->fpga=w->data+2;
        else {st=LT48_UNSUPPORTED;goto fail;}
    }
    if(!r->applet_count || !r->fpga){st=LT48_FORMAT_ERROR;goto fail;}
    TRY(lt48_load_verified_idle_fpga_internal(lt48_installation_path(x),&r->idle,&r->idle_size));*out=r;return LT48_OK;
fail:lt48_pic18_close(r);return st;
#undef TRY
}
lt48_status lt48_pic18_open(const lt48_installation *x,size_t device,lt48_pic18_resources **out) {
    if(!x||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;
    lt48_device_spec spec;lt48_status st=lt48_installation_resolve_device(x,device,&spec);if(st!=LT48_OK)return st;
    if(spec.catalogue.device_class!=2048 || strcmp(spec.definition_stem,"GDCC___"))return LT48_UNSUPPORTED;
    int legacy=spec.pinmap==0xc4&&((spec.definition_resource==0x20b&&spec.family_code==0x176)||
        (spec.definition_resource==0x20c&&spec.family_code==0x177));
    if(!legacy&&!lt48_pic18_admission_find(spec.definition_resource,spec.family_code,spec.pinmap,
        spec.catalogue.device_name,spec.catalogue.device_name_length))return LT48_UNSUPPORTED;
    st=lt48_pic18_j_open_candidate_internal(x,device,out);
    if(st!=LT48_UNSUPPORTED)return st;
    return pic18_open_profile(x,&spec,out);
}
static unsigned mask_at(const lt48_pic18_resources*r,unsigned a) {
    return lt48_pic_applet_image_mask(r->policy,r->mapping,a);
}
lt48_status lt48_pic18_validate(const lt48_pic18_resources*r,lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size) {
    if(!r)return LT48_INVALID_ARGUMENT;
    if(op<1||op>8)return LT48_UNSUPPORTED;
    if(start || length!=lt48_pic18_capacity(r))return LT48_SAFETY_REJECTED;
    if(r->j) {
        if((op==3||op==4||op==5||op==6)&&(!image||size!=length))return LT48_INVALID_ARGUMENT;
        return LT48_OK;
    }
    if(op==3||op==4||op==5||op==6) {
        if(!image || size!=r->policy->image_span)return LT48_INVALID_ARGUMENT;
        if((op==3||op==4)&&lt48_pic18_xx39_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)) {
            lt48_status st=lt48_pic18_xx39_configuration_safe(r->flash,image+r->policy->configuration_address,r->policy->configuration_count);
            if(st!=LT48_OK)return st;
        }
        for(unsigned a=0;a<r->policy->image_span;a++)if(!mask_at(r,a) && !(a>=r->policy->configuration_address && a-r->policy->configuration_address<r->policy->configuration_count) && image[a]!=255)return LT48_SAFETY_REJECTED;
    }
    return LT48_OK;
}
lt48_status lt48_pic18_validate_verify_options(const lt48_pic18_resources*r,lt48_operation_kind op,lt48_verify_margin_selection margin,unsigned type) {
    (void)op;if(!r||type>1||(margin!=LT48_VERIFY_MARGIN_ALL && margin!=LT48_VERIFY_MARGIN_NOMINAL && margin!=LT48_VERIFY_MARGIN_LOW && margin!=LT48_VERIFY_MARGIN_HIGH))return LT48_INVALID_ARGUMENT;return LT48_OK;
}
typedef struct pic18_session {
    const lt48_pic18_resources*r;const lt48_protocol_transport*api;void*context;
    unsigned char*image;unsigned mode;
    lt48_pic_source_session *source;
} pic18_session;
static lt48_status prepare_owner18(pic18_session *,lt48_pic_source_transport **,unsigned);
/* The shared owner has already reset, powered off and restored idle. Only
 * correct its prior Good LED when comparison subsequently finds a mismatch. */
static lt48_status report_failure18(pic18_session *s,lt48_status operation,lt48_pic18_result *out) {
    lt48_protocol_transport clean=*s->api;clean.cancelled=NULL;
    lt48_runtime_transport_session *wire=NULL;
    lt48_status st=lt48_runtime_transport_session_create(&clean,s->context,&wire);
    const unsigned char error[]={0x40,2};
    if(st==LT48_OK)st=lt48_runtime_transport_session_controller_write(wire,error,sizeof error);
    if(st==LT48_OK)st=lt48_runtime_transport_session_flush(wire);
    lt48_runtime_transport_session_close(wire);
    if(st!=LT48_OK){out->cleanup_status=st;out->cleanup_completed=0;}
    return operation;
}
/* Read code/EEPROM and its source READCFG tail in one powered owner. The
 * complete image remains staged until tail and unconditional cleanup succeed. */
static lt48_status read18(pic18_session *s,unsigned vcc,lt48_pic18_result *out) {
    const lt48_pic_applet_plan_profile *p=&s->r->profile->plan;
    lt48_pic_source_transport *owner=NULL;
    lt48_status st=prepare_owner18(s,&owner,vcc);
    lt48_pic_source_applet_stream_plan plan;
    lt48_pic_source_operation_result result={LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(st==LT48_OK)st=lt48_pic_applet_plan_validate(p,s->r->source,owner);
    if(st==LT48_OK)st=lt48_pic_applet_plan_make(p,1,0,s->r->mapping->dense_capacity,&plan);
    if(st==LT48_OK) {
        st=lt48_pic_source_transport_applet_read_tail_internal(owner,&plan,p->readcfg,
            s->image,s->r->policy->image_span,&result);
        out->cleanup_status=result.cleanup_status;
    } else out->cleanup_status=owner?lt48_pic_source_transport_cleanup_status_internal(owner):LT48_OK;
    out->cleanup_completed=out->cleanup_status==LT48_OK;
    lt48_pic_source_transport_close_internal(owner);return st;
}
static void compare18_region(pic18_session *s,const unsigned char *expected,int blank,
    uint32_t address,uint32_t count,const unsigned char *masks,lt48_pic18_result *out) {
    for(uint32_t i=0;i<count;i++) {
        uint32_t a=address+i;unsigned mask=masks?masks[i]:255;
        unsigned target=blank?255:expected[a];
        /* The exact K80 source ReadCfg callback converts an erased CONFIG2H
         * FF to57 before storing the image (reserved WDTPS encoding31->21).
         * Compare its erased image representation, retaining every mask bit.
         * This does not describe the physical erased byte, which remains7F
         * over the implemented bits in DS39972B table5-1. That same table
         * specifies CONFIG1H's unprogrammed value08, rather than maskDF. */
        const lt48_pic_applet_plan_profile *p=&s->r->profile->plan;
        if(blank&&(a==UINT32_C(0x300001)||a==UINT32_C(0x300003))&&
           lt48_pic18_k80_configuration_binding(p->definition,p->family,p->pinmap))
            target=a==UINT32_C(0x300001)?0x08:0x57;
        if((target^s->image[a])&mask) {
            if(a<out->first_mismatch)out->first_mismatch=a;
            out->mismatch_count++;
        }
    }
}
static lt48_status compare18(pic18_session*s,const unsigned char*expected,int blank,lt48_pic18_result*out) {
    /* Iterate physical regions, not the multi-megabyte sparse image holes. */
    for(size_t i=0;i<s->r->mapping->count;i++) {
        const lt48_pic_map_segment *region=s->r->mapping->segments+i;
        uint32_t starts[2],lengths[2];
        size_t count=lt48_pic_applet_image_storage_ranges(s->r->policy,region->image_offset,region->length,starts,lengths);
        for(size_t j=0;j<count;j++)compare18_region(s,expected,blank,starts[j],lengths[j],NULL,out);
    }
    const lt48_pic_applet_image_policy *p=s->r->policy;
    compare18_region(s,expected,blank,p->id_address,p->id_count,NULL,out);
    const lt48_pic_applet_plan_profile *plan=&s->r->profile->plan;
    const unsigned char *erase_mask=blank?lt48_pic18_f6x20_erase_mask(plan->definition,plan->family,plan->pinmap):NULL;
    if(blank&&!erase_mask)erase_mask=lt48_pic18_f6_erase_mask(plan->definition,plan->family,plan->pinmap);
    /* F6X20 and older F6 application configuration survives80h erase and is not part of
     * blank memory. Read/Compare/Verify still check every implemented bit. */
    compare18_region(s,expected,blank,p->configuration_address,p->configuration_count,erase_mask?erase_mask:p->configuration_mask,out);
    return out->mismatch_count?LT48_DEVICE_MISMATCH:LT48_OK;
}
/* Populate the source's configuration object through its pure Pre/Post pair.
 * The shared owner then programs that same canonical image. Conversion may
 * normalize reserved bits, but cannot alter code, EEPROM, IDs or implemented
 * configuration bits. Physical factory preservation needs an explicit policy
 * before any further family is admitted. */
static lt48_status complete18_configuration(void *context,lt48_pic_source_session *stage) {
    const lt48_pic18_resources *r=context;
    return lt48_pic_complete_configuration_fields(stage,r->legacy_composition,r->policy,r->mapping);
}
static lt48_status normalize18_configuration(const lt48_pic18_resources *r,lt48_pic_source_session *source) {
    if(r->legacy_composition)return lt48_pic_source_session_configuration_complete_internal(source,
        r->profile->plan.preconfig,r->profile->plan.postconfig,2,0,complete18_configuration,(void *)r);
    lt48_status st=lt48_pic_source_session_configuration_internal(source,r->profile->plan.preconfig,
        r->profile->plan.postconfig,2,0,NULL,0,NULL,0);
    if(st==LT48_OK&&lt48_pic18_xx39_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap)) {
        lt48_pic_source_state state;st=lt48_pic_source_session_state_internal(source,&state);
        unsigned char *copy=NULL;
        if(st==LT48_OK&&state.image_size!=r->policy->image_span)st=LT48_SAFETY_REJECTED;
        if(st==LT48_OK){copy=malloc(state.image_size);if(!copy)st=LT48_NO_MEMORY;}
        if(st==LT48_OK) {
            memcpy(copy,state.image,state.image_size);lt48_pic18_xx39_prepare_configuration(r->flash,copy+r->policy->configuration_address);
            state.image=copy;st=lt48_pic_source_session_restore_internal(source,&state);
        }
        free(copy);
    }
    return st;
}
static lt48_status preserve18_program(void *context,lt48_pic_source_session *source) {
    const lt48_pic18_resources *r=context;
    lt48_pic_preserve_span *spans=calloc(2*r->mapping->count+2,sizeof(*spans));
    if(!spans)return LT48_NO_MEMORY;
    size_t n=0;for(size_t i=0;i<r->mapping->count;i++) {
        const lt48_pic_map_segment *region=r->mapping->segments+i;
        uint32_t starts[2],lengths[2];
        size_t count=lt48_pic_applet_image_storage_ranges(r->policy,region->image_offset,region->length,starts,lengths);
        for(size_t j=0;j<count;j++)spans[n++]=(lt48_pic_preserve_span){starts[j],lengths[j],NULL};
    }
    if(r->policy->id_count)spans[n++]=(lt48_pic_preserve_span){r->policy->id_address,r->policy->id_count,NULL};
    spans[n++]=(lt48_pic_preserve_span){r->policy->configuration_address,r->policy->configuration_count,r->policy->configuration_mask};
    lt48_pic_preserve_policy guard={spans,n};const unsigned char *image=NULL;size_t size=0,required=0;
    lt48_status st=lt48_pic_source_session_image_internal(source,&image,&size);
    if(st==LT48_OK&&size!=r->policy->image_span)st=LT48_SAFETY_REJECTED;
    if(st==LT48_OK)st=lt48_pic_preserve_size(&guard,size,&required);
    unsigned char *snapshot=st==LT48_OK?malloc(required):NULL;
    if(st==LT48_OK&&!snapshot)st=LT48_NO_MEMORY;
    if(st==LT48_OK)st=lt48_pic_preserve_capture(&guard,image,size,snapshot,required);
    /* A rejected normalization must not leave changed configuration in the
     * caller's program session. Commit only after the preservation check. */
    lt48_pic_source_session *stage=NULL;
    if(st==LT48_OK)st=lt48_pic_source_session_clone_internal(source,&stage);
    if(st==LT48_OK)st=normalize18_configuration(r,stage);
    if(st==LT48_OK)st=lt48_pic_source_session_image_internal(stage,&image,&size);
    lt48_pic_preserve_result checked;
    if(st==LT48_OK)st=lt48_pic_preserve_check(&guard,image,size,snapshot,required,&checked);
    if(st==LT48_OK) {
        lt48_pic_source_state state;
        st=lt48_pic_source_session_state_internal(stage,&state);
        if(st==LT48_OK)st=lt48_pic_source_session_restore_internal(source,&state);
    }
    lt48_pic_source_session_close_internal(stage);
    free(snapshot);free(spans);return st;
}
static lt48_status prepare_owner18(pic18_session *s,lt48_pic_source_transport **owner,unsigned vcc) {
    const lt48_pic18_resources *r=s->r;const lt48_pic_applet_plan_profile *p=&r->profile->plan;
    *owner=NULL;
    lt48_pic_source_session_close_internal(s->source);s->source=NULL;
    lt48_runtime_options options={0};options.device_image=s->image;options.device_image_size=r->policy->image_span;
    options.device_element_bytes=p->element_width;options.use_explicit_buffer_ids=1;
    options.address_buffer=0;options.mode_buffer=8;options.parameter_buffer_id=p->data_in_buffer;options.root_timeout_seconds=10;
    lt48_status st=lt48_pic_source_resources_create_session_internal(r->source,&options,&s->source);
    if(st==LT48_OK&&s->mode==7&&r->runtime_policy)
        st=lt48_pic_source_raise_erase_wait_shape(s->source,11,r->runtime_policy->erase_us,
            r->runtime_policy->source_erase_magnitude);
    const lt48_pic_source_host_binding host={p->voltage_variable,r->profile->poweron,r->profile->poweroff};
    if(st==LT48_OK)st=lt48_pic_source_transport_create_internal(s->api,s->context,s->source,&host,
        vcc,r->policy->low_voltage,r->policy->high_voltage,owner);
    if(st==LT48_OK)st=lt48_pic_source_transport_idle_internal(*owner,r->idle,r->idle_size);
    if(st==LT48_OK)st=lt48_pic_applet_plan_prepare(p,r->source,*owner);
    return st;
}
static lt48_status protect18(pic18_session *s,lt48_pic18_result *out) {
    if(lt48_pic18_xx39_configuration_binding(s->r->profile->plan.definition,s->r->profile->plan.family,s->r->profile->plan.pinmap))
        lt48_pic18_xx39_prepare_configuration(s->r->flash,s->image+s->r->policy->configuration_address);
    lt48_pic_source_transport *owner=NULL;
    lt48_status st=prepare_owner18(s,&owner,s->r->policy->nominal_voltage);
    lt48_pic_source_operation_result result={LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(st==LT48_OK) {
        /* Use the audited whole-configuration worker: exact profile roles,
         * mode 13, ten-second callback, and the source DataIn success flag. */
        st=lt48_pic_applet_configuration_run(&s->r->profile->plan,s->r->source,owner,13,&result);
        out->cleanup_status=result.cleanup_status;
    } else out->cleanup_status=owner?lt48_pic_source_transport_cleanup_status_internal(owner):LT48_OK;
    out->cleanup_completed=out->cleanup_status==LT48_OK;
    lt48_pic_source_transport_close_internal(owner);
    return st;
}
static lt48_status erase18(pic18_session *s,lt48_pic18_result *out) {
    lt48_pic_source_transport *owner=NULL;
    lt48_status st=prepare_owner18(s,&owner,s->r->policy->nominal_voltage);
    lt48_pic_source_operation_result result={LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(st==LT48_OK) {
        st=lt48_pic_applet_erase_run(&s->r->profile->plan,s->r->source,owner,&result);
        out->cleanup_status=result.cleanup_status;
    } else out->cleanup_status=owner?lt48_pic_source_transport_cleanup_status_internal(owner):LT48_OK;
    out->cleanup_completed=out->cleanup_status==LT48_OK;
    lt48_pic_source_transport_close_internal(owner);return st;
}
static lt48_status program18(pic18_session *s,lt48_pic18_result *out) {
    const lt48_pic18_resources *r=s->r;const lt48_pic_applet_plan_profile *p=&r->profile->plan;
    if(lt48_pic18_xx39_configuration_binding(p->definition,p->family,p->pinmap))
        lt48_pic18_xx39_prepare_configuration(r->flash,s->image+r->policy->configuration_address);
    lt48_pic_source_transport *owner=NULL;
    lt48_status st=prepare_owner18(s,&owner,s->r->policy->nominal_voltage);
    lt48_pic_source_operation_result result={LT48_INVALID_ARGUMENT,LT48_OK,{0,UINT32_MAX},0};
    if(st==LT48_OK) {
        /* Retain independent physical low/high readback below. Configuration
         * is written unprotected before those checks; final protection is a
         * separate explicit pass. Shared source program ownership handles
         * startup, stream, configuration result and unconditional cleanup. */
        st=lt48_pic_applet_program_run(p,r->source,owner,UINT32_C(0x28),0,r->mapping->dense_capacity,
            NULL,0,preserve18_program,NULL,(void *)r,s->image,r->policy->image_span,&result);
        out->cleanup_status=result.cleanup_status;
    } else out->cleanup_status=owner?lt48_pic_source_transport_cleanup_status_internal(owner):LT48_OK;
    out->cleanup_completed=out->cleanup_status==LT48_OK;
    lt48_pic_source_transport_close_internal(owner);
    return st;
}
static lt48_status pic18_execute_legacy(const lt48_pic18_resources*r,const lt48_protocol_transport*api,void*context,lt48_operation_kind op,
    uint32_t start,size_t length,const unsigned char*image,size_t size,unsigned char*read,size_t read_size,
    lt48_verify_margin_selection margin,unsigned type,lt48_pic18_result*out) {
    if(!out||!api)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;
    lt48_status st=lt48_pic18_validate(r,op,start,length,image,size);if(st!=LT48_OK)return st;
    st=lt48_pic18_validate_verify_options(r,op,margin,type);if(st!=LT48_OK)return st;
    if(op==LT48_OPERATION_READ&&(!read||read_size<length))return LT48_BUFFER_TOO_SMALL;
    pic18_session s={0};s.r=r;s.api=api;s.context=context;s.image=malloc(length);
    unsigned char*expected=malloc(length);if(!s.image||!expected){free(s.image);free(expected);return LT48_NO_MEMORY;}
    memset(s.image,255,length);if(image)memcpy(expected,image,length);else memset(expected,255,length);
    if(op==LT48_OPERATION_READ||op==LT48_OPERATION_BLANK_CHECK||op==LT48_OPERATION_COMPARE||op==LT48_OPERATION_VERIFY) {
        unsigned passes=op==LT48_OPERATION_VERIFY&&margin==LT48_VERIFY_MARGIN_ALL?2:1;
        for(unsigned pass=0;st==LT48_OK&&pass<passes;pass++) {
            unsigned vcc=r->policy->nominal_voltage;if(op==LT48_OPERATION_VERIFY&&margin!=LT48_VERIFY_MARGIN_NOMINAL)vcc=(margin==LT48_VERIFY_MARGIN_HIGH||pass)?r->policy->high_voltage:r->policy->low_voltage;
            st=read18(&s,vcc,out);if(st==LT48_OK&&op!=LT48_OPERATION_READ)st=compare18(&s,expected,op==LT48_OPERATION_BLANK_CHECK,out);
        }
        if(st==LT48_OK&&op==LT48_OPERATION_READ)memcpy(read,s.image,length);
    } else if(op==LT48_OPERATION_ERASE) {
        const lt48_pic_applet_plan_profile *plan=&r->profile->plan;
        if(lt48_pic18_xx39_configuration_binding(plan->definition,plan->family,plan->pinmap)&&r->flash==12288) {
            st=read18(&s,r->policy->nominal_voltage,out);
            if(st==LT48_OK)st=lt48_pic18_xx39_configuration_safe(r->flash,s.image+r->policy->configuration_address,r->policy->configuration_count);
        }
        const unsigned char *erase_mask=lt48_pic18_f6x20_erase_mask(plan->definition,plan->family,plan->pinmap);
        if(!erase_mask)erase_mask=lt48_pic18_f6_erase_mask(plan->definition,plan->family,plan->pinmap);
        unsigned char before[14]={0};
        if(erase_mask) {
            if(r->policy->configuration_count!=sizeof(before))st=LT48_FORMAT_ERROR;
            if(st==LT48_OK)st=read18(&s,r->policy->nominal_voltage,out);
            if(st==LT48_OK)memcpy(before,s.image+r->policy->configuration_address,sizeof(before));
        }
        s.mode=7;if(st==LT48_OK)st=erase18(&s,out);
        if(st==LT48_OK)st=read18(&s,r->policy->nominal_voltage,out);
        if(st==LT48_OK)st=compare18(&s,NULL,1,out);
        if(st==LT48_OK&&erase_mask) {
            for(unsigned i=0;i<sizeof(before);i++) {
                uint32_t a=r->policy->configuration_address+i;
                unsigned mask=r->policy->configuration_mask[i]&~erase_mask[i];
                if((before[i]^s.image[a])&mask) {
                    if(a<out->first_mismatch)out->first_mismatch=a;
                    out->mismatch_count++;
                }
            }
            if(out->mismatch_count)st=LT48_DEVICE_MISMATCH;
        }
    } else if(op==LT48_OPERATION_SECURE) {
        unsigned char *masks=malloc(r->policy->configuration_count);
        unsigned char *snapshot=NULL;size_t snapshot_size=0;
        if(!masks)st=LT48_NO_MEMORY;
        if(st==LT48_OK)for(uint32_t i=0;i<r->policy->configuration_count;i++)masks[i]=(unsigned char)(r->policy->configuration_mask[i]&~r->policy->secure_mask[i]);
        lt48_pic_preserve_span spans[2];size_t span_count=0;
        if(r->policy->id_count)spans[span_count++]=(lt48_pic_preserve_span){r->policy->id_address,r->policy->id_count,NULL};
        spans[span_count++]=(lt48_pic_preserve_span){r->policy->configuration_address,r->policy->configuration_count,masks};
        lt48_pic_preserve_policy guard={spans,span_count};
        if(st==LT48_OK)st=lt48_pic_preserve_size(&guard,length,&snapshot_size);
        if(st==LT48_OK){snapshot=malloc(snapshot_size);if(!snapshot)st=LT48_NO_MEMORY;}
        if(st==LT48_OK)st=read18(&s,r->policy->nominal_voltage,out);
        if(st==LT48_OK)st=lt48_pic_preserve_capture(&guard,s.image,length,snapshot,snapshot_size);
        if(st==LT48_OK) {
            for(uint32_t i=0;i<r->policy->configuration_count;i++)s.image[r->policy->configuration_address+i]&=(unsigned char)~r->policy->secure_mask[i];
            if(lt48_pic18_xx39_configuration_binding(r->profile->plan.definition,r->profile->plan.family,r->profile->plan.pinmap))
                st=lt48_pic18_xx39_configuration_safe(r->flash,s.image+r->policy->configuration_address,r->policy->configuration_count);
            s.mode=13;if(st==LT48_OK)st=protect18(&s,out);
        }
        if(st==LT48_OK)st=read18(&s,r->policy->nominal_voltage,out);
        if(st==LT48_OK) {
            lt48_pic_preserve_result preserved;
            st=lt48_pic_preserve_check(&guard,s.image,length,snapshot,snapshot_size,&preserved);
            if(st==LT48_DEVICE_MISMATCH){out->first_mismatch=preserved.first_address;out->mismatch_count=preserved.mismatches;}
        }
        for(uint32_t i=0;st==LT48_OK&&i<r->policy->configuration_count;i++)if(s.image[r->policy->configuration_address+i]&r->policy->secure_mask[i])st=LT48_DEVICE_MISMATCH;
        free(snapshot);free(masks);
    } else {
        unsigned char *final_config=malloc(r->policy->configuration_count);
        if(!final_config){lt48_pic_source_session_close_internal(s.source);free(s.image);free(expected);return LT48_NO_MEMORY;}
        memcpy(final_config,expected+r->policy->configuration_address,r->policy->configuration_count);
        /* Delay all access-protection fields, not application configuration. */
        for(uint32_t i=0;i<r->policy->configuration_count;i++)expected[r->policy->configuration_address+i]|=r->policy->deferred_mask[i];
        memcpy(s.image,expected,length);s.mode=2;st=program18(&s,out);
        if(st==LT48_OK)st=read18(&s,r->policy->low_voltage,out);
        if(st==LT48_OK)st=compare18(&s,expected,0,out);
        if(st==LT48_OK)st=read18(&s,r->policy->high_voltage,out);
        if(st==LT48_OK)st=compare18(&s,expected,0,out);
        if(st==LT48_OK && memcmp(final_config,expected+r->policy->configuration_address,r->policy->configuration_count)) {
            memcpy(s.image,expected,length);memcpy(s.image+r->policy->configuration_address,final_config,r->policy->configuration_count);
            s.mode=13;st=protect18(&s,out);
            if(st==LT48_OK)st=read18(&s,r->policy->nominal_voltage,out);
            for(uint32_t i=0;st==LT48_OK&&i<r->policy->configuration_count;i++)if((s.image[r->policy->configuration_address+i]^final_config[i])&r->policy->configuration_mask[i])st=LT48_DEVICE_MISMATCH;
        }
        free(final_config);
    }
    out->operation_status=st;if(st==LT48_OK)out->completed_bytes=length;
    /* Readback comparison occurs after cleanup: correct its prior Good LED. */
    if(st!=LT48_OK && out->cleanup_completed)st=report_failure18(&s,st,out);
    lt48_pic_source_session_close_internal(s.source);free(s.image);free(expected);return st;
}

lt48_status lt48_pic18_execute_with_options(const lt48_pic18_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,
 unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,
 const lt48_pic18_execution_options *options,lt48_pic18_diagnostics *diagnostics,lt48_pic18_result *out) {
 if(diagnostics)memset(diagnostics,0,sizeof(*diagnostics));
 if(!out)return LT48_INVALID_ARGUMENT;
 memset(out,0,sizeof(*out));out->first_mismatch=UINT32_MAX;out->operation_status=LT48_INVALID_ARGUMENT;
 lt48_status st=lt48_pic18_validate(r,op,start,length,image,size);
 if(st==LT48_OK)st=lt48_pic18_validate_verify_options(r,op,margin,type);
 if(st==LT48_OK&&(!api||!api->read||!api->write))st=LT48_INVALID_ARGUMENT;
 if(st==LT48_OK&&options&&((options->flags&~3u)||(!options->id_decision&&options->id_context)))st=LT48_INVALID_ARGUMENT;
 if(st!=LT48_OK){out->operation_status=st;return st;}
 if(!r->j)return pic18_execute_legacy(r,api,context,op,start,length,image,size,read,read_size,margin,type,out);
 lt48_pic18_j_owner_result result={0};int flags=options?(int)options->flags:0;
 lt48_pic18_j_owner_set_id_decision(r->j,options?options->id_decision:NULL,options?options->id_context:NULL);
 if(op==LT48_OPERATION_PROGRAM||op==LT48_OPERATION_OVER_PROGRAM||op==LT48_OPERATION_SECURE)
  st=lt48_pic18_j_owner_mutate(r->j,api,context,op,flags,op==LT48_OPERATION_SECURE?NULL:image,
      op==LT48_OPERATION_SECURE?0:size,options?options->preservation_backup:NULL,options?options->preservation_backup_size:0,&result);
 else if(op==LT48_OPERATION_VERIFY)st=lt48_pic18_j_owner_verify(r->j,api,context,flags,image,size,margin,type,&result);
 else st=lt48_pic18_j_owner_memory(r->j,api,context,op,flags,image,size,read,read_size,&result);
 lt48_pic18_j_owner_set_id_decision(r->j,NULL,NULL);*out=result.operation;
 if(diagnostics){diagnostics->observed_id=result.observed_id;diagnostics->id_mismatch=result.id_mismatch;diagnostics->protected_read=result.protected_read;diagnostics->configuration_from_mirrors=result.configuration_from_mirrors;diagnostics->backup_valid=result.backup_valid;memcpy(diagnostics->missing_contacts,result.contact.missing,sizeof(diagnostics->missing_contacts));memcpy(diagnostics->unexpected_contacts,result.contact.unexpected,sizeof(diagnostics->unexpected_contacts));}
 return st;
}
lt48_status lt48_pic18_execute(const lt48_pic18_resources *r,const lt48_protocol_transport *api,void *context,
 lt48_operation_kind op,uint32_t start,size_t length,const unsigned char *image,size_t size,
 unsigned char *read,size_t read_size,lt48_verify_margin_selection margin,unsigned type,lt48_pic18_result *out) {
 return lt48_pic18_execute_with_options(r,api,context,op,start,length,image,size,read,read_size,margin,type,NULL,NULL,out);
}
