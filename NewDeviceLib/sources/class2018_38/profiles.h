/* Authored exact bindings. Original callbacks and FPGA payloads remain BYO. */
#include "admission.h"
static const struct mcu70_profile*cl38_binding(const lt48_device_spec*s){
 static const struct mcu70_profile rows[]={
 {"Dallas:DS89C420",0xf4,128,58,40,16384,64,7,11,3,218,0,1,2,0,0,0},
 {"Atmel:T89C51AC2 *44",0xea,120,51,44,65536,0,12,10,3,0,0,1,2,0,0,1},
 {"Atmel:T89C51AC2 *44TQ",0xea,120,51,44,65536,0,12,10,3,0,0,1,2,0,0,1},
 {"Atmel:T89C51CC01 *44",0xea,120,51,44,65536,0,12,10,3,0,0,1,2,0,0,1},
 {"Atmel:T89C51CC01 *44TQ",0xea,120,51,44,65536,0,12,10,3,0,0,1,2,0,0,1},
 {"Dallas:DS89C420 *44",0xf5,129,59,44,16384,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C420 *44TQ",0xf5,129,59,44,16384,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C430",0xf6,130,58,40,16384,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C430 *44",0xf7,131,59,44,16384,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C430 *44TQ",0xf7,131,59,44,16384,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C440",0xf8,132,58,40,32768,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C440 *44",0xf9,133,59,44,32768,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C440 *44TQ",0xf9,133,59,44,32768,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C450",0xfa,134,58,40,65536,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C450 *44",0xfb,135,59,44,65536,64,7,11,3,218,0,1,2,0,0,0},
 {"Dallas:DS89C450 *44TQ",0xfb,135,59,44,65536,64,7,11,3,218,0,1,2,0,0,0},
 {"Infineon.siemens:SABC505L-4E @80",0xdc,112,44,48,32768,0,12,9,3,0,0,1,2,0,0,1},
 {"Macronix:MX10FLCDPC",0xbe,93,27,40,65536,0,12,9,3,0,0,1,2,0,0,1},
 {"Macronix:MX10FMAXDPC",0xbe,93,27,40,65536,0,12,9,3,0,0,1,2,0,0,1},
 {"Macronix:MX10FMAXDQC *44",0xbf,94,28,44,65536,0,12,9,3,0,0,1,2,0,0,1},
 {"Philips:P89C138",0xb8,87,27,40,16384,0,12,9,3,0,0,1,2,0,0,1},
 {"Philips:P89C138 *44",0xb9,88,28,44,16384,0,12,9,3,0,0,1,2,0,0,1},
 {"Philips:P89C238",0xba,89,27,40,32768,0,12,9,3,0,0,1,2,0,0,1},
 {"Philips:P89C238 *44",0xbb,90,28,44,32768,0,12,9,3,0,0,1,2,0,0,1},
 {"Philips:P89C52 ",0xc6,97,31,40,8192,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C52 *44",0xc7,98,32,44,8192,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C535 *44",0xc0,95,29,44,8192,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C535 *44Q",0xc3,95,29,44,8192,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C536 *44",0xc1,96,30,44,16384,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C536 *44Q",0xc4,96,30,44,16384,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C538 *44",0xc2,96,30,44,65536,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C538 *44Q",0xc5,96,30,44,65536,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C54 ",0xc8,97,31,40,16384,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C54 *44",0xc9,98,32,44,16384,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C738",0xbc,91,27,40,65536,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:P89C738 *44",0xbd,92,28,44,65536,0,12,8,0,0,0,1,2,0,0,1},
 {"Philips:PXA-G39 *44",0xfc,136,60,44,32768,0,12,11,3,0,0,1,2,0,0,1},
 {"Philips:PXA-G49 *44",0xfd,137,60,44,65536,0,12,11,3,0,0,1,2,0,0,1},
 };
 if(s->catalogue.device_class!=2018||strcmp(s->definition_stem,"GAF____")||s->element_bits!=8||s->secondary_element_bits!=8)return NULL;
 char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);
 for(size_t i=0;i<sizeof rows/sizeof*rows;i++){const struct mcu70_profile*r=rows+i;if(!strcmp(key,r->key)&&s->definition_resource==r->d&&s->family_code==r->m&&s->pinmap==r->p&&s->pin_count==r->pins&&s->capacity_bytes==r->size&&s->secondary_element_count==r->secondary)return r;}
 return NULL;
}
