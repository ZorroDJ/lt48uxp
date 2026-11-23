/* Exact authored ISSI catalogue bindings; no original binary resources. */
static int issi2_member(const lt48_catalogue_entry*e){if(!e||e->device_class!=2009||e->device_type!=140||e->manufacturer_id!=157||e->device_id!=110||strcmp(e->vendor,"ISSI"))return 0;
if(e->device_name_length==strlen("IS49FL004T *32PLCC")&&!memcmp(e->device_name,"IS49FL004T *32PLCC",e->device_name_length))return 1;
if(e->device_name_length==strlen("IS49FL004T *32TS/W")&&!memcmp(e->device_name,"IS49FL004T *32TS/W",e->device_name_length))return 1;
return 0;}
