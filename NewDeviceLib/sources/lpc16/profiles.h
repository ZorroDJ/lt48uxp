/* Original LPC213x/214x/176x workflows; vendor resources remain BYO. */
static const uint32_t lpc16_sectors[]={0};
struct lpc16_profile{struct xp_closure base;struct xp_shared_word_shape shape;unsigned variables;};
static const struct lpc16_profile lpc16_profiles[]={
{ {361,234,147,48,262144,330,lpc16_sectors,1}, {361,14,7,0},2},
{ {360,233,147,48,131072,330,lpc16_sectors,1}, {360,14,7,0},2},
{ {362,235,147,48,524288,330,lpc16_sectors,1}, {362,14,7,0},2},
{ {323,198,130,48,32768,330,lpc16_sectors,1}, {323,14,7,0},2},
{ {324,199,130,48,65536,330,lpc16_sectors,1}, {324,14,7,0},2},
{ {325,200,130,48,131072,330,lpc16_sectors,1}, {325,14,7,0},2},
{ {326,201,130,48,262144,330,lpc16_sectors,1}, {326,14,7,0},2},
{ {327,202,130,48,512000,330,lpc16_sectors,1}, {327,14,7,0},2},
};
static const struct lpc16_profile*lpc16_find(unsigned cls,unsigned d){if(cls!=2038)return NULL;for(size_t i=0;i<sizeof lpc16_profiles/sizeof*lpc16_profiles;i++)if(lpc16_profiles[i].base.definition==d)return lpc16_profiles+i;return NULL;}
static int lpc16_binding(const lt48_device_spec*s){static const struct{const char*key;unsigned definition,manufacturer,device;}rows[]={
{"NXP:LPC1763 @100LQ",361,9729,8243},
{"NXP:LPC1764 @100LQ",360,9729,6434},
{"NXP:LPC1765 @100LQ",361,9729,14131},
{"NXP:LPC1766 @100LQ",361,9729,16179},
{"NXP:LPC1767 @100LQ",362,9729,10295},
{"NXP:LPC1768 @100LQ",362,9729,16183},
{"NXP:LPC1769 @100LQ",362,9745,16183},
{"NXP:LPC2131 @64LQ",323,2,65281},
{"NXP:LPC2132 @64LQ",324,2,65297},
{"NXP:LPC2134 @64LQ",325,2,65298},
{"NXP:LPC2136 @64LQ",326,2,65315},
{"NXP:LPC2138 @64LQ",327,2,65317},
{"NXP:LPC2142 @64LQ",324,2,65297},
{"NXP:LPC2144 @64LQ",325,2,65298},
{"NXP:LPC2146 @64LQ",326,2,65315},
{"NXP:LPC2148 @64LQ",327,2,65317},
};char key[256];snprintf(key,sizeof key,"%s:%.*s",s->catalogue.vendor,(int)s->catalogue.device_name_length,s->catalogue.device_name);for(size_t i=0;i<sizeof rows/sizeof*rows;i++)if(!strcmp(key,rows[i].key)&&s->definition_resource==rows[i].definition&&s->catalogue.manufacturer_id==rows[i].manufacturer&&s->catalogue.device_id==rows[i].device)return 1;return 0;}
