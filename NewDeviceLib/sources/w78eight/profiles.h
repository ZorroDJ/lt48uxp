/* Authored finite metadata; original payloads remain BYO. */
static int w78eight_binding(const lt48_device_spec*s){
 if(s->catalogue.device_class!=2038||s->element_bits!=8||s->secondary_element_count)return 0;
 static const struct{unsigned d,m,p,pins,n;const char*vendor,*name;}rows[]={
{114,48,25,32,16384,"Nuvoton","W78E374E"},
{115,48,25,32,32768,"Nuvoton","W78E378"},
{116,49,26,40,32768,"Nuvoton","W78E378E"},
{117,50,27,44,32768,"Nuvoton","W78E378P *44"},
{114,48,25,32,16384,"Winbond","W78E374E"},
{115,48,25,32,32768,"Winbond","W78E378"},
{116,49,26,40,32768,"Winbond","W78E378E"},
{117,50,27,44,32768,"Winbond","W78E378P *44"},
 };for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(s->definition_resource==rows[i].d&&s->family_code==rows[i].m&&s->pinmap==rows[i].p&&s->pin_count==rows[i].pins&&s->capacity_bytes==rows[i].n&&!strcmp(s->catalogue.vendor,rows[i].vendor)&&s->catalogue.device_name_length==strlen(rows[i].name)&&!memcmp(s->catalogue.device_name,rows[i].name,s->catalogue.device_name_length))return 1;return 0;
}
