/* Exact EMTC class2041 bindings, not name-prefix admission. */
#include "admission.h"
static int emtc18_binding(const lt48_catalogue_entry*e){
 if(e->device_class!=2041||!e->vendor||strcmp(e->vendor,"EMTC"))return 0;
 static const struct{unsigned d,man,id;const char*name;}rows[]={
{34,41,2,"EM29SF002AB"},
{34,41,2,"EM29SF002AB *32PLCC"},
{34,41,2,"EM29SF002AB *32TS"},
{33,41,1,"EM29SF002AT"},
{33,41,1,"EM29SF002AT *32PLCC"},
{33,41,1,"EM29SF002AT *32TS"},
{36,41,2,"EM29SF002NAB"},
{36,41,2,"EM29SF002NAB *32PLCC"},
{36,41,2,"EM29SF002NAB *32TS"},
{35,41,1,"EM29SF002NAT"},
{35,41,1,"EM29SF002NAT *32PLCC"},
{35,41,1,"EM29SF002NAT *32TS"},
{37,41,22,"EM29SF020A"},
{37,41,22,"EM29SF020A *32PLCC"},
{37,41,22,"EM29SF020A *32TS"},
{37,41,22,"EM29SF020NA"},
{37,41,22,"EM29SF020NA *32PLCC"},
{37,41,22,"EM29SF020NA *32TS"},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&strlen(rows[i].name)==e->device_name_length&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;
}
static int emtc18_admitted(unsigned d){return (d==0x21||d==0x22)?LT48_EMTC18_STAGE>=1:(d==0x23||d==0x24)?LT48_EMTC18_STAGE>=2:d==0x25?LT48_EMTC18_STAGE>=3:0;}

