/* Authored exact admission bindings; generated from frozen batch membership. */
#include "../green14/profiles.h"
static int sst_page_binding(const lt48_catalogue_entry*e){
 if(e->device_class!=2041)return 0;
 if(green14_binding(e))return 1;
 static const struct {unsigned d,man,id;const char*vendor,*name;} rows[]={
 {10,191,7,"SST","28EE011"},
 {10,191,7,"SST","29EE010"},
 {10,191,7,"SST","29EE010 *32PLCC"},
 {10,191,7,"SST","29EE010 *32TS"},
 {10,191,7,"SST","29EE011"},
 {11,191,16,"SST","29EE020"},
 {11,191,16,"SST","29EE020 *32PLCC"},
 {11,191,16,"SST","29EE020 *32TS"},
 {9,191,93,"SST","29EE512"},
 {9,191,93,"SST","29EE512 *32PLCC"},
 {9,191,93,"SST","29EE512 *32TS"},
 {13,191,8,"SST","29LE010"},
 {13,191,8,"SST","29LE010 *32PLCC"},
 {13,191,8,"SST","29LE010 *32TS"},
 {14,191,18,"SST","29LE020"},
 {14,191,18,"SST","29LE020 *32PLCC"},
 {14,191,18,"SST","29LE020 *32TS"},
 {12,191,61,"SST","29LE512"},
 {12,191,61,"SST","29LE512 *32PLCC"},
 {12,191,61,"SST","29LE512 *32TS"},
 {13,191,8,"SST","29VE010"},
 {13,191,8,"SST","29VE010 *32PLCC"},
 {13,191,8,"SST","29VE010 *32TS"},
 {14,191,18,"SST","29VE020"},
 {14,191,18,"SST","29VE020 *32PLCC"},
 {14,191,18,"SST","29VE020 *32TS"},
 {12,191,61,"SST","29VE512"},
 {12,191,61,"SST","29VE512 *32PLCC"},
 {12,191,61,"SST","29VE512 *32TS"},
 {10,218,193,"Winbond","W29C011A"},
 {10,218,193,"Winbond","W29C011A *32PLCC"},
 {10,218,193,"Winbond","W29EE011"},
 {10,218,193,"Winbond","W29EE011 *32PLCC"},
 {10,218,193,"Winbond","W29EE011 *32TS"},
 {9,218,200,"Winbond","W29EE512 *32PLCC"},
 {9,218,200,"Winbond","W29EE512 *32TS"},
 };
 for(size_t i=0;i<sizeof rows/sizeof *rows;i++)if(e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&e->vendor&&!strcmp(e->vendor,rows[i].vendor)&&strlen(rows[i].name)==e->device_name_length&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;
 return 0;
}
