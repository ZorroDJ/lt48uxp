/* Exact authored bindings to original PMC FWH workflows. */
static const struct gaaf_profile pmc6_profiles[]={
{138,99,42,262144,{1,4,260,262,266,267,65535,65535,65535,65535,281,277,278,280,279,276,273,65535,65535,65535,65535}},
{140,99,42,524288,{1,4,260,262,266,267,65535,65535,65535,65535,281,277,278,280,279,276,273,65535,65535,65535,65535}},
{141,99,42,1048576,{1,4,260,262,266,267,65535,65535,65535,65535,281,277,278,280,279,276,273,65535,65535,65535,65535}},
};
static int pmc6_member(const lt48_catalogue_entry*e){if(!e||e->device_class!=2009||strcmp(e->vendor,"PMC (Chingis)")||e->manufacturer_id!=157)return 0;
if(e->device_type==138&&e->device_id==109&&e->device_name_length==strlen("Pm49FL002T *32PLCC")&&!memcmp(e->device_name,"Pm49FL002T *32PLCC",e->device_name_length))return 1;
if(e->device_type==138&&e->device_id==109&&e->device_name_length==strlen("Pm49FL002T *32TS/W")&&!memcmp(e->device_name,"Pm49FL002T *32TS/W",e->device_name_length))return 1;
if(e->device_type==140&&e->device_id==110&&e->device_name_length==strlen("Pm49FL004T *32PLCC")&&!memcmp(e->device_name,"Pm49FL004T *32PLCC",e->device_name_length))return 1;
if(e->device_type==140&&e->device_id==110&&e->device_name_length==strlen("Pm49FL004T *32TS/W")&&!memcmp(e->device_name,"Pm49FL004T *32TS/W",e->device_name_length))return 1;
if(e->device_type==141&&e->device_id==106&&e->device_name_length==strlen("Pm49FL008T *32PLCC")&&!memcmp(e->device_name,"Pm49FL008T *32PLCC",e->device_name_length))return 1;
if(e->device_type==141&&e->device_id==106&&e->device_name_length==strlen("Pm49FL008T *32TS/W")&&!memcmp(e->device_name,"Pm49FL008T *32TS/W",e->device_name_length))return 1;
return 0;}
