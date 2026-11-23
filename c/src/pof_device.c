#include <labtool48/labtool48.h>
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Selected-source GC/GCB facade. File operations need only REZ data; device
 * operations additionally require the exact imported private runtime. */
struct lt48_pof_device {
    lt48_pal_resources *resources;
    lt48_pof_trs_source *source;
    lt48_status runtime_status;
};
void lt48_pof_device_close(lt48_pof_device *d) {
    if(!d)return;
    lt48_pal_resources_close(d->resources);
    lt48_pof_trs_source_close_internal(d->source);free(d);
}
lt48_status lt48_pof_device_open(const lt48_installation *installation,size_t index,lt48_pof_device **out) {
    if(!installation||!out)return LT48_INVALID_ARGUMENT;
    *out=NULL;lt48_device_spec spec;
    lt48_status st=lt48_installation_resolve_device(installation,index,&spec);
    if(st!=LT48_OK)return st;
    const unsigned tuples[][4]={{2026,0,0,0},{2026,2,2,2},{2026,3,3,3},
        {2026,4,4,4},{2026,5,5,5},{2026,16,16,8},{2026,17,17,9},
        {2028,0,0,0},{2028,1,1,1}};
    int selected=0;
    for(size_t i=0;i<sizeof tuples/sizeof tuples[0];i++)
        if(spec.catalogue.device_class==tuples[i][0]&&spec.definition_resource==tuples[i][1]&&
           spec.family_code==tuples[i][2]&&spec.pinmap==tuples[i][3])selected=1;
    if(!selected)return LT48_UNSUPPORTED;
    if(!spec.definition_stem||strcmp(spec.definition_stem,spec.catalogue.device_class==2026?"GC_____":"GCB____"))return LT48_DEVICE_MISMATCH;
    char path[4096];int n=snprintf(path,sizeof path,"%s/%s.rez",lt48_installation_path(installation),spec.definition_stem);
    if(n<0||(size_t)n>=sizeof path)return LT48_INVALID_ARGUMENT;
    lt48_pof_device *d=calloc(1,sizeof(*d));if(!d)return LT48_NO_MEMORY;
    st=lt48_pal_resources_open_definition(path,spec.definition_resource,spec.family_code,spec.pinmap,&d->resources);
    if(st==LT48_OK&&!lt48_pal_resources_pof_core_internal(d->resources))st=LT48_FORMAT_ERROR;
    if(st==LT48_OK){
        d->runtime_status=lt48_pof_trs_source_open_internal(lt48_installation_path(installation),&d->source);
        if(d->runtime_status==LT48_OK)d->runtime_status=lt48_pal_resources_pof_source_internal(d->resources,d->source);
        if(d->runtime_status!=LT48_OK&&d->runtime_status!=LT48_NOT_FOUND)st=d->runtime_status;
    }
    if(st!=LT48_OK){lt48_pof_device_close(d);return st;}
    *out=d;return LT48_OK;
}
size_t lt48_pof_device_capacity(const lt48_pof_device *d) {
    const lt48_pof_core *c=d?lt48_pal_resources_pof_core_internal(d->resources):NULL;
    return c?c->total_bytes:0;
}
lt48_status lt48_pof_device_runtime_status(const lt48_pof_device *d){return d?d->runtime_status:LT48_INVALID_ARGUMENT;}
lt48_status lt48_pof_device_validate(const lt48_pof_device *d,lt48_operation_kind op,
    const unsigned char *input,size_t size) {
    if(!d)return LT48_INVALID_ARGUMENT;
    switch(op) {
    case LT48_OPERATION_READ:case LT48_OPERATION_BLANK_CHECK:case LT48_OPERATION_SECURE:break;
    case LT48_OPERATION_PROGRAM:case LT48_OPERATION_OVER_PROGRAM:
    case LT48_OPERATION_VERIFY:case LT48_OPERATION_COMPARE:
        if(!input||size!=lt48_pof_device_capacity(d))return LT48_INVALID_ARGUMENT;
        break;
    default:return LT48_UNSUPPORTED;
    }
    return d->runtime_status;
}
lt48_status lt48_pof_device_execute(const lt48_pof_device *d,const lt48_protocol_transport *api,
    void *context,lt48_operation_kind op,const unsigned char *input,size_t input_size,
    unsigned char *output,size_t output_size,lt48_pof_operation_result *result) {
    if(!result)return LT48_INVALID_ARGUMENT;
    memset(result,0,sizeof(*result));result->traversal.first_mismatch=SIZE_MAX;
    lt48_status st=lt48_pof_device_validate(d,op,input,input_size);
    if(st!=LT48_OK){result->operation_status=st;return st;}
    st=lt48_pof_execute_internal(d->resources,d->source,api,context,op,input,input_size,output,output_size,result);
    result->operation_status=st;return st;
}
lt48_status lt48_pof_device_import(const lt48_pof_device *d,const unsigned char *file,size_t size,
    unsigned char *image,size_t capacity,int *security) {
    if(!d)return LT48_INVALID_ARGUMENT;
    return lt48_pof_core_import_internal(lt48_pal_resources_pof_core_internal(d->resources),file,size,image,capacity,security);
}
lt48_status lt48_pof_device_export(const lt48_pof_device *d,const unsigned char *image,size_t size,
    int security,unsigned char *file,size_t capacity,size_t *written) {
    if(!d)return LT48_INVALID_ARGUMENT;
    return lt48_pof_core_export_internal(lt48_pal_resources_pof_core_internal(d->resources),image,size,security,file,capacity,written);
}

