/* Exact Dataman 13.10 catalogue bindings; original payloads remain external. */
static int lv800three_member(const lt48_catalogue_entry*e){if(!e||e->device_class!=2031||e->device_type!=197||e->manufacturer_id!=127||e->device_id!=32)return 0;
return (!strcmp(e->vendor,"ACTRANS")&&e->device_name_length==15&&!memcmp(e->device_name,"AC39LV800 *48TS",15))||
(!strcmp(e->vendor,"ELAN")&&e->device_name_length==15&&!memcmp(e->device_name,"EM39LV800 *48TS",15))||
(!strcmp(e->vendor,"MIRA")&&e->device_name_length==15&&!memcmp(e->device_name,"P439LV800 *48TS",15));}
