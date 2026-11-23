#ifndef LT48_PIC18_XX39_TIMING_H
#define LT48_PIC18_XX39_TIMING_H
#include "source_resources.h"
#include "pic18_x220_runtime.h"
#include <string.h>
/* Authored DS30480C P11 correction. Exact BYO13.10 bindings only; changes
 * owned time variable10 from7000 to11000us before callbacks are exposed.
 * No source archive, voltage, pin map, applet or nonvolatile data is changed. */
static inline int lt48_pic18_xx39_timing_allowed(const lt48_device_spec *s) {
 if(!s||s->catalogue.device_class!=2048||!s->definition_stem||strcmp(s->definition_stem,"GDCC___")||!s->catalogue.device_name)return 0;
 if(s->definition_resource==28u&&s->family_code==22u&&s->pinmap==10u&&s->catalogue.device_name_length==10u&&!memcmp(s->catalogue.device_name,"PIC18F2439",10u))return 1;
 if(s->definition_resource==28u&&s->family_code==22u&&s->pinmap==10u&&s->catalogue.device_name_length==16u&&!memcmp(s->catalogue.device_name,"PIC18F2439 *28SO",16u))return 1;
 if(s->definition_resource==29u&&s->family_code==22u&&s->pinmap==10u&&s->catalogue.device_name_length==10u&&!memcmp(s->catalogue.device_name,"PIC18F2539",10u))return 1;
 if(s->definition_resource==29u&&s->family_code==22u&&s->pinmap==10u&&s->catalogue.device_name_length==16u&&!memcmp(s->catalogue.device_name,"PIC18F2539 *28SO",16u))return 1;
 if(s->definition_resource==30u&&s->family_code==23u&&s->pinmap==11u&&s->catalogue.device_name_length==10u&&!memcmp(s->catalogue.device_name,"PIC18F4439",10u))return 1;
 if(s->definition_resource==31u&&s->family_code==24u&&s->pinmap==12u&&s->catalogue.device_name_length==16u&&!memcmp(s->catalogue.device_name,"PIC18F4439 *44TQ",16u))return 1;
 if(s->definition_resource==32u&&s->family_code==25u&&s->pinmap==13u&&s->catalogue.device_name_length==17u&&!memcmp(s->catalogue.device_name,"PIC18F4439 @44QFN",17u))return 1;
 if(s->definition_resource==33u&&s->family_code==23u&&s->pinmap==11u&&s->catalogue.device_name_length==10u&&!memcmp(s->catalogue.device_name,"PIC18F4539",10u))return 1;
 if(s->definition_resource==34u&&s->family_code==24u&&s->pinmap==12u&&s->catalogue.device_name_length==16u&&!memcmp(s->catalogue.device_name,"PIC18F4539 *44TQ",16u))return 1;
 if(s->definition_resource==35u&&s->family_code==25u&&s->pinmap==13u&&s->catalogue.device_name_length==17u&&!memcmp(s->catalogue.device_name,"PIC18F4539 @44QFN",17u))return 1;
 return 0;
}
static inline lt48_status lt48_pic18_xx39_timing_open(const char *path,const unsigned char digest[32],const lt48_device_spec *s,lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 static const unsigned char expected[32]={0xc0,0xe9,0x0a,0x55,0xe9,0x7a,0x3a,0xc1,0x2d,0x73,0x00,0x6e,0xb9,0xda,0x11,0x39,0xe3,0xd5,0x68,0x8a,0xef,0x85,0x24,0x2b,0x00,0x01,0xbc,0x40,0x6c,0xab,0x46,0x40};
 if(!digest||memcmp(digest,expected,32)||!lt48_pic18_xx39_timing_allowed(s))return LT48_SAFETY_REJECTED;
 const lt48_pic_source_time_override correction={{1,10,1,1,70},{1,10,1,1,110}};
 return lt48_pic_source_resources_time_library_internal(path,digest,s->family_code,s->pinmap,&correction,1,out);
}
/* Full owned XX39 correction: same guarded configuration RAM relocation used
 * for X220, plus the manufacturer-required host time correction. */
static inline lt48_status lt48_pic18_xx39_runtime_open(const char *path,const unsigned char digest[32],
 const lt48_device_spec *s,lt48_pic_source_resources **out) {
 if(!out)return LT48_INVALID_ARGUMENT;
 *out=NULL;
 if(!lt48_pic18_xx39_timing_allowed(s))return LT48_SAFETY_REJECTED;
 const lt48_pic_source_time_override correction={{1,10,1,1,70},{1,10,1,1,110}};
 /* timing_open enforces the exact archive; close its temporary owner before
  * constructing the combined immutable owner. */
 lt48_pic_source_resources *check=NULL;
 lt48_status st=lt48_pic18_xx39_timing_open(path,digest,s,&check);
 lt48_pic_source_resources_close_internal(check);
 if(st!=LT48_OK)return st;
 return lt48_x220_runtime_time_open(path,digest,s->family_code,s->pinmap,&correction,1,out);
}
#endif