lt48_status lt48_pof_device_import_metadata(const lt48_pof_device *d,const unsigned char *file,size_t size,
    unsigned char *image,size_t capacity,lt48_pof_file_metadata *metadata) {
    if(!d)return LT48_INVALID_ARGUMENT;
    return lt48_pof_core_import_metadata_internal(lt48_pal_resources_pof_core_internal(d->resources),file,size,image,capacity,metadata);
}
lt48_status lt48_pof_device_export_metadata(const lt48_pof_device *d,const unsigned char *image,size_t size,
    const lt48_pof_file_metadata *metadata,unsigned char *file,size_t capacity,size_t *written) {
    if(!d)return LT48_INVALID_ARGUMENT;
    return lt48_pof_core_export_metadata_internal(lt48_pal_resources_pof_core_internal(d->resources),image,size,metadata,file,capacity,written);
}

lt48_status lt48_pof_device_validate_vectors(const lt48_pof_device *d,
    const lt48_pof_file_metadata *vectors,unsigned margin,unsigned x_mode) {
    if(!d)return LT48_INVALID_ARGUMENT;
    lt48_status status=lt48_pof_vectors_validate_internal(d->resources,vectors,margin,x_mode);
    return status==LT48_OK?d->runtime_status:status;
}
lt48_status lt48_pof_device_execute_vectors(const lt48_pof_device *d,
    const lt48_pof_file_metadata *vectors,unsigned margin,unsigned x_mode,
    const lt48_protocol_transport *api,void *context,lt48_vector_operation_result *result) {
    if(!result)return LT48_INVALID_ARGUMENT;
    memset(result,0,sizeof(*result));result->first_mismatch_row=SIZE_MAX;
    lt48_status status=lt48_pof_device_validate_vectors(d,vectors,margin,x_mode);
    if(status==LT48_OK)
        status=lt48_pof_vectors_execute_internal(d->resources,vectors,margin,x_mode,api,context,result);
    result->operation_status=status;return status;
}

lt48_status lt48_pof_device_blank_value(const lt48_pof_device *d,unsigned char *value) {
    if(!d||!value)return LT48_INVALID_ARGUMENT;
    *value=lt48_pal_resources_blank_fuse(d->resources);return LT48_OK;
}
