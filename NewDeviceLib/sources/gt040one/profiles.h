/* Exact authored bindings to original Great Tek ARF35LV040 workflow. */
static const struct gaaf_profile gt040one_profiles[]={
{108,81,32,524288,{211,4,212,218,215,214,65535,65535,65535,65535,234,228,229,231,230,227,232,65535,65535,65535,65535}},
};
static int gt040one_member(const lt48_catalogue_entry*e){if(!e||e->device_class!=2009||strcmp(e->vendor,"Great Tek")||e->manufacturer_id!=155)return 0;
if(e->device_type==108&&e->device_id==132&&e->device_name_length==strlen("ARF35LV040 ")&&!memcmp(e->device_name,"ARF35LV040 ",e->device_name_length))return 1;
return 0;}
