/* Authored SPHE6400 interface profiles; original procedures remain user-supplied. */
static const uint32_t sphe3_erase[]={0};
static const struct serial_flash_extra sphe3_profiles[]={
{ {244,140,1048576,64,310,400,29,8},3,{{0,0,1,310,1},{1,6,0,2,400},{2,2,0,64,0}},146408719u,0,0,{{0}},0,sphe3_erase,1,NULL,0},
{ {245,141,1048576,64,310,400,28,8},3,{{0,0,1,310,1},{1,6,0,2,400},{2,2,0,64,0}},146408719u,0,0,{{0}},0,sphe3_erase,1,NULL,0},
{ {243,139,1048576,64,310,400,28,8},3,{{0,0,1,310,1},{1,6,0,2,400},{2,2,0,64,0}},146408719u,0,0,{{0}},0,sphe3_erase,1,NULL,0},
};
static const struct serial_flash_extra*sphe3_binding(const lt48_device_spec*s){
if(s->catalogue.device_class!=2043||strcmp(s->definition_stem,"GDBB___")||strcmp(s->catalogue.vendor,"SUNPLUS"))return NULL;
if(s->definition_resource==244&&s->family_code==140&&s->pinmap==29&&s->capacity_bytes==1048576&&s->catalogue.manufacturer_id==0&&s->catalogue.device_id==0&&s->catalogue.device_name_length==strlen("SPHE6400A-SF  ")&&!memcmp(s->catalogue.device_name,"SPHE6400A-SF  ",s->catalogue.device_name_length))return sphe3_profiles+0;
if(s->definition_resource==245&&s->family_code==141&&s->pinmap==28&&s->capacity_bytes==1048576&&s->catalogue.manufacturer_id==0&&s->catalogue.device_id==0&&s->catalogue.device_name_length==strlen("SPHE6400A-SF2 ")&&!memcmp(s->catalogue.device_name,"SPHE6400A-SF2 ",s->catalogue.device_name_length))return sphe3_profiles+1;
if(s->definition_resource==243&&s->family_code==139&&s->pinmap==28&&s->capacity_bytes==1048576&&s->catalogue.manufacturer_id==0&&s->catalogue.device_id==0&&s->catalogue.device_name_length==strlen("SPHE6400A-SP  ")&&!memcmp(s->catalogue.device_name,"SPHE6400A-SP  ",s->catalogue.device_name_length))return sphe3_profiles+2;
return NULL;}
