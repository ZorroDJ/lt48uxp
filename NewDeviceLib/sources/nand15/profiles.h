/* Exact source bindings; NAND images are physical 512+16 byte pages. */
#include "admission.h"
#include "../nand2/profiles.h"
static int nand15_binding(const lt48_catalogue_entry*e){
 if(nand2_binding(e))return 1;
 if(e->device_class!=2041||!e->vendor)return 0;
 static const struct{unsigned d,man,id;const char*vendor,*name;}rows[]={
{28,32,115,"ST","NAND128W3A *48TS"},
{28,236,115,"Samsung","K9F2808U0B *48TS"},
{28,236,115,"Samsung","K9F2808U0C *48TS"},
{30,236,117,"Samsung","K9F5608U0B *48TS"},
{30,236,117,"Samsung","K9F5608U0C *48TS"},
{27,236,230,"Samsung","K9F6408U0A *44TS"},
{27,236,230,"Samsung","K9F6408U0B *44TS"},
{27,236,230,"Samsung","K9F6408U0C *44TS"},
{28,236,115,"Samsung","KM29U128T *48TS"},
{27,236,230,"Samsung","KM29U64000T *44TS"},
{28,152,115,"SanDisk","SDTNF-128 *48TS"},
{29,152,115,"Toshiba","TC581282AXB @56FBGA"},
{28,152,115,"Toshiba","TC58128AFT *48TS"},
{28,152,115,"Toshiba","TC58DVM72A1FT00 *48TS"},
{30,152,117,"Toshiba","TC58DVM82A1FT00 *48TS"},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&!strcmp(e->vendor,rows[i].vendor)&&strlen(rows[i].name)==e->device_name_length&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;
}
static int nand15_admitted(unsigned d){return (d==0x1f&&NAND2_ADMITTED)||(d>=0x1b&&d<=0x1e&&LT48_NAND15_STAGE>=1);}
