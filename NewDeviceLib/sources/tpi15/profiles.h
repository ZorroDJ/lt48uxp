/* Original class2038 TPI, exact BYO bindings. */
#include "admission.h"
static int tpi15_binding(const lt48_catalogue_entry*e){
 if(!e->vendor)return 0;
 static const struct{const char*vendor,*name;unsigned cls,d,man,id;}rows[]={
{"Atmel","ATTINY10 *6SOT",2038,347,30,912},
{"Atmel","ATTINY10 *8UDFN",2038,349,30,912},
{"Atmel","ATTINY102 *8SO",2038,350,30,3216},
{"Atmel","ATTINY102 *8UDFN",2038,350,30,3216},
{"Atmel","ATTINY104 *14SO",2038,351,30,2960},
{"Atmel","ATTINY20 *14SO",2038,352,30,3985},
{"Atmel","ATTINY20 *14TSS",2038,352,30,3985},
{"Atmel","ATTINY4 *6SOT",2038,346,30,2703},
{"Atmel","ATTINY4 *8UDFN",2038,348,30,2703},
{"Atmel","ATTINY40 *20SO",2038,353,30,3730},
{"Atmel","ATTINY40 *20TSS",2038,353,30,3730},
{"Atmel","ATTINY5 *6SOT",2038,346,30,2447},
{"Atmel","ATTINY5 *8UDFN",2038,348,30,2447},
{"Atmel","ATTINY9 *6SOT",2038,347,30,2192},
{"Atmel","ATTINY9 *8UDFN",2038,349,30,2192},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(e->device_class==rows[i].cls&&e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&!strcmp(e->vendor,rows[i].vendor)&&e->device_name_length==strlen(rows[i].name)&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;
}
static const uint32_t tpi15_none[]={0};
static const struct xp_closure tpi15_closures[]={
{347,221,139,6,1024,500,tpi15_none,1},
{349,222,140,8,1024,500,tpi15_none,1},
{350,223,141,8,1024,500,tpi15_none,1},
{351,224,142,14,1024,500,tpi15_none,1},
{352,225,143,14,2048,500,tpi15_none,1},
{346,221,139,6,512,500,tpi15_none,1},
{348,222,140,8,512,500,tpi15_none,1},
{353,226,144,20,4096,500,tpi15_none,1},
};
