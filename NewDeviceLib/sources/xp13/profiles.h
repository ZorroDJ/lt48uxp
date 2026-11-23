/* Original class2035 low-voltage word OTP/EPROM, exact BYO bindings. */
#include "admission.h"
static int xp13_binding(const lt48_catalogue_entry*e){
 if(!e->vendor)return 0;
 static const struct{const char*vendor,*name;unsigned cls,d,man,id;}rows[]={
{"OKI","MR27V12800J *48TS",2035,7,0,0},
{"OKI","MR27V1602F *44PS",2035,0,0,0},
{"OKI","MR27V1602F *48TS",2035,1,0,0},
{"OKI","MR27V3202F *44PS",2035,2,0,0},
{"OKI","MR27V3202F *44TS",2035,2,0,0},
{"OKI","MR27V3202F *48TS",2035,3,0,0},
{"OKI","MR27V3252J *48TS",2035,4,0,0},
{"OKI","MR27V6402G *44PS",2035,5,0,0},
{"OKI","MR27V6402G *44TS",2035,5,0,0},
{"OKI","MR27V6402G *48TS",2035,6,0,0},
{"SILICON7","SV5C3238UBA @72BGA",2033,22,44,17557},
{"SILICON7","SV5C3238UTA @72BGA",2033,21,44,17556},
{"ST","M27V160 *44",2035,8,0,0},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(e->device_class==rows[i].cls&&e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&!strcmp(e->vendor,rows[i].vendor)&&e->device_name_length==strlen(rows[i].name)&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;
}
static const uint32_t xp13_none[]={0};
static const struct xp_closure xp13_otp_closures[]={
{7,5,6,48,16777216,330,xp13_none,1},
{0,0,0,44,2097152,330,xp13_none,1},
{1,0,1,48,2097152,330,xp13_none,1},
{2,1,2,44,4194304,330,xp13_none,1},
{3,1,3,48,4194304,330,xp13_none,1},
{4,2,3,48,4194304,330,xp13_none,1},
{5,3,4,44,8388608,330,xp13_none,1},
{6,4,5,48,8388608,330,xp13_none,1},
{8,6,7,44,2097152,330,xp13_none,1},
};
/* Same M5/P16/all procedures as LRS1380; bottom geometry already used by D12. */
static const struct sh13_profile xp13_alias_bottom={{22,5,16,48,4194304,330,sh13_protect_21,1},{22,15,5,0},{22,sh13_erase_18,71},5};
