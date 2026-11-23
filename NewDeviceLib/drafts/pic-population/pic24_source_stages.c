#include "pic24_source_stages.h"
#include <string.h>
static int integer(const lt48_compile_variable *v,size_t n,unsigned id,uint32_t *out){
 unsigned found=0;for(size_t i=0;i<n;i++)if(v[i].variable_id==id){
  if(v[i].kind!=2 || found++)return 0;
 *out=(uint32_t)v[i].value|((uint32_t)v[i].channel<<16);
 }return found==1;
}
lt48_status lt48_pic24_source_stage_requests(const lt48_pic_source_resources *source,
 enum lt48_pic_applet_program_phase phase,const lt48_pic24_program_view *view,lt48_runtime_result **out){
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!source || !view || !view->geometry || !view->image || view->image_size!=view->geometry->source_image_bytes)return LT48_INVALID_ARGUMENT;
 if(view->configuration_words!=view->geometry->configuration_words ||
    view->mandatory_rule_family>LT48_PIC24_GB2 ||
    view->configuration_words!=(view->mandatory_rule_family==LT48_PIC24_GA0?2u:4u))return LT48_SAFETY_REJECTED;
 if(view->program.first_image_byte || view->program.first_target_address ||
    view->program.image_byte_count!=view->image_size || view->program.source_header[2] ||
    view->program.source_header[3] || view->program.source_header[4])return LT48_SAFETY_REJECTED;
 unsigned role,mode;
 switch(phase){
 case LT48_PIC_APP_PROGRAM_PROGRESS:role=8;mode=2;break;
 case LT48_PIC_APP_PROGRAM_ERASE:role=10;mode=7;break;
 case LT48_PIC_APP_PROGRAM_VERIFY:role=9;mode=4;break;
 case LT48_PIC_APP_PROGRAM_PROTECT:role=11;mode=13;break;
 default:return LT48_UNSUPPORTED;
 }
 const lt48_pin_map *pins;const lt48_compile_variable *variables;size_t count;
 lt48_status st=lt48_pic_source_resources_bindings_internal(source,&pins,&variables,&count);
 if(st!=LT48_OK)return st;
 uint32_t page,address,limit,bank;
 if(!integer(variables,count,19,&page) || page!=64 || !integer(variables,count,16,&address) ||
    address!=view->geometry->configuration_first_target_address || !integer(variables,count,20,&limit) ||
    limit!=view->geometry->configuration_first_image_byte || !integer(variables,count,23,&bank) ||
    bank!=view->program.source_header[8] || (uint64_t)limit+4*view->configuration_words!=view->image_size)return LT48_SAFETY_REJECTED;
 uint32_t device;int has_device=integer(variables,count,18,&device);
 if(has_device!=(view->mandatory_rule_family>=LT48_PIC24_GA2) ||
    (has_device && device!=(view->mandatory_rule_family==LT48_PIC24_GB2)))return LT48_SAFETY_REJECTED;
 lt48_logical_pin selector;
 if(lt48_pin_map_find_logical(pins,2,&selector)!=LT48_OK || selector.engine_pin!=view->program.source_header[1])return LT48_SAFETY_REJECTED;
 const lt48_compiled_wave *compiled=NULL;
 for(size_t i=0;i<lt48_pic_source_resources_count_internal(source);i++){
  lt48_pic_source_resource_entry entry;st=lt48_pic_source_resources_entry_internal(source,i,&entry);
  if(st!=LT48_OK)return st;
  if(entry.role==role){if(compiled || entry.source.wave_type!=0 || !entry.compiled)return LT48_SAFETY_REJECTED;compiled=entry.compiled;}
 }
 if(!compiled)return LT48_UNSUPPORTED;
 lt48_runtime_options options={0};options.mode=mode;
 options.use_explicit_buffer_ids=1;options.address_buffer=1;options.mode_buffer=8;
 options.device_image=view->image;options.device_image_size=view->image_size;options.device_element_bytes=1;
 return lt48_runtime_interpret(compiled,pins,variables,count,&options,out);
}
