#include "configuration.h"
#include <stdlib.h>
#include <string.h>

int lt48_pic_configuration_compose_internal(
    const lt48_pic_configuration_composition *contract,
    const unsigned char *input,size_t span,
    const lt48_pic_composition_value *values,size_t count,unsigned char *output) {
    return lt48_pic_configuration_compose(contract,input,span,values,count,output);
}

static lt48_status pure_conversion(const lt48_runtime_result *result) {
    for(size_t n=0;n<lt48_runtime_event_count(result);n++) {
        lt48_runtime_event event;
        lt48_status st=lt48_runtime_get_event(result,n,&event);if(st!=LT48_OK)return st;
        switch(event.kind) {
        case LT48_RUNTIME_EVALUATE_VARIABLE:
        case LT48_RUNTIME_BUFFER_ARITHMETIC:
        case LT48_RUNTIME_BUFFER_BIT_COPY:
        case LT48_RUNTIME_BUFFER_MEMMOVE:
        case LT48_RUNTIME_BUFFER_BYTE_OPERATION:
        case LT48_RUNTIME_BUFFER_FILL:break;
        case LT48_RUNTIME_RETURN_STATUS:if(!event.arguments[0])return LT48_DEVICE_MISMATCH;break;
        default:return LT48_UNSUPPORTED;
        }
    }
    return LT48_OK;
}

lt48_status lt48_pic_configuration_edit_internal(
    const lt48_compiled_wave *pre,const lt48_compiled_wave *post,
    const lt48_pin_map *pins,const lt48_compile_variable *variables,size_t variable_count,
    unsigned element_bytes,const unsigned char *input,size_t span,unsigned char *output,
    const lt48_pic_configuration_field *fields,size_t field_count,
    const lt48_runtime_configuration_value *edits,size_t edit_count) {
    if(!pre||!post||!pins||!input||!output||!span||!element_bytes||element_bytes>4||
       (variable_count&&!variables)||variable_count>SIZE_MAX/sizeof(*variables)||
       (field_count&&!fields)||(edit_count&&!edits)||edit_count>1024||field_count>1024)return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<edit_count;i++) {
        const lt48_runtime_configuration_value *edit=&edits[i];int found=0;
        if(edit->object_id!=-1)return LT48_UNSUPPORTED;
        for(size_t j=0;j<i;j++)if(edits[j].category==edit->category && edits[j].field==edit->field)return LT48_INVALID_ARGUMENT;
        for(size_t j=0;j<field_count;j++)if(fields[j].category==edit->category && fields[j].field==edit->field) {
            if(found || fields[j].minimum>fields[j].maximum || edit->value<fields[j].minimum || edit->value>fields[j].maximum)return LT48_INVALID_ARGUMENT;
            found=1;
        }
        if(!found)return LT48_UNSUPPORTED;
    }
    lt48_compile_variable *vars=calloc(variable_count?variable_count:1,sizeof(*vars));
    if(!vars)return LT48_NO_MEMORY;
    if(variable_count)memcpy(vars,variables,variable_count*sizeof(*vars));
    lt48_runtime_result *results[2]={NULL,NULL};lt48_status st;
    lt48_runtime_options opts={0};opts.device_image=input;opts.device_image_size=span;
    opts.device_element_bytes=element_bytes;opts.max_steps=100000;
#define TRY(expr) do{st=(expr);if(st!=LT48_OK)goto done;}while(0)
    TRY(lt48_runtime_interpret(pre,pins,vars,variable_count,&opts,&results[0]));
    TRY(pure_conversion(results[0]));
    const lt48_runtime_configuration_value *configuration=NULL;size_t count=0;
    TRY(lt48_runtime_get_configuration(results[0],&configuration,&count));
    lt48_runtime_configuration_value edited[1024];
    if(count>1024){st=LT48_RUNTIME_ERROR;goto done;}
    if(count)memcpy(edited,configuration,count*sizeof(*edited));
    for(size_t i=0;i<edit_count;i++) {
        size_t found=SIZE_MAX;
        for(size_t j=0;j<count;j++)if(edited[j].object_id==edits[i].object_id && edited[j].category==edits[i].category && edited[j].field==edits[i].field)found=j;
        if(found==SIZE_MAX){st=LT48_UNSUPPORTED;goto done;} /* Pre must establish the field binding. */
        edited[found].value=edits[i].value;
    }
    opts.configuration=edited;opts.configuration_count=count;
    const unsigned char *pre_image=NULL;size_t pre_span=0;
    TRY(lt48_runtime_get_image(results[0],&pre_image,&pre_span));
    if(pre_span!=span){st=LT48_RUNTIME_ERROR;goto done;}
    opts.device_image=pre_image;
    lt48_runtime_buffer_seed seeds[256];size_t seed_count=0;
    for(unsigned i=0;i<256;i++) {
        const unsigned char *data=NULL;size_t size=0;
        st=lt48_runtime_get_buffer(results[0],(uint8_t)i,&data,&size);
        if(st==LT48_NOT_FOUND)break;
        if(st!=LT48_OK)goto done;
        seeds[seed_count++]=(lt48_runtime_buffer_seed){(uint8_t)i,data,size};
    }
    opts.initial_buffers=seeds;opts.initial_buffer_count=seed_count;
    for(size_t i=0;i<variable_count;i++)TRY(lt48_runtime_get_variable(results[0],vars[i].variable_id,&vars[i]));
    TRY(lt48_runtime_interpret(post,pins,vars,variable_count,&opts,&results[1]));
    TRY(pure_conversion(results[1]));
    const unsigned char *converted=NULL;size_t size=0;
    TRY(lt48_runtime_get_image(results[1],&converted,&size));
    if(size!=span){st=LT48_RUNTIME_ERROR;goto done;}
    memcpy(output,converted,span);
done:
    for(unsigned i=0;i<2;i++)lt48_runtime_result_close(results[i]);
    free(vars);return st;
#undef TRY
}

lt48_status lt48_pic_configuration_normalize_internal(
    const lt48_compiled_wave *pre,const lt48_compiled_wave *post,
    const lt48_pin_map *pins,const lt48_compile_variable *variables,size_t variable_count,
    unsigned element_bytes,const unsigned char *input,size_t span,unsigned char *output) {
    return lt48_pic_configuration_edit_internal(pre,post,pins,variables,variable_count,
        element_bytes,input,span,output,NULL,0,NULL,0);
}
