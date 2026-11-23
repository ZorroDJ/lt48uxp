/* Exact authored HMS77C2000/2001 source policies; no vendor payload. */
static const b12otp_profile h77_profiles[]={
{340,231,79,"HMS77C2000",{512,63,8,63,1,0,8,100,1,100,0,4095,2,1,1},12,10,{{0,0,1,500,2},{0,2,0,1000,3},{0,4,0,1100,3},{0,3,1,1150,3},{1,13,0,0,150},{1,8,1,0,100},{1,10,0,1,100},{2,17,0,8,0},{2,19,0,511,0},{2,18,0,515,0},{0,5,1,475,2},{0,6,1,525,2}},{0,1,3,4,6,8,13,10,11,12},{0,5,290,297,291,292,293,294,295,296}},
{341,232,79,"HMS77C2001",{1024,63,8,63,1,0,8,100,1,100,0,4095,2,1,1},12,10,{{0,0,1,500,2},{0,2,0,1000,3},{0,4,0,1100,3},{0,3,1,1150,3},{1,13,0,0,150},{1,8,1,0,100},{1,10,0,1,100},{2,17,0,8,0},{2,19,0,1023,0},{2,18,0,1027,0},{0,5,1,475,2},{0,6,1,525,2}},{0,1,3,4,6,8,13,10,11,12},{0,5,290,297,291,292,293,294,295,296}},
};
static const b12otp_profile*h77_match(const lt48_device_spec*s){
if(!s||s->catalogue.device_class!=2015||!s->definition_stem||strcmp(s->definition_stem,"GADC___")||!s->catalogue.vendor||strcmp(s->catalogue.vendor,"Hyundai(Hynix)")||s->element_bits!=12||s->capacity_bytes!=8192||s->pin_count!=8||s->catalogue.manufacturer_id||s->catalogue.device_id)return NULL;
for(size_t i=0;i<sizeof h77_profiles/sizeof*h77_profiles;i++){const b12otp_profile*p=h77_profiles+i;if(s->definition_resource==p->definition&&s->family_code==p->family&&s->pinmap==p->pinmap&&s->catalogue.device_name_length==strlen(p->name)&&!memcmp(s->catalogue.device_name,p->name,strlen(p->name)))return p;}return NULL;}
static int h77_profile(const b12otp_profile*p){for(size_t i=0;i<sizeof h77_profiles/sizeof*h77_profiles;i++)if(p==h77_profiles+i)return 1;return 0;}
