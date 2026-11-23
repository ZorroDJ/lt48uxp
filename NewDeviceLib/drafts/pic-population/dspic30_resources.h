#ifndef LT48_DSPIC30_RESOURCES_H
#define LT48_DSPIC30_RESOURCES_H
#include "pic24_ka_resources.h"
#include "dspic30_policy.h"
typedef struct lt48_dspic30_resources lt48_dspic30_resources;
lt48_status lt48_dspic30_resources_open(const char *,const char *,lt48_dspic30_resources **);
void lt48_dspic30_resources_close(lt48_dspic30_resources *);
const lt48_dspic30_image_policy *lt48_dspic30_resources_geometry(const lt48_dspic30_resources *);
const lt48_dspic30_configuration_rules *lt48_dspic30_resources_rules(const lt48_dspic30_resources *);
const lt48_pic_source_resources *lt48_dspic30_resources_source(const lt48_dspic30_resources *,int);
lt48_status lt48_dspic30_resources_configuration_edit(const lt48_dspic30_resources *,unsigned char *,size_t,const lt48_runtime_configuration_value *,size_t);
#endif
