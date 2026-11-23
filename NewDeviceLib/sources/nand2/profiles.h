/* Exact 512-Mbit physical NAND aliases. */
#include "admission.h"
static int nand2_binding(const lt48_catalogue_entry*e){
 if(e->device_class!=2041||!e->vendor||!NAND2_ADMITTED)return 0;
 static const struct{unsigned d,man,id;const char*vendor,*name;}rows[]={
{31,236,118,"Samsung","K9F1208U0A *48TS"},
{31,236,118,"Samsung","K9F1208U0B *48TS"},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&!strcmp(e->vendor,rows[i].vendor)&&strlen(rows[i].name)==e->device_name_length&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;
}
