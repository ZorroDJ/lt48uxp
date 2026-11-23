static const struct s8_profile{unsigned definition,family,pinmap,capacity,fields,configuration,page,data,code,variables,ops,apps,nominal;}s8_profiles[]={
{627,473,230,8320,14,25329,64,128,8192,5,20,7,500},
{626,472,230,8832,14,25329,64,640,8192,5,20,7,500},
{628,474,231,17408,20,25368,128,1024,16384,5,20,7,500},
{629,474,231,33792,20,25368,128,1024,32768,5,20,7,500},
};
static const struct{unsigned definition,manufacturer,device;const char*vendor,*name;}s8_members[]={
{627,0,0,"ST","STM8S003K3T6 @32TQ"},
{626,0,0,"ST","STM8S103K3T6 @32TQ"},
{628,0,0,"ST","STM8S105K4T6 @32TQ"},
{629,0,0,"ST","STM8S105K6T6 @32TQ"},
};
