/* Original class2044 SST C-series LPC, exact BYO bindings. */
#include "admission.h"
static int sst49c7_binding(const lt48_catalogue_entry*e){
 if(!e->vendor)return 0;
 static const struct{const char*vendor,*name;unsigned cls,d,man,id;}rows[]={
{"SST","49LF004C *32PLCC",2044,22,191,84},
{"SST","49LF004C *32TS/W",2044,23,191,84},
{"SST","49LF008C *32PLCC",2044,24,191,89},
{"SST","49LF008C *32TS/W",2044,25,191,89},
{"SST","49LF016C *32PLCC",2044,26,191,92},
{"SST","49LF016C *32TS/W",2044,27,191,92},
{"SST","49LF016C *40TS",2044,28,191,92},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(i<LT48_SST49C7_ADMITTED&&e->device_class==rows[i].cls&&e->device_type==rows[i].d&&e->manufacturer_id==rows[i].man&&e->device_id==rows[i].id&&!strcmp(e->vendor,rows[i].vendor)&&e->device_name_length==strlen(rows[i].name)&&!memcmp(e->device_name,rows[i].name,e->device_name_length))return 1;return 0;
}
