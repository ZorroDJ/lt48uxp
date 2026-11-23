#ifndef LT48_DRAFT_PIC24_KA_RESOURCES_H
#define LT48_DRAFT_PIC24_KA_RESOURCES_H
#include "pic24_ka_configuration.h"
#include "../../sources/pic-common/source_resources.h"
typedef struct lt48_pic24_ka_resources lt48_pic24_ka_resources;
/* Construction/editor only; exact audited sixteen-alias table. Owns original
 * code/configuration and authored EEPROM-erase resource owners. A runtime
 * owner must sequence BOTH erases, verify independently and apply protection
 * last. No operation or GUI admission is granted by this constructor. */
lt48_status lt48_pic24_ka_resources_open(const char *,const char *,lt48_pic24_ka_resources **);
void lt48_pic24_ka_resources_close(lt48_pic24_ka_resources *);
const lt48_pic24_ka_image_policy *lt48_pic24_ka_resources_geometry(const lt48_pic24_ka_resources *);
const lt48_pic_source_resources *lt48_pic24_ka_resources_source(const lt48_pic24_ka_resources *,int);
lt48_status lt48_pic24_ka_resources_configuration_edit(const lt48_pic24_ka_resources *,unsigned char *,size_t,
 const lt48_runtime_configuration_value *,size_t);
#endif
